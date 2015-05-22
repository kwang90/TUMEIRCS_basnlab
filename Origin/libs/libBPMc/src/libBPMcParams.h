/*
 * libBPMcParams.h
 *
 *  Created on: 08.11.2013
 *      Author: kindt
 */

#ifndef LIBBPMCPARAMS_H_
#define LIBBPMCPARAMS_H_

//wireless mouse
/*
#define LIBBPMC_PARAM_DEFAULT_MAXTRAFFIC 375.0
#define LIBBPMC_PARAM_DEFAULT_BUFFERLEVEL 40 //in addition to emergency level
#define LIBBPMC_PARAM_DEFAULT_MAXLATENCY 4.0
#define LIBBPMC_PARAM_QUALITY_OF_SERVICE_TC_REDUCTION 1
#define LIBBPMC_PARAMS_IO_R_LENGTH 20
*/

//testpattern 2s
/*
#define LIBBPMC_PARAM_DEFAULT_MAXTRAFFIC 1200.0
#define LIBBPMC_PARAM_DEFAULT_BUFFERLEVEL 500 //in addition to emergency level
#define LIBBPMC_PARAM_DEFAULT_MAXLATENCY 10.0
#define LIBBPMC_PARAM_QUALITY_OF_SERVICE_TC_REDUCTION 1
#define LIBBPMC_PARAMS_IO_R_LENGTH 61
*/

//testpattern ramp + burst
/*
#define LIBBPMC_PARAM_DEFAULT_MAXTRAFFIC 980.0
#define LIBBPMC_PARAM_DEFAULT_BUFFERLEVEL 200 //in addition to emergency level
#define LIBBPMC_PARAM_DEFAULT_MAXLATENCY 15.0
#define LIBBPMC_PARAM_QUALITY_OF_SERVICE_TC_REDUCTION 1			//use for buffer + periodic, not for lazy decrease!
#define LIBBPMC_PARAMS_IO_R_LENGTH 98
#define LIBBPMC_PARAMS_TC_START 8
*/

//Daniels compressed ECG

#define LIBBPMC_PARAM_DEFAULT_MAXTRAFFIC 435.0
#define LIBBPMC_PARAM_DEFAULT_BUFFERLEVEL 100 //in addition to emergency level
#define LIBBPMC_PARAM_DEFAULT_MAXLATENCY 15.0
#define LIBBPMC_PARAM_QUALITY_OF_SERVICE_TC_REDUCTION 1
#define LIBBPMC_PARAMS_IO_R_LENGTH 201
#define LIBBPMC_PARAMS_TC_START 18






//BLE112-specific params
#define LIBBPMC_PARAM_BLE_NSEQ_MAX 0.5
#define LIBBPMC_PARAM_DEFAULT_WC_INTERVALS_UNTIL_UPDATE 8.0				//N_{i,l,max}
#define LIBBPMC_PARAM_USE_IDLE_LINE_DETECTION 0

#define LIBBPMC_PARAM_IDLE_LINE_TIMEOUT_MICROSECONDS 1000000			/// The default number of microseconds for the idle-line detection.

/* Window : 0.3s*/
#define LIBBPMC_PARAM_WORST_CASE_WINDOW 0.3								//worst case window used in seconds

#define LIBBPMC_PARAM_AVERAGE_THROUGHPUT_DEGENERATION_RELATIVE 0.003036 //Worst case reduction of the average throughput due to congestions for infinite window width(dimensionless, relative)
#define LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION 0.263  //Worst case reduction of the wiwndow traffic related to the time window of LIBBPMC_PARAM_WORST_CASE_WINDOW
#define LIBBPMC_PARAM_WORST_CASE_THROUGHPUT_DEGENERATION_2TC 0.5999999			//Worst case traffic within 2 connection intervals

/* Window : 0.5s
#define LIBBPMC_PARAM_WORST_CASE_WINDOW 0.5								//worst case window used in seconds
#define LIBBPMC_PARAM_AVERAGE_THROUGHPUT_DEGENERATION_RELATIVE 0.003036 //Worst case reduction of the average throughput due to congestions for infinite window width(dimensionless, relative)
#define LIBBPMC_PARAM_WORST_CASE_WINDOW_THROGUHPUT_DEGENERATION 0.263  //Worst case reduction of the wiwndow traffic related to the time window of LIBBPMC_PARAM_WORST_CASE_WINDOW
#define LIBBPMC_PARAM_WORST_CASE_THROUGHPUT_DEGENERATION_2TC 0.5999999			//Worst case traffic within 2 connection intervals
*/
#endif /* LIBBPMCPARAMS_H_ */
