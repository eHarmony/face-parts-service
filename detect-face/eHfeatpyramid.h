/** @file eHfeatpyramid.h
 *  @brief Feature pyramid data type and calculation
 *
 *  @author Hang Su
 *  @date 2012-08-14
 */
#ifndef EHFEATPYRAMID_H
#define EHFEATPYRAMID_H

#include "eHmatrix.h"
#include "eHimage.h"

/** @struct eHfeatpyramid
 *  @brief Image feature pyramid
 */
struct eHfeatpyramid {
	mat3d_ptr* feat; 	/**< @brief features of each level */
	double* scale; 		/**< @brief scaled of each level */
	int len; 		/**< @brief levels of pyra, size of feat & scale */
	int interval; 		/**< @brief levels within a double-size interval */
	int imy; 		/**< @brief image height */
	int imx; 		/**< @brief image width */
};

/** @typedef featpyra_t
 *  @brief Pointer to a feature pyramid
 */
typedef struct eHfeatpyramid featpyra_t;

/** @brief Allocate and compute a feature pyramid from an image
 *  @param hallucinate whether hallucinate a higher resolution interval
 */
featpyra_t* featpyra_create(const image_ptr im, int interval, int sbin, const int* maxsize, bool hallucinate=true);

/** @brief Delete a feature pyramid
 */
void featpyra_delete(featpyra_t* pyra);

#endif
