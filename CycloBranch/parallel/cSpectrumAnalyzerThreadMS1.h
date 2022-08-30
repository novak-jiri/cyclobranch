/**
	\file cSpectrumAnalyzerThreadMS1.h
	\brief The thread for an initial check of LC-MS and MSI spectra.
*/


#ifndef _CSPECTRUMANALYZERTHREADMS1_H
#define _CSPECTRUMANALYZERTHREADMS1_H

#include <QRunnable>
#include <QObject>

#include "core/cTheoreticalSpectrum.h"


class cMainThread;


/**
	\brief The class representing a thread for an initial check of LC-MS and MSI spectra.
*/
class cSpectrumAnalyzerThreadMS1 : public QObject, public QRunnable {

	Q_OBJECT
	
private:

	cMainThread* mainthread;
	cParameters* parameters;
	int peaklistseriesvectorid;
	cTheoreticalSpectrum* poolspectrum;
	vector< vector<int> >* hintsindex;
	bool* terminatecomputation;


public:


	/**
		\brief Initialize the spectrum analyzer.
		\param mainthread pointer to the main thread
		\param parameters pointer to program parameters
		\param peaklistseriesvectorid id of a peaklistseriesvector
		\param poolspectrum input pool of theoretical peaks
		\param hintsindex output vector of ids of experimental spectra in which a theoretical peak was found; for every theoretical peak
		\param terminatecomputation pointer to a variable determining that the thread must be stopped
	*/ 
	void initialize(cMainThread* mainthread, cParameters* parameters, int peaklistseriesvectorid, cTheoreticalSpectrum* poolspectrum, vector< vector<int> >* hintsindex, bool* terminatecomputation);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();

};


#endif

