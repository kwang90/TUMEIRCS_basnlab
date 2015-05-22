/**
 * \file libBPMTrafficStatistics.hpp
 * \brief Markov State analyzer for \ref libBPMTrafficMeasurement
 * This file will build a Markov chain by analyzing data recored by \ref libBPMTrafficMeasurement.
 * It will provide statical informations and predictions about concerning the next stable traffic state
 *
 */
#ifndef RCS_TRAFFICSTATISTICS_H_
#define RCS_TRAFFICSTATISTICS_H_

#include <inttypes.h>
#include "libBPMTrafficMeasurement.hpp"

///If 1, boost (http://www.boost.org/) is used. Otherwise, "homebrew" classes of libs/linAlg are used insterad.
#define LIBBPM_USEBOOST 0

#if LIBBPM_USEBOOST == 1
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
using namespace boost::numeric::ublas;
typedef boost::numeric::ublas::vector<uint32_t> flat_array_t;
typedef boost::numeric::ublas::vector<double> flat_array_double_t;
typedef  matrix<uint32_t> transition_cnt_t;
typedef  matrix<double> transition_prob_t;

#else
#include <linAlg/vector.hpp>
#include <linAlg/matrix.hpp>
typedef linAlg::Vector<uint32_t> flat_array_t;
typedef linAlg::Vector<double> flat_array_double_t;
typedef linAlg::Matrix<uint32_t> transition_cnt_t;
typedef linAlg::Matrix<double> transition_prob_t;


#endif

using namespace std;

class libBPMTrafficStatistics{
	private:
	uint32_t nClasses;									///< Number of traffic classes. To be set by constructor
	flat_array_double_t classBorders;					///< Traffic values that define the borders between the classes. To be set by \ref build().
	flat_array_double_t classBordersAvg;				///< Mean value of each class. To be set by \ref build().
	flat_array_t bytesPerClass;							///< Number of bytes sent in each class
	flat_array_t writesPerClass;						///< Number of transfers (i.e. write()-calls) in each class.To be set by \ref build().
	flat_array_double_t timePerClass;					///< Total time spent in each class. To be set by \ref build().
	transition_cnt_t transitionCntByWrites;				///< The number of transitions from one class into every another. To be set by \ref build().
	transition_prob_t transitionCntByTime;				///< The time of transitions from one class into every other. Example: write(n1) at time t1 in class 1, then write (n2)@t2 in class 1, then write(n3)@t3 in class 3 => t2-t1  is  assigned to transtion 1->1 and t3 - t2 to transition 1->3. To be set by \ref build().
	transition_prob_t transitionProbabByWrites;			///< The transition probability from one class into another, calculated on byte-per-class-basis. To be set by \ref build().
	transition_prob_t transitionProbabByTime;			///< The transition probability from one class into another, calculated on time-per-class-basis. To be set by \ref build().


	libBPMTrafficMeasurement *tm;						///< The traffic measurement to work on. To be set by the constructor.
	bool built;											///< Flag that indicates the statics have been built (to be set by \ref build() )

	public:
	/**
	 * \brief Constructor
	 * @param nClasses Number of classes (Markov-States) the traffic has to be subdivided into
	 * @param tm libBPMTrafficMeasurement to analyze
	 */
	libBPMTrafficStatistics(uint32_t nClasses, libBPMTrafficMeasurement *tm);

	/**
	 * \brief Build the statistics. Might be costly in terms of CPU time.
	 */
	void build();

	/**
	 * \brief Reset the class.
	 * After that, the build-flag will be set to false and all arrays/matrices are resetted
	 */
	void reset();

	/**
	 * \brief Returns the class number corresponding to a given traffic rate
	 * @param traffic Traffic the class number shall be determined
	 * @return Class that corrensponds to the traffic rate
	 */
	uint32_t getClass(const double &traffic) const;

	/**
	 * \brief Prints the traffic histograph (Number of bytes transferred per class)
	 */
	void printHistographByBytes();

	/**
	 * \brief Prints the traffic histograph (Time spent per class)
	 */
	void printHistographByTime();

	/** \brief Print the transition probability matrix determined by the number of writes to STDOUT
	 * The Transition probability matrix contains the probability for a transtion to any other class for each class.
	 * These transitions are determined on per-write-basis. That means:
	 * The number of transfers that happened within this class without changing to any other class are analyzed along with the number of writes that lead to a class transition in order to obtaint thr probabiltiy
	 */
	void printTransitionMatrixByWrites();
	/** \brief Print the transition probability matrix determined by the time the traffic rate has remained within one class.
	 * The Transition probability matrix contains the probability for a transtion to any other class for each class.
	 * These transitions are determined on per-time. That means:
	 * To determine this probability, the time that has been spent in one class with no transitions to other classes are analyzed.
	 * Example:
	 * Example: write(n1) at time t1 in class 1, then write (n2)@t2 in class 1, then write(n3)@t3 in class 3
	 *
	 * t2-t1  is  assigned to transtion 1->1 and t3 - t2 to transition 1->3
	 * The probabilty is obtained by dividing these times through the time spent in the corresponding class
	 */
	void printTransitionMatrixByTime();

	/**
	 * \brief Deprecated - do not use
	 * @param number of writes in future
	 * @return expected traffic
	 */
	double getTrafficPrediction(uint32_t n);

	/**
	 * \brief Check if the Statistics have been build.
	 * Returns true, if build() has been called before and it has completed succesfully.
	 * @return true, if the statistics have been build; false, otherwise
	 */
	bool isBuilt();

	/**
	 * \brief Returns the average duration (in terms of number of transferts) in a given state.
	 * This duration has been obtained on per-transfer-basis. It is similar to Markov-Chain residence times.
	 * @param state State the average number of transfers within is to be calculated…
	 * @return Number of transfers that took place per class without changing to another class for the given state
	 */
	double getAverageDurationInStateByWrites(uint32_t state);

	/**
	 * \brief prints the average residence time (average number of transfers without class transitions) by number of transfers to STDOUT.
	 * See \ref getAverageDurationInStateByWrites()
	 */
	void printAverageDurationsByWrites();

	/**
	 * \brief Returns the average residence time of a given state. This time is NOT the average time that took place within a class without
	 * class transitions, but is the Markov residence duration for the state calculated by the time transtion matrix. Consult the header-file libBPMTrafficStatistics.h for
	 * more information about this matrix. You probably don't want to use this function - use \ref getAvgTimePerClass() instead.
	 * @param state Traffic class
	 * @return average residence duration [s] in this state
	 */
	double getAverageDurationInStateByTime(uint32_t state);

	/**
	 * \brief Print average residence durations per class to STDOUT
	 * See \ref getAverageDurationInStateByTime()
	 */
	void printAverageDurationsByTime();

	/**
	 * \brief Return the average time the traffic rate was within a given traffic class before a transition occurred.
	 * This duration is not calculated fromt the Markov Chain but obtained directly from the traffic history.
	 * @param state Traffic class the average time is to be returned
	 * @return	Average duration in the traffic class before a class transtion occured.
	 */
	double getAvgTimePerClass(uint32_t state);

	/**
	 * \brief Return the average time the traffic rate was within a given traffic class before a transition occurred to STDOUT.
	 * See \ref getAvgTimePerClass()
	 */
	void  printAverageTimePerClass();

};

#endif /* LIBBPM_TRAFFICSTATISTICS_H_ */
