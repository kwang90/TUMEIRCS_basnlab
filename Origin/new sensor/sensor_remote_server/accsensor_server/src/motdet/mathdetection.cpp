#include "mathdetection.h"
using namespace std;

mathdetection::mathdetection()
{
   fft=NULL;

}

void mathdetection::do_xcorr(QList<flt_t> *output, QList<flt_t> *inputArr1, QList<flt_t> *inputArr2)
{
    QList<flt_t> outputArr1;
    QList<flt_t> outputArr2;
    QList<flt_t> Xr;
    QList<flt_t> Xi;
    QList<flt_t> Yr;
    QList<flt_t> Yi;
    QList<flt_t> zero;
    unsigned int newSize;
    //only for fft flt_t* z = new flt_t[size*2 - 1];

    //checks if the arrays have the same length
    if ( inputArr1->length() != inputArr2->length())
    {
        //term->add2Console(QString("ERROR: Arrays must be the same size!"));
        cerr << "ERROR: Input arrays of the crosscorrelation have to be the same size" << endl;
    }


        QList<flt_t> Zr;
        QList<flt_t> Zi;

        //enhance the arrays to the 2*lengthOfArry -1
        newSize = (2 * inputArr1->length()) - 1;
        for(int i = 0; i < (int)newSize; i++)
        {
            if(i < inputArr1->length())
            {
                outputArr1.insert(i, inputArr1->at(i));
                outputArr2.insert(i, inputArr2->at(i));
            }
            else
            {
                outputArr1.insert(i, 0.0);
                outputArr2.insert(i, 0.0);
            }
            zero.append(0.0);
        }


        //here starts the dft
        do_fft(1, (int) outputArr1.length() , &Xr, &Xi, &outputArr1, &zero);
        do_fft(1, (int) outputArr2.length() , &Yr, &Yi, &outputArr2, &zero);

        //here calculation of cross - correlation with A x B* = xcorr(a,b)
        for(int i = 0; i < outputArr1.length() ; i++)
        {
            Zr.insert(i, Xr.at(i) * Yr.at(i) + Xi.at(i) * Yi.at(i));
            Zi.insert(i, Xi.at(i) * Yr.at(i) - Xr.at(i) * Yi.at(i));
        }
        //i-dft of Z for xcorr
        do_fft(-1, (int) outputArr1.length(), output, &zero, &Zr, &Zi);

        //rescale
        for(int i = 0; i < output->length(); i++)
        {
            output->replace(i, output->at(i) / output->length());
        }

        //swap the results
        for(int i = 0; i < output->length()/2.0 - 1; i++)
        {
            output->swap(i,i+output->length()/2.0 + 1);
        }
        //swap middle value to the end
        float halfValue = output->takeAt(output->length()/2.0);
        output->append(halfValue);

        return;
    }

