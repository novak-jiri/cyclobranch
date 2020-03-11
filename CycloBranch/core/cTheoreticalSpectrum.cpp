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


void matchedSeries::store(ofstream& os) {
	int size;

	size = (int)series.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)series.size(); i++) {
		os.write((char *)&series[i], sizeof(int));
	}

	storeString(name, os);
}


void matchedSeries::load(ifstream& is) {
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


void cTheoreticalSpectrum::computeStatistics(bool writedescription) {
	double sumofallintensities = 0;
	
	experimentalpeaksmatched = 0;
	scrambledpeaksmatched = 0;
	sumofrelativeintensities = 0;

	if (writedescription) {
		unmatchedexperimentalpeakscount = 0;
	}

	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		sumofallintensities += experimentalpeaks[i].relativeintensity;

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

	weightedpeaksratio = 0;
	if (experimentalpeaks.size() > 0) {
		experimentalpeaksmatchedratio = ((double)experimentalpeaksmatched) / ((double)experimentalpeaks.size());
		if (sumofallintensities > 0) {
			weightedpeaksratio = sumofrelativeintensities / sumofallintensities;
		}
	}
	else {
		experimentalpeaksmatchedratio = 0;
	}
}


void cTheoreticalSpectrum::generateChargedFragments(cPeak& peak, map<string, int>& atoms, int& peaklistrealsize, int maxcharge, bool writedescription, bool disablesummary) {
	if (peak.mzratio > 0) {
		double tempratio;
		string tempdescription;
		map<string, int> tempmap;

		tempratio = peak.mzratio;
		if (writedescription) {
			tempdescription = peak.description;
		}
		for (int i = 1; i <= abs(maxcharge); i++) {

			peak.mzratio = charge(uncharge(tempratio, 1), (parameters->precursorcharge > 0) ? i : -i);
			peak.charge = (parameters->precursorcharge > 0) ? i : -i;
			if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
				tempmap = atoms;
				rechargeMap(peak.charge, tempmap);
				peak.formula.setFromMap(tempmap, false);
			}
			if (writedescription) {
				if (parameters->precursorcharge > 0) {
					peak.description = to_string(i) + "+ " + tempdescription;
				}
				else {
					peak.description = to_string(i) + "- " + tempdescription;
				}
			}

			if (writedescription) {
				if (peak.formula.hasAllElementsPositive()) {
					if (parameters->reportunmatchedtheoreticalpeaks || searchHint(peak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
						addPeakToList(peak, peaklistrealsize);
					}
					if (!parameters->generateisotopepattern) {
						addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, i, writedescription);
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
					addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, i, writedescription);
				}
			}

		}
		peak.mzratio = tempratio;
		if (writedescription) {
			peak.description = tempdescription;
		}

	}
}


void cTheoreticalSpectrum::generatePrecursorIon(vector<int>& intcomposition, cBricksDatabase& bricksdatabasewithcombinations, int& theoreticalpeaksrealsize, bool writedescription) {
	cPeak peak;
	int starttype, endtype;

	set<int> usedmodifications;
	usedmodifications.insert(0);

	map<string, int> atoms, tempmap;

	vector<int> currentlosses;
	vector<double> fragmentlossmass;
	vector<string> fragmentlosssummary;
	vector< map<string, int> > fragmentlossmap;

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

	switch (parameters->peptidetype) {
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

		// to do - performance optimization - no need to recalculate everything in every iteration
		updateListOfNeutralLosses(bricksdatabasewithcombinations, bricksdatabasewithcombinations[intcomposition[i] - 1], currentlosses, fragmentlossmass, fragmentlosssummary, fragmentlossmap, writedescription, disablesummary);
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

			for (auto it = usedmodifications.begin(); it != usedmodifications.end(); ++it) {

				for (int k = -1; k < (int)fragmentlossmass.size(); k++) {

					for (int m = 1; m <= abs(parameters->precursorcharge); m++) {

						peak.iontype = (eFragmentIonType)i;
						peak.neutrallosstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];
						peak.mzratio = tempratio + parameters->iondefinitions[(eFragmentIonType)i].massdifference - parameters->searchedmodifications[*it].massdifference;
						if (j >= 0) {
							peak.mzratio += parameters->neutrallossesdefinitions[peak.neutrallosstype].massdifference;
						}
						if (k >= 0) {
							peak.mzratio += fragmentlossmass[k];
						}
						peak.mzratio = charge(uncharge(peak.mzratio, 1), (parameters->precursorcharge > 0) ? m : -m);
						peak.charge = (parameters->precursorcharge > 0) ? m : -m;

						if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
							tempmap = atoms;
							mergeMaps(parameters->iondefinitions[(eFragmentIonType)i].summarymap, tempmap);
							if (j >= 0) {
								mergeMaps(parameters->neutrallossesdefinitions[peak.neutrallosstype].summarymap, tempmap);
							}
							if (k >= 0) {
								mergeMaps(fragmentlossmap[k], tempmap);
							}
							rechargeMap(peak.charge, tempmap);
							peak.formula.setFromMap(tempmap, false);
							peak.formula.addFormula(parameters->searchedmodifications[*it].summary, true);
						}

						if (writedescription) {
							string str;

							peak.description = parameters->iondefinitions[(eFragmentIonType)i].name;
							if ((k >= 0) || (j >= 0)) {
								peak.description += " -";
								if (k >= 0) {
									peak.description += fragmentlosssummary[k];
								}
								if (j >= 0) {
									peak.description += parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
								}
							}
							if (*it > 0) {
								peak.description += " (" + parameters->searchedmodifications[*it].name + " loss)";
							}
							peak.description += ":";

							if (parameters->precursorcharge > 0) {
								if (!parameters->precursoradduct.empty()) {
									str = "+" + parameters->precursoradduct;
									if (m > 1) {
										str += "+";
										if (m > 2) {
											str += to_string(m - 1);
										}
										str += "H";
									}
									peak.description.replace(peak.description.find("+zH"), 3, str);
								}
								else {
									str = "+";
									if (m > 1) {
										str += to_string(m);
									}
									str += "H";
									peak.description.replace(peak.description.find("+zH"), 3, str);
								}

								str = "]";
								if (m > 1) {
									str += to_string(m);
								}
								str += "+";
								peak.description.replace(peak.description.find("]+"), 2, str);
							}
							else {
								if (!parameters->precursoradduct.empty()) {
									str = "-" + parameters->precursoradduct;
									if (m > 1) {
										str += "-";
										if (m > 2) {
											str += to_string(m - 1);
										}
										str += "H";
									}
									peak.description.replace(peak.description.find("+zH"), 3, str);
								}
								else {
									str = "-";
									if (m > 1) {
										str += to_string(m);
									}
									str += "H";
									peak.description.replace(peak.description.find("+zH"), 3, str);
								}

								str = "]";
								if (m > 1) {
									str += to_string(m);
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
									addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, m, writedescription);
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
								addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, m, writedescription);
							}
						}

					}

				}

			}

		}

	}

}


