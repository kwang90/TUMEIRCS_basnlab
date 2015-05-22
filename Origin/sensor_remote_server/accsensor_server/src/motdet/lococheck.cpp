#include "lococheck.h"
#include <iostream>

lococheck::lococheck(mathdetection *mdtmp, QObject *parent) : QObject(parent)
{
   pos = 0;
   nw = 0;

   // Predefined threshold values for the respective variances
   Xth = 0.005;
   Yth = 0.005;
   Zth = 0.005;


   md = mdtmp;

   DNmag = new QList<flt_t>;

   // default value OK
   sp_th_r = 100.0;

   THRESHOLD_WALKINGCHECK_Z = 20.0;
   THRESHOLD_WALKINGCHECK_Y = 0.01;

   dataXold.clear();
   dataYold.clear();
   dataZold.clear();
}

int lococheck::updateXYZ(QList<flt_t>* X, QList<flt_t> *Y, QList<flt_t>* Z)
{
   QList<flt_t>::iterator lIteratorX=dataX.begin();
   QList<flt_t>::iterator lIteratorY=dataY.begin();
   QList<flt_t>::iterator lIteratorZ=dataZ.begin();

   int lValidBlock=0;


   if (dataX.size() >= (NR_OF_BLOCKS_ANALYSED*NUMBER_OF_SAMPLES_PER_BLOCK))
   {

      // delete old data
      dataX.erase(lIteratorX, lIteratorX+NUMBER_OF_SAMPLES_PER_BLOCK);
      dataY.erase(lIteratorY, lIteratorY+NUMBER_OF_SAMPLES_PER_BLOCK);
      dataZ.erase(lIteratorZ, lIteratorZ+NUMBER_OF_SAMPLES_PER_BLOCK);
      lValidBlock = 1;
   }
   // append new data
   dataX.append(*X);
   dataY.append(*Y);
   dataZ.append(*Z);

   nw = dataX.size();
   return lValidBlock;
}