void mathdetection::do_fft(int dir,int size,QList<flt_t> *Xr,QList<flt_t> *Xi, QList<flt_t> *xr, QList<flt_t> *xi)
{
    /*! Direct fourier transform\n
    \n
    xr and xi are the real and imaginary arrays.\n
\n
    size is the length of the arrays.\n
    \n
    dir = 1 gives forward transform\n
    dir = -1 gives reverse transform\n
    \n
    fills the results in the variables: real: Xr, imaginary: Xi).\n
*/
    nbr_points = xr->length();
    unsigned int dummy;
    //dummy = new unsigned int;
    if(fft->checkZeroPadding(nbr_points, &dummy) == false) //its a potence of 2 -> make fft
    {      
        fft = new FFTReal(nbr_points) ;
        QList<flt_t> outputOfFFT;
       // std::cout << "Using fft becaus of 2^n potence" << std::endl;

        fft->do_fft(&outputOfFFT, xr, nbr_points);
        fft->getImagAndReal(&outputOfFFT, nbr_points, Xr, Xi);

        outputOfFFT.clear();
        delete fft;
        fft = NULL;
    }
    else
    { //use dft
        //console term;
        //term.add2Console(QString("DFT"));
        wr = 0.0;
        wi = 0.0;
        w = 0.0;

        if (xi->isEmpty() == false)
        {
            for (n=0; n<size; n++)
            {
                Xr->insert(n, 0.0);
                Xi->insert(n, 0.0);

                for (m=0; m<size; m++) {

                    w = -2 * M_PI *m * n/size;

                    //change sign for the reverse transformation F -> f
                    if (dir == -1) w=-w;

                    wr=cos(w); wi=sin(w);

                    /*
            std::cout << "n: " << n << " w: " << w << std::endl;
            std::cout << "wr: " << wr << " wi: " << wi << "   xr->at(" << m << "): " << xr->at(m) << "   xi-at(" << m << "): " << xi->at(m) << std::endl;
*/
                    Xr->replace(n, Xr->at(n) + wr * xr->at(m) - wi * xi->at(m));
                    Xi->replace(n, Xi->at(n) + wr * xi->at(m) + wi * xr->at(m));
                }
            }
        }
        else
        {
            for (n=0; n<size; n++)
            {
                Xr->insert(n, 0.0);
                Xi->insert(n, 0.0);

                for (m=0; m<size; m++) {

                    w = -2 * M_PI *m * n/size;

                    //change sign for the reverse transformation F -> f
                    if (dir == -1) w=-w;

                    wr=cos(w); wi=sin(w);

                    Xr->replace(n, Xr->at(n) + wr * xr->at(m));
                    Xi->replace(n, Xi->at(n) + wi * xr->at(m));
                }

            }
        }
    }
    return;
}

double mathdetection::meanV(QList<flt_t> *inputArr1, int start, int end)
{
    double sum = 0.0;

    for(int i = start; i < end; i++)
    {
        sum += inputArr1->at(i);
    }

    return sum/(double) (end - start);
}

//absolute value for complex values
void mathdetection::absV(QList<flt_t> *inputArr1,QList<flt_t> *inputArr2, QList<flt_t>* outputArr)
{
    for(int i = 0; i < inputArr1->length(); i++)
    {
        outputArr->append( sqrt((inputArr1->at(i) * inputArr1->at(i)) + (inputArr2->at(i) * inputArr2->at(i))));
    }
    return;
}

void mathdetection::absV(QList<flt_t>* inputArr)
{
    for(int i = 0; i < inputArr->length(); i++)
    {
        if(inputArr->at(i) < 0)
            inputArr->replace(i, - inputArr->at(i));
        else
            inputArr->replace(i, inputArr->at(i));
    }
}

//absolute value for real Values
void mathdetection::absV(QList<flt_t> *inputArr1, QList<flt_t>* outputArr)
{
    for(int i = 0; i < inputArr1->length(); i++)
    {
        if(inputArr1->at(i) >= 0)
        {
            outputArr->append(inputArr1->at(i));
        }
        else
        {
            outputArr->append(-inputArr1->at(i));
        }
    }
    return;
}
//fftshift of matlab
void mathdetection::fftshift(QList<flt_t> * inputArr1, QList<flt_t>* outputArr)
{
    //check if even
    if(inputArr1->length() % 2 == 0)
    {  //even
        for(int i = 0; i < inputArr1->length(); i++)
        {
            if(i < inputArr1->length()/2)
            {
            outputArr->insert(i,inputArr1->at( i +  inputArr1->length()/2 ));
            }
            else
            {
            outputArr->insert(i,inputArr1->at( i  - inputArr1->length()/2));
            }
        }
    }
    else
    {
        //odd
     for(int i = 0; i < inputArr1->length(); i++)
      {
         if(i < floor(inputArr1->length()/2.0))
         {
         outputArr->insert(i,inputArr1->at( i + ceil(inputArr1->length()/2.0) ));
         }
         else
         {
         outputArr->insert(i,inputArr1->at( i  - floor(inputArr1->length()/2.0)));
         }

      }

    }

    return;
}

