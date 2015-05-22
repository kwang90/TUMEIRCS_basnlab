#ifndef LOCOCHECK_H
#define LOCOCHECK_H


/*! \brief Set threshold for minimum Energy for RunningCHECK*/
#define THRESHOLD_ALGODETECT 15

/*! \brief Set threshold for minimum Distance between Steps*/
#define DISTANCE_IN_GETSTEPS 50

/*! \brief Set sample rate. 100Hz for E1Ch, 400 for RCS ARM Senspr*/
#define SAMPLE_RATE 100

/* \brief number of input blocks to be used */
#define NR_OF_BLOCKS_ANALYSED    1

/* \brief number of samples per input block */
#define NUMBER_OF_SAMPLES_PER_BLOCK 64

/*! \brief Set threshold for variance of Y-data(walking)*/
//#define THRESHOLD_WALKINGCHECK_Y 0.01

/*! \brief Set threshold min. Distance between Steps in StepsW (respectively in StepsR) */
#define THRESHOLD_IN_CHECKMINDIST 50

/*! \brief Set number of values, which are copied from the last block to the current */
#define VALUES_OF_FORMER_BLOCK 128


#include <QList>
#include "mathdetection.h"
//#include <QtGui>
#include <QString>
#include <cmath>
//#include </opt/local/include/opencv/cv.h>
//#include </opt/local/include/opencv/highgui.h>

#include "motdetsat.h"


#include <QObject>
#include "motdetglobal.h"


typedef float flt_t; //mist. double statt float


/*! \brief Algorithms for the Motion Detection.*/
class DLL_EXPORT lococheck : public QObject

{
    Q_OBJECT//: virtual private MathDetection

public:
    lococheck(mathdetection *, QObject *parent = 0);

    /*! \brief Destructor. Should be filled with objects to delete. TODO! */
    ~lococheck();

    /*! \brief Checks for a single wakling/running step.*/
    void checkForStep(QList<flt_t>*,QList<flt_t>*,QList<flt_t>*,QList<flt_t>*);

    /*! \brief Contains the energy of a windowed signal (important for running/walking check) */
    QList<flt_t> *DNmag, DNmagOrig;

    /*! \brief If variable isn't zero, the person might be running.*/
    QList<flt_t> MainLowFreq_p;

    /*! \brief Object of the mathematic - class. */
    mathdetection* md;

    /*! \brief Sets the private dataX, dataY, dataZ Qlists of lococheck to the pointer, which are the input arrays of processdata. */
    int updateXYZ(QList<flt_t>*,QList<flt_t>*,QList<flt_t>*);

    QList<flt_t> * getDataX() {return &dataX;}
    QList<flt_t> * getDataY() {return &dataY;}
    QList<flt_t> * getDataZ() {return &dataZ;}


    /*! \brief Checks the acceleration data for stair steps. */
    void stairscheck(QList<flt_t>*, QList<flt_t>*, QList<QString>*);

    /*! \brief Checks if the walking and running steps are too close to anotuer, delete both if yes. */
    void checkIfRunningWalkingToClose(QList<flt_t>* , QList<flt_t>* );

public Q_SLOTS:
    void setTrRunning(double threshold);
    void setTrWalkingZ(double threshold);
    void setTrWalkingY(double threshold);
    void setTrXth(double threshold);
    void setTrYth(double threshold);
    void setTrZth(double threshold);

private:

    double Xth, Yth, Zth; //for resting
    QList<flt_t> xac;     // dataX - mean(dataX)

    /*! \brief This variables containing the unmodified acceleration data from the sensor. */
    QList<flt_t> dataX, dataY, dataZ;

    QList<flt_t> dataXold, dataYold, dataZold;

    double THRESHOLD_WALKINGCHECK_Z;

    double THRESHOLD_WALKINGCHECK_Y;

    /*! \brief addded tp-filtered and mean-value substracted added dataX and dataZ. */
    QList<flt_t> addedXZ;

    /*! \brief Defines a optimized (for detection algorithms) window of the acceleration data. */
    void getWindow();

    /*! \brief Checks if the person is resting or not. Returns "true" if person is resting, otherwise "false". */
    bool restingCheck(QList<flt_t>*,QList<flt_t>*,QList<flt_t>*,int , int);

    /*! \brief Verifies if the data contains walking steps. */
    void walkingcheck();

    /*! \brief Determines the postition of possible walking steps. */
    void getSteps(QList<flt_t>* , float , int , const int, QList<flt_t>* );

    /*! \brief Adds a number (specified through VALUES_OF_FORMER_BLOCK) of values from the last block to the current block. */
    void addValuesToBlock();

    /*! \brief Stores a number (specified through VALUES_OF_FORMER_BLOCK) of values of the actual block, for the next incoming block. */
    void storeValuesOfBlock();

    /*! \brief Start index in the raw acceleration data of the defined window. */
    int startIndex;

    /*! \brief End index in the raw acceleration data of the defined window. */
    int endIndex;

    /*! \brief Actual beginning index of the hardcoded window, which increases due a while - loop. */
    int pos;

    /*! \brief Threshold value for the differentiation between running/walking. */
    double sp_th_r;

    /*! \brief Hardcoded window size */
    int nw;

    /*! \brief Contains the windowed raw acceleartion data of x - Axe and y - Axe. */
    QList<flt_t> xWin, yWin;

    /*! \brief Contains temporarily the steps indices. */
    QList<flt_t> steps;

    /*! \brief Mean Value of the whole x (respectively z) array */
    double meanX, meanZ;

};

#endif // LOCOCHECK_H
