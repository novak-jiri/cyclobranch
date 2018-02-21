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


/**
	\brief The representation of a summary formula.
*/
class cSummaryFormula {

	string formula;

	void explodeSummary(map<string, int>& atoms, string& summary);

public:


	/**
		\brief The constructor.
	*/ 
	cSummaryFormula();


	/**
		\brief The constructor.
		\brief formula a chemical formula
	*/ 
	cSummaryFormula(string& formula);


	/**
		\brief Clear the structure.
	*/
	void clear();


	/**
		\brief Set the formula.
		\param formula a chemical formula
	*/
	void setFormula(string& formula);


	/**
		\brief Get the formula.
		\retval string reference to the formula
	*/
	string& getFormula();


	/**
		\brief Add a formula to the existing one.
		\param formula a chemical formula
	*/
	void addFormula(string& formula);

	
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