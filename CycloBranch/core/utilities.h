/**
	\file utilities.h
	\brief Auxiliary funtions and structures.
*/


#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <fstream>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>
#include <cfloat>
#include <cmath>
#include <QString>
#include <QByteArray>
#include <QMetaType>
#include <QAbstractItemModel>
#include <QModelIndex>
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
	\brief Default window size x.
*/
const int defaultwinsizex = 1350;


/**
	\brief Default window size y.
*/
const int defaultwinsizey = 875;


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
	dereplication = 3,
	compoundsearch = 4
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
	\brief Store a vector of integers into an output stream.
	\param v reference to a vector of integers
	\param os reference to an output stream
*/
void storeIntVector(vector<int>& v, ofstream& os);


/**
	\brief Load a vector of integers from an input stream.
	\param v reference to a vector of integers
	\param is reference to an input stream
*/
void loadIntVector(vector<int>& v, ifstream& is);


/**
	\brief Store a vector of doubles into an output stream.
	\param v reference to a vector of doubles
	\param os reference to an output stream
*/
void storeDoubleVector(vector<double>& v, ofstream& os);


/**
	\brief Load a vector of doubles from an input stream.
	\param v reference to a vector of doubles
	\param is reference to an input stream
*/
void loadDoubleVector(vector<double>& v, ifstream& is);


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
	\brief Store a map<string, int> into an output stream.
	\param map reference to a map
	\param os reference to an output stream
*/
void storeStringIntMap(map<string, int>& map, ofstream& os);


/**
	\brief Load a map<string, int> from an input stream.
	\param map reference to a map
	\param is reference to an input stream
*/
void loadStringIntMap(map<string, int>& map, ifstream& is);


/**
	\brief Store a vector of map<string, int> into an output stream.
	\param vector reference to a vector
	\param os reference to an output stream
*/
void storeStringIntMapVector(vector< map<string, int> >& vector, ofstream& os);


/**
	\brief Load a vector of map<string, int> from an input stream.
	\param vector reference to a vector
	\param is reference to an input stream
*/
void loadStringIntMapVector(vector< map<string, int> >& vector, ifstream& is);


/**
	\brief Store a map<int, string> into an output stream.
	\param map reference to a map
	\param os reference to an output stream
*/
void storeIntStringMap(map<int, string>& map, ofstream& os);


/**
	\brief Load a map<int, string> from an input stream.
	\param map reference to a map
	\param is reference to an input stream
*/
void loadIntStringMap(map<int, string>& map, ifstream& is);


/**
	\brief Convert an unordered_map<string, int> to a vector<string>.
	\param map an input map
	\param vector an output vector
*/
void convertStringIntUnorderedMapToStringVector(unordered_map<string, int>& map, vector<string>& vector);


/**
	\brief Check if \a c is a white space.
	\param c char
	\retval bool true if \a c is a white space; false otherwise.
*/
bool isWhiteSpace(char c);


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
	\brief Check if a file exists.
	\param filename filename
	\retval bool true if the file exists, false otherwise
*/
bool checkFile(string filename);


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
	\brief Store aggregated information about a coordinate.
*/
struct cCoordinateInfo {

	/**
		\brief scan id
	*/
	int id;


	/**
		\brief x coordinate
	*/ 
	int x;


	/**
		\brief y coordinate
	*/
	int y;


	/**
		\brief relative intensity
	*/ 
	double relativeintensity;


	/**
		\brief absolute intensity
	*/ 
	double absoluteintensity;


	/**
		\brief description
	*/ 
	string description;


	/**
		\brief The constructor.
	*/ 
	cCoordinateInfo() {
		id = 0;
		x = 0;
		y = 0;
		relativeintensity = 0;
		absoluteintensity = 0;
		description = "";
	}

};


/**
	\brief Register vector<cCoordinateInfo> by Qt.
*/
Q_DECLARE_METATYPE(vector<cCoordinateInfo>);


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


/**
	\brief Compare items in a string vector by size.
*/
struct compareStringBySize {

	/**
		\brief Comparison function.
		\param first first string
		\param second second string
		\retval bool true if the first string is shorter
	*/	
	bool operator()(const std::string& first, const std::string& second) {
		size_t size1 = first.size();
		size_t size2 = second.size();
		if (first.size() < second.size()) {
			return true;
		}
		if (first.size() > second.size()) {
			return false;
		}
		return first.compare(second) < 0;
	}

};


/**
	\brief Proxy model - int comparator.
	\param model model
	\param index index
	\param row row
	\param column column
	\param str query string
	\param parent parent
	\retval bool true if the item is valid
*/
bool proxyModelCheckInt(QAbstractItemModel* model, int index, int row, int column, QString str, const QModelIndex& parent);


/**
	\brief Proxy model - double comparator.
	\param model model
	\param index index
	\param row row
	\param column column
	\param str query string
	\param parent parent
	\retval bool true if the item is valid
*/
bool proxyModelCheckDouble(QAbstractItemModel* model, int index, int row, int column, QString str, const QModelIndex& parent);


/**
	\brief Proxy model - string comparator.
	\param model model
	\param index index
	\param row row
	\param column column
	\param itemstr string of current item
	\param str query string
	\param parent parent
	\param wholeword wholeword
	\param casesensitive casesensitive
	\retval bool true if the item is valid
*/
bool proxyModelCheckString(QAbstractItemModel* model, int index, int row, int column, QString& itemstr, QString str, const QModelIndex& parent, bool wholeword, Qt::CaseSensitivity casesensitive);


#endif

