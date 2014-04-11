/*
 * eHfacemodel.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#include "eHfacemodel.h"
#include "eHposemodel.h"
#include "eHfeatpyramid.h"
#include "eHmatrix.h"
#include "eHshiftdt.h"
#include "eHutils.h"

#include "rapidxml.hpp"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>

#define EH_MAX_LEN 800
#define EH_IMG_DEFAULT_SZ 320
#define EH_BODY_CAND_USED 3

using std::ifstream;
using std::ios;

static inline int min(int x, int y) { return (x <= y ? x : y); }
static inline int max(int x, int y) { return (x <= y ? y : x); }

#ifdef EH_TEST_TIMER
timeval time_spent_pyra;
timeval time_spent_conv;
timeval time_spent_dp;
timeval time_spent_detect;
//time_spent_conv.tv_sec=time_spent_conv.tv_usec=0;
//time_spent_pyra.tv_sec=time_spent_pyra.tv_usec=0;
//time_spent_dp.tv_sec=time_spent_dp.tv_usec=0;
//time_spent_detect.tv_sec=time_spent_detect.tv_usec=0;
#endif

/* NOTE: change field_width to actual value (and remove assertion) 
 * can get higher speed
 *
 * index changed from matlab 0-1 style to C style:
 *  - def.anchor[0] 
 *  - def.anchor[1]
 *  - part.defid 
 *  - part.filterid
 *  - part.parent 
 * index NOT changed from matlab style:
 *  - filter.i
 *  - def.i
 */
