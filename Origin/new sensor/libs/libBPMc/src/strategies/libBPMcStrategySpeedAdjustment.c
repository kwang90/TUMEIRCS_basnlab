/*
 * libBPMcStrategySpeedAdjustment.c
 *
 *  Created on: 21.10.2013
 *      Author: kindt, gopp
 */



//#include "../libBPMcInterface.h"
#include "libBPMcStrategySpeedAdjustment.h"


void libBPMcStrategySpeedAdjustment_init(libBPMcInterface* interface)
{
	libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) malloc(sizeof(libBPMcStrageySpeedAdjustmentInternals));
	SpeedAdjustmentCurrentTraffic = (currentTraffic*) malloc(sizeof(currentTraffic));
	SpeedAdjustmentParameter = SpeedAdjustmentInternals;

	interface->internalData = (void*) SpeedAdjustmentInternals;

	//init mutex
	virThread_mutex_init(&libBPMcStrategySpeedAdjustment_data_mutex,0);
	virThread_mutex_unlock(&libBPMcStrategySpeedAdjustment_data_mutex);

	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMStrategySpeedAdjustment_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategySpeedAdjustment_addOut);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategySpeedAdjustment_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategySpeedAdjustment_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategySpeedAdjustment_destory);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategySpeedAdjustment_setLatencyConstraints);

	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategySpeedAdjustment_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategySpeedAdjustment_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategySpeedAdjustment_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategySpeedAdjustment_setParam);

}

void libBPMcStrategySpeedAdjustment_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial)
{

	libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;
	SpeedAdjustmentInternals->nextConnectionInterval = tcInitial;

	SpeedAdjustmentInternals->increment = 1;

	SpeedAdjustmentParameter->interface = interface;

	SpeedAdjustmentCurrentTraffic->status = 0;
}

void libBPMcStrategySpeedAdjustment_addOut(libBPMcInterface* interface, libBPMcTraffic tr)
{
	if(interface->active)
	{
		virThread_mutex_lock(&libBPMcStrategySpeedAdjustment_data_mutex);

		libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;

		if(SpeedAdjustmentCurrentTraffic->status == 0)
		{
			SpeedAdjustmentCurrentTraffic->startTime = tr;
			SpeedAdjustmentCurrentTraffic->numberOfBytes = tr.nBytes;
			SpeedAdjustmentCurrentTraffic->status = 1;
		}
		if(SpeedAdjustmentCurrentTraffic->status == 1)
		{
			SpeedAdjustmentCurrentTraffic->numberOfBytes += tr.nBytes;
			SpeedAdjustmentCurrentTraffic->endTime = tr;

		}

		virThread_mutex_unlock(&libBPMcStrategySpeedAdjustment_data_mutex);
	}
	else
	{
		printf("not adjusting connection interval - disacitvated.\n");
	}
}


void libBPMcStrategySpeedAdjustment_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut)
{
	libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;
	SpeedAdjustmentInternals->bufIn = bufIn;
	SpeedAdjustmentInternals->bufOut = bufOut;
	SpeedAdjustmentParameter->bufIn = bufIn;
	SpeedAdjustmentParameter->bufOut = bufOut;
}

void libBPMcStrategySpeedAdjustment_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax)
{
	//libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;

	SpeedAdjustmentParameter->minConInt = tcMin;
	SpeedAdjustmentParameter->maxConInt = tcMax;
}

void libBPMcStrategySpeedAdjustment_destory(libBPMcInterface* interface)
{
	libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;
	free(SpeedAdjustmentInternals);
	free(interface);
	printf("destroy\n");
}


void libBPMcStrategySpeedAdjustment_getIdent(libBPMcInterface* interface,char* ident)
{
	strcpy(ident, "libBPMcStrategySpeedAdjustment");
}

