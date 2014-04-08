/** @file eHbbox.h
 *  @brief Multi-box bounding box type and operations
 *
 *  A "bounding box" is defined as a collection of boxes, which serve as 
 *  detection results of different parts for part-based detection algorithms.
 *
 * @author Hang Su
 * @date 2012-08-13
 */
#ifndef EHBBOX_H
#define EHBBOX_H

#include "eHbox.h"

#include <map>
#include <vector>

/** @def EH_BBOXS_PRUNE
 *  @brief Default pruning parameter for bbox_v_nms()
 *  @sa bbox_v_nms()
 */
#define EH_BBOXS_PRUNE 30000

/** @struct eHbbox
 *  @brief Multi-box "bounding box", used for detection result
 */
struct eHbbox {
	std::vector<fbox_t> boxes; /**< @brief part locations */
	double score; /**< @brief detection score @warning not calibrated */
	int component; /**< @brief component number for certain models */
	fbox_t outer; /**< @brief outer "real" bounding box of the detection */
	double area; /**< @brief area of outer */
};

/** @typedef bbox_t
 *  @brief Multi-box "bounding box", used for detection result
 */
typedef struct eHbbox bbox_t;

/** @brief Filling the fields of given bbox: outer, area
 */
void bbox_calcOut(bbox_t*);

/** @brief Clip the boxes to image boundary
 *
 *  @param imsize imsize[0]=height, imsize[1]=width
 *  @sa fbox_clip()
 */
void bbox_clipboxes(bbox_t& bbox, const int* imsize);

/** @brief Resize the input bboxs (in-place)
 *  @sa fbox_resize()
 */
void bbox_v_resize(std::vector<bbox_t>& bboxes, double scale);

/** @brief Move the input bboxs (in-place)
 *  @sa fbox_move()
 */
void bbox_v_move(std::vector<bbox_t>& bboxes, const int* offset);

/** @brief Non-maximum suppression
 *
 *  Greedily select high-scoring detections and skip detections that are 
 *  significantly coverd by a previously selected detection.
 *
 *  @param bboxes an array of detection results, changed inside function
 *  @param overlap two results are not both kept if their overlap ratio exceed this value
 *  @param prune initial bboxes are pruned for higher speed
 */
void bbox_v_nms(std::vector<bbox_t>& bboxes, double overlap, unsigned prune = EH_BBOXS_PRUNE);

#endif
