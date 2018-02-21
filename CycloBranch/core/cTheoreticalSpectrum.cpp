#include "core/cTheoreticalSpectrum.h"
#include "core/cDeNovoGraph.h"

#include "gui/cMainThread.h"


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


void cTheoreticalSpectrum::searchForPeakPairs(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, vector<set<int> >& experimentalpeakmatches, double fragmentmasserrortolerance) {
	experimentalpeakmatches.resize(experimentalpeaks.size());
	int left, right, middle;
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		left = 0;
		right = experimentalpeaks.size() - 1;
		while (left <= right) {
			middle = (left + right) / 2;
			if (isInPpmMassErrorTolerance(experimentalpeaks[middle].mzratio, theoreticalpeaks[i].mzratio, fragmentmasserrortolerance)) {
				experimentalpeakmatches[middle].insert(i);
				experimentalpeaks[middle].matched++;

				theoreticalpeaks[i].matched++;
				theoreticalpeaks[i].matchedid = middle;
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
		unmatchedpeaks.clear();
	}
	intensityweightedscore = 0;
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		if (experimentalpeaks[i].matched > 0) {
			experimentalpeaksmatched++;
			intensityweightedscore += experimentalpeaks[i].intensity;
			
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
				unmatchedpeaks.add(experimentalpeaks[i]);
			}
		}
	}
	peakstested = experimentalpeaks.size();
	experimentalpeaksmatchedratio = ((double)experimentalpeaksmatched) / ((double)experimentalpeaks.size());
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
#if OLIGOKETIDES == 1
	case linearoligoketide:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
		peak.seriesid = 0;

		switch (candidate.getKetidePrecursorType(bricksdatabasewithcombinations))
		{
		case ketide_precursor_h2o:
			starttype = (int)linear_oligoketide_precursor_ion_h_oh;
			endtype = (int)linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated;
			break;
		case ketide_precursor_h2:
			starttype = (int)linear_oligoketide_precursor_ion_h_h;
			endtype = (int)linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated;
			break;
		case ketide_precursor_h2o2:
			starttype = (int)linear_oligoketide_precursor_ion_oh_oh;
			endtype = (int)linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated;
			break;
		default:
			break;
		}

		if (candidate.hasFirstBrickArtificial(bricksdatabasewithcombinations) || candidate.hasLastBrickArtificial(bricksdatabasewithcombinations)) {
			starttype = (int)linear_oligoketide_precursor_ion_h_h;
			endtype = (int)linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated;
		}

		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		break;
	case cyclicoligoketide:
		peak.mzratio = 0;
		peak.seriesid = (int)intcomposition.size() - 1;
		starttype = (int)cyclic_oligoketide_precursor_ion;
		endtype = (int)cyclic_oligoketide_precursor_ion_co_loss_dehydrated_and_deamidated;
		break;
#endif
	case linearpolysaccharide:
		peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
		peak.seriesid = 0;
		starttype = (int)precursor_ion;
		endtype = (int)precursor_ion_co_loss_and_dehydrated_and_deamidated;
		usedmodifications.insert(candidate.getStartModifID());
		usedmodifications.insert(candidate.getEndModifID());
		break;
	case other:
		break;
	default:
		break;
	}

	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
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
				addMetalPeaks(peak, theoreticalpeaksrealsize, k, writedescription);
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
		for (unordered_set<string>::iterator it = scrambledsequences.begin(); it != scrambledsequences.end(); ++it) {
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

	for (unordered_set<string>::iterator it = temp.begin(); it != temp.end(); ++it) {
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


void cTheoreticalSpectrum::addAdductToDescription(string& description) {
	if (parameters->precursorAdductHasLi) {
		description += "_Li";
	}

	if (parameters->precursorAdductHasNa) {
		description += "_Na";
	}

	if (parameters->precursorAdductHasMg) {
		description += "_Mg";
	}

	if (parameters->precursorAdductHasAl) {
		description += "_Al";
	}

	if (parameters->precursorAdductHasK) {
		description += "_K";
	}

	if (parameters->precursorAdductHasCa) {
		description += "_Ca";
	}

	if (parameters->precursorAdductHasMn) {
		description += "_Mn";
	}

	if (parameters->precursorAdductHasFe) {
		description += "_Fe";
	}

	if (parameters->precursorAdductHasCo) {
		description += "_Co";
	}

	if (parameters->precursorAdductHasNi) {
		description += "_Ni";
	}

	if (parameters->precursorAdductHasCu) {
		description += "_Cu";
	}

	if (parameters->precursorAdductHasZn) {
		description += "_Zn";
	}

	if (parameters->precursorAdductHasGa) {
		description += "_Ga";
	}
}


int cTheoreticalSpectrum::countIsotopesOfMetals(cParameters* parameters) {
	int count = 0;
	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		count += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].numberofisotopes;
	}
	return count;
}


