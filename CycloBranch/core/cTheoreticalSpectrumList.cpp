#include "core/cTheoreticalSpectrumList.h"

#include "gui/cMainThread.h"


double cTheoreticalSpectrumList::getCurrentWorstScore() {
	QMutexLocker ml(&mutex);
	return currentworstscore;
}


void cTheoreticalSpectrumList::computeNumbersOfCompletedSeries(int fileid) {
	for (int i = 0; i < (int)theoreticalspectra[fileid].size(); i++) {
		for (int j = i + 1; j < (int)theoreticalspectra[fileid].size(); j++) {\
			if (theoreticalspectra[fileid][i].getCandidate().hasEqualTPermutations(theoreticalspectra[fileid][j].getCandidate())) {
				theoreticalspectra[fileid][i].setNumberOfCompletedSeries(theoreticalspectra[fileid][i].getNumberOfCompletedSeries() + 1);
				theoreticalspectra[fileid][j].setNumberOfCompletedSeries(theoreticalspectra[fileid][j].getNumberOfCompletedSeries() + 1);
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


void cTheoreticalSpectrumList::clear(int fileid) {
	theoreticalspectra[fileid].clear();
}


void cTheoreticalSpectrumList::initialize(cMainThread& os, cParameters& parameters, cDeNovoGraph* graph) {
	this->os = &os;
	this->parameters = &parameters;
	this->graph = graph;
}


void cTheoreticalSpectrumList::add(int fileid, cTheoreticalSpectrum& theoreticalspectrum) {
	theoreticalspectra[fileid].push_back(theoreticalspectrum);
}


cTheoreticalSpectrum& cTheoreticalSpectrumList::get(int fileid, int order) {
	return theoreticalspectra[fileid][order];
}


int cTheoreticalSpectrumList::size() {
	return (int)theoreticalspectra.size();
}


int cTheoreticalSpectrumList::size(int fileid) {
	return (int)theoreticalspectra[fileid].size();
}


void cTheoreticalSpectrumList::resize(int size) {
	theoreticalspectra.resize(size);
}


void cTheoreticalSpectrumList::resize(int fileid, int size) {
	theoreticalspectra[fileid].resize(size);
}


int cTheoreticalSpectrumList::parallelCompareAndStore(int fileid, cCandidateSet& candidates, bool& terminatecomputation) {
	cPeaksList peaklist = parameters->peaklistseriesvector[0][parameters->scannumber - 1];
	peaklist.sortbyMass();
	cCandidateSet permutations;
	theoreticalspectra[fileid].clear();
	regex rxsequencetag, rxsearchedsequence;
	cSpectrumComparatorThreadMS2* comparatorthread;
	string stmp;
	
	cBricksDatabase* bricksdb = 0;
	switch (parameters->mode) {
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

	if (parameters->peptidetype != other) {

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
				comparatorthread = new cSpectrumComparatorThreadMS2();
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
		theoreticalspectra[fileid].clear();
		theoreticalspectra[fileid].insert(theoreticalspectra[fileid].end(), make_move_iterator(spectrumbuffer.begin()), make_move_iterator(spectrumbuffer.end()));
		spectrumbuffer.clear();

	}
	else {

		cTheoreticalSpectrum t(parameters, (cCandidate &)(*candidates.getSet().begin()));
		int cnt = parameters->peaklistseriesvector[0].size();
		for (int i = 0; i < cnt; i++) {
			theoreticalspectra[fileid].push_back(t);
		}

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
	cPeaksList unmatchedpeaksinmatchedpatterns;
	cPeaksList tmppeaklist;
	resultspectra.resize(theoreticalspectra[fileid].size());
	for (int i = 0; i < (int)theoreticalspectra[fileid].size(); i++) {

		tsp.clear(false);
		tsp.setParameters(parameters);
		tsp.setCandidate(theoreticalspectra[fileid][i].getCandidate());

		theoreticalspectra[fileid][i].clear(true);

		unmatchedpeaksinmatchedpatterns.clear();

		if (parameters->mode == singlecomparison) {
			tmppeaklist = parameters->peaklistseriesvector[0][i];
			tmppeaklist.sortbyMass();
		}
		else {
			tmppeaklist = peaklist;
		}

		switch (parameters->peptidetype) {
			case linear:
				theoreticalpeaksrealsize = tsp.compareLinear(tmppeaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
				break;
			case cyclic:
				theoreticalpeaksrealsize = tsp.compareCyclic(tmppeaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
				break;
			case branched:
				theoreticalpeaksrealsize = tsp.compareBranched(tmppeaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
				break;
			case branchcyclic:
				theoreticalpeaksrealsize = tsp.compareBranchCyclic(tmppeaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
				break;
			case linearpolyketide:
				theoreticalpeaksrealsize = tsp.compareLinearPolyketide(tmppeaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
				break;
			case cyclicpolyketide:
				theoreticalpeaksrealsize = tsp.compareCyclicPolyketide(tmppeaklist, *bricksdb, true, rxsequencetag, rxsearchedsequence, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
				break;
			case other:
				theoreticalpeaksrealsize = tsp.compareOther(tmppeaklist, true, unmatchedpeaksinmatchedpatterns, &isotopeformuladesctoid);
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
		//resultspectra[i].getTheoreticalPeaks()->reduceIsotopeFormulaDescriptions(isotopeformuladesctoid);

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

	theoreticalspectra[fileid].clear();
	theoreticalspectra[fileid].insert(theoreticalspectra[fileid].end(), make_move_iterator(resultspectra.begin()), make_move_iterator(resultspectra.end()));
	resultspectra.clear();

	//computeNumbersOfCompletedSeries();

	// sort peaks in theoretical spectra by mass and set real names of peptides
	vector<string> paths;
	string tmps;
	for (int i = 0; i < (int)theoreticalspectra[fileid].size(); i++) {
		theoreticalspectra[fileid][i].sortByMass();
		if (parameters->peptidetype != other) {
			theoreticalspectra[fileid][i].getCandidate().setRealPeptideName(*bricksdb, parameters->peptidetype);
			theoreticalspectra[fileid][i].getCandidate().setAcronymPeptideNameWithHTMLReferences(*bricksdb, parameters->peptidetype);
			theoreticalspectra[fileid][i].getCandidate().setAcronyms(*bricksdb);
		}
		if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
			theoreticalspectra[fileid][i].getCandidate().setBackboneAcronyms(*bricksdb);
			theoreticalspectra[fileid][i].getCandidate().setBranchAcronyms(*bricksdb);
		}
		if (parameters->mode == denovoengine) {
			theoreticalspectra[fileid][i].getCandidate().setPath(*graph, parameters);
			tmps = theoreticalspectra[fileid][i].getCandidate().getPathAsString();
			auto it = std::find(paths.begin(), paths.end(), tmps);
			if (it == paths.end()) {
				theoreticalspectra[fileid][i].setPathId((int)paths.size());
				paths.push_back(tmps);
			}
			else {
				theoreticalspectra[fileid][i].setPathId(it - paths.begin());
			}
		}
		// parameters must not be used by viewer, they are not stored/loaded
		theoreticalspectra[fileid][i].setParameters(0);
	}

	if ((parameters->mode == denovoengine) || (parameters->mode == databasesearch)) {
		sortAndFitSize(fileid);
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

	switch (parameters->scoretype)	{
		case number_of_matched_peaks:
			comparatorfunction = &compareNumberOfMatchedPeaksDesc;
			break;
		case sum_of_relative_intensities:
			comparatorfunction = &compareSumOfRelIntDesc;
			break;
		case number_of_b_ions:
			comparatorfunction = &compareBandAllIonsDesc;
			break;
		case number_of_y_ions:
			comparatorfunction = &compareYandAllIonsDesc;
			break;
		case number_of_b_and_y_ions:
			comparatorfunction = &compareYBandAllIonsDesc;
			break;
		case weighted_ratio_of_matched_peaks:
			comparatorfunction = &compareWeightedRatioDesc;
			break;
		default:
			break;
	}

	auto it1 = lower_bound(spectrumbuffer.begin(), spectrumbuffer.end(), theoreticalspectrum, comparatorfunction);

	theoreticalspectrum.setValidSequence(*rxsearchedsequence);

	if (it1 != spectrumbuffer.end()) {
		double currentscore = 0;

		switch (parameters->scoretype) {
			case number_of_matched_peaks:
				currentscore = it1->getNumberOfMatchedPeaks();
				break;
			case sum_of_relative_intensities:
				currentscore = it1->getSumOfRelativeIntensities();
				break;
			case number_of_b_ions:
				currentscore = it1->getNumberOfMatchedPeaksB();
				break;
			case number_of_y_ions:
				currentscore = it1->getNumberOfMatchedPeaksY();
				break;
			case number_of_b_and_y_ions:
				currentscore = it1->getNumberOfMatchedPeaksYB();
				break;
			case weighted_ratio_of_matched_peaks:
				currentscore = it1->getWeightedRatioOfMatchedPeaks();
				break;
			default:
				break;
		}

		if ((parameters->mode == denovoengine) && (currentscore == score) && (it1->getCandidate().getPath() == theoreticalspectrum.getCandidate().getPath())) {
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
			case number_of_matched_peaks:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaks();
				break;
			case sum_of_relative_intensities:
				currentworstscore = prev(spectrumbuffer.end())->getSumOfRelativeIntensities();
				break;
			case number_of_b_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaksB();
				break;
			case number_of_y_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaksY();
				break;
			case number_of_b_and_y_ions:
				currentworstscore = prev(spectrumbuffer.end())->getNumberOfMatchedPeaksYB();
				break;
			case weighted_ratio_of_matched_peaks:
				currentworstscore = prev(spectrumbuffer.end())->getWeightedRatioOfMatchedPeaks();
				break;
			default:
				break;
		}
	}

	return currentworstscore;
}


void cTheoreticalSpectrumList::sortAndFitSize(int fileid) {

	switch (parameters->scoretype) {
		case number_of_matched_peaks:
			sort(theoreticalspectra[fileid].begin(), theoreticalspectra[fileid].end(), compareNumberOfMatchedPeaksDesc);
			break;
		case sum_of_relative_intensities:
			sort(theoreticalspectra[fileid].begin(), theoreticalspectra[fileid].end(), compareSumOfRelIntDesc);
			break;
		case number_of_b_ions:
			sort(theoreticalspectra[fileid].begin(), theoreticalspectra[fileid].end(), compareBandAllIonsDesc);
			break;
		case number_of_y_ions:
			sort(theoreticalspectra[fileid].begin(), theoreticalspectra[fileid].end(), compareYandAllIonsDesc);
			break;
		case number_of_b_and_y_ions:
			sort(theoreticalspectra[fileid].begin(), theoreticalspectra[fileid].end(), compareYBandAllIonsDesc);
			break;
		case weighted_ratio_of_matched_peaks:
			sort(theoreticalspectra[fileid].begin(), theoreticalspectra[fileid].end(), compareWeightedRatioDesc);
			break;
		default:
			break;
	}

	// might be useful for databasesearch mode
	if ((int)theoreticalspectra[fileid].size() > parameters->hitsreported) {
		theoreticalspectra[fileid].resize(parameters->hitsreported);
	}

}

