#include "gui/cMainThread.h"


QString appname = "CycloBranch";
QString appversion = "v. 1.0.850 (64-bit)";


bool cMainThread::checkModifications(cParameters& parameters, cSequence& sequence, int& startmodifid, int& endmodifid, int& middlemodifid, string& errormessage) {
	startmodifid = 0;
	endmodifid = 0;
	middlemodifid = 0;
	errormessage = "";

	if ((parameters.peptidetype == linear) || (parameters.peptidetype == branched) || (parameters.peptidetype == lasso) || (parameters.peptidetype == linearpolysaccharide)) {

		if ((parameters.peptidetype == linear) || (parameters.peptidetype == branched) || (parameters.peptidetype == linearpolysaccharide)) {
			startmodifid = -1;
			endmodifid = -1;
		}

		if ((parameters.peptidetype == branched) || (parameters.peptidetype == lasso)) {
			middlemodifid = -1;
		}

		for (int i = 0; i < (int)parameters.searchedmodifications.size(); i++) {

			if ((parameters.peptidetype == linear) || (parameters.peptidetype == branched) || (parameters.peptidetype == linearpolysaccharide)) {
				if (parameters.searchedmodifications[i].name.compare(sequence.getNTterminalModification()) == 0) {
					startmodifid = i;
				}

				if (parameters.searchedmodifications[i].name.compare(sequence.getCTterminalModification()) == 0) {
					endmodifid = i;
				}
			}

			if ((parameters.peptidetype == branched) || (parameters.peptidetype == lasso)) {
				if (parameters.searchedmodifications[i].name.compare(sequence.getBranchModification()) == 0) {
					middlemodifid = i;
				}
			}

		}

		if (startmodifid == -1) {
			errormessage = "The N-terminal modification in the sequence " + sequence.getSequence() + " is not listed in the field 'N-terminal and C-terminal modifications': " + sequence.getNTterminalModification();

			return false;
		}

		if (endmodifid == -1) {
			errormessage = "The C-terminal modification in the sequence " + sequence.getSequence() + " is not listed in the field 'N-terminal and C-terminal modifications': " + sequence.getCTterminalModification();
			return false;
		}

		if (middlemodifid == -1) {
			errormessage = "The branch modification in the sequence " + sequence.getSequence() + " is not listed in the field 'N-terminal and C-terminal modifications': " + sequence.getBranchModification();
			return false;
		}
		
	}

	return true;
}


void cMainThread::parseBranch(peptideType peptidetype, string& composition, vector<string>& vectorcomposition, int& branchstart, int& branchend) {
	string s = composition;
	cBrick b;
	branchstart = -1;
	branchend = -1;

	if ((peptidetype == branched) || (peptidetype == lasso)) {
		int i = 0;
		while (i < (int)s.size()) {
			if (s[i] == '\\') {
				s.erase(s.begin() + i);
			}
			else {
				i++;
			}
		}

		for (int i = 0; i < (int)s.size(); i++) {
			if (s[i] == '(') {
				if (i > 0) {
					b.clear();
					b.setComposition(s.substr(0, i - 1), false);
					branchstart = getNumberOfBricks(b.getComposition());
					s[i] = '-';
				}
				else {
					s.erase(s.begin());
					branchstart = 0;
				}
				break;
			}
		}

		for (int i = 0; i < (int)s.size(); i++) {
			if (s[i] == ')') {
				b.clear();
				b.setComposition(s.substr(0, i - 1), false);
				branchend = getNumberOfBricks(b.getComposition()) - 1;
				if (i < (int)s.size() - 1) {
					s[i] = '-';
				}
				else {
					s.erase(s.begin() + i);
				}
				break;
			}
		}

		if (branchend <= branchstart) {
			branchstart = -1;
			branchend = -1;
		}
	}

	b.clear();
	b.setComposition(s, false);
	b.explodeToStringComposition(vectorcomposition);
}


bool cMainThread::checkRegex(cParameters& parameters, string& sequence, string& errormessage) {
	errormessage = "";

	if (sequence.compare("") == 0) {
		errormessage = "The sequence is empty.";
		return false;
	}

	regex rx;
	// [^\\[\\]]+ is used instead of .+ to prevent from a too complex regex error
	switch (parameters.peptidetype)
	{
	case linear:
	case cyclic:
	case linearpolysaccharide:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case branched:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case lasso:
		rx = "(^(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$|^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\)(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?$)";
		break;
	case other:
	default:
		rx = ".*";
		break;
	}

	try {
		if (!(regex_search(sequence, rx))) {
			errormessage = "The format of sequence is invalid: " + sequence + ".";
			return false;
		}
	}
	catch (std::regex_error& e) {
		errormessage = "cMainThread::checkRegex: regex_search failed, error no. " + to_string(e.code());
		return false;
	}

	return true;
}


