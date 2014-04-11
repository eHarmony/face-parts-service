/** @file eHmatrix.h
 *  @brief Basic matrix types and operations
 *
 *  @author Hang Su
 *  @date 2012-07
 */
#ifndef EHMATRIX_H
#define EHMATRIX_H

#include <stdlib.h>

/** @struct mat2d_t
 *  @brief 2D matrix
 *  @note matrix is stored in column-major style
 */
struct mat2d_t {
	double* vals; 	/**< @brief values */
	size_t sizy; 	/**< @brief matrix height */
	size_t sizx; 	/**< @brief matrix width */
};

/** @struct eHmatrix3d
 *  @brief 3D matrix
 *  @note matrix is stored in column-row-page order
 */
struct mat3d_t {
	double* vals; 	/**< @brief values */
	size_t sizy; 	/**< @brief matrix height */
	size_t sizx; 	/**< @brief matrix width */
	size_t sizz; 	/**< @brief matrix depth */
};

/** @struct eHmatrixkd
 *  @brief k-dimension matrix
 */
struct matkd_t {
	double* vals; 	/**< @brief values */
	size_t k; 	/**< @brief number of dimensions */
	size_t* siz; 	/**< @brief sizes along each dimension */
}; 

/** @brief Allocate a 2D matrix
 *  @sa mat2d_delete()
 */
mat2d_t* mat2d_alloc(size_t sizy, size_t sizx);

/** @brief Destruct a 2D matrix
 *  @sa mat2d_alloc()
 */
void mat2d_delete(mat2d_t*);

/** @brief Allocate a 3D matrix
 *  @sa mat3d_delete()
 */
mat3d_t* mat3d_alloc(size_t sizy, size_t sizx, size_t sizz);

/** @brief Destruct a 3D matrix
 *  @sa mat3d_alloc()
 */
void mat3d_delete(mat3d_t*);

/** @brief Pad 3d matrix
 *  @param pad width of padding along each dimension
 *  @param pad_val values to be padded in each dimension
 */
void mat3d_pad(mat3d_t* mat, const size_t* pad, double pad_val);

/** @brief Fill continous region of a 3D matrix with fill_val
 *  @param start starting point
 *  @param width width in each dimension
 *  @param fill_val the value to be filled in
 */
void mat3d_fill(mat3d_t* mat, const size_t* start, const size_t* width, double fill_val);

/** @brief Allocate a kD matrix
 *  @sa matkd_delete()
 */
matkd_t* matkd_alloc(size_t k, size_t* sizs);

/** @brief Delete a kD matrix
 *  @sa matkd_alloc()
 */
void matkd_delete(matkd_t*);

#endif
