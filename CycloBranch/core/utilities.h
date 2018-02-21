/**
	\file utilities.h
	\brief Auxiliary funtions and structures.
*/


#ifndef _UTILITIES_H
#define _UTILITIES_H

#include <fstream>
#include <vector>
#include <string>


using namespace std;


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


#endif

