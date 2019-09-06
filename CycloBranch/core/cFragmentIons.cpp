#include "core/cFragmentIons.h"
#include "core/cSummaryFormula.h"


cPeriodicTableMap periodictablemap;
cIsotopeMap isotopemap;
cDefaultNeutralLosses defaultneutrallosses;


cPeriodicTableMap::cPeriodicTableMap() {
	table["+"] = -e;
	table["H"] = H;
	table["D"] = D;
	table["T"] = T;
	table["He"] = He;
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

	for (auto it = table.begin(); it != table.end(); ++it) {
		if ((it->first.compare("+") == 0) || 
			(it->first.compare("H") == 0) || 
			(it->first.compare("D") == 0) || 
			(it->first.compare("T") == 0) || 
			(it->first.compare("C") == 0) || 
			(it->first.compare("N") == 0) || 
			(it->first.compare("O") == 0) || 
			(it->first.compare("P") == 0) || 
			(it->first.compare("S") == 0) || 
			(it->first.compare("R") == 0) || 
			(it->first.compare("X") == 0)) {
			continue;
		}
		adductElements.push_back(it->first);
	}
}


double &cPeriodicTableMap::operator[](string& element) {
	return table[element];
}


int cPeriodicTableMap::count(string& element) {
	return (int)table.count(element);
}


map<string, double>& cPeriodicTableMap::getElements() {
	return table;
}


vector<string>& cPeriodicTableMap::getAdductElements() {
	return adductElements;
}


