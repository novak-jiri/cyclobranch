#include "core/cFragmentIons.h"


cPeriodicTableMap::cPeriodicTableMap() {
	table["H"] = H;
	table["Li"] = Li;
	table["Be"] = Be;
	table["B"] = B;
	table["C"] = C;
	table["N"] = N;
	table["O"] = O;
	table["F"] = F;
	table["Na"] = Na;
	table["Mg"] = Mg;
	table["Al"] = Al;
	table["Si"] = Si;
	table["P"] = P;
	table["S"] = S;
	table["Cl"] = Cl;
	table["K"] = K;
	table["Ca"] = Ca;
	table["Sc"] = Sc;
	table["Ti"] = Ti;
	table["V"] = V;
	table["Cr"] = Cr;
	table["Mn"] = Mn;
	table["Fe"] = Fe;
	table["Co"] = Co;
	table["Ni"] = Ni;
	table["Cu"] = Cu;
	table["Zn"] = Zn;
	table["Se"] = Se;
	table["Br"] = Br;
	table["Mo"] = Mo;
	table["I"] = I;
}


double &cPeriodicTableMap::operator[](string& element) {
	return table[element];
}


int cPeriodicTableMap::count(string& element) {
	return (int)table.count(element);
}


double getMassFromResidueSummary(string& summary, bool& error, string& errormessage) {
	if ((int)summary.size() == 0) {
		return 0;
	}

	regex rx;
	rx = "^([A-Z][a-z]*((-[0-9]+)|([0-9]*))*)+$";

	try {
		if (!regex_search(summary, rx)) {
			error = true;
			errormessage = "Invalid summary: " + summary + ".\n\n";
			return 0;
		}
	}
	catch (std::regex_error& e) {
		error = true;
		errormessage = "getMassFromResidueSummary: regex_search failed, error no. " + to_string((int)e.code()) + "\n";
		return 0;
	}

	// calculate mass from summary
	error = false;
	errormessage = "";

	cPeriodicTableMap map;
	double sum = 0;

	string tmps;
	tmps = summary[0];

	int tmpc = 0;

	bool minus = false;

	for (int i = 1; i < (int)summary.size(); i++) {

		if ((summary[i] >= 'A') && (summary[i] <= 'Z')) {
			if (map.count(tmps) > 0) {
				if (tmpc == 0) {
					sum += map[tmps];
				}
				else {
					sum += minus?-tmpc*map[tmps]:tmpc*map[tmps];
				}
				//cout << tmps << " " << (minus?"-":"") << tmpc << endl;
			}
			else {
				error = true;
				errormessage = "Undefined atom name: " + tmps + ".\n\n";
				return 0;
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

	// last element
	if (map.count(tmps) > 0) {
		if (tmpc == 0) {
			sum += map[tmps];
		}
		else {
			sum += minus?-tmpc*map[tmps]:tmpc*map[tmps];
		}
		//cout << tmps << " " << (minus?"-":"") << tmpc << endl;
	}
	else {
		error = true;
		errormessage = "Undefined atom name: " + tmps + ".\n\n";
		return 0;
	}

	return sum;
}


double charge(double mass, int charge) {
	return (mass + ((double)(charge - 1))*Hplus)/(double)charge;
}


double uncharge(double mass, int charge) {
	return mass*(double)charge - ((double)(charge - 1))*Hplus;
}


void fragmentDescription::store(ofstream& os) {
	int size;
	
	size = (int)name.size();
	os.write((char *)&size, sizeof(int));
	os.write(name.c_str(), (int)name.size());

	size = (int)summary.size();
	os.write((char *)&size, sizeof(int));
	os.write(summary.c_str(), (int)summary.size());

	os.write((char *)&massdifference, sizeof(double));
	os.write((char *)&nterminal, sizeof(bool));
	os.write((char *)&cterminal, sizeof(bool));
	os.write((char *)&parent, sizeof(fragmentIonType));
}


void fragmentDescription::load(ifstream& is) {
	int size;
	
	is.read((char *)&size, sizeof(int));
	name.resize(size);
	is.read(&name[0], name.size());

	is.read((char *)&size, sizeof(int));
	summary.resize(size);
	is.read(&summary[0], summary.size());

	is.read((char *)&massdifference, sizeof(double));
	is.read((char *)&nterminal, sizeof(bool));
	is.read((char *)&cterminal, sizeof(bool));
	is.read((char *)&parent, sizeof(fragmentIonType));
}


cFragmentIons::cFragmentIons() {
	recalculateFragments(false, false);
}


cFragmentIons::cFragmentIons(bool cyclicnterminus, bool cycliccterminus) {
	recalculateFragments(cyclicnterminus, cycliccterminus);
}


fragmentDescription &cFragmentIons::operator[](fragmentIonType iontype) {
	return fragmentions[iontype];
}


void cFragmentIons::recalculateFragments(bool cyclicnterminus, bool cycliccterminus) {

	fragmentions.clear();

	double nterminusshift = cyclicnterminus?-H2O:0;
	double cterminusshift = cycliccterminus?-H2O:0;
	
	// initialize B-ion
	fragmentions[b_ion].nterminal = true;
	fragmentions[b_ion].cterminal = false;
	fragmentions[b_ion].name = "B";
	fragmentions[b_ion].massdifference = B_ION + nterminusshift;
	fragmentions[b_ion].parent = b_ion;

	// initialize B-H2O ion
	fragmentions[b_ion_water_loss].nterminal = true;
	fragmentions[b_ion_water_loss].cterminal = false;
	fragmentions[b_ion_water_loss].name = "B*";
	fragmentions[b_ion_water_loss].massdifference = B_ION - H2O + nterminusshift;
	fragmentions[b_ion_water_loss].parent = b_ion;

	// initialize B-NH3 ion
	fragmentions[b_ion_ammonia_loss].nterminal = true;
	fragmentions[b_ion_ammonia_loss].cterminal = false;
	fragmentions[b_ion_ammonia_loss].name = "Bx";
	fragmentions[b_ion_ammonia_loss].massdifference = B_ION - NH3 + nterminusshift;
	fragmentions[b_ion_ammonia_loss].parent = b_ion;

	// initialize B-H2O-NH3 ion
	fragmentions[b_ion_water_and_ammonia_loss].nterminal = true;
	fragmentions[b_ion_water_and_ammonia_loss].cterminal = false;
	fragmentions[b_ion_water_and_ammonia_loss].name = "B*x";
	fragmentions[b_ion_water_and_ammonia_loss].massdifference = B_ION - H2O - NH3 + nterminusshift;
	fragmentions[b_ion_water_and_ammonia_loss].parent = b_ion;
	
	// initialize A-ion
	fragmentions[a_ion].nterminal = true;
	fragmentions[a_ion].cterminal = false;
	fragmentions[a_ion].name = "A";
	fragmentions[a_ion].massdifference = A_ION + nterminusshift;
	fragmentions[a_ion].parent = b_ion; // ok
	
	// initialize A-H2O ion
	fragmentions[a_ion_water_loss].nterminal = true;
	fragmentions[a_ion_water_loss].cterminal = false;
	fragmentions[a_ion_water_loss].name = "A*";
	fragmentions[a_ion_water_loss].massdifference = A_ION - H2O + nterminusshift;
	fragmentions[a_ion_water_loss].parent = a_ion;

	// initialize A-NH3 ion
	fragmentions[a_ion_ammonia_loss].nterminal = true;
	fragmentions[a_ion_ammonia_loss].cterminal = false;
	fragmentions[a_ion_ammonia_loss].name = "Ax";
	fragmentions[a_ion_ammonia_loss].massdifference = A_ION - NH3 + nterminusshift;
	fragmentions[a_ion_ammonia_loss].parent = a_ion;

	// initialize A-H2O-NH3 ion
	fragmentions[a_ion_water_and_ammonia_loss].nterminal = true;
	fragmentions[a_ion_water_and_ammonia_loss].cterminal = false;
	fragmentions[a_ion_water_and_ammonia_loss].name = "A*x";
	fragmentions[a_ion_water_and_ammonia_loss].massdifference = A_ION - H2O - NH3 + nterminusshift;
	fragmentions[a_ion_water_and_ammonia_loss].parent = a_ion;

	// initialize Y ion
	fragmentions[y_ion].nterminal = false;
	fragmentions[y_ion].cterminal = true;
	fragmentions[y_ion].name = "Y";
	fragmentions[y_ion].massdifference = Y_ION + cterminusshift;
	fragmentions[y_ion].parent = y_ion;

	// initialize Y-H2O ion
	fragmentions[y_ion_water_loss].nterminal = false;
	fragmentions[y_ion_water_loss].cterminal = true;
	fragmentions[y_ion_water_loss].name = "Y*";
	fragmentions[y_ion_water_loss].massdifference = Y_ION - H2O + cterminusshift;
	fragmentions[y_ion_water_loss].parent = y_ion;

	// initialize Y-NH3 ion
	fragmentions[y_ion_ammonia_loss].nterminal = false;
	fragmentions[y_ion_ammonia_loss].cterminal = true;
	fragmentions[y_ion_ammonia_loss].name = "Yx";
	fragmentions[y_ion_ammonia_loss].massdifference = Y_ION - NH3 + cterminusshift;
	fragmentions[y_ion_ammonia_loss].parent = y_ion;

	// initialize Y-H2O-NH3 ion
	fragmentions[y_ion_water_and_ammonia_loss].nterminal = false;
	fragmentions[y_ion_water_and_ammonia_loss].cterminal = true;
	fragmentions[y_ion_water_and_ammonia_loss].name = "Y*x";
	fragmentions[y_ion_water_and_ammonia_loss].massdifference = Y_ION - H2O - NH3 + cterminusshift;
	fragmentions[y_ion_water_and_ammonia_loss].parent = y_ion;

	// initialize precursor ion
	fragmentions[precursor_ion].nterminal = false;
	fragmentions[precursor_ion].cterminal = false;
	fragmentions[precursor_ion].name = "M";
	fragmentions[precursor_ion].massdifference = PRECURSOR_ION + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion].parent = precursor_ion;

	// initialize precursor ion - H2O
	fragmentions[precursor_ion_water_loss].nterminal = false;
	fragmentions[precursor_ion_water_loss].cterminal = false;
	fragmentions[precursor_ion_water_loss].name = "M*";
	fragmentions[precursor_ion_water_loss].massdifference = PRECURSOR_ION - H2O + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_water_loss].parent = precursor_ion;

	// initialize precursor ion - NH3
	fragmentions[precursor_ion_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_ammonia_loss].name = "Mx";
	fragmentions[precursor_ion_ammonia_loss].massdifference = PRECURSOR_ION - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_ammonia_loss].parent = precursor_ion;

	// initialize precursor ion - H2O - NH3
	fragmentions[precursor_ion_water_and_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_water_and_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_water_and_ammonia_loss].name = "M*x";
	fragmentions[precursor_ion_water_and_ammonia_loss].massdifference = PRECURSOR_ION - H2O - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_water_and_ammonia_loss].parent = precursor_ion;

	// initialize precursor ion - CO
	fragmentions[precursor_ion_co_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss].name = "M-CO";
	fragmentions[precursor_ion_co_loss].massdifference = PRECURSOR_ION - CO + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss].parent = precursor_ion;

	// initialize precursor ion - CO - H2O
	fragmentions[precursor_ion_co_loss_water_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss_water_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss_water_loss].name = "M*-CO";
	fragmentions[precursor_ion_co_loss_water_loss].massdifference = PRECURSOR_ION - CO - H2O + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss_water_loss].parent = precursor_ion_co_loss;

	// initialize precursor ion - CO - NH3
	fragmentions[precursor_ion_co_loss_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss_ammonia_loss].name = "Mx-CO";
	fragmentions[precursor_ion_co_loss_ammonia_loss].massdifference = PRECURSOR_ION - CO - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss_ammonia_loss].parent = precursor_ion_co_loss;

	// initialize precursor ion - CO - H2O - NH3
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].name = "M*x-CO";
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].massdifference = PRECURSOR_ION - CO - H2O - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].parent = precursor_ion_co_loss;

	// initialize cyclic precursor ion
	fragmentions[cyclic_precursor_ion].nterminal = false;
	fragmentions[cyclic_precursor_ion].cterminal = false;
	fragmentions[cyclic_precursor_ion].name = "M";
	fragmentions[cyclic_precursor_ion].massdifference = PRECURSOR_ION_CYCLIC;
	fragmentions[cyclic_precursor_ion].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - H2O
	fragmentions[cyclic_precursor_ion_water_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_water_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_water_loss].name = "M*";
	fragmentions[cyclic_precursor_ion_water_loss].massdifference = PRECURSOR_ION_CYCLIC - H2O;
	fragmentions[cyclic_precursor_ion_water_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - NH3
	fragmentions[cyclic_precursor_ion_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_ammonia_loss].name = "Mx";
	fragmentions[cyclic_precursor_ion_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - NH3;
	fragmentions[cyclic_precursor_ion_ammonia_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - H2O - NH3
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].name = "M*x";
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - H2O - NH3;
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - CO
	fragmentions[cyclic_precursor_ion_co_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss].name = "M-CO";
	fragmentions[cyclic_precursor_ion_co_loss].massdifference = PRECURSOR_ION_CYCLIC - CO;
	fragmentions[cyclic_precursor_ion_co_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - CO - H2O
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].name = "M*-CO";
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].massdifference = PRECURSOR_ION_CYCLIC - CO - H2O;
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].parent = cyclic_precursor_ion_co_loss;

	// initialize cyclic precursor ion - CO - NH3
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].name = "Mx-CO";
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - CO - NH3;
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].parent = cyclic_precursor_ion_co_loss;

	// initialize cyclic precursor ion - CO - H2O - NH3
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].name = "M*x-CO";
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - CO - H2O - NH3;
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].parent = cyclic_precursor_ion_co_loss;

	// initialize MS N-terminal ion H+
	fragmentions[ms_nterminal_ion_hplus].nterminal = true;
	fragmentions[ms_nterminal_ion_hplus].cterminal = false;
	fragmentions[ms_nterminal_ion_hplus].name = "N_H+";
	fragmentions[ms_nterminal_ion_hplus].massdifference = H2O + Hplus;
	fragmentions[ms_nterminal_ion_hplus].parent = ms_nterminal_ion_hplus;

	// initialize MS N-terminal ion Na+
	fragmentions[ms_nterminal_ion_naplus].nterminal = true;
	fragmentions[ms_nterminal_ion_naplus].cterminal = false;
	fragmentions[ms_nterminal_ion_naplus].name = "N_Na+";
	fragmentions[ms_nterminal_ion_naplus].massdifference = H2O + Naplus;
	fragmentions[ms_nterminal_ion_naplus].parent = ms_nterminal_ion_naplus;

	// initialize MS N-terminal ion K+
	fragmentions[ms_nterminal_ion_kplus].nterminal = true;
	fragmentions[ms_nterminal_ion_kplus].cterminal = false;
	fragmentions[ms_nterminal_ion_kplus].name = "N_K+";
	fragmentions[ms_nterminal_ion_kplus].massdifference = H2O + Kplus;
	fragmentions[ms_nterminal_ion_kplus].parent = ms_nterminal_ion_kplus;

	// initialize MS C-terminal ion H+
	fragmentions[ms_cterminal_ion_hplus].nterminal = false;
	fragmentions[ms_cterminal_ion_hplus].cterminal = true;
	fragmentions[ms_cterminal_ion_hplus].name = "C_H+";
	fragmentions[ms_cterminal_ion_hplus].massdifference = H2O + Hplus;
	fragmentions[ms_cterminal_ion_hplus].parent = ms_cterminal_ion_hplus;

	// initialize MS C-terminal ion Na+
	fragmentions[ms_cterminal_ion_naplus].nterminal = false;
	fragmentions[ms_cterminal_ion_naplus].cterminal = true;
	fragmentions[ms_cterminal_ion_naplus].name = "C_Na+";
	fragmentions[ms_cterminal_ion_naplus].massdifference = H2O + Naplus;
	fragmentions[ms_cterminal_ion_naplus].parent = ms_cterminal_ion_naplus;

	// initialize MS C-terminal ion K+
	fragmentions[ms_cterminal_ion_kplus].nterminal = false;
	fragmentions[ms_cterminal_ion_kplus].cterminal = true;
	fragmentions[ms_cterminal_ion_kplus].name = "C_K+";
	fragmentions[ms_cterminal_ion_kplus].massdifference = H2O + Kplus;
	fragmentions[ms_cterminal_ion_kplus].parent = ms_cterminal_ion_kplus;

	// initialize B-2H ion
	//fragmentions[b_ion_2H_loss].nterminal = true;
	//fragmentions[b_ion_2H_loss].cterminal = false;
	//fragmentions[b_ion_2H_loss].name = "-2HB";
	//fragmentions[b_ion_2H_loss].massdifference = B_ION - 2 * H;

}


