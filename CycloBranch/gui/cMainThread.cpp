#include "gui/cMainThread.h"


cIsotopePatternCache isotopepatterncache;


string cMainThread::getCurrentTime(QTime& time) {
	int secs = time.elapsed() / 1000;
	int mins = (secs / 60) % 60;
	int hrs = (secs / 3600);
	secs = secs % 60;

	return to_string(hrs) + " hrs, " + to_string(mins) + " min, " + to_string(secs) + " sec";
}


void cMainThread::singleThreadMS1(cMainThread* os, QTime& time, cTheoreticalSpectrum* testspectrum, vector< vector< vector<int> > >* testhintsindexvector, cTheoreticalSpectrumList* testlistoftheoreticalspectra, vector< vector<cPeaksList> >* testunmatchedpeaksvector) {
	if (testspectrum) {
		*os << "===== running test - singleThreadMS1 =====" << endl;
	}

	*os << "Time elapsed: " << getCurrentTime(time) << "." << endl << endl;

	*os << "Generating theoretical peaks... ";

	listoftheoreticalspectra->clear();
	listoftheoreticalspectra->resize((int)parameters.peaklistseriesvector.size());

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		listoftheoreticalspectra->clear(h);
	}

	if (testspectrum && testunmatchedpeaksvector && testlistoftheoreticalspectra) {
		testlistoftheoreticalspectra->clear();
		testlistoftheoreticalspectra->resize((int)parameters.peaklistseriesvector.size());

		for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
			testlistoftheoreticalspectra->clear(h);
		}

		testunmatchedpeaksvector->resize((int)parameters.peaklistseriesvector.size());
	}

	cTheoreticalSpectrum ts;
	ts.setParameters(&parameters);

	if (parameters.generateisotopepattern) {
		if (parameters.calculatefdrs) {
			parameters.sequencedatabase.attachDecoys(terminatecomputation);
		}

		if (terminatecomputation) {
			emitEndSignals();
			return;
		}

		ts.generateFineMSSpectrum(0, (int)parameters.sequencedatabase.size(), terminatecomputation);
	}
	else {
		ts.generateMSSpectrum(0, (int)parameters.sequencedatabase.size(), terminatecomputation, true);
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
	*os << "Number of input files: " << parameters.peaklistseriesvector.size() << endl;

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		*os << "Number of experimental peaklists in the file no. " << h + 1 << " : " << parameters.peaklistseriesvector[h].size() << endl;
		for (int i = 0; i < parameters.peaklistseriesvector[h].size(); i++) {
			if (terminatecomputation) {
				emitEndSignals();
				return;
			}
			parameters.peaklistseriesvector[h][i].sortbyMass();
		}
	}

	*os << endl << "Time elapsed: " << getCurrentTime(time) << "." << endl << endl;

	bool lcms;
	bool rtavailable;
	bool skipcomparison;

	vector< vector< vector<int> > > hintsindexvector;
	hintsindexvector.resize(parameters.peaklistseriesvector.size());

	vector< vector<cPeaksList> > unmatchedpeaksvector;
	unmatchedpeaksvector.resize(parameters.peaklistseriesvector.size());

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {

		unmatchedpeaksvector[h].resize(parameters.peaklistseriesvector[h].size());

		lcms = (parameters.peaklistseriesvector[h].size() > 1) && !((parameters.peaklistfileformats[h] == mis) || (parameters.peaklistfileformats[h] == imzML));
		rtavailable = (parameters.peaklistseriesvector[h].size() > 1) && (parameters.peaklistseriesvector[h][parameters.peaklistseriesvector[h].size() - 1].getRetentionTime() > 0);

		if (lcms || (parameters.peaklistfileformats[h] == mis) || (parameters.peaklistfileformats[h] == imzML)) {
			*os << "Analyzing spectra in the file no. " << h + 1 << " : " << endl;

			hintsindexvector[h].resize(ts.getNumberOfPeaks());

			for (int i = 0; i < parameters.peaklistseriesvector[h].size(); i++) {
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

				skipcomparison = false;
				if (lcms && rtavailable) {
					if (parameters.minimumrt != 0) {
						if (parameters.peaklistseriesvector[h][i].getRetentionTime() < parameters.minimumrt) {
							skipcomparison = true;
						}
					}
					if (parameters.maximumrt != 0) {
						if (parameters.peaklistseriesvector[h][i].getRetentionTime() > parameters.maximumrt) {
							skipcomparison = true;
						}
					}
				}

				cTheoreticalSpectrum tstmp;
				tstmp.setParameters(&parameters);
				tstmp.getHintsIndex(i, h, ts, unmatchedpeaksvector[h][i], hintsindexvector[h], lcms && rtavailable, skipcomparison);
			}

			for (auto& it : hintsindexvector[h]) {
				sort(it.begin(), it.end());
			}

			*os << " ok" << endl;
		}

		*os << endl << "Time elapsed: " << getCurrentTime(time) << "." << endl << endl;

		*os << "Processing the file no. " << h + 1 << " peaklist no. : " << endl;

		for (int i = 0; i < parameters.peaklistseriesvector[h].size(); i++) {
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

			skipcomparison = false;
			if (lcms && rtavailable) {
				if (parameters.minimumrt != 0) {
					if (parameters.peaklistseriesvector[h][i].getRetentionTime() < parameters.minimumrt) {
						skipcomparison = true;
					}
				}
				if (parameters.maximumrt != 0) {
					if (parameters.peaklistseriesvector[h][i].getRetentionTime() > parameters.maximumrt) {
						skipcomparison = true;
					}
				}
			}

			tstmp.compareMSSpectrum(i, h, ts, unmatchedpeaksvector[h][i], hintsindexvector[h], lcms && rtavailable, skipcomparison);
			if ((parameters.peaklistfileformats[h] == mis) || (parameters.peaklistfileformats[h] == imzML)) {
				if (!testspectrum) {
					parameters.peaklistseriesvector[h][i].clear();
				}
			}

			listoftheoreticalspectra->add(h, tstmp);
		}

		if (testspectrum && testunmatchedpeaksvector && testlistoftheoreticalspectra) {
			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				testlistoftheoreticalspectra->add(h, listoftheoreticalspectra->get(h, i));
			}
			(*testunmatchedpeaksvector)[h] = unmatchedpeaksvector[h];
		}

		if (!((parameters.peaklistfileformats[h] == mis) || (parameters.peaklistfileformats[h] == imzML))) {
			for (int i = 0; i < parameters.peaklistseriesvector[h].size(); i++) {
				if (terminatecomputation) {
					emitEndSignals();
					return;
				}

				if (!testspectrum) {
					parameters.peaklistseriesvector[h][i].clear();
				}
			}
		}

		*os << " ok" << endl << "Total number of spectra in the file no. " << h + 1 << " : " << parameters.peaklistseriesvector[h].size() << endl;

		if (parameters.calculatefdrs && parameters.generateisotopepattern) {

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
			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				targetscores = &(listoftheoreticalspectra->get(h, i).getTargetScores());
				decoyscores = &(listoftheoreticalspectra->get(h, i).getDecoyScores());

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
			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				targetscores = &(listoftheoreticalspectra->get(h, i).getTargetScores());
				decoyscores = &(listoftheoreticalspectra->get(h, i).getDecoyScores());

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

			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				listoftheoreticalspectra->get(h, i).setFDRs(targetscoresvector, fdrs, unmatchedpeaksvector[h][i]);
			}

			*os << "ok" << endl;

		}

		for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
			listoftheoreticalspectra->get(h, i).finalizeMSSpectrum(unmatchedpeaksvector[h][i], true);
		}

	}

	if (testspectrum && testhintsindexvector && testlistoftheoreticalspectra && testunmatchedpeaksvector) {
		*testspectrum = ts;
		*testhintsindexvector = hintsindexvector;

		//for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		//	for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
		//		testlistoftheoreticalspectra->add(h, listoftheoreticalspectra->get(h, i));
		//	}
		//}

		//*testunmatchedpeaksvector = unmatchedpeaksvector;

		*os << "===== test ended =====" << endl << endl;
	}

}


