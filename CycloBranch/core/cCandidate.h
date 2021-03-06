/**
	\file cCandidate.h
	\brief The representation of a peptide sequence candidate.
*/


#ifndef _CCANDIDATE_H
#define _CCANDIDATE_H

#include <vector>
#include <unordered_set>
#include <string>

#include "core/cParameters.h"
#include "core/cBricksDatabase.h"
#include "core/cDeNovoGraphNode.h"
#include "core/cSummaryFormula.h"

class cCandidateSet;
class cDeNovoGraph;


using namespace std;
using namespace boost;


/**
	\brief An auxiliary structure representing a branch permutation of a branched peptide.
*/
struct TRotationInfo {

	/**
		\brief Id of the rotation.
	*/ 
	int id;


	/**
		\brief Vector of ids of bricks.
	*/ 
	vector<int> bricks;

	
	/**
		\brief Position of a brick where a branch of a branched peptide starts.
	*/ 
	int middlebranchstart;


	/**
		\brief Position of a brick where a branch of a branched peptide ends.
	*/ 
	int middlebranchend;


	/**
		\brief Id of a terminal modification (at the beginning of a spectrum).
	*/ 
	int startmodifID;


	/**
		\brief Id of a terminal modification (at the end of a spectrum).
	*/ 
	int endmodifID;


	/**
		\brief Id of a terminal modification of a branched peptide (in the middle of a spectrum).
	*/ 
	int middlemodifID;


	/**
		\brief Ids of bricks separated by '-', the branch is specified by '(' and ')'; example: 1-2-3(4-5)6-7.
	*/ 
	string tcomposition;
		

	/**
		\brief The constructor.
	*/ 
	TRotationInfo() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/ 
	void clear() {
		id = 0;
		bricks.clear();
		middlebranchstart = 0;
		middlebranchend = 0;
		startmodifID = 0;
		endmodifID = 0;
		middlemodifID = 0;
		tcomposition = "";
	}


	/**
		\brief Test whether \a tcomposition starts with a bracket.
		\retval bool true when \a tcomposition starts with a bracket
	*/
	bool startsWithBracket() {
		if (tcomposition.size() == 0) {
			return false;
		}
		return tcomposition[0] == '(';
	}

	/**
		\brief Test whether \a tcomposition ends with a bracket.
		\retval bool true when \a tcomposition ends with a bracket
	*/ 
	bool endsWithBracket() {
		if (tcomposition.size() == 0) {
			return false;
		}
		return tcomposition.back() == ')';
	}

};


/**
	\brief The class representing a peptide sequence candidate.
*/
class cCandidate {

	int startmodifID;
	int endmodifID;
	int middlemodifID;
	int branchstart;
	int branchend;
	vector<nodeEdge> path;
	eFragmentIonType startiontype;
	string name;
	cSummaryFormula summary;

	vector<string> composition;
	string internalcomposition;
	int numberofinternalbricks;

	string realpeptidename;
	string acronympeptidename;
	vector<string> acronyms;
	vector<string> backboneacronyms;
	vector<string> branchacronyms;
	string stringpath;

	void updateInternalComposition();

	void getPermutationsIter(cCandidateSet& permutations, vector<string>& currentcandidate, int position, bool* terminatecomputation);

	void attachSubBranchCandidates(cCandidate& candidate, cCandidateSet& result, ePeptideType peptidetype, bool* terminatecomputation);

	void attachAllBranches(cCandidate& candidate, cCandidateSet& result, ePeptideType peptidetype, bool* terminatecomputation);

	void getPartialRotations(const string& composition, vector<string>& rotations);

	void getPartialBranchCyclicRotations(const string& composition, vector<cCandidate>& branchcyclicrotations, int branchstart, int branchend);

public:

	/**
		\brief The default constructor.
	*/ 
	cCandidate();
	

