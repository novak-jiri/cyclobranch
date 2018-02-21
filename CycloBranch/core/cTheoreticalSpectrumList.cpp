#include "core/cTheoreticalSpectrumList.h"

#include "gui/cMainThread.h"


double cTheoreticalSpectrumList::getCurrentWorstScore() {
	QMutexLocker ml(&mutex);
	return currentworstscore;
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
	spectrumbuffer.clear();
	os = 0;
	parameters = 0;
	graph = 0;

	currentworstscore = 0;
}


void cTheoreticalSpectrumList::initialize(cMainThread& os, cParameters& parameters, cDeNovoGraph* graph) {
	this->os = &os;
	this->parameters = &parameters;
	this->graph = graph;
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
	cPeaksList peaklist = parameters->peaklistseries[parameters->scannumber - 1];
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
	currentworstscore = 0;

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
		*os << endl << endl << "Error: Bad Regular Expression in the field 'Searched Sequence'." << endl << e.what() << endl;
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

				if ((i + 1) % 100 == 0) {
					*os << ".";
				}

				if ((i + 1) % 1000 == 0) {
					*os << i + 1;
				}

				if ((i + 1) % 10000 == 0) {
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

		// move the list to vector
		theoreticalspectra.clear();
		theoreticalspectra.insert(theoreticalspectra.end(), make_move_iterator(spectrumbuffer.begin()), make_move_iterator(spectrumbuffer.end()));
		spectrumbuffer.clear();

	}
	else {

		cTheoreticalSpectrum t(parameters, (cCandidate &)(*candidates.getSet().begin()));
		theoreticalspectra.push_back(t);

	}

	cTheoreticalSpectrum tsp;
	tsp.resizePeakList(5000);

	isotopepatterncache.lock();
	isotopepatterncache.clear();
	isotopepatterncache.unlock();

	unordered_map<string, int> peakdesctoid;
	unordered_map<string, int> isotopeformuladesctoid;

	parameters->peakidtodesc.clear();
	parameters->isotopeformulaidtodesc.clear();

	*os << " ok" << endl << endl << "Preparing the report... " << endl;

	// fill descriptions of peaks
	resultspectra.resize(theoreticalspectra.size());
	for (int i = 0; i < (int)theoreticalspectra.size(); i++) {

		tsp.clear(false);
		tsp.setParameters(parameters);
		tsp.setCandidate(theoreticalspectra[i].getCandidate());

		theoreticalspectra[i].clear(true);

		switch (parameters->peptidetype) {
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
			case linearpolyketide:
				theoreticalpeaksrealsize = tsp.compareLinearPolyketide(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
				break;
			case cyclicpolyketide:
				theoreticalpeaksrealsize = tsp.compareCyclicPolyketide(peaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence);
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
		resultspectra[i] = tsp;
		resultspectra[i].resizePeakList(theoreticalpeaksrealsize);
		resultspectra[i].getTheoreticalPeaks()->reducePeakDescriptions(peakdesctoid);
		resultspectra[i].getTheoreticalPeaks()->reduceIsotopeFormulaDescriptions(isotopeformuladesctoid);

		if ((i + 1) % 100 == 0) {
			*os << ".";
		}

		if ((i + 1) % 1000 == 0) {
			*os << i + 1;
		}

		if ((i + 1) % 10000 == 0) {
			*os << endl;
		}

	}

	convertStringIntUnorderedMapToStringVector(peakdesctoid, parameters->peakidtodesc);
	convertStringIntUnorderedMapToStringVector(isotopeformuladesctoid, parameters->isotopeformulaidtodesc);

	isotopepatterncache.lock();
	isotopepatterncache.clear();
	isotopepatterncache.unlock();

	theoreticalspectra.clear();
	theoreticalspectra.insert(theoreticalspectra.end(), make_move_iterator(resultspectra.begin()), make_move_iterator(resultspectra.end()));
	resultspectra.clear();

	//computeNumbersOfCompletedSeries();

	// sort peaks in theoretical spectra by mass and set real names of peptides
	vector<string> paths;
	string tmps;
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
			tmps = theoreticalspectra[i].getCandidate().getPathAsString();
			auto it = std::find(paths.begin(), paths.end(), tmps);
			if (it == paths.end()) {
				theoreticalspectra[i].setPathId((int)paths.size());
				paths.push_back(tmps);
			}
			else {
				theoreticalspectra[i].setPathId(it - paths.begin());
			}
		}
		// parameters must not be used by viewer, they are not stored/loaded
		theoreticalspectra[i].setParameters(0);
	}

	if ((parameters->mode == denovoengine) || (parameters->mode == databasesearch)) {
		sortAndFitSize();
	}
		
	// -1 = partial results, aborted by user
	return terminatecomputation ? -1 : 0;
}


