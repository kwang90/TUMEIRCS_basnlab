/*
 * libBPMTrafficStatistics.cpp
 *
 *  Created on: 17.05.2013
 *      Author: kindt
 */
#include "libBPMTrafficStatistics.hpp"
//#include <boost/numeric/ublas/io.hpp>
#include "debug.h"
#include <float.h>
#include "errorHandling.h"
libBPMTrafficStatistics::libBPMTrafficStatistics(uint32_t nClasses, libBPMTrafficMeasurement *tm) :
	bytesPerClass(nClasses),
	timePerClass(nClasses),
	classBorders(nClasses),
	classBordersAvg(nClasses),
	writesPerClass(nClasses),
	transitionCntByWrites(nClasses,nClasses),
	transitionCntByTime(nClasses,nClasses),
	transitionProbabByWrites(nClasses,nClasses),
	transitionProbabByTime(nClasses,nClasses)

{
	this->nClasses = nClasses;
	built = false;
	this->tm = tm;

}
uint32_t libBPMTrafficStatistics::getClass(const double &traffic) const{
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		if(traffic <= classBorders(cnt)){
			return cnt;
		}
	}
	return nClasses - 1;
}

void libBPMTrafficStatistics::build(){
	reset();
    //DEBUG2("build with "<<this->nClasses<<" traffic classes:");


	//build class borders
	double maxTraffic = tm->getMaxTraffic();
    //DEBUG2("maxTraffic="<<tm->getMaxTraffic());
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		classBorders(cnt) = maxTraffic / nClasses * (double) (cnt + 1);
		if(cnt > 0){
			classBordersAvg(cnt) = (classBorders(cnt - 1) + classBorders(cnt)) / 2.0;
		}else{
			classBordersAvg(0) = classBorders(0)/2.0;
		}
		timePerClass(cnt) = 0;
		bytesPerClass(cnt) = 0;
	}


	uint32_t trClass = 0;
	uint32_t trClassPrev = 0;
	bool first = true;
	for(int32_t cnt = tm->getBiggestIndex() - 1; cnt >= 0; cnt--){			//skip the last index as no duration is available for it
		trClass = getClass(tm->getTraffic(cnt));
        //DEBUG2("traffic["<<cnt<<"/"<<tm->getBiggestIndex()<<"] = "<<tm->getTraffic(cnt)<<" - trClass= "<<trClass);
		bytesPerClass(trClass) += tm->getNBytes(cnt);
		if(cnt < tm->getBiggestIndex()){
			timePerClass(trClass) += tm->getDuration(cnt);
		}
        //DEBUG2(cnt<<": Duration = "<<tm->getDuration(cnt)<<", nBytes = "<<tm->getNBytes(cnt));

		if(!first){
			writesPerClass(trClassPrev) += 1;

			if(trClassPrev != trClass){
                //DEBUG2("Transition from class "<<trClassPrev<<" to "<<trClass<<" @ cnt = "<<cnt<<", biggestIndex = "<<tm->getBiggestIndex());
				if(!first){
					transitionCntByWrites(trClassPrev,trClass) += 1;

					transitionCntByTime(trClassPrev,trClass) += tm->getDuration(cnt);
				}
			}else{
				transitionCntByWrites(trClass,trClass) += 1;
				transitionCntByTime(trClass,trClass) += tm->getDuration(cnt);

			}
		}
		first = false;


		trClassPrev = trClass;

	}
	/*
	double duration = tm->getGlobalTime();
	if(duration < 0){
		cout<<"Failure: global duration < 0 s"<<endl;
		exit(1);
	}
*/
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		for(uint32_t cnt2 = 0; cnt2 < nClasses; cnt2++){

			//calculations on per-byte-basis
			if(bytesPerClass(cnt) != 0){
				transitionProbabByWrites(cnt,cnt2) = (double)(transitionCntByWrites(cnt,cnt2)) / (double)(writesPerClass(cnt));
				transitionProbabByTime(cnt,cnt2) = (double)(transitionCntByTime(cnt,cnt2)) / (double)(timePerClass(cnt));


			}else{
					transitionProbabByWrites(cnt,cnt2) = 0.0;
			}
				//		}else{
		//			DEBUG2("Bytes per class = "<<bytesPerClass(cnt)<<" is zero for class "<<cnt<<", but tranisiton counter for class "<<cnt<<"->"<<cnt2<<" is "<<transitionCntByWrites(cnt,cnt2)<<" => Inconsitent data.");
		//			exit(1);
		//		}
		//	}
			if(transitionProbabByWrites(cnt,cnt2) > 1.0){
                //DEBUG2("Failure: Transition Probab ("<<cnt<<","<<cnt2<<") > 0!");
			//	exit(1);
			}
		}
	}



	built = true;
}


