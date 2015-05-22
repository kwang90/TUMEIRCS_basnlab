/*
 * selectSourceWidgetImpl.cpp
 *
 *  Created on: 15.03.2013
 *      Author: kindt
 */


#include "selectSourceWidgetImpl.h"
#include "mainWindowImpl.h"
selectSourceWidgetImpl::selectSourceWidgetImpl(QWidget* parent){
	setupUi(this);
	mwi = NULL;
	connect(pbOK,SIGNAL(clicked()),this,SLOT(updateData()));
}
void selectSourceWidgetImpl::setMainWindowImpl(MainWindowImpl* mwi){
	this->mwi  = mwi;
	devNode->setText(QString(mwi->getDevNode()));
	if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_ACM){
		rbACM->setChecked(true);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(false);
		rbUSBRaw->setChecked(false);
		rbE1CH->setChecked(false);
	}else if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_BLE_RAW){
		rbACM->setChecked(false);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(true);
		rbUSBRaw->setChecked(false);
		rbE1CH->setChecked(false);
	}else if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_USB_RAW){
		rbACM->setChecked(false);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(false);
		rbUSBRaw->setChecked(true);
		rbE1CH->setChecked(false);
	}else if(mwi->getProtocol() == MainWindowImpl::PROTOCOL_BLE_E1CH){
		rbACM->setChecked(false);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(false);
		rbUSBRaw->setChecked(false);
		rbE1CH->setChecked(true);

	}else{
		rbACM->setChecked(false);
		rbBLE->setChecked(true);
		rbBLERaw->setChecked(false);
		rbUSBRaw->setChecked(false);
		rbE1CH->setChecked(false);

	}
}

void selectSourceWidgetImpl::updateData(){
	if(mwi == NULL){
		return;
	}

	if(rbACM->isChecked()){
		mwi->setProtocol(MainWindowImpl::PROTOCOL_ACM);
	}else if(rbBLE->isChecked()){
		mwi->setProtocol(MainWindowImpl::PROTOCOL_BLE);
	}else if(rbUSBRaw->isChecked()){
		mwi->setProtocol(MainWindowImpl::PROTOCOL_USB_RAW);
	}else if(rbE1CH->isChecked()){
		mwi->setProtocol(MainWindowImpl::PROTOCOL_BLE_E1CH);
	}else{
		mwi->setProtocol(MainWindowImpl::PROTOCOL_BLE_RAW);
	}
	mwi->setDeviceNode((uint8_t*)devNode->text().toAscii().data());
	this->setVisible(false);
}
