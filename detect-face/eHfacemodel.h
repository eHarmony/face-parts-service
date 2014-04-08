/** @file eHfacemodel.h
 *  @brief face detection model and operations
 *
 *  @sa Xiangzin Zhu, Deva Ramanan, "Face Detection, Pose Estimation, and landmark Localization in the Wild". In  CVPR 2012.
 *
 * @author Hang Su
 * @date 2012-08-13
 */
#ifndef EHFACEMODEL_H
#define EHFACEMODEL_H

#include <vector>
#include "eHimage.h"
#include "eHfilter.h"
#include "eHbbox.h"
#include "eHposemodel.h"

//#define EH_TEST_TIMER

using std::vector;

typedef struct deformation_face {
	int i;
	int anchor[3]; /*ax ay ds*/
	double w[4]; /*only 1st is used for root*/
} facedef_t;

typedef struct part_face {
	int defid;
	int filterid;
	int parent;
	/* infos not directly from file */
	int sizy;
	int sizx;
	int scale;
	int startx;
	int starty;
	int step;
} facepart_t;

/** @struct eHfacemodel
 *  @brief Face detection model
 */
struct eHfacemodel {
	vector<filter_t> filters; 		/**< @brief part filters 
						  @note All part filters should be 
						  of the same size*/
	vector<facedef_t> defs; 		/**< @brief deformation params */
	vector<vector<facepart_t> > components; /**< @brief part infos */
	int maxsize[2]; 			/**< @brief XXX */
	int len; 				/**< @brief not used */
	int interval; 				/**< @brief interval of pyramid */
	int sbin; 				/**< @brief bin for building hog feature */
	double delta; 				/**< @brief not used */
	double thresh; 				/**< @brief threshold for detection score */
	double obj; 				/**< @brief not used */
};

/** @typedef facemodel_t
 *  @brief Face detection model
 */
typedef struct eHfacemodel facemodel_t;

/** @brief Parse face model from xml style string
 *  @note xmlstr is modified during parsing, this can be avoided by 
 *  using Non-Destrutive Mode of rapidxml
 */
facemodel_t* facemodel_parseXml(char* xmlstr);

/** @brief Read face model from file
 *  @sa facemodel_parseXml()
 */
facemodel_t* facemodel_readFromFile(const char* filepath);

/** @brief Perform face detection
 *  @param model face detection model
 *  @param img where to find faces from
 *  @param thrs threshold used for pruning results
 *  @return array of detected faces (together with part locations)
 */
vector<bbox_t> facemodel_detect(const facemodel_t* model, const image_ptr img, double thrs);

/** @brief Perform face detection using threshold inside model
 *  @param model face detection model
 *  @param img where to find faces from
 *  @return array of detected faces (together with part locations)
 */
vector<bbox_t> facemodel_detect(const facemodel_t* model, const image_ptr img);

/** @brief Perform face detection with help of body detection using 
 *  thresholds inside models
 *  @param facemodel face detection model
 *  @param posemodel body pose detection model
 *  @param img where to find faces from
 *  @return array of detected faces (together with part locations)
 */
vector<bbox_t> facemodel_detect(const facemodel_t* facemodel, const posemodel_t* posemodel, const image_ptr img);

/** @brief Delete a face model, release related memory
 */
void facemodel_delete(facemodel_t* model);

#endif
