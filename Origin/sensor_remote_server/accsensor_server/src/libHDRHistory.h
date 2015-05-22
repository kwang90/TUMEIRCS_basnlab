/*************************************************************************
 * libHDRHistory.h
 * March,April,November 2009, P.Kindt <pkindt@directbox.com>
 * Tools to mange the history of a signal, i.e. its behavior
 * during the last frames.
 *
 * Indexes (i.e. everywhere the type t_hist-index occurs) have the follwing meaning:
 * zero is the last element added, every value > 0 means how many elements in history
 * you want to go back.
 * Example:
 * 	libHDRHistory <char*> hist;
 *	hist.add("first");
 * 	hist.add("2nd");
 *	hist.add("3rd");
 *  cout<<hist[1]<<endl;		//go back one step in history, will print "2nd"

 ****************************************************************************/
#ifndef LIBHDR_HISTORY_H_
#define LIBHDR_HISTORY_H_


#include <vector>
#include <iostream>
#include <algorithm>			//for STL's quicksort algorithm
#include "errorHandling.h"
using namespace std;

#define DEBUGMSG(X) DEBUG(X)

typedef unsigned int t_hist_index;
enum histExtremumType{H_EXTREMUM_MAX, H_EXTREMUM_MIN};

template <class T> class libHDRHistory{
public:

protected:
	unsigned int position;					//"pointer" on current positon in buffer'S array
	unsigned int BiggestIndex;				//the biggest index in use in the vector, this is size-1 when filled completely
	bool empty;
	vector<T> RingBuf;

	//adjust positon and BiggestIndex
	void inkr();

	//calculate position in RingBuffer's Array. index is the number of
	//entries you want to go back from the entry last added.
	//i.E.: index=0 is the object added recently, index = 1 the object before...
	unsigned int getPos(t_hist_index index) const;


public:


	//constructor
	libHDRHistory(unsigned int size);

	//copy-constructor
	libHDRHistory(const libHDRHistory<T>& hm);

	//returns biggest index that can be accessed via get or operator[]]
	unsigned int getBiggestIndex() const;

	//is buffer completely empty?
	bool isEmpty() const;

	//is buffer complete filled (are there as many elements added as there are free places)?
	bool isFilled() const;

	//how many elements are there in the buffer? (if filled once, it will be size allways!)
	unsigned int getNrOfElements() const;

	//gets number of entries allocated - this is the size of the history
	unsigned int getSize() const;

	//add an Element to history's ringbuffer
	void add(T element);

	//same as add
	void operator+=(const T& element);

	//set current position pointer to NewIndex.
	//this NewIndex specifies the last index that is in use - the next
	//element added will be placed at NewIndex+1
	//NewIndex can be -1 to declare this history as empty. The next element added
	//will be placed at index 0.
	//This function is used by cYawHistory to fill first indices with rubbish and keep indices compatible
	//when building a rate of yaw -compensated history of just a part of the non-compensated
	//history. Parts of this compensated history that are not considered contain rubbish.
	//!Handle with care!
	void shiftPositionCursor(int NewIndex);

	//get element from history's RingBuffer. Just resturns refernece on element, does not
	//remove element from buffer
	T& get(unsigned int index);

	//get element from history's RingBuffer. Just resturns refernece on element, does not
	//remove element from buffer
	T get(unsigned int index) const;

	//same as get
	T operator[](unsigned int index) const;

	//calculate arithmethic mean value. "Simple" because libHDRHistoryManager's averageing methods can do much more
	T getSimpleAverage(t_hist_index IndexFrom, t_hist_index IndexTo);

	//calculate median value. "Simple" because libHDRHistoryManager's averageing methods can do much more
	T getSimpleMedian(t_hist_index IndexFrom, t_hist_index IndexTo);


	//sum up history's elements from index IndexFrom to index IndexTo
	T getSum(t_hist_index IndexFrom, t_hist_index IndexTo) const;

	//print this history's contents to stdtour
	void print();

	/*calculate extrema
	this function will return the history index of either a global minimum or maximum

	it can handle averaging - if AvgWindowLength is bigger than one,
	A number of AvgWindowLength entries are averaged and only averaged values
	are considered. In this case, the Index k having the biggest/smallest average value
	based on averaging from entry k to entry (k + AvgWindowLength) is returned.

	If StopAtValue is true, instead of the extrema the smallest index where value StopValue is reached
	is returned (reached = bigger/smaller depending on type of extremum). If this value
	is never reached, the Index of the extremum is returned.

	In case of two equal extrema, the smallest index is returned.
	*/

	t_hist_index getIndexOfExtremum(histExtremumType type, T* value, unsigned int AvgWindowLength ,bool StopAtValue, T StopValue);

	//clears history
	void clear();
};




