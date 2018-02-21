#include "parallel/cGraphReaderThread.h"

#include "gui/cMainThread.h"


int cGraphReaderThread::getCandidatesIter(bool cterminalstartingnode, cCandidateSet* candidates, int nodeid, vector<string>& composition, double precursormass, int startmodifID, int endmodifID, int middlemodifID, int middlepos, vector<nodeEdge>& perspectivepath, double cummass, bool* terminatecomputation) {
	nodeEdge ne;
	int tmpmiddlemodifID = 0;
	int finish;
	int size;
	bool cycle;
	int bid;

	if (*terminatecomputation) {
		return -1;
	}

	if ((cummass > precursormass) && !isInPpmMassErrorTolerance(cummass, precursormass, parameters->precursormasserrortolerance)) {
		return 0;
	}

	if ((parameters->peptidetype == branchcyclic) && (startmodifID > 0) && (middlemodifID > 0)) {
		return 0;
	}

	if ((*graph)[nodeid].size() > 0) {

		for (int i = 0; i < (int)(*graph)[nodeid].size(); i++) {

			// target charge of an edge must be equal to the source charge of a following edge
			if ((perspectivepath.size() > 0) && ((*graph)[nodeid][i].sourcecharge != (*graph)[perspectivepath.back().nodeid][perspectivepath.back().edgeid].targetcharge)) {
				continue;
			}

			// accumulation of middle modifications is not allowed
			if (((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) && (middlemodifID != 0) && ((*graph)[nodeid][i].middlemodifID != 0)) {
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

			if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
				tmpmiddlemodifID = max(middlemodifID, (*graph)[nodeid][i].middlemodifID);
				if (tmpmiddlemodifID != middlemodifID) {
					middlepos = (int)composition.size();
				}
			}

			composition.push_back((*graph)[nodeid][i].composition);
			finish = getCandidatesIter(cterminalstartingnode, candidates, (*graph)[nodeid][i].targetnode, composition, precursormass, startmodifID, (*graph)[nodeid][i].endmodifID, tmpmiddlemodifID, middlepos, perspectivepath, cummass + (*graph)[nodeid][i].massdifference, terminatecomputation);
			composition.pop_back();

			perspectivepath.pop_back();
			
			if (finish == -1) {
				return -1;
			}

		}
	}
	else {
		cCandidate candidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);

		if (!cterminalstartingnode && (((parameters->peptidetype == linear) && !parameters->cyclicnterminus && !parameters->cycliccterminus) || (parameters->peptidetype == branched) || (parameters->peptidetype == linearpolysaccharide))) {	
			if (candidate.hasLastBrickArtificial(*bricksdatabasewithcombinations) && (composition.size() > 0)) {
				bid = atoi(composition.back().c_str()) + 1; // offset of -H2O brick
				composition.pop_back();
				composition.push_back(to_string(bid));
				candidate.setCandidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);			
			}
		}

		if ((parameters->peptidetype == linear) && parameters->cyclicnterminus && candidate.hasLastBrickArtificial(*bricksdatabasewithcombinations) && (composition.size() > 0)) {
			if (!cterminalstartingnode) {
				bid = atoi(composition.back().c_str()) + 1; // offset of -H2O brick
			}
			else {
				bid = atoi(composition.back().c_str()) + 2; // offset of +H2O brick
			}
			composition.pop_back();
			composition.push_back(to_string(bid));
			candidate.setCandidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);			
		}

