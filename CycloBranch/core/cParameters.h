/**
	\file cParameters.h
	\brief Definitions of program parameters.
*/


#ifndef _CPARAMETERS_H
#define _CPARAMETERS_H

#include <iostream>
#include <fstream>
#include "core/utilities.h"
#include "core/cGlobalPreferences.h"
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
	mgf,
	mzML,
	mzXML,
	baf,
	raw,
	dat, 
	imzML,
	mis,
	ser
};


/**
	\brief The types of scores between an experimental and a theoretical spectrum.
*/
enum eScoreType {
	number_of_matched_peaks = 0,
	sum_of_relative_intensities = 1,
	number_of_b_ions = 2,
	number_of_y_ions = 3,
	number_of_b_and_y_ions = 4,
	weighted_ratio_of_matched_peaks = 5
};


/**
	\brief The class storing parameters of the application.
*/
class cParameters {

	cMainThread* os;

	void fixIntensities(cPeaksList& centroidspectrum, cPeaksList& profilespectrum);

	bool checkSeniorRules(vector<int>& combarray, vector<int>& valences, int maxcomponents);

	//double getMassAndCounts(vector<int>& combarray, vector<int>& countsofelements, vector<double>& massesofelements);


public:


	/**
		\brief Definitions of ion types.
    */
	cFragmentIons iondefinitions;
	

	/**
		\brief The type of analyzed peptide.
    */
    ePeptideType peptidetype;
 

	/**
		\brief A filename with a peak list.
    */
    string peaklistfilename;


	/**
		\brief Use profile data.
	*/
	bool useprofiledata;


	/**
		\brief Convert profile data.
	*/
	bool convertprofiledata;


	/**
		\brief Profile data file name.
	*/
	string profiledatafilename;


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
		\brief Minimum relative intensity threshold.
	*/
	double minimumrelativeintensitythreshold;


	/**
		\brief Minimum absolute intensity threshold.
	*/
	unsigned minimumabsoluteintensitythreshold;


	/**
		\brief Minimum m/z ratio.
    */
    double minimummz;


	/**
		\brief Maximum m/z ratio.
	*/
	double maximummz;


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
		\brief Maximum number of combined neutral losses.
	*/
	int maximumcombinedlosses;


	/**
		\brief True when matches of peaks which do not have parent peaks are removed.
    */
    //bool clearhitswithoutparent;


	/**
		\brief Report unmatched theoretical peaks.
	*/
	bool reportunmatchedtheoreticalpeaks;


	/**
		\brief True when complete isotope pattern is generated.
	*/
	bool generateisotopepattern;


	/**
		\brief The minimum number of peaks in a matched pattern.
	*/
	int minimumpatternsize;


	/**
		\brief The minimum number of consecutive scans in which a compound must be identified to be reported (LC-MS data only).
	*/
	int minimumfeaturesize;


	/**
		\brief The minimum number of ion types which must be matched to report a given compound.
	*/
	int minimumiontypes;


	/**
		\brief Apply Senior's filtering rules.
	*/
	bool basicformulacheck;


	/**
		\brief Advanced filtering rules are aplied if compounds are generated in MS mode.
	*/
	bool advancedformulacheck;


	/**
		\brief Check N/O ratio.
	*/
	bool noratiocheck;


	/**
		\brief Maximum m/z tolerance of matched isotopes.
	*/
	double mzdifftolerance;


	/**
		\brief Maximum tolerance of intensities of matched isotopes.
	*/
	double intensitytolerance;


	/**
		\brief True if the N-terminus of a linear peptide is cyclized.
    */
	bool cyclicnterminus;


	/**
		\brief True if the C-terminus of a linear peptide is cyclized.
    */
	bool cycliccterminus;


	/**
		\brief True if the internal fragments are generated.
	*/
	bool internalfragments;


	/**
		\brief True if the detection of scrambled ions is enabled.
    */
    bool enablescrambling;


	/**
		\brief True if the similarity search is enabled.
    */
    bool similaritysearch;


	/**
		\brief True if the order of polyketide blocks is regularly changed.
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
		\brief Formula of searched sequence.
	*/
	string searchedsequenceformula;


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
    vector<cFragmentIonType> searchedmodifications;


	/**
		\brief A vector of fragment ion types generated in the de novo graph.
    */
    vector<eFragmentIonType> fragmentionsfordenovograph;


	/**
		\brief A vector of ion types generated in theoretical conventional mass spectra.
	*/
	vector<cIonType> ionsfortheoreticalspectraMS1;


	/**
		\brief A vector of ion types generated in theoretical fragment ion spectra.
	*/
	vector<eFragmentIonType> ionsfortheoreticalspectraMS2;


	/**
		\brief A vector of all neutral losses.
	*/
	vector<neutralLoss> neutrallossesdefinitions;


	/**
		\brief A vector of all neutral losses (without combinations of losses).
	*/
	vector<neutralLoss> originalneutrallossesdefinitions;


	/**
		\brief A vector of neutral losses generated in theoretical spectra.
	*/
	vector<int> neutrallossesfortheoreticalspectra;


	/**
		\brief A number of generated combinations of neutral losses.
	*/
	int numberofgeneratedneutrallosses;


	/**
		\brief A vector of neutral losses generated in theoretical spectra (without combinations).
	*/
	vector<int> originalneutrallossesfortheoreticalspectra;


	/**
		\brief A vector of peak descriptions.
	*/
	vector<string> peakidtodesc;


	/**
		\brief A vector of isotope formula descriptions.
	*/
	vector<string> isotopeformulaidtodesc;


	/**
		\brief Max count of pixel x - parsed from imzML file.
	*/
	int defaultmaxx;


	/**
		\brief Max count of pixel y - parsed from imzML file.
	*/
	int defaultmaxy;


	/**
		\brief Pixel size x - parsed from imzML file.
	*/
	int defaultpixelsizex;


	/**
		\brief Pixel size y - parsed from imzML file.
	*/
	int defaultpixelsizey;


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
		\brief Prepare neutral losses and compounds.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int -1 when an error occurred, 0 otherwise
	*/
	int prepareLossesAndCompounds(bool& terminatecomputation);


	/**
		\brief Check if the names of modifications used in a sequence are correct.
		\param sequence an input sequence
		\param startmodifid an output identifier of a start modification
		\param endmodifid an output identifier of an end modification
		\param middlemodifid an output identifier of a branch modification
		\param errormessage an output error message if failed
		\retval bool true if success
	*/
	bool checkModifications(cSequence& sequence, int& startmodifid, int& endmodifid, int& middlemodifid, string& errormessage);


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
		\brief Calculate a list of neutral losses.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\param errormessage an error message if failed
		\retval int -1 when an error occurred, 0 otherwise
	*/
	int calculateNeutralLosses(bool& terminatecomputation, string& errormessage);


	/**
		\brief Generate compounds.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\param errormessage an error message if failed
		\retval int -1 when an error occurred, 0 otherwise
	*/
	int generateCompounds(bool& terminatecomputation, string& errormessage);


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