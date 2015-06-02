/*
 *  ErrorHandling.h
 *	Simple macros for debugging purposes
 *
 * Jun. 2011, Philipp Kindt <pkindt@directbox.com>
 * Institure for Realtime Computer Systems (RCS), TU München
 *
 */

#ifndef _ERROR_HANDLING_H_
#define _ERROR_HANDLING_H_

#include <iostream>
#include <stdlib.h>
using namespace std;
//backtrace system info from linux kernel.
//the code below is taken from http://www.kernel.org/doc/man-pages/ and slightly modified.
//Copyright for this code applies as for the kernel man pages.
//actual code in: printBackTrace.cpp
void printBackTrace();


#define DEBUG(msg) cout<<"[DEBUG]: "<<__FILE__<<", Line "<<__LINE__<<", "<<__FUNCTION__<<"() : "<<msg<<endl;
//#define DEBUG(msg)
#ifdef _IMGDEBUG
#define IMGDEBUG(msg) cout<<"[DEBUG]: "<<__FILE__<<", Line "<<__LINE__<<", "<<__FUNCTION__<<"() : "<<msg<<endl;
#else
#define IMGDEBUG(msg)
#endif

#define CONTINUEMSG(msg) cout<<"\t "<<msg<<endl;
#define WARN(msg) cout<<"[WARNING]: "<<__FILE__<<", Line "<<__LINE__<<", "<<__FUNCTION__<<"() : "<<msg<<endl;
#ifdef GUI_ERROR_HANDLING
	#define ERROR(msg) cerr<<"[ERROR]: (GUI) "<<__FILE__<<", Line "<<__LINE__<<", "<<__FUNCTION__<<"() : "<<msg<<endl;printBackTrace();exit(1);
#else
	#define ERROR(msg) cout<<"[ERROR]: (noGUI) "<<__FILE__<<", Line "<<__LINE__<<", "<<__FUNCTION__<<"() : "<<msg<<endl;printBackTrace();exit(1);
#endif

#endif /* ERROR_HANDLING_H_ */