void cTheoreticalSpectrum::generatePrecursorIonForOther(int& theoreticalpeaksrealsize, bool writedescription) {
	cPeak peak;
	string formula;

	map<string, int> atoms, tempmap;

	peak.isotope = false;
	peak.seriesid = 0;
	peak.iontype = fragmentIonTypeEnd;

	peak.mzratio = candidate.getSummaryFormula().getMass() + H - e;
	
	if (parameters->generateisotopepattern || writedescription) {
		formula = "H+";
		addStringFormulaToMap(formula, atoms);
		formula = candidate.getSummaryFormula().getSummary();
		addStringFormulaToMap(formula, atoms);
	}
	
	/*
	if (parameters->precursoradduct.compare("") != 0) {
		cSummaryFormula adduct;
		adduct.setFormula(parameters->precursoradduct);

		peak.mzratio += adduct.getMass() - H;

		if (parameters->generateisotopepattern || writedescription) {
			formula = "H-1";
			addStringFormulaToMap(formula, atoms);
			addStringFormulaToMap(parameters->precursoradduct, atoms);
		}
	}
	*/

	double tempratio = peak.mzratio;

	for (int i = -1; i < (int)parameters->neutrallossesfortheoreticalspectra.size(); i++) {

		for (int j = 1; j <= abs(parameters->precursorcharge); j++) {

			peak.neutrallosstype = (i == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[i];
			peak.mzratio = tempratio;
			if (i >= 0) {
				peak.mzratio += parameters->neutrallossesdefinitions[peak.neutrallosstype].massdifference;
			}
			peak.mzratio = charge(uncharge(peak.mzratio, 1), (parameters->precursorcharge > 0) ? j : -j);
			peak.charge = (parameters->precursorcharge > 0) ? j : -j;

			if (parameters->generateisotopepattern || writedescription) {
				tempmap = atoms;
				if (i >= 0) {
					mergeMaps(parameters->neutrallossesdefinitions[peak.neutrallosstype].summarymap, tempmap);
				}
				rechargeMap(peak.charge, tempmap);
				peak.formula.setFromMap(tempmap, false);
			}

			if (writedescription) {
				string str;

				peak.description = "[M+zH]+";
				if (i >= 0) {
					peak.description += " -";
					if (i >= 0) {
						peak.description += parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
					}
				}
				peak.description += ":";

				if (parameters->precursorcharge > 0) {
					if (!parameters->precursoradduct.empty()) {
						str = "+" + parameters->precursoradduct;
						if (j > 1) {
							str += "+";
							if (j > 2) {
								str += to_string(j - 1);
							}
							str += "H";
						}
						peak.description.replace(peak.description.find("+zH"), 3, str);
					}
					else {
						str = "+";
						if (j > 1) {
							str += to_string(j);
						}
						str += "H";
						peak.description.replace(peak.description.find("+zH"), 3, str);
					}

					str = "]";
					if (j > 1) {
						str += to_string(j);
					}
					str += "+";
					peak.description.replace(peak.description.find("]+"), 2, str);
				}
				else {
					if (!parameters->precursoradduct.empty()) {
						str = "-" + parameters->precursoradduct;
						if (j > 1) {
							str += "-";
							if (j > 2) {
								str += to_string(j - 1);
							}
							str += "H";
						}
						peak.description.replace(peak.description.find("+zH"), 3, str);
					}
					else {
						str = "-";
						if (j > 1) {
							str += to_string(j);
						}
						str += "H";
						peak.description.replace(peak.description.find("+zH"), 3, str);
					}

					str = "]";
					if (j > 1) {
						str += to_string(j);
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
						addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, j, writedescription);
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
					addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, j, writedescription);
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

	vector<int> currentlosses;
	vector<double> fragmentlossmass;
	vector<string> fragmentlosssummary;
	vector< map<string, int> > fragmentlossmap;
	
	// generate scrambled peaks from sequences
	for (auto it = scrambledsequences.begin(); it != scrambledsequences.end(); ++it) {

		b.setComposition((string &)*it, false);
		intcomposition.clear();
		b.explodeToIntComposition(intcomposition);

		disablesummary = false;
		if ((parameters->mode == denovoengine) && (parameters->blindedges == 2)) {
			for (int i = 0; i < (int)intcomposition.size(); i++) {
				if (bricksdatabase[intcomposition[i] - 1].isArtificial()) {
					disablesummary = true;
					break;
				}
			}
		}

		tempmzratio = 0;

		currentlosses.clear();
		fragmentlossmass.clear();
		fragmentlosssummary.clear();
		fragmentlossmap.clear();
		
		for (int i = 0; i < (int)intcomposition.size(); i++) {
			tempmzratio += bricksdatabase[intcomposition[i] - 1].getMass();

			// to do - possible performance optimization
			updateListOfNeutralLosses(bricksdatabase, bricksdatabase[intcomposition[i] - 1], currentlosses, fragmentlossmass, fragmentlosssummary, fragmentlossmap, writedescription, disablesummary);
		}

		tempmapready = false;
			
		for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

			for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

				for (int k = -1; k < (int)fragmentlossmass.size(); k++) {

					peak.clear();
					peak.iontype = (eFragmentIonType)parameters->ionsfortheoreticalspectraMS2[i];
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
					if (k >= 0) {
						peak.mzratio += fragmentlossmass[k];
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

							for (int m = 0; m < (int)intcomposition.size(); m++) {
								mergeMaps(bricksdatabase[intcomposition[m] - 1].getSummaryMap(), tempmap);
							}

							tempmapready = true;
						}

						atoms = tempmap;
						mergeMaps(parameters->iondefinitions[peak.iontype].summarymap, atoms);
						if (peak.neutrallosstype >= 0) {
							mergeMaps(parameters->neutrallossesdefinitions[peak.neutrallosstype].summarymap, atoms);
						}
						if (k >= 0) {
							mergeMaps(fragmentlossmap[k], atoms);
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
						if ((k >= 0) || (peak.neutrallosstype >= 0)) {
							peak.description += " -";
							if (k >= 0) {
								peak.description += fragmentlosssummary[k];
							}
							if (peak.neutrallosstype >= 0) {
								peak.description += parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
							}
						}

						addAdductToDescription(peak.description, parameters->metaladducts);
						peak.description += ": ";
						for (int m = 0; m < (int)intcomposition.size(); m++) {
							peak.description += "[" + bricksdatabase[intcomposition[m] - 1].getAcronymsAsString() + "]";
							if (m < (int)intcomposition.size() - 1) {
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


void cTheoreticalSpectrum::updateListOfNeutralLosses(cBricksDatabase& bricksdatabase, cBrick& block, vector<int>& currentlosses, vector<double>& fragmentlossmass, vector<string>& fragmentlosssummary, vector< map<string, int> >& fragmentlossmap, bool writedescription, bool disablesummary) {
	set< vector<int> > fragmentlossset;
	map<string, int> fmap;

	const unsigned max_losses = 25;

	vector<int> tmplossvector;
	tmplossvector.reserve(max_losses);

	unsigned binary, binarypos, currsize, loopend;

	// the data are updated only if the block contains a list of losses
	if (block.getLossIDs().size() > 0) {
		currentlosses.reserve(currentlosses.size() + block.getLossIDs().size());
		currentlosses.insert(currentlosses.end(), block.getLossIDs().begin(), block.getLossIDs().end());
		sort(currentlosses.begin(), currentlosses.end());

		fragmentlossset.clear();

		fragmentlossmass.clear();
		if (writedescription) {
			fragmentlosssummary.clear();
		}
		if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
			fragmentlossmap.clear();
		}

		currsize = (unsigned)currentlosses.size();
		if (currsize < max_losses) {
			loopend = 1 << currsize;
			for (unsigned j = 1; j < loopend; j++) {
				binary = j;
				binarypos = 0;
				tmplossvector.clear();
				while ((binary > 0) && (binarypos < currsize)) {
					if ((binary & 1) == 1) {
						tmplossvector.push_back(currentlosses[binarypos]);
					}
					binary = binary >> 1;
					binarypos++;
				}
				fragmentlossset.insert(tmplossvector);
			}

			fragmentlossmass.reserve(fragmentlossset.size());
			if (writedescription) {
				fragmentlosssummary.reserve(fragmentlossset.size());
			}
			if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
				fragmentlossmap.reserve(fragmentlossset.size());
			}

			for (auto& it : fragmentlossset) {
				fragmentlossmass.push_back(bricksdatabase.getMassOfNeutralLosses((vector<int>&)it));
				if (writedescription) {
					fragmentlosssummary.push_back(bricksdatabase.getSummaryFormulaOfNeutralLosses((vector<int>&)it));
				}
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					bricksdatabase.getMapOfNeutralLosses((vector<int>&)it, fmap);
					fragmentlossmap.push_back(fmap);
				}
			}
		}
	}

}


void cTheoreticalSpectrum::generateInternalFragments(cBricksDatabase& bricksdatabase, cPeak& peak, int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, int pos, map<string, int>& atoms, vector<int>& currentlosses, eFragmentIonType fragmentiontype, ePeptideType peptidetype, TRotationInfo* trotation, bool writedescription, bool disablesummary) {

	vector<int> internalcurrentlosses = currentlosses;

	vector<double> fragmentlossmass;
	vector<string> fragmentlosssummary;
	vector< map<string, int> > fragmentlossmap;

	vector<int> internalcomposition;
	
	double tmpmz1;
	map<string, int> tmpmap1;

	double tmpmz2;
	map<string, int> tmpmap2;

	int tmprotationid;
	int tmpseriesid;

	if ((peptidetype == branchcyclic) && trotation && (trotation->id == 0)) {

		if ((pos >= trotation->middlebranchstart) && (pos < trotation->middlebranchend)) {

			internalcomposition.clear();
			for (int i = 0; i <= pos; i++) {
				internalcomposition.push_back(intcomposition[i]);
			}

			tmpmz1 = peak.mzratio;
			if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
				tmpmap1 = atoms;
			}

			tmprotationid = peak.rotationid;
			tmpseriesid = peak.seriesid;

			peak.rotationid = -1;
			peak.seriesid = -1;

			for (int i = trotation->middlebranchend + 1; i < (int)intcomposition.size() - 1; i++) {
				internalcomposition.push_back(intcomposition[i]);

				peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					mergeMaps(bricksdatabase[intcomposition[i] - 1].getSummaryMap(), tmpmap1);
				}

				updateListOfNeutralLosses(bricksdatabase, bricksdatabase[intcomposition[pos] - 1], internalcurrentlosses, fragmentlossmass, fragmentlosssummary, fragmentlossmap, writedescription, disablesummary);

				tmpmz2 = peak.mzratio;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					tmpmap2 = tmpmap1;
				}
				
				for (int j = -1; j < (int)fragmentlossmass.size(); j++) {

					if (writedescription) {
						peak.description = "internal ";

						peak.description += parameters->iondefinitions[fragmentiontype].name.substr(0, 1) + to_string((int)internalcomposition.size());
						//	to do - polyketides
						//	if (parameters->iondefinitions[fragmentiontype].name.size() > 1) {
						//		peak.description += parameters->iondefinitions[fragmentiontype].name.substr(1, parameters->iondefinitions[fragmentiontype].name.length() - 1);
						//	}

						if ((peak.neutrallosstype >= 0) || (j >= 0)) {
							peak.description += " -";
							if (j >= 0) {
								peak.description += fragmentlosssummary[j];
							}
							if (peak.neutrallosstype >= 0) {
								peak.description += parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
							}
						}

						addAdductToDescription(peak.description, parameters->metaladducts);
						peak.description += ": ";

						for (int k = 0; k < (int)internalcomposition.size(); k++) {
							peak.description += "[" + bricksdatabase[internalcomposition[k] - 1].getAcronymsAsString() + "]";
							if (k < (int)internalcomposition.size() - 1) {
								peak.description += '-';
							}
						}
					}

					if (j >= 0) {
						peak.mzratio += fragmentlossmass[j];
						if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
							mergeMaps(fragmentlossmap[j], tmpmap2);
						}
					}

					generateChargedFragments(peak, tmpmap2, peaklistrealsize, maxcharge, writedescription, disablesummary);

					peak.mzratio = tmpmz2;
					if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
						tmpmap2 = tmpmap1;
					}

				}

			}

			peak.mzratio = tmpmz1;

			peak.rotationid = tmprotationid;
			peak.seriesid = tmpseriesid;

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

				if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch) || (writedescription && parameters->reportunmatchedtheoreticalpeaks) || searchHint(tmppeak.mzratio, experimentalpeaks, parameters->fragmentmasserrortolerance)) {
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
							(((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks)) {
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
					if (storeunmatchedpeaks && !theoreticalpeaks[j].decoy && ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks) {
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
					(((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks)) {
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
			if (storeunmatchedpeaks && !theoreticalpeaks[j].decoy && ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && parameters->generateisotopepattern && parameters->reportunmatchedtheoreticalpeaks) {
				outputtheoreticalpeaks.add(theoreticalpeaks[j]);
			}
		}
	}
}


int cTheoreticalSpectrum::removeUnmatchedFeatures(bool lcms, cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, vector< vector<int> >& hintsindex/*, int id*/) {
	if (theoreticalpeaksrealsize == 0) {
		return 0;
	}

	int size = parameters->peaklistseries.size();
	int k;

	int peaksremoved = 0;

	int featuresize;
	int maxfeaturesize;

	int patternsize;

	int start = 0;
	int stop = 0;
	int currid = theoreticalpeaks[0].compoundid;
	int compoundsize = 0;
	int msize;

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {

		if (currid != theoreticalpeaks[i].compoundid) {
			if (compoundsize < parameters->minimumiontypes) {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched > 0) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;

						peaksremoved++;
					}
				}
			}

			currid = theoreticalpeaks[i].compoundid;
			start = i;
			compoundsize = 0;
		}

		stop = i;

		// ok, matched and unmatched peaks are searched (except isotopes) to calculate compoundsize correctly 
		if ((theoreticalpeaks[i].isotope) /*|| (theoreticalpeaks[i].matched == 0)*/) {
			continue;
		}

		if ((int)hintsindex[i].size() >= parameters->minimumfeaturesize) {

			featuresize = 0;
			maxfeaturesize = 0;

			for (int j = 0; j < (int)hintsindex[i].size(); j++) {

				if (lcms) {
					if ((j > 0) && (hintsindex[i][j - 1] + 1 != hintsindex[i][j])) {
						if (featuresize > maxfeaturesize) {
							maxfeaturesize = featuresize;
						}
						featuresize = 0;
					}
				}

				if (parameters->generateisotopepattern) {					
					k = i;
					while ((k >= 0) && (theoreticalpeaks[k].groupid == theoreticalpeaks[i].groupid)) {
						k--;
					}
					k++;

					patternsize = 0;
					while ((k < theoreticalpeaksrealsize) && (theoreticalpeaks[k].groupid == theoreticalpeaks[i].groupid)) {
						msize = (int)hintsindex[k].size();
						for (int m = 0; m < msize; m++) {
							if (hintsindex[k][m] == hintsindex[i][j]) {
								patternsize++;
								break;
							}
						}

						if (patternsize >= parameters->minimumpatternsize) {
							break;
						}

						k++;
					}

					if (patternsize >= parameters->minimumpatternsize) {
						featuresize++;
					}
				}
				else {
					featuresize++;
				}

			}

			if (featuresize > maxfeaturesize) {
				maxfeaturesize = featuresize;
			}

			if (maxfeaturesize >= parameters->minimumfeaturesize) {
				compoundsize++;
			}

		}

	}

	if (compoundsize < parameters->minimumiontypes) {
		for (int j = start; j <= stop; j++) {
			if (theoreticalpeaks[j].matched > 0) {
				experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
				experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

				theoreticalpeaks[j].matched--;
				theoreticalpeaks[j].matchedid = -1;

				peaksremoved++;
			}
		}
	}

	return peaksremoved;
}


int cTheoreticalSpectrum::removeUnmatchedCompounds(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, int minimumiontypes) {
	if (theoreticalpeaksrealsize == 0) {
		return 0;
	}

	int peaksremoved = 0;

	int start = 0;
	int stop = 0;
	int currid = theoreticalpeaks[0].compoundid;
	int matched = 0;
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (currid != theoreticalpeaks[i].compoundid) {
			if (matched < minimumiontypes) {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched > 0) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;

						peaksremoved++;
					}
				}
			}
			
			currid = theoreticalpeaks[i].compoundid;
			start = i;
			matched = 0;
		}

		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0)) {
			matched++;
		}
		stop = i;
	}

	if (matched < minimumiontypes) {
		for (int j = start; j <= stop; j++) {
			if (theoreticalpeaks[j].matched > 0) {
				experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
				experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

				theoreticalpeaks[j].matched--;
				theoreticalpeaks[j].matchedid = -1;

				peaksremoved++;
			}
		}
	}

	return peaksremoved;
}


