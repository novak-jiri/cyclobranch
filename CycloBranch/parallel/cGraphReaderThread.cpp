#include "parallel/cGraphReaderThread.h"

#include "gui/cMainThread.h"


int cGraphReaderThread::getTheoreticalSpectraIter(bool cterminalstartingnode, cCandidateSet* candidates, int nodeid, vector<string>& composition, double precursormass, long long &count, int startmodifID, int endmodifID, int middlemodifID, int middlepos, vector<nodeEdge>& perspectivepath, double cummass, bool* terminatecomputation) {
	nodeEdge ne;
	int tmpmiddlemodifID = 0;
	int finish;
	int size;
	bool cycle;

	if (*terminatecomputation) {
		return -1;
	}

	if ((cummass > precursormass) && !isInPpmMassErrorTolerance(cummass, precursormass, parameters->precursormasserrortolerance)) {
		return 0;
	}

	if ((*graph)[nodeid].size() > 0) {

		for (int i = 0; i < (int)(*graph)[nodeid].size(); i++) {

			// accumulation of middle modifications is not allowed
			if (((parameters->peptidetype == branched) || (parameters->peptidetype == lasso)) && (middlemodifID != 0) && ((*graph)[nodeid][i].middlemodifID != 0)) {
				continue;
			}

			// eliminate candidates from cyclic paths
			cycle = false;
			for (int j = 0; j < (int)perspectivepath.size(); j++)  {
				if ((*graph)[nodeid][i].targetnode == perspectivepath[j].nodeid) {
					cycle = true;
					break;
				}
			}

			if (cycle) {
				continue;
			}
			
			ne.nodeid = nodeid;
			ne.edgeid = i;	
			perspectivepath.push_back(ne);

			if ((parameters->peptidetype == branched) || (parameters->peptidetype == lasso)) {
				tmpmiddlemodifID = max(middlemodifID, (*graph)[nodeid][i].middlemodifID);
				if (tmpmiddlemodifID != middlemodifID) {
					middlepos = (int)composition.size();
				}
			}

			composition.push_back((*graph)[nodeid][i].composition);
			finish = getTheoreticalSpectraIter(cterminalstartingnode, candidates, (*graph)[nodeid][i].targetnode, composition, precursormass, count, startmodifID, (*graph)[nodeid][i].endmodifID, tmpmiddlemodifID, middlepos, perspectivepath, cummass + (*graph)[nodeid][i].massdifference, terminatecomputation);
			composition.pop_back();

			perspectivepath.pop_back();
			
			if (finish == -1) {
				return -1;
			}

		}
	}
	else {
		cCandidate candidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);
		if ((candidate.getComposition().compare("") != 0) && (!candidate.hasOnlyArtificialBricks(*bricksdatabasewithcombinations))) {

			if (isInPpmMassErrorTolerance(precursormass, candidate.getPrecursorMass(*bricksdatabasewithcombinations, parameters), parameters->precursormasserrortolerance)) {
				
				cCandidateSet result;

				if (cterminalstartingnode) {
					candidate.revertComposition();
				}

				if ((parameters->peptidetype == branched) || (parameters->peptidetype == lasso)) {
					result.getSet().clear();
					candidate.prepareBranchedCandidate(result, parameters->peptidetype, terminatecomputation);
					candidates->lock();
					for (auto i = result.getSet().begin(); i != result.getSet().end(); ++i) {
						candidates->getSet().insert(*i);
					}
					candidates->unlock();
				}
				else {
					candidates->lock();
					candidates->getSet().insert(candidate);
					candidates->unlock();
				}

				count++;

				candidates->lock();
				size = candidates->size();
				candidates->unlock();

				while (size >= parameters->maximumnumberofcandidates) {
					usleep(1000);

					if (*terminatecomputation) {
						return -1;
					}

					candidates->lock();
					size = candidates->size();
					candidates->unlock();
				}

			}

		}

	}

	return 0;
}


int cGraphReaderThread::reverseCTerminalCandidates(int nodeid, cCandidateSet* candidates, double precursormass, long long &count, int startmodifID, bool* terminatecomputation) {
	vector<string> composition;
	vector<nodeEdge> perspectivepath;

	if (getTheoreticalSpectraIter((*graph)[nodeid].checkIonAnnotation(y_ion), candidates, nodeid, composition, precursormass, count, startmodifID, 0, 0, -1, perspectivepath, (*graph)[nodeid].getMZRatio(), terminatecomputation) == -1) {
		return -1;
	}

	return 0;
}


cGraphReaderThread::cGraphReaderThread() {
	graph = 0;
	bricksdatabasewithcombinations = 0;
	candidates = 0;
	parameters = 0;
	os = 0;
	lastsystemnode = 0;
	mode = 0;
	terminatecomputation = 0;

	// delete thread when run is finished
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}


void cGraphReaderThread::initialize(vector<cDeNovoGraphNode>& graph, cBricksDatabase& bricksdatabasewithcombinations, cCandidateSet& candidates, cParameters* parameters, cMainThread* os, int lastsystemnode, bool& terminatecomputation) {
	this->graph = &graph;
	this->bricksdatabasewithcombinations = &bricksdatabasewithcombinations;
	this->candidates = &candidates;
	this->parameters = parameters;
	this->os = os;
	this->lastsystemnode = lastsystemnode;
	this->mode = mode;
	this->terminatecomputation = &terminatecomputation;
}


void cGraphReaderThread::run() {
	vector<string> composition;
	vector<nodeEdge> perspectivepath;
	long long count = 0;
	int startmodifID = 0;
	double unchargedprecursormass = uncharge(parameters->precursormass, parameters->precursorcharge);

	switch (parameters->peptidetype)
	{
	case cyclic:
		if (getTheoreticalSpectraIter(false, candidates, 0, composition, unchargedprecursormass, count, 0, 0, 0, -1, perspectivepath, (*graph)[0].getMZRatio(), terminatecomputation) == -1) {
			// terminated by user
			return;
		}
		break;
	case linear:
	case branched:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 2 > 0) {
				startmodifID = i - 2;
			}
			if (reverseCTerminalCandidates(i, candidates, unchargedprecursormass, count, startmodifID, terminatecomputation) == -1) {
				// terminated by user
				return;
			}
		}
		break;
	case lasso:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 1/*2*/ > 0) {
				startmodifID = i - 1/*2*/;
			}
			if (reverseCTerminalCandidates(i, candidates, unchargedprecursormass, count, startmodifID, terminatecomputation) == -1) {
				// terminated by user
				return;
			}
		}
		break;
	case linearpolysaccharide:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 1 > 0) {
				startmodifID = i - 1;
			}
			if (reverseCTerminalCandidates(i, candidates, unchargedprecursormass, count, startmodifID, terminatecomputation) == -1) {
				// terminated by user
				return;
			}
		}
		break;
	default:
		break;
	}

}