void cTheoreticalSpectrum::addMetalPeaks(cPeak& peak, int& peaklistrealsize, int charge, bool writedescription) {
	if (parameters->precursorAdductHasLi) {
		cPeak tmppeak;

		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio -= (Li - Li6)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Li"), 2, "6Li");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasMg) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Mg25 - Mg)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Mg"), 2, "25Mg");
		}
		addPeakToList(tmppeak, peaklistrealsize);
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Mg26 - Mg)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Mg"), 2, "26Mg");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasK) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (K41 - K)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("K"), 1, "41K");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasCa) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Ca44 - Ca)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Ca"), 2, "44Ca");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasFe) {
		cPeak tmppeak;

		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio -= (Fe - Fe54)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Fe"), 2, "54Fe");
		}
		addPeakToList(tmppeak, peaklistrealsize);

		/*tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Fe57 - Fe)/(double)j;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find(':') - 2, 2, "57Fe");
		}
		addPeakToList(tmppeak, peaklistrealsize);*/
	}

	if (parameters->precursorAdductHasNi) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Ni60 - Ni)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Ni"), 2, "60Ni");
		}
		addPeakToList(tmppeak, peaklistrealsize);
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Ni61 - Ni)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Ni"), 2, "61Ni");
		}
		addPeakToList(tmppeak, peaklistrealsize);
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Ni62 - Ni)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Ni"), 2, "62Ni");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasCu) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Cu65 - Cu)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Cu"), 2, "65Cu");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasZn) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Zn66 - Zn)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Zn"), 2, "66Zn");
		}
		addPeakToList(tmppeak, peaklistrealsize);
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Zn67 - Zn)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Zn"), 2, "67Zn");
		}
		addPeakToList(tmppeak, peaklistrealsize);
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Zn68 - Zn)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Zn"), 2, "68Zn");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}

	if (parameters->precursorAdductHasGa) {
		cPeak tmppeak;
				
		tmppeak = peak;
		tmppeak.isotope = true;
		tmppeak.mzratio += (Ga71 - Ga)/(double)charge;
		if (writedescription) {
			tmppeak.description.replace(tmppeak.description.find("Ga"), 2, "71Ga");
		}
		addPeakToList(tmppeak, peaklistrealsize);
	}
}


void cTheoreticalSpectrum::removeUnmatchedMetalIsotopes(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, vector<set<int> >& experimentalpeakmatches) {
	int lastparent = 0;
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if (!theoreticalpeaks[i].isotope) {
			lastparent = i;
			continue;
		}

		if ((theoreticalpeaks[lastparent].matched == 0) && (theoreticalpeaks[i].matched > 0)) {
			experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
			experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

			theoreticalpeaks[i].matched--;
			theoreticalpeaks[i].matchedid = -1;
		}
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
	for (int i = 0; i < fragmentIonTypeEnd; i++) {
		matchedions[(eFragmentIonType)i] = 0;
	}

	candidate.clear();
	experimentalpeaksmatched = 0;
	scrambledpeaksmatched = 0;
	peakstested = 0;
	experimentalpeaksmatchedratio = 0;
	unmatchedpeaks.clear();
	coveragebyseries = "";
	valid = false;
	maskscore = 0;
	intensityweightedscore = 0;
	visualcoverage.clear();
	validposition = -1;
	reversevalidposition = -1;
	
	seriescompleted = 1;
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
					generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[j].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, &trotations[j]);
				}
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
					generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotations[j].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, &trotations[j]);
				}
			}

		}	

	}
	
	generatePrecursorIon(trotations[0].bricks, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


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
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ",";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

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
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype);
		}
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype);
		}
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


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
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ",";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

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