cIsotopeMap::cIsotopeMap() {
	isotope is;
	map<string, isotope> ismap;

	ismap.clear();
	is.mass = -e;
	is.probability = 1;
	ismap["+"] = is;
	table["+"] = ismap;

	ismap.clear();
	is.mass = H1;
	is.probability = Pr_H1;
	ismap["H"] = is;
	is.mass = H2;
	is.probability = Pr_H2;
	ismap["2H"] = is;
	table["H"] = ismap;

	ismap.clear();
	is.mass = D;
	is.probability = 1;
	ismap["D"] = is;
	table["D"] = ismap;

	ismap.clear();
	is.mass = T;
	is.probability = 1;
	ismap["T"] = is;
	table["T"] = ismap;

	ismap.clear();
	is.mass = He3;
	is.probability = Pr_He3;
	ismap["3He"] = is;
	is.mass = He4;
	is.probability = Pr_He4;
	ismap["He"] = is;
	table["He"] = ismap;

	ismap.clear();
	is.mass = Li6;
	is.probability = Pr_Li6;
	ismap["6Li"] = is;
	is.mass = Li7;
	is.probability = Pr_Li7;
	ismap["Li"] = is;
	table["Li"] = ismap;

	ismap.clear();
	is.mass = Be9;
	is.probability = Pr_Be9;
	ismap["Be"] = is;
	table["Be"] = ismap;

	ismap.clear();
	is.mass = B10;
	is.probability = Pr_B10;
	ismap["10B"] = is;
	is.mass = B11;
	is.probability = Pr_B11;
	ismap["B"] = is;
	table["B"] = ismap;

	ismap.clear();
	is.mass = C12;
	is.probability = Pr_C12;
	ismap["C"] = is;
	is.mass = C13;
	is.probability = Pr_C13;
	ismap["13C"] = is;
	table["C"] = ismap;

	ismap.clear();
	is.mass = N14;
	is.probability = Pr_N14;
	ismap["N"] = is;
	is.mass = N15;
	is.probability = Pr_N15;
	ismap["15N"] = is;
	table["N"] = ismap;

	ismap.clear();
	is.mass = O16;
	is.probability = Pr_O16;
	ismap["O"] = is;
	is.mass = O17;
	is.probability = Pr_O17;
	ismap["17O"] = is;
	is.mass = O18;
	is.probability = Pr_O18;
	ismap["18O"] = is;
	table["O"] = ismap;

	ismap.clear();
	is.mass = F19;
	is.probability = Pr_F19;
	ismap["F"] = is;
	table["F"] = ismap;

	ismap.clear();
	is.mass = Ne20;
	is.probability = Pr_Ne20;
	ismap["Ne"] = is;
	is.mass = Ne21;
	is.probability = Pr_Ne21;
	ismap["21Ne"] = is;
	is.mass = Ne22;
	is.probability = Pr_Ne22;
	ismap["22Ne"] = is;
	table["Ne"] = ismap;

	ismap.clear();
	is.mass = Na23;
	is.probability = Pr_Na23;
	ismap["Na"] = is;
	table["Na"] = ismap;

	ismap.clear();
	is.mass = Mg24;
	is.probability = Pr_Mg24;
	ismap["Mg"] = is;
	is.mass = Mg25;
	is.probability = Pr_Mg25;
	ismap["25Mg"] = is;
	is.mass = Mg26;
	is.probability = Pr_Mg26;
	ismap["26Mg"] = is;
	table["Mg"] = ismap;

	ismap.clear();
	is.mass = Al27;
	is.probability = Pr_Al27;
	ismap["Al"] = is;
	table["Al"] = ismap;

	ismap.clear();
	is.mass = Si28;
	is.probability = Pr_Si28;
	ismap["Si"] = is;
	is.mass = Si29;
	is.probability = Pr_Si29;
	ismap["29Si"] = is;
	is.mass = Si30;
	is.probability = Pr_Si30;
	ismap["30Si"] = is;
	table["Si"] = ismap;

	ismap.clear();
	is.mass = P31;
	is.probability = Pr_P31;
	ismap["P"] = is;
	table["P"] = ismap;

	ismap.clear();
	is.mass = S32;
	is.probability = Pr_S32;
	ismap["S"] = is;
	is.mass = S33;
	is.probability = Pr_S33;
	ismap["33S"] = is;
	is.mass = S34;
	is.probability = Pr_S34;
	ismap["34S"] = is;
	is.mass = S36;
	is.probability = Pr_S36;
	ismap["36S"] = is;
	table["S"] = ismap;

	ismap.clear();
	is.mass = Cl35;
	is.probability = Pr_Cl35;
	ismap["Cl"] = is;
	is.mass = Cl37;
	is.probability = Pr_Cl37;
	ismap["37Cl"] = is;
	table["Cl"] = ismap;

	ismap.clear();
	is.mass = Ar36;
	is.probability = Pr_Ar36;
	ismap["36Ar"] = is;
	is.mass = Ar38;
	is.probability = Pr_Ar38;
	ismap["38Ar"] = is;
	is.mass = Ar40;
	is.probability = Pr_Ar40;
	ismap["Ar"] = is;
	table["Ar"] = ismap;

	ismap.clear();
	is.mass = K39;
	is.probability = Pr_K39;
	ismap["K"] = is;
	is.mass = K40;
	is.probability = Pr_K40;
	ismap["40K"] = is;
	is.mass = K41;
	is.probability = Pr_K41;
	ismap["41K"] = is;
	table["K"] = ismap;

	ismap.clear();
	is.mass = Ca40;
	is.probability = Pr_Ca40;
	ismap["Ca"] = is;
	is.mass = Ca42;
	is.probability = Pr_Ca42;
	ismap["42Ca"] = is;
	is.mass = Ca43;
	is.probability = Pr_Ca43;
	ismap["43Ca"] = is;
	is.mass = Ca44;
	is.probability = Pr_Ca44;
	ismap["44Ca"] = is;
	is.mass = Ca46;
	is.probability = Pr_Ca46;
	ismap["46Ca"] = is;
	is.mass = Ca48;
	is.probability = Pr_Ca48;
	ismap["48Ca"] = is;
	table["Ca"] = ismap;

	ismap.clear();
	is.mass = Sc45;
	is.probability = Pr_Sc45;
	ismap["Sc"] = is;
	table["Sc"] = ismap;

	ismap.clear();
	is.mass = Ti46;
	is.probability = Pr_Ti46;
	ismap["46Ti"] = is;
	is.mass = Ti47;
	is.probability = Pr_Ti47;
	ismap["47Ti"] = is;
	is.mass = Ti48;
	is.probability = Pr_Ti48;
	ismap["Ti"] = is;
	is.mass = Ti49;
	is.probability = Pr_Ti49;
	ismap["49Ti"] = is;
	is.mass = Ti50;
	is.probability = Pr_Ti50;
	ismap["50Ti"] = is;
	table["Ti"] = ismap;

	ismap.clear();
	is.mass = V50;
	is.probability = Pr_V50;
	ismap["50V"] = is;
	is.mass = V51;
	is.probability = Pr_V51;
	ismap["V"] = is;
	table["V"] = ismap;

	ismap.clear();
	is.mass = Cr50;
	is.probability = Pr_Cr50;
	ismap["50Cr"] = is;
	is.mass = Cr52;
	is.probability = Pr_Cr52;
	ismap["Cr"] = is;
	is.mass = Cr53;
	is.probability = Pr_Cr53;
	ismap["53Cr"] = is;
	is.mass = Cr54;
	is.probability = Pr_Cr54;
	ismap["54Cr"] = is;
	table["Cr"] = ismap;

	ismap.clear();
	is.mass = Mn55;
	is.probability = Pr_Mn55;
	ismap["Mn"] = is;
	table["Mn"] = ismap;

	ismap.clear();
	is.mass = Fe54;
	is.probability = Pr_Fe54;
	ismap["54Fe"] = is;
	is.mass = Fe56;
	is.probability = Pr_Fe56;
	ismap["Fe"] = is;
	is.mass = Fe57;
	is.probability = Pr_Fe57;
	ismap["57Fe"] = is;
	is.mass = Fe58;
	is.probability = Pr_Fe58;
	ismap["58Fe"] = is;
	table["Fe"] = ismap;

	ismap.clear();
	is.mass = Co59;
	is.probability = Pr_Co59;
	ismap["Co"] = is;
	table["Co"] = ismap;

	ismap.clear();
	is.mass = Ni58;
	is.probability = Pr_Ni58;
	ismap["Ni"] = is;
	is.mass = Ni60;
	is.probability = Pr_Ni60;
	ismap["60Ni"] = is;
	is.mass = Ni61;
	is.probability = Pr_Ni61;
	ismap["61Ni"] = is;
	is.mass = Ni62;
	is.probability = Pr_Ni62;
	ismap["62Ni"] = is;
	is.mass = Ni64;
	is.probability = Pr_Ni64;
	ismap["64Ni"] = is;
	table["Ni"] = ismap;

	ismap.clear();
	is.mass = Cu63;
	is.probability = Pr_Cu63;
	ismap["Cu"] = is;
	is.mass = Cu65;
	is.probability = Pr_Cu65;
	ismap["65Cu"] = is;
	table["Cu"] = ismap;

	ismap.clear();
	is.mass = Zn64;
	is.probability = Pr_Zn64;
	ismap["Zn"] = is;
	is.mass = Zn66;
	is.probability = Pr_Zn66;
	ismap["66Zn"] = is;
	is.mass = Zn67;
	is.probability = Pr_Zn67;
	ismap["67Zn"] = is;
	is.mass = Zn68;
	is.probability = Pr_Zn68;
	ismap["68Zn"] = is;
	is.mass = Zn70;
	is.probability = Pr_Zn70;
	ismap["70Zn"] = is;
	table["Zn"] = ismap;

	ismap.clear();
	is.mass = Ga69;
	is.probability = Pr_Ga69;
	ismap["Ga"] = is;
	is.mass = Ga71;
	is.probability = Pr_Ga71;
	ismap["71Ga"] = is;
	table["Ga"] = ismap;

	ismap.clear();
	is.mass = Ge70;
	is.probability = Pr_Ge70;
	ismap["70Ge"] = is;
	is.mass = Ge72;
	is.probability = Pr_Ge72;
	ismap["72Ge"] = is;
	is.mass = Ge73;
	is.probability = Pr_Ge73;
	ismap["73Ge"] = is;
	is.mass = Ge74;
	is.probability = Pr_Ge74;
	ismap["Ge"] = is;
	is.mass = Ge76;
	is.probability = Pr_Ge76;
	ismap["76Ge"] = is;
	table["Ge"] = ismap;

	ismap.clear();
	is.mass = As75;
	is.probability = Pr_As75;
	ismap["As"] = is;
	table["As"] = ismap;

	ismap.clear();
	is.mass = Se74;
	is.probability = Pr_Se74;
	ismap["74Se"] = is;
	is.mass = Se76;
	is.probability = Pr_Se76;
	ismap["76Se"] = is;
	is.mass = Se77;
	is.probability = Pr_Se77;
	ismap["77Se"] = is;
	is.mass = Se78;
	is.probability = Pr_Se78;
	ismap["78Se"] = is;
	is.mass = Se80;
	is.probability = Pr_Se80;
	ismap["Se"] = is;
	is.mass = Se82;
	is.probability = Pr_Se82;
	ismap["82Se"] = is;
	table["Se"] = ismap;

	ismap.clear();
	is.mass = Br79;
	is.probability = Pr_Br79;
	ismap["Br"] = is;
	is.mass = Br81;
	is.probability = Pr_Br81;
	ismap["81Br"] = is;
	table["Br"] = ismap;

	ismap.clear();
	is.mass = Kr78;
	is.probability = Pr_Kr78;
	ismap["78Kr"] = is;
	is.mass = Kr80;
	is.probability = Pr_Kr80;
	ismap["80Kr"] = is;
	is.mass = Kr82;
	is.probability = Pr_Kr82;
	ismap["82Kr"] = is;
	is.mass = Kr83;
	is.probability = Pr_Kr83;
	ismap["83Kr"] = is;
	is.mass = Kr84;
	is.probability = Pr_Kr84;
	ismap["Kr"] = is;
	is.mass = Kr86;
	is.probability = Pr_Kr86;
	ismap["86Kr"] = is;
	table["Kr"] = ismap;

	ismap.clear();
	is.mass = Rb85;
	is.probability = Pr_Rb85;
	ismap["Rb"] = is;
	is.mass = Rb87;
	is.probability = Pr_Rb87;
	ismap["87Rb"] = is;
	table["Rb"] = ismap;

	ismap.clear();
	is.mass = Sr84;
	is.probability = Pr_Sr84;
	ismap["84Sr"] = is;
	is.mass = Sr86;
	is.probability = Pr_Sr86;
	ismap["86Sr"] = is;
	is.mass = Sr87;
	is.probability = Pr_Sr87;
	ismap["87Sr"] = is;
	is.mass = Sr88;
	is.probability = Pr_Sr88;
	ismap["Sr"] = is;
	table["Sr"] = ismap;

	ismap.clear();
	is.mass = Y89;
	is.probability = Pr_Y89;
	ismap["Y"] = is;
	table["Y"] = ismap;

	ismap.clear();
	is.mass = Zr90;
	is.probability = Pr_Zr90;
	ismap["Zr"] = is;
	is.mass = Zr91;
	is.probability = Pr_Zr91;
	ismap["91Zr"] = is;
	is.mass = Zr92;
	is.probability = Pr_Zr92;
	ismap["92Zr"] = is;
	is.mass = Zr94;
	is.probability = Pr_Zr94;
	ismap["94Zr"] = is;
	is.mass = Zr96;
	is.probability = Pr_Zr96;
	ismap["96Zr"] = is;
	table["Zr"] = ismap;

	ismap.clear();
	is.mass = Nb93;
	is.probability = Pr_Nb93;
	ismap["Nb"] = is;
	table["Nb"] = ismap;

	ismap.clear();
	is.mass = Mo92;
	is.probability = Pr_Mo92;
	ismap["92Mo"] = is;
	is.mass = Mo94;
	is.probability = Pr_Mo94;
	ismap["94Mo"] = is;
	is.mass = Mo95;
	is.probability = Pr_Mo95;
	ismap["95Mo"] = is;
	is.mass = Mo96;
	is.probability = Pr_Mo96;
	ismap["96Mo"] = is;
	is.mass = Mo97;
	is.probability = Pr_Mo97;
	ismap["97Mo"] = is;
	is.mass = Mo98;
	is.probability = Pr_Mo98;
	ismap["Mo"] = is;
	is.mass = Mo100;
	is.probability = Pr_Mo100;
	ismap["100Mo"] = is;
	table["Mo"] = ismap;

	ismap.clear();
	is.mass = Tc98;
	is.probability = Pr_Tc98;
	ismap["Tc"] = is;
	table["Tc"] = ismap;

	ismap.clear();
	is.mass = Ru96;
	is.probability = Pr_Ru96;
	ismap["96Ru"] = is;
	is.mass = Ru98;
	is.probability = Pr_Ru98;
	ismap["98Ru"] = is;
	is.mass = Ru99;
	is.probability = Pr_Ru99;
	ismap["99Ru"] = is;
	is.mass = Ru100;
	is.probability = Pr_Ru100;
	ismap["100Ru"] = is;
	is.mass = Ru101;
	is.probability = Pr_Ru101;
	ismap["101Ru"] = is;
	is.mass = Ru102;
	is.probability = Pr_Ru102;
	ismap["Ru"] = is;
	is.mass = Ru104;
	is.probability = Pr_Ru104;
	ismap["104Ru"] = is;
	table["Ru"] = ismap;

	ismap.clear();
	is.mass = Rh103;
	is.probability = Pr_Rh103;
	ismap["Rh"] = is;
	table["Rh"] = ismap;

	ismap.clear();
	is.mass = Pd102;
	is.probability = Pr_Pd102;
	ismap["102Pd"] = is;
	is.mass = Pd104;
	is.probability = Pr_Pd104;
	ismap["104Pd"] = is;
	is.mass = Pd105;
	is.probability = Pr_Pd105;
	ismap["105Pd"] = is;
	is.mass = Pd106;
	is.probability = Pr_Pd106;
	ismap["Pd"] = is;
	is.mass = Pd108;
	is.probability = Pr_Pd108;
	ismap["108Pd"] = is;
	is.mass = Pd110;
	is.probability = Pr_Pd110;
	ismap["110Pd"] = is;
	table["Pd"] = ismap;

	ismap.clear();
	is.mass = Ag107;
	is.probability = Pr_Ag107;
	ismap["Ag"] = is;
	is.mass = Ag109;
	is.probability = Pr_Ag109;
	ismap["109Ag"] = is;
	table["Ag"] = ismap;

	ismap.clear();
	is.mass = Cd106;
	is.probability = Pr_Cd106;
	ismap["106Cd"] = is;
	is.mass = Cd108;
	is.probability = Pr_Cd108;
	ismap["108Cd"] = is;
	is.mass = Cd110;
	is.probability = Pr_Cd110;
	ismap["110Cd"] = is;
	is.mass = Cd111;
	is.probability = Pr_Cd111;
	ismap["111Cd"] = is;
	is.mass = Cd112;
	is.probability = Pr_Cd112;
	ismap["112Cd"] = is;
	is.mass = Cd113;
	is.probability = Pr_Cd113;
	ismap["113Cd"] = is;
	is.mass = Cd114;
	is.probability = Pr_Cd114;
	ismap["Cd"] = is;
	is.mass = Cd116;
	is.probability = Pr_Cd116;
	ismap["116Cd"] = is;
	table["Cd"] = ismap;

	ismap.clear();
	is.mass = In113;
	is.probability = Pr_In113;
	ismap["113In"] = is;
	is.mass = In115;
	is.probability = Pr_In115;
	ismap["In"] = is;
	table["In"] = ismap;

	ismap.clear();
	is.mass = Sn112;
	is.probability = Pr_Sn112;
	ismap["112Sn"] = is;
	is.mass = Sn114;
	is.probability = Pr_Sn114;
	ismap["114Sn"] = is;
	is.mass = Sn115;
	is.probability = Pr_Sn115;
	ismap["115Sn"] = is;
	is.mass = Sn116;
	is.probability = Pr_Sn116;
	ismap["116Sn"] = is;
	is.mass = Sn117;
	is.probability = Pr_Sn117;
	ismap["117Sn"] = is;
	is.mass = Sn118;
	is.probability = Pr_Sn118;
	ismap["118Sn"] = is;
	is.mass = Sn119;
	is.probability = Pr_Sn119;
	ismap["119Sn"] = is;
	is.mass = Sn120;
	is.probability = Pr_Sn120;
	ismap["Sn"] = is;
	is.mass = Sn122;
	is.probability = Pr_Sn122;
	ismap["122Sn"] = is;
	is.mass = Sn124;
	is.probability = Pr_Sn124;
	ismap["124Sn"] = is;
	table["Sn"] = ismap;

	ismap.clear();
	is.mass = Sb121;
	is.probability = Pr_Sb121;
	ismap["Sb"] = is;
	is.mass = Sb123;
	is.probability = Pr_Sb123;
	ismap["123Sb"] = is;
	table["Sb"] = ismap;

	ismap.clear();
	is.mass = Te120;
	is.probability = Pr_Te120;
	ismap["120Te"] = is;
	is.mass = Te122;
	is.probability = Pr_Te122;
	ismap["122Te"] = is;
	is.mass = Te123;
	is.probability = Pr_Te123;
	ismap["123Te"] = is;
	is.mass = Te124;
	is.probability = Pr_Te124;
	ismap["124Te"] = is;
	is.mass = Te125;
	is.probability = Pr_Te125;
	ismap["125Te"] = is;
	is.mass = Te126;
	is.probability = Pr_Te126;
	ismap["126Te"] = is;
	is.mass = Te128;
	is.probability = Pr_Te128;
	ismap["128Te"] = is;
	is.mass = Te130;
	is.probability = Pr_Te130;
	ismap["Te"] = is;
	table["Te"] = ismap;

	ismap.clear();
	is.mass = I127;
	is.probability = Pr_I127;
	ismap["I"] = is;
	table["I"] = ismap;

	ismap.clear();
	is.mass = Xe124;
	is.probability = Pr_Xe124;
	ismap["124Xe"] = is;
	is.mass = Xe126;
	is.probability = Pr_Xe126;
	ismap["126Xe"] = is;
	is.mass = Xe128;
	is.probability = Pr_Xe128;
	ismap["128Xe"] = is;
	is.mass = Xe129;
	is.probability = Pr_Xe129;
	ismap["129Xe"] = is;
	is.mass = Xe130;
	is.probability = Pr_Xe130;
	ismap["130Xe"] = is;
	is.mass = Xe131;
	is.probability = Pr_Xe131;
	ismap["131Xe"] = is;
	is.mass = Xe132;
	is.probability = Pr_Xe132;
	ismap["Xe"] = is;
	is.mass = Xe134;
	is.probability = Pr_Xe134;
	ismap["134Xe"] = is;
	is.mass = Xe136;
	is.probability = Pr_Xe136;
	ismap["136Xe"] = is;
	table["Xe"] = ismap;

	ismap.clear();
	is.mass = Cs133;
	is.probability = Pr_Cs133;
	ismap["Cs"] = is;
	table["Cs"] = ismap;

	ismap.clear();
	is.mass = Ba130;
	is.probability = Pr_Ba130;
	ismap["130Ba"] = is;
	is.mass = Ba132;
	is.probability = Pr_Ba132;
	ismap["132Ba"] = is;
	is.mass = Ba134;
	is.probability = Pr_Ba134;
	ismap["134Ba"] = is;
	is.mass = Ba135;
	is.probability = Pr_Ba135;
	ismap["135Ba"] = is;
	is.mass = Ba136;
	is.probability = Pr_Ba136;
	ismap["136Ba"] = is;
	is.mass = Ba137;
	is.probability = Pr_Ba137;
	ismap["137Ba"] = is;
	is.mass = Ba138;
	is.probability = Pr_Ba138;
	ismap["Ba"] = is;
	table["Ba"] = ismap;

	ismap.clear();
	is.mass = La138;
	is.probability = Pr_La138;
	ismap["138La"] = is;
	is.mass = La139;
	is.probability = Pr_La139;
	ismap["La"] = is;
	table["La"] = ismap;

	ismap.clear();
	is.mass = Ce136;
	is.probability = Pr_Ce136;
	ismap["136Ce"] = is;
	is.mass = Ce138;
	is.probability = Pr_Ce138;
	ismap["138Ce"] = is;
	is.mass = Ce140;
	is.probability = Pr_Ce140;
	ismap["Ce"] = is;
	is.mass = Ce142;
	is.probability = Pr_Ce142;
	ismap["142Ce"] = is;
	table["Ce"] = ismap;

	ismap.clear();
	is.mass = Pr141;
	is.probability = Pr_Pr141;
	ismap["Pr"] = is;
	table["Pr"] = ismap;

	ismap.clear();
	is.mass = Nd142;
	is.probability = Pr_Nd142;
	ismap["Nd"] = is;
	is.mass = Nd143;
	is.probability = Pr_Nd143;
	ismap["143Nd"] = is;
	is.mass = Nd144;
	is.probability = Pr_Nd144;
	ismap["144Nd"] = is;
	is.mass = Nd145;
	is.probability = Pr_Nd145;
	ismap["145Nd"] = is;
	is.mass = Nd146;
	is.probability = Pr_Nd146;
	ismap["146Nd"] = is;
	is.mass = Nd148;
	is.probability = Pr_Nd148;
	ismap["148Nd"] = is;
	is.mass = Nd150;
	is.probability = Pr_Nd150;
	ismap["150Nd"] = is;
	table["Nd"] = ismap;

	ismap.clear();
	is.mass = Pm145;
	is.probability = Pr_Pm145;
	ismap["Pm"] = is;
	table["Pm"] = ismap;

	ismap.clear();
	is.mass = Sm144;
	is.probability = Pr_Sm144;
	ismap["144Sm"] = is;
	is.mass = Sm147;
	is.probability = Pr_Sm147;
	ismap["147Sm"] = is;
	is.mass = Sm148;
	is.probability = Pr_Sm148;
	ismap["148Sm"] = is;
	is.mass = Sm149;
	is.probability = Pr_Sm149;
	ismap["149Sm"] = is;
	is.mass = Sm150;
	is.probability = Pr_Sm150;
	ismap["150Sm"] = is;
	is.mass = Sm152;
	is.probability = Pr_Sm152;
	ismap["Sm"] = is;
	is.mass = Sm154;
	is.probability = Pr_Sm154;
	ismap["154Sm"] = is;
	table["Sm"] = ismap;

	ismap.clear();
	is.mass = Eu151;
	is.probability = Pr_Eu151;
	ismap["151Eu"] = is;
	is.mass = Eu153;
	is.probability = Pr_Eu153;
	ismap["Eu"] = is;
	table["Eu"] = ismap;

	ismap.clear();
	is.mass = Gd152;
	is.probability = Pr_Gd152;
	ismap["152Gd"] = is;
	is.mass = Gd154;
	is.probability = Pr_Gd154;
	ismap["154Gd"] = is;
	is.mass = Gd155;
	is.probability = Pr_Gd155;
	ismap["155Gd"] = is;
	is.mass = Gd156;
	is.probability = Pr_Gd156;
	ismap["156Gd"] = is;
	is.mass = Gd157;
	is.probability = Pr_Gd157;
	ismap["157Gd"] = is;
	is.mass = Gd158;
	is.probability = Pr_Gd158;
	ismap["Gd"] = is;
	is.mass = Gd160;
	is.probability = Pr_Gd160;
	ismap["160Gd"] = is;
	table["Gd"] = ismap;

	ismap.clear();
	is.mass = Tb159;
	is.probability = Pr_Tb159;
	ismap["Tb"] = is;
	table["Tb"] = ismap;

	ismap.clear();
	is.mass = Dy156;
	is.probability = Pr_Dy156;
	ismap["156Dy"] = is;
	is.mass = Dy158;
	is.probability = Pr_Dy158;
	ismap["158Dy"] = is;
	is.mass = Dy160;
	is.probability = Pr_Dy160;
	ismap["160Dy"] = is;
	is.mass = Dy161;
	is.probability = Pr_Dy161;
	ismap["161Dy"] = is;
	is.mass = Dy162;
	is.probability = Pr_Dy162;
	ismap["162Dy"] = is;
	is.mass = Dy163;
	is.probability = Pr_Dy163;
	ismap["163Dy"] = is;
	is.mass = Dy164;
	is.probability = Pr_Dy164;
	ismap["Dy"] = is;
	table["Dy"] = ismap;

	ismap.clear();
	is.mass = Ho165;
	is.probability = Pr_Ho165;
	ismap["Ho"] = is;
	table["Ho"] = ismap;

	ismap.clear();
	is.mass = Er162;
	is.probability = Pr_Er162;
	ismap["162Er"] = is;
	is.mass = Er164;
	is.probability = Pr_Er164;
	ismap["164Er"] = is;
	is.mass = Er166;
	is.probability = Pr_Er166;
	ismap["Er"] = is;
	is.mass = Er167;
	is.probability = Pr_Er167;
	ismap["167Er"] = is;
	is.mass = Er168;
	is.probability = Pr_Er168;
	ismap["168Er"] = is;
	is.mass = Er170;
	is.probability = Pr_Er170;
	ismap["170Er"] = is;
	table["Er"] = ismap;

	ismap.clear();
	is.mass = Tm169;
	is.probability = Pr_Tm169;
	ismap["Tm"] = is;
	table["Tm"] = ismap;

	ismap.clear();
	is.mass = Yb168;
	is.probability = Pr_Yb168;
	ismap["168Yb"] = is;
	is.mass = Yb170;
	is.probability = Pr_Yb170;
	ismap["170Yb"] = is;
	is.mass = Yb171;
	is.probability = Pr_Yb171;
	ismap["171Yb"] = is;
	is.mass = Yb172;
	is.probability = Pr_Yb172;
	ismap["172Yb"] = is;
	is.mass = Yb173;
	is.probability = Pr_Yb173;
	ismap["173Yb"] = is;
	is.mass = Yb174;
	is.probability = Pr_Yb174;
	ismap["Yb"] = is;
	is.mass = Yb176;
	is.probability = Pr_Yb176;
	ismap["176Yb"] = is;
	table["Yb"] = ismap;

	ismap.clear();
	is.mass = Lu175;
	is.probability = Pr_Lu175;
	ismap["Lu"] = is;
	is.mass = Lu176;
	is.probability = Pr_Lu176;
	ismap["176Lu"] = is;
	table["Lu"] = ismap;

	ismap.clear();
	is.mass = Hf174;
	is.probability = Pr_Hf174;
	ismap["174Hf"] = is;
	is.mass = Hf176;
	is.probability = Pr_Hf176;
	ismap["176Hf"] = is;
	is.mass = Hf177;
	is.probability = Pr_Hf177;
	ismap["177Hf"] = is;
	is.mass = Hf178;
	is.probability = Pr_Hf178;
	ismap["178Hf"] = is;
	is.mass = Hf179;
	is.probability = Pr_Hf179;
	ismap["179Hf"] = is;
	is.mass = Hf180;
	is.probability = Pr_Hf180;
	ismap["Hf"] = is;
	table["Hf"] = ismap;

	ismap.clear();
	is.mass = Ta180;
	is.probability = Pr_Ta180;
	ismap["180Ta"] = is;
	is.mass = Ta181;
	is.probability = Pr_Ta181;
	ismap["Ta"] = is;
	table["Ta"] = ismap;

	ismap.clear();
	is.mass = W180;
	is.probability = Pr_W180;
	ismap["180W"] = is;
	is.mass = W182;
	is.probability = Pr_W182;
	ismap["182W"] = is;
	is.mass = W183;
	is.probability = Pr_W183;
	ismap["183W"] = is;
	is.mass = W184;
	is.probability = Pr_W184;
	ismap["W"] = is;
	is.mass = W186;
	is.probability = Pr_W186;
	ismap["186W"] = is;
	table["W"] = ismap;

	ismap.clear();
	is.mass = Re185;
	is.probability = Pr_Re185;
	ismap["185Re"] = is;
	is.mass = Re187;
	is.probability = Pr_Re187;
	ismap["Re"] = is;
	table["Re"] = ismap;

	ismap.clear();
	is.mass = Os184;
	is.probability = Pr_Os184;
	ismap["184Os"] = is;
	is.mass = Os186;
	is.probability = Pr_Os186;
	ismap["186Os"] = is;
	is.mass = Os187;
	is.probability = Pr_Os187;
	ismap["187Os"] = is;
	is.mass = Os188;
	is.probability = Pr_Os188;
	ismap["188Os"] = is;
	is.mass = Os189;
	is.probability = Pr_Os189;
	ismap["189Os"] = is;
	is.mass = Os190;
	is.probability = Pr_Os190;
	ismap["190Os"] = is;
	is.mass = Os192;
	is.probability = Pr_Os192;
	ismap["Os"] = is;
	table["Os"] = ismap;

	ismap.clear();
	is.mass = Ir191;
	is.probability = Pr_Ir191;
	ismap["191Ir"] = is;
	is.mass = Ir193;
	is.probability = Pr_Ir193;
	ismap["Ir"] = is;
	table["Ir"] = ismap;

	ismap.clear();
	is.mass = Pt190;
	is.probability = Pr_Pt190;
	ismap["190Pt"] = is;
	is.mass = Pt192;
	is.probability = Pr_Pt192;
	ismap["192Pt"] = is;
	is.mass = Pt194;
	is.probability = Pr_Pt194;
	ismap["194Pt"] = is;
	is.mass = Pt195;
	is.probability = Pr_Pt195;
	ismap["Pt"] = is;
	is.mass = Pt196;
	is.probability = Pr_Pt196;
	ismap["196Pt"] = is;
	is.mass = Pt198;
	is.probability = Pr_Pt198;
	ismap["198Pt"] = is;
	table["Pt"] = ismap;

	ismap.clear();
	is.mass = Au197;
	is.probability = Pr_Au197;
	ismap["Au"] = is;
	table["Au"] = ismap;

	ismap.clear();
	is.mass = Hg196;
	is.probability = Pr_Hg196;
	ismap["196Hg"] = is;
	is.mass = Hg198;
	is.probability = Pr_Hg198;
	ismap["198Hg"] = is;
	is.mass = Hg199;
	is.probability = Pr_Hg199;
	ismap["199Hg"] = is;
	is.mass = Hg200;
	is.probability = Pr_Hg200;
	ismap["200Hg"] = is;
	is.mass = Hg201;
	is.probability = Pr_Hg201;
	ismap["201Hg"] = is;
	is.mass = Hg202;
	is.probability = Pr_Hg202;
	ismap["Hg"] = is;
	is.mass = Hg204;
	is.probability = Pr_Hg204;
	ismap["204Hg"] = is;
	table["Hg"] = ismap;

	ismap.clear();
	is.mass = Tl203;
	is.probability = Pr_Tl203;
	ismap["203Tl"] = is;
	is.mass = Tl205;
	is.probability = Pr_Tl205;
	ismap["Tl"] = is;
	table["Tl"] = ismap;

	ismap.clear();
	is.mass = Pb204;
	is.probability = Pr_Pb204;
	ismap["204Pb"] = is;
	is.mass = Pb206;
	is.probability = Pr_Pb206;
	ismap["206Pb"] = is;
	is.mass = Pb207;
	is.probability = Pr_Pb207;
	ismap["207Pb"] = is;
	is.mass = Pb208;
	is.probability = Pr_Pb208;
	ismap["Pb"] = is;
	table["Pb"] = ismap;

	ismap.clear();
	is.mass = Bi209;
	is.probability = Pr_Bi209;
	ismap["Bi"] = is;
	table["Bi"] = ismap;

	ismap.clear();
	is.mass = Po209;
	is.probability = Pr_Po209;
	ismap["Po"] = is;
	table["Po"] = ismap;

	ismap.clear();
	is.mass = At210;
	is.probability = Pr_At210;
	ismap["At"] = is;
	table["At"] = ismap;

	ismap.clear();
	is.mass = Rn222;
	is.probability = Pr_Rn222;
	ismap["Rn"] = is;
	table["Rn"] = ismap;

	ismap.clear();
	is.mass = Fr223;
	is.probability = Pr_Fr223;
	ismap["Fr"] = is;
	table["Fr"] = ismap;

	ismap.clear();
	is.mass = Ra226;
	is.probability = Pr_Ra226;
	ismap["Ra"] = is;
	table["Ra"] = ismap;

	ismap.clear();
	is.mass = Ac227;
	is.probability = Pr_Ac227;
	ismap["Ac"] = is;
	table["Ac"] = ismap;

	ismap.clear();
	is.mass = Th232;
	is.probability = Pr_Th232;
	ismap["Th"] = is;
	table["Th"] = ismap;

	ismap.clear();
	is.mass = Pa231;
	is.probability = Pr_Pa231;
	ismap["Pa"] = is;
	table["Pa"] = ismap;

	ismap.clear();
	is.mass = U234;
	is.probability = Pr_U234;
	ismap["234U"] = is;
	is.mass = U235;
	is.probability = Pr_U235;
	ismap["235U"] = is;
	is.mass = U238;
	is.probability = Pr_U238;
	ismap["U"] = is;
	table["U"] = ismap;

	ismap.clear();
	is.mass = Np237;
	is.probability = Pr_Np237;
	ismap["Np"] = is;
	table["Np"] = ismap;

	ismap.clear();
	is.mass = Pu244;
	is.probability = Pr_Pu244;
	ismap["Pu"] = is;
	table["Pu"] = ismap;

	ismap.clear();
	is.mass = Am243;
	is.probability = Pr_Am243;
	ismap["Am"] = is;
	table["Am"] = ismap;

	ismap.clear();
	is.mass = Cm247;
	is.probability = Pr_Cm247;
	ismap["Cm"] = is;
	table["Cm"] = ismap;

	ismap.clear();
	is.mass = Bk247;
	is.probability = Pr_Bk247;
	ismap["Bk"] = is;
	table["Bk"] = ismap;

	ismap.clear();
	is.mass = Cf251;
	is.probability = Pr_Cf251;
	ismap["Cf"] = is;
	table["Cf"] = ismap;

	ismap.clear();
	is.mass = Es252;
	is.probability = Pr_Es252;
	ismap["Es"] = is;
	table["Es"] = ismap;

	ismap.clear();
	is.mass = Fm257;
	is.probability = Pr_Fm257;
	ismap["Fm"] = is;
	table["Fm"] = ismap;

	ismap.clear();
	is.mass = Md258;
	is.probability = Pr_Md258;
	ismap["Md"] = is;
	table["Md"] = ismap;

	ismap.clear();
	is.mass = No259;
	is.probability = Pr_No259;
	ismap["No"] = is;
	table["No"] = ismap;

	ismap.clear();
	is.mass = Lr262;
	is.probability = Pr_Lr262;
	ismap["Lr"] = is;
	table["Lr"] = ismap;

	ismap.clear();
	is.mass = R;
	is.probability = 1;
	ismap["R"] = is;
	table["R"] = ismap;

	ismap.clear();
	is.mass = X;
	is.probability = 1;
	ismap["X"] = is;
	table["X"] = ismap;
}


