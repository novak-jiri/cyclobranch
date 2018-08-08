#include "core/cTheoreticalSpectrum.h"
#include "core/cDeNovoGraph.h"
#include "core/cIsotopePatternCache.h"

#include "gui/cMainThread.h"


int getNumberOfScoreHits(vector<double>& scores, double value) {
	int left, right, middle, best, size;
	size = (int)scores.size();
	left = 0;
	right = size - 1;
	best = 0;
	while (left <= right) {
		middle = (left + right) / 2;
		if (value < scores[middle]) {
			right = middle - 1;
		}
		else {
			left = middle + 1;
			if (value == scores[middle]) {
				best = middle;
				// no break - fast search of the rightmost equal value
			}
		}
		if (fabs(scores[middle] - value) < fabs(scores[best] - value)) {
			best = middle;
		}
	}
	while ((best < size) && (scores[best] <= value)) {
		best++;
	}
	return best;
}


void visualSeries::store(ofstream& os) {
	int size;

	size = (int)series.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)series.size(); i++) {
		os.write((char *)&series[i], sizeof(int));
	}

	storeString(name, os);
}


void visualSeries::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	series.resize(size);
	for (int i = 0; i < (int)series.size(); i++) {
		is.read((char *)&series[i], sizeof(int));
	}

	loadString(name, is);
}


void cTheoreticalSpectrum::clearFalseHits(map<eFragmentIonType, map<int, vector<int> > >& series, vector<eFragmentIonType>& ions, vector<int>& losses) {
	for (int i = 0; i < (int)ions.size(); i++) {

		// parent (j == -1) is ignored here
		for (int j = 0; j < (int)losses.size(); j++) {

			for (int k = 0; k < (int)series[ions[i]][losses[j]].size(); k++) {

				if (series[ions[i]][-1][k] == 0) {
					series[ions[i]][losses[j]][k] = 0;
				}

			}

		}

	}
}


void cTheoreticalSpectrum::searchForPeakPairs(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, double fragmentmasserrortolerance) {
	int left, right, middle, best;
	double bestppm, currentppm;

	int experimentalpeakssize = experimentalpeaks.size();
	experimentalmatches.resize(experimentalpeakssize);

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		left = 0;
		right = experimentalpeakssize - 1;
		while (left <= right) {
			middle = (left + right) / 2;
			if (isInPpmMassErrorTolerance(experimentalpeaks[middle].mzratio, theoreticalpeaks[i].mzratio, fragmentmasserrortolerance)) {
				best = middle;
				bestppm = abs(ppmError(experimentalpeaks[middle].mzratio, theoreticalpeaks[i].mzratio));

				while ((middle > 0) && isInPpmMassErrorTolerance(experimentalpeaks[middle - 1].mzratio, theoreticalpeaks[i].mzratio, fragmentmasserrortolerance)) {
					currentppm = abs(ppmError(experimentalpeaks[middle - 1].mzratio, theoreticalpeaks[i].mzratio));
					if (currentppm < bestppm) {
						best = middle - 1;
						bestppm = currentppm;
					}
					middle--;
				}

				while ((middle < experimentalpeakssize - 1) && isInPpmMassErrorTolerance(experimentalpeaks[middle + 1].mzratio, theoreticalpeaks[i].mzratio, fragmentmasserrortolerance)) {
					currentppm = abs(ppmError(experimentalpeaks[middle + 1].mzratio, theoreticalpeaks[i].mzratio));
					if (currentppm < bestppm) {
						best = middle + 1;
						bestppm = currentppm;
					}
					middle++;
				}

				experimentalmatches[best].insert(i);
				experimentalpeaks[best].matched++;

				theoreticalpeaks[i].matched++;
				theoreticalpeaks[i].matchedid = best;
				break;
			}
			if (theoreticalpeaks[i].mzratio < experimentalpeaks[middle].mzratio) {
				right = middle - 1;
			}
			else {
				left = middle + 1;
			}
		}
	}
}


bool cTheoreticalSpectrum::searchHint(double mzratio, cPeaksList& experimentalpeaks, double fragmentmasserrortolerance) {
	int left, right, middle;
	int experimentalpeakssize = experimentalpeaks.size();

	left = 0;
	right = experimentalpeakssize - 1;
	while (left <= right) {
		middle = (left + right) / 2;
		if (isInPpmMassErrorTolerance(experimentalpeaks[middle].mzratio, mzratio, fragmentmasserrortolerance)) {
			return true;
		}
		if (mzratio < experimentalpeaks[middle].mzratio) {
			right = middle - 1;
		}
		else {
			left = middle + 1;
		}
	}
	return false;
}


void cTheoreticalSpectrum::computeStatistics(bool writedescription) {
	experimentalpeaksmatched = 0;
	scrambledpeaksmatched = 0;
	sumofrelativeintensities = 0;

	if (writedescription) {
		unmatchedexperimentalpeakscount = 0;
	}

	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		if (experimentalpeaks[i].matched > 0) {
			experimentalpeaksmatched++;
			
			sumofrelativeintensities += experimentalpeaks[i].relativeintensity;
			
			if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
				if (!experimentalpeaks[i].scrambled) {
					matchedions[experimentalpeaks[i].iontype][experimentalpeaks[i].neutrallosstype]++;
				}
				else {
					scrambledpeaksmatched++;
				}
			}
		}
		else {
			if (writedescription) {
				unmatchedexperimentalpeakscount++;
			}
		}
	}

	peakstested = experimentalpeaks.size();

	if (experimentalpeaks.size() > 0) {
		experimentalpeaksmatchedratio = ((double)experimentalpeaksmatched) / ((double)experimentalpeaks.size());
	}
	else {
		experimentalpeaksmatchedratio = 0;
	}
}


void cTheoreticalSpectrum::generatePrecursorIon(vector<int>& intcomposition, cBricksDatabase& bricksdatabasewithcombinations, int& theoreticalpeaksrealsize, bool writedescription) {
	cPeak peak;
	int starttype, endtype;
	set<int> usedmodifications;
	usedmodifications.insert(0);

	map<string, int> atoms, tempmap;
	atoms.clear();

	bool disablesummary = false;
	if ((parameters->mode == denovoengine) && (parameters->blindedges == 2)) {
		for (int i = 0; i < (int)intcomposition.size(); i++) {
			if (bricksdatabasewithcombinations[intcomposition[i] - 1].isArtificial()) {
				disablesummary = true;
				break;
			}
		}
	}

	peak.isotope = false;

	switch (parameters->peptidetype)
	{
	case linear:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getStartModifID()].summary, atoms);
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getEndModifID()].summary, atoms);
		}
		peak.seriesid = 0;
		starttype = (int)precursor_ion;
		endtype = (int)precursor_ion;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		break;
	case cyclic:
		peak.mzratio = 0;
		peak.seriesid = (int)intcomposition.size() - 1;
		starttype = (int)cyclic_precursor_ion;
		endtype = (int)cyclic_precursor_ion;
		break;
	case branched:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference + parameters->searchedmodifications[candidate.getMiddleModifID()].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getStartModifID()].summary, atoms);
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getEndModifID()].summary, atoms);
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getMiddleModifID()].summary, atoms);
		}
		peak.seriesid = 0;
		starttype = (int)precursor_ion;
		endtype = (int)precursor_ion;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		usedmodifications.insert(candidate.getMiddleModifID());
		break;
	case branchcyclic:
		peak.mzratio = parameters->searchedmodifications[candidate.getMiddleModifID()].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getMiddleModifID()].summary, atoms);
		}
		peak.seriesid = 0;
		starttype = (int)cyclic_precursor_ion;
		endtype = (int)cyclic_precursor_ion;
		usedmodifications.insert(candidate.getMiddleModifID());
		break;
	case linearpolyketide:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getStartModifID()].summary, atoms);
			addStringFormulaToMap(parameters->searchedmodifications[candidate.getEndModifID()].summary, atoms);
		}
		peak.seriesid = 0;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		break;
	case cyclicpolyketide:
		peak.mzratio = 0;
		peak.seriesid = (int)intcomposition.size() - 1;
		starttype = (int)cyclic_polyketide_precursor_ion;
		endtype = (int)cyclic_polyketide_precursor_ion;
		break;
	case other:
		break;
	default:
		break;
	}

	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			mergeMaps(bricksdatabasewithcombinations[intcomposition[i] - 1].getSummaryMap(), atoms);
		}
	}

	if (parameters->peptidetype == linearpolyketide) {
		double precursormass = charge(uncharge(parameters->precursormass, parameters->precursorcharge), (parameters->precursorcharge > 0)?1:-1);
		double thprecursormass = peak.mzratio + parameters->iondefinitions[linear_polyketide_precursor_ion_oh_oh].massdifference;
		thprecursormass = (parameters->precursorcharge > 0)?thprecursormass:thprecursormass - 2*(H - e);
		double bestmz = fabs(precursormass - thprecursormass);
		int bestmatch = 1;

		thprecursormass = peak.mzratio + parameters->iondefinitions[linear_polyketide_precursor_ion_h_oh].massdifference;
		thprecursormass = (parameters->precursorcharge > 0)?thprecursormass:thprecursormass - 2*(H - e);
		if (fabs(precursormass - thprecursormass) < bestmz) {
			bestmz = fabs(precursormass - thprecursormass);
			bestmatch = 2;
		}

		thprecursormass = peak.mzratio + parameters->iondefinitions[linear_polyketide_precursor_ion_h_h].massdifference;
		thprecursormass = (parameters->precursorcharge > 0)?thprecursormass:thprecursormass - 2*(H - e);
		if (fabs(precursormass - thprecursormass) < bestmz) {
			bestmz = fabs(precursormass - thprecursormass);
			bestmatch = 3;
		}

		if (bestmatch == 1) {
			starttype = (int)linear_polyketide_precursor_ion_oh_oh;
			endtype = (int)linear_polyketide_precursor_ion_oh_oh;
		}

		if (bestmatch == 2) {
			starttype = (int)linear_polyketide_precursor_ion_h_oh;
			endtype = (int)linear_polyketide_precursor_ion_h_oh;
		}

		if (bestmatch == 3) {
			starttype = (int)linear_polyketide_precursor_ion_h_h;
			endtype = (int)linear_polyketide_precursor_ion_h_h;
		}
	}
	
	double tempratio = peak.mzratio;
	for (int i = starttype; i <= endtype; i++) {
		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
			for (set<int>::iterator it = usedmodifications.begin(); it != usedmodifications.end(); ++it) {
				for (int k = 1; k <= abs(parameters->precursorcharge); k++) {
					peak.iontype = (eFragmentIonType)i;
					peak.neutrallosstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];
					peak.mzratio = tempratio + parameters->iondefinitions[(eFragmentIonType)i].massdifference - parameters->searchedmodifications[*it].massdifference;
					if (j >= 0) {
						peak.mzratio += parameters->neutrallossesdefinitions[peak.neutrallosstype].massdifference;
					}
					peak.mzratio = charge(uncharge(peak.mzratio, 1), (parameters->precursorcharge > 0) ? k : -k);
					peak.charge = (parameters->precursorcharge > 0) ? k : -k;

					if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
						tempmap = atoms;
						mergeMaps(parameters->iondefinitions[(eFragmentIonType)i].summarymap, tempmap);
						if (j >= 0) {
							mergeMaps(parameters->neutrallossesdefinitions[peak.neutrallosstype].summarymap, tempmap);
						}
						rechargeMap(peak.charge, tempmap);
						peak.formula.setFromMap(tempmap, false);
						peak.formula.addFormula(parameters->searchedmodifications[*it].summary, true);
					}

					if (writedescription) {
						string str;

						peak.description = parameters->iondefinitions[(eFragmentIonType)i].name;
						if (j >= 0) {
							peak.description += "-" + parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
						}
						if (*it > 0) {
							peak.description += " (" + parameters->searchedmodifications[*it].name + " loss)";
						}
						peak.description += ":";

						if (parameters->precursorcharge > 0) {
							if (!parameters->precursoradduct.empty()) {
								str = "+" + parameters->precursoradduct;
								if (k > 1) {
									str += "+";
									if (k > 2) {
										str += to_string(k - 1);
									}
									str += "H";
								}
								peak.description.replace(peak.description.find("+zH"), 3, str);
							}
							else {
								str = "+";
								if (k > 1) {
									str += to_string(k);
								}
								str += "H";
								peak.description.replace(peak.description.find("+zH"), 3, str);
							}

							str = "]";
							if (k > 1) {
								str += to_string(k);
							}
							str += "+";
							peak.description.replace(peak.description.find("]+"), 2, str);
						}
						else {
							if (!parameters->precursoradduct.empty()) {
								str = "-" + parameters->precursoradduct;
								if (k > 1) {
									str += "-";
									if (k > 2) {
										str += to_string(k - 1);
									}
									str += "H";
								}
								peak.description.replace(peak.description.find("+zH"), 3, str);
							}
							else {
								str = "-";
								if (k > 1) {
									str += to_string(k);
								}
								str += "H";
								peak.description.replace(peak.description.find("+zH"), 3, str);
							}

							str = "]";
							if (k > 1) {
								str += to_string(k);
							}
							str += "-";
							peak.description.replace(peak.description.find("]+"), 2, str);
						}
					}

					if (writedescription) {
						if (peak.formula.hasAllElementsPositive()) {
							if (parameters->reportunmatchedtheoreticalpeaks || searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
								addPeakToList(peak, theoreticalpeaksrealsize);
							}
							if (!parameters->generateisotopepattern) {
								addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, k, writedescription);
							}
						}
					}
					else {
						if (parameters->generateisotopepattern) {
							if (peak.formula.hasAllElementsPositive()) {
								if (searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
									addPeakToList(peak, theoreticalpeaksrealsize);
								}
							}
						}
						else {
							if (searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
								addPeakToList(peak, theoreticalpeaksrealsize);
							}
							addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, k, writedescription);
						}
					}

				}
			}
		}
	}
}


