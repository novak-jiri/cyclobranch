/**
	\file cTheoreticalSpectrum.h
	\brief The representation of a theoretical mass spectrum.
*/


#ifndef _CTHEORETICALSPECTRUM_H
#define _CTHEORETICALSPECTRUM_H

#include <vector>
#include <set>
#include <string>
#include <map>
#include <QMetaType>

#include "core/cParameters.h"
#include "core/cPeakListSeries.h"
#include "core/cCandidateSet.h"

class cMainThread;
class cDeNovoGraph;


using namespace std;


/**
	\brief The structure representing a splitting site of a cyclic peptide.
*/
struct splitSite {
	
	/**
		\brief Order of the first amino acid where a cyclic peptide is split.
	*/
	int first;


	/**
		\brief Order of the second amino acid where a cyclic peptide is split.
	*/
	int second;


	splitSite() {
		first = 0;
		second = 0;
	}
};


/**
	\brief An auxiliary structure for visualisation of matched series of a cyclic peptide.
*/
struct visualSeries {

	/**
		\brief A vector of matched peaks in a series.
	*/
	vector<int> series;


	/**
		\brief A name of a series of fragment ions.
	*/
	string name;


	/**
		\brief The constructor.
	*/
	visualSeries() {
		series.clear();
		name = "";
	}


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
	\brief The class representing a theoretical mass spectrum.
*/
class cTheoreticalSpectrum {

	cParameters* parameters;
	cPeaksList theoreticalpeaks;
	cPeaksList experimentalpeaks;
	cCandidate candidate;
	int experimentalpeaksmatched;
	int scrambledpeaksmatched;
	map<eFragmentIonType, int> matchedions;
	int peakstested;
	double experimentalpeaksmatchedratio;
	cPeaksList unmatchedpeaks;
	string coveragebyseries;
	bool valid;
	double intensityweightedscore;
	int maskscore;
	vector<visualSeries> visualcoverage;
	int validposition;
	int reversevalidposition;
	int seriescompleted;

	// remove false hits, i.e., b-H2O without existing b-ion
	void clearFalseHits(map<eFragmentIonType, vector<int> >& series, vector<eFragmentIonType>& fragmentions);

	// search for matches of experimental and theoretical peaks
	void searchForPeakPairs(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, vector<set<int> >& experimentalpeakmatches, double fragmentmasserrortolerance);

	// compute additional scores
	void computeStatistics(bool writedescription);

	// generate precursor ion and its variants
	void generatePrecursorIon(vector<int>& intcomposition, cBricksDatabase& bricksdatabasewithcombinations, int& theoreticalpeaksrealsize, bool writedescription);

	// generate scrambled sequences
	void generateScrambledIons(cBricksDatabase& bricksdatabase, bool writedescription, int& theoreticalpeaksrealsize);

	// select and normalize scrambled sequences
	void selectAndNormalizeScrambledSequences(unordered_set<string>& scrambledsequences);

	// select a proper fragment ion type for an experimental peak when masses of more theoretical fragment ions collide
	eFragmentIonType selectHigherPriorityIonTypeCID(eFragmentIonType experimentalpeakiontype, eFragmentIonType theoreticalpeakiontype);

	// add a peak to the list
	void addPeakToList(cPeak& peak, int& theoreticalpeaksrealsize);

	// add an adduct to a peak description
	void addAdductToDescription(string& description);

	// count isotopes of metals
	int countIsotopesOfMetals(cParameters* parameters);

	// add theoretical peaks containing metals
	void addMetalPeaks(cPeak& peak, int& peaklistrealsize, int charge, bool writedescription);

	// remove unmatched biometal isotopes
	void removeUnmatchedMetalIsotopes(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, vector<set<int> >& experimentalpeakmatches);


public:


	/**
		\brief The constructor.
	*/ 
	cTheoreticalSpectrum();


	/**
		\brief The constructor.
		\param parameters a pointer to the parameters of the application
		\param candidate reference to a peptide sequence candidate
	*/ 
	cTheoreticalSpectrum(cParameters* parameters, cCandidate& candidate);


