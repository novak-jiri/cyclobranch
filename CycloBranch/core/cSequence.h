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

	peptideType peptidetype;
	string sequence;
	string nterminalmodification;
	string cterminalmodification;
	string branchmodification;
	string name;
	cSummaryFormula summary;
	string reference;

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
	*/ 
	void load(ifstream& is);


	/**
		\brief Get peptide type.
		\retval peptideType peptide type
	*/ 
	peptideType getPeptideType();


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
		\brief Get reference.
		\retval string reference
	*/ 
	string& getReference();


	/**
		\brief Set peptide type.
		\param peptidetype peptide type
	*/ 
	void setPeptideType(peptideType peptidetype);


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
	string& getSummaryFormula();

};


#endif
