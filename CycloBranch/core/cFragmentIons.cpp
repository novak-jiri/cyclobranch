#include "core/cFragmentIons.h"
#include "core/cSummaryFormula.h"


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


double charge(double neutralmass, int charge) {
	return (neutralmass + (double)charge*Hplus)/(double)abs(charge);
}


double uncharge(double mzratio, int charge) {
	return mzratio*(double)abs(charge) - (double)charge*Hplus;
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
	string s = "";
	recalculateFragments(false, false, s);
}


cFragmentIons::cFragmentIons(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct) {
	recalculateFragments(cyclicnterminus, cycliccterminus, precursoradduct);
}


fragmentDescription &cFragmentIons::operator[](fragmentIonType iontype) {
	return fragmentions[iontype];
}


void cFragmentIons::recalculateFragments(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct) {

	fragmentions.clear();

	double nterminusshift = cyclicnterminus?-H2O:0;
	double cterminusshift = cycliccterminus?-H2O:0;
	double adductshift = 0;

	if (precursoradduct.compare("") != 0) {
		cSummaryFormula formula;
		formula.setFormula(precursoradduct);
		adductshift = formula.getMass() - H;
	}
	
	// initialize A-ion
	fragmentions[a_ion].nterminal = true;
	fragmentions[a_ion].cterminal = false;
	fragmentions[a_ion].name = "A";
	fragmentions[a_ion].massdifference = A_ION + nterminusshift + adductshift;
	fragmentions[a_ion].parent = b_ion; // ok
	
	// initialize A-H2O ion
	fragmentions[a_ion_dehydrated].nterminal = true;
	fragmentions[a_ion_dehydrated].cterminal = false;
	fragmentions[a_ion_dehydrated].name = "A*";
	fragmentions[a_ion_dehydrated].massdifference = A_ION - H2O + nterminusshift + adductshift;
	fragmentions[a_ion_dehydrated].parent = a_ion;

	// initialize A-NH3 ion
	fragmentions[a_ion_deamidated].nterminal = true;
	fragmentions[a_ion_deamidated].cterminal = false;
	fragmentions[a_ion_deamidated].name = "Ax";
	fragmentions[a_ion_deamidated].massdifference = A_ION - NH3 + nterminusshift + adductshift;
	fragmentions[a_ion_deamidated].parent = a_ion;

	// initialize A-H2O-NH3 ion
	fragmentions[a_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[a_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[a_ion_dehydrated_and_deamidated].name = "A*x";
	fragmentions[a_ion_dehydrated_and_deamidated].massdifference = A_ION - H2O - NH3 + nterminusshift + adductshift;
	fragmentions[a_ion_dehydrated_and_deamidated].parent = a_ion;

	// initialize B-ion
	fragmentions[b_ion].nterminal = true;
	fragmentions[b_ion].cterminal = false;
	fragmentions[b_ion].name = "B";
	fragmentions[b_ion].massdifference = B_ION + nterminusshift + adductshift;
	fragmentions[b_ion].parent = b_ion;

	// initialize B-H2O ion
	fragmentions[b_ion_dehydrated].nterminal = true;
	fragmentions[b_ion_dehydrated].cterminal = false;
	fragmentions[b_ion_dehydrated].name = "B*";
	fragmentions[b_ion_dehydrated].massdifference = B_ION - H2O + nterminusshift + adductshift;
	fragmentions[b_ion_dehydrated].parent = b_ion;

	// initialize B-NH3 ion
	fragmentions[b_ion_deamidated].nterminal = true;
	fragmentions[b_ion_deamidated].cterminal = false;
	fragmentions[b_ion_deamidated].name = "Bx";
	fragmentions[b_ion_deamidated].massdifference = B_ION - NH3 + nterminusshift + adductshift;
	fragmentions[b_ion_deamidated].parent = b_ion;

	// initialize B-H2O-NH3 ion
	fragmentions[b_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[b_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[b_ion_dehydrated_and_deamidated].name = "B*x";
	fragmentions[b_ion_dehydrated_and_deamidated].massdifference = B_ION - H2O - NH3 + nterminusshift + adductshift;
	fragmentions[b_ion_dehydrated_and_deamidated].parent = b_ion;

	// initialize C-ion
	fragmentions[c_ion].nterminal = true;
	fragmentions[c_ion].cterminal = false;
	fragmentions[c_ion].name = "C";
	fragmentions[c_ion].massdifference = C_ION + nterminusshift + adductshift;
	fragmentions[c_ion].parent = c_ion;
	
	// initialize C-H2O ion
	fragmentions[c_ion_dehydrated].nterminal = true;
	fragmentions[c_ion_dehydrated].cterminal = false;
	fragmentions[c_ion_dehydrated].name = "C*";
	fragmentions[c_ion_dehydrated].massdifference = C_ION - H2O + nterminusshift + adductshift;
	fragmentions[c_ion_dehydrated].parent = c_ion;

	// initialize C-NH3 ion
	fragmentions[c_ion_deamidated].nterminal = true;
	fragmentions[c_ion_deamidated].cterminal = false;
	fragmentions[c_ion_deamidated].name = "Cx";
	fragmentions[c_ion_deamidated].massdifference = C_ION - NH3 + nterminusshift + adductshift;
	fragmentions[c_ion_deamidated].parent = c_ion;

	// initialize C-H2O-NH3 ion
	fragmentions[c_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[c_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[c_ion_dehydrated_and_deamidated].name = "C*x";
	fragmentions[c_ion_dehydrated_and_deamidated].massdifference = C_ION - H2O - NH3 + nterminusshift + adductshift;
	fragmentions[c_ion_dehydrated_and_deamidated].parent = c_ion;

	// initialize X ion
	fragmentions[x_ion].nterminal = false;
	fragmentions[x_ion].cterminal = true;
	fragmentions[x_ion].name = "X";
	fragmentions[x_ion].massdifference = X_ION + cterminusshift + adductshift;
	fragmentions[x_ion].parent = x_ion;

	// initialize X-H2O ion
	fragmentions[x_ion_dehydrated].nterminal = false;
	fragmentions[x_ion_dehydrated].cterminal = true;
	fragmentions[x_ion_dehydrated].name = "X*";
	fragmentions[x_ion_dehydrated].massdifference = X_ION - H2O + cterminusshift + adductshift;
	fragmentions[x_ion_dehydrated].parent = x_ion;

	// initialize X-NH3 ion
	fragmentions[x_ion_deamidated].nterminal = false;
	fragmentions[x_ion_deamidated].cterminal = true;
	fragmentions[x_ion_deamidated].name = "Xx";
	fragmentions[x_ion_deamidated].massdifference = X_ION - NH3 + cterminusshift + adductshift;
	fragmentions[x_ion_deamidated].parent = x_ion;

	// initialize X-H2O-NH3 ion
	fragmentions[x_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[x_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[x_ion_dehydrated_and_deamidated].name = "X*x";
	fragmentions[x_ion_dehydrated_and_deamidated].massdifference = X_ION - H2O - NH3 + cterminusshift + adductshift;
	fragmentions[x_ion_dehydrated_and_deamidated].parent = x_ion;

	// initialize Y ion
	fragmentions[y_ion].nterminal = false;
	fragmentions[y_ion].cterminal = true;
	fragmentions[y_ion].name = "Y";
	fragmentions[y_ion].massdifference = Y_ION + cterminusshift + adductshift;
	fragmentions[y_ion].parent = y_ion;

	// initialize Y-H2O ion
	fragmentions[y_ion_dehydrated].nterminal = false;
	fragmentions[y_ion_dehydrated].cterminal = true;
	fragmentions[y_ion_dehydrated].name = "Y*";
	fragmentions[y_ion_dehydrated].massdifference = Y_ION - H2O + cterminusshift + adductshift;
	fragmentions[y_ion_dehydrated].parent = y_ion;

	// initialize Y-NH3 ion
	fragmentions[y_ion_deamidated].nterminal = false;
	fragmentions[y_ion_deamidated].cterminal = true;
	fragmentions[y_ion_deamidated].name = "Yx";
	fragmentions[y_ion_deamidated].massdifference = Y_ION - NH3 + cterminusshift + adductshift;
	fragmentions[y_ion_deamidated].parent = y_ion;

	// initialize Y-H2O-NH3 ion
	fragmentions[y_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[y_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[y_ion_dehydrated_and_deamidated].name = "Y*x";
	fragmentions[y_ion_dehydrated_and_deamidated].massdifference = Y_ION - H2O - NH3 + cterminusshift + adductshift;
	fragmentions[y_ion_dehydrated_and_deamidated].parent = y_ion;

	// initialize Z ion
	fragmentions[z_ion].nterminal = false;
	fragmentions[z_ion].cterminal = true;
	fragmentions[z_ion].name = "Z";
	fragmentions[z_ion].massdifference = Z_ION + cterminusshift + adductshift;
	fragmentions[z_ion].parent = z_ion;

	// initialize Z-H2O ion
	fragmentions[z_ion_dehydrated].nterminal = false;
	fragmentions[z_ion_dehydrated].cterminal = true;
	fragmentions[z_ion_dehydrated].name = "Z*";
	fragmentions[z_ion_dehydrated].massdifference = Z_ION - H2O + cterminusshift + adductshift;
	fragmentions[z_ion_dehydrated].parent = z_ion;

	// initialize Z-NH3 ion
	fragmentions[z_ion_deamidated].nterminal = false;
	fragmentions[z_ion_deamidated].cterminal = true;
	fragmentions[z_ion_deamidated].name = "Zx";
	fragmentions[z_ion_deamidated].massdifference = Z_ION - NH3 + cterminusshift + adductshift;
	fragmentions[z_ion_deamidated].parent = z_ion;

	// initialize Z-H2O-NH3 ion
	fragmentions[z_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[z_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[z_ion_dehydrated_and_deamidated].name = "Z*x";
	fragmentions[z_ion_dehydrated_and_deamidated].massdifference = Z_ION - H2O - NH3 + cterminusshift + adductshift;
	fragmentions[z_ion_dehydrated_and_deamidated].parent = z_ion;

	// initialize precursor ion
	fragmentions[precursor_ion].nterminal = false;
	fragmentions[precursor_ion].cterminal = false;
	fragmentions[precursor_ion].name = "[M+zH]+";
	fragmentions[precursor_ion].massdifference = PRECURSOR_ION + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion].parent = precursor_ion;

	// initialize precursor ion - H2O
	fragmentions[precursor_ion_dehydrated].nterminal = false;
	fragmentions[precursor_ion_dehydrated].cterminal = false;
	fragmentions[precursor_ion_dehydrated].name = "[M+zH]+ *";
	fragmentions[precursor_ion_dehydrated].massdifference = PRECURSOR_ION - H2O + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_dehydrated].parent = precursor_ion;

	// initialize precursor ion - NH3
	fragmentions[precursor_ion_deamidated].nterminal = false;
	fragmentions[precursor_ion_deamidated].cterminal = false;
	fragmentions[precursor_ion_deamidated].name = "[M+zH]+ x";
	fragmentions[precursor_ion_deamidated].massdifference = PRECURSOR_ION - NH3 + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_deamidated].parent = precursor_ion;

	// initialize precursor ion - H2O - NH3
	fragmentions[precursor_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[precursor_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[precursor_ion_dehydrated_and_deamidated].name = "[M+zH]+ *x";
	fragmentions[precursor_ion_dehydrated_and_deamidated].massdifference = PRECURSOR_ION - H2O - NH3 + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_dehydrated_and_deamidated].parent = precursor_ion;

	// initialize precursor ion - CO
	fragmentions[precursor_ion_co_loss].nterminal = false;
	fragmentions[precursor_ion_co_loss].cterminal = false;
	fragmentions[precursor_ion_co_loss].name = "[M+zH]+ -CO";
	fragmentions[precursor_ion_co_loss].massdifference = PRECURSOR_ION - CO + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_co_loss].parent = precursor_ion;

	// initialize precursor ion - CO - H2O
	fragmentions[precursor_ion_co_loss_and_dehydrated].nterminal = false;
	fragmentions[precursor_ion_co_loss_and_dehydrated].cterminal = false;
	fragmentions[precursor_ion_co_loss_and_dehydrated].name = "[M+zH]+ *-CO";
	fragmentions[precursor_ion_co_loss_and_dehydrated].massdifference = PRECURSOR_ION - CO - H2O + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_co_loss_and_dehydrated].parent = precursor_ion_co_loss;

	// initialize precursor ion - CO - NH3
	fragmentions[precursor_ion_co_loss_and_deamidated].nterminal = false;
	fragmentions[precursor_ion_co_loss_and_deamidated].cterminal = false;
	fragmentions[precursor_ion_co_loss_and_deamidated].name = "[M+zH]+ x-CO";
	fragmentions[precursor_ion_co_loss_and_deamidated].massdifference = PRECURSOR_ION - CO - NH3 + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_co_loss_and_deamidated].parent = precursor_ion_co_loss;

	// initialize precursor ion - CO - H2O - NH3
	fragmentions[precursor_ion_co_loss_and_dehydrated_and_deamidated].nterminal = false;
	fragmentions[precursor_ion_co_loss_and_dehydrated_and_deamidated].cterminal = false;
	fragmentions[precursor_ion_co_loss_and_dehydrated_and_deamidated].name = "[M+zH]+ *x-CO";
	fragmentions[precursor_ion_co_loss_and_dehydrated_and_deamidated].massdifference = PRECURSOR_ION - CO - H2O - NH3 + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion_co_loss_and_dehydrated_and_deamidated].parent = precursor_ion_co_loss;

	// initialize cyclic precursor ion
	fragmentions[cyclic_precursor_ion].nterminal = false;
	fragmentions[cyclic_precursor_ion].cterminal = false;
	fragmentions[cyclic_precursor_ion].name = "[M+zH]+";
	fragmentions[cyclic_precursor_ion].massdifference = PRECURSOR_ION_CYCLIC + adductshift;
	fragmentions[cyclic_precursor_ion].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - H2O
	fragmentions[cyclic_precursor_ion_dehydrated].nterminal = false;
	fragmentions[cyclic_precursor_ion_dehydrated].cterminal = false;
	fragmentions[cyclic_precursor_ion_dehydrated].name = "[M+zH]+ *";
	fragmentions[cyclic_precursor_ion_dehydrated].massdifference = PRECURSOR_ION_CYCLIC - H2O + adductshift;
	fragmentions[cyclic_precursor_ion_dehydrated].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - NH3
	fragmentions[cyclic_precursor_ion_deamidated].nterminal = false;
	fragmentions[cyclic_precursor_ion_deamidated].cterminal = false;
	fragmentions[cyclic_precursor_ion_deamidated].name = "[M+zH]+ x";
	fragmentions[cyclic_precursor_ion_deamidated].massdifference = PRECURSOR_ION_CYCLIC - NH3 + adductshift;
	fragmentions[cyclic_precursor_ion_deamidated].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - H2O - NH3
	fragmentions[cyclic_precursor_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[cyclic_precursor_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[cyclic_precursor_ion_dehydrated_and_deamidated].name = "[M+zH]+ *x";
	fragmentions[cyclic_precursor_ion_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_CYCLIC - H2O - NH3 + adductshift;
	fragmentions[cyclic_precursor_ion_dehydrated_and_deamidated].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - CO
	fragmentions[cyclic_precursor_ion_co_loss].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss].name = "[M+zH]+ -CO";
	fragmentions[cyclic_precursor_ion_co_loss].massdifference = PRECURSOR_ION_CYCLIC - CO + adductshift;
	fragmentions[cyclic_precursor_ion_co_loss].parent = cyclic_precursor_ion;

	// initialize cyclic precursor ion - CO - H2O
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated].name = "[M+zH]+ *-CO";
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated].massdifference = PRECURSOR_ION_CYCLIC - CO - H2O + adductshift;
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated].parent = cyclic_precursor_ion_co_loss;

	// initialize cyclic precursor ion - CO - NH3
	fragmentions[cyclic_precursor_ion_co_loss_and_deamidated].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_and_deamidated].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_and_deamidated].name = "[M+zH]+ x-CO";
	fragmentions[cyclic_precursor_ion_co_loss_and_deamidated].massdifference = PRECURSOR_ION_CYCLIC - CO - NH3 + adductshift;
	fragmentions[cyclic_precursor_ion_co_loss_and_deamidated].parent = cyclic_precursor_ion_co_loss;

	// initialize cyclic precursor ion - CO - H2O - NH3
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated].nterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated].cterminal = false;
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated].name = "[M+zH]+ *x-CO";
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_CYCLIC - CO - H2O - NH3 + adductshift;
	fragmentions[cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated].parent = cyclic_precursor_ion_co_loss;

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
		it->second.store(os);
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
		description.load(is);
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


int loadModificationsFromPlainTextStream(ifstream &stream, vector<fragmentDescription>& modifications, string& errormessage, bool ignoreerrors) {
	string s;
	fragmentDescription modification;
	size_t pos;
	double mass;

	bool error = false;
	errormessage = "";
	cSummaryFormula formula;
	
	modifications.clear();
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

		modification.clear();

		// load the name
		pos = s.find('\t');
		if (pos != string::npos) {
			modification.name = s.substr(0, pos);
			s = s.substr(pos + 1);
		}
		else {
			break;
		}

		// load summary
		pos = s.find('\t');
		if (pos != string::npos) {
			modification.summary = s.substr(0, pos);
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


		// load N-terminal flag
		pos = s.find('\t');
		if (pos != string::npos) {
			modification.nterminal = (s.substr(0, pos).compare("0") == 0)?false:true;
			s = s.substr(pos + 1);
		}
		else {
			break;
		}


		// load C-terminal flag
		#if OS_TYPE == UNX
			if ((s.size() > 0) && (s.back() == '\r')) {
				s = s.substr(0, s.size() - 1);
			}
		#endif
		modification.cterminal = (s.compare("0") == 0)?false:true;


		// calculate mass from the summary
		formula.clear();
		formula.setFormula(modification.summary);
		if (formula.isValid(errormessage)) {
			modification.massdifference = formula.getMass();
		}
		else {
			if (!ignoreerrors) {
				error = true;
				break;
			}
		}

		modifications.push_back(modification);

	}

	if (!ignoreerrors && error) {
		return -1;
	}

	return 0;
}


void storeModificationsToPlainTextStream(ofstream &stream, vector<fragmentDescription>& modifications) {
	for (int i = 0; i < (int)modifications.size(); i++) {
		stream << removeWhiteSpacesExceptSpaces(modifications[i].name) << "\t";
		stream << removeWhiteSpacesExceptSpaces(modifications[i].summary) << "\t";
		stream << std::fixed << std::setprecision(10) << modifications[i].massdifference << "\t";

		if (modifications[i].nterminal) {
			stream << 1 << "\t";
		}
		else {
			stream << 0 << "\t";
		}

		if (modifications[i].cterminal) {
			stream << 1 << endl;
		}
		else {
			stream << 0 << endl;
		}
	}
}