//adjust position-"pointer" and BiggestIndex
template <class T> void libHDRHistory<T>::inkr(){

	if(empty){		//no Indexes to inkrement at first add()
		empty = false;
		return;
	};

	if(position >= RingBuf.size() - 1){
		position = 0;
	}else{
		position++;
	}
	if(BiggestIndex < RingBuf.size() - 1) BiggestIndex++;
}


//calculate position in RingBuffer's Array. index is the number of
//entries you want to go back from the entry last added.
//i.E.: index=0 is the object added recently, index = 1 the object before...
//if am index is given which is out of range, this function will return the
//position of the "oldest" element in RingBuf
template <class T> unsigned int libHDRHistory<T>::getPos(t_hist_index index) const{
	unsigned int rv;
	if(index > position){
		if(index > BiggestIndex){
			DEBUGMSG("index="<<index<<" is out of range! BiggestIndex="<<BiggestIndex);
			DEBUGMSG("This should never happen. Goodbye.");
			exit(1);
		}
		return BiggestIndex - (index-position-1);		//-1 because there is a position zero!

	}else{
		return position - index;
	}
}




template <class T> libHDRHistory<T>::libHDRHistory(unsigned int size): RingBuf(size){
	position = 0;
	BiggestIndex=0;
	empty = true;
}

template <class T> libHDRHistory<T>::libHDRHistory(const libHDRHistory<T>& hm): RingBuf(hm.RingBuf){
	position = hm.position;
	BiggestIndex=hm.BiggestIndex;
	empty = hm.empty;

}


//is buffer completely empty?
template <class T> bool libHDRHistory<T>::isEmpty() const{
	return empty;
}

//is buffer complete filled (are there as many elements added as there are free places)?
template <class T> bool libHDRHistory<T>::isFilled() const{
	if(BiggestIndex == RingBuf.size()-1) return true;
	return false;
}

//how many elements are there in the buffer? (if filled once, it will be size allways!)
template <class T> unsigned int libHDRHistory<T>::getNrOfElements() const{
	if(isEmpty()) return 0;
	return BiggestIndex + 1;
}

//returns  biggest array index
template <class T> unsigned int libHDRHistory<T>::getBiggestIndex() const{
	return BiggestIndex;
}

//gets number of entries allocated - this is the size
template <class T> unsigned int libHDRHistory<T>::getSize() const{
	return RingBuf.size();
}

//add an Element to history's ringbuffer
template <class T> void libHDRHistory<T>::add(T element){
	inkr();
	RingBuf[position] = element;
}

//set current position pointer to NewIndex.
//this NewIndex specifies the last index that is in use - the next
//element added will be placed at NewIndex+1
//NewIndex can be -1 to declare this history as empty. The next element added
//will be placed at index 0.
//This function is used by cYawHistory to fill first indices with rubbish and keep indices compatible
//when building a rate of yaw -compensated history of just a part of the non-compensated
//history. Parts of this compensated history that are not considered contain rubbish.
//!Handle with care!
template <class T> void libHDRHistory<T>::shiftPositionCursor(int NewIndex){
	if(NewIndex < 0){
		NewIndex = 0;
		empty = true;
	}else{
		empty = false;
	}

	if(NewIndex > (RingBuf.size() - 1)){
		DEBUGMSG("libHDRHistory<T>::ShiftPositionCursor(): Invalid NewIndex; NewIndex="<<NewIndex<<", size="<<RingBuf.size());
		DEBUGMSG("This should never happen and indicates buggy code. Exiting.");
		exit(1);
	}
	position = NewIndex;
	if(BiggestIndex < NewIndex) BiggestIndex = NewIndex;

}

