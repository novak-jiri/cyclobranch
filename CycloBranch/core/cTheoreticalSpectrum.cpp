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

	size = (int)name.size();
	os.write((char *)&size, sizeof(int));
	os.write(name.c_str(), name.size());
}


void visualSeries::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	series.resize(size);
	for (int i = 0; i < (int)series.size(); i++) {
		is.read((char *)&series[i], sizeof(int));
	}

	is.read((char *)&size, sizeof(int));
	name.resize(size);
	is.read(&name[0], name.size());
}


void cTheoreticalSpectrum::clearFalseHits(map<fragmentIonType, vector<int> >& series, vector<fragmentIonType>& fragmentions) {
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


void cTheoreticalSpectrum::computeSomeStatistics(bool writedescription) {
	experimentalpeaksmatched = 0;
	if (writedescription) {
		unmatchedpeaks = "";
	}
	intensityweightedscore = 0;
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		if (experimentalpeaks[i].matched > 0) {
			experimentalpeaksmatched++;
			intensityweightedscore += experimentalpeaks[i].intensity;
			matchedions[experimentalpeaks[i].iontype]++;
		}
		else {
			if (writedescription) {
				unmatchedpeaks += to_string((long double)experimentalpeaks[i].mzratio) + " ";
			}
		}
	}
	peakstested = experimentalpeaks.size();
	experimentalpeaksmatchedratio = ((double)experimentalpeaksmatched) / ((double)experimentalpeaks.size());
}