	/**
		\brief The constructor.
		\param composition reference to a vector of strings where each string is a sequence of ids of bricks separated by '-' (each string corresponds to an edge in the de novo graph)
		\param path reference to a path in the de novo graph which corresponds to a peptide sequence candidate
		\param startiontype start ion type in the de novo graph
		\param startmodifID id of a terminal modification (at the beginning of a spectrum)
		\param endmodifID id of a terminal modification (at the end of a spectrum)
		\param middlemodifID id of a terminal modification of a branched peptide (in the middle of a spectrum)
		\param middlepos position in \a composition corresponding to a branch of a branched peptide
	*/ 
	cCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int middlepos);


	/**
		\brief The constructor.
		\param composition reference to a vector of strings where each string is a sequence of ids of bricks separated by '-' (each string corresponds to an edge in the de novo graph)
		\param path reference to a path in the de novo graph which corresponds to a peptide sequence candidate
		\param startiontype start ion type in the de novo graph
		\param startmodifID id of a terminal modification (at the beginning of a spectrum)
		\param endmodifID id of a terminal modification (at the end of a spectrum)
		\param middlemodifID id of a terminal modification of a branched peptide (in the middle of a spectrum)
		\param branchstart position of a brick where a branch of a branched peptide starts (it is assumed that all strings in \a composition are concatenated)
		\param branchend position of a brick where a branch of a branched peptide ends (it is assumed that all strings in \a composition are concatenated)
	*/ 
	cCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int branchstart, int branchend);


	/**
		\brief Set a peptide sequence candidate.
		\param composition reference to a vector of strings where each string is a sequence of ids of bricks separated by '-' (each string corresponds to an edge in the de novo graph)
		\param path reference to a path in the de novo graph which corresponds to a peptide sequence candidate
		\param startiontype start ion type in the de novo graph
		\param startmodifID id of a terminal modification (at the beginning of a spectrum)
		\param endmodifID id of a terminal modification (at the end of a spectrum)
		\param middlemodifID id of a terminal modification of a branched peptide (in the middle of a spectrum)
		\param middlepos position in \a composition corresponding to a branch of a branched peptide
	*/ 
	void setCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int middlepos);


	/**
		\brief Set a peptide sequence candidate.
		\param composition reference to a vector of strings where each string is a sequence of ids of bricks separated by '-' (each string corresponds to an edge in the de novo graph)
		\param path reference to a path in the de novo graph which corresponds to a peptide sequence candidate
		\param startiontype start ion type in the de novo graph
		\param startmodifID id of a terminal modification (at the beginning of a spectrum)
		\param endmodifID id of a terminal modification (at the end of a spectrum)
		\param middlemodifID id of a terminal modification of a branched peptide (in the middle of a spectrum)
		\param branchstart position of a brick where a branch of a branched peptide starts (it is assumed that all strings in \a composition are concatenated)
		\param branchend position of a brick where a branch of a branched peptide ends (it is assumed that all strings in \a composition are concatenated)
	*/ 
	void setCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int branchstart, int branchend);
	

	/**
		\brief Clear the candidate.
	*/ 
	void clear();


	/**
		\brief Access to the composition of the peptide sequence candidate.
		\retval reference to a string of ids of bricks separated by '-'
	*/ 
	string& getComposition();


	/**
		\brief Reverse the bricks composition of the candidate.
	*/ 
	void revertComposition();


	/**
		\brief Prepare candidates of a branched or a branch-cyclic peptide.
		\param result reference to an output set of possible peptide sequence candidates
		\param peptidetype a type of an analyzed peptide
		\param terminatecomputation pointer to a variable determining that the computation must be stopped
	*/ 
	void prepareBranchedCandidates(cCandidateSet& result, ePeptideType peptidetype, bool* terminatecomputation);


	/**
		\brief Get a T-composition of a branched peptide.
		\retval string containing ids of bricks separated by '-', the branch is specified by '(' and ')'; example: 1-2-3(4-5)6-7
	*/ 
	string getTComposition();


	/**
		\brief Get a reverted T-composition of a branched peptide.
		\param usebrackets if true then the branch is separated by '(' and ')' else '-' is used instead of brackets
		\retval string containing ids of bricks separated by '-', the branch may be specified by '(' and ')'; example: 1-2-3(4-5)6-7
	*/ 
	string getRevertedTComposition(bool usebrackets);


	/**
		\brief Get a T-composition of a branched peptide composed from acronyms of bricks.
		\param brickdatabase reference to the database of bricks
		\retval string containing acronyms of bricks separated by '-', the branch is specified by '(' and ')'
	*/ 
	string getAcronymsTComposition(cBricksDatabase& brickdatabase);


	/**
		\brief Get a T-composition of a branched peptide composed from real names of bricks.
		\param brickdatabase reference to the database of bricks
		\retval string containing real names of bricks separated by '-', the branch is specified by '(' and ')'
	*/ 
	string getRealNameTComposition(cBricksDatabase& brickdatabase);


	/**
		\brief Get a number of blocks forming a branch.
		\retval int number of blocks
	*/ 
	int getBranchSize();


	/**
		\brief Get starting position of a branch.
		\retval int starting position of a branch
	*/ 
	int getBranchStart();


	/**
		\brief Get end position of a branch.
		\retval int end position of a branch
	*/ 
	int getBranchEnd();


	/**
		\brief Get permutations of branches a branched peptide.
		\param tpermutations reference to an output vector of auxiliary structures where permutations will be stored
	*/ 
	void getPermutationsOfBranches(vector<TRotationInfo>& tpermutations);


	/**
		\brief Calculate the precursor mass of the peptide sequence candidate from bricks.
		\param brickdatabasewithcombinations reference to an input database of bricks with combinations of bricks
		\param parameters a pointer to the parameters of the application
		\retval double precursor mass of the candidate
	*/ 
	double calculatePrecursorMassFromBricks(cBricksDatabase& brickdatabasewithcombinations, cParameters* parameters);


	/**
		\brief Compare two candidates.
		\param candidate reference to the other candidate
		\retval bool true when the candidates are equal
	*/ 
	bool isEqualTo(cCandidate& candidate);


	/**
		\brief Get a set of candidates with permutations of bricks.
		\param permutations reference to an output set of candidates
		\param terminatecomputation pointer to a variable determining that the computation must be stopped
	*/ 
	void getPermutations(cCandidateSet& permutations, bool* terminatecomputation);


	/**
		\brief Get id of a terminal modification (at the beginning of a spectrum).
		\retval int id of a terminal modification
	*/ 
	int getStartModifID();


	/**
		\brief Set id of a terminal modification (at the beginning of a spectrum).
		\param id of a terminal modification
	*/ 
	void setStartModifID(int id);


	/**
		\brief Get id of a terminal modification (at the end of a spectrum).
		\retval int id of a terminal modification
	*/ 
	int getEndModifID();


	/**
		\brief Set id of a terminal modification (at the end of a spectrum).
		\param id of a terminal modification
	*/ 
	void setEndModifID(int id);


	/**
		\brief Get id of a terminal modification of a branched peptide (in the middle of a spectrum).
		\retval int id of a terminal modification
	*/ 
	int getMiddleModifID();


	/**
		\brief Set id of a terminal modification of a branched peptide (in the middle of a spectrum).
		\param id of a terminal modification
	*/ 
	void setMiddleModifID(int id);


	/**
		\brief Compare two candidates.
		\param candidate reference to the other candidate
		\retval bool true when the other candidate is greater
	*/ 
	bool compare(cCandidate& candidate);


	/**
		\brief Get a path in the de novo graph corresponding to the peptide sequence candidate.
		\retval vector<nodeEdge> vector of pairs node and edge
	*/ 
	vector<nodeEdge>& getPath();


	/**
		\brief compare TPermutations of two candidates.
		\param candidate reference to the second candidate
		\retval true when candidates have equal TPermutations
	*/ 
	bool hasEqualTPermutations(cCandidate& candidate);


	/**
		\brief Check whether the candidate is composed exclusively from artificial bricks.
		\param brickdatabasewithcombinations reference to an input database of bricks with combinations of bricks
		\retval bool true when the candidate is composed exclusively from artificial bricks; false when the candidate constains a regular brick
	*/ 
	bool hasOnlyArtificialBricks(cBricksDatabase& brickdatabasewithcombinations);

	
	/**
		\brief Check if the first brick is artificial.
		\param brickdatabasewithcombinations reference to an input database of bricks with combinations of bricks
		\retval bool true if the first brick is artificial, false otherwise
	*/ 
	bool hasFirstBrickArtificial(cBricksDatabase& brickdatabasewithcombinations);


	/**
		\brief Check if the last brick is artificial.
		\param brickdatabasewithcombinations reference to an input database of bricks with combinations of bricks
		\retval bool true if the last brick is artificial, false otherwise
	*/ 
	bool hasLastBrickArtificial(cBricksDatabase& brickdatabasewithcombinations);


	/**
		\brief Check if the last brick is invalid.
		\param brickdatabasewithcombinations reference to an input database of bricks with combinations of bricks
		\retval bool true if the last brick is invalid, false otherwise
	*/ 
	bool hasLastBrickInvalid(cBricksDatabase& brickdatabasewithcombinations);


	/**
		\brief Get rotations of a cyclic peptide sequence.
		\param rotations reference to an output vector containing rotations of a sequence
		\param includerevertedrotations if true then reverted rotations are also included
	*/ 
	void getRotations(vector<string>& rotations, bool includerevertedrotations);


	/**
		\brief Get branch-cyclic rotations of a branch-cyclic peptide sequence.
		\param branchcyclicrotations reference to an output vector containing branch-cyclic rotations of a sequence
		\param includerevertedrotations if true then reverted branch-cyclic rotations are also included
	*/ 
	void getBranchCyclicRotations(vector<cCandidate>& branchcyclicrotations, bool includerevertedrotations);


	/**
		\brief Calculate the summary formula of the peptide sequence candidate.
		\param parameters a reference to the parameters of the application
		\param peptidetype the type of peptide
		\param precursormass experimantal precursor mass
		\retval string summary formula
	*/ 
	cSummaryFormula calculateSummaryFormulaFromBricks(cParameters& parameters, ePeptideType peptidetype, double precursormass = 0);


	/**
		\brief Get the summary formula of the peptide sequence candidate.
		\retval cSummaryFormula summary formula
	*/ 
	cSummaryFormula& getSummaryFormula();


	/**
		\brief Set the summary formula of the peptide sequence candidate.
		\param summary summary formula
	*/ 
	void setSummaryFormula(cSummaryFormula& summary);


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


	/**
		\brief Set the name of peptide.
		\param name name
	*/ 
	void setName(string& name);


	/**
		\brief Get the name of peptide.
		\retval string name
	*/ 
	string& getName();


	/**
		\brief Set a vector of acronyms corresponding to a peptide sequence candidate.
		\param bricksdatabase a database of building blocks
	*/ 
	void setAcronyms(cBricksDatabase& bricksdatabase);


	/**
		\brief Set a vector of acronyms corresponding to a backbone of a peptide sequence candidate.
		\param bricksdatabase a database of building blocks
	*/ 
	void setBackboneAcronyms(cBricksDatabase& bricksdatabase);


	/**
		\brief Set a vector of acronyms corresponding to a branch of a peptide sequence candidate.
		\param bricksdatabase a database of building blocks
	*/ 
	void setBranchAcronyms(cBricksDatabase& bricksdatabase);


	/**
		\brief Get a vector of acronyms corresponding to a peptide sequence candidate.
		\retval vector<string> a vector of acronyms
	*/ 
	vector<string>& getAcronyms();


	/**
		\brief Get a vector of acronyms corresponding to a backbone of a peptide sequence candidate.
		\retval vector<string> a vector of acronyms
	*/ 
	vector<string>& getBackboneAcronyms();


	/**
		\brief Get a vector of acronyms corresponding to a branch of a peptide sequence candidate.
		\retval vector<string> a vector of acronyms
	*/ 
	vector<string>& getBranchAcronyms();


	/**
		\brief Set a path in the de novo graph corresponding to the candidate.
		\param graph reference to the de novo graph
		\param parameters parameters of the application
	*/ 
	void setPath(cDeNovoGraph& graph, cParameters* parameters);


	/**
		\brief Get a path in the de novo graph corresponding to the candidate as a string.
		\retval string reference to a path corresponding to the spectrum
	*/ 
	string& getPathAsString();


	/**
		\brief Set a real peptide name.
		\param bricksdatabase the database of building blocks
		\param peptidetype the type of peptide
	*/ 
	void setRealPeptideName(cBricksDatabase& bricksdatabase, ePeptideType peptidetype);


	/**
		\brief Set an acronym peptide name.
		\param bricksdatabase the database of building blocks
		\param peptidetype the type of peptide
	*/ 
	void setAcronymPeptideNameWithHTMLReferences(cBricksDatabase& bricksdatabase, ePeptideType peptidetype);


	/**
		\brief Get a real peptide name.
		\retval string reference to the real peptide name
	*/ 
	string& getRealPeptideName();


	/**
		\brief Get an acronym peptide name.
		\retval string reference to the acronym peptide name
	*/ 
	string& getAcronymPeptideNameWithHTMLReferences();


	/**
		\brief Set start ion type used in the de novo graph.
		\param iontype fragment ion type
	*/ 
	void setStartIonType(eFragmentIonType iontype);


	/**
		\brief Get start ion type used in the de novo graph.
		\retval eFragmentIonType fragment ion type
	*/ 
	eFragmentIonType getStartIonType();


	/**
		\brief Check if the order of blocks is correct.
		\param bricksdatabase a database of building blocks
		\param peptidetype the type of peptide
		\param regularblocksorder check regular order of ketide blocks
		\retval bool true when the order of blocks is correct; false otherwise
	*/ 
	bool checkKetideSequence(cBricksDatabase& bricksdatabase, ePeptideType peptidetype, bool regularblocksorder);


	/**
		\brief Get the residue type of the left-most building block.
		\param bricksdatabase a database of building blocks
		\retval eResidueLossType residue loss type of the left-most building block
	*/ 
	eResidueLossType getLeftResidueType(cBricksDatabase& bricksdatabase);


	/**
		\brief Get the residue type of the right-most building block.
		\param bricksdatabase a database of building blocks
		\retval eResidueLossType residue loss type of the right-most building block
	*/ 
	eResidueLossType getRightResidueType(cBricksDatabase& bricksdatabase);


	/**
		\brief Check if the numbers of H2 loss and H2O loss blocks are correct.
		\param bricksdatabase a database of building blocks
		\param peptidetype the type of peptide
		\param regularblocksorder check regular order of ketide blocks
		\retval bool true when the numbers of blocks are correct
	*/ 
	bool checkKetideBlocks(cBricksDatabase& bricksdatabase, ePeptideType peptidetype, bool regularblocksorder);


};


/**
	\brief The structure defining a hash function of cCandidate.
*/
struct hash_cCandidate {

	/**
		\brief Hash a peptide sequence candidate.
		\param candidate reference to a peptide sequence candidate
		\retval size_t hashed candidate
	*/
	size_t operator()(const cCandidate& candidate) const {
		return std::hash<string>()(((cCandidate&)candidate).getComposition());
	}

};


/**
	\brief Overloaded operator ==.
	\param a first candidate
	\param b second candidate
	\retval bool true when candidates are equal
*/
bool operator == (cCandidate const& a, cCandidate const& b);


#endif