#if OLIGOKETIDES == 1
		eResidueLossType leftresiduelosstype = bricksdatabasewithcombinations[intcomposition[0] - 1].getResidueLossType();
		bool hasfirstblockartificial = bricksdatabasewithcombinations[intcomposition[0] - 1].isArtificial();
#endif

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[j], bricksdatabasewithcombinations, writedescription, i, splittingsites, parameters->searchedmodifications, parameters->peptidetype, 0, leftresiduelosstype, hasfirstblockartificial);
		}

	}

	if (parameters->enablescrambling) {
		generateScrambledIons(bricksdatabasewithcombinations, writedescription, theoreticalpeaksrealsize);
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


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
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
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
							experimentalpeakmatches[theoreticalpeaks[j].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[j].matchedid].find(j));
							experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

							theoreticalpeaks[j].matched--;
							theoreticalpeaks[j].matchedid = -1;

						}
						else {

							matchesscrambled = false;
							for (set<int>::iterator it = experimentalpeakmatches[res].begin(); it != experimentalpeakmatches[res].end(); ++it) {
								if ((theoreticalpeaks[*it].matched > 0) && theoreticalpeaks[*it].scrambled) {
									matchesscrambled = true;
									break;
								}
							}

							// a scrambled peak is not matched by parent
							if (!matchesscrambled) {
								experimentalpeakmatches[theoreticalpeaks[j].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[j].matchedid].find(j));
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


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeakmatches.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ",";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
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
			vector<nodeEdge> cpath = candidate.getPath();
			candidate.setCandidate(v, cpath, candidate.getStartModifID(), candidate.getEndModifID(), candidate.getMiddleModifID(), branchcyclicrotations[i].getBranchStart(), branchcyclicrotations[i].getBranchEnd());
			candidate.setName(name);
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
						generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[j][k].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, j, splittingsites, parameters->searchedmodifications, parameters->peptidetype, &trotationsofbranchcyclicrotations[j][k]);
					}
				}

				// all permmutations except 3 and 5 have invalid c-terms
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal && ((k == 3) || (k == 5) /*|| ((k == 0) && (trotationsofbranchcyclicrotations[j][k].endsWithBracket())) || ((k == 2) && (trotationsofbranchcyclicrotations[j][k].startsWithBracket()))*/)) {
					// if the end modification is cterminal, generate c-terminal ions (**)
					if (parameters->searchedmodifications[trotationsofbranchcyclicrotations[j][k].endmodifID].cterminal) {
						generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, trotationsofbranchcyclicrotations[j][k].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, j, splittingsites, parameters->searchedmodifications, parameters->peptidetype, &trotationsofbranchcyclicrotations[j][k]);
					}
				}

			}	

		}

	}

	generatePrecursorIon(trotationsofbranchcyclicrotations[0][0].bricks, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


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
					experimentalpeakmatches[theoreticalpeaks[j].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[j].matchedid].find(j));
					experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

					theoreticalpeaks[j].matched--;
					theoreticalpeaks[j].matchedid = -1;
				}
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ",";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

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


#if OLIGOKETIDES == 1


