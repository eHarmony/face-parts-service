/** @file eHimage.h
 *  @brief Basic image type and operations
 *  @note images are stored using column major style
 *
 *  @author Hang Su
 *  @date 2012-08-13
 */
#ifndef EHIMAGE_H
#define EHIMAGE_H

#include <stdlib.h>
#include <string>

#include "eHbox.h"
#include "eHbbox.h"

/** @struct eHimage
 *  @brief Basic image data structure
 *  @note Using column major (Fortran) style
 *  @note Data is associated with (double* data), (double* ch[3]) only 
 *  provide a view into data
 */
struct image_t {
	double* data; 		/**< @brief pixel value data */
	double* ch[3]; 		/**< @brief a view into each channel */
	size_t sizy; 		/**< @brief image height */
	size_t sizx; 		/**< @brief image width */
	size_t nchannel; 	/**< @brief number of channels */
	int imsize[3]; 		/**< @brief [sizy sizx nchanel] */
	bool is_shared; 	/**< @brief whether share data with a parent image */
	size_t stepy; 		/**< @brief step between columns */
	size_t stepyx; 		/**< @brief step between channels */
}; 

/** @brief Allocate a new image of size [sizy, sizx, nch]
 *  @return pointer to the allocated image
 *  @note Returned image is not initialized
 */
image_t* image_alloc(size_t sizy, size_t sizx, size_t nch = 3);

/** @brief Allocate a new image of size [sizy, sizx, nch], and initialize 
 *  all pixel values to fill
 *  @return pointer to the allocated image
 */
image_t* image_alloc(size_t sizy, size_t sizx, size_t nch, const double* fillval);

/** @brief Delete image and associated memory
 *  @note If it's a shared image(the "child"), no data is destroyed; if the image 
 *  that owns the data is deleted, all descendants are not accessible anymore
 */
void image_delete(image_t* img);

/** @brief Fill all pixels with same values
 *  @param img target
 *  @param val value to be filled to each pixel, it should be at least 
 *  the same length as img->nchannel
 */
void image_fill(image_t* img, const double* val);

/** @brief Read Jpeg image file
 *  @return pointer to allocatd image, NULL if failed
 */
image_t* image_readJPG(const char* filename);
	
/** @brief Fast image subsampling
 *  
 *  Unlike image_resize(), this function can only be used to down-scale 
 *  an image, and focus more on anti-aliasing when building pyramid
 *  @param img the image to be subsampled
 *  @param scale subsample scale (<1)
 *  @return subsampleded image, or NULL if scale>1
 *  @sa image_resize()
 *  @note input image remains alive and unchanged
 */
image_t* image_subsample(const image_t* img, double scale);

/** @brief Get an image half the size of input one
 *  @param img the image to be reduced
 *  @return reduced image
 *  @note input image remains alive and unchanged
 */
image_t* image_reduce(const image_t* img);

/** @brief Resize an image using bilateral interpolation
 *  @param image the image to be resized
 *  @param scale resizing scale
 *  @return resized image
 *  @sa image_subsample()
 *  @note input image remains alive and unchanged
 */
image_t* image_resize(const image_t* img, double scale);

/** @brief Crop image
 *  This function can be used in two ways, either get shared data from original 
 *  image, or allocate a new image, which is more expensive.
 *  @param img original image
 *  @param crop crop area within img
 *  @param store offset [offy offx] of the cropped patch inside image if not NULL
 *  @param shared indicate whether the result shares data with original image
 *  @return cropped image patch, NULL if allocation failed
 */
image_t* image_crop(const image_t* img, fbox_t crop, int* offset=NULL, bool shared=true);

#endif
