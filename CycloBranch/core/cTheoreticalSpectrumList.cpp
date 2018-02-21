#include "core/cTheoreticalSpectrumList.h"

#include "gui/cMainThread.h"


double cTheoreticalSpectrumList::getCurrentWorstScore() {
	QMutexLocker ml(&mutex);
	return worstScore;
}


void cTheoreticalSpectrumList::computeNumbersOfCompletedSeries() {
	for (int i = 0; i < (int)theoreticalspectra.size(); i++) {
		for (int j = i + 1; j < (int)theoreticalspectra.size(); j++) {\
			if (theoreticalspectra[i].getCandidate().hasEqualTPermutations(theoreticalspectra[j].getCandidate())) {
				theoreticalspectra[i].setNumberOfCompletedSeries(theoreticalspectra[i].getNumberOfCompletedSeries() + 1);
				theoreticalspectra[j].setNumberOfCompletedSeries(theoreticalspectra[j].getNumberOfCompletedSeries() + 1);
			}
		}
	}
}


void cTheoreticalSpectrumList::fixRegularExpression(string& s) {
	if (s.size() == 0) {
		return;
	}

	bool fixbegin = false;
	bool fixend = false;

	if ((s[0] >= '0') && (s[0] <= '9')) {
		fixbegin = true;
	}

	if ((s[s.size() - 1] >= '0') && (s[s.size() - 1] <= '9')) {
		fixend = true;
	}

	if (fixbegin) {
		s = "(^|[^0-9])" + s;
	}

	if (fixend) {
		s += "($|[^0-9])";
	}
}


cTheoreticalSpectrumList::cTheoreticalSpectrumList() {
	clear();
}


void cTheoreticalSpectrumList::clear() {
	theoreticalspectra.clear();
	os = 0;
	parameters = 0;
	graph = 0;

	worstScore = 0;
	worstNumberOfMatchedPeaks = 0;
	refreshlimit = 0;
}


void cTheoreticalSpectrumList::initialize(cMainThread& os, cParameters& parameters, cDeNovoGraph* graph) {
	this->os = &os;
	this->parameters = &parameters;
	this->graph = graph;
	
	refreshlimit = parameters.hitsreported + 500;
}


void cTheoreticalSpectrumList::add(cTheoreticalSpectrum& theoreticalspectrum) {
	theoreticalspectra.push_back(theoreticalspectrum);
}


int cTheoreticalSpectrumList::size() {
	return (int)theoreticalspectra.size();
}


cTheoreticalSpectrum& cTheoreticalSpectrumList::operator[](int position) {
	return theoreticalspectra[position];
}