facemodel_t* facemodel_parseXml(char* xmlstr) {
	using namespace rapidxml;
	facemodel_t* model = new facemodel_t;
	xml_document<> doc;
	doc.parse<0>(xmlstr);
	xml_node<>* root = doc.first_node("facemodel");

	/* first level nodes */
	xml_node<>* filters = root->first_node("filters");
	xml_node<>* defs = root->first_node("defs");
	xml_node<>* components = root->first_node("components");
	xml_node<>* maxsize = root->first_node("maxsize");
	xml_node<>* len = root->first_node("len");
	xml_node<>* interval = root->first_node("interval");
	xml_node<>* sbin = root->first_node("sbin");
	xml_node<>* delta = root->first_node("delta");
	xml_node<>* thresh = root->first_node("thresh");
	xml_node<>* obj = root->first_node("obj");

	int field_width=-1;
    int* tmp_int_ptr;
    double* tmp_double_ptr;

	/* model->filters : vector<filter_t> */
	int num_filters = strtol(filters->first_attribute("num")->value(),NULL,10);
	xml_node<>* filter = filters->first_node("filter");
	xml_node<>* filter_i = filter->first_node("i");
	xml_node<>* filter_w = filter->first_node("w");
	filter_t tmp_facefilter;
		/*1st filter*/
	tmp_facefilter.i = strtol(filter_i->value(),NULL,10);
	field_width = -1;
    tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
	assert(field_width==3);
    tmp_facefilter.w.sizy = tmp_int_ptr[0];
    tmp_facefilter.w.sizx = tmp_int_ptr[1];
    tmp_facefilter.w.sizz = tmp_int_ptr[2];
	field_width = -1;
	tmp_facefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
    assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
    delete[] tmp_int_ptr;
	model->filters.push_back(tmp_facefilter);
		/*other filters*/
	for(int i = 1; i<num_filters; i++){
		filter = filter->next_sibling();
		filter_i = filter->first_node("i");
		filter_w = filter->first_node("w");
		tmp_facefilter.i = strtol(filter_i->value(),NULL,10);
		field_width = -1;
        tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
		assert(field_width==3);
        tmp_facefilter.w.sizy = tmp_int_ptr[0];
        tmp_facefilter.w.sizx = tmp_int_ptr[1];
        tmp_facefilter.w.sizz = tmp_int_ptr[2];
		field_width = -1; 
		tmp_facefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
        assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
        delete[] tmp_int_ptr;
		model->filters.push_back(tmp_facefilter);
	}

	/* model->defs : vector<facedef_t> */
	int num_defs = strtol(defs->first_attribute("num")->value(),NULL,10);
	xml_node<>* def = defs->first_node("def");
	xml_node<>* def_i = def->first_node("i");
	xml_node<>* def_w = def->first_node("w");
	xml_node<>* def_anchor = def->first_node("anchor");
	facedef_t tmp_facedef;
		/*1st def*/
	tmp_facedef.i = strtol(def_i->value(),NULL,10);
	field_width = -1;
    tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
	assert(field_width==1);
	field_width = -1;
    tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
    assert(field_width==tmp_int_ptr[0]);
    for(int ii=0;ii<field_width;ii++) tmp_facedef.w[ii]=tmp_double_ptr[ii];
    delete[] tmp_int_ptr;
    delete[] tmp_double_ptr;
	field_width = -1;
    tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
	assert(field_width==3);
    tmp_facedef.anchor[0]=tmp_int_ptr[0]-1;
    tmp_facedef.anchor[1]=tmp_int_ptr[1]-1;
    tmp_facedef.anchor[2]=tmp_int_ptr[2];
    delete[] tmp_int_ptr;
	model->defs.push_back(tmp_facedef);
		/*other defs*/
	for(int i=1;i<num_defs;i++){
		def = def->next_sibling();
		def_i = def->first_node("i");
		def_w = def->first_node("w");
		def_anchor = def->first_node("anchor");
		tmp_facedef.i = strtol(def_i->value(),NULL,10);
		field_width = -1;
        tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
		assert(field_width==1);
		field_width = -1;
        tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
        assert(field_width==tmp_int_ptr[0]);
        for(int ii=0;ii<field_width;ii++) tmp_facedef.w[ii]=tmp_double_ptr[ii];
        delete[] tmp_int_ptr;
        delete[] tmp_double_ptr;
		field_width = -1;
        tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
		assert(field_width==3);
        tmp_facedef.anchor[0]=tmp_int_ptr[0]-1;
        tmp_facedef.anchor[1]=tmp_int_ptr[1]-1;
        tmp_facedef.anchor[2]=tmp_int_ptr[2];
        delete[] tmp_int_ptr;
		model->defs.push_back(tmp_facedef);
	}

	/* model->components : vector<vector<facepart_t> >*/
	int num_components = strtol(components->first_attribute("num")->value(),NULL,10);
	int num_parts;
	xml_node<>* component = components->first_node("component");
	xml_node<>* part;
	facepart_t tmp_facepart;
	vector<facepart_t> tmp_vector_facepart;
		/*1st component*/
	num_parts = strtol(component->first_attribute("num")->value(),NULL,10);
	part = component->first_node("part");
	tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
	tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
	tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
	tmp_vector_facepart.clear();
	tmp_vector_facepart.push_back(tmp_facepart);
	for(int j=1;j<num_parts;j++){
		part = part->next_sibling();
		tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
		tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
		tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
		tmp_vector_facepart.push_back(tmp_facepart);
	}
	model->components.push_back(tmp_vector_facepart);
		/*other components*/
	for(int i =1;i<num_components; i++) {
		component = component->next_sibling();
		num_parts = strtol(component->first_attribute("num")->value(),NULL,10);
		part = component->first_node("part");
		tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
		tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
		tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
		tmp_vector_facepart.clear();
		tmp_vector_facepart.push_back(tmp_facepart);
		for(int j=1;j<num_parts;j++){
			part = part->next_sibling();
			tmp_facepart.defid = strtol(part->first_attribute("defid")->value(),NULL,10)-1;
			tmp_facepart.filterid = strtol(part->first_attribute("filterid")->value(),NULL,10)-1;
			tmp_facepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
			tmp_vector_facepart.push_back(tmp_facepart);
		}
		model->components.push_back(tmp_vector_facepart);

	}

	/* model->maxsize : int[2] */
	field_width = -1;
    tmp_int_ptr=parseCSVstr2int(maxsize->first_attribute("size")->value(),&field_width);
	assert(field_width==2);
    model->maxsize[0]=tmp_int_ptr[0];
    model->maxsize[1]=tmp_int_ptr[1];
    delete[] tmp_int_ptr;

	/* model->len : int */
	model->len = strtol(len->first_attribute("val")->value(),NULL,10);

	/* model->interval : int */
	model->interval = strtol(interval->first_attribute("val")->value(),NULL,10);

	/* model->sbin : int */
	model->sbin = strtol(sbin->first_attribute("val")->value(),NULL,10);

	/* model->delta : double */
	model->delta = strtod(delta->first_attribute("val")->value(),NULL);

	/* model->thresh : double */
	model->thresh = strtod(thresh->first_attribute("val")->value(),NULL);

	/* model->obj : double */
	model->obj = strtod(obj->first_attribute("val")->value(),NULL);

	/* additional fields (not from file) */
    facepart_t* part_ptr;
	int par, ax, ay, ds, step, virtpady, virtpadx;
	for (unsigned i=0;i<model->components.size();i++){
		for (unsigned j=0; j<model->components[i].size();j++) {
            part_ptr = &(model->components[i][j]);
            part_ptr->sizy = model->filters[part_ptr->filterid].w.sizy;
            part_ptr->sizx = model->filters[part_ptr->filterid].w.sizx;
			/* store the offset and scale of each part relative to the root */
            ax = model->defs[part_ptr->defid].anchor[0];
            ay = model->defs[part_ptr->defid].anchor[1];
            ds = model->defs[part_ptr->defid].anchor[2];
            if ((par = part_ptr->parent)>=0) {
                part_ptr->scale = ds + model->components[i][par].scale;
			} else {
				assert(j==0);
                part_ptr->scale = 0;
			}
			assert(par<(int)j);
			/* amount of (virtual) padding to hallucinate */
			if (ds==0) step = 1;else
				step = int(pow(2.0,(double)ds));
			virtpady = (step-1)*max(model->maxsize[0]-1-1,0);
			virtpadx = (step-1)*max(model->maxsize[1]-1-1,0);
			/* starting points (simulates additional padding at finer scales */
            part_ptr->starty = ay-virtpady;
            part_ptr->startx = ax-virtpadx;
            part_ptr->step = step;
		}
	}

	return model;
}

