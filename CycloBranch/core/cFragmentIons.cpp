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
	table["Ne"] = Ne;
	table["Na"] = Na;
	table["Mg"] = Mg;
	table["Al"] = Al;
	table["Si"] = Si;
	table["P"] = P;
	table["S"] = S;
	table["Cl"] = Cl;
	table["Ar"] = Ar;
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
	table["Ga"] = Ga;
	table["Ge"] = Ge;
	table["As"] = As;
	table["Se"] = Se;
	table["Br"] = Br;
	table["Kr"] = Kr;
	table["Rb"] = Rb;
	table["Sr"] = Sr;
	table["Y"] = Y;
	table["Zr"] = Zr;
	table["Nb"] = Nb;
	table["Mo"] = Mo;
	table["Tc"] = Tc;
	table["Ru"] = Ru;
	table["Rh"] = Rh;
	table["Pd"] = Pd;
	table["Ag"] = Ag;
	table["Cd"] = Cd;
	table["In"] = In;
	table["Sn"] = Sn;
	table["Sb"] = Sb;
	table["Te"] = Te;
	table["I"] = I;
	table["Xe"] = Xe;
	table["Cs"] = Cs;
	table["Ba"] = Ba;
	table["La"] = La;
	table["Ce"] = Ce;
	table["Pr"] = Pr;
	table["Nd"] = Nd;
	table["Pm"] = Pm;
	table["Sm"] = Sm;
	table["Eu"] = Eu;
	table["Gd"] = Gd;
	table["Tb"] = Tb;
	table["Dy"] = Dy;
	table["Ho"] = Ho;
	table["Er"] = Er;
	table["Tm"] = Tm;
	table["Yb"] = Yb;
	table["Lu"] = Lu;
	table["Hf"] = Hf;
	table["Ta"] = Ta;
	table["W"] = W;
	table["Re"] = Re;
	table["Os"] = Os;
	table["Ir"] = Ir;
	table["Pt"] = Pt;
	table["Au"] = Au;
	table["Hg"] = Hg;
	table["Tl"] = Tl;
	table["Pb"] = Pb;
	table["Bi"] = Bi;
	table["Po"] = Po;
	table["At"] = At;
	table["Rn"] = Rn;
	table["Fr"] = Fr;
	table["Ra"] = Ra;
	table["Ac"] = Ac;
	table["Th"] = Th;
	table["Pa"] = Pa;
	table["U"] = U;
	table["Np"] = Np;
	table["Pu"] = Pu;
	table["Am"] = Am;
	table["Cm"] = Cm;
	table["Bk"] = Bk;
	table["Cf"] = Cf;
	table["Es"] = Es;
	table["Fm"] = Fm;
	table["Md"] = Md;
	table["No"] = No;
	table["Lr"] = Lr;
	table["R"] = R;
	table["X"] = X;
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
	os.write((char *)&parent, sizeof(eFragmentIonType));
	os.write((char *)&positive, sizeof(bool));
	os.write((char *)&multiplier, sizeof(int));
	os.write((char *)&numberofisotopes, sizeof(int));
}


void fragmentDescription::load(ifstream& is) {
	loadString(name, is);
	loadString(summary, is);
	is.read((char *)&massdifference, sizeof(double));
	is.read((char *)&nterminal, sizeof(bool));
	is.read((char *)&cterminal, sizeof(bool));
	is.read((char *)&parent, sizeof(eFragmentIonType));
	is.read((char *)&positive, sizeof(bool));
	is.read((char *)&multiplier, sizeof(int));
	is.read((char *)&numberofisotopes, sizeof(int));
}


cFragmentIons::cFragmentIons() {
	string s = "";
	recalculateFragments(false, false, s);
}


cFragmentIons::cFragmentIons(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct) {
	recalculateFragments(cyclicnterminus, cycliccterminus, precursoradduct);
}