void lococheck::checkForStep(QList<flt_t>* walking, QList<flt_t>* running, QList<flt_t>* StepsW, QList<flt_t>* StepsR)
{

   pos = 0;
   //initialize local variables
   double xmean;

   QList<flt_t> Xr, Xi,zero, steps_p, steps_w, cElem;
   QList<flt_t> DN,Trough,Peak;

   static unsigned int lCallCounter=0;

   // check, if input block is big enough, skip first blocks...
   if( dataX.length() < (NR_OF_BLOCKS_ANALYSED*NUMBER_OF_SAMPLES_PER_BLOCK))
   {
      return;
   }
   // pos = 0; nw = NUMBER_OF_SAMPLES_PER_BLOCK*NR_OF_BLOCKS_ANALYSED
   if (restingCheck(&dataX, &dataY, &dataZ, pos, pos + nw) == false) //equal to zero!
   {
      //Calculating energy of x - Axe in window
      xac = dataX;
      xmean = md->meanV(&xac, pos, pos+nw);
      md->addScalar2QList(&xac, -xmean);

      // Doing FFT
      //zero has to be empty, if only QLists!
      Xr.clear(); Xi.clear(); DN.clear(); DNmag->clear();
      md->do_fft(1, xac.length(), &Xr, &Xi, &xac ,&zero);
      md->absV(&Xr,&Xi,&DN);
      md->fftshift(&DN, DNmag);
      DNmagOrig = *DNmag;
      // Matlab: fix(...)
      for(int i = 0; i < DNmag->length(); i++)
      {
         DNmag->replace(i,floor(DNmag->at(i)/sp_th_r) * sp_th_r);
      }

      MainLowFreq_p.clear();
      md->algodetect(DNmag, &MainLowFreq_p, THRESHOLD_ALGODETECT);


      // detect running...
      if(MainLowFreq_p.length() == 2)
      {
         running->append(1);
         walking->append(0);

         //getSteps(&xac, 0.1, 0, 50, &steps_p);
         cElem.clear();
         cElem = md->kclustering(&xac, 0.1);

         // get maximum of both elements
         float jk;
         if(cElem.at(0) < cElem.at(1))
            jk = cElem.at(1);
         else
            jk = cElem.at(0);

         steps_p.clear();
         md->algodetect(&xac, &steps_p ,jk);

         // Check function here to ensure that detected peaks are not
         // too closed to one another
         if(!steps_p.isEmpty())
         {
            // TODO: Check checkMinDist();
            md->checkMinDistz(&steps_p, &xac, pos, 35.0);
         }
         //TODO Idx adjustment
         steps_p.removeAll(0);

         //md->addScalar2QList(&steps_p, lCallCounter*NUMBER_OF_SAMPLES_PER_BLOCK, StepsR);
         *StepsR = steps_p;
      }
      else // Walking ?
      {
         *DNmag = DNmagOrig;
         // in everycase we aren't running...
         running->append(0);
         MainLowFreq_p.clear();

         //walkingcheck();
         // MATLAB: fix()
         for(int i = 0; i < DNmag->length(); i++)
         {
            DNmag->replace(i,floor(DNmag->at(i)/THRESHOLD_WALKINGCHECK_Z) * THRESHOLD_WALKINGCHECK_Z);
         }
         md->algodetect(DNmag, &MainLowFreq_p, 6.0);
         if(!MainLowFreq_p.isEmpty() && md->var(&dataY, 0, dataY.length()) > THRESHOLD_WALKINGCHECK_Y )
         {
            cElem.clear();
            cElem = md->kclustering(&xac, 0.1);

            // get maximum of both elements
            float jk;
            if(cElem.at(0) < cElem.at(1))
               jk = cElem.at(1);
            else
               jk = cElem.at(0);

            //clear steps
            steps_w.clear();
            md->algodetect(&xac, &steps_w ,jk);
            // Check function here to ensure that detected peaks are not
            // too closed to one another
            if(!steps_w.isEmpty())
            {
               walking->append(1);
               md->checkMinDistz(&steps_w, &dataX, pos, THRESHOLD_IN_CHECKMINDIST);
               steps_w.removeAll(0);

               //md->addScalar2QList(&steps_w, lCallCounter*NUMBER_OF_SAMPLES_PER_BLOCK, StepsW);
               *StepsW = steps_w;
            }
            QApplication::processEvents();
         }
         else
         {
            walking->append(-1);
            running->append(-1);
         }
      }
   }
   else
   {
      //we are resting
      walking->append(-1);
      running->append(-1);
   }

   //QApplication::processEvents();



   //delete zeros in StepsWalking StepsW

   //check for multiple entries of the same step / running in the qlist
   md->unique(StepsW);
   md->unique(StepsR);

    md->checkMinDistz(StepsR, &dataX, 0, 35);
    md->checkMinDistz(StepsW, &dataX, 0, THRESHOLD_IN_CHECKMINDIST);
   return;
}