void cTheoreticalSpectrum::generateScrambledIons(cBricksDatabase& bricksdatabase, bool writedescription, int& theoreticalpeaksrealsize) {
	cBrick b;
	b.setComposition(candidate.getComposition(), false);
	vector<string> stringcomposition;
	b.explodeToStringComposition(stringcomposition);

	vector<int> intcomposition;

	if (stringcomposition.size() > 30) {
		// max 2^30 subsequences is tested
		return;
	}

	unordered_set<string> scrambledsequences;
	scrambledsequences.clear();

	int numberofbricks = (int)stringcomposition.size();
	int stop = (1 << numberofbricks) - 1;

	string subseq;
	int brickscount;

	// generate all possible combinations of blocks
	for (int i = 1; i < stop; i++) {
			subseq = "";
			brickscount = 0;
			for (int j = 0; j < numberofbricks; j++) {
				if ((i >> j) % 2 == 1) {
					if (brickscount > 0) {
						subseq += "-";
					}
					subseq += stringcomposition[j];
					brickscount++;
				}
			}
			if ((brickscount > 1) && (brickscount < numberofbricks - 1)) {
				scrambledsequences.insert(subseq);
			}
	}

	// select and normalize scrambled sequences
	selectAndNormalizeScrambledSequences(scrambledsequences);

	unordered_set<unsigned long long> theoreticalmzratios;
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		theoreticalmzratios.insert((unsigned long long)(theoreticalpeaks[i].mzratio*(double)1000000));
	}

	cPeaksList scrambledpeaks;
	scrambledpeaks.resize(5000);
	int scrambledspeaksrealsize = 0;

	cPeak peak;
	map<string, int> atoms;

	double tempmzratio;
	map<string, int> tempmap;
	bool tempmapready, disablesummary;
	
	// generate scrambled peaks from sequences
	for (auto it = scrambledsequences.begin(); it != scrambledsequences.end(); ++it) {

		b.setComposition((string &)*it, false);
		intcomposition.clear();
		b.explodeToIntComposition(intcomposition);

		tempmzratio = 0;
		for (int i = 0; i < (int)intcomposition.size(); i++) {
			tempmzratio += bricksdatabase[intcomposition[i] - 1].getMass();
		}

		disablesummary = false;
		if ((parameters->mode == denovoengine) && (parameters->blindedges == 2)) {
			for (int i = 0; i < (int)intcomposition.size(); i++) {
				if (bricksdatabase[intcomposition[i] - 1].isArtificial()) {
					disablesummary = true;
					break;
				}
			}
		}

		tempmapready = false;
			
		for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

			for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

				peak.clear();
				peak.iontype = (eFragmentIonType)parameters->ionsfortheoreticalspectra[i];
				if (j >= 0) {
					peak.neutrallosstype = parameters->neutrallossesfortheoreticalspectra[j];
				}
				else {
					peak.neutrallosstype = -1;
				}
				peak.mzratio = parameters->iondefinitions[peak.iontype].massdifference;
				if (peak.neutrallosstype >= 0) {
					peak.mzratio += parameters->neutrallossesdefinitions[peak.neutrallosstype].massdifference;
				}
				peak.mzratio += tempmzratio;

				peak.charge = 1;
				if (parameters->precursorcharge < 0) {
					peak.mzratio = charge(uncharge(peak.mzratio, 1), -1);
					peak.charge = -1;
				}

				// skip scrambled peaks whose m/z ratios collide with common fragment ions
				if (theoreticalmzratios.find((unsigned long long)(peak.mzratio*(double)1000000)) != theoreticalmzratios.end()) {
					continue;
				}

				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {

					if (!tempmapready) {
						tempmap.clear();

						for (int k = 0; k < (int)intcomposition.size(); k++) {
							mergeMaps(bricksdatabase[intcomposition[k] - 1].getSummaryMap(), tempmap);
						}

						tempmapready = true;
					}

					atoms = tempmap;
					mergeMaps(parameters->iondefinitions[peak.iontype].summarymap, atoms);
					if (peak.neutrallosstype >= 0) {
						mergeMaps(parameters->neutrallossesdefinitions[peak.neutrallosstype].summarymap, atoms);
					}

					if (peak.charge == -1) {
						rechargeMap(-1, atoms);
					}

					peak.formula.setFromMap(atoms, false);
				}

				peak.scrambled = true;

				if (writedescription) {

					if (parameters->precursorcharge < 0) {
						peak.description = "1- ";
					}
					else {
						peak.description = "1+ ";
					}

					peak.description += "scrambled ";
					peak.description += parameters->iondefinitions[peak.iontype].name.substr(0, 1) + to_string((int)intcomposition.size());
					if (parameters->iondefinitions[peak.iontype].name.size() > 1) {
						peak.description += parameters->iondefinitions[peak.iontype].name.substr(1, parameters->iondefinitions[peak.iontype].name.length() - 1);
					}
					if (peak.neutrallosstype >= 0) {
						peak.description += "-" + parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
					}

					addAdductToDescription(peak.description, parameters->metaladducts);
					peak.description += ": ";
					for (int k = 0; k < (int)intcomposition.size(); k++) {
						peak.description += "[" + bricksdatabase[intcomposition[k] - 1].getAcronymsAsString() + "]";
						if (k < (int)intcomposition.size() - 1) {
							peak.description += '-';
						}
					}

				}

				if (scrambledpeaks.size() > scrambledspeaksrealsize) {
					scrambledpeaks[scrambledspeaksrealsize] = peak;
				}
				else {
					scrambledpeaks.add(peak);
				}
				scrambledspeaksrealsize++;

			}
		
		}
	
	}

	scrambledpeaks.resize(scrambledspeaksrealsize);

	// attach scrambled peaks to common peaks
	for (int i = 0; i < (int)scrambledpeaks.size(); i++) {
		if (writedescription) {
			if (scrambledpeaks[i].formula.hasAllElementsPositive()) {
				if (parameters->reportunmatchedtheoreticalpeaks || searchHint(scrambledpeaks[i].mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
					addPeakToList(scrambledpeaks[i], theoreticalpeaksrealsize);
				}
				if (!parameters->generateisotopepattern) {
					addMetalPeaks(scrambledpeaks[i], parameters->metaladducts, theoreticalpeaksrealsize, 1, writedescription);
				}
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				if (scrambledpeaks[i].formula.hasAllElementsPositive()) {
					if (searchHint(scrambledpeaks[i].mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
						addPeakToList(scrambledpeaks[i], theoreticalpeaksrealsize);
					}
				}
			}
			else {
				if (searchHint(scrambledpeaks[i].mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
					addPeakToList(scrambledpeaks[i], theoreticalpeaksrealsize);
				}
				addMetalPeaks(scrambledpeaks[i], parameters->metaladducts, theoreticalpeaksrealsize, 1, writedescription);
			}
		}
	}

}


void cTheoreticalSpectrum::selectAndNormalizeScrambledSequences(unordered_set<string>& scrambledsequences) {
	unordered_set<string> temp = scrambledsequences;
	scrambledsequences.clear();
	
	string twocompositions = candidate.getComposition() + "-" + candidate.getComposition();
	cBrick b;
	b.setComposition(twocompositions, false);
	string reversedtwocompositions = b.getReverseComposition();

	for (auto it = temp.begin(); it != temp.end(); ++it) {
		if ((twocompositions.find((string& )*it) == string::npos) && (reversedtwocompositions.find((string& )*it) == string::npos)) {
			b.setComposition((string& )*it, true);
			scrambledsequences.insert(b.getComposition());

			/*
			b.setComposition((string&)*it, false);
			if (scrambledsequences.count(b.getReverseComposition()) == 0) {
				scrambledsequences.insert(b.getComposition());
			}
			*/
		}
	}
}


void cTheoreticalSpectrum::selectHigherPriorityIonTypeCID(cPeak& experimentalpeak, cPeak& theoreticalpeak) {

	switch (experimentalpeak.iontype) {
		case a_ion:
			break;
		case b_ion:
			if (theoreticalpeak.iontype == a_ion) {
				experimentalpeak.iontype = theoreticalpeak.iontype;
				experimentalpeak.neutrallosstype = theoreticalpeak.neutrallosstype;
			}
			break;
		case c_ion:
			if ((theoreticalpeak.iontype == y_ion) || (theoreticalpeak.iontype == b_ion) || (theoreticalpeak.iontype == a_ion)) {
				experimentalpeak.iontype = theoreticalpeak.iontype;
				experimentalpeak.neutrallosstype = theoreticalpeak.neutrallosstype;
			}
			break;
		case x_ion:
			if ((theoreticalpeak.iontype == y_ion) || (theoreticalpeak.iontype == b_ion) || (theoreticalpeak.iontype == a_ion)) {
				experimentalpeak.iontype = theoreticalpeak.iontype;
				experimentalpeak.neutrallosstype = theoreticalpeak.neutrallosstype;
			}
			break;
		case y_ion:
			if ((theoreticalpeak.iontype == b_ion) || (theoreticalpeak.iontype == a_ion)) {
				experimentalpeak.iontype = theoreticalpeak.iontype;
				experimentalpeak.neutrallosstype = theoreticalpeak.neutrallosstype;
			}
			break;
		case z_ion:
			if ((theoreticalpeak.iontype == y_ion) || (theoreticalpeak.iontype == b_ion) || (theoreticalpeak.iontype == a_ion)) {
				experimentalpeak.iontype = theoreticalpeak.iontype;
				experimentalpeak.neutrallosstype = theoreticalpeak.neutrallosstype;
			}
			break;
		default:
			break;
	}

}


void cTheoreticalSpectrum::addPeakToList(cPeak& peak, int& theoreticalpeaksrealsize) {
	if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
		theoreticalpeaks[theoreticalpeaksrealsize] = peak;
	}
	else {
		theoreticalpeaks.add(peak);
	}
	theoreticalpeaksrealsize++;
}


void cTheoreticalSpectrum::addAdductToDescription(string& description, vector<string>& metals) {
	for (int i = 0; i < (int)metals.size(); i++) {
		description += "_" + metals[i];
	}
}


void cTheoreticalSpectrum::addMetalPeaks(cPeak& peak, vector<string>& metals, int& peaklistrealsize, int peakcharge, bool writedescription) {

	double isotopethreshold = 0.03;

	cPeak tmppeak;
	double basemass;
	string isotopename;
	
	for (int i = 0; i < (int)metals.size(); i++) {

		map<string, isotope> isotopes = isotopemap[metals[i]];
		basemass = periodictablemap[metals[i]];

		for (auto it = isotopes.begin(); it != isotopes.end(); ++it) {

			if (metals[i].compare(it->first) == 0) {
				continue;
			}

			if (it->second.probability >= isotopethreshold) {
				tmppeak = peak;
				tmppeak.isotope = true;
				tmppeak.mzratio += (it->second.mass - basemass)/(double)peakcharge;

				if (writedescription) {
					isotopename = it->first;
					isotopename = stripHTML(isotopename);
					tmppeak.description.replace(tmppeak.description.find(metals[i]), metals[i].size(), isotopename);
				}

				if ((parameters->mode == dereplication) || (writedescription && parameters->reportunmatchedtheoreticalpeaks) || searchHint(tmppeak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
					addPeakToList(tmppeak, peaklistrealsize);
				}
			}

		}

	}

}


void cTheoreticalSpectrum::removeUnmatchedMetalIsotopes(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks) {
	int lastparent = 0;
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope) {
			lastparent = i;
			continue;
		}

		if ((theoreticalpeaks[lastparent].matched == 0) && (theoreticalpeaks[i].matched > 0)) {
			experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
			experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

			theoreticalpeaks[i].matched--;
			theoreticalpeaks[i].matchedid = -1;
		}
	}
}


void cTheoreticalSpectrum::removeUnmatchedIsotopePatterns(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, cPeaksList& outputtheoreticalpeaks, bool storeunmatchedpeaks) {
	if (theoreticalpeaksrealsize == 0) {
		return;
	}

	int minimumenvelopepeaks = parameters->minimumpatternsize;
	
	int groupid = theoreticalpeaks[0].groupid;
	int start = 0;
	int stop = 0;
	int maximumintensityid = 0;
	double maximumintensity = theoreticalpeaks[0].relativeintensity;
	double maximumexperimentalintensity;
	if (theoreticalpeaks[maximumintensityid].matched) {
		maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
	}
	int matchedpeaks = 0;
	
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {

		// detect group boundaries and the most intense peak in the group
		if (groupid == theoreticalpeaks[i].groupid) {
			if (theoreticalpeaks[i].relativeintensity > maximumintensity) {
				maximumintensityid = i;
				maximumintensity = theoreticalpeaks[i].relativeintensity;
				if (theoreticalpeaks[i].matched) {
					maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
				}
			}

			stop = i;
		}

		if (groupid != theoreticalpeaks[i].groupid) {

			if (theoreticalpeaks[maximumintensityid].matched) {
				for (int j = start; j <= stop; j++) {
					if ((theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold)) {
						matchedpeaks++;
					}
				}
			}
			
			if (theoreticalpeaks[maximumintensityid].matched && (theoreticalpeaks[maximumintensityid].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) && (matchedpeaks >= minimumenvelopepeaks)) {
				for (int j = start; j <= stop; j++) {
					// clear matched peaks below the intensity threshold
					if ((theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 < parameters->minimumrelativeintensitythreshold)) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;
					}

					// copy unmatched theoretical peaks in matched envelopes into the output
					if (storeunmatchedpeaks && !theoreticalpeaks[j].decoy && (theoreticalpeaks[j].matched == 0)) {
						if ((theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) ||
							((parameters->mode == dereplication) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks)) {
							outputtheoreticalpeaks.add(theoreticalpeaks[j]);
						}
					}
				}
			}
			// clear matches of all peaks in the group if the most intense peak was not matched or if the most intense peak is below the minimum relative intensity threshold
			else {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched > 0) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;
					}
					if (storeunmatchedpeaks && !theoreticalpeaks[j].decoy && (parameters->mode == dereplication) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks) {
						outputtheoreticalpeaks.add(theoreticalpeaks[j]);
					}
				}
			}

			// initialize next group
			groupid = theoreticalpeaks[i].groupid;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
			if (theoreticalpeaks[i].matched) {
				maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
			}
			matchedpeaks = 0;
		}

	}

	if (theoreticalpeaks[maximumintensityid].matched) {
		for (int j = start; j <= stop; j++) {
			if ((theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold)) {
				matchedpeaks++;
			}
		}
	}

	if (theoreticalpeaks[maximumintensityid].matched && (theoreticalpeaks[maximumintensityid].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) && (matchedpeaks >= minimumenvelopepeaks)) {
		for (int j = start; j <= stop; j++) {
			if ((theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 < parameters->minimumrelativeintensitythreshold)) {
				experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
				experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

				theoreticalpeaks[j].matched--;
				theoreticalpeaks[j].matchedid = -1;
			}

			if (storeunmatchedpeaks && !theoreticalpeaks[j].decoy && (theoreticalpeaks[j].matched == 0)) {
				if ((theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) ||
					((parameters->mode == dereplication) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks)) {
					outputtheoreticalpeaks.add(theoreticalpeaks[j]);
				}
			}
		}
	}
	else {
		for (int j = start; j <= stop; j++) {
			if (theoreticalpeaks[j].matched > 0) {
				experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
				experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

				theoreticalpeaks[j].matched--;
				theoreticalpeaks[j].matchedid = -1;
			}
			if (storeunmatchedpeaks && !theoreticalpeaks[j].decoy && (parameters->mode == dereplication) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks) {
				outputtheoreticalpeaks.add(theoreticalpeaks[j]);
			}
		}
	}
}


