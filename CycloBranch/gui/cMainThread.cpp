#include "gui/cMainThread.h"


bool cMainThread::checkModifications(cParameters& parameters, cSequence& sequence, int& startmodifid, int& endmodifid, int& middlemodifid, string& errormessage) {
	startmodifid = 0;
	endmodifid = 0;
	middlemodifid = 0;
	errormessage = "";

	if ((sequence.getPeptideType() == linear) || (sequence.getPeptideType() == branched) || (sequence.getPeptideType() == lasso) || (sequence.getPeptideType() == linearpolysaccharide)) {

		if ((sequence.getPeptideType() == linear) || (sequence.getPeptideType() == branched) || (sequence.getPeptideType() == linearpolysaccharide)) {
			startmodifid = -1;
			endmodifid = -1;
		}

		if ((sequence.getPeptideType() == branched) || (sequence.getPeptideType() == lasso)) {
			middlemodifid = -1;
		}

		for (int i = 0; i < (int)parameters.searchedmodifications.size(); i++) {

			if ((sequence.getPeptideType() == linear) || (sequence.getPeptideType() == branched) || (sequence.getPeptideType() == linearpolysaccharide)) {
				if (parameters.searchedmodifications[i].name.compare(sequence.getNTterminalModification()) == 0) {
					startmodifid = i;
				}

				if (parameters.searchedmodifications[i].name.compare(sequence.getCTterminalModification()) == 0) {
					endmodifid = i;
				}
			}

			if ((sequence.getPeptideType() == branched) || (sequence.getPeptideType() == lasso)) {
				if (parameters.searchedmodifications[i].name.compare(sequence.getBranchModification()) == 0) {
					middlemodifid = i;
				}
			}

		}

		if (startmodifid == -1) {
			errormessage = "The N-terminal modification in the sequence " + sequence.getSequence() + " is not defined: " + sequence.getNTterminalModification();
			return false;
		}

		if (endmodifid == -1) {
			errormessage = "The C-terminal modification in the sequence " + sequence.getSequence() + " is not defined: " + sequence.getCTterminalModification();
			return false;
		}

		if (middlemodifid == -1) {
			errormessage = "The branch modification in the sequence " + sequence.getSequence() + " is not defined: " + sequence.getBranchModification();
			return false;
		}
		
	}

	return true;
}


