/**
	\file cGraphReaderThread.h
	\brief The thread which scans the de novo graph for peptide sequence candidates.
*/


#ifndef _CGRAPHREADERTHREAD_H
#define _CGRAPHREADERTHREAD_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <QThread>

#include "core/cParameters.h"
#include "core/cCandidateSet.h"
#include "core/cDeNovoGraphNode.h"
#include "core/cBricksDatabase.h"


using namespace std;


class cMainThread;


/**
	\brief The class representing a thread which scans the de novo graph for peptide sequence candidates.
*/
class cGraphReaderThread : public QThread {

	Q_OBJECT
	
private:

	vector<cDeNovoGraphNode>* graph;
	cBricksDatabase* bricksdatabasewithcombinations;
	cCandidateSet* candidates;
	cParameters* parameters;
	cMainThread* os;
	int lastsystemnode;
	long long unsigned* count;
	int scanmode;
	bool* terminatecomputation;

	int getCandidatesIter(eFragmentIonType startiontype, cCandidateSet* candidates, int nodeid, vector<string>& composition, double precursormass, int startmodifID, int endmodifID, int middlemodifID, int middlepos, vector<nodeEdge>& perspectivepath, double cummass, string& startsummary, bool* terminatecomputation);

	double getCandidatePrecursorMass(string& startsummary, vector<nodeEdge>& perspectivepath);

	cSummaryFormula getCandidateSummaryFormula(string& startsummary, vector<nodeEdge>& perspectivepath);

public:


	/**
		\brief The constructor.
	*/ 
	cGraphReaderThread();


	/**
		\brief Initialize the graph reader.
		\param graph reference to the de novo graph
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param candidates reference to a set where peptide sequence candidates will be stored
		\param parameters pointer to program parameters
		\param os pointer to the main thread of the application
		\param lastsystemnode position of the last system node in the de novo graph
		\param count an output number of peptide sequence candidates
		\param scanmode 0 = get candidates; 1 = calculate the number of peptide sequence candidates
		\param terminatecomputation reference to a variable determining that the thread must be stopped
	*/ 
	void initialize(vector<cDeNovoGraphNode>& graph, cBricksDatabase& bricksdatabasewithcombinations, cCandidateSet& candidates, cParameters* parameters, cMainThread* os, int lastsystemnode, long long unsigned& count, int scanmode, bool& terminatecomputation);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();

};


#endif

