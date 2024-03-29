/**
	\file cSequence.h
	\brief The representation of a sequence.
*/


#ifndef _CSEQUENCE_H
#define _CSEQUENCE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "core/utilities.h"
#include "core/cFragmentIons.h"
#include "core/cSummaryFormula.h"


using namespace std;
using namespace boost;


/**
	\brief The class representing a sequence.
*/
class cSequence {

	ePeptideType peptidetype;
	string sequence;
	string nterminalmodification;
	string cterminalmodification;
	string branchmodification;
	string name;
	cSummaryFormula summary;
	string reference;
	double rtmin;
	double rtmax;
	bool decoy;

public:


	/**
		\brief The constructor.
	*/ 
	cSequence();


	/**
		\brief Clear the structure.
	*/ 
	void clear();


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
		\param fileversionpart1 first number of .res the file version
		\param fileversionpart2 second number of .res the file version
		\param fileversionpart3 third number of .res the file version
	*/ 
	void load(ifstream& is, int fileversionpart1, int fileversionpart2, int fileversionpart3);


	/**
		\brief Get peptide type.
		\retval ePeptideType peptide type
	*/ 
	ePeptideType getPeptideType();


	/**
		\brief Get sequence.
		\retval string sequence
	*/ 
	string& getSequence();


	/**
		\brief Get name.
		\retval string name
	*/ 
	string& getName();


	/**
		\brief Get minimum retention time.
		\retval double minimum retention time
	*/
	double getMinimumRetentionTime();


	/**
		\brief Get maximum retention time.
		\retval double maximum retention time
	*/
	double getMaximumRetentionTime();


	/**
		\brief Get reference.
		\retval string reference
	*/ 
	string& getReference();


	/**
		\brief Set peptide type.
		\param peptidetype peptide type
	*/ 
	void setPeptideType(ePeptideType peptidetype);


	/**
		\brief Set sequence.
		\param sequence sequence
	*/ 
	void setSequence(const string& sequence);


	/**
		\brief Set name.
		\param name name
	*/ 
	void setName(const string& name);


	/**
		\brief Set retention time.
		\param rtmin minimum retention time
		\param rtmax maximum retention time
	*/
	void setRetentionTime(double rtmin, double rtmax);


	/**
		\brief Set reference.
		\param reference reference
	*/ 
	void setReference(const string& reference);


	/**
		\brief Set N-terminal modification.
		\param modification name of an N-terminal modification
	*/ 
	void setNTterminalModification(const string& modification);


	/**
		\brief Set C-terminal modification.
		\param modification name of a C-terminal modification
	*/ 
	void setCTterminalModification(const string& modification);


	/**
		\brief Set branch modification.
		\param modification name of a branch modification
	*/ 
	void setBranchModification(const string& modification);


	/**
		\brief Get N-terminal modification.
		\retval string name of N-terminal modification
	*/ 
	string& getNTterminalModification();


	/**
		\brief Get C-terminal modification.
		\retval string name of C-terminal modification
	*/ 
	string& getCTterminalModification();


	/**
		\brief Get branch modification.
		\retval string name of a branch modification
	*/ 
	string& getBranchModification();


	/**
		\brief Get name of sequence as a HTML link to sequence reference.
		\retval string HTML link
	*/ 
	string getNameWithReferenceAsHTMLString();


	/**
		\brief Set the summary formula.
		\param formula summary formula
	*/ 
	void setSummaryFormula(const string& formula);


	/**
		\brief Get the summary formula.
		\retval string summary formula
	*/
	string getSummaryFormula();


	/**
		\brief Check if sequence/compound is decoy.
		\retval bool true if sequence is decoy, false otherwise
	*/
	bool isDecoy();


	/**
		\brief Set decoy flag.
		\param decoy true if sequence is decoy, false otherwise 
	*/
	void setDecoy(bool decoy);

};


#endif