void cMainThread::multiThreadMS1(cMainThread* os, QTime& time, cTheoreticalSpectrum* testspectrum, vector< vector< vector<int> > >* testhintsindexvector, cTheoreticalSpectrumList* testlistoftheoreticalspectra, vector< vector<cPeaksList> >* testunmatchedpeaksvector) {
	if (testspectrum) {
		*os << "===== running test - multiThreadMS1 =====" << endl;
	}

	*os << "Time elapsed: " << getCurrentTime(time) << "." << endl << endl;

	*os << "Generating theoretical peaks... ";

	listoftheoreticalspectra->clear();
	listoftheoreticalspectra->resize((int)parameters.peaklistseriesvector.size());

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		listoftheoreticalspectra->clear(h);
	}

	if (testspectrum && testunmatchedpeaksvector && testlistoftheoreticalspectra) {
		testlistoftheoreticalspectra->clear();
		testlistoftheoreticalspectra->resize((int)parameters.peaklistseriesvector.size());

		for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
			testlistoftheoreticalspectra->clear(h);
		}

		testunmatchedpeaksvector->resize((int)parameters.peaklistseriesvector.size());
	}

	if (parameters.generateisotopepattern && parameters.calculatefdrs) {
		parameters.sequencedatabase.attachDecoys(terminatecomputation);
	}

	vector<cTheoreticalSpectrum*> spectrumpool;

	QThreadPool::globalInstance()->setMaxThreadCount(parameters.maximumnumberofthreads);

	int sequencestart = 0;
	int sequencestop = max(100, parameters.sequencedatabase.size() / (2 * parameters.maximumnumberofthreads));

	int poolreserve = parameters.sequencedatabase.size() / sequencestop;
	if (parameters.sequencedatabase.size() % sequencestop > 0) {
		poolreserve++;
	}

	spectrumpool.reserve(poolreserve);

	initParallelOutputState(poolreserve);

	while (sequencestart < parameters.sequencedatabase.size()) {
		if (terminatecomputation) {
			break;
		}

		cSpectrumGeneratorThreadMS1* generatorthread = new cSpectrumGeneratorThreadMS1();
		cTheoreticalSpectrum* poolitem = new cTheoreticalSpectrum();

		generatorthread->initialize(this, &parameters, sequencestart, min(sequencestart + sequencestop, (int)parameters.sequencedatabase.size()), poolitem, &terminatecomputation);
		spectrumpool.push_back(poolitem);

		QThreadPool::globalInstance()->start(generatorthread);

		sequencestart += sequencestop;
	}

	QThreadPool::globalInstance()->waitForDone();

	if (terminatecomputation) {
		for (auto& it : spectrumpool) {
			delete it;
		}
		emitEndSignals();
		return;
	}

	unordered_map<string, int> tempmap;
	parameters.peakidtodesc.clear();
	parameters.isotopeformulaidtodesc.clear();

	tempmap.clear();

	size_t spectrumpoolsize = spectrumpool.size();
	int numberoftheoreticalpeaks = 0;
	int lastgroupid = 0;
	int thpeaksize;

	for (size_t i = 0; i < spectrumpoolsize; i++) {
		thpeaksize = spectrumpool[i]->getTheoreticalPeaks()->size();
		
		spectrumpool[i]->getTheoreticalPeaks()->reducePeakDescriptions(tempmap);
		numberoftheoreticalpeaks += thpeaksize;

		for (int j = 0; j < thpeaksize; j++) {
			(*spectrumpool[i]->getTheoreticalPeaks())[j].groupid += lastgroupid;
		}

		if (thpeaksize > 0) {
			lastgroupid = (*spectrumpool[i]->getTheoreticalPeaks())[thpeaksize - 1].groupid + 1;
		}
	}
	convertStringIntUnorderedMapToStringVector(tempmap, parameters.peakidtodesc);

	//tempmap.clear();

	//for (size_t i = 0; i < spectrumpoolsize; i++) {
	//	spectrumpool[i]->getTheoreticalPeaks()->reduceIsotopeFormulaDescriptions(tempmap);
	//}
	//convertStringIntUnorderedMapToStringVector(tempmap, parameters.isotopeformulaidtodesc);

	*os << "ok" << endl;
	*os << "Comparing theoretical peaks with the experimental peaklist(s)... " << endl;
	*os << "Number of input files: " << parameters.peaklistseriesvector.size() << endl;

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		*os << "Number of experimental peaklists in the file no. " << h + 1 << " : " << parameters.peaklistseriesvector[h].size() << endl;
		for (int i = 0; i < parameters.peaklistseriesvector[h].size(); i++) {
			if (terminatecomputation) {
				for (auto& it : spectrumpool) {
					delete it;
				}
				emitEndSignals();
				return;
			}

			parameters.peaklistseriesvector[h][i].sortbyMass();
		}
	}

	*os << endl << "Time elapsed: " << getCurrentTime(time) << "." << endl << endl;

	bool lcms;

	vector< vector< vector< vector<int> > > > hintsindexpoolvector;
	hintsindexpoolvector.resize(parameters.peaklistseriesvector.size());

	vector< vector< vector<int> > > hintsindexvector;
	hintsindexvector.resize(parameters.peaklistseriesvector.size());

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {

		hintsindexpoolvector[h].resize(spectrumpoolsize);

		lcms = (parameters.peaklistseriesvector[h].size() > 1) && !((parameters.peaklistfileformats[h] == mis) || (parameters.peaklistfileformats[h] == imzML));

		if (lcms || (parameters.peaklistfileformats[h] == mis) || (parameters.peaklistfileformats[h] == imzML)) {
			*os << "Analyzing spectra in the file no. " << h + 1 << " ... ";

			if (testspectrum && testhintsindexvector) {
				hintsindexvector[h].resize(numberoftheoreticalpeaks);
			}

			initParallelOutputState((int)spectrumpoolsize);

			for (size_t i = 0; i < spectrumpoolsize; i++) {
				if (terminatecomputation) {
					break;
				}

				cSpectrumAnalyzerThreadMS1* analyzerthread = new cSpectrumAnalyzerThreadMS1();
				analyzerthread->initialize(this, &parameters, h, spectrumpool[i], &hintsindexpoolvector[h][i], &terminatecomputation);

				QThreadPool::globalInstance()->start(analyzerthread);
			}

			QThreadPool::globalInstance()->waitForDone();

			if (testspectrum && testhintsindexvector) {
				serializeHintsIndexPool(hintsindexpoolvector[h], hintsindexvector[h]);
			}

			*os << "ok" << endl;
		}

	}

	*os << endl << "Time elapsed: " << getCurrentTime(time) << "." << endl << endl;

	if (terminatecomputation) {
		for (auto& it : spectrumpool) {
			delete it;
		}
		emitEndSignals();
		return;
	}

	*os << "Processing the peaklist(s)... ";

	vector< vector<cPeaksList> > unmatchedpeaksvector;
	unmatchedpeaksvector.resize(parameters.peaklistseriesvector.size());

	vector<cTheoreticalSpectrumList*> theoreticalspectrumlistpool;
	vector< vector<cPeaksList> > unmatchedpeakspool;

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		unmatchedpeaksvector[h].resize(parameters.peaklistseriesvector[h].size());

		theoreticalspectrumlistpool.resize(spectrumpoolsize);
		unmatchedpeakspool.resize(spectrumpoolsize);

		initParallelOutputState((int)spectrumpoolsize);

		for (size_t i = 0; i < spectrumpoolsize; i++) {
			if (terminatecomputation) {
				break;
			}

			theoreticalspectrumlistpool[i] = new cTheoreticalSpectrumList();

			theoreticalspectrumlistpool[i]->clear();
			theoreticalspectrumlistpool[i]->resize((int)parameters.peaklistseriesvector.size());

			for (int j = 0; j < (int)parameters.peaklistseriesvector.size(); j++) {
				theoreticalspectrumlistpool[i]->clear(j);
			}

			cSpectrumComparatorThreadMS1* comparatorthread = new cSpectrumComparatorThreadMS1();
			comparatorthread->initialize(this, &parameters, h, spectrumpool[i], &hintsindexpoolvector[h][i], theoreticalspectrumlistpool[i], &unmatchedpeakspool[i], &terminatecomputation);

			QThreadPool::globalInstance()->start(comparatorthread);
		}

		QThreadPool::globalInstance()->waitForDone();

		serializeTheoreticalSpectrumListPool(h, theoreticalspectrumlistpool, *listoftheoreticalspectra);

		for (auto& it : theoreticalspectrumlistpool) {
			delete it;
		}

		serializeUnmatchedPeaksPool(h, unmatchedpeakspool, unmatchedpeaksvector);

		unmatchedpeakspool.clear();

		if (testspectrum && testunmatchedpeaksvector && testlistoftheoreticalspectra) {
			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				testlistoftheoreticalspectra->add(h, listoftheoreticalspectra->get(h, i));
			}
			(*testunmatchedpeaksvector)[h] = unmatchedpeaksvector[h];
		}

		for (int i = 0; i < parameters.peaklistseriesvector[h].size(); i++) {
			if (terminatecomputation) {
				for (auto& it : spectrumpool) {
					delete it;
				}
				emitEndSignals();
				return;
			}

			if (!testspectrum) {
				parameters.peaklistseriesvector[h][i].clear();
			}
		}

		*os << " ok" << endl << "Total number of spectra in the file no. " << h + 1 << " : " << parameters.peaklistseriesvector[h].size() << endl;

		if (parameters.calculatefdrs && parameters.generateisotopepattern) {

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
			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				targetscores = &(listoftheoreticalspectra->get(h, i).getTargetScores());
				decoyscores = &(listoftheoreticalspectra->get(h, i).getDecoyScores());

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
			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				targetscores = &(listoftheoreticalspectra->get(h, i).getTargetScores());
				decoyscores = &(listoftheoreticalspectra->get(h, i).getDecoyScores());

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

			for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
				listoftheoreticalspectra->get(h, i).setFDRs(targetscoresvector, fdrs, unmatchedpeaksvector[h][i]);
			}

			*os << "ok" << endl;

		}

		for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
			listoftheoreticalspectra->get(h, i).finalizeMSSpectrum(unmatchedpeaksvector[h][i], true);
		}

	}

	cTheoreticalSpectrum ts;

	if (testspectrum) {
		ts.setParameters(&parameters);
		serializeSpectrumPool(spectrumpool, ts);
	}

	for (auto& it : spectrumpool) {
		delete it;
	}

	if (testspectrum && testhintsindexvector && testlistoftheoreticalspectra && testunmatchedpeaksvector) {
		*testspectrum = ts;
		*testhintsindexvector = hintsindexvector;

		//for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		//	for (int i = 0; i < listoftheoreticalspectra->size(h); i++) {
		//		testlistoftheoreticalspectra->add(h, listoftheoreticalspectra->get(h, i));
		//	}
		//}

		//*testunmatchedpeaksvector = unmatchedpeaksvector;

		*os << "===== test ended =====" << endl << endl;
	}
}


