/**
	\file cFragmentIons.h
	\brief Basic definitions of fragment ions occurring in the mass spectra.
*/


#ifndef _CFRAGMENTIONS_H
#define _CFRAGMENTIONS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
#include <QMetaType>
#include "core/utilities.h"


using namespace std;


// monoisotopic masses
const double Hplus = 1.00727645;
const double Naplus = 22.989222;
const double Kplus = 38.963158;
const double Liplus = 7.015455;
const double H = 1.0078250321;
const double D = 2.014102;
const double T = 3.016049;
const double Li = 7.016004;
const double Be = 9.012182;
const double B = 11.009306;
const double C = 12.0;
const double N = 14.0030740052;
const double O = 15.9949146221;
const double F = 18.998404;
const double Ne = 19.992439;
const double Na = 22.989771;
const double Mg = 23.985043;
const double Al = 26.981539;
const double Si = 27.976927;
const double P = 30.97376151;
const double S = 31.97207069;
const double Cl = 34.968853;
const double Ar = 39.962383;
const double K = 38.963707;
const double Ca = 39.962589;
const double Sc = 44.95591;
const double Ti = 47.947948;
const double V = 50.943962;
const double Cr = 51.940514;
const double Mn = 54.938049;
const double Fe = 55.93494;
const double Co = 58.933201;
const double Ni = 57.935349;
const double Cu = 62.9296;
const double Zn = 63.929146;
const double Ga = 68.925583;
const double Ge = 73.921181;
const double As = 74.921600;
const double Se = 79.916519;
const double Br = 78.918327;
const double Kr = 83.911507;
const double Rb = 84.911789;
const double Sr = 87.905617;
const double Y = 88.905846;
const double Zr = 89.904701;
const double Nb = 92.906403;
const double Mo = 97.905411;
const double Tc = 97.907219;
const double Ru = 101.904388;
const double Rh = 102.905502;
const double Pd = 105.903481;
const double Ag = 106.905090;
const double Cd = 113.903358;
const double In = 114.903877;
const double Sn = 119.902199;
const double Sb = 120.903824;
const double Te = 129.906219;
const double I = 126.904457;
const double Xe = 131.904160;
const double Cs = 132.905441;
const double Ba = 137.905243;
const double La = 138.906342;
const double Ce = 139.905441;
const double Pr = 140.907593;
const double Nd = 141.907700;
const double Pm = 144.912750;
const double Sm = 151.919693;
const double Eu = 152.921204;
const double Gd = 157.924103;
const double Tb = 158.925293;
const double Dy = 163.929199;
const double Ho = 164.930298;
const double Er = 165.930298;
const double Tm = 168.934204;
const double Yb = 173.938904;
const double Lu = 174.940796;
const double Hf = 179.946503;
const double Ta = 180.947998;
const double W = 183.950897;
const double Re = 186.955597;
const double Os = 191.961502;
const double Ir = 192.962906;
const double Pt = 194.964798;
const double Au = 196.966507;
const double Hg = 201.970596;
const double Tl = 204.974396;
const double Pb = 207.976593;
const double Bi = 208.980392;
const double Po = 208.982422;
const double At = 209.987137;
const double Rn = 222.017563;
const double Fr = 209.996399;
const double Ra = 226.025406;
const double Ac = 227.027740;
const double Th = 232.038055;
const double Pa = 231.035904;
const double U = 238.050781;
const double Np = 237.048172;
const double Pu = 242.058746;
const double Am = 243.061371;
const double Cm = 247.070343;
const double Bk = 247.070297;
const double Cf = 251.079575;
const double Es = 252.082977;
const double Fm = 257.095093;
const double Md = 258.098419;
const double No = 259.101044;
const double Lr = 262.109802;

const double R = 0;
const double X = 0;
const double e = H - Hplus;

