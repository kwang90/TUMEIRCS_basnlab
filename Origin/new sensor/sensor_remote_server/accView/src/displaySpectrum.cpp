/*
 * displaySpectrum.cpp
 *
 *  Created on: 18.03.2013
 *      Author: kindt
 */


/*
 * displaySpectrum.cpp
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */


#include <QtGui>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QWheelEvent>
#include <QTimer>
#include <vector>
#include <iostream>
#include "displaySpectrum.h"
using namespace std;

displaySpectrum::displaySpectrum(QWidget* parent){
	if (parent != NULL) {
		resize(parent->size());
	}
	mwi = NULL;
	sema = new QSemaphore(1);
	this->setAutoFillBackground(true);
	QPalette pal = this->palette();
	pal.setColor(QPalette::Normal, QPalette::Window, Qt::white);
	this->setPalette(pal);
	this->setBackgroundRole(QPalette::Window);
	data = NULL;
	max = 0.0;
	this->show();
}

displaySpectrum::~displaySpectrum(){
	if(sema != NULL){
		delete sema;
	}
}


void displaySpectrum::setMainWindowImpl(MainWindowImpl* mwi) {
	this->mwi = mwi;
}
void displaySpectrum::setData(double* data) {

	if(sema != NULL){
		sema->acquire();
	}
	this->data = data;
	if(sema != NULL){
		sema->release();
	}
}

void displaySpectrum::paintEvent(QPaintEvent* ev) {
	if (mwi == NULL) {
		return;
	}
	if(sema != NULL){
		sema->acquire();
	}
	if(data == NULL){
		if(sema != NULL){
			sema->release();
		}
		return;
	}
	double avg = 0;
	QPainter p(this);
	uint32_t pos;
	double pixelPos;
	double pixelPosPrev;
	pixelPosPrev = 0;
	for(int32_t cnt = 0; cnt < width() / 2; cnt++){
		pixelPos = (data)[cnt];
		if(pixelPos > max){
			max = pixelPos;
		}
		avg += pixelPos;
		pixelPos /= max;
		pixelPos *= (height());
		p.drawLine((cnt - 1) * 2,height() - 5 - pixelPosPrev,cnt * 2,height() - 5 - pixelPos);
		pixelPosPrev = pixelPos;
	}
	avg /= width()/2;
	if(max > avg){
		max = avg;
	}
	p.end();
	if(sema != NULL){
		sema->release();
	}
}


void displaySpectrum::resizeEvent(QResizeEvent* event){
	if(sema != NULL){
		sema->acquire();
	}
	if(sema != NULL){
		sema->release();
	}
}