//same as add
template <class T> void libHDRHistory<T>::operator+=(const T& element){
	add(element);
}

//get element from history's RingBuffer. Just resturns refernece on element, does not
//remove element from buffer (like used to happen on some stack implementations)
template <class T> T& libHDRHistory<T>::get(unsigned int index){
	return RingBuf[getPos(index)];
}

//get element from history's RingBuffer. Just resturns value of element, does not
//remove element from buffer (like used to happen on some stack implementations)
template <class T> T libHDRHistory<T>::get(unsigned int index) const{
	return RingBuf[getPos(index)];
}

//same as get
template <class T> T libHDRHistory<T>::operator[](unsigned int index) const{
	return get(index);
}

//sum up history's elements from index IndexFrom to index IndexTo
template <class T> T libHDRHistory<T>::getSum(t_hist_index IndexFrom, t_hist_index IndexTo) const{
	if((IndexTo > BiggestIndex)||(IndexFrom > IndexTo)){
		DEBUGMSG("libHDRHistory::GetSum(): Invalid Index IndexFrom="<<IndexFrom<<" IndexTo="<<IndexTo);
		DEBUGMSG("This is a software bug. Please change the source... bye");
		exit(1);

	}

	T sum = (T) 0;
	for(unsigned int cnt = IndexFrom; cnt <= IndexTo; cnt++){
			sum += get(cnt);
	}
	return sum;
}

//calculates arithmethic mean value in window specified by IndexFrom and indexTo
//'simple' because libHDRHistoryManager's averaging functions can do much more (median,...)
template <class T> T libHDRHistory<T>::getSimpleAverage(t_hist_index IndexFrom, t_hist_index IndexTo){
	//avoid bad indexes
	if((IndexTo > BiggestIndex)||(IndexFrom > IndexTo)){
		DEBUGMSG("libHDRHistory::GetSimpleAverage(): Invalid Index IndexFrom="<<IndexFrom<<" IndexTo="<<IndexTo);
		DEBUGMSG("This is a software bug. Please change the source... bye");
		exit(1);

	}

	T average = (T) 0;
	for(unsigned int cnt = IndexFrom; cnt <= IndexTo; cnt++){
			average += get(cnt);
	}
	average = average / (T)(IndexTo - IndexFrom + 1);		//+1 as we calc in indexes
	return average;
}


