#include "core/cSummaryFormula.h"
#include "core/cPeaksList.h"
#include "core/cPeakListSeries.h"
#include "core/cBricksDatabase.h"


void reduceMapToStringFormula(map<string, int>& atoms, string& formula) {
	formula = "";
	for (auto it = atoms.begin(); it != atoms.end(); ++it) {
		if (it->second != 0) {
			formula += it->first;
			if (it->second != 1) {
				formula += to_string(it->second);
			}
		}
	}
}


void addStringFormulaToMap(string& formula, map<string, int>& atoms) {
	if ((int)formula.size() == 0) {
		return;
	}

	map<string, int>::iterator it;

	string tmps;
	tmps = formula[0];

	int tmpc = 0;
	bool minus = false;
	for (int i = 1; i < (int)formula.size(); i++) {

		if (((formula[i] >= 'A') && (formula[i] <= 'Z')) || (formula[i] == '+')) {
			if (periodictablemap.count(tmps) > 0) {
				if (tmpc == 0) {
					it = atoms.find(tmps);
					if (it == atoms.end()) {
						atoms[tmps] = 1;
					}
					else {
						it->second += 1;
					}
				}
				else {
					it = atoms.find(tmps);
					if (it == atoms.end()) {
						atoms[tmps] = minus ? -tmpc : tmpc;
					}
					else {
						it->second += minus ? -tmpc : tmpc;
					}
				}
			}
			else {
				return;
			}

			tmps = formula[i];
			tmpc = 0;
			minus = false;
		}

		if ((formula[i] >= 'a') && (formula[i] <= 'z')) {
			tmps += formula[i];
		}

		if ((formula[i] >= '0') && (formula[i] <= '9')) {
			tmpc *= 10;
			tmpc += formula[i] - '0';
		}

		if (formula[i] == '-') {
			minus = true;
		}

	}

	// the last element
	if (periodictablemap.count(tmps) > 0) {
		if (tmpc == 0) {
			it = atoms.find(tmps);
			if (it == atoms.end()) {
				atoms[tmps] = 1;
			}
			else {
				it->second += 1;
			}
		}
		else {
			it = atoms.find(tmps);
			if (it == atoms.end()) {
				atoms[tmps] = minus ? -tmpc : tmpc;
			}
			else {
				it->second += minus ? -tmpc : tmpc;
			}
		}
	}
}


void mergeMaps(map<string, int>& source, map<string, int>& target) {
	map<string, int>::iterator it2;
	for (auto& it : source) {
		it2 = target.find(it.first);
		if (it2 == target.end()) {
			target[it.first] = it.second;
		}
		else {
			it2->second += it.second;
		}
	}
}


void rechargeMap(int charge, map<string, int>& atoms) {
	if (charge == 1) {
		return;
	}

	map<string, int>::iterator it;

	it = atoms.find("H");
	if (it == atoms.end()) {
		atoms["H"] = charge - 1;
	}
	else {
		it->second += charge - 1;
	}

	it = atoms.find("+");
	if (it == atoms.end()) {
		atoms["+"] = charge - 1;
	}
	else {
		it->second += charge - 1;
	}
}


void cSummaryFormula::combineAtoms(cPeakListSeries& peaklistseries, cPeaksList& isotopeprofile, int depth, double mass, double intensity, int charge, string description, double minimumabsoluteintensity) {
	if (depth < peaklistseries.size()) {
		for (int i = 0; i < peaklistseries[depth].size(); i++) {
			combineAtoms(peaklistseries, isotopeprofile, depth + 1, mass + peaklistseries[depth][i].mzratio, intensity * peaklistseries[depth][i].absoluteintensity, charge, description + peaklistseries[depth][i].description, minimumabsoluteintensity);
		}
	}
	else {
		if (intensity >= minimumabsoluteintensity) {
			cPeak p;
			p.mzratio = mass/(double)charge;
			p.absoluteintensity = intensity;
			p.description = description;
			isotopeprofile.add(p);
		}
	}
}