void lococheck::stairscheck(QList<flt_t>* StepsW, QList<flt_t>* StairsW, QList<QString>* motionClass)
{
   /*!
    %
    % Program to differentiate between walking downstairs and walking\n
    % upstairs/level walking. For this purpose, the cross correlation between\n
    % the X- and Z-axis acceleration data is calculated for 15 data samples\n
    % before and after the index of a detected walking step.\n
    %\n
    % INPUTS:\n
    %   steps_w     :   Vector containing the index of all the walking steps\n
    %   dataX       :   Acceleartion data in the X axis\n
    %   dataZ       :   Acceleration data in the Z axis\n
    %   dataY       :   Acceleration data in the Y axis\n
    %\n
    % OUPUTS:\n
    %   stairs     :   Vector containing the indices of the detected walking\n
    %                  steps and the classification of the walking steps into\n
    %                  walking downstairs and walking upstairs/level walking. \n
    %                  '0' refers to walking upstairs/level walking. \n
    %                  '1' refers to walking downstairs,\n
    %\n
    % USAGE:\n
    % stairscheck(steps_w, dataX, dataZ)\n
    */

   QList<flt_t> xTP, zTP, troughX, troughZ, peakX, peakZ;
   QList<flt_t> differenceTOZMaxima, differenceTOXMaxima;


   StairsW->clear();

   //smoothening the x and z data
   md->butterworth(&dataX, &xTP, 1);
   md->butterworth(&dataZ, &zTP, 1);

   //check for maximas in the x / z data
   md->checkXtreme(&xTP, &troughX, &peakX, -1.0);
   md->checkXtreme(&zTP, &troughZ, &peakZ, -1.0);
   StairsW = StepsW;

   //    //take care of every detected walking step (iteratively)
   //    for(int i = 0; i < StepsW->length(); i++)
   //    {
   //
   //        //search nearest Maximum in z-Axe for step
   //        md->addScalar2QList(&peakZ, -StepsW->at(i), &differenceTOZMaxima);
   //        //get Minimum Z to StepW
   //        int minIndexZ = md->minimumItem(&differenceTOZMaxima);
   //        flt_t minValueZ = differenceTOZMaxima.at(minIndexZ);
   //
   //        //if minValue is bigger than zero, take first index
   //        if(minValueZ > 0)
   //        {
   //            minIndexZ = 0;
   //            minValueZ = differenceTOZMaxima.at(minIndexZ);
   //        }
   //
   //        //search nearest Maxium for closestNegativeValue in x-Axe Maximum
   //        md->addScalar2QList(&peakX, - peakZ.at(minIndexZ) , &differenceTOXMaxima);
   //
   //        //delete negative Values
   //        for(int i = 0; i < differenceTOXMaxima.length(); i++)
   //        {
   //            if(differenceTOXMaxima.at(i) < 0)
   //            {
   //                differenceTOXMaxima.removeAt(i);
   //            }
   //        }
   //
   //        //get Minimum X to Z
   //        int minIndexX = md->minimumItem(&differenceTOXMaxima);
   //
   //        StairsW->append(peakX.at(minIndexX) + abs(floor(minValueZ + 15.0)/3.0));
   //    }

   //delete too close stairsSteps
   for(int i = 0; i < StairsW->length() - 1; i++)
   {
      if(StairsW->at(i) + 20 > StairsW->at(i + 1))
      {
         StairsW->removeAt(i);
      }
   }

   QList<flt_t> valueX, indexX, valueX2, indexX2;

   md->zeroCrossing(&xTP, 1.0, 1, &indexX, &valueX);
   md->zeroCrossing(&xTP, 1.0, 2, &indexX2, &valueX2);
   //parameters with new indices for new range
   int minimumDistanceBefore = 0;
   int minimumDistanceAfter = 0;
   int minIndexBefore = 0;
   int minIndexAfter = 0;

   for(int i = 0; i < StairsW->length(); i++)
   {
      //get the first index of the new observed range, starts before STEP
      for(int m = 0; m < indexX.length(); m++)
      {
         if( m == 0 )
         {
            minimumDistanceBefore = indexX.at(m) - StairsW->at(i);
            minIndexBefore = m;
         }
         else if( indexX.at(m) - StairsW->at(i) < 0 && minimumDistanceBefore < indexX.at(m) - StairsW->at(i) )
         {
            minIndexBefore = m;
            minimumDistanceBefore = indexX.at(m) - StairsW->at(i);
         }
      }

      for(int m = 0; m < indexX2.length(); m++)
      {
         if(m == 0)
         {
            minimumDistanceAfter = indexX2.at(indexX2.length()-1);
         }
         //get the second index of the new observed range, ends after STEP
         if( indexX2.at(m) - StairsW->at(i) > 0 && minimumDistanceAfter > indexX2.at(m) - StairsW->at(i) )
         {
            minIndexAfter = m;
            minimumDistanceAfter = indexX2.at(m) - StairsW->at(i);
         }

      }
      //        QList<flt_t> dummyQList;
      //        md->addScalar2QList(&indexX2, - StairsW->at(i), &dummyQList);
      //        md->neg(&dummyQList);
      //        //md->absV(&dummyQList);
      //        int index2 =  md->minimumItem(&dummyQList);
      //
      //        for(int k = 0; k < indexX.length() - 1; k++)
      //        {
      //            if( indexX.at(minIndex) > indexX2.at(index2))
      //            {
      //                if(index2 == indexX2.length())
      //                {
      //                    if( xTP.length() >= indexX.at(minIndex) + 80 )
      //                    {
      //                        indexX2.replace(index2, indexX.at(minIndex) + 80);
      //                    }
      //                    else
      //                    {
      //                        indexX2.replace(index2, xTP.length());
      //                    }
      //                }
      //                else
      //                {
      //                    index2 = index2 + 1;
      //                }
      //            }
      //            else
      //            {
      //                break;
      //            }
      //            if(k == indexX.length() - 1)
      //                indexX2.replace(index2, indexX.at(minIndex) + 80);
      //        }

      startIndex = -1;
      endIndex = -1;

      //check if indices too close
      if( abs((float)(indexX.at(minIndexBefore) - indexX2.at(minIndexAfter)) < 40 && indexX2.length() > minIndexAfter + 1))
      {
         minIndexAfter += 1;
         startIndex = indexX.at(minIndexBefore);
         endIndex = indexX2.at(minIndexAfter);
      }
      else if(abs((float)(indexX.at(minIndexBefore) - indexX2.at(minIndexAfter))) < 40)
      {
         if(zTP.length() > startIndex + 63)
         {
            startIndex = indexX.at(minIndexBefore);
            endIndex = startIndex + 63;
         }
         else
         {
            startIndex = indexX.at(minIndexBefore);
            endIndex = zTP.length() - 1;
         }
      }
      else if( minimumDistanceBefore == 0 &&  minimumDistanceAfter == 0)
      {
         //break this step is no stair step
         motionClass->append(QString("walking"));
         continue;
      }
      else if( minimumDistanceBefore == 0 &&  minimumDistanceAfter != 0)
      {
         endIndex = indexX2.at(minimumDistanceAfter);

         if(endIndex > 63)
         {
            startIndex = indexX2.at(minimumDistanceAfter) - 64;
         }
         else
         {
            startIndex = 0;
         }
      }
      else if( minimumDistanceBefore != 0 &&  minimumDistanceAfter == 0)
      {
         startIndex = indexX.at(minimumDistanceBefore);
         if(zTP.length() > startIndex + 64)
         {
            endIndex = startIndex + 64;
         }
         else
         {
            endIndex = zTP.length() - 1;
         }
      }

      //std::cout << "STAIRS: value of startIndex: " << startIndex << "value of endIndex: " << endIndex << std::endl;

      //        //check Minima
      //        QList<flt_t> troughZerosX, peakZerosX;
      //        QList<flt_t> minimalValues, minimalIndex;
      //
      //        md->checkXtreme(&xTP, &troughZerosX, &peakZerosX,  -1.0, indexX.at(minIndexBefore), indexX2.at(minIndexAfter));
      //
      //
      //        for(int k = 0; k < troughZerosX.length(); k++)
      //        {
      //            if(dataX.at(indexX.at(minIndexBefore) + troughZerosX.at(k)) < -1)
      //            {
      //                minimalValues.append(xTP.at(indexX.at(minIndexBefore) + troughZerosX.at(k)));
      //                minimalIndex.append(indexX.at(minIndexBefore) + troughZerosX.at(k));
      //            }
      //        }
      //
      //        //if checkXtreme-fct could'nt find a minima, we usw the
      //        //minima-search
      //        if (minimalIndex.isEmpty() == true)
      //        {
      //            minimalIndex.append(md->minimumItem(&xTP, indexX.at(minIndexBefore), indexX2.at(minIndexAfter)));
      //        }
      //
      //        //define the start and end value of the window
      //        if (minimalValues.isEmpty() == true)
      //        {
      //            startIndex = indexX.at(minIndexBefore);
      //            endIndex = indexX2.at(minIndexAfter);
      //        }
      //        else
      //        {
      //            if (minimalValues.length() == 1)
      //            {
      //                if(minimalIndex.at(0) > indexX.at(minIndexBefore) + (indexX2.at(minIndexAfter) + indexX.at(minIndexBefore) / 2))
      //                {
      //                    startIndex = indexX.at(minIndexBefore);
      //                    endIndex = minimalIndex.at(0);
      //                }
      //                else
      //                {
      //                    startIndex = minimalIndex.at(0);
      //                    endIndex = indexX2.at(minIndexAfter);
      //                }
      //            }
      //            else
      //            {
      //                int absMinIndex1, absMinIndex2;
      //                //get smallest value
      //                int indexOfMinimumValues = md->minimumItem(&minimalValues);
      //                absMinIndex1 = minimalIndex.at(indexOfMinimumValues);
      //                //delete smallest value
      //                minimalValues.removeAt(indexOfMinimumValues);
      //                minimalIndex.removeAt(indexOfMinimumValues);
      //                //get second smallest value
      //                indexOfMinimumValues = md->minimumItem(&minimalValues);
      //                absMinIndex2 = minimalIndex.at(indexOfMinimumValues);
      //
      //                if(absMinIndex1 < absMinIndex2)
      //                {
      //                    startIndex = absMinIndex1;
      //                    endIndex = absMinIndex2;
      //                }
      //                else
      //                {
      //                    startIndex =absMinIndex2;
      //                    endIndex = absMinIndex1;
      //                }
      //            }
      //        }
      //        //too close area -> expand
      //        if( endIndex - startIndex < 15 )
      //            endIndex = startIndex + 25;


      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //%        check if step or stairs
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      QList<flt_t> trZ, peakZ;
      QList<QString> decideSG;
      double sum = 0.0;
      md->checkXtreme(&zTP, &trZ, &peakZ, -1.0, startIndex, endIndex);
      //std::cout << "Laenge von zTP: " << zTP.length() << "Lange von trZ: " << trZ.length() << "Laenge von peakZ: " << peakZ.length() << std::endl;

      if ( trZ.isEmpty() == false && peakZ.isEmpty() == false )
      {
         //            if(peakZ.at(0) > trZ.at(trZ.length()-1))
         sum = md->sum(&zTP, startIndex, endIndex);
      }
      //std::cout << "Wert von sum: " << sum << std::endl;


      //############################
      //not needed?
      if(sum != 0.0)
      {
         if( sum < 0.5 )
         {
            decideSG.append(QString("s"));
         }
         else
         {
            decideSG.append(QString("g"));
         }
      }
      else
      {
         decideSG.append(QString("g"));
      }
      QByteArray mBArray = decideSG.at(decideSG.length()-1).toAscii();
      //std::cout << "STAIRS: decideSG: " << mBArray.constData() << std::endl;
      //############################

      QList<flt_t> c, xInputXCORR, zInputXCORR, shiftC;
      //if the detected stair-step isn't between the barriers of the
      //start- and end Indices, there is no stairstep
      if( StairsW->at(i) > endIndex || StairsW->at(i) < startIndex)
      {
         motionClass->append(QString("unkown"));
         continue;
      }

      //Initialize QLists only with the desired values from start- to end Index. TODO: CHANGE xcorr and dft to start endincdics
      for(int k = startIndex; k < endIndex; k++)
      {
         xInputXCORR.append(dataX.at(k));
         zInputXCORR.append(dataZ.at(k));
      }
      md->do_xcorr(&c, &xInputXCORR, &zInputXCORR);

      md->butterworth(&c, 1);
      md->fftshift(&c, &shiftC);
      double MaximumValueOfCross = 0.0;
      if(c.length() > 1)
      {
         MaximumValueOfCross = md->maximumItem(&c, 0, c.length()-1);
         //std::cout << "Maximum Value of c: " << MaximumValueOfCross << std::endl;
      }

      //clear
      xInputXCORR.clear();
      zInputXCORR.clear();


      int fifth = floor(shiftC.length() / 5.0);

      int maximumOfCIndex = md->maximumItem(&shiftC, fifth, 4*fifth);
      int minimumOfCIndex = md->minimumItem(&shiftC, fifth, 4*fifth);

      if(MaximumValueOfCross > 4.0)
      {
         motionClass->append("Going stairs");
      }
      else
      {
         motionClass->append("walking");
      }



      c.clear();
   }
   //here end of for - loop for each stair-
   return;
}

