/**
	\file cSpectrumComparatorThreadMS1.h
	\brief The thread for comparison of a theoretical spectrum with a peak list (MS1 level).
*/


#ifndef _CSPECTRUMCOMPARATORTHREADMS1_H
#define _CSPECTRUMCOMPARATORTHREADMS1_H

#include <QRunnable>
#include <QObject>

#include "core/cTheoreticalSpectrum.h"


class cTheoreticalSpectrumList;
class cMainThread;


/**
	\brief The class representing a thread for comparison of a theoretical spectrum with a peak list (MS1 level).
*/
class cSpectrumComparatorThreadMS1 : public QObject, public QRunnable {

	Q_OBJECT
	
private:

	cMainThread* mainthread;
	cParameters* parameters;
	int peaklistseriesvectorid;
	cTheoreticalSpectrum* poolspectrum;
	vector< vector<int> >* hintsindex;
	cTheoreticalSpectrumList* theoreticalspectrumlist;
	vector<cPeaksList>* unmatchedpeaks;
	bool* terminatecomputation;


public:


	/**
		\brief Initialize the spectrum comparator.
		\param mainthread pointer to the main thread
		\param parameters pointer to program parameters
		\param peaklistseriesvectorid id of a peaklistseriesvector
		\param poolspectrum input pool of theoretical peaks
		\param hintsindex input vector of ids of experimental spectra in which a theoretical peak was found; for every theoretical peak
		\param theoreticalspectrumlist output list of matched peaks
		\param unmatchedpeaks output list of unmatched peaks
		\param terminatecomputation pointer to a variable determining that the thread must be stopped
	*/
	void initialize(cMainThread* mainthread, cParameters* parameters, int peaklistseriesvectorid, cTheoreticalSpectrum* poolspectrum, vector< vector<int> >* hintsindex, cTheoreticalSpectrumList* theoreticalspectrumlist, vector<cPeaksList>* unmatchedpeaks, bool* terminatecomputation);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();

};


#endif

