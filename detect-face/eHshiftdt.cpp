/*
 * eHshiftdt.cpp
 *
 * Generalized Distance Transform
 * see: "Distance Transforms of Sampled Functions" 
 * (P. F. Felzenszwalb and D. P. Huttenlocher, 2004)
 * This applies computes a min convolution of a quadratic function ax^2+bx
 *
 * Hang Su
 * 2012-07 @ eH
 */
#define EH_INF 1E20
#define EH_MAX_LEN 800

#include <math.h>
#include "eHmatrix.h"
#include "eHshiftdt.h"

static inline int square(int x) {return x*x;}

/* 
 * 1-d distance transform with quadratic distance: ax^2+bx
 * result is on an shifted, subsampled grid
 * used by eHshiftdt()
 */
void dt1d(const double* src, double* dst, int* ptr, int sstep, int slen, 
		double a, double b, int dshift, double dstep, int dlen);

/* wrapper for default setting */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		const double* vals, int sizx, int sizy, 
		const double* w) {
	eHshiftdt(M, Ix, Iy, sizx, sizy, 0, 0, 1, vals, sizx, sizy, w);
}

/*
 * NOTE: M, Ix, Iy should already be allocated before passed in, 
 * they are then modified as output results
 * M, Ix, Iy, vals - column first order
 */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		int lenx, int leny, int offx, int offy, int dstep, 
		const double* vals, int sizx, int sizy, 
		const double* w) {
		//double ax, double bx, double ay, double by) {
	/*
	 * Calculation is performed as 1-d transforms in 2 steps
	 * vals (sizy*sizx) => tmpM (leny*sizx) => M (leny*lenx)
	 */
	double* tmpM;
	int* tmpIy;
	/* negating to define a cost */
	double ax = -w[0];
	double bx = -w[1];
	double ay = -w[2];
	double by = -w[3];

	tmpM = new double[leny*sizx];
	tmpIy = new int[leny*sizx];

	/* 1-d distance transforms on columns */
	for(int x=0; x<sizx; x++) {
		dt1d(vals+x*sizy, tmpM+x*leny, tmpIy+x*leny, 1, sizy, ay, by, offy, dstep, leny);
	}

	/* 1-d distance transforms on rows */
	for(int y=0; y<leny; y++) {
		dt1d(tmpM+y, M+y, Ix+y, leny, sizx, ax, bx, offx, dstep, lenx);
	}

	/* get argmins */
	for (int p=0; p<leny*lenx; p++){
		int y = p%leny;
		Iy[p] = tmpIy[Ix[p]*leny+y];
	}

	delete[] tmpM;
	delete[] tmpIy;
}

void dt1d(const double* src, double* dst, int* ptr, int sstep, int slen, 
		double a, double b, int dshift, double dstep, int dlen) {
	int* v = new int[slen];
	double* z = new double[slen+1];

	/*
	 * k - index of rightmost parabola
	 * v - locations of parabolas
	 * z - locations of boundaries between parabolas
	 * q - running index
	 * s - intersection between two parabolas
	 */
	int k=0;
	int q=0;
	v[0] = 0;
	z[0] = -EH_INF;
	z[1] = +EH_INF;

	for (q = 1; q <= slen-1; q++) {
		double s = ((src[q*sstep]-src[v[k]*sstep])-b*(q-v[k])+a*(square(q)-square(v[k]))) / (2*a*(q-v[k]));
		while(s <= z[k]) {
			k--;
			s = ((src[q*sstep]-src[v[k]*sstep])-b*(q-v[k])+a*(square(q)-square(v[k]))) / (2*a*(q-v[k]));
		}
		k++;
		v[k] = q;
		z[k] = s;
		z[k+1] = +EH_INF;
	}

	k = 0;
	q = dshift;

	/* fill in values of distance transform */
	for(int i=0; i <= dlen-1; i++) {
		while(z[k+1] < q)
			k++;
		dst[i*sstep] = a*square(q-v[k])+b*(q-v[k])+src[v[k]*sstep];
		ptr[i*sstep] = v[k];
		q += dstep;
	}

	delete[] z;
	delete[] v;
}