void cTheoreticalSpectrum::calculateEnvelopeScores(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks) {
	
	// theoreticalpeaks must not be modified here
	// the highest peaks in the input theoretical envelopes are normalized to 100% intensity
	
	if (theoreticalpeaksrealsize == 0) {
		return;
	}

	int groupid = theoreticalpeaks[0].groupid;
	bool decoyenvelope = theoreticalpeaks[0].decoy;
	double envelopescore = 0;
	int start = 0;
	int stop = 0;
	int maximumintensityid = 0;
	double maximumintensity = theoreticalpeaks[0].relativeintensity;
	double maximumexperimentalintensity;
	if (theoreticalpeaks[maximumintensityid].matched) {
		maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
	}
	int matchedpeaks = 0;

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {

		// detect group boundaries and the most intense peak in the group
		if (groupid == theoreticalpeaks[i].groupid) {
			if (theoreticalpeaks[i].relativeintensity > maximumintensity) {
				maximumintensityid = i;
				maximumintensity = theoreticalpeaks[i].relativeintensity;
				if (theoreticalpeaks[i].matched) {
					maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
				}
			}

			stop = i;
		}

		// normalize the theoretical intensities
		if (groupid != theoreticalpeaks[i].groupid) {
			if (theoreticalpeaks[maximumintensityid].matched) {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched) {
						//envelopescore += fabs(theoreticalpeaks[j].relativeintensity - experimentalpeaks[theoreticalpeaks[j].matchedid].relativeintensity / experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity * 100.0);
						matchedpeaks++;
					}
					else {
						if (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
							//envelopescore += theoreticalpeaks[j].relativeintensity;
						}
					}
				}
				//envelopescore = getStandardIntensityDeviation(theoreticalpeaks, start, stop, maximumexperimentalintensity);
				envelopescore = getAngleDistance(theoreticalpeaks, start, stop);
				//envelopescore = getStandardPPMDeviation(theoreticalpeaks, start, stop);
				//envelopescore /= experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity*experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
				//envelopescore /= (double)(matchedpeaks*matchedpeaks);
				if (!decoyenvelope) {
					targetscores.insert(make_pair(groupid, envelopescore));
				}
				else {
					decoyscores.insert(make_pair(groupid, envelopescore));
				}
			}

			// initialize next group
			groupid = theoreticalpeaks[i].groupid;
			decoyenvelope = theoreticalpeaks[i].decoy;
			envelopescore = 0;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
			if (theoreticalpeaks[i].matched) {
				maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
			}
			matchedpeaks = 0;
		}

	}

	if (theoreticalpeaks[maximumintensityid].matched) {
		for (int j = start; j <= stop; j++) {
			if (theoreticalpeaks[j].matched) {
				//envelopescore += fabs(theoreticalpeaks[j].relativeintensity - experimentalpeaks[theoreticalpeaks[j].matchedid].relativeintensity / experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity * 100.0);
				matchedpeaks++;
			}
			else {
				if (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
					//envelopescore += theoreticalpeaks[j].relativeintensity;
				}
			}
		}
		//envelopescore = getStandardIntensityDeviation(theoreticalpeaks, start, stop, maximumexperimentalintensity);
		envelopescore = getAngleDistance(theoreticalpeaks, start, stop);
		//envelopescore = getStandardPPMDeviation(theoreticalpeaks, start, stop);
		//envelopescore /= experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity*experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
		//envelopescore /= (double)(matchedpeaks*matchedpeaks);
		if (!decoyenvelope) {
			targetscores.insert(make_pair(groupid, envelopescore));
		}
		else {
			decoyscores.insert(make_pair(groupid, envelopescore));
		}
	}

}


void cTheoreticalSpectrum::removeDecoyPeakMatches(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks) {
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (theoreticalpeaks[i].decoy && (theoreticalpeaks[i].matched > 0)) {
			experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
			experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;
			
			theoreticalpeaks[i].matched--;
			theoreticalpeaks[i].matchedid = -1;
		}
	}
}


void cTheoreticalSpectrum::normalizeTheoreticalIntensities(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize) {

	if (theoreticalpeaksrealsize == 0) {
		return;
	}

	int groupid = theoreticalpeaks[0].groupid;
	int start = 0;
	int stop = 0;
	int maximumintensityid = 0;
	double maximumintensity = theoreticalpeaks[0].relativeintensity;

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {

		// detect group boundaries and the most intense peak in the group
		if (groupid == theoreticalpeaks[i].groupid) {
			if (theoreticalpeaks[i].relativeintensity > maximumintensity) {
				maximumintensityid = i;
				maximumintensity = theoreticalpeaks[i].relativeintensity;
			}

			stop = i;
		}

		// normalize the theoretical intensities
		if (groupid != theoreticalpeaks[i].groupid) {
			if (theoreticalpeaks[maximumintensityid].matchedrelativeintensity > 0) {
				for (int j = start; j <= stop; j++) {
					theoreticalpeaks[j].relativeintensity = theoreticalpeaks[j].relativeintensity*theoreticalpeaks[maximumintensityid].matchedrelativeintensity / 100.0;
				}
			}

			// initialize next group
			groupid = theoreticalpeaks[i].groupid;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
		}

	}

	if (theoreticalpeaks[maximumintensityid].matchedrelativeintensity > 0) {
		for (int j = start; j <= stop; j++) {
			theoreticalpeaks[j].relativeintensity = theoreticalpeaks[j].relativeintensity*theoreticalpeaks[maximumintensityid].matchedrelativeintensity / 100.0;
		}
	}

}


double cTheoreticalSpectrum::recalibrateToMostIntenseTheoreticalPeak(cPeaksList& peaklist) {
	if (peaklist.size() == 0) {
		return DBL_MAX;
	}

	// get the highest theoretical peak
	int highestpeak = 0;
	for (int i = 1; i < peaklist.size(); i++) {
		if (peaklist[i].relativeintensity > peaklist[highestpeak].relativeintensity) {
			highestpeak = i;
		}
	}

	if (!peaklist[highestpeak].matched) {
		return DBL_MAX;
	}

	double difference = experimentalpeaks[peaklist[highestpeak].matchedid].mzratio - peaklist[highestpeak].mzratio;
	double highestpeakppmerror = abs(ppmError(experimentalpeaks[peaklist[highestpeak].matchedid].mzratio, peaklist[highestpeak].mzratio));

	// shift the theoretical spectrum
	for (int i = 0; i < peaklist.size(); i++) {
		peaklist[i].mzratio = peaklist[i].mzratio + difference;
	}

	return highestpeakppmerror;
}


double cTheoreticalSpectrum::getStandardPPMDeviation(cPeaksList& theoreticalpeaks, int start, int stop) {
	double average, deviation, difference;
	int matchedpeaks;

	cPeaksList recalibrated;
	for (int i = start; i <= stop; i++) {
		recalibrated.add(theoreticalpeaks[i]);
	}
	
	//double highestpeakppmerror = recalibrateToMostIntenseTheoreticalPeak(recalibrated);
	double highestpeakppmerror = parameters->fragmentmasserrortolerance;

	average = 0;
	matchedpeaks = 0;
	for (int i = 0; i < recalibrated.size(); i++) {
		if (recalibrated[i].matched) {
			average += abs(ppmError(experimentalpeaks[recalibrated[i].matchedid].mzratio, recalibrated[i].mzratio));
			matchedpeaks++;
		}
		else {
			average += highestpeakppmerror;
			matchedpeaks++;
		}
	}

	if (matchedpeaks == 0) {
		return DBL_MAX;
	}

	average /= (double)matchedpeaks;

	deviation = 0;
	for (int i = 0; i < recalibrated.size(); i++) {
		if (recalibrated[i].matched) {
			difference = abs(ppmError(experimentalpeaks[recalibrated[i].matchedid].mzratio, recalibrated[i].mzratio)) - average;
			deviation += difference*difference;
		}
		else {
			difference = highestpeakppmerror - average;
			deviation += difference*difference;
		}
	}
	deviation /= (double)matchedpeaks;
	deviation = sqrt(deviation);

	return deviation;
}


double cTheoreticalSpectrum::getStandardIntensityDeviation(cPeaksList& theoreticalpeaks, int start, int stop, double maximumexperimentalintensity) {
	double average, deviation, difference;
	int matchedpeaks;

	average = 0;
	matchedpeaks = 0;
	for (int i = start; i <= stop; i++) {
		if (theoreticalpeaks[i].matched) {
			average += fabs(theoreticalpeaks[i].relativeintensity - experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity / maximumexperimentalintensity * 100.0);
			matchedpeaks++;
		}
		else {
			if (theoreticalpeaks[i].relativeintensity * maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
				average += theoreticalpeaks[i].relativeintensity;
				matchedpeaks++;
			}
		}
	}

	if (matchedpeaks == 0) {
		return DBL_MAX;
	}

	average /= (double)matchedpeaks;

	deviation = 0;
	for (int i = start; i <= stop; i++) {
		if (theoreticalpeaks[i].matched) {
			difference = fabs(theoreticalpeaks[i].relativeintensity - experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity / maximumexperimentalintensity * 100.0) - average;
			deviation += difference*difference;
		}
		else {
			if (theoreticalpeaks[i].relativeintensity * maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
				difference = theoreticalpeaks[i].relativeintensity - average;
				deviation += difference*difference;
			}
		}
	}
	deviation /= (double)matchedpeaks;
	deviation = sqrt(deviation);

	return deviation;
}


double cTheoreticalSpectrum::getStandardMZDifferenceDeviation(cPeaksList& theoreticalpeaks, int start, int stop) {
	vector<double> theoreticalmz;
	vector<double> experimentalmz;
	double average, deviation, difference;
	int differencescount;

	for (int i = start; i <= stop; i++) {
		if (theoreticalpeaks[i].matched) {
			theoreticalmz.push_back(theoreticalpeaks[i].mzratio);
			experimentalmz.push_back(experimentalpeaks[theoreticalpeaks[i].matchedid].mzratio);
		}
	}

	differencescount = (int)theoreticalmz.size() - 1;

	if (differencescount <= 0) {
		return DBL_MAX;
	}

	average = 0;
	for (int i = 0; i < differencescount; i++) {
		average += fabs(fabs(theoreticalmz[i + 1] - theoreticalmz[i]) - fabs(experimentalmz[i + 1] - experimentalmz[i]));
	}

	average /= (double)differencescount;

	deviation = 0;
	for (int i = 0; i < differencescount; i++) {
		difference = fabs(fabs(theoreticalmz[i + 1] - theoreticalmz[i]) - fabs(experimentalmz[i + 1] - experimentalmz[i])) - average;
		deviation += difference*difference;
	}
	deviation /= (double)differencescount;
	deviation = sqrt(deviation);

	return deviation;
}


double cTheoreticalSpectrum::getAngleDistance(cPeaksList& theoreticalpeaks, int start, int stop) {
	vector<double> theoreticalintensities;
	vector<double> experimentalintensities;

	int maxintensityid = 0;

	for (int i = start; i <= stop; i++) {
		if (theoreticalpeaks[i].matched) {
			theoreticalintensities.push_back(theoreticalpeaks[i].relativeintensity / 100.0);
			experimentalintensities.push_back(experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity / 100.0);

			if (experimentalintensities.back() > experimentalintensities[maxintensityid]) {
				maxintensityid = (int)experimentalintensities.size() - 1;
			}
		}
	}

	if (theoreticalintensities.empty() || experimentalintensities.empty()) {
		return 1;
	}

	double numerator = 0;
	double denominatora = 0;
	double denominatorb = 0;

	int size = (int)theoreticalintensities.size();
	for (int i = 0; i < size; i++) {
		theoreticalintensities[i] = theoreticalintensities[i] * experimentalintensities[maxintensityid];
		numerator += theoreticalintensities[i] * experimentalintensities[i];
		denominatora += theoreticalintensities[i] * theoreticalintensities[i];
		denominatorb += experimentalintensities[i] * experimentalintensities[i];
	}

	return acos(numerator / (sqrt(denominatora) * sqrt(denominatorb))) / (pi / 2.0);
}


void cTheoreticalSpectrum::generateFragmentIsotopePatterns(int& theoreticalpeaksrealsize, bool writedescription, unordered_map<string, int>* isotopeformuladesctoid) {
	cPeaksList tmppeaklist = theoreticalpeaks;
	int tmppeaklistsize = theoreticalpeaksrealsize;

	//static long long misses = 0;
	//static long long all = 0;

	cPeaksList isotopepattern;
	int isotopepatternsize;
	//static int test = 0;
	int descid;

	cPeak tmppeak;
	theoreticalpeaksrealsize = 0;
	for (int i = 0; i < tmppeaklistsize; i++) {
		if (!tmppeaklist[i].formula.isEmpty()) {
			isotopepattern.clear();
			string hashedformula = to_string(tmppeaklist[i].charge) + "@" + tmppeaklist[i].formula.getSummary();

			isotopepatterncache.lock();
			bool hit = isotopepatterncache.get(hashedformula, isotopepattern);
			isotopepatterncache.unlock();

			if (!hit) {
				isotopepattern = tmppeaklist[i].formula.getIsotopePattern(parameters->fwhm, abs(tmppeaklist[i].charge), (tmppeaklist[i].charge < 0) ? false : true, writedescription);

				isotopepatterncache.lock();
				isotopepatterncache.put(hashedformula, isotopepattern);
				isotopepatterncache.unlock();

				//misses++;
			}
			isotopepatternsize = isotopepattern.size();

			//all++;
			//if (all % 100000 == 0) {
			//	cout << "all: " << all << " misses:" << misses << " hits:" << all - misses << " misses-to-all ratio:" << (double)misses/(double)all*100.0 << "%" << endl;
			//}

			for (int j = 0; j < isotopepatternsize; j++) {
				tmppeak = tmppeaklist[i];
				tmppeak.mzratio = isotopepattern[j].mzratio;
				tmppeak.relativeintensity = isotopepattern[j].relativeintensity;
				tmppeak.absoluteintensity = isotopepattern[j].absoluteintensity;
				tmppeak.groupid = i;
				if (writedescription && isotopeformuladesctoid) {
					tmppeak.isotopeformula = isotopepattern[j].description;
					//test++;
					if (isotopeformuladesctoid->count(tmppeak.isotopeformula) > 0) {
						tmppeak.isotopeformulaid = (*isotopeformuladesctoid)[tmppeak.isotopeformula];
					}
					else {
						descid = (int)isotopeformuladesctoid->size();
						tmppeak.isotopeformulaid = descid;
						isotopeformuladesctoid->insert(make_pair(tmppeak.isotopeformula, descid));
					}
					tmppeak.isotopeformula.clear();
				}
				addPeakToList(tmppeak, theoreticalpeaksrealsize);
			}
			//cout << "desc: " << test << " " << isotopeformuladesctoid->size() << endl;
		}
		else {
			tmppeak = tmppeaklist[i];
			tmppeak.relativeintensity = 100;
			tmppeak.absoluteintensity = 100;
			tmppeak.groupid = i;
			addPeakToList(tmppeak, theoreticalpeaksrealsize);
		}
	}
}