void cTheoreticalSpectrum::generateScrambledIons(cBricksDatabase& bricksdatabase, bool writedescription, int& theoreticalpeaksrealsize) {
	unordered_set<string> scrambledsequences, commonsequences;
	vector<int> intcomposition;
	scrambledsequences.clear();
	int minimumsizeofscrambledsequences = 2;
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
	// generate scrambled sequences
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

	// normalize scrambled sequences
	normalizeScrambledSequences(scrambledsequences);

	cPeaksList scrambledpeaks;
	scrambledpeaks.resize(5000);
	int scrambledspeaksrealsize = 0;

	// generate scrambled peaks
	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		for (unordered_set<string>::iterator it = scrambledsequences.begin(); it != scrambledsequences.end(); ++it) {
			b.setComposition((string &)*it, false);
			intcomposition.clear();
			b.explodeToIntComposition(intcomposition);
			cPeak peak;
			peak.iontype = (fragmentIonType)parameters->fragmentionsfortheoreticalspectra[i];
			peak.mzratio = parameters->fragmentdefinitions[peak.iontype].massdifference;
			peak.removeme = false;
			peak.scrambled = true;
			for (int i = 0; i < (int)intcomposition.size(); i++) {
				peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();
			}

			if (writedescription) {
				peak.description = "scrambled ";
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

	// erase all scrambled peaks whose mz ratios collide with common fragment ions
	// eraseAll - time consuming 55.8%
	// scrambledpeaks.resize(scrambledspeaksrealsize);
	// scrambledpeaks.sortbyMass();
	// for (int i = 0; i < theoreticalpeaksrealsize; i++) {
	// 	scrambledpeaks.eraseAll(theoreticalpeaks[i].mzratio);
	// }

	// attach scrambled peaks to common peaks
	for (int i = 0; i < (int)scrambledpeaks.size(); i++) {
		if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
			theoreticalpeaks[theoreticalpeaksrealsize] = scrambledpeaks[i];
		}
		else {
			theoreticalpeaks.add(scrambledpeaks[i]);
		}
		theoreticalpeaksrealsize++;
	}

}


void cTheoreticalSpectrum::normalizeScrambledSequences(unordered_set<string>& scrambledsequences) {
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


fragmentIonType cTheoreticalSpectrum::selectHigherPriorityIonType(fragmentIonType experimentalpeakiontype, fragmentIonType theoreticalpeakiontype) {
	if (experimentalpeakiontype != fragmentIonTypeEnd) {

		switch (experimentalpeakiontype)
		{
		case b_ion:
			break;
		case b_ion_water_loss:
		case b_ion_ammonia_loss:
			if (theoreticalpeakiontype == b_ion) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case b_ion_water_and_ammonia_loss:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_water_loss) || (theoreticalpeakiontype == b_ion_ammonia_loss)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case a_ion:
			if (theoreticalpeakiontype == b_ion) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case a_ion_water_loss:
		case a_ion_ammonia_loss:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_water_loss) || (theoreticalpeakiontype == b_ion_ammonia_loss) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case a_ion_water_and_ammonia_loss:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_water_loss) || (theoreticalpeakiontype == b_ion_ammonia_loss) || (theoreticalpeakiontype == a_ion) || (theoreticalpeakiontype == a_ion_water_loss) || (theoreticalpeakiontype == a_ion_ammonia_loss)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case y_ion:
			if ((theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == a_ion)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case y_ion_water_loss:
		case y_ion_ammonia_loss:
			if ((theoreticalpeakiontype == y_ion) || (theoreticalpeakiontype == b_ion) || (theoreticalpeakiontype == b_ion_water_loss) || (theoreticalpeakiontype == b_ion_ammonia_loss) || (theoreticalpeakiontype == a_ion)  || (theoreticalpeakiontype == a_ion_water_loss) || (theoreticalpeakiontype == a_ion_ammonia_loss)) {
				experimentalpeakiontype = theoreticalpeakiontype;
			}
			break;
		case y_ion_water_and_ammonia_loss:
			experimentalpeakiontype = theoreticalpeakiontype;
		default:
			break;
		}

	}
	else {
		experimentalpeakiontype = theoreticalpeakiontype;
	}

	return experimentalpeakiontype;
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
		matchedions[(fragmentIonType)i] = 0;
	}

	candidate.clear();
	experimentalpeaksmatched = 0;
	peakstested = 0;
	experimentalpeaksmatchedratio = 0;
	unmatchedpeaks = "";
	coveragebyseries = "";
	valid = false;
	maskscore = 0;
	intensityweightedscore = 0;
	visualcoverage.clear();
	validposition = -1;
	reversevalidposition = -1;
	
	realpeptidename = "";
	acronympeptidename = "";
	acronyms.clear();
	path = "";
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
	vector<string> stringcomposition;
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
	catch (std::regex_error& e) {
		e;
		return -2;
	}

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		for (int j = 0; j < (int)trotations.size(); j++) {

			if (writedescription) {
				stringcomposition.clear();
				for (int k = 0; k < (int)trotations[j].bricks.size(); k++) {
					stringcomposition.push_back(to_string(trotations[j].bricks[k]));
				}
			}

			// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
			// in this case the modifID is 0, and both n-terminal (*) and c-terminal (**) fragments are generated

			// j == 2 and j == 5 have invalid n-terms
			if ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) && (j != 2) && (j != 5)) {
				// if the start modification for j == 1 or j == 4 is nterminal, generate n-terminal ions (*)
				if (!(((j == 1) || (j == 4)) && (!parameters->searchedmodifications[trotations[j].startmodifID].nterminal))) {
					generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, trotations[j].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, branched, &trotations[j]);
				}
			}

			// j == 2 and j == 4 have invalid c-terms
			if ((parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) && (j != 2) && (j != 4)) {
				// if the end modification for j == 3 or j == 5 is cterminal, generate c-terminal ions (**)
				if (!(((j == 3) || (j == 5)) && (!parameters->searchedmodifications[trotations[j].endmodifID].cterminal))) {
					generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, trotations[j].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, branched, &trotations[j]);
				}
			}

		}	
	}


	// generate the precursor ion
	cPeak peak;
	peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference + parameters->searchedmodifications[candidate.getMiddleModifID()].massdifference;
	for (int i = 0; i < (int)trotations[0].bricks.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[trotations[0].bricks[i] - 1].getMass();
	}
	peak.seriesid = 0;

	double tempratio = peak.mzratio;
	for (int i = (int)precursor_ion; i <= (int)precursor_ion_co_loss_water_and_ammonia_loss; i++) {
		for (int j = 1; j <= parameters->precursorcharge; j++) {
			peak.mzratio = tempratio + parameters->fragmentdefinitions[(fragmentIonType)i].massdifference;
			peak.iontype = (fragmentIonType)i;

			if (writedescription) {
				peak.description = to_string(j) + "+ " + parameters->fragmentdefinitions[(fragmentIonType)i].name + ":";
			}

			peak.mzratio = charge(peak.mzratio, j);
			peak.charge = j;

			if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
				theoreticalpeaks[theoreticalpeaksrealsize] = peak;
			}
			else {
				theoreticalpeaks.add(peak);
			}
			theoreticalpeaksrealsize++;

		}
	}


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


	// coverage of series
	vector<map<fragmentIonType, vector<int> > > series;
	series.resize(trotations.size());
	for (int i = 0; i < (int)series.size(); i++) {
		for (int j = 0; j < parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			series[i][parameters->fragmentionsfortheoreticalspectra[j]].resize(trotations[0].bricks.size());
		}
	}
	
	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if ((theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)trotations.size(); i++) {	
			clearFalseHits(series[i], parameters->fragmentionsfortheoreticalspectra);
		}

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if ((theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && ((series[theoreticalpeaks[i].rotationid].count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[theoreticalpeaks[i].rotationid][parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {		
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ", ";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				experimentalpeaks[i].matchedrotations.push_back(theoreticalpeaks[*it].rotationid);
				theoreticalpeaks[*it].matchdescription += ", measured mass " + to_string((long double)experimentalpeaks[i].mzratio) + " matched with " + to_string((long double)ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio)) + " ppm error";
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonType(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

		visualcoverage.clear();
		visualSeries tempseries;

		coveragebyseries = "T-Permutations:<br/>\n";
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
				for (int k = 0; k < series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 1; k++) {
					coveragebyseries += to_string(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					tempseries.series.push_back(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					if (k < series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 2) {
						coveragebyseries += " ";
					}
				}
				coveragebyseries += "<br/>\n";
				visualcoverage.push_back(tempseries);
			}
		}

	}

	computeSomeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLinear(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {
	cBrick brick;
	vector<int> intcomposition;
	vector<string> stringcomposition;
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
	catch (std::regex_error& e) {
		e;
		return -2;
	}

	intcomposition.clear();
	brick.clear();
	brick.setComposition(candidate.getComposition(), false);
	brick.explodeToIntComposition(intcomposition);

	if (writedescription) {
		stringcomposition.clear();
		brick.explodeToStringComposition(stringcomposition);
	}

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, linear);
		}
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, linear);
		}
	}


	// generate the precursor ion
	cPeak peak;
	peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
	}
	peak.seriesid = 0;

	double tempratio = peak.mzratio;
	for (int i = (int)precursor_ion; i <= (int)precursor_ion_co_loss_water_and_ammonia_loss; i++) {
		for (int j = 1; j <= parameters->precursorcharge; j++) {
			peak.mzratio = tempratio + parameters->fragmentdefinitions[(fragmentIonType)i].massdifference;
			peak.iontype = (fragmentIonType)i;

			if (writedescription) {
				peak.description = to_string(j) + "+ " + parameters->fragmentdefinitions[(fragmentIonType)i].name + ":";
			}

			peak.mzratio = charge(peak.mzratio, j);
			peak.charge = j;

			if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
				theoreticalpeaks[theoreticalpeaksrealsize] = peak;
			}
			else {
				theoreticalpeaks.add(peak);
			}
			theoreticalpeaksrealsize++;

		}
	}


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


	// coverage of series
	map<fragmentIonType, vector<int> > series;
	for (int i = 0; i < parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		series[parameters->fragmentionsfortheoreticalspectra[i]].resize(intcomposition.size());
	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if ((theoreticalpeaks[i].matched > 0) && (series.count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->fragmentionsfortheoreticalspectra);

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if ((theoreticalpeaks[i].matched > 0) && ((series.count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {		
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ", ";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				experimentalpeaks[i].matchedrotations.push_back(theoreticalpeaks[*it].rotationid);
				theoreticalpeaks[*it].matchdescription += ", measured mass " + to_string((long double)experimentalpeaks[i].mzratio) + " matched with " + to_string((long double)ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio)) + " ppm error";
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonType(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
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
			for (int j = 0; j < series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 1; j++) {
				coveragebyseries += to_string(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				tempseries.series.push_back(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				if (j < series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 2) {
					coveragebyseries += " ";
				}
			}
			coveragebyseries += "<br/>\n";
			visualcoverage.push_back(tempseries);
		}

	}

	computeSomeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	cBrick brick;
	vector<int> intcomposition;
	vector<string> stringcomposition;
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
	catch (std::regex_error& e) {
		e;
		return -2;
	}


	for (int i = 0; i < 2*r; i++) {

		intcomposition.clear();
		brick.clear();
		brick.setComposition(rotations[i], false);
		brick.explodeToIntComposition(intcomposition);

		if (writedescription) {
			stringcomposition.clear();
			brick.explodeToStringComposition(stringcomposition);
		}

		for (int j = 0; j < (int)parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, intcomposition, parameters->fragmentionsfortheoreticalspectra[j], bricksdatabasewithcombinations, writedescription, i, splittingsites, parameters->searchedmodifications, cyclic);
		}

	}

	if (parameters->enablescrambling) {
		generateScrambledIons(bricksdatabasewithcombinations, writedescription, theoreticalpeaksrealsize);
	}

	
	// generate the precursor ion
	cPeak peak;
	peak.mzratio = 0;
	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
	}
	peak.seriesid = (int)intcomposition.size() - 1;

	double tempratio = peak.mzratio;
	for (int i = (int)cyclic_precursor_ion; i <= (int)cyclic_precursor_ion_co_loss_water_and_ammonia_loss; i++) {
		for (int j = 1; j <= parameters->precursorcharge; j++) {
			peak.mzratio = tempratio + parameters->fragmentdefinitions[(fragmentIonType)i].massdifference;
			peak.iontype = (fragmentIonType)i;

			if (writedescription) {
				peak.description = to_string(j) + "+ " + parameters->fragmentdefinitions[(fragmentIonType)i].name + ":";
			}

			peak.mzratio = charge(peak.mzratio, j);
			peak.charge = j;

			if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
				theoreticalpeaks[theoreticalpeaksrealsize] = peak;
			}
			else {
				theoreticalpeaks.add(peak);
			}
			theoreticalpeaksrealsize++;

		}
	}


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


	// coverage of series
	vector<map<fragmentIonType, vector<int> > > series;
	series.resize(rotations.size());
	for (int i = 0; i < (int)series.size(); i++) {
		for (int j = 0; j < parameters->fragmentionsfortheoreticalspectra.size(); j++) {
			series[i][parameters->fragmentionsfortheoreticalspectra[j]].resize(r);
		}
	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if ((theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && (series[theoreticalpeaks[i].rotationid].count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].rotationid][theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}


	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)rotations.size(); i++) {	
			clearFalseHits(series[i], parameters->fragmentionsfortheoreticalspectra);
		}

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if ((theoreticalpeaks[i].matched > 0) && (theoreticalpeaks[i].rotationid >= 0) && ((series[theoreticalpeaks[i].rotationid].count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[theoreticalpeaks[i].rotationid][parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {	
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


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeakmatches.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ", ";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				experimentalpeaks[i].matchedrotations.push_back(theoreticalpeaks[*it].rotationid);
				theoreticalpeaks[*it].matchdescription += ", measured mass " + to_string((long double)experimentalpeaks[i].mzratio) + " matched with " + to_string((long double)ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio)) + " ppm error";
			}

			if (experimentalpeaks[i].iontype != fragmentIonTypeEnd) {

				if (theoreticalpeaks[*it].scrambled == experimentalpeaks[i].scrambled) {

					switch (experimentalpeaks[i].iontype)
					{
					case b_ion:
						break;
					case b_ion_water_loss:
					case b_ion_ammonia_loss:
						if (theoreticalpeaks[*it].iontype == b_ion) {
							experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						}
						break;
					case b_ion_water_and_ammonia_loss:
						if ((theoreticalpeaks[*it].iontype == b_ion) || (theoreticalpeaks[*it].iontype == b_ion_water_loss) || (theoreticalpeaks[*it].iontype == b_ion_ammonia_loss)) {
							experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						}
						break;
					case a_ion:
						if (theoreticalpeaks[*it].iontype == b_ion) {
							experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						}
						break;
					case a_ion_water_loss:
					case a_ion_ammonia_loss:
						if ((theoreticalpeaks[*it].iontype == b_ion) || (theoreticalpeaks[*it].iontype == b_ion_water_loss) || (theoreticalpeaks[*it].iontype == b_ion_ammonia_loss) || (theoreticalpeaks[*it].iontype == a_ion)) {
							experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						}
						break;
					case a_ion_water_and_ammonia_loss:
						experimentalpeaks[i].iontype = theoreticalpeaks[*it].iontype;
						break;
					default:
						break;
					}

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

		coveragebyseries = "Rotations:<br/>\n";
		for (int i = 0; i < (int)rotations.size(); i++) {
			/*
			if (i == 0) {
				coveragebyseries += "&gt;&gt;&gt;<br/>\n";
			}
			if (i == r) {
				coveragebyseries += "<br/>\n&lt;&lt;&lt;<br/>\n";
			}
			*/
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
				for (int k = 0; k < series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 1; k++) {
					coveragebyseries += to_string(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					tempseries.series.push_back(series[i][parameters->fragmentionsfortheoreticalspectra[j]][k]);
					if (k < series[i][parameters->fragmentionsfortheoreticalspectra[j]].size() - 2) {
						coveragebyseries += " ";
					}
				}
				coveragebyseries += "<br/>\n";
				visualcoverage.push_back(tempseries); // comment
			}
		}

	}
		

	vector<int> mask; // peaks converted to letters
	vector<int> tempvector;
	visualSeries visualtempseries;
	for (int k = 0; k < r; k++) {
		mask.push_back(0);
		tempvector.push_back(0);
		if (writedescription) {
			visualtempseries.series.push_back(0);
		}
	}

	if (writedescription) {
		//visualcoverage.clear(); // uncomment
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

			if (writedescription) {
				visualtempseries.name = to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
				visualtempseries.name += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[j]].name;
			}
			
			if (i < r) {			
				for (int k = 0; k < r; k++) {
					mask[k] += tempvector[(k + r - i) % r];

					if (writedescription) {
						visualtempseries.series[k] = tempvector[(k + r - i) % r];
					}
				}		
			}
			else {
				reverse(tempvector.begin(),tempvector.end());

				for (int k = 0; k < r; k++) {
					mask[k] += tempvector[(k + i) % r];

					if (writedescription) {
						visualtempseries.series[k] = tempvector[(k + i) % r];
					}
				}
			}

			if (writedescription) {
				//visualcoverage.push_back(visualtempseries); // uncomment
			}

		}

	}

	maskscore = 0;
	for (int k = 0; k < (int)mask.size(); k++) {
		maskscore += mask[k];
	}

	computeSomeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLasso(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {

	vector<splitSite> splittingsites;
	int theoreticalpeaksrealsize = 0;
	vector<string> stringcomposition;


	vector<cCandidate> lassorotations;
	candidate.getLassoRotations(lassorotations, true);


	int r = (int)lassorotations.size() / 2;
	validposition = -1;
	reversevalidposition = -1;


	vector<vector<TRotationInfo> > trotationsoflassorotations;
	trotationsoflassorotations.resize(lassorotations.size());
	for (int i = 0; i < (int)lassorotations.size(); i++) {
		lassorotations[i].getPermutationsOfBranches(trotationsoflassorotations[i]);
	}


	try {
		bool stop = true;
		for (int i = 0; i < (int)lassorotations.size(); i++) {
			for (int j = 0; j < (int)trotationsoflassorotations[i].size(); j++) {
				if (regex_search(trotationsoflassorotations[i][j].tcomposition, sequencetag)) {
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
				for (int i = 0; i < (int)lassorotations.size(); i++) {
					if (regex_search(lassorotations[i].getComposition(), searchedsequence)) {
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

					for (int j = 0; j < (int)trotationsoflassorotations[i].size(); j++) {
						if (regex_search(trotationsoflassorotations[i][j].tcomposition, searchedsequence) &&
						(parameters->searchedmodifications[trotationsoflassorotations[i][j].middlemodifID].name.compare(parameters->searchedsequenceTmodif) == 0)) {
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

				splittingsites.resize(lassorotations.size());
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
	catch (std::regex_error& e) {
		e;
		return -2;
	}


	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {

		for (int j = 0; j < (int)lassorotations.size(); j++) {

			for (int k = 0; k < (int)trotationsoflassorotations[j].size(); k++) {

				if (writedescription) {
					// to do - performace issue, generated repeatedly
					stringcomposition.clear();
					for (int m = 0; m < (int)trotationsoflassorotations[j][k].bricks.size(); m++) {
						stringcomposition.push_back(to_string(trotationsoflassorotations[j][k].bricks[m]));
					}
				}

				// we do not know whether the middle branch is n-terminal or c-terminal for an unmodified middle branch
				// in this case the modifID is 0, and both n-terminal (*) and c-terminal (**) fragments are generated

				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) {
					// if the start modification for k == 1 or k == 4 is nterminal, generate n-terminal ions (*)
					if (!(((k == 1) || (k == 4)) && (!parameters->searchedmodifications[trotationsoflassorotations[j][k].startmodifID].nterminal))) {
						generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, trotationsoflassorotations[j][k].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, j, splittingsites, parameters->searchedmodifications, lasso, &trotationsoflassorotations[j][k]);
					}
				}

				// all permmutations except 3 and 5 have invalid c-terms
				if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal && ((k == 3) || (k == 5) || ((k == 0) && (trotationsoflassorotations[j][k].endsWithBracket())) || ((k == 2) && (trotationsoflassorotations[j][k].startsWithBracket())))) {
					// if the end modification is cterminal, generate c-terminal ions (**)
					if (parameters->searchedmodifications[trotationsoflassorotations[j][k].endmodifID].cterminal) {
						generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, trotationsoflassorotations[j][k].bricks, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, j, splittingsites, parameters->searchedmodifications, lasso, &trotationsoflassorotations[j][k]);
					}
				}

			}	

		}

	}


	// generate the precursor ion
	// to do - performance
	cBrick b;
	b.setComposition(candidate.getComposition(), false);
	vector<int> intcomposition;
	b.explodeToIntComposition(intcomposition);

	cPeak peak;
	peak.mzratio = parameters->searchedmodifications[candidate.getMiddleModifID()].massdifference;
	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
	}
	peak.seriesid = 0;

	double tempratio = peak.mzratio;
	for (int i = (int)cyclic_precursor_ion; i <= (int)cyclic_precursor_ion_co_loss_water_and_ammonia_loss; i++) {
		for (int j = 1; j <= parameters->precursorcharge; j++) {
			peak.mzratio = tempratio + parameters->fragmentdefinitions[(fragmentIonType)i].massdifference;
			peak.iontype = (fragmentIonType)i;

			if (writedescription) {
				peak.description = to_string(j) + "+ " + parameters->fragmentdefinitions[(fragmentIonType)i].name + ":";
			}

			peak.mzratio = charge(peak.mzratio, j);
			peak.charge = j;

			if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
				theoreticalpeaks[theoreticalpeaksrealsize] = peak;
			}
			else {
				theoreticalpeaks.add(peak);
			}
			theoreticalpeaksrealsize++;

		}
	}


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


	// coverage of series
	vector<vector<map<fragmentIonType, vector<int> > > > series;
	series.resize(lassorotations.size());
	for (int i = 0; i < (int)lassorotations.size(); i++) {
		series[i].resize(6);
		//series[i].resize(trotationsoflassorotations[i].size()); // to do - potential bug

		for (int j = 0; j < (int)series[i].size(); j++) {
			for (int k = 0; k < parameters->fragmentionsfortheoreticalspectra.size(); k++) {
				series[i][j][parameters->fragmentionsfortheoreticalspectra[k]].resize(trotationsoflassorotations[i][0].bricks.size());
			}
		}
	
		for (int j = 0; j < theoreticalpeaksrealsize; j++) {
			if ((theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) && (series[i][theoreticalpeaks[j].rotationid % 6].count(theoreticalpeaks[j].iontype) == 1)) {
				if (i == theoreticalpeaks[j].rotationid / 6) {
					series[i][theoreticalpeaks[j].rotationid % 6][theoreticalpeaks[j].iontype][theoreticalpeaks[j].seriesid]++;
				}
			}
		}
	}


	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		for (int i = 0; i < (int)lassorotations.size(); i++) {
			for (int j = 0; j < (int)trotationsoflassorotations[i].size(); j++) {	
				clearFalseHits(series[i][j], parameters->fragmentionsfortheoreticalspectra);
			}

			for (int j = 0; j < theoreticalpeaksrealsize; j++) {
				if ((theoreticalpeaks[j].matched > 0) && (theoreticalpeaks[j].rotationid >= 0) && (i == theoreticalpeaks[j].rotationid / 6) && ((series[i][theoreticalpeaks[j].rotationid % 6].count(parameters->fragmentdefinitions[theoreticalpeaks[j].iontype].parent) == 0) || (series[i][theoreticalpeaks[j].rotationid % 6][parameters->fragmentdefinitions[theoreticalpeaks[j].iontype].parent][theoreticalpeaks[j].seriesid] == 0))) {		
					experimentalpeakmatches[theoreticalpeaks[j].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[j].matchedid].find(j));
					experimentalpeaks[theoreticalpeaks[j].matchedid].matched--;

					theoreticalpeaks[j].matched--;
					theoreticalpeaks[j].matchedid = -1;
				}
			}
		}
	}


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ", ";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				experimentalpeaks[i].matchedrotations.push_back(theoreticalpeaks[*it].rotationid);
				theoreticalpeaks[*it].matchdescription += ", measured mass " + to_string((long double)experimentalpeaks[i].mzratio) + " matched with " + to_string((long double)ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio)) + " ppm error";
			}

			experimentalpeaks[i].iontype = selectHigherPriorityIonType(experimentalpeaks[i].iontype,theoreticalpeaks[*it].iontype);
		}
	}


	if (writedescription) {

		coveragebyseries = "T-Permutations of lasso rotations:<br/>\n";
		for (int i = 0; i < (int)lassorotations.size(); i++) {
			if (i == 0) {
				coveragebyseries += "&gt;&gt;&gt;<br/>\n";
			}
			if (i == r) {
				coveragebyseries += "&lt;&lt;&lt;<br/>\n";
			}
			for (int j = 0; j < (int)trotationsoflassorotations[i].size(); j++) {
				coveragebyseries += to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
				coveragebyseries += to_string(j + 1) + " ~ ";
				coveragebyseries += bricksdatabasewithcombinations.getAcronymNameOfTPeptide(trotationsoflassorotations[i][j].tcomposition, false);
				coveragebyseries += "<br/>\n";
			}
			coveragebyseries += "<br/>\n";
		}

		coveragebyseries += "Series of matched peaks:<br/>\n";
		for (int i = 0; i < (int)lassorotations.size(); i++) {
			for (int j = 0; j < (int)series[i].size(); j++) {
				for (int k = 0; k < (int)parameters->fragmentionsfortheoreticalspectra.size(); k++) {
					coveragebyseries += to_string(splittingsites[i].first + 1) + "-" + to_string(splittingsites[i].second + 1) + "_";
					coveragebyseries += to_string(j + 1) + "_";
					coveragebyseries += parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[k]].name + "  ";
					for (int m = 0; m < series[i][j][parameters->fragmentionsfortheoreticalspectra[k]].size() - 1; m++) {
						coveragebyseries += to_string(series[i][j][parameters->fragmentionsfortheoreticalspectra[k]][m]);
						if (m < series[i][j][parameters->fragmentionsfortheoreticalspectra[k]].size() - 2) {
							coveragebyseries += " ";
						}
					}
					coveragebyseries += "<br/>\n";
				}
			}
		}

	}


	computeSomeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::compareLinearPolysaccharide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence) {
	cBrick brick;
	vector<int> intcomposition;
	vector<string> stringcomposition;
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
	catch (std::regex_error& e) {
		e;
		return -2;
	}

	intcomposition.clear();
	brick.clear();
	brick.setComposition(candidate.getComposition(), false);
	brick.explodeToIntComposition(intcomposition);

	if (writedescription) {
		stringcomposition.clear();
		brick.explodeToStringComposition(stringcomposition);
	}

	for (int i = 0; i < (int)parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].nterminal) {
			generateNTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, linearpolysaccharide);
		}
		if (parameters->fragmentdefinitions[parameters->fragmentionsfortheoreticalspectra[i]].cterminal) {
			generateCTerminalFragmentIons(parameters->precursorcharge, theoreticalpeaksrealsize, stringcomposition, intcomposition, parameters->fragmentionsfortheoreticalspectra[i], bricksdatabasewithcombinations, writedescription, 0, splittingsites, parameters->searchedmodifications, linearpolysaccharide);
		}
	}


	// generate the precursor ion
	cPeak peak;
	peak.mzratio = parameters->searchedmodifications[candidate.getStartModifID()].massdifference + parameters->searchedmodifications[candidate.getEndModifID()].massdifference;
	for (int i = 0; i < (int)intcomposition.size(); i++) {
		peak.mzratio += bricksdatabasewithcombinations[intcomposition[i] - 1].getMass();
	}
	peak.seriesid = 0;

	double tempratio = peak.mzratio;
	for (int i = (int)precursor_ion; i <= (int)precursor_ion_co_loss_water_and_ammonia_loss; i++) {
		for (int j = 1; j <= parameters->precursorcharge; j++) {
			peak.mzratio = tempratio + parameters->fragmentdefinitions[(fragmentIonType)i].massdifference;
			peak.iontype = (fragmentIonType)i;

			if (writedescription) {
				peak.description = to_string(j) + "+ " + parameters->fragmentdefinitions[(fragmentIonType)i].name + ":";
			}

			peak.mzratio = charge(peak.mzratio, j);
			peak.charge = j;

			if (theoreticalpeaks.size() > theoreticalpeaksrealsize) {
				theoreticalpeaks[theoreticalpeaksrealsize] = peak;
			}
			else {
				theoreticalpeaks.add(peak);
			}
			theoreticalpeaksrealsize++;

		}
	}


	// search the theoretical peaks in the experimental peak list
	experimentalpeaks = sortedpeaklist;
	vector<set<int> > experimentalpeakmatches;
	searchForPeakPairs(theoreticalpeaks, theoreticalpeaksrealsize, experimentalpeaks, experimentalpeakmatches, parameters->fragmentmasserrortolerance);


	// coverage of series
	map<fragmentIonType, vector<int> > series;
	for (int i = 0; i < parameters->fragmentionsfortheoreticalspectra.size(); i++) {
		series[parameters->fragmentionsfortheoreticalspectra[i]].resize(intcomposition.size());
	}

	for (int i = 0; i < theoreticalpeaksrealsize; i++) {
		if ((theoreticalpeaks[i].matched > 0) && (series.count(theoreticalpeaks[i].iontype) == 1)) {
			series[theoreticalpeaks[i].iontype][theoreticalpeaks[i].seriesid]++;
		}
	}
	

	// peak hits without parents are removed
	if (parameters->clearhitswithoutparent) {
		clearFalseHits(series, parameters->fragmentionsfortheoreticalspectra);

		for (int i = 0; i < theoreticalpeaksrealsize; i++) {
			if ((theoreticalpeaks[i].matched > 0) && ((series.count(parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent) == 0) || (series[parameters->fragmentdefinitions[theoreticalpeaks[i].iontype].parent][theoreticalpeaks[i].seriesid] == 0))) {		
				experimentalpeakmatches[theoreticalpeaks[i].matchedid].erase(experimentalpeakmatches[theoreticalpeaks[i].matchedid].find(i));
				experimentalpeaks[theoreticalpeaks[i].matchedid].matched--;

				theoreticalpeaks[i].matched--;
				theoreticalpeaks[i].matchedid = -1;
			}
		}
	}


	// fill annotations of peaks
	for (int i = 0; i < (int)experimentalpeaks.size(); i++) {
		for (set<int>::iterator it = experimentalpeakmatches[i].begin(); it != experimentalpeakmatches[i].end(); ++it) {
			if (writedescription) {
				if (experimentalpeaks[i].description.compare("") != 0) {
					experimentalpeaks[i].description += ", ";
				}
				experimentalpeaks[i].description += theoreticalpeaks[*it].description.substr(0,theoreticalpeaks[*it].description.find(':'));
				experimentalpeaks[i].matchedrotations.push_back(theoreticalpeaks[*it].rotationid);
				theoreticalpeaks[*it].matchdescription += ", measured mass " + to_string((long double)experimentalpeaks[i].mzratio) + " matched with " + to_string((long double)ppmError(experimentalpeaks[i].mzratio, theoreticalpeaks[*it].mzratio)) + " ppm error";
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
			for (int j = 0; j < series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 1; j++) {
				coveragebyseries += to_string(series[parameters->fragmentionsfortheoreticalspectra[i]][j]);
				if (j < series[parameters->fragmentionsfortheoreticalspectra[i]].size() - 2) {
					coveragebyseries += " ";
				}
			}
			coveragebyseries += "<br/>\n";
		}

	}

	computeSomeStatistics(writedescription);

	return theoreticalpeaksrealsize;
}