void cMainThread::initParallelOutputState(int size) {
	paralleloutputstate = 0;
	paralleloutputsize = size;
}


void cMainThread::serializeSpectrumPool(vector<cTheoreticalSpectrum*>& spectrumpool, cTheoreticalSpectrum& ts) {
	size_t poolsize = spectrumpool.size();
	for (size_t i = 0; i < poolsize; i++) {
		ts.getTheoreticalPeaks()->attach(*spectrumpool[i]->getTheoreticalPeaks());
	}

	if (parameters.generateisotopepattern) {
		int lastgroupid = 0;
		int currentgroupid = 0;

		for (int i = 0; i < ts.getTheoreticalPeaks()->size(); i++) {	
			if (lastgroupid != (*ts.getTheoreticalPeaks())[i].groupid) {
				currentgroupid++;
			}

			lastgroupid = (*ts.getTheoreticalPeaks())[i].groupid;
			(*ts.getTheoreticalPeaks())[i].groupid = currentgroupid;
		}
	}
}


void cMainThread::serializeHintsIndexPool(vector< vector< vector<int> > >& hintsindexpool, vector< vector<int> >& hintsindex) {
	hintsindex.clear();
	size_t hintsindexpoolsize = hintsindexpool.size();
	for (size_t i = 0; i < hintsindexpoolsize; i++) {
		hintsindex.reserve(hintsindex.size() + hintsindexpool[i].size());
		hintsindex.insert(hintsindex.end(), hintsindexpool[i].begin(), hintsindexpool[i].end());
	}
}


