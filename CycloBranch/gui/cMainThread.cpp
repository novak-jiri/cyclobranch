#include "gui/cMainThread.h"


QString appname = "CycloBranch";
QString appversion = "v. 1.0.696 (64-bit)";


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

	if (!parameters.bricksdatabase.replaceAcronymsByIDs(parameters.sequencetag)) {
		*os << "Error: invalid brick acronym typed in the sequence tag." << endl;
		emit safeExit();
		return;	
	}

	if (parameters.mode == 1) {
		if (parameters.searchedsequence.compare("") == 0) {
			*os << "Error: Searched sequence is empty." << endl;
			safeExit();
			return;
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
		default:
			rx = "";
			break;
		}

		try {
			if (!(regex_search(parameters.searchedsequence, rx))) {
				*os << "Error: The format of searched sequence is invalid." << endl;
				safeExit();
				return;
			}
		}
		catch (std::regex_error& e) {
			*os << "cMainThread::run: regex_search failed, error no. " << e.code() << endl;
			safeExit();
			return;
		}
	}

	if (!parameters.bricksdatabase.replaceAcronymsByIDs(parameters.searchedsequence)) {
		*os << "Error: invalid brick acronym typed in the searched sequence." << endl;
		emit safeExit();
		return;	
	}

	bool error = false;
	string errormessage = "";
	for (int i = 0; i < (int)parameters.searchedmodifications.size(); i++) {
		parameters.searchedmodifications[i].massdifference = getMassFromResidueSummary(parameters.searchedmodifications[i].summary, error, errormessage);
		if (error) {
			*os << errormessage << endl;
			emit safeExit();
			return;	
		}
	}

	parameters.peaklist.sortbyMass();
	parameters.peaklist.cropMinimumMZRatio(parameters.minimummz);
	parameters.peaklist.cropMaximumMZRatio(uncharge(parameters.precursormass, parameters.precursorcharge));
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

	//search engine
	if (parameters.mode == 0) {

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

		// process the theoretical spectra
		*os << "Starting the graph reader... ";
		// mode 0 = get candidates
		graph.startGraphReader(candidates, terminatecomputation);
		*os << "ok" << endl << endl;

	}
	// comparison of a theoretical spectrum
	else {

		vector<string> v;
		cCandidate c;
		cBrick b;
		string s;
		int startmodifid, endmodifid, middlemodifid;
		int branchstart = -1;
		int branchend = -1;

		if ((parameters.peptidetype == linear) || (parameters.peptidetype == branched) || (parameters.peptidetype == lasso) || (parameters.peptidetype == linearpolysaccharide)) {

			if ((parameters.peptidetype == linear) || (parameters.peptidetype == branched) || (parameters.peptidetype == linearpolysaccharide)) {
				startmodifid = -1;
				endmodifid = -1;
			}
			else {
				startmodifid = 0;
				endmodifid = 0;
			}

			if ((parameters.peptidetype == branched) || (parameters.peptidetype == lasso)) {
				middlemodifid = -1;
			}
			else {
				middlemodifid = 0;
			}

			for (int i = 0; i < (int)parameters.searchedmodifications.size(); i++) {

				if ((parameters.peptidetype == linear) || (parameters.peptidetype == branched) || (parameters.peptidetype == linearpolysaccharide)) {
					if (parameters.searchedmodifications[i].name.compare(parameters.searchedsequenceNtermmodif) == 0) {
						startmodifid = i;
					}

					if (parameters.searchedmodifications[i].name.compare(parameters.searchedsequenceCtermmodif) == 0) {
						endmodifid = i;
					}
				}

				if ((parameters.peptidetype == branched) || (parameters.peptidetype == lasso)) {
					if (parameters.searchedmodifications[i].name.compare(parameters.searchedsequenceTmodif) == 0) {
						middlemodifid = i;
					}
				}

			}

			if (startmodifid == -1) {
				*os << endl << "Error: Unknown N-terminal modification is used." << endl;
				emitEndSignals();
				endNow();
				return;
			}

			if (endmodifid == -1) {
				*os << endl << "Error: Unknown C-terminal modification is used." << endl;
				emitEndSignals();
				endNow();
				return;
			}

			if (middlemodifid == -1) {
				*os << endl << "Error: Unknown T-modification is used." << endl;
				emitEndSignals();
				endNow();
				return;
			}
		
		}

		s = parameters.searchedsequence;
		if ((parameters.peptidetype == branched) || (parameters.peptidetype == lasso)) {
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
		b.explodeToStringComposition(v);
		vector<nodeEdge> tmp;
		c.setCandidate(v, tmp, startmodifid, endmodifid, middlemodifid, branchstart, branchend);

		candidates.getSet().insert(c);
		graphreaderisworking = false;

	}

	*os << "Comparing theoretical spectra of candidates with the peak list... " << endl;
	*os << "Permutations of combinations of bricks: ";
	if (parameters.generatebrickspermutations) {
		*os << "on" << endl;
	}
	else {
		*os << "off" << endl;
	}
	theoreticalspectra.initialize(*os, parameters, &graph);
	if (theoreticalspectra.parallelCompareAndStore(candidates, terminatecomputation) == -1) {
		emitEndSignals();
		endNow();
		return;
	}
	*os << "ok" << endl;

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

	if (parameters.mode != 0) {
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

