#include "parallel/cGraphReaderThread.h"

#include "gui/cMainThread.h"


int cGraphReaderThread::getCandidatesIter(eFragmentIonType startiontype, cCandidateSet* candidates, int nodeid, vector<string>& composition, double precursormass, int startmodifID, int endmodifID, int middlemodifID, int middlepos, vector<nodeEdge>& perspectivepath, double cummass, string& startsummary, bool* terminatecomputation) {
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
			finish = getCandidatesIter(startiontype, candidates, (*graph)[nodeid][i].targetnode, composition, precursormass, startmodifID, (*graph)[nodeid][i].endmodifID, tmpmiddlemodifID, middlepos, perspectivepath, cummass + (*graph)[nodeid][i].massdifference, startsummary, terminatecomputation);
			composition.pop_back();

			perspectivepath.pop_back();
			
			if (finish == -1) {
				return -1;
			}

		}
	}
	else {
		cCandidate candidate(composition, perspectivepath, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);

		bool cterminalstartingnode = ((parameters->peptidetype == linear) || (parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) && (startiontype == y_ion);

		if (!cterminalstartingnode && (((parameters->peptidetype == linear) && !parameters->cyclicnterminus && !parameters->cycliccterminus) || (parameters->peptidetype == branched))) {	
			if (candidate.hasLastBrickArtificial(*bricksdatabasewithcombinations) && (composition.size() > 0)) {
				bid = atoi(composition.back().c_str()) + 1; // offset of -H2O brick
				composition.pop_back();
				composition.push_back(to_string(bid));
				candidate.setCandidate(composition, perspectivepath, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);			
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
			candidate.setCandidate(composition, perspectivepath, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);			
		}

		if ((candidate.getComposition().compare("") != 0) && !candidate.hasOnlyArtificialBricks(*bricksdatabasewithcombinations) && !candidate.hasLastBrickInvalid(*bricksdatabasewithcombinations)) {

			cEdge lastedge;
			if (perspectivepath.size() > 0) {
				lastedge = (*graph)[perspectivepath.back().nodeid][perspectivepath.back().edgeid];
			}

			if (isInPpmMassErrorTolerance(precursormass, getCandidatePrecursorMass(startsummary, perspectivepath), parameters->precursormasserrortolerance)
				&& (candidate.hasLastBrickArtificial(*bricksdatabasewithcombinations) || ((startiontype != fragmentIonTypeEnd) && (startiontype == lastedge.sourceion)
				&& ((lastedge.targetion == precursor_ion) || ((parameters->peptidetype != linear) && (parameters->peptidetype != branched)))
				&& ((lastedge.targetion == linear_polyketide_precursor_ion_h_h) || (lastedge.targetion == linear_polyketide_precursor_ion_h_oh) || (lastedge.targetion == linear_polyketide_precursor_ion_oh_oh) || (parameters->peptidetype != linearpolyketide))
				&& ((lastedge.targetion == cyclic_polyketide_precursor_ion) || (parameters->peptidetype != cyclicpolyketide))))) {

				cCandidateSet result;
				cSummaryFormula sumform = getCandidateSummaryFormula(startsummary, perspectivepath);
				candidate.setSummaryFormula(sumform);

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
				else if ((parameters->peptidetype == linearpolyketide) && candidate.hasLastBrickArtificial(*bricksdatabasewithcombinations) && (composition.size() > 0) && (perspectivepath.size() > 0)) {

					if (scanmode == 0) {

						candidates->lock();
						cSummaryFormula candidateformula = candidate.getSummaryFormula();

						if ((*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l1h_ion) || (*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l1oh_ion)) {
							bid = atoi(composition.back().c_str()) + 1; // offset of -H2O brick
							composition.pop_back();
							composition.push_back(to_string(bid));
							candidate.setCandidate(composition, perspectivepath, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);
							candidate.setSummaryFormula(candidateformula);
							candidates->getSet().insert(candidate);

							bid = atoi(composition.back().c_str()) + 3; // offset of -H2 brick
							composition.pop_back();
							composition.push_back(to_string(bid));
							candidate.setCandidate(composition, perspectivepath, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);
							candidate.setSummaryFormula(candidateformula);
							candidates->getSet().insert(candidate);
						}

						if ((*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l2h_ion) || (*graph)[perspectivepath[0].nodeid].checkIonAnnotation(l2oh_ion)) {
							candidates->getSet().insert(candidate);		// nop

							bid = atoi(composition.back().c_str()) + 4; // offset of -O brick
							composition.pop_back();
							composition.push_back(to_string(bid));
							candidate.setCandidate(composition, perspectivepath, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);
							candidate.setSummaryFormula(candidateformula);
							candidates->getSet().insert(candidate);
						}

						candidates->unlock();

					}
					else {
						*count += 2;
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


double cGraphReaderThread::getCandidatePrecursorMass(string& startsummary, vector<nodeEdge>& perspectivepath) {
	double mass = 0;
	cSummaryFormula formula;
	formula.addFormula(startsummary);
	for (int i = 0; i < (int)perspectivepath.size(); i++) {
		if ((*graph)[perspectivepath[i].nodeid][perspectivepath[i].edgeid].summary.empty()) {
			mass += (*graph)[perspectivepath[i].nodeid][perspectivepath[i].edgeid].massdifference;
		}
		else {
			formula.addFormula((*graph)[perspectivepath[i].nodeid][perspectivepath[i].edgeid].summary);		
		}
	}
	return mass + formula.getMass();
}


cSummaryFormula cGraphReaderThread::getCandidateSummaryFormula(string& startsummary, vector<nodeEdge>& perspectivepath) {
	cSummaryFormula formula;
	formula.setPartial(false);
	formula.addFormula(startsummary);
	for (int i = 0; i < (int)perspectivepath.size(); i++) {
		formula.addFormula((*graph)[perspectivepath[i].nodeid][perspectivepath[i].edgeid].summary);
		if ((*graph)[perspectivepath[i].nodeid][perspectivepath[i].edgeid].summary.empty()) {
			formula.setPartial(true);
		}
	}
	string s = "H+";
	formula.addFormula(s, true);
	return formula;
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
	cEdge e;
	double unchargedprecursormass = charge(uncharge(parameters->precursormass, parameters->precursorcharge), (parameters->precursorcharge > 0)?1:-1);

	switch (parameters->peptidetype) {
		case cyclic:
			e = (*graph)[0][0];
			if (getCandidatesIter(e.targetion, candidates, e.targetnode, composition, unchargedprecursormass, 0, 0, 0, -1, perspectivepath, (*graph)[e.targetnode].getMZRatio(), e.summary, terminatecomputation) == -1) {
				// terminated
				return;
			}
			break;
		case linear:
		case branched:
			for (int i = 0; i < (*graph)[0].size(); i++) {
				e = (*graph)[0][i];
				if (getCandidatesIter(e.targetion, candidates, e.targetnode, composition, unchargedprecursormass, e.startmodifID, 0, 0, -1, perspectivepath, (*graph)[e.targetnode].getMZRatio(), e.summary, terminatecomputation) == -1) {
					// terminated
					return;
				}
			}
			break;
		case branchcyclic:
			for (int i = 0; i < (*graph)[0].size(); i++) {
				e = (*graph)[0][i];
				if (getCandidatesIter(e.targetion, candidates, e.targetnode, composition, unchargedprecursormass, e.startmodifID, 0, 0, -1, perspectivepath, (*graph)[e.targetnode].getMZRatio(), e.summary, terminatecomputation) == -1) {
					// terminated
					return;
				}
			}
			break;
		case linearpolyketide:
			for (int i = 0; i < (*graph)[0].size(); i++) {
				e = (*graph)[0][i];
				if (getCandidatesIter(e.targetion, candidates, e.targetnode, composition, unchargedprecursormass, e.startmodifID, 0, 0, -1, perspectivepath, (*graph)[e.targetnode].getMZRatio(), e.summary, terminatecomputation) == -1) {
					// terminated
					return;
				}
			}
			break;
		case cyclicpolyketide:
			for (int i = 0; i < (*graph)[0].size(); i++) {
				e = (*graph)[0][i];
				if (getCandidatesIter(e.targetion, candidates, e.targetnode, composition, unchargedprecursormass, 0, 0, 0, -1, perspectivepath, (*graph)[e.targetnode].getMZRatio(), e.summary, terminatecomputation) == -1) {
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