double cSummaryFormula::getIntensity(cBricksDatabase& bricksprobabilities, vector<int>& combarray, int mostintenseid, int mostintenseatomscount, int isotopecount) {
	vector<int> isotopesummary;
	isotopesummary.resize(isotopecount, 0);
	for (int i = 0; i < (int)combarray.size(); i++) {
		isotopesummary[combarray[i] - 1]++;
	}
	isotopesummary[mostintenseid] += mostintenseatomscount;

	double intensity = 1;
	for (int i = isotopesummary[mostintenseid] + 1; i <= mostintenseatomscount + (int)combarray.size(); i++) {
		intensity *= (double)i;
	}

	for (int i = 0; i < (int)isotopesummary.size(); i++) {
		if (i != mostintenseid) {
			intensity /= fact(isotopesummary[i]);
		}
	}

	for (int i = 0; i < (int)combarray.size(); i++) {
		intensity *= bricksprobabilities[combarray[i] - 1].getMass();
	}

	if (mostintenseatomscount > 0) {
		intensity *= pow(bricksprobabilities[mostintenseid].getMass(), (double)mostintenseatomscount);
	}

	return intensity;
}


void cSummaryFormula::getIsotopeSummary(string& description, cBricksDatabase& bricks, vector<int>& combarray, int mostintenseid, int mostintenseatomscount, int isotopecount, bool positive) {
	if (isotopecount == 0) {
		return;
	}

	vector<int> isotopesummary;
	isotopesummary.resize(isotopecount, 0);
	for (int i = 0; i < (int)combarray.size(); i++) {
		isotopesummary[combarray[i] - 1]++;
	}
	isotopesummary[mostintenseid] += mostintenseatomscount;

	if ((bricks.size() > 0) && (bricks[0].getSummary().compare("+") == 0)) {
		description = to_string(isotopesummary[0]);
		if (positive) {
			description += "+ ";
		}
		else {
			description += "- ";
		}
		return;
	}

	description = "";
	for (int i = 0; i < (int)isotopesummary.size(); i++) {
		if (isotopesummary[i] != 0) {
			description += bricks[i].getSummary();
			if ((isotopesummary[i] != 1)) {
				description += "<sub>" + to_string(isotopesummary[i]) + "</sub>";
			}
			description += " ";
		}
	}
}


void cSummaryFormula::reduceIsotopeProfile(cPeaksList& isotopeprofile, cPeaksList& reducedprofile, double fwhm) {
	reducedprofile = isotopeprofile;
	reducedprofile.sortbyAbsoluteIntensityDesc();

	// initialize clusters
	vector<cPeaksList> clusters;
	clusters.resize(reducedprofile.size());
	for (int i = 0; i < reducedprofile.size(); i++) {
		clusters[i].add(reducedprofile[i]);
	}

	// merge clusters
	for (int i = 0; i < clusters.size(); i++) {
		if (clusters[i].size() == 0) {
			continue;
		}
		for (int j = i + 1; j < clusters.size(); j++) {
			if (clusters[j].size() == 0) {
				continue;
			}
			if (fabs(clusters[j][0].mzratio - clusters[i][0].mzratio) < fwhm) {
				clusters[i].add(clusters[j][0]);
				clusters[j].clear();
			}
		}
	}

	// reduce peaks
	reducedprofile.clear();
	double relsum;
	int isize;
	for (int i = 0; i < clusters.size(); i++) {
		isize = clusters[i].size();

		if (isize == 0) {
			continue;
		}
		
		cPeak peak;
		relsum = 0;

		for (int j = 0; j < isize; j++) {
			relsum += clusters[i][j].absoluteintensity;
		}

		for (int j = 0; j < isize; j++) {
			peak.mzratio += clusters[i][j].mzratio*clusters[i][j].absoluteintensity;
			peak.absoluteintensity += clusters[i][j].absoluteintensity;
			//peak.relativeintensity += clusters[i][j].relativeintensity;
			peak.description += clusters[i][j].description;
			if (isize > 1) {
				peak.description += "[" + QString::number(clusters[i][j].absoluteintensity / relsum * 100.0, 'f', 2).toStdString() + "%]";
			}
			peak.description += ";";
		}

		peak.mzratio /= relsum;
		reducedprofile.add(peak);
	}
}


cSummaryFormula::cSummaryFormula() {
	clear();
}