int cTheoreticalSpectrum::getNumberOfPeaks() {
	return theoreticalpeaks.size();
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaks() {
	return experimentalpeaksmatched;
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaks(fragmentIonType iontype) {
	return matchedions[iontype];
}


double cTheoreticalSpectrum::getRatioOfMatchedPeaks() {
	return experimentalpeaksmatchedratio;
}


int cTheoreticalSpectrum::getNumberOfMatchedPeaksYB() {
	return matchedions[y_ion]+matchedions[b_ion];
}


void cTheoreticalSpectrum::printMatch(ofstream& os, peptideType peptidetype) {
	vector<string> rotations;
	
	os << endl << "Peptide: " << realpeptidename << endl;

	switch (peptidetype)
	{
	case linear:
	case cyclic:
	case linearpolysaccharide:
		os << "Composition: " << candidate.getComposition();
		break;
	case branched:
	case lasso:
		os << "Composition: " << candidate.getTComposition();
		break;
	default:
		break;
	}
	
	if (valid) {
		os << " (valid)";
	}
	os << endl;

	os << "Number of bricks: " << getNumberOfBricks(candidate.getComposition()) << endl << endl;
	
	if ((peptidetype == linear) || (peptidetype == linearpolysaccharide)) {
		os << "Modifications: ";
		os << candidate.getStartModifID() << " " << candidate.getEndModifID() << endl;
	}

	if (peptidetype == branched) {
		os << "Modifications: ";
		os << candidate.getStartModifID() << " " << candidate.getMiddleModifID() << " " << candidate.getEndModifID() << endl;
	}

	if (peptidetype == lasso) {
		os << "Modifications: ";
		os << candidate.getMiddleModifID() << endl;
	}

	os << "Coverage by series:" << endl;
	os << coveragebyseries << endl;

	switch (peptidetype)
	{
	case linear:
		os << "Matched y-ions and b-ions: " << matchedions[y_ion] + matchedions[b_ion] << endl;
		os << "Matched y-ions: " << matchedions[y_ion] << endl;
		os << "Matched b-ions: " << matchedions[b_ion] << endl;
		os << "Matched a-ions: " << matchedions[a_ion] << endl;
		os << "Matched y*-ions: " << matchedions[y_ion_water_loss] << endl;
		os << "Matched b*-ions: " << matchedions[b_ion_water_loss] << endl;
		os << "Matched a*-ions: " << matchedions[a_ion_water_loss] << endl;
		os << "Matched yx-ions: " << matchedions[y_ion_ammonia_loss] << endl;
		os << "Matched bx-ions: " << matchedions[b_ion_ammonia_loss] << endl;
		os << "Matched ax-ions: " << matchedions[a_ion_ammonia_loss] << endl;
		break;
	case cyclic:
		os << "Matched b-ions: " << matchedions[b_ion] << endl;
		os << "Matched a-ions: " << matchedions[a_ion] << endl;
		os << "Matched b*-ions: " << matchedions[b_ion_water_loss] << endl;
		os << "Matched a*-ions: " << matchedions[a_ion_water_loss] << endl;
		os << "Matched bx-ions: " << matchedions[b_ion_ammonia_loss] << endl;
		os << "Matched ax-ions: " << matchedions[a_ion_ammonia_loss] << endl;
		os << "Matched b*x-ions: " << matchedions[b_ion_water_and_ammonia_loss] << endl;
		os << "Matched a*x-ions: " << matchedions[a_ion_water_and_ammonia_loss] << endl;
		os << "Matched b-ions and b*-ions: " << matchedions[b_ion] + matchedions[b_ion_water_loss] << endl;
		os << "Matched b-ions and bx-ions: " << matchedions[b_ion] + matchedions[b_ion_ammonia_loss] << endl << endl;
		break;
	case branched:
		os << "Matched y-ions and b-ions: " << matchedions[y_ion] + matchedions[b_ion] << endl;
		os << "Matched y-ions: " << matchedions[y_ion] << endl;
		os << "Matched b-ions: " << matchedions[b_ion] << endl;
		os << "Matched a-ions: " << matchedions[a_ion] << endl;
		os << "Matched y*-ions: " << matchedions[y_ion_water_loss] << endl;
		os << "Matched b*-ions: " << matchedions[b_ion_water_loss] << endl;
		os << "Matched a*-ions: " << matchedions[a_ion_water_loss] << endl;
		os << "Matched yx-ions: " << matchedions[y_ion_ammonia_loss] << endl;
		os << "Matched bx-ions: " << matchedions[b_ion_ammonia_loss] << endl;
		os << "Matched ax-ions: " << matchedions[a_ion_ammonia_loss] << endl;
		break;
	case lasso:
		os << "Matched y-ions and b-ions: " << matchedions[y_ion] + matchedions[b_ion] << endl;
		os << "Matched y-ions: " << matchedions[y_ion] << endl;
		os << "Matched b-ions: " << matchedions[b_ion] << endl;
		os << "Matched a-ions: " << matchedions[a_ion] << endl;
		os << "Matched y*-ions: " << matchedions[y_ion_water_loss] << endl;
		os << "Matched b*-ions: " << matchedions[b_ion_water_loss] << endl;
		os << "Matched a*-ions: " << matchedions[a_ion_water_loss] << endl;
		os << "Matched yx-ions: " << matchedions[y_ion_ammonia_loss] << endl;
		os << "Matched bx-ions: " << matchedions[b_ion_ammonia_loss] << endl;
		os << "Matched ax-ions: " << matchedions[a_ion_ammonia_loss] << endl;
		break;
	case linearpolysaccharide:
		break;
	default:
		break;
	}

	os << "Matched peaks: " << experimentalpeaksmatched << endl;
	os << "Total peaks: " << peakstested << endl;
	os << "Sum of Relative Intensities: " << intensityweightedscore << endl;
	os << "Ratio of matched peaks: " << experimentalpeaksmatchedratio*100.0f << "%" << endl << endl;
	os << "Unmatched measured peaks:" << endl << unmatchedpeaks << endl << endl;
	os << "Theoretical peaks:" << endl << theoreticalpeaks.print();
}


void cTheoreticalSpectrum::generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<string>& stringcomposition, vector<int>& intcomposition, fragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, peptideType peptidetype, TRotationInfo* trotation) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	peak.mzratio = parameters->fragmentdefinitions[fragmentiontype].massdifference;
	if ((peptidetype == linear) || (peptidetype == linearpolysaccharide)) {
		peak.mzratio += searchedmodifications[candidate.getStartModifID()].massdifference;
	}
	if ((peptidetype == branched) || (peptidetype == lasso)) {
		peak.mzratio += searchedmodifications[trotation->startmodifID].massdifference;
	}

	peak.iontype = fragmentiontype;
	peak.scrambled = false;

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
	case lasso:
		peak.rotationid = rotationid*6 + trotation->id; // to do - potential bug
		break;
	case linearpolysaccharide:
		peak.rotationid = rotationid;
		break;
	default:
		break;
	}

	for (int i = 0; i < (int)intcomposition.size() - 1; i++) {
		peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();


		if ((peptidetype == branched) || (peptidetype == lasso)) {		
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

		peak.seriesid = i;

		if (writedescription) {

			peak.description = "";
			if (peptidetype == cyclic) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
			}
			if (peptidetype == branched) {
				peak.description += to_string(peak.rotationid + 1) + "_";
			}
			if (peptidetype == lasso) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
				peak.description += to_string(trotation->id + 1) + "_";
			}
			peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 1) + to_string(i + 1);
			if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 1) {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(1, parameters->fragmentdefinitions[fragmentiontype].name.length() - 1);
			}
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
			for (int j = 1; j <= maxcharge; j++) {

				peak.mzratio = charge(tempratio, j);
				peak.charge = j;
				if (writedescription) {
					peak.description = to_string(j) + "+ " + tempdescription;
				}

				if (theoreticalpeaks.size() > peaklistrealsize) {
					theoreticalpeaks[peaklistrealsize] = peak;
				}
				else {
					theoreticalpeaks.add(peak);
				}
				peaklistrealsize++;

			}
			peak.mzratio = tempratio;
			if (writedescription) {
				peak.description = tempdescription;
			}

		}

	}

}