// if isotopes are changed then fragmentions[xxx].numberofisotopes must be also fixed !
const double Li6 = 6.015122;		// 7.5%
const double Mg25 = 24.985837;		// 10%
const double Mg26 = 25.982593;		// 11.01%
const double K41 = 40.961825;		// 6.73%
const double Cr50 = 49.946046;		// 4.345%
const double Cr53 = 52.940651;		// 9.501%
const double Fe54 = 53.939612;		// 5.8%
const double Ni60 = 59.930788;		// 26.223%
const double Ni62 = 61.928346;		// 3.634%
const double Cu65 = 64.927793;		// 30.85%
const double Zn66 = 65.926034;		// 27.9%
const double Zn67 = 66.927129;		// 4.1%
const double Zn68 = 67.924846;		// 18.8%
const double Ga71 = 70.9247;		// 39.89%


/**
	\brief The class representing a periodic table.
*/
class cPeriodicTableMap {

	map<string, double> table;

public:
	

	/**
		\brief The constructor.
	*/ 
	cPeriodicTableMap();


	/**
		\brief Overloaded operator [].
		\param element reference to a string with a name of an element
		\retval double mass of an element
	*/ 
	double &operator[](string& element);


	/**
		\brief Check whether an element occurs in the table.
		\param element reference to a string with a name of an element
		\retval int 1 when element is presented in the table, 0 otherwise
	*/ 
	int count(string& element);

};


const double H2O = 2 * H + O;
const double NH3 = 3 * H + N;
const double CO = C + O;

const double A_ION = Hplus - C - O;
const double B_ION = Hplus;
const double C_ION = Hplus + NH3;
const double X_ION = Hplus + 2 * O + C;
const double Y_ION = Hplus + H2O;
const double Z_ION = Hplus + O - N - H;

const double PRECURSOR_ION = Hplus + H2O;
const double PRECURSOR_ION_CYCLIC = Hplus;

const double L0H_ION = Hplus - 2 * H;
const double L1H_ION = Hplus;
const double L2H_ION = Hplus + 2 * H;
const double R1H_ION = Hplus;
const double R2H_ION = Hplus + 2 * H;
const double L1OH_ION = Hplus + O;
const double L2OH_ION = Hplus + H2O;
const double R1OH_ION = Hplus + O;
const double R2OH_ION = Hplus + H2O;

const double PRECURSOR_ION_LINEAR_POLYKETIDE_H_H = Hplus + 2 * H;
const double PRECURSOR_ION_LINEAR_POLYKETIDE_H_OH = Hplus + H2O;
const double PRECURSOR_ION_LINEAR_POLYKETIDE_OH_OH = Hplus + 2 * H + 2 * O;


/**
	\brief Register ePeptideType by Qt.
*/
Q_DECLARE_METATYPE(ePeptideType);


/**
	\brief Get mzratio from neutral mass.
	\param neutralmass neutral mass
	\param charge charge
	\retval double mzratio
*/ 
double charge(double neutralmass, int charge);


/**
	\brief Get neutral mass from mzratio.
	\param mzratio mzratio
	\param charge charge
	\retval double neutral mass
*/ 
double uncharge(double mzratio, int charge);