cSummaryFormula::cSummaryFormula(string& formula, bool partial) {
	setFormula(formula, partial);
}


void cSummaryFormula::clear() {
	formula = "";
	partial = false;
}


void cSummaryFormula::setFormula(const string& formula, bool partial) {
	this->formula = formula;
	this->partial = partial;
}


string cSummaryFormula::getSummary() {
	return formula;
}


string cSummaryFormula::getFancySummary(int charge) {
	int size = (int)formula.size();
	string tmpformula;

	if (size == 0) {
		return tmpformula;
	}

	for (int i = 0; i < size; i++) {
		if ((formula[i] >= 'A') && (formula[i] <= 'Z')) {
			tmpformula = formula.substr(i);
			break;
		}
	}

	if (charge > 0) {
		tmpformula += " " + to_string(charge) + "+";
	}

	if (charge < 0) {
		tmpformula += " " + to_string(abs(charge)) + "-";
	}

	return tmpformula;
}


bool cSummaryFormula::isPartial() {
	return partial;
}


void cSummaryFormula::setPartial(bool partial) {
	this->partial = partial;
}


void cSummaryFormula::addFormula(string& formula, bool remove) {
	if ((int)formula.size() == 0) {
		return;
	}

	map<string, int> atoms;
	atoms.clear();

	addStringFormulaToMap(formula, atoms);

	if (remove) {
		for (auto it = atoms.begin(); it != atoms.end(); ++it) {
			it->second = -it->second;
		}
	}

	addStringFormulaToMap(this->formula, atoms);

	reduceMapToStringFormula(atoms, this->formula);
}


void cSummaryFormula::setFromMap(map<string, int>& atoms, bool partial) {
	reduceMapToStringFormula(atoms, this->formula);
	this->partial = partial;
}


bool cSummaryFormula::isValid(string& errormessage) {
	if ((int)formula.size() == 0) {
		return true;
	}

	regex rx;
	rx = "^(([A-Z][a-z]*|\\+)((-[0-9]+)|([0-9]*)))+$";

	try {
		if (!regex_search(formula, rx)) {
			errormessage = "Invalid summary: " + formula + ".\n\n";
			return false;
		}
	}
	catch (regex_error& e) {
		errormessage = "getMassFromResidueSummary: regex_search failed, error no. " + to_string((int)e.code()) + "\n";
		return false;
	}

	errormessage = "";

	string tmps;
	tmps = formula[0];

	for (int i = 1; i < (int)formula.size(); i++) {

		if (((formula[i] >= 'A') && (formula[i] <= 'Z')) || (formula[i] == '+')) {
			if (periodictablemap.count(tmps) == 0) {
				errormessage = "Undefined atom name: " + tmps + ".\n\n";
				return false;
			}

			tmps = formula[i];
		}

		if ((formula[i] >= 'a') && (formula[i] <= 'z')) {
			tmps += formula[i];
		}

	}

	// the last element
	if (periodictablemap.count(tmps) == 0) {
		errormessage = "Undefined atom name: " + tmps + ".\n\n";
		return false;
	}

	return true;
}


bool cSummaryFormula::isEmpty() {
	return (formula.size() == 0);
}


bool cSummaryFormula::hasAllElementsPositive() {
	// start from the position 2 to keep negatively charged ions but to remove incorrect ions e.g. "+-1C3H-1"
	if (formula.find("-", 2) == string::npos) {
		return true;
	}
	return false;
}


double cSummaryFormula::getMass() {
	if ((int)formula.size() == 0) {
		return 0;
	}

	double sum = 0;

	string tmps;
	tmps = formula[0];

	int tmpc = 0;

	bool minus = false;

	for (int i = 1; i < (int)formula.size(); i++) {

		if (((formula[i] >= 'A') && (formula[i] <= 'Z')) || (formula[i] == '+')) {
			if (periodictablemap.count(tmps) > 0) {
				if (tmpc == 0) {
					sum += periodictablemap[tmps];
				}
				else {
					sum += minus?-tmpc*periodictablemap[tmps]:tmpc*periodictablemap[tmps];
				}
			}
			else {
				return 0;
			}

			tmps = formula[i];
			tmpc = 0;
			minus = false;
		}

		if ((formula[i] >= 'a') && (formula[i] <= 'z')) {
			tmps += formula[i];
		}

		if ((formula[i] >= '0') && (formula[i] <= '9')) {
			tmpc *= 10;
			tmpc += formula[i] - '0';
		}

		if (formula[i] == '-') {
			minus = true;
		}

	}

	// the last element
	if (periodictablemap.count(tmps) > 0) {
		if (tmpc == 0) {
			sum += periodictablemap[tmps];
		}
		else {
			sum += minus?-tmpc*periodictablemap[tmps]:tmpc*periodictablemap[tmps];
		}
	}
	else {
		return 0;
	}

	return sum;
}