void cTheoreticalSpectrum::fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(int& theoreticalpeaksrealsize, ePeptideType peptidetype, cPeaksList& unmatchedpeaksinmatchedpatterns, bool reportunmatchedtheoreticalpeaks, bool writedescription) {
	cPeaksList reducedpeaklist;
	vector<set<int> > updatedexperimentalmatches;

	if (!reportunmatchedtheoreticalpeaks) {
		updatedexperimentalmatches.resize(experimentalmatches.size());
	}

	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			cPeak matchedpeak;

			if (!reportunmatchedtheoreticalpeaks) {
				updatedexperimentalmatches[i].insert((int)reducedpeaklist.size());
				matchedpeak = theoreticalpeaks[*it];
			}

			if (writedescription) {
				if (!reportunmatchedtheoreticalpeaks) {
					matchedpeak.matchedmz = experimentalpeaks[i].mzratio;
					matchedpeak.matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
					matchedpeak.matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
					matchedpeak.matchedppm = ppmError(experimentalpeaks[i].mzratio, matchedpeak.mzratio);
				}
				else {
					theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
					theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
					theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
					theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
				}
			}

			if (!reportunmatchedtheoreticalpeaks) {
				reducedpeaklist.add(matchedpeak);
			}

			if (experimentalpeaks[i].iontype == fragmentIonTypeEnd) {
				experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
				experimentalpeaks[i].neutrallosstype = theoreticalpeaks[*it].neutrallosstype;
				if (peptidetype == cyclic) {
					experimentalpeaks[i].scrambled = theoreticalpeaks[*it].scrambled;
				}
			}
			else {
				if (peptidetype != cyclic) {
					if ((theoreticalpeaks[*it].neutrallosstype == -1) && (experimentalpeaks[i].neutrallosstype != -1)) {
						experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						experimentalpeaks[i].neutrallosstype = theoreticalpeaks[*it].neutrallosstype;
					}
					else {
						if (!((theoreticalpeaks[*it].neutrallosstype != -1) && (experimentalpeaks[i].neutrallosstype == -1))) {
							selectHigherPriorityIonTypeCID(experimentalpeaks[i], theoreticalpeaks[*it]);
						}
					}
				}
				else {
					if (theoreticalpeaks[*it].scrambled == experimentalpeaks[i].scrambled) {
						if ((theoreticalpeaks[*it].neutrallosstype == -1) && (experimentalpeaks[i].neutrallosstype != -1)) {
							experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
							experimentalpeaks[i].neutrallosstype = theoreticalpeaks[*it].neutrallosstype;
						}
						else {
							if (!((theoreticalpeaks[*it].neutrallosstype != -1) && (experimentalpeaks[i].neutrallosstype == -1))) {
								selectHigherPriorityIonTypeCID(experimentalpeaks[i], theoreticalpeaks[*it]);
							}
						}
					}
					else {
						if (experimentalpeaks[i].scrambled && !theoreticalpeaks[*it].scrambled) {
							experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
							experimentalpeaks[i].neutrallosstype = theoreticalpeaks[*it].neutrallosstype;
							experimentalpeaks[i].scrambled = theoreticalpeaks[*it].scrambled;
						}
					}
				}
			}

		}
	}

	if (!reportunmatchedtheoreticalpeaks) {
		experimentalmatches = updatedexperimentalmatches;
		theoreticalpeaks = reducedpeaklist;

		theoreticalpeaks.attach(unmatchedpeaksinmatchedpatterns);
		theoreticalpeaksrealsize = theoreticalpeaks.size();
	}
}


cTheoreticalSpectrum::cTheoreticalSpectrum() {
	clear();
}


cTheoreticalSpectrum::cTheoreticalSpectrum(cParameters* parameters, cCandidate& candidate) {
	clear();
	this->parameters = parameters;
	this->candidate = candidate;
}


void cTheoreticalSpectrum::clear(bool clearpeaklist) {
	parameters = 0;

	if (clearpeaklist) {
		theoreticalpeaks.clear();
	}

	experimentalpeaks.clear();
	matchedions.clear();

	candidate.clear();
	experimentalpeaksmatched = 0;
	experimentalmatches.clear();
	scrambledpeaksmatched = 0;
	peakstested = 0;
	experimentalpeaksmatchedratio = 0;
	unmatchedexperimentalpeakscount = 0;
	coveragebyseries = "";
	valid = false;
	sumofrelativeintensities = 0;
	visualcoverage.clear();
	validposition = -1;
	reversevalidposition = -1;
	
	seriescompleted = 1;
	pathid = 0;

	targetscores.clear();
	decoyscores.clear();
	fdrscores.clear();
}


cCandidate& cTheoreticalSpectrum::getCandidate() {
	return candidate;
}


void cTheoreticalSpectrum::setCandidate(cCandidate& candidate) {
	this->candidate = candidate;
}


int cTheoreticalSpectrum::compareBranched(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {

	cBrick brick;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	int losstype;

	vector<TRotationInfo> trotations;
	candidate.getPermutationsOfBranches(trotations);

	try {
		bool stop = true;
		for (int i = 0; i < (int)trotations.size(); i++) {
			if (regex_search(trotations[i].tcomposition, sequencetag)) {
				stop = false;
				break;
			}
		}
		if (stop) {
			return -2;
		}

		if (writedescription) {
			setValidSequence(searchedsequence);
		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}


	// initialize the experimental peaklist
	experimentalpeaks = sortedpeaklist;


	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			for (int k = 0; k < (int)trotations.size(); k++) {

				// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
				// in this case the modifID is 0, and both n-terminal and c-terminal fragments are generated
				// k == 2, 4, 5 are invalid
				if ((k == 0) || ((k == 1) && (parameters->searchedmodifications[trotations[k].startmodifID].nterminal)) || ((k == 3) && (parameters->searchedmodifications[trotations[k].endmodifID].cterminal))) {
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].nterminal) {
						generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[k].bricks, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotations[k]);
					}
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].cterminal) {
						generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[k].bricks, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotations[k]);
					}
				}

			}

		}

	}
	
	generatePrecursorIon(trotations[0].bricks, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	if (parameters->generateisotopepattern) {
		generateFragmentIsotopePatterns(theoreticalpeaksrealsize, writedescription, isotopeformuladesctoid);
	}


	// search the theoretical peaks in the experimental peak list
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, unmatchedpeaksinmatchedpatterns, writedescription && !parameters->reportunmatchedtheoreticalpeaks);
	}


	// coverage of series
	vector<map<eFragmentIonType, map<int, vector<int> > > > series;
	series.resize(trotations.size());
	for (int i = 0; i < (int)series.size(); i++) {

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				if (k >= 0) {
					series[i][parameters->ionsfortheoreticalspectra[j]][parameters->neutrallossesfortheoreticalspectra[k]].resize(trotations[0].bricks.size());
				}
				else {
					series[i][parameters->ionsfortheoreticalspectra[j]][-1].resize(trotations[0].bricks.size());
				}

			}

		}

	}
	
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) 
			&& (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)
			&& (series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)trotations.size(); i++) {	
			clearFalseHits(series[i], parameters->ionsfortheoreticalspectra, parameters->neutrallossesfortheoreticalspectra);
		}

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) 
				&& (series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][-1][theoreticalpeaks[i].seriesid] == 0)) {
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}*/


	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);
	}


	fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(theoreticalpeaksrealsize, branched, unmatchedpeaksinmatchedpatterns, parameters->reportunmatchedtheoreticalpeaks, writedescription);


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		if (parameters->generateisotopepattern) {
			if (!parameters->reportunmatchedtheoreticalpeaks) {
				theoreticalpeaks.sortbyGroupId();
			}
			normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaksrealsize);
		}

		visualcoverage.clear();
		visualSeries tempseries;
		bool reportrow;

		coveragebyseries = "Linearized sequences:<br/>";
		for (int i = 0; i < (int)trotations.size(); i++) {
			coveragebyseries += to_string(i + 1) + " ~ " + bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotations[i].tcomposition, false) + "<br/>";
		}
		coveragebyseries += "<br/>";

		if (parameters->ionsfortheoreticalspectra.size() > 0) {
			coveragebyseries += "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int m = 0; m < (int)series[0][parameters->ionsfortheoreticalspectra[0]][-1].size() - 1; m++) {
				coveragebyseries += "<td>" + to_string(m + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)series.size(); i++) {
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectra[0]][-1].size()) + "\">" + to_string(i + 1) + ":</td></tr>";
				for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {
					for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {
						tempseries.name = to_string(i + 1) + "_" + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].name;
						if (k >= 0) {
							tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
						}
						tempseries.series.clear();
						losstype = (k == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[k];
						reportrow = false;
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectra[j]][losstype].size() - 1; m++) {
							if (series[i][parameters->ionsfortheoreticalspectra[j]][losstype][m] > 0) {
								reportrow = true;
								break;
							}
						}
						if (reportrow) {
							coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
						}
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectra[j]][losstype].size() - 1; m++) {
							if (reportrow) {
								coveragebyseries += "<td>" + to_string(series[i][parameters->ionsfortheoreticalspectra[j]][losstype][m]) + "</td>";
							}
							tempseries.series.push_back(series[i][parameters->ionsfortheoreticalspectra[j]][losstype][m]);
						}
						if (reportrow) {
							coveragebyseries += "</tr>";
						}
						visualcoverage.push_back(tempseries);
					}
				}
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectra[0]][-1].size()) + "\">&nbsp;</td></tr>";
			}
			coveragebyseries += "</table>";
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLinear(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {
	
	cBrick brick;
	vector<int> intcomposition;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	int losstype;

	try {
		if (!regex_search(candidate.getComposition(), sequencetag)) {
			return -2;
		}
	
		if (writedescription) {
			setValidSequence(searchedsequence);
		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}


	// initialize the experimental peaklist
	experimentalpeaks = sortedpeaklist;


	intcomposition.clear();
	brick.clear();
	brick.setComposition(candidate.getComposition(), false);
	brick.explodeToIntComposition(intcomposition);

	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].nterminal) {
				generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder);
			}

			if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].cterminal) {
				generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder);
			}

		}

	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	if (parameters->generateisotopepattern) {
		generateFragmentIsotopePatterns(theoreticalpeaksrealsize, writedescription, isotopeformuladesctoid);
	}


	// search the theoretical peaks in the experimental peak list
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, unmatchedpeaksinmatchedpatterns, writedescription && !parameters->reportunmatchedtheoreticalpeaks);
	}


	// coverage of series
	map<eFragmentIonType, map<int, vector<int> > > series;
	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			if (j >= 0) {
				series[parameters->ionsfortheoreticalspectra[i]][parameters->neutrallossesfortheoreticalspectra[j]].resize(intcomposition.size());
			}
			else {
				series[parameters->ionsfortheoreticalspectra[i]][-1].resize(intcomposition.size());
			}

		}

	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) 
			&& (series.count(theoreticalpeaks[i].iontype) == 1) 
			&& (series[theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->ionsfortheoreticalspectra, parameters->neutrallossesfortheoreticalspectra);

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid != -1) 
				&& (series[theoreticalpeaks[i].iontype][-1][theoreticalpeaks[i].seriesid] == 0)) {
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}*/


	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);
	}


	fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(theoreticalpeaksrealsize, linear, unmatchedpeaksinmatchedpatterns, parameters->reportunmatchedtheoreticalpeaks, writedescription);


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		if (parameters->generateisotopepattern) {
			if (!parameters->reportunmatchedtheoreticalpeaks) {
				theoreticalpeaks.sortbyGroupId();
			}
			normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaksrealsize);
		}

		visualcoverage.clear();
		visualSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectra.size() > 0) {
			coveragebyseries = "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectra[0]][-1].size() - 1; k++) {
				coveragebyseries += "<td>" + to_string(k + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {
				for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
					tempseries.name = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name;
					if (j >= 0) {
						tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
					}
					tempseries.series.clear();
					losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];
					reportrow = false;
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectra[i]][losstype].size() - 1; k++) {
						if (series[parameters->ionsfortheoreticalspectra[i]][losstype][k] > 0) {
							reportrow = true;
							break;
						}
					}
					if (reportrow) {
						coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
					}
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectra[i]][losstype].size() - 1; k++) {
						if (reportrow) {
							coveragebyseries += "<td>" + to_string(series[parameters->ionsfortheoreticalspectra[i]][losstype][k]) + "</td>";
						}
						tempseries.series.push_back(series[parameters->ionsfortheoreticalspectra[i]][losstype][k]);
					}
					if (reportrow) {
						coveragebyseries += "</tr>";
					}
					visualcoverage.push_back(tempseries);
				}
			}
			coveragebyseries += "</table>";
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;

}