void cMainThread::serializeUnmatchedPeaksPool(int peaklistseriesvectorid, vector< vector<cPeaksList> >& unmatchedpeakspool, vector< vector<cPeaksList> >& unmatchedpeaks) {
	size_t unmatchedpeakspoolsize = unmatchedpeakspool.size();
	for (size_t i = 0; i < unmatchedpeakspoolsize; i++) {
		if (unmatchedpeaks[peaklistseriesvectorid].size() != unmatchedpeakspool[i].size()) {
			unmatchedpeaks[peaklistseriesvectorid].clear();
			unmatchedpeaks[peaklistseriesvectorid].resize(parameters.peaklistseriesvector[peaklistseriesvectorid].size());
			return;
		}
		for (size_t j = 0; j < unmatchedpeakspool[i].size(); j++) {
			unmatchedpeaks[peaklistseriesvectorid][j].attach(unmatchedpeakspool[i][j]);
		}
	}
}


void cMainThread::serializeTheoreticalSpectrumListPool(int peaklistseriesvectorid, vector<cTheoreticalSpectrumList*>& theoreticalspectrumlistpool, cTheoreticalSpectrumList& theoreticalspectrumlist) {
	if (theoreticalspectrumlistpool.size() == 0) {
		return;
	}

	for (int i = 0; i < theoreticalspectrumlistpool[0]->size(peaklistseriesvectorid); i++) {
		theoreticalspectrumlist.add(peaklistseriesvectorid, theoreticalspectrumlistpool[0]->get(peaklistseriesvectorid, i));
	}

	for (int i = 1; i < theoreticalspectrumlistpool.size(); i++) {
		for (int j = 0; j < theoreticalspectrumlistpool[i]->size(peaklistseriesvectorid); j++) {
			theoreticalspectrumlist.get(peaklistseriesvectorid, j).attach(theoreticalspectrumlistpool[i]->get(peaklistseriesvectorid, j));
		}
	}
}


