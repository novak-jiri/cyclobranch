#include "core/cDeNovoGraph.h"

#include "gui/cMainThread.h"


bool compareNodes(cDeNovoGraphNode& node1, cDeNovoGraphNode& node2) {
	if (node1.getMZRatio() < node2.getMZRatio()) {
		return true;
	}
	return false;
}


cDeNovoGraph::cDeNovoGraph() {
	parameters = 0;
	graph.clear();
	lastsystemnode = 0;
	os = 0;
	startnode = 0;

	graphreaderthread = 0;
}


bool cDeNovoGraph::findPath(int sourcenodeid, int edgeid, int targetnodeid, string& composition, string brickspath, int maximumbricksincombination, vector<nodeEdge>& path, bool& terminatecomputation) {

	if (terminatecomputation) {
		return false;
	}

	string localbrickpath;
	bool found;
	bool badpath;
	int count = getNumberOfBricks(brickspath);
	nodeEdge ne;

	if ((path.size() > 1) && (sourcenodeid == targetnodeid)) {
		cBrick b;
		b.setComposition(brickspath, true);
		if (composition.compare(b.getComposition()) == 0)  {
			//*os << " path: " << graph[sourcenodeid].getMZRatio() << " ";
			return true;
		}
	}
	
	if (count < maximumbricksincombination) {
		for (int i = 0; i < (int)graph[sourcenodeid].size(); i++) {
			if ((i == edgeid) && (path.size() == 0)) {
				continue;
			}

			// target charge of an edge must be equal to the source charge of a following edge
			if ((path.size() > 0) && (graph[sourcenodeid][i].sourcecharge != graph[path.back().nodeid][path.back().edgeid].targetcharge)) {
				continue;
			}

			badpath = false;
			for (int j = 0; j < (int)path.size(); j++) {
				if (path[j].nodeid == sourcenodeid) {
					badpath = true;
					break;
				}
			}
			if (badpath) {
				continue;
			}

			localbrickpath = brickspath;
			if (localbrickpath.size() > 0) {
				localbrickpath += '-';
			}
			localbrickpath += graph[sourcenodeid][i].composition;

			ne.nodeid = sourcenodeid;
			ne.edgeid = i;
			path.push_back(ne);

			found = findPath(graph[sourcenodeid][i].targetnode, i, targetnodeid, composition, localbrickpath, maximumbricksincombination, path, terminatecomputation);
			
			path.pop_back();
			
			if (found) {
				//*os << graph[sourcenodeid].getMZRatio() << " ";
				return true;
			}

			if (terminatecomputation) {
				return false;
			}
		}
	}

	return false;
}


void cDeNovoGraph::createVectorsOfEdges() {
	for (int i = 0; i < (int)graph.size(); i++) {
		graph[i].createVectorOfEdges();
	}
}


bool cDeNovoGraph::finishInPrecursor(int currentnode, double cummass) {

	if (graph[currentnode].size() == 0) {
		if (currentnode == (int)graph.size() - 1) {
			return true;
		}
		else {
			return false;
		}
	}

	if (cummass > graph.back().getMZRatio()) {
		return false;
	}

	for (int i = 0; i < (int)graph[currentnode].size(); i++) {
		if (finishInPrecursor(graph[currentnode][i].targetnode, cummass + graph[currentnode][i].massdifference)) {
			return true;
		}
	}

	return false;
}


void cDeNovoGraph::initialize(cMainThread& os, cParameters& parameters) {
	graph.clear();
	lastsystemnode = 0;
	this->os = &os;
	this->parameters = &parameters;
	startnode = 0;
}