	/**
		\brief Clear the spectrum.
		\param clearpeaklist if true then all variables including the list of theoretical peaks are cleared else the list of theoretical peaks is not cleared
	*/ 
	void clear(bool clearpeaklist = true);
	

	/**
		\brief Get the peptide spectrum candidate.
		\retval cCandidate reference to the peptide sequence candidate
	*/ 
	cCandidate& getCandidate();
	

	/**
		\brief Set a peptide spectrum candidate.
		\param candidate reference to the peptide sequence candidate
	*/ 
	void setCandidate(cCandidate& candidate);


	/**
		\brief Get the number of peaks in the spectrum.
		\retval int the number of peaks
	*/ 
	int getNumberOfPeaks();


	/**
		\brief Compare the theoretical spectrum of a branched peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareBranched(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


	/**
		\brief Compare the theoretical spectrum of a linear peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareLinear(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


	/**
		\brief Compare the theoretical spectrum of a cyclic peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


	/**
		\brief Compare the theoretical spectrum of a branch-cyclic peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareBranchCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


#if POLYKETIDE_SIDEROPHORES == 1


	/**
		\brief Compare the theoretical spectrum of a linear polyketide siderophore with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareLinearPolyketideSiderophore(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


	/**
		\brief Compare the theoretical spectrum of a cyclic polyketide siderophore with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareCyclicPolyketideSiderophore(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


#endif


	/**
		\brief Compare the theoretical spectrum of a linear polysaccharide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareLinearPolysaccharide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence);


	/**
		\brief Compare theoretical peaks with an experimental spectrum.
	*/ 
	void generateMSSpectrum();


	/**
		\brief Compare theoretical peaks with an experimental spectrum.
		\param peaklist reference to an experimental peaklist
	*/ 
	void compareMSSpectrum(cPeaksList& peaklist);


	/**
		\brief Get the number of matched peaks between an experimental and a theoretical spectrum.
		\retval int number of matched peaks
	*/ 
	int getNumberOfMatchedPeaks() const;


	/**
		\brief Get the number of matched scrambled peaks between an experimental and a theoretical spectrum.
		\retval int number of matched scrambled peaks
	*/ 
	int getNumberOfScrambledPeaks();


	/**
		\brief Get the number of matched peaks between an experimental and a theoretical spectrum of a specified fragment ion type.
		\param iontype a fragment ion type
		\retval int number of matched peaks
	*/ 
	int getNumberOfMatchedPeaks(eFragmentIonType iontype) const;


	/**
		\brief Get the number of matched Y and B ions between an experimental and a theoretical spectrum.
		\retval int number of matched peaks
	*/ 
	int getNumberOfMatchedPeaksYB() const;


	/**
		\brief Get a ratio of matched peaks.
		\retval double ratio of matched peaks
	*/ 
	double getRatioOfMatchedPeaks();


	/**
		\brief Generate a N-terminal fragment ion series.
		\param maxcharge a charge of precursor ion
		\param peaklistrealsize real size of the peak list
		\param intcomposition reference to a vector of ids of bricks as integers
		\param fragmentiontype fragment ion type which will be generated
		\param bricksdatabase reference to a database of building blocks
		\param writedescription if true then string descriptions of fragment ions are generated
		\param rotationid id of a cyclic peptide
		\param splittingsites reference to a vector of splitting sites of a cyclic peptide
		\param searchedmodifications reference to a vector of searched modifications
		\param peptidetype the type of searched peptide
		\param trotation a pointer to a T-permutation of a branched peptide
		\param leftresiduelosstype a residue type of the leftmost building block
		\param hasfirstblockartificial true when the first block is artificial, false otherwise
	*/ 
	void generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, TRotationInfo* trotation = 0, eResidueLossType leftresiduelosstype = water, bool hasfirstblockartificial = false);
	