void cMainThread::printMatrix(cPeaksList& peaklist) {
	unsigned roundedmz;
	map<unsigned, unsigned> blocksmap;
	multimap<unsigned, unsigned> blocksmultimap;

	double precisionvalue = 1000.0;
	int size = (int)peaklist.size();

	string htmlfilename = parameters.peaklistfilenames[0];
	if (htmlfilename.rfind('/') != string::npos) {
		htmlfilename = htmlfilename.substr(0, htmlfilename.rfind('/')) + "_matrix.html";
	}

	ofstream html(htmlfilename);
	html << "<html><head></head><body><p><table border=\"1\" cellpadding=\"0\" cellspacing=\"0\">";

	html << "<tr><td></td>";
	for (int i = 0; i < size; i++) {
		html << "<td>" << peaklist[i].mzratio << "</td>";
	}
	html << "</tr>";

	for (int i = 0; i < size; i++) {
		html << "<tr>";

		html << "<td>" << peaklist[i].mzratio << "</td>";

		for (int j = 0; j < size; j++) {
			html << "<td>";

			if (i > j) {
				roundedmz = round(fabs(peaklist[i].mzratio - peaklist[j].mzratio) * precisionvalue);
				html << (double)roundedmz / precisionvalue;

				if (blocksmap.count(roundedmz) == 0) {
					blocksmap[roundedmz] = 1;
				}
				else {
					blocksmap[roundedmz]++;
				}
			}

			html << "</td>";
		}
		html << "</tr>";
	}

	html << "</table></p>";

	html << "<p><table border=\"1\" cellpadding=\"0\" cellspacing=\"0\">";

	for (auto& it : blocksmap) {
		blocksmultimap.insert(make_pair(it.second, it.first));
	}

	html << "<tr><td>count</td><td>m/z difference</td><td>block</td></tr>";

	cBricksDatabase blockdb;
	ifstream blocksstream(parameters.bricksdatabasefilename);
	string error;
	blockdb.loadFromPlainTextStream(blocksstream, error, true, true);

	for (auto it = blocksmultimap.rbegin(); it != blocksmultimap.rend(); ++it) {
		html << "<tr>";
		html << "<td>" << it->first << "</td>";
		html << "<td>" << (double)it->second / precisionvalue << "</td>";
		html << "<td>";
		
		for (int j = 0; j < blockdb.size(); j++) {
			if (round(blockdb[j].getMass() * precisionvalue) == it->second) {
				html << blockdb[j].getAcronymsAsString() << ", " << blockdb[j].getSummary() << "; ";
			}
		}

		html << "</td>";
		html << "</tr>";
	}

	html << "</table></p>";

	html << "</body>";
}


