#include "parallel/cFormulaPredictorThread.h"


int cFormulaPredictorThread::generateCompoundsFormulaPredictor(vector<string>& formulas) {
	double minimummz = mzvalue - mzvalue * mzerror * 0.000001;
	double maximummz = mzvalue + mzvalue * mzerror * 0.000001;

	unsigned long long compoundsgenerated = 0;
	unsigned long long compoundsused = 0;
	unsigned long long compoundslimit = 5000000;

	cSummaryFormula tmpformula;
	int size;

	cBricksDatabase elementsbrickdatabase;
	cBrick tmpbrick;
	string tmpstr, tmpstr2;
	vector<int> valences;
	bool validvalences = false;

	vector<int> limitsofelements;
	vector<int> countsofelements;
	vector<double> massesofelements;
	vector<string> namesofelements;

	int numberofbasicbricks = 0;

	double sumofmasses;
	double tmpmzdifference;
	bool alloutofmz;

	double minadd = 0;
	//double maxadd = 0;

	for (int i = 0; i < (int)selectedelements->size(); i++) {
		tmpstr = (*selectedelements)[i];

		if (tmpstr.rfind(':') != string::npos) {
			tmpstr = tmpstr.substr(0, tmpstr.rfind(':'));
		}

		tmpformula.setFormula(tmpstr, false);

		tmpbrick.clear();
		tmpbrick.setName((*selectedelements)[i]);
		tmpbrick.setComposition(to_string(numberofbasicbricks + 1), false);
		tmpbrick.setMass(tmpformula.getMass());
		tmpbrick.setSummary(tmpstr);
		tmpbrick.createSummaryMap();

		if ((tmpbrick.getSummaryMap().size() != 1)) {
			*errmsg = "Bad input element: " + tmpstr + ". Only single elements can be used (e.g. H, C, O, N, P, S).";
			return -1;
		}
		else {
			auto it = tmpbrick.getSummaryMap().begin();
			if (it->second != 1) {
				*errmsg = "Bad input element: " + tmpstr + ". Only single elements can be used (e.g. H, C, O, N, P, S).";
				return -1;
			}
		}

		elementsbrickdatabase.push_back(tmpbrick);

		numberofbasicbricks++;
	}

	elementsbrickdatabase.sortbyMass();

	int carbonpos = -1;
	//bool enablelimitfilter = false;
	for (int i = 0; i < elementsbrickdatabase.size(); i++) {
		countsofelements.push_back(0);
		massesofelements.push_back(elementsbrickdatabase[i].getMass());

		tmpstr = elementsbrickdatabase[i].getName();
		if (tmpstr.rfind(':') != string::npos) {
			tmpstr = tmpstr.substr(tmpstr.rfind(':') + 1);
			limitsofelements.push_back(QVariant(tmpstr.c_str()).toInt());
			//enablelimitfilter = true;
		}
		else {
			limitsofelements.push_back(0);
		}

		tmpstr = elementsbrickdatabase[i].getSummary();
		namesofelements.push_back(tmpstr);

		if (tmpstr.compare("C") == 0) {
			carbonpos = i;
		}

		if (tmpstr.compare("H") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("C") == 0) {
			valences.push_back(4);
		}

		if (tmpstr.compare("O") == 0) {
			valences.push_back(2);
		}

		if (tmpstr.compare("N") == 0) {
			valences.push_back(3);
		}

		if (tmpstr.compare("S") == 0) {
			valences.push_back(6);
		}

		if (tmpstr.compare("P") == 0) {
			valences.push_back(5);
		}

		if (tmpstr.compare("Li") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Na") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("K") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("F") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Cl") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Br") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("I") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Si") == 0) {
			valences.push_back(4);
		}
	}

	if (basicformulacheck && (elementsbrickdatabase.size() > 0) && (elementsbrickdatabase.size() == valences.size())) {
		validvalences = true;
	}

	vector<int> combarray;
	for (int i = 0; i < maximumcombinedelements; i++) {
		combarray.push_back(0);
	}

	if (selectedions->size() > 0) {
		minadd = (*selectedions)[0].massdifference;
		//maxadd = (*selectedions)[0].massdifference;
		for (int i = 1; i < (int)selectedions->size(); i++) {
			if ((*selectedions)[i].massdifference < minadd) {
				minadd = (*selectedions)[i].massdifference;
			}
			//if ((*selectedions)[i].massdifference > maxadd) {
			//	maxadd = (*selectedions)[i].massdifference;
			//}
		}
	}

	double unchargedmaximummz = charge(uncharge(maximummz, chargevalue), (chargevalue > 0) ? 1 : -1) - minadd;
	//double unchargedmaximummz = maximummz - minadd;

	//bool skipcombination;
	//while (elementsbrickdatabase.nextCombinationFast(combarray, countsofelements, massesofelements, sumofmasses, numberofbasicbricks, maximumcombinedelements, 0, maximummz)) {
	while (elementsbrickdatabase.nextCombinationFastLimited(combarray, countsofelements, limitsofelements, massesofelements, sumofmasses, numberofbasicbricks, maximumcombinedelements, 0, unchargedmaximummz)) {
		if (*terminatecomputation) {
			formulas.clear();
			return 0;
		}

		/*if (enablelimitfilter) {
			skipcombination = false;
			size = (int)countsofelements.size();
			for (int j = 0; j < size; j++) {
				if ((limitsofelements[j] > 0) && (countsofelements[j] > limitsofelements[j])) {
					skipcombination = true;
					break;
				}
			}

			if (skipcombination) {
				continue;
			}
		}*/

		if (validvalences) {
			if (!checkSeniorRules(combarray, valences, 1)) {
				continue;
			}
		}

		//sumofmasses = getMassAndCounts(combarray, countsofelements, massesofelements);

		alloutofmz = true;
		for (auto& it : *selectedions) {
			for (int j = 0; j < abs(chargevalue); j++) {
				tmpmzdifference = sumofmasses + it.massdifference;
				if (chargevalue > 0) {
					tmpmzdifference += j * (H - e);
				}
				else {
					tmpmzdifference -= j * (H - e);
				}
				if (j > 0) {
					tmpmzdifference /= (double)(j + 1);
				}
				if ((tmpmzdifference >= minimummz) && (tmpmzdifference <= maximummz)) {
					alloutofmz = false;
				}
			}
		}

		if (alloutofmz) {
			continue;
		}

		if (advancedformulacheck) {
			if (checkAdvancedFilteringRules(noratiocheck, sumofmasses, countsofelements, namesofelements)) {
				continue;
			}
		}

		compoundsgenerated++;

		tmpstr.clear();
		tmpstr2.clear();
		size = (int)countsofelements.size();
		for (int j = 0; j < size; j++) {
			if (countsofelements[j] > 0) {
				if (j == carbonpos) {
					tmpstr2 = namesofelements[j] + to_string(countsofelements[j]);
				}
				else {
					tmpstr += namesofelements[j] + to_string(countsofelements[j]);
				}
			}
		}
		tmpstr = tmpstr2 + tmpstr;

		if (compoundsused <= compoundslimit) {
			formulas.push_back(tmpstr);
		}
		compoundsused++;
	}

	//cout << "Number of generated compounds: " << compoundsgenerated << endl;
	//cout << "Number of used compounds: " << compoundsused << endl << endl;

	return 0;
}


