#include "core/cDeNovoGraphNode.h"


void nodeEdge::store(ofstream& os) {
	os.write((char *)&nodeid, sizeof(int));
	os.write((char *)&edgeid, sizeof(int));
}


void nodeEdge::load(ifstream& is) {
	is.read((char *)&nodeid, sizeof(int));
	is.read((char *)&edgeid, sizeof(int));
}


bool operator == (nodeEdge const& a, nodeEdge const& b) {
	return (a.nodeid == b.nodeid) && (a.edgeid == b.edgeid);
}


bool operator == (cEdge const& a, cEdge const& b) {
	return (a.targetnode == b.targetnode) && (a.brickid == b.brickid) && (a.sourceion == b.sourceion) && (a.targetion == b.targetion);
}


bool compareEdgesByPPMError(const cEdge& a, const cEdge& b) {
	return (a.ppmerror < b.ppmerror);
}


bool compareEdgesByTargetNodeIntensityDesc(const cEdge& a, const cEdge& b) {
	return (a.targetintensity > b.targetintensity);
}


cDeNovoGraphNode::cDeNovoGraphNode() {
	clear();
}


void cDeNovoGraphNode::clear() {
	mzratio = 0;
	intensity = 0;
	ionannotation.clear();
	
	for (int i = 0; i < fragmentIonTypeEnd; i++)
	{
		ionannotation[(eFragmentIonType)i] = false;
	}
	
	tempedges.clear();
	edges.clear();
}


double cDeNovoGraphNode::getMZRatio() {
	return mzratio;
}


void cDeNovoGraphNode::setMZRatio(double mzratio) {
	this->mzratio = mzratio;
}


double cDeNovoGraphNode::getIntensity() {
	return intensity;
}


void cDeNovoGraphNode::setIntensity(double intensity) {
	this->intensity = intensity;
}


void cDeNovoGraphNode::insertTempEdge(cEdge& e) {
	tempedges.insert(e);
}


void cDeNovoGraphNode::insertEdge(cEdge& e) {
	edges.push_back(e);
}


void cDeNovoGraphNode::sortEdgesByPPMError()  {
	sort(edges.begin(), edges.end(), compareEdgesByPPMError);
}


void cDeNovoGraphNode::sortEdgesByTargetNodeIntensity()  {
	sort(edges.begin(), edges.end(), compareEdgesByTargetNodeIntensityDesc);
}


void cDeNovoGraphNode::createVectorOfEdges() {
	edges.clear();
	while (tempedges.size() > 0) {
		edges.push_back(*tempedges.begin());
		tempedges.erase(tempedges.begin());
	}
}


int cDeNovoGraphNode::size() {
	return (int)edges.size();
}


cEdge& cDeNovoGraphNode::operator[](int position) {
	return edges[position];
}


void cDeNovoGraphNode::removeEdge(int position) {
	edges.erase(edges.begin() + position);
}


void cDeNovoGraphNode::clearEdges() {
	edges.clear();
}


void cDeNovoGraphNode::addIonAnnotation(eFragmentIonType iontype) {
	ionannotation[iontype] = true;
}


bool cDeNovoGraphNode::checkIonAnnotation(eFragmentIonType iontype) {
	return ionannotation[iontype];
}


