/** @file eHposemodel.h
 *  @brief Human body/pose detection model and operations
 *  @sa Y. Yang, D. Ramanan, "Articulated Pose Estimation using Flexible Mixtures of Parts". In CVPR 2011.
 *
 *  @author Hang Su
 *  @date 2012-08
 */
#ifndef EHPOSEMODEL_H
#define EHPOSEMODEL_H

#include <vector>
#include "eHimage.h"
#include "eHfilter.h"
#include "eHbbox.h"

using std::vector;

typedef struct deformation_pose {
	int i;
	int anchor[3];
	double w[4];
} posedef_t;

typedef struct part_pose {
	int* biasid; /*numpar x num (except for root, which is 1)*/
	int* defid; /*num (except for root, which is NULL)*/
	int* filterid; /*num*/
	int num;
	int numpar;
	int parent;
	/* infos not directly from file */
	int* sizy; /*num*/
	int* sizx;/*num*/
	int scale;
	int step;
	int* startx;/*num*/
	int* starty;/*num*/
} posepart_t;

typedef struct bias_pose {
	int i;
	double w;
} posebias_t;

/** @struct eHposemodel
 *  @brief Human body/pose mdoel
 */
struct eHposemodel {
	vector<posebias_t> biases; 	/**< @brief bias towards part combinations */
	vector<filter_t> filters; 	/**< @brief part filters @note all filters should be of the same size */
	vector<posedef_t> defs;  	/**< @brief deformation params */
	vector<posepart_t> parts; 	/**< @brief part configurations */
	int maxsize[2]; 		/**< @brief XXX */
	int len; 			/**< @brief not used */
	int interval; 			/**< @brief interval of feature pyramid */
	int sbin; 			/**< @brief bin for building hog feature */
	double thresh; 			/**< @brief threshold for detection score */
	double obj; 			/**< @brief not used */
};

/** @typedef posemode_t
 *  @brief Human body/pose model
 */
typedef struct eHposemodel posemodel_t;

/** @brief Parse body/pose model from xml style string
 *  @note xmlstr is modified during parsing, this can be avoided by 
 *  using Non-Destrutive Mode of rapidxml
 */
posemodel_t* posemodel_parseXml(char* xmlstr);

/** @brief Read body/pose model from file
 *  @sa posemodel_parseXml()
 */
posemodel_t* posemodel_readFromFile(const char* filepath);

/** @brief Perform body/pose detection
 *  @param model pose detection model
 *  @param img where to find human poses from
 *  @param thrs threshold used for pruning results
 *  @return array of detected poses (together with part locations)
 */
vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img, double thrs);

/** @brief Perform body/pose detection using default threshold
 *  @param model pose detection model
 *  @param img where to find human poses from
 *  @return array of detected poses (together with part locations)
 */
vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img);

/** @brief Delete a pose model, release related memory
 */
void posemodel_delete(posemodel_t* model);

#endif
