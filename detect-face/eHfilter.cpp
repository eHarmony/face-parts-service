/*
 * eHfilter.cpp
 *
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHfilter.h"
#include <assert.h>
#include <vector>
#include <parallel_for.h>

using std::vector;

extern "C" {
#include <cblas.h>
}

struct thread_data {
	mat3d_t A;
	mat3d_t B;
	mat2d_t C;
};

/* reshape matrix
 * (Old)y x z ==> (F)z x y
 * NOTE: result is stored in F, which should be allocated already
 */
double* prepare_filter(double* F, double* Old, size_t sizy, size_t sizx, size_t sizz) {
	unsigned f, x, y;
	for (f = 0; f < sizz; f++) {
		for (x = 0; x < sizx; x++) {
			for (y = 0; y < sizy; y++) {
				F[f + x*(sizz) + y*(sizx*sizz)] =  
					Old[y + x*sizy + f*(sizy*sizx)];
			}
		}
    }
    return F;
}

/* reshape matrix
 * (Old)y x z ==> (F)y z x
 * NOTE: result is stored in F, which should be allocated already
 */
void prepare_map(double* F, double* Old, size_t sizy, size_t sizx, size_t sizz) {
	unsigned f, x, y;
	for (f = 0; f < sizz; f++) {
		for (x = 0; x < sizx; x++) {
			for (y = 0; y < sizy; y++) {
				F[y + f*sizy + x*(sizy*sizz)] =  
					Old[y + x*sizy + f*(sizy*sizx)];
			}
		}
    }
}

/* convolve A and B using cblas */
class convolve {
    thread_data* const thread_data_array;

public:
    convolve(thread_data* thread_data) : thread_data_array(thread_data) {}

    void operator()(const tbb::blocked_range<size_t>& range) const {
        for (size_t i=range.begin(); i!=range.end(); ++i) {
            run(i);
        }
    }

    void run(size_t i) const {
        thread_data args = thread_data_array[i];
        double *A = args.A.vals;
        double *B = args.B.vals;
        double *C = args.C.vals;

        for (unsigned x = 0; x < args.C.sizx; x++) {
            for (unsigned y = 0; y < args.B.sizz; y++) {
                double *A_off = A + x*(args.A.sizy*args.A.sizx) + y;
                double *B_off = B + y*(args.B.sizy*args.B.sizx);
                double *C_off = C + x*(args.C.sizy);
                double one = 1;
                int m = args.C.sizy;
                int n = args.B.sizy*args.B.sizx;
                int lda = args.A.sizy;
                int incx = 1;
                int incy = 1;
                cblas_dgemv(CblasColMajor,CblasNoTrans, m, n, one, A_off, lda, B_off, incx, one, C_off, incy);
            }
        }
    }
};

/*
 * entry point
 * resp = eHconv(cell of B, A, start, end);
 */
mat3d_t* filterv_apply(const vector<filter_t> filters, const mat3d_t* feats, int start, int end, bool multiThreaded) {
 //void  eHconv(vector<mat2d_t*>& resps, const mat3d_t* feats, const vector<filter_t> filters, int start, int end) {

  size_t len = end-start+1;
  assert(end>=start);
  assert(len<=filters.size());
  int filter_h = filters[0].w.sizy;
  int filter_w = filters[0].w.sizx;
  int filter_z = filters[0].w.sizz;
  int filter_len = filter_h*filter_w*filter_z;
  int height = feats->sizy - filter_h + 1;
  int width = feats->sizx - filter_w + 1;
  assert(height>=1 && width>=1);
  mat3d_t* resps= mat3d_alloc(height, width, filters.size());
  /*XXX necessary */
  for(unsigned i=0;i<resps->sizx*resps->sizy*resps->sizz;i++)
	  resps->vals[i]=0;

  double *tmp_filter, *tmp_feats;
  thread_data* td = new thread_data[len];
  tmp_feats = new double[feats->sizy*feats->sizx*feats->sizz];
  tmp_filter = new double[filter_len*len];

  prepare_map(tmp_feats,feats->vals,feats->sizy,feats->sizx,feats->sizz);

  for (size_t i = 0; i < len; ++i) {
	  td[i].A.vals = tmp_feats;
	  td[i].A.sizy = feats->sizy;
	  td[i].A.sizx = feats->sizz;
      td[i].A.sizz = feats->sizx;
      td[i].B.vals = prepare_filter(tmp_filter+i*filter_len, filters[i].w.vals,
          filter_h, filter_w, filter_z);
	  td[i].B.sizy = filter_z;
	  td[i].B.sizx = filter_w;
	  td[i].B.sizz = filter_h;
	  td[i].C.vals = resps->vals+i*height*width;
	  td[i].C.sizy = height;
	  td[i].C.sizx = width;
  }
  if (multiThreaded) {
    tbb::parallel_for(tbb::blocked_range<size_t>(0, len), convolve(td));
  }
  else {
      convolve process(td);
      for (size_t i=0; i<len; ++i) {
          process.run(i);
      }
  }

  delete[] tmp_filter;
  delete[] tmp_feats;
  delete[] td;
  return resps;
}

