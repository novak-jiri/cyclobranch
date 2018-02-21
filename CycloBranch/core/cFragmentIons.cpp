#include "core/cFragmentIons.h"


cPeriodicTableMap::cPeriodicTableMap() {
	table["H"] = H;
	table["D"] = D;
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


peptideType getPeptideTypeFromString(string s) {
	if (s.compare("linear") == 0) {
		return linear;
	}
	if (s.compare("cyclic") == 0) {
		return cyclic;
	}
	if (s.compare("branched") == 0) {
		return branched;
	}
	if (s.compare("lasso") == 0) {
		return lasso;
	}
	if (s.compare("linearpolysaccharide") == 0) {
		return linearpolysaccharide;
	}
	if (s.compare("other") == 0) {
		return other;
	}

	return other;
}


string getStringFromPeptideType(peptideType peptidetype) {
	switch (peptidetype)
	{
	case linear:
		return "linear";
		break;
	case cyclic:
		return "cyclic";
		break;
	case branched:
		return "branched";
		break;
	case lasso:
		return "lasso";
		break;
	case linearpolysaccharide:
		return "linearpolysaccharide";
		break;
	case other:
		return "other";
		break;
	default:
		break;
	}

	return "other";
}


double charge(double mass, int charge) {
	return (mass + ((double)(charge - 1))*Hplus)/(double)charge;
}


double uncharge(double mass, int charge) {
	return mass*(double)charge - ((double)(charge - 1))*Hplus;
}


void fragmentDescription::store(ofstream& os) {
	storeString(name, os);
	storeString(summary, os);
	os.write((char *)&massdifference, sizeof(double));
	os.write((char *)&nterminal, sizeof(bool));
	os.write((char *)&cterminal, sizeof(bool));
	os.write((char *)&parent, sizeof(fragmentIonType));
	os.write((char *)&positive, sizeof(bool));
	os.write((char *)&multiplier, sizeof(int));
}


void fragmentDescription::load(ifstream& is) {
	loadString(name, is);
	loadString(summary, is);
	is.read((char *)&massdifference, sizeof(double));
	is.read((char *)&nterminal, sizeof(bool));
	is.read((char *)&cterminal, sizeof(bool));
	is.read((char *)&parent, sizeof(fragmentIonType));
	is.read((char *)&positive, sizeof(bool));
	is.read((char *)&multiplier, sizeof(int));
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
	fragmentions[precursor_ion].name = "[M+zH]+";
	fragmentions[precursor_ion].massdifference = PRECURSOR_ION + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion].parent = precursor_ion;

	// initialize precursor ion - H2O
	fragmentions[precursor_ion_water_loss].nterminal = false;
	fragmentions[precursor_ion_water_loss].cterminal = false;
	fragmentions[precursor_ion_water_loss].name = "[M+zH]+ *";
	fragmentions[precursor_ion_water_loss].massdifference = PRECURSOR_ION - H2O + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_water_loss].parent = precursor_ion;

	// initialize precursor ion - NH3
	fragmentions[precursor_ion_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_ammonia_loss].name = "[M+zH]+ x";
	fragmentions[precursor_ion_ammonia_loss].massdifference = PRECURSOR_ION - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_ammonia_loss].parent = precursor_ion;

	// initialize precursor ion - H2O - NH3
	fragmentions[precursor_ion_water_and_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_water_and_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_water_and_ammonia_loss].name = "[M+zH]+ *x";
	fragmentions[precursor_ion_water_and_ammonia_loss].massdifference = PRECURSOR_ION - H2O - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_water_and_ammonia_loss].parent = precursor_ion;

	// initialize precursor ion - CO
	fragmentions[precursor_ion_co_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss].name = "[M+zH]+ -CO";
	fragmentions[precursor_ion_co_loss].massdifference = PRECURSOR_ION - CO + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss].parent = precursor_ion;

	// initialize precursor ion - CO - H2O
	fragmentions[precursor_ion_co_loss_water_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss_water_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss_water_loss].name = "[M+zH]+ *-CO";
	fragmentions[precursor_ion_co_loss_water_loss].massdifference = PRECURSOR_ION - CO - H2O + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss_water_loss].parent = precursor_ion_co_loss;

	// initialize precursor ion - CO - NH3
	fragmentions[precursor_ion_co_loss_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss_ammonia_loss].name = "[M+zH]+ x-CO";
	fragmentions[precursor_ion_co_loss_ammonia_loss].massdifference = PRECURSOR_ION - CO - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss_ammonia_loss].parent = precursor_ion_co_loss;

	// initialize precursor ion - CO - H2O - NH3
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].name = "[M+zH]+ *x-CO";
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].massdifference = PRECURSOR_ION - CO - H2O - NH3 + nterminusshift  + cterminusshift;
	fragmentions[precursor_ion_co_loss_water_and_ammonia_loss].parent = precursor_ion_co_loss;

	// initialize cyclic precursor ion
	fragmentions[cyclic_precursor_ion].nterminal = false;
	fragmentions[cyclic_precursor_ion].cterminal = false;
	fragmentions[cyclic_precursor_ion].name = "[M+zH]+";
	fragmentions[cyclic_precursor_ion].massdifference = PRECURSOR_ION_CYCLIC;
	fragmentions[cyclic_precursor_ion].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - H2O
	fragmentions[cyclic_precursor_ion_water_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_water_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_water_loss].name = "[M+zH]+ *";
	fragmentions[cyclic_precursor_ion_water_loss].massdifference = PRECURSOR_ION_CYCLIC - H2O;
	fragmentions[cyclic_precursor_ion_water_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - NH3
	fragmentions[cyclic_precursor_ion_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_ammonia_loss].name = "[M+zH]+ x";
	fragmentions[cyclic_precursor_ion_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - NH3;
	fragmentions[cyclic_precursor_ion_ammonia_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - H2O - NH3
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].name = "[M+zH]+ *x";
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - H2O - NH3;
	fragmentions[cyclic_precursor_ion_water_and_ammonia_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - CO
	fragmentions[cyclic_precursor_ion_co_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss].name = "[M+zH]+ -CO";
	fragmentions[cyclic_precursor_ion_co_loss].massdifference = PRECURSOR_ION_CYCLIC - CO;
	fragmentions[cyclic_precursor_ion_co_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - CO - H2O
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].name = "[M+zH]+ *-CO";
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].massdifference = PRECURSOR_ION_CYCLIC - CO - H2O;
	fragmentions[cyclic_precursor_ion_co_loss_water_loss].parent = cyclic_precursor_ion_co_loss;

	// initialize cyclic precursor ion - CO - NH3
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].name = "[M+zH]+ x-CO";
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].massdifference = PRECURSOR_ION_CYCLIC - CO - NH3;
	fragmentions[cyclic_precursor_ion_co_loss_ammonia_loss].parent = cyclic_precursor_ion_co_loss;

	// initialize cyclic precursor ion - CO - H2O - NH3
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_water_and_ammonia_loss].name = "[M+zH]+ *x-CO";
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

	// initialize ion H+
	fragmentions[ms_hplus].nterminal = true;
	fragmentions[ms_hplus].cterminal = true;
	fragmentions[ms_hplus].name = "[M+H]+";
	fragmentions[ms_hplus].massdifference = Hplus;
	fragmentions[ms_hplus].parent = ms_hplus;
	fragmentions[ms_hplus].positive = true;
	fragmentions[ms_hplus].multiplier = 1;

	// initialize ion Na+
	fragmentions[ms_naplus].nterminal = true;
	fragmentions[ms_naplus].cterminal = true;
	fragmentions[ms_naplus].name = "[M+Na]+";
	fragmentions[ms_naplus].massdifference = Naplus;
	fragmentions[ms_naplus].parent = ms_naplus;
	fragmentions[ms_naplus].positive = true;
	fragmentions[ms_naplus].multiplier = 1;

	// initialize ion K+
	fragmentions[ms_kplus].nterminal = true;
	fragmentions[ms_kplus].cterminal = true;
	fragmentions[ms_kplus].name = "[M+K]+";
	fragmentions[ms_kplus].massdifference = Kplus;
	fragmentions[ms_kplus].parent = ms_kplus;
	fragmentions[ms_kplus].positive = true;
	fragmentions[ms_kplus].multiplier = 1;

	// initialize ion H-
	fragmentions[ms_hminus].nterminal = true;
	fragmentions[ms_hminus].cterminal = true;
	fragmentions[ms_hminus].name = "[M-H]-";
	fragmentions[ms_hminus].massdifference = -Hplus;
	fragmentions[ms_hminus].parent = ms_hminus;
	fragmentions[ms_hminus].positive = false;
	fragmentions[ms_hminus].multiplier = 1;

	// initialize ion [3M+2Fe-5H]+
	fragmentions[ms_3M2Fe5H].nterminal = true;
	fragmentions[ms_3M2Fe5H].cterminal = true;
	fragmentions[ms_3M2Fe5H].name = "[3M+2Fe-5H]+";
	fragmentions[ms_3M2Fe5H].massdifference = 2*Fe - 6*H + Hplus;
	fragmentions[ms_3M2Fe5H].parent = ms_3M2Fe5H;
	fragmentions[ms_3M2Fe5H].positive = true;
	fragmentions[ms_3M2Fe5H].multiplier = 3;

	// initialize ion [2M+Fe-2H]+
	fragmentions[ms_2MFe2H].nterminal = true;
	fragmentions[ms_2MFe2H].cterminal = true;
	fragmentions[ms_2MFe2H].name = "[2M+Fe-2H]+";
	fragmentions[ms_2MFe2H].massdifference = Fe - 3*H + Hplus;
	fragmentions[ms_2MFe2H].parent = ms_2MFe2H;
	fragmentions[ms_2MFe2H].positive = true;
	fragmentions[ms_2MFe2H].multiplier = 2;

	// initialize ion [3M+Fe-2H]+
	fragmentions[ms_3MFe2H].nterminal = true;
	fragmentions[ms_3MFe2H].cterminal = true;
	fragmentions[ms_3MFe2H].name = "[3M+Fe-2H]+";
	fragmentions[ms_3MFe2H].massdifference = Fe - 3*H + Hplus;
	fragmentions[ms_3MFe2H].parent = ms_3MFe2H;
	fragmentions[ms_3MFe2H].positive = true;
	fragmentions[ms_3MFe2H].multiplier = 3;

	// initialize ion [M+Fe-2H]+
	fragmentions[ms_MFe2H].nterminal = true;
	fragmentions[ms_MFe2H].cterminal = true;
	fragmentions[ms_MFe2H].name = "[M+Fe-2H]+";
	fragmentions[ms_MFe2H].massdifference = Fe - 3*H + Hplus;
	fragmentions[ms_MFe2H].parent = ms_MFe2H;
	fragmentions[ms_MFe2H].positive = true;
	fragmentions[ms_MFe2H].multiplier = 1;

	// initialize ion [3M+2Fe-6H+Na]+
	fragmentions[ms_3M2Fe6HNa].nterminal = true;
	fragmentions[ms_3M2Fe6HNa].cterminal = true;
	fragmentions[ms_3M2Fe6HNa].name = "[3M+2Fe-6H+Na]+";
	fragmentions[ms_3M2Fe6HNa].massdifference = 2*Fe - 6*H + Naplus;
	fragmentions[ms_3M2Fe6HNa].parent = ms_3M2Fe6HNa;
	fragmentions[ms_3M2Fe6HNa].positive = true;
	fragmentions[ms_3M2Fe6HNa].multiplier = 3;

	// initialize ion [2M+Fe-3H+Na]+
	fragmentions[ms_2MFe3HNa].nterminal = true;
	fragmentions[ms_2MFe3HNa].cterminal = true;
	fragmentions[ms_2MFe3HNa].name = "[2M+Fe-3H+Na]+";
	fragmentions[ms_2MFe3HNa].massdifference = Fe - 3*H + Naplus;
	fragmentions[ms_2MFe3HNa].parent = ms_2MFe3HNa;
	fragmentions[ms_2MFe3HNa].positive = true;
	fragmentions[ms_2MFe3HNa].multiplier = 2;

	// initialize ion [3M+Fe-3H+Na]+
	fragmentions[ms_3MFe3HNa].nterminal = true;
	fragmentions[ms_3MFe3HNa].cterminal = true;
	fragmentions[ms_3MFe3HNa].name = "[3M+Fe-3H+Na]+";
	fragmentions[ms_3MFe3HNa].massdifference = Fe - 3*H + Naplus;
	fragmentions[ms_3MFe3HNa].parent = ms_3MFe3HNa;
	fragmentions[ms_3MFe3HNa].positive = true;
	fragmentions[ms_3MFe3HNa].multiplier = 3;

	// initialize ion [M+Fe-3H+Na]+
	fragmentions[ms_MFe3HNa].nterminal = true;
	fragmentions[ms_MFe3HNa].cterminal = true;
	fragmentions[ms_MFe3HNa].name = "[M+Fe-3H+Na]+";
	fragmentions[ms_MFe3HNa].massdifference = Fe - 3*H + Naplus;
	fragmentions[ms_MFe3HNa].parent = ms_MFe3HNa;
	fragmentions[ms_MFe3HNa].positive = true;
	fragmentions[ms_MFe3HNa].multiplier = 1;

	// initialize ion [3M+2Fe-7H]-
	fragmentions[ms_3M2Fe7H].nterminal = true;
	fragmentions[ms_3M2Fe7H].cterminal = true;
	fragmentions[ms_3M2Fe7H].name = "[3M+2Fe-7H]-";
	fragmentions[ms_3M2Fe7H].massdifference = 2*Fe - 6*H - Hplus;
	fragmentions[ms_3M2Fe7H].parent = ms_3M2Fe7H;
	fragmentions[ms_3M2Fe7H].positive = false;
	fragmentions[ms_3M2Fe7H].multiplier = 3;

	// initialize ion [2M+Fe-4H]-
	fragmentions[ms_2MFe4H].nterminal = true;
	fragmentions[ms_2MFe4H].cterminal = true;
	fragmentions[ms_2MFe4H].name = "[2M+Fe-4H]-";
	fragmentions[ms_2MFe4H].massdifference = Fe - 3*H - Hplus;
	fragmentions[ms_2MFe4H].parent = ms_2MFe4H;
	fragmentions[ms_2MFe4H].positive = false;
	fragmentions[ms_2MFe4H].multiplier = 2;

	// initialize ion [3M+Fe-4H]-
	fragmentions[ms_3MFe4H].nterminal = true;
	fragmentions[ms_3MFe4H].cterminal = true;
	fragmentions[ms_3MFe4H].name = "[3M+Fe-4H]-";
	fragmentions[ms_3MFe4H].massdifference = Fe - 3*H - Hplus;
	fragmentions[ms_3MFe4H].parent = ms_3MFe4H;
	fragmentions[ms_3MFe4H].positive = false;
	fragmentions[ms_3MFe4H].multiplier = 3;

	// initialize ion [M+Fe-4H]-
	fragmentions[ms_MFe4H].nterminal = true;
	fragmentions[ms_MFe4H].cterminal = true;
	fragmentions[ms_MFe4H].name = "[M+Fe-4H]-";
	fragmentions[ms_MFe4H].massdifference = Fe - 3*H - Hplus;
	fragmentions[ms_MFe4H].parent = ms_MFe4H;
	fragmentions[ms_MFe4H].positive = false;
	fragmentions[ms_MFe4H].multiplier = 1;

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

