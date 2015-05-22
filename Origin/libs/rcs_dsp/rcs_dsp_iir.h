/*
 * rcs_dsp_iir.h
 *
 *  Created on: 19.03.2013
 *      Author: kindt
 */

#ifndef RCS_DSP_IIR_H_
#define RCS_DSP_IIR_H_
#ifdef __cplusplus
#pragma message("C++ detected")
extern "C" {
#endif
#include <inttypes.h>

typedef struct _rcs_dsp_iir{
	uint32_t nFeedForward;
	uint32_t nFeedBack;
	double* coeffFwd;
	double* coeffBack;

	double* histOut;
	double* histIn;

	uint32_t ringBufPos;
	uint32_t dataInHist;

} rcs_dsp_iir;

void rcs_dsp_iir_init(rcs_dsp_iir* iir);
void rcs_dsp_iir_setFeedForward(rcs_dsp_iir* iir, uint32_t nCoeff, double* coeffs);
void rcs_dsp_iir_setFeedBack(rcs_dsp_iir* iir, uint32_t nCoeff, double* coeffs);
double get_in_from_hist(rcs_dsp_iir* iir, uint32_t index);
double get_out_from_hist(rcs_dsp_iir* iir,uint32_t index);
double rcs_dsp_iir_apply(rcs_dsp_iir* iir, double in);
#ifdef __cplusplus
#pragma message("C++ detected")
}
#endif
#endif /* RCS_DSP_IIR_H_ */
