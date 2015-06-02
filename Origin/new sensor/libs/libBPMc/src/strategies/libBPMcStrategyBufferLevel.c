/*
 * libBPMcStrategyBufferLevel.c
 *
 *  Created on: 21.10.2013
 *      Author: kindt, gopp
 */



//#include "../libBPMcInterface.h"
#include "libBPMcStrategyBufferLevel.h"


void libBPMcStrategyBufferLevel_init(libBPMcInterface* interface)
{
	libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) malloc(sizeof(libBPMcStrategyBufferLevelInternals));
	BufferLevelParameter = BufferLevelInternals;

	BufferLevelCurrentTraffic = (currentTrafficBufferLevel*) malloc(sizeof(currentTrafficBufferLevel));

	interface->internalData = (void*) BufferLevelInternals;

	//init mutex
	virThread_mutex_init(&libBPMcStrategyBufferLevel_data_mutex,0);
	virThread_mutex_unlock(&libBPMcStrategyBufferLevel_data_mutex);

	//Set all callbacks needed for power management
	libBPMcInterface_setActivationCallBack(interface,libBPMStrategyBufferLevel_setActivation);
	libBPMcInterface_setAddCallBackOut(interface,libBPMcStrategyBufferLevel_addOut);
	libBPMcInterface_setSetBufferPtrCallBack(interface,libBPMcStrategyBufferLevel_setBuffer);
	libBPMcInterface_setSetStartInfoCallBack(interface, libBPMcStrategyBufferLevel_startInfo);
	libBPMcInterface_setDestroyCallBack(interface,libBPMcStrategyBufferLevel_destory);
	libBPMcInterface_setSetLatencyConstraintsCallBack(interface,libBPMcStrategyBufferLevel_setLatencyConstraints);

	//Set all callbacks needed for parameter exchange
	libBPMcInterface_setGetIdentCallBack(interface,libBPMcStrategyBufferLevel_getIdent);
	libBPMcInterface_setGetParamListCallBack(interface,libBPMcStrategyBufferLevel_getParamList);
	libBPMcInterface_setGetParamCallBack(interface,libBPMcStrategyBufferLevel_getParam);
	libBPMcInterface_setSetParamCallBack(interface,libBPMcStrategyBufferLevel_setParam);

}

void libBPMcStrategyBufferLevel_startInfo(libBPMcInterface* interface, int32_t tcInitial, double nSeqInitial)
{
	//printf("BufferLevel strategy initialized.");
	libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;
	//BufferLevelInternals->nextConnectionInterval = tcInitial;
	BufferLevelInternals->increment = 1;
	BufferLevelParameter->connectionInterval = (uint32_t)tcInitial;
	BufferLevelParameter->interface = interface;
}

void libBPMcStrategyBufferLevel_addOut(libBPMcInterface* interface, libBPMcTraffic tr)
{
	if(interface->active)
	{
		virThread_mutex_lock(&libBPMcStrategyBufferLevel_data_mutex);

		//libBPMcStrageySpeedAdjustmentInternals *SpeedAdjustmentInternals = (libBPMcStrageySpeedAdjustmentInternals*) interface->internalData;

		if(BufferLevelCurrentTraffic->status == 0)
		{
			BufferLevelCurrentTraffic->startTime = tr;
			BufferLevelCurrentTraffic->numberOfBytes = tr.nBytes;
			BufferLevelCurrentTraffic->status = 1;
		}
		if(BufferLevelCurrentTraffic->status == 1)
		{
			BufferLevelCurrentTraffic->numberOfBytes += tr.nBytes;
			BufferLevelCurrentTraffic->endTime = tr;

		}

		virThread_mutex_unlock(&libBPMcStrategyBufferLevel_data_mutex);
	}
	else
	{
		printf("not adjusting connection interval - disacitvated.\n");
	}
}


void libBPMcStrategyBufferLevel_setBuffer(libBPMcInterface* interface, libBPMcBuffer* bufIn, libBPMcBuffer* bufOut)
{
	libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;
	BufferLevelInternals->bufIn = bufIn;
	BufferLevelInternals->bufOut = bufOut;
	BufferLevelParameter->bufIn = bufIn;
	BufferLevelParameter->bufOut = bufOut;
}

