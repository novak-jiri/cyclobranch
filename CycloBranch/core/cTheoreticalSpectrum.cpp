#include "core/cTheoreticalSpectrum.h"
#include "core/cDeNovoGraph.h"

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


void cTheoreticalSpectrum::clearFalseHits(map<eFragmentIonType, vector<int> >& series, vector<eFragmentIonType>& fragmentions) {
	for (int i = 0; i < (int)fragmentions.size(); i++) {
		for (int j = 0; j < (int)series[fragmentions[i]].size(); j++) {

			// parent fragment ion type is not defined in fragmentions or parent of a specific ion does not exist
			if ((series.count(parameters->fragmentdefinitions[fragmentions[i]].parent) == 0) || (series[parameters->fragmentdefinitions[fragmentions[i]].parent][j] == 0)) {
				series[fragmentions[i]][j] = 0;
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
	experimentalpeaksmatched = 0;
	scrambledpeaksmatched = 0;
	if (writedescription) {
		unmatchedexperimentalpeakscount = 0;
	}
	intensityweightedscore = 0;
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		if (experimentalpeaks[i].matched > 0) {
			experimentalpeaksmatched++;
			intensityweightedscore += experimentalpeaks[i].relativeintensity;
			
			if (parameters && ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch))) {
				if (!experimentalpeaks[i].scrambled) {
					matchedions[experimentalpeaks[i].iontype]++;
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

	peak.isotope = false;

	switch (parameters->peptidetype)
	{
	case linear:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
		peak.seriesid = 0;
		starttype = (int)precursor_ion;
		endtype = (int)precursor_ion_co_loss_and_dehydrated_and_deamidated;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		break;
	case cyclic:
		peak.mzratio = 0;
		peak.seriesid = (int)intcomposition.size() - 1;
		starttype = (int)cyclic_precursor_ion;
		endtype = (int)cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated;
		break;
	case branched:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference + parameters->searchedmodifications[candidate.getMiddleModifID()].massdifference;
		peak.seriesid = 0;
		starttype = (int)precursor_ion;
		endtype = (int)precursor_ion_co_loss_and_dehydrated_and_deamidated;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		usedmodifications.insert(candidate.getMiddleModifID());
		break;
	case branchcyclic:
		peak.mzratio = parameters->searchedmodifications[candidate.getMiddleModifID()].massdifference;
		peak.seriesid = 0;
		starttype = (int)cyclic_precursor_ion;
		endtype = (int)cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated;
		usedmodifications.insert(candidate.getMiddleModifID());
		break;
	case linearpolyketide:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
		peak.seriesid = 0;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		break;
	case cyclicpolyketide:
		peak.mzratio = 0;
		peak.seriesid = (int)intcomposition.size() - 1;
		starttype = (int)cyclic_polyketide_precursor_ion;
		endtype = (int)cyclic_polyketide_precursor_ion_co_loss_dehydrated_and_deamidated;
		break;
	case other:
		break;
	default:
		break;
	}

	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
	}

	if (parameters->peptidetype == linearpolyketide) {
		double precursormass = charge(uncharge(parameters->precursormass, parameters->precursorcharge), (parameters->precursorcharge > 0)?1:-1);
		double thprecursormass = peak.mzratio + parameters->fragmentdefinitions[linear_polyketide_precursor_ion_oh_oh].massdifference;
		thprecursormass = (parameters->precursorcharge > 0)?thprecursormass:thprecursormass - 2*(H - e);
		double bestmz = fabs(precursormass - thprecursormass);
		int bestmatch = 1;

		thprecursormass = peak.mzratio + parameters->fragmentdefinitions[linear_polyketide_precursor_ion_h_oh].massdifference;
		thprecursormass = (parameters->precursorcharge > 0)?thprecursormass:thprecursormass - 2*(H - e);
		if (fabs(precursormass - thprecursormass) < bestmz) {
			bestmz = fabs(precursormass - thprecursormass);
			bestmatch = 2;
		}

		thprecursormass = peak.mzratio + parameters->fragmentdefinitions[linear_polyketide_precursor_ion_h_h].massdifference;
		thprecursormass = (parameters->precursorcharge > 0)?thprecursormass:thprecursormass - 2*(H - e);
		if (fabs(precursormass - thprecursormass) < bestmz) {
			bestmz = fabs(precursormass - thprecursormass);
			bestmatch = 3;
		}

		if (bestmatch == 1) {
			starttype = (int)linear_polyketide_precursor_ion_oh_oh;
			endtype = (int)linear_polyketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated;
		}

		if (bestmatch == 2) {
			starttype = (int)linear_polyketide_precursor_ion_h_oh;
			endtype = (int)linear_polyketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated;
		}

		if (bestmatch == 3) {
			starttype = (int)linear_polyketide_precursor_ion_h_h;
			endtype = (int)linear_polyketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated;
		}
	}
	
	double tempratio = peak.mzratio;
	for (int i = starttype; i <= endtype; i++) {
		for(set<int>::iterator j = usedmodifications.begin(); j != usedmodifications.end(); ++j) {
			for (int k = 1; k <= abs(parameters->precursorcharge); k++) {
				peak.mzratio = tempratio + parameters->fragmentdefinitions[(eFragmentIonType)i].massdifference - parameters->searchedmodifications[*j].massdifference;
				peak.iontype = (eFragmentIonType)i;

				if (writedescription) {
					string str;

					if (parameters->precursorcharge > 0) {
						peak.description = parameters->fragmentdefinitions[(eFragmentIonType)i].name;
						if (*j > 0) {
							peak.description += " (" + parameters->searchedmodifications[*j].name + " loss)";
						}
						peak.description += ":";

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
						peak.description = parameters->fragmentdefinitions[(eFragmentIonType)i].name + ":";

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

				peak.mzratio = charge(uncharge(peak.mzratio, 1), (parameters->precursorcharge > 0)?k:-k);
				peak.charge = (parameters->precursorcharge > 0)?k:-k;
			
				addPeakToList(peak, theoreticalpeaksrealsize);
				addMetalPeaks(peak, parameters->metaladducts, theoreticalpeaksrealsize, k, writedescription);
			}
		}
	}
}


void cTheoreticalSpectrum::generateScrambledIons(cBricksDatabase& bricksdatabase, bool writedescription, int& theoreticalpeaksrealsize) {
	unordered_set<string> scrambledsequences, commonsequences;
	vector<int> intcomposition;
	scrambledsequences.clear();
	//int minimumsizeofscrambledsequences = 2;
	cBrick b;

	b.setComposition(candidate.getComposition(), false);
	vector<string> stringcomposition;
	b.explodeToStringComposition(stringcomposition);

	if (stringcomposition.size() > 32) {
		// max 2^32 subsequences is tested
		return;
	}

	int numberofbricks = (int)stringcomposition.size();
	string subseq;
	bool single;
	// generate all possible combinations of blocks
	for (int i = 1; i < (int)pow(2, numberofbricks) - 1; i++) {
			subseq = "";
			single = true;
			for (int j = 0; j < numberofbricks; j++) {
				if ((i >> j) % 2 == 1) {
					if ((int)subseq.size() > 0) {
						subseq += "-";
						single = false;
					}
					subseq += stringcomposition[j];
				}
			}
			if (!single) {
				scrambledsequences.insert(subseq);
			}
	}

	// select and normalize scrambled sequences
	selectAndNormalizeScrambledSequences(scrambledsequences);

	cPeaksList scrambledpeaks;
	scrambledpeaks.resize(5000);
	int scrambledspeaksrealsize = 0;

	// generate scrambled peaks from sequences
	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		for (auto it = scrambledsequences.begin(); it != scrambledsequences.end(); ++it) {
			b.setComposition((string &)*it, false);
			intcomposition.clear();
			b.explodeToIntComposition(intcomposition);
			cPeak peak;
			peak.iontype = (eFragmentIonType)parameters->fragmentionsfortheoreticalspectra[i];
			peak.mzratio = parameters->fragmentdefinitions[peak.iontype].massdifference;
			peak.removeme = false;
			peak.scrambled = true;
			for (int i = 0; i < (int)intcomposition.size(); i++) {
				peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();
			}
			
			if (parameters->precursorcharge < 0) {
				peak.mzratio = charge(uncharge(peak.mzratio, 1), -1);
			}

			if (writedescription) {

				if (parameters->precursorcharge < 0) {
					peak.description = "1- ";
				}
				else {
					peak.description = "1+ ";
				}

				peak.description += "scrambled ";
				peak.description += parameters->fragmentdefinitions[peak.iontype].name.substr(0, 1) + to_string((int)intcomposition.size());
				if (parameters->fragmentdefinitions[peak.iontype].name.size() > 1) {
					peak.description += parameters->fragmentdefinitions[peak.iontype].name.substr(1, parameters->fragmentdefinitions[peak.iontype].name.length() - 1);
				}
				peak.description += ": ";
				for (int i = 0; i < (int)intcomposition.size(); i++) {
					peak.description += "[" + bricksdatabase[intcomposition[i] - 1].getAcronymsAsString() + "]";
					if (i < (int)intcomposition.size() - 1) {
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
	scrambledpeaks.resize(scrambledspeaksrealsize);

	// find and erase all scrambled peaks whose mz ratios collide with common fragment ions
	scrambledpeaks.sortbyMass();
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
	 	scrambledpeaks.findObsolete(theoreticalpeaks[i].mzratio);
	}
	scrambledpeaks.removeObsolete();

	// attach scrambled peaks to common peaks
	for (int i = 0; i < (int)scrambledpeaks.size(); i++) {
		addPeakToList(scrambledpeaks[i], theoreticalpeaksrealsize);
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
			if (scrambledsequences.count(b.getReverseComposition()) == 0) {
				scrambledsequences.insert(b.getComposition());
			}
		}
	}
}


eFragmentIonType cTheoreticalSpectrum::selectHigherPriorityIonTypeCID(eFragmentIonType experimentalpeakiontype, eFragmentIonType theoreticalpeakiontype) {
	if (experimentalpeakiontype != fragmentIonTypeEnd) {

		switch (experimentalpeakiontype)
		{
		case a_ion:
			if (theoreticalpeakiontype == b_ion) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case a_ion_dehydrated:
		case a_ion_deamidated:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case a_ion_dehydrated_and_deamidated:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == b_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case b_ion:
			break;
		case b_ion_dehydrated:
		case b_ion_deamidated:
			if (theoreticalpeakiontype == b_ion) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case b_ion_dehydrated_and_deamidated:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case c_ion:
			if ((theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case c_ion_dehydrated:
		case c_ion_deamidated:
			if ((theoreticalpeakiontype == c_ion) || (theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == y_ion_dehydrated) || (theoreticalpeakiontype == y_ion_deamidated) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case c_ion_dehydrated_and_deamidated:
			if ((theoreticalpeakiontype == c_ion) || (theoreticalpeakiontype == c_ion_dehydrated) || (theoreticalpeakiontype == c_ion_deamidated) || (theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == y_ion_dehydrated) || (theoreticalpeakiontype == y_ion_deamidated) || (theoreticalpeakiontype == y_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == b_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated) || (theoreticalpeakiontype == a_ion_dehydrated_and_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case x_ion:
			if ((theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case x_ion_dehydrated:
		case x_ion_deamidated:
			if ((theoreticalpeakiontype == x_ion) || (theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == y_ion_dehydrated) || (theoreticalpeakiontype == y_ion_deamidated) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case x_ion_dehydrated_and_deamidated:
			if ((theoreticalpeakiontype == x_ion) || (theoreticalpeakiontype == x_ion_dehydrated) || (theoreticalpeakiontype == x_ion_deamidated) || (theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == y_ion_dehydrated) || (theoreticalpeakiontype == y_ion_deamidated) || (theoreticalpeakiontype == y_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == b_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated) || (theoreticalpeakiontype == a_ion_dehydrated_and_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case y_ion:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case y_ion_dehydrated:
		case y_ion_deamidated:
			if ((theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == a_ion)  || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case y_ion_dehydrated_and_deamidated:
			experimentalpeakiontype = theoreticalpeakiontype;
			break;
		case z_ion:
			if ((theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case z_ion_dehydrated:
		case z_ion_deamidated:
			if ((theoreticalpeakiontype == z_ion) || (theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == y_ion_dehydrated) || (theoreticalpeakiontype == y_ion_deamidated) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case z_ion_dehydrated_and_deamidated:
			if ((theoreticalpeakiontype == z_ion) || (theoreticalpeakiontype == z_ion_dehydrated) || (theoreticalpeakiontype == z_ion_deamidated) || (theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == y_ion_dehydrated) || (theoreticalpeakiontype == y_ion_deamidated) || (theoreticalpeakiontype == y_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_dehydrated) || (theoreticalpeakiontype == b_ion_deamidated) || (theoreticalpeakiontype == b_ion_dehydrated_and_deamidated) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_dehydrated) || (theoreticalpeakiontype == a_ion_deamidated) || (theoreticalpeakiontype == a_ion_dehydrated_and_deamidated)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		default:
			break;
		}

	}
	else {
		experimentalpeakiontype = theoreticalpeakiontype;
	}

	return experimentalpeakiontype;
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

				addPeakToList(tmppeak, peaklistrealsize);
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


void cTheoreticalSpectrum::removeUnmatchedIsotopePatterns(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, cPeaksList& outputtheoreticalpeaks) {
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
					if (!theoreticalpeaks[j].decoy && (theoreticalpeaks[j].matched == 0)) {
						if (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
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

			if (!theoreticalpeaks[j].decoy && (theoreticalpeaks[j].matched == 0)) {
				if (theoreticalpeaks[j].relativeintensity*maximumexperimentalintensity / 100.0 >= parameters->minimumrelativeintensitythreshold) {
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
			for (int j = start; j <= stop; j++) {
				theoreticalpeaks[j].relativeintensity = theoreticalpeaks[j].relativeintensity*theoreticalpeaks[maximumintensityid].matchedrelativeintensity / 100.0;
			}

			// initialize next group
			groupid = theoreticalpeaks[i].groupid;
			start = i;
			stop = i;
			maximumintensityid = i;
			maximumintensity = theoreticalpeaks[i].relativeintensity;
		}

	}

	for (int j = start; j <= stop; j++) {
		theoreticalpeaks[j].relativeintensity = theoreticalpeaks[j].relativeintensity*theoreticalpeaks[maximumintensityid].matchedrelativeintensity / 100.0;
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
	for (int i = 0; i < fragmentIonTypeEnd; i++) {
		matchedions[(eFragmentIonType)i] = 0;
	}

	candidate.clear();
	experimentalpeaksmatched = 0;
	experimentalmatches.clear();
	scrambledpeaksmatched = 0;
	peakstested = 0;
	experimentalpeaksmatchedratio = 0;
	unmatchedexperimentalpeakscount = 0;
	coveragebyseries = "";
	valid = false;
	maskscore = 0;
	intensityweightedscore = 0;
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


int cTheoreticalSpectrum::compareBranched(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	cBrick brick;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;

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
			valid = false;
			if (parameters->searchedsequence.size() > 0) {
				for (int i = 0; i < (int)trotations.size(); i++) {
					if (regex_search(trotations[i].tcomposition, searchedsequence) &&
					(parameters->searchedmodifications[trotations[i].startmodifID].name.compare(parameters->searchedsequenceNtermmodif) == 0) && 
					(parameters->searchedmodifications[trotations[i].middlemodifID].name.compare(parameters->searchedsequenceTmodif) == 0) && 
					(parameters->searchedmodifications[trotations[i].endmodifID].name.compare(parameters->searchedsequenceCtermmodif) == 0)) {
						valid = true;
						break;
					}
				}
			}
		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {

		for (int j = 0; j < (int)trotations.size(); j++) {

			// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
			// in this case the modifID is 0, and both n-terminal and c-terminal fragments are generated
			// j == 2, 4, 5 are invalid
			if ((j == 0) || ((j == 1) && (parameters->searchedmodifications[trotations[j].startmodifID].nterminal)) || ((j == 3) && (parameters->searchedmodifications[trotations[j].endmodifID].cterminal))) {
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) {
					generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[j].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotations[j]);
				}
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
					generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[j].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotations[j]);
				}
			}

		}	

	}
	
	generatePrecursorIon(trotations[0].bricks, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	// coverage of series
	vector<map<eFragmentIonType, vector<int> > > series;
	series.resize(trotations.size());
	for (int i = 0; i < (int)series.size(); i++) {
		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			series[i][parameters->fragmentionsfortheoreticalspectra[j]].resize(trotations[0].bricks.size());
		}
	}
	
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)trotations.size(); i++) {	
			clearFalseHits(series[i], parameters->fragmentionsfortheoreticalspectra);
		}

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && ((series[theoreticalpeaks[i].rotationid].count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[theoreticalpeaks[i].rotationid][parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {		
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			if (writedescription) {
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		visualcoverage.clear();
		visualSeries tempseries;

		coveragebyseries = "Linearized sequences:<br/>\n";
		for (int i = 0; i < (int)trotations.size(); i++) {
			coveragebyseries += to_string(i + 1) + " ~ " + bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotations[i].tcomposition, false) + "<br/>\n";
		}
		coveragebyseries += "<br/>\n";

		coveragebyseries += "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)series.size(); i++) {
			for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
				tempseries.name = to_string(i + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name;
				coveragebyseries += tempseries.name + "  ";
				tempseries.series.clear();
				for (int k = 0; k < (int)series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 1; k++) {
					coveragebyseries += to_string(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					tempseries.series.push_back(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					if (k < (int)series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 2) {
						coveragebyseries += " ";
					}
				}
				coveragebyseries += "<br/>\n";
				visualcoverage.push_back(tempseries);
			}
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLinear(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {
	cBrick brick;
	vector<int> intcomposition;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;

	try {
		if (!regex_search(candidate.getComposition(), sequencetag)) {
			return -2;
		}
	
		if (writedescription) {
			valid = false;
			if ((parameters->searchedsequence.size() > 0) && regex_search(candidate.getComposition(), searchedsequence) &&
				(parameters->searchedmodifications[candidate.getStartModifID()].name.compare(parameters->searchedsequenceNtermmodif) == 0) && 
				(parameters->searchedmodifications[candidate.getEndModifID()].name.compare(parameters->searchedsequenceCtermmodif) == 0)) {
				valid = true;
			}	
		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}

	intcomposition.clear();
	brick.clear();
	brick.setComposition(candidate.getComposition(), false);
	brick.explodeToIntComposition(intcomposition);

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder);
		}
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder);
		}
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	// coverage of series
	map<eFragmentIonType, vector<int> > series;
	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		series[parameters->fragmentionsfortheoreticalspectra[i]].resize(intcomposition.size());
	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (series.count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->fragmentionsfortheoreticalspectra);

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid != -1) && ((series.count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {		
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			if (writedescription) {
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		visualcoverage.clear();
		visualSeries tempseries;

		coveragebyseries = "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			tempseries.name = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name;
			coveragebyseries += tempseries.name + "  ";
			tempseries.series.clear();
			for (int j = 0; j < (int)series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 1; j++) {
				coveragebyseries += to_string(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				tempseries.series.push_back(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				if (j < (int)series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 2) {
					coveragebyseries += " ";
				}
			}
			coveragebyseries += "<br/>\n";
			visualcoverage.push_back(tempseries);
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	cBrick brick;
	vector<int> intcomposition;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;

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

			valid = false;
		
			if (parameters->searchedsequence.size() > 0) {
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
							reversevalidposition = 2*r - validposition;
						}

						break;
					}
				}
		
				if (validposition != -1) {
					valid = true;
				}
			}

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

	eResidueLossType leftresiduelosstype = h2o_loss;
	bool hasfirstblockartificial = false;

	for (int i = 0; i < 2*r; i++) {

		intcomposition.clear();
		brick.clear();
		brick.setComposition(rotations[i], false);
		brick.explodeToIntComposition(intcomposition);

		eResidueLossType leftresiduelosstype = bricksdatabasewithcombinations[intcomposition[0] - 1].getResidueLossType();
		bool hasfirstblockartificial = bricksdatabasewithcombinations[intcomposition[0] - 1].isArtificial();

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[j], bricksdatabasewithcombinations, writedescription, i, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, leftresiduelosstype, hasfirstblockartificial);
		}

	}

	if (parameters->enablescrambling) {
		generateScrambledIons(bricksdatabasewithcombinations, writedescription, theoreticalpeaksrealsize);
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	// coverage of series
	vector<map<eFragmentIonType, vector<int> > > series;
	series.resize(rotations.size());
	for (int i = 0; i < (int)series.size(); i++) {
		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			series[i][parameters->fragmentionsfortheoreticalspectra[j]].resize(r);
		}
	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}


	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)rotations.size(); i++) {	
			clearFalseHits(series[i], parameters->fragmentionsfortheoreticalspectra);
		}

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && ((series[theoreticalpeaks[i].rotationid].count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[theoreticalpeaks[i].rotationid][parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {	
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
			for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
				if (parameters->fragmentionsfortheoreticalspectra[i] == parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent) {
					continue;
				}

				int j = 0;
				while (j < theoreticalpeaksrealsize) {				
					// current peak is scrambled, matched and currently tested ion type matches the ion type of the scrambled peak
					if (theoreticalpeaks[j].scrambled && (theoreticalpeaks[j].matched > 0) && (parameters->fragmentionsfortheoreticalspectra[i] == theoreticalpeaks[j].iontype)) {
						
						// parent not found
						res = experimentalpeaks.find(theoreticalpeaks[j].mzratio - parameters->fragmentdefinitions[theoreticalpeaks[j].iontype].massdifference + parameters->fragmentdefinitions[parameters->fragmentdefinitions[theoreticalpeaks[j].iontype].parent].massdifference, parameters->fragmentmasserrortolerance);
						if (res == -1) {
							experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
							experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

							theoreticalpeaks[j].matched--;
							theoreticalpeaks[j].matchedid = -1;

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
								experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
								experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

								theoreticalpeaks[j].matched--;
								theoreticalpeaks[j].matchedid = -1;
							}
							else {
								j++;
							}
						}
					}
					else {
						j++;
					}
				}
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalmatches.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			if (writedescription) {
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			if (experimentalpeaks[i].iontype != fragmentIonTypeEnd) {
				if (theoreticalpeaks[*it].scrambled == experimentalpeaks[i].scrambled) {
					experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
				}
				else {
					if (experimentalpeaks[i].scrambled && !theoreticalpeaks[*it].scrambled) {
						experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						experimentalpeaks[i].scrambled = theoreticalpeaks[*it].scrambled;
					}
				}
			}
			else {
				experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
				experimentalpeaks[i].scrambled = theoreticalpeaks[*it].scrambled;
			}
		}
	}


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		visualSeries tempseries;
		visualcoverage.clear();

		coveragebyseries = "Linearized sequences from all ring break up points:<br/>\n";
		for (int i = 0; i < (int)rotations.size(); i++) {
			if (i == r) {
				coveragebyseries += "<br/>\n";
			}
			coveragebyseries += to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + " " + bricksdatabasewithcombinations.getAcronymName(rotations[i], false) + "<br/>\n";
		}
		coveragebyseries += "<br/>\n";

		coveragebyseries += "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)series.size(); i++) {
			if (i == r) {
				coveragebyseries += "<br/>\n";
			}
			for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
				tempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name;
				coveragebyseries += tempseries.name + "  ";
				tempseries.series.clear();
				for (int k = 0; k < (int)series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 1; k++) {
					coveragebyseries += to_string(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					tempseries.series.push_back(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					if (k < (int)series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 2) {
						coveragebyseries += " ";
					}
				}
				coveragebyseries += "<br/>\n";
				visualcoverage.push_back(tempseries);
			}
		}

	}
		

	vector<int> mask; // peaks converted to letters
	vector<int> tempvector;
	for (int k = 0; k < r; k++) {
		mask.push_back(0);
		tempvector.push_back(0);
	}

	for (int i = 0; i < (int)series.size(); i++) {

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {

			for (int k = 0; k < r; k++) {
				tempvector[k] = 0;
			}
				
			for (int k = 0; k < r; k++) {
				// the first peak determines the first letter
				if ((k == 0) && (series[i][parameters->fragmentionsfortheoreticalspectra[j]][k] > 0)) {
					tempvector[0] = 1;
				}

				// the pre-last peak and the precursor determine the last letter
				if ((k == r - 2) && (series[i][parameters->fragmentionsfortheoreticalspectra[j]][k] > 0)) {
					tempvector[r - 1] = 1;
				}

				if ((k >= 0) && (k < r - 2) && (series[i][parameters->fragmentionsfortheoreticalspectra[j]][k] > 0) && (series[i][parameters->fragmentionsfortheoreticalspectra[j]][k + 1] > 0)) {
					tempvector[k + 1] = 1;
				}			
			}

			if (i < r) {			
				for (int k = 0; k < r; k++) {
					mask[k] += tempvector[(k + r - i) % r];
				}		
			}
			else {
				reverse(tempvector.begin(),tempvector.end());

				for (int k = 0; k < r; k++) {
					mask[k] += tempvector[(k + i) % r];
				}
			}

		}

	}

	maskscore = 0;
	for (int k = 0; k < (int)mask.size(); k++) {
		maskscore += mask[k];
	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareBranchCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	vector<cCandidate> branchcyclicrotations;

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
			valid = false;
			if (parameters->searchedsequence.size() > 0) {
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
							reversevalidposition = 2*r - validposition;
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
			}

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


	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {

		for (int j = 0; j < (int)branchcyclicrotations.size(); j++) {

			for (int k = 0; k < (int)trotationsofbranchcyclicrotations[j].size(); k++) {

				// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
				// in this case the modifID is 0, and both n-terminal (*) and c-terminal (**) fragments are generated

				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) {
					// if the start modification for k == 1 or k == 4 is nterminal, generate n-terminal ions (*)
					if (!(((k == 1) || (k == 4)) && (!parameters->searchedmodifications[trotationsofbranchcyclicrotations[j][k].startmodifID].nterminal))) {
						generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[j][k].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, j, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotationsofbranchcyclicrotations[j][k]);
					}
				}

				// all permmutations except 3 and 5 have invalid c-terms
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal && ((k == 3) || (k == 5) /*|| ((k == 0) && (trotationsofbranchcyclicrotations[j][k].endsWithBracket())) || ((k == 2) && (trotationsofbranchcyclicrotations[j][k].startsWithBracket()))*/)) {
					// if the end modification is cterminal, generate c-terminal ions (**)
					if (parameters->searchedmodifications[trotationsofbranchcyclicrotations[j][k].endmodifID].cterminal) {
						generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[j][k].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, j, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, &trotationsofbranchcyclicrotations[j][k]);
					}
				}

			}	

		}

	}

	generatePrecursorIon(trotationsofbranchcyclicrotations[0][0].bricks, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	// coverage of series
	vector<vector<map<eFragmentIonType, vector<int> > > > series;
	series.resize(branchcyclicrotations.size());
	for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
		series[i].resize(6);
		//series[i].resize(trotationsofbranchcyclicrotations[i].size()); // to do - potential bug

		for (int j = 0; j < (int)series[i].size(); j++) {
			for (int k = 0; k < (int)parameters->fragmentionsfortheoreticalspectra.size(); k++) {
				series[i][j][parameters->fragmentionsfortheoreticalspectra[k]].resize(trotationsofbranchcyclicrotations[i][0].bricks.size());
			}
		}
	
		for (int j = 0; j < theoreticalpeaksrealsize; j++) {
			if (!theoreticalpeaks[j].isotope && (theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) && (series[i][theoreticalpeaks[j].rotationid % 6].count(theoreticalpeaks[j].iontype) == 1)) {
				if (i == theoreticalpeaks[j].rotationid / 6) {
					series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[j].iontype][theoreticalpeaks[j].seriesid]++;
				}
			}
		}
	}


	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {	
				clearFalseHits(series[i][j], parameters->fragmentionsfortheoreticalspectra);
			}

			for (int j = 0; j < theoreticalpeaksrealsize; j++) {
				if (!theoreticalpeaks[j].isotope && (theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) && (i == theoreticalpeaks[j].rotationid / 6) && ((series[i][theoreticalpeaks[j].rotationid % 6].count(parameters->fragmentdefinitions[theoreticalpeaks[j].iontype].parent) == 0) || (series[i][theoreticalpeaks[j].rotationid % 6][parameters->fragmentdefinitions[theoreticalpeaks[j].iontype].parent][theoreticalpeaks[j].seriesid] == 0))) {		
					experimentalmatches[theoreticalpeaks[j].matchedid].erase(experimentalmatches[theoreticalpeaks[j].matchedid].find(j));
					experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

					theoreticalpeaks[j].matched--;
					theoreticalpeaks[j].matchedid = -1;
				}
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			if (writedescription) {
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		visualSeries tempseries;
		visualcoverage.clear();

		coveragebyseries = "Linearized sequences from all ring break up points:<br/>\n";
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)trotationsofbranchcyclicrotations[i].size(); j++) {
				coveragebyseries += to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
				coveragebyseries += to_string(j + 1) + " ~ ";
				coveragebyseries += bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotationsofbranchcyclicrotations[i][j].tcomposition, false);
				coveragebyseries += "<br/>\n";
			}
			coveragebyseries += "<br/>\n";
		}

		coveragebyseries += "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
			for (int j = 0; j < (int)series[i].size(); j++) {
				for (int k = 0; k < (int)parameters->fragmentionsfortheoreticalspectra.size(); k++) {
					tempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
					tempseries.name += to_string(j + 1) + "_" + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[k]].name;
					coveragebyseries += tempseries.name + "  ";
					tempseries.series.clear();
					for (int m = 0; m < (int)series[i][j][parameters->fragmentionsfortheoreticalspectra[k]].size() - 1; m++) {
						coveragebyseries += to_string(series[i][j][parameters->fragmentionsfortheoreticalspectra[k]][m]);
						tempseries.series.push_back(series[i][j][parameters->fragmentionsfortheoreticalspectra[k]][m]);
						if (m < (int)series[i][j][parameters->fragmentionsfortheoreticalspectra[k]].size() - 2) {
							coveragebyseries += " ";
						}
					}
					coveragebyseries += "<br/>\n";
					visualcoverage.push_back(tempseries);
				}
			}
		}

	}


	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLinearPolyketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {
	
	if (!candidate.checkKetideSequence(bricksdatabasewithcombinations, parameters->peptidetype, parameters->regularblocksorder)) {
		return -2;
	}

	cBrick brick;
	vector<int> intcomposition;
	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;

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
			valid = false;
			if ((parameters->searchedsequence.size() > 0) && 
				((regex_search(candidate.getComposition(), searchedsequence) &&
				(parameters->searchedmodifications[candidate.getStartModifID()].name.compare(parameters->searchedsequenceNtermmodif) == 0) && 
				(parameters->searchedmodifications[candidate.getEndModifID()].name.compare(parameters->searchedsequenceCtermmodif) == 0))
				||
				(regex_search(revertedcandidate.getComposition(), searchedsequence) &&
				(parameters->searchedmodifications[revertedcandidate.getStartModifID()].name.compare(parameters->searchedsequenceNtermmodif) == 0) && 
				(parameters->searchedmodifications[revertedcandidate.getEndModifID()].name.compare(parameters->searchedsequenceCtermmodif) == 0)))
				) {
				valid = true;
			}	
		}
	} 
	catch (regex_error& /*e*/) {
		return -2;
	}

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		if (
			((hasfirstblockartificial || !parameters->regularblocksorder || (leftresiduelosstype == h2_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l1h_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l2h_ion)))
			|| 
			((hasfirstblockartificial || !parameters->regularblocksorder || (leftresiduelosstype == h2o_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l1oh_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l2oh_ion)))
			) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, leftresiduelosstype, hasfirstblockartificial);
		}
		if (
			((haslastblockartificial || !parameters->regularblocksorder || (rightresiduelosstype == h2_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r1h_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r2h_ion)))
			|| 
			((haslastblockartificial || !parameters->regularblocksorder || (rightresiduelosstype == h2o_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r1oh_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r2oh_ion)))
			) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, parameters->regularblocksorder, 0, rightresiduelosstype, haslastblockartificial);
		}
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	experimentalmatches.clear();
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, parameters->fragmentmasserrortolerance);


	// coverage of series
	map<eFragmentIonType, vector<int> > series;
	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		series[parameters->fragmentionsfortheoreticalspectra[i]].resize(intcomposition.size());
	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (series.count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}
	
	
	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->fragmentionsfortheoreticalspectra);

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if (!theoreticalpeaks[i].isotope && (theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid != -1) && ((series.count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {		
				experimentalmatches[theoreticalpeaks[i].matchedid].erase(experimentalmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (auto it = experimentalmatches[i].begin(); it != experimentalmatches[i].end(); ++it) {
			if (writedescription) {
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedrelativeintensity = experimentalpeaks[i].relativeintensity;
				theoreticalpeaks[*it].matchedabsoluteintensity = experimentalpeaks[i].absoluteintensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

		theoreticalpeaks.fillOrderIDs();

		visualcoverage.clear();
		visualSeries tempseries;

		coveragebyseries = "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			tempseries.name = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name;
			coveragebyseries += tempseries.name + "  ";
			tempseries.series.clear();
			for (int j = 0; j < (int)series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 1; j++) {
				coveragebyseries += to_string(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				tempseries.series.push_back(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				if (j < (int)series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 2) {
					coveragebyseries += " ";
				}
			}
			coveragebyseries += "<br/>\n";
			visualcoverage.push_back(tempseries);
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;

}


int cTheoreticalSpectrum::compareCyclicPolyketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	if (!candidate.checkKetideSequence(bricksdatabasewithcombinations, parameters->peptidetype, parameters->regularblocksorder)) {
		return -2;
	}

	return compareCyclic(sortedpeaklist, bricksdatabasewithcombinations, writedescription, sequencetag, searchedsequence);

}


void cTheoreticalSpectrum::generateMSSpectrum(bool writedescription) {
	cPeak peak;
	cSummaryFormula formula;
	regex rx;

	string metalname;
	vector< vector<string> > metals;
	metals.resize(parameters->fragmentionsfortheoreticalspectra.size());

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		metalname = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name;
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

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {

			for (int k = 0; k < abs(parameters->precursorcharge); k++) {

				peak.mzratio = (double)parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].multiplier*formula.getMass() + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].massdifference;
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive) {
					peak.mzratio += (double)k*(H - e);
				}
				else {
					peak.mzratio -= (double)k*(H - e);
				}
				peak.mzratio = peak.mzratio/(double)(k + 1);

				if (writedescription) {
					peak.description = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(0, parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.size() - 1) + " ";
					peak.description += to_string(k + 1);
					if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive) {
						peak.description += "+";
					}
					else {
						peak.description += "-";
					}
					peak.description += " " + parameters->sequencedatabase[i].getNameWithReferenceAsHTMLString() + " (" + parameters->sequencedatabase[i].getSummaryFormula() + "): ";
				}
				
				addPeakToList(peak, theoreticalpeaksrealsize);
				addMetalPeaks(peak, metals[j], theoreticalpeaksrealsize, k + 1, writedescription);				

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

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {

			cSummaryFormula ionformula;
			for (int k = 0; k < (double)parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].multiplier; k++) {
				tmp = formula.getSummary();
				ionformula.addFormula(tmp);
			}
			ionformula.addFormula(parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].summary);

			for (int k = 0; k < abs(parameters->precursorcharge); k++) {

				cSummaryFormula chargedformula;
				chargedformula = ionformula;

				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive) {
					for (int m = 0; m < k; m++) {
						chargedformula.addFormula(proton);
					}
				}
				else {
					for (int m = 0; m < k; m++) {
						chargedformula.addFormula(proton, true);
					}
				}

				description = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(0, parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.size() - 1) + " ";
				description += to_string(k + 1);
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive) {
					description += "+";
				}
				else {
					description += "-";
				}
				description += " " + parameters->sequencedatabase[i].getNameWithReferenceAsHTMLString() + " (";
				
				cPeaksList isotopepattern = chargedformula.getIsotopePattern(parameters->fwhm, k + 1, parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive);
				for (int m = 0; m < isotopepattern.size(); m++) {
					isotopepattern[m].description = description + isotopepattern[m].description + "): ";
					isotopepattern[m].groupid = groupid;
					isotopepattern[m].decoy = parameters->sequencedatabase[i].isDecoy();
					addPeakToList(isotopepattern[m], theoreticalpeaksrealsize);
				}

				groupid++;
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
		removeUnmatchedIsotopePatterns(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks, unmatchedpeaksinmatchedpatterns);
		targetscores.clear();
		decoyscores.clear();
		calculateEnvelopeScores(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);
		removeDecoyPeakMatches(*tsfullpeaklist, tsfull.getNumberOfPeaks(), experimentalpeaks);		
	}

	theoreticalpeaks.clear();
	vector<set<int> > updatedexperimentalmatches;
	updatedexperimentalmatches.resize(experimentalmatches.size());

	// fill annotations of peaks
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


void cTheoreticalSpectrum::finalizeMSSpectrum(cPeaksList& unmatchedpeaksinmatchedpatterns) {
	if (parameters->generateisotopepattern) {
		theoreticalpeaks.attach(unmatchedpeaksinmatchedpatterns);
	}
	
	theoreticalpeaks.fillOrderIDs();

	if (parameters->generateisotopepattern) {
		theoreticalpeaks.sortbyGroupId();
		normalizeTheoreticalIntensities(theoreticalpeaks, theoreticalpeaks.size());
	}

	computeStatistics(true);

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


int cTheoreticalSpectrum::getNumberOfMatchedPeaks(eFragmentIonType iontype) const {
	return matchedions.at(iontype);
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaksYB() const {
	return matchedions.at(y_ion)+matchedions.at(b_ion);
}


double cTheoreticalSpectrum::getRatioOfMatchedPeaks() {
	return experimentalpeaksmatchedratio;
}


void cTheoreticalSpectrum::generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation, eResidueLossType leftresiduelosstype, bool hasfirstblockartificial) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	peak.mzratio = parameters->fragmentdefinitions[fragmentiontype].massdifference;
	if ((peptidetype == linear) || (peptidetype == linearpolyketide)) {
		peak.mzratio += searchedmodifications[candidate.getStartModifID()].massdifference;
	}
	if ((peptidetype == branched) || (peptidetype == branchcyclic)) {
		peak.mzratio += searchedmodifications[trotation->startmodifID].massdifference;
	}

	peak.iontype = fragmentiontype;
	peak.scrambled = false;
	peak.isotope = false;

	switch (peptidetype)
	{
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

	for (int i = 0; i < (int)intcomposition.size() - 1; i++) {
		peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();


		if ((peptidetype == branched) || (peptidetype == branchcyclic)) {		
			if (i == trotation->middlebranchstart) {
				peak.mzratio += searchedmodifications[trotation->middlemodifID].massdifference;
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
			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l1h_ion) && (i % 2 == 0)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l2h_ion) && (i % 2 == 1)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l1oh_ion) && (i % 2 == 1)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l2oh_ion) && (i % 2 == 0)) {
				continue;
			}	
		}

		if (peptidetype == linearpolyketide) {
			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && ((parameters->fragmentdefinitions[fragmentiontype].parent == l1oh_ion) || (parameters->fragmentdefinitions[fragmentiontype].parent == l2oh_ion))) {
				continue;
			}

			if (!hasfirstblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->fragmentdefinitions[candidate.getStartIonType()].nterminal && ((parameters->fragmentdefinitions[fragmentiontype].parent == l1oh_ion) || (parameters->fragmentdefinitions[fragmentiontype].parent == l2oh_ion))) {
				continue;
			}

			if (!hasfirstblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->fragmentdefinitions[candidate.getStartIonType()].cterminal && ((parameters->fragmentdefinitions[fragmentiontype].parent == l1h_ion) || (parameters->fragmentdefinitions[fragmentiontype].parent == l2h_ion))) {
				continue;
			}
		}
		
		if (regularblocksorder && (peptidetype == cyclicpolyketide)) {

			//if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l0h_ion)) { // b+2H ion is generated instead of b-2H
			//	continue;
			//}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l1h_ion) && (i % 2 == 0)) {
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l2h_ion) && (i % 2 == 1)) {
				continue;
			}

			//if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l0h_ion) && (i % 2 == 1)) {
			//	continue;
			//}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l1h_ion) && (i % 2 == 0)) { // ok - even numbers of blocks => always b-ion
				continue;
			}

			if (!hasfirstblockartificial && (leftresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == l2h_ion)) { // b-2H ion is generated instead of b+2H
				continue;
			}	
		}

		peak.seriesid = i;

		if (writedescription) {

			peak.description = "";
			if ((peptidetype == cyclic) || (peptidetype == cyclicpolyketide)) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
			}
			if (peptidetype == branched) {
				peak.description += to_string(peak.rotationid + 1) + "_";
			}
			if (peptidetype == branchcyclic) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
				peak.description += to_string(trotation->id + 1) + "_";
			}

			if ((peptidetype == linearpolyketide) || (peptidetype == cyclicpolyketide)) {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 2) + to_string(i + 1);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 2) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(2, parameters->fragmentdefinitions[fragmentiontype].name.length() - 2);
				}
			}
			else {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 1) + to_string(i + 1);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 1) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(1, parameters->fragmentdefinitions[fragmentiontype].name.length() - 1);
				}
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
				if (writedescription) {
					if (parameters->precursorcharge > 0) {
						peak.description = to_string(j) + "+ " + tempdescription;
					}
					else {
						peak.description = to_string(j) + "- " + tempdescription;
					}
				}

				addPeakToList(peak, peaklistrealsize);
				addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, j, writedescription);

			}
			peak.mzratio = tempratio;
			if (writedescription) {
				peak.description = tempdescription;
			}

		}

	}

}


