/** @file eHfilter.h
 *  @brief Filters applied on image features
 *
 *  @author Hang Su
 *  @date 2012-08
 */
#ifndef EHFILTER_H
#define EHFILTER_H

#include "eHmatrix.h"
#include <vector>

/** @struct eHfilter
 *  @brief Image feature filter
 */
struct eHfilter {
	int i; 		/**< @brief filter index, not used */
	mat3d_t w;  	/**< @brief filter [y,x,f], where (y,x) is location, f is feature index */
};

/** @typedef filter_t
 *  @brief Image feature filter
 */
typedef struct eHfilter filter_t;

/** @brief Convolve a feature map with a set of filters - Multithreaded version
 *  @param filters a set of part filters
 *  @param feats feature map 
 *  @param start range of filters used - first one
 *  @param end range of filters used - last one
 *  @return filter responses
 *  @note filter responses is allocated inside, proper delete is necessary after use
 *  @note cblas library is required
 *  @sa filterv_apply_ST()
 */
mat3d_ptr filterv_apply(const std::vector<filter_t> filters, const mat3d_ptr feats, int start, int end);

/** @brief Convolve a feature map with a set of filters - Singlethreaded version
 *  @param filters a set of part filters
 *  @param feats feature map 
 *  @param start range of filters used - first one
 *  @param end range of filters used - last one
 *  @return filter responses
 *  @note filter responses is allocated inside, proper delete is necessary after use
 *  @sa filterv_apply()
 */
mat3d_ptr filterv_apply_ST(const std::vector<filter_t> filters, const mat3d_ptr feats, int start, int end);

#endif
