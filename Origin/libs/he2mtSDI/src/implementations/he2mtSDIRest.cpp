/*
 * he2mtSDIRest.cpp
 *
 *  Created on: 29.04.2014
 *      Author: kindt
 */

#include "he2mtSDIRest.hpp"
#include "../he2mtDebug.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <jsoncpp/json/json.h>

#define HE2MT_SDI_REST_PARAM_DEFAULT_SERVERURL "http://localhost:8000/api/"

he2mtSDIRest::he2mtSDIRest(){
	lastErrNo = HE2MT_SDI_ERROR_SUCCESS;
	proxy = NULL;
	authorized = false;
	strcpy(serverURL, HE2MT_SDI_REST_PARAM_DEFAULT_SERVERURL);
	printf("Init\n");
}

void he2mtSDIRest::init(){
	printf("url: %s\n",serverURL);
	proxy = rest_proxy_new(serverURL,FALSE);
	authorized = false;

}


bool he2mtSDIRest::authorize(char* applicationID, char* applicationSecret, char* username, char* password){

	authorized = false;
	uint32_t len;
	const gchar* payload;
	RestProxyCall* call;
	Json::Value jValue, jRoot;
	Json::Reader jReader;
	HE2MT_DEBUG("Authorizing against REST server...");
	call = rest_proxy_new_call(proxy);
	rest_proxy_call_set_method(call,"POST");
	rest_proxy_call_set_function(call,"oauth2/token/");
	rest_proxy_call_add_params(call,"client_id",applicationID,"client_secret",applicationSecret,"grant_type","password","username",username,"password",password,NULL);
	rest_proxy_call_sync(call,&error);


	if(error == NULL){
		lastErrNo = HE2MT_SDI_ERROR_SUCCESS;
		len = rest_proxy_call_get_payload_length(call);
		payload = rest_proxy_call_get_payload(call);
		if(jReader.parse(payload,jRoot)){
			if(jRoot["access_token"].isString()){
			strcpy(oAuthToken,jRoot["access_token"].asCString());
			authorized = true;
			HE2MT_DEBUG("Authorization succesful. OAuth token is %s",oAuthToken);
			g_object_unref(call);
			return true;
			}else{
				HE2MT_DEBUG("Authorization failed: Invalid credentials or invalid REST answer - no token received.");
				error = NULL;
				g_object_unref(call);
				return false;
			}
		}else{
			HE2MT_DEBUG("Authorization failed: could not parse JSON.");
			error = NULL;
			g_object_unref(call);
			return false;
		};
	}else{
		HE2MT_DEBUG("Authorization failed: REST call failure. glib/glibrest error: %s",error->message);
		lastErrNo = HE2MT_SDI_ERROR_AUTH_FAILED;

		g_object_unref(call);
		error = NULL;
		return false;
	};
	error = NULL;

}

