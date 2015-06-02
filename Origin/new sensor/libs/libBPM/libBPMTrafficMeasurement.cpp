#include "libBPMTrafficMeasurement.hpp"
#include "debug.h"
#include <math.h>
	libBPMTrafficMeasurement::libBPMTrafficMeasurement(){
		trHist = new libBPMHistory<libBPMTraffic>(LIBBPM_TRAFFIC_HISTORY_LENGHT);
		maxTraffic = 0;
		byteSum = 0;
		minimumTimeDifferenceNotToJoin = 0.0075;
		trPrev.nBytes = 0;
		trPrev.time.tv_nsec = 0;
		trPrev.time.tv_sec = 0;

	}

	libBPMTrafficMeasurement::~libBPMTrafficMeasurement(){
		delete trHist;
	}

	double libBPMTrafficMeasurement::getTimeDifference(const struct timespec &t1, const struct timespec &t2) const{
		return (double) t2.tv_sec - (double) t1.tv_sec + ((double) t2.tv_nsec - (double) t1.tv_nsec) / 1000000000.0;
	}

	void libBPMTrafficMeasurement::add(uint32_t nBytes){
		libBPMTraffic traf;
		double currentTraffic = 0.0;
		clock_gettime(CLOCK_MONOTONIC,&traf.time);
		byteSum += (uint64_t) nBytes;

		if((!trHist->isEmpty())&&(fabs(getTimeDifference(traf.time,trPrev.time)) < minimumTimeDifferenceNotToJoin)){
				trHist->get(0).nBytes += nBytes;
                //DEBUG2("JOINING ("<<nBytes<<"): ");

		}else{
			traf.nBytes = nBytes;
			trHist->add(traf);
			trPrev.time= traf.time;

		}

		if(trHist->getBiggestIndex() > 0){
			currentTraffic = getCurrentTraffic();

			if(currentTraffic > maxTraffic){
				maxTraffic = currentTraffic;
			}
		}
        //DEBUG2("add("<<nBytes<<"): rate = "<<currentTraffic<<"\tMAX = "<<maxTraffic);

	}


	libBPMHistory<libBPMTraffic>& libBPMTrafficMeasurement::getTrafficHist(){
		return *trHist; 
	}

	uint32_t libBPMTrafficMeasurement::getBiggestIndex() const{
		return trHist->getBiggestIndex();
	}

	double libBPMTrafficMeasurement::getTraffic(uint32_t index) const{
		if(index > trHist->getBiggestIndex() - 1){
			return -1.0;
		}
		if(trHist->getNrOfElements() < 2){
			return -1.0;
		}
		struct timespec now;

		double diffBetweenElements = getTimeDifference(((*trHist)[index + 1]).time,((*trHist)[index]).time);
		//cout<<"getTraffic("<<index<<"): timeDiff="<<diffBetweenElements<<endl;

		return (((*trHist)[index]).nBytes/diffBetweenElements);

/*
		clock_gettime(CLOCK_MONOTONIC,&now);
		double diffBetweenNow = getTimeDifference(((*trHist)[index + 1]).time,now);
	//	cout<<"diffBetweenLast = "<<diffBetweenLast<<", diffBetweenNow = "<<diffBetweenNow<<endl;
		if((diffBetweenNow > diffBetweenElements)&&(index == 0)){
			return (((*trHist)[index]).nBytes/diffBetweenNow);

		}else{
		//cout<<"Difference: "<<getTimeDifference(((*trHist)[index + 1]).time,((*trHist)[index]).time)<<endl;
			return (((*trHist)[index + 1]).nBytes/diffBetweenElements);
		}
*/
	}

	double libBPMTrafficMeasurement::getDuration(uint32_t index) const{
			if(index > trHist->getBiggestIndex() - 1){
                //DEBUG2("wrong index : "<<index<<" - index must be smaller than biggestIndex -1 ");
				exit(1);
				return -1.0;
			}
			if(trHist->getNrOfElements() < 2){
                //DEBUG2("error - only one element in list. ");
				exit(1);
				return -1.0;
			}


			double diffBetweenElements = getTimeDifference(((*trHist)[index + 1]).time,((*trHist)[index]).time);
			return diffBetweenElements;
	}


	double libBPMTrafficMeasurement::getCurrentTraffic() const{
		return getTraffic(0);
	}


	double libBPMTrafficMeasurement::getAvgTraffic(uint32_t nIndexes) const{
		if(trHist->getNrOfElements() < 2){
			DEBUG2("warn: empty");
			return -1.0;
		};
		if(nIndexes > trHist->getBiggestIndex() + 1){
			nIndexes = trHist->getBiggestIndex() + 1;
		}
		uint64_t nBytes = 0;
		for(uint32_t cnt = 1; cnt < nIndexes; cnt++){
			nBytes += (uint64_t) trHist->get(cnt).nBytes;
//			cout<<"cnt ="<<cnt<<" - "<<trHist->get(cnt).nBytes<<" - sum="<<nBytes<<endl;
		}
		double time = getTimeDifference(trHist->get(nIndexes - 1).time, trHist->get(0).time);
//		cout<<"Time eleapsed: "<<time<<endl;
		return (double) nBytes / time;
	}	
	
	double libBPMTrafficMeasurement::getAvgTrafficInTimeWindow(double timeWindow) const{
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			double timeElapsed;

			if(trHist->getNrOfElements() < 2){
				return -1.0;
			};
			uint64_t nBytes = 0;
			uint32_t lastIndex = 0;
			for(uint32_t cnt = 1; cnt <= trHist->getBiggestIndex(); cnt++){
				timeElapsed = getTimeDifference(trHist->get(cnt).time,now);
				if(timeElapsed > timeWindow){
		//			cout<<"stopping - timeElapsed= "<<timeElapsed<<", timeWindow = "<<timeWindow<<", lastIndex = "<<lastIndex<<endl;
					break;
				}
				lastIndex = cnt;
				nBytes += (uint64_t) trHist->get(cnt).nBytes;
		//		cout<<"cnt ="<<cnt<<" - current="<<trHist->get(cnt).nBytes<<" - sum="<<nBytes<<" - timeElapsed: "<<timeElapsed<<endl;
			}
			if(lastIndex < 1){
				return getCurrentTraffic();
			}
			double time = getTimeDifference(trHist->get(lastIndex).time, trHist->get(0).time);
			//cout<<"nBytes = "<<nBytes<<" - Time eleapsed: "<<time<<endl;
			return (double) nBytes / time;
	}

	uint32_t libBPMTrafficMeasurement::getNBytesInTimeWindow(double timeWindow) const{
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			double timeElapsed;

			if(trHist->getNrOfElements() < 2){
				return -1.0;
			};
			uint64_t nBytes = 0;
			uint32_t lastIndex = 0;
			for(uint32_t cnt = 1; cnt <= trHist->getBiggestIndex(); cnt++){
				timeElapsed = getTimeDifference(trHist->get(cnt).time,now);
				if(timeElapsed > timeWindow){
	//				cout<<"stopping - timeElapsed= "<<timeElapsed<<", timeWindow = "<<timeWindow<<", lastIndex = "<<lastIndex<<endl;
					break;
				}
				lastIndex = cnt;
				nBytes += (uint64_t) trHist->get(cnt).nBytes;
	//			cout<<"cnt ="<<cnt<<" - current="<<trHist->get(cnt).nBytes<<" - sum="<<nBytes<<" - timeElapsed: "<<timeElapsed<<endl;
			}
			return nBytes;
	}


	uint32_t libBPMTrafficMeasurement::getNBytes(uint32_t index) const{
		if(index > trHist->getBiggestIndex()){
			return 0;
		}else{
			return trHist->get(index).nBytes;
		}
	}

	double libBPMTrafficMeasurement::getMaxTraffic() const{
		return maxTraffic;
	}

	uint64_t const& libBPMTrafficMeasurement::getByteSum() const{
		return byteSum;
	}


	void libBPMTrafficMeasurement::reset(){
		trHist->clear();
		byteSum = 0;
		maxTraffic = 0.0;
	}

	double libBPMTrafficMeasurement::getGlobalTime() const{
			if(trHist->getNrOfElements() < 2){
				return -1.0;
			}


			double diffBetweenElements = getTimeDifference(((*trHist)[trHist->getBiggestIndex()]).time,((*trHist)[0]).time);
			return diffBetweenElements;
	}