cMainThread::cMainThread(cParameters& parameters, bool enablelogwindow, bool enablestdout) {
	this->parameters = parameters;
	this->enablelogwindow = enablelogwindow;
	this->enablestdout = enablestdout;

	// delete thread when run is finished
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

	terminatecomputation = false;
	graphreaderisworking = false;
}


void cMainThread::setGraphReaderIsWorking(bool working) {
	graphreaderisworking = working;
}


bool cMainThread::isGraphReaderWorking() {
	return graphreaderisworking;
}


cMainThread& cMainThread::operator<<(const char* x) {
	if (enablelogwindow) {
		emit message(x);
	}

	if (enablestdout) {
		cout << x;
	}

	return *this;
}


cMainThread& cMainThread::operator<<(const string& x) {
	if (enablelogwindow) {
		emit message(x.c_str());
	}

	if (enablestdout) {
		cout << x;
	}

	return *this;
}


cMainThread& cMainThread::operator<<(StandardEndLine manip) {
	if (enablelogwindow) {
		QString qs = "\n";
		emit message(qs);
	}

	if (enablestdout) {
		manip(cout);
		cout.flush();
	}

	return *this;
}


void cMainThread::run() {

	emitStartSignals();

	cMainThread* os = this;
	string errormessage;
	
	cCandidateSet candidates;
	candidates.getSet().clear();

	QTime time;
	time.start();

	*os << "====================================================================================================" << endl;

	*os << appname.toStdString() << " started at " << time.currentTime().toString().toStdString() << "." << endl << endl;

	parameters.setOutputStream(*os);
	if (parameters.checkAndPrepare() == -1) {
		emit safeExit();
		return;
	}
	*os << parameters.printToString();

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		if (!parameters.bricksdatabase.replaceAcronymsByIDs(parameters.sequencetag, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emit safeExit();
			return;	
		}
	}

	if (parameters.mode == singlecomparison) {
		errormessage = "";
		if (!checkRegex(parameters, parameters.searchedsequence, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emit safeExit();
			return;	
		}
	}

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		if (!parameters.bricksdatabase.replaceAcronymsByIDs(parameters.searchedsequence, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emit safeExit();
			return;	
		}
	}

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		if (parameters.peptidetype != cyclic) {
			// check summary formulas of modifications
			errormessage = "";
			cSummaryFormula formula;
			for (int i = 0; i < (int)parameters.searchedmodifications.size(); i++) {
				formula.clear();
				formula.setFormula(parameters.searchedmodifications[i].summary);
				if (formula.isValid(errormessage)) {
					parameters.searchedmodifications[i].massdifference = formula.getMass();
				}
				else {
					*os << errormessage << endl;
					emit safeExit();
					return;	
				}
			}
		}
	}

	parameters.peaklist.sortbyMass();
	parameters.peaklist.cropMinimumMZRatio(parameters.minimummz);

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		parameters.peaklist.cropMaximumMZRatio(uncharge(parameters.precursormass, parameters.precursorcharge));
	}

	if (parameters.peaklist.normalizeIntenzity() == -1) {
		*os << "Error: the spectrum cannot be normalized; the maximum intensity is <= 0." << endl;
		emit safeExit();
		return;
	}
	parameters.peaklist.cropIntenzity(parameters.minimumrelativeintensitythreshold);
	//parameters.peaklist.maxHighestPeaksInWindow(10, 50);

	*os << "Peaklist:" << endl;
	*os << parameters.peaklist.print();
	if (parameters.masserrortolerancefordeisotoping > 0) {
		parameters.peaklist.removeIsotopes(parameters.precursorcharge, parameters.masserrortolerancefordeisotoping, this);
	}


	// de novo search engine
	if (parameters.mode == denovoengine) {

		// create the de novo graph
		graph.initialize(*os, parameters);
		if (graph.createGraph(terminatecomputation) == -1) {
			endNow();
			return;
		}

		emit setGraph(graph.printGraph());

		// blind paths are removed
		// note: paths finishing in precursor minus X are removed, however, precursor peak is always present thus any candidate should not be lost
		if (parameters.blindedges == 1) {
			if (graph.removeEdgesFormingPathsNotStartingFromFirstNode(terminatecomputation) == -1) {
				endNow();
				return;
			}

			if (graph.removeEdgesFormingPathsNotFinishingInPrecursor(terminatecomputation) == -1) {
				endNow();
				return;
			}
		}

		// blind paths are connected
		if (parameters.blindedges == 2) {
			if (graph.connectEdgesFormingPathsNotStartingFromFirstNode(terminatecomputation) == -1) {
				endNow();
				return;
			}

			if (graph.connectEdgesFormingPathsNotFinishingInPrecursor(terminatecomputation) == -1) {
				endNow();
				return;
			}
		}
		//graph.getBrickDatabaseWithCombinations()->print();

		emit setGraph(graph.printGraph());

		if (graph.removePathsWhichCanBeSubstitutedByLongerPath(terminatecomputation) == -1) {
			endNow();
			return;
		}

		graph.sortEdgesByTargetNodeIntensity();

		emit setGraph(graph.printGraph());

		*os << "Calculating the number of paths forming peptide sequence candidates... ";
		long long unsigned count = 0;
		long long unsigned lastcount = 0;
		graph.startGraphReader(candidates, count, 1, terminatecomputation);
		while (graphreaderisworking) {
			while (count >= lastcount + 1000000) {
				lastcount += 1000000;
				*os << lastcount << " ";
				if (lastcount % 10000000 == 0) {
					*os << endl;
				}
			}
			usleep(1000);
		}		
		*os << " ok" << endl;
		*os << "Number of paths found: " << count << endl << endl;

		if (count > maximumcandidates) {
			*os << "The number of sequence candidates is too high. The identification would be very time-consuming. Please, change the settings and search again." << endl << endl;
			*os << "Aborted." << endl;
			emitEndSignals();
			endNow();
			return;
		}

		count = 0;
		graph.startGraphReader(candidates, count, 0, terminatecomputation);

	}


	// single comparison of a peaklist with a theoretical spectrum
	if (parameters.mode == singlecomparison) {
		vector<string> v;
		cCandidate c;
		int startmodifid, endmodifid, middlemodifid, branchstart, branchend;

		cSequence sequence;
		sequence.setNTterminalModification(parameters.searchedsequenceNtermmodif);
		sequence.setCTterminalModification(parameters.searchedsequenceCtermmodif);
		sequence.setBranchModification(parameters.searchedsequenceTmodif);

		errormessage = "";
		if (!checkModifications(parameters, sequence, startmodifid, endmodifid, middlemodifid, errormessage)) {
			*os << endl << "Error: " << errormessage << endl;
			emitEndSignals();
			endNow();
			return;
		}

		parseBranch(parameters.peptidetype, parameters.searchedsequence, v, branchstart, branchend);

		vector<nodeEdge> netmp;
		c.setCandidate(v, netmp, startmodifid, endmodifid, middlemodifid, branchstart, branchend);
		candidates.getSet().insert(c);
		
		graphreaderisworking = false;
	}


	// database search - MS/MS mode
	if (parameters.mode == databasesearch) {
		string composition;
		vector<string> v;
		cCandidate c;
		vector<nodeEdge> netmp;
		int startmodifid, endmodifid, middlemodifid, branchstart, branchend;

		bool calculatesummaries = false;

		for (int i = 0; i < parameters.sequencedatabase.size(); i++) {

			v.clear();
			c.clear();
			netmp.clear();
			errormessage = "";

			// check peptide type
			if (!calculatesummaries && (parameters.peptidetype != parameters.sequencedatabase[i].getPeptideType())) {
				continue;
			}

			// check format of sequence
			if (!calculatesummaries && !checkRegex(parameters, parameters.sequencedatabase[i].getSequence(), errormessage)) {
				*os << "Ignored sequence: " << errormessage << endl;
				continue;
			}

			// replace acronyms of bricks by ids
			composition = parameters.sequencedatabase[i].getSequence();
			if (!parameters.bricksdatabase.replaceAcronymsByIDs(composition, errormessage)) {
				*os << "Ignored sequence: " << errormessage << endl;
				continue;
			}

			// check whether modification are defined
			if (!checkModifications(parameters, parameters.sequencedatabase[i], startmodifid, endmodifid, middlemodifid, errormessage)) {
				*os << "Ignored sequence: " << errormessage << endl;
				continue;
			}

			// parse branch of a branched or a lasso peptide
			parseBranch(parameters.sequencedatabase[i].getPeptideType(), composition, v, branchstart, branchend);

			// set candidate and check precursor mass error
			c.setCandidate(v, netmp, startmodifid, endmodifid, middlemodifid, branchstart, branchend);
			if (!calculatesummaries && !isInPpmMassErrorTolerance(uncharge(parameters.precursormass, parameters.precursorcharge), c.getPrecursorMass(parameters.bricksdatabase, &parameters), parameters.precursormasserrortolerance)) {
				continue;
			}

			c.setName(parameters.sequencedatabase[i].getName());
			candidates.getSet().insert(c);

			if (calculatesummaries) {
				*os << c.getSummaryFormula(parameters, parameters.sequencedatabase[i].getPeptideType()) << endl;
			}
		}
		
		if (calculatesummaries) {
			*os << endl << "done." << endl;
			emit safeExit();
			return;
		}

		*os << "Number of candidates in the precursor mass error tolerance found: " << candidates.size() << endl;

		graphreaderisworking = false;

	}


	// database search - MS mode
	if (parameters.mode == dereplication) {
		*os << "Comparing theoretical peaks with the peak list... " << endl;
		cTheoreticalSpectrum ts;
		ts.compareMSSpectrum(&parameters);
		theoreticalspectra.initialize(*os, parameters, &graph);
		theoreticalspectra.add(ts);
		*os << " ok" << endl;
	}
	

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		*os << "Comparing theoretical spectra of candidates with the peak list... " << endl;
		theoreticalspectra.initialize(*os, parameters, &graph);
		if (theoreticalspectra.parallelCompareAndStore(candidates, terminatecomputation) == -1) {
			emitEndSignals();
			endNow();
			return;
		}
		*os << " ok" << endl;
	}

	/*
	*os << endl << "Explanation list of peaks (m/z ratio: <Result ID 1> <Result ID 2> etc.):" << endl;
	if (theoreticalspectra.size() > 0) {
		for (int i = 0; i < theoreticalspectra[0].getExperimentalSpectrum().size(); i++) {
			*os << parameters.peaklist[i].mzratio << ": ";
			for (int j = 0; j < theoreticalspectra.size(); j++) {
				if (theoreticalspectra[j].getExperimentalSpectrum()[i].matched > 0) {
					*os << j + 1 << " ";
				}
			}
			*os << endl;
		}
	}
	*/

	/*
	double maxintensity;
	double mzratio;
	if (theoreticalspectra.size() > 0) {
		for (int i = 0; i < theoreticalspectra.size(); i++) {
			*os << i + 1 << ": ";
			maxintensity = 0;
			mzratio = 0;
			for (int j = 0; j < theoreticalspectra[i].getExperimentalSpectrum().size(); j++) {
				if ((theoreticalspectra[i].getExperimentalSpectrum()[j].matched == 0) && (theoreticalspectra[i].getExperimentalSpectrum()[j].intensity > maxintensity)) {
					maxintensity = theoreticalspectra[i].getExperimentalSpectrum()[j].intensity;
					mzratio = theoreticalspectra[i].getExperimentalSpectrum()[j].mzratio;
				}
			}
			*os << mzratio << " " << maxintensity << endl;
		}
	}
	*/
	
	emitEndSignals();

	int secs = time.elapsed() / 1000;
	int mins = (secs / 60) % 60;
	int hrs =  (secs / 3600);
	secs = secs % 60;

	*os << endl << appname.toStdString() << " successfully finished at " << time.currentTime().toString().toStdString();
	*os << " (time elapsed: " << to_string(hrs) << " hrs, " << to_string(mins) << " min, " << to_string(secs) << " sec)." << endl;

	*os << "====================================================================================================" << endl;

}


void cMainThread::emitStartSignals() {
	emit enableRunButtonAndSettings(false);
	emit enableStopButton(true);
	emit enableButtonsHandlingResults(false);

	if (parameters.mode != denovoengine) {
		emit setGraph("");
	}
}


void cMainThread::emitEndSignals() {
	emit sendParameters(parameters);

	emit prepareColumns();

	for (int i = 0; i < theoreticalspectra.size(); i++) {
		emit sendTheoreticalSpectrum(theoreticalspectra[i]);
		theoreticalspectra[i].clear();
	}

	emit fitColumns();

	emit enableRunButtonAndSettings(true);
	emit enableStopButton(false);
	emit enableButtonsHandlingResults(true);
}


void cMainThread::endNow() {
	emit setGraph(graph.printGraph());
	emit safeExit();
	if (terminatecomputation) {
		*this << endl << "Aborted by user." << endl;
	}
}


void cMainThread::stopComputation() {
	*this << endl << endl << "Please wait while stopping threads and generating partial results..." << endl;
	terminatecomputation = true;
}


void cMainThread::graphReaderFinished() {
	graphreaderisworking = false;
}

