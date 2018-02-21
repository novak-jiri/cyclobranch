#include "parallel/cSpectrumComparatorThread.h"

#include "core/cTheoreticalSpectrumList.h"


bool compareBandAllIonsDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(b_ion) > b.getNumberOfMatchedPeaks(b_ion)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(b_ion) < b.getNumberOfMatchedPeaks(b_ion)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks() < b.getNumberOfMatchedPeaks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareBBwaterLossAndAllIonsDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_dehydrated) > b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_dehydrated)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_dehydrated) < b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_dehydrated)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks() < b.getNumberOfMatchedPeaks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareBBammoniaLossAndAllIonsDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_deamidated) > b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_deamidated)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_deamidated) < b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_deamidated)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks() < b.getNumberOfMatchedPeaks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareYBandAllIonsDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaksYB() > b.getNumberOfMatchedPeaksYB()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaksYB() < b.getNumberOfMatchedPeaksYB()) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks() < b.getNumberOfMatchedPeaks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareYandAllIonsDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(y_ion) > b.getNumberOfMatchedPeaks(y_ion)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(y_ion) < b.getNumberOfMatchedPeaks(y_ion)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks() < b.getNumberOfMatchedPeaks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareWeightedIntensityDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getWeightedIntensityScore() > b.getWeightedIntensityScore()) {
		return true;
	}
	if (a.getWeightedIntensityScore() < b.getWeightedIntensityScore()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareNumberOfMatchedPeaksDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks() < b.getNumberOfMatchedPeaks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


bool compareNumberOfMatchedBricksDesc(const cTheoreticalSpectrum& a, const cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedBricks() > b.getNumberOfMatchedBricks()) {
		return true;
	}
	if (a.getNumberOfMatchedBricks() < b.getNumberOfMatchedBricks()) {
		return false;
	}
	if (a.getPathId() < b.getPathId()) {
		return true;
	}
	return false;
}


void cSpectrumComparatorThread::initialize(cCandidate& candidate, cPeaksList& peaklist, cBricksDatabase* bricksdatabasewithcombinations, cTheoreticalSpectrumList* theoreticalspectrumlist, cParameters* parameters, regex* rxsequencetag, regex* rxsearchedsequence, double worstscore, bool* terminatecomputation) {
	this->candidate = candidate;
	this->peaklist = peaklist;
	this->theoreticalspectrumlist = theoreticalspectrumlist;
	this->parameters = parameters;
	this->rxsequencetag = rxsequencetag;
	this->rxsearchedsequence = rxsearchedsequence;
	this->worstscore = worstscore;
	this->terminatecomputation = terminatecomputation;
	this->bricksdatabasewithcombinations = bricksdatabasewithcombinations;
}


void cSpectrumComparatorThread::run() {
	cCandidateSet permutations;
	double score;
		
	permutations.getSet().clear();
	if ((parameters->mode == denovoengine) && (parameters->generatebrickspermutations)) {
		candidate.getPermutations(permutations, terminatecomputation);
	}
	else {
		permutations.getSet().insert(candidate);
	}

	cTheoreticalSpectrum tsp;
	tsp.resizePeakList(5000);
	int theoreticalpeaksrealsize = 0;

	double unchargedprecursormass = charge(uncharge(parameters->precursormass, parameters->precursorcharge), (parameters->precursorcharge > 0)?1:-1);

	for (auto i = permutations.getSet().begin(); i != permutations.getSet().end(); ++i) {

		if (*terminatecomputation) {
			//terminated by user
			return;
		}

		tsp.clear(false); // hot
		tsp.setParameters(parameters);
		tsp.setCandidate((cCandidate&)*i);

		if ((parameters->peptidetype == linearpolyketide) || (parameters->peptidetype == cyclicpolyketide)) {
			if (!tsp.getCandidate().checkKetideBlocks(*bricksdatabasewithcombinations, parameters->peptidetype, parameters->regularblocksorder)) {
				continue;
			}
		}

		switch (parameters->peptidetype)
		{
		case linear:
			theoreticalpeaksrealsize = tsp.compareLinear(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case cyclic:
			theoreticalpeaksrealsize = tsp.compareCyclic(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case branched:
			theoreticalpeaksrealsize = tsp.compareBranched(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case branchcyclic:
			theoreticalpeaksrealsize = tsp.compareBranchCyclic(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case linearpolyketide:
			theoreticalpeaksrealsize = tsp.compareLinearPolyketide(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case cyclicpolyketide:
			theoreticalpeaksrealsize = tsp.compareCyclicPolyketide(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case other:
			break;
		default:
			break;
		}

		// invalid sequence tag
		if (theoreticalpeaksrealsize == -2) {
			continue;
		}

		score = 0;
		switch (parameters->scoretype) {
		case b_ions:
			score = tsp.getNumberOfMatchedPeaks(b_ion);
			break;
		case b_ions_and_b_dehydrated_ions:
			score = tsp.getNumberOfMatchedPeaks(b_ion) + tsp.getNumberOfMatchedPeaks(b_ion_dehydrated);
			break;
		case b_ions_and_b_deamidated_ions:
			score = tsp.getNumberOfMatchedPeaks(b_ion) + tsp.getNumberOfMatchedPeaks(b_ion_deamidated);
			break;
		case y_ions_and_b_ions:
			score = tsp.getNumberOfMatchedPeaksYB();
			break;
		case y_ions:
			score = tsp.getNumberOfMatchedPeaks(y_ion);
			break;
		case weighted_intensity:
			score = tsp.getWeightedIntensityScore();
			break;
		case matched_peaks:
			score = tsp.getNumberOfMatchedPeaks();
			break;
		case matched_bricks:
			score = tsp.getNumberOfMatchedBricks();
			break;
		}

		if (score >= worstscore) {
			theoreticalspectrumlist->addButDoNotFitSize(tsp, theoreticalpeaksrealsize);
		}
		
	}

}

