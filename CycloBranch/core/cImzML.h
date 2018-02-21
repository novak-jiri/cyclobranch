/**
	\file cImzML.h
	\brief imzML parser.
*/


#ifndef _CIMZML_H
#define _CIMZML_H


#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <QFile>
#include <QCryptographicHash>

#include "core/cMzML.h"
 
using namespace std;
using namespace xercesc;


class cPeaksList;


/**
	\brief A parsed imzML item.
*/
struct cImzMLItem {

	/**
		\brief Image - x coordinate.
	*/
	int x;


	/**
		\brief Image - y coordinate.
	*/
	int y;


	/**
		\brief Starting offset of m/z values array in ibd file (in bytes).
	*/
	long long unsigned mzstart;


	/**
		\brief Length of m/z values array in ibd file (in bytes).
	*/
	long long unsigned mzlength;


	/**
		\brief Starting offset of intensity values array in ibd file (in bytes).
	*/
	long long unsigned intensitystart;


	/**
		\brief Length of intensity values array in ibd file (in bytes).
	*/
	long long unsigned intensitylength;


	/**
		\brief Title.
	*/
	string title;


	/**
		\brief The constructor.
	*/ 
	cImzMLItem() {
		x = 0;
		y = 0;
		mzstart = 0;
		mzlength = 0;
		intensitystart = 0;
		intensitylength = 0;
		title = "";
	}

};


 
/**
	\brief imzML parser.
*/
class cImzML {

	XercesDOMParser* parser;
	DOMDocument* document;

	bool profilespectra;
	bool use_64bit_precision;
	vector<cImzMLItem> imzmlitems;

public:
	

	/**
		\brief The constructor.
	*/ 
	cImzML();
	

	/**
		\brief The destructor.
	*/ 
	~cImzML();
 

	/**
		\brief Parse an imzml file.
		\param filename imzml filename
		\param maxcountx max count of pixel x - parsed from imzML file
		\param maxcounty max count of pixel y - parsed from imzML file
		\param vendor vendor type
		\retval 0 = success; 1 = failed (zlib compression detected)
	*/ 
	int parse(string& filename, int& maxcountx, int& maxcounty, eVendorType& vendor);
 

	/**
		\brief Update values after conversion of profile spectra into centroid spectra.
		\param peaklists an updated vector of peaklists
		\param convertedibdfilename filename of a converted ibd file
	*/ 
	void updateRawDataToPeakList(vector<cPeaksList>& peaklists, string& convertedibdfilename);
 

	/**
		\brief Get a vector of cImzML items.
		\retval vector<cImzMLItem> a vector of cImzML items
	*/ 
	vector<cImzMLItem>& getItems();


	/**
		\brief Check if the file contains profile spectra.
		\retval bool true if the file contains profile spectra
	*/ 
	bool hasProfileSpectra();


	/**
		\brief Check the precision of data.
		\retval bool if true, 64bit precision is used; if false, 32bit precision is used
	*/ 
	bool use64BitPrecision();


	/**
		\brief Write an imzml file.
		\param filename an output imzml filename
	*/ 
	void write(string& filename);
 
};


#endif

