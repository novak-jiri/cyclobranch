#include "core/cBricksDatabase.h"

#include "gui/cMainThread.h"


bool compareBrickMasses(const cBrick& a, const cBrick& b) {
	return (a.getMass() < b.getMass());
}


void getNameOfCompositionFromIntVector(string& composition, vector<int>& combarray) {
	composition = "";
	for (int i = 0; i < (int)combarray.size(); i++) {
		if (combarray[i] > 0) {
			composition += to_string(combarray[i]);
		}
		if ((i < (int)combarray.size() - 1) && (combarray[i + 1] > 0)) {
			composition += "-";
		}
	}
}


string& getReversedNameOfCompositionFromStringVector(string& composition, vector<string>& stringcomposition) {
	composition = "";
	for (int i = (int)stringcomposition.size() - 1; i >= 0; i--) {
		composition += stringcomposition[i];
		if (i > 0) {
			composition += "-";
		}
	}
	return composition;
}


void generateBricksPermutations(vector<string>& bricks, vector<string>& currentpermutation, vector<string>& permutations) {
	if (bricks.size() == 0) {
		string s = "";
		for (int i = 0; i < (int)currentpermutation.size(); i++) {
			s += currentpermutation[i];
			if (i < (int)currentpermutation.size() - 1) {
				s += "-";
			}
		}
		permutations.push_back(s);
	}
	else {
		for (int i = 0; i < (int)bricks.size(); i++) {
			currentpermutation.push_back(bricks[i]);
			vector<string> subbricks = bricks;
			subbricks.erase(subbricks.begin() + i);
			generateBricksPermutations(subbricks, currentpermutation, permutations);
			currentpermutation.pop_back();
		}
	}
}


void cBricksDatabase::addLossToMap(string& lossstr, vector<int>& lossids) {
	lossids.clear();

	if (lossstr.size() == 0) {
		return;
	}

	map<string, int> tmpmap;
	string tmpstring;
	int tmpsize;

	cSummaryFormula formula;
	double tmpmass;

	size_t last = 0;
	size_t next = 0;
	while ((next = lossstr.find(";", last)) != string::npos) {
		tmpstring = lossstr.substr(last, next - last);

		if (lossorders.count(tmpstring) > 0) {
			lossids.push_back(lossorders[tmpstring]);
		}
		else {
			tmpsize = (int)lossorders.size();
			lossorders[tmpstring] = tmpsize;
			lossids.push_back(tmpsize);

			losssummaries.push_back(tmpstring);

			formula.clear();
			formula.addFormula(tmpstring, true);
			tmpmass = formula.getMass();
			lossmasses.push_back(tmpmass);

			tmpmap.clear();
			tmpstring = formula.getSummary();
			addStringFormulaToMap(tmpstring, tmpmap);
			lossmaps.push_back(tmpmap);
		}

		last = next + 1;
	}

	tmpstring = lossstr.substr(last);

	if (lossorders.count(tmpstring) > 0) {
		lossids.push_back(lossorders[tmpstring]);
	}
	else {
		tmpsize = (int)lossorders.size();
		lossorders[tmpstring] = tmpsize;
		lossids.push_back(tmpsize);

		losssummaries.push_back(tmpstring);

		formula.clear();
		formula.addFormula(tmpstring, true);
		tmpmass = formula.getMass();
		lossmasses.push_back(tmpmass);

		tmpmap.clear();
		tmpstring = formula.getSummary();
		addStringFormulaToMap(tmpstring, tmpmap);
		lossmaps.push_back(tmpmap);
	}
}


double cBricksDatabase::getMassFromCombCounts(vector<int>& combcounts, vector<double>& combmasses) {
	int size = (int)combcounts.size();
	double mass = 0;
	for (int i = 0; i < size; i++) {
		if (combcounts[i] > 0) {
			mass += (double)(combcounts[i]) * combmasses[i];
		}
	}
	return mass;
}