map<string, isotope> &cIsotopeMap::operator[](string& element) {
	return table[element];
}


int cIsotopeMap::count(string& element) {
	return (int)table.count(element);
}


double charge(double neutralmass, int charge) {
	return (neutralmass + (double)charge*(H - e))/(double)abs(charge);
}


double uncharge(double mzratio, int charge) {
	return mzratio*(double)abs(charge) - (double)charge*(H - e);
}


void fragmentDescription::store(ofstream& os) {
	storeString(name, os);
	storeString(summary, os);
	storeStringIntMap(summarymap, os);
	os.write((char *)&massdifference, sizeof(double));
	os.write((char *)&nterminal, sizeof(bool));
	os.write((char *)&cterminal, sizeof(bool));
	os.write((char *)&positive, sizeof(bool));
	os.write((char *)&multiplier, sizeof(int));
}


void fragmentDescription::load(ifstream& is) {
	loadString(name, is);
	loadString(summary, is);
	loadStringIntMap(summarymap, is);
	is.read((char *)&massdifference, sizeof(double));
	is.read((char *)&nterminal, sizeof(bool));
	is.read((char *)&cterminal, sizeof(bool));
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


fragmentDescription &cFragmentIons::operator[](eFragmentIonType iontype) {
	return fragmentions[iontype];
}


void cFragmentIons::recalculateFragments(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct) {

	fragmentions.clear();

	double nterminusshift = cyclicnterminus?-H2O:0;
	double cterminusshift = cycliccterminus?-H2O:0;
	double adductshift = 0;

	string nterminusshiftsummary = cyclicnterminus?"H-2O-1":"";
	string cterminusshiftsummary = cycliccterminus?"H-2O-1":"";
	string adductshiftsummary = "";

	if (precursoradduct.compare("") != 0) {
		cSummaryFormula formula;
		formula.setFormula(precursoradduct);
		adductshift = formula.getMass() - H;
		adductshiftsummary = formula.getSummary() + "H-1";
	}

	// initialize A-ion
	fragmentions[a_ion].nterminal = true;
	fragmentions[a_ion].cterminal = false;
	fragmentions[a_ion].name = "A";
	fragmentions[a_ion].massdifference = A_ION + nterminusshift + adductshift;
	fragmentions[a_ion].summary = "C-1O-1H+" + nterminusshiftsummary + adductshiftsummary;
	
	// initialize B-ion
	fragmentions[b_ion].nterminal = true;
	fragmentions[b_ion].cterminal = false;
	fragmentions[b_ion].name = "B";
	fragmentions[b_ion].massdifference = B_ION + nterminusshift + adductshift;
	fragmentions[b_ion].summary = "H+" + nterminusshiftsummary + adductshiftsummary;

	// initialize C-ion
	fragmentions[c_ion].nterminal = true;
	fragmentions[c_ion].cterminal = false;
	fragmentions[c_ion].name = "C";
	fragmentions[c_ion].massdifference = C_ION + nterminusshift + adductshift;
	fragmentions[c_ion].summary = "NH3H+" + nterminusshiftsummary + adductshiftsummary;
	
	// initialize X ion
	fragmentions[x_ion].nterminal = false;
	fragmentions[x_ion].cterminal = true;
	fragmentions[x_ion].name = "X";
	fragmentions[x_ion].massdifference = X_ION + cterminusshift + adductshift;
	fragmentions[x_ion].summary = "CO2H+" + cterminusshiftsummary + adductshiftsummary;

	// initialize Y ion
	fragmentions[y_ion].nterminal = false;
	fragmentions[y_ion].cterminal = true;
	fragmentions[y_ion].name = "Y";
	fragmentions[y_ion].massdifference = Y_ION + cterminusshift + adductshift;
	fragmentions[y_ion].summary = "H2OH+" + cterminusshiftsummary + adductshiftsummary;

	// initialize Z ion
	fragmentions[z_ion].nterminal = false;
	fragmentions[z_ion].cterminal = true;
	fragmentions[z_ion].name = "Z";
	fragmentions[z_ion].massdifference = Z_ION + cterminusshift + adductshift;
	fragmentions[z_ion].summary = "ON-1+" + cterminusshiftsummary + adductshiftsummary;

	// initialize precursor ion
	fragmentions[precursor_ion].nterminal = false;
	fragmentions[precursor_ion].cterminal = false;
	fragmentions[precursor_ion].name = "[M+zH]+";
	fragmentions[precursor_ion].massdifference = PRECURSOR_ION + nterminusshift + cterminusshift + adductshift;
	fragmentions[precursor_ion].summary = "H2OH+" + nterminusshiftsummary + cterminusshiftsummary + adductshiftsummary;

	// initialize cyclic precursor ion
	fragmentions[cyclic_precursor_ion].nterminal = false;
	fragmentions[cyclic_precursor_ion].cterminal = false;
	fragmentions[cyclic_precursor_ion].name = "[M+zH]+";
	fragmentions[cyclic_precursor_ion].massdifference = PRECURSOR_ION_CYCLIC + adductshift;
	fragmentions[cyclic_precursor_ion].summary = "H+" + adductshiftsummary;

	// initialize l0h ion
	fragmentions[l0h_ion].nterminal = true;
	fragmentions[l0h_ion].cterminal = false;
	fragmentions[l0h_ion].name = "LB-2H";
	fragmentions[l0h_ion].massdifference = L0H_ION + adductshift;
	fragmentions[l0h_ion].summary = "H-2H+" + adductshiftsummary;

	// initialize l1h ion
	fragmentions[l1h_ion].nterminal = true;
	fragmentions[l1h_ion].cterminal = false;
	fragmentions[l1h_ion].name = "LB";
	fragmentions[l1h_ion].massdifference = L1H_ION + adductshift;
	fragmentions[l1h_ion].summary = "H+" + adductshiftsummary;

	// initialize l2h ion
	fragmentions[l2h_ion].nterminal = true;
	fragmentions[l2h_ion].cterminal = false;
	fragmentions[l2h_ion].name = "LB+2H";
	fragmentions[l2h_ion].massdifference = L2H_ION + adductshift;
	fragmentions[l2h_ion].summary = "H2H+" + adductshiftsummary;

	// initialize r1h ion
	fragmentions[r1h_ion].nterminal = true;
	fragmentions[r1h_ion].cterminal = false;
	fragmentions[r1h_ion].name = "RB";
	fragmentions[r1h_ion].massdifference = R1H_ION + adductshift;
	fragmentions[r1h_ion].summary = "H+" + adductshiftsummary;

	// initialize r2h ion
	fragmentions[r2h_ion].nterminal = true;
	fragmentions[r2h_ion].cterminal = false;
	fragmentions[r2h_ion].name = "RB+2H";
	fragmentions[r2h_ion].massdifference = R2H_ION + adductshift;
	fragmentions[r2h_ion].summary = "H2H+" + adductshiftsummary;

	// initialize l1oh ion
	fragmentions[l1oh_ion].nterminal = false;
	fragmentions[l1oh_ion].cterminal = true;
	fragmentions[l1oh_ion].name = "LY-2H";
	fragmentions[l1oh_ion].massdifference = L1OH_ION + adductshift;
	fragmentions[l1oh_ion].summary = "OH+" + adductshiftsummary;

	// initialize l2oh ion
	fragmentions[l2oh_ion].nterminal = false;
	fragmentions[l2oh_ion].cterminal = true;
	fragmentions[l2oh_ion].name = "LY";
	fragmentions[l2oh_ion].massdifference = L2OH_ION + adductshift;
	fragmentions[l2oh_ion].summary = "H2OH+" + adductshiftsummary;

	// initialize r1oh ion
	fragmentions[r1oh_ion].nterminal = false;
	fragmentions[r1oh_ion].cterminal = true;
	fragmentions[r1oh_ion].name = "RY-2H";
	fragmentions[r1oh_ion].massdifference = R1OH_ION + adductshift;
	fragmentions[r1oh_ion].summary = "OH+" + adductshiftsummary;

	// initialize r2oh ion
	fragmentions[r2oh_ion].nterminal = false;
	fragmentions[r2oh_ion].cterminal = true;
	fragmentions[r2oh_ion].name = "RY";
	fragmentions[r2oh_ion].massdifference = R2OH_ION + adductshift;
	fragmentions[r2oh_ion].summary = "H2OH+" + adductshiftsummary;

	// initialize linear polyketide precursor ion (H-...-H)
	fragmentions[linear_polyketide_precursor_ion_h_h].nterminal = false;
	fragmentions[linear_polyketide_precursor_ion_h_h].cterminal = false;
	fragmentions[linear_polyketide_precursor_ion_h_h].name = "[M+zH]+ (H-...-H)";
	fragmentions[linear_polyketide_precursor_ion_h_h].massdifference = PRECURSOR_ION_LINEAR_POLYKETIDE_H_H + nterminusshift + cterminusshift + adductshift;
	fragmentions[linear_polyketide_precursor_ion_h_h].summary = "H2H+" + nterminusshiftsummary + cterminusshiftsummary + adductshiftsummary;

	// initialize linear polyketide precursor ion (H-...-OH)
	fragmentions[linear_polyketide_precursor_ion_h_oh].nterminal = false;
	fragmentions[linear_polyketide_precursor_ion_h_oh].cterminal = false;
	fragmentions[linear_polyketide_precursor_ion_h_oh].name = "[M+zH]+ (H-...-OH)";
	fragmentions[linear_polyketide_precursor_ion_h_oh].massdifference = PRECURSOR_ION_LINEAR_POLYKETIDE_H_OH + nterminusshift + cterminusshift + adductshift;
	fragmentions[linear_polyketide_precursor_ion_h_oh].summary = "H2OH+" + nterminusshiftsummary + cterminusshiftsummary + adductshiftsummary;

	// initialize linear polyketide precursor ion (HO-...-OH)
	fragmentions[linear_polyketide_precursor_ion_oh_oh].nterminal = false;
	fragmentions[linear_polyketide_precursor_ion_oh_oh].cterminal = false;
	fragmentions[linear_polyketide_precursor_ion_oh_oh].name = "[M+zH]+ (HO-...-OH)";
	fragmentions[linear_polyketide_precursor_ion_oh_oh].massdifference = PRECURSOR_ION_LINEAR_POLYKETIDE_OH_OH + nterminusshift + cterminusshift + adductshift;
	fragmentions[linear_polyketide_precursor_ion_oh_oh].summary = "H2O2H+" + nterminusshiftsummary + cterminusshiftsummary + adductshiftsummary;

	// initialize cyclic polyketide precursor ion
	fragmentions[cyclic_polyketide_precursor_ion].nterminal = false;
	fragmentions[cyclic_polyketide_precursor_ion].cterminal = false;
	fragmentions[cyclic_polyketide_precursor_ion].name = "[M+zH]+";
	fragmentions[cyclic_polyketide_precursor_ion].massdifference = H - e + adductshift;
	fragmentions[cyclic_polyketide_precursor_ion].summary = "H+" + adductshiftsummary;

	// initialize ion [M+H]+
	fragmentions[ms_Hplus].nterminal = true;
	fragmentions[ms_Hplus].cterminal = true;
	fragmentions[ms_Hplus].name = "[M+H]+";
	fragmentions[ms_Hplus].massdifference = H - e;
	fragmentions[ms_Hplus].positive = true;
	fragmentions[ms_Hplus].multiplier = 1;
	fragmentions[ms_Hplus].summary = "H+";

	// initialize ion [M+Na]+
	fragmentions[ms_Naplus].nterminal = true;
	fragmentions[ms_Naplus].cterminal = true;
	fragmentions[ms_Naplus].name = "[M+Na]+";
	fragmentions[ms_Naplus].massdifference = Na - e;
	fragmentions[ms_Naplus].positive = true;
	fragmentions[ms_Naplus].multiplier = 1;
	fragmentions[ms_Naplus].summary = "Na+";

	// initialize ion [M+K]+
	fragmentions[ms_Kplus].nterminal = true;
	fragmentions[ms_Kplus].cterminal = true;
	fragmentions[ms_Kplus].name = "[M+K]+";
	fragmentions[ms_Kplus].massdifference = K - e;
	fragmentions[ms_Kplus].positive = true;
	fragmentions[ms_Kplus].multiplier = 1;
	fragmentions[ms_Kplus].summary = "K+";

	// initialize ion [M-H]-
	fragmentions[ms_Hminus].nterminal = true;
	fragmentions[ms_Hminus].cterminal = true;
	fragmentions[ms_Hminus].name = "[M-H]-";
	fragmentions[ms_Hminus].massdifference = -H + e;
	fragmentions[ms_Hminus].positive = false;
	fragmentions[ms_Hminus].multiplier = 1;
	fragmentions[ms_Hminus].summary = "H-1+-1";

	// initialize ion [M+Na-2H]-
	fragmentions[ms_Naminus].nterminal = true;
	fragmentions[ms_Naminus].cterminal = true;
	fragmentions[ms_Naminus].name = "[M+Na-2H]-";
	fragmentions[ms_Naminus].massdifference = Na - 2*H + e;
	fragmentions[ms_Naminus].positive = false;
	fragmentions[ms_Naminus].multiplier = 1;
	fragmentions[ms_Naminus].summary = "NaH-2+-1";

	// initialize ion [M+K-2H]-
	fragmentions[ms_Kminus].nterminal = true;
	fragmentions[ms_Kminus].cterminal = true;
	fragmentions[ms_Kminus].name = "[M+K-2H]-";
	fragmentions[ms_Kminus].massdifference = K - 2*H + e;
	fragmentions[ms_Kminus].positive = false;
	fragmentions[ms_Kminus].multiplier = 1;
	fragmentions[ms_Kminus].summary = "KH-2+-1";

	// initialize ion [M+Fe-2H]+
	fragmentions[ms_MFe2H].nterminal = true;
	fragmentions[ms_MFe2H].cterminal = true;
	fragmentions[ms_MFe2H].name = "[M+Fe-2H]+";
	fragmentions[ms_MFe2H].massdifference = Fe - 3*H + H - e;
	fragmentions[ms_MFe2H].positive = true;
	fragmentions[ms_MFe2H].multiplier = 1;
	fragmentions[ms_MFe2H].summary = "FeH-3H+";

	// initialize ion [M+Fe-3H+Na]+
	fragmentions[ms_MFe3HNa].nterminal = true;
	fragmentions[ms_MFe3HNa].cterminal = true;
	fragmentions[ms_MFe3HNa].name = "[M+Fe-3H+Na]+";
	fragmentions[ms_MFe3HNa].massdifference = Fe - 3*H + Na - e;
	fragmentions[ms_MFe3HNa].positive = true;
	fragmentions[ms_MFe3HNa].multiplier = 1;
	fragmentions[ms_MFe3HNa].summary = "FeH-3Na+";

	// initialize ion [M+Fe-3H+K]+
	fragmentions[ms_MFe3HK].nterminal = true;
	fragmentions[ms_MFe3HK].cterminal = true;
	fragmentions[ms_MFe3HK].name = "[M+Fe-3H+K]+";
	fragmentions[ms_MFe3HK].massdifference = Fe - 3*H + K - e;
	fragmentions[ms_MFe3HK].positive = true;
	fragmentions[ms_MFe3HK].multiplier = 1;
	fragmentions[ms_MFe3HK].summary = "FeH-3K+";

	// initialize ion [2M+Fe-2H]+
	fragmentions[ms_2MFe2H].nterminal = true;
	fragmentions[ms_2MFe2H].cterminal = true;
	fragmentions[ms_2MFe2H].name = "[2M+Fe-2H]+";
	fragmentions[ms_2MFe2H].massdifference = Fe - 3*H + H - e;
	fragmentions[ms_2MFe2H].positive = true;
	fragmentions[ms_2MFe2H].multiplier = 2;
	fragmentions[ms_2MFe2H].summary = "FeH-3H+";

	// initialize ion [2M+Fe-3H+Na]+
	fragmentions[ms_2MFe3HNa].nterminal = true;
	fragmentions[ms_2MFe3HNa].cterminal = true;
	fragmentions[ms_2MFe3HNa].name = "[2M+Fe-3H+Na]+";
	fragmentions[ms_2MFe3HNa].massdifference = Fe - 3*H + Na - e;
	fragmentions[ms_2MFe3HNa].positive = true;
	fragmentions[ms_2MFe3HNa].multiplier = 2;
	fragmentions[ms_2MFe3HNa].summary = "FeH-3Na+";

	// initialize ion [2M+Fe-3H+K]+
	fragmentions[ms_2MFe3HK].nterminal = true;
	fragmentions[ms_2MFe3HK].cterminal = true;
	fragmentions[ms_2MFe3HK].name = "[2M+Fe-3H+K]+";
	fragmentions[ms_2MFe3HK].massdifference = Fe - 3*H + K - e;
	fragmentions[ms_2MFe3HK].positive = true;
	fragmentions[ms_2MFe3HK].multiplier = 2;
	fragmentions[ms_2MFe3HK].summary = "FeH-3K+";

	// initialize ion [3M+Fe-2H]+
	fragmentions[ms_3MFe2H].nterminal = true;
	fragmentions[ms_3MFe2H].cterminal = true;
	fragmentions[ms_3MFe2H].name = "[3M+Fe-2H]+";
	fragmentions[ms_3MFe2H].massdifference = Fe - 3*H + H - e;
	fragmentions[ms_3MFe2H].positive = true;
	fragmentions[ms_3MFe2H].multiplier = 3;
	fragmentions[ms_3MFe2H].summary = "FeH-3H+";

	// initialize ion [3M+Fe-3H+Na]+
	fragmentions[ms_3MFe3HNa].nterminal = true;
	fragmentions[ms_3MFe3HNa].cterminal = true;
	fragmentions[ms_3MFe3HNa].name = "[3M+Fe-3H+Na]+";
	fragmentions[ms_3MFe3HNa].massdifference = Fe - 3*H + Na - e;
	fragmentions[ms_3MFe3HNa].positive = true;
	fragmentions[ms_3MFe3HNa].multiplier = 3;
	fragmentions[ms_3MFe3HNa].summary = "FeH-3Na+";

	// initialize ion [3M+Fe-3H+K]+
	fragmentions[ms_3MFe3HK].nterminal = true;
	fragmentions[ms_3MFe3HK].cterminal = true;
	fragmentions[ms_3MFe3HK].name = "[3M+Fe-3H+K]+";
	fragmentions[ms_3MFe3HK].massdifference = Fe - 3*H + K - e;
	fragmentions[ms_3MFe3HK].positive = true;
	fragmentions[ms_3MFe3HK].multiplier = 3;
	fragmentions[ms_3MFe3HK].summary = "FeH-3K+";

	// initialize ion [3M+2Fe-5H]+
	fragmentions[ms_3M2Fe5H].nterminal = true;
	fragmentions[ms_3M2Fe5H].cterminal = true;
	fragmentions[ms_3M2Fe5H].name = "[3M+2Fe-5H]+";
	fragmentions[ms_3M2Fe5H].massdifference = 2*Fe - 6*H + H - e;
	fragmentions[ms_3M2Fe5H].positive = true;
	fragmentions[ms_3M2Fe5H].multiplier = 3;
	fragmentions[ms_3M2Fe5H].summary = "Fe2H-6H+";

	// initialize ion [3M+2Fe-6H+Na]+
	fragmentions[ms_3M2Fe6HNa].nterminal = true;
	fragmentions[ms_3M2Fe6HNa].cterminal = true;
	fragmentions[ms_3M2Fe6HNa].name = "[3M+2Fe-6H+Na]+";
	fragmentions[ms_3M2Fe6HNa].massdifference = 2*Fe - 6*H + Na - e;
	fragmentions[ms_3M2Fe6HNa].positive = true;
	fragmentions[ms_3M2Fe6HNa].multiplier = 3;
	fragmentions[ms_3M2Fe6HNa].summary = "Fe2H-6Na+";

	// initialize ion [3M+2Fe-6H+K]+
	fragmentions[ms_3M2Fe6HK].nterminal = true;
	fragmentions[ms_3M2Fe6HK].cterminal = true;
	fragmentions[ms_3M2Fe6HK].name = "[3M+2Fe-6H+K]+";
	fragmentions[ms_3M2Fe6HK].massdifference = 2*Fe - 6*H + K - e;
	fragmentions[ms_3M2Fe6HK].positive = true;
	fragmentions[ms_3M2Fe6HK].multiplier = 3;
	fragmentions[ms_3M2Fe6HK].summary = "Fe2H-6K+";

	// initialize ion [M+Fe-4H]-
	fragmentions[ms_MFe4H].nterminal = true;
	fragmentions[ms_MFe4H].cterminal = true;
	fragmentions[ms_MFe4H].name = "[M+Fe-4H]-";
	fragmentions[ms_MFe4H].massdifference = Fe - 3*H - H + e;
	fragmentions[ms_MFe4H].positive = false;
	fragmentions[ms_MFe4H].multiplier = 1;
	fragmentions[ms_MFe4H].summary = "FeH-3H-1+-1";

	// initialize ion [2M+Fe-4H]-
	fragmentions[ms_2MFe4H].nterminal = true;
	fragmentions[ms_2MFe4H].cterminal = true;
	fragmentions[ms_2MFe4H].name = "[2M+Fe-4H]-";
	fragmentions[ms_2MFe4H].massdifference = Fe - 3*H - H + e;
	fragmentions[ms_2MFe4H].positive = false;
	fragmentions[ms_2MFe4H].multiplier = 2;
	fragmentions[ms_2MFe4H].summary = "FeH-3H-1+-1";

	// initialize ion [3M+Fe-4H]-
	fragmentions[ms_3MFe4H].nterminal = true;
	fragmentions[ms_3MFe4H].cterminal = true;
	fragmentions[ms_3MFe4H].name = "[3M+Fe-4H]-";
	fragmentions[ms_3MFe4H].massdifference = Fe - 3*H - H + e;
	fragmentions[ms_3MFe4H].positive = false;
	fragmentions[ms_3MFe4H].multiplier = 3;
	fragmentions[ms_3MFe4H].summary = "FeH-3H-1+-1";

	// initialize ion [3M+2Fe-7H]-
	fragmentions[ms_3M2Fe7H].nterminal = true;
	fragmentions[ms_3M2Fe7H].cterminal = true;
	fragmentions[ms_3M2Fe7H].name = "[3M+2Fe-7H]-";
	fragmentions[ms_3M2Fe7H].massdifference = 2*Fe - 6*H - H + e;
	fragmentions[ms_3M2Fe7H].positive = false;
	fragmentions[ms_3M2Fe7H].multiplier = 3;
	fragmentions[ms_3M2Fe7H].summary = "Fe2H-6H-1+-1";

	// initialize ion [M+NH4]+
	fragmentions[ms_NH4plus].nterminal = true;
	fragmentions[ms_NH4plus].cterminal = true;
	fragmentions[ms_NH4plus].name = "[M+NH4]+";
	fragmentions[ms_NH4plus].massdifference = H + NH3 - e;
	fragmentions[ms_NH4plus].positive = true;
	fragmentions[ms_NH4plus].multiplier = 1;
	fragmentions[ms_NH4plus].summary = "NH4+";

	// initialize ion [M+Cl]-
	fragmentions[ms_Clminus].nterminal = true;
	fragmentions[ms_Clminus].cterminal = true;
	fragmentions[ms_Clminus].name = "[M+Cl]-";
	fragmentions[ms_Clminus].massdifference = Cl + e;
	fragmentions[ms_Clminus].positive = false;
	fragmentions[ms_Clminus].multiplier = 1;
	fragmentions[ms_Clminus].summary = "Cl+-1";

	// initialize ion [M]+
	fragmentions[ms_plus].nterminal = true;
	fragmentions[ms_plus].cterminal = true;
	fragmentions[ms_plus].name = "[M]+";
	fragmentions[ms_plus].massdifference = -e;
	fragmentions[ms_plus].positive = true;
	fragmentions[ms_plus].multiplier = 1;
	fragmentions[ms_plus].summary = "+";

	// initialize ion [M]-
	fragmentions[ms_minus].nterminal = true;
	fragmentions[ms_minus].cterminal = true;
	fragmentions[ms_minus].name = "[M]-";
	fragmentions[ms_minus].massdifference = e;
	fragmentions[ms_minus].positive = false;
	fragmentions[ms_minus].multiplier = 1;
	fragmentions[ms_minus].summary = "+-1";

	// initialize ion Li+
	fragmentions[ms_liplus].nterminal = true;
	fragmentions[ms_liplus].cterminal = true;
	fragmentions[ms_liplus].name = "[M+Li]+";
	fragmentions[ms_liplus].massdifference = Li - e;
	fragmentions[ms_liplus].positive = true;
	fragmentions[ms_liplus].multiplier = 1;
	fragmentions[ms_liplus].summary = "Li+";

	// initialize ion [M+Mg-H]+
	fragmentions[ms_MMgH].nterminal = true;
	fragmentions[ms_MMgH].cterminal = true;
	fragmentions[ms_MMgH].name = "[M+Mg-H]+";
	fragmentions[ms_MMgH].massdifference = Mg - 2*H + H - e;
	fragmentions[ms_MMgH].positive = true;
	fragmentions[ms_MMgH].multiplier = 1;
	fragmentions[ms_MMgH].summary = "MgH-2H+";

	// initialize ion [M+Mg-2H+Na]+
	fragmentions[ms_MMg2HNa].nterminal = true;
	fragmentions[ms_MMg2HNa].cterminal = true;
	fragmentions[ms_MMg2HNa].name = "[M+Mg-2H+Na]+";
	fragmentions[ms_MMg2HNa].massdifference = Mg - 2*H + Na - e;
	fragmentions[ms_MMg2HNa].positive = true;
	fragmentions[ms_MMg2HNa].multiplier = 1;
	fragmentions[ms_MMg2HNa].summary = "MgH-2Na+";

	// initialize ion [M+Mg-2H+K]+
	fragmentions[ms_MMg2HK].nterminal = true;
	fragmentions[ms_MMg2HK].cterminal = true;
	fragmentions[ms_MMg2HK].name = "[M+Mg-2H+K]+";
	fragmentions[ms_MMg2HK].massdifference = Mg - 2*H + K - e;
	fragmentions[ms_MMg2HK].positive = true;
	fragmentions[ms_MMg2HK].multiplier = 1;
	fragmentions[ms_MMg2HK].summary = "MgH-2K+";

	// initialize ion [M+Mg-3H]-
	fragmentions[ms_MMg3H].nterminal = true;
	fragmentions[ms_MMg3H].cterminal = true;
	fragmentions[ms_MMg3H].name = "[M+Mg-3H]-";
	fragmentions[ms_MMg3H].massdifference = Mg - 2*H - H + e;
	fragmentions[ms_MMg3H].positive = false;
	fragmentions[ms_MMg3H].multiplier = 1;
	fragmentions[ms_MMg3H].summary = "MgH-2H-1+-1";

	// initialize ion [M+Al-2H]+
	fragmentions[ms_MAl2H].nterminal = true;
	fragmentions[ms_MAl2H].cterminal = true;
	fragmentions[ms_MAl2H].name = "[M+Al-2H]+";
	fragmentions[ms_MAl2H].massdifference = Al - 3*H + H - e;
	fragmentions[ms_MAl2H].positive = true;
	fragmentions[ms_MAl2H].multiplier = 1;
	fragmentions[ms_MAl2H].summary = "AlH-3H+";

	// initialize ion [M+Al-3H+Na]+
	fragmentions[ms_MAl3HNa].nterminal = true;
	fragmentions[ms_MAl3HNa].cterminal = true;
	fragmentions[ms_MAl3HNa].name = "[M+Al-3H+Na]+";
	fragmentions[ms_MAl3HNa].massdifference = Al - 3*H + Na - e;
	fragmentions[ms_MAl3HNa].positive = true;
	fragmentions[ms_MAl3HNa].multiplier = 1;
	fragmentions[ms_MAl3HNa].summary = "AlH-3Na+";

	// initialize ion [M+Al-3H+K]+
	fragmentions[ms_MAl3HK].nterminal = true;
	fragmentions[ms_MAl3HK].cterminal = true;
	fragmentions[ms_MAl3HK].name = "[M+Al-3H+K]+";
	fragmentions[ms_MAl3HK].massdifference = Al - 3*H + K - e;
	fragmentions[ms_MAl3HK].positive = true;
	fragmentions[ms_MAl3HK].multiplier = 1;
	fragmentions[ms_MAl3HK].summary = "AlH-3K+";

	// initialize ion [M+Al-4H]-
	fragmentions[ms_MAl4H].nterminal = true;
	fragmentions[ms_MAl4H].cterminal = true;
	fragmentions[ms_MAl4H].name = "[M+Al-4H]-";
	fragmentions[ms_MAl4H].massdifference = Al - 3*H - H + e;
	fragmentions[ms_MAl4H].positive = false;
	fragmentions[ms_MAl4H].multiplier = 1;
	fragmentions[ms_MAl4H].summary = "AlH-3H-1+-1";

	// initialize ion [M+Si-3H]+
	fragmentions[ms_MSi3H].nterminal = true;
	fragmentions[ms_MSi3H].cterminal = true;
	fragmentions[ms_MSi3H].name = "[M+Si-3H]+";
	fragmentions[ms_MSi3H].massdifference = Si - 4 * H + H - e;
	fragmentions[ms_MSi3H].positive = true;
	fragmentions[ms_MSi3H].multiplier = 1;
	fragmentions[ms_MSi3H].summary = "SiH-4H+";

	// initialize ion [M+Si-4H+Na]+
	fragmentions[ms_MSi4HNa].nterminal = true;
	fragmentions[ms_MSi4HNa].cterminal = true;
	fragmentions[ms_MSi4HNa].name = "[M+Si-4H+Na]+";
	fragmentions[ms_MSi4HNa].massdifference = Si - 4 * H + Na - e;
	fragmentions[ms_MSi4HNa].positive = true;
	fragmentions[ms_MSi4HNa].multiplier = 1;
	fragmentions[ms_MSi4HNa].summary = "SiH-4Na+";

	// initialize ion [M+Si-4H+K]+
	fragmentions[ms_MSi4HK].nterminal = true;
	fragmentions[ms_MSi4HK].cterminal = true;
	fragmentions[ms_MSi4HK].name = "[M+Si-4H+K]+";
	fragmentions[ms_MSi4HK].massdifference = Si - 4 * H + K - e;
	fragmentions[ms_MSi4HK].positive = true;
	fragmentions[ms_MSi4HK].multiplier = 1;
	fragmentions[ms_MSi4HK].summary = "SiH-4K+";

	// initialize ion [M+Si-5H]-
	fragmentions[ms_MSi5H].nterminal = true;
	fragmentions[ms_MSi5H].cterminal = true;
	fragmentions[ms_MSi5H].name = "[M+Si-5H]-";
	fragmentions[ms_MSi5H].massdifference = Si - 4 * H - H + e;
	fragmentions[ms_MSi5H].positive = false;
	fragmentions[ms_MSi5H].multiplier = 1;
	fragmentions[ms_MSi5H].summary = "SiH-4H-1+-1";

	// initialize ion [M+Ca-H]+
	fragmentions[ms_MCaH].nterminal = true;
	fragmentions[ms_MCaH].cterminal = true;
	fragmentions[ms_MCaH].name = "[M+Ca-H]+";
	fragmentions[ms_MCaH].massdifference = Ca - 2*H + H - e;
	fragmentions[ms_MCaH].positive = true;
	fragmentions[ms_MCaH].multiplier = 1;
	fragmentions[ms_MCaH].summary = "CaH-2H+";

	// initialize ion [M+Ca-2H+Na]+
	fragmentions[ms_MCa2HNa].nterminal = true;
	fragmentions[ms_MCa2HNa].cterminal = true;
	fragmentions[ms_MCa2HNa].name = "[M+Ca-2H+Na]+";
	fragmentions[ms_MCa2HNa].massdifference = Ca - 2*H + Na - e;
	fragmentions[ms_MCa2HNa].positive = true;
	fragmentions[ms_MCa2HNa].multiplier = 1;
	fragmentions[ms_MCa2HNa].summary = "CaH-2Na+";

	// initialize ion [M+Ca-2H+K]+
	fragmentions[ms_MCa2HK].nterminal = true;
	fragmentions[ms_MCa2HK].cterminal = true;
	fragmentions[ms_MCa2HK].name = "[M+Ca-2H+K]+";
	fragmentions[ms_MCa2HK].massdifference = Ca - 2*H + K - e;
	fragmentions[ms_MCa2HK].positive = true;
	fragmentions[ms_MCa2HK].multiplier = 1;
	fragmentions[ms_MCa2HK].summary = "CaH-2K+";

	// initialize ion [M+Ca-3H]-
	fragmentions[ms_MCa3H].nterminal = true;
	fragmentions[ms_MCa3H].cterminal = true;
	fragmentions[ms_MCa3H].name = "[M+Ca-3H]-";
	fragmentions[ms_MCa3H].massdifference = Ca - 2*H - H + e;
	fragmentions[ms_MCa3H].positive = false;
	fragmentions[ms_MCa3H].multiplier = 1;
	fragmentions[ms_MCa3H].summary = "CaH-2H-1+-1";

	// initialize ion [M+Sc-2H]+
	fragmentions[ms_MSc2H].nterminal = true;
	fragmentions[ms_MSc2H].cterminal = true;
	fragmentions[ms_MSc2H].name = "[M+Sc-2H]+";
	fragmentions[ms_MSc2H].massdifference = Sc - 3*H + H - e;
	fragmentions[ms_MSc2H].positive = true;
	fragmentions[ms_MSc2H].multiplier = 1;
	fragmentions[ms_MSc2H].summary = "ScH-3H+";

	// initialize ion [M+Sc-3H+Na]+
	fragmentions[ms_MSc3HNa].nterminal = true;
	fragmentions[ms_MSc3HNa].cterminal = true;
	fragmentions[ms_MSc3HNa].name = "[M+Sc-3H+Na]+";
	fragmentions[ms_MSc3HNa].massdifference = Sc - 3*H + Na - e;
	fragmentions[ms_MSc3HNa].positive = true;
	fragmentions[ms_MSc3HNa].multiplier = 1;
	fragmentions[ms_MSc3HNa].summary = "ScH-3Na+";

	// initialize ion [M+Sc-3H+K]+
	fragmentions[ms_MSc3HK].nterminal = true;
	fragmentions[ms_MSc3HK].cterminal = true;
	fragmentions[ms_MSc3HK].name = "[M+Sc-3H+K]+";
	fragmentions[ms_MSc3HK].massdifference = Sc - 3*H + K - e;
	fragmentions[ms_MSc3HK].positive = true;
	fragmentions[ms_MSc3HK].multiplier = 1;
	fragmentions[ms_MSc3HK].summary = "ScH-3K+";

	// initialize ion [M+Sc-4H]-
	fragmentions[ms_MSc4H].nterminal = true;
	fragmentions[ms_MSc4H].cterminal = true;
	fragmentions[ms_MSc4H].name = "[M+Sc-4H]-";
	fragmentions[ms_MSc4H].massdifference = Sc - 3*H - H + e;
	fragmentions[ms_MSc4H].positive = false;
	fragmentions[ms_MSc4H].multiplier = 1;
	fragmentions[ms_MSc4H].summary = "ScH-3H-1+-1";

	// initialize ion [M+Cr-2H]+
	fragmentions[ms_MCr2H].nterminal = true;
	fragmentions[ms_MCr2H].cterminal = true;
	fragmentions[ms_MCr2H].name = "[M+Cr-2H]+";
	fragmentions[ms_MCr2H].massdifference = Cr - 3*H + H - e;
	fragmentions[ms_MCr2H].positive = true;
	fragmentions[ms_MCr2H].multiplier = 1;
	fragmentions[ms_MCr2H].summary = "CrH-3H+";

	// initialize ion [M+Cr-3H+Na]+
	fragmentions[ms_MCr3HNa].nterminal = true;
	fragmentions[ms_MCr3HNa].cterminal = true;
	fragmentions[ms_MCr3HNa].name = "[M+Cr-3H+Na]+";
	fragmentions[ms_MCr3HNa].massdifference = Cr - 3*H + Na - e;
	fragmentions[ms_MCr3HNa].positive = true;
	fragmentions[ms_MCr3HNa].multiplier = 1;
	fragmentions[ms_MCr3HNa].summary = "CrH-3Na+";

	// initialize ion [M+Cr-3H+K]+
	fragmentions[ms_MCr3HK].nterminal = true;
	fragmentions[ms_MCr3HK].cterminal = true;
	fragmentions[ms_MCr3HK].name = "[M+Cr-3H+K]+";
	fragmentions[ms_MCr3HK].massdifference = Cr - 3*H + K - e;
	fragmentions[ms_MCr3HK].positive = true;
	fragmentions[ms_MCr3HK].multiplier = 1;
	fragmentions[ms_MCr3HK].summary = "CrH-3K+";

	// initialize ion [M+Cr-4H]-
	fragmentions[ms_MCr4H].nterminal = true;
	fragmentions[ms_MCr4H].cterminal = true;
	fragmentions[ms_MCr4H].name = "[M+Cr-4H]-";
	fragmentions[ms_MCr4H].massdifference = Cr - 3*H - H + e;
	fragmentions[ms_MCr4H].positive = false;
	fragmentions[ms_MCr4H].multiplier = 1;
	fragmentions[ms_MCr4H].summary = "CrH-3H-1+-1";

	// initialize ion [M+Mn-H]+
	fragmentions[ms_MMnH].nterminal = true;
	fragmentions[ms_MMnH].cterminal = true;
	fragmentions[ms_MMnH].name = "[M+Mn-H]+";
	fragmentions[ms_MMnH].massdifference = Mn - 2*H + H - e;
	fragmentions[ms_MMnH].positive = true;
	fragmentions[ms_MMnH].multiplier = 1;
	fragmentions[ms_MMnH].summary = "MnH-2H+";

	// initialize ion [M+Mn-2H+Na]+
	fragmentions[ms_MMn2HNa].nterminal = true;
	fragmentions[ms_MMn2HNa].cterminal = true;
	fragmentions[ms_MMn2HNa].name = "[M+Mn-2H+Na]+";
	fragmentions[ms_MMn2HNa].massdifference = Mn - 2*H + Na - e;
	fragmentions[ms_MMn2HNa].positive = true;
	fragmentions[ms_MMn2HNa].multiplier = 1;
	fragmentions[ms_MMn2HNa].summary = "MnH-2Na+";

	// initialize ion [M+Mn-2H+K]+
	fragmentions[ms_MMn2HK].nterminal = true;
	fragmentions[ms_MMn2HK].cterminal = true;
	fragmentions[ms_MMn2HK].name = "[M+Mn-2H+K]+";
	fragmentions[ms_MMn2HK].massdifference = Mn - 2*H + K - e;
	fragmentions[ms_MMn2HK].positive = true;
	fragmentions[ms_MMn2HK].multiplier = 1;
	fragmentions[ms_MMn2HK].summary = "MnH-2K+";

	// initialize ion [M+Mn-3H]-
	fragmentions[ms_MMn3H].nterminal = true;
	fragmentions[ms_MMn3H].cterminal = true;
	fragmentions[ms_MMn3H].name = "[M+Mn-3H]-";
	fragmentions[ms_MMn3H].massdifference = Mn - 2*H - H + e;
	fragmentions[ms_MMn3H].positive = false;
	fragmentions[ms_MMn3H].multiplier = 1;
	fragmentions[ms_MMn3H].summary = "MnH-2H-1+-1";

	// initialize ion [M+Co-H]+
	fragmentions[ms_MCoH].nterminal = true;
	fragmentions[ms_MCoH].cterminal = true;
	fragmentions[ms_MCoH].name = "[M+Co-H]+";
	fragmentions[ms_MCoH].massdifference = Co - 2*H + H - e;
	fragmentions[ms_MCoH].positive = true;
	fragmentions[ms_MCoH].multiplier = 1;
	fragmentions[ms_MCoH].summary = "CoH-2H+";

	// initialize ion [M+Co-2H+Na]+
	fragmentions[ms_MCo2HNa].nterminal = true;
	fragmentions[ms_MCo2HNa].cterminal = true;
	fragmentions[ms_MCo2HNa].name = "[M+Co-2H+Na]+";
	fragmentions[ms_MCo2HNa].massdifference = Co - 2*H + Na - e;
	fragmentions[ms_MCo2HNa].positive = true;
	fragmentions[ms_MCo2HNa].multiplier = 1;
	fragmentions[ms_MCo2HNa].summary = "CoH-2Na+";

	// initialize ion [M+Co-2H+K]+
	fragmentions[ms_MCo2HK].nterminal = true;
	fragmentions[ms_MCo2HK].cterminal = true;
	fragmentions[ms_MCo2HK].name = "[M+Co-2H+K]+";
	fragmentions[ms_MCo2HK].massdifference = Co - 2*H + K - e;
	fragmentions[ms_MCo2HK].positive = true;
	fragmentions[ms_MCo2HK].multiplier = 1;
	fragmentions[ms_MCo2HK].summary = "CoH-2K+";

	// initialize ion [M+Co-3H]-
	fragmentions[ms_MCo3H].nterminal = true;
	fragmentions[ms_MCo3H].cterminal = true;
	fragmentions[ms_MCo3H].name = "[M+Co-3H]-";
	fragmentions[ms_MCo3H].massdifference = Co - 2*H - H + e;
	fragmentions[ms_MCo3H].positive = false;
	fragmentions[ms_MCo3H].multiplier = 1;
	fragmentions[ms_MCo3H].summary = "CoH-2H-1+-1";

	// initialize ion [M+Ni-H]+
	fragmentions[ms_MNiH].nterminal = true;
	fragmentions[ms_MNiH].cterminal = true;
	fragmentions[ms_MNiH].name = "[M+Ni-H]+";
	fragmentions[ms_MNiH].massdifference = Ni - 2*H + H - e;
	fragmentions[ms_MNiH].positive = true;
	fragmentions[ms_MNiH].multiplier = 1;
	fragmentions[ms_MNiH].summary = "NiH-2H+";

	// initialize ion [M+Ni-2H+Na]+
	fragmentions[ms_MNi2HNa].nterminal = true;
	fragmentions[ms_MNi2HNa].cterminal = true;
	fragmentions[ms_MNi2HNa].name = "[M+Ni-2H+Na]+";
	fragmentions[ms_MNi2HNa].massdifference = Ni - 2*H + Na - e;
	fragmentions[ms_MNi2HNa].positive = true;
	fragmentions[ms_MNi2HNa].multiplier = 1;
	fragmentions[ms_MNi2HNa].summary = "NiH-2Na+";

	// initialize ion [M+Ni-2H+K]+
	fragmentions[ms_MNi2HK].nterminal = true;
	fragmentions[ms_MNi2HK].cterminal = true;
	fragmentions[ms_MNi2HK].name = "[M+Ni-2H+K]+";
	fragmentions[ms_MNi2HK].massdifference = Ni - 2*H + K - e;
	fragmentions[ms_MNi2HK].positive = true;
	fragmentions[ms_MNi2HK].multiplier = 1;
	fragmentions[ms_MNi2HK].summary = "NiH-2K+";

	// initialize ion [M+Ni-3H]-
	fragmentions[ms_MNi3H].nterminal = true;
	fragmentions[ms_MNi3H].cterminal = true;
	fragmentions[ms_MNi3H].name = "[M+Ni-3H]-";
	fragmentions[ms_MNi3H].massdifference = Ni - 2*H - H + e;
	fragmentions[ms_MNi3H].positive = false;
	fragmentions[ms_MNi3H].multiplier = 1;
	fragmentions[ms_MNi3H].summary = "NiH-2H-1+-1";

	// initialize ion [M+Cu-H]+
	fragmentions[ms_MCuH].nterminal = true;
	fragmentions[ms_MCuH].cterminal = true;
	fragmentions[ms_MCuH].name = "[M+Cu-H]+";
	fragmentions[ms_MCuH].massdifference = Cu - 2*H + H - e;
	fragmentions[ms_MCuH].positive = true;
	fragmentions[ms_MCuH].multiplier = 1;
	fragmentions[ms_MCuH].summary = "CuH-2H+";

	// initialize ion [M+Cu-2H+Na]+
	fragmentions[ms_MCu2HNa].nterminal = true;
	fragmentions[ms_MCu2HNa].cterminal = true;
	fragmentions[ms_MCu2HNa].name = "[M+Cu-2H+Na]+";
	fragmentions[ms_MCu2HNa].massdifference = Cu - 2*H + Na - e;
	fragmentions[ms_MCu2HNa].positive = true;
	fragmentions[ms_MCu2HNa].multiplier = 1;
	fragmentions[ms_MCu2HNa].summary = "CuH-2Na+";

	// initialize ion [M+Cu-2H+K]+
	fragmentions[ms_MCu2HK].nterminal = true;
	fragmentions[ms_MCu2HK].cterminal = true;
	fragmentions[ms_MCu2HK].name = "[M+Cu-2H+K]+";
	fragmentions[ms_MCu2HK].massdifference = Cu - 2*H + K - e;
	fragmentions[ms_MCu2HK].positive = true;
	fragmentions[ms_MCu2HK].multiplier = 1;
	fragmentions[ms_MCu2HK].summary = "CuH-2K+";

	// initialize ion [M+Cu-3H]-
	fragmentions[ms_MCu3H].nterminal = true;
	fragmentions[ms_MCu3H].cterminal = true;
	fragmentions[ms_MCu3H].name = "[M+Cu-3H]-";
	fragmentions[ms_MCu3H].massdifference = Cu - 2*H - H + e;
	fragmentions[ms_MCu3H].positive = false;
	fragmentions[ms_MCu3H].multiplier = 1;
	fragmentions[ms_MCu3H].summary = "CuH-2H-1+-1";

	// initialize ion [M+Zn-H]+
	fragmentions[ms_MZnH].nterminal = true;
	fragmentions[ms_MZnH].cterminal = true;
	fragmentions[ms_MZnH].name = "[M+Zn-H]+";
	fragmentions[ms_MZnH].massdifference = Zn - 2*H + H - e;
	fragmentions[ms_MZnH].positive = true;
	fragmentions[ms_MZnH].multiplier = 1;
	fragmentions[ms_MZnH].summary = "ZnH-2H+";

	// initialize ion [M+Zn-2H+Na]+
	fragmentions[ms_MZn2HNa].nterminal = true;
	fragmentions[ms_MZn2HNa].cterminal = true;
	fragmentions[ms_MZn2HNa].name = "[M+Zn-2H+Na]+";
	fragmentions[ms_MZn2HNa].massdifference = Zn - 2*H + Na - e;
	fragmentions[ms_MZn2HNa].positive = true;
	fragmentions[ms_MZn2HNa].multiplier = 1;
	fragmentions[ms_MZn2HNa].summary = "ZnH-2Na+";

	// initialize ion [M+Zn-2H+K]+
	fragmentions[ms_MZn2HK].nterminal = true;
	fragmentions[ms_MZn2HK].cterminal = true;
	fragmentions[ms_MZn2HK].name = "[M+Zn-2H+K]+";
	fragmentions[ms_MZn2HK].massdifference = Zn - 2*H + K - e;
	fragmentions[ms_MZn2HK].positive = true;
	fragmentions[ms_MZn2HK].multiplier = 1;
	fragmentions[ms_MZn2HK].summary = "ZnH-2K+";

	// initialize ion [M+Zn-3H]-
	fragmentions[ms_MZn3H].nterminal = true;
	fragmentions[ms_MZn3H].cterminal = true;
	fragmentions[ms_MZn3H].name = "[M+Zn-3H]-";
	fragmentions[ms_MZn3H].massdifference = Zn - 2*H - H + e;
	fragmentions[ms_MZn3H].positive = false;
	fragmentions[ms_MZn3H].multiplier = 1;
	fragmentions[ms_MZn3H].summary = "ZnH-2H-1+-1";

	// initialize ion [M+Ga-2H]+
	fragmentions[ms_MGa2H].nterminal = true;
	fragmentions[ms_MGa2H].cterminal = true;
	fragmentions[ms_MGa2H].name = "[M+Ga-2H]+";
	fragmentions[ms_MGa2H].massdifference = Ga - 3*H + H - e;
	fragmentions[ms_MGa2H].positive = true;
	fragmentions[ms_MGa2H].multiplier = 1;
	fragmentions[ms_MGa2H].summary = "GaH-3H+";

	// initialize ion [M+Ga-3H+Na]+
	fragmentions[ms_MGa3HNa].nterminal = true;
	fragmentions[ms_MGa3HNa].cterminal = true;
	fragmentions[ms_MGa3HNa].name = "[M+Ga-3H+Na]+";
	fragmentions[ms_MGa3HNa].massdifference = Ga - 3*H + Na - e;
	fragmentions[ms_MGa3HNa].positive = true;
	fragmentions[ms_MGa3HNa].multiplier = 1;
	fragmentions[ms_MGa3HNa].summary = "GaH-3Na+";

	// initialize ion [M+Ga-3H+K]+
	fragmentions[ms_MGa3HK].nterminal = true;
	fragmentions[ms_MGa3HK].cterminal = true;
	fragmentions[ms_MGa3HK].name = "[M+Ga-3H+K]+";
	fragmentions[ms_MGa3HK].massdifference = Ga - 3*H + K - e;
	fragmentions[ms_MGa3HK].positive = true;
	fragmentions[ms_MGa3HK].multiplier = 1;
	fragmentions[ms_MGa3HK].summary = "GaH-3K+";

	// initialize ion [M+Ga-4H]-
	fragmentions[ms_MGa4H].nterminal = true;
	fragmentions[ms_MGa4H].cterminal = true;
	fragmentions[ms_MGa4H].name = "[M+Ga-4H]-";
	fragmentions[ms_MGa4H].massdifference = Ga - 3*H - H + e;
	fragmentions[ms_MGa4H].positive = false;
	fragmentions[ms_MGa4H].multiplier = 1;
	fragmentions[ms_MGa4H].summary = "GaH-3H-1+-1";

	// initialize maps of atoms
	for (auto& it : fragmentions) {
		it.second.summarymap.clear();
		addStringFormulaToMap(it.second.summary, it.second.summarymap);
	}
}


void cFragmentIons::store(ofstream& os) {
	int size;

	size = (int)fragmentions.size();
	os.write((char *)&size, sizeof(int));
	for (auto it = fragmentions.begin(); it != fragmentions.end(); ++it) {
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


void initializeFragmentIonsForDeNovoGraphOfLinearPolyketide(vector<eFragmentIonType>& fragmentions) {
	fragmentions.push_back(l1h_ion);
	fragmentions.push_back(l2h_ion);
	fragmentions.push_back(l1oh_ion);
	fragmentions.push_back(l2oh_ion);
	fragmentions.push_back(linear_polyketide_precursor_ion_h_h);
	fragmentions.push_back(linear_polyketide_precursor_ion_h_oh);
	fragmentions.push_back(linear_polyketide_precursor_ion_oh_oh);
}


void initializeFragmentIonsForDeNovoGraphOfCyclicPolyketide(vector<eFragmentIonType>& fragmentions) {
	//fragmentions.push_back(l0h_ion);
	fragmentions.push_back(l1h_ion);
	fragmentions.push_back(l2h_ion);
	fragmentions.push_back(cyclic_polyketide_precursor_ion);
}


void neutralLoss::store(ofstream& os) {
	storeString(summary, os);
	storeStringIntMap(summarymap, os);
	os.write((char *)&massdifference, sizeof(double));
}


void neutralLoss::load(ifstream& is) {
	loadString(summary, is);
	loadStringIntMap(summarymap, is);
	is.read((char *)&massdifference, sizeof(double));
}


cDefaultNeutralLosses::cDefaultNeutralLosses() {
	neutrallosses.push_back("H2O");
	neutrallosses.push_back("NH3");
	neutrallosses.push_back("CO");
	neutrallosses.push_back("CO2");
	neutrallosses.push_back("CONH");
	neutrallosses.push_back("CH2");
	neutrallosses.push_back("C6H4");	// benzene ring
	neutrallosses.push_back("CH2N2");	// Arg loss
	neutrallosses.push_back("CH2O");	// Ser side chain
	neutrallosses.push_back("CH2S");	// Cys side chain
	neutrallosses.push_back("C4H9N");	// Lys side chain
	neutrallosses.push_back("C4H4N2");	// His side chain
	neutrallosses.push_back("C4H9N3");	// Arg side chain
	neutrallosses.push_back("C9H7N");	// Trp side chain
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