fragmentDescription &cFragmentIons::operator[](eFragmentIonType iontype) {
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
	fragmentions[ms_hplus].numberofisotopes = 0;

	// initialize ion Na+
	fragmentions[ms_naplus].nterminal = true;
	fragmentions[ms_naplus].cterminal = true;
	fragmentions[ms_naplus].name = "[M+Na]+";
	fragmentions[ms_naplus].massdifference = Naplus;
	fragmentions[ms_naplus].parent = ms_naplus;
	fragmentions[ms_naplus].positive = true;
	fragmentions[ms_naplus].multiplier = 1;
	fragmentions[ms_naplus].numberofisotopes = 0;

	// initialize ion K+
	fragmentions[ms_kplus].nterminal = true;
	fragmentions[ms_kplus].cterminal = true;
	fragmentions[ms_kplus].name = "[M+K]+";
	fragmentions[ms_kplus].massdifference = Kplus;
	fragmentions[ms_kplus].parent = ms_kplus;
	fragmentions[ms_kplus].positive = true;
	fragmentions[ms_kplus].multiplier = 1;
	fragmentions[ms_kplus].numberofisotopes = 1;

	// initialize ion H-
	fragmentions[ms_hminus].nterminal = true;
	fragmentions[ms_hminus].cterminal = true;
	fragmentions[ms_hminus].name = "[M-H]-";
	fragmentions[ms_hminus].massdifference = -Hplus;
	fragmentions[ms_hminus].parent = ms_hminus;
	fragmentions[ms_hminus].positive = false;
	fragmentions[ms_hminus].multiplier = 1;
	fragmentions[ms_hminus].numberofisotopes = 0;

	// initialize ion [M+Fe-2H]+
	fragmentions[ms_MFe2H].nterminal = true;
	fragmentions[ms_MFe2H].cterminal = true;
	fragmentions[ms_MFe2H].name = "[M+Fe-2H]+";
	fragmentions[ms_MFe2H].massdifference = Fe - 3*H + Hplus;
	fragmentions[ms_MFe2H].parent = ms_MFe2H;
	fragmentions[ms_MFe2H].positive = true;
	fragmentions[ms_MFe2H].multiplier = 1;
	fragmentions[ms_MFe2H].numberofisotopes = 1;

	// initialize ion [M+Fe-3H+Na]+
	fragmentions[ms_MFe3HNa].nterminal = true;
	fragmentions[ms_MFe3HNa].cterminal = true;
	fragmentions[ms_MFe3HNa].name = "[M+Fe-3H+Na]+";
	fragmentions[ms_MFe3HNa].massdifference = Fe - 3*H + Naplus;
	fragmentions[ms_MFe3HNa].parent = ms_MFe3HNa;
	fragmentions[ms_MFe3HNa].positive = true;
	fragmentions[ms_MFe3HNa].multiplier = 1;
	fragmentions[ms_MFe3HNa].numberofisotopes = 1;

	// initialize ion [2M+Fe-2H]+
	fragmentions[ms_2MFe2H].nterminal = true;
	fragmentions[ms_2MFe2H].cterminal = true;
	fragmentions[ms_2MFe2H].name = "[2M+Fe-2H]+";
	fragmentions[ms_2MFe2H].massdifference = Fe - 3*H + Hplus;
	fragmentions[ms_2MFe2H].parent = ms_2MFe2H;
	fragmentions[ms_2MFe2H].positive = true;
	fragmentions[ms_2MFe2H].multiplier = 2;
	fragmentions[ms_2MFe2H].numberofisotopes = 1;

	// initialize ion [2M+Fe-3H+Na]+
	fragmentions[ms_2MFe3HNa].nterminal = true;
	fragmentions[ms_2MFe3HNa].cterminal = true;
	fragmentions[ms_2MFe3HNa].name = "[2M+Fe-3H+Na]+";
	fragmentions[ms_2MFe3HNa].massdifference = Fe - 3*H + Naplus;
	fragmentions[ms_2MFe3HNa].parent = ms_2MFe3HNa;
	fragmentions[ms_2MFe3HNa].positive = true;
	fragmentions[ms_2MFe3HNa].multiplier = 2;
	fragmentions[ms_2MFe3HNa].numberofisotopes = 1;

	// initialize ion [3M+Fe-2H]+
	fragmentions[ms_3MFe2H].nterminal = true;
	fragmentions[ms_3MFe2H].cterminal = true;
	fragmentions[ms_3MFe2H].name = "[3M+Fe-2H]+";
	fragmentions[ms_3MFe2H].massdifference = Fe - 3*H + Hplus;
	fragmentions[ms_3MFe2H].parent = ms_3MFe2H;
	fragmentions[ms_3MFe2H].positive = true;
	fragmentions[ms_3MFe2H].multiplier = 3;
	fragmentions[ms_3MFe2H].numberofisotopes = 1;

	// initialize ion [3M+Fe-3H+Na]+
	fragmentions[ms_3MFe3HNa].nterminal = true;
	fragmentions[ms_3MFe3HNa].cterminal = true;
	fragmentions[ms_3MFe3HNa].name = "[3M+Fe-3H+Na]+";
	fragmentions[ms_3MFe3HNa].massdifference = Fe - 3*H + Naplus;
	fragmentions[ms_3MFe3HNa].parent = ms_3MFe3HNa;
	fragmentions[ms_3MFe3HNa].positive = true;
	fragmentions[ms_3MFe3HNa].multiplier = 3;
	fragmentions[ms_3MFe3HNa].numberofisotopes = 1;

	// initialize ion [3M+2Fe-5H]+
	fragmentions[ms_3M2Fe5H].nterminal = true;
	fragmentions[ms_3M2Fe5H].cterminal = true;
	fragmentions[ms_3M2Fe5H].name = "[3M+2Fe-5H]+";
	fragmentions[ms_3M2Fe5H].massdifference = 2*Fe - 6*H + Hplus;
	fragmentions[ms_3M2Fe5H].parent = ms_3M2Fe5H;
	fragmentions[ms_3M2Fe5H].positive = true;
	fragmentions[ms_3M2Fe5H].multiplier = 3;
	fragmentions[ms_3M2Fe5H].numberofisotopes = 1;

	// initialize ion [3M+2Fe-6H+Na]+
	fragmentions[ms_3M2Fe6HNa].nterminal = true;
	fragmentions[ms_3M2Fe6HNa].cterminal = true;
	fragmentions[ms_3M2Fe6HNa].name = "[3M+2Fe-6H+Na]+";
	fragmentions[ms_3M2Fe6HNa].massdifference = 2*Fe - 6*H + Naplus;
	fragmentions[ms_3M2Fe6HNa].parent = ms_3M2Fe6HNa;
	fragmentions[ms_3M2Fe6HNa].positive = true;
	fragmentions[ms_3M2Fe6HNa].multiplier = 3;
	fragmentions[ms_3M2Fe6HNa].numberofisotopes = 1;

	// initialize ion [M+Fe-4H]-
	fragmentions[ms_MFe4H].nterminal = true;
	fragmentions[ms_MFe4H].cterminal = true;
	fragmentions[ms_MFe4H].name = "[M+Fe-4H]-";
	fragmentions[ms_MFe4H].massdifference = Fe - 3*H - Hplus;
	fragmentions[ms_MFe4H].parent = ms_MFe4H;
	fragmentions[ms_MFe4H].positive = false;
	fragmentions[ms_MFe4H].multiplier = 1;
	fragmentions[ms_MFe4H].numberofisotopes = 1;

	// initialize ion [2M+Fe-4H]-
	fragmentions[ms_2MFe4H].nterminal = true;
	fragmentions[ms_2MFe4H].cterminal = true;
	fragmentions[ms_2MFe4H].name = "[2M+Fe-4H]-";
	fragmentions[ms_2MFe4H].massdifference = Fe - 3*H - Hplus;
	fragmentions[ms_2MFe4H].parent = ms_2MFe4H;
	fragmentions[ms_2MFe4H].positive = false;
	fragmentions[ms_2MFe4H].multiplier = 2;
	fragmentions[ms_2MFe4H].numberofisotopes = 1;

	// initialize ion [3M+Fe-4H]-
	fragmentions[ms_3MFe4H].nterminal = true;
	fragmentions[ms_3MFe4H].cterminal = true;
	fragmentions[ms_3MFe4H].name = "[3M+Fe-4H]-";
	fragmentions[ms_3MFe4H].massdifference = Fe - 3*H - Hplus;
	fragmentions[ms_3MFe4H].parent = ms_3MFe4H;
	fragmentions[ms_3MFe4H].positive = false;
	fragmentions[ms_3MFe4H].multiplier = 3;
	fragmentions[ms_3MFe4H].numberofisotopes = 1;

	// initialize ion [3M+2Fe-7H]-
	fragmentions[ms_3M2Fe7H].nterminal = true;
	fragmentions[ms_3M2Fe7H].cterminal = true;
	fragmentions[ms_3M2Fe7H].name = "[3M+2Fe-7H]-";
	fragmentions[ms_3M2Fe7H].massdifference = 2*Fe - 6*H - Hplus;
	fragmentions[ms_3M2Fe7H].parent = ms_3M2Fe7H;
	fragmentions[ms_3M2Fe7H].positive = false;
	fragmentions[ms_3M2Fe7H].multiplier = 3;
	fragmentions[ms_3M2Fe7H].numberofisotopes = 1;

	// initialize ion Li+
	fragmentions[ms_liplus].nterminal = true;
	fragmentions[ms_liplus].cterminal = true;
	fragmentions[ms_liplus].name = "[M+Li]+";
	fragmentions[ms_liplus].massdifference = Liplus;
	fragmentions[ms_liplus].parent = ms_liplus;
	fragmentions[ms_liplus].positive = true;
	fragmentions[ms_liplus].multiplier = 1;
	fragmentions[ms_liplus].numberofisotopes = 1;

	// initialize ion [M+Mg-H]+
	fragmentions[ms_MMgH].nterminal = true;
	fragmentions[ms_MMgH].cterminal = true;
	fragmentions[ms_MMgH].name = "[M+Mg-H]+";
	fragmentions[ms_MMgH].massdifference = Mg - 2*H + Hplus;
	fragmentions[ms_MMgH].parent = ms_MMgH;
	fragmentions[ms_MMgH].positive = true;
	fragmentions[ms_MMgH].multiplier = 1;
	fragmentions[ms_MMgH].numberofisotopes = 2;

	// initialize ion [M+Mg-2H+Na]+
	fragmentions[ms_MMg2HNa].nterminal = true;
	fragmentions[ms_MMg2HNa].cterminal = true;
	fragmentions[ms_MMg2HNa].name = "[M+Mg-2H+Na]+";
	fragmentions[ms_MMg2HNa].massdifference = Mg - 2*H + Naplus;
	fragmentions[ms_MMg2HNa].parent = ms_MMg2HNa;
	fragmentions[ms_MMg2HNa].positive = true;
	fragmentions[ms_MMg2HNa].multiplier = 1;
	fragmentions[ms_MMg2HNa].numberofisotopes = 2;

	// initialize ion [M+Mg-3H]-
	fragmentions[ms_MMg3H].nterminal = true;
	fragmentions[ms_MMg3H].cterminal = true;
	fragmentions[ms_MMg3H].name = "[M+Mg-3H]-";
	fragmentions[ms_MMg3H].massdifference = Mg - 2*H - Hplus;
	fragmentions[ms_MMg3H].parent = ms_MMg3H;
	fragmentions[ms_MMg3H].positive = false;
	fragmentions[ms_MMg3H].multiplier = 1;
	fragmentions[ms_MMg3H].numberofisotopes = 2;

	// initialize ion [M+Al-2H]+
	fragmentions[ms_MAl2H].nterminal = true;
	fragmentions[ms_MAl2H].cterminal = true;
	fragmentions[ms_MAl2H].name = "[M+Al-2H]+";
	fragmentions[ms_MAl2H].massdifference = Al - 3*H + Hplus;
	fragmentions[ms_MAl2H].parent = ms_MAl2H;
	fragmentions[ms_MAl2H].positive = true;
	fragmentions[ms_MAl2H].multiplier = 1;
	fragmentions[ms_MAl2H].numberofisotopes = 0;

	// initialize ion [M+Al-3H+Na]+
	fragmentions[ms_MAl3HNa].nterminal = true;
	fragmentions[ms_MAl3HNa].cterminal = true;
	fragmentions[ms_MAl3HNa].name = "[M+Al-3H+Na]+";
	fragmentions[ms_MAl3HNa].massdifference = Al - 3*H + Naplus;
	fragmentions[ms_MAl3HNa].parent = ms_MAl3HNa;
	fragmentions[ms_MAl3HNa].positive = true;
	fragmentions[ms_MAl3HNa].multiplier = 1;
	fragmentions[ms_MAl3HNa].numberofisotopes = 0;

	// initialize ion [M+Al-4H]-
	fragmentions[ms_MAl4H].nterminal = true;
	fragmentions[ms_MAl4H].cterminal = true;
	fragmentions[ms_MAl4H].name = "[M+Al-4H]-";
	fragmentions[ms_MAl4H].massdifference = Al - 3*H - Hplus;
	fragmentions[ms_MAl4H].parent = ms_MAl4H;
	fragmentions[ms_MAl4H].positive = false;
	fragmentions[ms_MAl4H].multiplier = 1;
	fragmentions[ms_MAl4H].numberofisotopes = 0;

	// initialize ion [M+Ca-H]+
	fragmentions[ms_MCaH].nterminal = true;
	fragmentions[ms_MCaH].cterminal = true;
	fragmentions[ms_MCaH].name = "[M+Ca-H]+";
	fragmentions[ms_MCaH].massdifference = Ca - 2*H + Hplus;
	fragmentions[ms_MCaH].parent = ms_MCaH;
	fragmentions[ms_MCaH].positive = true;
	fragmentions[ms_MCaH].multiplier = 1;
	fragmentions[ms_MCaH].numberofisotopes = 1;

	// initialize ion [M+Ca-2H+Na]+
	fragmentions[ms_MCa2HNa].nterminal = true;
	fragmentions[ms_MCa2HNa].cterminal = true;
	fragmentions[ms_MCa2HNa].name = "[M+Ca-2H+Na]+";
	fragmentions[ms_MCa2HNa].massdifference = Ca - 2*H + Naplus;
	fragmentions[ms_MCa2HNa].parent = ms_MCa2HNa;
	fragmentions[ms_MCa2HNa].positive = true;
	fragmentions[ms_MCa2HNa].multiplier = 1;
	fragmentions[ms_MCa2HNa].numberofisotopes = 1;

	// initialize ion [M+Ca-3H]-
	fragmentions[ms_MCa3H].nterminal = true;
	fragmentions[ms_MCa3H].cterminal = true;
	fragmentions[ms_MCa3H].name = "[M+Ca-3H]-";
	fragmentions[ms_MCa3H].massdifference = Ca - 2*H - Hplus;
	fragmentions[ms_MCa3H].parent = ms_MCa3H;
	fragmentions[ms_MCa3H].positive = false;
	fragmentions[ms_MCa3H].multiplier = 1;
	fragmentions[ms_MCa3H].numberofisotopes = 1;

	// initialize ion [M+Mn-H]+
	fragmentions[ms_MMnH].nterminal = true;
	fragmentions[ms_MMnH].cterminal = true;
	fragmentions[ms_MMnH].name = "[M+Mn-H]+";
	fragmentions[ms_MMnH].massdifference = Mn - 2*H + Hplus;
	fragmentions[ms_MMnH].parent = ms_MMnH;
	fragmentions[ms_MMnH].positive = true;
	fragmentions[ms_MMnH].multiplier = 1;
	fragmentions[ms_MMnH].numberofisotopes = 0;

	// initialize ion [M+Mn-2H+Na]+
	fragmentions[ms_MMn2HNa].nterminal = true;
	fragmentions[ms_MMn2HNa].cterminal = true;
	fragmentions[ms_MMn2HNa].name = "[M+Mn-2H+Na]+";
	fragmentions[ms_MMn2HNa].massdifference = Mn - 2*H + Naplus;
	fragmentions[ms_MMn2HNa].parent = ms_MMn2HNa;
	fragmentions[ms_MMn2HNa].positive = true;
	fragmentions[ms_MMn2HNa].multiplier = 1;
	fragmentions[ms_MMn2HNa].numberofisotopes = 0;

	// initialize ion [M+Mn-3H]-
	fragmentions[ms_MMn3H].nterminal = true;
	fragmentions[ms_MMn3H].cterminal = true;
	fragmentions[ms_MMn3H].name = "[M+Mn-3H]-";
	fragmentions[ms_MMn3H].massdifference = Mn - 2*H - Hplus;
	fragmentions[ms_MMn3H].parent = ms_MMn3H;
	fragmentions[ms_MMn3H].positive = false;
	fragmentions[ms_MMn3H].multiplier = 1;
	fragmentions[ms_MMn3H].numberofisotopes = 0;

	// initialize ion [M+Co-H]+
	fragmentions[ms_MCoH].nterminal = true;
	fragmentions[ms_MCoH].cterminal = true;
	fragmentions[ms_MCoH].name = "[M+Co-H]+";
	fragmentions[ms_MCoH].massdifference = Co - 2*H + Hplus;
	fragmentions[ms_MCoH].parent = ms_MCoH;
	fragmentions[ms_MCoH].positive = true;
	fragmentions[ms_MCoH].multiplier = 1;
	fragmentions[ms_MCoH].numberofisotopes = 0;

	// initialize ion [M+Co-2H+Na]+
	fragmentions[ms_MCo2HNa].nterminal = true;
	fragmentions[ms_MCo2HNa].cterminal = true;
	fragmentions[ms_MCo2HNa].name = "[M+Co-2H+Na]+";
	fragmentions[ms_MCo2HNa].massdifference = Co - 2*H + Naplus;
	fragmentions[ms_MCo2HNa].parent = ms_MCo2HNa;
	fragmentions[ms_MCo2HNa].positive = true;
	fragmentions[ms_MCo2HNa].multiplier = 1;
	fragmentions[ms_MCo2HNa].numberofisotopes = 0;

	// initialize ion [M+Co-3H]-
	fragmentions[ms_MCo3H].nterminal = true;
	fragmentions[ms_MCo3H].cterminal = true;
	fragmentions[ms_MCo3H].name = "[M+Co-3H]-";
	fragmentions[ms_MCo3H].massdifference = Co - 2*H - Hplus;
	fragmentions[ms_MCo3H].parent = ms_MCo3H;
	fragmentions[ms_MCo3H].positive = false;
	fragmentions[ms_MCo3H].multiplier = 1;
	fragmentions[ms_MCo3H].numberofisotopes = 0;

	// initialize ion [M+Ni-H]+
	fragmentions[ms_MNiH].nterminal = true;
	fragmentions[ms_MNiH].cterminal = true;
	fragmentions[ms_MNiH].name = "[M+Ni-H]+";
	fragmentions[ms_MNiH].massdifference = Ni - 2*H + Hplus;
	fragmentions[ms_MNiH].parent = ms_MNiH;
	fragmentions[ms_MNiH].positive = true;
	fragmentions[ms_MNiH].multiplier = 1;
	fragmentions[ms_MNiH].numberofisotopes = 3;

	// initialize ion [M+Ni-2H+Na]+
	fragmentions[ms_MNi2HNa].nterminal = true;
	fragmentions[ms_MNi2HNa].cterminal = true;
	fragmentions[ms_MNi2HNa].name = "[M+Ni-2H+Na]+";
	fragmentions[ms_MNi2HNa].massdifference = Ni - 2*H + Naplus;
	fragmentions[ms_MNi2HNa].parent = ms_MNi2HNa;
	fragmentions[ms_MNi2HNa].positive = true;
	fragmentions[ms_MNi2HNa].multiplier = 1;
	fragmentions[ms_MNi2HNa].numberofisotopes = 3;

	// initialize ion [M+Ni-3H]-
	fragmentions[ms_MNi3H].nterminal = true;
	fragmentions[ms_MNi3H].cterminal = true;
	fragmentions[ms_MNi3H].name = "[M+Ni-3H]-";
	fragmentions[ms_MNi3H].massdifference = Ni - 2*H - Hplus;
	fragmentions[ms_MNi3H].parent = ms_MNi3H;
	fragmentions[ms_MNi3H].positive = false;
	fragmentions[ms_MNi3H].multiplier = 1;
	fragmentions[ms_MNi3H].numberofisotopes = 3;

	// initialize ion [M+Cu-H]+
	fragmentions[ms_MCuH].nterminal = true;
	fragmentions[ms_MCuH].cterminal = true;
	fragmentions[ms_MCuH].name = "[M+Cu-H]+";
	fragmentions[ms_MCuH].massdifference = Cu - 2*H + Hplus;
	fragmentions[ms_MCuH].parent = ms_MCuH;
	fragmentions[ms_MCuH].positive = true;
	fragmentions[ms_MCuH].multiplier = 1;
	fragmentions[ms_MCuH].numberofisotopes = 1;

	// initialize ion [M+Cu-2H+Na]+
	fragmentions[ms_MCu2HNa].nterminal = true;
	fragmentions[ms_MCu2HNa].cterminal = true;
	fragmentions[ms_MCu2HNa].name = "[M+Cu-2H+Na]+";
	fragmentions[ms_MCu2HNa].massdifference = Cu - 2*H + Naplus;
	fragmentions[ms_MCu2HNa].parent = ms_MCu2HNa;
	fragmentions[ms_MCu2HNa].positive = true;
	fragmentions[ms_MCu2HNa].multiplier = 1;
	fragmentions[ms_MCu2HNa].numberofisotopes = 1;

	// initialize ion [M+Cu-3H]-
	fragmentions[ms_MCu3H].nterminal = true;
	fragmentions[ms_MCu3H].cterminal = true;
	fragmentions[ms_MCu3H].name = "[M+Cu-3H]-";
	fragmentions[ms_MCu3H].massdifference = Cu - 2*H - Hplus;
	fragmentions[ms_MCu3H].parent = ms_MCu3H;
	fragmentions[ms_MCu3H].positive = false;
	fragmentions[ms_MCu3H].multiplier = 1;
	fragmentions[ms_MCu3H].numberofisotopes = 1;

	// initialize ion [M+Zn-H]+
	fragmentions[ms_MZnH].nterminal = true;
	fragmentions[ms_MZnH].cterminal = true;
	fragmentions[ms_MZnH].name = "[M+Zn-H]+";
	fragmentions[ms_MZnH].massdifference = Zn - 2*H + Hplus;
	fragmentions[ms_MZnH].parent = ms_MZnH;
	fragmentions[ms_MZnH].positive = true;
	fragmentions[ms_MZnH].multiplier = 1;
	fragmentions[ms_MZnH].numberofisotopes = 3;

	// initialize ion [M+Zn-2H+Na]+
	fragmentions[ms_MZn2HNa].nterminal = true;
	fragmentions[ms_MZn2HNa].cterminal = true;
	fragmentions[ms_MZn2HNa].name = "[M+Zn-2H+Na]+";
	fragmentions[ms_MZn2HNa].massdifference = Zn - 2*H + Naplus;
	fragmentions[ms_MZn2HNa].parent = ms_MZn2HNa;
	fragmentions[ms_MZn2HNa].positive = true;
	fragmentions[ms_MZn2HNa].multiplier = 1;
	fragmentions[ms_MZn2HNa].numberofisotopes = 3;

	// initialize ion [M+Zn-3H]-
	fragmentions[ms_MZn3H].nterminal = true;
	fragmentions[ms_MZn3H].cterminal = true;
	fragmentions[ms_MZn3H].name = "[M+Zn-3H]-";
	fragmentions[ms_MZn3H].massdifference = Zn - 2*H - Hplus;
	fragmentions[ms_MZn3H].parent = ms_MZn3H;
	fragmentions[ms_MZn3H].positive = false;
	fragmentions[ms_MZn3H].multiplier = 1;
	fragmentions[ms_MZn3H].numberofisotopes = 3;

	// initialize ion [M+Ga-2H]+
	fragmentions[ms_MGa2H].nterminal = true;
	fragmentions[ms_MGa2H].cterminal = true;
	fragmentions[ms_MGa2H].name = "[M+Ga-2H]+";
	fragmentions[ms_MGa2H].massdifference = Ga - 3*H + Hplus;
	fragmentions[ms_MGa2H].parent = ms_MGa2H;
	fragmentions[ms_MGa2H].positive = true;
	fragmentions[ms_MGa2H].multiplier = 1;
	fragmentions[ms_MGa2H].numberofisotopes = 1;

	// initialize ion [M+Ga-3H+Na]+
	fragmentions[ms_MGa3HNa].nterminal = true;
	fragmentions[ms_MGa3HNa].cterminal = true;
	fragmentions[ms_MGa3HNa].name = "[M+Ga-3H+Na]+";
	fragmentions[ms_MGa3HNa].massdifference = Ga - 3*H + Naplus;
	fragmentions[ms_MGa3HNa].parent = ms_MGa3HNa;
	fragmentions[ms_MGa3HNa].positive = true;
	fragmentions[ms_MGa3HNa].multiplier = 1;
	fragmentions[ms_MGa3HNa].numberofisotopes = 1;

	// initialize ion [M+Ga-4H]-
	fragmentions[ms_MGa4H].nterminal = true;
	fragmentions[ms_MGa4H].cterminal = true;
	fragmentions[ms_MGa4H].name = "[M+Ga-4H]-";
	fragmentions[ms_MGa4H].massdifference = Ga - 3*H - Hplus;
	fragmentions[ms_MGa4H].parent = ms_MGa4H;
	fragmentions[ms_MGa4H].positive = false;
	fragmentions[ms_MGa4H].multiplier = 1;
	fragmentions[ms_MGa4H].numberofisotopes = 1;

#if OLIGOKETIDES == 1

	// initialize l0h ion
	fragmentions[l0h_ion].nterminal = true;
	fragmentions[l0h_ion].cterminal = false;
	fragmentions[l0h_ion].name = "LB-2H";
	fragmentions[l0h_ion].massdifference = L0H_ION + adductshift;
	fragmentions[l0h_ion].parent = l0h_ion;

	// initialize l0h-H2O ion
	fragmentions[l0h_ion_dehydrated].nterminal = true;
	fragmentions[l0h_ion_dehydrated].cterminal = false;
	fragmentions[l0h_ion_dehydrated].name = "LB*-2H";
	fragmentions[l0h_ion_dehydrated].massdifference = L0H_ION - H2O + adductshift;
	fragmentions[l0h_ion_dehydrated].parent = l0h_ion;

	// initialize l0h-NH3 ion
	fragmentions[l0h_ion_deamidated].nterminal = true;
	fragmentions[l0h_ion_deamidated].cterminal = false;
	fragmentions[l0h_ion_deamidated].name = "LBx-2H";
	fragmentions[l0h_ion_deamidated].massdifference = L0H_ION - NH3 + adductshift;
	fragmentions[l0h_ion_deamidated].parent = l0h_ion;

	// initialize l0h-H2O-NH3 ion
	fragmentions[l0h_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[l0h_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[l0h_ion_dehydrated_and_deamidated].name = "LB*x-2H";
	fragmentions[l0h_ion_dehydrated_and_deamidated].massdifference = L0H_ION - H2O - NH3 + adductshift;
	fragmentions[l0h_ion_dehydrated_and_deamidated].parent = l0h_ion;

	// initialize l0h-CO ion
	fragmentions[l0h_ion_co_loss].nterminal = true;
	fragmentions[l0h_ion_co_loss].cterminal = false;
	fragmentions[l0h_ion_co_loss].name = "LB-2H-CO";
	fragmentions[l0h_ion_co_loss].massdifference = L0H_ION - CO + adductshift;
	fragmentions[l0h_ion_co_loss].parent = l0h_ion;

	// initialize l0h-CO-H2O ion
	fragmentions[l0h_ion_co_loss_dehydrated].nterminal = true;
	fragmentions[l0h_ion_co_loss_dehydrated].cterminal = false;
	fragmentions[l0h_ion_co_loss_dehydrated].name = "LB*-2H-CO";
	fragmentions[l0h_ion_co_loss_dehydrated].massdifference = L0H_ION - CO - H2O + adductshift;
	fragmentions[l0h_ion_co_loss_dehydrated].parent = l0h_ion;

	// initialize l0h-CO-NH3 ion
	fragmentions[l0h_ion_co_loss_deamidated].nterminal = true;
	fragmentions[l0h_ion_co_loss_deamidated].cterminal = false;
	fragmentions[l0h_ion_co_loss_deamidated].name = "LBx-2H-CO";
	fragmentions[l0h_ion_co_loss_deamidated].massdifference = L0H_ION - CO - NH3 + adductshift;
	fragmentions[l0h_ion_co_loss_deamidated].parent = l0h_ion;

	// initialize l0h-CO-H2O-NH3 ion
	fragmentions[l0h_ion_co_loss_dehydrated_and_deamidated].nterminal = true;
	fragmentions[l0h_ion_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[l0h_ion_co_loss_dehydrated_and_deamidated].name = "LB*x-2H-CO";
	fragmentions[l0h_ion_co_loss_dehydrated_and_deamidated].massdifference = L0H_ION - CO - H2O - NH3 + adductshift;
	fragmentions[l0h_ion_co_loss_dehydrated_and_deamidated].parent = l0h_ion;

	// initialize l1h ion
	fragmentions[l1h_ion].nterminal = true;
	fragmentions[l1h_ion].cterminal = false;
	fragmentions[l1h_ion].name = "LB";
	fragmentions[l1h_ion].massdifference = L1H_ION + adductshift;
	fragmentions[l1h_ion].parent = l1h_ion;

	// initialize l1h-H2O ion
	fragmentions[l1h_ion_dehydrated].nterminal = true;
	fragmentions[l1h_ion_dehydrated].cterminal = false;
	fragmentions[l1h_ion_dehydrated].name = "LB*";
	fragmentions[l1h_ion_dehydrated].massdifference = L1H_ION - H2O + adductshift;
	fragmentions[l1h_ion_dehydrated].parent = l1h_ion;

	// initialize l1h-NH3 ion
	fragmentions[l1h_ion_deamidated].nterminal = true;
	fragmentions[l1h_ion_deamidated].cterminal = false;
	fragmentions[l1h_ion_deamidated].name = "LBx";
	fragmentions[l1h_ion_deamidated].massdifference = L1H_ION - NH3 + adductshift;
	fragmentions[l1h_ion_deamidated].parent = l1h_ion;

	// initialize l1h-H2O-NH3 ion
	fragmentions[l1h_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[l1h_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[l1h_ion_dehydrated_and_deamidated].name = "LB*x";
	fragmentions[l1h_ion_dehydrated_and_deamidated].massdifference = L1H_ION - H2O - NH3 + adductshift;
	fragmentions[l1h_ion_dehydrated_and_deamidated].parent = l1h_ion;

	// initialize l1h-CO ion
	fragmentions[l1h_ion_co_loss].nterminal = true;
	fragmentions[l1h_ion_co_loss].cterminal = false;
	fragmentions[l1h_ion_co_loss].name = "LB-CO";
	fragmentions[l1h_ion_co_loss].massdifference = L1H_ION - CO + adductshift;
	fragmentions[l1h_ion_co_loss].parent = l1h_ion;

	// initialize l1h-CO-H2O ion
	fragmentions[l1h_ion_co_loss_dehydrated].nterminal = true;
	fragmentions[l1h_ion_co_loss_dehydrated].cterminal = false;
	fragmentions[l1h_ion_co_loss_dehydrated].name = "LB*-CO";
	fragmentions[l1h_ion_co_loss_dehydrated].massdifference = L1H_ION - CO - H2O + adductshift;
	fragmentions[l1h_ion_co_loss_dehydrated].parent = l1h_ion;

	// initialize l1h-CO-NH3 ion
	fragmentions[l1h_ion_co_loss_deamidated].nterminal = true;
	fragmentions[l1h_ion_co_loss_deamidated].cterminal = false;
	fragmentions[l1h_ion_co_loss_deamidated].name = "LBx-CO";
	fragmentions[l1h_ion_co_loss_deamidated].massdifference = L1H_ION - CO - NH3 + adductshift;
	fragmentions[l1h_ion_co_loss_deamidated].parent = l1h_ion;

	// initialize l1h-CO-H2O-NH3 ion
	fragmentions[l1h_ion_co_loss_dehydrated_and_deamidated].nterminal = true;
	fragmentions[l1h_ion_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[l1h_ion_co_loss_dehydrated_and_deamidated].name = "LB*x-CO";
	fragmentions[l1h_ion_co_loss_dehydrated_and_deamidated].massdifference = L1H_ION - CO - H2O - NH3 + adductshift;
	fragmentions[l1h_ion_co_loss_dehydrated_and_deamidated].parent = l1h_ion;

	// initialize l2h ion
	fragmentions[l2h_ion].nterminal = true;
	fragmentions[l2h_ion].cterminal = false;
	fragmentions[l2h_ion].name = "LB+2H";
	fragmentions[l2h_ion].massdifference = L2H_ION + adductshift;
	fragmentions[l2h_ion].parent = l2h_ion;

	// initialize l2h-H2O ion
	fragmentions[l2h_ion_dehydrated].nterminal = true;
	fragmentions[l2h_ion_dehydrated].cterminal = false;
	fragmentions[l2h_ion_dehydrated].name = "LB*+2H";
	fragmentions[l2h_ion_dehydrated].massdifference = L2H_ION - H2O + adductshift;
	fragmentions[l2h_ion_dehydrated].parent = l2h_ion;

	// initialize l2h-NH3 ion
	fragmentions[l2h_ion_deamidated].nterminal = true;
	fragmentions[l2h_ion_deamidated].cterminal = false;
	fragmentions[l2h_ion_deamidated].name = "LBx+2H";
	fragmentions[l2h_ion_deamidated].massdifference = L2H_ION - NH3 + adductshift;
	fragmentions[l2h_ion_deamidated].parent = l2h_ion;

	// initialize l2h-H2O-NH3 ion
	fragmentions[l2h_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[l2h_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[l2h_ion_dehydrated_and_deamidated].name = "LB*x+2H";
	fragmentions[l2h_ion_dehydrated_and_deamidated].massdifference = L2H_ION - H2O - NH3 + adductshift;
	fragmentions[l2h_ion_dehydrated_and_deamidated].parent = l2h_ion;

	// initialize l2h-CO ion
	fragmentions[l2h_ion_co_loss].nterminal = true;
	fragmentions[l2h_ion_co_loss].cterminal = false;
	fragmentions[l2h_ion_co_loss].name = "LB+2H-CO";
	fragmentions[l2h_ion_co_loss].massdifference = L2H_ION - CO + adductshift;
	fragmentions[l2h_ion_co_loss].parent = l2h_ion;

	// initialize l2h-CO-H2O ion
	fragmentions[l2h_ion_co_loss_dehydrated].nterminal = true;
	fragmentions[l2h_ion_co_loss_dehydrated].cterminal = false;
	fragmentions[l2h_ion_co_loss_dehydrated].name = "LB*+2H-CO";
	fragmentions[l2h_ion_co_loss_dehydrated].massdifference = L2H_ION - CO - H2O + adductshift;
	fragmentions[l2h_ion_co_loss_dehydrated].parent = l2h_ion;

	// initialize l2h-CO-NH3 ion
	fragmentions[l2h_ion_co_loss_deamidated].nterminal = true;
	fragmentions[l2h_ion_co_loss_deamidated].cterminal = false;
	fragmentions[l2h_ion_co_loss_deamidated].name = "LBx+2H-CO";
	fragmentions[l2h_ion_co_loss_deamidated].massdifference = L2H_ION - CO - NH3 + adductshift;
	fragmentions[l2h_ion_co_loss_deamidated].parent = l2h_ion;

	// initialize l2h-CO-H2O-NH3 ion
	fragmentions[l2h_ion_co_loss_dehydrated_and_deamidated].nterminal = true;
	fragmentions[l2h_ion_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[l2h_ion_co_loss_dehydrated_and_deamidated].name = "LB*x+2H-CO";
	fragmentions[l2h_ion_co_loss_dehydrated_and_deamidated].massdifference = L2H_ION - CO - H2O - NH3 + adductshift;
	fragmentions[l2h_ion_co_loss_dehydrated_and_deamidated].parent = l2h_ion;

	// initialize r1h ion
	fragmentions[r1h_ion].nterminal = true;
	fragmentions[r1h_ion].cterminal = false;
	fragmentions[r1h_ion].name = "RB";
	fragmentions[r1h_ion].massdifference = R1H_ION + adductshift;
	fragmentions[r1h_ion].parent = r1h_ion;

	// initialize r1h-H2O ion
	fragmentions[r1h_ion_dehydrated].nterminal = true;
	fragmentions[r1h_ion_dehydrated].cterminal = false;
	fragmentions[r1h_ion_dehydrated].name = "RB*";
	fragmentions[r1h_ion_dehydrated].massdifference = R1H_ION - H2O + adductshift;
	fragmentions[r1h_ion_dehydrated].parent = r1h_ion;

	// initialize r1h-NH3 ion
	fragmentions[r1h_ion_deamidated].nterminal = true;
	fragmentions[r1h_ion_deamidated].cterminal = false;
	fragmentions[r1h_ion_deamidated].name = "RBx";
	fragmentions[r1h_ion_deamidated].massdifference = R1H_ION - NH3 + adductshift;
	fragmentions[r1h_ion_deamidated].parent = r1h_ion;

	// initialize r1h-H2O-NH3 ion
	fragmentions[r1h_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[r1h_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[r1h_ion_dehydrated_and_deamidated].name = "RB*x";
	fragmentions[r1h_ion_dehydrated_and_deamidated].massdifference = R1H_ION - H2O - NH3 + adductshift;
	fragmentions[r1h_ion_dehydrated_and_deamidated].parent = r1h_ion;

	// initialize r1h-CO ion
	fragmentions[r1h_ion_co_loss].nterminal = true;
	fragmentions[r1h_ion_co_loss].cterminal = false;
	fragmentions[r1h_ion_co_loss].name = "RB-CO";
	fragmentions[r1h_ion_co_loss].massdifference = R1H_ION - CO + adductshift;
	fragmentions[r1h_ion_co_loss].parent = r1h_ion;

	// initialize r1h-CO-H2O ion
	fragmentions[r1h_ion_co_loss_dehydrated].nterminal = true;
	fragmentions[r1h_ion_co_loss_dehydrated].cterminal = false;
	fragmentions[r1h_ion_co_loss_dehydrated].name = "RB*-CO";
	fragmentions[r1h_ion_co_loss_dehydrated].massdifference = R1H_ION - CO - H2O + adductshift;
	fragmentions[r1h_ion_co_loss_dehydrated].parent = r1h_ion;

	// initialize r1h-CO-NH3 ion
	fragmentions[r1h_ion_co_loss_deamidated].nterminal = true;
	fragmentions[r1h_ion_co_loss_deamidated].cterminal = false;
	fragmentions[r1h_ion_co_loss_deamidated].name = "RBx-CO";
	fragmentions[r1h_ion_co_loss_deamidated].massdifference = R1H_ION - CO - NH3 + adductshift;
	fragmentions[r1h_ion_co_loss_deamidated].parent = r1h_ion;

	// initialize r1h-CO-H2O-NH3 ion
	fragmentions[r1h_ion_co_loss_dehydrated_and_deamidated].nterminal = true;
	fragmentions[r1h_ion_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[r1h_ion_co_loss_dehydrated_and_deamidated].name = "RB*x-CO";
	fragmentions[r1h_ion_co_loss_dehydrated_and_deamidated].massdifference = R1H_ION - CO - H2O - NH3 + adductshift;
	fragmentions[r1h_ion_co_loss_dehydrated_and_deamidated].parent = r1h_ion;

	// initialize r2h ion
	fragmentions[r2h_ion].nterminal = true;
	fragmentions[r2h_ion].cterminal = false;
	fragmentions[r2h_ion].name = "RB+2H";
	fragmentions[r2h_ion].massdifference = R2H_ION + adductshift;
	fragmentions[r2h_ion].parent = r2h_ion;

	// initialize r2h-H2O ion
	fragmentions[r2h_ion_dehydrated].nterminal = true;
	fragmentions[r2h_ion_dehydrated].cterminal = false;
	fragmentions[r2h_ion_dehydrated].name = "RB*+2H";
	fragmentions[r2h_ion_dehydrated].massdifference = R2H_ION - H2O + adductshift;
	fragmentions[r2h_ion_dehydrated].parent = r2h_ion;

	// initialize r2h-NH3 ion
	fragmentions[r2h_ion_deamidated].nterminal = true;
	fragmentions[r2h_ion_deamidated].cterminal = false;
	fragmentions[r2h_ion_deamidated].name = "RBx+2H";
	fragmentions[r2h_ion_deamidated].massdifference = R2H_ION - NH3 + adductshift;
	fragmentions[r2h_ion_deamidated].parent = r2h_ion;

	// initialize r2h-H2O-NH3 ion
	fragmentions[r2h_ion_dehydrated_and_deamidated].nterminal = true;
	fragmentions[r2h_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[r2h_ion_dehydrated_and_deamidated].name = "RB*x+2H";
	fragmentions[r2h_ion_dehydrated_and_deamidated].massdifference = R2H_ION - H2O - NH3 + adductshift;
	fragmentions[r2h_ion_dehydrated_and_deamidated].parent = r2h_ion;

	// initialize r2h-CO ion
	fragmentions[r2h_ion_co_loss].nterminal = true;
	fragmentions[r2h_ion_co_loss].cterminal = false;
	fragmentions[r2h_ion_co_loss].name = "RB+2H-CO";
	fragmentions[r2h_ion_co_loss].massdifference = R2H_ION - CO + adductshift;
	fragmentions[r2h_ion_co_loss].parent = r2h_ion;

	// initialize r2h-CO-H2O ion
	fragmentions[r2h_ion_co_loss_dehydrated].nterminal = true;
	fragmentions[r2h_ion_co_loss_dehydrated].cterminal = false;
	fragmentions[r2h_ion_co_loss_dehydrated].name = "RB*+2H-CO";
	fragmentions[r2h_ion_co_loss_dehydrated].massdifference = R2H_ION - CO - H2O + adductshift;
	fragmentions[r2h_ion_co_loss_dehydrated].parent = r2h_ion;

	// initialize r2h-CO-NH3 ion
	fragmentions[r2h_ion_co_loss_deamidated].nterminal = true;
	fragmentions[r2h_ion_co_loss_deamidated].cterminal = false;
	fragmentions[r2h_ion_co_loss_deamidated].name = "RBx+2H-CO";
	fragmentions[r2h_ion_co_loss_deamidated].massdifference = R2H_ION - CO - NH3 + adductshift;
	fragmentions[r2h_ion_co_loss_deamidated].parent = r2h_ion;

	// initialize r2h-CO-H2O-NH3 ion
	fragmentions[r2h_ion_co_loss_dehydrated_and_deamidated].nterminal = true;
	fragmentions[r2h_ion_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[r2h_ion_co_loss_dehydrated_and_deamidated].name = "RB*x+2H-CO";
	fragmentions[r2h_ion_co_loss_dehydrated_and_deamidated].massdifference = R2H_ION - CO - H2O - NH3 + adductshift;
	fragmentions[r2h_ion_co_loss_dehydrated_and_deamidated].parent = r2h_ion;

	// initialize l1oh ion
	fragmentions[l1oh_ion].nterminal = false;
	fragmentions[l1oh_ion].cterminal = true;
	fragmentions[l1oh_ion].name = "LY-2H";
	fragmentions[l1oh_ion].massdifference = L1OH_ION + adductshift;
	fragmentions[l1oh_ion].parent = l1oh_ion;

	// initialize l1oh-H2O ion
	fragmentions[l1oh_ion_dehydrated].nterminal = false;
	fragmentions[l1oh_ion_dehydrated].cterminal = true;
	fragmentions[l1oh_ion_dehydrated].name = "LY*-2H";
	fragmentions[l1oh_ion_dehydrated].massdifference = L1OH_ION - H2O + adductshift;
	fragmentions[l1oh_ion_dehydrated].parent = l1oh_ion;

	// initialize l1oh-NH3 ion
	fragmentions[l1oh_ion_deamidated].nterminal = false;
	fragmentions[l1oh_ion_deamidated].cterminal = true;
	fragmentions[l1oh_ion_deamidated].name = "LYx-2H";
	fragmentions[l1oh_ion_deamidated].massdifference = L1OH_ION - NH3 + adductshift;
	fragmentions[l1oh_ion_deamidated].parent = l1oh_ion;

	// initialize l1oh-H2O-NH3 ion
	fragmentions[l1oh_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[l1oh_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[l1oh_ion_dehydrated_and_deamidated].name = "LY*x-2H";
	fragmentions[l1oh_ion_dehydrated_and_deamidated].massdifference = L1OH_ION - H2O - NH3 + adductshift;
	fragmentions[l1oh_ion_dehydrated_and_deamidated].parent = l1oh_ion;

	// initialize l1oh-CO ion
	fragmentions[l1oh_ion_co_loss].nterminal = false;
	fragmentions[l1oh_ion_co_loss].cterminal = true;
	fragmentions[l1oh_ion_co_loss].name = "LY-2H-CO";
	fragmentions[l1oh_ion_co_loss].massdifference = L1OH_ION - CO + adductshift;
	fragmentions[l1oh_ion_co_loss].parent = l1oh_ion;

	// initialize l1oh-CO-H2O ion
	fragmentions[l1oh_ion_co_loss_dehydrated].nterminal = false;
	fragmentions[l1oh_ion_co_loss_dehydrated].cterminal = true;
	fragmentions[l1oh_ion_co_loss_dehydrated].name = "LY*-2H-CO";
	fragmentions[l1oh_ion_co_loss_dehydrated].massdifference = L1OH_ION - CO - H2O + adductshift;
	fragmentions[l1oh_ion_co_loss_dehydrated].parent = l1oh_ion;

	// initialize l1oh-CO-NH3 ion
	fragmentions[l1oh_ion_co_loss_deamidated].nterminal = false;
	fragmentions[l1oh_ion_co_loss_deamidated].cterminal = true;
	fragmentions[l1oh_ion_co_loss_deamidated].name = "LYx-2H-CO";
	fragmentions[l1oh_ion_co_loss_deamidated].massdifference = L1OH_ION - CO - NH3 + adductshift;
	fragmentions[l1oh_ion_co_loss_deamidated].parent = l1oh_ion;

	// initialize l1oh-CO-H2O-NH3 ion
	fragmentions[l1oh_ion_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[l1oh_ion_co_loss_dehydrated_and_deamidated].cterminal = true;
	fragmentions[l1oh_ion_co_loss_dehydrated_and_deamidated].name = "LY*x-2H-CO";
	fragmentions[l1oh_ion_co_loss_dehydrated_and_deamidated].massdifference = L1OH_ION - CO - H2O - NH3 + adductshift;
	fragmentions[l1oh_ion_co_loss_dehydrated_and_deamidated].parent = l1oh_ion;

	// initialize l2oh ion
	fragmentions[l2oh_ion].nterminal = false;
	fragmentions[l2oh_ion].cterminal = true;
	fragmentions[l2oh_ion].name = "LY";
	fragmentions[l2oh_ion].massdifference = L2OH_ION + adductshift;
	fragmentions[l2oh_ion].parent = l2oh_ion;

	// initialize l2oh-H2O ion
	fragmentions[l2oh_ion_dehydrated].nterminal = false;
	fragmentions[l2oh_ion_dehydrated].cterminal = true;
	fragmentions[l2oh_ion_dehydrated].name = "LY*";
	fragmentions[l2oh_ion_dehydrated].massdifference = L2OH_ION - H2O + adductshift;
	fragmentions[l2oh_ion_dehydrated].parent = l2oh_ion;

	// initialize l2oh-NH3 ion
	fragmentions[l2oh_ion_deamidated].nterminal = false;
	fragmentions[l2oh_ion_deamidated].cterminal = true;
	fragmentions[l2oh_ion_deamidated].name = "LYx";
	fragmentions[l2oh_ion_deamidated].massdifference = L2OH_ION - NH3 + adductshift;
	fragmentions[l2oh_ion_deamidated].parent = l2oh_ion;

	// initialize l2oh-H2O-NH3 ion
	fragmentions[l2oh_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[l2oh_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[l2oh_ion_dehydrated_and_deamidated].name = "LY*x";
	fragmentions[l2oh_ion_dehydrated_and_deamidated].massdifference = L2OH_ION - H2O - NH3 + adductshift;
	fragmentions[l2oh_ion_dehydrated_and_deamidated].parent = l2oh_ion;

	// initialize l2oh-CO ion
	fragmentions[l2oh_ion_co_loss].nterminal = false;
	fragmentions[l2oh_ion_co_loss].cterminal = true;
	fragmentions[l2oh_ion_co_loss].name = "LY-CO";
	fragmentions[l2oh_ion_co_loss].massdifference = L2OH_ION - CO + adductshift;
	fragmentions[l2oh_ion_co_loss].parent = l2oh_ion;

	// initialize l2oh-CO-H2O ion
	fragmentions[l2oh_ion_co_loss_dehydrated].nterminal = false;
	fragmentions[l2oh_ion_co_loss_dehydrated].cterminal = true;
	fragmentions[l2oh_ion_co_loss_dehydrated].name = "LY*-CO";
	fragmentions[l2oh_ion_co_loss_dehydrated].massdifference = L2OH_ION - CO - H2O + adductshift;
	fragmentions[l2oh_ion_co_loss_dehydrated].parent = l2oh_ion;

	// initialize l2oh-CO-NH3 ion
	fragmentions[l2oh_ion_co_loss_deamidated].nterminal = false;
	fragmentions[l2oh_ion_co_loss_deamidated].cterminal = true;
	fragmentions[l2oh_ion_co_loss_deamidated].name = "LYx-CO";
	fragmentions[l2oh_ion_co_loss_deamidated].massdifference = L2OH_ION - CO - NH3 + adductshift;
	fragmentions[l2oh_ion_co_loss_deamidated].parent = l2oh_ion;

	// initialize l2oh-CO-H2O-NH3 ion
	fragmentions[l2oh_ion_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[l2oh_ion_co_loss_dehydrated_and_deamidated].cterminal = true;
	fragmentions[l2oh_ion_co_loss_dehydrated_and_deamidated].name = "LY*x-CO";
	fragmentions[l2oh_ion_co_loss_dehydrated_and_deamidated].massdifference = L2OH_ION - CO - H2O - NH3 + adductshift;
	fragmentions[l2oh_ion_co_loss_dehydrated_and_deamidated].parent = l2oh_ion;

	// initialize r1oh ion
	fragmentions[r1oh_ion].nterminal = false;
	fragmentions[r1oh_ion].cterminal = true;
	fragmentions[r1oh_ion].name = "RY-2H";
	fragmentions[r1oh_ion].massdifference = R1OH_ION + adductshift;
	fragmentions[r1oh_ion].parent = r1oh_ion;

	// initialize r1oh-H2O ion
	fragmentions[r1oh_ion_dehydrated].nterminal = false;
	fragmentions[r1oh_ion_dehydrated].cterminal = true;
	fragmentions[r1oh_ion_dehydrated].name = "RY*-2H";
	fragmentions[r1oh_ion_dehydrated].massdifference = R1OH_ION - H2O + adductshift;
	fragmentions[r1oh_ion_dehydrated].parent = r1oh_ion;

	// initialize r1oh-NH3 ion
	fragmentions[r1oh_ion_deamidated].nterminal = false;
	fragmentions[r1oh_ion_deamidated].cterminal = true;
	fragmentions[r1oh_ion_deamidated].name = "RYx-2H";
	fragmentions[r1oh_ion_deamidated].massdifference = R1OH_ION - NH3 + adductshift;
	fragmentions[r1oh_ion_deamidated].parent = r1oh_ion;

	// initialize r1oh-H2O-NH3 ion
	fragmentions[r1oh_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[r1oh_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[r1oh_ion_dehydrated_and_deamidated].name = "RY*x-2H";
	fragmentions[r1oh_ion_dehydrated_and_deamidated].massdifference = R1OH_ION - H2O - NH3 + adductshift;
	fragmentions[r1oh_ion_dehydrated_and_deamidated].parent = r1oh_ion;

	// initialize r1oh-CO ion
	fragmentions[r1oh_ion_co_loss].nterminal = false;
	fragmentions[r1oh_ion_co_loss].cterminal = true;
	fragmentions[r1oh_ion_co_loss].name = "RY-2H-CO";
	fragmentions[r1oh_ion_co_loss].massdifference = R1OH_ION - CO + adductshift;
	fragmentions[r1oh_ion_co_loss].parent = r1oh_ion;

	// initialize r1oh-CO-H2O ion
	fragmentions[r1oh_ion_co_loss_dehydrated].nterminal = false;
	fragmentions[r1oh_ion_co_loss_dehydrated].cterminal = true;
	fragmentions[r1oh_ion_co_loss_dehydrated].name = "RY*-2H-CO";
	fragmentions[r1oh_ion_co_loss_dehydrated].massdifference = R1OH_ION - CO - H2O + adductshift;
	fragmentions[r1oh_ion_co_loss_dehydrated].parent = r1oh_ion;

	// initialize r1oh-CO-NH3 ion
	fragmentions[r1oh_ion_co_loss_deamidated].nterminal = false;
	fragmentions[r1oh_ion_co_loss_deamidated].cterminal = true;
	fragmentions[r1oh_ion_co_loss_deamidated].name = "RYx-2H-CO";
	fragmentions[r1oh_ion_co_loss_deamidated].massdifference = R1OH_ION - CO - NH3 + adductshift;
	fragmentions[r1oh_ion_co_loss_deamidated].parent = r1oh_ion;

	// initialize r1oh-CO-H2O-NH3 ion
	fragmentions[r1oh_ion_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[r1oh_ion_co_loss_dehydrated_and_deamidated].cterminal = true;
	fragmentions[r1oh_ion_co_loss_dehydrated_and_deamidated].name = "RY*x-2H-CO";
	fragmentions[r1oh_ion_co_loss_dehydrated_and_deamidated].massdifference = R1OH_ION - CO - H2O - NH3 + adductshift;
	fragmentions[r1oh_ion_co_loss_dehydrated_and_deamidated].parent = r1oh_ion;

	// initialize r2oh ion
	fragmentions[r2oh_ion].nterminal = false;
	fragmentions[r2oh_ion].cterminal = true;
	fragmentions[r2oh_ion].name = "RY";
	fragmentions[r2oh_ion].massdifference = R2OH_ION + adductshift;
	fragmentions[r2oh_ion].parent = r2oh_ion;

	// initialize r2oh-H2O ion
	fragmentions[r2oh_ion_dehydrated].nterminal = false;
	fragmentions[r2oh_ion_dehydrated].cterminal = true;
	fragmentions[r2oh_ion_dehydrated].name = "RY*";
	fragmentions[r2oh_ion_dehydrated].massdifference = R2OH_ION - H2O + adductshift;
	fragmentions[r2oh_ion_dehydrated].parent = r2oh_ion;

	// initialize r2oh-NH3 ion
	fragmentions[r2oh_ion_deamidated].nterminal = false;
	fragmentions[r2oh_ion_deamidated].cterminal = true;
	fragmentions[r2oh_ion_deamidated].name = "RYx";
	fragmentions[r2oh_ion_deamidated].massdifference = R2OH_ION - NH3 + adductshift;
	fragmentions[r2oh_ion_deamidated].parent = r2oh_ion;

	// initialize r2oh-H2O-NH3 ion
	fragmentions[r2oh_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[r2oh_ion_dehydrated_and_deamidated].cterminal = true;
	fragmentions[r2oh_ion_dehydrated_and_deamidated].name = "RY*x";
	fragmentions[r2oh_ion_dehydrated_and_deamidated].massdifference = R2OH_ION - H2O - NH3 + adductshift;
	fragmentions[r2oh_ion_dehydrated_and_deamidated].parent = r2oh_ion;

	// initialize r2oh-CO ion
	fragmentions[r2oh_ion_co_loss].nterminal = false;
	fragmentions[r2oh_ion_co_loss].cterminal = true;
	fragmentions[r2oh_ion_co_loss].name = "RY-CO";
	fragmentions[r2oh_ion_co_loss].massdifference = R2OH_ION - CO + adductshift;
	fragmentions[r2oh_ion_co_loss].parent = r2oh_ion;

	// initialize r2oh-CO-H2O ion
	fragmentions[r2oh_ion_co_loss_dehydrated].nterminal = false;
	fragmentions[r2oh_ion_co_loss_dehydrated].cterminal = true;
	fragmentions[r2oh_ion_co_loss_dehydrated].name = "RY*-CO";
	fragmentions[r2oh_ion_co_loss_dehydrated].massdifference = R2OH_ION - CO - H2O + adductshift;
	fragmentions[r2oh_ion_co_loss_dehydrated].parent = r2oh_ion;

	// initialize r2oh-CO-NH3 ion
	fragmentions[r2oh_ion_co_loss_deamidated].nterminal = false;
	fragmentions[r2oh_ion_co_loss_deamidated].cterminal = true;
	fragmentions[r2oh_ion_co_loss_deamidated].name = "RYx-CO";
	fragmentions[r2oh_ion_co_loss_deamidated].massdifference = R2OH_ION - CO - NH3 + adductshift;
	fragmentions[r2oh_ion_co_loss_deamidated].parent = r2oh_ion;

	// initialize r2oh-CO-H2O-NH3 ion
	fragmentions[r2oh_ion_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[r2oh_ion_co_loss_dehydrated_and_deamidated].cterminal = true;
	fragmentions[r2oh_ion_co_loss_dehydrated_and_deamidated].name = "RY*x-CO";
	fragmentions[r2oh_ion_co_loss_dehydrated_and_deamidated].massdifference = R2OH_ION - CO - H2O - NH3 + adductshift;
	fragmentions[r2oh_ion_co_loss_dehydrated_and_deamidated].parent = r2oh_ion;

	// initialize linear oligoketide precursor ion (H-...-H)
	fragmentions[linear_oligoketide_precursor_ion_h_h].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h].name = "[M+zH]+ (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h].parent = linear_oligoketide_precursor_ion_h_h;

	// initialize linear oligoketide precursor ion (H-...-H) - H2O
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated].name = "[M+zH]+ * (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - H2O + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated].parent = linear_oligoketide_precursor_ion_h_h;

	// initialize linear oligoketide precursor ion (H-...-H) - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_h_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_deamidated].name = "[M+zH]+ x (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_deamidated].parent = linear_oligoketide_precursor_ion_h_h;

	// initialize linear oligoketide precursor ion (H-...-H) - H2O - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated_and_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated_and_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated_and_deamidated].name = "[M+zH]+ *x (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - H2O - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_dehydrated_and_deamidated].parent = linear_oligoketide_precursor_ion_h_h;

	// initialize linear oligoketide precursor ion (H-...-H) - CO
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss].name = "[M+zH]+ -CO (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - CO + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss].parent = linear_oligoketide_precursor_ion_h_h_co_loss;

	// initialize linear oligoketide precursor ion (H-...-H) - CO - H2O
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated].name = "[M+zH]+ *-CO (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - CO - H2O + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated].parent = linear_oligoketide_precursor_ion_h_h_co_loss;

	// initialize linear oligoketide precursor ion (H-...-H) - CO - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_deamidated].name = "[M+zH]+ x-CO (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - CO - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_deamidated].parent = linear_oligoketide_precursor_ion_h_h_co_loss;

	// initialize linear oligoketide precursor ion (H-...-H) - CO - H2O - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated].name = "[M+zH]+ *x-CO (H-...-H)";
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_H - CO - H2O - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated].parent = linear_oligoketide_precursor_ion_h_h_co_loss;

	// initialize linear oligoketide precursor ion (H-...-OH)
	fragmentions[linear_oligoketide_precursor_ion_h_oh].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh].name = "[M+zH]+ (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh].parent = linear_oligoketide_precursor_ion_h_oh;

	// initialize linear oligoketide precursor ion (H-...-OH) - H2O
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated].name = "[M+zH]+ * (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - H2O + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated].parent = linear_oligoketide_precursor_ion_h_oh;

	// initialize linear oligoketide precursor ion (H-...-OH) - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_oh_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_deamidated].name = "[M+zH]+ x (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_deamidated].parent = linear_oligoketide_precursor_ion_h_oh;

	// initialize linear oligoketide precursor ion (H-...-OH) - H2O - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated_and_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated_and_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated_and_deamidated].name = "[M+zH]+ *x (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - H2O - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_dehydrated_and_deamidated].parent = linear_oligoketide_precursor_ion_h_oh;

	// initialize linear oligoketide precursor ion (H-...-OH) - CO
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss].name = "[M+zH]+ -CO (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - CO + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss].parent = linear_oligoketide_precursor_ion_h_oh_co_loss;

	// initialize linear oligoketide precursor ion (H-...-OH) - CO - H2O
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated].name = "[M+zH]+ *-CO (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - CO - H2O + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated].parent = linear_oligoketide_precursor_ion_h_oh_co_loss;

	// initialize linear oligoketide precursor ion (H-...-OH) - CO - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_deamidated].name = "[M+zH]+ x-CO (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - CO - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_deamidated].parent = linear_oligoketide_precursor_ion_h_oh_co_loss;

	// initialize linear oligoketide precursor ion (H-...-OH) - CO - H2O - NH3
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated].name = "[M+zH]+ *x-CO (H-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_H_OH - CO - H2O - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated].parent = linear_oligoketide_precursor_ion_h_oh_co_loss;

	// initialize linear oligoketide precursor ion (HO-...-OH)
	fragmentions[linear_oligoketide_precursor_ion_oh_oh].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh].name = "[M+zH]+ (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh].parent = linear_oligoketide_precursor_ion_oh_oh;

	// initialize linear oligoketide precursor ion (HO-...-OH) - H2O
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated].name = "[M+zH]+ * (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - H2O + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated].parent = linear_oligoketide_precursor_ion_oh_oh;

	// initialize linear oligoketide precursor ion (HO-...-OH) - NH3
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_deamidated].name = "[M+zH]+ x (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_deamidated].parent = linear_oligoketide_precursor_ion_oh_oh;

	// initialize linear oligoketide precursor ion (HO-...-OH) - H2O - NH3
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated_and_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated_and_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated_and_deamidated].name = "[M+zH]+ *x (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - H2O - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_dehydrated_and_deamidated].parent = linear_oligoketide_precursor_ion_oh_oh;

	// initialize linear oligoketide precursor ion (HO-...-OH) - CO
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss].name = "[M+zH]+ -CO (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - CO + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss].parent = linear_oligoketide_precursor_ion_oh_oh_co_loss;

	// initialize linear oligoketide precursor ion (HO-...-OH) - CO - H2O
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated].name = "[M+zH]+ *-CO (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - CO - H2O + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated].parent = linear_oligoketide_precursor_ion_oh_oh_co_loss;

	// initialize linear oligoketide precursor ion (HO-...-OH) - CO - NH3
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_deamidated].name = "[M+zH]+ x-CO (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - CO - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_deamidated].parent = linear_oligoketide_precursor_ion_oh_oh_co_loss;

	// initialize linear oligoketide precursor ion (HO-...-OH) - CO - H2O - NH3
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated].name = "[M+zH]+ *x-CO (HO-...-OH)";
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated].massdifference = PRECURSOR_ION_LINEAR_OLIGOKETIDE_OH_OH - CO - H2O - NH3 + adductshift;
	fragmentions[linear_oligoketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated].parent = linear_oligoketide_precursor_ion_oh_oh_co_loss;

	// initialize cyclic oligoketide precursor ion
	fragmentions[cyclic_oligoketide_precursor_ion].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion].name = "[M+zH]+";
	fragmentions[cyclic_oligoketide_precursor_ion].massdifference = Hplus + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion].parent = cyclic_oligoketide_precursor_ion;

	// initialize cyclic oligoketide precursor ion - H2O
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated].name = "[M+zH]+ *";
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated].massdifference = Hplus - H2O + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated].parent = cyclic_oligoketide_precursor_ion;

	// initialize cyclic oligoketide precursor ion - NH3
	fragmentions[cyclic_oligoketide_precursor_ion_deamidated].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_deamidated].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_deamidated].name = "[M+zH]+ x";
	fragmentions[cyclic_oligoketide_precursor_ion_deamidated].massdifference = Hplus - NH3 + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_deamidated].parent = cyclic_oligoketide_precursor_ion;

	// initialize cyclic oligoketide precursor ion - H2O - NH3
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated_and_deamidated].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated_and_deamidated].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated_and_deamidated].name = "[M+zH]+ *x";
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated_and_deamidated].massdifference = Hplus - H2O - NH3 + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_dehydrated_and_deamidated].parent = cyclic_oligoketide_precursor_ion;

	// initialize cyclic oligoketide precursor ion - CO
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss].name = "[M+zH]+ -CO";
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss].massdifference = Hplus - CO + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss].parent = cyclic_oligoketide_precursor_ion_co_loss;

	// initialize cyclic oligoketide precursor ion - CO - H2O
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated].name = "[M+zH]+ *-CO";
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated].massdifference = Hplus - CO - H2O + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated].parent = cyclic_oligoketide_precursor_ion_co_loss;

	// initialize cyclic oligoketide precursor ion - CO - NH3
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_deamidated].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_deamidated].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_deamidated].name = "[M+zH]+ x-CO";
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_deamidated].massdifference = Hplus - CO - NH3 + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_deamidated].parent = cyclic_oligoketide_precursor_ion_co_loss;

	// initialize cyclic oligoketide precursor ion - CO - H2O - NH3
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated_and_deamidated].nterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated_and_deamidated].cterminal = false;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated_and_deamidated].name = "[M+zH]+ *x-CO";
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated_and_deamidated].massdifference = Hplus - CO - H2O - NH3 + adductshift;
	fragmentions[cyclic_oligoketide_precursor_ion_co_loss_dehydrated_and_deamidated].parent = cyclic_oligoketide_precursor_ion_co_loss;
