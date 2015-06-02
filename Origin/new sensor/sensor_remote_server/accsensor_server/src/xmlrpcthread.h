/*
 * xmlrpcthread.h - XML RPC requeast manager for detected activities
 *
 *  Created on: 20.06.2013
 *  Author: Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 *
 * This class is suppsed to run in its own QThread. It will issue a MedCPS XML-RPC-Request and then finish.
 * The typical sequence how to use it is:
 * - Create object
 * - Assign object to a QThread with the moveToThread()-method
 * - call setServerUrl() to set an XMl-RPC-server to use
 * - set an Activity using setNextActivity(). The obejct will store and timestamp the activity.
 * - run execXmlRpcCall() in a new thread (by starting the qThread previously assigned). The XML-RPC-call will be executed
 * - Wait for the thread to terminate and evaluate the success-variable.
 *
 * Please note: functions as setServerUrl() are normally handled by the main thread, whereas execXmlRpcCall() is supposed to be ru
 * in its own thread.
 */

#ifndef XMLRPCTHREAD_H_
#define XMLRPCTHREAD_H_

#include <QObject>;
#include <QThread>;
#include <QSemaphore>;

#include "rcsActivity.h"

using namespace std;

class xmlRpcThread: public QObject{
	Q_OBJECT

private:
	QSemaphore sema;			// a semaphore for thread-savety. Whenever a method changes a value, it should aquire this semahore before
	bool success;				//status of  the last XML-RPC-call. True if successful
	rcsActivity nextActivity;	//the next avtivity to be transmitted via XML-RPC
	char* serverUrl;			//the URL of the server including the port. for example: localhost:50000
public:
	xmlRpcThread();
	~xmlRpcThread();

	//Sets server URL to connect to including the port. A valid URL would be localhost:50000 for example
	void setServerUrl(char* url);

	// returns true, if the previous XML-RPC call was executed sucessfully and false, otherwise.
	bool isSuccess();

	//set the next activity to be tranmitted via XML-RPC to the server
	void setNextActivity(rcsActivity::activity_type activity);

	public Q_SLOTS:

	//Perfom the XML-RPC-call. Make sure to call setServerUrl() before using this function and setnextActivity before every use of this function
	void execXmlRpcCall();


};


#endif /* XMLRPCTHREAD_H_ */
