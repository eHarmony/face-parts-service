/** @file eHbox.h
 *  @brief Box types and operations
 *
 *  Boxes are defined as tuples (x1, y1, x2, y2), where 
 *  x1<=x2, y1<=y2; so (x1,y1) is the top-left corner, 
 *  (x2,y2) is the bottom-right corner.
 *
 *  @author Hang Su
 *  @date 2012-08-13
 */
#ifndef EHBOX_H
#define EHBOX_H

#include <stdlib.h>
#include <vector>
using std::vector;

/** @struct eHbox_f
 *  @brief box with floating-point boundaries
 *
 *  Boxes are defined as tuples (x1, y1, x2, y2), where 
 *  x1<=x2, y1<=y2; so (x1,y1) is the top-left corner, 
 *  (x2,y2) is the bottom-right corner.
 */
struct eHbox_f {
	double x1; /**< @brief left bound */
	double y1; /**< @brief up bound */
	double x2; /**< @brief right bound */
	double y2; /**< @brief bottom bound */
};

/** @typedef fbox_t
 *  @brief box with floating-point boundaries
 */
typedef struct eHbox_f fbox_t;

/** @struct eHbox_i
 *  @brief box with integer boundaries
 *
 *  Boxes are defined as tuples (x1, y1, x2, y2), where 
 *  x1<=x2, y1<=y2; so (x1,y1) is the top-left corner, 
 *  (x2,y2) is the bottom-right corner.
 */
struct eHbox_i {
	int x1; /**< @brief left bound */
	int y1; /**< @brief up bound */
	int x2; /**< @brief right bound */
	int y2; /**< @brief bottom bound */
};

/** @typedef ibox_t
 *  @brief box with integer boundaries
 */
typedef struct eHbox_i ibox_t;

/** @brief Set values for given box
 */
void fbox_set(fbox_t* box, double x1, double y1, double x2, double y2);

/** @brief Round floating-point box to intege box
 */
ibox_t fbox_getibox(fbox_t* box);

/** @brief Set values for given box
 */
void ibox_set(ibox_t*, int x1, int y1, int x2, int y2);

/** @brief Compute area of intersection between two boxes
 */
double fbox_interArea(const fbox_t& box1, const fbox_t& box2);

/** @brief Clip box so it's limited by the image size
 */
void fbox_clip(fbox_t& box, const int* imsize);

/** @brief Merge boxes, resulting box can be optionally padded
 *
 *  @param boxes array of boxes to be merged
 *  @param idxs indexs within vector, indicating which boxes to merge
 *  @param num length of idxs
 *  @param padding [l r u d] as ratio of corresponding edge, if NULL is passed in, 
 *  no padding is performed
 *  @return merged box
 */
fbox_t fbox_merge(const vector<fbox_t>& boxes, const int* idxs, int num, const double* padding=NULL);

/** @brief Get a resized box, input box is not changed
 *  @sa fbox_resize()
 */
fbox_t fbox_getResized(const fbox_t& box, double scale);

/** @brief Get a moved box, input box is not changed
 *  @sa fbox_move()
 */
fbox_t fbox_getMoved(const fbox_t& box, const int* offset);

/** @brief Resize the input box
 *  @sa fbox_getResized()
 */
void fbox_resize(fbox_t* box, double scale);

/** @brief Move the position of input box by given offset
 *  @sa fbox_getMoved()
 */
void fbox_move(fbox_t* box, const int* offset);

#endif