bool cBricksDatabase::nextCombination(vector<int>& combarray, int numberofbasicbricks, int maximumbricksincombination, double maximumcumulativemass, double neutralprecursormass) {
	int pointer = 0;
	int cyFlag = 0;
	double mass;

	do {

		combarray[pointer]++;

		// set combarray[pointer] to the maximum value when outside of the mass range
		mass = getMassOfComposition(combarray, numberofbasicbricks);

		if ((cyFlag == 0) && (combarray[pointer] <= numberofbasicbricks) && (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)))) {
			combarray[pointer] = numberofbasicbricks + 1;
		}

		if (combarray[pointer] <= numberofbasicbricks) {

			if (cyFlag > 0) {

				for (int i = pointer - 1; i >= 0; i--) {
					combarray[i] = combarray[pointer];
				}

				// skip combinations outside of the mass range
				mass = getMassOfComposition(combarray, numberofbasicbricks);

				while ((combarray[pointer] <= numberofbasicbricks) && (pointer < maximumbricksincombination - 1) && (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)))) {
					pointer++;
					combarray[pointer]++;

					for (int i = pointer - 1; i >= 0; i--) {
						combarray[i] = combarray[pointer];
					}

					mass = getMassOfComposition(combarray, numberofbasicbricks);
				}

				if ((combarray[pointer] > numberofbasicbricks) || ((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass))) {
					return false;
				}

			}

			return true;

		}
		else {

			cyFlag = 1;
			pointer++;

		}

	} while (pointer < maximumbricksincombination);

	return false;
}


bool cBricksDatabase::nextCombinationFast(vector<int>& combarray, vector<int>& combcounts, vector<double>& combmasses, double& mass, int numberofbasicbricks, int maximumbricksincombination, double maximumcumulativemass, double neutralprecursormass) {
	int pointer = 0;
	int cyFlag = 0;

	mass = 0;

	do {

		if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
			combcounts[combarray[pointer] - 1]--;
		}

		combarray[pointer]++;

		if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
			combcounts[combarray[pointer] - 1]++;
		}

		// set combarray[pointer] to the maximum value when outside of the mass range
		mass = getMassFromCombCounts(combcounts, combmasses);

		if ((cyFlag == 0) && (combarray[pointer] <= numberofbasicbricks) && (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)))) {
			if (combarray[pointer] > 0) {
				combcounts[combarray[pointer] - 1]--;
			}

			combarray[pointer] = numberofbasicbricks + 1;

			mass = getMassFromCombCounts(combcounts, combmasses);
		}

		if (combarray[pointer] <= numberofbasicbricks) {

			if (cyFlag > 0) {

				for (int i = pointer - 1; i >= 0; i--) {
					if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
						combcounts[combarray[i] - 1]--;
					}

					combarray[i] = combarray[pointer];

					if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
						combcounts[combarray[i] - 1]++;
					}
				}

				// skip combinations outside of the mass range
				mass = getMassFromCombCounts(combcounts, combmasses);

				while ((combarray[pointer] <= numberofbasicbricks) && (pointer < maximumbricksincombination - 1) && (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)))) {
					pointer++;

					if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
						combcounts[combarray[pointer] - 1]--;
					}

					combarray[pointer]++;

					if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
						combcounts[combarray[pointer] - 1]++;
					}

					for (int i = pointer - 1; i >= 0; i--) {
						if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
							combcounts[combarray[i] - 1]--;
						}

						combarray[i] = combarray[pointer];

						if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
							combcounts[combarray[i] - 1]++;
						}
					}

					mass = getMassFromCombCounts(combcounts, combmasses);
				}

				if ((combarray[pointer] > numberofbasicbricks) || ((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass))) {
					return false;
				}

			}

			return true;

		}
		else {

			cyFlag = 1;
			pointer++;

		}

	} while (pointer < maximumbricksincombination);

	return false;
}


