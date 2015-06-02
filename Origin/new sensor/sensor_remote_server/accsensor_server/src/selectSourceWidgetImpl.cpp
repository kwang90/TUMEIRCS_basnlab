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
void selectSourceWidgetImpl::setMainWindowImpl(mainWindowImpl* mwi){
	this->mwi = mwi;
	devNode->setText(QString(mwi->getDeviceNode()));
	if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_ACM){
		rbACM->setChecked(true);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(false);
		rbE1CH->setChecked(false);
	}else if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_BLE){
		rbACM->setChecked(true);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(false);
		rbE1CH->setChecked(false);
	}else if(mwi->getProtocol() == mainWindowImpl::PROTOCOL_BLE_E1CH){
		rbACM->setChecked(false);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(false);
		rbE1CH->setChecked(true);
	}else{
		rbACM->setChecked(false);
		rbBLE->setChecked(false);
		rbBLERaw->setChecked(true);
		rbE1CH->setChecked(false);
	}
}

void selectSourceWidgetImpl::updateData(){
	if(mwi == NULL){
		return;
	}

	if(rbACM->isChecked()){
		mwi->setProtocol(mainWindowImpl::PROTOCOL_ACM);
	}else if(rbBLE->isChecked()){
		mwi->setProtocol(mainWindowImpl::PROTOCOL_BLE);
	}else if(rbE1CH->isChecked()){
		mwi->setProtocol(mainWindowImpl::PROTOCOL_BLE_E1CH);
	}else{
		mwi->setProtocol(mainWindowImpl::PROTOCOL_BLE_RAW);
	}
	mwi->setDeviceNode((uint8_t*)devNode->text().toAscii().data());
	this->setVisible(false);
}