	/**
		\brief Generate a C-terminal fragment ion series.
		\param maxcharge a charge of precursor ion
		\param peaklistrealsize real size of the peak list
		\param intcomposition reference to a vector of ids of bricks as integers
		\param fragmentiontype fragment ion type which will be generated
		\param bricksdatabase reference to a database of building blocks
		\param writedescription if true then string descriptions of fragment ions are generated
		\param rotationid id of a cyclic peptide
		\param splittingsites reference to a vector of splitting sites of a cyclic peptide
		\param searchedmodifications reference to a vector of searched modifications
		\param peptidetype the type of searched peptide
		\param trotation a pointer to a T-permutation of a branched peptide
		\param rightresiduelosstype a residue type of the rightmost building block
		\param haslastblockartificial true when the last block is artificial, false otherwise
	*/ 
	void generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, TRotationInfo* trotation = 0, eResidueLossType rightresiduelosstype = water, bool haslastblockartificial = false);


	/**
		\brief Sort peaks by mass.
		\param limit sort first \a limit peaks only; if \a limit == -1 then all peaks are sorted
	*/ 
	void sortByMass(int limit = -1);


	/**
		\brief Resize the list of peaks.
		\param size the number of peaks
	*/ 
	void resizePeakList(int size);


	/**
		\brief Get the precursor mass of the peptide sequence candidate.
		\param brickdatabasewithcombinations reference to an input database of bricks with combinations of bricks
		\retval double precursor mass of the candidate
	*/ 
	double getPrecursorMass(cBricksDatabase& brickdatabasewithcombinations);


	/**
		\brief Get the sum of relative intensities of matched peaks.
		\retval double sum of relative intensities of matched peaks
	*/ 
	double getWeightedIntensityScore() const;


	/**
		\brief Overloaded operator [].
		\param position position of a peak in the peak list
		\retval cPeak reference to the peak
	*/ 
	cPeak& operator[](int position);


	/**
		\brief Get an experimental spectrum which has been compared with the theoretical spectrum.
		\retval cPeaksList reference to an experimental spectrum
	*/ 
	cPeaksList& getExperimentalSpectrum();


	/**
		\brief Get coverage of series of fragment ions.
		\retval string coverage of the series of fragment ions
	*/ 
	string getCoverageBySeries();


	/**
		\brief Get a list of unmatched peaks.
		\retval cPeaksList a pointer to a list of unmatched peaks
	*/ 
	cPeaksList* getUnmatchedPeaks();


	/**
		\brief Get a list of theoretical peaks.
		\retval cPeaksList a pointer to a list of theoretical peaks
	*/ 
	cPeaksList* getTheoreticalPeaks();


	/**
		\brief Check whether the theoretical spectrum corresponds to a searched sequence.
		\retval bool if true then the theoretical spectrum corresponds to a searched sequence
	*/ 
	bool isValid();


	/**
		\brief Get a position of a rotation of a cyclic peptide sequence corresponding to searched sequence.
		\retval int position of sequence
	*/ 
	int getValidPosition();


	/**
		\brief Get a reverted position of a rotation of a cyclic peptide sequence corresponding to searched sequence.
		\retval int reverted position of sequence
	*/ 
	int getReverseValidPosition();


	/**
		\brief Get the number of matched bricks.
		\retval int number of matched bricks
	*/ 
	int getNumberOfMatchedBricks() const;


	/**
		\brief Get a vector of fragment ion series for visualization.
		\retval vector<visualSeries> vector of fragment ion series for visualisation
	*/ 
	vector<visualSeries>& getVisualCoverage();


	/**
		\brief Get a number of series which identify the peptide.
		\retval int number of completed series
	*/ 
	int getNumberOfCompletedSeries();


	/**
		\brief Set a number of series which identify the peptide.
		\param numberofcompletedseries number of completed series
	*/ 
	void setNumberOfCompletedSeries(int numberofcompletedseries);


	/**
		\brief Set a pointer to the parameters of the application.
		\param parameters the pointer to the parameters of the application
	*/ 
	void setParameters(cParameters* parameters);


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