cFormulaPredictorThread::cFormulaPredictorThread() {
	chargevalue = 0;
	maximumcombinedelements = 0;
	basicformulacheck = false;
	advancedformulacheck = false;
	noratiocheck = false;
	mzvalue = 0;
	mzerror = 0;
	selectedions = 0;
	selectedelements = 0;
	ionitems = 0;
	success = 0;
	terminatecomputation = 0;
	errmsg = 0;

	// delete thread when run is finished
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}


void cFormulaPredictorThread::initialize(int chargevalue, int maximumcombinedelements, bool basicformulacheck, bool advancedformulacheck, bool noratiocheck, double mzvalue, double mzerror, vector<cIonType>& selectedions, vector<string>& selectedelements, vector<cFormulaPredictorRowItem>& ionitems, bool& success, bool& terminatecomputation, string& errmsg) {
	this->chargevalue = chargevalue;
	this->maximumcombinedelements = maximumcombinedelements;
	this->basicformulacheck = basicformulacheck;
	this->advancedformulacheck = advancedformulacheck;
	this->noratiocheck = noratiocheck;
	this->mzvalue = mzvalue;
	this->mzerror = mzerror;
	this->selectedions = &selectedions;
	this->selectedelements = &selectedelements;
	this->ionitems = &ionitems;
	this->success = &success;
	this->terminatecomputation = &terminatecomputation;
	this->errmsg = &errmsg;
}


void cFormulaPredictorThread::run() {
	vector<string> formulas;
	cFormulaPredictorRowItem ionitem;

	cSummaryFormula formula;
	string proton = "H+";

	if (generateCompoundsFormulaPredictor(formulas) == 0) {
		*success = true;

		for (auto& it : formulas) {
			for (auto& it2 : *selectedions) {
				for (int i = 0; i < abs(chargevalue); i++) {
					formula.setFormula(it);
					formula.addFormula(it2.formula);

					for (int j = 0; j < i; j++) {
						if (chargevalue > 0) {
							formula.addFormula(proton);
						}
						else {
							formula.addFormula(proton, true);
						}
					}

					ionitem.theoreticalmz = formula.getMass() / (double)(i + 1);
					ionitem.mzerror = ppmError(mzvalue, ionitem.theoreticalmz);
					ionitem.iontype = fixLabelCharge(it2.name, (chargevalue > 0) ? 1 : -1, i + 1) + " ";
					ionitem.iontype += to_string(i + 1);
					if (it2.positive) {
						ionitem.iontype += "+";
					}
					else {
						ionitem.iontype += "-";
					}

					ionitem.charge = i + 1;
					if (chargevalue < 0) {
						ionitem.charge = -ionitem.charge;
					}

					ionitem.ionformula = it;

					if (isInPpmMassErrorTolerance(mzvalue, ionitem.theoreticalmz, mzerror)) {
						ionitems->push_back(ionitem);
					}
				}
			}
		}
	}
}