facemodel_t* facemodel_readFromFile(const char* filepath) {
	ifstream fin(filepath);
	if (fin.fail()) return NULL;
	fin.seekg(0,ios::end);
	size_t length = fin.tellg();
	char* xmlstr = new char[length+1];
	fin.seekg(0,ios::beg);
	fin.read(xmlstr,length);
	xmlstr[length] = '\0';
	fin.close();
    facemodel_t* model = facemodel_parseXml(xmlstr);

    if (model->components.size() == 13) {
        for (int i=90; i>=-90; i-=15) {
            model->posemap.push_back(i);
        }
    }
    else if (model->components.size() == 18) {
        for (int i=90; i>=15; i-=15) {
            model->posemap.push_back(i);
        }
        for (int i=0; i<6; ++i) {
            model->posemap.push_back(0);
        }
        for (int i=-15; i>=-90; i-=15) {
            model->posemap.push_back(i);
        }
    }

	delete[] xmlstr;
	return model;
}

/* local data storage for message passing */
struct facepart_data {
	double* score;
	int* Ix;
	int* Iy;
	int sizScore[2];
	int sizI[2];
	int level;
};

vector<bbox_t> facemodel_detect(const facemodel_t* model, const image_t* img) {
	return facemodel_detect(model, img, model->thresh);
}
vector<bbox_t> facemodel_detect(const facemodel_t* model, const image_t* img, double thrs) {
    vector<bbox_t> boxes;
#ifdef EH_TEST_TIMER
	timeval start_detect, end_detect, interval_detect;
	gettimeofday(&start_detect, NULL);
#endif
	/* build feature pyramid */
	int imsize[] = {img->sizy, img->sizx};
#ifdef EH_TEST_TIMER
	timeval start_pyra, end_pyra, interval_pyra;
	gettimeofday(&start_pyra,NULL);
#endif
    featpyra_t* pyra = featpyra_create(img, model->interval, model->sbin, model->maxsize,true);
#ifdef EH_TEST_TIMER
	gettimeofday(&end_pyra,NULL);
	timersub(&end_pyra,&start_pyra,&interval_pyra);
	timeradd(&interval_pyra,&time_spent_pyra, &time_spent_pyra);
	printf("Pyramid in seconds: %ld.%ld seconds\n", (interval_pyra.tv_sec), (interval_pyra.tv_usec));
#endif

    mat3d_t** resp = new mat3d_t*[pyra->len];
    memset(resp, 0, pyra->len*sizeof(mat3d_t*));
 	/* index changed from matlab 0-1 style to C style:
	 * 	level
	 */
	int rlevel, k; /* root level, part# */
	for(unsigned c=0;c<model->components.size();c++){
		const vector<facepart_t>* parts = &(model->components[c]);
		int numparts = parts->size();
		vector<facepart_data> parts_data;
		parts_data.resize(numparts);
		for(rlevel=model->interval;rlevel<pyra->len;rlevel++){
			const facepart_t* rootpart = &(parts->at(0));
			facepart_data* rootpart_data = &(parts_data.at(0));
			/* local part scores */
			for(k=0;k<numparts;k++){
				int fid = parts->at(k).filterid;
				int level = rlevel-(parts->at(k)).scale*model->interval;
				if(resp[level]==NULL){
#ifdef EH_TEST_TIMER
					timeval start_conv, end_conv, interval_conv;
					gettimeofday(&start_conv,NULL);
#endif
                    resp[level]=filterv_apply(model->filters, pyra->feat[level], 0, model->filters.size()-1);
#ifdef EH_TEST_TIMER
					gettimeofday(&end_conv,NULL);
					timersub(&end_conv,&start_conv,&interval_conv);
					timeradd(&interval_conv,&time_spent_conv,&time_spent_conv);
#endif
				}
				parts_data.at(k).level = level;
                int len = (resp[level]->sizy)*(resp[level]->sizx);
                parts_data.at(k).score = new double[len];
				memcpy(parts_data.at(k).score,resp[level]->vals+fid*len,len*sizeof(double));
				parts_data.at(k).sizScore[0] = resp[level]->sizy;
				parts_data.at(k).sizScore[1] = resp[level]->sizx;
			}
#ifdef EH_TEST_TIMER
			timeval start_dp, end_dp, interval_dp;
			gettimeofday(&start_dp,NULL);
#endif
			/* part relations - tree message passing */
			for(k=numparts-1;k>0;k--){
				const facepart_t* child = &(parts->at(k));
				facepart_data* child_data = &(parts_data.at(k));
				int par = child->parent;
				int Ny = parts_data.at(par).sizScore[0];
				int Nx = parts_data.at(par).sizScore[1];
				/* assume all filters are of the same size */
				assert(Ny == child_data->sizScore[0] && Nx == child_data->sizScore[1]);
				double* msg = new double[Nx*Ny];
				child_data->Iy = new int[Ny*Nx];
				child_data->Ix = new int[Ny*Nx];
				eHshiftdt(msg,child_data->Ix,child_data->Iy,Nx,Ny,
					child->startx,child->starty,
					child->step,child_data->score,
					child_data->sizScore[1],child_data->sizScore[0],
					model->defs[child->defid].w
					);
				for(int i=0;i<Ny*Nx;i++)
					parts_data.at(par).score[i]+=msg[i];
				delete[] msg;
			}
			
			/* add bias to root score (const term) */
			vector<int> slct; slct.reserve(10000);
			for(int i=0;i<rootpart_data->sizScore[0]*rootpart_data->sizScore[1];i++) {
                rootpart_data->score[i] += model->defs[rootpart->defid].w[0];
				if(rootpart_data->score[i]>=thrs)
					slct.push_back(i);
			}
			
			/* backtrack */
			if(!slct.empty()) {
				/* root */
				int k0=boxes.size();
				int newboxes_len = slct.size();
				boxes.resize(k0+newboxes_len);
				int* ptr = new int[numparts*newboxes_len];/*XXX*/
				int padx = max(model->maxsize[1]-2,0);
				int pady = max(model->maxsize[0]-2,0);
				double scale;
				for(int i=0;i<newboxes_len;i++){
					ptr[i]=slct[i];
					scale = pyra->scale[rootpart_data->level];
					int y = slct[i]%rootpart_data->sizScore[0];
					int x = (slct[i]-y)/rootpart_data->sizScore[0];
					fbox_t tmpbox = {
						(x-padx)*scale,
						(y-pady)*scale,
						(x-padx+rootpart->sizx)*scale-1,
						(y-pady+rootpart->sizy)*scale-1
					};
					boxes[k0+i].boxes.push_back(tmpbox);
					boxes[k0+i].component = c;
					boxes[k0+i].score = rootpart_data->score[slct[i]];
				}
				/*remaining parts*/
				for(k=1;k<numparts;k++){
					const facepart_t* tmppart = &(parts->at(k));
					facepart_data* tmppart_data = &(parts_data.at(k));
					int par = tmppart->parent;
					scale = pyra->scale[tmppart_data->level];
					int x,y;
					for(int i=0;i<newboxes_len;i++){
						x = tmppart_data->Ix[ptr[par*newboxes_len+i]];
						y = tmppart_data->Iy[ptr[par*newboxes_len+i]];
						ptr[k*newboxes_len+i] = x*tmppart_data->sizScore[0]+y;
						fbox_t tmpbox = {
							(x-padx)*scale,
							(y-pady)*scale,
							(x-padx+tmppart->sizx)*scale-1,
							(y-pady+tmppart->sizy)*scale-1
						};
						boxes[k0+i].boxes.push_back(tmpbox);
					}
				}

				delete[] ptr;
			}
#ifdef EH_TEST_TIMER
			gettimeofday(&end_dp,NULL);
			timersub(&end_dp,&start_dp,&interval_dp);
			timeradd(&interval_dp, &time_spent_dp, &time_spent_dp);
#endif

			/* clean Ix Iy score */
			for(k=numparts-1;k>0;k--){
				facepart_data* child = &(parts_data.at(k));
				delete[] child->Ix;
				delete[] child->Iy;
				delete[] child->score;
			}
			delete[] parts_data.at(0).score;

		}
	}

	for(int i=0;i<pyra->len;i++)
		if(resp[i]!=NULL) mat3d_delete(resp[i]);
	delete[] resp;
	featpyra_delete(pyra);

	for (unsigned i=0; i<boxes.size(); i++)
		bbox_clipboxes(boxes[i],imsize);

    if (!model->posemap.empty()) {
        for (vector<bbox_t>::iterator iter = boxes.begin(); iter != boxes.end(); ++iter) {
            int component = iter->component;
            if (component >=0 && component < (int)model->posemap.size()) {
                iter->pose = model->posemap[component];
            }
        }
    }

	bbox_v_nms(boxes, 0.3);

#ifdef EH_TEST_TIMER
	gettimeofday(&end_detect,NULL);
	timersub(&end_detect,&start_detect,&interval_detect);
	timeradd(&interval_detect, &time_spent_detect, &time_spent_detect);
	printf("Detection in seconds: %ld.%ld seconds\n", (interval_detect.tv_sec), (interval_detect.tv_usec));
#endif

	return boxes;
}