void libBPMTrafficStatistics::printHistographByBytes(){
	//	cerr<<"############################################[ traffic histograph / bytes per class]############################################"<<endl;
	uint32_t borderPrev = 0;
	uint32_t border = 0;

	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		borderPrev = border;
		border = classBorders(cnt);
	//	cerr<<setprecision(4)<<"["<<borderPrev<<"-"<<border<<"]\t\t";
	}
	//cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//	cerr<<bytesPerClass(cnt)<<"\t\t";
	}
	//cerr<<endl;
	//cerr<<"########################################[sum: "<<tm->getByteSum()<<"bytes ]##########################################################"<<endl;

}

void libBPMTrafficStatistics::printHistographByTime(){
	//cerr<<"############################################[ traffic histograph / time by class]############################################"<<endl;
	uint32_t borderPrev = 0;
	uint32_t border = 0;
	double durationSum = 0;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		borderPrev = border;
		border = classBorders(cnt);
		//		cerr<<"["<<borderPrev<<"-"<<border<<"]\t\t";
	}
	//cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//cerr<<timePerClass(cnt)<<"\t\t";
		durationSum += timePerClass(cnt);
	}
	//cerr<<endl;
	//cerr<<setprecision(4)<<"##################################################[ sum: "<<durationSum<<"]###########################################################"<<endl;

}

void libBPMTrafficStatistics::printTransitionMatrixByWrites(){
	//cerr<<"#########################################[ transition probability matrix by writes]########################################"<<endl;
	//cerr<<" \t";
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//		cerr<<"   ["<<cnt<<"]\t\t";
	}
	//cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//		cerr<<"["<<cnt<<"]\t";
		for(uint32_t cnt2 = 0; cnt2 < nClasses; cnt2++){
			//cerr<<setprecision(4)<<fixed<<transitionProbabByWrites(cnt2,cnt)<<"\t\t";
		}
		//resetiosflags(ios::scientific);
		//cerr<<endl;
	}
//	cerr<<endl;
//	cerr<<"###########################################################################################################################"<<endl;

}

void libBPMTrafficStatistics::printTransitionMatrixByTime(){
//	cerr<<"######################################[ transition probability matrix by time ]#############################################"<<endl;
//	cerr<<" \t";
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//		cerr<<"   ["<<cnt<<"]\t\t";
	}
	//	cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//		cerr<<"["<<cnt<<"]\t";
		for(uint32_t cnt2 = 0; cnt2 < nClasses; cnt2++){
			//			cerr<<setprecision(4)<<fixed<<transitionProbabByTime(cnt2,cnt)<<"\t\t";
		}
	//	resetiosflags(ios::scientific);
		//		cerr<<endl;
	}
	//	cerr<<endl;
	//	cerr<<"#############################################################################################################################"<<endl;

}

double libBPMTrafficStatistics::getTrafficPrediction(uint32_t n){
/*
	assert(built);
	boost::numeric::ublas::matrix<double> Mtmp(n,n);
	flat_array_double_t trafficObservation(nClasses);
	for(uint32_t i = 0; i < nClasses; i++){
		trafficObservation(i) = 0;
	}
	if(tm->getCurrentTraffic()>0){
	//	DEBUG2("tm->getCurrentTraffic()="<<tm->getCurrentTraffic()<<" => class= "<<getClass(tm->getCurrentTraffic()));
		trafficObservation(getClass(tm->getCurrentTraffic())) = 1.0;//classBorders(getClass(tm->getCurrentTraffic()));

	}else{
	//	DEBUG2("tm->getCurrentTraffic()="<<tm->getCurrentTraffic());

		trafficObservation(0) = 1.0;


	}
	//cout<<endl<<"Obeservation: "<<trafficObservation<<" - current Taffic: "<<tm->getCurrentTraffic()<<" - class: "<<getClass(tm->getCurrentTraffic())<<endl;
	transition_prob_t P = transitionProbabByWrites;
	//cout<<endl<<"[n = "<<n<<"]  P="<<P<<endl;
	if(n > 1){
		Mtmp = P;
		for(uint32_t cnt = 0; cnt < n - 1; cnt++){
			Mtmp = prod(Mtmp, P);
			//cout<<"P["<<cnt<<"] ="<<P<<" - O*T*P= "<<prod(trafficObservation,P)<<endl;

		}
	}
//	cout<<"Mtmp="<<P<<" - O*T*P= "<<prod(trafficObservation,Mtmp)<<endl;
//	cout<<"CLASSBORDERS="<<classBordersAvg<<endl;
	return inner_prod(prod(trafficObservation,Mtmp),classBordersAvg);
//	return 0.0;

//	return prod(classBordersSQ);
*/
}

