/*
 * eHimage.cpp
 *
 * Hang Su
 * 2012-07 @ eH
 */

#include <CImg.h>
#include "eHimage.h"
#include "eHbox.h"

#include <assert.h>
#include <string.h>
#include <weblogger.h>

static inline int round2int(double x) { return (int)(x+0.5);}

image_t* image_alloc(size_t sizy, size_t sizx, size_t nch){
    image_t* img = new struct image_t;
	img->sizy = sizy;
	img->sizx = sizx;
	img->nchannel = nch;
	img->imsize[0] = sizy;
	img->imsize[1] = sizx;
	img->imsize[2] = nch;
	img->data = new double[sizy*sizx*nch];
	for(unsigned i=0;i<nch;i++) {
		img->ch[i] = img->data + i*sizy*sizx;
	}
	img->is_shared = false;
	img->stepy = img->sizy;
	img->stepyx = img->sizy*img->sizx;
	return img;
}

image_t* image_alloc(size_t sizy, size_t sizx, size_t nch, const double* fillval) {
    image_t* img = new struct image_t;
	img->sizy = sizy;
	img->sizx = sizx;
	img->nchannel = nch;
	img->imsize[0] = sizy;
	img->imsize[1] = sizx;
	img->imsize[2] = nch;
	img->data = new double[sizy*sizx*nch];

	for(unsigned i=0;i<nch;i++) {
		img->ch[i] = img->data + i*sizy*sizx;
		for(unsigned xy=0; xy<sizy*sizx; xy++)
			img->ch[i][xy] = fillval[i];
	}
	img->is_shared = false;
	img->stepy = img->sizy;
	img->stepyx = img->sizy*img->sizx;
	return img;
}

void image_delete(image_t* img){
	if(NULL==img)
		return;
	if(!img->is_shared) {
		if(img->data!=NULL)
			delete[] img->data;
	}
	delete img;
}

void image_zero(image_t* img, const double* val) {
	if(img==NULL || img->data==NULL || val==NULL) return;
	unsigned ch, y, x;
	for(ch=0; ch<img->nchannel; ch++)
		for(y=0; y<img->sizy; y++)
			for(x=0;x<img->sizx; x++)
				img->ch[ch][x*img->stepy+y]=val[ch];
}

image_t* image_readJPG(const char* filename) {
    cimg_library::CImg<int> img;
    img.load_jpeg(filename);
    if(!img.data()) {
        WebLogger::instance()->log(QtCriticalMsg, QString("Error: can not open ") + filename);
        return NULL;
    }
    image_t* im = image_alloc(img.height(), img.width());
    for(unsigned y=0;y<im->sizy;y++) {
        for(unsigned x=0;x<im->sizx;x++) {
            int offset = y+x*im->stepy;
            im->ch[0][offset]=img(x, y, 0, 0);
            im->ch[1][offset]=img(x, y, 0, 1);
            im->ch[2][offset]=img(x, y, 0, 2);
        }
    }
    return im;
}

/* struct used for caching interpolation values */
/* used by image_subsample() */
struct alphainfo {
	int si, di;
	double alpha;
};

/* copy src into dst using pre-computed interpolation values */
/* used by image_subsample() */
void alphacopy(double* src, double*dst, struct alphainfo *ofs, int n) {
	struct alphainfo *end = ofs+n;
	while(ofs != end) {
		dst[ofs->di] += ofs->alpha * src[ofs->si];
		ofs++;
	}
}

