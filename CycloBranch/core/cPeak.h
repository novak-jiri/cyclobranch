/**
	\file cPeak.h
	\brief The representation of a peak in a mass spectrum.
*/


#ifndef _CPEAK_H
#define _CPEAK_H

#include <string>
#include <fstream>
#include "core/cFragmentIons.h"


using namespace std;


/**
	\brief The types of scores between an experimental and a theoretical spectrum.
*/
enum scoreType {
	b_ions = 0,
	b_ions_and_b_water_loss_ions = 1,
	b_ions_and_b_ammonia_loss_ions = 2,
	y_ions_and_b_ions = 3,
	y_ions = 4,
	weighted_intensity = 5,
	matched_peaks = 6,
	matched_bricks = 7
};


/**
	\brief A structure representing a peak in a mass spectrum.
*/
struct cPeak {

	/**
		\brief MZ ratio.
	*/ 
	double mzratio;


	/**
		\brief Intensity.
	*/ 
	double intensity;


	/**
		\brief Description of a peak.
	*/ 
	string description;

	
	/**
		\brief An extended description when the peak is matched.
	*/ 
	string matchdescription;


	/**
		\brief The type of a fragment ion corresponding to the peak.
	*/ 
	fragmentIonType iontype;

	
	/**
		\brief Number of matches of an experimental peak (when the peak is compared to a theoretical spectrum).
	*/ 
	int matched;


	/**
		\brief An order of an experimental peak which has been matched with a theoretical peak.
	*/ 
	int matchedid;


	/**
		\brief A charge of the peak.
	*/ 
	int charge;

		
	/**
		\brief Id of a rotation of a cyclic peptide or a permutation of a branched peptide which has been matched.
	*/ 
	int rotationid;

		
	/**
		\brief Vector of ids of rotations matched by an experimental peak.
	*/
	vector<int> matchedrotations;

	
	/**
		\brief An order of a peak in a series (e.g., B_5 in a B series).
	*/ 
	int seriesid;

		
	/**
		\brief True when the peak is an isotope peak.
	*/ 
	bool isotope;

	
	/**
		\brief True when the peak should be removed.
	*/ 
	bool removeme;


	/**
		\brief True when the peak corresponds to a scrambled fragment ion.
	*/ 
	bool scrambled;


	/**
		\brief The constructor.
	*/ 
	cPeak();


	/**
		\brief Clear the structure.
	*/
	void clear();


	/**
		\brief Test whether the structure is empty.
		\retval bool true when the structure is empty
	*/
	bool empty();


	/**
		\brief Test whether the peak matches a rotation of a cyclic peptide.
		\param rotationid id of a rotation of a cyclic peptide
		\retval bool true when the peak matches the rotation
	*/
	bool hasMatchedRotation(int rotationid);


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