vector<bbox_t> facemodel_detect(const facemodel_t* facemodel, const posemodel_t* posemodel, const image_t* img) {
	double scale1 = EH_IMG_DEFAULT_SZ / (double) min(img->sizx,img->sizy);
    image_t* img1;
	//if(scale1<1)
	//	img1 = image_subsample(img, scale1);
	//else
		img1 = image_resize(img, scale1);
	/* 1st step, detect face in scaled image */
    vector<bbox_t> faces = facemodel_detect(facemodel, img1, facemodel->thresh);
	/* only ask for help of body detection if no faces are detected */
	if(faces.empty()) {
		/* 2nd step, detect body */
        vector<bbox_t> poses = posemodel_detect(posemodel, img1, posemodel->thresh);
		image_delete(img1);
		/* if no body detected, give up */
		if(poses.empty())
			return faces;
		if(poses.size()>EH_BODY_CAND_USED)
			poses.resize(EH_BODY_CAND_USED);
		int idxs_head[] = {0, 1};
		int idxslen_head = 2;
		double padding[] = {0.25, 0.25, 0.25, 0};
		for(unsigned i=0;i<poses.size();i++) {
            bbox_t pose = poses[i];
			fbox_t head = fbox_merge(pose.boxes, idxs_head, 
					idxslen_head, padding);
			fbox_resize(&head, 1/scale1);
			fbox_clip(head,img->imsize);
			int offset[2];
            image_t* patch = image_crop(img, head, (int*)&offset);
			double scale2 = EH_IMG_DEFAULT_SZ / (double) min(head.x2-head.x1, head.y2-head.y1);
            image_t* patch2 = image_resize(patch, scale2);
            image_delete(patch);
            vector<bbox_t> facesfp = facemodel_detect(facemodel, patch2, facemodel->thresh);
			if(!facesfp.empty()) {
				bbox_v_resize(facesfp, 1/scale2);
				bbox_v_move(facesfp, offset);
			}
			faces.resize(faces.size()+facesfp.size());
			std::copy(facesfp.begin(),facesfp.end(),faces.end()-facesfp.size());
			image_delete(patch2);
		}
		bbox_v_nms(faces,0.5);
	} else {
		bbox_v_resize(faces,1/scale1);
		image_delete(img1);
	}
	for(unsigned i=0;i<faces.size();i++)
		bbox_calcOut(&faces[i]);
	return faces;
}

void facemodel_delete(facemodel_t* model) {
	/* because filters[i].w.vals was allocated inside parseCSV2double, 
	 * it should be released manually */
	for(unsigned i=0;i<model->filters.size();i++)
		delete[] model->filters[i].w.vals;
	delete model;
}