bool lococheck::restingCheck(QList<flt_t>* dataX, QList<flt_t>* dataY,QList<flt_t>* dataZ, int startIndex, int endIndex)
{

   double varX, varY, varZ;
   bool resting = false;

   //Calculating the variances
   varX = md->var(dataX, startIndex, endIndex);
   varY = md->var(dataY, startIndex, endIndex);
   varZ = md->var(dataZ, startIndex, endIndex);

   // Checking for resting state
   if (varX < (float)Xth)
   {
      if (varZ < (float)Zth)
      {
         if (varY < (float)Yth)
         {
            resting = true;
         }
      }
   }
   else
   {
      resting = false;
   }
   return resting;
}

void lococheck::walkingcheck()
{

//   QList<flt_t> Xr,Xi, zero, DNmag_x, MainLowFreq_p, DNx;

//   //clear steps
//   steps.clear();

//   // MATLAB: fix()
//   for(int i = 0; i < DNmag_x.length(); i++)
//   {
//      DNmag_x.replace(i,floor(DNmag_x.at(i)/THRESHOLD_WALKINGCHECK_Z) * THRESHOLD_WALKINGCHECK_Z);
//   }

//   md->algodetect(&DNmag_x, &MainLowFreq_p, 6.0);

//   if(MainLowFreq_p.isEmpty() == false && md->var(&yWin, 0, yWin.length()) > THRESHOLD_WALKINGCHECK_Y )
//   {
//      getSteps(&xWin,0.1, 0, DISTANCE_IN_GETSTEPS,&steps);
//   }
//   else
//   {
//      steps.clear();
//   }

//   return;

}