void mathdetection::algodetect(QList<flt_t>* inputArr,QList<flt_t>* outputArr, float threshold)
{
   int count = 0;
   int type = 1;
   int a = 0;
   int b = 0;


   while(count < inputArr->length())
   {

      if(type == 1)
      {
         if(inputArr->at(count) <= (inputArr->at(a) - threshold))
         {
            type = 3;
         }
         else if( inputArr->at(count) >= (inputArr->at(b) + threshold))
         {
            type = 2;
         }

         if(inputArr->at(count) >= inputArr->at(a))
         {
            a = count;
         }
         else if(inputArr->at(count) <= inputArr->at(b))
         {
            b= count;
         }
      }
      else if(type == 2)
      {
         if(inputArr->at(a) <= inputArr->at(count) )
         {
            a = count;
         }
         else if(inputArr->at(a) >= (inputArr->at(count) + threshold))
         {
            //here Maximum
            outputArr->append(a);
            b = count;
            type = 3;
         }
      }
      else if(type == 3)
      {
         if(inputArr->at(count) <= inputArr->at(b))
         {
            b = count;
         }
         else if(inputArr->at(count) >= (inputArr->at(b) + threshold))
         {
            //here Minimum, but not used, because (-inputArr -> Maximum) ==> Minimum
            //min->append(b);
            a=count;
            type = 2;
         }
      }
      count++;
   }
   return;
}

float mathdetection::var(QList<flt_t>* inputArr, int startIndex, int endIndex)
{
    double mean = meanV(inputArr, startIndex, endIndex);
    double result = 0.0;

    for(int i = startIndex; i < endIndex ; i++)
    {
        result += (inputArr->at(i) - mean) * (inputArr->at(i) - mean);
    }
    result = result / (endIndex - startIndex) ;

    return result;
}

void mathdetection::diff(QList<flt_t>* inputArr,QList<flt_t>* outputArr, int start, int end)
{
   if (end == 0)
   {
      end = inputArr->size()-1;
   }
    for(int i = start; i < end-1; i++)
    {
        outputArr->append(inputArr->at(i + 1) - inputArr->at(i));
    }

    return;
}
float mathdetection::abs(float value)
{
    if(value >= 0.0)
    {
        return value;
    }
    else
    {
        return -value;
    }
}
QList<flt_t> mathdetection::kclustering(QList<flt_t>* inputArr, float threshold)
{
    /*!
    % Algorithm to perform unsupervised learning and two class separation given\n
    % the data set 'data' and termination condition e. The program outputs the\n
    % two mean values of the given data set 'data'. The two mean values are\n
    % necessary to distinguish the class of data points that contain the\n
    % extrema and the class of data points that do not. This algorithm is based\n
    % on the k-means clustering algortihm used in pattern recogntion problems.\n
    %\n
    % INPUTS:\n
    %   data    :   array containing the data that is to be clustered.\n
    %   tm      :   threshold value which determins when the clustering\n
    %               algorithm stops executing\n
    %\n
    % OUTPUTS:\n
    %   c1 c2   :   the mean of the 2 clusters\n
    %\n
    % Usage:\n
    % [mean1 mean2] = kclustering(dataset, terminationValue)\n
    %\n
    */

    float c1 = inputArr->at(0);
    float c2 = inputArr->at(1);
    float lastc1 = c1;
    float lastc2 = c2;
    QList<flt_t> type1;
    QList<flt_t> type2;

    while(true)
    {
       type1.clear();
       type2.clear();
        for(int i = 0; i < inputArr->length(); i++)
        {
            if ( (abs(c1 - inputArr->at(i)) < abs(c2 - inputArr->at(i))) || (abs(c1 - inputArr->at(i)) == 0) && (abs(c1 - inputArr->at(i)) == 0))
            {
                type1.append(inputArr->at(i));
            }
            else
            {
                type2.append(inputArr->at(i));
            }
        }

        c1 = meanV(&type1, 0, type1.length());
        c2 = meanV(&type2, 0, type2.length());

        if( abs(lastc2 - c2) < threshold && abs(lastc1 - c1) < threshold)
        {
            QList<flt_t> list;
            list << c1 << c2;
            return list;
        }
        lastc1 = c1;
        lastc2 = c2;
    }

}
void mathdetection::neg(QList<flt_t>* inputArr, QList<flt_t>* outputArr)
{
    for(int i = 0; i < inputArr->length(); i++)
        outputArr->insert(i, - inputArr->at(i));

    return;
}
void mathdetection::neg(QList<flt_t>* Arr)
{
    for(int i = 0; i < Arr->length(); i++)
        Arr->replace(i, - Arr->at(i));

    return;
}
void mathdetection::checkMinDistz(QList<flt_t>* steps, QList<flt_t>* dataX, int start, const int threshold)
{
    QList<flt_t> distances, distances2;
    int minIndex = -1;
    int idx=0;
    float lengthStep = steps->length();
    const int thresholdDist = threshold;    // Set Threshold for minimum distance between two walking steps candidates here.
    int i = -1;

    while( i < lengthStep-1 && (lengthStep >= 2))
    {
       i++;
       distances.clear();
        distances2.clear();
        addScalar2QList(steps, -steps->at(i), &distances); //delete one element from the array
        absV(&distances, &distances2); //absolute distance
        distances2.removeAt(i); // delete the item, which deleted two steps above
        minIndex = minimumItem(&distances2); //get smallest value
        idx = distances.indexOf(distances2.at(minIndex));

        if(minIndex < 0)
        {
           std::cerr << "checkMinDist: minIndex < 0!" << std::endl;
           return;
        }
        else
        {
            if( distances2.at(minIndex) < thresholdDist)
            {
                if( (dataX->at(start + steps->at(i)) - dataX->at(start + steps->at(idx))) >= 0 )
                {
                    steps->removeAt(idx);
                }
                else if( (dataX->at(start + steps->at(i)) - dataX->at(start + steps->at(idx))) < 0  )
                {
                    steps->removeAt(i);
                }
                i -= 1;
                lengthStep -= 1;

             }
            else
            {
                lengthStep = steps->length();
                continue;
            }
        }
    }
    return;
}


