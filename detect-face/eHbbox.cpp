/*
 * eHbbox.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHbbox.h"
#include <vector>
#include <list>
#include <algorithm>

#define EH_INF 1E20

using std::vector;
using std::list;

static inline double min(double x, double y) {return (x <= y ? x : y); }
static inline double max(double x, double y) {return (x <= y ? y : x); }

/*
 * filling the fields of given bbox: 
 * outer - bounding box covering all parts
 * area - area of outer
 */
void bbox_calcOut(bbox_t* bbox){
	vector<fbox_t>::iterator iter;
	fbox_set(&(bbox->outer),EH_INF,EH_INF,-EH_INF,-EH_INF);
	for(iter=bbox->boxes.begin();iter!=bbox->boxes.end();iter++)
		fbox_set(&(bbox->outer),
				min(bbox->outer.x1,iter->x1),
				min(bbox->outer.y1,iter->y1),
				max(bbox->outer.x2,iter->x2),
				max(bbox->outer.y2,iter->y2)
		      );
	bbox->area = max(0,(bbox->outer.x2-bbox->outer.x1))
		*max(0,(bbox->outer.y2-bbox->outer.y1));
}

void bbox_clipboxes(bbox_t& bbox, const int* imsize) {
	for(unsigned i = 0;i<bbox.boxes.size();i++) {
		fbox_clip(bbox.boxes[i], imsize);
	}
}

void bbox_v_resize(vector<bbox_t>& bboxes, double scale) {
	for(unsigned i=0;i<bboxes.size();i++) {
		for(unsigned j=0;j<bboxes[i].boxes.size();j++) {
			fbox_resize(&(bboxes[i].boxes[j]),scale);
		}
	}
}

void bbox_v_move(vector<bbox_t>& bboxes, const int* offset) {
	for(unsigned i=0;i<bboxes.size();i++) {
		for(unsigned j=0;j<bboxes[i].boxes.size();j++) {
			fbox_move(&(bboxes[i].boxes[j]),offset);
		}
	}
}
bool compare_score(bbox_t a, bbox_t b) {return (a.score>b.score);}

void bbox_v_nms(vector<bbox_t>& bboxes, double overlap, unsigned prune) {
	if (bboxes.empty())
		return ;
	/* sort bboxes according to score */
	sort(bboxes.begin(), bboxes.end(), compare_score);

	/* throw away boxes with low score if there are too many */
	if(bboxes.size()>prune) 
		bboxes.resize(prune);

	/* get bounding box & area */
	vector<bbox_t>::iterator iter;
	for(iter=bboxes.begin();iter!=bboxes.end();iter++)
		bbox_calcOut(&(*iter));

	/* use list for frequent removal */
	list<bbox_t> lsbboxes(bboxes.begin(),bboxes.end());
	list<bbox_t>::iterator iter_l1, iter_l2, iter_tmp;
	double intersect;
	iter_l1 = lsbboxes.begin();
	while(iter_l1!=lsbboxes.end()){
		iter_l2 = iter_l1;
		iter_l2++;
		while(iter_l2!=lsbboxes.end()){
			intersect = fbox_interArea(iter_l1->outer, iter_l2->outer);
			if((intersect/iter_l1->area)>overlap || (intersect/iter_l2->area)>overlap) {
				iter_tmp = iter_l2;
				iter_tmp++;
				lsbboxes.erase(iter_l2);
				iter_l2 = iter_tmp;
				continue;
			}
			iter_l2++;
		}
		iter_l1++;
	}
	/* copy results back to vector*/
	bboxes.resize(lsbboxes.size());
	copy(lsbboxes.begin(),lsbboxes.end(),bboxes.begin());
}