int cTheoreticalSpectrum::compareLinearOligoketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {
	
	if (!candidate.checkKetideSequence(bricksdatabasewithcombinations, parameters->peptidetype)) {
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
			|| ((leftresiduelosstype == h2o_loss) && (candidate.getStartModifID() > 0) && parameters->searchedmodifications[candidate.getStartModifID()].nterminal)
			|| ((rightresiduelosstype == h2_loss) && (candidate.getEndModifID() > 0) && parameters->searchedmodifications[candidate.getEndModifID()].cterminal)
			|| ((rightresiduelosstype == h2o_loss) && (candidate.getEndModifID() > 0) && parameters->searchedmodifications[candidate.getEndModifID()].nterminal)) {
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
			((hasfirstblockartificial || (leftresiduelosstype == h2_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l1h_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l2h_ion)))
			|| 
			((hasfirstblockartificial || (leftresiduelosstype == h2o_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l1oh_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == l2oh_ion)))
			) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, 0, leftresiduelosstype, hasfirstblockartificial);
		}
		if (
			((haslastblockartificial || (rightresiduelosstype == h2_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r1h_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r2h_ion)))
			|| 
			((haslastblockartificial || (rightresiduelosstype == h2o_loss)) && ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r1oh_ion) || (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].parent == r2oh_ion)))
			) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype, 0, rightresiduelosstype, haslastblockartificial);
		}
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


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
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ",";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonTypeCID(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

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


int cTheoreticalSpectrum::compareCyclicOligoketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	if (!candidate.checkKetideSequence(bricksdatabasewithcombinations, parameters->peptidetype)) {
		return -2;
	}

	return compareCyclic(sortedpeaklist, bricksdatabasewithcombinations, writedescription, sequencetag, searchedsequence);

}


#endif


int cTheoreticalSpectrum::compareLinearPolysaccharide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {
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
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype);
		}
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, parameters->peptidetype);
		}
	}

	generatePrecursorIon(intcomposition, bricksdatabasewithcombinations, theoreticalpeaksrealsize, writedescription);


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


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
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches);


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ",";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
				theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
				theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
			}

			// to do
			if (experimentalpeaks[i].iontype != fragmentIonTypeEnd) {
				if ((experimentalpeaks[i].iontype != y_ion) && (theoreticalpeaks[*it].iontype == y_ion)) {
					experimentalpeaks[i].iontype = y_ion;
				}
			}
			else {
				experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
			}
		}
	}


	if (writedescription) {

		coveragebyseries = "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
			coveragebyseries += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].name + "  ";
			for (int j = 0; j < (int)series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 1; j++) {
				coveragebyseries += to_string(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				if (j < (int)series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 2) {
					coveragebyseries += " ";
				}
			}
			coveragebyseries += "<br/>\n";
		}

	}

	computeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


void cTheoreticalSpectrum::generateMSSpectrum() {
	cPeak peak;
	cSummaryFormula formula;
	string metalname;
	regex rx;

	theoreticalpeaks.resize(((int)parameters->fragmentionsfortheoreticalspectra.size() + countIsotopesOfMetals(parameters)) * parameters->sequencedatabase.size() * abs(parameters->precursorcharge));
	int theoreticalpeaksrealsize = 0;

	for (int i = 0; i < parameters->sequencedatabase.size(); i++) {

		peak.clear();
		peak.isotope = false;

		formula.clear();

		formula.setFormula(parameters->sequencedatabase[i].getSummaryFormula());

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {

			parameters->precursorAdductHasLi = false;
			parameters->precursorAdductHasMg = false;
			parameters->precursorAdductHasK = false;
			parameters->precursorAdductHasCa = false;
			parameters->precursorAdductHasFe = false;
			parameters->precursorAdductHasNi = false;
			parameters->precursorAdductHasCu = false;
			parameters->precursorAdductHasZn = false;
			parameters->precursorAdductHasGa = false;

			metalname = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name;

			// to do - optimize performance
			rx = "Li";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasLi = true;
			}
			rx = "Mg";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasMg = true;
			}
			rx = "K";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasK = true;
			}
			rx = "Ca";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasCa = true;
			}
			rx = "Fe";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasFe = true;
			}
			rx = "Ni";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasNi = true;
			}
			rx = "Cu";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasCu = true;
			}
			rx = "Zn";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasZn = true;
			}
			rx = "Ga";
			if (regex_search(metalname, rx)) {
				parameters->precursorAdductHasGa = true;
			}

			for (int k = 0; k < abs(parameters->precursorcharge); k++) {

				peak.mzratio = (double)parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].multiplier*formula.getMass() + parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].massdifference;
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive) {
					peak.mzratio += (double)k*Hplus;
				}
				else {
					peak.mzratio -= (double)k*Hplus;
				}
				peak.mzratio = peak.mzratio/(double)(k + 1);
				peak.description = parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.substr(0, parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name.size() - 1) + " ";
				peak.description += to_string(k + 1);
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].positive) {
					peak.description += "+";
				}
				else {
					peak.description += "-";
				}
				peak.description += " " + parameters->sequencedatabase[i].getNameWithReferenceAsHTMLString() + " (" + parameters->sequencedatabase[i].getSummaryFormula() + "): ";
				
				addPeakToList(peak, theoreticalpeaksrealsize);
				addMetalPeaks(peak, theoreticalpeaksrealsize, k + 1, true);				

			}
			
		}

	}

	theoreticalpeaks.resize(theoreticalpeaksrealsize);
}


