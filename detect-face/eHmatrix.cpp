/*
 * eHmatrix.cpp
 *
 * Hang Su
 * 2012-07 @ eH
 */
#include "eHmatrix.h"

mat2d_ptr mat2d_alloc(size_t sizy, size_t sizx) {
	mat2d_t* mat = new mat2d_t;
	mat->vals = new double[sizy*sizx];
	mat->sizy = sizy;
	mat->sizx = sizx;
	return mat;
}

void mat2d_delete(mat2d_ptr mat) {
	delete[] mat->vals;
	delete mat;
}

mat3d_ptr mat3d_alloc(size_t sizy, size_t sizx, size_t sizz) {
	mat3d_t* mat = new mat3d_t;
	mat->vals = new double[sizy*sizx*sizz];
	mat->sizy = sizy;
	mat->sizx = sizx;
	mat->sizz = sizz;
	return mat;
}

void mat3d_delete(mat3d_ptr mat) {
	delete[] mat->vals;
	delete mat;
}

void mat3d_pad(mat3d_ptr mat, const size_t* pad, double pad_val) {
	size_t newy = mat->sizy+pad[0]*2;
	size_t newx = mat->sizx+pad[1]*2;
	size_t newz = mat->sizz+pad[2]*2;
	double* vals_new = new double[newy*newx*newz];

	unsigned yy, xx, zz;
	bool pady, padx, padz;
	for(yy=0;yy<newy;yy++){
		if(yy<pad[0] || yy>=mat->sizy+pad[0])
			pady=true;
		else
			pady=false;
		for(xx=0;xx<newx;xx++){
			if(xx<pad[1] || xx>=mat->sizx+pad[1])
				padx=true;
			else
				padx=false;
			for(zz=0;zz<newz;zz++){
				if(zz<pad[2] || zz>=mat->sizz+pad[2])
					padz=true;
				else
					padz=false;
				if (pady || padx || padz)
					vals_new[yy+xx*newy+zz*newy*newx] = pad_val;
				else
					vals_new[yy+xx*newy+zz*newy*newx] = 
						mat->vals[yy-pad[0]+(xx-pad[1])*mat->sizy+(zz-pad[2])*mat->sizy*mat->sizx];
			}
		}
	}
	
	delete[] mat->vals;
	mat->vals = vals_new;
	mat->sizy = newy;
	mat->sizx = newx;
	mat->sizz = newz;
}

void  mat3d_fill(mat3d_ptr mat, const size_t* start, const size_t* width, double fill_val){
	unsigned yy, xx, zz;
	for (yy=start[0];yy<start[0]+width[0];yy++)
		for (xx=start[1];xx<start[1]+width[1];xx++)
			for (zz=start[2];zz<start[2]+width[2];zz++)
				mat->vals[yy+xx*mat->sizy+zz*mat->sizy*mat->sizx] = fill_val;
}

matkd_ptr matkd_alloc(size_t k, size_t* sizs) {
	matkd_t* mat = new matkd_t;
	mat->k = k;
	mat->siz = new size_t[k];
	int len = 1;
	for (unsigned i=0;i<k;i++) {
		len*=sizs[i];
		mat->siz[i]=sizs[i];
	}
	mat->vals = new double[len];
	return mat;
}

void matkd_delete(matkd_ptr mat) {
	delete[] mat->siz;
	delete[] mat->vals;
	delete mat;
}