#if OLIGOKETIDES == 1
		if ((parameters->peptidetype == linearoligoketide) && candidate.hasLastBrickArtificial(*bricksdatabasewithcombinations) && (composition.size() > 0) && (perspectivepath.size() > 0)) {
		
			if ((*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l1h_ion)) {
				switch (candidate.getKetidePrecursorType(*bricksdatabasewithcombinations))
				{
				case ketide_precursor_h2o:
					bid = atoi(composition.back().c_str()) + 1; // offset of -H2O brick
					break;
				case ketide_precursor_h2:
					bid = atoi(composition.back().c_str()) + 3; // offset of -H2 brick
					break;
				case ketide_precursor_h2o2:
					bid = atoi(composition.back().c_str()) + 8; // offset of -H2O2 brick
					break;
				default:
					bid = atoi(composition.back().c_str()); // nop
					break;
				}
				composition.pop_back();
				composition.push_back(to_string(bid));
				candidate.setCandidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);			
			}

			if ((*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l2h_ion)) {
				switch (candidate.getKetidePrecursorType(*bricksdatabasewithcombinations))
				{
				case ketide_precursor_h2o:
					bid = atoi(composition.back().c_str()) + 4; // offset of -O brick
					break;
				case ketide_precursor_h2:
					bid = atoi(composition.back().c_str()); // nop
					break;
				case ketide_precursor_h2o2:
					bid = atoi(composition.back().c_str()) + 6; // offset of -O2 brick
					break;
				default:
					bid = atoi(composition.back().c_str()); // nop
					break;
				}
				composition.pop_back();
				composition.push_back(to_string(bid));
				candidate.setCandidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);			
			}

			if ((*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l1oh_ion)) {
				switch (candidate.getKetidePrecursorType(*bricksdatabasewithcombinations))
				{
				case ketide_precursor_h2o:
					bid = atoi(composition.back().c_str()) + 3; // offset of -H2 brick
					break;
				case ketide_precursor_h2:
					bid = atoi(composition.back().c_str()) + 7; // offset of -H2+O brick
					break;
				case ketide_precursor_h2o2:
					bid = atoi(composition.back().c_str()) + 1; // offset of -H2O brick
					break;
				default:
					bid = atoi(composition.back().c_str()); // nop
					break;
				}
				composition.pop_back();
				composition.push_back(to_string(bid));
				candidate.setCandidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);			
			}

			if ((*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l2oh_ion)) {
				switch (candidate.getKetidePrecursorType(*bricksdatabasewithcombinations))
				{
				case ketide_precursor_h2o:
					bid = atoi(composition.back().c_str()); // nop
					break;
				case ketide_precursor_h2:
					bid = atoi(composition.back().c_str()) + 5; // offset of +O brick
					break;
				case ketide_precursor_h2o2:
					bid = atoi(composition.back().c_str()) + 4; // offset of -O brick
					break;
				default:
					bid = atoi(composition.back().c_str()); // nop
					break;
				}
				composition.pop_back();
				composition.push_back(to_string(bid));
				candidate.setCandidate(composition, perspectivepath, startmodifID, endmodifID, middlemodifID, middlepos);			
			}
		
		}