void cTheoreticalSpectrum::removeUnmatchedPatternsFineSpectra(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks) {
	if (theoreticalpeaksrealsize == 0) {
		return;
	}

	int groupid = theoreticalpeaks[0].groupid;
	int start = 0;
	int stop = 0;
	int maximumintensityid = 0;
	double maximumintensity = theoreticalpeaks[0].relativeintensity;
	bool cleargroup = false;

	int langle = 0;
	int rangle = 0;
	double fwhmthreshold = 0.001;
	string subdesc;

	bool hasLi = false;
	bool hasB = false;
	bool hasMg = false;
	bool hasSi = false;
	bool hasS = false;
	bool hasCl = false;
	bool hasK = false;
	bool hasTi = false;
	bool hasCr = false;
	bool hasFe = false;
	bool hasNi = false;
	bool hasCu = false;
	bool hasZn = false;
	bool hasGa = false;

	string elementLi = " Li";
	string elementB  = " B";
	string elementMg = " Mg";
	string elementSi = " Si";
	string elementS  = " S";
	string elementCl = " Cl";
	string elementK  = " K";
	string elementTi = " Ti";
	string elementCr = " Cr";
	string elementFe = " Fe";
	string elementNi = " Ni";
	string elementCu = " Cu";
	string elementZn = " Zn";
	string elementGa = " Ga";

	string isotopeLi6  = " 6Li ";
	string isotopeB10  = " 10B ";
	string isotopeMg25 = " 25Mg ";
	string isotopeMg26 = " 26Mg ";
	string isotopeSi29 = " 29Si ";
	string isotopeSi30 = " 30Si ";
	string isotopeS34  = " 34S ";
	string isotopeCl37 = " 37Cl ";
	string isotopeK41  = " 41K ";
	string isotopeTi46 = " 46Ti ";
	string isotopeTi47 = " 47Ti ";
	string isotopeTi49 = " 49Ti ";
	string isotopeTi50 = " 50Ti ";
	string isotopeCr50 = " 50Cr ";
	string isotopeCr53 = " 53Cr ";
	string isotopeFe54 = " 54Fe ";
	string isotopeNi60 = " 60Ni ";
	string isotopeNi62 = " 62Ni ";
	string isotopeCu65 = " 65Cu ";
	string isotopeZn66 = " 66Zn ";
	string isotopeZn67 = " 67Zn ";
	string isotopeZn68 = " 68Zn ";
	string isotopeGa71 = " 71Ga ";
	
	size_t elempos;
	size_t subdescsize;

	double maximumexperimentalintensity;
	if (theoreticalpeaks[maximumintensityid].matched) {
		maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
	}

	langle = (int)parameters->peakidtodesc[theoreticalpeaks[start].descriptionid].rfind('(');
	rangle = (int)parameters->peakidtodesc[theoreticalpeaks[start].descriptionid].rfind(')');
	if ((langle != string::npos) && (rangle != string::npos)) {
		subdesc = parameters->peakidtodesc[theoreticalpeaks[start].descriptionid].substr(langle + 1, rangle - langle - 1);
		subdescsize = subdesc.size();

		elempos = subdesc.find("Li");
		if (elempos != string::npos) {
			hasLi = true;
		}

		elempos = subdesc.find("B");
		if (elempos != string::npos) {
			if (elempos + 1 < subdescsize) {
				if ((subdesc[elempos + 1] < 'a') || (subdesc[elempos + 1] > 'z')) {
					hasB = true;
				}
			}
			else {
				hasB = true;
			}
		}

		elempos = subdesc.find("Ti");
		if (elempos != string::npos) {
			hasTi = true;
		}

		elempos = subdesc.find("Cr");
		if (elempos != string::npos) {
			hasCr = true;
		}

		elempos = subdesc.find("Fe");
		if (elempos != string::npos) {
			hasFe = true;
		}

		if (parameters->fwhm <= fwhmthreshold) {

			elempos = subdesc.find("Mg");
			if (elempos != string::npos) {
				hasMg = true;
			}

			elempos = subdesc.find("Si");
			if (elempos != string::npos) {
				hasSi = true;
			}

			elempos = subdesc.find("S");
			if (elempos != string::npos) {
				if (elempos + 1 < subdescsize) {
					if ((subdesc[elempos + 1] < 'a') || (subdesc[elempos + 1] > 'z')) {
						hasS = true;
					}
				}
				else {
					hasS = true;
				}
			}

			elempos = subdesc.find("Cl");
			if (elempos != string::npos) {
				hasCl = true;
			}

			elempos = subdesc.find("K");
			if (elempos != string::npos) {
				if (elempos + 1 < subdescsize) {
					if ((subdesc[elempos + 1] < 'a') || (subdesc[elempos + 1] > 'z')) {
						hasK = true;
					}
				}
				else {
					hasK = true;
				}
			}

			elempos = subdesc.find("Ni");
			if (elempos != string::npos) {
				hasNi = true;
			}

			elempos = subdesc.find("Cu");
			if (elempos != string::npos) {
				hasCu = true;
			}

			elempos = subdesc.find("Zn");
			if (elempos != string::npos) {
				hasZn = true;
			}

			elempos = subdesc.find("Ga");
			if (elempos != string::npos) {
				hasGa = true;
			}

		}

	}
	
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {

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

		if ((groupid != theoreticalpeaks[i].groupid) || (i == theoreticalpeaksrealsize - 1)) {

			// lithium
			if (hasLi) {
				if (checkIsotope(elementLi, isotopeLi6, Li6, Li7, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
					cleargroup = true;
				}
			}

			// boron 
			if (hasB) {
				if (checkIsotope(elementB, isotopeB10, B10, B11, 0.3, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
					cleargroup = true;
				}
			}

			// titanium (part 1)
			if (hasTi) {
				if (checkIsotope(elementTi, isotopeTi46, Ti46, Ti48, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
					cleargroup = true;
				}
				if (checkIsotope(elementTi, isotopeTi47, Ti47, Ti48, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
					cleargroup = true;
				}
			}

			// chromium (part 1)
			if (hasCr) {
				if (checkIsotope(elementCr, isotopeCr50, Cr50, Cr52, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
					cleargroup = true;
				}
			}

			// iron
			if (hasFe) {
				if (checkIsotope(elementFe, isotopeFe54, Fe54, Fe56, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
					cleargroup = true;
				}
			}

			if (parameters->fwhm <= fwhmthreshold) {

				// magnesium
				if (hasMg) {
					if (checkIsotope(elementMg, isotopeMg25, Mg25, Mg24, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
					if (checkIsotope(elementMg, isotopeMg26, Mg26, Mg24, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// silicon
				if (hasSi) {
					if (checkIsotope(elementSi, isotopeSi29, Si29, Si28, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
					if (checkIsotope(elementSi, isotopeSi30, Si30, Si28, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// sulfur
				if (hasS) {
					if (checkIsotope(elementS, isotopeS34, S34, S32, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// chlorine
				if (hasCl) {
					if (checkIsotope(elementCl, isotopeCl37, Cl37, Cl35, 0.4, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// potassium
				if (hasK) {
					if (checkIsotope(elementK, isotopeK41, K41, K39, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// titanium (part 2)
				if (hasTi) {
					if (checkIsotope(elementTi, isotopeTi49, Ti49, Ti48, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
					if (checkIsotope(elementTi, isotopeTi50, Ti50, Ti48, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// chromium (part 2)
				if (hasCr) {
					if (checkIsotope(elementCr, isotopeCr53, Cr53, Cr52, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// nickel
				if (hasNi) {
					if (checkIsotope(elementNi, isotopeNi60, Ni60, Ni58, 0.5, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
					if (checkIsotope(elementNi, isotopeNi62, Ni62, Ni58, 0.1, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// copper
				if (hasCu) {
					if (checkIsotope(elementCu, isotopeCu65, Cu65, Cu63, 0.6, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// zinc
				if (hasZn) {
					if (checkIsotope(elementZn, isotopeZn66, Zn66, Zn64, 0.7, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
					if (checkIsotope(elementZn, isotopeZn67, Zn67, Zn64, 0.2, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
					if (checkIsotope(elementZn, isotopeZn68, Zn68, Zn64, 0.5, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

				// gallium
				if (hasGa) {
					if (checkIsotope(elementGa, isotopeGa71, Ga71, Ga69, 0.8, theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, start, stop, maximumintensityid, maximumexperimentalintensity)) {
						cleargroup = true;
					}
				}

			}

			if (cleargroup) {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched > 0) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;
					}
				}
			}
			
			groupid = theoreticalpeaks[i].groupid;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
			cleargroup = false;

			hasLi = false;
			hasB = false;
			hasMg = false;
			hasSi = false;
			hasS = false;
			hasCl = false;
			hasK = false;
			hasTi = false;
			hasCr = false;
			hasFe = false;
			hasNi = false;
			hasCu = false;
			hasZn = false;
			hasGa = false;

			if (theoreticalpeaks[i].matched) {
				maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
			}

			langle = (int)parameters->peakidtodesc[theoreticalpeaks[start].descriptionid].rfind('(');
			rangle = (int)parameters->peakidtodesc[theoreticalpeaks[start].descriptionid].rfind(')');
			if ((langle != string::npos) && (rangle != string::npos)) {
				subdesc = parameters->peakidtodesc[theoreticalpeaks[start].descriptionid].substr(langle + 1, rangle - langle - 1);
				subdescsize = subdesc.size();

				elempos = subdesc.find("Li");
				if (elempos != string::npos) {
					hasLi = true;
				}

				elempos = subdesc.find("B");
				if (elempos != string::npos) {
					if (elempos + 1 < subdescsize) {
						if ((subdesc[elempos + 1] < 'a') || (subdesc[elempos + 1] > 'z')) {
							hasB = true;
						}
					}
					else {
						hasB = true;
					}
				}

				elempos = subdesc.find("Ti");
				if (elempos != string::npos) {
					hasTi = true;
				}

				elempos = subdesc.find("Cr");
				if (elempos != string::npos) {
					hasCr = true;
				}

				elempos = subdesc.find("Fe");
				if (elempos != string::npos) {
					hasFe = true;
				}

				if (parameters->fwhm <= fwhmthreshold) {

					elempos = subdesc.find("Mg");
					if (elempos != string::npos) {
						hasMg = true;
					}

					elempos = subdesc.find("Si");
					if (elempos != string::npos) {
						hasSi = true;
					}

					elempos = subdesc.find("S");
					if (elempos != string::npos) {
						if (elempos + 1 < subdescsize) {
							if ((subdesc[elempos + 1] < 'a') || (subdesc[elempos + 1] > 'z')) {
								hasS = true;
							}
						}
						else {
							hasS = true;
						}
					}

					elempos = subdesc.find("Cl");
					if (elempos != string::npos) {
						hasCl = true;
					}

					elempos = subdesc.find("K");
					if (elempos != string::npos) {
						if (elempos + 1 < subdescsize) {
							if ((subdesc[elempos + 1] < 'a') || (subdesc[elempos + 1] > 'z')) {
								hasK = true;
							}
						}
						else {
							hasK = true;
						}
					}

					elempos = subdesc.find("Ni");
					if (elempos != string::npos) {
						hasNi = true;
					}

					elempos = subdesc.find("Cu");
					if (elempos != string::npos) {
						hasCu = true;
					}

					elempos = subdesc.find("Zn");
					if (elempos != string::npos) {
						hasZn = true;
					}

					elempos = subdesc.find("Ga");
					if (elempos != string::npos) {
						hasGa = true;
					}

				}

			}
			
		}

	}

}


void cTheoreticalSpectrum::removeUnmatchedPatternsByIntensityRatio(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks) {
	if (theoreticalpeaksrealsize == 0) {
		return;
	}

	int groupid = theoreticalpeaks[0].groupid;
	int start = 0;
	int stop = 0;
	int maximumintensityid = 0;
	double maximumintensity = theoreticalpeaks[0].relativeintensity;
	//int minimummatchedintensityid = 0;
	double minimummatchedintensity = DBL_MAX;
	bool cleargroup = false;

	double maximumexperimentalintensity;
	if (theoreticalpeaks[maximumintensityid].matched) {
		maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
	}

	double thratio, expratio;

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
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

		if ((groupid != theoreticalpeaks[i].groupid) || (i == theoreticalpeaksrealsize - 1)) {

			if (theoreticalpeaks[maximumintensityid].matched > 0) {
				if (maximumexperimentalintensity >= parameters->minimumrelativeintensitythreshold) {
					for (int j = start; j <= stop; j++) {
						if ((theoreticalpeaks[j].matched > 0)
							&& (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold)
							&& (theoreticalpeaks[j].relativeintensity < minimummatchedintensity)) {
							//minimummatchedintensityid = j;
							minimummatchedintensity = theoreticalpeaks[j].relativeintensity;
						}
					}

					for (int j = start; j <= stop; j++) {
						if (j == maximumintensityid) {
							continue;
						}

						if (theoreticalpeaks[j].matched > 0) {
							if (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
								thratio = theoreticalpeaks[j].relativeintensity / theoreticalpeaks[maximumintensityid].relativeintensity;
								expratio = experimentalpeaks[theoreticalpeaks[j].matchedid].relativeintensity / experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;

								if (fabs(thratio - expratio) * 100.0 > parameters->intensitytolerance) {
									cleargroup = true;
									break;
								}
							}
						}
						else {
							if ((parameters->fwhm >= 0.01) && (theoreticalpeaks[j].relativeintensity > minimummatchedintensity)) {
								cleargroup = true;
								break;
							}
						}
					}
				}
				else {
					cleargroup = true;
				}
			}
			else {
				cleargroup = true;
			}

			if (cleargroup) {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched > 0) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;
					}
				}
			}

			groupid = theoreticalpeaks[i].groupid;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
			//minimummatchedintensityid = i;
			minimummatchedintensity = DBL_MAX;
			cleargroup = false;
			if (theoreticalpeaks[i].matched) {
				maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
			}
		}
	}
}


void cTheoreticalSpectrum::removeUnmatchedPatternsByMZDifference(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks) {
	if (theoreticalpeaksrealsize == 0) {
		return;
	}

	int groupid = theoreticalpeaks[0].groupid;
	int start = 0;
	int stop = 0;
	int maximumintensityid = 0;
	double maximumintensity = theoreticalpeaks[0].relativeintensity;
	bool cleargroup = false;

	double maximumexperimentalintensity;
	if (theoreticalpeaks[maximumintensityid].matched) {
		maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].relativeintensity;
	}

	double thdiff, expdiff;

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
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

		if ((groupid != theoreticalpeaks[i].groupid) || (i == theoreticalpeaksrealsize - 1)) {

			if (theoreticalpeaks[maximumintensityid].matched > 0) {
				if (maximumexperimentalintensity >= parameters->minimumrelativeintensitythreshold) {
					for (int j = start; j <= stop; j++) {
						if (j == maximumintensityid) {
							continue;
						}

						if (theoreticalpeaks[j].matched > 0) {
							if (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
								thdiff = fabs(theoreticalpeaks[maximumintensityid].mzratio - theoreticalpeaks[j].mzratio);
								expdiff = fabs(experimentalpeaks[theoreticalpeaks[maximumintensityid].matchedid].mzratio - experimentalpeaks[theoreticalpeaks[j].matchedid].mzratio);

								if (fabs(thdiff - expdiff) / theoreticalpeaks[maximumintensityid].mzratio * 1000000.0 > parameters->mzdifftolerance) {
									cleargroup = true;
									break;
								}
							}
						}
					}
				}
				else {
					cleargroup = true;
				}
			}
			else {
				cleargroup = true;
			}

			if (cleargroup) {
				for (int j = start; j <= stop; j++) {
					if (theoreticalpeaks[j].matched > 0) {
						experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
						experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

						theoreticalpeaks[j].matched--;
						theoreticalpeaks[j].matchedid = -1;
					}
				}
			}

			groupid = theoreticalpeaks[i].groupid;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
			cleargroup = false;
			if (theoreticalpeaks[i].matched) {
				maximumexperimentalintensity = experimentalpeaks[theoreticalpeaks[i].matchedid].relativeintensity;
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


bool cTheoreticalSpectrum::checkIsotope(string& elementstring, string& isotopestring, double isotopemass1, double isotopemass2, double intensityratio, cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, int start, int stop, int maximumintensityid, double maximumexperimentalintensity) {
	bool notdetected = false;
	int isotopeposition = -1;
	double intensitymultiplier = 1.0;
	double mzdiff;
	size_t pos;

	if (theoreticalpeaks[maximumintensityid].matched > 0) {
		if (maximumexperimentalintensity >= parameters->minimumrelativeintensitythreshold) {
			for (int i = start; i <= stop; i++) {
				if (theoreticalpeaks[i].matched > 0) {
					mzdiff = fabs(uncharge(theoreticalpeaks[i].mzratio, theoreticalpeaks[i].charge) - uncharge(theoreticalpeaks[maximumintensityid].mzratio, theoreticalpeaks[maximumintensityid].charge) - isotopemass1 + isotopemass2);
					if (mzdiff < 0.1) {
						if (parameters->peakidtodesc[theoreticalpeaks[i].descriptionid].find('%') == string::npos) {
							pos = parameters->peakidtodesc[theoreticalpeaks[i].descriptionid].rfind(isotopestring);
							if (pos != string::npos) {
								intensitymultiplier = getIntensityMultiplier(theoreticalpeaks, i, elementstring, pos);
								isotopeposition = i;
								break;
							}
						}
					}
				}
			}
		}
	}

	if (isotopeposition == -1) {
		notdetected = true;
	}
	else {
		if ((theoreticalpeaks[isotopeposition].matched > 0) && (theoreticalpeaks[maximumintensityid].matched > 0)) {
			if (maximumexperimentalintensity >= parameters->minimumrelativeintensitythreshold) {
				if (theoreticalpeaks[isotopeposition].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
					if (experimentalpeaks[theoreticalpeaks[isotopeposition].matchedid].relativeintensity > intensityratio * intensitymultiplier * maximumexperimentalintensity) {
						notdetected = true;
					}
				}
				else {
					notdetected = true;
				}
			}
		}
	}

	return notdetected;
}


double cTheoreticalSpectrum::getIntensityMultiplier(cPeaksList& theoreticalpeaks, int peakid, string& elementstring, size_t startpos) {
	size_t pos = parameters->peakidtodesc[theoreticalpeaks[peakid].descriptionid].find(elementstring, startpos);
	if (pos == string::npos) {
		// one occurrence
		return 1.0;
	}

	string shortstr = parameters->peakidtodesc[theoreticalpeaks[peakid].descriptionid].substr(pos + elementstring.size());
	if ((shortstr.size() > 0) && (shortstr[0] == ' ')) {
		// two occurrences
		return 2.0;
	}

	pos = shortstr.find(' ');
	if (pos != string::npos) {
		shortstr = shortstr.substr(0, pos);
		return atof(shortstr.c_str()) + 1.0;
	}

	return 1.0;
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
	weightedpeaksratio = 0;
	unmatchedexperimentalpeakscount = 0;
	coveragebyseries = "";
	valid = false;
	sumofrelativeintensities = 0;
	rotationslabels.clear();
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


	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			for (int k = 0; k < (int)trotations.size(); k++) {

				// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
				// in this case the modifID is 0, and both n-terminal and c-terminal fragments are generated
				// k == 2, 4, 5 are invalid
				if ((k == 0) || ((k == 1) && (parameters->searchedmodifications[trotations[k].startmodifID].nterminal)) || ((k == 3) && (parameters->searchedmodifications[trotations[k].endmodifID].cterminal))) {
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].nterminal) {
						generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[k].bricks, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotations[k]);
					}
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].cterminal) {
						generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[k].bricks, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotations[k]);
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
	/*vector<map<eFragmentIonType, map<int, vector<int> > > > series;
	series.resize(trotations.size());
	for (int i = 0; i < (int)series.size(); i++) {

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS2.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				if (k >= 0) {
					series[i][parameters->ionsfortheoreticalspectraMS2[j]][parameters->neutrallossesfortheoreticalspectra[k]].resize(trotations[0].bricks.size());
				}
				else {
					series[i][parameters->ionsfortheoreticalspectraMS2[j]][-1].resize(trotations[0].bricks.size());
				}

			}

		}

	}
	
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && (theoreticalpeaks[i].seriesid >= 0)
			&& (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)
			&& (series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}*/
	

	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)trotations.size(); i++) {	
			clearFalseHits(series[i], parameters->ionsfortheoreticalspectraMS2, parameters->neutrallossesfortheoreticalspectra);
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

		coveragebyseries = "Linearized sequences:<br/>";
		for (int i = 0; i < (int)trotations.size(); i++) {
			coveragebyseries += to_string(i + 1) + " ~ " + bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotations[i].tcomposition, false) + "<br/>";
		}
		coveragebyseries += "<br/>";

		/*matchedSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectraMS2.size() > 0) {
			coveragebyseries += "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int m = 0; m < (int)series[0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size() - 1; m++) {
				coveragebyseries += "<td>" + to_string(m + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)series.size(); i++) {
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size()) + "\">" + to_string(i + 1) + ":</td></tr>";
				for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS2.size(); j++) {
					for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {
						tempseries.name = to_string(i + 1) + "_" + parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[j]].name;
						if (k >= 0) {
							tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
						}
						tempseries.series.clear();
						losstype = (k == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[k];
						reportrow = false;
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype].size() - 1; m++) {
							if (series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype][m] > 0) {
								reportrow = true;
								break;
							}
						}
						if (reportrow) {
							coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
						}
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype].size() - 1; m++) {
							if (reportrow) {
								coveragebyseries += "<td>" + to_string(series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype][m]) + "</td>";
							}
							tempseries.series.push_back(series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype][m]);
						}
						if (reportrow) {
							coveragebyseries += "</tr>";
						}
					}
				}
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size()) + "\">&nbsp;</td></tr>";
			}
			coveragebyseries += "</table>";
		}*/

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

	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			if (parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].nterminal) {
				generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder);
			}

			if (parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].cterminal) {
				generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder);
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
	/*map<eFragmentIonType, map<int, vector<int> > > series;
	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			if (j >= 0) {
				series[parameters->ionsfortheoreticalspectraMS2[i]][parameters->neutrallossesfortheoreticalspectra[j]].resize(intcomposition.size());
			}
			else {
				series[parameters->ionsfortheoreticalspectraMS2[i]][-1].resize(intcomposition.size());
			}

		}

	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].seriesid >= 0)
			&& (series.count(theoreticalpeaks[i].iontype) == 1) 
			&& (series[theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}*/
	

	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->ionsfortheoreticalspectraMS2, parameters->neutrallossesfortheoreticalspectra);

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

		/*matchedSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectraMS2.size() > 0) {
			coveragebyseries = "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectraMS2[0]][-1].size() - 1; k++) {
				coveragebyseries += "<td>" + to_string(k + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {
				for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
					tempseries.name = parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].name;
					if (j >= 0) {
						tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
					}
					tempseries.series.clear();
					losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];
					reportrow = false;
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectraMS2[i]][losstype].size() - 1; k++) {
						if (series[parameters->ionsfortheoreticalspectraMS2[i]][losstype][k] > 0) {
							reportrow = true;
							break;
						}
					}
					if (reportrow) {
						coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
					}
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectraMS2[i]][losstype].size() - 1; k++) {
						if (reportrow) {
							coveragebyseries += "<td>" + to_string(series[parameters->ionsfortheoreticalspectraMS2[i]][losstype][k]) + "</td>";
						}
						tempseries.series.push_back(series[parameters->ionsfortheoreticalspectraMS2[i]][losstype][k]);
					}
					if (reportrow) {
						coveragebyseries += "</tr>";
					}
				}
			}
			coveragebyseries += "</table>";
		}*/

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

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS2.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				losstype = (k == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[k];

				generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectraMS2[j], losstype, bricksdatabasewithcombinations, writedescription, i, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, leftresiduelosstype, hasfirstblockartificial);
			
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
	/*vector<map<eFragmentIonType, map<int, vector<int> > > > series;
	series.resize(rotations.size());
	for (int i = 0; i < (int)series.size(); i++) {

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS2.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				if (k >= 0) {
					series[i][parameters->ionsfortheoreticalspectraMS2[j]][parameters->neutrallossesfortheoreticalspectra[k]].resize(r);
				}
				else {
					series[i][parameters->ionsfortheoreticalspectraMS2[j]][-1].resize(r);
				}

			}

		}

	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && (theoreticalpeaks[i].seriesid >= 0)
			&& (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)
			&& (series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}*/


	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)rotations.size(); i++) {	
			clearFalseHits(series[i], parameters->ionsfortheoreticalspectraMS2, parameters->neutrallossesfortheoreticalspectra);
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
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

				// parents at j == -1 are skipped
				for (int j = 0; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

					int k = 0;
					while (k < theoreticalpeaksrealsize) {
						// current peak is scrambled, matched, tested ion type matches the ion type and tested neutral loss type matches the loss type of the scrambled peak
						if (theoreticalpeaks[k].scrambled && (theoreticalpeaks[k].matched > 0) && (parameters->ionsfortheoreticalspectraMS2[i] == theoreticalpeaks[k].iontype) && (parameters->neutrallossesfortheoreticalspectra[j] == theoreticalpeaks[k].neutrallosstype)) {

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

		string rotationlabel;
		rotationslabels.clear();

		coveragebyseries = "Linearized sequences from all ring break up points:<br/>";
		for (int i = 0; i < (int)rotations.size(); i++) {
			if (i == r) {
				coveragebyseries += "<br/>";
			}
			rotationlabel = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1);
			rotationslabels.push_back(rotationlabel);
			coveragebyseries += rotationlabel + " " + bricksdatabasewithcombinations.getAcronymName(rotations[i], false) + "<br/>";
		}
		coveragebyseries += "<br/>";

		/*matchedSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectraMS2.size() > 0) {
			coveragebyseries += "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int m = 0; m < (int)series[0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size() - 1; m++) {
				coveragebyseries += "<td>" + to_string(m + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)series.size(); i++) {
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size()) + "\">" + to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + ":</td></tr>";
				for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS2.size(); j++) {
					for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {
						tempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_" + parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[j]].name;
						if (k >= 0) {
							tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
						}
						tempseries.series.clear();
						losstype = (k == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[k];
						reportrow = false;
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype].size() - 1; m++) {
							if (series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype][m] > 0) {
								reportrow = true;
								break;
							}
						}
						if (reportrow) {
							coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
						}
						for (int m = 0; m < (int)series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype].size() - 1; m++) {
							if (reportrow) {
								coveragebyseries += "<td>" + to_string(series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype][m]) + "</td>";
							}
							tempseries.series.push_back(series[i][parameters->ionsfortheoreticalspectraMS2[j]][losstype][m]);
						}
						if (reportrow) {
							coveragebyseries += "</tr>";
						}
					}
				}
				coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size()) + "\">&nbsp;</td></tr>";
			}
			coveragebyseries += "</table>";
		}*/

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


	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			for (int m = 0; m < (int)branchcyclicrotations.size(); m++) {

				for (int n = 0; n < (int)trotationsofbranchcyclicrotations[m].size(); n++) {

					// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
					// in this case the modifID is 0, and both n-terminal (*) and c-terminal (**) fragments are generated

					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].nterminal) {
						// if the start modification for n == 1 or n == 4 is nterminal, generate n-terminal ions (*)
						if (!(((n == 1) || (n == 4)) && (!parameters->searchedmodifications[trotationsofbranchcyclicrotations[m][n].startmodifID].nterminal))) {
							generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[m][n].bricks, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, m, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotationsofbranchcyclicrotations[m][n]);
						}
					}

					// all permmutations except 3 and 5 have invalid c-terms
					if (parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].cterminal && ((n == 3) || (n == 5) /*|| ((n == 0) && (trotationsofbranchcyclicrotations[m][n].endsWithBracket())) || ((n == 2) && (trotationsofbranchcyclicrotations[m][n].startsWithBracket()))*/)) {
						// if the end modification is cterminal, generate c-terminal ions (**)
						if (parameters->searchedmodifications[trotationsofbranchcyclicrotations[m][n].endmodifID].cterminal) {
							generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[m][n].bricks, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, m, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotationsofbranchcyclicrotations[m][n]);
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
	/*vector<vector<map<eFragmentIonType, map<int, vector<int> > > > > series;
	series.resize(branchcyclicrotations.size());
	for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
		series[i].resize(6);
		//series[i].resize(trotationsofbranchcyclicrotations[i].size()); // to do - potential bug

		for (int j = 0; j < (int)series[i].size(); j++) {

			for (int k = 0; k < (int)parameters->ionsfortheoreticalspectraMS2.size(); k++) {

				for (int m = -1; m < (int)parameters->neutrallossesfortheoreticalspectra.size(); m++) {

					if (m >= 0) {
						series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][parameters->neutrallossesfortheoreticalspectra[m]].resize(trotationsofbranchcyclicrotations[i][0].bricks.size());
					}
					else {
						series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][-1].resize(trotationsofbranchcyclicrotations[i][0].bricks.size());
					}

				}

			}

		}
	
		for (int j = 0; j < theoreticalpeaksrealsize; j++) {
			if (!theoreticalpeaks[j].isotope && (theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) && (theoreticalpeaks[j].seriesid >= 0)
				&& (series[i][theoreticalpeaks[j].rotationid % 6].count(theoreticalpeaks[j].iontype) == 1)
				&& (series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[j].iontype].count(theoreticalpeaks[j].neutrallosstype) == 1)) {
				if (i == theoreticalpeaks[j].rotationid / 6) {
					series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[j].iontype][theoreticalpeaks[j].neutrallosstype][theoreticalpeaks[j].seriesid]++;
				}
			}
		}
	}*/


	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {	
				clearFalseHits(series[i][j], parameters->ionsfortheoreticalspectraMS2, parameters->neutrallossesfortheoreticalspectra);
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

		string rotationlabel;
		rotationslabels.clear();

		coveragebyseries = "Linearized sequences from all ring break up points:<br/>";
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			rotationlabel = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1);
			rotationslabels.push_back(rotationlabel);
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {
				coveragebyseries += rotationlabel  + "_";
				coveragebyseries += to_string(j + 1) + " ~ ";
				coveragebyseries += bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotationsofbranchcyclicrotations[i][j].tcomposition, false);
				coveragebyseries += "<br/>";
			}
			coveragebyseries += "<br/>";
		}

		/*matchedSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectraMS2.size() > 0) {
			coveragebyseries += "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int n = 0; n < (int)series[0][0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size() - 1; n++) {
				coveragebyseries += "<td>" + to_string(n + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
				for (int j = 0; j < (int)series[i].size(); j++) {
					coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size()) + "\">" + to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_" + to_string(j + 1) + ":</td></tr>";
					for (int k = 0; k < (int)parameters->ionsfortheoreticalspectraMS2.size(); k++) {
						for (int m = -1; m < (int)parameters->neutrallossesfortheoreticalspectra.size(); m++) {
							tempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
							tempseries.name += to_string(j + 1) + "_" + parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[k]].name;
							if (m >= 0) {
								tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[m]].summary;
							}
							tempseries.series.clear();
							losstype = (m == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[m];
							reportrow = false;
							for (int n = 0; n < (int)series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][losstype].size() - 1; n++) {
								if (series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][losstype][n] > 0) {
									reportrow = true;
									break;
								}
							}
							if (reportrow) {
								coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
							}
							for (int n = 0; n < (int)series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][losstype].size() - 1; n++) {
								if (reportrow) {
									coveragebyseries += "<td>" + to_string(series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][losstype][n]) + "</td>";
								}
								tempseries.series.push_back(series[i][j][parameters->ionsfortheoreticalspectraMS2[k]][losstype][n]);
							}
							if (reportrow) {
								coveragebyseries += "</tr>";
							}
						}
					}
					coveragebyseries += "<tr><td colspan=\"" + to_string((int)series[0][0][parameters->ionsfortheoreticalspectraMS2[0]][-1].size()) + "\">&nbsp;</td></tr>";
				}
			}
			coveragebyseries += "</table>";
		}*/

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


	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];

			if (
				((hasfirstblockartificial || !parameters->regularblocksorder || (leftresiduelosstype == h2_loss)) && ((parameters->ionsfortheoreticalspectraMS2[i] == l1h_ion) || (parameters->ionsfortheoreticalspectraMS2[i] == l2h_ion)))
				||
				((hasfirstblockartificial || !parameters->regularblocksorder || (leftresiduelosstype == h2o_loss)) && ((parameters->ionsfortheoreticalspectraMS2[i] == l1oh_ion) || (parameters->ionsfortheoreticalspectraMS2[i] == l2oh_ion)))
				) {
				generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, leftresiduelosstype, hasfirstblockartificial);
			}
			if (
				((haslastblockartificial || !parameters->regularblocksorder || (rightresiduelosstype == h2_loss)) && ((parameters->ionsfortheoreticalspectraMS2[i] == r1h_ion) || (parameters->ionsfortheoreticalspectraMS2[i] == r2h_ion)))
				||
				((haslastblockartificial || !parameters->regularblocksorder || (rightresiduelosstype == h2o_loss)) && ((parameters->ionsfortheoreticalspectraMS2[i] == r1oh_ion) || (parameters->ionsfortheoreticalspectraMS2[i] == r2oh_ion)))
				) {
				generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->ionsfortheoreticalspectraMS2[i], losstype, bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, rightresiduelosstype, haslastblockartificial);
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
	/*map<eFragmentIonType, map<int, vector<int> > > series;
	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {

		for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

			if (j >= 0) {
				series[parameters->ionsfortheoreticalspectraMS2[i]][parameters->neutrallossesfortheoreticalspectra[j]].resize(intcomposition.size());
			}
			else {
				series[parameters->ionsfortheoreticalspectraMS2[i]][-1].resize(intcomposition.size());
			}

		}

	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].seriesid >= 0)
			&& (series.count(theoreticalpeaks[i].iontype) == 1)
			&& (series[theoreticalpeaks[i].iontype].count(theoreticalpeaks[i].neutrallosstype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].neutrallosstype][theoreticalpeaks[i].seriesid]++;
		}
	}*/
	
	
	// peak hits without parents are removed
	/*if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->ionsfortheoreticalspectraMS2, parameters->neutrallossesfortheoreticalspectra);

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

		/*matchedSeries tempseries;
		bool reportrow;

		if (parameters->ionsfortheoreticalspectraMS2.size() > 0) {
			coveragebyseries = "Series of matched peaks:";
			coveragebyseries += "<table cellpadding=\"0\" cellspacing=\"0\" border=\"0\" style=\"font-family:'Courier';font-size:9pt\"><tr><td></td>";
			for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectraMS2[0]][-1].size() - 1; k++) {
				coveragebyseries += "<td>" + to_string(k + 1) + "&nbsp;</td>";
			}
			coveragebyseries += "</tr>";
			for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS2.size(); i++) {
				for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {
					tempseries.name = parameters->iondefinitions[parameters->ionsfortheoreticalspectraMS2[i]].name;
					if (j >= 0) {
						tempseries.name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
					}
					tempseries.series.clear();
					losstype = (j == -1) ? -1 : parameters->neutrallossesfortheoreticalspectra[j];
					reportrow = false;
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectraMS2[i]][losstype].size() - 1; k++) {
						if (series[parameters->ionsfortheoreticalspectraMS2[i]][losstype][k] > 0) {
							reportrow = true;
							break;
						}
					}
					if (reportrow) {
						coveragebyseries += "<tr><td>" + tempseries.name + "&nbsp;</td>";
					}
					for (int k = 0; k < (int)series[parameters->ionsfortheoreticalspectraMS2[i]][losstype].size() - 1; k++) {
						if (reportrow) {
							coveragebyseries += "<td>" + to_string(series[parameters->ionsfortheoreticalspectraMS2[i]][losstype][k]) + "</td>";
						}
						tempseries.series.push_back(series[parameters->ionsfortheoreticalspectraMS2[i]][losstype][k]);
					}
					if (reportrow) {
						coveragebyseries += "</tr>";
					}
				}
			}
			coveragebyseries += "</table>";
		}*/

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


int cTheoreticalSpectrum::compareOther(cPeaksList& sortedpeaklist, bool writedescription, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid) {

	int theoreticalpeaksrealsize = 0;

	// initialize the experimental peaklist
	experimentalpeaks = sortedpeaklist;

	generatePrecursorIonForOther(theoreticalpeaksrealsize, writedescription);

	if (parameters->generateisotopepattern) {
		generateFragmentIsotopePatterns(theoreticalpeaksrealsize, writedescription, isotopeformuladesctoid);
	}

	// search the theoretical peaks in the experimental peak list
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);

	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, unmatchedpeaksinmatchedpatterns, writedescription && !parameters->reportunmatchedtheoreticalpeaks);
	}

	if (!parameters->generateisotopepattern) {
		removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);
	}

	fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(theoreticalpeaksrealsize, other, unmatchedpeaksinmatchedpatterns, parameters->reportunmatchedtheoreticalpeaks, writedescription);

	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		if (parameters->generateisotopepattern) {
			if (!parameters->reportunmatchedtheoreticalpeaks) {
				theoreticalpeaks.sortbyGroupId();
			}
			normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaksrealsize);
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;

}


void cTheoreticalSpectrum::generateMSSpectrum(bool& terminatecomputation, bool writedescription) {
	cPeak peak;
	cSummaryFormula formula;
	regex rx;

	string metalname;
	vector< vector<string> > metals;
	metals.resize(parameters->ionsfortheoreticalspectraMS1.size());

	for (int i = 0; i < (int)parameters->ionsfortheoreticalspectraMS1.size(); i++) {
		metalname = parameters->ionsfortheoreticalspectraMS1[i].name;
		for (int j = 0; j < (int)periodictablemap.getAdductElements().size(); j++) {
			rx = periodictablemap.getAdductElements()[j] + "[^a-z]";
			if (regex_search(metalname, rx)) {
				metals[i].push_back(periodictablemap.getAdductElements()[j]);
			}
		}
	}
	
	int theoreticalpeaksrealsize = 0;
	for (int i = 0; i < parameters->sequencedatabase.size(); i++) {

		if (terminatecomputation) {
			theoreticalpeaks.clear();
			theoreticalpeaksrealsize = 0;
			return;
		}

		peak.clear();
		peak.compoundid = i;
		peak.isotope = false;

		formula.clear();
		formula.setFormula(parameters->sequencedatabase[i].getSummaryFormula());

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS1.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				for (int m = 0; m < abs(parameters->precursorcharge); m++) {

					//peak.iontype = parameters->ionsfortheoreticalspectra[j]; // unused
					peak.neutrallosstype = (k >= 0) ? parameters->neutrallossesfortheoreticalspectra[k] : -1;
					peak.charge = (parameters->precursorcharge > 0) ? m + 1 : -(m + 1);

					peak.mzratio = (double)parameters->ionsfortheoreticalspectraMS1[j].multiplier*formula.getMass() + parameters->ionsfortheoreticalspectraMS1[j].massdifference;
					if (parameters->ionsfortheoreticalspectraMS1[j].positive) {
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
						peak.description = parameters->ionsfortheoreticalspectraMS1[j].name.substr(0, parameters->ionsfortheoreticalspectraMS1[j].name.size() - 1) + " ";
						peak.description += to_string(m + 1);
						if (parameters->ionsfortheoreticalspectraMS1[j].positive) {
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


void cTheoreticalSpectrum::generateFineMSSpectrum(bool& terminatecomputation) {
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
		if (terminatecomputation) {
			parameters->sequencedatabase.clear();
			return;
		}

		summary = parameters->sequencedatabase[i].getSummaryFormula() + "H";
		sequence.setSummaryFormula(summary);
		parameters->sequencedatabase.push_back(sequence);
	}

	for (int i = 0; i < parameters->sequencedatabase.size(); i++) {

		if (terminatecomputation) {
			theoreticalpeaks.clear();
			theoreticalpeaksrealsize = 0;
			return;
		}

		formula.clear();
		formula.setFormula(parameters->sequencedatabase[i].getSummaryFormula());

		for (int j = 0; j < (int)parameters->ionsfortheoreticalspectraMS1.size(); j++) {

			for (int k = -1; k < (int)parameters->neutrallossesfortheoreticalspectra.size(); k++) {

				cSummaryFormula ionformula;

				for (int m = 0; m < (double)parameters->ionsfortheoreticalspectraMS1[j].multiplier; m++) {
					tmp = formula.getSummary();
					ionformula.addFormula(tmp);
				}
				ionformula.addFormula(parameters->ionsfortheoreticalspectraMS1[j].formula);

				if (k >= 0) {
					ionformula.addFormula(parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary, true);
				}

				for (int n = 0; n < abs(parameters->precursorcharge); n++) {

					cSummaryFormula chargedformula;
					chargedformula = ionformula;

					if (parameters->ionsfortheoreticalspectraMS1[j].positive) {
						for (int p = 0; p < n; p++) {
							chargedformula.addFormula(proton);
						}
					}
					else {
						for (int p = 0; p < n; p++) {
							chargedformula.addFormula(proton, true);
						}
					}

					description = parameters->ionsfortheoreticalspectraMS1[j].name.substr(0, parameters->ionsfortheoreticalspectraMS1[j].name.size() - 1) + " ";
					description += to_string(n + 1);
					if (parameters->ionsfortheoreticalspectraMS1[j].positive) {
						description += "+";
					}
					else {
						description += "-";
					}
					if (k >= 0) {
						description += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[k]].summary;
					}
					description += " " + parameters->sequencedatabase[i].getNameWithReferenceAsHTMLString() + " (";

					cPeaksList isotopepattern = chargedformula.getIsotopePattern(parameters->fwhm, n + 1, parameters->ionsfortheoreticalspectraMS1[j].positive, true);
					for (int p = 0; p < isotopepattern.size(); p++) {
						isotopepattern[p].description = description + isotopepattern[p].description + "): ";
						//isotopepattern[p].iontype = parameters->ionsfortheoreticalspectra[j]; // unused
						isotopepattern[p].neutrallosstype = (k >= 0) ? parameters->neutrallossesfortheoreticalspectra[k] : -1;
						isotopepattern[p].charge = (parameters->precursorcharge > 0) ? n + 1 : -(n + 1);
						isotopepattern[p].groupid = groupid;
						isotopepattern[p].compoundid = i;
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


void cTheoreticalSpectrum::getHintsIndex(int id, cTheoreticalSpectrum& tsfull, cPeaksList& unmatchedpeaksinmatchedpatterns, vector< vector<int> >& hintsindex) {
	experimentalpeaks = parameters->peaklistseries[id];

	cPeaksList* tsfullpeaklist = tsfull.getTheoreticalPeaks();

	experimentalmatches.clear();
	searchForPeakPairs(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, parameters->fragmentmasserrortolerance);

	if (parameters->generateisotopepattern) {
		if (parameters->minimumpatternsize > 1) {
			removeUnmatchedPatternsFineSpectra(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		}
		if (parameters->intensitytolerance > 0) {
			removeUnmatchedPatternsByIntensityRatio(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		}
		if (parameters->mzdifftolerance > 0) {
			removeUnmatchedPatternsByMZDifference(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		}
	}

	// clear matched isotopes of unmatched monoisotopic peaks
	unmatchedpeaksinmatchedpatterns.clear();
	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, unmatchedpeaksinmatchedpatterns, false);
	}
	else {
		removeUnmatchedMetalIsotopes(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
	}

	if (parameters->generateisotopepattern) {
		removeDecoyPeakMatches(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
	}

	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			hintsindex[*it].push_back(id);

			(*tsfullpeaklist)[*it].matched = 0;
			(*tsfullpeaklist)[*it].matchedid = -1;
		}
	}
}


void cTheoreticalSpectrum::compareMSSpectrum(int id, cTheoreticalSpectrum& tsfull, cPeaksList& unmatchedpeaksinmatchedpatterns, vector< vector<int> >& hintsindex) {
	experimentalpeaks = parameters->peaklistseries[id];

	cPeaksList* tsfullpeaklist = tsfull.getTheoreticalPeaks();

	experimentalmatches.clear();
	searchForPeakPairs(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, parameters->fragmentmasserrortolerance);

	if (parameters->generateisotopepattern) {
		if (parameters->minimumpatternsize > 1) {
			removeUnmatchedPatternsFineSpectra(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		}
		if (parameters->intensitytolerance > 0) {
			removeUnmatchedPatternsByIntensityRatio(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		}
		if (parameters->mzdifftolerance > 0) {
			removeUnmatchedPatternsByMZDifference(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		}
	}

	bool lcms = (parameters->peaklistseries.size() > 1) && !((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML));

	// pre-cleaning (relative intensity threshold, minimumpatternsize)
	if (parameters->generateisotopepattern) {
		if ((parameters->minimumiontypes > 1) || (lcms && (parameters->minimumfeaturesize > 1))) {
			removeUnmatchedIsotopePatterns(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, unmatchedpeaksinmatchedpatterns, false);
		}
	}

	// mark isotopes
	if (parameters->generateisotopepattern) {
		if ((parameters->minimumiontypes > 1) || (lcms && (parameters->minimumfeaturesize > 1))) {
			tsfullpeaklist->markIsotopes();
		}
	}

	// LC-MS data or MSI data
	if (lcms || (parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
		removeUnmatchedFeatures(lcms, *tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, hintsindex/*, id*/);
	}
	// direct MS 
	else {
		if (parameters->minimumiontypes > 1) {
			removeUnmatchedCompounds(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, parameters->minimumiontypes);
		}
	}

	// clear marks of isotopes
	if (parameters->generateisotopepattern) {
		if ((parameters->minimumiontypes > 1) || (lcms && (parameters->minimumfeaturesize > 1))) {
			tsfullpeaklist->setIsotopeFlags(false);
		}
	}

	// clear matched isotopes of unmatched monoisotopic peaks
	unmatchedpeaksinmatchedpatterns.clear();
	if (parameters->generateisotopepattern) {
		removeUnmatchedIsotopePatterns(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, unmatchedpeaksinmatchedpatterns, true);
	}
	else {
		removeUnmatchedMetalIsotopes(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
	}

	// calculate scores
	if (parameters->generateisotopepattern) {
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


double cTheoreticalSpectrum::getWeightedRatioOfMatchedPeaks() const {
	return weightedpeaksratio;
}


void cTheoreticalSpectrum::generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, int neutrallosstype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<cFragmentIonType>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation, eResidueLossType leftresiduelosstype, bool hasfirstblockartificial) {
	cPeak peak;
	map<string, int> atoms;

	vector<int> currentlosses;
	vector<double> fragmentlossmass;
	vector<string> fragmentlosssummary;
	vector< map<string, int> > fragmentlossmap;

	double tmpmz;
	map<string, int> tmpmap;

	//int tmprotationid;
	//int tmpseriesid;

	bool skipcommonfragments;

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

		updateListOfNeutralLosses(bricksdatabase, bricksdatabase[intcomposition[i] - 1], currentlosses, fragmentlossmass, fragmentlosssummary, fragmentlossmap, writedescription, disablesummary);

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
				if (parameters->internalfragments) {
					if (!((peptidetype == branchcyclic) && (trotation->id == 0))) {
						continue;
					}
				}
				else {
					continue;
				}
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

		skipcommonfragments = (trotation && (i >= trotation->middlebranchstart) && (i < trotation->middlebranchend) && (peptidetype == branchcyclic) && (trotation->id == 0));

		if (!skipcommonfragments) {

			tmpmz = peak.mzratio;
			if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
				tmpmap = atoms;
			}

			//tmprotationid = peak.rotationid;
			//tmpseriesid = peak.seriesid;

			for (int j = -1; j < (int)fragmentlossmass.size(); j++) {

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
					if ((peak.neutrallosstype >= 0) || (j >= 0)) {
						peak.description += " -";
						if (j >= 0) {
							peak.description += fragmentlosssummary[j];
						}
						if (peak.neutrallosstype >= 0) {
							peak.description += parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
						}
					}
					addAdductToDescription(peak.description, parameters->metaladducts);
					peak.description += ": ";
					for (int k = 0; k <= i; k++) {
						peak.description += "[" + bricksdatabase[intcomposition[k] - 1].getAcronymsAsString() + "]";
						if (k < i) {
							peak.description += '-';
						}
					}

				}

				if (j >= 0) {
					peak.mzratio += fragmentlossmass[j];
					if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
						mergeMaps(fragmentlossmap[j], tmpmap);
					}

					//peak.rotationid = -1;
					//peak.seriesid = -1;
				}
				else {
					//peak.rotationid = tmprotationid;
					//peak.seriesid = tmpseriesid;
				}

				generateChargedFragments(peak, tmpmap, peaklistrealsize, maxcharge, writedescription, disablesummary);

				peak.mzratio = tmpmz;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					tmpmap = atoms;
				}

			}

			//peak.rotationid = tmprotationid;
			//peak.seriesid = tmpseriesid;

		}

		if (parameters->internalfragments) {
			generateInternalFragments(bricksdatabase, peak, maxcharge, peaklistrealsize, intcomposition, i, atoms, currentlosses, fragmentiontype, peptidetype, trotation, writedescription, disablesummary);
		}

	}

}


void cTheoreticalSpectrum::generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, int neutrallosstype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<cFragmentIonType>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation, eResidueLossType rightresiduelosstype, bool haslastblockartificial) {
	cPeak peak;
	map<string, int> atoms;

	vector<int> currentlosses;
	vector<double> fragmentlossmass;
	vector<string> fragmentlosssummary;
	vector< map<string, int> > fragmentlossmap;

	double tmpmz;
	map<string, int> tmpmap;

	//int tmprotationid;
	//int tmpseriesid;

	bool skipcommonfragments;

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

		updateListOfNeutralLosses(bricksdatabase, bricksdatabase[intcomposition[i] - 1], currentlosses, fragmentlossmass, fragmentlosssummary, fragmentlossmap, writedescription, disablesummary);

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
				if (parameters->internalfragments) {
					// to do where applicable
					continue;
				}
				else {
					continue;
				}
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

		// to do where applicable
		skipcommonfragments = false;

		if (!skipcommonfragments) {

			tmpmz = peak.mzratio;
			if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
				tmpmap = atoms;
			}

			//tmprotationid = peak.rotationid;
			//tmpseriesid = peak.seriesid;

			for (int j = -1; j < (int)fragmentlossmass.size(); j++) {

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
					if ((peak.neutrallosstype >= 0) || (j >= 0)) {
						peak.description += " -";
						if (j >= 0) {
							peak.description += fragmentlosssummary[j];
						}
						if (peak.neutrallosstype >= 0) {
							peak.description += parameters->neutrallossesdefinitions[peak.neutrallosstype].summary;
						}
					}
					addAdductToDescription(peak.description, parameters->metaladducts);
					peak.description += ": ";
					for (int k = (int)intcomposition.size() - 1; k >= i; k--) {
						peak.description += "[" + bricksdatabase[intcomposition[k] - 1].getAcronymsAsString() + "]";
						if (k > i) {
							peak.description += '-';
						}
					}

				}

				if (j >= 0) {
					peak.mzratio += fragmentlossmass[j];
					if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
						mergeMaps(fragmentlossmap[j], tmpmap);
					}

					//peak.rotationid = -1;
					//peak.seriesid = -1;
				}
				else {
					//peak.rotationid = tmprotationid;
					//peak.seriesid = tmpseriesid;
				}

				generateChargedFragments(peak, tmpmap, peaklistrealsize, maxcharge, writedescription, disablesummary);

				peak.mzratio = tmpmz;
				if (!disablesummary && (parameters->generateisotopepattern || writedescription)) {
					tmpmap = atoms;
				}

			}

			//peak.rotationid = tmprotationid;
			//peak.seriesid = tmpseriesid;

		}

		if (parameters->internalfragments) {
			// to do where applicable
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


vector<string>& cTheoreticalSpectrum::getLabelsOfRotations() {
	return rotationslabels;
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
	os.write((char *)&weightedpeaksratio, sizeof(double));

	os.write((char *)&unmatchedexperimentalpeakscount, sizeof(int));
	storeString(coveragebyseries, os);

	os.write((char *)&valid, sizeof(bool));
	os.write((char *)&sumofrelativeintensities, sizeof(double));

	storeStringVector(rotationslabels, os);

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
	is.read((char *)&weightedpeaksratio, sizeof(double));

	is.read((char *)&unmatchedexperimentalpeakscount, sizeof(int));
	loadString(coveragebyseries, is);

	is.read((char *)&valid, sizeof(bool));
	is.read((char *)&sumofrelativeintensities, sizeof(double));

	loadStringVector(rotationslabels, is);

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

