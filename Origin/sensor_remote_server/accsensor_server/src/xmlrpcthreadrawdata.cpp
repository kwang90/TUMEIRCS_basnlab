/*
 * xmlrpcthread.cpp
 *
 *  Created on: 20.06.2013
 *      Author: kindt
 */
#include "xmlrpcthreadrawdata.h"
#include <iostream>
#include <time.h>
#include <string.h>
#include <xmlrpc-c/client_simple.hpp>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <inttypes.h>

#define MEDCPS_XMLRPC_SENSOR_TYPE 1
#define MEDCPS_XMLRPC_SENSOR_ID 1


using namespace std;

	xmlRpcThreadRawData::xmlRpcThreadRawData():
	sema(1)
	{
		success = false;
		serverUrl = NULL;
		//waitSema.acquire();
	};

	xmlRpcThreadRawData::~xmlRpcThreadRawData(){
		if(serverUrl != NULL){
			free(serverUrl);
		}
		serverUrl = NULL;

	}

	void xmlRpcThreadRawData::setServerUrl(char* url){
		if(serverUrl != NULL){
			free(serverUrl);
		}
		this->serverUrl = strdup(url);
	}



	bool xmlRpcThreadRawData::isSuccess(){
		bool rv;

		sema.acquire();
			rv = success;
		sema.release();

		return rv;
	}



	void xmlRpcThreadRawData::setNextAcceleration(int16_t nextAccX, int16_t nextAccY, int16_t nextAccZ){
		sema.acquire();
		this->nextAccX = nextAccX;
		this->nextAccY = nextAccY;
		this->nextAccZ = nextAccZ;
 		time(&(nextTimestamp));
		sema.release();

		waitSema.release();
	}

	void xmlRpcThreadRawData::execXmlRpcCall(){
		string const method = "MedicalCPSInterface.setAcceleration";
		sema.acquire();
		if(serverUrl == NULL){
			cerr<<"serverUrl not set."<<endl;
			sema.release();
			success = false;
			this->thread()->quit();

			return;
		}
		sema.release();
		xmlrpc_c::clientSimple xmlRpcCli;
		xmlrpc_c::value* result;
		uint16_t timestamp_lo, timestamp_hi;
		int16_t sendAccX, sendAccY,sendAccZ;
		while(1){

		//	cout<<"Waiting for Data..."<<endl;
			waitSema.acquire();
		//	cout<<"sending out RPC call..."<<endl;
			//split up time to 32-bit-integer-packets
			timestamp_lo = nextTimestamp &0xFFFFFFFF;
			timestamp_hi = nextTimestamp & (0xFFFFFFFF<<16);
//			cout<<"Timestamp: "<<nextActivity.timestamp<<endl;

			try{
			//do XML-RPC call
			sema.acquire();
			sendAccX = nextAccX;
			sendAccY = nextAccY;
			sendAccZ = nextAccZ;
			sema.release();
			result = new xmlrpc_c::value;

			xmlRpcCli.call(serverUrl,method,"iiiiiii", result, MEDCPS_XMLRPC_SENSOR_ID, timestamp_lo, timestamp_hi,sendAccX,sendAccY,sendAccZ,0);
		//	cout<<"result: "<<xmlrpc_c::value_boolean(*result)<<endl;
			if(xmlrpc_c::value_boolean(*result)){
				success = true;
			}else{
				success = false;
			}
			delete result;
			}catch(exception const &e){
				cerr<<"XML-RPC error "<<e.what()<<endl;
				success = false;

			}catch(...){
				cerr<<"unknown error."<<endl;
				success = false;

			}
			sema.release();
			Q_EMIT xmlRawCallFinished();
			waitSema.acquire();
		}
	//	cout<<"XML RPC call thread finished."<<endl;

		this->thread()->quit();
	};



	void xmlRpcThreadRawData::sendOut(){

		waitSema.release();
	}