void mathdetection::addScalar2QList(QList<flt_t>* inputArr, float scalar,QList<flt_t>* outputArr )
{
    for(int i = 0; i < inputArr->length(); i++)
    {
        outputArr->insert(i, inputArr->at(i) + scalar);
    }
    return;
}

void mathdetection::addScalar2QList(QList<flt_t>* inputArr, float scalar)
{
    for(int i = 0; i < inputArr->length(); i++)
    {
        inputArr->replace(i, inputArr->at(i) + scalar);
    }
    return;
}

int mathdetection::minimumItem(QList<flt_t>* inputArr)
{
    float isMinimum;
    int index = -1;

    for(int i = 0; i < inputArr->length(); i++)
    {
        if(i == 0)
        {
            isMinimum = inputArr->at(i);
            index = i;
        }
        else
        {
            if(isMinimum > inputArr->at(i))
            {
                isMinimum = inputArr->at(i);
                index = i;
            }
        }
    }

    return index;
}

int mathdetection::minimumItem(QList<flt_t>* inputArr, int  startIndex, int endIndex)
{
    float isMinimum;
    int index = -1;

    //check if indices are possible
    if(startIndex < 0 || endIndex > inputArr->length())
    {
        std::cerr << "ERROR: start Index is perhaps smaller than zero:" << startIndex << std::endl;
        std::cerr << "or endIndex is longer then inputArray. endIndex:  " << endIndex << " inputArr: " << inputArr->length() << std::endl;
    }

    for(int i = startIndex; i <= endIndex; i++)
    {
        if(i == startIndex)
        {
            isMinimum = inputArr->at(i);
            index = i;
        }
        else
        {
            if(isMinimum > inputArr->at(i))
            {
                isMinimum = inputArr->at(i);
                index = i;
            }
        }
    }

    return index;
}

int mathdetection::maximumItem(QList<flt_t>* inputArr, int  startIndex, int endIndex)
{
    //isMaximum can be used to transfer also the value of the maximum entry, not used YET!
    float isMaximum;
    int index = -1;

    //check if indices are possible
    if(startIndex < 0 || endIndex > inputArr->length())
    {
        //term->add2Console(QString("ERROR: Indices are not possible !!"));
        return -1;
    }

    for(int i = startIndex; i < endIndex; i++)
    {
        if(i == startIndex)
        {
            isMaximum = inputArr->at(i);
            index = i;
        }
        else
        {
            if(isMaximum < inputArr->at(i))
            {
                isMaximum = inputArr->at(i);
                index = i;
            }
        }
    }


    return index;
}

