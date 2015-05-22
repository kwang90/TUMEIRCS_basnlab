#include "processdata.h"


processdata::processdata()
{
   md = new mathdetection();
   lc = new lococheck(md);
   blocksProcessed = 0;
   formerActivity = -1;
   indexWalkingSteps.clear();
   holdValue = 0;
   currentIntegralState = INITIAL_STATE;


}


int processdata::startDetection(QList<flt_t>* dataXInput, QList<flt_t>* dataYInput, QList<flt_t>* dataZInput)
{

   deleteOldArrays();
   checkInputArrays(dataXInput, dataYInput, dataZInput);

   //create object!
   if (!lc->updateXYZ(dataXInput, dataYInput, dataZInput))
   {
      return 3;
   }
   lc->checkForStep(&walking, &running, &StepsW, &StepsR);

   //        if (StepsW.isEmpty() == false)
   //        {
   //        lc->stairscheck(&StepsW, &StairsW, &walkOrStair);
   //        }
   //        else
   //        {
   //            StairsW.clear();
   //        }

   //save new steps and delete previous ones
   saveStepsWR();
   blocksProcessed++;






   if(StepsW.isEmpty() == 0)
   {
      //if(walkOrStair.)  add here walking check result
      formerActivity = 1;
      return 1; //walking step detected
   }
   else if (StepsR.isEmpty() == 0)
   {
      formerActivity = 2;
      return 2; //running step detected
   }
   else if (StepsR.isEmpty() == 1 && StepsW.isEmpty() == 1)
   {
      if(formerActivity == 1)
      {
         formerActivity = -1;
         return 1;
      }
      else
      {
         if (formerActivity == 2 )
         {
            formerActivity = -1;
            return 2;
         }
         else
         {
            return 3;
         }
      }
   }
   else
   {
      return 0; //none of this above -> error
   }

}

int processdata::saveStepsWR()
{
   //explantion about Blocks:
   //fB_k = is values of former Block at time k
   //iB_k = value of new incoming Block at time k
   //There are three incoming blocks to look at. Each Block which are calculated in the algorithm
   // consists of fB_k-1 + iB_k. The StepsRold etc. includes fB_k-1 + iB_k.
   //For a new calculation fB_k + iB_k+1 is the new Block. Thus there is an overlapping of : fB_k == iB_k
   // Delete one of that, so as a result there is: fB_k-1 + (iB_k OR fB_k) + iB_k+1

   //save also detected running/walking steps for one block
   //if return = 0 --> steps running and walking
   //if return = 1 --> no steps running
   //if return = 2 --> no steps walking
   //if return = 3 --> no steps running/walking

   int verifyResult = 0;

   if(!StepsR.isEmpty())
   {
      //      if(indexRunningSteps.isEmpty() == true)
      //      {
      //         indexRunningSteps = StepsR;
      //      }
      //      else
      //      {
      //         checkIfRunningIsNewInformation();
      //         //deleteTooOldStepsR();
      for(int i = 0; i < StepsR.length(); i++)
      {
         //if(checkRunning.at(i) == 1)
         {
            indexRunningSteps.append(StepsR.at(i) + NUMBER_OF_SAMPLES_PER_BLOCK * (blocksProcessed+1));
         }
      }
      md->unique(&indexRunningSteps);
      if ((indexRunningSteps.size() - StepsR.size()) > 10)
      {
         for (int i=indexRunningSteps.size()-StepsR.size()-10; i<indexRunningSteps.size()-1; i++)
         {
            if ((indexRunningSteps.at(i+1)-indexRunningSteps.at(i))<35)
            {
               indexRunningSteps.removeAt(i);
            }
         }
      }
   }
   else
   {
      //deleteTooOldStepsR();
      verifyResult = 1;
   }

   if(!StepsW.isEmpty())
   {
      //        if(indexWalkingSteps.isEmpty() == true)
      //        {
      //            indexWalkingSteps = StepsW;
      //        }
      //        else
      //        {
      //            checkIfWalkingIsNewInformation();
      //deleteTooOldStepsW();

      for(int i = 0; i < StepsW.size(); i++)
      {
         motdetsat::getInstance()->deliver("StepSW: " + QString::number(StepsW.at(i)));
         // motdetsat::getInstance()->deliver("Processed: " + QString::number(blocksProcessed));
         indexWalkingSteps.append(StepsW.at(i) + NUMBER_OF_SAMPLES_PER_BLOCK * (blocksProcessed+1));
         // if(checkWalking.at(i) == 1)
         // {
         //checkForTooCloseSteps();
         //  }
         //}
      }
      md->unique(&indexWalkingSteps);

      if ((indexWalkingSteps.size() - StepsW.size()) > 10)
      {
         for (int i=indexWalkingSteps.size()-StepsW.size()-10; i<indexWalkingSteps.size()-1; i++)
         {
            if ((indexWalkingSteps.at(i+1)-indexWalkingSteps.at(i))<THRESHOLD_IN_CHECKMINDIST)
            {
               //                qDebug("deleting step %d", (int) indexWalkingSteps.at(i));
               //                qDebug("NOT deleting step %d", (int) indexWalkingSteps.at(i+1));
               indexWalkingSteps.removeAt(i);
            }
         }

      }
   }
   else
   {
      // deleteTooOldStepsW();
      if(verifyResult == 1)
      {
         verifyResult = 3;
      }
      else
      {
         verifyResult = 2;
      }
   }

   return verifyResult;
}


