#include "parallel/cSpectrumGeneratorThreadMS1.h"

#include "gui/cMainThread.h"


void cSpectrumGeneratorThreadMS1::initialize(cMainThread* mainthread, cParameters* parameters, int sequencestart, int sequencestop, cTheoreticalSpectrum* poolspectrum, bool* terminatecomputation) {
	this->mainthread = mainthread;
	this->parameters = parameters;
	this->sequencestart = sequencestart;
	this->sequencestop = sequencestop;
	this->poolspectrum = poolspectrum;
	this->terminatecomputation = terminatecomputation;
}


void cSpectrumGeneratorThreadMS1::run() {
	if ((poolspectrum == 0) || (parameters == 0)) {
		return;
	}

	poolspectrum->setParameters(parameters);

	// to do - use reserve for theoreticalpeaks

	if (parameters->generateisotopepattern) {
		poolspectrum->generateFineMSSpectrum(sequencestart, sequencestop, *terminatecomputation);
	}
	else {
		poolspectrum->generateMSSpectrum(sequencestart, sequencestop, *terminatecomputation, true);
	}

	if (mainthread) {
		mainthread->addToParallelOutputState(1);
	}
}

