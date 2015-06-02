/*
 * displayWidget.h
 *
 *  Created on: 14.03.2013
 *      Author: kindt
 */

#ifndef DISPLAYWIDGET_H_
#define DISPLAYWIDGET_H_


#include <QtGui>
#include <vector>
#include <QWidget>
#include <QSemaphore>

#include <inttypes.h>
class MainWindowImpl;
using namespace std;
class displayWidget : public QWidget{
private:
	MainWindowImpl* mwi;
	std::vector<int32_t>* accData;
	uint32_t ringBufPos;
	double scaleFactor;
	QSemaphore *sema;
public:
	displayWidget(QWidget* parent = 0);
	~displayWidget();
	virtual void paintEvent(QPaintEvent* ev);
	void setMainWindowImpl(MainWindowImpl* mwi);
	void addData(int32_t acc);
	void resizeEvent(QResizeEvent* event);
	int32_t getData(uint32_t index);
	uint32_t getDataLength();
	void setScaleFactor(double scaleFactor);

};
#endif
