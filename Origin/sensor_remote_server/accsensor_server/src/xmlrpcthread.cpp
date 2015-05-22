/*
 * xmlrpcthread.cpp
 *
 *  Created on: 20.06.2013
 *      Author: kindt
 */

#include "xmlrpcthread.h"
#include <iostream>
#include <time.h>
#include <string.h>
#include <xmlrpc-c/client_simple.hpp>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <inttypes.h>
#include "rcsActivity.h"

#define MEDCPS_XMLRPC_SENSOR_TYPE 1
#define MEDCPS_XMLRPC_SENSOR_ID 1


using namespace std;

	xmlRpcThread::xmlRpcThread():
	sema(1)
	{
		success = false;
		serverUrl = NULL;
	};

	xmlRpcThread::~xmlRpcThread(){
		if(serverUrl != NULL){
			free(serverUrl);
		}
		serverUrl = NULL;

	}

	void xmlRpcThread::setServerUrl(char* url){
		if(serverUrl != NULL){
			free(serverUrl);
		}
		this->serverUrl = strdup(url);
	}



	bool xmlRpcThread::isSuccess(){
		bool rv;
		sema.acquire();
			rv = success;
		sema.release();
		return rv;
	}



	void xmlRpcThread::setNextActivity(rcsActivity::activity_type activity){
		nextActivity.activity = activity;
		time(&(nextActivity.timestamp));

	}

	void xmlRpcThread::execXmlRpcCall(){
		string const method = "MedicalCPSInterface.activitySensorCall";
		sema.acquire();
		if(serverUrl == NULL){
			cerr<<"serverUrl not set."<<endl;
			sema.release();
			success = false;
			this->thread()->quit();

			return;
		}
	try{
		xmlrpc_c::clientSimple xmlRpcCli;
		xmlrpc_c::value result;
		uint32_t timestamp_lo, timestamp_hi;

		//split up time to 32-bit-integer-packets
		timestamp_lo = nextActivity.timestamp &0xFFFFFFFF;
		timestamp_hi = nextActivity.timestamp & (0xFFFFFFFF<<16);
//		cout<<"Timestamp: "<<nextActivity.timestamp<<endl;

		//do XML-RPC call
		xmlRpcCli.call(serverUrl,method,"iiiiii", &result, MEDCPS_XMLRPC_SENSOR_TYPE, MEDCPS_XMLRPC_SENSOR_ID, timestamp_lo, timestamp_hi,nextActivity.activity,0);

		//		cout<<"result: "<<xmlrpc_c::value_boolean(result)<<endl;
		success = xmlrpc_c::value_boolean(result);

		}catch(exception const &e){
			cerr<<"XML-RPC error "<<e.what()<<endl;
			success = false;
		}catch(...){
			cerr<<"unknown error."<<endl;
			success = false;
		}

		sema.release();
	//	cout<<"XML RPC call thread finished."<<endl;
		this->thread()->quit();
	};