void cFragmentIons::store(ofstream& os) {
	int size;

	size = (int)fragmentions.size();
	os.write((char *)&size, sizeof(int));
	for (map<fragmentIonType,fragmentDescription>::iterator it = fragmentions.begin(); it != fragmentions.end(); ++it) {
		os.write((char *)&it->first, sizeof(fragmentIonType));
		os.write((char *)&it->second, sizeof(fragmentDescription));
	}
}


void cFragmentIons::load(ifstream& is) {
	int size;
	fragmentIonType iontype;
	fragmentDescription description;

	is.read((char *)&size, sizeof(int));
	fragmentions.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&iontype, sizeof(fragmentIonType));
		is.read((char *)&description, sizeof(fragmentDescription));
		fragmentions[iontype] = description;
	}
}


void initializeFragmentIonsForDeNovoGraphOfCyclicPeptides(vector<fragmentIonType>& fragmentions) {
	fragmentions.push_back(b_ion);
}


void initializeFragmentIonsForDeNovoGraphOfLinearPeptides(vector<fragmentIonType>& fragmentions) {
	fragmentions.push_back(y_ion);
	fragmentions.push_back(b_ion);
	fragmentions.push_back(precursor_ion);
}


void initializeFragmentIonsForDeNovoGraphOfTPeptides(vector<fragmentIonType>& fragmentions) {
	fragmentions.push_back(y_ion);
	fragmentions.push_back(b_ion);
	fragmentions.push_back(precursor_ion);
}


void initializeFragmentIonsForDeNovoGraphOfLassoPeptides(vector<fragmentIonType>& fragmentions) {
	fragmentions.push_back(b_ion);
	//fragmentions.push_back(y_ion);
}


void initializeFragmentIonsForDeNovoGraphOfLinearPolysaccharide(vector<fragmentIonType>& fragmentions) {
	fragmentions.push_back(ms_nterminal_ion_hplus);
	//fragmentions.push_back(ms_cterminal_ion_hplus);
	fragmentions.push_back(precursor_ion);
}

