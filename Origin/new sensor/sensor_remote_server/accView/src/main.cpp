/*
 * main.c
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */

#include "mainWindowImpl.h"
#include <QApplication>
int main(int argc, char* argv[]){
	QApplication app(argc,argv);
	MainWindowImpl mwi;
	mwi.show();
	if(argc >= 2){
		mwi.setDevNode(argv[1]);
	}
	if(argc == 3){
		if(strcmp(argv[2],"BLE") == 0){
			mwi.setProtocol(MainWindowImpl::PROTOCOL_BLE);
		}else{
			mwi.setProtocol(MainWindowImpl::PROTOCOL_ACM);

		}
	}
	return app.exec();
	return 1;
}
