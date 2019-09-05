/**
	\file cMzML.h
	\brief mzML parser.
*/


#ifndef _CMZML_H
#define _CMZML_H


#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMemory.hpp>
#include <xercesc/util/Base64.hpp>
#include <string>
#include <vector>
#include <sstream>

#include "core/utilities.h"

 
using namespace std;
using namespace xercesc;


class cPeaksList;
class cMainThread;


/**
	\brief Compare element's tag name with a string.
	\param element pointer to element
	\param name string to be compared
	\retval bool true if the strings are equal, false otherwise.
*/
bool compareElementTagName(DOMElement* element, const char* name);


/**
	\brief Get an attribute.
	\param element pointer to element
	\param name attribute name
	\retval string attribute value
*/
string getAttribute(DOMElement* element, const char* name);


/**
	\brief Set an attribute.
	\param element pointer to element
	\param name attribute name
	\param value attribute value
*/
void setAttribute(DOMElement* element, const char* name, const char* value);


/**
	\brief mzML parser.
*/
class cMzML {

	XercesDOMParser* parser;
	DOMDocument* document;

	bool profilespectra;


public:
	

	/**
		\brief The constructor.
	*/ 
	cMzML();
	

	/**
		\brief The destructor.
	*/ 
	~cMzML();
 

	/**
		\brief Parse a mzml file.
		\param filename mzml filename
		\param peaklists mzml filename
		\param profilespectrumid id of a profile spectrum to be extracted, -1 = extract all profile spectra
		\param mode program mode
		\param os pointer to the main thread of the application (output stream)
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval 0 = success; -1 = aborted by user
	*/ 
	int parse(string& filename, vector<cPeaksList>& peaklists, int profilespectrumid, eModeType mode, cMainThread* os, bool& terminatecomputation);
 

	/**
		\brief Check if the file contains profile spectra.
		\retval bool true if the file contains profile spectra
	*/ 
	bool hasProfileSpectra();


};


#endif

