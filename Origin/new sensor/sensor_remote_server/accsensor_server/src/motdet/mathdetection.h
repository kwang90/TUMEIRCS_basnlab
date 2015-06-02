#ifndef MATHDETECTION_H
#define MATHDETECTION_H
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <QList>
#include "fftreal.h"
//#include "console.h"
#include <QtGui>
#include <QtAlgorithms>
#include "QObject"
#include "motdetglobal.h"

/* Change this typedef to use a different flt_ting point type in your FFTs
            (i.e. flt_t, double or long double). */
    typedef float	flt_t;

 /*! \brief Needed math for the MotionDetection.*/
class DLL_EXPORT mathdetection
{
public:
    mathdetection();

        /*! \brief Calculates the cross-correlation of the inputArr1 and inputArr2 and fills the resulting values in the variable output.*/
    void do_xcorr(QList<flt_t> *output, QList<flt_t> *inputArr1, QList<flt_t> *inputArr2);

    /*! \brief Calculates the discrete fourier transformation of an input double array and fills the result in a double array. */
    //void do_dft(int dir,int size,double *Xr,double *Xi, double *xr, double *xi);

    /*! \brief Calculates the discrete fourier transformation of an input QList<float> and fills the result in a double array.  */
    //void do_dft(int dir,int size,double *Xr,double *Xi, QList<flt_t> *xr, QList<flt_t> *xi);

    /*! \brief Calculates the discrete fourier transformation of an input QList<float> and fills the result in a QList<float>.  */
    void do_fft(int dir,int size,QList<flt_t> *Xr,QList<flt_t> *Xi, QList<flt_t> *xr, QList<flt_t> *xi);

    /*! \brief Calculates the absolute distance of a complex QList<float>. First: input real , second: input imaginary, third: result */
    void absV(QList<flt_t>*, QList<flt_t>*, QList<flt_t>*);

    /*! \brief Calculates the absolute distance of a real QList<float>. First: input , second: result */
    void absV(QList<flt_t> *, QList<flt_t>*);

    /*! \brief Calculates the absolute distance of a real QList<float>. First: input / result */
    void absV(QList<flt_t> *);

    /*! \brief  the fftshift() known from MATLAB. First: input , second: result */
    void fftshift(QList<flt_t> *, QList<flt_t>*);

    /*! \brief  Returns the mean value as a double variable, the second input variable is the start-, the third the end-index, for the calculation. */
    double meanV(QList<flt_t> *, int, int);

    /*! \brief  Detects the the maxima and minima of a given data set (first input), second output, third threshold value.*/
    void algodetect(QList<flt_t>*,QList<flt_t>*, float);

    /*! \brief  Returns the variance of the input QList. */
    float var(QList<flt_t>*, int, int);

    /*! \brief  Returns the absolute distance of an real-variable. */
    float abs(float value);

    /*! \brief  Differentiate the first input variable saves the result in the second input variable.*/
    void diff(QList<flt_t> *input, QList<flt_t> *output,int start=0,int=0);

    /*! \brief  Calculates a k - clustering for the first input variable, with the abort-accuracy of the second input variable.*/
    QList<flt_t> kclustering(QList<flt_t>*, float);

    /*! \brief  Negotiate the first input variable and saves the restult in the second. */
    void neg(QList<flt_t>*, QList<flt_t>*);

    /*! \brief  Negotiate the variable. */
    void neg(QList<flt_t>*);

    /*! \brief  Checks if the steps are too close to another. First input variable is the steps, the second is the raw dataX accelleration data, the third is the start index, the forth is a threshold value.*/
    void checkMinDistz(QList<flt_t> *steps, QList<flt_t> *dataX, int start, const int threshold);

    /*! \brief  Returns the index of the smallest value of an array.*/
    int minimumItem(QList<flt_t>*);

    /*! \brief  Returns the index of the highest value of an array. First: input Arr, Second: startIndex, Third: endIndex.*/
    int maximumItem(QList<flt_t>* , int  , int );

    /*! \brief  Returns the index of the smallest item of an array. First: input Arr, Second: startIndex, Third: endIndex. */
    int minimumItem(QList<flt_t>*, int, int);

    /*! \brief Adds the second input variable to all the items of the first input variable and saves the result in the third variable.*/
    void addScalar2QList(QList<flt_t>*, float,QList<flt_t>* );

    /*! \brief Adds the second input variable to all the items of the first input variable.*/
    void addScalar2QList(QList<flt_t>*, float);

    /*! \brief Butterworth - filter: first item: input, second item: output, third item: mode*/
    void butterworth(QList<flt_t>*,QList<flt_t>*, int);

    /*! \brief Butterworth - filter: first item: input/output, second item: mode*/
    void butterworth(QList<flt_t>*, int);

    /*! \brief Checks the first input variable for extremums, the second input variable contains all troughs, the third all peaks. The forth input variable is a threshold variable, if -1.0, the function calculates an own threshold value with k-clustering.*/
    void checkXtreme(QList<flt_t>*,QList<flt_t>*,QList<flt_t>*,flt_t);

    /*! \brief Checks the first input variable for extremums, the second input variable contains all troughs, the third all peaks. The forth input variable is a threshold variable,
    if -1.0, the function calculates an own threshold value with k-clustering. Forth: startIndex, fifth: endIndex*/
    void checkXtreme(QList<flt_t>*,QList<flt_t>*,QList<flt_t>*,flt_t, int, int);

    /*! \brief Find unique elements of first input variable. */
    void unique(QList<flt_t>*);

    /*! \brief This function calculates the zero crossing points. */
    void zeroCrossing(QList<flt_t>*, float, int, QList<flt_t>*, QList<flt_t>*);

    /*! \brief Signum function. */
    void sign(QList<flt_t>*, QList<flt_t>*);

    /*! \brief Sums the values up from start to end index of an QList. Second: startIndex, third: endIndex . */
    double sum(QList<flt_t>*,int, int);


private:
        int n,m,i;
        double w;
        double wr;
        double wi;
        int nbr_points;
        FFTReal* fft;
};

#endif // MATHDETECTION_H