void libBPMcStrategyBufferLevel_destory(libBPMcInterface* interface)
{
	libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;
	free(BufferLevelInternals);
	free(interface);
	printf("destroy\n");
}


void libBPMcStrategyBufferLevel_getIdent(libBPMcInterface* interface,char* ident)
{
	strcpy(ident, "libBPMcStrategyBufferLevel");
}

char* libBPMcStrategyBufferLevel_getParamList(libBPMcInterface* interface)
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

void libBPMcStrategyBufferLevel_setLatencyConstraints(libBPMcInterface* interface, uint32_t tcMin, uint32_t tcMax)
{
	//libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;

	BufferLevelParameter->minConInt = tcMin;
	BufferLevelParameter->maxConInt = tcMax;
}

uint8_t libBPMcStrategyBufferLevel_setParam(libBPMcInterface* interface,const char* param, const char* value)
{
	libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;

	virThread_mutex_lock(&libBPMcStrategyBufferLevel_data_mutex);

	BufferLevelParameter = (libBPMcStrategyBufferLevelInternals*) interface->internalData;


	if(strcmp(param,"minLevel")==0)
	{
		printf("setting param minLevel to %u,\n",atoi(value));
		BufferLevelParameter->minLevel = atoi(value);
	}
	else if(strcmp(param, "maxLevel")==0)
	{
		printf("setting param maxLevel to %u,\n",atoi(value));
		BufferLevelParameter->maxLevel = atoi(value);
	}
	else
	{
		return 0;		//failure
	}

	virThread_mutex_unlock(&libBPMcStrategyBufferLevel_data_mutex);

	return 1;
}

uint8_t libBPMcStrategyBufferLevel_getParam(libBPMcInterface* interface,const char* param, char* value)
{
	libBPMcStrategyBufferLevelInternals *BufferLevelInternals = (libBPMcStrategyBufferLevelInternals*) interface->internalData;

	if(value == NULL)
	{
		return 0;	//error
	}
	if(strcmp(param,"minLevel")==0)
	{
		virThread_mutex_lock(&libBPMcStrategyBufferLevel_data_mutex);
		sprintf(value, "%u",BufferLevelParameter->minLevel);
		virThread_mutex_unlock(&libBPMcStrategyBufferLevel_data_mutex);
	}
	else if(strcmp(param, "maxLevel")==0)
	{
		virThread_mutex_lock(&libBPMcStrategyBufferLevel_data_mutex);
		sprintf(value, "%u",BufferLevelParameter->maxLevel);
		virThread_mutex_unlock(&libBPMcStrategyBufferLevel_data_mutex);
	}
	else
	{
		return 0;		//failure
	}
	return 1;

}

void libBPMStrategyBufferLevel_setActivation(libBPMcInterface* interface, uint8_t active)
{
	interface->active = active;
	//start thread
	if(interface->active == active)
	{
		//start thread
		virThread_thread_create(&libBPMcStrategyBufferLevel_Thread,libBPMcStrategyBufferLevel_update_thread,NULL,"bufferLevel",500,1);
	}
	else
	{
		//destroy thread
	}
}

