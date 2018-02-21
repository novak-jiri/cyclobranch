/**
	\file cParameters.h
	\brief Definitions of program parameters.
*/


#ifndef _CPARAMETERS_H
#define _CPARAMETERS_H

#include <iostream>
#include <fstream>
#include "core/utilities.h"
#include "core/cPeakListSeries.h"
#include "core/cBricksDatabase.h"
#include "core/cSequenceDatabase.h"


class cMainThread;


using namespace std;
using namespace boost;


/**
	\brief Peak list file formats supported by the application.
*/
enum ePeakListFileFormat {
	txt = 0,
	mgf = 1,
	mzML = 2,
	mzXML = 3,
	baf = 4,
	imzML = 5,
	mis = 6,
	ser = 7
};


/**
	\brief The class storing parameters of the application.
*/
class cParameters {

	cMainThread* os;

public:


	/**
		\brief Definitions of fragment ion types.
    */
	cFragmentIons fragmentdefinitions;
	

	/**
		\brief The type of analyzed peptide.
    */
    ePeptideType peptidetype;
 

	/**
		\brief A filename with a peak list.
    */
    string peaklistfilename;


	/**
		\brief A file format of peak list.
    */
	ePeakListFileFormat peaklistfileformat;


	/**
		\brief A structure representing a series of peaklists.
    */
	cPeakListSeries peaklistseries;


	/**
		\brief Scan number.
    */
    int scannumber;


	/**
		\brief Precursor mass of the peptide.
    */
    double precursormass;


	/**
		\brief An adduct of a precursor ion.
    */
    string precursoradduct;


	/**
		\brief A list of parsed metal adducts from \a precursoradduct.
    */
    vector<string> metaladducts;


	/**
		\brief Precursor mass error tolerance (ppm).
    */
    double precursormasserrortolerance;


	/**
		\brief The charge of the precursor ion.
    */
    int precursorcharge;


	/**
		\brief Fragment mass error tolerance (ppm).
    */
    double fragmentmasserrortolerance;


	/**
		\brief Mass error tolerance for deisotoping.
    */
    double masserrortolerancefordeisotoping;


	/**
		\brief Minimum relative intensity threshold.
    */
    double minimumrelativeintensitythreshold;


	/**
		\brief Minimum mz.
    */
    double minimummz;


	/**
		\brief FWHM.
    */
    double fwhm;


	/**
		\brief A file name of a bricks database.
    */
    string bricksdatabasefilename;


	/**
		\brief A structure representing a database of bricks.
    */
    cBricksDatabase bricksdatabase;


	/**
		\brief The maximum number of bricks in combination (at the beginning of a spectrum).
    */
    int maximumbricksincombinationbegin;


	/**
		\brief The maximum number of bricks in combination (in the middle of a spectrum).
    */
	int maximumbricksincombinationmiddle;


	/**
		\brief The maximum number of bricks in combination (at the end of a spectrum).
    */
	int maximumbricksincombinationend;


	/**
		\brief The global maximum number of bricks in combination (maximum from \a maximumbricksincombinationbegin, \a maximumbricksincombinationmiddle and \a maximumbricksincombinationend).
    */
	int maximumbricksincombination;


	/**
		\brief The maximum cummulative mass of bricks.
    */
    double maximumcumulativemass;


	/**
		\brief True when permutations of bricks are generated.
    */
    bool generatebrickspermutations;


	/**
		\brief A file with modifications.
    */
    string modificationsfilename;


	/**
		\brief Maximum number of threads.
    */
    int maximumnumberofthreads;


	/**
		\brief Program mode.
    */
	eModeType mode;


	/**
		\brief A type of score.
    */
    eScoreType scoretype;


	/**
		\brief True when matches of peaks which do not have parent peaks are removed.
    */
    bool clearhitswithoutparent;


	/**
		\brief True when complete isotope pattern is generated.
    */
    bool generateisotopepattern;


	/**
		\brief True when the N-terminus of a linear peptide is cyclized.
    */
	bool cyclicnterminus;


	/**
		\brief True when the C-terminus of a linear peptide is cyclized.
    */
	bool cycliccterminus;


	/**
		\brief True when the detection of scrambled ions is enabled.
    */
    bool enablescrambling;


	/**
		\brief True when the similarity search is enabled.
    */
    bool similaritysearch;


	/**
		\brief True when the order of polyketide blocks is regular.
    */
    bool regularblocksorder;


	/**
		\brief Maximum number of reported hits.
    */
    int hitsreported;


	/**
		\brief A sequence tag.
    */
    string sequencetag;


	/**
		\brief A copy of sequence tag.
    */
    string originalsequencetag;


	/**
		\brief A searched sequence.
    */
    string searchedsequence;


	/**
		\brief A copy of searched sequence.
    */
    string originalsearchedsequence;


	/**
		\brief N-terminal modification of a searched peptide.
    */
    string searchedsequenceNtermmodif;

	
	/**
		\brief C-terminal modification of a searched peptide.
    */
	string searchedsequenceCtermmodif;

	
	/**
		\brief Terminal modification of a branch of searched branched peptide.
    */
	string searchedsequenceTmodif;


	/**
		\brief Maximum number of peptide sequence candidates.
    */
    int maximumnumberofcandidates;


	/**
		\brief The identifier of an action with edges forming incomplete paths.
    */
    int blindedges;


	/**
		\brief A file name of a sequence database.
    */
    string sequencedatabasefilename;


	/**
		\brief A structure representing a database of sequences.
    */
    cSequenceDatabase sequencedatabase;


	/**
		\brief A vector of searched N-terminal and C-terminal modifications.
    */
    vector<fragmentDescription> searchedmodifications;


	/**
		\brief A vector of fragment ion types generated in the de novo graph.
    */
    vector<eFragmentIonType> fragmentionsfordenovograph;


	/**
		\brief A vector of fragment ion types generated in theoretical spectra.
	*/
	vector<eFragmentIonType> fragmentionsfortheoreticalspectra;


	/**
		\brief A map of peak ids to descriptions.
	*/
	map<int, string> peakidtodesc;


	/**
		\brief A map of peak descriptions to ids.
	*/
	map<string, int> peakdesctoid;


	/**
		\brief Max count of pixel x - parsed from imzML file.
	*/
	int maxcountx;


	/**
		\brief Max count of pixel y - parsed from imzML file.
	*/
	int maxcounty;


	/**
		\brief Vendor type.
	*/
	eVendorType vendor;


	/**
		\brief The constructor.
	*/ 
	cParameters();


	/**
		\brief Clear the structure.
	*/ 
	void clear();


	/**
		\brief Test restrictions of parameters and prepare some internal parameters.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int -1 when an error occurred, 0 otherwise
	*/ 
	int checkAndPrepare(bool& terminatecomputation);


	/**
		\brief Print parameters to a string.
		\retval string string with parameters
	*/ 
	string printToString();


	/**
		\brief Set the ouput stream.
		\param os reference to the main thread of the application (output stream)
	*/ 
	void setOutputStream(cMainThread& os);


	/**
		\brief Update definitions of fragment ions when \a cyclicnterminus, \a cycliccterminus or \a precursoradduct have been changed.
	*/ 
	void updateFragmentDefinitions();


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


/**
	\brief Register cParameters by Qt.
*/
Q_DECLARE_METATYPE(cParameters);  


#endif