/*Calculates the median. it is more robust against changing values
and (hopefully) more useful than the average value.
*/
template <class T> T libHDRHistory<T>::getSimpleMedian(t_hist_index IndexFrom, t_hist_index IndexTo){
	//avoid bad indexes
	if((IndexFrom > BiggestIndex)||(IndexFrom > IndexTo)){
		DEBUGMSG("libHDRHistory::GetMedian(): Invalid Index IndexFrom="<<IndexFrom<<" IndexTo="<<IndexTo);
		DEBUGMSG("This is a software bug. Fix and recompile! bye...");
		exit(1);
	}
	if(IndexTo > libHDRHistory<T>::BiggestIndex){
		IndexTo = libHDRHistory<T>::BiggestIndex;
	}

	if(IndexFrom > IndexTo){
		IndexFrom = IndexTo;
		DEBUGMSG("libHDRHistorymanager::GetMedian(): IndexFrom > IndexTo: had to adjst: IndexFrom="<<IndexFrom<<"  IndexTo="<<IndexTo);
	}

	//a ringbuf is unfortunately not an array -> we cannot use STL's sort
	//efficiently => build array first, use quicksort on it after that

	 vector<T> hektor(IndexTo - IndexFrom + 1);					//hektor's a good name for a vector!

	 for(int cnt = 0; cnt < (IndexTo - IndexFrom + 1); cnt++){
	    	 hektor[cnt] = libHDRHistory<T>::get(IndexFrom + cnt);
	 }


	 sort(hektor.begin(), (hektor.end()));


	 if((IndexTo - IndexFrom + 1)%2 != 0){		//uneven nr of elements
		 return hektor[(IndexTo - IndexFrom + 1)/2];
	 }else{										//even nr of elements
		 return (hektor[(IndexTo - IndexFrom)/2] + hektor[(IndexTo - IndexFrom)/2 + 1])/2;
	 }

}





	/*calculate extrema
	this function will return the history index of either a global minimum or maximum

	it can handle averaging - if AvgWindowLength is bigger than one,
	A number of AvgWindowLength entries are averaged and only averaged values
	are considered. In this case, the Index k having the biggest/smallest average value
	based on averaging from entry k to entry (k + AvgWindowLength) is returned.

	If StopAtValue is true, instead of the extrema the smallest index where value StopValue is reached
	is returned (reached = bigger/smaller depending on type of extremum). If this value
	is never reached, the Index of the extremum is returned.

	In case of two equal extrema, the smallest index is returned.
	*/
	template <class T> t_hist_index libHDRHistory<T>::getIndexOfExtremum(histExtremumType ExtrType, T* value,unsigned int AvgWindowLength = 0, bool StopAtValue=false, T StopValue = (T)0){
	T ExtremumValue, TempValue;
	bool ForceNoAvg = false;
	if(AvgWindowLength > getNrOfElements()){
		DEBUGMSG("libHDRHistory::GetIndexOfExtremum(): Averaging Frame Window exceeds Number of Elements in History");
		DEBUGMSG("\t\tSwitching off averaging feature.");
		ForceNoAvg = true;
		AvgWindowLength = 1;
	}
	if(AvgWindowLength == 0){
		AvgWindowLength = 1;
	}

	if((AvgWindowLength <= 1)||(ForceNoAvg)){
		ExtremumValue = get(0);
	}else{
		ExtremumValue = getSimpleAverage(0,AvgWindowLength - 1);
	}
	unsigned int ExtremumIndex = 0;
	for(unsigned int cnt = 0; cnt <= getBiggestIndex() - AvgWindowLength + 1; cnt++){

		if((AvgWindowLength <= 1)||(ForceNoAvg)){
			TempValue = get(cnt);
		}else{
			TempValue = getSimpleAverage(cnt,cnt + AvgWindowLength - 1);
		}

		if(((ExtrType == H_EXTREMUM_MAX)&&(TempValue > ExtremumValue))||((ExtrType == H_EXTREMUM_MIN)&&(TempValue < ExtremumValue))){
			ExtremumValue = TempValue;
			ExtremumIndex = cnt;
		}

		//if StopAtValue is activated an value has been reached, stop now.
		if(StopAtValue){
			if(((ExtrType == H_EXTREMUM_MAX)&&(ExtremumValue >= StopValue))||((ExtrType == H_EXTREMUM_MIN)&&(ExtremumValue <= StopValue))){
				ExtremumValue = TempValue;
				ExtremumIndex = cnt;
				break;
			}
		}

	}
	if(value != NULL){
		*value = ExtremumValue;
	}
	return ExtremumIndex;
}

//clear history
template <class T> void libHDRHistory<T>::clear(){
	position = 0;
	BiggestIndex=0;
	empty = true;

}


//print this history's content to stdout
template <class T> void libHDRHistory<T>::print(){
	cout<<"----[libHDRHistory Instance "<<this<<"]---------"<<endl;
	for(int cnt = 0; cnt <= getBiggestIndex(); cnt++){
		 cout<<"    ["<<cnt<<"] = "<<get(cnt)<<endl;
	}
	cout<<"--------------------------------------------------------"<<endl;
}



#endif /* LIBHDR_HISTORY_H_ */
