#include "gui/cMainThread.h"


cIsotopePatternCache isotopepatterncache;


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

	isotopepatterncache.lock();
	isotopepatterncache.clear();
	isotopepatterncache.unlock();

	int hrs, mins, secs;

	QTime time;
	time.start();

	*os << "====================================================================================================" << endl;

	*os << appname.toStdString() << " started at " << time.currentTime().toString().toStdString() << "." << endl << endl;

	if (parameters.mode == denovoengine) {
		if (parameters.peptidetype == other) {
			*os << "Error: The peptide type 'other' cannot be used in this mode." << endl;
			emitEndSignals();
			return;
		}
	}

	parameters.setOutputStream(*os);
	if (parameters.checkAndPrepare(terminatecomputation) == -1) {
		emitEndSignals();
		return;
	}

	*os << parameters.printToString();

	if ((parameters.mode == singlecomparison) && (parameters.peptidetype != other)) {
		if (!checkRegex(parameters.peptidetype, parameters.searchedsequence, errormessage)) {
			*os << "Error: " << errormessage << endl;
			emitEndSignals();
			return;	
		}
	}

	if ((parameters.mode == denovoengine) || ((parameters.mode == singlecomparison) && (parameters.peptidetype != other)) || ((parameters.mode == databasesearch) && (parameters.peptidetype != other))) {
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

	if (parameters.peaklistseries.size() == 0) {
		*os << "Error: no peaklist found. The format of peaklist is likely incorrect." << endl;
		emitEndSignals();
		return;
	}

	if (((parameters.mode == denovoengine) || (parameters.mode == databasesearch)) && (parameters.scannumber > parameters.peaklistseries.size())) {
		*os << "Number of spectra in the file: " << parameters.peaklistseries.size() << endl;
		*os << "Error: no peaklist found (scan no. " << parameters.scannumber << ")." << endl;
		emitEndSignals();
		return;
	}

	int startscanno = ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.mode == singlecomparison)) ? 0 : parameters.scannumber - 1;
	int endscanno = ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.mode == singlecomparison)) ? parameters.peaklistseries.size() : parameters.scannumber;
	for (int i = startscanno; i < endscanno; i++) {
		parameters.peaklistseries[i].sortbyMass();

		if (parameters.peaklistseries[i].normalizeIntenzity() == -1) {
			if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.mode == singlecomparison)) {
				*os << "Warning: the spectrum no. " << i + 1 << " is empty or the format of peaklist is incorrect." << endl;
			}
			else {
				*os << "Number of spectra in the file: " << parameters.peaklistseries.size() << endl;
				*os << "Error: the spectrum no. " << i + 1 << " is empty or the format of peaklist is incorrect." << endl;
				emitEndSignals();
				return;
			}
		}

		parameters.peaklistseries[i].cropMinimumMZRatio(parameters.minimummz, parameters.fragmentmasserrortolerance);

		if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
			parameters.peaklistseries[i].cropMaximumMZRatio(charge(uncharge(parameters.precursormass, parameters.precursorcharge), (parameters.precursorcharge > 0)?1:-1), parameters.precursormasserrortolerance);
		}

		if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
			if (parameters.maximummz > 0) {
				parameters.peaklistseries[i].cropMaximumMZRatio(parameters.maximummz, parameters.fragmentmasserrortolerance);
			}
		}

		parameters.peaklistseries[i].cropAbsoluteIntenzity(parameters.minimumabsoluteintensitythreshold);
		parameters.peaklistseries[i].cropRelativeIntenzity(parameters.minimumrelativeintensitythreshold);

		if ((parameters.mode == denovoengine) || (parameters.mode == databasesearch) || ((parameters.mode == singlecomparison) && (parameters.peaklistseries.size() == 1))) {
			*os << "Number of spectra in the file: " << parameters.peaklistseries.size() << endl;
			*os << "Processing spectrum no.: " << i + 1; 
			if (parameters.peaklistseries[i].getTitle().size() > 0) {
				*os << " (" << parameters.peaklistseries[i].getTitle() << "):";
			}
			*os << endl;
			*os << parameters.peaklistseries[i].print();
		}
	}

	if (parameters.prepareLossesAndCompounds(terminatecomputation) == -1) {
		emitEndSignals();
		return;
	}

	if (parameters.mode == compoundsearch) {
		int compoundslimit = 5000000;
		if (parameters.generateisotopepattern) {
			compoundslimit = 1000000;
		}
		if (parameters.sequencedatabase.size() > compoundslimit) {
			parameters.sequencedatabase.clear();
			*os << "Error: The number of generated compounds exceeded the limit " + to_string(compoundslimit) + ". Please, adjust the settings to limit the number of compounds." << endl;

			secs = time.elapsed() / 1000;
			mins = (secs / 60) % 60;
			hrs = (secs / 3600);
			secs = secs % 60;

			*os << "Execution time: " << to_string(hrs) << " hrs, " << to_string(mins) << " min, " << to_string(secs) << " sec." << endl << endl;

			emitEndSignals();
			return;
		}
	}

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		int startmodifid, endmodifid, middlemodifid;
		
		cSequence sequence;
		sequence.setNTterminalModification(parameters.searchedsequenceNtermmodif);
		sequence.setCTterminalModification(parameters.searchedsequenceCtermmodif);
		sequence.setBranchModification(parameters.searchedsequenceTmodif);
		sequence.setPeptideType(parameters.peptidetype);

		// check if the names of modifications used in the searched sequence are correct
		errormessage = "";
		if (!parameters.checkModifications(sequence, startmodifid, endmodifid, middlemodifid, errormessage)) {
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

			cSummaryFormula formula;
			string formstr;
			if (parameters.peptidetype == other) {
				v.push_back(to_string(1));
				c.setCandidate(v, netmp, fragmentIonTypeEnd, 0, 0, 0, -1, -1);

				formula.setFormula(parameters.searchedsequenceformula);
				formstr = parameters.precursoradduct.empty() ? "" : "H-1";
				formula.addFormula(formstr);
				formstr = parameters.precursoradduct;
				formula.addFormula(formstr);
			}
			else {
				parseBranch(parameters.peptidetype, parameters.searchedsequence, v, branchstart, branchend);
				// startmodifid, endmodifid and middlemodifid were filled up by checkModifications
				c.setCandidate(v, netmp, fragmentIonTypeEnd, startmodifid, endmodifid, middlemodifid, branchstart, branchend);
				formula = c.calculateSummaryFormulaFromBricks(parameters, parameters.peptidetype, parameters.precursormass);
			}

			c.setSummaryFormula(formula);
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

		// incomplete paths are removed
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

		// incomplete paths are connected
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
		string composition, formstr;
		vector<string> v;
		cCandidate c;
		cSummaryFormula formula;
		vector<nodeEdge> netmp;
		int startmodifid, endmodifid, middlemodifid, branchstart, branchend;

		bool calculatesummaries = false;

		for (int i = 0; i < parameters.sequencedatabase.size(); i++) {

			v.clear();
			c.clear();
			netmp.clear();
			errormessage = "";

			// check peptide type
			if (!calculatesummaries) {
				if (parameters.peptidetype != other) {
					if (parameters.peptidetype != parameters.sequencedatabase[i].getPeptideType()) {
						continue;
					}
				}
			}

			if (parameters.peptidetype != other) {

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
				if (!parameters.checkModifications(parameters.sequencedatabase[i], startmodifid, endmodifid, middlemodifid, errormessage)) {
					*os << "Ignored sequence: " << errormessage << endl;
					continue;
				}

				// parse branch of a branched or a branch-cyclic peptide
				parseBranch(parameters.sequencedatabase[i].getPeptideType(), composition, v, branchstart, branchend);

				// set candidate
				c.setCandidate(v, netmp, fragmentIonTypeEnd, startmodifid, endmodifid, middlemodifid, branchstart, branchend);

				if (!calculatesummaries && ((parameters.sequencedatabase[i].getPeptideType() == linearpolyketide) || (parameters.sequencedatabase[i].getPeptideType() == cyclicpolyketide))) {

					if (!c.checkKetideSequence(parameters.bricksdatabase, parameters.sequencedatabase[i].getPeptideType(), parameters.regularblocksorder)) {
						if (parameters.sequencedatabase[i].getPeptideType() == linearpolyketide) {
							*os << "Ignored sequence: " << parameters.sequencedatabase[i].getName() << " " << parameters.sequencedatabase[i].getSequence() << "; the order of building blocks is not correct." << endl;
						}
						else {
							*os << "Ignored sequence: " << parameters.sequencedatabase[i].getName() << " " << parameters.sequencedatabase[i].getSequence() << "; the number or order of building blocks is not correct." << endl;
						}
						continue;
					}

					eResidueLossType leftresiduelosstype = c.getLeftResidueType(parameters.bricksdatabase);
					eResidueLossType rightresiduelosstype = c.getRightResidueType(parameters.bricksdatabase);

					if (parameters.peptidetype == linearpolyketide) {
						if (((leftresiduelosstype == h2_loss) && (c.getStartModifID() > 0) && parameters.searchedmodifications[c.getStartModifID()].cterminal)
							|| ((rightresiduelosstype == h2_loss) && (c.getEndModifID() > 0) && parameters.searchedmodifications[c.getEndModifID()].cterminal)) {
							*os << "Ignored sequence: " << parameters.sequencedatabase[i].getName() << " " << parameters.sequencedatabase[i].getSequence() << "; the C-terminal modification is attached to the N-terminus." << endl;
							continue;
						}
					}
					else {
						if (((leftresiduelosstype == h2_loss) && (c.getStartModifID() > 0) && parameters.searchedmodifications[c.getStartModifID()].cterminal)
							|| ((leftresiduelosstype == h2o_loss) && (c.getStartModifID() > 0) && parameters.searchedmodifications[c.getStartModifID()].nterminal)
							|| ((rightresiduelosstype == h2_loss) && (c.getEndModifID() > 0) && parameters.searchedmodifications[c.getEndModifID()].cterminal)
							|| ((rightresiduelosstype == h2o_loss) && (c.getEndModifID() > 0) && parameters.searchedmodifications[c.getEndModifID()].nterminal)) {
							*os << "Ignored sequence: " << parameters.sequencedatabase[i].getName() << " " << parameters.sequencedatabase[i].getSequence() << "; the N-terminal modification is attached to the C-terminus or vice versa." << endl;
							continue;
						}
					}

				}

			}
			else {
				v.push_back(to_string(i + 1));
				c.setCandidate(v, netmp, fragmentIonTypeEnd, 0, 0, 0, -1, -1);
			}

			// check the precursor mass error
			formula.setFormula(parameters.sequencedatabase[i].getSummaryFormula());
			formstr = "H+";
			formula.addFormula(formstr);
			formstr = (parameters.precursorcharge > 0)?"":"H-2+-2";
			formula.addFormula(formstr);
			formstr = parameters.precursoradduct.empty()?"":"H-1";
			formula.addFormula(formstr);
			formstr = parameters.precursoradduct;
			formula.addFormula(formstr);
			if (!calculatesummaries && !parameters.similaritysearch && !isInPpmMassErrorTolerance(charge(uncharge(parameters.precursormass, parameters.precursorcharge), (parameters.precursorcharge > 0)?1:-1), formula.getMass(), parameters.precursormasserrortolerance)) {
				continue;
			}

			c.setName(parameters.sequencedatabase[i].getName());
			formstr = (parameters.precursorcharge > 0)?"H-1+-1":"H+";
			formula.addFormula(formstr);
			c.setSummaryFormula(formula);
			candidates.getSet().insert(c);

			if (calculatesummaries && (parameters.peptidetype != other)) {
				*os << i + 1 << " " << c.calculateSummaryFormulaFromBricks(parameters, parameters.sequencedatabase[i].getPeptideType()).getSummary() << endl;
			}
		}
		
		if (calculatesummaries) {
			*os << endl << "done." << endl;
			emitEndSignals();
			return;
		}

		*os << "Number of candidates found: " << candidates.size() << endl;

		graphreaderisworking = false;

	}


	// database search - MS mode
	if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
		*os << "Generating theoretical peaks... ";

		theoreticalspectrumlist->initialize(*os, parameters, &graph);

		cTheoreticalSpectrum ts;
		ts.setParameters(&parameters);

		if (parameters.generateisotopepattern) {
			ts.generateFineMSSpectrum(terminatecomputation);
		}
		else {
			ts.generateMSSpectrum(terminatecomputation, true);
		}

		if (terminatecomputation) {
			emitEndSignals();
			return;
		}

		unordered_map<string, int> tempmap;
		parameters.peakidtodesc.clear();
		parameters.isotopeformulaidtodesc.clear();

		tempmap.clear();
		ts.getTheoreticalPeaks()->reducePeakDescriptions(tempmap);
		convertStringIntUnorderedMapToStringVector(tempmap, parameters.peakidtodesc);

		//tempmap.clear();
		//ts.getTheoreticalPeaks()->reduceIsotopeFormulaDescriptions(tempmap);
		//convertStringIntUnorderedMapToStringVector(tempmap, parameters.isotopeformulaidtodesc);

		*os << "ok" << endl;
		*os << "Comparing theoretical peaks with the experimental peaklist(s)... " << endl;
		*os << "Number of experimental peaklists: " << parameters.peaklistseries.size() << endl;

		vector<cPeaksList> unmatchedpeaks;
		unmatchedpeaks.resize(parameters.peaklistseries.size());

		for (int i = 0; i < parameters.peaklistseries.size(); i++) {
			if (terminatecomputation) {
				emitEndSignals();
				return;
			}

			parameters.peaklistseries[i].sortbyMass();
		}

		vector< vector<int> > hintsindex;
		hintsindex.resize(ts.getNumberOfPeaks());

		bool lcms = (parameters.peaklistseries.size() > 1) && !((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML));

		if (lcms || (parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			*os << "Analyzing spectra : " << endl;

			for (int i = 0; i < parameters.peaklistseries.size(); i++) {
				if ((i + 1) % 100 == 0) {
					*os << i + 1 << " ";
				}
				if ((i + 1) % 2500 == 0) {
					*os << endl;
				}

				if (terminatecomputation) {
					emitEndSignals();
					return;
				}

				cTheoreticalSpectrum tstmp;
				tstmp.setParameters(&parameters);
				tstmp.getHintsIndex(i, ts, unmatchedpeaks[i], hintsindex);

				for (auto& it : hintsindex) {
					sort(it.begin(), it.end());
				}
			}

			*os << " ok" << endl;
		}

		*os << "Processing the peaklist no. : " << endl;

		for (int i = 0; i < parameters.peaklistseries.size(); i++) {
			if ((i + 1) % 100 == 0) {
				*os << i + 1 << " ";
			}
			if ((i + 1) % 2500 == 0) {
				*os << endl;
			}

			if (terminatecomputation) {
				emitEndSignals();
				return;
			}

			cTheoreticalSpectrum tstmp;
			tstmp.setParameters(&parameters);
			tstmp.compareMSSpectrum(i, ts, unmatchedpeaks[i], hintsindex);
			if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
				parameters.peaklistseries[i].clear();
			}
			theoreticalspectrumlist->add(tstmp);
		}

		if (!((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML))) {
			for (int i = 0; i < parameters.peaklistseries.size(); i++) {
				if (terminatecomputation) {
					emitEndSignals();
					return;
				}

				parameters.peaklistseries[i].clear();
			}
		}

		*os << " ok" << endl << "Total number of spectra: " << parameters.peaklistseries.size() << endl;

		if (parameters.generateisotopepattern)	{

			*os << endl << "Calculating FDRs... ";

			vector<double> targetscoresvector;
			vector<double> decoyscoresvector;
			vector<double> allscoresvector;
			vector<double> fdrs;
			map<int, double> *targetscores;
			map<int, double> *decoyscores;
			map<int, int> numbersofmatchedenvelopes;
			double minfdr;
			int targetscoresvectorsize;

			// calculate numbers of matched envelopes in single-pixel spectra
			for (int i = 0; i < (int)theoreticalspectrumlist->size(); i++) {
				targetscores = &((*theoreticalspectrumlist)[i].getTargetScores());
				decoyscores = &((*theoreticalspectrumlist)[i].getDecoyScores());

				for (auto& it : *targetscores) {
					if (numbersofmatchedenvelopes.count(it.first) == 0) {
						numbersofmatchedenvelopes.insert(make_pair(it.first, 1));
					}
					else {
						numbersofmatchedenvelopes[it.first]++;
					}
				}

				for (auto& it : *decoyscores) {
					if (numbersofmatchedenvelopes.count(it.first) == 0) {
						numbersofmatchedenvelopes.insert(make_pair(it.first, 1));
					}
					else {
						numbersofmatchedenvelopes[it.first]++;
					}
				}
			}

			// divide scores by numbers of matched envelopes in all single-pixel spectra
			for (int i = 0; i < (int)theoreticalspectrumlist->size(); i++) {
				targetscores = &((*theoreticalspectrumlist)[i].getTargetScores());
				decoyscores = &((*theoreticalspectrumlist)[i].getDecoyScores());

				for (auto& it : *targetscores) {
					//it.second /= sqrt((double)numbersofmatchedenvelopes[it.first]);
					targetscoresvector.push_back(it.second);
				}

				for (auto& it : *decoyscores) {
					//it.second /= sqrt((double)numbersofmatchedenvelopes[it.first]);
					decoyscoresvector.push_back(it.second);
				}
			}

			// calculate FDRs
			sort(targetscoresvector.begin(), targetscoresvector.end());
			sort(decoyscoresvector.begin(), decoyscoresvector.end());

			allscoresvector = targetscoresvector;
			allscoresvector.insert(allscoresvector.end(), decoyscoresvector.begin(), decoyscoresvector.end());
			sort(allscoresvector.begin(), allscoresvector.end());

			targetscoresvectorsize = (int)targetscoresvector.size();
			fdrs.resize(targetscoresvectorsize);
			for (int i = 0; i < targetscoresvectorsize; i++) {
				fdrs[i] = (double)getNumberOfScoreHits(decoyscoresvector, targetscoresvector[i]) / (double)max(getNumberOfScoreHits(allscoresvector, targetscoresvector[i]), 1);
			}

			// calculate q-values
			/*for (int i = targetscoresvectorsize - 1; i >= 0; i--) {
				if ((i == targetscoresvectorsize - 1) || (fdrs[i] < minfdr)) {
					minfdr = fdrs[i];
				}
				if (fdrs[i] > minfdr) {
					fdrs[i] = minfdr;
				}
			}*/

			for (int i = 0; i < (int)theoreticalspectrumlist->size(); i++) {
				(*theoreticalspectrumlist)[i].setFDRs(targetscoresvector, fdrs, unmatchedpeaks[i]);
			}

			*os << "ok" << endl;

		}

		for (int i = 0; i < (int)theoreticalspectrumlist->size(); i++) {
			(*theoreticalspectrumlist)[i].finalizeMSSpectrum(unmatchedpeaks[i], true);
		}

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

	secs = time.elapsed() / 1000;
	mins = (secs / 60) % 60;
	hrs =  (secs / 3600);
	secs = secs % 60;

	*os << endl << appname.toStdString() << " successfully finished at " << time.currentTime().toString().toStdString();
	*os << " (time elapsed: " << to_string(hrs) << " hrs, " << to_string(mins) << " min, " << to_string(secs) << " sec)." << endl;

	*os << "====================================================================================================" << endl;

	emitEndSignals();

}


void cMainThread::emitEndSignals() {
	emit setGraph(graph.printGraph());

	if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.neutrallossesfortheoreticalspectra.size() > 10000)) {
		parameters.neutrallossesdefinitions.clear();
		parameters.neutrallossesfortheoreticalspectra.clear();
	}

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

