/*
 * spectrum.c
 *
 *  Created on: 18.03.2013
 *      Author: kindt
 */

#include "spectrum.h"
#include <fftw3.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <stdlib.h>

using namespace std;
spectrum::spectrum(){
	data = NULL;
	results = NULL;
	width = 0;
	ringBufPos = 0;
	filled = false;
	dftReady = false;
	samplingFrequency = 1.0;
	samplingInterval = 1.0;
	fourierFrequencySpacing = 0;
}
void spectrum::setDataLenght(uint32_t nSamples){
	if(data != NULL){
		delete[] data;
		delete[] results;
	}
	cout<<"data lenght set to "<<nSamples<<" samples"<<endl;
	data = new double[nSamples * 2];
	results = new double[nSamples * 2];
	memset(data,0,nSamples * 2);
	memset(results,0,nSamples * 2);
	ringBufPos = 0;
	filled = false;
	width = nSamples;
	dftReady = false;
	fourierFrequencySpacing = samplingFrequency / ((double) width);
}

void spectrum::addData(double sample){
	if(data == NULL){
		cout<<"warning: data is NULL."<<endl;
		return;
	}
	dftReady = false;
	if(isinf(sample) || isnan(sample)){
		cout<<"Invalid value: "<<sample<<" - either INF or NAN..."<<endl;
		exit(1);
	}
//	cout<<"ringBufPos="<<ringBufPos<<" - width = "<<width<<endl;
	this->data[ringBufPos] = sample;
//	cout<<"sample: "<<this->data[ringBufPos]<<endl;
	ringBufPos++;
	if(ringBufPos >= width){
		ringBufPos = 0;
	}
	if(ringBufPos == width - 1){
		filled = true;
	}
}

double* spectrum::calcSpectrum(){
	dftReady = true;
	if((data == NULL)||(results == NULL)||(width <= 1)||(!filled)){
		return NULL;
	}
	int32_t start = width - ringBufPos;

	/*
	 * prepare results - put linear in memory
	 */
	double* buf = (double*) fftw_malloc(width * sizeof(double));
	for(uint32_t cnt = ringBufPos; cnt < width; cnt++){
	//	printf("1:%d => %d = %f\n",cnt,cnt - ringBufPos,data[cnt]);
		buf[cnt - ringBufPos] = data[cnt];
	}
	for(uint32_t cnt = 0; cnt < ringBufPos; cnt++){
	//	printf("2:%d => %d = %f\n",cnt,cnt - ringBufPos,data[cnt]);
		buf[cnt + start] = data[cnt];
	}

	/*
	 * Perform DFT. Real-valued input samples are in general non-sinosidal and, therefore, will be complex in general
	 *
	 */
	fftw_complex *out = (fftw_complex*) fftw_alloc_complex(width);
	if(out == NULL){
			cerr<<"fft_alloc failed"<<endl;
			exit(1);

	}
	fftw_plan p = fftw_plan_dft_r2c_1d(width,buf, out,FFTW_ESTIMATE);
	fftw_execute(p);


	for(uint32_t cnt = 0; cnt < width; cnt++){
		cout<<"buf["<<cnt<<"] = "<<buf[cnt]<<" - out["<<cnt<<"] = "<<out[cnt][0]<<" + "<<out[cnt][1]<<"i"<<endl;
		results[cnt] = sqrt(out[cnt][0] * out[cnt][0] + out[cnt][1] * out[cnt][1]);

		if((isinf(results[cnt]))||(isnan(results[cnt]))){
			cout<<"ERROR - DFT-Resuls are infinite or not a number. This cannot happen. In: in["<<cnt<<"] = "<<buf[cnt]<<" - Result: "<<out[cnt][0]<<"+ "<<out[cnt][1]<<"i - sqrt(result^2) = "<<results[cnt]<<endl;
		}
	//	cout<<","<<results[cnt];
	}
///	cout<<"\n\n";
	fftw_destroy_plan(p);

	fftw_free(out);
	fftw_free(buf);
	return results;
}

double* spectrum::getResults() const{
	return results;

}

double& spectrum::get(uint32_t index) const{
	if(index > width){
		cout<<"Error: index = "<<index<<" > width = "<<width<<endl;
		exit(1);
	}
	if(!filled){
		cout<<"Error : buffer not filled,yet. Please make sure it is full before calling spectrum::get() "<<endl;
		exit(1);
	}

	if(index < ringBufPos){
		return data[ringBufPos - index - 1];
	}else{
		return data[width + ringBufPos - index - 1];
	}


}


double spectrum::getSpectralAverage() const{
	double avg = 0;
	if(!dftReady){
		cout<<"Error: DFT data not up to date. Call calcSpectrum() before using the getSpectralAverage()-function. After calling addData() or setDataLenght(), calcSpectrum() has to be called again."<<endl;
		exit(1);
	}
	for(uint32_t cnt = 0; cnt < width; cnt++){
		avg += results[cnt];
	}
	avg /= ((double) width);
	return avg;
}

double spectrum::getFourierFrequency(uint8_t index){
	if(index >= width){
		cout<<"Invalid index: "<<index<<" - max: "<<width<<endl;
		exit(1);
	}
	return index*fourierFrequencySpacing;
}

double spectrum::getLocalSpectralDeviation(uint32_t index, uint32_t order){
	if((index >= width)||(index < order)){
		cout<<"invalid index: "<<index<<endl;
		exit(1);
	}
	if(order == 0){
		cout<<"invalid order: "<<order<<endl;
		exit(1);
	}

	if(order == 1){
		return (get(index) - get(index - 1))/fourierFrequencySpacing;
	}else{
		return (getLocalSpectralDeviation(index, order-1) - getLocalSpectralDeviation(index - 1, order-1))/fourierFrequencySpacing;
	}

}


vector<int>* spectrum::localizeSpectrumPeaks(double relThreshold){
	vector<int>* rv = new vector<int>;

	return rv;
}