int cTheoreticalSpectrum::compareCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {

	cBrick brick;
	vector<int> intcomposition;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	int losstype;

	vector<string> rotations;
	candidate.getRotations(rotations, true);
	
	int r = (int)rotations.size() / 2;
	validposition = -1;
	reversevalidposition = -1;


	try {
		bool stop = true;
		for (int i = 0; i < (int)rotations.size(); i++) {
			if (regex_search(rotations[i], sequencetag)) {
				stop = false;
				break;
			}
		}
		if (stop) {
			return -2;
		}

		if (writedescription) {
			setValidSequence(searchedsequence);
		
			bool enableshiftoflabels = false;
			if (!enableshiftoflabels || (validposition == -1)) {
				validposition = 0;
				reversevalidposition = r;
			}

			if (validposition != -1) {

				splittingsites.resize(rotations.size());
				splittingsites[validposition].first = 0;
				splittingsites[validposition].second = r - 1;
				splittingsites[reversevalidposition].first = r - 1;
				splittingsites[reversevalidposition].second = 0;

				if (validposition < r) {
					for (int i = 1; i < r; i++) {
						splittingsites[(validposition + i) % r].first = (splittingsites[(validposition + i + r - 1) % r].first + 1) % r;
						splittingsites[(validposition + i) % r].second = (splittingsites[(validposition + i + r - 1) % r].second + 1) % r;
						splittingsites[((reversevalidposition + i) % r) + r].first = (splittingsites[((reversevalidposition + i + r - 1) % r) + r].first + r - 1) % r;
						splittingsites[((reversevalidposition + i) % r) + r].second = (splittingsites[((reversevalidposition + i + r - 1) % r) + r].second + r - 1) % r;
					}
				}
				else {
					for (int i = 1; i < r; i++) {
						splittingsites[((validposition + i) % r) + r].first = (splittingsites[((validposition + i + r - 1) % r) + r].first + 1) % r;
						splittingsites[((validposition + i) % r) + r].second = (splittingsites[((validposition + i + r - 1) % r) + r].second + 1) % r;
						splittingsites[(reversevalidposition + i) % r].first = (splittingsites[(reversevalidposition + i + r - 1) % r].first + r - 1) % r;
						splittingsites[(reversevalidposition + i) % r].second = (splittingsites[(reversevalidposition + i + r - 1) % r].second + r - 1) % r;
					}
				}

			}

		}
	}
	catch (regex_error& /*e*/) {
		return -2;
	}


	// initialize the experimental peaklist
	experimentalpeaks = sortedpeaklist;


	eResidueLossType leftresiduelosstype = h2o_loss;
	bool hasfirstblockartificial = false;

	for (int i = 0; i < 2*r; i++) {

		intcomposition.clear();
		brick.clear();
		brick.setComposition(rotations[i], false);
		brick.explodeToIntComposition(intcomposition);

		eResidueLossType leftresiduelosstype = bricksdatabasewithcombinations[intcomposition[0] - 1].getResidueLossType();
		bool hasfirstblockartificial = bricksdatabasewithcombinations[intcomposition[0] - 1].isArtificial();

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				losstype = (k == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[k];

				generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectra[j], losstype, bricksdatabasewithcombinations, writedescription, i, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, leftresiduelosstype, hasfirstblockartificial);
			
			}

		}

	}

	if (parameters->enablescrambling) {
		generateScrambledIons(bricksdatabasewithcombinations, writedescription, theoreticalpeaksrealsize);
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	if (parameters->generateisotopepattern) {
		generateFragmentIsotopePatterns(theoreticalpeaksrealsize, writedescription, isotopeformuladesctoid);
	}


	// search the theoretical peaks in the experimental peak list
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, unmatchedpeaksinmatchedpatterns, writedescription && !parameters->reportunmatchedtheoreticalpeaks);
	}


	// coverage of series
	vector<map<eFragmentIonType, map<int, vector<int> > > > series;
	series.resize(rotations.size());
	for (int i = 0; i < (int)series.size(); i++) {

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				if (k >= 0) {
					series[i][parameters->ionsfortheoreticalspectra[j]][parameters->neutrallossesfortheoreticalspectra[k]].resize(r);
				}
				else {
					series[i][parameters->ionsfortheoreticalspectra[j]][-1].resize(r);
				}

			}

		}

	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) 
			&& (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)
			&& (series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}


	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)rotations.size(); i++) {	
			clearFalseHits(series[i], parameters->ionsfortheoreticalspectra, parameters->neutrallossesfortheoreticalspectra);
		}

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) 
				&& (series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][-1][theoreticalpeaks[i].seriesid] == 0)) {
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}

		// remove scrambled peaks without parents
		if (parameters->enablescrambling) {
			int res;
			bool matchesscrambled;
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

				// parents at j == -1 are skipped
				for (int j = 0; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

					int k = 0;
					while (k < theoreticalpeaksrealsize) {
						// current peak is scrambled, matched, tested ion type matches the ion type and tested neutral loss type matches the loss type of the scrambled peak
						if (theoreticalpeaks[k].scrambled && (theoreticalpeaks[k].matched > 0) && (parameters->ionsfortheoreticalspectra[i] == theoreticalpeaks[k].iontype) && (parameters->neutrallossesfortheoreticalspectra[j] == theoreticalpeaks[k].neutrallosstype)) {

							// parent not found
							res = experimentalpeaks.find(theoreticalpeaks[k].mzratio - parameters->neutrallossesdefinitions[theoreticalpeaks[k].neutrallosstype].massdifference, parameters->fragmentmasserrortolerance);
							if (res == -1) {
								experimentalmatches[theoreticalpeaks[k].matchedid].erase(experimentalmatches[theoreticalpeaks[k].matchedid].find(k));
								experimentalpeaks[theoreticalpeaks[k].matchedid].matched--;

								theoreticalpeaks[k].matched--;
								theoreticalpeaks[k].matchedid = -1;
							}
							else {
								matchesscrambled = false;
								for (auto it = experimentalmatches[res].begin(); it != experimentalmatches[res].end(); ++it) {
									if ((theoreticalpeaks[*it].matched > 0) && theoreticalpeaks[*it].scrambled) {
										matchesscrambled = true;
										break;
									}
								}

								// a scrambled peak is not matched by parent
								if (!matchesscrambled) {
									experimentalmatches[theoreticalpeaks[k].matchedid].erase(experimentalmatches[theoreticalpeaks[k].matchedid].find(k));
									experimentalpeaks[theoreticalpeaks[k].matchedid].matched--;

									theoreticalpeaks[k].matched--;
									theoreticalpeaks[k].matchedid = -1;
								}
								else {
									k++;
								}
							}
						}
						else {
							k++;
						}
					}

				}

			}
		}
	}*/


	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);
	}


	fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(theoreticalpeaksrealsize, cyclic, unmatchedpeaksinmatchedpatterns, parameters->reportunmatchedtheoreticalpeaks, writedescription);
	

	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		if (parameters->generateisotopepattern) {
			if (!parameters->reportunmatchedtheoreticalpeaks) {
				theoreticalpeaks.sortbyGroupId();
			}
			normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaksrealsize);
		}

		visualcoverage.clear();
		visualSeries tempseries;
		bool reportrow;

		coveragebyseries = "Linearized sequences from all ring break up points:<br/>";
		for (int i = 0; i < (int)rotations.size(); i++) {
			if (i == r) {
				coveragebyseries += "<br/>";
			}
			coveragebyseries += to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + " " + bricksdatabasewithcombinations.getAcronymName(rotations[i], false) + "<br/>";
		}
		coveragebyseries += "<br/>";

		if (parameters->ionsfortheoreticalspectra.size() > 0) {
			coveragebyseries += "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int m = 0; m < (int)series[0][parameters->ionsfortheoreticalspectra[0]][-1].size() - 1; m++) {
				coveragebyseries += "<td>" + to_string(m + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)series.size(); i++) {
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectra[0]][-1].size()) + "\">" + to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + ":</td></tr>";
				for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {
					for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {
						tempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_" + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].name;
						if (k >= 0) {
							tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
						}
						tempseries.series.clear();
						losstype = (k == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[k];
						reportrow = false;
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectra[j]][losstype].size() - 1; m++) {
							if (series[i][parameters->ionsfortheoreticalspectra[j]][losstype][m] > 0) {
								reportrow = true;
								break;
							}
						}
						if (reportrow) {
							coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
						}
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectra[j]][losstype].size() - 1; m++) {
							if (reportrow) {
								coveragebyseries += "<td>" + to_string(series[i][parameters->ionsfortheoreticalspectra[j]][losstype][m]) + "</td>";
							}
							tempseries.series.push_back(series[i][parameters->ionsfortheoreticalspectra[j]][losstype][m]);
						}
						if (reportrow) {
							coveragebyseries += "</tr>";
						}
						visualcoverage.push_back(tempseries);
					}
				}
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectra[0]][-1].size()) + "\">&nbsp;</td></tr>";
			}
			coveragebyseries += "</table>";
		}

	}
		
	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareBranchCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {

	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	vector<cCandidate> branchcyclicrotations;
	int losstype;

	// normalize the candidate
	candidate.getBranchCyclicRotations(branchcyclicrotations, false);
	int numberofbricks = getNumberOfBricks(candidate.getComposition());
	for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
		if (branchcyclicrotations[i].getBranchEnd() == numberofbricks - 1) {
			vector<string> v;
			v.push_back(branchcyclicrotations[i].getComposition());
			string name = candidate.getName();
			cSummaryFormula summary = candidate.getSummaryFormula();
			vector<nodeEdge> cpath = candidate.getPath();
			candidate.setCandidate(v, cpath, candidate.getStartIonType(), candidate.getStartModifID(), candidate.getEndModifID(), candidate.getMiddleModifID(), branchcyclicrotations[i].getBranchStart(), branchcyclicrotations[i].getBranchEnd());
			candidate.setName(name);
			candidate.setSummaryFormula(summary);
			break;
		}
	}

	// get branch-cyclic rotations
	candidate.getBranchCyclicRotations(branchcyclicrotations, true);

	// get T-permutations of branch-cyclic rotations
	vector<vector<TRotationInfo> > trotationsofbranchcyclicrotations;
	trotationsofbranchcyclicrotations.resize(branchcyclicrotations.size());
	for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
		branchcyclicrotations[i].getPermutationsOfBranches(trotationsofbranchcyclicrotations[i]);
	}


	int r = (int)branchcyclicrotations.size() / 2;
	validposition = -1;
	reversevalidposition = -1;

	try {
		bool stop = true;
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {
				if (regex_search(trotationsofbranchcyclicrotations[i][j].tcomposition, sequencetag)) {
					stop = false;
					break;
				}
			}
		}
		if (stop) {
			return -2;
		}

		if (writedescription) {
			setValidSequence(searchedsequence);

			bool enableshiftoflabels = true;
			if (!enableshiftoflabels || (validposition == -1)) {
				validposition = 0;
				reversevalidposition = r;
			}

			if (validposition != -1) {

				splittingsites.resize(branchcyclicrotations.size());
				splittingsites[validposition].first = 0;
				splittingsites[validposition].second = r - 1;
				splittingsites[reversevalidposition].first = r - 1;
				splittingsites[reversevalidposition].second = 0;

				if (validposition < r) {
					for (int i = 1; i < r; i++) {
						splittingsites[(validposition + i) % r].first = (splittingsites[(validposition + i + r - 1) % r].first + 1) % r;
						splittingsites[(validposition + i) % r].second = (splittingsites[(validposition + i + r - 1) % r].second + 1) % r;
						splittingsites[((reversevalidposition + i) % r) + r].first = (splittingsites[((reversevalidposition + i + r - 1) % r) + r].first + r - 1) % r;
						splittingsites[((reversevalidposition + i) % r) + r].second = (splittingsites[((reversevalidposition + i + r - 1) % r) + r].second + r - 1) % r;
					}
				}
				else {
					for (int i = 1; i < r; i++) {
						splittingsites[((validposition + i) % r) + r].first = (splittingsites[((validposition + i + r - 1) % r) + r].first + 1) % r;
						splittingsites[((validposition + i) % r) + r].second = (splittingsites[((validposition + i + r - 1) % r) + r].second + 1) % r;
						splittingsites[(reversevalidposition + i) % r].first = (splittingsites[(reversevalidposition + i + r - 1) % r].first + r - 1) % r;
						splittingsites[(reversevalidposition + i) % r].second = (splittingsites[(reversevalidposition + i + r - 1) % r].second + r - 1) % r;
					}
				}

			}

		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}


	// initialize the experimental peaklist
	experimentalpeaks = sortedpeaklist;


	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			for (int m = 0; m < (int)branchcyclicrotations.size(); m++) {

				for (int n = 0; n < (int)trotationsofbranchcyclicrotations[m].size(); n++) {

					// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
					// in this case the modifID is 0, and both n-terminal (*) and c-terminal (**) fragments are generated

					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].nterminal) {
						// if the start modification for n == 1 or n == 4 is nterminal, generate n-terminal ions (*)
						if (!(((n == 1) || (n == 4)) && (!parameters->searchedmodifications[trotationsofbranchcyclicrotations[m][n].startmodifID].nterminal))) {
							generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[m][n].bricks, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, m, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotationsofbranchcyclicrotations[m][n]);
						}
					}

					// all permmutations except 3 and 5 have invalid c-terms
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].cterminal && ((n == 3) || (n == 5) /*|| ((n == 0) && (trotationsofbranchcyclicrotations[m][n].endsWithBracket())) || ((n == 2) && (trotationsofbranchcyclicrotations[m][n].startsWithBracket()))*/)) {
						// if the end modification is cterminal, generate c-terminal ions (**)
						if (parameters->searchedmodifications[trotationsofbranchcyclicrotations[m][n].endmodifID].cterminal) {
							generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[m][n].bricks, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, m, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotationsofbranchcyclicrotations[m][n]);
						}
					}

				}

			}

		}

	}

	generatePrecursorIon(trotationsofbranchcyclicrotations[0][0].bricks, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	if (parameters->generateisotopepattern) {
		generateFragmentIsotopePatterns(theoreticalpeaksrealsize, writedescription, isotopeformuladesctoid);
	}


	// search the theoretical peaks in the experimental peak list
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, unmatchedpeaksinmatchedpatterns, writedescription && !parameters->reportunmatchedtheoreticalpeaks);
	}


	// coverage of series
	vector<vector<map<eFragmentIonType, map<int, vector<int> > > > > series;
	series.resize(branchcyclicrotations.size());
	for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
		series[i].resize(6);
		//series[i].resize(trotationsofbranchcyclicrotations[i].size()); // to do - potential bug

		for (int j = 0; j < (int)series[i].size(); j++) {

			for (int k = 0; k < (int)parameters->ionsfortheoreticalspectra.size(); k++) {

				for (int m = -1; m < (int)parameters->neutrallossesfortheoreticalspectra.size(); m++) {

					if (m >= 0) {
						series[i][j][parameters->ionsfortheoreticalspectra[k]][parameters->neutrallossesfortheoreticalspectra[m]].resize(trotationsofbranchcyclicrotations[i][0].bricks.size());
					}
					else {
						series[i][j][parameters->ionsfortheoreticalspectra[k]][-1].resize(trotationsofbranchcyclicrotations[i][0].bricks.size());
					}

				}

			}

		}
	
		for (int j = 0; j < theoreticalpeaksrealsize; j++) {
			if (!theoreticalpeaks[j].isotope && (theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) 
				&& (series[i][theoreticalpeaks[j].rotationid % 6].count(theoreticalpeaks[j].iontype) == 1)
				&& (series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[j].iontype].count(theoreticalpeaks[j].neutrallosstype) == 1)) {
				if (i == theoreticalpeaks[j].rotationid / 6) {
					series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[j].iontype][theoreticalpeaks[j].neutrallosstype][theoreticalpeaks[j].seriesid]++;
				}
			}
		}
	}


	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {	
				clearFalseHits(series[i][j], parameters->ionsfortheoreticalspectra, parameters->neutrallossesfortheoreticalspectra);
			}

			for (int j = 0; j < theoreticalpeaksrealsize; j++) {
				if (!theoreticalpeaks[j].isotope && (theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) && (i == theoreticalpeaks[j].rotationid / 6) 
					&& (series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[i].iontype][-1][theoreticalpeaks[i].seriesid] == 0)) {
					experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
					experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

					theoreticalpeaks[j].matched--;
					theoreticalpeaks[j].matchedid = -1;
				}
			}
		}
	}*/


	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);
	}


	fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(theoreticalpeaksrealsize, branchcyclic, unmatchedpeaksinmatchedpatterns, parameters->reportunmatchedtheoreticalpeaks, writedescription);


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		if (parameters->generateisotopepattern) {
			if (!parameters->reportunmatchedtheoreticalpeaks) {
				theoreticalpeaks.sortbyGroupId();
			}
			normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaksrealsize);
		}

		visualcoverage.clear();
		visualSeries tempseries;
		bool reportrow;

		coveragebyseries = "Linearized sequences from all ring break up points:<br/>";
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {
				coveragebyseries += to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
				coveragebyseries += to_string(j + 1) + " ~ ";
				coveragebyseries += bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotationsofbranchcyclicrotations[i][j].tcomposition, false);
				coveragebyseries += "<br/>";
			}
			coveragebyseries += "<br/>";
		}

		if (parameters->ionsfortheoreticalspectra.size() > 0) {
			coveragebyseries += "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int n = 0; n < (int)series[0][0][parameters->ionsfortheoreticalspectra[0]][-1].size() - 1; n++) {
				coveragebyseries += "<td>" + to_string(n + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
				for (int j = 0; j < (int)series[i].size(); j++) {
					coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][0][parameters->ionsfortheoreticalspectra[0]][-1].size()) + "\">" + to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_" + to_string(j + 1) + ":</td></tr>";
					for (int k = 0; k < (int)parameters->ionsfortheoreticalspectra.size(); k++) {
						for (int m = -1; m < (int)parameters->neutrallossesfortheoreticalspectra.size(); m++) {
							tempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
							tempseries.name += to_string(j + 1) + "_" + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[k]].name;
							if (m >= 0) {
								tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[m]].summary;
							}
							tempseries.series.clear();
							losstype = (m == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[m];
							reportrow = false;
							for (int n = 0; n < (int)series[i][j][parameters->ionsfortheoreticalspectra[k]][losstype].size() - 1; n++) {
								if (series[i][j][parameters->ionsfortheoreticalspectra[k]][losstype][n] > 0) {
									reportrow = true;
									break;
								}
							}
							if (reportrow) {
								coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
							}
							for (int n = 0; n < (int)series[i][j][parameters->ionsfortheoreticalspectra[k]][losstype].size() - 1; n++) {
								if (reportrow) {
									coveragebyseries += "<td>" + to_string(series[i][j][parameters->ionsfortheoreticalspectra[k]][losstype][n]) + "</td>";
								}
								tempseries.series.push_back(series[i][j][parameters->ionsfortheoreticalspectra[k]][losstype][n]);
							}
							if (reportrow) {
								coveragebyseries += "</tr>";
							}
							visualcoverage.push_back(tempseries);
						}
					}
					coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][0][parameters->ionsfortheoreticalspectra[0]][-1].size()) + "\">&nbsp;</td></tr>";
				}
			}
			coveragebyseries += "</table>";
		}

	}


	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLinearPolyketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {
	
	if (!candidate.checkKetideSequence(bricksdatabasewithcombinations, parameters->peptidetype, parameters->regularblocksorder)) {
		return -2;
	}

	cBrick brick;
	vector<int> intcomposition;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	int losstype;

	intcomposition.clear();
	brick.clear();
	brick.setComposition(candidate.getComposition(), false);
	brick.explodeToIntComposition(intcomposition);

	eResidueLossType leftresiduelosstype = bricksdatabasewithcombinations[intcomposition[0] - 1].getResidueLossType();
	eResidueLossType rightresiduelosstype = bricksdatabasewithcombinations[intcomposition.back() - 1].getResidueLossType();

	bool hasfirstblockartificial = bricksdatabasewithcombinations[intcomposition[0] - 1].isArtificial();
	bool haslastblockartificial = bricksdatabasewithcombinations[intcomposition.back() - 1].isArtificial();

	if (!hasfirstblockartificial && !haslastblockartificial) {
		if (((leftresiduelosstype == h2_loss) && (candidate.getStartModifID() > 0) && parameters->searchedmodifications[candidate.getStartModifID()].cterminal) 
/*			|| ((leftresiduelosstype == h2o_loss) && (candidate.getStartModifID() > 0) && parameters->searchedmodifications[candidate.getStartModifID()].nterminal)*/
			|| ((rightresiduelosstype == h2_loss) && (candidate.getEndModifID() > 0) && parameters->searchedmodifications[candidate.getEndModifID()].cterminal)
/*			|| ((rightresiduelosstype == h2o_loss) && (candidate.getEndModifID() > 0) && parameters->searchedmodifications[candidate.getEndModifID()].nterminal)*/) {
			return -2;
		}
	}

	cCandidate revertedcandidate = candidate;
	revertedcandidate.revertComposition();

	try {
		if (!regex_search(candidate.getComposition(), sequencetag) && !regex_search(revertedcandidate.getComposition(), sequencetag)) {
			return -2;
		}
	
		if (writedescription) {
			setValidSequence(searchedsequence);
		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}


	// initialize the experimental peaklist
	experimentalpeaks = sortedpeaklist;


	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			if (
				((hasfirstblockartificial || !parameters->regularblocksorder || (leftresiduelosstype == h2_loss)) && ((parameters->ionsfortheoreticalspectra[i] == l1h_ion) || (parameters->ionsfortheoreticalspectra[i] == l2h_ion)))
				||
				((hasfirstblockartificial || !parameters->regularblocksorder || (leftresiduelosstype == h2o_loss)) && ((parameters->ionsfortheoreticalspectra[i] == l1oh_ion) || (parameters->ionsfortheoreticalspectra[i] == l2oh_ion)))
				) {
				generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, leftresiduelosstype, hasfirstblockartificial);
			}
			if (
				((haslastblockartificial || !parameters->regularblocksorder || (rightresiduelosstype == h2_loss)) && ((parameters->ionsfortheoreticalspectra[i] == r1h_ion) || (parameters->ionsfortheoreticalspectra[i] == r2h_ion)))
				||
				((haslastblockartificial || !parameters->regularblocksorder || (rightresiduelosstype == h2o_loss)) && ((parameters->ionsfortheoreticalspectra[i] == r1oh_ion) || (parameters->ionsfortheoreticalspectra[i] == r2oh_ion)))
				) {
				generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectra[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, rightresiduelosstype, haslastblockartificial);
			}

		}

	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	if (parameters->generateisotopepattern) {
		generateFragmentIsotopePatterns(theoreticalpeaksrealsize, writedescription, isotopeformuladesctoid);
	}


	// search the theoretical peaks in the experimental peak list
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, unmatchedpeaksinmatchedpatterns, writedescription && !parameters->reportunmatchedtheoreticalpeaks);
	}


	// coverage of series
	map<eFragmentIonType, map<int, vector<int> > > series;
	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			if (j >= 0) {
				series[parameters->ionsfortheoreticalspectra[i]][parameters->neutrallossesfortheoreticalspectra[j]].resize(intcomposition.size());
			}
			else {
				series[parameters->ionsfortheoreticalspectra[i]][-1].resize(intcomposition.size());
			}

		}

	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0)
			&& (series.count(theoreticalpeaks[i].iontype) == 1)
			&& (series[theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}
	
	
	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->ionsfortheoreticalspectra, parameters->neutrallossesfortheoreticalspectra);

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid != -1) 
				&& (series[theoreticalpeaks[i].iontype][-1][theoreticalpeaks[i].seriesid] == 0)) {
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}*/


	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);
	}


	fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(theoreticalpeaksrealsize, linearpolyketide, unmatchedpeaksinmatchedpatterns, parameters->reportunmatchedtheoreticalpeaks, writedescription);


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		if (parameters->generateisotopepattern) {
			if (!parameters->reportunmatchedtheoreticalpeaks) {
				theoreticalpeaks.sortbyGroupId();
			}
			normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaksrealsize);
		}

		visualcoverage.clear();
		visualSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectra.size() > 0) {
			coveragebyseries = "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectra[0]][-1].size() - 1; k++) {
				coveragebyseries += "<td>" + to_string(k + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {
				for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
					tempseries.name = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name;
					if (j >= 0) {
						tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
					}
					tempseries.series.clear();
					losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];
					reportrow = false;
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectra[i]][losstype].size() - 1; k++) {
						if (series[parameters->ionsfortheoreticalspectra[i]][losstype][k] > 0) {
							reportrow = true;
							break;
						}
					}
					if (reportrow) {
						coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
					}
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectra[i]][losstype].size() - 1; k++) {
						if (reportrow) {
							coveragebyseries += "<td>" + to_string(series[parameters->ionsfortheoreticalspectra[i]][losstype][k]) + "</td>";
						}
						tempseries.series.push_back(series[parameters->ionsfortheoreticalspectra[i]][losstype][k]);
					}
					if (reportrow) {
						coveragebyseries += "</tr>";
					}
					visualcoverage.push_back(tempseries);
				}
			}
			coveragebyseries += "</table>";
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;

}