/* resize along each column (result is transposed) */
/* used by image_subsample() */
void subsample1dtran(const image_t* src, size_t sheight,
		image_t* dst, size_t dheight, size_t width) throw(std::bad_alloc){
	double scale = (double)dheight/(double)sheight;
	double invscale = (double)sheight/(double)dheight;

	/* cache interpolation values since they can be shared 
     * among different columns*/
    int len = (int)ceil(dheight*invscale) + 2*dheight;
	alphainfo* ofs=new alphainfo[len];
	int k = 0;
	for (unsigned dy=0;dy<dheight;dy++) {
		double fsy1 = dy * invscale;
		double fsy2 = fsy1 + invscale;
		int sy1 = (int)ceil(fsy1);
		int sy2 = (int)floor(fsy2);
		if(sy1-fsy1 > 1e-3) {
			assert(k<len);
			//assert(sy-1 >= 0);
			ofs[k].di = dy*width;
			ofs[k].si = sy1-1;
			ofs[k++].alpha = (sy1-fsy1)*scale;
		}
		for (int sy = sy1;sy<sy2;sy++) {
			assert(k<len);
			assert(sy<(int)sheight);
			ofs[k].di = dy*width;
			ofs[k].si = sy;
			ofs[k++].alpha = scale;
		}
		if(fsy2-sy2 > 1e-3) {
			assert(k<len);
			assert(sy2<(int)sheight);
			ofs[k].di = dy*width;
			ofs[k].si = sy2;
			ofs[k++].alpha = (fsy2-sy2)*scale;
		}
	}
	for (int nch = 0; nch<3; nch++) {
		for (unsigned x = 0; x<width; x++) {
			double *s = src->ch[nch] + x*src->stepy;
			double *d = dst->ch[nch] + x;
			alphacopy(s,d,ofs,k);
		}
	}
	delete[] ofs;
}

/** @brief Fast image subsampling
 *  @note src image is not destroyed
 */
image_t* image_subsample(const image_t* img, double scale) {
	if(scale>1 || scale <=0 || img==NULL || img->data==NULL)
		return NULL;
	size_t dst_sizy = (unsigned int)round2int(img->sizy*scale);
	size_t dst_sizx = (unsigned int)round2int(img->sizx*scale);
	double initialval[] = {0, 0, 0};
	image_t* scaled = image_alloc(dst_sizy, dst_sizx, img->nchannel, initialval);
	image_t* tmp = image_alloc(img->sizx,dst_sizy, img->nchannel, initialval);
	/* scale in columns, and transposed */
	subsample1dtran(img,img->sizy,tmp,dst_sizy, img->sizx);
	/* scale in (old)rows, and transposed back */
	subsample1dtran(tmp,img->sizx,scaled,dst_sizx,dst_sizy);
	image_delete(tmp);
	return scaled;
}

void resize1dtran(const image_t* src, image_t* dst) throw(std::bad_alloc){
	double scale = (dst->sizx-1.0) / (src->sizy-1.0);
	double invscale = 1/scale;
	int* pre = new int[dst->sizx];
	double* alpha = new double[dst->sizx];
	pre[0] = 0; alpha[0] = 1.0;
	pre[dst->sizx-1] = src->sizy-2;  alpha[dst->sizx-1] = 0;
	for(unsigned i=1;i<dst->sizx-1;i++) {
		pre[i]=(int)floor(invscale*i);
		alpha[i]=invscale*i-floor(invscale*i);
	}
	unsigned ch, y, x;
	for(ch=0;ch<dst->nchannel;ch++) {
		for(y=0;y<dst->sizy;y++) {
			for(x=0;x<dst->sizx;x++) {
				dst->ch[ch][x*dst->stepy+y] = 
					src->ch[ch][y*src->stepy+pre[x]]*alpha[x] 
					+ src->ch[ch][y*src->stepy+pre[x]+1]*(1-alpha[x]);
			}
		}
	}
	delete[] pre;
	delete[] alpha;
}

image_t* image_resize(const image_t* img, double scale) {
	if(scale<=0 || img==NULL || img->data==NULL)
		return NULL;
	size_t dst_sizy = (unsigned)round2int(img->sizy*scale);
	size_t dst_sizx = (unsigned)round2int(img->sizx*scale);
	image_t* scaled = image_alloc(dst_sizy, dst_sizx, img->nchannel);
	image_t* tmp = image_alloc(img->sizx, dst_sizy, img->nchannel);
	
	/* scale in colums, and transposed */
	resize1dtran(img,tmp);
	/* scale in (old)rows, and transposed */
	resize1dtran(tmp,scaled);
	image_delete(tmp);
	return scaled;
}

