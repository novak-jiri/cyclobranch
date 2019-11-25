/**
	\file cBricksDatabase.h
	\brief The database of building blocks.
*/


#ifndef _CBRICKSDATABASE_H
#define _CBRICKSDATABASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>

#include "core/utilities.h"
#include "core/cBrick.h"
#include "core/cSummaryFormula.h"


using namespace std;


class cMainThread;


/**
	\brief Compare masses of two bricks.
	\param a first brick
	\param b second brick
	\retval bool true if the mass of \a a is less than the mass of \a b
*/ 
bool compareBrickMasses(const cBrick& a, const cBrick& b);


/**
	\brief Convert vector of ids of bricks into a string.
	\param composition reference to a string where ids of bricks separated by '-' will be stored
	\param combarray reference to an input integer vector of ids of bricks
*/ 
void getNameOfCompositionFromIntVector(string& composition, vector<int>& combarray);


/**
	\brief Convert vector of ids of bricks into a string having reversed ids of bricks.
	\param composition reference to a string where ids of bricks separated by '-' will be stored
	\param stringcomposition reference to an input string vector of ids of bricks
	\retval string reference to a string where ids of bricks separated by '-' will be stored
*/ 
string& getReversedNameOfCompositionFromStringVector(string& composition, vector<string>& stringcomposition);


/**
	\brief Generate permutations of bricks.
	\param bricks reference to an input vector of strings (ids of bricks)
	\param currentpermutation reference to an empty vector of strings (auxiliary parameter)
	\param permutations reference to an empty output vector of strings where the result will be stored
*/ 
void generateBricksPermutations(vector<string>& bricks, vector<string>& currentpermutation, vector<string>& permutations);


/**
	\brief The class representing a database of building blocks (bricks).
*/
class cBricksDatabase {

	vector<cBrick> bricks;

	map<string, int> lossorders;

	vector<double> lossmasses;
	vector<string> losssummaries;
	vector< map<string, int> > lossmaps;
	
	void addLossToMap(string& lossstr, vector<int>& lossids);

	double getMassFromCombCounts(vector<int>& combcounts, vector<double>& combmasses);


public:
	
	
	/**
		\brief Get next combination of bricks.
		\param combarray reference to an input/output vector of integers
		\param numberofbasicbricks initial number of bricks in a database
		\param maximumbricksincombination maximum number of combined bricks
		\param maximumcumulativemass maximum cummulative mass of combined bricks
		\param neutralprecursormass neutral precursor mass
		\retval bool true when a valid vector is stored in combarray, false when all valid vectors were already generated
	*/
	bool nextCombination(vector<int>& combarray, int numberofbasicbricks, int maximumbricksincombination, double maximumcumulativemass, double neutralprecursormass);


	/**
		\brief Get next combination of bricks.
		\param combarray reference to an input/output vector of integers
		\param combcounts reference to an input/output vector containing counts of elements in \a combarray
		\param combmasses reference to an input vector containing masses of elements in \a combarray
		\param mass sum of masses of blocks
		\param numberofbasicbricks initial number of bricks in a database
		\param maximumbricksincombination maximum number of combined bricks
		\param maximumcumulativemass maximum cummulative mass of combined bricks
		\param neutralprecursormass neutral precursor mass
		\retval bool true when a valid vector is stored in combarray, false when all valid vectors were already generated
	*/
	bool nextCombinationFast(vector<int>& combarray, vector<int>& combcounts, vector<double>& combmasses, double& mass, int numberofbasicbricks, int maximumbricksincombination, double maximumcumulativemass, double neutralprecursormass);


	/**
		\brief Get next combination of bricks.
		\param combarray reference to an input/output vector of integers
		\param combcounts reference to an input/output vector containing counts of elements in \a combarray
		\param comblimits reference to an input vector containing maximum allowed numbers of elements in \a combarray
		\param combmasses reference to an input vector containing masses of elements in \a combarray
		\param mass sum of masses of blocks
		\param numberofbasicbricks initial number of bricks in a database
		\param maximumbricksincombination maximum number of combined bricks
		\param maximumcumulativemass maximum cummulative mass of combined bricks
		\param neutralprecursormass neutral precursor mass 
		\retval bool true when a valid vector is stored in combarray, false when all valid vectors were already generated
	*/ 
	bool nextCombinationFastLimited(vector<int>& combarray, vector<int>& combcounts, vector<int>& comblimits, vector<double>& combmasses, double& mass, int numberofbasicbricks, int maximumbricksincombination, double maximumcumulativemass, double neutralprecursormass);


	/**
		\brief The constructor.
	*/ 
	cBricksDatabase();