int cTheoreticalSpectrum::compareCyclicPolyketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {

	if (!candidate.checkKetideSequence(bricksdatabasewithcombinations, parameters->peptidetype, parameters->regularblocksorder)) {
		return -2;
	}

	return compareCyclic(sortedpeaklist, bricksdatabasewithcombinations, writedescription, sequencetag, searchedsequence, unmatchedpeaksinmatchedpatterns, isotopeformuladesctoid);

}


void cTheoreticalSpectrum::generateMSSpectrum(bool writedescription) {
	cPeak peak;
	cSummaryFormula formula;
	regex rx;

	string metalname;
	vector< vector<string> > metals;
	metals.resize(parameters->ionsfortheoreticalspectra.size());

	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {
		metalname = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name;
		for (int j = 0; j < (int)periodictablemap.getAdductElements().size(); j++) {
			rx = periodictablemap.getAdductElements()[j] + "[^a-z]";
			if (regex_search(metalname, rx)) {
				metals[i].push_back(periodictablemap.getAdductElements()[j]);
			}
		}
	}
	
	int theoreticalpeaksrealsize = 0;
	for (int i = 0; i < parameters->sequencedatabase.size(); i++) {

		peak.clear();
		peak.isotope = false;

		formula.clear();
		formula.setFormula(parameters->sequencedatabase[i].getSummaryFormula());

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				for (int m = 0; m < abs(parameters->precursorcharge); m++) {

					peak.mzratio = (double)parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].multiplier*formula.getMass() + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].massdifference;
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].positive) {
						peak.mzratio += (double)m*(H - e);
					}
					else {
						peak.mzratio -= (double)m*(H - e);
					}
					if (k >= 0) {
						peak.mzratio += parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].massdifference;
					}
					peak.mzratio = peak.mzratio / (double)(m + 1);

					if (writedescription) {
						peak.description = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].name.substr(0, parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].name.size() - 1) + " ";
						peak.description += to_string(m + 1);
						if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].positive) {
							peak.description += "+";
						}
						else {
							peak.description += "-";
						}
						if (k >= 0) {
							peak.description += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
						}
						peak.description += " " + parameters->sequencedatabase[i].getNameWithReferenceAsHTMLString() + " (" + parameters->sequencedatabase[i].getSummaryFormula() + "): ";
					}

					addPeakToList(peak, theoreticalpeaksrealsize);
					addMetalPeaks(peak, metals[j], theoreticalpeaksrealsize, m + 1, writedescription);

				}

			}
			
		}

	}

	theoreticalpeaks.resize(theoreticalpeaksrealsize);
}


void cTheoreticalSpectrum::generateFineMSSpectrum() {
	cSummaryFormula formula;
	string proton = "H+";
	string description;
	string tmp;

	int theoreticalpeaksrealsize = 0;
	int groupid = 0;

	cSequence sequence;
	string summary;
	string name;

	int seqdbsize = parameters->sequencedatabase.size();
	sequence.setPeptideType(other);
	sequence.setDecoy(true);

	for (int i = 0; i < seqdbsize; i++) {
		summary = parameters->sequencedatabase[i].getSummaryFormula() + "H";
		sequence.setSummaryFormula(summary);
		parameters->sequencedatabase.push_back(sequence);
	}

	for (int i = 0; i < parameters->sequencedatabase.size(); i++) {

		formula.clear();
		formula.setFormula(parameters->sequencedatabase[i].getSummaryFormula());

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectra.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				cSummaryFormula ionformula;

				for (int m = 0; m < (double)parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].multiplier; m++) {
					tmp = formula.getSummary();
					ionformula.addFormula(tmp);
				}
				ionformula.addFormula(parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].summary);

				if (k >= 0) {
					ionformula.addFormula(parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary, true);
				}

				for (int n = 0; n < abs(parameters->precursorcharge); n++) {

					cSummaryFormula chargedformula;
					chargedformula = ionformula;

					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].positive) {
						for (int p = 0; p < n; p++) {
							chargedformula.addFormula(proton);
						}
					}
					else {
						for (int p = 0; p < n; p++) {
							chargedformula.addFormula(proton, true);
						}
					}

					description = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].name.substr(0, parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].name.size() - 1) + " ";
					description += to_string(n + 1);
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].positive) {
						description += "+";
					}
					else {
						description += "-";
					}
					if (k >= 0) {
						description += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
					}
					description += " " + parameters->sequencedatabase[i].getNameWithReferenceAsHTMLString() + " (";

					cPeaksList isotopepattern = chargedformula.getIsotopePattern(parameters->fwhm, n + 1, parameters->iondefinitions[parameters->ionsfortheoreticalspectra[j]].positive, true);
					for (int p = 0; p < isotopepattern.size(); p++) {
						isotopepattern[p].description = description + isotopepattern[p].description + "): ";
						isotopepattern[p].groupid = groupid;
						isotopepattern[p].decoy = parameters->sequencedatabase[i].isDecoy();
						addPeakToList(isotopepattern[p], theoreticalpeaksrealsize);
					}

					groupid++;
				}

			}
			
		}

	}

	theoreticalpeaks.resize(theoreticalpeaksrealsize);
}


void cTheoreticalSpectrum::compareMSSpectrum(cPeaksList& peaklist, cTheoreticalSpectrum& tsfull, cPeaksList& unmatchedpeaksinmatchedpatterns) {
	experimentalpeaks = peaklist;
	peaklist.clear();
	experimentalpeaks.sortbyMass();

	cPeaksList* tsfullpeaklist = tsfull.getTheoreticalPeaks();

	experimentalmatches.clear();
	searchForPeakPairs(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, parameters->fragmentmasserrortolerance);

	unmatchedpeaksinmatchedpatterns.clear();
	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
	}
	else {
		removeUnmatchedIsotopePatterns(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, unmatchedpeaksinmatchedpatterns, true);
		targetscores.clear();
		decoyscores.clear();
		calculateEnvelopeScores(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		removeDecoyPeakMatches(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);		
	}

	if (parameters->reportunmatchedtheoreticalpeaks && !parameters->generateisotopepattern) {
		theoreticalpeaks = *tsfullpeaklist;

		// fill annotations of experimental peaks
		for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
			for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);

				(*tsfullpeaklist)[*it].matched = 0;
				(*tsfullpeaklist)[*it].matchedid = -1;
			}
		}
	}
	else {
		theoreticalpeaks.clear();
		vector<set<int> > updatedexperimentalmatches;
		updatedexperimentalmatches.resize(experimentalmatches.size());

		// fill annotations of experimental peaks and remove unmatched theoretical peaks
		for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
			for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
				updatedexperimentalmatches[i].insert((int)theoreticalpeaks.size());

				cPeak matchedpeak;
				matchedpeak = (*tsfullpeaklist)[*it];

				matchedpeak.matchedmz = experimentalpeaks[i].mzratio;
				matchedpeak.matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				matchedpeak.matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				matchedpeak.matchedppm = ppmError(experimentalpeaks[i].mzratio, matchedpeak.mzratio);
				theoreticalpeaks.add(matchedpeak);

				(*tsfullpeaklist)[*it].matched = 0;
				(*tsfullpeaklist)[*it].matchedid = -1;
			}
			experimentalpeaks[i].matched = (int)updatedexperimentalmatches[i].size();
		}

		experimentalmatches = updatedexperimentalmatches;
	}
}


void cTheoreticalSpectrum::finalizeMSSpectrum(cPeaksList& unmatchedpeaksinmatchedpatterns, bool writedescription) {
	if (parameters->generateisotopepattern) {
		theoreticalpeaks.attach(unmatchedpeaksinmatchedpatterns);
	}
	
	theoreticalpeaks.fillOrderIDs();

	if (parameters->generateisotopepattern) {
		theoreticalpeaks.sortbyGroupId();
		normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaks.size());
	}

	computeStatistics(writedescription);

	theoreticalpeaks.sortbyMass();
}


