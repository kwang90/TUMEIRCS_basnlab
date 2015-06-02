
#include "libBPMcConstraints.h"
#include "libBPMcParams.h"

uint32_t libBPMcConstraints_getMaxConnectionInterval(double nSeq, uint32_t nBytesControl, uint32_t nBytesMaxWrite, double rAppMax, double tLatencyMax,uint32_t qualityOfServiceTcReduction){
/*
	double nyWC = 1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION;
	double A = sqrt(\
				400*nSeq*nSeq * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * nyWC*nyWC \
				- 800 * nSeq * nSeq * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * nyWC \
				+ 400 * nSeq * nSeq * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE \
				- 40 * nSeq * nBytesControl *  2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * (1-nyWC) \
				+ 40 * nSeq * nBytesMaxWrite * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * (nyWC - 1) \
				+ 40 * nSeq * LIBBPMC_PARAM_WORST_CASE_WINDOW * rAppMax * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * (nyWC - 1)\
				+ nBytesControl * nBytesControl \
				+ 2.0 * nBytesControl * rAppMax * LIBBPMC_PARAM_WORST_CASE_WINDOW \
				+ 2.0 * nBytesControl * nBytesMaxWrite \
				+ nBytesMaxWrite * nBytesMaxWrite \
				+ 2.0 * nBytesMaxWrite * rAppMax * LIBBPMC_PARAM_WORST_CASE_WINDOW \
				+ rAppMax * LIBBPMC_PARAM_WORST_CASE_WINDOW * rAppMax * LIBBPMC_PARAM_WORST_CASE_WINDOW \
				- nSeq * rAppMax * tLatencyMax * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * nyWC * 80.0\
				+ nSeq * rAppMax * tLatencyMax * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * 80.0\
				);



	double rv = -1 * (nBytesControl + nBytesMaxWrite + LIBBPMC_PARAM_WORST_CASE_WINDOW * rAppMax - 20.0 * nSeq * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE + 20 * nSeq * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * nyWC - A)/ (2 * 2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * rAppMax);
*/
	double Nk = LIBBPMC_PARAMS_IO_R_LENGTH + LIBBPMC_PARAM_WORST_CASE_WINDOW * rAppMax + nBytesControl;
	double gamma = Nk * Nk  - 80.0 * Nk * nSeq * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * (1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION) + 1600.0 * nSeq * nSeq * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * (1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION) * (1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION) + 160.0 * rAppMax * tLatencyMax * nSeq * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * (1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION);
	double rv1 = (40.0 * nSeq * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE - Nk - sqrt(gamma)*(1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION))/(4.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * rAppMax);
	double rv2= (40.0 * nSeq * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE - Nk + sqrt(gamma)*(1.0-LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION))/(4.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE * rAppMax);
//	printf("\n\n---------------\n");
//	printf("rv1 = %f, rv2 = %f\n",rv1/0.00125,rv2/0.00125);
//	printf("-------------------\n");

	return floor(rv2/0.00125) - qualityOfServiceTcReduction;
}

uint32_t libBPMcConstraints_getBufferSize(double nSeq, uint32_t nBytesControl, uint32_t nBytesMaxWrite, double rAppMax, double tLatencyMax, uint32_t qualityOfServiceTcReduction){
	double tcMax = (double) libBPMcConstraints_getMaxConnectionInterval(nSeq,nBytesControl, nBytesMaxWrite, rAppMax,tLatencyMax,qualityOfServiceTcReduction) * 0.00125;
//	fprintf(stderr,"tcMax = %d/%f\n",tcMax/0.00125,tcMax);

	return ceil(2.0 * LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE *  (tcMax  * rAppMax - 20.0*nSeq*(1.0 - LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION)) + LIBBPMC_PARAM_WORST_CASE_WINDOW * rAppMax + LIBBPMC_PARAMS_IO_R_LENGTH + nBytesControl);

}