void cMainThread::printIonEstimations(cPeaksList& peaklist) {
	vector<string> testedions;
	//testedions.push_back("deamidated b-ion");
	testedions.push_back("dehydrated b-ion");
	testedions.push_back("a-ion");
	//testedions.push_back("deamidated a-ion");
	//testedions.push_back("dehydrated a-ion");

	vector<double> massshifts;
	//massshifts.push_back(17.026549);
	massshifts.push_back(18.01057);
	massshifts.push_back(27.99492);
	//massshifts.push_back(45.021464);
	//massshifts.push_back(46.005479);

	double adduct = 0;
	adduct = 21.981946;

	string htmlfilename = parameters.peaklistfilenames[0];
	if (htmlfilename.rfind('/') != string::npos) {
		htmlfilename = htmlfilename.substr(0, htmlfilename.rfind('/')) + "_estimations.html";
	}

	ofstream html(htmlfilename);
	html << "<html><head></head><body><p><table border=\"1\" cellpadding=\"0\" cellspacing=\"0\">";

	html << "<tr><td> </td>";
	for (int i = 0; i < (int)testedions.size(); i++) {
		html << "<td> " << testedions[i] << " (-" << massshifts[i] << ") </td>";
	}
	html << "</tr>";

	for (int i = 0; i < peaklist.size(); i++) {
		html << "<tr><td> ";
		html << peaklist[i].mzratio - adduct;
		html << " </td>";
		for (int j = 0; j < (int)massshifts.size(); j++) {
			html << "<td> ";
			if (searchHint(peaklist[i].mzratio - massshifts[j], peaklist, parameters.fragmentmasserrortolerance)) {
				html << peaklist[i].mzratio - massshifts[j] - adduct;
			}
			html << " </td>";
		}
		html << "</tr>";
	}

	html << "</table></p>";

	html << "</body>";
}


