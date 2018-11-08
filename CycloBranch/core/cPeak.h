/**
	\file cPeak.h
	\brief The representation of a peak in a mass spectrum.
*/


#ifndef _CPEAK_H
#define _CPEAK_H

#include <string>
#include <fstream>
#include "core/cFragmentIons.h"
#include "core/cSummaryFormula.h"


using namespace std;


/**
	\brief A structure representing a peak in a mass spectrum.
*/
struct cPeak {

	/**
		\brief Summary formlula.
	*/
	cSummaryFormula formula;

	/**
		\brief Isotope summary formula.
	*/
	string isotopeformula;

	/**
		\brief Id of isotope summary formula.
	*/
	int isotopeformulaid;

	/**
		\brief MZ ratio.
	*/ 
	double mzratio;


	/**
		\brief Relative ntensity.
	*/ 
	double relativeintensity;


	/**
		\brief Absolute intensity.
	*/ 
	double absoluteintensity;


	/**
		\brief Description of a peak.
	*/
	string description;


	/**
		\brief Description id of a peak.
	*/
	int descriptionid;


	/**
		\brief m/z ratio when the peak was matched with an experimental peak.
	*/ 
	double matchedmz;


	/**
		\brief Relative intensity when the peak was matched with an experimental peak.
	*/ 
	double matchedrelativeintensity;


	/**
		\brief Absolute intensity when the peak was matched with an experimental peak.
	*/ 
	double matchedabsoluteintensity;


	/**
		\brief An error in ppm when the peak was matched with an experimental peak.
	*/ 
	double matchedppm;


	/**
		\brief Ion type corresponding to the peak.
	*/ 
	eFragmentIonType iontype;


	/**
		\brief Neutral loss type corresponding to the peak.
	*/
	int neutrallosstype;

	
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
		\brief An order of a peak in a series (e.g., B_5 in a B series).
	*/ 
	int seriesid;

		
	/**
		\brief Identifier of a group of peaks.
	*/ 
	int groupid;


	/**
		\brief Identifier of a compound.
	*/
	int compoundid;

		
	/**
		\brief True if the peak is an isotope peak.
	*/ 
	bool isotope;

	
	/**
		\brief True if the peak should be removed.
	*/ 
	bool removeme;


	/**
		\brief True if the peak corresponds to a scrambled fragment ion.
	*/
	bool scrambled;


	/**
		\brief True if the peak corresponds to a decoy compound.
	*/
	bool decoy;


	/**
		\brief An order id of a peak in a peaklist.
	*/
	int orderid;


	/**
		\brief The constructor.
	*/
	cPeak();


	/**
		\brief The copy constructor.
		\param peak reference to cPeak
	*/
	cPeak(const cPeak& peak);


	/**
		\brief Overloaded operator=.
		\param peak reference to cPeak
	*/
	cPeak& operator=(const cPeak& peak);


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