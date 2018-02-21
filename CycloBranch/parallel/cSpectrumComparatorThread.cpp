#include "parallel/cSpectrumComparatorThread.h"

#include "core/cTheoreticalSpectrumList.h"


bool compareBandAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(b_ion) > b.getNumberOfMatchedPeaks(b_ion)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(b_ion) < b.getNumberOfMatchedPeaks(b_ion)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	return false;
}


bool compareBBwaterLossAndAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_water_loss) > b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_water_loss)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_water_loss) < b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_water_loss)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	return false;
}


bool compareBBammoniaLossAndAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_ammonia_loss) > b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_ammonia_loss)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(b_ion) + a.getNumberOfMatchedPeaks(b_ion_ammonia_loss) < b.getNumberOfMatchedPeaks(b_ion) + b.getNumberOfMatchedPeaks(b_ion_ammonia_loss)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	return false;
}


bool compareYBandAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaksYB() > b.getNumberOfMatchedPeaksYB()) {
		return true;
	}
	if (a.getNumberOfMatchedPeaksYB() < b.getNumberOfMatchedPeaksYB()) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	return false;
}


bool compareYandAllIonsDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	if (a.getNumberOfMatchedPeaks(y_ion) > b.getNumberOfMatchedPeaks(y_ion)) {
		return true;
	}
	if (a.getNumberOfMatchedPeaks(y_ion) < b.getNumberOfMatchedPeaks(y_ion)) {
		return false;
	}
	if (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks()) {
		return true;
	}
	return false;
}


bool compareWeightedIntensityDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	return (a.getWeightedIntensityScore() > b.getWeightedIntensityScore());
}


bool compareNumberOfMatchedPeaksDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	return (a.getNumberOfMatchedPeaks() > b.getNumberOfMatchedPeaks());
}


bool compareNumberOfMatchedBricksDesc(cTheoreticalSpectrum& a, cTheoreticalSpectrum& b) {
	return (a.getNumberOfMatchedBricks() > b.getNumberOfMatchedBricks());
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
	if ((parameters->mode == 0) && (parameters->generatebrickspermutations)) {
		candidate.getPermutations(permutations, terminatecomputation);
	}
	else {
		permutations.getSet().insert(candidate);
	}

	cTheoreticalSpectrum tsp;
	tsp.resizePeakList(5000);
	int theoreticalpeaksrealsize = 0;

	for (auto i = permutations.getSet().begin(); i != permutations.getSet().end(); ++i) {

		if (*terminatecomputation) {
			//terminated by user
			return;
		}

		tsp.clear(false); // hot
		tsp.setParameters(parameters);
		tsp.setCandidate((cCandidate&)*i);

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
		case lasso:
			theoreticalpeaksrealsize = tsp.compareLasso(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
			break;
		case linearpolysaccharide:
			theoreticalpeaksrealsize = tsp.compareLinearPolysaccharide(peaklist, *bricksdatabasewithcombinations, false, *rxsequencetag, *rxsearchedsequence);
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
		case b_ions_and_b_water_loss_ions:
			score = tsp.getNumberOfMatchedPeaks(b_ion) + tsp.getNumberOfMatchedPeaks(b_ion_water_loss);
			break;
		case b_ions_and_b_ammonia_loss_ions:
			score = tsp.getNumberOfMatchedPeaks(b_ion) + tsp.getNumberOfMatchedPeaks(b_ion_ammonia_loss);
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

