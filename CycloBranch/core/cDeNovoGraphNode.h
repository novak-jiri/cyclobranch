/**
	\file cDeNovoGraphNode.h
	\brief The representation of a node in the de novo graph.
*/


#ifndef _CDENOVOGRAPHNODE_H
#define _CDENOVOGRAPHNODE_H

#include <unordered_set>
#include <vector>
#include <map>
#include <fstream>

#include "core/cFragmentIons.h"

using namespace std;


/**
	\brief An auxiliary structure to store relationship between a node and an edge.
*/
struct nodeEdge {

	/**
		\brief Id of a node.
	*/
	int nodeid;


	/**
		\brief Id of an edge.
	*/
	int edgeid;


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
	\brief The structure representing an outgoing edge from a node.
*/
struct cEdge {

	/**
		\brief Id of a target node.
	*/
	int targetnode;


	/**
		\brief Id of a brick.
	*/
	int brickid;


	/**
		\brief Error in ppm.
	*/
	double ppmerror;


	/**
		\brief A mass difference.
	*/
	double massdifference;


	/**
		\brief Charge of the source node.
	*/
	int sourcecharge;


	/**
		\brief Charge of the target node.
	*/
	int targetcharge;


	/**
		\brief Intensity of the target node.
	*/
	double targetintensity;


	/**
		\brief ids of bricks separated by '-'.
	*/
	string composition;


	/**
		\brief Id of a terminal modification (at the end of a spectrum).
	*/
	int endmodifID;


	/**
		\brief Id of a terminal modification of a branched peptide (in the middle of a spectrum).
	*/
	int middlemodifID;


	/**
		\brief The constructor.
	*/ 
	cEdge() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/ 
	void clear() {
		targetnode = 0;
		brickid = 0;
		ppmerror = 0;
		massdifference = 0;
		sourcecharge = 1;
		targetcharge = 1;
		targetintensity = 0;
		composition = "";
		endmodifID = 0;
		middlemodifID = 0;
	}

};


/**
	\brief The structure defining a hash function of cEdge.
*/
struct hash_cEdge {

	/**
		\brief Hash an edge.
		\param e reference to an edge
		\retval size_t hashed edge
	*/
	size_t operator()(const cEdge& e) const {
		return std::hash<int>()(e.targetnode) ^ std::hash<int>()(e.brickid);
	}

};


/**
	\brief Overloaded operator ==.
	\param a first edge
	\param b second edge
	\retval bool true when edges are equal
*/
bool operator == (cEdge const& a, cEdge const& b);


/**
	\brief Compare edges by ppm error.
	\param a first edge
	\param b second edge
	\retval bool true when \a a has smaller ppm error than \a b
*/
bool compareEdgesByPPMError(const cEdge& a, const cEdge& b);


/**
	\brief Compare edges by target node intensity.
	\param a first edge
	\param b second edge
	\retval bool true when the target node of \a a has bigger intensity than the target node of \a b
*/
bool compareEdgesByTargetNodeIntensityDesc(const cEdge& a, const cEdge& b);


/**
	\brief The class representing a node in the de novo graph.
*/
class cDeNovoGraphNode {

	double mzratio;
	double intensity;

	unordered_set<cEdge, hash_cEdge> tempedges;
	vector<cEdge> edges;
	map<eFragmentIonType, bool> ionannotation;

public:


	/**
		\brief The constructor.
	*/ 
	cDeNovoGraphNode();


	/**
		\brief Clear the class.
	*/ 
	void clear();


	/**
		\brief Get the mz ratio of the node.
		\retval double mz ratio
	*/ 
	double getMZRatio();

	
	/**
		\brief Set the mz ratio of the node.
		\param mzratio mz ratio
	*/ 
	void setMZRatio(double mzratio);


	/**
		\brief Get the intensity of the node.
		\retval double intensity
	*/ 
	double getIntensity();

	
	/**
		\brief Set the intensity of the node.
		\param intensity intensity
	*/ 
	void setIntensity(double intensity);


	/**
		\brief Insert an edge into a temporary unordered set.
		\param e reference to an edge
	*/ 
	void insertTempEdge(cEdge& e);


	/**
		\brief Insert an edge into a vector of edges.
		\param e reference to an edge
	*/ 
	void insertEdge(cEdge& e);


	/**
		\brief Sort a vector of edges by ppm error.
	*/ 
	void sortEdgesByPPMError();


	/**
		\brief Sort a vector of edges by target node intensity.
	*/ 
	void sortEdgesByTargetNodeIntensity();


	/**
		\brief Create a vector of edges from an unordered set of temporary edges.
	*/ 
	void createVectorOfEdges();


	/**
		\brief Get the number of edges.
		\retval int number of edges
	*/
	int size();


	/**
		\brief Overloaded operator [].
		\param position position of an edge
		\retval edge reference to an edge
	*/
	cEdge& operator[](int position);


	/**
		\brief Remove an edge
		\param position position of removed edge
	*/
	void removeEdge(int position);


	/**
		\brief Clear edges.
	*/
	void clearEdges();


	/**
		\brief Add ion annotation.
		\param iontype type of fragment ion
	*/
	void addIonAnnotation(eFragmentIonType iontype);


	/**
		\brief Check ion annotation.
		\param iontype type of fragment ion
		\retval bool true when the ion type was assigned to the node
	*/
	bool checkIonAnnotation(eFragmentIonType iontype);

};


#endif
