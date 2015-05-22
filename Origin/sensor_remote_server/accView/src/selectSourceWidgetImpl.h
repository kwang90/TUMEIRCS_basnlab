/*
 * selectSourceWidgetImpl.h
 *
 *  Created on: 15.03.2013
 *      Author: kindt
 */

#ifndef SELECTSOURCEWIDGETIMPL_H_
#define SELECTSOURCEWIDGETIMPL_H_
#include "ui_selectSourceWidget.h"
#include <QWidget>
class MainWindowImpl;
class selectSourceWidgetImpl : public QWidget, private Ui::selectSourceWidget{

	Q_OBJECT

	public:
	MainWindowImpl* mwi;
	selectSourceWidgetImpl(QWidget* parent);
	void setMainWindowImpl(MainWindowImpl* mwi);

	public slots:

	void updateData();




};
#endif /* SELECTSOURCEWIDGETIMPL_H_ */