double cTheoreticalSpectrumList::updatekNNList(cTheoreticalSpectrum& theoreticalspectrum, int theoreticalpeaksrealsize, double score, regex* rxsearchedsequence) {
	QMutexLocker ml(&mutex);

	if (score < currentworstscore) {
		return currentworstscore;
	}

	bool (*comparatorfunction)(const cTheoreticalSpectrum&, const cTheoreticalSpectrum&);

	switch (parameters->scoretype) {
		case b_ions:
			comparatorfunction = &compareBandAllIonsDesc;
			break;
		case b_ions_and_b_dehydrated_ions:
			comparatorfunction = &compareBBwaterLossAndAllIonsDesc;
			break;
		case b_ions_and_b_deamidated_ions:
			comparatorfunction = &compareBBammoniaLossAndAllIonsDesc;
			break;
		case y_ions_and_b_ions:
			comparatorfunction = &compareYBandAllIonsDesc;
			break;
		case y_ions:
			comparatorfunction = &compareYandAllIonsDesc;
			break;
		case weighted_intensity:
			comparatorfunction = &compareWeightedIntensityDesc;
			break;
		case matched_peaks:
			comparatorfunction = &compareNumberOfMatchedPeaksDesc;
			break;
		case matched_bricks:
			comparatorfunction = &compareNumberOfMatchedBricksDesc;
			break;
	}

	auto it1 = lower_bound(spectrumbuffer.begin(), spectrumbuffer.end(), theoreticalspectrum, comparatorfunction);

	theoreticalspectrum.setValidSequence(*rxsearchedsequence);

	if (it1 != spectrumbuffer.end()) {
		double currentscore = 0;

		switch (parameters->scoretype) {
			case b_ions:
				currentscore = it1->getNumberOfMatchedPeaks(b_ion);
				break;
			case b_ions_and_b_dehydrated_ions:
				currentscore = it1->getNumberOfMatchedPeaks(b_ion) + it1->getNumberOfMatchedPeaks(b_ion_dehydrated);
				break;
			case b_ions_and_b_deamidated_ions:
				currentscore = it1->getNumberOfMatchedPeaks(b_ion) + it1->getNumberOfMatchedPeaks(b_ion_deamidated);
				break;
			case y_ions_and_b_ions:
				currentscore = it1->getNumberOfMatchedPeaksYB();
				break;
			case y_ions:
				currentscore = it1->getNumberOfMatchedPeaks(y_ion);
				break;
			case weighted_intensity:
				currentscore = it1->getWeightedIntensityScore();
				break;
			case matched_peaks:
				currentscore = it1->getNumberOfMatchedPeaks();
				break;
			case matched_bricks:
				currentscore = it1->getNumberOfMatchedBricks();
				break;
		}

		if ((currentscore == score) && (it1->getCandidate().getPath() == theoreticalspectrum.getCandidate().getPath())) {
			if (!it1->isValid() && theoreticalspectrum.isValid()) {
				*it1 = theoreticalspectrum;
				it1->resizePeakList(theoreticalpeaksrealsize);
			}
			return currentworstscore;
		}
	}
	
	auto it2 = spectrumbuffer.insert(it1, theoreticalspectrum);
	it2->resizePeakList(theoreticalpeaksrealsize);

	if ((int)spectrumbuffer.size() > parameters->hitsreported) {
		spectrumbuffer.pop_back();

		switch (parameters->scoretype) {
			case b_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaks(b_ion);
				break;
			case b_ions_and_b_dehydrated_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaks(b_ion) + prev(spectrumbuffer.end())->getNumberOfMatchedPeaks(b_ion_dehydrated);
				break;
			case b_ions_and_b_deamidated_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaks(b_ion) + prev(spectrumbuffer.end())->getNumberOfMatchedPeaks(b_ion_deamidated);
				break;
			case y_ions_and_b_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaksYB();
				break;
			case y_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaks(y_ion);
				break;
			case weighted_intensity:
				currentworstscore = prev(spectrumbuffer.end())->getWeightedIntensityScore();
				break;
			case matched_peaks:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaks();
				break;
			case matched_bricks:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedBricks();
				break;
		}

	}

	return currentworstscore;
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

	// might be useful for databasesearch mode
	if ((int)theoreticalspectra.size() > parameters->hitsreported) {
		theoreticalspectra.resize(parameters->hitsreported);
	}

}

