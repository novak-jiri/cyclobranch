/**
	\file cTheoreticalSpectrumList.h
	\brief The representation of lists of theoretical spectra.
*/


#ifndef _CTHEORETICALSPECTRUMLIST_H
#define _CTHEORETICALSPECTRUMLIST_H

#include <algorithm>
#include <list>
#include <QMutex>

#include "parallel/cSpectrumAnalyzerThreadMS1.h"
#include "parallel/cSpectrumComparatorThreadMS1.h"
#include "parallel/cSpectrumComparatorThreadMS2.h"
#include "parallel/cSpectrumGeneratorThreadMS1.h"

class cMainThread;


/**
	\brief The class representing lists of theoretical spectra.
*/
class cTheoreticalSpectrumList {

	vector<vector<cTheoreticalSpectrum> > theoreticalspectra;
	list<cTheoreticalSpectrum> spectrumbuffer;
	cMainThread* os;
	cParameters* parameters;
	cDeNovoGraph* graph;

	double currentworstscore;

	QMutex mutex;

	double getCurrentWorstScore();

	void computeNumbersOfCompletedSeries(int fileid);

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
		\brief Clear a specified file id.
		\param fileid file id
	*/
	void clear(int fileid);


	/**
		\brief Initialize the list.
		\param os reference to the main thread of the application
		\param parameters reference to the parameters of the application
		\param graph pointer to the de novo graph
	*/ 
	void initialize(cMainThread& os, cParameters& parameters, cDeNovoGraph* graph);


	/**
		\brief Add a spectrum to the list.
		\param fileid file id
		\param theoreticalspectrum reference to a spectrum
	*/ 
	void add(int fileid, cTheoreticalSpectrum& theoreticalspectrum);


	/**
		\brief Get a reference to a spectrum.
		\param fileid file id
		\param order id of a spectrum
		\retval cTheoreticalSpectrum reference to a spectrum
	*/
	cTheoreticalSpectrum& get(int fileid, int order);


	/**
		\brief Get the number of files.
		\retval int number of files
	*/
	int size();


	/**
		\brief Get the number of spectra in a file.
		\param fileid file id
		\retval int number of spectra
	*/ 
	int size(int fileid);


	/**
		\brief Resize the vector of files.
		\param size size
	*/
	void resize(int size);


	/**
		\brief Resize a file id vector.
		\param fileid file id
		\param size size
	*/
	void resize(int fileid, int size);


	/**
		\brief Parallel comparison of theoretical spectra with an experimental spectrum.
		\param fileid file id
		\param candidates reference to a set of peptide sequence candidates
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 when successfully finished; -1 when an error occurred
	*/ 
	int parallelCompareAndStore(int fileid, cCandidateSet& candidates, bool& terminatecomputation);


	/**
		\brief Update the kNN list of peptide sequence candidates (thread-safe).
		\param theoreticalspectrum reference to the newly added spectrum
		\param theoreticalpeaksrealsize the number of peaks in the newly added spectrum (required because of a performance improvement)
		\param score score of the candidate to be inserted
		\param rxsearchedsequence regular expression corresponding to searched sequence
		\retval double current value of the worst score in the list of candidates
	*/ 
	double updatekNNList(cTheoreticalSpectrum& theoreticalspectrum, int theoreticalpeaksrealsize, double score, regex* rxsearchedsequence);


	/**
		\brief Sort the list according to a score defined by cParameters::scoretype and crop the list according cParameters::hitsreported.
		\param fileid file id
	*/
	void sortAndFitSize(int fileid);

};


#endif

