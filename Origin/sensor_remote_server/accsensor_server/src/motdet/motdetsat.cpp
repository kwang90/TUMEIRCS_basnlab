#include "motdetsat.h"




motdetsat::motdetsat()
{

}


motdetsat::~motdetsat()
{

}

motdetsat* motdetsat::getInstance()
{
  static motdetsat msmotdetsat;
  return &msmotdetsat;
}



void motdetsat::deliver(QString aMsg)
{

Q_EMIT signalMsg(aMsg);
}


