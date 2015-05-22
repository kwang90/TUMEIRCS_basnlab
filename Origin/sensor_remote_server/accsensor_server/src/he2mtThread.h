/*
 * he2mtThread.h - HE2mT requeast manager for detected activities
 *
 *  2013, 2014, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 *
 * This class is suppsed to run in its own QThread. It will issue a HE2MT SDI-Request and then finish.
 * The typical sequence how to use it is:
 * - Create object
 * - Assign object to a QThread with the moveToThread()-method
 * - set activity to send using setNectAcceleration(). The obejct will store and timestamp the activity.
 * - run execHe2mtTransaction() in a new thread (by starting the qThread previously assigned). The HE2mT-Call will be executed
 * - Wait for the thread to terminate and evaluate the success-variable.
 *
 * Please note: functions as setServerUrl() are normally handled by the main thread, whereas execXmlRpcCall() is supposed to be run
 * in its own thread.
 */

#ifndef HE2MT_THREAD_H_
#define HE2MT_THREAD_H_


#include <inttypes.h>
#include <implementations/he2mtSDIRest.hpp>
#include <QObject>;
#include <QThread>;
#include <QSemaphore>;
#include "rcsActivity.h"
#include <vector>
#include <time.h>
using namespace std;

class he2mtThread: public QObject{
	Q_OBJECT

private:
	QSemaphore sema;			// a semaphore for thread-savety. Whenever a method changes a value, it should aquire this semahore before
	QSemaphore waitSema;		//semaphore the XMLRPC-streaming-thread waits for
	bool success;				//status of  the last XML-RPC-call. True if successful
	float* accValues;
	rcsActivity::activity_type nextActivity;
	time_t nextTimestampActivity;			//time the activity was detected in seconds
	time_t nextTimestampRawAcc;			//time the activity was detected in seconds
	uint32_t rawAccBufferLength;
	uint32_t accelerationsWaiting;
	bool activityWaiting;
	he2mtSDIRest sdi;
	char bleMAC[13];
	double accSamplingPeriod;
	bool authorizationRequested;
	char client_id[257];
	char client_secret[257];
	char username[257];
	char password[257];
public:
	he2mtThread();
	~he2mtThread();

	//Sets server URL to connect to including the port. A valid URL would be localhost:50000 for example
	void setServerUrl(char* url);

	// returns true, if the previous HE2mT call was executed sucessfully and false, otherwise.
	bool isSuccess();
	void setRawAccBufferLength(uint32_t nTriples);

	void setBLEMAC(char* mac);

	//set the next activity to be tranmitted via XML-RPC to the server
	bool setNextAcceleration(float nextAccX, float nextAccY, float nextAccZ);
	void setNextActivity(rcsActivity::activity_type activity);

	void sendOut();


	bool authorize(char* client_id, char* client_secret, char* username, char* password);
	he2mtSDI* getSDI();

	public Q_SLOTS:

	//Perfom the HE2mT-call. Make sure to call setServerUrl() before using this function and setnextActivity before every use of this function
	void execHe2mtTransaction();


	Q_SIGNALS:

	void he2mtTransactionFinished(void);


};

#undef QT_NO_SIGNALS

#endif /* HE2MT_THREAD_H_ */
