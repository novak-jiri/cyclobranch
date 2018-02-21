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
	\brief The representation of a summary formula.
*/
class cSummaryFormula {

	bool partial;

	string formula;

	void explodeSummary(map<string, int>& atoms, string& summary);

	void combineAtoms(cPeakListSeries& peaklistseries, cPeaksList& isotopeprofile, int depth, double mass, double intensity, int charge, string description, double minimumabsoluteintensity);

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
		\brief Check the syntax of a chemical formula.
		\param errormessage an error message if formula is invalid
		\retval bool true when formula is valid
	*/
	bool isValid(string& errormessage);


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
		\retval cPeaksList isotopic pattern of the formula
	*/
	cPeaksList getIsotopePattern(double fwhm, int charge, bool positive);


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