bool cBricksDatabase::nextCombinationFastLimited(vector<int>& combarray, vector<int>& combcounts, vector<int>& comblimits, vector<double>& combmasses, double& mass, int numberofbasicbricks, int maximumbricksincombination, double maximumcumulativemass, double neutralprecursormass) {
	int pointer = 0;
	int cyFlag = 0;
	int lastval;
	
	mass = 0;
	
	do {

		if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
			combcounts[combarray[pointer] - 1]--;
		}

		combarray[pointer]++;

		if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
			combcounts[combarray[pointer] - 1]++;
		}

		// set combarray[pointer] to the maximum value when outside of the mass range
		mass = getMassFromCombCounts(combcounts, combmasses);

		if ((cyFlag == 0) && (combarray[pointer] <= numberofbasicbricks) && (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)) || ((comblimits[combarray[pointer] - 1] > 0) && (combcounts[combarray[pointer] - 1] > comblimits[combarray[pointer] - 1])))) {
			
			if (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass))) {

				lastval = combarray[pointer];

				if (combarray[pointer] > 0) {
					combcounts[combarray[pointer] - 1]--;
				}

				combarray[pointer] = numberofbasicbricks + 1;

				while ((pointer + 1 < maximumbricksincombination) && (combarray[pointer + 1] >= lastval - 1)) {

					pointer++;

					if (combarray[pointer] > 0) {
						combcounts[combarray[pointer] - 1]--;
					}

					combarray[pointer] = numberofbasicbricks + 1;

				}
				
			}
			else {

				while ((combarray[pointer] <= numberofbasicbricks) && ((comblimits[combarray[pointer] - 1] > 0) && (combcounts[combarray[pointer] - 1] > comblimits[combarray[pointer] - 1]))) {

					if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
						combcounts[combarray[pointer] - 1]--;
					}

					combarray[pointer]++;

					if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
						combcounts[combarray[pointer] - 1]++;
					}

				}
				
			}

			mass = getMassFromCombCounts(combcounts, combmasses);

		}

		if (combarray[pointer] <= numberofbasicbricks) {

			if (cyFlag > 0) {

				for (int i = pointer - 1; i >= 0; i--) {
					if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
						combcounts[combarray[i] - 1]--;
					}

					combarray[i] = combarray[i + 1];
					while ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks) && (comblimits[combarray[i] - 1] > 0) && (combcounts[combarray[i] - 1] + 1 > comblimits[combarray[i] - 1])) {
						combarray[i]++;
					}

					if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
						combcounts[combarray[i] - 1]++;
					}
				}

				// skip combinations outside of the mass range
				mass = getMassFromCombCounts(combcounts, combmasses);

				while ((combarray[pointer] <= numberofbasicbricks) && (pointer < maximumbricksincombination - 1) && (((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)) || ((comblimits[combarray[pointer] - 1] > 0) && (combcounts[combarray[pointer] - 1] > comblimits[combarray[pointer] - 1])) || (combarray[0] > numberofbasicbricks))) {
					pointer++;

					if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
						combcounts[combarray[pointer] - 1]--;
					}

					combarray[pointer]++;

					if ((combarray[pointer] > 0) && (combarray[pointer] <= numberofbasicbricks)) {
						combcounts[combarray[pointer] - 1]++;
					}

					for (int i = pointer - 1; i >= 0; i--) {
						if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
							combcounts[combarray[i] - 1]--;
						}

						combarray[i] = combarray[i + 1];
						while ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks) && (comblimits[combarray[i] - 1] > 0) && (combcounts[combarray[i] - 1] + 1 > comblimits[combarray[i] - 1])) {
							combarray[i]++;
						}

						if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
							combcounts[combarray[i] - 1]++;
						}
					}

					mass = getMassFromCombCounts(combcounts, combmasses);
				}

				if ((combarray[pointer] > numberofbasicbricks) || ((maximumcumulativemass > 0) && (mass > maximumcumulativemass)) || ((neutralprecursormass > 0) && (mass > neutralprecursormass)) || ((comblimits[combarray[pointer] - 1] > 0) && (combcounts[combarray[pointer] - 1] > comblimits[combarray[pointer] - 1])) || (combarray[0] > numberofbasicbricks)) {
					return false;
				}

			}

			return true;

		}
		else  {

			cyFlag = 1;
			pointer++;

		}

	} while (pointer < maximumbricksincombination);

	return false;
}


cBricksDatabase::cBricksDatabase() {
	clear();
}