void cTheoreticalSpectrum::compareMSSpectrum(cPeaksList& peaklist) {
	experimentalpeaks = peaklist;
	experimentalpeaks.sortbyMass();

	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaks.size(), experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);

	removeUnmatchedMetalIsotopes(theoreticalpeaks, theoreticalpeaks.size(), experimentalpeaks, experimentalpeakmatches);

	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {

			if (experimentalpeaks[i].description.compare("") != 0) {
				experimentalpeaks[i].description += ",";
			}
			experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.rfind(':'));
			theoreticalpeaks[*it].matchedmz = experimentalpeaks[i].mzratio;
			theoreticalpeaks[*it].matchedintensity = experimentalpeaks[i].intensity;
			theoreticalpeaks[*it].matchedppm = ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio);
		}
	}

	computeStatistics(true);

	cPeaksList tmppeaklist = theoreticalpeaks;
	theoreticalpeaks.clear();

	// remove unmatched theoretical peaks to save memory
	for (int i = 0; i < tmppeaklist.size(); i++) {
		if (tmppeaklist[i].matchedmz > 0) {
			theoreticalpeaks.add(tmppeaklist[i]);
		}
	}

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


void cTheoreticalSpectrum::generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, TRotationInfo* trotation, eResidueLossType leftresiduelosstype, bool hasfirstblockartificial) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	peak.mzratio = parameters->fragmentdefinitions[fragmentiontype].massdifference;
	if ((peptidetype == linear) || (peptidetype == linearpolysaccharide)
#if OLIGOKETIDES == 1
		|| (peptidetype == linearoligoketide)
#endif
		) {
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
	case linearpolysaccharide:
		peak.rotationid = rotationid;
		break;
#if OLIGOKETIDES == 1
	case linearoligoketide:
		peak.rotationid = rotationid;
		break;
	case cyclicoligoketide:
		peak.rotationid = rotationid;
		break;
#endif
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

#if OLIGOKETIDES == 1
		if (peptidetype == linearoligoketide) {
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
		
		if (peptidetype == cyclicoligoketide) {

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
#endif

		peak.seriesid = i;

		if (writedescription) {

			peak.description = "";
#if OLIGOKETIDES == 1
			if ((peptidetype == cyclic) || (peptidetype == cyclicoligoketide)) {
#else
			if (peptidetype == cyclic) {
#endif
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
			}
			if (peptidetype == branched) {
				peak.description += to_string(peak.rotationid + 1) + "_";
			}
			if (peptidetype == branchcyclic) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
				peak.description += to_string(trotation->id + 1) + "_";
			}

#if OLIGOKETIDES == 1
			if ((peptidetype == linearoligoketide) || (peptidetype == cyclicoligoketide)) {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 2) + to_string(i + 1);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 2) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(2, parameters->fragmentdefinitions[fragmentiontype].name.length() - 2);
				}
			}
			else {
#endif
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 1) + to_string(i + 1);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 1) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(1, parameters->fragmentdefinitions[fragmentiontype].name.length() - 1);
				}
