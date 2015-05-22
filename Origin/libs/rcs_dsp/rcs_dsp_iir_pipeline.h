/*
 * rcsFilterPipeline.h
 *
 *  Created on: 20.03.2013
 *      Author: kindt
 */

#ifndef RCSFILTERPIPELINE_H_
#define RCSFILTERPIPELINE_H_
#include "rcs_dsp_iir.h"
#include "rcs_dsp_ll.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct _rcs_dsp_iir_pipeline{
	rcs_dsp_ll nodes;

} rcs_dsp_iir_pipeline;

typedef struct _rcs_dsp_iir_pipeline_node{
	rcs_dsp_iir* iir;
	char* ident;
} rcs_dsp_iir_pipeline_node;

void rcs_dsp_iir_pipeline_init(rcs_dsp_iir_pipeline *pl);
void rcs_dsp_iir_pipeline_add(rcs_dsp_iir_pipeline *pl, rcs_dsp_iir* iir,char* ident);
double rcs_dsp_iir_pipeline_apply(rcs_dsp_iir_pipeline *pl, double in);
rcs_dsp_iir_pipeline_node* rcs_dsp_iir_pipeline_get_node(rcs_dsp_iir_pipeline *pl, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif /* RCSFILTERPIPELINE_H_ */
