#include "libBPMcBuffer.h"
#include "libBPMcDebug.h"
#include "../../bleEnergyModel/src/ble_model.h"
#include <stdio.h>
#include <inttypes.h>
#include <pthread.h>
#include <time.h>
#include "libBPMcMatrix.h"
#include "libBPMcTrafficMeasurement.h"
#include "libBPMcTrafficAnalysis.h"
#include "strategies/libBPMcStrategyMarkov.h"
#include "strategies/libBPMcStrategyBuffer.h"
libBPMcBuffer b;


#ifdef LIBBPMC_TEST_STANDALONE
void read_thread(){
	char tmp2[40];
	uint32_t bytes_read;
	while(1){
		bytes_read = libBPMcBuffer_read(&b,tmp2,40,0);
		printf("read: %d bytes:\n",bytes_read);
		printf("rd: {");
		write(1,tmp2,bytes_read);
		printf("}\n");
		usleep(5000000);

	}
}
int main(){

		uint32_t tcOld = 20;
		uint32_t tcNew = 21;
		double nSeqOld = 0.5;
		double nSeqNew = 0.5 ;
		double qTcOld = 0;
		double qTcNew = 0;
		double qUpdate = 0;
		//workaround
		if(nSeqOld < 1){
			nSeqOld = 1;
		}
		if(nSeqNew < 1){
			nSeqNew = 1;
		}
		printf("(uint8_t) nSeqOld = %d\n",(uint8_t) nSeqOld);
		printf("(uint8_t) nSeqNew = %d\n",(uint8_t) nSeqNew);
	//charge consumed without connection update per connection interval
	 	 qTcOld = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,10,37,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);			//master
	     qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,37,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave
	 	 qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,15,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);			//master
	     qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,10,15,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave
	     qTcOld /= 2.0;

		//charge consumed when updating the connection interval
		 qTcNew = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,10,37,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//master transmission
		 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,37,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave transmission
		 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,15,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//master transmission
		 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,10,15,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave transmission
		 qTcNew /= 2.0;

		 //update cost
		 qUpdate = ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,1,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);
	     qUpdate += ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,0,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);

	     //savings in one Interval of tcNew (this amount of charge is consumed less within 1*tcNew compared to before the update)
	     double qSavings = (tcNew * 0.00125 / (tcOld * 0.00125) * qTcOld - qTcNew);
	     double nPayOff = qUpdate / qSavings;
	     printf("QUpdate master+slave: %f\n",qUpdate);
		 printf("Intervals: %d -> %d \t Charges: qTcOld = %f, qTcNew = %f, qUpdate = %f\n",tcOld, tcNew, qTcOld, qTcNew, qUpdate);
		 printf("This corresponds to %f intervals of m+s\n",nPayOff);



	libBPMcMatrix ma;
	libBPMcMatrix mb;
	libBPMcMatrix mc;
	libBPMcMatrix_init(&ma,3,3);
	libBPMcMatrix_init(&mb,3,3);
	ma.data[0][0] = 1;
	ma.data[0][1] = 2;
	ma.data[0][2] = 3;
	ma.data[1][0] = 4;
	ma.data[1][1] = 5;
	ma.data[1][2] = 6;
	ma.data[2][0] = 7;
	ma.data[2][1] = 8;
	ma.data[2][2] = 9;

	mb.data[0][0] = 10;
	mb.data[0][1] = 20;
	mb.data[0][2] = 30;
	mb.data[1][0] = 40;
	mb.data[1][1] = 50;
	mb.data[1][2] = 60;
	mb.data[2][0] = 70;
	mb.data[2][1] = 80;
	mb.data[2][2] = 90;

	libBPMcMatrix_matMult(&ma,&mb,&mc,1);
	printf("a = \n");
	libBPMcMatrix_print(&ma);
	printf("b = \n");
	libBPMcMatrix_print(&mb);
	printf("a x b = \n");
	libBPMcMatrix_print(&mc);
	printf("a x a = \n");
	libBPMcMatrix_matMult(&ma,&ma,&mc,0);
	libBPMcMatrix_matPow(&ma,&mc,2,0);
	libBPMcMatrix_matExpTaylor(&ma,&mc,50,0);

	libBPMcMatrix_print(&mc);

