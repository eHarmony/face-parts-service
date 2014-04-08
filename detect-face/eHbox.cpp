#include "eHbox.h"
#include "eHutils.h"

#include "stdlib.h"

#include <math.h>

#define EH_INF 1E20

static inline double min(double x, double y) {return (x <= y ? x : y); }

static inline double max(double x, double y) {return (x <= y ? y : x); }

static inline int round2int(double x) { return (int)(x+0.5); }

void fbox_set(fbox_t* box, double x1, double y1, double x2, double y2) {
	box->x1 = x1;
	box->y1 = y1;
	box->x2 = x2;
	box->y2 = y2;
}

ibox_t fbox_getibox(fbox_t* box) {
	ibox_t intbox;
	intbox.x1 = round2int(box->x1);
	intbox.x2 = round2int(box->x2);
	intbox.y1 = round2int(box->y1);
	intbox.y2 = round2int(box->y2);
	return intbox;
}

void ibox_set(ibox_t* box, int x1, int y1, int x2, int y2) {
	box->x1 = x1;
	box->y1 = y1;
	box->x2 = x2;
	box->y2 = y2;
}

double fbox_interArea(const fbox_t& box1, const fbox_t& box2) {
	double w = max(0,min(box1.x2,box2.x2)-max(box1.x1,box2.x1));
	double h = max(0,min(box1.y2,box2.y2)-max(box1.y1,box2.y1));
	return w*h;
}

void fbox_clip(fbox_t& box, const int* imsize) {
	box.x1 = max(0, box.x1);
	box.y1 = max(0, box.y1);
	box.x2 = min(imsize[1]-1, box.x2);
	box.y2 = min(imsize[0]-1, box.y2);
}

fbox_t fbox_merge(const vector<fbox_t>& boxes, const int* idxs, int num, const double* padding) {
	double tmp_x1=EH_INF, tmp_x2=-EH_INF, tmp_y1=EH_INF, tmp_y2=-EH_INF;
	for (int i=0;i<num;i++) {
		tmp_x1 = min(tmp_x1,boxes[idxs[i]].x1);
		tmp_x2 = max(tmp_x2,boxes[idxs[i]].x2);
		tmp_y1 = min(tmp_y1,boxes[idxs[i]].y1);
		tmp_y2 = max(tmp_y2,boxes[idxs[i]].y2);
	}
	fbox_t merged;
	if(padding!=NULL) {
		double w = tmp_x2-tmp_x1;
		double h = tmp_y2-tmp_y1;
		fbox_set(&merged, tmp_x1-w*padding[0], tmp_y1-h*padding[2], 
				tmp_x2+w*padding[1], tmp_y2+h*padding[3]);
	} else {
		fbox_set(&merged, tmp_x1, tmp_y1, tmp_x2, tmp_y2);
	}
	return merged;
}

fbox_t fbox_getResized(const fbox_t& box, double scale) {
	fbox_t resized;
	fbox_set(&resized, box.x1*scale, box.y1*scale, box.x2*scale, box.y2*scale);
	return resized;
}

fbox_t fbox_getMoved(const fbox_t& box, const int* offset) {
	fbox_t moved;
	fbox_set(&moved, box.x1+offset[1], box.y1+offset[0], box.x2+offset[1], box.y2+offset[0]);
	return moved;
}

void fbox_resize(fbox_t* box, double scale) {
	fbox_set(box, box->x1*scale, box->y1*scale, box->x2*scale, box->y2*scale);
}

void fbox_move(fbox_t* box, const int* offset) {
	fbox_set(box, box->x1+offset[1], box->y1+offset[0], box->x2+offset[1], box->y2+offset[0]);
}