bool he2mtSDIRest::setSensorData(dataslot_t dataSlot, char* sensorAddress ,char* timeStampSensor, double samplingPeriod, uint32_t nValues, void* valueList){
	setlocale(LC_NUMERIC, "en_US.UTF-8");
	uint32_t len;
	const gchar* payload;
	if(!authorized){
		HE2MT_DEBUG("Not authorized yet - call authorize() before using this function!");
		return false;
	}
	RestProxyCall* call;
	static char tmpStr[256];
	uint32_t cnt;
	static char* tmpStrDyn = NULL;

	call = rest_proxy_new_call(proxy);
	rest_proxy_call_set_method(call,"POST");

//	rest_proxy_call_add_params(call,"format","json","post[title]","setSensorData","post[body]","test",NULL);
	rest_proxy_call_add_params(call,"generated_at",timeStampSensor,"ble_adress",sensorAddress,NULL);

	switch(dataSlot){
		case HE2MT_SDI_DATA_SLOT_ACTIVITY:
			rest_proxy_call_set_function(call,"activity/");
			break;
		case HE2MT_SDI_DATA_SLOT_RAWACC:
			rest_proxy_call_set_function(call,"activity_raw_data/");
		//	rest_proxy_call_add_params(call,"rawAcc", NULL);

			break;
		default:
			break;
	};
	sprintf(tmpStr,"Bearer %s",oAuthToken);
	rest_proxy_call_add_header(call,"Authorization",tmpStr);
	sprintf(tmpStr,"%d",nValues);
	rest_proxy_call_add_params(call,"nvalues", tmpStr,NULL);
	sprintf(tmpStr,"%f",samplingPeriod);
	rest_proxy_call_add_params(call,"sampling_period", tmpStr,NULL);



	switch(dataSlot){

	case HE2MT_SDI_DATA_SLOT_RAWACC:
		printf("He2mt raw data transaction\n");
		if(nValues > 0){

			//2do: Fixme - to few bytes alloc'ed
			tmpStrDyn = (char*) malloc((6*6*nValues + 10000)* sizeof(char));
			if(tmpStrDyn == NULL){
				HE2MT_DEBUG("malloc() failed!");
				exit(1);
			}
			strcpy(tmpStrDyn,"[");
			for(cnt = 0; cnt < (nValues - 1); cnt++){
				sprintf(tmpStr,"[%.2f,",((float*)valueList)[3*cnt]);
				strcat(tmpStrDyn,tmpStr);
				sprintf(tmpStr,"%.2f,",((float*)valueList)[3*cnt + 1]);
				strcat(tmpStrDyn,tmpStr);
				sprintf(tmpStr,"%.2f],",((float*)valueList)[3*cnt + 2]);
				strcat(tmpStrDyn,tmpStr);
			}
			sprintf(tmpStr,"[%.2f,",((float*)valueList)[3*(nValues -1)]);
			strcat(tmpStrDyn,tmpStr);
			sprintf(tmpStr,"%.2f,",((float*)valueList)[3*(nValues - 1)+ 1]);
			strcat(tmpStrDyn,tmpStr);
			sprintf(tmpStr,"%.2f]]",((float*)valueList)[3*(nValues - 1) + 2]);
			strcat(tmpStrDyn,tmpStr);
			rest_proxy_call_add_params(call,"sensor_values",tmpStrDyn,NULL);
			free(tmpStrDyn);
		}
		break;


	case HE2MT_SDI_DATA_SLOT_ACTIVITY:

		sprintf(tmpStr,"%d",*((uint32_t*)valueList));
		rest_proxy_call_add_params(call,"status",tmpStr,NULL);
		break;

	default:
		break;

	}


	rest_proxy_call_sync(call,&error);
	len = rest_proxy_call_get_payload_length(call);
	payload = rest_proxy_call_get_payload(call);
	write(1,payload,len);
	if(error == NULL){
			HE2MT_DEBUG("Data transmission successful.");
			lastErrNo = HE2MT_SDI_ERROR_SUCCESS;
			g_object_unref(call);
			return true;
	}else{
		HE2MT_DEBUG("Data transmission failed. Error: %s",error->message);
		lastErrNo = HE2MT_SDI_ERROR_COMMUNICATION_FAILURE;
		g_object_unref(call);
		g_error_free(error);
		error = NULL;
		return false;
	};
}


bool he2mtSDIRest::disconnect(){
	g_object_unref(proxy);

	proxy = NULL;

	authorized = false;

}


he2mtSDI::errorcode_t he2mtSDIRest::getLastError(){
	return lastErrNo;
}

void* he2mtSDIRest::getApplicationErrorInfo(){
	return NULL;
}

void he2mtSDIRest::getIdent(char* ident){
	strcpy(ident, "He2MT REST Interface");
}

bool he2mtSDIRest::setParam(const char* param, const char* value){
	if(strcmp(param,"serverURL")==0){
		strcpy(this->serverURL,value);
		printf("setting Server URL to %s\n",value);
	}else{
		HE2MT_DEBUG("Unknown parameter: %s",param);
		return false;
	}
	return true;
}

bool he2mtSDIRest::getParam(const char* param, char* value){
	if(value == NULL){
		HE2MT_DEBUG("value must not be NULL. It must point to a valid buffer, having at least 256 bytes of lenght.");
		return false;
	}

	if(strcmp(param,"serverURL")==0){
		strcpy(value,serverURL);
	}else if(strcmp(param, "vendor")==0){
		sprintf(value, "%s","RCS, TUM");
		HE2MT_DEBUG("Unknown parameter: %s",param);
		return false;
	}
	return true;

}

char* he2mtSDIRest::getParamList(){
	char* paramList = (char*) malloc(1000);
	strcpy(paramList,"");
	char* rv;
	char lineBuf[256];
	sprintf(lineBuf,"%s|%s|%d|%d\n","serverURL","s",0,1000);
	strcat(paramList, lineBuf);
	rv = strdup(paramList);
	free(paramList);
	return rv;
}