int cTheoreticalSpectrum::getNumberOfPeaks() {
	return theoreticalpeaks.size();
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaks() const {
	return experimentalpeaksmatched;
}


int cTheoreticalSpectrum::getNumberOfScrambledPeaks() {
	return scrambledpeaksmatched;
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaks(eFragmentIonType iontype, int neutrallosstype) const {
	if (matchedions.count(iontype) == 1) {
		if (matchedions.at(iontype).count(neutrallosstype) == 1) {
			return matchedions.at(iontype).at(neutrallosstype);
		}
	}
	return 0;
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaksB() const {
	int value = 0;

	if (matchedions.count(b_ion) == 1) {
		if (matchedions.at(b_ion).count(-1) == 1) {
			value += matchedions.at(b_ion).at(-1);
		}
	}

	// LB
	if (matchedions.count(l1h_ion) == 1) {
		if (matchedions.at(l1h_ion).count(-1) == 1) {
			value += matchedions.at(l1h_ion).at(-1);
		}
	}

	// RB
	if (matchedions.count(r1h_ion) == 1) {
		if (matchedions.at(r1h_ion).count(-1) == 1) {
			value += matchedions.at(r1h_ion).at(-1);
		}
	}

	return value;
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaksY() const {
	int value = 0;

	if (matchedions.count(y_ion) == 1) {
		if (matchedions.at(y_ion).count(-1) == 1) {
			value += matchedions.at(y_ion).at(-1);
		}
	}

	// LY
	if (matchedions.count(l2oh_ion) == 1) {
		if (matchedions.at(l2oh_ion).count(-1) == 1) {
			value += matchedions.at(l2oh_ion).at(-1);
		}
	}

	// RY
	if (matchedions.count(r2oh_ion) == 1) {
		if (matchedions.at(r2oh_ion).count(-1) == 1) {
			value += matchedions.at(r2oh_ion).at(-1);
		}
	}

	return value;
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaksYB() const {
	return getNumberOfMatchedPeaksB() + getNumberOfMatchedPeaksY();
}


double cTheoreticalSpectrum::getRatioOfMatchedPeaks() {
	return experimentalpeaksmatchedratio;
}


void cTheoreticalSpectrum::generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, int neutrallosstype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation, eResidueLossType leftresiduelosstype, bool hasfirstblockartificial) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	map<string, int> atoms, tempmap;
	atoms.clear();

	bool disablesummary = false;
	if ((parameters->mode == denovoengine) && (parameters->blindedges == 2) && bricksdatabase[intcomposition[0] - 1].isArtificial()) {
		disablesummary = true;
	}

	peak.mzratio = parameters->iondefinitions[fragmentiontype].massdifference;
	if (neutrallosstype >= 0) {
		peak.mzratio += parameters->neutrallossesdefinitions[neutrallosstype].massdifference;
	}
	if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
		mergeMaps(parameters->iondefinitions[fragmentiontype].summarymap, atoms);
		if (neutrallosstype >= 0) {
			mergeMaps(parameters->neutrallossesdefinitions[neutrallosstype].summarymap, atoms);
		}
	}

	if ((peptidetype == linear) || (peptidetype == linearpolyketide)) {
		peak.mzratio += searchedmodifications[candidate.getStartModifID()].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(searchedmodifications[candidate.getStartModifID()].summary, atoms);
		}
	}
	if ((peptidetype == branched) || (peptidetype == branchcyclic)) {
		peak.mzratio += searchedmodifications[trotation->startmodifID].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(searchedmodifications[trotation->startmodifID].summary, atoms);
		}
	}

	peak.iontype = fragmentiontype;
	peak.neutrallosstype = neutrallosstype;
	peak.scrambled = false;
	peak.isotope = false;

	switch (peptidetype) {
		case linear:
			peak.rotationid = rotationid;
			break;
		case cyclic:
			peak.rotationid = rotationid;
			break;
		case branched:
			peak.rotationid = trotation->id;
			break;
		case branchcyclic:
			peak.rotationid = rotationid*6 + trotation->id; // to do - potential bug
			break;
		case linearpolyketide:
			peak.rotationid = rotationid;
			break;
		case cyclicpolyketide:
			peak.rotationid = rotationid;
			break;
		case other:
			break;
		default:
			break;
	}

	string prefixstr;
	if (writedescription) {
		if ((peptidetype == cyclic) || (peptidetype == cyclicpolyketide)) {
			prefixstr = to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
		}
		if (peptidetype == branched) {
			prefixstr = to_string(peak.rotationid + 1) + "_";
		}
		if (peptidetype == branchcyclic) {
			prefixstr = to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_" + to_string(trotation->id + 1) + "_";
		}
	}

	for (int i = 0; i < (int)intcomposition.size() - 1; i++) {
		peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();

		if ((parameters->mode == denovoengine) && (parameters->blindedges == 2) && bricksdatabase[intcomposition[i] - 1].isArtificial()) {
			peak.formula.clear();
			atoms.clear();
			disablesummary = true;
		}

		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			mergeMaps(bricksdatabase[intcomposition[i] - 1].getSummaryMap(), atoms);
		}

		if ((peptidetype == branched) || (peptidetype == branchcyclic)) {		
			if (i == trotation->middlebranchstart) {
				peak.mzratio += searchedmodifications[trotation->middlemodifID].massdifference;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					addStringFormulaToMap(searchedmodifications[trotation->middlemodifID].summary, atoms);
				}
			}

			if ((i >= trotation->middlebranchstart) && (i < trotation->middlebranchend)) {
				continue;
			}

			// redundant short n-term fragments are not generated
			//if (((trotation->id == 3) || ((trotation->id == 4))) && (i < trotation->middlebranchstart)) {
			//	continue;
			//}

			// redundant long n-term fragments are not generated
			//if ((trotation->id == 1) && (i >= trotation->middlebranchend)) {
			//	continue;
			//}
		}

		if (regularblocksorder && (peptidetype == linearpolyketide)) {
			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (fragmentiontype == l1h_ion) && (i % 2 == 0)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (fragmentiontype == l2h_ion) && (i % 2 == 1)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (fragmentiontype == l1oh_ion) && (i % 2 == 1)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (fragmentiontype == l2oh_ion) && (i % 2 == 0)) {
				continue;
			}	
		}

		if (peptidetype == linearpolyketide) {
			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && ((fragmentiontype == l1oh_ion) || (fragmentiontype == l2oh_ion))) {
				continue;
			}

			if (!hasfirstblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->iondefinitions[candidate.getStartIonType()].nterminal && ((fragmentiontype == l1oh_ion) || (fragmentiontype == l2oh_ion))) {
				continue;
			}

			if (!hasfirstblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->iondefinitions[candidate.getStartIonType()].cterminal && ((fragmentiontype == l1h_ion) || (fragmentiontype == l2h_ion))) {
				continue;
			}
		}
		
		if (regularblocksorder && (peptidetype == cyclicpolyketide)) {

			//if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (fragmentiontype == l0h_ion)) { // b+2H ion is generated instead of b-2H
			//	continue;
			//}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (fragmentiontype == l1h_ion) && (i % 2 == 0)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (fragmentiontype == l2h_ion) && (i % 2 == 1)) {
				continue;
			}

			//if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (fragmentiontype == l0h_ion) && (i % 2 == 1)) {
			//	continue;
			//}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (fragmentiontype == l1h_ion) && (i % 2 == 0)) { // ok - even numbers of blocks => always b-ion
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (fragmentiontype == l2h_ion)) { // b-2H ion is generated instead of b+2H
				continue;
			}	
		}

		peak.seriesid = i;

		if (writedescription) {

			peak.description = "";
			if ((peptidetype == cyclic) || (peptidetype == cyclicpolyketide) || (peptidetype == branched) || (peptidetype == branchcyclic)) {
				peak.description += prefixstr;
			}
			if ((peptidetype == linearpolyketide) || (peptidetype == cyclicpolyketide)) {
				peak.description += parameters->iondefinitions[fragmentiontype].name.substr(0, 2) + to_string(i + 1);
				if (parameters->iondefinitions[fragmentiontype].name.size() > 2) {
					peak.description += parameters->iondefinitions[fragmentiontype].name.substr(2, parameters->iondefinitions[fragmentiontype].name.length() - 2);
				}
			}
			else {
				peak.description += parameters->iondefinitions[fragmentiontype].name.substr(0, 1) + to_string(i + 1);
				if (parameters->iondefinitions[fragmentiontype].name.size() > 1) {
					peak.description += parameters->iondefinitions[fragmentiontype].name.substr(1, parameters->iondefinitions[fragmentiontype].name.length() - 1);
				}
			}
			if (peak.neutrallosstype >= 0) {
				peak.description += "-" + parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
			}
			addAdductToDescription(peak.description, parameters->metaladducts);
			peak.description += ": ";
			for (int j = 0; j <= i; j++) {
				peak.description += "[" + bricksdatabase[intcomposition[j] - 1].getAcronymsAsString() + "]";
				if (j < i) {
					peak.description += '-';
				}
			}

		}

		if (peak.mzratio > 0) {

			tempratio = peak.mzratio;		
			if (writedescription) {
				tempdescription = peak.description;
			}
			for (int j = 1; j <= abs(maxcharge); j++) {

				peak.mzratio = charge(uncharge(tempratio, 1), (parameters->precursorcharge > 0)?j:-j);
				peak.charge = (parameters->precursorcharge > 0)?j:-j;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					tempmap = atoms;
					rechargeMap(peak.charge, tempmap);
					peak.formula.setFromMap(tempmap, false);
				}
				if (writedescription) {
					if (parameters->precursorcharge > 0) {
						peak.description = to_string(j) + "+ " + tempdescription;
					}
					else {
						peak.description = to_string(j) + "- " + tempdescription;
					}
				}

				if (writedescription) {
					if (peak.formula.hasAllElementsPositive()) {
						if (parameters->reportunmatchedtheoreticalpeaks || searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
							addPeakToList(peak, peaklistrealsize);
						}
						if (!parameters->generateisotopepattern) {
							addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, j, writedescription);
						}
					}
				}
				else {
					if (parameters->generateisotopepattern) {
						if (peak.formula.hasAllElementsPositive()) {
							if (searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
								addPeakToList(peak, peaklistrealsize);
							}
						}
					}
					else {
						if (searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
							addPeakToList(peak, peaklistrealsize);
						}
						addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, j, writedescription);
					}
				}

			}
			peak.mzratio = tempratio;
			if (writedescription) {
				peak.description = tempdescription;
			}

		}

	}

}


void cTheoreticalSpectrum::generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, int neutrallosstype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation, eResidueLossType rightresiduelosstype, bool haslastblockartificial) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	map<string, int> atoms, tempmap;
	atoms.clear();

	bool disablesummary = false;
	if ((parameters->mode == denovoengine) && (parameters->blindedges == 2) && bricksdatabase[intcomposition[intcomposition.size() - 1] - 1].isArtificial()) {
		disablesummary = true;
	}

	peak.mzratio = parameters->iondefinitions[fragmentiontype].massdifference;
	if (neutrallosstype >= 0) {
		peak.mzratio += parameters->neutrallossesdefinitions[neutrallosstype].massdifference;
	}
	if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
		mergeMaps(parameters->iondefinitions[fragmentiontype].summarymap, atoms);
		if (neutrallosstype >= 0) {
			mergeMaps(parameters->neutrallossesdefinitions[neutrallosstype].summarymap, atoms);
		}
	}

	if ((peptidetype == linear) || (peptidetype == linearpolyketide)) {
		peak.mzratio += searchedmodifications[candidate.getEndModifID()].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(searchedmodifications[candidate.getEndModifID()].summary, atoms);
		}
	}
	if ((peptidetype == branched) || (peptidetype == branchcyclic)) {
		peak.mzratio += searchedmodifications[trotation->endmodifID].massdifference;
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			addStringFormulaToMap(searchedmodifications[trotation->endmodifID].summary, atoms);
		}
	}

	peak.iontype = fragmentiontype;
	peak.neutrallosstype = neutrallosstype;
	peak.scrambled = false;
	peak.isotope = false;

	switch (peptidetype) {
		case linear:
			peak.rotationid = rotationid;
			break;
		case cyclic:
			peak.rotationid = rotationid;
			break;
		case branched:
			peak.rotationid = trotation->id;
			break;
		case branchcyclic:
			peak.rotationid = rotationid*6 + trotation->id; // to do - potential bug
			break;
		case linearpolyketide:
			peak.rotationid = rotationid;
			break;
		case cyclicpolyketide:
			peak.rotationid = rotationid;
			break;
		case other:
			break;
		default:
			break;
	}

	string prefixstr;
	if (writedescription) {
		if (peptidetype == cyclicpolyketide) {
			prefixstr = to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
		}
		//if (peptidetype == cyclic) {
		//	prefixstr = to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
		//}
		if (peptidetype == branched) {
			prefixstr = to_string(peak.rotationid + 1) + "_";
		}
		if (peptidetype == branchcyclic) {
			prefixstr = to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_" + to_string(trotation->id + 1) + "_";
		}
	}

	int order = -1;
	for (int i = (int)intcomposition.size() - 1; i > 0; i--) {
		peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();

		if ((parameters->mode == denovoengine) && (parameters->blindedges == 2) && bricksdatabase[intcomposition[i] - 1].isArtificial()) {
			peak.formula.clear();
			atoms.clear();
			disablesummary = true;
		}

		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			mergeMaps(bricksdatabase[intcomposition[i] - 1].getSummaryMap(), atoms);
		}
		order++;

		if ((peptidetype == branched) || (peptidetype == branchcyclic)) {		
			if (i == trotation->middlebranchend) {
				peak.mzratio += searchedmodifications[trotation->middlemodifID].massdifference;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					addStringFormulaToMap(searchedmodifications[trotation->middlemodifID].summary, atoms);
				}
			}

			if ((i > trotation->middlebranchstart) && (i <= trotation->middlebranchend)) {
				continue;
			}

			// redundant short c-term fragments are not generated
			//if (((trotation->id == 1) || (trotation->id == 5)) && (i > trotation->middlebranchend)) {
			//	continue;
			//}

			// redundant long c-term fragments are not generated
			//if ((trotation->id == 3) && (i <= trotation->middlebranchstart)) {
			//	continue;
			//}
		}

		if (regularblocksorder && (peptidetype == linearpolyketide)) {
			if (!haslastblockartificial && (rightresiduelosstype == h2_loss) && (fragmentiontype == r1h_ion) && (order % 2 == 0)) {
				continue;
			}

			if (!haslastblockartificial && (rightresiduelosstype == h2_loss) && (fragmentiontype == r2h_ion) && (order % 2 == 1)) {
				continue;
			}

			if (!haslastblockartificial && (rightresiduelosstype == h2o_loss) && (fragmentiontype == r1oh_ion) && (order % 2 == 1)) {
				continue;
			}

			if (!haslastblockartificial && (rightresiduelosstype == h2o_loss) && (fragmentiontype == r2oh_ion) && (order % 2 == 0)) {
				continue;
			}		
		}

		if (peptidetype == linearpolyketide) {
			if (!haslastblockartificial && (rightresiduelosstype == h2_loss) && ((fragmentiontype == r1oh_ion) || (fragmentiontype == r2oh_ion))) {
				continue;
			}

			//if (!haslastblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->iondefinitions[candidate.getStartIonType()].nterminal && ((fragmentiontype == r1oh_ion) || (fragmentiontype == r2oh_ion))) {
			//	continue;
			//}

			//if (!haslastblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->iondefinitions[candidate.getStartIonType()].cterminal && ((fragmentiontype == r1h_ion) || (fragmentiontype == r2h_ion))) {
			//	continue;
			//}
		}

		peak.seriesid = (int)intcomposition.size() - i - 1;

		if (writedescription) {

			peak.description = "";
			if ((peptidetype == cyclicpolyketide) /* || (peptidetype == cyclic) */ || (peptidetype == branched) || (peptidetype == branchcyclic)) {
				peak.description += prefixstr;
			}
			if ((peptidetype == linearpolyketide) || (peptidetype == cyclicpolyketide)) {
				peak.description += parameters->iondefinitions[fragmentiontype].name.substr(0, 2) + to_string((int)intcomposition.size() - i);
				if (parameters->iondefinitions[fragmentiontype].name.size() > 2) {
					peak.description += parameters->iondefinitions[fragmentiontype].name.substr(2, parameters->iondefinitions[fragmentiontype].name.length() - 2);
				}
			}
			else {
				peak.description += parameters->iondefinitions[fragmentiontype].name.substr(0, 1) + to_string((int)intcomposition.size() - i);
				if (parameters->iondefinitions[fragmentiontype].name.size() > 1) {
					peak.description += parameters->iondefinitions[fragmentiontype].name.substr(1, parameters->iondefinitions[fragmentiontype].name.length() - 1);
				}
			}
			if (peak.neutrallosstype >= 0) {
				peak.description += "-" + parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
			}
			addAdductToDescription(peak.description, parameters->metaladducts);
			peak.description += ": ";
			for (int j = (int)intcomposition.size() - 1; j >= i; j--) {
				peak.description += "[" + bricksdatabase[intcomposition[j] - 1].getAcronymsAsString() + "]";
				if (j > i) {
					peak.description += '-';
				}
			}

		}

		if (peak.mzratio > 0) {

			tempratio = peak.mzratio;
			if (writedescription) {
				tempdescription = peak.description;
			}
			for (int j = 1; j <= abs(maxcharge); j++) {

				peak.mzratio = charge(uncharge(tempratio, 1), (parameters->precursorcharge > 0)?j:-j);
				peak.charge = (parameters->precursorcharge > 0)?j:-j;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					tempmap = atoms;
					rechargeMap(peak.charge, tempmap);
					peak.formula.setFromMap(tempmap, false);
				}
				if (writedescription) {
					if (parameters->precursorcharge > 0) {
						peak.description = to_string(j) + "+ " + tempdescription;
					}
					else {
						peak.description = to_string(j) + "- " + tempdescription;
					}
				}
				
				if (writedescription) {
					if (peak.formula.hasAllElementsPositive()) {
						if (parameters->reportunmatchedtheoreticalpeaks || searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
							addPeakToList(peak, peaklistrealsize);
						}
						if (!parameters->generateisotopepattern) {
							addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, j, writedescription);
						}
					}
				}
				else {
					if (parameters->generateisotopepattern) {
						if (peak.formula.hasAllElementsPositive()) {
							if (searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
								addPeakToList(peak, peaklistrealsize);
							}
						}
					}
					else {
						if (searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
							addPeakToList(peak, peaklistrealsize);
						}
						addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, j, writedescription);
					}
				}

			}
			peak.mzratio = tempratio;
			if (writedescription) {
				peak.description = tempdescription;
			}

		}

	}

}


