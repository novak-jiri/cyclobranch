/**
	\file utilities.h
	\brief Auxiliary funtions and structures.
*/


#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include <QString>
#include <QByteArray>
#include <QMetaType>
#include <boost/regex.hpp>

class cBrick;


#define WIN 0   // Windows
#define UNX 1   // Linux/UNIX
#define OSX 2	// Mac OS X


#ifdef LINUX
	#define OS_TYPE UNX
	#define sscanf_s sscanf
	#define sprintf_s sprintf
#else
	#ifdef MACOSX
		#define OS_TYPE OSX
		#define sscanf_s sscanf
		#define sprintf_s sprintf
	#else
		#define OS_TYPE WIN
	#endif
#endif


#define MICROSCOPY_IMAGES


using namespace std;
using namespace boost;


/**
	\brief The pi value.
*/
const double pi = 3.141592653589793;


/**
	\brief Vendor types.
*/
enum eVendorType {
	unknownvendor = 0,
	bruker = 1,
	waters = 2
};


/**
	\brief Running modes of the application.
*/
enum eModeType {
	denovoengine = 0,
	singlecomparison = 1,
	databasesearch = 2,
	dereplication = 3
};


/**
	\brief The types of peptides supported by the application.
*/
enum ePeptideType {
	linear = 0,
	cyclic = 1,
	branched = 2,
	branchcyclic = 3,
	linearpolyketide = 4,
	cyclicpolyketide = 5,
	other = 6
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
	\brief A directory where the application is installed under Linux or OSX.
*/
extern QString installdir;


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
bool checkRegex(ePeptideType peptidetype, string& sequence, string& errormessage);


/**
	\brief Parse a position of a branch from a peptide sequence.
	\param peptidetype peptide type
	\param composition a peptide sequence where all blocks "[blockX]-[blockY]" were replaced by ids "1-2"; but "\(" and "\)" are still included
	\param vectorcomposition an output vector with ids of blocks represented as strings
	\param branchstart start position of a branch (an output value)
	\param branchend end position of a branch (an output value)
*/ 
void parseBranch(ePeptideType peptidetype, string& composition, vector<string>& vectorcomposition, int& branchstart, int& branchend);


/**
	\brief Convert a string to peptide type.
	\param s string
	\retval ePeptideType type of peptide
*/
ePeptideType getPeptideTypeFromString(string& s);


/**
	\brief Convert the peptide type to a string.
	\param peptidetype type of peptide
	\retval string string
*/
string getStringFromPeptideType(ePeptideType peptidetype);


/**
	\brief Crop a precision of a double to six decimal places.
	\param value an input value
	\retval double a value with the limited precision
*/
double cropPrecisionToSixDecimals(double value);


/**
	\brief Crop decimals of a double.
	\param value an input value
	\retval QByteArray a value with the limited precision
*/
QByteArray cropDecimalsByteArray(double value);


/**
	\brief Crop a precision of a double to six decimal places.
	\param value an input value
	\retval QByteArray a value with the limited precision
*/
QByteArray cropPrecisionToSixDecimalsByteArray(double value);


/**
	\brief Store coordinates x and y.
*/
struct cCoordinates {

	/**
		\brief x coordinate
	*/ 
	int x;


	/**
		\brief y coordinate
	*/
	int y;


	/**
		\brief scan id
	*/ 
	int id;


	/**
		\brief m/z ratio
	*/ 
	double mzratio;


	/**
		\brief relative intensity
	*/ 
	double relativeintensity;


	/**
		\brief absolute intensity
	*/ 
	double absoluteintensity;


	/**
		\brief name of compound
	*/ 
	string name;


	/**
		\brief The constructor.
	*/ 
	cCoordinates() {
		x = 0;
		y = 0;
		id = 0;
		mzratio = 0;
		relativeintensity = 0;
		absoluteintensity = 0;
		name = "";
	}


	/**
		\brief The constructor.
		\param x coordinate x
		\param y coordinate y
		\param id scan id
		\param mzratio m/z ratio
		\param relativeintensity relativeintensity
		\param absoluteintensity absoluteintensity
		\param name the name of item
	*/ 
	cCoordinates(int x, int y, int id, double mzratio, double relativeintensity, double absoluteintensity, string name = "") {
		set(x, y, id, mzratio, relativeintensity, absoluteintensity, name);
	}


	/**
		\brief Set the coordinates.
		\param x coordinate x
		\param y coordinate y
		\param id scan id
		\param mzratio m/z ratio
		\param relativeintensity relativeintensity
		\param absoluteintensity absoluteintensity
		\param name the name of item
	*/ 
	void set(int x, int y, int id, double mzratio, double relativeintensity, double absoluteintensity, string name = "") {
		this->x = x;
		this->y = y;
		this->id = id;
		this->mzratio = mzratio;
		this->relativeintensity = relativeintensity;
		this->absoluteintensity = absoluteintensity;
		this->name = name;
	}

};


/**
	\brief Register vector<cCoordinates> by Qt.
*/
Q_DECLARE_METATYPE(vector<cCoordinates>);


/**
	\brief The structure defining a hash function of cCoordinates.
*/
struct hash_cCoordinates {

	/**
		\brief Hash cCoordinates.
		\param coordinates cCoordinates
		\retval size_t hashed cCoordinates
	*/
	size_t operator()(const cCoordinates& coordinates) const {
		return std::hash<int>()(coordinates.x) ^ std::hash<int>()(coordinates.y) ^ std::hash<int>()(coordinates.id);
	}

};


/**
	\brief Overloaded operator ==.
	\param a first candidate
	\param b second candidate
	\retval bool true when cCoordinates are equal
*/
bool operator == (cCoordinates const& a, cCoordinates const& b);


/**
	\brief Get factorial of a value.
	\param value value
	\retval double factorial of the value
*/
double fact(int value);


/**
	\brief Get median from a vector of doubles.
	\param vector vector of doubles
	\retval double median value
*/
double median(vector<double>& vector);


/**
	\brief Strip HTML tags from a HTML string.
	\param htmlstring reference to a string with HTML tags
	\retval string stripped string
*/
string stripHTML(string& htmlstring);


/**
	\brief Strip isomers of building blocks from an input peptide sequence.
	\param peptidesequence a peptide sequence
*/
void stripIsomers(string& peptidesequence);


/**
	\brief Strip isomers of building blocks from an input string vector.
	\param acronyms vector of building block acronyms with isomers
*/
void stripIsomersFromStringVector(vector<string>& acronyms);


#endif