//void processdata::deleteTooOldStepsR()
//{
//    //Consider only 2 calculations
//    for(int i = 0; i < indexRunningSteps.length(); i++)
//    {
//        if(indexRunningSteps.at(i) < VALUES_OF_FORMER_BLOCK)
//        {
//            indexRunningSteps.insert(i,0.0);
//        }
//    }
//    indexRunningSteps.removeAll(0.0);
//    return;
//}
//
//void processdata::deleteTooOldStepsW()
//{
//    //Consider only 2 calculations
//    for(int i = 0; i < indexWalkingSteps.length(); i++)
//    {
//        if(indexWalkingSteps.at(i) < VALUES_OF_FORMER_BLOCK)
//        {
//            indexWalkingSteps.insert(i,0.0);
//        }
//    }
//    indexWalkingSteps.removeAll(0.0);
//    return;
//}

void processdata::checkIfRunningIsNewInformation()
{
   for(int i = 0; i < StepsR.length(); i++)
   {
      if(StepsR.at(i) < VALUES_OF_FORMER_BLOCK)
      {
         checkRunning.insert(i, 0);
      }
      else
      {
         checkRunning.insert(i, 1);
      }
   }

   return;
}

void processdata::checkIfWalkingIsNewInformation()
{
   for(int i = 0; i < StepsW.length(); i++)
   {
      if(StepsW.at(i) < VALUES_OF_FORMER_BLOCK)
      {
         checkWalking.insert(i, 0);
      }
      else
      {
         checkWalking.insert(i, 1);
      }
   }

   return;
}


QList<flt_t> processdata::getTimeDifferencesWalking()
{
   QList<flt_t>  timeDifference;

   //actually no step
   if(indexWalkingSteps.isEmpty() == true)
   {
      return timeDifference;
   }
   else
   {
      for(int i = 0; i < indexWalkingSteps.length() - 1; i++)
      {
         timeDifference.insert(i, (indexWalkingSteps.at(i+1) - indexWalkingSteps.at(i))/SAMPLE_RATE );
      }
   }
   return timeDifference;
}

QList<flt_t> processdata::getTimeDifferencesRunning()
{
   QList<flt_t>  timeDifference;

   //actually no step
   if(indexRunningSteps.isEmpty() == true)
   {
      return timeDifference;
   }
   else
   {
      for(int i = 0; i < indexRunningSteps.length() - 1; i++)
      {
         timeDifference.insert(i, (indexRunningSteps.at(i+1) - indexRunningSteps.at(i))/SAMPLE_RATE );
      }
   }
   return timeDifference;
}