int cBricksDatabase::loadFromPlainTextStream(ifstream &stream, string& errormessage, bool ignoreerrors, bool skiph2blocks) {
	string s, tmps;
	cBrick b;
	size_t pos;
	double mass;

	regex rx;
	string name;

	bool error = false;
	errormessage = "";
	cSummaryFormula formula;
	vector<int> lossids;
	
	bricks.clear();
	while (stream.good()) {
		getline(stream,s);

		// skip a comment
		if ((s.length() > 0) && (s[0] == '#')) {
			continue;
		}

		// replaces commas with dots
		pos = s.find(',');
		while (pos != string::npos) {
			s.replace(pos, 1, ".");
			pos = s.find(',');
		}

		b.clear();

		// load the name
		pos = s.find('\t');
		if (pos != string::npos) {
			b.setName(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		name = b.getName();

		rx = "^\\(-2H\\) ";
		if (regex_search(name, rx)) {
			b.setResidueLossType(h2_loss);
		}

		// load acronyms
		pos = s.find('\t');
		if (pos != string::npos) {
			b.setAcronyms(s.substr(0, pos));
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load summary
		pos = s.find('\t');
		if (pos != string::npos) {
			b.setSummary(s.substr(0, pos));
			b.createSummaryMap();
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load the mass (unused !)
		pos = s.find('\t');
		if (pos != string::npos) {
			sscanf_s(s.substr(0, pos).c_str(), "%lf", &mass);
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load the list of losses
		pos = s.find('\t');
		if (pos != string::npos) {
			tmps = s.substr(0, pos);
			b.setLosses(tmps);
			addLossToMap(tmps, lossids);
			b.setLossIDs(lossids);
			s = s.substr(pos + 1);
		}
		else {
			// nothing to do - compatibility with the old format of database
		}

		// load references
		#if OS_TYPE != WIN
			if ((s.size() > 0) && (s.back() == '\r')) {
				s = s.substr(0, s.size() - 1);
			}
		#endif
		b.setReferences(s);

		// store brick
		if ((ignoreerrors || (!ignoreerrors && !b.empty())) && (!skiph2blocks || (skiph2blocks && (b.getResidueLossType() == h2o_loss)))) {

			// calculate mass from the summary
			formula.clear();
			formula.setFormula(b.getSummary());
			if (formula.isValid(errormessage)) {
				b.setMass(formula.getMass());
			}
			else {
				if (!ignoreerrors) {
					error = true;
					break;
				}
			}

			if (!ignoreerrors && (b.getMass() == 0)) {
				error = true;
				errormessage = "Invalid brick no. " + to_string(size() + 1) + ".\n\n";
				break;
			}

			bricks.push_back(b);
		}
	}

	if (!ignoreerrors && error) {
		return -1;
	}

	return 0;
}


void cBricksDatabase::storeToPlainTextStream(ofstream &stream) {
	for (int i = 0; i < (int)bricks.size(); i++) {
		stream << bricks[i].getName() << "\t";
		stream << bricks[i].getAcronymsAsString() << "\t";
		stream << bricks[i].getSummary() << "\t";
		stream << std::fixed << std::setprecision(10) << bricks[i].getMass() << "\t";
		stream << bricks[i].getLosses() << "\t";
		stream << bricks[i].getReferencesAsString() << endl;
	}
}


int cBricksDatabase::size() {
	return (int)bricks.size();
}


void cBricksDatabase::print(/*ostream &stream*/) {
	for (int i = 0; i < size(); i++) {
		printf("%i %s %lf\n", i + 1, bricks[i].getAcronymsAsString().c_str(), bricks[i].getMass());
	}
}


void cBricksDatabase::sortbyMass() {
	sort(bricks.begin(), bricks.end(), compareBrickMasses);
}


cBrick& cBricksDatabase::operator[](int position) {
	return bricks[position];
}


double cBricksDatabase::getMassOfComposition(vector<int>& combarray, int numberofbasicbricks) {
	double mass = 0;
	int i = 0;
	int size = (int)combarray.size();
	while (i < size) {
		if ((combarray[i] > 0) && (combarray[i] <= numberofbasicbricks)) {
			mass += bricks[combarray[i] - 1].getMass();
		}
		i++;
	}
	return mass;
}


string cBricksDatabase::getRealName(string& composition) {
	cBrick b;
	b.setComposition(composition, false);
	
	vector<int> items;
	b.explodeToIntComposition(items);
	
	string s = "";
	for (int i = 0; i < (int)items.size(); i++) {
		s += "[" + bricks[items[i]-1].getName() + "]";
		if (i < (int)items.size() - 1) {
			s += "-";
		}
	}

	return s;
}


string cBricksDatabase::getAcronymName(string& composition, bool useHTMLReferences) {
	cBrick b;
	b.setComposition(composition, false);
	
	vector<int> items;
	b.explodeToIntComposition(items);
	
	string s = "";
	for (int i = 0; i < (int)items.size(); i++) {
		s += "[";
		if (useHTMLReferences) {
			s += bricks[items[i]-1].getAcronymsWithReferencesAsHTMLString();
		}
		else {
			s += bricks[items[i]-1].getAcronymsAsString();	
		}
		s += "]";
		if (i < (int)items.size() - 1) {
			s += "-";
		}
	}

	return s;
}


string cBricksDatabase::getTagName(string& composition) {
	cBrick b;
	b.setComposition(composition, false);
	
	vector<int> items;
	b.explodeToIntComposition(items);
	
	string s = "";
	for (int i = 0; i < (int)items.size(); i++) {
		s += "[";
		s += bricks[items[i]-1].getFirstAcronymAsString();	
		s += "]";
		if (i < (int)items.size() - 1) {
			s += "-";
		}
	}

	return s;
}


string cBricksDatabase::getAcronymNameOfTPeptide(const string& tcomposition, bool useHTMLReferences) {
	string comp = tcomposition;

	int leftbracket = getNumberOfBricks(comp.substr(0, comp.find('(')));
	comp[comp.find('(')] = '-';

	if (comp[0] == '-') {
		comp.erase(comp.begin());
	}

	int rightbracket = getNumberOfBricks(comp.substr(0, comp.find(')'))) - 1;
	comp[comp.find(')')] = '-';

	if (comp.back() == '-') {
		comp.erase(comp.end() - 1);
	}

	cBrick b;
	b.setComposition(comp, false);

	vector<int> items;
	b.explodeToIntComposition(items);
	
	string s = "";
	for (int i = 0; i < (int)items.size(); i++) {

		if (i == leftbracket) {
			if (leftbracket > 0) {
				s += " ";
			}
			s += "( ";
		}

		s += "[";
		if (useHTMLReferences) {
			s += bricks[items[i]-1].getAcronymsWithReferencesAsHTMLString();
		}
		else {
			s += bricks[items[i]-1].getAcronymsAsString();	
		}
		s += "]";
		
		if (i == rightbracket) {
			s += " ) ";
		}
		else {
			if ((i < (int)items.size() - 1) && (leftbracket != i + 1)) {
				s += "-";
			}
		}

	}

	return s;
}


string cBricksDatabase::getTagNameOfTPeptide(string& tcomposition) {
	string comp = tcomposition;
	int leftbracket = getNumberOfBricks(comp.substr(0, comp.find('(')));
	comp[comp.find('(')] = '-';
	int rightbracket = getNumberOfBricks(comp.substr(0, comp.find(')')));
	comp[comp.find(')')] = '-';
	
	cBrick b;
	b.setComposition(comp, false);

	vector<int> items;
	b.explodeToIntComposition(items);
	
	string s = "";
	for (int i = 0; i < (int)items.size(); i++) {
		s += "[";
		s += bricks[items[i]-1].getFirstAcronymAsString();	
		s += "]";
		if (i < (int)items.size() - 1) {
			if (i == leftbracket - 1) {
				s += "\\(";
			}
			else if (i == rightbracket - 1) {
				s += "\\)";
			}
			else {
				s += "-";
			}
		}
	}

	return s;
}


void cBricksDatabase::clear() {
	bricks.clear();
	lossorders.clear();
	lossmasses.clear();
	losssummaries.clear();
	lossmaps.clear();
}


bool cBricksDatabase::replaceAcronymsByIDs(string& sequence, string& errormessage) {
	string s = "";
	string acronym;
	bool insidebrick = false;
	bool found = false;
	//regex rx;
	//rx = "^\\d*\\.?\\d*$";

	for (int i = 0; i < (int)sequence.size(); i++) {
		if (sequence[i] == '[') {
			acronym = "";
			insidebrick = true;
			continue;
		}

		if (sequence[i] == ']') {
			found = false;
			for (int j = 0; j < (int)bricks.size(); j++) {
				for (int k = 0; k < (int)bricks[j].getAcronyms().size(); k++) {
					if (acronym.compare(bricks[j].getAcronyms()[k]) == 0) {
						s += to_string(j + 1);
						found = true;
						break;
					}
				}
				if (found) {
					break;
				}
			}

			// valinomycin oznaci i neco navic pri connect a 1,1,1
			//if (!found) {
			//	if (regex_search(acronym, rx)) {
			//		found = true;
			//	}
			//}
			
			if (!found) {
				errormessage = "Invalid brick acronym '" + acronym + "' typed in the sequence: " + sequence + ".";
				return false;
			}

			insidebrick = false;
			continue;
		}

		if (insidebrick) {
			acronym += sequence[i];
		}
		else {
			s += sequence[i];
		}
	}

	sequence = s;

	return true;
}


void cBricksDatabase::push_back(cBrick& brick) {
	bricks.push_back(brick);
}


void cBricksDatabase::removeLastBrick() {
	bricks.pop_back();
}


string cBricksDatabase::calculateSummaryFromComposition(int brickid) {
	cSummaryFormula formula;
	vector<int> items;
	bricks[brickid].explodeToIntComposition(items);

	for (int i = 0; i < (int)items.size(); i++) {
		formula.addFormula(bricks[items[i] - 1].getSummary());
	}

	return formula.getSummary();
}


void cBricksDatabase::store(ofstream& os) {
	int size;

	size = (int)bricks.size();
	os.write((char *)&size, sizeof(int));

	for (int i = 0; i < (int)bricks.size(); i++) {
		bricks[i].store(os);
	}

	storeStringIntMap(lossorders, os);
	storeDoubleVector(lossmasses, os);
	storeStringVector(losssummaries, os);
	storeStringIntMapVector(lossmaps, os);
}


void cBricksDatabase::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	bricks.resize(size);

	for (int i = 0; i < (int)bricks.size(); i++) {
		bricks[i].load(is);
	}

	loadStringIntMap(lossorders, is);
	loadDoubleVector(lossmasses, is);
	loadStringVector(losssummaries, is);
	loadStringIntMapVector(lossmaps, is);
}


bool cBricksDatabase::checkKetideBlocks(cBrick& brickseries, bool regularblocksorder) {
	vector<int> intcomposition;
	brickseries.explodeToIntComposition(intcomposition);

	if (intcomposition.size() == 0) {
		return false;
	}

	if (!regularblocksorder) {
		return true;
	}

	int h2_blocks = 0;
	int h2o_blocks = 0;
	for (int i = 0; i < (int)intcomposition.size(); i++) {
		if (bricks[intcomposition[i] - 1].getResidueLossType() == h2_loss) {
			h2_blocks++;
		}
		if (bricks[intcomposition[i] - 1].getResidueLossType() == h2o_loss) {
			h2o_blocks++;
		}
	}

	if ((h2_blocks == h2o_blocks) || (h2_blocks == h2o_blocks + 1) || (h2_blocks + 1 == h2o_blocks)) {
		return true;
	}

	/*
	if ((h2_blocks == 0) && (h2o_blocks == (int)intcomposition.size())) {
		return true;
	}
	*/

	return false;
}


double cBricksDatabase::getMassOfNeutralLosses(vector<int>& lossids) {
	int size = (int)lossmasses.size();
	double mass = 0;
	for (auto& it : lossids) {
		if (it < size) {
			mass += lossmasses[it];
		}
	}
	return mass;
}


string cBricksDatabase::getSummaryFormulaOfNeutralLosses(vector<int>& lossids) {
	int size = (int)losssummaries.size();
	string summary = "";
	for (auto& it : lossids) {
		if (it < size) {
			summary += losssummaries[it];
		}
	}
	return summary;
}


void cBricksDatabase::getMapOfNeutralLosses(vector<int>& lossids, map<string, int>& summarymap) {
	int size = (int)lossmaps.size();
	summarymap.clear();
	for (auto& it : lossids) {
		if (it < size) {
			mergeMaps(lossmaps[it], summarymap);
		}
	}
}

