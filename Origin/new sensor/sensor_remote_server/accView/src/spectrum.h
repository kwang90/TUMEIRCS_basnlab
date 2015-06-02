/*
 * spectrum.h
 *
 *  Created on: 18.03.2013
 *      Author: kindt
 */
#include <inttypes.h>
#include <vector>
#ifndef SPECTRUM_H_
#define SPECTRUM_H_

using namespace std;
class spectrum{
private:
	double* data;					//input data
	double* results;				//fftw results
	uint32_t ringBufPos;			//current position in ringbuf
	uint32_t width;					//total number of bytes the ringbuf can obtain
	bool filled;					//true, if ringbuf is filled completely. Is invalidated by setDataLenght
	bool dftReady;					//DFT has been computed for the current set of values. Is invalidated by setDataLenght() and addData();
	double samplingFrequency;		//Sampling rate of sampled data in [Hz].
	double samplingInterval;		//will allways be 1/samplingRate. Unit is [s]
	double fourierFrequencySpacing;	//will contain the frequency spacing for the fourier 5frequences. Example: DFT coefficient 0 corresponds to 0* fourierFrequencySpacing (DC-component); DFT coefficient 15 corresponds to 15*fourierFrequencySpacing...


public:

	/*
	 * Constructor will set sampling rate to 1 Hz;
	 */
	spectrum();


	/**
	 * Set number of samples of input data. As soon as this amount of data has been added, calcSpectrum() can be used.
	 */
	void setDataLenght(uint32_t nSamples);

	/**
	 * Read sample having index < with
	 */
	double& get(uint32_t index) const;

	/*
	 * Add a new sample
	 *
	 */
	void addData(double data);

	/* Calculates the spectrum of the samples added and
	 * returns spectral amplitude as an array. The number of these coefficients is the same
	 * as width (=nSamples of setDataLenght)
	 * The  pointer returned is free'ed automatically at destruction or with setDataLength() - do not free manually.
	 */
	double* calcSpectrum();

	/* Returns results of last calcSpectrum().
	 * This pointer is free'ed automatically at destruction or with setDataLength() - do not free manually.
	 */
	double* getResults() const;

	/* Returns the average of all dft coefficients of the last DFT. Before this
	 * function can be called, calcSpectrum() must be called. If setDataLenght() ot addData() is called,
	 * calcSpectrum() has to be called again before this function can be called.
	 */
	double getSpectralAverage() const;

	double getFourierFrequency(uint8_t index);
	double getLocalSpectralDeviation(uint32_t index, uint32_t order);
	vector<int>* localizeSpectrumPeaks(double relThreshold);


	vector<int> localizePeaks(double relThreshold);
};

#endif /* SPECTRUM_H_ */
