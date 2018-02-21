/**
	\file cFragmentIons.h
	\brief Basic definitions of fragment ions occurring in the mass spectra.
*/


#ifndef _CFRAGMENTIONS_H
#define _CFRAGMENTIONS_H

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <vector>
#include <QMetaType>
#include "core/utilities.h"


using namespace std;


// monoisotopic masses
const double Hplus = 1.00727645;
const double Naplus = 22.989222;
const double Kplus = 38.963158;
const double H = 1.0078250321;
const double D = 2.014102;
const double Li = 7.016004;
const double Be = 9.012182;
const double B = 11.009306;
const double C = 12.0;
const double N = 14.0030740052;
const double O = 15.9949146221;
const double F = 18.998404;
const double Na = 22.989771;
const double Mg = 23.985043;
const double Al = 26.981539;
const double Si = 27.976927;
const double P = 30.97376151;
const double S = 31.97207069;
const double Cl = 34.968853;
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
const double Se = 79.916519;
const double Br = 78.918327;
const double Mo = 97.905411;
const double I = 126.904457;
const double e = H - Hplus;


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


/**
	\brief The types of peptides supported by the application.
*/
enum peptideType {
	linear = 0,
	cyclic = 1,
	branched = 2,
	lasso = 3,
	linearpolysaccharide = 4,
	other = 5
};


/**
	\brief Convert a string to peptide type.
	\param s string
	\retval peptideType type of peptide
*/
peptideType getPeptideTypeFromString(string s);


/**
	\brief Convert the peptide type to a string.
	\param peptidetype type of peptide
	\retval string string
*/
string getStringFromPeptideType(peptideType peptidetype);


/**
	\brief Register peptideType by Qt.
*/
Q_DECLARE_METATYPE(peptideType);


/**
	\brief Convert single charged mz ratio to multiple charged mz ratio.
	\param mass single charged mz ratio
	\param charge new charge
	\retval double multiple charged mzratio
*/ 
double charge(double mass, int charge);


/**
	\brief Convert multiple charged mz ratio to single charged mz ratio.
	\param mass multiple charged mz ratio
	\param charge old charge
	\retval double single charged mzratio
*/ 
double uncharge(double mass, int charge);


/**
	\brief The types of supported fragment ions.
*/
enum fragmentIonType {
	b_ion = 0,
	b_ion_water_loss = 1,
	b_ion_ammonia_loss = 2,
	b_ion_water_and_ammonia_loss = 3,
	a_ion = 4,
	a_ion_water_loss = 5,
	a_ion_ammonia_loss = 6,
	a_ion_water_and_ammonia_loss = 7,
	y_ion = 8,
	y_ion_water_loss = 9,
	y_ion_ammonia_loss = 10,
	y_ion_water_and_ammonia_loss = 11,
	precursor_ion = 12,
	precursor_ion_water_loss = 13,
	precursor_ion_ammonia_loss = 14,
	precursor_ion_water_and_ammonia_loss = 15,
	precursor_ion_co_loss = 16,
	precursor_ion_co_loss_water_loss = 17,
	precursor_ion_co_loss_ammonia_loss = 18,
	precursor_ion_co_loss_water_and_ammonia_loss = 19,
	cyclic_precursor_ion = 20,
	cyclic_precursor_ion_water_loss = 21,
	cyclic_precursor_ion_ammonia_loss = 22,
	cyclic_precursor_ion_water_and_ammonia_loss = 23,
	cyclic_precursor_ion_co_loss = 24,
	cyclic_precursor_ion_co_loss_water_loss = 25,
	cyclic_precursor_ion_co_loss_ammonia_loss = 26,
	cyclic_precursor_ion_co_loss_water_and_ammonia_loss = 27,
	ms_nterminal_ion_hplus = 28,
	ms_nterminal_ion_naplus = 29,
	ms_nterminal_ion_kplus = 30,
	ms_cterminal_ion_hplus = 31,
	ms_cterminal_ion_naplus = 32,
	ms_cterminal_ion_kplus = 33,
	ms_hplus = 34,
	ms_naplus = 35,
	ms_kplus = 36,
	ms_hminus = 37,
	ms_3M2Fe5H = 38,
	ms_2MFe2H = 39,
	ms_3MFe2H = 40,
	ms_MFe2H = 41,
	ms_3M2Fe6HNa = 42,
	ms_2MFe3HNa = 43,
	ms_3MFe3HNa = 44,
	ms_MFe3HNa = 45,
	ms_3M2Fe7H = 46,
	ms_2MFe4H = 47,
	ms_3MFe4H = 48,
	ms_MFe4H = 49,
	//b_ion_2H_loss = 34,
	fragmentIonTypeEnd
};


/**
	\brief Register vector<fragmentIonType> by Qt.
*/
Q_DECLARE_METATYPE(vector<fragmentIonType>);


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
	fragmentIonType parent;


	/**
		\brief True when the fragment is charged positively; false when the fragment is charged negatively.
	*/ 
	bool positive;


	/**
		\brief The multiplier (n) of M in [nM + H]+.
	*/ 
	int multiplier;


	/**
		\brief The default constructor.
	*/ 
	fragmentDescription() {
		name = "";
		summary = "";
		massdifference = 0;
		nterminal = false;
		cterminal = false;
		parent = fragmentIonTypeEnd;
		positive = true;
		multiplier = 1;
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
		\param parent parent fragment type
	*/ 
	fragmentDescription(string name, double massdifference, string summary, bool nterminal, bool cterminal, fragmentIonType parent = fragmentIonTypeEnd, bool positive = true, int multiplier = 1) {
		this->name = name;
		this->massdifference = massdifference;
		this->summary = summary;
		this->nterminal = nterminal;
		this->cterminal = cterminal;
		this->parent = parent;
		this->positive = positive;
		this->multiplier = multiplier;
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

	map<fragmentIonType,fragmentDescription> fragmentions;

public:


	/**
		\brief The constructor.
	*/ 
	cFragmentIons();


	/**
		\brief The constructor.
		\param cyclicnterminus true when the N-terminus of a linear peptide is cyclized
		\param cycliccterminus true when the C-terminus of a linear peptide is cyclized
	*/ 
	cFragmentIons(bool cyclicnterminus, bool cycliccterminus);


	/**
		\brief Overloaded operator [].
		\param iontype type of fragment ion
		\retval fragmentDescription reference to a structure with detailed information about the fragment ion
	*/ 
	fragmentDescription &operator[](fragmentIonType iontype);


	/**
		\brief Recalculate all fragment ion types.
		\param cyclicnterminus true when the N-terminus of a linear peptide is cyclized
		\param cycliccterminus true when the C-terminus of a linear peptide is cyclized
	*/ 
	void recalculateFragments(bool cyclicnterminus, bool cycliccterminus);


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
void initializeFragmentIonsForDeNovoGraphOfCyclicPeptides(vector<fragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of linear peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLinearPeptides(vector<fragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of branched peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfTPeptides(vector<fragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of lasso peptides.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLassoPeptides(vector<fragmentIonType>& fragmentions);


/**
	\brief Initialize fragment ion types for the de novo graph of a custom linear type.
	\param fragmentions reference to a vector of fragment ion types
*/ 
void initializeFragmentIonsForDeNovoGraphOfLinearPolysaccharide(vector<fragmentIonType>& fragmentions);


#endif