/**
 * \file libBPMInterfaceTemplate.hpp
 * \brief Common implementation of parts of the \libBPMInterface class.
 * The only function of libBPMInterface that is not implemented is \ref add().
 * If you derive your power management policy classes directly from this class, you will get a pre-implemented
 * \ref setConIntCallBack() - function.
 *
 * (c) Oct. 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 */

#ifndef BPMINTERFACETEMPLATE_H_
#define BPMINTERFACETEMPLATE_H_
#include "libBPMInterface.hpp"
class libBPMInterfaceTemplate: libBPMInterface{
private:
	libBPMsetConIntCallBack setConnectionIntervalCB;		///< Connection interval callback function. Initially set to NULL. Can be updated by \ref setConIntCallBack()
	uint32_t tcInitial;									///< The initial connection interval used by the BLE device. Initialized to zero, but can be updated by \ref setConIntCallBack()
public:

	/**
	 * The Constructuor will initialize \ref setConnectionIntervalCB to NULL and \ref tcInitial to 0
	 */
	libBPMInterfaceTemplate();

	/**
	 * Set the connection interval update callback (to be called by the BLE serial line interface before calling add() the first time.
	 * @param setConnectionIntervalCB	Callback function for updating the connection interval
	 * @param TcInitial	The initial connection interval used by the BLE module before this power management has changed it.
	 */
	void setConIntCallBack(libBPMsetConIntCallBack setConnectionIntervalCB, uint32_t TcInitial);

};

libBPMInterfaceTemplate::libBPMInterfaceTemplate(){
	setConnectionIntervalCB = NULL;
	tcInitial = 0;
}

void libBPMInterfaceTemplate::setConIntCallBack(libBPMsetConIntCallBack setConnectionIntervalCB, uint32_t tcInitial){
	this->setConnectionIntervalCB = setConnectionIntervalCB;
	tcInitial = tcInitial;
}


#endif /* BPMINTERFACETEMPLATE_H_ */