int cTheoreticalSpectrumList::parallelCompareAndStore(cCandidateSet& candidates, bool& terminatecomputation) {
	cPeaksList peaklist = parameters->peaklistseries[0];
	peaklist.sortbyMass();
	cCandidateSet permutations;
	theoreticalspectra.clear();
	regex rxsequencetag, rxsearchedsequence;
	cSpectrumComparatorThread* comparatorthread;
	string stmp;
	
	cBricksDatabase* bricksdb = 0;
	switch (parameters->mode)
	{
	case denovoengine:
		bricksdb = graph->getBrickDatabaseWithCombinations();
		break;
	case singlecomparison:
		bricksdb = &parameters->bricksdatabase;
		break;
	case databasesearch:
		bricksdb = &parameters->bricksdatabase;
		break;
	default:
		return -1;
		break;
	}
	
	vector<cTheoreticalSpectrum> resultspectra;
	//int pos;
	
	int theoreticalpeaksrealsize = 0;
	worstScore = 0;
	worstNumberOfMatchedPeaks = 0;

	stmp = parameters->sequencetag;
	fixRegularExpression(stmp);

	/*
	pos = (int)stmp.find("(");
	if (pos != string::npos) {
		stmp.replace(pos, 1, "\\(");
	}
	pos = (int)stmp.find(")");
	if (pos != string::npos) {
		stmp.replace(pos, 1, "\\)");
	}
	*/

	try {
		rxsequencetag = stmp;
	}
	catch (regex_error& e) {
		*os << endl << endl << "Error: Bad Regular Expression in Peptide Sequence Tag." << endl << e.what() << endl;
		return -1;
	}

	stmp = parameters->searchedsequence;
	fixRegularExpression(stmp);

	/*
	pos = (int)stmp.find("(");
	if (pos != string::npos) {
		stmp.replace(pos, 1, "\\(");
	}
	pos = (int)stmp.find(")");
	if (pos != string::npos) {
		stmp.replace(pos, 1, "\\)");
	}
	*/

	try {
		rxsearchedsequence = stmp;
	}
	catch (regex_error& e) {
		*os << endl << endl << "Error: Bad Regular Expression in Searched Peptide Sequence." << endl << e.what() << endl;
		return -1;
	}

	if ((parameters->mode == denovoengine) || (parameters->mode == databasesearch)) {

		QThreadPool::globalInstance()->setMaxThreadCount(parameters->maximumnumberofthreads);

		// compare theoretical spectra with the peaklist in parallel
		int i = 0;
		int size;
		candidates.lock();
		size = candidates.size();
		candidates.unlock();
		while ((size > 0) || (os->isGraphReaderWorking())) {

			if (terminatecomputation) {
				break;
			}

			if ((QThreadPool::globalInstance()->activeThreadCount() < QThreadPool::globalInstance()->maxThreadCount()) && (size > 0)) {
				comparatorthread = new cSpectrumComparatorThread();
				candidates.lock();
				auto x = candidates.getSet().begin();
				comparatorthread->initialize((cCandidate&)(*x), peaklist, bricksdb, this, parameters, &rxsequencetag, &rxsearchedsequence, getCurrentWorstScore(), &terminatecomputation);
				candidates.getSet().erase(candidates.getSet().begin());
				candidates.unlock();
				QThreadPool::globalInstance()->start(comparatorthread);

				if ((i % 100 == 0) && (i > 0)) {
					*os << ".";
				}

				if ((i % 1000 == 0) && (i > 0)) {
					*os << i << " ";
				}

				if ((i % 10000 == 0) && (i > 0)) {
					//*os << "(Remaining candidates in buffer: " << size << "; Are they all ?: " << (os->isGraphReaderWorking() ? "no" : "yes") << ")" << endl;
					*os << endl;
				}

				i++;
			}
			else {
				os->usleep(100);
			}

			candidates.lock();
			size = candidates.size();
			candidates.unlock();

		}

		QThreadPool::globalInstance()->waitForDone();

		while (os->isGraphReaderWorking()) {
			os->usleep(1000);
		}

		sortAndFitSize();

	}
	else {

		cTheoreticalSpectrum t(parameters, (cCandidate &)(*candidates.getSet().begin()));
		theoreticalspectra.push_back(t);

	}

	cTheoreticalSpectrum tsp;
	tsp.resizePeakList(5000);

	// fill descriptions of peaks
	resultspectra.resize(theoreticalspectra.size());
	for (int i = 0; i < (int)theoreticalspectra.size(); i++) {

		tsp.clear(false);
		tsp.setParameters(parameters);
		tsp.setCandidate(theoreticalspectra[i].getCandidate());

		switch (parameters->peptidetype)
		{
		case linear:
			theoreticalpeaksrealsize = tsp.compareLinear(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
		case cyclic:
			theoreticalpeaksrealsize = tsp.compareCyclic(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
		case branched:
			theoreticalpeaksrealsize = tsp.compareBranched(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
		case branchcyclic:
			theoreticalpeaksrealsize = tsp.compareBranchCyclic(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
#if OLIGOKETIDES == 1
		case linearoligoketide:
			theoreticalpeaksrealsize = tsp.compareLinearOligoketide(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
		case cyclicoligoketide:
			theoreticalpeaksrealsize = tsp.compareCyclicOligoketide(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
#endif
		case linearpolysaccharide:
			theoreticalpeaksrealsize = tsp.compareLinearPolysaccharide(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
			break;
		case other:
		default:
			break;
		}

		// invalid sequence tag
		if (theoreticalpeaksrealsize == -2) {
			continue;
		}
		resultspectra[i] = tsp;
		resultspectra[i].resizePeakList(theoreticalpeaksrealsize);
	}

	theoreticalspectra = resultspectra;

	//computeNumbersOfCompletedSeries();

	// sort peaks in theoretical spectra by mass and set real names of peptides
	for (int i = 0; i < (int)theoreticalspectra.size(); i++) {
		theoreticalspectra[i].sortByMass();
		theoreticalspectra[i].getCandidate().setRealPeptideName(*bricksdb, parameters->peptidetype);
		theoreticalspectra[i].getCandidate().setAcronymPeptideNameWithHTMLReferences(*bricksdb, parameters->peptidetype);
		theoreticalspectra[i].getCandidate().setAcronyms(*bricksdb);
		if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
			theoreticalspectra[i].getCandidate().setBackboneAcronyms(*bricksdb);
			theoreticalspectra[i].getCandidate().setBranchAcronyms(*bricksdb);
		}
		if (parameters->mode == denovoengine) {
			theoreticalspectra[i].getCandidate().setPath(*graph, parameters);
		}
		// parameters must not be used by viewer, they are not stored/loaded
		theoreticalspectra[i].setParameters(0);
	}
		
	// -1 = partial results, aborted by user
	return terminatecomputation ? -1 : 0;
}


void cTheoreticalSpectrumList::addButDoNotFitSize(cTheoreticalSpectrum& theoreticalspectrum, int theoreticalpeaksrealsize) {
	QMutexLocker ml(&mutex);

	if ((int)theoreticalspectra.size() < parameters->hitsreported) {

		theoreticalspectra.push_back(theoreticalspectrum);
		theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);

		if ((int)theoreticalspectra.size() == parameters->hitsreported) {
			switch (parameters->scoretype) {
			case b_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBandAllIonsDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion);
				break;
			case b_ions_and_b_dehydrated_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBBwaterLossAndAllIonsDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion) + theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion_dehydrated);
				break;
			case b_ions_and_b_deamidated_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBBammoniaLossAndAllIonsDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion) + theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion_deamidated);
				break;
			case y_ions_and_b_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareYBandAllIonsDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaksYB();
				break;
			case y_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareYandAllIonsDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(y_ion);
				break;
			case weighted_intensity:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareWeightedIntensityDesc);
				worstScore = theoreticalspectra.back().getWeightedIntensityScore();
				break;
			case matched_peaks:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareNumberOfMatchedPeaksDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks();
				break;
			case matched_bricks:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareNumberOfMatchedBricksDesc);
				worstScore = theoreticalspectra.back().getNumberOfMatchedBricks();
				break;
			}
			worstNumberOfMatchedPeaks = theoreticalspectra.back().getNumberOfMatchedPeaks();
		}

	}
	else {

		switch (parameters->scoretype) {
		case b_ions:
			if ((worstScore < theoreticalspectrum.getNumberOfMatchedPeaks(b_ion)) || ((worstScore == theoreticalspectrum.getNumberOfMatchedPeaks(b_ion)) && (worstNumberOfMatchedPeaks < theoreticalspectrum.getNumberOfMatchedPeaks()))) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case b_ions_and_b_dehydrated_ions:
			if ((worstScore < theoreticalspectrum.getNumberOfMatchedPeaks(b_ion) + theoreticalspectrum.getNumberOfMatchedPeaks(b_ion_dehydrated)) || ((worstScore == theoreticalspectrum.getNumberOfMatchedPeaks(b_ion) + theoreticalspectrum.getNumberOfMatchedPeaks(b_ion_dehydrated)) && (worstNumberOfMatchedPeaks < theoreticalspectrum.getNumberOfMatchedPeaks()))) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case b_ions_and_b_deamidated_ions:
			if ((worstScore < theoreticalspectrum.getNumberOfMatchedPeaks(b_ion) + theoreticalspectrum.getNumberOfMatchedPeaks(b_ion_deamidated)) || ((worstScore == theoreticalspectrum.getNumberOfMatchedPeaks(b_ion) + theoreticalspectrum.getNumberOfMatchedPeaks(b_ion_deamidated)) && (worstNumberOfMatchedPeaks < theoreticalspectrum.getNumberOfMatchedPeaks()))) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case y_ions_and_b_ions:
			if ((worstScore < theoreticalspectrum.getNumberOfMatchedPeaksYB()) || ((worstScore == theoreticalspectrum.getNumberOfMatchedPeaksYB()) && (worstNumberOfMatchedPeaks < theoreticalspectrum.getNumberOfMatchedPeaks()))) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case y_ions:
			if (worstScore < theoreticalspectrum.getNumberOfMatchedPeaks(y_ion)) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case weighted_intensity:
			if (worstScore < theoreticalspectrum.getWeightedIntensityScore()) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case matched_peaks:
			if (worstScore < theoreticalspectrum.getNumberOfMatchedPeaks()) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		case matched_bricks:
			if (worstScore < theoreticalspectrum.getNumberOfMatchedBricks()) {
				theoreticalspectra.push_back(theoreticalspectrum);
				theoreticalspectra.back().resizePeakList(theoreticalpeaksrealsize);
			}
			break;
		}

		if ((int)theoreticalspectra.size() >= refreshlimit) {
	
			switch (parameters->scoretype) {
			case b_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBandAllIonsDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion);
				break;
			case b_ions_and_b_dehydrated_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBBwaterLossAndAllIonsDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion) + theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion_dehydrated);
				break;
			case b_ions_and_b_deamidated_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBBammoniaLossAndAllIonsDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion) + theoreticalspectra.back().getNumberOfMatchedPeaks(b_ion_deamidated);
				break;
			case y_ions_and_b_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareYBandAllIonsDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaksYB();
				break;
			case y_ions:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareYandAllIonsDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks(y_ion);
				break;
			case weighted_intensity:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareWeightedIntensityDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getWeightedIntensityScore();
				break;
			case matched_peaks:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareNumberOfMatchedPeaksDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedPeaks();
				break;
			case matched_bricks:
				sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareNumberOfMatchedBricksDesc);
				theoreticalspectra.resize(parameters->hitsreported);
				worstScore = theoreticalspectra.back().getNumberOfMatchedBricks();
				break;
			}

			worstNumberOfMatchedPeaks = theoreticalspectra.back().getNumberOfMatchedPeaks();

		}
	}

}


void cTheoreticalSpectrumList::sortAndFitSize() {

	switch (parameters->scoretype) {
	case b_ions:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBandAllIonsDesc);
		break;
	case b_ions_and_b_dehydrated_ions:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBBwaterLossAndAllIonsDesc);
		break;
	case b_ions_and_b_deamidated_ions:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareBBammoniaLossAndAllIonsDesc);
		break;
	case y_ions_and_b_ions:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareYBandAllIonsDesc);
		break;
	case y_ions:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareYandAllIonsDesc);
		break;
	case weighted_intensity:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareWeightedIntensityDesc);
		break;
	case matched_peaks:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareNumberOfMatchedPeaksDesc);
		break;
	case matched_bricks:
		sort(theoreticalspectra.begin(), theoreticalspectra.end(), compareNumberOfMatchedBricksDesc);
		break;
	}

	if ((int)theoreticalspectra.size() > parameters->hitsreported) {
		theoreticalspectra.resize(parameters->hitsreported);
	}

}

