#ifndef PROCESSDATA_H
#define PROCESSDATA_H

#include "lococheck.h"
#include "mathdetection.h"

const QString locoCompDate = __DATE__;
const QString locoCompTime = __TIME__;
const QString locoCompStamp = QString( "Mot Lib Compiled at " + locoCompDate + " - " +  locoCompTime);

enum integralState { INITIAL_STATE, COUNTING_POSITIVES, NULL_DETECTED, DROPPING_NEGATIVES};

/*! \brief Is the whole procedure for the calculation, seperated from the gui. If you don't need the gui, use this class and the included classes lococheck and mathdetection.*/
class DLL_EXPORT processdata : public QObject
{
   Q_OBJECT
public:
   processdata();

   ~processdata();

   double getVersion(){return 0.44;}
   QString getLocoCompStamp(){return locoCompStamp;}
   int startDetection(QList<flt_t>*, QList<flt_t>*, QList<flt_t>*);
   float integrateAcc(QList<flt_t> *, int);
   void integrateValueImpl(flt_t);
   QList<flt_t> integrateValue(QList<flt_t> *data);
   flt_t  PositiveIntegralImpl(flt_t);
   QList<flt_t>  PositiveIntegral(QList<flt_t> *);

   QList<flt_t> getTimeDifferencesWalking();
   QList<flt_t> getTimeDifferencesRunning();
   QList<flt_t> getStepsW();
   QList<flt_t> getStepsR();
   QList<flt_t> getStairsW();
   QList<flt_t> getWalking();
   QList<flt_t> getRunning();
   QList<flt_t> indexRunningSteps, indexWalkingSteps;

   /*! \brief Delivers the position of the steps within a block of 128 samples. */
   QList<flt_t>  getPositionOfSteps();

   /*! \brief This variables containing the unmodified acceleration data from the sensor. */
   //QList<flt_t> dataX, dataY, dataZ;
   lococheck* lc;
   void resetBlocksProcessed(){blocksProcessed = 0;}

private:
   //objects

   //methods
   void deleteOldArrays();
   bool checkInputArrays(QList<flt_t>* dataXInput,QList<flt_t>* dataYInput,QList<flt_t>* dataZInput);
   int saveStepsWR();
   void getTimeBetweenWalkingSteps();
   void deleteDetectionOfThePreviousBlock();
   //void deleteTooOldStepsW();
   //void deleteTooOldStepsR();
   void checkIfRunningIsNewInformation();
   void checkIfWalkingIsNewInformation();



   //variables
   /*! \brief Mathdetection object for doing the math */
   mathdetection *md;

   int formerActivity;
   /*! \brief Contain the indices of the detected running or walking motion. */
   QList<flt_t> StepsR,StepsW;

   QList<int> checkRunning, checkWalking;



   /*! \brief Samples between two detected steps.*/
   QList<flt_t> differenceTimeSteps;
   /*! \brief Contains the indices of the detected stair steps. */
   QList<flt_t> StairsW;

   /*! \brief Contains the number of executions of the motion Detection algortihms. */
   long blocksProcessed;

   /*! \brief This variables containing a one, if the window had a running / walking step, a zero if not. */
   QList<flt_t>running, walking;



   QList<float> areaParts;
   QList<float> areaParts1;

   float tmpArea;


   integralState currentIntegralState; // 0=INIT 1= pos 1=NULL 2=NEG
   float holdValue;

};

#endif // PROCESSDATA_H

/*
Changes:
22.11.10  14:32:
- alle std::cout auskommentiert
Version 0.42 -> 0.43


*/
