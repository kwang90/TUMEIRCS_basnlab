/*
 * rcsActivity.h - Class representing an activity detected by an activity sensor
 *
 *  Created on: 20.06.2013
 *      Author: kindt
 */

#ifndef RCSACTIVITY_H_
#define RCSACTIVITY_H_
#include <time.h>

struct rcsActivity{

	//type of the activity
	enum activity_type{ACTIVITY_TYPE_UNKNOWN, ACTIVITY_TYPE_RESTING, ACTIVITY_TYPE_WALKING,ACTIVITY_TYPE_RUNNING};
public:
	activity_type activity;		//type of activity
	time_t timestamp;			//time the activity was detected in seconds
};





#endif /* RCSACTIVITY_H_ */