void lococheck::getSteps(QList<flt_t>* inputArr, float clustThres, int pos, const int checkMinThres, QList<flt_t>* steps)
{
//   QList<flt_t> cElem = md->kclustering(inputArr, clustThres);
//   float jk;

//   if(cElem.at(0) < cElem.at(1))
//      jk = cElem.at(0);
//   else
//      jk = cElem.at(1);

//   md->algodetect1(inputArr,steps ,jk);

//   /* % Check function here to ensure that detected peaks are not too
//     % closed to one another */
//   if(steps->length() > 0)
//   {
//      md->checkMinDist(steps, inputArr, pos, checkMinThres);
//   }
//   return;
}

void lococheck::getWindow()
{
   QList<flt_t> diffDataX, Trough, Peak, dummyArray;

   dummyArray.clear();

   for(int i = pos; i < nw + pos; i++)
      dummyArray.append(addedXZ.at(i) * 10);

   md->diff(&dummyArray, &diffDataX, 0, dummyArray.length());
   md->butterworth(&diffDataX, 0);

   // check for troughs and peaks
   md->checkXtreme(&diffDataX, &Trough, &Peak, -1);


   //define start and end value of the window //bis hier ok
   if(Trough.length() == 0)
   {
      startIndex = pos;
      endIndex = pos + nw;
   }
   else
   {
      if(Trough.length() == 1)
      {
         if(Trough.at(0) > nw/2.0)
         {
            startIndex = pos;
            endIndex = pos + Trough.at(0);
         }
         else
         {
            startIndex = pos + Trough.at(0);
            endIndex = pos + nw;
         }
      }
      else
      {
         //get the smallest 2 values
         QList<flt_t> minValues;

         for(int k = 0; k < Trough.length(); k++)
         {
            minValues.append(diffDataX.at(Trough.at(k)));
         }

         int index1 = md->minimumItem(&minValues);
         minValues.removeAt(index1);
         int index2 = md->minimumItem(&minValues);

         if(index1 < index2)
         {
            startIndex = pos + Trough.at(index1);
            endIndex = pos + Trough.at(index2);
         }
         else
         {
            startIndex = pos + Trough.at(index2);
            endIndex = pos + Trough.at(index1);
         }
      }
   }
   return;
}