char* libBPMcStrategySpeedAdjustment_getParamList(libBPMcInterface* interface)
{

	char* paramList = (char*) malloc(1000);
	strcpy(paramList,"");
	char* rv;
	char lineBuf[256];
	sprintf(lineBuf,"%s|%s|%d|%d\n","increment","d",1,500);
	strcat(paramList, lineBuf);
	sprintf(lineBuf,"%s|%s|%d|%d\n","dummyParam","f2",0,10);
	strcat(paramList, lineBuf);
	rv = strdup(paramList);
	free(paramList);
	return rv;
}

uint8_t libBPMcStrategySpeedAdjustment_setParam(libBPMcInterface* interface,const char* param, const char* value)
{
	libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;

	virThread_mutex_lock(&libBPMcStrategySpeedAdjustment_data_mutex);

	SpeedAdjustmentParameter = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;


	if(strcmp(param,"updateInterval")==0)
	{
		printf("setting param updateInterval to %lf,\n",atof(value));
		SpeedAdjustmentParameter->updateInterval = atof(value);
	}
	else if(strcmp(param, "dummyParameter")==0)
	{
						//do something fancy
	}
	else
	{
		return 0;		//failure
	}

	virThread_mutex_unlock(&libBPMcStrategySpeedAdjustment_data_mutex);

	return 1;
}

uint8_t libBPMcStrategySpeedAdjustment_getParam(libBPMcInterface* interface,const char* param, char* value)
{
	libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;

	if(value == NULL)
	{
		return 0;	//error
	}
	if(strcmp(param,"updateInterval")==0)
	{
		virThread_mutex_lock(&libBPMcStrategySpeedAdjustment_data_mutex);
		sprintf(value, "%lf",SpeedAdjustmentParameter->updateInterval);
		virThread_mutex_unlock(&libBPMcStrategySpeedAdjustment_data_mutex);
	}
	else if(strcmp(param, "dummyParam")==0)
	{
		sprintf(value, "%f",0.2345);		//dummyparam allways has value 0.2345
	}
	else
	{
		return 0;		//failure
	}
	return 1;

}

void libBPMStrategySpeedAdjustment_setActivation(libBPMcInterface* interface, uint8_t active)
{
	interface->active = active;

	if(interface->active == active)
	{
		//start thread
		virThread_thread_create(&libBPMcStrategySpeedAdjustment_Thread,libBPMcStrategySpeedAdjustment_update_thread,NULL,"speedAdjustment",500,1);

	}
	else
	{
		//destroy thread
	}
}