/* reduce along each column (result is transposed) */
/* used by image_reduce() */
void reduce1dtran(const image_t* src, size_t sheight,
		image_t* dst, size_t dheight, size_t width) {
	double *s, *d;
	for (int nch = 0; nch<3; nch++) {
		for (unsigned x = 0; x<width; x++) {
			s = src->ch[nch] + x*src->stepy;
			d = dst->ch[nch] + x;

			/* First row */
			*d = s[0]*0.6875 + s[1]*0.2500 + s[2]*0.0625;
			
			/* middle rows */
			for (unsigned y = 1; y<dheight-2;y++) {
				s += 2;
				d += width;
				*d = s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 
					+ s[1]*0.25 + s[2]*0.0625;
			}

			/* Last two rows */
			s += 2;
			d += width;
			if (dheight*2 <= sheight) {
				*d = s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 
					+ s[1]*0.25 + s[2]*0.0625;
			} else {
				*d = s[1]*0.3125 + s[0]*0.375 + s[-1]*0.25 
					+ s[-2]*0.0625;
			}
			s += 2;
			d += width;
			*d = s[0]*0.6875 + s[-1]*0.25 + s[-2]*0.0625;
		}
	}
}

/*
 * Reduce size to half, using 5-tap binomial filter for anti-aliasing
 * (see Burt & Adelson's Laplacian Pyramid paper for details)
 */
image_t* image_reduce(const image_t* img) {
	size_t dst_sizy = (unsigned int)round2int(img->sizy*.5);
	size_t dst_sizx = (unsigned int)round2int(img->sizx*.5);
	image_t* scaled = image_alloc(dst_sizy, dst_sizx, img->nchannel);
	image_t* tmp = image_alloc(img->sizx,dst_sizy, img->nchannel);

	/* scale in columns, and transposed */
	reduce1dtran(img,img->sizy,tmp,dst_sizy,img->sizx);
	/* scale in (old)columns, and transposed back */
	reduce1dtran(tmp,img->sizx,scaled,dst_sizx,dst_sizy);

	image_delete(tmp);
	return scaled;
}

image_t* image_crop(const image_t* img, fbox_t crop, int* offset, bool shared) {
	image_t* result;
	fbox_clip(crop, img->imsize);
    ibox_t intcrop = fbox_getibox(&crop);
	if(shared) {
		result = new image_t;
		result->sizx = intcrop.x2-intcrop.x1+1;
		result->sizy = intcrop.y2-intcrop.y1+1;
		result->imsize[0] = result->sizy;
		result->imsize[1] = result->sizx;
		result->stepy = img->stepy;
		result->stepyx = img->stepyx;
		result->is_shared = true;
		result->nchannel = img->nchannel;
		result->imsize[2] = result->nchannel;
		result->data = img->data + (intcrop.x1*img->stepy + intcrop.y1);
		for(unsigned i=0;i<result->nchannel;i++)
			result->ch[i] = result->data + img->stepyx*i;
	} else {
		result = image_alloc(intcrop.y2-intcrop.y1+1,intcrop.x2-intcrop.x1+1,img->nchannel);
		for(unsigned c=0;c<result->nchannel;c++){
			for(unsigned y=0;y<result->sizy;y++) {
				for(unsigned x=0;x<result->sizx;x++) {
					result->data[c*result->stepyx+x*result->stepy+y] = 
						img->data[c*img->stepyx+(x+intcrop.x1)*result->sizy+(y+intcrop.y1)];
				}
			}
		}
	}
	if(offset!=NULL) {
		offset[0] = intcrop.y1;
		offset[1] = intcrop.x1;
	}
	return result;
}