#endif

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
	for (map<eFragmentIonType,fragmentDescription>::iterator it = fragmentions.begin(); it != fragmentions.end(); ++it) {
		os.write((char *)&it->first, sizeof(eFragmentIonType));
		it->second.store(os);
	}
}


void cFragmentIons::load(ifstream& is) {
	int size;
	eFragmentIonType iontype;
	fragmentDescription description;

	is.read((char *)&size, sizeof(int));
	fragmentions.clear();
	for (int i = 0; i < size; i++) {
		is.read((char *)&iontype, sizeof(eFragmentIonType));
		description.load(is);
		fragmentions[iontype] = description;
	}
}


void initializeFragmentIonsForDeNovoGraphOfCyclicPeptides(vector<eFragmentIonType>& fragmentions) {
	fragmentions.push_back(b_ion);
}


void initializeFragmentIonsForDeNovoGraphOfLinearPeptides(vector<eFragmentIonType>& fragmentions) {
	fragmentions.push_back(y_ion);
	fragmentions.push_back(b_ion);
	fragmentions.push_back(precursor_ion);
}


void initializeFragmentIonsForDeNovoGraphOfTPeptides(vector<eFragmentIonType>& fragmentions) {
	fragmentions.push_back(y_ion);
	fragmentions.push_back(b_ion);
	fragmentions.push_back(precursor_ion);
}