cMainThread::cMainThread(cParameters& parameters, cTheoreticalSpectrumList& listoftheoreticalspectra, bool enablelogwindow, bool enablestdout) {
	this->parameters = parameters;
	this->listoftheoreticalspectra = &listoftheoreticalspectra;
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


void cMainThread::addToParallelOutputState(int value) {
	QMutexLocker ml(&mutex);
	paralleloutputstate += value;

	if (paralleloutputsize > 0) {
		*this << endl << int((double)paralleloutputstate / (double)paralleloutputsize * 100.0) << "%";
		if (paralleloutputsize == paralleloutputstate) {
			*this << endl;
		}
	}
}


void cMainThread::run() {

	if (parameters.mode != denovoengine) {
		emit setGraph("");
	}

	cMainThread* os = this;
	string errormessage;

	int startscanno;
	int endscanno;
	
	cCandidateSet candidates;
	candidates.getSet().clear();

	listoftheoreticalspectra->clear();

	isotopepatterncache.lock();
	isotopepatterncache.clear();
	isotopepatterncache.unlock();

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

	if (parameters.peaklistseriesvector.size() == 0) {
		*os << "Error: no peaklist found. The format of a peaklist is likely incorrect." << endl;
		emitEndSignals();
		return;
	}

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		if (parameters.peaklistseriesvector.size() > 1) {
			*os << "Error: multiple files cannot be processed in MS/MS mode." << endl;
			emitEndSignals();
			return;
		}
	}

	for (auto& it : parameters.peaklistseriesvector) {
		if (it.size() == 0) {
			*os << "Error: no peaklist found. The format of a peaklist is likely incorrect." << endl;
			emitEndSignals();
			return;
		}
	}

	if (((parameters.mode == denovoengine) || (parameters.mode == databasesearch)) && (parameters.scannumber > parameters.peaklistseriesvector[0].size())) {
		*os << "Number of spectra in the file: " << parameters.peaklistseriesvector[0].size() << endl;
		*os << "Error: no peaklist found (scan no. " << parameters.scannumber << ")." << endl;
		emitEndSignals();
		return;
	}

	for (int h = 0; h < (int)parameters.peaklistseriesvector.size(); h++) {
		startscanno = ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.mode == singlecomparison)) ? 0 : parameters.scannumber - 1;
		endscanno = ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.mode == singlecomparison)) ? parameters.peaklistseriesvector[h].size() : parameters.scannumber;

		for (int i = startscanno; i < endscanno; i++) {
			parameters.peaklistseriesvector[h][i].sortbyMass();

			if (parameters.peaklistseriesvector[h][i].normalizeIntenzity() == -1) {
				if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch) || (parameters.mode == singlecomparison)) {
					*os << "Warning: the spectrum no. " << i + 1 << " in the file no. " << h + 1 << " is empty or the format of peaklist is incorrect." << endl;
				}
				else {
					*os << "Number of spectra in the file: " << parameters.peaklistseriesvector[h].size() << endl;
					*os << "Error: the spectrum no. " << i + 1 << " in the file no. " << h + 1 << " is empty or the format of peaklist is incorrect." << endl;
					emitEndSignals();
					return;
				}
			}

			parameters.peaklistseriesvector[h][i].cropMinimumMZRatio(parameters.minimummz, parameters.fragmentmasserrortolerance);

			if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
				parameters.peaklistseriesvector[h][i].cropMaximumMZRatio(charge(uncharge(parameters.precursormass, parameters.precursorcharge), (parameters.precursorcharge > 0) ? 1 : -1), parameters.precursormasserrortolerance);
			}

			if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
				if (parameters.maximummz > 0) {
					parameters.peaklistseriesvector[h][i].cropMaximumMZRatio(parameters.maximummz, parameters.fragmentmasserrortolerance);
				}
			}

			parameters.peaklistseriesvector[h][i].cropAbsoluteIntenzity(parameters.minimumabsoluteintensitythreshold);
			parameters.peaklistseriesvector[h][i].cropRelativeIntenzity(parameters.minimumrelativeintensitythreshold);

			if ((parameters.mode == denovoengine) || (parameters.mode == databasesearch) || ((parameters.mode == singlecomparison) && (parameters.peaklistseriesvector[h].size() == 1))) {
				*os << "Number of spectra in the file: " << parameters.peaklistseriesvector[h].size() << endl;
				*os << "Processing spectrum no.: " << i + 1;
				if (parameters.peaklistseriesvector[h][i].getTitle().size() > 0) {
					*os << " (" << parameters.peaklistseriesvector[h][i].getTitle() << "):";
				}
				*os << endl;
				*os << parameters.peaklistseriesvector[h][i].print();
			}
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
			*os << "Execution time: " << getCurrentTime(time) << "." << endl << endl;
			emitEndSignals();
			return;
		}
	}

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		//printMatrix(parameters.peaklistseries[parameters.scannumber - 1]);
		//printIonEstimations(parameters.peaklistseries[parameters.scannumber - 1]);

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
		bool runtest = false;

		if (!runtest) {
			if (parameters.maximumnumberofthreads > 1) {
				multiThreadMS1(os, time);
			}
			else {
				singleThreadMS1(os, time);
			}
		}
		else {
			cTheoreticalSpectrum* singletest = new cTheoreticalSpectrum();
			cTheoreticalSpectrum* multitest = new cTheoreticalSpectrum();

			vector<vector< vector<int> > > singlehintsindex;
			vector<vector< vector<int> > > multihintsindex;

			cTheoreticalSpectrumList* singlelistoftheoreticalspectra = new cTheoreticalSpectrumList();
			cTheoreticalSpectrumList* multilistoftheoreticalspectra = new cTheoreticalSpectrumList();

			vector<vector< cPeaksList > > singleunmatchedpeaks;
			vector<vector< cPeaksList > > multiunmatchedpeaks;

			cSequenceDatabase dbbackup = parameters.sequencedatabase;
			singleThreadMS1(os, time, singletest, &singlehintsindex, singlelistoftheoreticalspectra, &singleunmatchedpeaks);

			parameters.sequencedatabase = dbbackup;
			multiThreadMS1(os, time, multitest, &multihintsindex, multilistoftheoreticalspectra, &multiunmatchedpeaks);

			if (singletest->getTheoreticalPeaks()->equals(*multitest->getTheoreticalPeaks())) {
				*os << endl << "theoretical peaks test ok - singleThreadMS1 vs. multiThreadMS1" << endl;
			}
			else {
				*os << endl << "theoretical peakstest failed - singleThreadMS1 vs. multiThreadMS1" << endl;
			}

			bool hintsindextest = true;
			if (singlehintsindex.size() == multihintsindex.size()) {
				for (int i = 0; i < singlehintsindex.size(); i++) {
					if (singlehintsindex[i].size() == multihintsindex[i].size()) {
						for (int j = 0; j < singlehintsindex[i].size(); j++) {
							if (singlehintsindex[i][j].size() == multihintsindex[i][j].size()) {
								for (int k = 0; k < singlehintsindex[i][j].size(); k++) {
									if (singlehintsindex[i][j][k] != multihintsindex[i][j][k]) {
										*os << "hintsindex " << i << " " << j << " " << k << " failed" << endl;
										hintsindextest = false;
										break;
									}
								}
								if (!hintsindextest) {
									break;
								}
							}
							else {
								*os << "singlehintsindex[i][j].size() == multihintsindex[i][j].size()" << " failed";
								hintsindextest = false;
								break;
							}
						}
					}
					else {
						*os << "singlehintsindex[i].size() == multihintsindex[i].size()" << " failed";
						hintsindextest = false;
						break;
					}
				}
			}
			else {
				*os << "singlehintsindex.size() == multihintsindex.size()" << " failed";
				hintsindextest = false;
			}

			if (hintsindextest) {
				*os << endl << "hintsindex test ok - singleThreadMS1 vs. multiThreadMS1" << endl;
			}
			else {
				*os << endl << "hintsindex test failed - singleThreadMS1 vs. multiThreadMS1" << endl;
			}

			bool theoreticalspectrumlisttest = true;
			if (singlelistoftheoreticalspectra->size() == multilistoftheoreticalspectra->size()) {
				for (int i = 0; i < singlelistoftheoreticalspectra->size(); i++) {
					if (singlelistoftheoreticalspectra->size(i) == multilistoftheoreticalspectra->size(i)) {
						for (int j = 0; j < singlelistoftheoreticalspectra->size(i); j++) {
							if (!singlelistoftheoreticalspectra->get(i, j).equals(multilistoftheoreticalspectra->get(i, j))) {
								cout << "(theoreticalspectrumlist in " << i + 1 << " " << j + 1 << ")" << endl << endl;
								theoreticalspectrumlisttest = false;

								cout << singlelistoftheoreticalspectra->get(i, j).getTheoreticalPeaks()->print(false, true);
								cout << multilistoftheoreticalspectra->get(i, j).getTheoreticalPeaks()->print(false, true);

								break;
							}
						}
					}
					else {
						theoreticalspectrumlisttest = false;
						break;
					}
				}
			}
			else {
				theoreticalspectrumlisttest = false;
			}

			if (theoreticalspectrumlisttest) {
				*os << endl << "theoreticalspectrumlisttest test ok - singleThreadMS1 vs. multiThreadMS1" << endl;
			}
			else {
				*os << endl << "theoreticalspectrumlisttest test failed - singleThreadMS1 vs. multiThreadMS1" << endl;
			}

			bool unmatchedpeakstest = true;
			if (singleunmatchedpeaks.size() == multiunmatchedpeaks.size()) {
				for (int i = 0; i < singleunmatchedpeaks.size(); i++) {
					if (singleunmatchedpeaks[i].size() == multiunmatchedpeaks[i].size()) {
						for (int j = 0; j < singleunmatchedpeaks[i].size(); j++) {
							if (!singleunmatchedpeaks[i][j].equals(multiunmatchedpeaks[i][j])) {
								cout << "(unmatchedpeaks in " << i + 1 << " " << j + 1 << ")" << endl << endl;
								unmatchedpeakstest = false;
								break;
							}
						}
					}
					else {
						unmatchedpeakstest = false;
						break;
					}
				}
			}
			else {
				unmatchedpeakstest = false;
			}

			if (unmatchedpeakstest) {
				*os << endl << "unmatchedpeaks test ok - singleThreadMS1 vs. multiThreadMS1" << endl;
			}
			else {
				*os << endl << "unmatchedpeaks test failed - singleThreadMS1 vs. multiThreadMS1" << endl;
			}

			delete singletest;
			delete multitest;

			delete singlelistoftheoreticalspectra;
			delete multilistoftheoreticalspectra;
		}
	}
	

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {	
		*os << "Comparing theoretical spectra of candidates with the peak list... " << endl;
		listoftheoreticalspectra->initialize(*os, parameters, &graph);
		listoftheoreticalspectra->resize(1);
		if (listoftheoreticalspectra->parallelCompareAndStore(0, candidates, terminatecomputation) == -1) {
			emitEndSignals();
			return;
		}
		*os << " ok" << endl;
	}

	*os << endl << appname.toStdString() << " successfully finished at " << time.currentTime().toString().toStdString();
	*os << " (time elapsed: " << getCurrentTime(time) << ")." << endl;

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

