/**
	\file cTheoreticalSpectrum.h
	\brief The representation of a theoretical mass spectrum.
*/


#ifndef _CTHEORETICALSPECTRUM_H
#define _CTHEORETICALSPECTRUM_H

#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <string>
#include <QMetaType>

#include "core/cParameters.h"
#include "core/cPeakListSeries.h"
#include "core/cCandidateSet.h"

class cMainThread;
class cDeNovoGraph;


using namespace std;


/**
	\brief Get the number of hits with equal or better score.
	\param scores vector of scores
	\param value current score
	\retval int number of hits
*/
int getNumberOfScoreHits(vector<double>& scores, double value);


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
	\brief An auxiliary structure for visualisation of matched series of fragment ions.
*/
struct matchedSeries {

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
	matchedSeries() {
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
	vector<set<int> > experimentalmatches;
	int scrambledpeaksmatched;
	map<eFragmentIonType, map<int, int> > matchedions;
	int peakstested;
	double experimentalpeaksmatchedratio;
	int unmatchedexperimentalpeakscount;
	string coveragebyseries;
	bool valid;
	double sumofrelativeintensities;
	vector<string> rotationslabels;
	int validposition;
	int reversevalidposition;
	int seriescompleted;
	int pathid;
	map<int, double> targetscores;
	map<int, double> decoyscores;
	map<int, double> fdrscores;

	// remove hits of peaks without hits of parent peaks
	void clearFalseHits(map<eFragmentIonType, map<int, vector<int> > >& series, vector<eFragmentIonType>& ions, vector<int>& losses);

	// search for matches of experimental and theoretical peaks
	void searchForPeakPairs(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, double fragmentmasserrortolerance);

	// compute additional scores
	void computeStatistics(bool writedescription);

	// generate charged forms of fragments
	void generateChargedFragments(cPeak& peak, map<string, int>& atoms, int& peaklistrealsize, int maxcharge, bool writedescription, bool disablesummary);

	// generate precursor ion and its variants
	void generatePrecursorIon(vector<int>& intcomposition, cBricksDatabase& bricksdatabasewithcombinations, int& theoreticalpeaksrealsize, bool writedescription);

	// generate precursor ion and its variants for other type
	void generatePrecursorIonForOther(int& theoreticalpeaksrealsize, bool writedescription);

	// generate scrambled sequences
	void generateScrambledIons(cBricksDatabase& bricksdatabase, bool writedescription, int& theoreticalpeaksrealsize);

	// update the list of losses
	void updateListOfNeutralLosses(cBricksDatabase& bricksdatabase, cBrick& block, vector<int>& currentlosses, vector<double>& fragmentlossmass, vector<string>& fragmentlosssummary, vector< map<string, int> >& fragmentlossmap, bool writedescription, bool disablesummary);

	// generate internal fragments
	void generateInternalFragments(cBricksDatabase& bricksdatabase, cPeak& peak, int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, int pos, map<string, int>& atoms, vector<int>& currentlosses, eFragmentIonType fragmentiontype, ePeptideType peptidetype, TRotationInfo* trotation, bool writedescription, bool disablesummary);

	// select and normalize scrambled sequences
	void selectAndNormalizeScrambledSequences(unordered_set<string>& scrambledsequences);

	// select a proper fragment ion type for an experimental peak when masses of more theoretical fragment ions collide
	void selectHigherPriorityIonTypeCID(cPeak& experimentalpeak, cPeak& theoreticalpeak);

	// add a peak to the list
	void addPeakToList(cPeak& peak, int& theoreticalpeaksrealsize);

	// add an adduct to a peak description
	void addAdductToDescription(string& description, vector<string>& metals);

	// add theoretical peaks containing metals
	void addMetalPeaks(cPeak& peak, vector<string>& metals, int& peaklistrealsize, int peakcharge, bool writedescription);

	// remove unmatched biometal isotopes
	void removeUnmatchedMetalIsotopes(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks);

	// remove unmatched isotope patterns
	void removeUnmatchedIsotopePatterns(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, cPeaksList& outputtheoreticalpeaks, bool storeunmatchedpeaks);

	// remove unmatched features
	int removeUnmatchedFeatures(bool lcms, cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, vector< vector<int> >& hintsindex/*, int id*/);

	// remove unmatched compounds
	int removeUnmatchedCompounds(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks, int minimumiontypes);

	// remove unmatched patterns in fine isotopic patterns
	void removeUnmatchedPatternsFineSpectra(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks);

	// remove unmatched pattern by intensity ratio
	void removeUnmatchedPatternsByIntensityRatio(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks);

	// remove unmatched pattern by m/z difference
	void removeUnmatchedPatternsByMZDifference(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks);

	// calculate envelope scores
	void calculateEnvelopeScores(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks);

	// remove decoy peak matches
	void removeDecoyPeakMatches(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize, cPeaksList& experimentalpeaks);

	// normalize theoretical intensities
	void normalizeTheoreticalIntensities(cPeaksList& theoreticalpeaks, int theoreticalpeaksrealsize);

	// calibrate peaklist to the most intense theoretical peak
	double recalibrateToMostIntenseTheoreticalPeak(cPeaksList& peaklist);

	// calculate standard PPM deviation
	double getStandardPPMDeviation(cPeaksList& theoreticalpeaks, int start, int stop);

	// calculate standard intensity deviation
	double getStandardIntensityDeviation(cPeaksList& theoreticalpeaks, int start, int stop, double maximumexperimentalintensity);

	// calculate standard m/z difference deviation
	double getStandardMZDifferenceDeviation(cPeaksList& theoreticalpeaks, int start, int stop);

	// calculate the angle distance
	double getAngleDistance(cPeaksList& theoreticalpeaks, int start, int stop);

	// generate isotope patterns of fragment ions
	void generateFragmentIsotopePatterns(int& theoreticalpeaksrealsize, bool writedescription, unordered_map<string, int>* isotopeformuladesctoid);

	// fill annotations of experimental peaks and remove unmatched theoretical peaks
	void fillExperimentalAnnotationsAndRemoveUnmatchedTheoreticalPeaks(int& theoreticalpeaksrealsize, ePeptideType peptidetype, cPeaksList& unmatchedpeaksinmatchedpatterns, bool reportunmatchedtheoreticalpeaks, bool writedescription);

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
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareBranched(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Compare the theoretical spectrum of a linear peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareLinear(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Compare the theoretical spectrum of a cyclic peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Compare the theoretical spectrum of a branch-cyclic peptide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareBranchCyclic(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Compare the theoretical spectrum of a linear polyketide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareLinearPolyketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Compare the theoretical spectrum of a cyclic polyketide with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param bricksdatabasewithcombinations reference to a database of bricks with combinations of bricks
		\param writedescription if true then string descriptions of peaks are filled
		\param sequencetag reference to a regex of a sequence tag
		\param searchedsequence reference to a regex of a searched sequence
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated; -2 when the sequence tag does not match the peptide sequence candidate
	*/ 
	int compareCyclicPolyketide(cPeaksList& sortedpeaklist, cBricksDatabase& bricksdatabasewithcombinations, bool writedescription, regex& sequencetag, regex& searchedsequence, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Compare the theoretical spectrum of a metabolite with an experimental spectrum.
		\param sortedpeaklist reference to an experimental peaklist
		\param writedescription if true then string descriptions of peaks are filled
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param isotopeformuladesctoid a map of isotope descriptions
		\retval int number theoretical peaks generated
	*/
	int compareOther(cPeaksList& sortedpeaklist, bool writedescription, cPeaksList& unmatchedpeaksinmatchedpatterns, unordered_map<string, int>* isotopeformuladesctoid);


	/**
		\brief Generate a simple mass spectrum.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\param writedescription if true then string descriptions of peaks are filled
	*/ 
	void generateMSSpectrum(bool& terminatecomputation, bool writedescription);


	/**
		\brief Generate a simple mass spectrum with fine isotopic patterns.
		\param terminatecomputation reference to a variable determining that the computation must be stopped
	*/ 
	void generateFineMSSpectrum(bool& terminatecomputation);


	/**
		\brief Get a map of search hints to identify features in LC-MS data.
		\param id identifier of an experimental spectrum
		\param tsfull theoretical spectrum
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param hintsindex index of experimental spectra for every matched theoretical peak
	*/
	void getHintsIndex(int id, cTheoreticalSpectrum& tsfull, cPeaksList& unmatchedpeaksinmatchedpatterns, vector< vector<int> >& hintsindex);


	/**
		\brief Compare theoretical peaks with an experimental spectrum.
		\param id identifier of an experimental spectrum
		\param tsfull theoretical spectrum with descriptions of peaks
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param hintsindex index of experimental spectra for every matched theoretical peak
	*/
	void compareMSSpectrum(int id, cTheoreticalSpectrum& tsfull, cPeaksList& unmatchedpeaksinmatchedpatterns, vector< vector<int> >& hintsindex);


	/**
		\brief Finalize MS spectrum after comparison.
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
		\param writedescription if true then string descriptions of fragment ions are generated
	*/
	void finalizeMSSpectrum(cPeaksList& unmatchedpeaksinmatchedpatterns, bool writedescription);


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
		\param neutrallosstype a neutral loss type
		\retval int number of matched peaks
	*/ 
	int getNumberOfMatchedPeaks(eFragmentIonType iontype, int neutrallosstype) const;


	/**
		\brief Get the number of matched B ions between an experimental and a theoretical spectrum.
		\retval int number of matched peaks
	*/
	int getNumberOfMatchedPeaksB() const;


	/**
		\brief Get the number of matched Y ions between an experimental and a theoretical spectrum.
		\retval int number of matched peaks
	*/
	int getNumberOfMatchedPeaksY() const;


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
		\param fragmentiontype fragment ion type to be generated
		\param neutrallosstype neutral loss type to be generated
		\param bricksdatabase reference to a database of building blocks
		\param writedescription if true then string descriptions of fragment ions are generated
		\param rotationid id of a cyclic peptide
		\param splittingsites reference to a vector of splitting sites of a cyclic peptide
		\param searchedmodifications reference to a vector of searched modifications
		\param peptidetype the type of searched peptide
		\param regularblocksorder check regular order of ketide blocks
		\param trotation a pointer to a T-permutation of a branched peptide
		\param leftresiduelosstype a residue type of the leftmost building block
		\param hasfirstblockartificial true when the first block is artificial, false otherwise
	*/ 
	void generateNTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, int neutrallosstype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation = 0, eResidueLossType leftresiduelosstype = h2o_loss, bool hasfirstblockartificial = false);
	

	/**
		\brief Generate a C-terminal fragment ion series.
		\param maxcharge a charge of precursor ion
		\param peaklistrealsize real size of the peak list
		\param intcomposition reference to a vector of ids of bricks as integers
		\param fragmentiontype fragment ion type to be generated
		\param neutrallosstype neutral loss type to be generated
		\param bricksdatabase reference to a database of building blocks
		\param writedescription if true then string descriptions of fragment ions are generated
		\param rotationid id of a cyclic peptide
		\param splittingsites reference to a vector of splitting sites of a cyclic peptide
		\param searchedmodifications reference to a vector of searched modifications
		\param peptidetype the type of searched peptide
		\param regularblocksorder check regular order of ketide blocks
		\param trotation a pointer to a T-permutation of a branched peptide
		\param rightresiduelosstype a residue type of the rightmost building block
		\param haslastblockartificial true when the last block is artificial, false otherwise
	*/ 
	void generateCTerminalFragmentIons(int maxcharge, int& peaklistrealsize, vector<int>& intcomposition, eFragmentIonType fragmentiontype, int neutrallosstype, cBricksDatabase& bricksdatabase, bool writedescription, int rotationid, vector<splitSite>& splittingsites, vector<fragmentDescription>& searchedmodifications, ePeptideType peptidetype, bool regularblocksorder, TRotationInfo* trotation = 0, eResidueLossType rightresiduelosstype = h2o_loss, bool haslastblockartificial = false);


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
		\brief Get the sum of relative intensities of matched peaks.
		\retval double sum of relative intensities of matched peaks
	*/ 
	double getSumOfRelativeIntensities() const;


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
		\brief Get ids of theoretical peaks which hit an experimental peak.
		\retval set<int> set of ids of theoretical peaks
	*/
	set<int>& getExperimentalMatches(int peakid);


	/**
		\brief Get coverage of series of fragment ions.
		\retval string coverage of the series of fragment ions
	*/ 
	string getCoverageBySeries();


	/**
		\brief Get the number of unmatched experimental peaks.
		\retval number of unmatched experimental peaks
	*/ 
	int getUnmatchedExperimentalPeaksCount();


	/**
		\brief Get a list of theoretical peaks.
		\retval cPeaksList a pointer to a list of theoretical peaks
	*/ 
	cPeaksList* getTheoreticalPeaks();


	/**
		\brief Check if the candidate sequence corresponds to the searched sequence.
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
		\brief Set a flag determining if the candidate sequence corresponds to the searched sequence.
		\param searchedsequence searched sequence
	*/
	void setValidSequence(regex& searchedsequence);


	/**
		\brief Get the labels of cyclic rotations.
		\retval vector<string> a vector of labels
	*/
	vector<string>& getLabelsOfRotations();


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
		\brief Set path id.
		\param pathid id of a path
	*/ 
	void setPathId(int pathid);


	/**
		\brief Get path id.
		\retval int id of a path
	*/ 
	int getPathId() const;


	/**
		\brief Set a pointer to the parameters of the application.
		\param parameters the pointer to the parameters of the application
	*/ 
	void setParameters(cParameters* parameters);


	/**
		\brief Get target pattern score.
		\param groupid id of a group of peaks forming a target pattern
		\retval double target pattern score
	*/
	double getTargetPatternScore(int groupid);


	/**
		\brief Get target pattern FDR.
		\param groupid id of a group of peaks forming a target pattern
		\retval double target pattern FDR
	*/
	double getTargetPatternFDR(int groupid);


	/**
		\brief Get map of target scores.
		\retval map<int, double> map of target scores
	*/
	map<int, double>& getTargetScores();


	/**
		\brief Get map of decoy scores.
		\retval map<int, double> map of decoy scores
	*/
	map<int, double>& getDecoyScores();


	/**
		\brief Set false discovery rates to peak groups.
		\param targetscoresvector vector of target scores
		\param fdrs vector of FDRs
		\param unmatchedpeaksinmatchedpatterns unmatched peaks in matched isotope patterns
	*/
	void setFDRs(vector<double>& targetscoresvector, vector<double>& fdrs, cPeaksList& unmatchedpeaksinmatchedpatterns);


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