void mathdetection::butterworth(QList<flt_t>* inputArr, QList<flt_t>* outputArr, int mode)
{
    /*!
        *******************************************************************************\n
        * Function:      ButterworthFilter\n
        *\n
        * Description:   This function uses the Butterworth filter and returns a new\n
        *                value for an individual flt_ting point value.\n
        *\n
        ******************************************************************************\n
        * Butterworth Bandpass filter \n
        * sample rate - 150 \n
        *\n
        * mode = 0 - for step - \n
        *      - check with 5nd order \n
        *      - corner frequence = 1.95 \n
        *      IMPORTANT: MODE 0 DOESN'T WORK ANYMORE!!! //Delete
        *\n
        * mode = 1 - for stair - \n
        *      - check with 2nd order for performance reasons \n
        *      - corner frequence = 35 \n
    */

    /* coefficients will vary depending on sampling rate */
    /* and cornering frequencies       */
        flt_t dCoefficient1;
        flt_t dCoefficient2;
        flt_t dGain;

    /*we have at the moment only one sample rate (150 hz) */
    switch(mode)
    {
    case 0:
        dCoefficient1 = 0.7755049052;
        dCoefficient2 = -4.0746122421;
        dGain =  1.212806923e+07;
        break;
    case 1:
        dCoefficient1 = -0.1958157127;
         dCoefficient2 = 0.3695273774;
        dGain = 4.840925170e+00;
        break;
    default:
        dCoefficient1 = -1.0;
        dCoefficient2 = -1.0;
        dGain = -1.0;
        break;
    }

        flt_t xv[] = {0.0, 0.0, 0.0};
        flt_t yv[] = {0.0, 0.0, 0.0};

        for(int i = 0; i < inputArr->length(); i++)
        {
        xv[0] = xv[1];
        xv[1] = xv[2];
        xv[2] = inputArr->at(i) / dGain;
        yv[0] = yv[1];
        yv[1] = yv[2];
        yv[2] = (xv[0] + xv[2]) + 2.0 * xv[1]  + (dCoefficient1 * yv[0]) +
                (dCoefficient2 * yv[1]);

        outputArr->insert(i,yv[2]);
        }

    return;

}

void mathdetection::butterworth(QList<flt_t>* inputArr, int mode)
{
/*!
    *******************************************************************************\n
    * Function:      ButterworthFilter\n
    *\n
    * Description:   This function uses the Butterworth filter and returns a new\n
    *                value for an individual flt_ting point value.\n
    *\n
    ******************************************************************************\n
    * Butterworth Bandpass filter \n
    * sample rate - 150 \n
    *\n
    * mode = 0 - for step - \n
    *      - check with 5nd order \n
    *      - corner frequence = 1.875 \n
    *\n
    * mode = 1 - for stair - \n
    *      - check with 5nd order \n
    *      - corner frequence = 14 \n
*/

    /* coefficients will vary depending on sampling rate */
    /* and cornering frequencies       */
        flt_t dCoefficient1;
        flt_t dCoefficient2;
        flt_t dGain;

    /*we have at the moment only one sample rate (150 hz) */
        /*we have at the moment only one sample rate (150 hz) */
        switch(mode)
        {
        case 0:
             dCoefficient1 = 0.7755049052;
             dCoefficient2 = -4.0746122421;
             dGain =  1.212806923e+07;
            break;
        case 1:
            dCoefficient1 = -0.1958157127;
             dCoefficient2 = 0.3695273774;
            dGain = 4.840925170e+00;
            break;
        default:
            dCoefficient1 = -1.0;
            dCoefficient2 = -1.0;
            dGain = -1.0;
            break;
        }

        flt_t xv[] = {0.0, 0.0, 0.0};
        flt_t yv[] = {0.0, 0.0, 0.0};

        for(int i = 0; i < inputArr->length(); i++)
        {
        xv[0] = xv[1];
        xv[1] = xv[2];
        xv[2] = inputArr->at(i) / dGain;
        yv[0] = yv[1];
        yv[1] = yv[2];
        yv[2] = (xv[0] + xv[2]) + 2.0 * xv[1]  + (dCoefficient1 * yv[0]) +
                (dCoefficient2 * yv[1]);
        inputArr->replace(i,yv[2]);
        }

return;

}

