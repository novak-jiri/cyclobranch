#include "parallel/cSpectrumAnalyzerThreadMS1.h"

#include "gui/cMainThread.h"


void cSpectrumAnalyzerThreadMS1::initialize(cMainThread* mainthread, cParameters* parameters, int peaklistseriesvectorid, cTheoreticalSpectrum* poolspectrum, vector< vector<int> >* hintsindex, bool* terminatecomputation) {
	this->mainthread = mainthread;
	this->parameters = parameters;
	this->peaklistseriesvectorid = peaklistseriesvectorid;
	this->poolspectrum = poolspectrum;
	this->hintsindex = hintsindex;
	this->terminatecomputation = terminatecomputation;
}


void cSpectrumAnalyzerThreadMS1::run() {
	if ((parameters == 0) || (poolspectrum == 0) || (hintsindex == 0)) {
		return;
	}

	if (peaklistseriesvectorid >= parameters->peaklistseriesvector.size()) {
		return;
	}

	poolspectrum->setParameters(parameters);

	int peaklistseriessize = parameters->peaklistseriesvector[peaklistseriesvectorid].size();

	hintsindex->clear();
	hintsindex->resize(poolspectrum->getNumberOfPeaks());

	vector<cPeaksList> unmatchedpeaks;
	unmatchedpeaks.resize(peaklistseriessize);

	bool lcms = (parameters->peaklistseriesvector[peaklistseriesvectorid].size() > 1) && !((parameters->peaklistfileformats[peaklistseriesvectorid] == mis) || (parameters->peaklistfileformats[peaklistseriesvectorid] == imzML));
	bool rtavailable = (parameters->peaklistseriesvector[peaklistseriesvectorid].size() > 1) && (parameters->peaklistseriesvector[peaklistseriesvectorid][parameters->peaklistseriesvector[peaklistseriesvectorid].size() - 1].getRetentionTime() > 0);
	bool skipcomparison;

	for (int i = 0; i < peaklistseriessize; i++) {
		if (*terminatecomputation) {
			hintsindex->clear();
			return;
		}

		skipcomparison = false;
		if (lcms && rtavailable) {
			if (parameters->minimumrt != 0) {
				if (parameters->peaklistseriesvector[peaklistseriesvectorid][i].getRetentionTime() < parameters->minimumrt) {
					skipcomparison = true;
				}
			}
			if (parameters->maximumrt != 0) {
				if (parameters->peaklistseriesvector[peaklistseriesvectorid][i].getRetentionTime() > parameters->maximumrt) {
					skipcomparison = true;
				}
			}
		}

		cTheoreticalSpectrum tstmp;
		tstmp.setParameters(parameters);
		tstmp.getHintsIndex(i, peaklistseriesvectorid, *poolspectrum, unmatchedpeaks[i], *hintsindex, lcms && rtavailable, skipcomparison);
	}

	for (auto& it : *hintsindex) {
		sort(it.begin(), it.end());
	}

	if (mainthread) {
		mainthread->addToParallelOutputState(1);
	}
}