processdata::~processdata()
{
   delete lc;
}

void processdata::deleteDetectionOfThePreviousBlock(){

   //NOT USED !!
   if(StepsR.isEmpty() == false)
   {
      for(int i = 0; i < StepsR.length(); i++)
      {
         if(StepsR.at(i) < VALUES_OF_FORMER_BLOCK)
         {
            StepsR.removeAt(i);
         }
      }
   }

   if(StepsW.isEmpty() == false)
   {
      for(int i = 0; i < StepsW.length(); i++)
      {
         if(StepsW.at(i) < VALUES_OF_FORMER_BLOCK)
         {
            StepsW.removeAt(i);
         }
      }
   }
   return;
}

bool processdata::checkInputArrays(QList<flt_t>* dataXInput,QList<flt_t>* dataYInput,QList<flt_t>* dataZInput)
{
   if(dataXInput->isEmpty() || dataYInput->isEmpty() || dataZInput->isEmpty())
   {
      std::cerr << "ERROR: one of the input acceleration data has no elements." << std::endl;
      std::cerr << " X - Acceleration data has:" << dataXInput->length() << "entries" << std::endl;
      std::cerr << " Y - Acceleration data has:" << dataYInput->length() << "entries" << std::endl;
      std::cerr << " Z - Acceleration data has:" << dataZInput->length() << "entries" << std::endl;
      return false;
   }
   return true;
}



QList<flt_t> processdata::getStepsW()
{
   return indexWalkingSteps;
}
QList<flt_t> processdata::getStepsR()
{
   return indexRunningSteps;
}

QList<flt_t> processdata::getStairsW()
{
   return StairsW;
}

QList<flt_t> processdata::getWalking()
{
   return walking;
}

QList<flt_t> processdata::getRunning()
{
   return running;
}



QList<flt_t> processdata::getPositionOfSteps()
{
   return indexWalkingSteps;
}

void processdata::deleteOldArrays()
{
   //delete old arrays
   StepsR.clear();
   StepsW.clear();
   StairsW.clear();
   running.clear();
   walking.clear();
   checkRunning.clear();
   checkWalking.clear();
   return;
}

float processdata::integrateAcc(QList<float> *checkList, int negativeIntegral)
{
   checkList = lc->getDataX();
   int totalValues = checkList->count();
   negativeIntegral = 0; // debug
   //    int areaTreshold = ui.sbMonTreshold->value();
   //float areaEpsilon = 100 ;
   //   float areaEpsilon = ui.sbMonEpsilon->value() / 34.1;
   float areaEpsilon = 100.0 / 34.1;

   float areaTreshold = -1.0;
   float tmpArea = 0.0;
   int areaCounter = 0;
   float meanArea = 0.0;
   bool countingArea = 0;
   float currentValueDelta = 0.0;
   float meanAreaMemory[10];
   int i;
   for(i=0 ; i<totalValues; i++)
   {
      currentValueDelta = (checkList->at(i) - areaTreshold) ;//* (1 - (2 * negativeIntegral));
      if(currentValueDelta > 0)
      {
         tmpArea += currentValueDelta/10.0;
         countingArea = 1;
      }
      if(currentValueDelta <= 0)
      {
         if (countingArea == 1)
         {
            meanArea += tmpArea;
            // if (tmpArea > areaEpsilon)
            {
               meanAreaMemory[areaCounter] = meanArea * 341.0;
               meanArea = 0;
               areaCounter++;
            }
         }
         countingArea = 0;
         tmpArea = 0;
      }
   }
   if (areaCounter > 0)
      meanArea = meanArea / areaCounter;
   meanArea = meanArea * (341.0/10.0); //skalierung
   return meanArea;
}