int cDeNovoGraph::createGraph(bool& terminatecomputation) {
	cDeNovoGraphNode node;
	cSummaryFormula formula;
	cSummaryFormula tmpformula;
	double mass;
	double termmass;
	int left, right, middle, targetnode, i;
	cEdge edge;
	double unchargedprecursormass = charge(uncharge(parameters->precursormass, parameters->precursorcharge), (parameters->precursorcharge > 0)?1:-1);
	double unchargedmz;
	
	sortedpeaklist = parameters->peaklistseries[parameters->scannumber - 1];
	// insert the single charged precursor, if neccessary
	sortedpeaklist.sortbyMass();
	sortedpeaklist.cropMaximumMZRatio(unchargedprecursormass, parameters->precursormasserrortolerance);
	if ((sortedpeaklist.size() > 0) && (!isInPpmMassErrorTolerance(sortedpeaklist[sortedpeaklist.size() - 1].mzratio, unchargedprecursormass, parameters->precursormasserrortolerance))) { 
		cPeak p;
		p.mzratio = unchargedprecursormass;
		sortedpeaklist.add(p);
	}

	*os << "Creating the de novo graph... " << endl;

	// deconvolution
	sortedpeaklist.removeChargeVariants(parameters->precursorcharge, parameters->fragmentmasserrortolerance);
	*os << "Number of nodes after deconvolution: " << sortedpeaklist.size() << endl;

	//if (!parameters->generateisotopepattern) {

		// remove dehydrated peaks
		sortedpeaklist.removeNeutralLoss(-H2O, parameters->precursorcharge, parameters->fragmentmasserrortolerance);
		*os << "Number of nodes when dehydrated ions are removed: " << sortedpeaklist.size() << endl;

		// remove deamidated peaks
		sortedpeaklist.removeNeutralLoss(-NH3, parameters->precursorcharge, parameters->fragmentmasserrortolerance);
		*os << "Number of nodes when deamidated ions are removed: " << sortedpeaklist.size() << endl;

	//}

	double negativeshift = (parameters->precursorcharge > 0)?0:-2*(H - e);
	string negativeshiftsummary = (parameters->precursorcharge > 0)?"":"H-2+-2";

	// insert system nodes
	switch (parameters->peptidetype)
	{
	case cyclic:
		node.clear();
		node.setMZRatio(0);
		node.setIntensity(0);
		node.addIonAnnotation(b_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[b_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(b_ion);
		graph.push_back(node);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[b_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 1;
		edge.targetion = b_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		lastsystemnode = (int)graph.size() - 1;
		startnode = 1;
		break;
	case linear:
	case branched:
		node.clear();
		node.setMZRatio(0);
		node.setIntensity(0);
		node.addIonAnnotation(b_ion);
		node.addIonAnnotation(y_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[b_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(b_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[y_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(y_ion);
		graph.push_back(node);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[b_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 1;
		edge.targetion = b_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[y_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 2;
		edge.targetion = y_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		targetnode = 2;

		for (i = 1; i < (int)parameters->searchedmodifications.size(); i++) {
			
			if (parameters->searchedmodifications[i].nterminal) {
				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[b_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(b_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";	
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[b_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = b_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();
				
				graph[0].insertTempEdge(edge);
			}

			if (parameters->searchedmodifications[i].cterminal) {
				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[y_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(y_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[y_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = y_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();

				graph[0].insertTempEdge(edge);
			}

		}

		lastsystemnode = (int)graph.size() - 1;
		startnode = 1;
		break;
	case branchcyclic:
		node.clear();
		node.setMZRatio(0);
		node.setIntensity(0);
		node.addIonAnnotation(b_ion);
		//node.addIonAnnotation(y_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[b_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(b_ion);
		graph.push_back(node);

		//node.clear();
		//node.setMZRatio(parameters->fragmentdefinitions[y_ion].massdifference + negativeshift);
		//node.setIntensity(0);
		//node.addIonAnnotation(y_ion);
		//graph.push_back(node);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[b_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 1;
		edge.targetion = b_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		//edge.clear();
		//edge.composition = "0";
		//tmpformula.clear();
		//tmpformula.addFormula(parameters->fragmentdefinitions[y_ion].summary);
		//tmpformula.addFormula(negativeshiftsummary);
		//edge.summary = tmpformula.getSummary();
		//edge.targetnode = 2;
		//edge.targetion = y_ion;
		//edge.massdifference = graph[edge.targetnode].getMZRatio();
		//graph[0].insertTempEdge(edge);

		/*
		
		targetnode = 2;

		for (i = 1; i < (int)parameters->searchedmodifications.size(); i++) {

			if (parameters->searchedmodifications[i].nterminal) {
				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[b_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(b_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";	
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[b_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = b_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();
				
				graph[0].insertTempEdge(edge);
			}

			if (parameters->searchedmodifications[i].cterminal) {
				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[y_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(y_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[y_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = y_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();

				graph[0].insertTempEdge(edge);
			}

		}
		*/

		lastsystemnode = (int)graph.size() - 1;
		startnode = 1;
		break;
	case linearpolyketide:
		node.clear();
		node.setMZRatio(0);
		node.setIntensity(0);
		node.addIonAnnotation(l1h_ion);
		node.addIonAnnotation(l2h_ion);
		node.addIonAnnotation(l1oh_ion);
		node.addIonAnnotation(l2oh_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[l1h_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(l1h_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[l2h_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(l2h_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[l1oh_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(l1oh_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[l2oh_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(l2oh_ion);
		graph.push_back(node);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[l1h_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 1;
		edge.targetion = l1h_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[l2h_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 2;
		edge.targetion = l2h_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[l1oh_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 3;
		edge.targetion = l1oh_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[l2oh_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 4;
		edge.targetion = l2oh_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		targetnode = 4;

		for (i = 1; i < (int)parameters->searchedmodifications.size(); i++) {	

			if (parameters->searchedmodifications[i].nterminal) {
				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[l1h_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(l1h_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";	
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[l1h_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = l1h_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();

				graph[0].insertTempEdge(edge);

				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[l2h_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(l2h_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[l2h_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = l2h_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();

				graph[0].insertTempEdge(edge);
			}

			if (parameters->searchedmodifications[i].cterminal) {
				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[l1oh_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(l1oh_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[l1oh_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = l1oh_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();

				graph[0].insertTempEdge(edge);

				node.clear();
				node.setMZRatio(parameters->fragmentdefinitions[l2oh_ion].massdifference + parameters->searchedmodifications[i].massdifference + negativeshift);
				node.setIntensity(0);
				node.addIonAnnotation(l2oh_ion);
				graph.push_back(node);

				edge.clear();
				edge.composition = "0";
				edge.startmodifID = i;
				tmpformula.clear();
				tmpformula.addFormula(parameters->fragmentdefinitions[l2oh_ion].summary);
				tmpformula.addFormula(parameters->searchedmodifications[i].summary);
				tmpformula.addFormula(negativeshiftsummary);
				edge.summary = tmpformula.getSummary();
				targetnode++;
				edge.targetnode = targetnode;
				edge.targetion = l2oh_ion;
				edge.massdifference = graph[edge.targetnode].getMZRatio();

				graph[0].insertTempEdge(edge);
			}
	
		}

		lastsystemnode = (int)graph.size() - 1;
		startnode = 1;
		break;
	case cyclicpolyketide:
		node.clear();
		node.setMZRatio(0);
		node.setIntensity(0);
		//node.addIonAnnotation(l0h_ion);
		node.addIonAnnotation(l1h_ion);
		node.addIonAnnotation(l2h_ion);
		graph.push_back(node);

		//node.clear();
		//node.setMZRatio(parameters->fragmentdefinitions[l0h_ion].massdifference + negativeshift);
		//node.setIntensity(0);
		//node.addIonAnnotation(l0h_ion);
		//graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[l1h_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(l1h_ion);
		graph.push_back(node);

		node.clear();
		node.setMZRatio(parameters->fragmentdefinitions[l2h_ion].massdifference + negativeshift);
		node.setIntensity(0);
		node.addIonAnnotation(l2h_ion);
		graph.push_back(node);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[l1h_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 1;
		edge.targetion = l1h_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		edge.clear();
		edge.composition = "0";
		tmpformula.clear();
		tmpformula.addFormula(parameters->fragmentdefinitions[l2h_ion].summary);
		tmpformula.addFormula(negativeshiftsummary);
		edge.summary = tmpformula.getSummary();
		edge.targetnode = 2;
		edge.targetion = l2h_ion;
		edge.massdifference = graph[edge.targetnode].getMZRatio();
		graph[0].insertTempEdge(edge);

		/*edge.clear();
		edge.composition = "0";
		edge.targetnode = 3;
		graph[0].insertTempEdge(e);*/

		lastsystemnode = (int)graph.size() - 1;
		startnode = 1;
		break;
	case other:
		*os << "Invalid peptide type 'Other'." << endl;
		return -1;
	default:
		*os << "Undefined peptide type." << endl;
		return -1;
	}

	// sort(graph.begin() + startnode, graph.end(), compareNodes);

	// initialize nodes by mzratios from a sorted peak list
	for (i = 0; i < sortedpeaklist.size(); i++) {
		node.clear();
		node.setMZRatio(sortedpeaklist[i].mzratio);
		node.setIntensity(sortedpeaklist[i].relativeintensity);
		graph.push_back(node);
	}

	bricksdatabasewithcombinations = parameters->bricksdatabase;
	*os << "Number of bricks: " << parameters->bricksdatabase.size() << endl;
	*os << "Calculating number of brick combinations... ";

	// initialization for generating the combinations of bricks
	vector<int> combarray;
	int numberofbasicbricks = bricksdatabasewithcombinations.size();
	string compositionname;
	cBrick b;
	bool usedbrick;

	long long totaltested = 0;
	for (i = 0; i < parameters->maximumbricksincombination; i++) {
		combarray.push_back(0);
	}

	while (bricksdatabasewithcombinations.nextCombination(combarray, numberofbasicbricks, parameters->maximumbricksincombination, parameters->maximumcumulativemass, uncharge(parameters->precursormass, parameters->precursorcharge))) {
		if (terminatecomputation) {
			return -1;
		}
		totaltested++;
	}
	
	*os << "ok" << endl;
	*os << "Combinations of bricks: " << totaltested << endl;

	totaltested = 0;
	combarray.clear();
	for (i = 0; i < parameters->maximumbricksincombination; i++) {
		combarray.push_back(0);
	}

	// initialize edges
	*os << "Generating combinations of bricks and generating edges... " << endl;

	int j = 0;
	while (bricksdatabasewithcombinations.nextCombination(combarray, numberofbasicbricks, parameters->maximumbricksincombination, parameters->maximumcumulativemass, uncharge(parameters->precursormass, parameters->precursorcharge))) {

		if (terminatecomputation) {
			return -1;
		}

		totaltested++;

		if (totaltested % 1000000 == 0) {
			*os << totaltested << " ";
		}

		if (totaltested % 10000000 == 0) {
			*os << endl;
		}

		usedbrick = false;

		getNameOfCompositionFromIntVector(compositionname, combarray);
		if (j < numberofbasicbricks) {
			bricksdatabasewithcombinations[j].setComposition(compositionname, true);
		}
		else {
			b.clear();
			b.setMass(bricksdatabasewithcombinations.getMassOfComposition(combarray, numberofbasicbricks));
			b.setComposition(compositionname, true);

			if (((parameters->peptidetype == linearpolyketide) || (parameters->peptidetype == cyclicpolyketide)) && !bricksdatabasewithcombinations.checkKetideBlocks(b, parameters->regularblocksorder)) {
				continue;
			}

			bricksdatabasewithcombinations.push_back(b);
		}

		//cout << bricksdatabasewithcombinations.getAcronymName(bricksdatabasewithcombinations[j].getComposition(), false) << " " << bricksdatabasewithcombinations[j].getMass() << endl;

		i = startnode;
		while (i < (int)graph.size()) {

			if (terminatecomputation) {
				return -1;
			}
	
			if (i <= lastsystemnode) {
				if (getNumberOfBricks(bricksdatabasewithcombinations[j].getComposition()) > parameters->maximumbricksincombinationbegin) {
					i = lastsystemnode + 1;
				}
			}

			if ((i > lastsystemnode) && (i < (int)graph.size() - 1)) {
				if (getNumberOfBricks(bricksdatabasewithcombinations[j].getComposition()) > parameters->maximumbricksincombinationmiddle) {
					i = (int)graph.size() - 1;
				}
			}

			if (i == (int)graph.size() - 1) {
				if (getNumberOfBricks(bricksdatabasewithcombinations[j].getComposition()) > parameters->maximumbricksincombinationend) {
					break;
				}
			}

			for (int k = 0; k < (int)parameters->fragmentionsfordenovograph.size(); k++) { 
				
				if (/*!graph[i].ionannotation[parameters->fragmentionsfordenovograph[k]] ||*/ (parameters->fragmentionsfordenovograph[k] == precursor_ion)
					|| (parameters->fragmentionsfordenovograph[k] == linear_polyketide_precursor_ion_h_h) || (parameters->fragmentionsfordenovograph[k] == linear_polyketide_precursor_ion_h_oh) 
					|| (parameters->fragmentionsfordenovograph[k] == linear_polyketide_precursor_ion_oh_oh) || (parameters->fragmentionsfordenovograph[k] == cyclic_polyketide_precursor_ion)
					) {
					continue;
				}

				for (int m = 0; m < (int)parameters->fragmentionsfordenovograph.size(); m++) {
					
					// test for incompatible ion series
					if ((!((parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].nterminal == parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[m]].nterminal) && 
						(parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].cterminal == parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[m]].cterminal)) ||
						(((parameters->peptidetype == linearpolyketide) || (parameters->peptidetype == cyclicpolyketide)) && 
						!(/*((parameters->fragmentionsfordenovograph[k] == l0h_ion) && (parameters->fragmentionsfordenovograph[m] == l0h_ion)) ||*/ ((parameters->fragmentionsfordenovograph[k] == l1h_ion) && (parameters->fragmentionsfordenovograph[m] == l1h_ion)) || ((parameters->fragmentionsfordenovograph[k] == l2h_ion) && (parameters->fragmentionsfordenovograph[m] == l2h_ion))
						|| ((parameters->fragmentionsfordenovograph[k] == l1oh_ion) && (parameters->fragmentionsfordenovograph[m] == l1oh_ion)) || ((parameters->fragmentionsfordenovograph[k] == l2oh_ion) && (parameters->fragmentionsfordenovograph[m] == l2oh_ion)))))						
						&& (parameters->fragmentionsfordenovograph[m] != linear_polyketide_precursor_ion_h_h) && (parameters->fragmentionsfordenovograph[m] != linear_polyketide_precursor_ion_h_oh)
						&& (parameters->fragmentionsfordenovograph[m] != linear_polyketide_precursor_ion_oh_oh) && (parameters->fragmentionsfordenovograph[m] != cyclic_polyketide_precursor_ion)
						&& (parameters->fragmentionsfordenovograph[m] != precursor_ion)) {
						continue;
					}

					// terminal modifications
					for (int n = 0; n < (int)parameters->searchedmodifications.size(); n++) {

						if ((n > 0) && (parameters->fragmentionsfordenovograph[m] != precursor_ion)
							&& (parameters->fragmentionsfordenovograph[m] != linear_polyketide_precursor_ion_h_h) && (parameters->fragmentionsfordenovograph[m] != linear_polyketide_precursor_ion_h_oh)
							&& (parameters->fragmentionsfordenovograph[m] != linear_polyketide_precursor_ion_oh_oh) && (parameters->fragmentionsfordenovograph[m] != cyclic_polyketide_precursor_ion)) {
							continue;
						}

						// middle modifications for branched and branch-cyclic peptides
						for (int p = 0; p < (int)parameters->searchedmodifications.size(); p++) {

							if ((p > 0) && ((parameters->peptidetype == linear) || (parameters->peptidetype == cyclic)
								|| (parameters->peptidetype == linearpolyketide) || (parameters->peptidetype == cyclicpolyketide))) {
								continue;
							}

							termmass = 0;

							if ((parameters->fragmentionsfordenovograph[m] == precursor_ion) 
								|| (parameters->fragmentionsfordenovograph[m] == linear_polyketide_precursor_ion_h_h) || (parameters->fragmentionsfordenovograph[m] == linear_polyketide_precursor_ion_h_oh)
								|| (parameters->fragmentionsfordenovograph[m] == linear_polyketide_precursor_ion_oh_oh) || (parameters->fragmentionsfordenovograph[m] == cyclic_polyketide_precursor_ion)) {

								if ((parameters->peptidetype == linear) || (parameters->peptidetype == linearpolyketide)) {
									if ((n > 0) && ((parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].nterminal && parameters->searchedmodifications[n].nterminal) || (parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].cterminal && parameters->searchedmodifications[n].cterminal))) {
										continue;
									}
								}

								if (parameters->peptidetype == branched) {
									if ((n > 0) && (p > 0) && ((parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].nterminal && parameters->searchedmodifications[n].nterminal && parameters->searchedmodifications[p].nterminal) || (parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].cterminal && parameters->searchedmodifications[n].cterminal && parameters->searchedmodifications[p].cterminal))) {
										continue;
									}
								}

								termmass = parameters->searchedmodifications[n].massdifference;

							}

							for (int z1 = 1; z1 <= abs(parameters->precursorcharge); z1++) {

								if ((z1 > 1) && (i <= lastsystemnode)) {
									break;
								}

								for (int z2 = 1; z2 <= abs(parameters->precursorcharge); z2++) {

									unchargedmz = charge(uncharge(graph[i].getMZRatio(), (parameters->precursorcharge > 0)?z1:-z1), (parameters->precursorcharge > 0)?1:-1);
									if ((!isInPpmMassErrorTolerance(unchargedmz, unchargedprecursormass, parameters->fragmentmasserrortolerance)) && (unchargedmz > unchargedprecursormass)) {
										continue;
									}

									mass = bricksdatabasewithcombinations[j].getMass();
									mass -= parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].massdifference;
									mass += parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[m]].massdifference;				
									mass += termmass;

									if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
										mass += parameters->searchedmodifications[p].massdifference;
									}

									// precursor is connected from the right side (sequence tags)
									if (i == (int)graph.size() - 1) {
										mass = unchargedmz - mass;
									}
									else {
										mass += unchargedmz;
									}

									// search for a peak in a distance determined by a mass of a brick
									left = lastsystemnode + 1;
									right = (int)graph.size() - 2;
									// binary search
									while (left <= right) {

										middle = (left + right) / 2;
										if (isInPpmMassErrorTolerance(charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1), mass, parameters->fragmentmasserrortolerance)) {
											// found
											while ((middle > lastsystemnode + 1) && isInPpmMassErrorTolerance(charge(uncharge(graph[middle - 1].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1), mass, parameters->fragmentmasserrortolerance)) {
												middle--;
											}
											while ((middle < (int)graph.size()) && (isInPpmMassErrorTolerance(charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1), mass, parameters->fragmentmasserrortolerance))) {

												// target node cannot be a system node
												if (middle <= lastsystemnode) {
													middle++;
													continue;
												}

												// avoid cyclic paths
												if (i == middle) {
													middle++;
													continue;
												}

												// mass corresponding to the target node must be <= uncharged precursor mass
												if (charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1) > unchargedprecursormass) {
													middle++;
													continue;
												}

												// irrelevant connection with precursor
												if (((parameters->fragmentionsfordenovograph[m] == precursor_ion) 
													|| (parameters->fragmentionsfordenovograph[m] == linear_polyketide_precursor_ion_h_h) || (parameters->fragmentionsfordenovograph[m] == linear_polyketide_precursor_ion_h_oh)
													|| (parameters->fragmentionsfordenovograph[m] == linear_polyketide_precursor_ion_oh_oh) || (parameters->fragmentionsfordenovograph[m] == cyclic_polyketide_precursor_ion)) 
													&& (middle != (int)graph.size() - 1) && (i != (int)graph.size() - 1)) {
													middle++;
													continue;
												}
																								
												// create an edge
												cEdge e;

												formula.clear();
												string sumstr = bricksdatabasewithcombinations.calculateSummaryFromComposition(j);
												formula.addFormula(sumstr);
												formula.addFormula(parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[k]].summary, true);
												formula.addFormula(parameters->fragmentdefinitions[parameters->fragmentionsfordenovograph[m]].summary);

												if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
													formula.addFormula(parameters->searchedmodifications[p].summary);
												}

												if (termmass != 0) {
													formula.addFormula(parameters->searchedmodifications[n].summary);
												}
									
												if (i == (int)graph.size() - 1) {
													e.targetnode = i;
													e.brickid = j;
													e.ppmerror = ppmError(charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1), mass);
													e.massdifference = abs(unchargedmz - charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1));
													e.sourcecharge = (parameters->precursorcharge > 0)?z2:-z2;
													e.targetcharge = (parameters->precursorcharge > 0)?z1:-z1;
													e.targetintensity = graph[i].getIntensity();
													e.composition = bricksdatabasewithcombinations[j].getComposition();
													e.summary = formula.getSummary();
													e.endmodifID = n;
													e.middlemodifID = p;
													e.sourceion = parameters->fragmentionsfordenovograph[k];
													e.targetion = parameters->fragmentionsfordenovograph[m];

													// insert the new edge and/or attach the annotation to the source node if the edge exists
													graph[middle].insertTempEdge(e);
												}
												else {
													e.targetnode = middle;
													e.brickid = j;
													e.ppmerror = ppmError(charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1), mass);
													e.massdifference = abs(charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1) - unchargedmz);
													e.sourcecharge = (parameters->precursorcharge > 0)?z1:-z1;
													e.targetcharge = (parameters->precursorcharge > 0)?z2:-z2;
													e.targetintensity = graph[middle].getIntensity();
													e.composition = bricksdatabasewithcombinations[j].getComposition();
													e.summary = formula.getSummary();
													e.endmodifID = n;
													e.middlemodifID = p;

													// insert the new edge and/or attach the annotation to the source node if the edge exists
													graph[i].insertTempEdge(e);
												}

												usedbrick = true;

												middle++;
											}
											break;
										}
										if (mass < charge(uncharge(graph[middle].getMZRatio(), (parameters->precursorcharge > 0)?z2:-z2), (parameters->precursorcharge > 0)?1:-1)) {
											right = middle - 1;
										}
										else {
											left = middle + 1;
										}

									}

								}

							}

						}

					}

				}

			}

			i++;

		}

		if (((int)bricksdatabasewithcombinations.size() > numberofbasicbricks) && (!usedbrick)) {
			bricksdatabasewithcombinations.removeLastBrick();
		}
		else {
			j++;
		}

	}

	createVectorsOfEdges();

	*os << endl << "The de novo graph has been successfully created." << endl;
	*os << "Number of bricks combinations used: " << bricksdatabasewithcombinations.size() << endl;
	*os << "Number of nodes: " << graph.size() << endl;
	*os << "Number of edges: " << getNumberOfEdges() << endl << endl;
	
	return 0;
}


int cDeNovoGraph::removeEdgesFormingPathsNotFinishingInPrecursor(bool& terminatecomputation) {
	*os << "Removing edges forming incomplete paths (not finishing in the precursor)... ";
	double neutralprecursormass = uncharge(parameters->precursormass, parameters->precursorcharge);
	bool removed = true;
	bool isprecursor;
	int edgesremoved = 0;
	int j;
	while (removed) {
		removed = false;
		for (int i = startnode; i < (int)graph.size(); i++) {

			if (terminatecomputation) {
				return -1;
			}

			j = 0;
			while (j < (int)graph[i].size()) {

				// Is the target precursor ?
				isprecursor = false;
				for (int k = 1; k <= abs(parameters->precursorcharge); k++) {
					if (isInPpmMassErrorTolerance(graph[graph[i][j].targetnode].getMZRatio(), charge(neutralprecursormass, (parameters->precursorcharge > 0)?k:-k), parameters->precursormasserrortolerance)) {
						isprecursor = true;
						break;
					}
				}

				// the target has not any outgoing edge and the target is not the precursor
				if ((graph[graph[i][j].targetnode].size() == 0) && !isprecursor) {
					graph[i].removeEdge(j);
					edgesremoved++;
					removed = true;
				}
				else {
					j++;
				}
			}
		}
	}
	*os << "ok" << endl;
	*os << "Edges removed: " << edgesremoved << endl;
	*os << "Number of edges: " << getNumberOfEdges() << endl << endl;

	return 0;
}
	

int cDeNovoGraph::removeEdgesFormingPathsNotStartingFromFirstNode(bool& terminatecomputation) {
	*os << "Removing edges forming incomplete paths (not starting from the first node)... ";
	bool removed = true;
	int edgesremoved = 0;
	bool match;

	while (removed) {
		removed = false;
		for (int i = lastsystemnode + 1; i < (int)graph.size(); i++) {

			if (terminatecomputation) {
				return -1;
			}

			// Am I a target node ?
			match = false;
			for (int j = startnode; j < (int)graph.size(); j++) {
				for (int k = 0; k < (int)graph[j].size(); k++) {
					if (graph[j][k].targetnode == i) {
						match = true;
						break;
					}
				}
				if (match) {
					break;
				}
			}

			// remove all outgoing edges
			if (!match) {
				edgesremoved += (int)graph[i].size();
				graph[i].clearEdges();
			}

		}
	}
	*os << "ok" << endl;
	*os << "Edges removed: " << edgesremoved << endl;
	*os << "Number of edges: " << getNumberOfEdges() << endl << endl;

	return 0;
}
	

int cDeNovoGraph::connectEdgesFormingPathsNotFinishingInPrecursor(bool& terminatecomputation) {
	*os << "Connecting edges with the precursor's node to generate sequence tags... ";
	//*os << "Connecting edges forming incomplete paths (not finishing in the precursor)... ";
	int edgescreated = 0;
	int precursorposition = (int)graph.size() - 1;

	for (int i = lastsystemnode + 1; i < precursorposition; i++) {

		if (terminatecomputation) {
			return -1;
		}

		// the target has not any outgoing edge and the target is not the precursor
		//if (!finishInPrecursor(i, graph[i].getMZRatio())) {
			cEdge e;
			e.targetnode = (int)graph.size() - 1;
			e.brickid = (int)bricksdatabasewithcombinations.size() + 1;
			e.ppmerror = 0;
			e.massdifference = abs(graph[precursorposition].getMZRatio() - graph[i].getMZRatio());
			e.sourcecharge = (parameters->precursorcharge > 0)?1:-1;
			e.targetcharge = (parameters->precursorcharge > 0)?1:-1;
			e.targetintensity = graph[e.targetnode].getIntensity();
			e.composition = to_string(e.brickid);
			e.endmodifID = 0;
			e.middlemodifID = 0;
			graph[i].insertEdge(e);
			edgescreated++;

			cBrick b;
			string s = "unknown";

			// normal brick
			b.setName(to_string(e.massdifference));
			b.setAcronyms(to_string(e.massdifference));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference);
			b.setComposition(to_string(e.brickid), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);


			// -H2O brick (1)
			b.setName(to_string(e.massdifference - H2O));
			b.setAcronyms(to_string(e.massdifference - H2O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference - H2O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);

			// +H2O brick (2)
			b.setName(to_string(e.massdifference + H2O));
			b.setAcronyms(to_string(e.massdifference + H2O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference + H2O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);

			// -H2 brick (3)
			b.setName(to_string(e.massdifference - 2*H));
			b.setAcronyms(to_string(e.massdifference - 2*H));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference - 2*H);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);

			// -O brick (4)
			b.setName(to_string(e.massdifference - O));
			b.setAcronyms(to_string(e.massdifference - O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference - O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);
/*
			// +O brick (5)
			b.setName(to_string(e.massdifference + O));
			b.setAcronyms(to_string(e.massdifference + O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference + O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);

			// -O2 brick (6)
			b.setName(to_string(e.massdifference - 2*O));
			b.setAcronyms(to_string(e.massdifference - 2*O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference - 2*O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);

			// -H2+O brick (7)
			b.setName(to_string(e.massdifference - 2*H + O));
			b.setAcronyms(to_string(e.massdifference - 2*H + O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference - 2*H + O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);

			// -H2O2 brick (8)
			b.setName(to_string(e.massdifference - 2*H - 2*O));
			b.setAcronyms(to_string(e.massdifference - 2*H - 2*O));
			b.setReferences(s);
			b.setSummary(s);
			b.setMass(e.massdifference - 2*H - 2*O);
			b.setComposition(to_string((int)bricksdatabasewithcombinations.size() + 1), false);
			b.setArtificial(true);
			bricksdatabasewithcombinations.push_back(b);
*/

		//}

	}

	*os << "ok" << endl;
	*os << "Edges created: " << edgescreated << endl;
	*os << "Number of edges: " << getNumberOfEdges() << endl << endl;

	return 0;
}
	

int cDeNovoGraph::connectEdgesFormingPathsNotStartingFromFirstNode(bool& terminatecomputation) {
	*os << "Connecting edges with the starting node to generate sequence tags... ";
	//*os << "Connecting edges forming incomplete paths (not starting from the first node)... ";
	int edgescreated = 0;
	//bool match;

	for (int i = lastsystemnode + 1; i < (int)graph.size() - 1; i++) {

		if (terminatecomputation) {
			return -1;
		}

		// Am I a target node ?
		/*match = false;
		for (int j = startnode; j < (int)graph.size() - 1; j++) {
			for (int k = 0; k < (int)graph[j].size(); k++) {
				if (graph[j][k].targetnode == i) {
					match = true;
					break;
				}
			}
			if (match) {
				break;
			}
		}*/

		// connect with starting nodes
		//if (!match) {
			for (int j = startnode; j <= lastsystemnode; j++) {
				cEdge e;
				e.targetnode = i;
				e.brickid = (int)bricksdatabasewithcombinations.size() + 1;
				e.ppmerror = 0;
				e.massdifference = abs(graph[i].getMZRatio() - graph[j].getMZRatio());
				e.sourcecharge = (parameters->precursorcharge > 0)?1:-1;
				e.targetcharge = (parameters->precursorcharge > 0)?1:-1;
				e.targetintensity = graph[i].getIntensity();
				e.composition = to_string(e.brickid);
				e.endmodifID = 0;
				e.middlemodifID = 0;
				graph[j].insertEdge(e);
				edgescreated++;

				cBrick b;
				b.setName(to_string(e.massdifference));
				b.setAcronyms(to_string(e.massdifference));
				string s = "unknown";
				b.setReferences(s);
				b.setSummary(s);
				b.setMass(e.massdifference);
				b.setComposition(to_string(e.brickid), false);
				b.setArtificial(true);
				bricksdatabasewithcombinations.push_back(b);
			}
		//}
	
	}

	*os << "ok" << endl;
	*os << "Edges created: " << edgescreated << endl;
	*os << "Number of edges: " << getNumberOfEdges() << endl << endl;

	return 0;
}
	

int cDeNovoGraph::removePathsWhichCanBeSubstitutedByLongerPath(bool& terminatecomputation) {
	*os << "Removing edges forming paths which can be substituted by longer paths... ";
	int edgesremoved = 0;
	int j;
	vector<nodeEdge> path;
	
	for (int i = startnode; i < (int)graph.size(); i++) {

		if (terminatecomputation) {
			return -1;
		}
		
		//*os << i << " ";
		j = 0;
		while (j < (int)graph[i].size()) {
			cBrick b;
			b.setComposition(graph[i][j].composition, true);
			path.clear();
			if (findPath(i, j, graph[i][j].targetnode, b.getComposition(), "", parameters->maximumbricksincombination, path, terminatecomputation)) {
				//*os << "removing edge: " << graph[i].getMZRatio() << "->" << graph[graph[i][j].targetnode].getMZRatio() << " " << bricksdatabasewithcombinations.getTagName(b.getComposition()) << endl;
				graph[i].removeEdge(j);
				edgesremoved++;
			}
			else {
				j++;
			}

			if (terminatecomputation) {
				return -1;
			}

		}
	}
	*os << "ok" << endl;
	*os << "Edges removed: " << edgesremoved << endl;
	*os << "Number of edges: " << getNumberOfEdges() << endl << endl;
    
	return 0;
}


void cDeNovoGraph::startGraphReader(cCandidateSet& candidates, long long unsigned& count, int scanmode, bool& terminatecomputation) {
	graphreaderthread = new cGraphReaderThread();
	graphreaderthread->initialize(graph, bricksdatabasewithcombinations, candidates, parameters, os, lastsystemnode, count, scanmode, terminatecomputation);
	os->connect(graphreaderthread, SIGNAL(finished()), os, SLOT(graphReaderFinished()));
	os->setGraphReaderIsWorking(true);
	graphreaderthread->start();
}


int cDeNovoGraph::getNumberOfEdges() {
	int edges = 0;
	for (int i = 0; i < (int)graph.size(); i++) {
		edges += (int)graph[i].size();
	}
	return edges;
}


void cDeNovoGraph::printPaths(cMainThread* os, cDeNovoGraphNode& node, vector<string>& path, vector<string>& composition) {
	string s;

	if (node.size() > 0) {
		for (int i = 0; i < (int)node.size(); i++) {
			if (node.getMZRatio() > 0) {
				s = to_string(node.getMZRatio());
				s += " (";
				s += node[i].composition;
				s += ")";
				path.push_back(s);
				composition.push_back(node[i].composition);
			}
			printPaths(os, graph[node[i].targetnode], path, composition);
			if (node.getMZRatio() > 0) {
				path.pop_back();
				composition.pop_back();
			}
		}
	}
	else {
		s = to_string(node.getMZRatio());
		path.push_back(s);

		vector<nodeEdge> tmp;
		cCandidate candidate(composition, tmp, fragmentIonTypeEnd, 0, 0, 0, -1);
	
		*os << "bricks: " << getNumberOfBricks(candidate.getComposition()) << " edges: " << composition.size() << " composition: ";
		*os << candidate.getComposition();
		*os << " path: ";
		for (int i = 0; i < (int)path.size(); i++) {
			*os << path[i];
			if (i < (int)path.size() - 1) {
				*os << " -> ";
			}
		}
		*os << endl;

		path.pop_back();
	}

}


string cDeNovoGraph::printGraph() {
	const int edgesmaxreport = 200;
	string s, s2;
	string g = "";

	for (int i = 0; i < (int)graph.size(); i++) {
		
		s = to_string(graph[i].getMZRatio());

		if ((int)graph[i].size() > 0) {
			g += "The node " + s + " with relative intensity " + to_string(graph[i].getIntensity()) + " has " + to_string(graph[i].size()) + " outgoing edge(s):<br/>";
		}
		else {
			g += "The node " + s + " with relative intensity " + to_string(graph[i].getIntensity()) + " does not have any outgoing edge.<br/>";
		}
		
		//g += "Possible annotations: ";
		//g += graph[i].printAnnotations();
		//g += "\n";
		
		for (int j = 0; j < min((int)graph[i].size(), edgesmaxreport); j++) {

			s2 = to_string(graph[graph[i][j].targetnode].getMZRatio());
			g += s + "->" + s2 + " using brick(s): ";
			if (graph[i][j].composition.compare("0") == 0) {
				g += "none";
			}
			else {
				g += bricksdatabasewithcombinations.getAcronymName(graph[i][j].composition, true);
			}
			s2 = to_string(graph[i][j].ppmerror);
			g += " (mass difference: " + to_string(graph[i][j].massdifference) + ", ";
			g += "target intensity: " + to_string(graph[graph[i][j].targetnode].getIntensity()) + ", ";
			g += "ppm error: " + s2 + ", ";
			g += "source charge: " + to_string(graph[i][j].sourcecharge) + ", ";
			g += "target charge: " + to_string(graph[i][j].targetcharge);
			if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
				if (graph[i][j].middlemodifID > 0) {
					g += ", branch modification: " + parameters->searchedmodifications[graph[i][j].middlemodifID].name;
				}
			}
			if (graph[i][j].endmodifID > 0) {
				g += ", terminal modification: " + parameters->searchedmodifications[graph[i][j].endmodifID].name;
			}
			g += ", summary: " + graph[i][j].summary;
			//g += ", source annotation" + to_string(graph[i][j].sourceion);
			//g += ", target annotation" + to_string(graph[i][j].targetion);
			//g += graph[i][j].printSourceAnnotation(fragmentdefinitions);
			//g += "->";
			//g += graph[i][j].printTargetAnnotation(fragmentdefinitions);
			g += ")<br/>";
		}

		if ((int)graph[i].size() > edgesmaxreport) {
			g += "... and " + to_string((int)graph[i].size() - edgesmaxreport) + " other edge(s).<br/>";
		}

		g += "<br/>";
	}

	//vector<string> path;
	//vector<string> composition;
	//printPaths(os,graph[0],path,composition);

	return g;
}


cDeNovoGraphNode& cDeNovoGraph::operator[](int position) {
	return graph[position];
}


void cDeNovoGraph::sortEdgesByPPMError() {
	for (int i = 0; i < (int)graph.size(); i++) {
		graph[i].sortEdgesByPPMError();
	}
}


void cDeNovoGraph::sortEdgesByTargetNodeIntensity() {
	for (int i = 0; i < (int)graph.size(); i++) {
		graph[i].sortEdgesByTargetNodeIntensity();
	}
}


cBricksDatabase* cDeNovoGraph::getBrickDatabaseWithCombinations() {
	return &bricksdatabasewithcombinations;
}