/**
	\brief The types of supported fragment ions.
*/
enum eFragmentIonType {
	a_ion = 1,
	a_ion_dehydrated,
	a_ion_deamidated,
	a_ion_dehydrated_and_deamidated,
	b_ion,
	b_ion_dehydrated,
	b_ion_deamidated,
	b_ion_dehydrated_and_deamidated,
	c_ion,
	c_ion_dehydrated,
	c_ion_deamidated,
	c_ion_dehydrated_and_deamidated,
	x_ion,
	x_ion_dehydrated,
	x_ion_deamidated,
	x_ion_dehydrated_and_deamidated,
	y_ion,
	y_ion_dehydrated,
	y_ion_deamidated,
	y_ion_dehydrated_and_deamidated,
	z_ion,
	z_ion_dehydrated,
	z_ion_deamidated,
	z_ion_dehydrated_and_deamidated,
	precursor_ion,
	precursor_ion_dehydrated,
	precursor_ion_deamidated,
	precursor_ion_dehydrated_and_deamidated,
	precursor_ion_co_loss,
	precursor_ion_co_loss_and_dehydrated,
	precursor_ion_co_loss_and_deamidated,
	precursor_ion_co_loss_and_dehydrated_and_deamidated,
	cyclic_precursor_ion,
	cyclic_precursor_ion_dehydrated,
	cyclic_precursor_ion_deamidated,
	cyclic_precursor_ion_dehydrated_and_deamidated,
	cyclic_precursor_ion_co_loss,
	cyclic_precursor_ion_co_loss_and_dehydrated,
	cyclic_precursor_ion_co_loss_and_deamidated,
	cyclic_precursor_ion_co_loss_and_dehydrated_and_deamidated,
	ms_hplus,
	ms_naplus,
	ms_kplus,
	ms_hminus,
	ms_MFe2H,
	ms_MFe3HNa,
	ms_2MFe2H,
	ms_2MFe3HNa,
	ms_3MFe2H,
	ms_3MFe3HNa,
	ms_3M2Fe5H,
	ms_3M2Fe6HNa,
	ms_MFe4H,
	ms_2MFe4H,
	ms_3MFe4H,
	ms_3M2Fe7H,
	ms_liplus,
	ms_MMgH,
	ms_MMg2HNa,
	ms_MMg3H,
	ms_MAl2H,
	ms_MAl3HNa,
	ms_MAl4H,
	ms_MCaH,
	ms_MCa2HNa,
	ms_MCa3H,
	ms_MCr2H,
	ms_MCr3HNa,
	ms_MCr4H,
	ms_MMnH,
	ms_MMn2HNa,
	ms_MMn3H,
	ms_MCoH,
	ms_MCo2HNa,
	ms_MCo3H,
	ms_MNiH,
	ms_MNi2HNa,
	ms_MNi3H,
	ms_MCuH,
	ms_MCu2HNa,
	ms_MCu3H,
	ms_MZnH,
	ms_MZn2HNa,
	ms_MZn3H,
	ms_MGa2H,
	ms_MGa3HNa,
	ms_MGa4H,
	l0h_ion,
	l0h_ion_dehydrated,
	l0h_ion_deamidated,
	l0h_ion_dehydrated_and_deamidated,
	l0h_ion_co_loss,
	l0h_ion_co_loss_dehydrated,
	l0h_ion_co_loss_deamidated,
	l0h_ion_co_loss_dehydrated_and_deamidated,
	l1h_ion,
	l1h_ion_dehydrated,
	l1h_ion_deamidated,
	l1h_ion_dehydrated_and_deamidated,
	l1h_ion_co_loss,
	l1h_ion_co_loss_dehydrated,
	l1h_ion_co_loss_deamidated,
	l1h_ion_co_loss_dehydrated_and_deamidated,
	l2h_ion,
	l2h_ion_dehydrated,
	l2h_ion_deamidated,
	l2h_ion_dehydrated_and_deamidated,
	l2h_ion_co_loss,
	l2h_ion_co_loss_dehydrated,
	l2h_ion_co_loss_deamidated,
	l2h_ion_co_loss_dehydrated_and_deamidated,
	r1h_ion,
	r1h_ion_dehydrated,
	r1h_ion_deamidated,
	r1h_ion_dehydrated_and_deamidated,
	r1h_ion_co_loss,
	r1h_ion_co_loss_dehydrated,
	r1h_ion_co_loss_deamidated,
	r1h_ion_co_loss_dehydrated_and_deamidated,
	r2h_ion,
	r2h_ion_dehydrated,
	r2h_ion_deamidated,
	r2h_ion_dehydrated_and_deamidated,
	r2h_ion_co_loss,
	r2h_ion_co_loss_dehydrated,
	r2h_ion_co_loss_deamidated,
	r2h_ion_co_loss_dehydrated_and_deamidated,
	l1oh_ion,
	l1oh_ion_dehydrated,
	l1oh_ion_deamidated,
	l1oh_ion_dehydrated_and_deamidated,
	l1oh_ion_co_loss,
	l1oh_ion_co_loss_dehydrated,
	l1oh_ion_co_loss_deamidated,
	l1oh_ion_co_loss_dehydrated_and_deamidated,
	l2oh_ion,
	l2oh_ion_dehydrated,
	l2oh_ion_deamidated,
	l2oh_ion_dehydrated_and_deamidated,
	l2oh_ion_co_loss,
	l2oh_ion_co_loss_dehydrated,
	l2oh_ion_co_loss_deamidated,
	l2oh_ion_co_loss_dehydrated_and_deamidated,
	r1oh_ion,
	r1oh_ion_dehydrated,
	r1oh_ion_deamidated,
	r1oh_ion_dehydrated_and_deamidated,
	r1oh_ion_co_loss,
	r1oh_ion_co_loss_dehydrated,
	r1oh_ion_co_loss_deamidated,
	r1oh_ion_co_loss_dehydrated_and_deamidated,
	r2oh_ion,
	r2oh_ion_dehydrated,
	r2oh_ion_deamidated,
	r2oh_ion_dehydrated_and_deamidated,
	r2oh_ion_co_loss,
	r2oh_ion_co_loss_dehydrated,
	r2oh_ion_co_loss_deamidated,
	r2oh_ion_co_loss_dehydrated_and_deamidated,
	linear_polyketide_precursor_ion_h_h,
	linear_polyketide_precursor_ion_h_h_dehydrated,
	linear_polyketide_precursor_ion_h_h_deamidated,
	linear_polyketide_precursor_ion_h_h_dehydrated_and_deamidated,
	linear_polyketide_precursor_ion_h_h_co_loss,
	linear_polyketide_precursor_ion_h_h_co_loss_dehydrated,
	linear_polyketide_precursor_ion_h_h_co_loss_deamidated,
	linear_polyketide_precursor_ion_h_h_co_loss_dehydrated_and_deamidated,
	linear_polyketide_precursor_ion_h_oh,
	linear_polyketide_precursor_ion_h_oh_dehydrated,
	linear_polyketide_precursor_ion_h_oh_deamidated,
	linear_polyketide_precursor_ion_h_oh_dehydrated_and_deamidated,
	linear_polyketide_precursor_ion_h_oh_co_loss,
	linear_polyketide_precursor_ion_h_oh_co_loss_dehydrated,
	linear_polyketide_precursor_ion_h_oh_co_loss_deamidated,
	linear_polyketide_precursor_ion_h_oh_co_loss_dehydrated_and_deamidated,
	linear_polyketide_precursor_ion_oh_oh,
	linear_polyketide_precursor_ion_oh_oh_dehydrated,
	linear_polyketide_precursor_ion_oh_oh_deamidated,
	linear_polyketide_precursor_ion_oh_oh_dehydrated_and_deamidated,
	linear_polyketide_precursor_ion_oh_oh_co_loss,
	linear_polyketide_precursor_ion_oh_oh_co_loss_dehydrated,
	linear_polyketide_precursor_ion_oh_oh_co_loss_deamidated,
	linear_polyketide_precursor_ion_oh_oh_co_loss_dehydrated_and_deamidated,
	cyclic_polyketide_precursor_ion,
	cyclic_polyketide_precursor_ion_dehydrated,
	cyclic_polyketide_precursor_ion_deamidated,
	cyclic_polyketide_precursor_ion_dehydrated_and_deamidated,
	cyclic_polyketide_precursor_ion_co_loss,
	cyclic_polyketide_precursor_ion_co_loss_dehydrated,
	cyclic_polyketide_precursor_ion_co_loss_deamidated,
	cyclic_polyketide_precursor_ion_co_loss_dehydrated_and_deamidated,
	fragmentIonTypeEnd
};


