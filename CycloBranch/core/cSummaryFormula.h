/**
	\file cSummaryFormula.h
	\brief The representation of a summary formula.
*/


#ifndef _CSUMMARYFORMULA_H
#define _CSUMMARYFORMULA_H

#include <string>
#include <fstream>
#include "core/cFragmentIons.h"


using namespace std;
using namespace boost;


class cPeaksList;
class cPeakListSeries;
class cBricksDatabase;


/**
	\brief Reduce a map of atoms to a string.
	\param atoms an input map of atoms
	\param formula an output string formula
*/
void reduceMapToStringFormula(map<string, int>& atoms, string& formula);


/**
	\brief Explode and add a summary formula string to a map.
	\param formula an input string formula
	\param atoms an input/output map of atoms
*/
void addStringFormulaToMap(string& formula, map<string, int>& atoms);


/**
	\brief Merge two maps of atoms into a target map.
	\param source a source map
	\param target a target map
*/
void mergeMaps(map<string, int>& source, map<string, int>& target);


/**
	\brief Change the charge from 1+ to another charge in a map.
	\param charge new charge
	\param atoms an input/output map of atoms
*/
void rechargeMap(int charge, map<string, int>& atoms);


/**
	\brief The representation of a summary formula.
*/
class cSummaryFormula {

	bool partial;

	string formula;

	void combineAtoms(cPeakListSeries& peaklistseries, int peaklistseriessize, vector<int>& peaklistseriessizes, cPeaksList& isotopeprofile, vector<int>& depthvector, int depth, double mass, double intensity, int charge, double minimumabsoluteintensity);

	double getIntensity(cBricksDatabase& bricksprobabilities, vector<int>& combarray, int mostintenseid, int mostintenseatomscount, int isotopecount);

	void getIsotopeSummary(string& description, cBricksDatabase& bricks, vector<int>& combarray, int mostintenseid, int mostintenseatomscount, int isotopecount, bool positive);

	void reduceIsotopeProfile(cPeaksList& isotopeprofile, cPeaksList& reducedprofile, double fwhm);

public:


	/**
		\brief The constructor.
	*/ 
	cSummaryFormula();


	/**
		\brief The constructor.
		\param formula a chemical formula
		\param partial true when the formula is incomplete
	*/ 
	cSummaryFormula(string& formula, bool partial = false);


	/**
		\brief Clear the structure.
	*/
	void clear();


	/**
		\brief Set the formula.
		\param formula a chemical formula
		\param partial true when the formula is incomplete
	*/
	void setFormula(const string& formula, bool partial = false);


	/**
		\brief Get the summary formula as a string.
		\retval string formula as a string
	*/
	string getSummary();


	/**
		\brief Get the fancy summary formula as a string.
		\param charge peak charge
		\retval string formula as a string
	*/
	string getFancySummary(int charge);


	/**
		\brief Check if the formula is incomplete or not.
		\retval bool true when the formula is incomplete, false when the formula is complete 
	*/
	bool isPartial();


	/**
		\brief Set the formula as partial.
		\param partial true if the formula is incomplete 
	*/
	void setPartial(bool partial);


	/**
		\brief Add a formula to the existing one.
		\param formula a chemical formula
		\param remove if true then the formula is removed; if false then the formula is added
	*/
	void addFormula(string& formula, bool remove = false);


	/**
		\brief Set the string formula from a map of atoms.
		\param atoms an input map of atoms
		\param partial true if the formula is incomplete 
	*/
	void setFromMap(map<string, int>& atoms, bool partial);

	
	/**
		\brief Check the syntax of a chemical formula.
		\param errormessage an error message if formula is invalid
		\retval bool true when formula is valid
	*/
	bool isValid(string& errormessage);


	/**
		\brief Check if the formula is empty.
		\retval bool true if the formula is empty; false otherwise
	*/
	bool isEmpty();


	/**
		\brief Check if the formula has the numbers of all elements positive.
		\retval bool true if the formula has the numbers of all elements positive
	*/
	bool hasAllElementsPositive();


	/**
		\brief Get mass of the formula.
		\retval double mass of the formula
	*/
	double getMass();


	/**
		\brief Get a theoretical isotope pattern of the formula.
		\param fwhm FWHM
		\param charge charge
		\param positive true if the charge is positive charge, false if the charge is negative
		\param writedescription if true then string descriptions of peaks are filled
		\retval cPeaksList isotopic pattern of the formula
	*/
	cPeaksList getIsotopePattern(double fwhm, int charge, bool positive, bool writedescription);


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


#endif