void cTheoreticalSpectrum::generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation, eResidueLossType rightresiduelosstype, bool haslastblockartificial) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	peak.mzratio = parameters->fragmentdefinitions[fragmentiontype].massdifference;
	if ((peptidetype == linear) || (peptidetype == linearpolyketide)) {
		peak.mzratio += searchedmodifications[candidate.getEndModifID()].massdifference;
	}
	if ((peptidetype == branched) || (peptidetype == branchcyclic)) {
		peak.mzratio += searchedmodifications[trotation->endmodifID].massdifference;
	}

	peak.iontype = fragmentiontype;
	peak.scrambled = false;
	peak.isotope = false;

	switch (peptidetype)
	{
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

	int order = -1;
	for (int i = (int)intcomposition.size() - 1; i > 0; i--) {
		peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();
		order++;

		if ((peptidetype == branched) || (peptidetype == branchcyclic)) {		
			if (i == trotation->middlebranchend) {
				peak.mzratio += searchedmodifications[trotation->middlemodifID].massdifference;
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
			if (!haslastblockartificial && (rightresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == r1h_ion) && (order % 2 == 0)) {
				continue;
			}

			if (!haslastblockartificial && (rightresiduelosstype == h2_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == r2h_ion) && (order % 2 == 1)) {
				continue;
			}

			if (!haslastblockartificial && (rightresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == r1oh_ion) && (order % 2 == 1)) {
				continue;
			}

			if (!haslastblockartificial && (rightresiduelosstype == h2o_loss) && (parameters->fragmentdefinitions[fragmentiontype].parent == r2oh_ion) && (order % 2 == 0)) {
				continue;
			}		
		}

		if (peptidetype == linearpolyketide) {
			if (!haslastblockartificial && (rightresiduelosstype == h2_loss) && ((parameters->fragmentdefinitions[fragmentiontype].parent == r1oh_ion) || (parameters->fragmentdefinitions[fragmentiontype].parent == r2oh_ion))) {
				continue;
			}

			//if (!haslastblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->fragmentdefinitions[candidate.getStartIonType()].nterminal && ((parameters->fragmentdefinitions[fragmentiontype].parent == r1oh_ion) || (parameters->fragmentdefinitions[fragmentiontype].parent == r2oh_ion))) {
			//	continue;
			//}

			//if (!haslastblockartificial && (candidate.getStartIonType() != fragmentIonTypeEnd) && parameters->fragmentdefinitions[candidate.getStartIonType()].cterminal && ((parameters->fragmentdefinitions[fragmentiontype].parent == r1h_ion) || (parameters->fragmentdefinitions[fragmentiontype].parent == r2h_ion))) {
			//	continue;
			//}
		}

		peak.seriesid = (int)intcomposition.size() - i - 1;

		if (writedescription) {

			peak.description = "";
			if (peptidetype == cyclicpolyketide) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
			}
			//if (peptidetype == cyclic) {
			//	peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
			//}
			if (peptidetype == branched) {
				peak.description += to_string(peak.rotationid + 1) + "_";
			}
			if (peptidetype == branchcyclic) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
				peak.description += to_string(trotation->id + 1) + "_";
			}

			if ((peptidetype == linearpolyketide) || (peptidetype == cyclicpolyketide)) {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 2) + to_string((int)intcomposition.size() - i);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 2) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(2, parameters->fragmentdefinitions[fragmentiontype].name.length() - 2);
				}
			}
			else {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 1) + to_string((int)intcomposition.size() - i);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 1) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(1, parameters->fragmentdefinitions[fragmentiontype].name.length() - 1);
				}
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
				if (writedescription) {
					if (parameters->precursorcharge > 0) {
						peak.description = to_string(j) + "+ " + tempdescription;
					}
					else {
						peak.description = to_string(j) + "- " + tempdescription;
					}
				}
				
				addPeakToList(peak, peaklistrealsize);
				addMetalPeaks(peak, parameters->metaladducts, peaklistrealsize, j, writedescription);				

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