#if OLIGOKETIDES == 1
			}
#endif

			addAdductToDescription(peak.description);
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
				addMetalPeaks(peak, peaklistrealsize, j, writedescription);

			}
			peak.mzratio = tempratio;
			if (writedescription) {
				peak.description = tempdescription;
			}

		}

	}

}


void cTheoreticalSpectrum::generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, TRotationInfo* trotation, eResidueLossType rightresiduelosstype, bool haslastblockartificial) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	peak.mzratio = parameters->fragmentdefinitions[fragmentiontype].massdifference;
	if ((peptidetype == linear) || (peptidetype == linearpolysaccharide)
#if OLIGOKETIDES == 1
		|| (peptidetype == linearoligoketide)
#endif
		) {
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
#if OLIGOKETIDES == 1
	case linearoligoketide:
		peak.rotationid = rotationid;
		break;
	case cyclicoligoketide:
		peak.rotationid = rotationid;
		break;
#endif
	case linearpolysaccharide:
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

#if OLIGOKETIDES == 1
		if (peptidetype == linearoligoketide) {
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
#endif

		peak.seriesid = (int)intcomposition.size() - i - 1;

		if (writedescription) {

			peak.description = "";
#if OLIGOKETIDES == 1
			if (peptidetype == cyclicoligoketide) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
			}
#endif
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

#if OLIGOKETIDES == 1
			if ((peptidetype == linearoligoketide) || (peptidetype == cyclicoligoketide)) {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 2) + to_string((int)intcomposition.size() - i);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 2) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(2, parameters->fragmentdefinitions[fragmentiontype].name.length() - 2);
				}
			}
			else {
#endif
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 1) + to_string((int)intcomposition.size() - i);
				if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 1) {
					peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(1, parameters->fragmentdefinitions[fragmentiontype].name.length() - 1);
				}
#if OLIGOKETIDES == 1
			}
#endif

			addAdductToDescription(peak.description);
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
				addMetalPeaks(peak, peaklistrealsize, j, writedescription);				

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


double cTheoreticalSpectrum::getPrecursorMass(cBricksDatabase& brickdatabasewithcombinations) {
	return candidate.getPrecursorMass(brickdatabasewithcombinations, parameters);
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


string cTheoreticalSpectrum::getCoverageBySeries() {
	return coveragebyseries;
}


cPeaksList* cTheoreticalSpectrum::getUnmatchedPeaks() {
	return &unmatchedpeaks;
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


void cTheoreticalSpectrum::setParameters(cParameters* parameters) {
	this->parameters = parameters;
}


void cTheoreticalSpectrum::store(ofstream& os) {
	int size;

	theoreticalpeaks.store(os);
	experimentalpeaks.store(os);
	candidate.store(os);

	os.write((char *)&experimentalpeaksmatched, sizeof(int));
	os.write((char *)&scrambledpeaksmatched, sizeof(int));

	size = (int)matchedions.size();
	os.write((char *)&size, sizeof(int));
	for (map<eFragmentIonType, int>::iterator it = matchedions.begin(); it != matchedions.end(); ++it) {
		os.write((char *)&it->first, sizeof(eFragmentIonType));
		os.write((char *)&it->second, sizeof(int));
	}

	os.write((char *)&peakstested, sizeof(int));
	os.write((char *)&experimentalpeaksmatchedratio, sizeof(double));

	unmatchedpeaks.store(os);
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
}


void cTheoreticalSpectrum::load(ifstream& is) {
	int size;
	eFragmentIonType iontype;
	int number;

	parameters = 0;

	theoreticalpeaks.load(is);
	experimentalpeaks.load(is);
	candidate.load(is);

	is.read((char *)&experimentalpeaksmatched, sizeof(int));
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

	unmatchedpeaks.load(is);
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
}

