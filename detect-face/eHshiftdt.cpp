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

#include <parallel_for.h>
#include <math.h>
#include "eHmatrix.h"
#include "eHshiftdt.h"

static inline int square(int x) {return x*x;}

/* wrapper for default setting */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		const double* vals, int sizx, int sizy, 
        const double* w, bool multiThreaded) {
    eHshiftdt(M, Ix, Iy, sizx, sizy, 0, 0, 1, vals, sizx, sizy, w, multiThreaded);
}

struct shiftData {
    const double* src;
    double* dst;
    int* ptr;
    int sstep;
    int slen;
    double a;
    double b;
    int dshift;
    double dstep;
    int dlen;
};

/*
 * 1-d distance transform with quadratic distance: ax^2+bx
 * result is on a shifted, subsampled grid
 * used by eHshiftdt().
 *
 * Profiling has shown this to be a slow point, so there has been some work to try and make this faster
 */
class dt1d {
    shiftData* const data;
public:
    dt1d(shiftData* thread_data) : data(thread_data) {}

    void operator()(const tbb::blocked_range<size_t>& range) const {
        for (size_t i=range.begin(); i!=range.end(); ++i) {
            run(i);
        }
    }

    void run(size_t i) const {
        shiftData currentData = data[i];
        int* v = new int[currentData.slen];
        double* z = new double[currentData.slen+1];

        int* vBegin = v;
        double* zBegin = z;

        /*
         * v - locations of parabolas
         * z - locations of boundaries between parabolas
         * q - running index
         * s - intersection between two parabolas
         */
        int q=0;
        *v = 0;
        *z = -EH_INF;
        *(z+1) = +EH_INF;
        double twoA = 2*currentData.a;

        for (q = 1; q < currentData.slen; ++q) {
            int qSstep = q*currentData.sstep;
            int qSquare = square(q);
            double s = ((currentData.src[qSstep]-currentData.src[*v*currentData.sstep])-
                    currentData.b*(q-*v)+currentData.a*(qSquare-square(*v))) / (twoA*(q-*v));
            while(s <= *z) {
                --v;
                --z;
                s = ((currentData.src[qSstep]-currentData.src[*v*currentData.sstep])-
                        currentData.b*(q-*v)+currentData.a*(qSquare-square(*v))) / (twoA*(q-*v));
            }
            ++v;
            ++z;
            *v = q;
            *z = s;
            *(z+1) = +EH_INF;
        }

        v = vBegin;
        z = zBegin;
        q = currentData.dshift;

        /* fill in values of distance transform */
        for(int i=0; i < currentData.dlen; ++i) {
            while(*(z+1) < q) {
                ++z;
                ++v;
            }
            currentData.dst[i*currentData.sstep] = currentData.a*square(q-*v)+
                    currentData.b*(q-*v)+currentData.src[*v*currentData.sstep];
            currentData.ptr[i*currentData.sstep] = *v;
            q += currentData.dstep;
        }

        delete[] zBegin;
        delete[] vBegin;
    }
};

/*
 * NOTE: M, Ix, Iy should already be allocated before passed in, 
 * they are then modified as output results
 * M, Ix, Iy, vals - column first order
 */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		int lenx, int leny, int offx, int offy, int dstep, 
		const double* vals, int sizx, int sizy, 
        const double* w, bool multiThreaded) {
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
    shiftData* data = new shiftData[sizx];
    for(int x=0; x<sizx; x++) {
        data[x].src = vals+x*sizy;
        data[x].dst = tmpM+x*leny;
        data[x].ptr = tmpIy+x*leny;
        data[x].sstep = 1;
        data[x].slen = sizy;
        data[x].a = ay;
        data[x].b = by;
        data[x].dshift = offy;
        data[x].dstep = dstep;
        data[x].dlen = leny;
	}

    if (multiThreaded) {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, sizx), dt1d(data));
    }
    else {
        dt1d process(data);
        for (int i=0; i<sizx; ++i) {
            process.run(i);
        }
    }

    delete[] data;

    data = new shiftData[sizy];
	/* 1-d distance transforms on rows */
	for(int y=0; y<leny; y++) {
        data[y].src = tmpM+y;
        data[y].dst = M+y;
        data[y].ptr = Ix+y;
        data[y].sstep = leny;
        data[y].slen = sizx;
        data[y].a = ax;
        data[y].b = bx;
        data[y].dshift = offx;
        data[y].dstep = dstep;
        data[y].dlen = lenx;
	}

    if (multiThreaded) {
      tbb::parallel_for(tbb::blocked_range<size_t>(0, sizy), dt1d(data));
    }
    else {
        dt1d process(data);
        for (int i=0; i<sizy; ++i) {
            process.run(i);
        }
    }

    delete[] data;

	/* get argmins */
	for (int p=0; p<leny*lenx; p++){
		int y = p%leny;
		Iy[p] = tmpIy[Ix[p]*leny+y];
	}

	delete[] tmpM;
	delete[] tmpIy;
}