void cTheoreticalSpectrum::generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<string>& stringcomposition, vector<int>& intcomposition, fragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, peptideType peptidetype, TRotationInfo* trotation) {
	cPeak peak;
	double tempratio;
	string tempdescription;

	peak.mzratio = parameters->fragmentdefinitions[fragmentiontype].massdifference;
	if ((peptidetype == linear) || (peptidetype == linearpolysaccharide)) {
		peak.mzratio += searchedmodifications[candidate.getEndModifID()].massdifference;
	}
	if ((peptidetype == branched) || (peptidetype == lasso)) {
		peak.mzratio += searchedmodifications[trotation->endmodifID].massdifference;
	}

	peak.iontype = fragmentiontype;
	peak.scrambled = false;

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
	case lasso:
		peak.rotationid = rotationid*6 + trotation->id; // to do - potential bug
		break;
	case linearpolysaccharide:
		peak.rotationid = rotationid;
		break;
	default:
		break;
	}

	for (int i = (int)intcomposition.size() - 1; i > 0; i--) {
		peak.mzratio += bricksdatabase[intcomposition[i] - 1].getMass();

		if ((peptidetype == branched) || (peptidetype == lasso)) {		
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

		peak.seriesid = (int)intcomposition.size() - i - 1;

		if (writedescription) {

			peak.description = "";
			//if (peptidetype == cyclic) {
			//	peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1); // +"_";
			//}
			if (peptidetype == branched) {
				peak.description += to_string(peak.rotationid + 1) + "_";
			}
			if (peptidetype == lasso) {
				peak.description += to_string(splittingsites[rotationid].first + 1) + "-" + to_string(splittingsites[rotationid].second + 1) + "_";
				peak.description += to_string(trotation->id + 1) + "_";
			}
			peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(0, 1) + to_string((int)intcomposition.size() - i);
			
			if (parameters->fragmentdefinitions[fragmentiontype].name.size() > 1) {
				peak.description += parameters->fragmentdefinitions[fragmentiontype].name.substr(1, parameters->fragmentdefinitions[fragmentiontype].name.length() - 1);
			}
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
			for (int j = 1; j <= maxcharge; j++) {

				peak.mzratio = charge(tempratio, j);
				peak.charge = j;
				if (writedescription) {
					peak.description = to_string(j) + "+ " + tempdescription;
				}

				if (theoreticalpeaks.size() > peaklistrealsize) {
					theoreticalpeaks[peaklistrealsize] = peak;
				}
				else {
					theoreticalpeaks.add(peak);
				}
				peaklistrealsize++;

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


double cTheoreticalSpectrum::getWeightedIntensityScore() {
	return intensityweightedscore;
}


void cTheoreticalSpectrum::setRealPeptideName(cBricksDatabase& bricksdatabase, peptideType peptidetype) {
	switch (peptidetype)
	{
	case linear:
	case cyclic:
	case linearpolysaccharide:
		realpeptidename = bricksdatabase.getRealName(candidate.getComposition());
		break;
	case branched:
	case lasso:
		realpeptidename = candidate.getRealNameTComposition(bricksdatabase);
		break;
	default:
		realpeptidename = "";
		break;
	}
}


void cTheoreticalSpectrum::setAcronymPeptideNameWithHTMLReferences(cBricksDatabase& bricksdatabase, peptideType peptidetype) {
	switch (peptidetype)
	{
	case linear:
	case cyclic:
	case linearpolysaccharide:
		acronympeptidename = bricksdatabase.getAcronymName(candidate.getComposition(), true);
		break;
	case branched:
	case lasso:
		acronympeptidename = candidate.getAcronymsTComposition(bricksdatabase);
		break;
	default:
		acronympeptidename = "";
		break;
	}
}


string& cTheoreticalSpectrum::getRealPeptideName() {
	return realpeptidename;
}


string& cTheoreticalSpectrum::getAcronymPeptideNameWithHTMLReferences() {
	return acronympeptidename;
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


string cTheoreticalSpectrum::getUnmatchedPeaks() {
	return unmatchedpeaks;
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


int cTheoreticalSpectrum::getNumberOfMatchedBricks() {
	return maskscore;
}


vector<visualSeries>& cTheoreticalSpectrum::getVisualCoverage() {
	return visualcoverage;
}


void cTheoreticalSpectrum::setAcronyms(cBricksDatabase& bricksdatabase) {
	vector<int> bricks;
	cBrick b;
	b.clear();
	b.setComposition(candidate.getComposition(), false);
	b.explodeToIntComposition(bricks);

	acronyms.clear();
	for (int i = 0; i < (int)bricks.size(); i++) {
		acronyms.push_back(bricksdatabase[bricks[i] - 1].getAcronymsAsString());
	}
}


vector<string>& cTheoreticalSpectrum::getAcronyms() {
	return acronyms;
}


void cTheoreticalSpectrum::setPath(cDeNovoGraph& graph) {
	cDeNovoGraphNode* currentnode;
	cDeNovoGraphNode* targetnode;
	cEdge* currentedge;
	path = "";
	for (int i = 0; i < (int)candidate.getPath().size(); i++) {
		currentnode = &graph[candidate.getPath()[i].nodeid];
		currentedge = &((*currentnode)[candidate.getPath()[i].edgeid]);
		targetnode = &graph[currentedge->targetnode];

		path += to_string(currentnode->getMZRatio());
		path += " -> ";
		path += to_string(targetnode->getMZRatio());
		path += " using brick(s): ";
		if (currentedge->composition.compare("0") == 0) {
			path += "none";
		}
		else {
			path += graph.getBrickDatabaseWithCombinations()->getAcronymName(currentedge->composition, true);
		}

		path += " (mass difference: " + to_string(currentedge->massdifference) + ", ";
		path += "source intensity: " + to_string(currentnode->getIntensity()) + ", ";
		path += "target intensity: " + to_string(targetnode->getIntensity()) + ", ";
		path += "ppm error: " + to_string(currentedge->ppmerror) + ", ";
		path += "source charge: " + to_string(currentedge->sourcecharge) + ", ";
		path += "target charge: " + to_string(currentedge->targetcharge);
		if ((parameters->peptidetype == branched) || (parameters->peptidetype == lasso)) {
			if (currentedge->middlemodifID > 0) {
				path += ", branch modification: " + parameters->searchedmodifications[currentedge->middlemodifID].name;
			}
		}
		if (currentedge->endmodifID > 0) {
			path += ", terminal modification: " + parameters->searchedmodifications[currentedge->endmodifID].name;
		}
		//path += currentedge->printSourceAnnotation(fragmentdefinitions);
		//path += "->";
		//path += currentedge->printTargetAnnotation(fragmentdefinitions);
		path += ")<br/>";
	}	
}


string& cTheoreticalSpectrum::getPath() {
	return path;
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

	size = (int)matchedions.size();
	os.write((char *)&size, sizeof(int));
	for (map<fragmentIonType, int>::iterator it = matchedions.begin(); it != matchedions.end(); ++it) {
		os.write((char *)&it->first, sizeof(fragmentIonType));
		os.write((char *)&it->second, sizeof(int));
	}

	os.write((char *)&peakstested, sizeof(int));
	os.write((char *)&experimentalpeaksmatchedratio, sizeof(double));

	size = (int)unmatchedpeaks.size();
	os.write((char *)&size, sizeof(int));
	os.write(unmatchedpeaks.c_str(), unmatchedpeaks.size());

	size = (int)coveragebyseries.size();
	os.write((char *)&size, sizeof(int));
	os.write(coveragebyseries.c_str(), coveragebyseries.size());

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

	size = (int)realpeptidename.size();
	os.write((char *)&size, sizeof(int));
	os.write(realpeptidename.c_str(), realpeptidename.size());

	size = (int)acronympeptidename.size();
	os.write((char *)&size, sizeof(int));
	os.write(acronympeptidename.c_str(), acronympeptidename.size());

	size = (int)acronyms.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)acronyms.size(); i++) {
		size = (int)acronyms[i].size();
		os.write((char *)&size, sizeof(int));
		os.write(acronyms[i].c_str(), acronyms[i].size());
	}

	size = (int)path.size();
	os.write((char *)&size, sizeof(int));
	os.write(path.c_str(), path.size());
}


void cTheoreticalSpectrum::load(ifstream& is) {
	int size;
	fragmentIonType iontype;
	int number;

	parameters = 0;

	theoreticalpeaks.load(is);
	experimentalpeaks.load(is);
	candidate.load(is);

	is.read((char *)&experimentalpeaksmatched, sizeof(int));

	is.read((char *)&size, sizeof(int));
	matchedions.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&iontype, sizeof(fragmentIonType));
		is.read((char *)&number, sizeof(int));
		matchedions[iontype] = number;
	}

	is.read((char *)&peakstested, sizeof(int));
	is.read((char *)&experimentalpeaksmatchedratio, sizeof(double));

	is.read((char *)&size, sizeof(int));
	unmatchedpeaks.resize(size);
	is.read(&unmatchedpeaks[0], unmatchedpeaks.size());

	is.read((char *)&size, sizeof(int));
	coveragebyseries.resize(size);
	is.read(&coveragebyseries[0], coveragebyseries.size());

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

	is.read((char *)&size, sizeof(int));
	realpeptidename.resize(size);
	is.read(&realpeptidename[0], realpeptidename.size());

	is.read((char *)&size, sizeof(int));
	acronympeptidename.resize(size);
	is.read(&acronympeptidename[0], acronympeptidename.size());

	is.read((char *)&size, sizeof(int));
	acronyms.resize(size);
	for (int i = 0; i < (int)acronyms.size(); i++) {
		is.read((char *)&size, sizeof(int));
		acronyms[i].resize(size);
		is.read(&acronyms[i][0], acronyms[i].size());
	}

	is.read((char *)&size, sizeof(int));
	path.resize(size);
	is.read(&path[0], path.size());
}

