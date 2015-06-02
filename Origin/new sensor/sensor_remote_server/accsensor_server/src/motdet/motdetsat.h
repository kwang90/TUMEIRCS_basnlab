#ifndef MOTDETSAT_H
#define MOTDETSAT_H


#include "motdetglobal.h"
#include <QString>
#include <QObject>

class DLL_EXPORT motdetsat : public QObject
{
    Q_OBJECT

private:
    motdetsat();
    motdetsat(const motdetsat& cc);
public:
    virtual ~motdetsat();
public:
    static motdetsat* getInstance();
    void deliver(QString aMsg);
Q_SIGNALS:
     void signalMsg(QString aMsg);
private:



};

#endif // motdetsat_H