void mathdetection::checkXtreme(QList<flt_t>* inputArr, QList<flt_t>* Trough, QList<flt_t>* Peak, flt_t threshold)
{
    /*!
    %\n
    % Program to find the location of the local minima and maxima in the decomposed\n
    % version of an arbitrarily large data set 'dataX'. The program outputs the\n
    % locations of all the extreme points in the array 'uT' and 'uP'.\n
    %\n
    % INPUTS:\n
    %   (inputArr) dataX       :   X axis acceleration data after wavelet decomposition for PT check.\n
    %\n
    % OUTPUTS:\n
    %  Trough uT          :   vector containing the index of the trough points.\n
    %  Peak uP          :   vector containing the index of the peak points.\n
    %\n
    % Usage:\n
    %\n
    % [LocalMinIndex LocalMaxIndex] = checkXtreme(dataset);\n
    %\n
    % Note:\n
    % The total number of data points processed by the program should be an integer\n
    % multiple of the size of the window frame.\n
  */
    const int nx = inputArr->length();
    const float kth = 0.0001;
    int win = 128; // optional windowing of the data (hamming!!!)
    int nw = win;
    int pos = 0;
    double xMean;
    QList<flt_t> xAc, cElem, xAc_t, Pe_t, Pe_p;

    /*%if length of input vector is smaller than, hamming-window -> adjust length
    %of hamming - window */
    if (nx < win)
    {
        win = nx;
        nw = nx;
    }
    else if( nx < 110)
    {
        win = nx;
        nw = nx;
    }


    while ((pos + nw) < (nx))             //% stop when the last element of the signal is reached
    {
        xAc.clear();
        xAc_t.clear();
        cElem.clear();

        xMean = meanV(inputArr, pos, pos + nw);

        for (int k = pos; k < pos + nw; k++)
        {
            xAc.append(inputArr->at(k) - xMean);
        }

        /*
        yac_t = -1*yac;                     % defining the trough yac value
        yac_p = 1*yac;                      % defining the peak yac value */
        neg(&xAc, &xAc_t);
        //%%%% process window y %%%%
        //TROUGH DETECTION
        if (threshold == -1.0)
        {
            int jk;

            cElem = kclustering(&xAc_t, kth);

            if(abs(cElem.at(0)) > abs(cElem.at(1)))
                jk = cElem.at(0);
            else
                jk = cElem.at(1);

            algodetect(&xAc_t, &Pe_t, jk); //%0.05);
        }
        else
        {
            algodetect(&xAc_t, &Pe_t, threshold);
        }

        for(int l = 0; l < Pe_t.length(); l++)
        {
            Trough->append(pos + Pe_t.at(l));
        }
        //    i_t = i_t + 1;
        //    m_t = m_t + 1;

        //PEAK DETECTION
        cElem.clear();
        if (threshold == -1.0)
        {
            int jl;
            cElem = kclustering(&xAc, kth);

            if(abs(cElem.at(0)) > abs(cElem.at(1)))
                jl = cElem.at(0);
            else
                jl = cElem.at(1);

            algodetect(&xAc,&Pe_p, jl); //%0.05);
        }
        else
        {
            algodetect(&xAc, &Pe_p, threshold);
        }

        for(int l = 0; l < Pe_p.length(); l++)
        {
            Peak->append(pos+ Pe_p.at(l));
        }
        //i_p = i_p + 1;
        //m_p = m_p + 1;
        // %%%% end process window y %%%%

        // %difference between processed window and complete vector length
        if( pos + (double)nw/2.0 + nw > nx && (pos + nw) != nx  )
        {
            pos = round(pos + (double) nw/2.0 -1);
            nw = nx - pos;
        }
        else if((pos + nw) == nx)
        {
            pos = nx; //%break;;
        }
        else
        {
            pos = pos + nw/2;
        }

        /*% move on to next window frame*/
        Pe_p.clear();
        Pe_t.clear();

        //        while (k_t ~=length(Trough))
        //            k_t = k_t + 1;
        //
        //            trough = [trough Trough{k_t}];
        //        end
        //
        //        uT = round(unique(trough));
        //
        //        while (k_p ~=length(Peak))
        //            k_p = k_p + 1;
        //
        //            peak = [peak Peak{k_p}];
        //        end
        //
        //        uP = round(unique(peak));

    }
    //unique check
    unique(Trough);
    unique(Peak);

    return;
}


