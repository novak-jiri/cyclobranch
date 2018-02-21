/**
	\file cBrick.h
	\brief The representation of a brick.
*/


#ifndef _CBRICK_H
#define _CBRICK_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "core/utilities.h"

using namespace std;


/**
	\brief Residue loss types.
*/
enum eResidueLossType {
	h2o_loss = 0,
	h2_loss
};


/**
	\brief Get the number of bricks in a composition.
	\param composition string containing ids of bricks separated by '-'
	\retval number number of bricks
*/ 
int getNumberOfBricks(const string& composition);


/**
	\brief The class representing a building block (brick).
*/
class cBrick {
	string name;
	vector<string> acronyms;
	vector<string> references;
	string summary;
	double mass;
	string composition;
	bool artificial;
	eResidueLossType residuelosstype;

public:

	/**
		\brief The constructor.
	*/ 
	cBrick();


	/**
		\brief Clear all variables.
	*/ 
	void clear();


	/**
		\brief Test whether the brick is empty.
		\retval true if the brick is empty
	*/ 
	bool empty();


	/**
		\brief Access to a string variable which stores a full name of the brick.
		\retval reference to a string
	*/ 
	string& getName();


	/**
		\brief Set a full name of the brick.
		\param name reference to a string
	*/ 
	void setName(const string& name);

	
	/**
		\brief Access to a string variable which stores a summary molecular formula of the brick.
		\retval reference to a string
	*/ 
	string& getSummary();


	/**
		\brief Access to a vector of strings which stores acronyms of all izomers corresponding to the brick.
		\retval reference to a vector of strings
	*/ 
	vector<string>& getAcronyms();


	/**
		\brief Access to a vector of strings which stores references (e.g., to ChemSpider) of all izomers corresponding to the brick.
		\retval reference to a vector of strings
	*/ 
	vector<string>& getReferences();


	/**
		\brief Access to a string containing composition of the brick (ids of bricks separated by '-')
		\retval reference to a string
	*/ 
	string& getComposition();


	/**
		\brief Reverse the composition of the brick.
		\retval string with reverted composition of bricks (ids of bricks separated by '-')
	*/ 
	string getReverseComposition();


	/**
		\brief Get the mass of the brick.
		\retval double mass
	*/ 
	double getMass() const;


	/**
		\brief Set the mass of the brick.
		\param mass mass of the brick
	*/ 
	void setMass(double mass);


	/**
		\brief Set the composition of the brick.
		\param composition reference to a string with composition (ids of bricks separated by '-')
		\param normalize if true then ids of bricks are sorted in ascending order
	*/ 
	void setComposition(const string& composition, bool normalize);


	/**
		\brief Set the summary molecular formula of the brick.
		\param summary reference to a string
	*/ 
	void setSummary(const string& summary);


	/**
		\brief Set acronyms of all izomers corresponding to the brick.
		\param acronyms reference to a string where the acronyms are separated by '/'
	*/ 
	void setAcronyms(const string& acronyms);


	/**
		\brief Set references (e.g., to ChemSpider) of all izomers corresponding to the brick.
		\param references reference to a string where references to izomers are separated by '/'
	*/ 
	void setReferences(const string& references);


	/**
		\brief Normalize the composition of the brick (ids of bricks are sorted in ascending order).
	*/ 
	void normalizeComposition();


	/**
		\brief Explode the composition of the brick into a vector of intergers (ids of subbricks).
		\param bricks reference to a vector of integers
	*/ 
	void explodeToIntComposition(vector<int>& bricks);


	/**
		\brief Explode the composition of the brick into a vector of strings (ids of subbricks).
		\param bricks reference to a vector of strings
	*/ 
	void explodeToStringComposition(vector<string>& bricks);


	/**
		\brief Get all acronyms.
		\retval string acronyms of all izomers separated by '/'
	*/ 
	string getAcronymsAsString();
	

	/**
		\brief Get all references.
		\retval string references of all izomers separated by '/'
	*/ 
	string getReferencesAsString();
	

	/**
		\brief Get the first acronym.
		\retval string acronym of the first izomer
	*/ 
	string getFirstAcronymAsString();


	/**
		\brief Get all acronyms with HTML links to theirs references.
		\retval string acronyms of all izomers with HTML references separated by '/'
	*/ 
	string getAcronymsWithReferencesAsHTMLString();


	/**
		\brief Set a flag determining whether the brick is a calculated mass block or a regular brick.
		\param artificial true when the brick is a calculated mass block; false when the brick is a regular brick.
	*/
	void setArtificial(bool artificial);


	/**
		\brief Get a flag determining whether the brick is a calculated mass block or a regular brick.
		\retval bool true when the brick is a calculated mass block; false when the brick is a regular brick.
	*/
	bool isArtificial();


	/**
		\brief Set the residue loss type.
		\param residuelosstype the residue loss type.
	*/
	void setResidueLossType(eResidueLossType residuelosstype);


	/**
		\brief Get the residue loss type.
		\retval eResidueLossType the residue loss type.
	*/
	eResidueLossType getResidueLossType();


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