cPeaksList cSummaryFormula::getIsotopePattern(double fwhm, int charge, bool positive, bool writedescription) {
	map<string, int> atoms;
	addStringFormulaToMap(formula, atoms);

	cBricksDatabase bricksdatabase;
	cBricksDatabase bricksprobabilities;
	vector<int> combarray;
	cBrick brick;
	int numberofbasicbricks;
	int maximumbricksincombination;
	
	double mostintense;
	int mostintenseid;

	cPeakListSeries peaklistseries;
	for (auto it = atoms.begin(); it != atoms.end(); ++it) {
		cPeaksList peaklist;

		string atom = it->first;
		int atomcount = abs(it->second);
		map<string, isotope> atomisotopes = isotopemap[atom];

		numberofbasicbricks = 0;
		mostintense = 0;
		mostintenseid = 0;
		bricksdatabase.clear();
		bricksprobabilities.clear();
		for (auto it2 = atomisotopes.begin(); it2 != atomisotopes.end(); ++it2) {
			brick.clear();
			brick.setComposition(to_string(numberofbasicbricks + 1), false);
			brick.setMass(it2->second.mass);
			brick.setSummary(it2->first);
			bricksdatabase.push_back(brick);

			brick.clear();
			brick.setComposition(to_string(numberofbasicbricks + 1), false);
			brick.setMass(it2->second.probability);
			bricksprobabilities.push_back(brick);

			if (it2->second.probability > mostintense) {
				mostintense = it2->second.probability;
				mostintenseid = numberofbasicbricks;
			}

			numberofbasicbricks++;
		}

		maximumbricksincombination = min(5, atomcount);

		combarray.clear();
		for (int i = 0; i < maximumbricksincombination; i++) {
			combarray.push_back(1);
		}

		do {
			cPeak peak;
			peak.mzratio = bricksdatabase.getMassOfComposition(combarray, numberofbasicbricks) + periodictablemap[atom] * (atomcount - maximumbricksincombination);
			if (it->second < 0) {
				peak.mzratio = -peak.mzratio;
			}
			peak.absoluteintensity = getIntensity(bricksprobabilities, combarray, mostintenseid, atomcount - maximumbricksincombination, (int)atomisotopes.size());
			if (writedescription) {
				getIsotopeSummary(peak.description, bricksdatabase, combarray, mostintenseid, atomcount - maximumbricksincombination, (int)atomisotopes.size(), positive);
			}
			peaklist.add(peak);
		} while (bricksdatabase.nextCombination(combarray, numberofbasicbricks, maximumbricksincombination, 0, 0));

		peaklistseries.addPeakList(peaklist);
	}

	cPeaksList isotopeprofile;
	combineAtoms(peaklistseries, isotopeprofile, 0, 0, 1, charge, "", 0.00001);
	isotopeprofile.normalizeIntenzity();
	isotopeprofile.cropRelativeIntenzity(0.1);

	cPeaksList reducedprofile;
	reduceIsotopeProfile(isotopeprofile, reducedprofile, fwhm);
	reducedprofile.normalizeIntenzity();
	reducedprofile.cropRelativeIntenzity(0.1);

	return reducedprofile;
}


void cSummaryFormula::store(ofstream& os) {
	storeString(formula, os);
	os.write((char *)&partial, sizeof(bool));
}


void cSummaryFormula::load(ifstream& is) {
	loadString(formula, is);
	is.read((char *)&partial, sizeof(bool));
}

