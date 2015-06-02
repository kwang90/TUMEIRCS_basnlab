/*
 * main.cpp
 *
 *  Created on: 20.06.2013
 *      Author: kindt
 */




#include "mainWindowImpl.h"
//#include <QApplication>
int main(int argc, char* argv[]){
	QApplication app(argc,argv);
	QApplication::setStyle("oxygen");
	mainWindowImpl mwi;
	mwi.show();
	return app.exec();
	return 1;
}