void lococheck::checkIfRunningWalkingToClose(QList<flt_t>* StepsW, QList<flt_t>* StepsR)
{
   QList<flt_t> minimumList, deleteIndexRunning,deleteIndexWalking;

   for(int i = 0; i < StepsW->length(); i++)
   {
      minimumList.clear();
      md->addScalar2QList(StepsR, -StepsW->at(i), &minimumList);
      md->absV(&minimumList);
      int minimumIndex = 0;

      while(true)
      {
         minimumIndex = md->minimumItem(&minimumList);
         if(minimumList.at(minimumIndex) < 40)
         {
            deleteIndexWalking.append(i);
            deleteIndexRunning.append(minimumIndex);
            minimumList.removeAt(minimumIndex);
         }
         else
         {
            break;
         }
      }
   }

   md->unique(&deleteIndexWalking);
   md->unique(&deleteIndexRunning);

   for(int i = 0; i < deleteIndexWalking.length(); i++)
   {
      StepsW->removeAt(deleteIndexWalking.at(i));
   }

   for(int i = 0; i < deleteIndexRunning.length(); i++)
   {
      StepsW->removeAt(deleteIndexRunning.at(i));
   }


   return;
}


lococheck::~lococheck()
{
   delete md;
   delete [] DNmag;
}

void lococheck::setTrRunning(double threshold)
{
   sp_th_r =  threshold;

}

void lococheck::setTrXth(double threshold)
{
   Xth = threshold;

}


void lococheck::setTrYth(double threshold)
{
   Yth = threshold;

}

void lococheck::setTrZth(double threshold)
{
   Zth = threshold;

}


void lococheck::setTrWalkingZ(double threshold)
{
   THRESHOLD_WALKINGCHECK_Z =  threshold;

}

void lococheck::setTrWalkingY(double threshold)
{

   THRESHOLD_WALKINGCHECK_Y = threshold;

}

