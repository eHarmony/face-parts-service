/** @file eHimageFeature.h
 *  @brief Compute image features
 *
 *  @author Hang Su
 *  @date 2012-08
 */
#include "eHimage.h"
#include "eHmatrix.h"

/** @brief Compute HOG feature of a color image
 *  @param img 3 channel double precision image
 *  @param sbin bin size
 *  @return HOG feature matrix
 *  @note feature matrix is allocated inside, proper delete is necessary after use
 */
mat3d_ptr eHhog(const image_ptr img, int sbin);