/**
	\brief Register vector<eFragmentIonType> by Qt.
*/
Q_DECLARE_METATYPE(vector<eFragmentIonType>);


/**
	\brief The structure for a detailed definition of a fragment ion type.
*/
struct fragmentDescription {

	/**
		\brief Name of the fragment.
	*/ 
	string name;


	/**
		\brief Summary formula of the fragment.
	*/ 
	string summary;


	/**
		\brief Mass difference of the fragment.
	*/ 
	double massdifference;


	/**
		\brief True when the fragment is N-terminal.
	*/ 
	bool nterminal;


	/**
		\brief True when the fragment is C-terminal.
	*/ 
	bool cterminal;


	/**
		\brief Parent fragment type.
	*/ 
	eFragmentIonType parent;


	/**
		\brief True when the fragment is charged positively; false when the fragment is charged negatively.
	*/ 
	bool positive;


	/**
		\brief The multiplier (n) of M in [nM + H]+.
	*/ 
	int multiplier;


	/**
		\brief Number of implemented isotopes of a biometal ion.
	*/ 
	int numberofisotopes;


	/**
		\brief The default constructor.
	*/ 
	fragmentDescription() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/ 
	void clear() {
		name = "";
		summary = "";
		massdifference = 0;
		nterminal = false;
		cterminal = false;
		parent = fragmentIonTypeEnd;
		positive = true;
		multiplier = 1;
		numberofisotopes = 0;
	}