double cTheoreticalSpectrum::getWeightedIntensityScore() const {
	return intensityweightedscore;
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


int cTheoreticalSpectrum::getNumberOfMatchedBricks() const {
	return maskscore;
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
	cPeaksList updatedunmatchedpeaks;
	size = (int)unmatchedpeaksinmatchedpatterns.size();
	for (int i = 0; i < size; i++) {
		if (updatedgroupids.count(unmatchedpeaksinmatchedpatterns[i].groupid) == 1) {
			updatedunmatchedpeaks.add(unmatchedpeaksinmatchedpatterns[i]);
		}
	}
	unmatchedpeaksinmatchedpatterns = updatedunmatchedpeaks;
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
		os.write((char *)&it->second, sizeof(int));
	}

	os.write((char *)&peakstested, sizeof(int));
	os.write((char *)&experimentalpeaksmatchedratio, sizeof(double));

	os.write((char *)&unmatchedexperimentalpeakscount, sizeof(int));
	storeString(coveragebyseries, os);

	os.write((char *)&valid, sizeof(bool));
	os.write((char *)&intensityweightedscore, sizeof(double));
	os.write((char *)&maskscore, sizeof(int));

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
		is.read((char *)&number, sizeof(int));
		matchedions[iontype] = number;
	}

	is.read((char *)&peakstested, sizeof(int));
	is.read((char *)&experimentalpeaksmatchedratio, sizeof(double));

	is.read((char *)&unmatchedexperimentalpeakscount, sizeof(int));
	loadString(coveragebyseries, is);

	is.read((char *)&valid, sizeof(bool));
	is.read((char *)&intensityweightedscore, sizeof(double));
	is.read((char *)&maskscore, sizeof(int));

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

