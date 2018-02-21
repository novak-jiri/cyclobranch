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
 
using namespace std;
using namespace xercesc;


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
		\brief The constructor.
	*/ 
	cImzMLItem() {
		x = 0;
		y = 0;
		mzstart = 0;
		mzlength = 0;
		intensitystart = 0;
		intensitylength = 0;
	}

};


 
/**
	\brief imzML parser.
*/
class cImzML {

	XercesDOMParser* parser;
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
	*/ 
	void parse(string filename);
 

	/**
		\brief Get a vector of cImzML items.
		\retval vector<cImzMLItem> a vector of cImzML items
	*/ 
	vector<cImzMLItem>& getItems();
 
};


#endif