#endif

		if ((candidate.getComposition().compare("") != 0) && !candidate.hasOnlyArtificialBricks(*bricksdatabasewithcombinations) && !candidate.hasLastBrickInvalid(*bricksdatabasewithcombinations)) {

			if (isInPpmMassErrorTolerance(precursormass, candidate.getPrecursorMass(*bricksdatabasewithcombinations, parameters), parameters->precursormasserrortolerance)
#if OLIGOKETIDES == 1
				&& (((parameters->peptidetype != linearoligoketide) && (parameters->peptidetype != cyclicoligoketide)) || candidate.checkKetideBlocks(*bricksdatabasewithcombinations, parameters->peptidetype))
#endif
				) {
				
				cCandidateSet result;

				if (cterminalstartingnode) {
					candidate.revertComposition();
				}

				if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
					result.getSet().clear();
					candidate.prepareBranchedCandidates(result, parameters->peptidetype, terminatecomputation);
					if (scanmode == 0) {
						candidates->lock();
						for (auto i = result.getSet().begin(); i != result.getSet().end(); ++i) {
							candidates->getSet().insert(*i);
						}
						candidates->unlock();
					}
					else {
						*count += (long long unsigned)result.getSet().size();
						if (*count > maximumcandidates) {
							return -1;
						}
					}
				}
				else {
					if (scanmode == 0) {
						candidates->lock();
						candidates->getSet().insert(candidate);
						candidates->unlock();
					}
					else {
						*count += 1;
						if (*count > maximumcandidates) {
							return -1;
						}
					}
				}

				if (scanmode == 0) {
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
	scanmode = 0;
	terminatecomputation = 0;
	count = 0;

	// delete thread when run is finished
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}


void cGraphReaderThread::initialize(vector<cDeNovoGraphNode>& graph, cBricksDatabase& bricksdatabasewithcombinations, cCandidateSet& candidates, cParameters* parameters, cMainThread* os, int lastsystemnode, long long unsigned& count, int scanmode, bool& terminatecomputation) {
	this->graph = &graph;
	this->bricksdatabasewithcombinations = &bricksdatabasewithcombinations;
	this->candidates = &candidates;
	this->parameters = parameters;
	this->os = os;
	this->lastsystemnode = lastsystemnode;
	this->count = &count;
	this->scanmode = scanmode;
	this->terminatecomputation = &terminatecomputation;
}


void cGraphReaderThread::run() {
	vector<string> composition;
	vector<nodeEdge> perspectivepath;
	int startmodifID = 0;
	double unchargedprecursormass = charge(uncharge(parameters->precursormass, parameters->precursorcharge), (parameters->precursorcharge > 0)?1:-1);

	switch (parameters->peptidetype)
	{
	case cyclic:
		if (getCandidatesIter(false, candidates, 0, composition, unchargedprecursormass, 0, 0, 0, -1, perspectivepath, (*graph)[0].getMZRatio(), terminatecomputation) == -1) {
			// terminated
			return;
		}
		break;
	case linear:
	case branched:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 2 > 0) {
				startmodifID = i - 2;
			}
			if (getCandidatesIter((*graph)[i].checkIonAnnotation(y_ion), candidates, i, composition, unchargedprecursormass, startmodifID, 0, 0, -1, perspectivepath, (*graph)[i].getMZRatio(), terminatecomputation) == -1) {
				// terminated
				return;
			}
		}
		break;
	case branchcyclic:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 1/*2*/ > 0) {
				startmodifID = i - 1/*2*/;
			}
			if (parameters->searchedmodifications[startmodifID].nterminal) {
				if (getCandidatesIter((*graph)[i].checkIonAnnotation(y_ion), candidates, i, composition, unchargedprecursormass, startmodifID, 0, 0, -1, perspectivepath, (*graph)[i].getMZRatio(), terminatecomputation) == -1) {
					// terminated
					return;
				}
			}
		}
		break;
#if OLIGOKETIDES == 1
	case linearoligoketide:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 4 > 0) {
				startmodifID = i - 4;
			}
			if (getCandidatesIter(false, candidates, i, composition, unchargedprecursormass, startmodifID, 0, 0, -1, perspectivepath, (*graph)[i].getMZRatio(), terminatecomputation) == -1) {
				// terminated
				return;
			}
		}
		break;
	case cyclicoligoketide:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (getCandidatesIter(false, candidates, i, composition, unchargedprecursormass, 0, 0, 0, -1, perspectivepath, (*graph)[i].getMZRatio(), terminatecomputation) == -1) {
				// terminated
				return;
			}
		}
		break;
#endif
	case linearpolysaccharide:
		for (int i = 1; i <= lastsystemnode; i++) {
			if (i - 1 > 0) {
				startmodifID = i - 1;
			}
			if (getCandidatesIter((*graph)[i].checkIonAnnotation(y_ion), candidates, i, composition, unchargedprecursormass, startmodifID, 0, 0, -1, perspectivepath, (*graph)[i].getMZRatio(), terminatecomputation) == -1) {
				// terminated
				return;
			}
		}
		break;
	case other:
		break;
	default:
		break;
	}

}