void mathdetection::checkXtreme(QList<flt_t>* inputArr, QList<flt_t>* Trough, QList<flt_t>* Peak, flt_t threshold, int startIndex, int endIndex)
{
    /*!
    %\n
    % Program to find the location of the local minima and maxima in the decomposed\n
    % version of an arbitrarily large data set 'dataX'. The program outputs the\n
    % locations of all the extreme points in the array 'uT' and 'uP'.\n
    %\n
    % INPUTS:\n
    %   dataX       :   X axis acceleration data after wavelet decomposition for PT check.\n
    %\n
    % OUTPUTS:\n
    %   uT          :   vector containing the index of the trough points.\n
    %   uP          :   vector containing the index of the peak points.\n
    %\n
    % Usage:\n
    %\n
    % [LocalMinIndex LocalMaxIndex] = checkXtreme(dataset);\n
    %\n
    % Note:\n
    % The total number of data points processed by the program should be an integer\n
    % multiple of the size of the window frame.\n
  */
    const int nx = endIndex - startIndex;
    const float kth = 0.0001;
    int win = 128; // optional windowing of the data (hamming!!!)
    int nw = win;
    int pos = startIndex;
    double xMean;
    QList<flt_t> xAc, cElem, xAc_t, Pe_t, Pe_p;

    if( startIndex < 0 || endIndex > inputArr->length())
    {
        std::cerr << "ERROR: start Index is perhaps smaller than zero:" << startIndex << std::endl;
        std::cerr << "or endIndex is longer then inputArray. endIndex:  " << endIndex << " inputArr: " << inputArr->length() << std::endl;
    }


    /*%if length of input vector is smaller than, hamming-window -> adjust length
    %of hamming - window */
    if (nx < win)
    {
        win = nx;
        nw = nx;
    }
    else if( nx < 110)
    {
        win = nx;
        nw = nx;
    }


    while ((pos + nw) < (endIndex))             //% stop when the last element of the signal is reached
    {
        xAc.clear();
        cElem.clear();

        xMean = meanV(inputArr, pos, pos + nw);

        for (int k = pos; k < pos + nw; k++)
        {
            xAc.append(inputArr->at(k) - xMean);
        }

        /*
        yac_t = -1*yac;                     % defining the trough yac value
        yac_p = 1*yac;                      % defining the peak yac value */
        neg(&xAc, &xAc_t);
        //%%%% process window y %%%%
        //TROUGH DETECTION
        if (threshold == -1.0)
        {
            int jk;

            cElem = kclustering(&xAc_t, kth);
            //std::cout << "Wert 1 von cElem: " << cElem.at(0) << "Wert 2 von cElem: " << cElem.at(1) << std::endl;
            if(abs(cElem.at(0)) > abs(cElem.at(1)))
                jk = cElem.at(0);
            else
                jk = cElem.at(1);

            algodetect(&xAc_t, &Pe_t, jk); //%0.05);
        }
        else
        {
            algodetect(&xAc_t, &Pe_t, threshold);
        }

        for(int l = 0; l < Pe_t.length(); l++)
        {
            Trough->append(pos + Pe_t.at(l));
        }
        //    i_t = i_t + 1;
        //    m_t = m_t + 1;

        //PEAK DETECTION
        cElem.clear();
        if (threshold == -1.0)
        {
            int jl;
            cElem = kclustering(&xAc, kth);

            if(abs(cElem.at(0)) > abs(cElem.at(1)))
                jl = cElem.at(0);
            else
                jl = cElem.at(1);

            algodetect(&xAc,&Pe_p, jl); //%0.05);
        }
        else
        {
            algodetect(&xAc, &Pe_p, threshold);
        }

        for(int l = 0; l < Pe_p.length(); l++)
        {
            Peak->append(pos+ Pe_p.at(l));
        }
        //i_p = i_p + 1;
        //m_p = m_p + 1;
        // %%%% end process window y %%%%

        // %difference between processed window and complete vector length
        if( pos + 2*nw > nx && (pos + nw + 1) != endIndex  )
        {
            pos = pos + nw -1;
            nw = endIndex - pos - 1;
        }
        else if((pos + nw + 1) == endIndex)
        {
            pos = endIndex;
        }
        else
        {
            pos = pos + nw;
        }

        /*% move on to next window frame*/
        Pe_p.clear();
        Pe_t.clear();
    }
    //unique check
    unique(Trough);
    unique(Peak);

    return;
}

