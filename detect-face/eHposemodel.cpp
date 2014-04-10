/*
 * eHposemodel.cpp
 *
 * Hang Su
 * 2012-08 @ eH
 */
#define EH_INF 1E20

#include "eHposemodel.h"
#include "eHfeatpyramid.h"
#include "eHmatrix.h"
#include "eHshiftdt.h"
#include "eHutils.h"

#include "rapidxml.hpp"

#include "iostream"
#include "fstream"

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>

using std::ifstream;
using std::ios;

static inline int max(int x, int y) { return (x <= y ? y : x); }

posemodel_t* posemodel_parseXml(char* xmlstr) {
	using namespace rapidxml;
	posemodel_t* model = new posemodel_t;
	xml_document<> doc;
	doc.parse<0>(xmlstr);
	xml_node<>* root = doc.first_node("posemodel");

	/* first level nodes */
	xml_node<>* biases = root->first_node("biases");
	xml_node<>* filters = root->first_node("filters");
	xml_node<>* defs = root->first_node("defs");
	xml_node<>* parts = root->first_node("parts");
	xml_node<>* maxsize = root->first_node("maxsize");
	xml_node<>* len = root->first_node("len");
	xml_node<>* interval = root->first_node("interval");
	xml_node<>* sbin = root->first_node("sbin");
	xml_node<>* thresh = root->first_node("thresh");
	xml_node<>* obj = root->first_node("obj");

	int field_width = -1;
	int* tmp_int_ptr;
	double* tmp_double_ptr;

	/* model->biases : vector<posebias_t> */
	int num_biases = strtol(biases->first_attribute("num")->value(),NULL,10);
	xml_node<>* bias = biases->first_node("bias");
	xml_node<>* bias_i = bias->first_node("i");
	xml_node<>* bias_w = bias->first_node("w");
	posebias_t tmp_posebias;
		/*1st bias*/
	tmp_posebias.i = strtol(bias_i->value(),NULL,10);
	tmp_posebias.w = strtod(bias_w->value(),NULL);
	model->biases.push_back(tmp_posebias);
		/*other biases*/
	for(int i = 1; i<num_biases; i++){
		bias = bias->next_sibling();
		bias_i = bias->first_node("i");
		bias_w = bias->first_node("w");
		tmp_posebias.i = strtol(bias_i->value(),NULL,10);
		tmp_posebias.w = strtod(bias_w->value(),NULL);
		model->biases.push_back(tmp_posebias);
	}

	/* model->filters : vector<filter_t> */
	int num_filters = strtol(filters->first_attribute("num")->value(),NULL,10);
	xml_node<>* filter = filters->first_node("filter");
	xml_node<>* filter_i = filter->first_node("i");
	xml_node<>* filter_w = filter->first_node("w");
	filter_t tmp_posefilter;
		/*1st filter*/
	tmp_posefilter.i = strtol(filter_i->value(),NULL,10);
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
	assert(field_width==3);
	tmp_posefilter.w.sizy = tmp_int_ptr[0];
	tmp_posefilter.w.sizx = tmp_int_ptr[1];
	tmp_posefilter.w.sizz = tmp_int_ptr[2];
	field_width = -1;
	tmp_posefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
	assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
	delete[] tmp_int_ptr;
	model->filters.push_back(tmp_posefilter);
		/*other filters*/
	for(int i = 1; i<num_filters; i++){
		filter = filter->next_sibling();
		filter_i = filter->first_node("i");
		filter_w = filter->first_node("w");
		tmp_posefilter.i = strtol(filter_i->value(),NULL,10);
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(filter_w->first_attribute("size")->value(),&field_width);
		assert(field_width==3);
		tmp_posefilter.w.sizy = tmp_int_ptr[0];
		tmp_posefilter.w.sizx = tmp_int_ptr[1];
		tmp_posefilter.w.sizz = tmp_int_ptr[2];
		field_width = -1; 
		tmp_posefilter.w.vals = parseCSVstr2double(filter_w->value(),&field_width);
		assert(field_width==tmp_int_ptr[0]*tmp_int_ptr[1]*tmp_int_ptr[2]);
		delete[] tmp_int_ptr;
		model->filters.push_back(tmp_posefilter);
	}

	/* model->defs : vector<posedef_t> */
	int num_defs = strtol(defs->first_attribute("num")->value(),NULL,10);
	xml_node<>* def = defs->first_node("def");
	xml_node<>* def_i = def->first_node("i");
	xml_node<>* def_w = def->first_node("w");
	xml_node<>* def_anchor = def->first_node("anchor");
	posedef_t tmp_posedef;
		/*1st def*/
	tmp_posedef.i = strtol(def_i->value(),NULL,10);
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
	assert(field_width==1);assert(tmp_int_ptr[0]==4);
	field_width = -1;
	tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
	assert(field_width==tmp_int_ptr[0]);
	for(int ii=0;ii<field_width;ii++) tmp_posedef.w[ii]=tmp_double_ptr[ii];
	delete[] tmp_int_ptr;
	delete[] tmp_double_ptr;
	field_width = -1;
	tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
	assert(field_width==3);
	tmp_posedef.anchor[0]=tmp_int_ptr[0]-1;
	tmp_posedef.anchor[1]=tmp_int_ptr[1]-1;
	tmp_posedef.anchor[2]=tmp_int_ptr[2];
	delete[] tmp_int_ptr;
	model->defs.push_back(tmp_posedef);
		/*other defs*/
	for(int i=1;i<num_defs;i++){
		def = def->next_sibling();
		def_i = def->first_node("i");
		def_w = def->first_node("w");
		def_anchor = def->first_node("anchor");
		tmp_posedef.i = strtol(def_i->value(),NULL,10);
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(def_w->first_attribute("size")->value(),&field_width);
		assert(field_width==1);
		field_width = -1;
		tmp_double_ptr = parseCSVstr2double(def_w->value(),&field_width);
		assert(field_width==tmp_int_ptr[0]);
		for(int ii=0;ii<field_width;ii++) tmp_posedef.w[ii]=tmp_double_ptr[ii];
		delete[] tmp_int_ptr;
		delete[] tmp_double_ptr;
		field_width = -1;
		tmp_int_ptr = parseCSVstr2int(def_anchor->value(),&field_width);
		assert(field_width==3);
		tmp_posedef.anchor[0]=tmp_int_ptr[0]-1;
		tmp_posedef.anchor[1]=tmp_int_ptr[1]-1;
		tmp_posedef.anchor[2]=tmp_int_ptr[2];
		delete[] tmp_int_ptr;
		model->defs.push_back(tmp_posedef);
	}

	/* model->parts : vector<posepart_t> */
	int num_parts = strtol(parts->first_attribute("num")->value(),NULL,10);
	xml_node<>* part;
	posepart_t tmp_posepart;
		/*1st part*/
	part = parts->first_node("part");
	tmp_posepart.num = strtol(part->first_attribute("num")->value(),NULL,10);
	tmp_posepart.numpar = strtol(part->first_attribute("numpar")->value(),NULL,10);
	tmp_posepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
	tmp_posepart.biasid = new int;
	tmp_posepart.biasid[0] = strtol(part->first_attribute("biasid")->value(),NULL,10)-1;
	tmp_posepart.defid = NULL; /*root has no defid*/
	field_width = -1;
	tmp_posepart.filterid = parseCSVstr2int(part->first_attribute("filterid")->value(),&field_width,-1);
	assert(tmp_posepart.num == field_width);
	model->parts.push_back(tmp_posepart);
		/*other parts*/
	for(int i =1;i<num_parts; i++) {
		part = part->next_sibling();
		tmp_posepart.num = strtol(part->first_attribute("num")->value(),NULL,10);
		tmp_posepart.numpar = strtol(part->first_attribute("numpar")->value(),NULL,10);
		tmp_posepart.parent = strtol(part->first_attribute("parent")->value(),NULL,10)-1;
		field_width = -1;
		tmp_posepart.biasid = parseCSVstr2int(part->first_attribute("biasid")->value(),&field_width,-1);
		assert(tmp_posepart.num*tmp_posepart.numpar == field_width);
		field_width = -1;
		tmp_posepart.defid = parseCSVstr2int(part->first_attribute("defid")->value(),&field_width,-1);
		assert(tmp_posepart.num == field_width);
		field_width = -1;
		tmp_posepart.filterid = parseCSVstr2int(part->first_attribute("filterid")->value(),&field_width,-1);
		assert(tmp_posepart.num == field_width);
		model->parts.push_back(tmp_posepart);
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

	/* model->thresh : double */
	model->thresh = strtod(thresh->first_attribute("val")->value(),NULL);

	/* model->obj : double */
	model->obj = strtod(obj->first_attribute("val")->value(),NULL);

	/* additional fields (not from file) :
	 * sizy, sizx, starty, startx, scale, step
	 * an assumption seems hold: all ds are the same, so only 1 scale and step 
	 * stored for each part*/
	int num, par, ds, step, virtpady, virtpadx;
	posepart_t* part_ptr;
	part_ptr = &(model->parts[0]);
	part_ptr->startx = part_ptr->starty = NULL;
	part_ptr->scale = 0; part_ptr->step = 1;
	num = part_ptr->num;
	part_ptr->sizy = new int[num];
	part_ptr->sizx = new int[num];
	for (int j=0;j<num;j++) {
		part_ptr->sizy[j] = model->filters[part_ptr->filterid[j]].w.sizy;
		part_ptr->sizx[j] = model->filters[part_ptr->filterid[j]].w.sizx;
	}
	for (unsigned i=1;i<model->parts.size();i++){
		part_ptr = &(model->parts[i]);
		par = part_ptr->parent;
		assert(par<(int)i);
		num = part_ptr->num;
		part_ptr->sizy = new int[num];
		part_ptr->sizx = new int[num];
		part_ptr->starty = new int[num];
		part_ptr->startx = new int[num];
		ds = model->defs[part_ptr->defid[0]].anchor[2];
		step = (ds==0 ? 1 : int(pow(2.0,(double)ds)));
		virtpady = (step-1)*max(model->maxsize[0]-1-1,0);
		virtpadx = (step-1)*max(model->maxsize[1]-1-1,0);
		part_ptr->step = step;
		part_ptr->scale = ds + model->parts[par].scale;
		for(int j=0;j<part_ptr->num;j++) {
			part_ptr->sizy[j] = model->filters[part_ptr->filterid[j]].w.sizy;
			part_ptr->sizx[j] = model->filters[part_ptr->filterid[j]].w.sizx;
			part_ptr->starty[j] = model->defs[part_ptr->defid[j]].anchor[1]-virtpady;
			part_ptr->startx[j] = model->defs[part_ptr->defid[j]].anchor[0]-virtpadx;
		}
	}
	return model;
}

posemodel_t* posemodel_readFromFile(const char* filepath) {
	ifstream fin(filepath);
	if (fin.fail()) return NULL;
	fin.seekg(0,ios::end);
	size_t length = fin.tellg();
	char* xmlstr = new char[length+1];
	fin.seekg(0,ios::beg);
	fin.read(xmlstr,length);
	xmlstr[length] = '\0';
	fin.close();
	posemodel_t* model = posemodel_parseXml(xmlstr);
	delete[] xmlstr;
	return model;
}

/* local data storage for message passing */
struct posepart_data {
	double *score; 
	int* Iy;
	int* Ix;
	int* Ik;
	int sizScore[3];
	int sizI[3]; /*same as parent's sizScore*/
	int level;
};

vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img, double thrs){
	vector<bbox_t> boxes;

	int imsize[] = {img->sizy, img->sizx};
	featpyra_t* pyra = featpyra_create(img, model->interval, model->sbin, model->maxsize,false);
	mat3d_ptr* resp = new mat3d_ptr[pyra->len];
	memset(resp, 0, pyra->len*sizeof(mat3d_ptr));
	
	int numparts = model->parts.size();
	vector<posepart_data> parts_data;
	parts_data.resize(numparts);
	for(int rlevel=0; rlevel<pyra->len; rlevel++) {
		const posepart_t* rootpart = &(model->parts[0]);
		posepart_data* rootpart_data = &(parts_data[0]);

		/* Compute local scores */
		for(int k=0;k<numparts;k++) {
			int level = rlevel-model->parts[k].scale*model->interval;
			int numtypes = model->parts[k].num;
			if(resp[level]==NULL) {
                resp[level] = filterv_apply_ST(model->filters, pyra->feat[level], 0, model->filters.size()-1);
			}
			/*assume all filters are of the same size*/
			int len = (resp[level]->sizy)*(resp[level]->sizx);
			parts_data.at(k).score = new double[len*numtypes];
			for(int i=0;i<numtypes;i++) {
				memcpy(parts_data[k].score + i*len, 
						resp[level]->vals+model->parts[k].filterid[i]*len, 
						len*sizeof(double));
			}
			parts_data[k].sizScore[0]=resp[level]->sizy;
			parts_data[k].sizScore[1]=resp[level]->sizx;
			parts_data[k].sizScore[2]=numtypes;
			parts_data[k].level = level;
		}

		/*again, assume all filters are of the same size*/
		int Ny = parts_data[0].sizScore[0];
		int Nx = parts_data[0].sizScore[1];
		/* part relations - tree message passing */
		double *score0, *msg;
		int *Ix0, *Iy0;
		for(int p=numparts-1;p>0;p--) {
			const posepart_t* child = &(model->parts[p]);
			posepart_data* child_data = &(parts_data[p]);
			int par = child->parent;
			int numtypes = child->num;
			int numtypes_par = child->numpar;
			msg = new double[Ny*Nx*numtypes_par];
			child_data->Iy = new int[Ny*Nx*numtypes_par];
			child_data->Ix = new int[Ny*Nx*numtypes_par];
			child_data->Ik = new int[Ny*Nx*numtypes_par];
			score0 = new double[Ny*Nx*numtypes];
			Ix0 = new int[Ny*Nx*numtypes];
			Iy0 = new int[Ny*Nx*numtypes];
			for(int k=0;k<numtypes;k++) {
				eHshiftdt(score0+k*Ny*Nx,Ix0+k*Ny*Nx,Iy0+k*Ny*Nx,
						Nx,Ny,child->startx[k],child->starty[k],child->step,
						child_data->score+k*Ny*Nx,Nx,Ny,
						model->defs[child->defid[k]].w);
			}
			/*at each parent location, for each mixture 1:L, find best child*/
			for(int l=0;l<model->parts[par].num;l++){
				for(int i=0;i<Ny*Nx;i++){
					double tmp_max = -EH_INF;
					int tmp_argmax = -1;
					for(int j=0;j<numtypes;j++) {
						if(score0[j*Ny*Nx+i]+model->biases[child->biasid[j*numtypes_par+l]].w>tmp_max) {
							tmp_max = score0[j*Ny*Nx+i]+model->biases[child->biasid[j*numtypes_par+l]].w;
							tmp_argmax = j;
						}
					}
					msg[l*Ny*Nx+i]=tmp_max;
					child_data->Ik[l*Ny*Nx+i]=tmp_argmax;
					child_data->Iy[l*Ny*Nx+i]=Iy0[tmp_argmax*Ny*Nx+i];
					child_data->Ix[l*Ny*Nx+i]=Ix0[tmp_argmax*Ny*Nx+i];
				}
			}
			for(int i=0;i<Ny*Nx*numtypes_par;i++)
				parts_data[par].score[i]+=msg[i];
			delete[] score0;
			delete[] Ix0;
			delete[] Iy0;
			delete[] msg;
		}

		/* add bias */
		double rootbias = model->biases[rootpart->biasid[0]].w;
		double* rscore = new double[Ny*Nx];
		int* rIk = new int[Ny*Nx];
		vector<int> slct; slct.reserve(10000);
		for(int i=0;i<Ny*Nx;i++){
			double tmp_max=-EH_INF;
			int tmp_argmax=-1;
			for (int j=0;j<rootpart->num;j++) {
				rootpart_data->score[j*Ny*Nx+i] += rootbias;
				if(rootpart_data->score[j*Ny*Nx+i]>tmp_max) {
					tmp_max = rootpart_data->score[j*Ny*Nx+i];
					tmp_argmax = j;
				}
			}
			rscore[i] = tmp_max;
			rIk[i] = tmp_argmax;
			if(tmp_max>=thrs)
				slct.push_back(i);
		}

		/* backtrack */
		if(!slct.empty()) {
			/* root */
			int k0=boxes.size();
			int newboxes_len = slct.size();
			boxes.resize(k0+newboxes_len);
			int* ptr = new int[numparts*newboxes_len];
			int padx = max(model->maxsize[1]-2,0);
			int pady = max(model->maxsize[0]-2,0);
			double scale;
			for(int i=0;i<newboxes_len;i++){
				//ptr[i]=slct[i];
				scale = pyra->scale[rootpart_data->level];
				int y = slct[i]%Ny;
				int x = (slct[i]-y)/Ny;
				int mix = rIk[slct[i]];
				ptr[i] = mix*Ny*Nx+slct[i];
				fbox_t tmpbox = {
					(x-padx)*scale,
					(y-pady)*scale,
					(x-padx+rootpart->sizx[mix])*scale-1,
					(y-pady+rootpart->sizy[mix])*scale-1
				};
				boxes[k0+i].boxes.push_back(tmpbox);
				boxes[k0+i].component = 0; /* only 1 component for pose model */
				boxes[k0+i].score = rscore[slct[i]];
			}
			/*remaining parts*/
			for(int k=1;k<numparts;k++){
				const posepart_t* tmppart = &(model->parts[k]);
				posepart_data* tmppart_data = &(parts_data[k]);
				int par = tmppart->parent;
				scale = pyra->scale[tmppart_data->level];
				int x,y,mix;
				for(int i=0;i<newboxes_len;i++){
					x = tmppart_data->Ix[ptr[par*newboxes_len+i]];
					y = tmppart_data->Iy[ptr[par*newboxes_len+i]];
					mix = tmppart_data->Ik[ptr[par*newboxes_len+i]];
					ptr[k*newboxes_len+i] = mix*Ny*Nx+x*Ny+y;
					fbox_t tmpbox = {
						(x-padx)*scale,
						(y-pady)*scale,
						(x-padx+tmppart->sizx[mix])*scale-1,
						(y-pady+tmppart->sizy[mix])*scale-1
					};
					boxes[k0+i].boxes.push_back(tmpbox);
				}
			}

			delete[] ptr;
		}


		/* clean Ix Iy Iz */
		for(int p=numparts-1;p>0;p--) {
			posepart_data* child = &(parts_data[p]);
			delete[] child->Ik;
			delete[] child->Iy;
			delete[] child->Ix;
			delete[] child->score;
		}
		delete[] parts_data[0].score;
		delete[] rscore;
		delete[] rIk;

	}
	
	/* released allocated memory */
	for(int i=0; i<pyra->len; i++) 
		if(resp[i]!=NULL) mat3d_delete(resp[i]);
	delete[] resp;
	featpyra_delete(pyra);
	
	/* clip boxes within image and do non-maximum suppression */
	for(unsigned i=0; i<boxes.size(); i++)
		bbox_clipboxes(boxes[i], imsize);
	bbox_v_nms(boxes,0.1,1000);

	/*testing code: display outer bbox*/
/*	using namespace cv;
	Mat M(img->sizy,img->sizx,CV_8UC3);
	for(unsigned y=0;y<img->sizy;y++) {
		for(unsigned x=0;x<img->sizx;x++) {
			M.at<Vec3b>(y,x)[2]=img->ch[0][y+x*img->sizy];
			M.at<Vec3b>(y,x)[1]=img->ch[1][y+x*img->sizy];
			M.at<Vec3b>(y,x)[0]=img->ch[2][y+x*img->sizy];
		}
	}

	for(unsigned i=0;i<boxes.size();i++){
		int x1 = (int)boxes[i].outer.x1;
		int y1 = (int)boxes[i].outer.y1;
		int w = (int)boxes[i].outer.x2 - x1;
		int h = (int)boxes[i].outer.y2 - y1;
		rectangle(M, Rect(x1,y1,w,h),Scalar(0,255,0));
	}
	namedWindow("test", CV_WINDOW_AUTOSIZE);
	imshow("test",M);
	waitKey();
*/
	return boxes;
}
vector<bbox_t> posemodel_detect(const posemodel_t* model, const image_ptr img) {
	return posemodel_detect(model, img, model->thresh);
}

void posemodel_delete(posemodel_t* model) {
	/*
	 * filters[i].w.vals was allocated inside parseCSV2double
	 */
	for(unsigned i=0;i<model->filters.size();i++)
		delete[] model->filters[i].w.vals;
	/*
	 * parts[i].biasid/defid/filterid were allocated inside parseCSV2int
	 * parts[i].sizy/sizx/starty/startx were newed
	 */
	delete model->parts[0].biasid;
	delete[] model->parts[0].filterid;
	delete[] model->parts[0].sizy;
	delete[] model->parts[0].sizx;
	for(unsigned i=1;i<model->parts.size();i++) {
		delete[] model->parts[i].biasid;
		delete[] model->parts[i].defid;
		delete[] model->parts[i].filterid;
		delete[] model->parts[i].sizy;
		delete[] model->parts[i].sizx;
		delete[] model->parts[i].starty;
		delete[] model->parts[i].startx;
	}	
	delete model;
}
