/**
	\file cSequenceDatabase.h
	\brief The database of sequences.
*/


#ifndef _CSEQUENCEDATABASE_H
#define _CSEQUENCEDATABASE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "core/utilities.h"
#include "core/cSequence.h"


using namespace std;


class cMainThread;


/**
	\brief The class representing a database of sequences.
*/
class cSequenceDatabase {

	vector<cSequence> sequences;

	void parseRT(bool keeprt);


public:


	/**
		\brief The constructor.
	*/ 
	cSequenceDatabase();


	/**
		\brief Load the database of sequences from a plain text stream.
		\param stream reference to an input file stream
		\param keeprt keep retention time as a part of the compound name
	*/ 
	void loadFromPlainTextStream(ifstream &stream, bool keeprt);


	/**
		\brief Store the database of sequences into a plain text stream.
		\param stream reference to an output file stream
		\param keeprt keep retention time as a part of the compound name
	*/
	void storeToPlainTextStream(ofstream &stream, bool keeprt);


	/**
		\brief Get the number of sequences in the database.
		\retval int number of sequences
	*/ 
	int size();


	/**
		\brief Overloaded operator [].
		\param position of the sequence in the vector of sequences
		\retval cSequence reference to a sequence
	*/ 
	cSequence& operator[](int position);


	/**
		\brief Clear the vector of sequences.
	*/ 
	void clear();


	/**
		\brief Push a new sequence into the vector of sequences.
		\param sequence an inserted sequence
	*/ 
	void push_back(cSequence& sequence);


	/**
		\brief Erase a sequence.
		\param index index of a sequence
	*/
	void erase(int index);


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
		\param fileversionpart1 first number of .res the file version
		\param fileversionpart2 second number of .res the file version
		\param fileversionpart3 third number of .res the file version
	*/ 
	void load(ifstream& is, int fileversionpart1, int fileversionpart2, int fileversionpart3);


	/**
		\brief Attach decoys.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
	*/
	void attachDecoys(bool& terminatecomputation);

};


#endif