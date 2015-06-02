/*
 * displaySpectrum.h
 *
 *  Created on: 18.03.2013
 *      Author: kindt
 */

#ifndef DISPLAYSPECTRUM_H_
#define DISPLAYSPECTRUM_H_

#include <QtGui>
#include <QWidget>
#include <vector>
#include <inttypes.h>
#include <QSemaphore>
class MainWindowImpl;
using namespace std;
class displaySpectrum : public QWidget{
private:
	MainWindowImpl* mwi;
	double* data;
	double max;
	QSemaphore *sema;
public:
	displaySpectrum(QWidget* parent = 0);
	~displaySpectrum();
	virtual void paintEvent(QPaintEvent* ev);
	void setMainWindowImpl(MainWindowImpl* mwi);
	void setData(double* data);
	void resizeEvent(QResizeEvent* event);

};

#endif /* DISPLAYSPECTRUM_H_ */
