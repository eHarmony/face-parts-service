/** @file eHshiftdt.h
 *
 *  @brief Generalized Distance Transform
 *
 *  @sa P. F. Felzenszwalb and D. P. Huttenlocher, "Distance Transforms of Sampled Functions". 2004.
 *  @author Hang Su
 *  @date 2012-07
 */

/** @brief Perform generalized distance transform
 *  
 *  This applies computes a min convolution of a quadratic function ax^2+bx
 *  This outputs results on a shifted(offy, offx), subsampled(dstep) grid
 *  @note M, Ix, Iy should be properly allocated before passed in, 
 *  they are then modified as output results
 */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		int lenx, int leny, int offx, int offy, int dstep, 
		const double* vals, int sizx, int sizy, 
		const double* w);

/* wrapper for default setting */
void eHshiftdt(double* M, int* Ix, int* Iy, 
		const double* vals, int sizx, int sizy, 
		const double* w);
