/*
 *
 * Sep 2011, Philipp Kindt <pkindt@directbox.com>
 */

#include "genericParametrizationWidgetImpl.hpp"
#include "string.h"
#include <QDoubleSpinBox>
#include <iostream>
#include "errorHandling.h"
#include "math.h"
#include <stdio.h>
genericParametrizationWidgetImpl::genericParametrizationWidgetImpl(QWidget* parent):
Ui::genericParametrizationWidget(),
QWidget(parent)
{
	setupUi(this);
	this->nWidgets = 0;
	interface = NULL;
}


genericParametrizationWidgetImpl::~genericParametrizationWidgetImpl(){
	interface = NULL;
}
void genericParametrizationWidgetImpl::closeEvent(QCloseEvent* ce){
}

void genericParametrizationWidgetImpl::setParamInterface(genericParamInterface *interface){
	cerr<<"da"<<endl;
	if(interface != NULL){
		this->interface = interface;
		cerr<<"1"<<endl;
		updateGui();
		cerr<<"2"<<endl;
	}else{
		cerr<<"NULL"<<endl;
	}
}

void genericParametrizationWidgetImpl::updateGui(){
	if(interface == NULL){
		return;
	}
	char valBuf[256];

	//read out param list
	char* parList =  interface->getParamList();
	char* tmp = parList;

	char tmpTok[256];

	//variables to store the current param values
	double limit_low;		//the lower limit of a param widget
	double limit_high;		//the upperlimit of a param widget
	char* ident;			//the ident of the param
	char* decident;			//the decident decides weather the param is of type decimal ("d"), flaot ("f) or string ("s"). When float, it mai contain the number of valid decimals: ("f11") means the value must have a precision of 11 decimals
	char* decidentPrecision;//pointer pointing on the decimals selection string after "f".
	uint32_t precision;		//the number of decimals from decident as anumeric value

	resetWidgets();
	nWidgets = 0;

	//widgets to create...
	QDoubleSpinBox *spTmp;
	QLineEdit *leTmp;

	if(strlen(parList ) < 1) return;

	//parse each line...
	while((*tmp != '\0')&&(sscanf(tmp,"%s\n",tmpTok))){
		tmp = tmp + strlen(tmpTok) + 1;			//skip newilne

		//split tokens, separator is "|".
		ident = strtok(tmpTok, "|");
		decident = strtok(NULL,"|");

		//is there a precision (for floats)?
		if(strlen(decident) != 1){
			//yes, there is!
			decidentPrecision = decident + 1;
			precision = atoi(decidentPrecision);
			sprintf(decident, "%c",decident[0]);
		}else{
			//nope, decident is just one character => no precision value. set default precision
			precision = 1;
		}
		if(strcmp(decident,"d")==0){
			precision = 0;
		}

		//set limits
		limit_low = atof(strtok(NULL,"|"));
		limit_high = atof(strtok(NULL,"|"));


		//store values to vector
		limits_high.push_back(limit_high);
		limits_low.push_back(limit_high);
		idents.push_back(QString(ident));
		parTypes.push_back(QString(decident));

		//read out the value of the interface param
		interface->getParam(ident,valBuf);

		if((strcmp(decident,"f")==0)||(strcmp(decident,"d")==0)){
			//Float or decimal: Add double spinbox
			spTmp = new QDoubleSpinBox(NULL);
			spTmp->setMinimum(limit_low);
			spTmp->setMaximum(limit_high);
			spTmp->setDecimals(precision);
			spTmp->setSingleStep(1.0/pow(10,precision));
			spTmp->setValue(atof(valBuf));
			spTmp->setObjectName(QString(ident));
			spTmp->setFixedWidth(15*(precision + log10(limit_high) + 3));
			widgets.push_back((QWidget*) spTmp);
			formLayout->addRow(QString(ident), spTmp);
			connect(spTmp,SIGNAL(valueChanged(double)),this,SLOT(guiToInterface()));
		}else if(strcmp(decident,"s")==0){
			//string param: add QLineEdit
			leTmp = new QLineEdit();
			if(limit_high != 0){
				leTmp->setMaxLength(limit_high);
			}
		//	leTmp->setSizePolicy(QSizePolicy::MinimumExpanding);
			leTmp->setText(QString(valBuf));
			leTmp->setObjectName(QString(ident));
			widgets.push_back((QWidget*) leTmp);
			formLayout->addRow(QString(ident), leTmp);
			connect(leTmp,SIGNAL(editingFinished()),this,SLOT(guiToInterface()));
		}else{
			printf("Unknown parameter decident: %s",decident);
			exit(1);
		}

		//increase number of widgets
		nWidgets++;
	}
	free(parList);
}

void genericParametrizationWidgetImpl::resetWidgets(){
	vector<QWidget*>::iterator it;
	for(it = widgets.begin(); it != widgets.end(); it++){
		delete(formLayout->labelForField(*it));
		delete *it;
	}
	widgets.clear();
	limits_high.clear();
	limits_low.clear();
	parTypes.clear();
	idents.clear();
}

genericParamInterface* genericParametrizationWidgetImpl::getParamInterface(){
	return interface;
}
void genericParametrizationWidgetImpl::guiToInterface(){
	if(interface == NULL){
		return;
	}
	vector<QWidget*>::iterator it;
	char str[256];

	for(uint32_t cnt = 0; cnt < widgets.size(); cnt++){
		if(parTypes[cnt]!=QString("s")){
			//String param
			sprintf(str,"%f",((QDoubleSpinBox*) widgets[cnt])->value());
			interface->setParam(widgets[cnt]->objectName().toAscii().data(),str);
		}else{

			//float or decimal param
			interface->setParam(widgets[cnt]->objectName().toAscii().data(),((QLineEdit*) widgets[cnt])->text().toAscii().data());
		}
	}
}
