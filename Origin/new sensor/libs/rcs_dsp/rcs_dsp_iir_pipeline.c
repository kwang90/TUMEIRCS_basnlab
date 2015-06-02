/*
 * rcsFilterPipeline.h
 *
 *  Created on: 20.03.2013
 *      Author: kindt
 */


#include "rcs_dsp_iir_pipeline.h"
#include <inttypes.h>

#ifndef NULL
#define NULL ((void*) 0)
#endif
void rcs_dsp_iir_pipeline_init(rcs_dsp_iir_pipeline *pl){
	rcs_dsp_ll_init(&(pl->nodes));
}

void rcs_dsp_iir_pipeline_add(rcs_dsp_iir_pipeline *pl, rcs_dsp_iir* iir,char* ident){
	rcs_dsp_iir_pipeline_node* node = malloc(sizeof(rcs_dsp_iir_pipeline_node));
	node->iir = iir;
	node->ident = strdup(ident);
	rcs_dsp_ll_push(&(pl->nodes), node);
}

double rcs_dsp_iir_pipeline_apply(rcs_dsp_iir_pipeline *pl, double in){
	if(rcs_dsp_ll_isempty(&(pl->nodes))){
		return 0;
	}
	double result = 0;
	rcs_dsp_ll_node* lln = pl->nodes.root;
	rcs_dsp_iir_pipeline_node* pln;
	do{
		pln = (rcs_dsp_iir_pipeline_node*) lln->data;
		result += rcs_dsp_iir_apply(pln->iir, result);

	}while((lln = rcs_dsp_ll_get_next(lln))!=NULL);
	return result;
}

rcs_dsp_iir_pipeline_node* rcs_dsp_iir_pipeline_get_node(rcs_dsp_iir_pipeline *pl, uint32_t index){
	if(rcs_dsp_ll_isempty(&(pl->nodes))){
			return 0;
		}
		rcs_dsp_ll_node* lln = pl->nodes.root;
		rcs_dsp_iir_pipeline_node* pln;
		uint32_t cnt = 0;

		do{
			pln = (rcs_dsp_iir_pipeline_node*) lln->data;
			if(cnt == index){
				return pln;
			}
			cnt++;
		}while((lln = rcs_dsp_ll_get_next(lln))!=NULL);
		return NULL;;
}

