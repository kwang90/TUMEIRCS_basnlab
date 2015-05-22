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
class mainWindowImpl;
class selectSourceWidgetImpl : public QWidget, private Ui::selectSourceWidget{

	Q_OBJECT

	public:
	mainWindowImpl* mwi;
	selectSourceWidgetImpl(QWidget* parent);
	void setMainWindowImpl(mainWindowImpl* mwi);

	public Q_SLOTS:

	void updateData();




};
#endif /* SELECTSOURCEWIDGETIMPL_H_ */