void* libBPMcStrategyBufferLevel_update_thread(void* param)
{
	uint32_t m_bufFillLevelByte = 0;
	uint32_t m_buffersize = LIBBPMC_BUFFER_QUEUE_BUF_LEN;
	uint8_t m_bufFillLevelPercent = 0;
	uint8_t m_bufFillLevelPercentTmp = 0;

	uint32_t m_conInterval = BufferLevelParameter->connectionInterval;
	uint32_t m_conIntervalOld = 0;

	uint16_t m_i = 0;

	double m_traffic = 0.0;

	//update period in sec
	double m_period = 0.05;
	double m_periodTmp = 0.0;

	uint8_t m_flag = 0;

	//csv output
#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
	if(csvOutputBuffervalues == 1)
	{
		FILE *fa = fopen("bufferLevel.csv", "a+");
		fprintf(fa,"fillLevel; fillLevelPercent; inputSpeed; outputSpeed\n");
		fclose(fa);
	}

	if(csvOutputTime == 1)
	{
		FILE *ga = fopen("bufferLevelTime.csv", "a+");
		fprintf(ga,"sec;ms\n");
		fclose(ga);
	}
#endif

	//while loop for strategy
	while(1)
	{
		//*******************************************************************
		//sleep for m_period if 6x connection interval is smaller
		//else sleep for 6x connection interval
		//*******************************************************************
		m_periodTmp = m_period;

		if((m_conInterval * 1.25 * 6/1000) > m_period)
		{
			m_periodTmp = (m_conInterval * 1.25 * 6/1000);
		}

		if(m_periodTmp > 1.0)
		{
			uint16_t sleepRounds_sec = (uint16_t)m_periodTmp;

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
			if(csvOutputTime == 1)
			{
				FILE *gb = fopen("bufferLevelTime.csv", "a+");
				fprintf(gb,"%u;",sleepRounds_sec);
				fclose(gb);
			}
#endif
			for(m_i = 0; m_i < sleepRounds_sec; m_i++)
			{
				virThread_delay_ms(1000);
			}

			uint16_t sleepRounds_msec = (uint16_t)((m_periodTmp - sleepRounds_sec)*1000);

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
			if(csvOutputTime == 1)
			{
				FILE *gc = fopen("bufferLevelTime.csv", "a+");
				fprintf(gc,"%u\n",sleepRounds_msec);
				fclose(gc);
			}
#endif
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

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
			if(csvOutputTime == 1)
			{
				FILE *gd = fopen("bufferLevelTime.csv", "a+");
				fprintf(gd,"0;%u\n",sleepRounds_msec);
				fclose(gd);
			}
#endif
			for(m_i = 0; m_i < sleepRounds_msec; m_i++)
			{
				virThread_delay_ms(10);
			}
		}

		//***********************************************************************

		//get buffer fill level in byte
		m_bufFillLevelByte = libBPMcBuffer_getBytesUsed(BufferLevelParameter->bufIn);
		//calculate fill level in percent
		m_bufFillLevelPercent = m_bufFillLevelByte * 100 / m_buffersize;

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
		if(csvOutputBuffervalues == 1)
		{
			FILE *fh = fopen("bufferLevel.csv", "a+");
			fprintf(fh,"%u;%u;%lf;%lf;\n",m_bufFillLevelByte,m_bufFillLevelPercent,m_traffic,20.0 * numOfIntervals * 1000.0/(m_conInterval * 1.25));
			fclose(fh);
		}
#endif
		//**********************************************************************
		//if the fill level in percent is smaller than the minimum fill level and the flag is 0
		//then the speed gets updated
		//**********************************************************************
		if((m_bufFillLevelPercent < BufferLevelParameter->minLevel) && (m_flag == 0))
		{
			printf("decrease speed\n");

			//get the current traffic
			m_traffic = getCurrentTraffic_bufferLevel();

			if(m_traffic > 0)
			{
				//set flag to 1 so the speed has been updated
				m_flag = 1;

				m_conIntervalOld = m_conInterval;

				//calculate the new connection interval
				m_conInterval = (uint32_t)(1000.0 * numOfIntervals * 20.0 / (1.25 * m_traffic));

				printf("Traffic: %f - Interval: old: %d -> new: %d\n",m_traffic,m_conIntervalOld, m_conInterval);

				//check if the new calculated value is in range of min and max connection interval
				if(m_conInterval < BufferLevelParameter->minConInt)
				{
					m_conInterval = BufferLevelParameter->minConInt;
				}
				if(m_conInterval > BufferLevelParameter->maxConInt)
				{
					m_conInterval = BufferLevelParameter->maxConInt;
				}

				//if the new connection interval is higher than the old one, then update
				if(m_conInterval > m_conIntervalOld)
				{
					//update connection interval
					libBPMcInterface_callSetConIntCallBack(BufferLevelParameter->interface,m_conInterval,10);
				}

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
				if(csvOutputBuffervalues == 1)
				{
					FILE *fc = fopen("bufferLevel.csv", "a+");
					fprintf(fc,"%u;%u;%lf;%lf;update decrease\n",m_bufFillLevelByte,m_bufFillLevelPercent,m_traffic,20.0 * numOfIntervals * 1000.0/(m_conInterval * 1.25));
					fclose(fc);
				}
#endif
			}
		}
		//********************************************************************************


		//if the fill level of the buffer lies between the min and max value then the flag is set back to 0
		//this means that the strategy is now allowed to update the speed again
		if((m_bufFillLevelPercent > BufferLevelParameter->minLevel)&&(m_bufFillLevelPercent < BufferLevelParameter->maxLevel))
		{
			//set flag back to 0
			m_flag = 0;
			//set the emergency fill level of the buffer back to the maximum value
			m_bufFillLevelPercentTmp = BufferLevelParameter->maxLevel;
		}


		//*********************************************************************************
		//if the buffer level reaches the maximum level and the flag is 0 then the speed gets updated
		//the speed also gets updated when the temporary fill level reaches it limit (something like emergency update)
		//and the flag is 1, so this emergency plan can only run once
		//*********************************************************************************
		if(((m_bufFillLevelPercent > BufferLevelParameter->maxLevel) && (m_flag == 0)) || ((m_bufFillLevelPercent > (m_bufFillLevelPercentTmp)) && (m_flag == 1)))
		{
			//get current traffic
			m_traffic = getCurrentTraffic_bufferLevel();

			if(m_traffic > 0)
			{
				//increase flag
				m_flag++;

				m_conIntervalOld = m_conInterval;

				//calculate new connection interval
				m_conInterval = (uint32_t)(1000.0 * numOfIntervals * 20.0 / (1.25 * (m_traffic + m_bufFillLevelByte)));

				//calculate the emergency fill level
				m_bufFillLevelPercentTmp = (m_bufFillLevelByte + m_traffic / (1000 / m_conInterval) * 6) * 100 / m_buffersize;

				//if emergency level is higher than 90 percent then set it to 90 percent to have a chance to update before buffer is full
				if(m_bufFillLevelPercentTmp > 90)
				{
					m_bufFillLevelPercentTmp = 90;
				}

				//check if the calculated connection interval is in range
				if(m_conInterval < BufferLevelParameter->minConInt)
				{
					m_conInterval = BufferLevelParameter->minConInt;
				}
				if(m_conInterval > BufferLevelParameter->maxConInt)
				{
					m_conInterval = BufferLevelParameter->maxConInt;
				}

				//update connection interval
				libBPMcInterface_callSetConIntCallBack(BufferLevelParameter->interface,m_conInterval,10);

#if SBLE_PLATTFORM_ARCHITECTURE==SBLE_PLATTFORM_ARCHITECTURE_POSIX
				if(csvOutputBuffervalues == 1)
				{
					FILE *fb = fopen("bufferLevel.csv", "a+");
					fprintf(fb,"%u;%u;%lf;%lf;uptdate increase\n",m_bufFillLevelByte,m_bufFillLevelPercent,m_traffic,20.0 * numOfIntervals * 1000.0/(m_conInterval * 1.25));
					fclose(fb);
				}
#endif
			}
		}

		//************************************************************************************************
	}
}

