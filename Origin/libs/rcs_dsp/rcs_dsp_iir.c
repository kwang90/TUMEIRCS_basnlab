/*
 * rcs_dsp_iir.h
 *
 *  Created on: 19.03.2013
 *      Author: kindt
 */

#include <malloc.h>
#include <string.h>
#include "rcs_dsp_iir.h"

void rcs_dsp_iir_init(rcs_dsp_iir* iir){
	iir->nFeedForward = 0;
	iir->nFeedBack = 0;
	iir->coeffFwd = NULL;
	iir->coeffBack = NULL;
	iir->histIn = NULL;
	iir->histOut = NULL;
	iir->ringBufPos = 0;
	iir->dataInHist = 0;
};

void rcs_dsp_iir_setFeedForward(rcs_dsp_iir* iir, uint32_t nCoeff, double* coeffs){
	iir->nFeedForward = nCoeff;
	uint32_t cnt;

	iir->coeffFwd = malloc(nCoeff*sizeof(double));
	iir->histIn = malloc(nCoeff*sizeof(double));
	if(iir->histIn == NULL){
		printf("Malloc failed.");;
		rcs_dsp_iir_init(&iir);
		return;
	}
	memset(iir->histIn,0,nCoeff);
	//copy coefficients
	for(cnt = 0; cnt < nCoeff; cnt++){
		iir->coeffFwd[cnt] = coeffs[cnt];
	}

	//reset ringbuf pos
	iir->ringBufPos = 0;

}

void rcs_dsp_iir_setFeedBack(rcs_dsp_iir* iir, uint32_t nCoeff, double* coeffs){
	iir->nFeedBack = nCoeff;
	uint32_t cnt;
	iir->coeffBack = malloc(nCoeff*sizeof(double));
	iir->histOut = malloc(nCoeff*sizeof(double));

	if(coeffs[0] != 0.0){
		printf("waring: first feedback coefficient must be zero! (causal system)\n");
	}
	//copy coefficients
	for(cnt = 0; cnt < nCoeff; cnt++){
		iir->coeffBack[cnt] = coeffs[cnt];
	}

	//reset ringbuf pos
	iir->ringBufPos = 0;
}


double get_in_from_hist(rcs_dsp_iir* iir, uint32_t index){
	if(index > iir->nFeedForward){
		printf("warining: index out of range: %d - max is %d\n",index, iir->nFeedForward);
		return 0.0;
	}
	if(index > iir->dataInHist){
		printf("warining: history does not contain engough elemts for index %d - max is %d\n",index, iir->dataInHist);
		return 0.0;
	}

	int32_t absIndex;
	absIndex = iir->ringBufPos - index;
	if(absIndex < 0){
		absIndex = iir->nFeedForward + absIndex;
	}
//	printf("absIndex(in) = %d\n",absIndex);
	return iir->histIn[absIndex];
}



double get_out_from_hist(rcs_dsp_iir* iir,uint32_t index){
	if(index > iir->nFeedBack){
//		printf("warining: index out of range: %d - max is %d\n",index, iir->nFeedBack);
		return 0.0;
	}
	if(index > iir->dataInHist){
	//	printf("warining: history does not contain engough elemts for index %d - max is %d\n",index, iir->dataInHist);
		return 0.0;
	}
	if(index == 0){
	//	printf("warining: invalid Index: 0 for output history. This index is not in the past, and the time-machine has yet to be invented by the RCS.\n");
		return 0.0;
	}

	int32_t absIndex;
	absIndex = iir->ringBufPos - index;
	if(absIndex < 0){
		absIndex = iir->nFeedBack + absIndex;
	}

	return iir->histOut[absIndex];
}
double rcs_dsp_iir_apply(rcs_dsp_iir* iir, double in){
	double response;
	if((iir->coeffFwd == NULL)||(iir->coeffBack == NULL)){
		printf("feedForward/feedBack path not configured. Please call rcs_dsp_iir_setFeedForward() and rcs_dsp_iir/setFeedBack(), first.\n");
		return 0;
	}
	if(iir->nFeedForward != iir->nFeedBack ){
		printf("Coefficients do not match: number of coefficients in feedforward-path not equal to feedback-path.\n");
		return 0;
	}

	//add data
	iir->histIn[iir->ringBufPos] = in;
	iir->dataInHist++;
	if(iir->dataInHist > iir->nFeedForward){
		iir->dataInHist = iir->nFeedForward;
	}

	response = 0.0;
	uint32_t cnt;
	for(cnt = 0; cnt < iir->nFeedForward; cnt++){

		response += get_in_from_hist(iir, cnt) * iir->coeffFwd[cnt];
	//	printf("\tcnt=%d: coeffFwd=%.4f, coeffBackWd=%.3f\t\t histIn=%.3f, histOut=%.3f\n",cnt, iir->coeffFwd[cnt],iir->coeffBack[cnt],get_in_from_hist(iir, cnt),get_out_from_hist(iir, cnt));
		if(cnt > 0){
			response += -1.0 * ((double) get_out_from_hist(iir,cnt)) * iir->coeffBack[cnt];
		}
	}

	iir->histOut[iir->ringBufPos] = response;


//	printf("\n\n------------------hist-----------------\n");
//	uint32_t i;
//	for(i = 0; i < iir->dataInHist; i++){
//		printf("h[%d]: i->%.3f o-> %.3f - ",i, get_in_from_hist(iir,i),get_out_from_hist(iir,i));
//	}
//	printf("\n-----------------------------------------\n\n");

	iir->ringBufPos++;
	iir->ringBufPos = iir->ringBufPos%iir->nFeedForward;

	return response;
}