//	double penaltyTime = libBPMcStrategyMarkov_getPenaltyTime(15,50,0.1,0.0333,0.5);
//	printf("penalty time: %f s\n",penaltyTime);
	/*
	char tmp[] = "test123456789ABCDEFG";
	char tmp2[40];
	libBPMcBuffer_init(&b);
	pthread_t thread;
  //  pthread_create(&thread, NULL, read_thread, (void *) NULL);
	//usleep(1000000);
    libBPMcBuffer_write(&b,tmp,10,1);
	libBPMcBuffer_read(&b,tmp2,40,0);
	printf("read back: %s\n",tmp2);
    //usleep(1000000);

	libBPMcBuffer_write(&b,tmp,2,1);
	usleep(10000000);
	libBPMcBuffer_write(&b,tmp,1,1);
	usleep(1000000);

	libBPMcBuffer_write(&b,tmp,10,1);
	usleep(1000000);
	libBPMcBuffer_write(&b,tmp,2,1);
	usleep(1000000);
	libBPMcBuffer_write(&b,tmp,1,1);
	usleep(1000);

	printf("test 2\n");
*/
	libBPMcTrafficMeasurement trm;
	libBPMcTrafficMeasurement_init(&trm,5);
	libBPMcTrafficMeasurement_setClassBorder(&trm,0,10);
	libBPMcTrafficMeasurement_setClassBorder(&trm,1,20);
	libBPMcTrafficMeasurement_setClassBorder(&trm,2,30);
	libBPMcTrafficMeasurement_setClassBorder(&trm,3,40);
	libBPMcTrafficMeasurement_setClassBorder(&trm,4,50);
	libBPMcTraffic tr;
	tr.nBytes = 1;
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(10000);										//100 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(20000);										//50 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(35000);										//28,6 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(45000);										//22,2 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(65000);										//15,38 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	tr.nBytes = 10;

	usleep(100000);


	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(100000);		//charge consumed without connection update per connection interval
	 qTcOld = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,10,37,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);			//master
    qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,37,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave
	 qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcOld*0.00125,nSeqOld,15,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);			//master
    qTcOld += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcOld*0.00125,nSeqOld,10,15,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave
    qTcOld /= 2.0;

	//charge consumed when updating the connection interval
	 qTcNew = ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,10,37,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//master transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,37,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(1,tcNew*0.00125,nSeqNew,15,10,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//master transmission
	 qTcNew += ble_e_model_c_getChargeConnectionIntervalSamePayload(0,tcNew*0.00125,nSeqNew,10,15,LIBBPMC_STRATEGY_BUFFER_BLE_TXPOWER,-1);		//slave transmission
	 qTcNew /= 2.0;

	 //update cost
	 qUpdate = ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,1,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);
    qUpdate += ble_e_model_ce_getChargeForConnectionProcedure(0,SC_SCAN_TYPE_PERIODIC,0,tcOld*0.00125,tcNew*0.00125,BLE_E_MODE_CE_EVENT_TYPE_REPLACE_EMPTY);
	 //	LIBBPMC_DEBUG("QUpdate master+slave: %f\n",qUpdate);
	 //	LIBBPMC_DEBUG("Intervals: %d -> %d \t Charges: qTcOld = %f, qTcNew = %f, qUpdate = %f\n",tcOld, tcNew, qTcOld, qTcNew, qUpdate);
								//100 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(200000);										//50 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(350000);										//28,6 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(450000);										//22,2 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(650000);										//15,38 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(65000);										//153,8 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(6500000);										//15,38 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(6500000);										//15,38 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(65000);										//1538 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);

	usleep(165000);
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(259000);										//153,8 bps
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	clock_gettime(CLOCK_MONOTONIC,&tr.time);
	libBPMcTrafficMeasurement_add(&trm,tr);
	usleep(165000);

	printf("Traffic in class: %d\n",libBPMcTrafficMeasurement_determineClass(&trm,45));

	printf("Avg Durations:\n");
	uint32_t cnt;
	for(cnt = 0; cnt < trm.nClasses; cnt++){
		printf("\t [%d] measured: %f s\n",cnt,libBPMcTrafficMeasurement_getAvgResidenceDuration(&trm,cnt));

	}
	printf("######################[STATES]############################################\n");
	libBPMcTrafficMeasurement_printTransitionMatrix(&trm,TRANSITION_PROBAB_CLASSES);
	printf("######################[TransRate]##############################################\n");
	libBPMcTrafficMeasurement_printTransitionMatrix(&trm,TRANSITION_PROBAB_TRANSRATE);


	libBPMcMatrix distribution;
	printf("######################[intensity matrix]#############################\n");
	libBPMcTrafficMeasurement_getProbabilityMatrix(&trm,TRANSITION_PROBAB_TRANSRATE,&distribution,1,1);
	libBPMcMatrix_print(&distribution);
	libBPMcMatrix_destroy(&distribution);


	printf("######################[PREDICTED DISTRIBUTION]#############################\n");
	libBPMcTrafficAnalysis_getStateDistributionAfterTime(&trm,5.0,&distribution,1,1);
	libBPMcMatrix_print(&distribution);
	printf("Probability sum: %.3f\n",libBPMcMatrix_getSumOfElements(&distribution));
	printf("######################[PREDICTED TRAFFIC]#############################\n");

	double predictionMin = libBPMcTrafficAnalysis_getTrafficPrediction(&trm,&distribution,CLASSTRAFFIC_RELATION_MIN);
	double predictionAvg = libBPMcTrafficAnalysis_getTrafficPrediction(&trm,&distribution,CLASSTRAFFIC_RELATION_AVG);
	double predictionMax = libBPMcTrafficAnalysis_getTrafficPrediction(&trm,&distribution,CLASSTRAFFIC_RELATION_MAX);
	printf("prediction: min: %f\t avg: %f\t max: %f\n",predictionMin,predictionAvg,predictionMax);


	//double predictionAvg = libBPMcTrafficAnalysis_getTrafficPrediction(&trm,&distribution,CLASSTRAFFIC_RELATION_AVG);
	//double predictionMax = libBPMcTrafficAnalysis_getTrafficPrediction(&trm,&distribution,CLASSTRAFFIC_RELATION_MAX);
	exit(0);
	printf("_-------------------------------------------------------------------_\n\n");
	libBPMcMatrix_destroy(&distribution);

	libBPMcTrafficMeasurement_destroy(&trm);

	libBPMcMatrix a,b,c,d;

	libBPMcMatrix_init(&a,3,2);
	libBPMcMatrix_init(&b,2,3);
	libBPMcMatrix_init(&d,3,3);

	a.data[0][0] = 1;
	a.data[0][1] = 2;
	a.data[1][0] = 3;
	a.data[1][1] = 4;
	a.data[2][0] = 5;
	a.data[2][1] = 6;

	b.data[0][0] = 1;
	b.data[0][1] = 2;
	b.data[0][2] = 3;
	b.data[1][0] = 4;
	b.data[1][1] = 5;
	b.data[1][2] = 6;

	d.data[0][0] = 0.1;
	d.data[0][1] = 0.2;
	d.data[0][2] = 0.3;
	d.data[1][0] = 0.4;
	d.data[1][1] = 0.5;
	d.data[1][2] = 0.6;
	d.data[2][0] = 0.7;
	d.data[2][1] = 0.8;
	d.data[2][2] = 0.9;

	libBPMcMatrix_matMult(&a,&b,&c,1);
	libBPMcMatrix_print(&a);
	libBPMcMatrix_print(&b);
	libBPMcMatrix_print(&c);
	libBPMcMatrix_destroy(&c);
	libBPMcMatrix_matMult(&b,&a,&c,1);
	libBPMcMatrix_print(&c);
	libBPMcMatrix_destroy(&c);
	libBPMcMatrix_matExpTaylor(&d,&c,2 * (d.x),1);


	//	libBPMcMatrix_sum(&d,&d,&c,1);
//	libBPMcMatrix_multByScalar(&c,&c,10,0);
	//	libBPMcMatrix_matPow(&d,&c,3,1);
//	libBPMcMatrix_matMult(&d,&d,&c,1);
	libBPMcMatrix_print(&c);

}


#endif