bool libBPMTrafficStatistics::isBuilt(){
	return built;
}

double libBPMTrafficStatistics::getAverageDurationInStateByWrites(uint32_t trClass){
	if(!isBuilt()){
		DEBUG2("ERROR: statistics have not been built...");
		exit(1);
	}
	if(transitionProbabByWrites(trClass,trClass)==1.0){
		return DBL_MAX;
	}

	double multiplier = transitionProbabByWrites(trClass,trClass)/((1.0 - transitionProbabByWrites(trClass,trClass))*(1.0 - transitionProbabByWrites(trClass,trClass)));
	double sum = 0;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){

		if(cnt != trClass){
			sum = sum + transitionProbabByWrites(trClass, cnt);
		}
	}

	return multiplier * sum;
}

double libBPMTrafficStatistics::getAverageDurationInStateByTime(uint32_t trClass){
	if(!isBuilt()){
		DEBUG2("ERROR: statistics have not been built...");
		exit(1);
	}
	if(transitionProbabByTime(trClass,trClass) == 1.0){
		return DBL_MAX;
	}
	double multiplier = transitionProbabByTime(trClass,trClass)/((1.0 - transitionProbabByTime(trClass,trClass))*(1.0 - transitionProbabByTime(trClass,trClass)));
	double sum = 0;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){

		if(cnt != trClass){
			sum = sum + transitionProbabByTime(trClass, cnt);
		}
	}

	return multiplier * sum;
}


void libBPMTrafficStatistics::printAverageDurationsByWrites(){

//	cerr<<"######################################[ avg class durations by writes ]########################################"<<endl;
//	cerr<<"";
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//		cerr<<"   ["<<cnt<<"]\t\t";
	}
	//	cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//			cerr<<setprecision(4)<<fixed<<getAverageDurationInStateByWrites(cnt)<<"\t\t";
		//		resetiosflags(ios::scientific);
	}
	//	cerr<<endl;
	//cerr<<"#############################################################################################################"<<endl;

}
void libBPMTrafficStatistics::printAverageDurationsByTime(){

	//	cerr<<"######################################[ avg class durations by time ]###########################################"<<endl;
	//cerr<<"";
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//	cerr<<"   ["<<cnt<<"]\t\t";
	}
	//	cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//	cerr<<setprecision(4)<<fixed<<getAverageDurationInStateByTime(cnt) <<"\t\t";
		//	resetiosflags(ios::scientific);
	}
	//	cerr<<endl;
	//	cerr<<"##############################################################################################################"<<endl;

}


void libBPMTrafficStatistics::reset(){
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		classBorders(cnt) = 0;
		classBordersAvg(cnt) = 0;
		bytesPerClass(cnt) = 0;
		writesPerClass(cnt) = 0;
		for(uint32_t cnt2 = 0; cnt2 < nClasses; cnt2++){
			transitionCntByWrites(cnt,cnt2) = 0;
			transitionCntByTime(cnt,cnt2) = 0;
			transitionProbabByWrites(cnt,cnt2) = 0;
			transitionProbabByTime(cnt,cnt2) = 0;

		}
	}
    //DEBUG2("Statistics reset done\n");
}




double libBPMTrafficStatistics::getAvgTimePerClass(uint32_t state){
	uint32_t cnt;
	uint32_t sumOfTransitions=0;
	double sumOfDurations = 0.0;
	if(!isBuilt()){
		DEBUG2("ERROR: statistics have not been built...");
		exit(1);
	}
	if(state > nClasses){
		DEBUG2("ERROR: state > nClasses");
		exit(1);
	}

	for(cnt = 0; cnt < nClasses; cnt++){
		if(state != cnt){
			sumOfTransitions += transitionCntByWrites(state,cnt);
		}
	}
	if(sumOfTransitions == 0.0){
		return 0.0;
	}else{
		return timePerClass[state]/sumOfTransitions;

	}

}


void libBPMTrafficStatistics::printAverageTimePerClass(){

	//	cerr<<"######################################[ avg time per class]########################################"<<endl;
	//	cerr<<"";
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//	cerr<<"   ["<<cnt<<"]\t\t";
	}
	//	cerr<<endl;
	for(uint32_t cnt = 0; cnt < nClasses; cnt++){
		//			cerr<<setprecision(4)<<fixed<<getAvgTimePerClass(cnt)<<"\t\t";
		//		resetiosflags(ios::scientific);
	}
	//	cerr<<endl;
	//cerr<<"#############################################################################################################"<<endl;

}