	/**
		\brief The constructor.
		\param name name of the fragment
		\param massdifference mass difference of the fragment
		\param summary summary formula of the fragment
		\param nterminal true when the fragment is N-terminal
		\param cterminal true when the fragment is C-terminal
		\param positive true when the fragment is charged positively; false when the fragment is charged negatively
		\param multiplier the multiplier (n) of M in [nM + H]+
		\param numberofisotopes the number of implemented isotopes of a biometal ion
		\param parent parent fragment type
	*/ 
	fragmentDescription(string name, double massdifference, string summary, bool nterminal, bool cterminal, eFragmentIonType parent = fragmentIonTypeEnd, bool positive = true, int multiplier = 1, int numberofisotopes = 0) {
		this->name = name;
		this->massdifference = massdifference;
		this->summary = summary;
		this->nterminal = nterminal;
		this->cterminal = cterminal;
		this->parent = parent;
		this->positive = positive;
		this->multiplier = multiplier;
		this->numberofisotopes = numberofisotopes;
	}


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);

};


/**
	\brief Register vector<fragmentDescription> by Qt.
*/
Q_DECLARE_METATYPE(vector<fragmentDescription>);


/**
	\brief The detailed definitions of all supported fragment ion types.
*/
class cFragmentIons {

	map<eFragmentIonType,fragmentDescription> fragmentions;

public:


	/**
		\brief The constructor.
	*/ 
	cFragmentIons();


	/**
		\brief The constructor.
		\param cyclicnterminus true when the N-terminus of a linear peptide is cyclized
		\param cycliccterminus true when the C-terminus of a linear peptide is cyclized
		\param precursoradduct an adduct of a precursor ion
	*/ 
	cFragmentIons(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct);


	/**
		\brief Overloaded operator [].
		\param iontype type of fragment ion
		\retval fragmentDescription reference to a structure with detailed information about the fragment ion
	*/ 
	fragmentDescription &operator[](eFragmentIonType iontype);


	/**
		\brief Recalculate all fragment ion types.
		\param cyclicnterminus true when the N-terminus of a linear peptide is cyclized
		\param cycliccterminus true when the C-terminus of a linear peptide is cyclized
		\param precursoradduct an adduct of a precursor ion
	*/ 
	void recalculateFragments(bool cyclicnterminus, bool cycliccterminus, string& precursoradduct);


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);

};


/**
	\brief Register cFragmentIons by Qt.
*/
Q_DECLARE_METATYPE(cFragmentIons);                   


/**
	\brief Initialize fragment ion types for the de novo graph of cyclic peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfCyclicPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of linear peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLinearPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of branched peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfTPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of branch-cyclic peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfBranchCyclicPeptides(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of a linear polyketide
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLinearPolyketide(vector<eFragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of a cyclic polyketide
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfCyclicPolyketide(vector<eFragmentIonType>& fragmentions);


/**
	\brief Load modifications from a plain text stream.
	\param stream reference to an input file stream
	\param modifications reference to a vector where modifications will be placed
	\param errormessage reference to a string where an error message might be stored
	\param ignoreerrors if true errors are ignored
	\retval int 0 when modifications were successfully loaded, -1 when an error occurred (\a errormessage is filled up)
*/ 
int loadModificationsFromPlainTextStream(ifstream &stream, vector<fragmentDescription>& modifications, string& errormessage, bool ignoreerrors);


/**
	\brief Store modifications into a plain text stream.
	\param stream reference to an output file stream
	\param modifications reference to a vector of modifications which will be stored
*/ 
void storeModificationsToPlainTextStream(ofstream &stream, vector<fragmentDescription>& modifications);


#endif

