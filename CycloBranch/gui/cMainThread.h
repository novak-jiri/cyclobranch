/**
	\file cMainThread.h
	\brief The thread launched by the command 'Search->Run'.
*/


#ifndef _CMAINTHREAD_H
#define _CMAINTHREAD_H

#include <iostream>
#include <QThread>
#include <QThreadPool>
#include <QTime>
#include <QMutex>

#include "core/utilities.h"
#include "core/cParameters.h"
#include "core/cDeNovoGraph.h"
#include "core/cTheoreticalSpectrum.h"
#include "core/cIsotopePatternCache.h"

using namespace std;


/**
	\brief Global isotope pattern cache.
*/
extern cIsotopePatternCache isotopepatterncache;


/**
	\brief The class representing a thread launched by the command 'Search->Run'.
*/
class cMainThread : public QThread {
	
	Q_OBJECT

private:

	bool enablelogwindow;
	bool enablestdout;
	bool terminatecomputation;
	bool graphreaderisworking;

	cDeNovoGraph graph;
	cTheoreticalSpectrumList* listoftheoreticalspectra;
	cParameters parameters;

	QMutex mutex;

	int paralleloutputstate;
	int paralleloutputsize;

	string getCurrentTime(QTime& time);

	void singleThreadMS1(cMainThread* os, QTime& time, cTheoreticalSpectrum* testspectrum = 0, vector< vector< vector<int> > >* testhintsindexvector = 0, cTheoreticalSpectrumList* testlistoftheoreticalspectra = 0, vector< vector<cPeaksList> >* testunmatchedpeaksvector = 0);

	void multiThreadMS1(cMainThread* os, QTime& time, cTheoreticalSpectrum* testspectrum = 0, vector< vector< vector<int> > >* testhintsindexvector = 0, cTheoreticalSpectrumList* testlistoftheoreticalspectra = 0, vector< vector<cPeaksList> >* testunmatchedpeaksvector = 0);

	void initParallelOutputState(int size);

	void serializeSpectrumPool(vector<cTheoreticalSpectrum*>& spectrumpool, cTheoreticalSpectrum& ts);

	void serializeHintsIndexPool(vector< vector< vector<int> > >& hintsindexpool, vector< vector<int> >& hintsindex);

	void serializeUnmatchedPeaksPool(int peaklistseriesvectorid, vector< vector<cPeaksList> >& unmatchedpeakspool, vector< vector<cPeaksList> >& unmatchedpeaks);

	void serializeTheoreticalSpectrumListPool(int peaklistseriesvectorid, vector<cTheoreticalSpectrumList*>& theoreticalspectrumlistpool, cTheoreticalSpectrumList& theoreticalspectrumlist);

	void printMatrix(cPeaksList& peaklist);

	void printIonEstimations(cPeaksList& peaklist);


public:


	/**
		\brief The constructor.
		\param parameters reference to input paramaters
		\param listoftheoreticalspectra lists of theoretical spectra
		\param enablelogwindow if true then messages are logged into the log window
		\param enablestdout if true then messages are logged into the standard output
	*/ 
	cMainThread(cParameters& parameters, cTheoreticalSpectrumList& listoftheoreticalspectra, bool enablelogwindow = true, bool enablestdout = true);


	/**
		\brief Set the flag that the graph reader is working.
		\param working set true when the graph reader is working, otherwise set false
	*/ 
	void setGraphReaderIsWorking(bool working);


	/**
		\brief Check the flag whether the graph reader is working.
		\retval bool true when the graph reader is working, false otherwise
	*/ 
	bool isGraphReaderWorking();


	/**
		\brief This is the type of std::cout.
	*/ 
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;


	/**
		\brief This is the function signature of std::endl.
	*/ 
    typedef CoutType& (*StandardEndLine)(CoutType&);


	/**
		\brief Overloaded operator <<.
	*/ 
	template<class T> cMainThread& operator<<(const T& x) {
		if (enablelogwindow) {
			emit message(to_string(x).c_str());
		}

		if (enablestdout) {
			cout << x;
		}

		return *this;
	}


	/**
		\brief Overloaded operator <<.
	*/ 
	cMainThread& operator<<(const char* x);


	/**
		\brief Overloaded operator <<.
	*/ 
	cMainThread& operator<<(const string& x);


 	/**
		\brief Define an operator << to take in std::endl.
	*/ 
	cMainThread& operator<<(StandardEndLine manip);


	/**
		\brief Add a value to the output state (for internal threads).
		\param value value
	*/
	void addToParallelOutputState(int value);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();


	/**
		\brief The signals emitted when the thread has successfully finished.
	*/ 
	void emitEndSignals();


signals:


	/**
		\brief A message which will be shown in log window.
		\param s text of the message
	*/ 
	void message(QString s);


	/**
		\brief Enable or disable options 'Search->Run' and 'Search->Settings...'.
		\param enable true == enable, false == disable
	*/ 
	void enableRunButtonAndSettings(bool enable);


	/**
		\brief Enable or disable the option 'Search->Stop'.
		\param enable true == enable, false == disable
	*/ 
	void enableStopButton(bool enable);


	/**
		\brief Enable or disable buttons handling results.
		\param enable true == enable, false == disable
	*/ 
	void enableButtonsHandlingResults(bool enable);


	/**
		\brief Send parameters to cMainWindow when the thread has finished.
		\param parameters parameters
	*/ 
	void sendParameters(cParameters parameters);


	/**
		\brief Report results.
	*/ 
	void reportSpectra();


	/**
		\brief Set the graph into the cGraphWidget.
		\param s graph printed as a string
	*/ 
	void setGraph(string s);


private slots:

	void stopComputation();

	void graphReaderFinished();

};


#endif