	/**
		\brief Load the database of bricks from a plain text stream.
		\param stream reference to an input file stream
		\param errormessage reference to a string where an error message might be stored
		\param ignoreerrors if true errors are ignored
		\param skiph2blocks ignore ketide 2H loss blocks
		\retval int 0 when the database was successfully loaded, -1 when an error occurred (\a errormessage is filled up)
	*/ 
	int loadFromPlainTextStream(ifstream &stream, string& errormessage, bool ignoreerrors, bool skiph2blocks);


	/**
		\brief Store the database of bricks into a plain text stream.
		\param stream reference to an output file stream
	*/ 
	void storeToPlainTextStream(ofstream &stream);


	/**
		\brief Get the number of bricks in the database.
		\retval int number of bricks
	*/ 
	int size();


	/**
		\brief Print the database of bricks.
	*/ 
	void print(/*ostream &stream*/);


	/**
		\brief Sort the database of bricks by mass in ascending order.
	*/ 
	void sortbyMass();


	/**
		\brief Overloaded operator [].
		\param position of the brick in the vector of bricks
		\retval cBrick reference to a brick
	*/ 
	cBrick& operator[](int position);


	/**
		\brief Get the mass of a composition of bricks.
		\param combarray reference to an input integer vector of ids
		\param numberofbasicbricks number of basic building blocks
		\retval double cumulative mass of the bricks in the \a combarray
	*/ 
	double getMassOfComposition(vector<int>& combarray, int numberofbasicbricks);


	/**
		\brief Get the real name of a peptide from a composition.
		\param composition reference to a string containing ids of bricks separated by '-'
		\retval string name of the peptide
	*/ 
	string getRealName(string& composition);


	/**
		\brief Get the name of a peptide composed from acronyms of bricks.
		\param composition reference to a string containing ids of bricks separated by '-'
		\param useHTMLReferences if true then each acronym is converted to a link to its reference (e.g., in ChemSpider)
		\retval string name of the peptide
	*/ 
	string getAcronymName(string& composition, bool useHTMLReferences);


	/**
		\brief Get a peptide sequence tag from a composition.
		\param composition reference to a string containing ids of bricks separated by '-'
		\retval string sequence tag
	*/ 
	string getTagName(string& composition);


	/**
		\brief Get the name of a branched peptide composed from acronyms of bricks.
		\param tcomposition reference to a string containing ids of bricks separated by '-', the branch is specified by '(' and ')'; example: 1-2-3(4-5)6-7
		\param useHTMLReferences if true then each acronym is converted to a link to its reference (e.g., in ChemSpider)
		\retval string name of the peptide
	*/ 
	string getAcronymNameOfTPeptide(const string& tcomposition, bool useHTMLReferences);


	/**
		\brief Get a peptide sequence tag of a branched peptide from a composition.
		\param tcomposition reference to a string containing ids of bricks separated by '-', the branch is specified by '(' and ')'; example: 1-2-3(4-5)6-7
		\retval string sequence tag
	*/ 
	string getTagNameOfTPeptide(string& tcomposition);


	/**
		\brief Clear the vector of bricks.
	*/ 
	void clear();


	/**
		\brief Replace acronyms of bricks by ids.
		\param sequence reference to an input/output string containing acronyms/ids of bricks
		\param errormessage reference to a string where an error message is stored when \a sequence contains an unknown building block
		\retval bool true when the replacement was successful, false when an acronym of a brick is unknown
	*/ 
	bool replaceAcronymsByIDs(string& sequence, string& errormessage);


	/**
		\brief Push a new brick into the vector of bricks.
		\param brick an inserted brick
	*/ 
	void push_back(cBrick& brick);


	/**
		\brief Remove the last brick from the vector of bricks if possible.
	*/ 
	void removeLastBrick();


	/**
		\brief Calculate the summary formula of a composition.
		\param brickid brick id
		\retval string summary formula
	*/
	string calculateSummaryFromComposition(int brickid);


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
		\brief Check if the numbers of H2 loss and H2O loss blocks are correct.
		\param brickseries a tested combination of building blocks
		\param regularblocksorder check regular order of ketide blocks
		\retval bool true when the numbers of blocks are correct
	*/ 
	bool checkKetideBlocks(cBrick& brickseries, bool regularblocksorder);


	/**
		\brief Get the summary mass of neutral losses.
		\param lossids a vector of ids of neutral losses
		\retval double mass
	*/
	double getMassOfNeutralLosses(vector<int>& lossids);


	/**
		\brief Get the summary formula of neutral losses.
		\param lossids a vector of ids of neutral losses
		\retval string formula
	*/
	string getSummaryFormulaOfNeutralLosses(vector<int>& lossids);


	/**
		\brief Get the summary map of neutral losses.
		\param lossids an input vector of ids of neutral losses
		\param summarymap an output map of atoms
	*/
	void getMapOfNeutralLosses(vector<int>& lossids, map<string, int>& summarymap);

};


#endif
