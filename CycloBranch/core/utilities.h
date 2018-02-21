/**
	\file utilities.h
	\brief Auxiliary funtions and structures.
*/


#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <fstream>
#include <vector>
#include <string>
#include <QString>
#include <boost/regex.hpp>

class cBrick;


#define WIN 0   // Windows
#define UNX 1   // Linux/UNIX

#ifdef UNIX
	#define OS_TYPE UNX
	#define sscanf_s sscanf
	#define sprintf_s sprintf
#else
	#define OS_TYPE WIN
#endif


using namespace std;
using namespace boost;


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
	\brief The name of the application.
*/
extern QString appname;


/**
	\brief The version of the application.
*/
extern QString appversion;


/**
	\brief A directory where the application is installed in Linux.
*/
extern QString linuxinstalldir;


/**
	\brief Store a string into an output stream.
	\param s reference to a string
	\param os reference to an output stream
*/ 
void storeString(string& s, ofstream& os);


/**
	\brief Load a string from an input stream.
	\param s reference to a string
	\param is reference to an input stream
*/ 
void loadString(string& s, ifstream& is);


/**
	\brief Store a vector of strings into an output stream.
	\param v reference to a vector of strings
	\param os reference to an output stream
*/ 
void storeStringVector(vector<string>& v, ofstream& os);


/**
	\brief Load a vector of strings from an input stream.
	\param v reference to a vector of strings
	\param is reference to an input stream
*/ 
void loadStringVector(vector<string>& v, ifstream& is);


/**
	\brief Check if \a c is a white space (except normal space).
	\param c char
	\retval bool true if \a c is a white space (except normal space); false otherwise.
*/ 
bool isWhiteSpaceExceptSpace(char c);


/**
	\brief Remove white spaces except normal spaces from a string.
	\param s reference to a string
	\retval string reference to the input string
*/ 
string& removeWhiteSpacesExceptSpaces(string& s);


/**
	\brief Check the syntax of a peptide sequence.
	\param peptidetype peptide type
	\param sequence peptide sequence
	\param errormessage an error message if false is returned
	\retval bool true when the syntax is correct, false otherwise
*/ 
bool checkRegex(peptideType peptidetype, string& sequence, string& errormessage);


/**
	\brief Parse a position of a branch from a peptide sequence.
	\param peptidetype peptide type
	\param composition a peptide sequence where all blocks "[blockX]-[blockY]" were replaced by ids "1-2"; but "\(" and "\)" are still included
	\param vectorcomposition an output vector with ids of blocks represented as strings
	\param branchstart start position of a branch (an output value)
	\param branchend end position of a branch (an output value)
*/ 
void parseBranch(peptideType peptidetype, string& composition, vector<string>& vectorcomposition, int& branchstart, int& branchend);


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


#endif

