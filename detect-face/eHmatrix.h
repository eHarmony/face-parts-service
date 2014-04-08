/** @file eHmatrix.h
 *  @brief Basic matrix types and operations
 *
 *  @author Hang Su
 *  @date 2012-07
 */
#ifndef EHMATRIX_H
#define EHMATRIX_H

#include <stdlib.h>

/** @struct eHmatrix2d
 *  @brief 2D matrix
 *  @note matrix is stored in column-major style
 */
struct eHmatrix2d {
	double* vals; 	/**< @brief values */
	size_t sizy; 	/**< @brief matrix height */
	size_t sizx; 	/**< @brief matrix width */
};

/** @typedef mat2d_t
 *  @brief 2D matrix
 */
typedef struct eHmatrix2d mat2d_t;

/** @typedef mat2d_ptr
 *  @brief pointer to a 2D matrix
 */
typedef mat2d_t* mat2d_ptr;

/** @struct eHmatrix3d
 *  @brief 3D matrix
 *  @note matrix is stored in column-row-page order
 */
struct eHmatrix3d {
	double* vals; 	/**< @brief values */
	size_t sizy; 	/**< @brief matrix height */
	size_t sizx; 	/**< @brief matrix width */
	size_t sizz; 	/**< @brief matrix depth */
};

/** @typedef mat3d_t
 *  @brief 3D matrix
 */
typedef struct eHmatrix3d mat3d_t;

/** @typedef mat3d_ptr
 *  @brief pointer to a 3D matrix
 */
typedef mat3d_t* mat3d_ptr;

/** @struct eHmatrixkd
 *  @brief k-dimension matrix
 */
struct eHmatrixkd {
	double* vals; 	/**< @brief values */
	size_t k; 	/**< @brief number of dimensions */
	size_t* siz; 	/**< @brief sizes along each dimension */
}; 

/** @typedef matkd_t
 *  @brief k-dimension matrix
 */
typedef struct eHmatrixkd matkd_t;

/** @typedef matkd_ptr
 *  @brief pointer to a k-dimension matrix
 */
typedef matkd_t* matkd_ptr;

/** @brief Allocate a 2D matrix
 *  @sa mat2d_delete()
 */
mat2d_ptr mat2d_alloc(size_t sizy, size_t sizx);

/** @brief Destruct a 2D matrix
 *  @sa mat2d_alloc()
 */
void mat2d_delete(mat2d_ptr);

/** @brief Allocate a 3D matrix
 *  @sa mat3d_delete()
 */
mat3d_ptr mat3d_alloc(size_t sizy, size_t sizx, size_t sizz);

/** @brief Destruct a 3D matrix
 *  @sa mat3d_alloc()
 */
void mat3d_delete(mat3d_ptr);

/** @brief Pad 3d matrix
 *  @param pad width of padding along each dimension
 *  @param pad_val values to be padded in each dimension
 */
void mat3d_pad(mat3d_ptr mat, const size_t* pad, double pad_val);

/** @brief Fill continous region of a 3D matrix with fill_val
 *  @param start starting point
 *  @param width width in each dimension
 *  @param fill_val the value to be filled in
 */
void mat3d_fill(mat3d_ptr mat, const size_t* start, const size_t* width, double fill_val);

/** @brief Allocate a kD matrix
 *  @sa matkd_delete()
 */
matkd_ptr matkd_alloc(size_t k, size_t* sizs);

/** @brief Delete a kD matrix
 *  @sa matkd_alloc()
 */
void matkd_delete(matkd_ptr);

#endif
