/**
 * \file libBPMTrafficMeasurement.hpp
 * \brief History-Based anaylzer of traffic
 * This class measures traffic by adding write() - calls (realized by the function add() ) to a history.
 * Each time, a write() - function is called (or, in different contexts, bytes are written or received), the function
 * \ref add() should be called. It will save the number of bytes sent/received including a timestamp.
 * With this data, the traffic rate over time can be retrived.
 *
 * Okt. 2013, Philipp Kindt <kindt@rcs.ei.tum.de>
 *
 */
#ifndef _LIBBPM_TRAFFIC_MEASUREMENT_H
#define _LIBBPM_TRAFFIC_MEASUREMENT_H

#include "libBPMPlatform.hpp"
#include <inttypes.h>
#include "libBPMHistory.hpp"
#include "libBPMTraffic.hpp"


#if LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_POSIX
#include <time.h>
#elif LIBBPM_PLATFORM_ARCHITECTURE==LIBBPM_PLATFORM_ARCHITECTURE_STM32F4_FREERTOS
#include <time.h>
#include <hhd_rtc.h>
#endif

///The maximum history length. It is NOT preallocated. The history can grow up to this number of entries.
#define LIBBPM_TRAFFIC_HISTORY_LENGHT 20000



/**
 * Class for trafic measuring in sequential streams
 */
class libBPMTrafficMeasurement{
private:
	libBPMHistory<libBPMTraffic> *trHist;		///< A traffic history
	double maxTraffic;							///< The maximum traffic that ocurred (generated by build()
	uint64_t byteSum;							///< The number of bytes transferred in whole
	double minimumTimeDifferenceNotToJoin;		///< This parameter is set within the constructor and determines the time interval two subsequent writes are regarded as one transmission with the sum of numbers of bytes
	libBPMTraffic trPrev;						///< Traffic of the previous write (determines weather to join both traffics or not)

	/**
	 * Returns the time between t1 and t2 in seconds
	 * @param t1	Time 1
	 * @param t2	Time 2
	 * @return	Time [s] between t1 and t2
	 */
	double getTimeDifference(const struct timespec &t1, const struct timespec &t2) const;


public:
	libBPMTrafficMeasurement();
	~libBPMTrafficMeasurement();

	/**
	 * \brief Indicate that a number of bytes is transferred.
	 * This function should be called whenever data is transmitted. It will keep track of the data and add a timestamp
	 * @param nBytes
	 */
	void add(uint32_t nBytes);

	/**
	 * \brief Returns a reference to the history used.
	 * Handle with care, reference is non-constant!
	 * @return Reference to traffic history
	 */
	libBPMHistory<libBPMTraffic>& getTrafficHist();

	/**
	 * \brief Returns the current traffic rate.
	 * It will only work if at least two entries are in the traffic history.
	 * Shortcut for getATraffic(0)
	 * @return Last recent traffic [bytes/s]
	 */
	double getCurrentTraffic() const;

	/**
	 * \brief Returns the traffic rate of a specific transfer (add() - call)
	 * @param index Determines the index of the transfer. 0 => last recent of add. Each number > 0 goes one step back in history.
	 * @return Traffic [bytes/s] corresponding to the index
	 */
	double getTraffic(uint32_t index) const;

	/**
	 * \brief Returns the average traffic over the last nIndexes transfers
	 *
	 * @param nIndexes Number of indices to consider
	 * @return Avergage traffic [bytes/s]
	 */
	double getAvgTraffic(uint32_t nIndexes) const;

	/**
	 * \brief Returns the average traffic within a time-window of timeWindow seconds starting from the last transfer (=last time add() was called)
	 * @param timeWindow Time [s] beginning from last recent transfer the average traffic should be calculated for.
	 * @return Average traffic [bytes/s] of time window.
	 */
	double getAvgTrafficInTimeWindow(double timeWindow) const;

	/**
	 * \brief Returns the number of bytes within a time-window of timeWindow seconds starting from the last transfer (=last time add() was called)
	 * @param timeWindow Time [s] beginning from last recent transfer the number of bytes transferred shall be calculated for.
	 * @return Number of bytes sent in time window.
	 */
	uint32_t getNBytesInTimeWindow(double timeWindow) const;

	/**
	 * \brief Returns Duration of transfer index
	 * The duration is the time between the transfer index + 1 and index.
	 * @param index Index of transfer (0=> last recent transfer, 1 => the transfer before the last recent one,...)
	 * @return Duration of transfer specified by index
	 */
	double getDuration(uint32_t index) const;

	/**
	 * \brief Returns the time between the first transfer in the history and the last recent transfer.
  	 * This is not necessary the first transfer that ever happened as the history might have become full and therefore some thransfers could have been dropped
	 * @return Time [s] between the first transfer in historyand the last recent one.
	 */
	double getGlobalTime() const;

	/**
	 * \brief Returns the biggest filled index of the history.
	 * @return Biggest index of the history that is in use
	 */
	uint32_t getBiggestIndex() const;

	/**
	 * \brief Returns the number of bytes sent by transfer specified by index
	 * @return number of bytes sent within specified transfer
	 */
	uint32_t getNBytes(uint32_t index) const;

	/**
	 * \brief Returns the maximum traffic rate that occured in the whole history.
	 * Please note that some elements might have been removed from the history because it was full. Therefore,
	 * not the maximum traffic rate that occured ever is necessarily returned.
	 * @return Maximum traffic that occurred
	 */
	double getMaxTraffic() const;

	/**
	 * \brief Returns the number of bytes sent within the whole history
	 * Please note that some elements might have been removed from the history because it was full. Therefore,
	 * not the byte sume that has ever been transferred is necessarily returned.
		 *
	 * @return Number of bytes sent within whole history
	 */
	uint64_t const& getByteSum() const;

	/**
	 * \brief Resets the history. All elements are removed.
	 */
	void reset();
};

#endif