QList<flt_t>
      processdata::PositiveIntegral(QList<flt_t> *data)
{
   int i=0;
   flt_t tmp;
   for (i=0; i<data->length();i++)
   {
      tmp = PositiveIntegralImpl(data->at(i));
      if (tmp > 0.0)
         areaParts1.append(tmp);
   }
   return areaParts1;
}


QList<flt_t>
processdata::integrateValue(QList<flt_t> *data)
{
   flt_t checkValue;
   flt_t integrateThreshold = -1.0;
   flt_t myEpsilon = 0.0; //100.0 / 34.1;

   for(int a=0;a<data->count();a++)
   {
      //evtl skalierungen
      checkValue = data->at(a) - integrateThreshold;
      integrateValueImpl(checkValue);
      //ui.listWidget->addItem(QString("currIntegrate ")+ QString::number(currentIntegralState));
      //   ui.listWidget->addItem(QString("currIntegrate ")+ QString::number(checkValue*(34.1)));
      if (currentIntegralState == COUNTING_POSITIVES)
      {
         tmpArea += checkValue;
         // ui.listWidget->addItem(QString("1! "));
      }
      if (currentIntegralState == NULL_DETECTED)
      {
         //            ui.listWidget->addItem(QString("currIntegrate ")+ QString::number(tmpArea));
         //            ui.listWidget->addItem(QString("currIntegrate ")+ QString::number(myEpsilon));
         if (tmpArea < myEpsilon)
         {
            //  ui.listWidget->addItem(QString("2! "));
            holdValue += tmpArea;
         }
         else
         {
            //                ui.listWidget->addItem(QString("3! "));
            areaParts.append((tmpArea + holdValue));//*(34.1));
            holdValue = 0;
            //ui.listWidget->addItem(QString("V ") + QString::number(areaParts.at(areaParts.count()-1)));
         }
         tmpArea = 0;
      }
   }

 return areaParts;
}




void processdata::integrateValueImpl(flt_t myValue)
{
   // 0=INIT 1= pos 1=NULL 2=NEG
   //qDebug("currInt %i", currentIntegralState);
   switch(currentIntegralState)
   {
   case INITIAL_STATE:
      if (myValue < 0 )
         currentIntegralState = DROPPING_NEGATIVES;
      //        else
      //            currentIntegralState = INITIAL_STATE;   NO CHANGE - KEEP STATE
      break;
   case COUNTING_POSITIVES:
      if (myValue < 0 )
         currentIntegralState = NULL_DETECTED;
      //        else
      //            currentIntegralState = COUNTING_POSITIVES;   NO CHANGE - KEEP STATE
      break;
   case NULL_DETECTED:
      if (myValue < 0 )
         currentIntegralState = DROPPING_NEGATIVES;
      else
         currentIntegralState = COUNTING_POSITIVES;
      break;
   case DROPPING_NEGATIVES:
      if (myValue < 0 )
         currentIntegralState = DROPPING_NEGATIVES;
      else
         currentIntegralState = COUNTING_POSITIVES;
      break;
   }
}


flt_t  processdata::PositiveIntegralImpl(flt_t data)
{
   flt_t lTempIntegral = 0.0;
   static flt_t lIntegral;
   static int sPosNeg = 2;
   flt_t CrossingThreshold = -1.0;


   if (sPosNeg == 1)  //positive
   {
      if (data < CrossingThreshold)
      {
         sPosNeg = 2;
         lTempIntegral = lIntegral;
         lIntegral = 0.0;
//			DEBUGVAL("\nnegative Crossing at %lu", lTempIntegral);
         return (lTempIntegral);
      }
      else
      {
         lIntegral += (data-CrossingThreshold);// / 10.0;
         return 0;
      }
   }
   else //negative
   {
      if (data > CrossingThreshold)
      {
         sPosNeg = 1;
         lIntegral = data-CrossingThreshold;
//			DEBUGVAL("\npositive Crossing at %lu", lTempIntegral);
         return 0;
      }
      else
      {
         return 0;
      }
   }
}
