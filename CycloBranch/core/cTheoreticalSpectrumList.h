/**
	\file cTheoreticalSpectrumList.h
	\brief The representation of a list of theoretical mass spectra.
*/


#ifndef _CTHEORETICALSPECTRUMLIST_H
#define _CTHEORETICALSPECTRUMLIST_H

#include <algorithm>
#include <QMutex>

#include "parallel/cSpectrumComparatorThread.h"

class cMainThread;


/**
	\brief The class representing a list of theoretical spectra.
*/
class cTheoreticalSpectrumList {

	vector<cTheoreticalSpectrum> theoreticalspectra;
	cMainThread* os;
	cParameters* parameters;
	cDeNovoGraph* graph;

	double worstScore;
	int worstNumberOfMatchedPeaks;
	int refreshlimit;

	QMutex mutex;

	double getCurrentWorstScore();

	void computeNumbersOfCompletedSeries();

	void fixRegularExpression(string& s);

public:


	/**
		\brief The constructor.
	*/ 
	cTheoreticalSpectrumList();


	/**
		\brief Clear the class.
	*/ 
	void clear();


	/**
		\brief Initialize the list.
		\param os reference to the main thread of the application
		\param parameters reference to the parameters of the application
		\param graph pointer to the de novo graph
	*/ 
	void initialize(cMainThread& os, cParameters& parameters, cDeNovoGraph* graph);


	/**
		\brief Add a spectrum to the list.
		\param theoreticalspectrum reference to a spectrum
	*/ 
	void add(cTheoreticalSpectrum& theoreticalspectrum);


	/**
		\brief Get the number of spectra.
		\retval int number of spectra
	*/ 
	int size();


	/**
		\brief Overloaded operator [].
		\param position position of a spectrum in the vector
		\retval reference to the spectrum
	*/ 
	cTheoreticalSpectrum& operator[](int position);


	/**
		\brief Parallel comparison of theoretical spectra with an experimental spectrum.
		\param candidates reference to a set of peptide sequence candidates
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 when successfully finished; -1 when an error occurred
	*/ 
	int parallelCompareAndStore(cCandidateSet& candidates, bool& terminatecomputation);


	/**
		\brief Add a new spectrum to the list when the number cParameters::hitsreported has not been exceeded or when it has been exceeded but
		the score of the newly added spectrum is better than the worst score of a peptide in the list (thread-safe).
		\param theoreticalspectrum reference to a newly added spectrum
		\param theoreticalpeaksrealsize the number of peaks in the newly added spectrum (required because of a performance improvement)
	*/ 
	void addButDoNotFitSize(cTheoreticalSpectrum& theoreticalspectrum, int theoreticalpeaksrealsize);


	/**
		\brief Sort the list according to a score defined by cParameters::scoretype and crop the list according cParameters::hitsreported.
	*/ 
	void sortAndFitSize();

};


#endif

