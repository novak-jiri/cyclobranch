#include "parallel/cSpectrumComparatorThreadMS1.h"

#include "core/cTheoreticalSpectrumList.h"
#include "gui/cMainThread.h"


void cSpectrumComparatorThreadMS1::initialize(cMainThread* mainthread, cParameters* parameters, int peaklistseriesvectorid, cTheoreticalSpectrum* poolspectrum, vector< vector<int> >* hintsindex, cTheoreticalSpectrumList* theoreticalspectrumlist, vector<cPeaksList>* unmatchedpeaks, bool* terminatecomputation) {
	this->mainthread = mainthread;
	this->parameters = parameters;
	this->peaklistseriesvectorid = peaklistseriesvectorid;
	this->poolspectrum = poolspectrum;
	this->hintsindex = hintsindex;
	this->theoreticalspectrumlist = theoreticalspectrumlist;
	this->unmatchedpeaks = unmatchedpeaks;
	this->terminatecomputation = terminatecomputation;
}


void cSpectrumComparatorThreadMS1::run() {
	if ((parameters == 0) || (poolspectrum == 0) || (hintsindex == 0) || (theoreticalspectrumlist == 0) || (unmatchedpeaks == 0)) {
		return;
	}

	if (peaklistseriesvectorid >= parameters->peaklistseriesvector.size()) {
		return;
	}

	poolspectrum->setParameters(parameters);

	int peaklistseriessize = parameters->peaklistseriesvector[peaklistseriesvectorid].size();

	unmatchedpeaks->clear();
	unmatchedpeaks->resize(peaklistseriessize);

	// to do - theoreticalspectrumlist - reserve

	bool lcms = (parameters->peaklistseriesvector[peaklistseriesvectorid].size() > 1) && !((parameters->peaklistfileformats[peaklistseriesvectorid] == mis) || (parameters->peaklistfileformats[peaklistseriesvectorid] == imzML));
	bool rtavailable = (parameters->peaklistseriesvector[peaklistseriesvectorid].size() > 1) && (parameters->peaklistseriesvector[peaklistseriesvectorid][parameters->peaklistseriesvector[peaklistseriesvectorid].size() - 1].getRetentionTime() > 0);
	bool skipcomparison;

	for (int i = 0; i < peaklistseriessize; i++) {
		if (*terminatecomputation) {
			// to do - clear something ?
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
		tstmp.compareMSSpectrum(i, peaklistseriesvectorid, *poolspectrum, (*unmatchedpeaks)[i], *hintsindex, lcms && rtavailable, skipcomparison);
		theoreticalspectrumlist->add(peaklistseriesvectorid, tstmp);
	}

	if (mainthread) {
		mainthread->addToParallelOutputState(1);
	}
}