void* libBPMcStrategySpeedAdjustment_update_thread(void* param)
{
	param = NULL;
	uint32_t m_updateInterval;

	//period to set
	double m_period = SpeedAdjustmentParameter->updateInterval;

	//tmp period
	double m_periodTmp = 0.0;

	double m_trafficAverage = 0;

	uint16_t m_conInterval = 0;
	uint16_t m_conIntervalOld = 3200;

	uint32_t m_buffsize = 0;
	uint32_t m_buffsizeOld = 0;

	double m_outspeed = 0;

	uint8_t m_firstRun = 0;

	uint16_t m_i = 0;

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
	FILE *fa = fopen("speedAdjustment.csv", "a+");
	fprintf(fa,"inputSpeed(u);buffersize;outputSpeed\n");
	fclose(fa);
#endif

	while(1)
	{
		//first run, first update after 10 seconds
		if(m_firstRun == 0)
		{
			m_periodTmp = 10.0;
			m_firstRun = 1;
		}
		else
		{
			m_periodTmp = m_period;
		}

		//*****************************************************************************
		//if period smaller than 6x connection interval, then update interval is 6x connection interval
		//*****************************************************************************
		if((m_conInterval * 1.25 * 6/1000) > m_period)
		{
			m_periodTmp = (m_conInterval * 1.25 * 6/1000);
		}

		if(m_periodTmp > 1.0)
		{
			uint16_t sleepRounds_sec = (uint16_t)m_periodTmp;

			for(m_i = 0; m_i < sleepRounds_sec; m_i++)
			{
				virThread_delay_ms(1000);
			}

			uint16_t sleepRounds_msec = (uint16_t)((m_periodTmp - sleepRounds_sec)*1000);

			//10ms
			for(m_i = 0; m_i < sleepRounds_msec/10; m_i++)
			{
				virThread_delay_ms(10);
			}
		}
		else
		{
			//calculate for 10ms steps
			uint16_t sleepRounds_msec = (uint16_t)(m_periodTmp * 100);

			for(m_i = 0; m_i < sleepRounds_msec; m_i++)
			{
				virThread_delay_ms(10);
			}
		}
		//**************************************************************************************************

		//get current traffic
		m_trafficAverage = getCurrentTraffic();

		//get buffer fill level
		m_buffsize = libBPMcBuffer_getBytesUsed(SpeedAdjustmentParameter->bufIn);

		//calculate connection interval
		//attention the calculation is depending on the sending speed which is set in the sble_serial_calc_speed_adjustment function
		m_conInterval = sble_serial_calc_speed_adjustment(m_trafficAverage, m_buffsize,m_periodTmp);

		//check if connection interval is in range
		if(m_conInterval < SpeedAdjustmentParameter->minConInt)
		{
			m_conInterval = SpeedAdjustmentParameter->minConInt;
		}

		if(m_conInterval > SpeedAdjustmentParameter->maxConInt)
		{
			m_conInterval = SpeedAdjustmentParameter->maxConInt;
		}

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		FILE *f = fopen("speedAdjustment.csv", "a+");
		fprintf(f,"%lf;%u;",m_trafficAverage,m_buffsize);
		fclose(f);
#endif

		//if the old connection interval is higher than the new one, then the speed can be updated
		//the speed then gets increased
		if(m_conIntervalOld > m_conInterval)
		{
			libBPMcInterface_callSetConIntCallBack(SpeedAdjustmentParameter->interface,m_conInterval,5);
			m_conIntervalOld = m_conInterval;
			m_buffsizeOld = m_buffsize;
		}
		else
		{
			//else check if the buffer level is smaller than before and is under a minimum of 100 byte
			//then decreasing the speed is allowed
			if((m_conIntervalOld < m_conInterval) && (m_buffsizeOld >= m_buffsize) && (m_buffsize < 100))
			{
				libBPMcInterface_callSetConIntCallBack(SpeedAdjustmentParameter->interface,m_conInterval,5);
				m_conIntervalOld = m_conInterval;
				m_buffsizeOld = m_buffsize;
			}
		}

		//calculate outputspeed for graphic
		m_outspeed = numOfIntervals * 20.0 * 1000.0/(m_conInterval * 1.25); //for fast ble

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		FILE *fi = fopen("speedAdjustment.csv", "a+");
		fprintf(fi,"%lf\n", m_outspeed);
		fclose(fi);
#endif

	}
	return NULL;

}

//calculate current traffic
double getCurrentTraffic(void)
{

	virThread_mutex_lock(&libBPMcStrategySpeedAdjustment_data_mutex);

	double m_currentTraffic = 0.0;

	//check if status is one means that there is a start and an end time
	if(SpeedAdjustmentCurrentTraffic->status == 1)
	{
		double diffBetweenElements = libBPMc_Traffic_getTimeDifference(SpeedAdjustmentCurrentTraffic->startTime.time,SpeedAdjustmentCurrentTraffic->endTime.time);
		m_currentTraffic = SpeedAdjustmentCurrentTraffic->numberOfBytes / diffBetweenElements;
		SpeedAdjustmentCurrentTraffic->numberOfBytes = 0;
		SpeedAdjustmentCurrentTraffic->status = 0;
	}
	else
	{
		//else set the traffic to its maximum
		m_currentTraffic = 100000.0;
	}

	virThread_mutex_unlock(&libBPMcStrategySpeedAdjustment_data_mutex);

	return m_currentTraffic;


}
