/*
 * displayWidget.cpp
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
#include "displayWidget.h"
using namespace std;


displayWidget::displayWidget(QWidget* parent){
	printf("Construction @ %x\n",this);
	if (parent != NULL) {
		//resize(parent->size());
		cout << "Resize:" << endl;
		cout << parent->size().width() << endl;
		;
		cout << parent->size().height() << endl;
		;
	} else {
		cout << "NULL" << endl;
	}
	mwi = NULL;
	this->sema = new QSemaphore(1);
	if(this->sema == NULL){
		printf("error: semaphore creation failed.");
		exit(1);
	};
	this->sema->release();
	accData = new vector<int32_t>(width() + 10);
	ringBufPos = 0;
	scaleFactor = 1;

	this->setAutoFillBackground(true);
	QPalette pal = this->palette();
	pal.setColor(QPalette::Normal, QPalette::Window, Qt::white);
	this->setPalette(pal);
	this->setBackgroundRole(QPalette::Window);
	this->show();
}

displayWidget::~displayWidget(){
	delete accData;
	printf("destroy @ %x\n",this);
	if(sema != NULL){
		delete sema;
		sema = NULL;
	}
}

void displayWidget::setMainWindowImpl(MainWindowImpl* mwi) {
	this->mwi = mwi;
}

void displayWidget::paintEvent(QPaintEvent* ev) {
	if (mwi == NULL) {
		return;
	}
	if(sema != NULL){
		sema->acquire();
	}
	QPainter p(this);
	uint32_t pos;
	int32_t pixelPos;
	int32_t pixelPosPrev;
	pixelPosPrev = height()/2;
	for(int32_t cnt = 0; cnt < width(); cnt++){
		pos = (ringBufPos + cnt) % width();
		pixelPos = (*accData)[pos];
		pixelPos = (double)pixelPos * ((double)height()/2.0 - 1.0)/(double)scaleFactor;
		pixelPos += height()/2;
		p.drawLine(cnt - 1,pixelPosPrev,cnt,pixelPos);
		pixelPosPrev = pixelPos;
	}
	p.end();
	if(sema != NULL){
		sema->release();
	}

}

void displayWidget::addData(int32_t data){
	if(sema != NULL){
		sema->acquire();
	}

	(*accData)[ringBufPos] = data;
	ringBufPos += 1;
	ringBufPos = ringBufPos%width();
	if(sema != NULL){
		sema->release();
	}
}

void displayWidget::resizeEvent(QResizeEvent* event){
//	printf("resize Event begin\n");
	if(sema != NULL){
		sema->acquire();
	}else{
		printf("\nnSEMA NULL\n\n");
	}
	accData->resize(width() + 10);
	ringBufPos = 0;
	if(sema != NULL){
		sema->release();
	}
//	printf("resize Event end\n");
}

uint32_t displayWidget::getDataLength(){
	uint32_t rv;
	if(sema != NULL){
		sema->acquire();
	}

	rv = accData->size() - 10;
	if(sema != NULL){
		sema->release();
	}
	return rv;
}

int32_t displayWidget::getData(uint32_t index){
	int32_t rv;
	if(sema != NULL){
		sema->acquire();
	}

	uint32_t pos = (ringBufPos + index) % width();
	rv =  (*accData)[pos];
	if(sema != NULL){
		sema->release();
	}
	return rv;
}

void displayWidget::setScaleFactor(double scaleFactor){
	if(this->sema != NULL){
		this->sema->acquire();
	}

	this->scaleFactor = scaleFactor;
	if(this->sema != NULL){
		this->sema->release();
	}
}