//calculate current traffic
double getCurrentTraffic_bufferLevel(void)
{
	virThread_mutex_lock(&libBPMcStrategyBufferLevel_data_mutex);

	double m_currentTraffic = 0.0;

	//if status one means that there are at least a start and an end time to calculate the current speed
	if(BufferLevelCurrentTraffic->status == 1)
	{
		//calculates the time difference between start and end
		double diffBetweenElements = libBPMc_Traffic_getTimeDifference(BufferLevelCurrentTraffic->startTime.time,BufferLevelCurrentTraffic->endTime.time);

		if(BufferLevelCurrentTraffic->numberOfBytes == 0)
		{
			return -1.0;
		}

		//calculates current traffic
		m_currentTraffic = BufferLevelCurrentTraffic->numberOfBytes / diffBetweenElements;

		//reset of the values
		BufferLevelCurrentTraffic->numberOfBytes = 0;
		BufferLevelCurrentTraffic->status = 0;


		//if current traffic is higher than the max speed of the bluetooth, then it is set to the maximum speed of the bluetooth
		if((uint32_t)m_currentTraffic > (uint32_t)(20.0 * numOfIntervals * 1000.0 / 7.5))
		{
			m_currentTraffic = (20.0 * numOfIntervals * 1000.0 / 7.5);
		}
	}
	else
	{
		m_currentTraffic = -1.0;
	}

	virThread_mutex_unlock(&libBPMcStrategyBufferLevel_data_mutex);

	return m_currentTraffic;
}
