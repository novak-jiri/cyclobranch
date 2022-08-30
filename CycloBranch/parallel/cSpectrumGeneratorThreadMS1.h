/**
	\file cSpectrumGeneratorThreadMS1.h
	\brief The thread for generating a theoretical spectrum (MS1 level).
*/


#ifndef _CSPECTRUMGENERATORTHREADMS1_H
#define _CSPECTRUMGENERATORTHREADMS1_H

#include <QRunnable>
#include <QObject>

#include "core/cTheoreticalSpectrum.h"


class cMainThread;


/**
	\brief The class representing a thread for generating a theoretical spectrum (MS1 level).
*/
class cSpectrumGeneratorThreadMS1 : public QObject, public QRunnable {

	Q_OBJECT
	
private:

	cMainThread* mainthread;
	cParameters* parameters;
	int sequencestart;
	int sequencestop;
	cTheoreticalSpectrum* poolspectrum;
	bool* terminatecomputation;


public:


	/**
		\brief Initialize the spectrum generator.
		\param mainthread pointer to the main thread
		\param parameters pointer to program parameters
		\param sequencestart index of the first item to be processed
		\param sequencestop index of the last item to be processed (excluding the last item)
		\param poolspectrum output pool of theoretical peaks
		\param terminatecomputation pointer to a variable determining that the thread must be stopped
	*/ 
	void initialize(cMainThread* mainthread, cParameters* parameters, int sequencestart, int sequencestop, cTheoreticalSpectrum* poolspectrum, bool* terminatecomputation);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();

};


#endif

