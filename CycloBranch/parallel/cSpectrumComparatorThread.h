/**
	\file cSpectrumComparatorThread.h
	\brief The thread for comparison of a theoretical spectrum with a peak list.
*/


#ifndef _CSPECTRUMCOMPARATORTHREAD_H
#define _CSPECTRUMCOMPARATORTHREAD_H

#include <QRunnable>
#include <QObject>

#include "core/cTheoreticalSpectrum.h"


class cTheoreticalSpectrumList;


/**
	\brief Compare scores of two theoretical spectra (number of b-ions and all ions secondly).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareBandAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (number of b-ions + water loss b-ions and all ions as a secondary score).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareBBwaterLossAndAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (number of b-ions + ammonia loss b-ions and all ions as a secondary score).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareBBammoniaLossAndAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (number of b-ions + y-ions and all ions as a secondary score).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareYBandAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (number of y-ions and all ions as a secondary score).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareYandAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (sum of relative intensities of matched peaks).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareWeightedIntensityDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (number of matched peaks).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareNumberOfMatchedPeaksDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief Compare scores of two theoretical spectra (number of matched bricks).
	\param a first theoretical spectrum
	\param b second theoretical spectrum
	\retval bool true if the score of \a a is greater than the score of \a b
*/ 
bool compareNumberOfMatchedBricksDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b);


/**
	\brief The class representing a thread for comparison of a theoretical spectrum with a peak list.
*/
class cSpectrumComparatorThread : public QObject, public QRunnable {

	Q_OBJECT
	
private:

	cCandidate candidate;
	cPeaksList peaklist;
	cTheoreticalSpectrumList* theoreticalspectrumlist;
	cParameters* parameters;
	regex* rxsequencetag;
	regex* rxsearchedsequence;
	double worstscore;
	bool* terminatecomputation;
	cBricksDatabase* bricksdatabasewithcombinations;

public:


	/**
		\brief Initialize the spectrum comparator.
		\param candidate reference to a peptide sequence candidate
		\param peaklist reference to a peak list
		\param bricksdatabasewithcombinations pointer to a database of bricks with combinations of bricks
		\param theoreticalspectrumlist pointer to the result list of theoretical spectra
		\param parameters pointer to program parameters
		\param rxsequencetag pointer to a regular expression of a sequence tag
		\param rxsearchedsequence pointer to a regular expression of a searched sequence
		\param worstscore worst score in the resulting set of theoretical spectra
		\param terminatecomputation reference to a variable determining that the thread must be stopped
	*/ 
	void initialize(cCandidate& candidate, cPeaksList& peaklist, cBricksDatabase* bricksdatabasewithcombinations, cTheoreticalSpectrumList* theoreticalspectrumlist, cParameters* parameters, regex* rxsequencetag, regex* rxsearchedsequence, double worstscore, bool* terminatecomputation);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();

};


#endif