void cTheoreticalSpectrum::sortByMass(int limit) {
	theoreticalpeaks.sortbyMass(limit);
}


void cTheoreticalSpectrum::resizePeakList(int size) {
	theoreticalpeaks.resize(size);
}


double cTheoreticalSpectrum::getSumOfRelativeIntensities() const {
	return sumofrelativeintensities;
}


cPeak& cTheoreticalSpectrum::operator[](int position) {
	return theoreticalpeaks[position];
}


cPeaksList& cTheoreticalSpectrum::getExperimentalSpectrum() {
	return experimentalpeaks;
}


set<int>& cTheoreticalSpectrum::getExperimentalMatches(int peakid) {
	return experimentalmatches[peakid];
}


string cTheoreticalSpectrum::getCoverageBySeries() {
	return coveragebyseries;
}


int cTheoreticalSpectrum::getUnmatchedExperimentalPeaksCount() {
	return unmatchedexperimentalpeakscount;
}


cPeaksList* cTheoreticalSpectrum::getTheoreticalPeaks() {
	return &theoreticalpeaks;
}


bool cTheoreticalSpectrum::isValid() {
	return valid;
}


int cTheoreticalSpectrum::getValidPosition() {
	return validposition;
}


int cTheoreticalSpectrum::getReverseValidPosition() {
	return reversevalidposition;
}


void cTheoreticalSpectrum::setValidSequence(regex& searchedsequence) {
	valid = false;

	if (parameters->searchedsequence.size() == 0) {
		return;
	}

	cCandidate revertedcandidate;
	vector<string> rotations;
	vector<TRotationInfo> trotations;
	vector<cCandidate> branchcyclicrotations;
	vector<vector<TRotationInfo> > trotationsofbranchcyclicrotations;
	int r, numberofbricks;

	switch (parameters->peptidetype) {
		case linear:
			if (regex_search(candidate.getComposition(), searchedsequence) &&
				(parameters->searchedmodifications[candidate.getStartModifID()].name.compare(parameters->searchedsequenceNtermmodif) == 0) &&
				(parameters->searchedmodifications[candidate.getEndModifID()].name.compare(parameters->searchedsequenceCtermmodif) == 0)) {
				valid = true;
			}
			break;
		case cyclic:
		case cyclicpolyketide:
			candidate.getRotations(rotations, true);
			r = (int)rotations.size() / 2;

			for (int i = 0; i < (int)rotations.size(); i++) {
				if (regex_search(rotations[i], searchedsequence)) {
					validposition = i;

					if (validposition == 0) {
						reversevalidposition = r;
					}
					else if (validposition == r) {
						reversevalidposition = 0;
					}
					else {
						reversevalidposition = 2 * r - validposition;
					}

					break;
				}
			}

			if (validposition != -1) {
				valid = true;
			}
			break;
		case branched:
			candidate.getPermutationsOfBranches(trotations);
			for (int i = 0; i < (int)trotations.size(); i++) {
				if (regex_search(trotations[i].tcomposition, searchedsequence) &&
					(parameters->searchedmodifications[trotations[i].startmodifID].name.compare(parameters->searchedsequenceNtermmodif) == 0) &&
					(parameters->searchedmodifications[trotations[i].middlemodifID].name.compare(parameters->searchedsequenceTmodif) == 0) &&
					(parameters->searchedmodifications[trotations[i].endmodifID].name.compare(parameters->searchedsequenceCtermmodif) == 0)) {
					valid = true;
					break;
				}
			}
			break;
		case branchcyclic:
			// normalize the candidate
			candidate.getBranchCyclicRotations(branchcyclicrotations, false);
			numberofbricks = getNumberOfBricks(candidate.getComposition());
			for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
				if (branchcyclicrotations[i].getBranchEnd() == numberofbricks - 1) {
					vector<string> v;
					v.push_back(branchcyclicrotations[i].getComposition());
					string name = candidate.getName();
					cSummaryFormula summary = candidate.getSummaryFormula();
					vector<nodeEdge> cpath = candidate.getPath();
					candidate.setCandidate(v, cpath, candidate.getStartIonType(), candidate.getStartModifID(), candidate.getEndModifID(), candidate.getMiddleModifID(), branchcyclicrotations[i].getBranchStart(), branchcyclicrotations[i].getBranchEnd());
					candidate.setName(name);
					candidate.setSummaryFormula(summary);
					break;
				}
			}

			// get branch-cyclic rotations
			candidate.getBranchCyclicRotations(branchcyclicrotations, true);

			// get T-permutations of branch-cyclic rotations
			trotationsofbranchcyclicrotations.resize(branchcyclicrotations.size());
			for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
				branchcyclicrotations[i].getPermutationsOfBranches(trotationsofbranchcyclicrotations[i]);
			}

			r = (int)branchcyclicrotations.size() / 2;
			
			for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
				if (regex_search(branchcyclicrotations[i].getComposition(), searchedsequence)) {
					validposition = i;

					if (validposition == 0) {
						reversevalidposition = r;
					}
					else if (validposition == r) {
						reversevalidposition = 0;
					}
					else {
						reversevalidposition = 2 * r - validposition;
					}
				}

				for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {
					if (regex_search(trotationsofbranchcyclicrotations[i][j].tcomposition, searchedsequence) &&
						(parameters->searchedmodifications[trotationsofbranchcyclicrotations[i][j].middlemodifID].name.compare(parameters->searchedsequenceTmodif) == 0)) {
						valid = true;
						break;
					}
				}
			}
			break;
		case linearpolyketide:
			revertedcandidate = candidate;
			revertedcandidate.revertComposition();
			if ((regex_search(candidate.getComposition(), searchedsequence) &&
				(parameters->searchedmodifications[candidate.getStartModifID()].name.compare(parameters->searchedsequenceNtermmodif) == 0) &&
				(parameters->searchedmodifications[candidate.getEndModifID()].name.compare(parameters->searchedsequenceCtermmodif) == 0))
				||
				(regex_search(revertedcandidate.getComposition(), searchedsequence) &&
				(parameters->searchedmodifications[revertedcandidate.getStartModifID()].name.compare(parameters->searchedsequenceNtermmodif) == 0) &&
				(parameters->searchedmodifications[revertedcandidate.getEndModifID()].name.compare(parameters->searchedsequenceCtermmodif) == 0))) {
				valid = true;
			}
			break;
		case other:
			break;
		default:
			break;
	}
}


vector<visualSeries>& cTheoreticalSpectrum::getVisualCoverage() {
	return visualcoverage;
}


int cTheoreticalSpectrum::getNumberOfCompletedSeries() {
	return seriescompleted;
}


void cTheoreticalSpectrum::setNumberOfCompletedSeries(int numberofcompletedseries) {
	seriescompleted = numberofcompletedseries;
}


void cTheoreticalSpectrum::setPathId(int pathid) {
	this->pathid = pathid;
}


int cTheoreticalSpectrum::getPathId() const {
	return pathid;
}


void cTheoreticalSpectrum::setParameters(cParameters* parameters) {
	this->parameters = parameters;
}


double cTheoreticalSpectrum::getTargetPatternScore(int groupid) {
	return (targetscores.count(groupid) == 1) ? targetscores[groupid] : DBL_MAX;
}


double cTheoreticalSpectrum::getTargetPatternFDR(int groupid) {
	return (fdrscores.count(groupid) == 1) ? fdrscores[groupid] : DBL_MAX;
}


map<int, double>& cTheoreticalSpectrum::getTargetScores() {
	return targetscores;
}


map<int, double>& cTheoreticalSpectrum::getDecoyScores() {
	return decoyscores;
}


void cTheoreticalSpectrum::setFDRs(vector<double>& targetscoresvector, vector<double>& fdrs, cPeaksList& unmatchedpeaksinmatchedpatterns) {
	if (targetscoresvector.empty() || (targetscoresvector.size() != fdrs.size())) {
		return;
	}

	int size;
	int position;

	for (auto it = targetscores.begin(); it != targetscores.end(); ++it) {
		position = getNumberOfScoreHits(targetscoresvector, it->second) - 1;
		fdrscores[it->first] = fdrs[position];
	}

	// clear matches of peaks exceeding a FDR threshold
	cPeaksList updatedtheoreticalpeaks;
	vector<set<int> > updatedexperimentalmatches;
	updatedexperimentalmatches.resize(experimentalmatches.size());
	set<int> updatedgroupids;

	// matched peaks
	size = (int)experimentalpeaks.size();
	for (int i = 0; i < size; i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			if ((fdrscores.count(theoreticalpeaks[*it].groupid) == 1) /*&& (fdrscores[theoreticalpeaks[*it].groupid] <= parameters->maximumfdr)*/) {
				updatedexperimentalmatches[i].insert((int)updatedtheoreticalpeaks.size());
				updatedtheoreticalpeaks.add(theoreticalpeaks[*it]);
				updatedgroupids.insert(theoreticalpeaks[*it].groupid);
			}
		}
		experimentalpeaks[i].matched = (int)updatedexperimentalmatches[i].size();
	}

	theoreticalpeaks = updatedtheoreticalpeaks;
	experimentalmatches = updatedexperimentalmatches;

	// unmatched peaks in matched patterns
	if (!parameters->reportunmatchedtheoreticalpeaks) {
		cPeaksList updatedunmatchedpeaks;
		size = (int)unmatchedpeaksinmatchedpatterns.size();
		for (int i = 0; i < size; i++) {
			if (updatedgroupids.count(unmatchedpeaksinmatchedpatterns[i].groupid) == 1) {
				updatedunmatchedpeaks.add(unmatchedpeaksinmatchedpatterns[i]);
			}
		}
		unmatchedpeaksinmatchedpatterns = updatedunmatchedpeaks;
	}
}


void cTheoreticalSpectrum::store(ofstream& os) {
	int size, size2;

	theoreticalpeaks.store(os);
	experimentalpeaks.store(os);
	candidate.store(os);

	os.write((char *)&experimentalpeaksmatched, sizeof(int));

	size = (int)experimentalmatches.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < size; i++) {
		size2 = (int)experimentalmatches[i].size();
		os.write((char *)&size2, sizeof(int));
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			os.write((char *)&(*it), sizeof(int));
		}
	}

	os.write((char *)&scrambledpeaksmatched, sizeof(int));

	size = (int)matchedions.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = matchedions.begin(); it != matchedions.end(); ++it) {
		os.write((char *)&it->first, sizeof(eFragmentIonType));
		
		size = (int)it->second.size();
		os.write((char *)&size, sizeof(int));
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
			os.write((char *)&it2->first, sizeof(int));
			os.write((char *)&it2->second, sizeof(int));
		}
	}

	os.write((char *)&peakstested, sizeof(int));
	os.write((char *)&experimentalpeaksmatchedratio, sizeof(double));

	os.write((char *)&unmatchedexperimentalpeakscount, sizeof(int));
	storeString(coveragebyseries, os);

	os.write((char *)&valid, sizeof(bool));
	os.write((char *)&sumofrelativeintensities, sizeof(double));

	size = (int)visualcoverage.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)visualcoverage.size(); i++) {
		visualcoverage[i].store(os);
	}

	os.write((char *)&validposition, sizeof(int));
	os.write((char *)&reversevalidposition, sizeof(int));
	os.write((char *)&seriescompleted, sizeof(int));
	os.write((char *)&pathid, sizeof(int));

	size = (int)targetscores.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = targetscores.begin(); it != targetscores.end(); ++it) {
		os.write((char *)&it->first, sizeof(int));
		os.write((char *)&it->second, sizeof(double));
	}

	size = (int)decoyscores.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = decoyscores.begin(); it != decoyscores.end(); ++it) {
		os.write((char *)&it->first, sizeof(int));
		os.write((char *)&it->second, sizeof(double));
	}

	size = (int)fdrscores.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = fdrscores.begin(); it != fdrscores.end(); ++it) {
		os.write((char *)&it->first, sizeof(int));
		os.write((char *)&it->second, sizeof(double));
	}
}


void cTheoreticalSpectrum::load(ifstream& is) {
	int size, size2;
	eFragmentIonType iontype;
	int losstype;
	int number;
	double value;
	set<int> setint;

	parameters = 0;

	theoreticalpeaks.load(is);
	experimentalpeaks.load(is);
	candidate.load(is);

	is.read((char *)&experimentalpeaksmatched, sizeof(int));

	is.read((char *)&size, sizeof(int));
	experimentalmatches.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&size2, sizeof(int));
		setint.clear();
		for (int j = 0; j < size2; j++) {
			is.read((char *)&number, sizeof(int));
			setint.insert(number);
		}
		experimentalmatches.push_back(setint);
	}

	is.read((char *)&scrambledpeaksmatched, sizeof(int));

	is.read((char *)&size, sizeof(int));
	matchedions.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&iontype, sizeof(eFragmentIonType));

		is.read((char *)&size2, sizeof(int));
		for (int j = 0; j < size2; j++) {
			is.read((char *)&losstype, sizeof(int));
			is.read((char *)&number, sizeof(int));
			matchedions[iontype][losstype] = number;
		}
	}

	is.read((char *)&peakstested, sizeof(int));
	is.read((char *)&experimentalpeaksmatchedratio, sizeof(double));

	is.read((char *)&unmatchedexperimentalpeakscount, sizeof(int));
	loadString(coveragebyseries, is);

	is.read((char *)&valid, sizeof(bool));
	is.read((char *)&sumofrelativeintensities, sizeof(double));

	is.read((char *)&size, sizeof(int));
	visualcoverage.resize(size);
	for (int i = 0; i < (int)visualcoverage.size(); i++) {
		visualcoverage[i].load(is);
	}

	is.read((char *)&validposition, sizeof(int));
	is.read((char *)&reversevalidposition, sizeof(int));
	is.read((char *)&seriescompleted, sizeof(int));
	is.read((char *)&pathid, sizeof(int));

	is.read((char *)&size, sizeof(int));
	targetscores.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&number, sizeof(int));
		is.read((char *)&value, sizeof(double));
		targetscores[number] = value;
	}

	is.read((char *)&size, sizeof(int));
	decoyscores.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&number, sizeof(int));
		is.read((char *)&value, sizeof(double));
		decoyscores[number] = value;
	}

	is.read((char *)&size, sizeof(int));
	fdrscores.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&number, sizeof(int));
		is.read((char *)&value, sizeof(double));
		fdrscores[number] = value;
	}
}