cMainThread::cMainThread(cParameters& parameters, cTheoreticalSpectrumList& theoreticalspectrumlist, bool enablelogwindow, bool enablestdout) {
	this->parameters = parameters;
	this->theoreticalspectrumlist = &theoreticalspectrumlist;
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

	if (parameters.mode != denovoengine) {
		emit setGraph("");
	}

	cMainThread* os = this;
	string errormessage;
	
	cCandidateSet candidates;
	candidates.getSet().clear();

	theoreticalspectrumlist->clear();

	QTime time;
	time.start();

	*os << "====================================================================================================" << endl;

	*os << appname.toStdString() << " started at " << time.currentTime().toString().toStdString() << "." << endl << endl;

	parameters.setOutputStream(*os);
	if (parameters.checkAndPrepare() == -1) {
		emitEndSignals();
		return;
	}
	*os << parameters.printToString();

	if (parameters.mode == singlecomparison) {
		if (!checkRegex(parameters.peptidetype, parameters.searchedsequence, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emitEndSignals();
			return;	
		}
	}

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		if (!parameters.bricksdatabase.replaceAcronymsByIDs(parameters.searchedsequence, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emitEndSignals();
			return;	
		}

		if (!parameters.bricksdatabase.replaceAcronymsByIDs(parameters.sequencetag, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emitEndSignals();
			return;	
		}
	}

	parameters.peaklist.sortbyMass();
	parameters.peaklist.cropMinimumMZRatio(parameters.minimummz, parameters.fragmentmasserrortolerance);

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		parameters.peaklist.cropMaximumMZRatio(charge(uncharge(parameters.precursormass, parameters.precursorcharge), (parameters.precursorcharge > 0)?1:-1), parameters.precursormasserrortolerance);
	}

	if (parameters.peaklist.normalizeIntenzity() == -1) {
		*os << "Error: the spectrum cannot be normalized because the maximum intensity is <= 0. The format of peaklist is likely incorrect." << endl;
		emitEndSignals();
		return;
	}
	parameters.peaklist.cropIntenzity(parameters.minimumrelativeintensitythreshold);
	//parameters.peaklist.maxHighestPeaksInWindow(10, 50);

	*os << "Peaklist:" << endl;
	*os << parameters.peaklist.print();
	if (parameters.masserrortolerancefordeisotoping > 0) {
		parameters.peaklist.removeIsotopes(parameters.precursorcharge, parameters.masserrortolerancefordeisotoping, this);
	}


	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		int startmodifid, endmodifid, middlemodifid;
		
		cSequence sequence;
		sequence.setNTterminalModification(parameters.searchedsequenceNtermmodif);
		sequence.setCTterminalModification(parameters.searchedsequenceCtermmodif);
		sequence.setBranchModification(parameters.searchedsequenceTmodif);
		sequence.setPeptideType(parameters.peptidetype);

		// check for names of modifications which are used with the searched sequence
		errormessage = "";
		if (!checkModifications(parameters, sequence, startmodifid, endmodifid, middlemodifid, errormessage)) {
			*os << endl << "Error: " << errormessage << endl;
			emitEndSignals();
			return;
		}

		// single comparison of a peaklist with a theoretical spectrum
		if (parameters.mode == singlecomparison) {
			int branchstart, branchend;
			vector<string> v;
			cCandidate c;
			vector<nodeEdge> netmp;

			parseBranch(parameters.peptidetype, parameters.searchedsequence, v, branchstart, branchend);
			// startmodifid, endmodifid and middlemodifid were filled up by checkModifications
			c.setCandidate(v, netmp, startmodifid, endmodifid, middlemodifid, branchstart, branchend);
			candidates.getSet().insert(c);
		
			graphreaderisworking = false;
		}
	}


	// de novo search engine
	if (parameters.mode == denovoengine) {

		// create the de novo graph
		graph.initialize(*os, parameters);
		if (graph.createGraph(terminatecomputation) == -1) {
			emitEndSignals();
			return;
		}

		emit setGraph(graph.printGraph());

		// blind paths are removed
		// note: paths finishing in precursor minus X are removed, however, precursor peak is always present thus any candidate should not be lost
		if (parameters.blindedges == 1) {
			if (graph.removeEdgesFormingPathsNotStartingFromFirstNode(terminatecomputation) == -1) {
				emitEndSignals();
				return;
			}

			if (graph.removeEdgesFormingPathsNotFinishingInPrecursor(terminatecomputation) == -1) {
				emitEndSignals();
				return;
			}
		}

		// blind paths are connected
		if (parameters.blindedges == 2) {
			if (graph.connectEdgesFormingPathsNotStartingFromFirstNode(terminatecomputation) == -1) {
				emitEndSignals();
				return;
			}

			if (graph.connectEdgesFormingPathsNotFinishingInPrecursor(terminatecomputation) == -1) {
				emitEndSignals();
				return;
			}
		}
		//graph.getBrickDatabaseWithCombinations()->print();

		emit setGraph(graph.printGraph());

		if (graph.removePathsWhichCanBeSubstitutedByLongerPath(terminatecomputation) == -1) {
			emitEndSignals();
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
			return;
		}

		count = 0;
		graph.startGraphReader(candidates, count, 0, terminatecomputation);

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
			if (!calculatesummaries && !checkRegex(parameters.sequencedatabase[i].getPeptideType(), parameters.sequencedatabase[i].getSequence(), errormessage)) {
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

			// parse branch of a branched or a branch-cyclic peptide
			parseBranch(parameters.sequencedatabase[i].getPeptideType(), composition, v, branchstart, branchend);

			// set candidate and check precursor mass error
			c.setCandidate(v, netmp, startmodifid, endmodifid, middlemodifid, branchstart, branchend);
			if (!calculatesummaries && !isInPpmMassErrorTolerance(charge(uncharge(parameters.precursormass, parameters.precursorcharge), (parameters.precursorcharge > 0)?1:-1), c.getPrecursorMass(parameters.bricksdatabase, &parameters), parameters.precursormasserrortolerance)) {
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
			emitEndSignals();
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
		theoreticalspectrumlist->initialize(*os, parameters, &graph);
		theoreticalspectrumlist->add(ts);
		*os << " ok" << endl;
	}
	

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		*os << "Comparing theoretical spectra of candidates with the peak list... " << endl;
		theoreticalspectrumlist->initialize(*os, parameters, &graph);
		if (theoreticalspectrumlist->parallelCompareAndStore(candidates, terminatecomputation) == -1) {
			emitEndSignals();
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
	
	int secs = time.elapsed() / 1000;
	int mins = (secs / 60) % 60;
	int hrs =  (secs / 3600);
	secs = secs % 60;

	*os << endl << appname.toStdString() << " successfully finished at " << time.currentTime().toString().toStdString();
	*os << " (time elapsed: " << to_string(hrs) << " hrs, " << to_string(mins) << " min, " << to_string(secs) << " sec)." << endl;

	*os << "====================================================================================================" << endl;

	emitEndSignals();

}


void cMainThread::emitEndSignals() {
	emit setGraph(graph.printGraph());

	emit sendParameters(parameters);
	emit reportSpectra();

	if (terminatecomputation) {
		*this << endl << "Aborted by user. Partial results have been reported if they are available." << endl;
	}

	emit enableRunButtonAndSettings(true);
	emit enableStopButton(false);
	emit enableButtonsHandlingResults(true);
}


void cMainThread::stopComputation() {
	if (!terminatecomputation) {
		*this << endl << endl << "Stopping..." << endl;
	}
	terminatecomputation = true;
}


void cMainThread::graphReaderFinished() {
	graphreaderisworking = false;
}