void initializeFragmentIonsForDeNovoGraphOfBranchCyclicPeptides(vector<eFragmentIonType>& fragmentions) {
	fragmentions.push_back(b_ion);
	//fragmentions.push_back(y_ion);
}


#if OLIGOKETIDES == 1


void initializeFragmentIonsForDeNovoGraphOfLinearOligoketide(vector<eFragmentIonType>& fragmentions) {
	fragmentions.push_back(l1h_ion);
	fragmentions.push_back(l2h_ion);
	fragmentions.push_back(l1oh_ion);
	fragmentions.push_back(l2oh_ion);
	fragmentions.push_back(linear_oligoketide_precursor_ion_h_h);
	fragmentions.push_back(linear_oligoketide_precursor_ion_h_oh);
	fragmentions.push_back(linear_oligoketide_precursor_ion_oh_oh);
}


void initializeFragmentIonsForDeNovoGraphOfCyclicOligoketide(vector<eFragmentIonType>& fragmentions) {
	//fragmentions.push_back(l0h_ion);
	fragmentions.push_back(l1h_ion);
	fragmentions.push_back(l2h_ion);
	fragmentions.push_back(cyclic_oligoketide_precursor_ion);
}


#endif


void initializeFragmentIonsForDeNovoGraphOfLinearPolysaccharide(vector<eFragmentIonType>& fragmentions) {
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
		#if OS_TYPE != WIN
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

