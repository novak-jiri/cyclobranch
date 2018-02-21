#include "core/cSummaryFormula.h"


void cSummaryFormula::explodeSummary(map<string, int>& atoms, string& summary) {
	if ((int)summary.size() == 0) {
		return;
	}

	cPeriodicTableMap map;
	string tmps;
	tmps = summary[0];

	int tmpc = 0;
	bool minus = false;
	for (int i = 1; i < (int)summary.size(); i++) {

		if ((summary[i] >= 'A') && (summary[i] <= 'Z')) {
			if (map.count(tmps) > 0) {
				if (tmpc == 0) {
					if (atoms.count(tmps) == 0) {
						atoms[tmps] = 1;
					}
					else {
						atoms[tmps] += 1;
					}
				}
				else {
					if (atoms.count(tmps) == 0) {
						atoms[tmps] = minus?-tmpc:tmpc;
					}
					else {
						atoms[tmps] += minus?-tmpc:tmpc;
					}
				}
			}
			else {
				return;
			}

			tmps = summary[i];
			tmpc = 0;
			minus = false;
		}

		if ((summary[i] >= 'a') && (summary[i] <= 'z')) {
			tmps += summary[i];
		}

		if ((summary[i] >= '0') && (summary[i] <= '9')) {
			tmpc *= 10;
			tmpc += summary[i] - '0';
		}

		if (summary[i] == '-') {
			minus = true;
		}

	}

	// the last element
	if (map.count(tmps) > 0) {
		if (tmpc == 0) {
			if (atoms.count(tmps) == 0) {
				atoms[tmps] = 1;
			}
			else {
				atoms[tmps] += 1;
			}
		}
		else {
			if (atoms.count(tmps) == 0) {
				atoms[tmps] = minus?-tmpc:tmpc;
			}
			else {
				atoms[tmps] += minus?-tmpc:tmpc;
			}
		}
	}
}


cSummaryFormula::cSummaryFormula() {
	clear();
}


cSummaryFormula::cSummaryFormula(string& formula) {
	this->formula = formula;
}


void cSummaryFormula::clear() {
	formula = "";
}


void cSummaryFormula::setFormula(string& formula) {
	this->formula = formula;
}


string& cSummaryFormula::getFormula() {
	return formula;
}


void cSummaryFormula::addFormula(string& formula) {
	if ((int)formula.size() == 0) {
		return;
	}

	map<string, int> atoms;
	atoms.clear();
	explodeSummary(atoms, this->formula);
	explodeSummary(atoms, formula);

	this->formula = "";
	for (auto it = atoms.begin(); it != atoms.end(); ++it) {
		if (it->second != 0) {
			this->formula += it->first;
			if (it->second != 1) {
				this->formula += to_string(it->second);
			}
		}
	}
}


bool cSummaryFormula::isValid(string& errormessage) {
	if ((int)formula.size() == 0) {
		return true;
	}

	regex rx;
	rx = "^([A-Z][a-z]*((-[0-9]+)|([0-9]*))*)+$";

	try {
		if (!regex_search(formula, rx)) {
			errormessage = "Invalid summary: " + formula + ".\n\n";
			return false;
		}
	}
	catch (std::regex_error& e) {
		errormessage = "getMassFromResidueSummary: regex_search failed, error no. " + to_string((int)e.code()) + "\n";
		return false;
	}

	errormessage = "";

	cPeriodicTableMap map;
	string tmps;
	tmps = formula[0];

	for (int i = 1; i < (int)formula.size(); i++) {

		if ((formula[i] >= 'A') && (formula[i] <= 'Z')) {
			if (map.count(tmps) == 0) {
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
	if (map.count(tmps) == 0) {
		errormessage = "Undefined atom name: " + tmps + ".\n\n";
		return false;
	}

	return true;
}


double cSummaryFormula::getMass() {
	if ((int)formula.size() == 0) {
		return 0;
	}

	cPeriodicTableMap map;
	double sum = 0;

	string tmps;
	tmps = formula[0];

	int tmpc = 0;

	bool minus = false;

	for (int i = 1; i < (int)formula.size(); i++) {

		if ((formula[i] >= 'A') && (formula[i] <= 'Z')) {
			if (map.count(tmps) > 0) {
				if (tmpc == 0) {
					sum += map[tmps];
				}
				else {
					sum += minus?-tmpc*map[tmps]:tmpc*map[tmps];
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
	if (map.count(tmps) > 0) {
		if (tmpc == 0) {
			sum += map[tmps];
		}
		else {
			sum += minus?-tmpc*map[tmps]:tmpc*map[tmps];
		}
	}
	else {
		return 0;
	}

	return sum;
}


void cSummaryFormula::store(ofstream& os) {
	storeString(formula, os);
}


void cSummaryFormula::load(ifstream& is) {
	loadString(formula, is);
}

