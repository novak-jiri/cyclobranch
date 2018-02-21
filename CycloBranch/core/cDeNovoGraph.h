/**
	\file cDeNovoGraph.h
	\brief The implementation of the de novo graph.
*/


#ifndef _CDENOVOGRAPH_H
#define _CDENOVOGRAPH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "core/cParameters.h"
#include "core/cDeNovoGraphNode.h"
#include "core/cPeaksList.h"
#include "core/cBricksDatabase.h"
#include "core/cTheoreticalSpectrumList.h"
#include "parallel/cGraphReaderThread.h"

class cMainThread;


using namespace std;


/**
	\brief Maximum number of candidates to be read from the graph.
*/
const long long unsigned maximumcandidates = 1000000000;


/**
	\brief Compare two nodes.
	\param node1 reference to the first node
	\param node2 reference to the second node
	\retval bool true when the mz ratio of the first node is smaller than the mz ratio of the second node
*/
bool compareNodes(cDeNovoGraphNode& node1, cDeNovoGraphNode& node2);


/**
	\brief The class implementing the de novo graph.
*/
class cDeNovoGraph {

	cParameters* parameters;
	vector<cDeNovoGraphNode> graph;
	cPeaksList sortedpeaklist;
	cBricksDatabase bricksdatabasewithcombinations;
	int lastsystemnode;
	cMainThread* os;
	int startnode;

	cGraphReaderThread* graphreaderthread;

	bool findPath(int sourcenodeid, int edgeid, int targetnodeid, string& composition, string brickspath, int maximumbricksincombination, vector<int>& path);

	// create vectors of edges from temporary unordered_sets
	void createVectorsOfEdges();

	// check whether a path leads to the precursor
	bool finishInPrecursor(int currentnode, double cummass);

public:

	/**
		\brief The constructor.
	*/ 
	cDeNovoGraph();


	/**
		\brief Initialize the graph.
		\param os reference to the main thread of the application
		\param parameters reference to program parameters
	*/ 
	void initialize(cMainThread& os, cParameters& parameters);


	/**
		\brief Create the graph.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = successfully finished, -1 computation terminated
	*/ 
	int createGraph(bool& terminatecomputation);


	/**
		\brief Remove edges which do not form a path finishing in the precursor.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = successfully finished, -1 computation terminated
	*/ 
	int removeEdgesFormingPathsNotFinishingInPrecursor(bool& terminatecomputation);


	/**
		\brief Remove edges which do not start from system nodes.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = successfully finished, -1 computation terminated
	*/ 
	int removeEdgesFormingPathsNotStartingFromFirstNode(bool& terminatecomputation);


	/**
		\brief Connect edges which do not form a path finishing in the precursor.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = successfully finished, -1 computation terminated
	*/ 
	int connectEdgesFormingPathsNotFinishingInPrecursor(bool& terminatecomputation);


	/**
		\brief Connect edges which do not start from system nodes.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = successfully finished, -1 computation terminated
	*/ 
	int connectEdgesFormingPathsNotStartingFromFirstNode(bool& terminatecomputation);


	/**
		\brief Remove paths which can be substituted by longer paths.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = successfully finished, -1 computation terminated
	*/ 
	int removePathsWhichCanBeSubstitutedByLongerPath(bool& terminatecomputation);


	/**
		\brief Start the graph reader.
		\param candidates output set of candidates
		\param count an output number of peptide sequence candidates
		\param scanmode 0 = get candidates; 1 = calculate the number of peptide sequence candidates
		\param terminatecomputation reference to a variable determining that the computation must be stopped
	*/ 
	void startGraphReader(cCandidateSet& candidates, long long unsigned& count, int scanmode, bool& terminatecomputation);


	/**
		\brief Get the number of edges.
		\retval int number of edges
	*/ 
	int getNumberOfEdges();


	/**
		\brief Print all paths in the graph into a stream.
		\param os pointer to the main thread of the application (stream, respectively).
		\param node reference to a starting node
		\param path reference to an auxiliary empty vector of strings
		\param composition reference to an auxiliary empty vector of strings
	*/ 
	void printPaths(cMainThread* os, cDeNovoGraphNode& node, vector<string>& path, vector<string>& composition);


	/**
		\brief Print the graph to a string.
		\retval string the printed graph
	*/ 
	string printGraph();


	/**
		\brief Overloaded operator [].
		\param position position of a node
		\retval cDeNovoGraphNode reference to a node
	*/ 
	cDeNovoGraphNode& operator[](int position);


	/**
		\brief Sort edges by ppm error.
	*/ 
	void sortEdgesByPPMError();

	
	/**
		\brief Sort edges by target node intensity.
	*/ 
	void sortEdgesByTargetNodeIntensity();

	
	/**
		\brief Access to the database of bricks with combinations of bricks.
		\retval cBricksDatabase pointer to a database of bricks
	*/ 
	cBricksDatabase* getBrickDatabaseWithCombinations();

};


#endif