void mathdetection::unique(QList<flt_t>* inputArr)
{
    int index = 0;
    int i = 0;
    qSort(inputArr->begin(), inputArr->end(), qLess<flt_t>());
    while(i < inputArr->length() - 1)
    {
        while(true)
        {
            index = inputArr->indexOf(inputArr->at(i),i+1);
            if(index == -1)
            {
                break;
            }
            else
            {
                inputArr->removeAt(index);
            }
        }
        i += 1;
    }
    return;
}

void mathdetection::zeroCrossing(QList<flt_t>* inputArr, float offset, int mode, QList<flt_t>* crossingpoints , QList<flt_t>* p)
{
    /*!
%   Mode determines the way of calculating the zero crossing points.\n
%   mode=0: every zero crossing point is detected - u really don't care! \n
%   mode=1: only neg-pos transitions are detected - increasing! \n
%   mode=2: only pos-neg transitions are detected - declining! \n
*/
    QList<flt_t> internalArr, crossingpoints_raw, crossingpoints_raw2;

    addScalar2QList(inputArr, offset, &internalArr);
    sign(&internalArr, &crossingpoints_raw);
    diff(&crossingpoints_raw, &crossingpoints_raw2, 0, crossingpoints_raw.length());

    //clear
    crossingpoints->clear();
    p->clear();

    // transitions in both direction...
    if (mode == 0)
    {
        for(int i = 0; i < crossingpoints_raw2.length(); i++)
        {
            if( crossingpoints_raw2.at(i) == -2 || crossingpoints_raw2.at(i) == +2)
            {
                crossingpoints->append(i);
            }
        }

        //check if t has the last index , if yes delete
        if (crossingpoints->isEmpty() == false)
        {
            for (int i = 0; i < crossingpoints->length(); i++)
            {
                p->insert(i, internalArr.at(crossingpoints->at(i)));
            }
        }
        else
        {
            p->clear();
        }
    }
    else if (mode == 1)
    {
        for(int i = 0; i < crossingpoints_raw2.length(); i++)
        {
            if(crossingpoints_raw2.at(i) == +2)
            {
                crossingpoints->append(i);
            }
        }

        //check if t has the last index , if yes delete
        if (crossingpoints->isEmpty() == false)
        {
            for (int i = 0; i < crossingpoints->length(); i++)
            {
                p->insert(i, internalArr.at(crossingpoints->at(i)));

            }
        }
        else
        {
            p->clear();
        }
    }
    else if(mode == 2)
    {

        for(int i = 0; i < crossingpoints_raw2.length(); i++)
        {
            if(crossingpoints_raw2.at(i) == -2)
            {
                crossingpoints->append(i);
            }
        }

        //check if t has the last index , if yes delete
        if (crossingpoints->isEmpty() == false)
        {
            for (int i = 0; i < crossingpoints->length(); i++)
            {
                p->insert(i, internalArr.at(crossingpoints->at(i)));

            }
        }
        else
        {
            p->clear();
        }
    }
    return;
}

void mathdetection::sign(QList<flt_t>* inputArr, QList<flt_t>* outputArr)
{
    for(int i = 0; i < inputArr->length(); i++)
    {
        if(inputArr->at(i) == 0.0)
        {
            outputArr->append(0);
        }
        else if( inputArr->at(i) < 0.0)
        {
            outputArr->append(-1);
        }
        else
        {
            outputArr->append(+1);
        }
    }
    return;
}

double mathdetection::sum(QList<flt_t>* inputArr,int startIndex, int endIndex)
{
    double sum = 0.0;

    for(int i = startIndex; i <= endIndex; i++)
    {
        sum += inputArr->at(i);
    }

    return sum;
}
