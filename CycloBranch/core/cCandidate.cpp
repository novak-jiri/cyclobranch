#include "core/cCandidate.h"

#include "core/cCandidateSet.h"
#include "core/cDeNovoGraph.h"


void cCandidate::updateInternalComposition() {
	internalcomposition = "";
	for (int i = 0; i < (int)composition.size(); i++) {
		internalcomposition += composition[i];
		if (i < (int)composition.size() - 1) {
			internalcomposition += "-";
		}
	}

	numberofinternalbricks = getNumberOfBricks(internalcomposition);
}


void cCandidate::getPermutationsIter(cCandidateSet& permutations, vector<string>& currentcandidate, int position, bool* terminatecomputation) {
	if (*terminatecomputation) {
		return;
	}

	vector<string> bricks, currentpermutation;
	vector<string> localpermutations;

	bricks.clear();
	localpermutations.clear();

	if (position < (int)composition.size()) {
		cBrick b;
		b.setComposition(composition[position], false);
		b.explodeToStringComposition(bricks);
		generateBricksPermutations(bricks, currentpermutation, localpermutations);
		
		for (int k = 0; k < (int)localpermutations.size(); k++) {
			currentcandidate.push_back(localpermutations[k]);
			getPermutationsIter(permutations, currentcandidate, position + 1, terminatecomputation);
			currentcandidate.pop_back();
		}
	}
	else {
		cCandidate c(currentcandidate, path, startiontype, startmodifID, endmodifID, middlemodifID, branchstart, branchend);
		c.setSummaryFormula(summary);
		permutations.getSet().insert(c);
	}
}


void cCandidate::attachSubBranchCandidates(cCandidate& candidate, cCandidateSet& result, ePeptideType peptidetype, bool* terminatecomputation) {
	if (*terminatecomputation) {
		return;
	}

	int start = candidate.branchstart;
	int end = candidate.branchend;

	if (peptidetype == branched) {
		if (start == 0) {
			start = 1;
		}

		if (end == candidate.numberofinternalbricks - 1) {
			end--;
		}
	}

	if (end <= start) {
		return;
	}
	
	cCandidate c;
	for (int i = start; i <= end; i++) {
		for (int j = i + 1; j <= end; j++) {
			c = candidate;
			c.branchstart = i;
			c.branchend = j;
			result.getSet().insert(c);
		}
	}
}


void cCandidate::attachAllBranches(cCandidate& candidate, cCandidateSet& result, ePeptideType peptidetype, bool* terminatecomputation) {
	cCandidate c;
	int start, end;
	int cumsize = 0;
	for (int i = 0; i < (int)candidate.composition.size(); i++) {
		start = cumsize;
		cumsize += getNumberOfBricks(candidate.composition[i]);
		end = cumsize - 1;
		c = candidate;
		c.branchstart = start;
		c.branchend = end;
		attachSubBranchCandidates(c, result, peptidetype, terminatecomputation);
	}
}


void cCandidate::getPartialRotations(const string& composition, vector<string>& rotations) {
	cBrick b;
	string s;
	vector<string> bricks;
	b.setComposition(composition, false);
	b.explodeToStringComposition(bricks);

	int count = (int)bricks.size();
	for (int i = 0; i < count; i++) {
		s = "";
		for (int j = 0; j < count; j++) {
			s += bricks[(i + j) % count];
			if (j < count - 1) {
				s += "-";
			}
		}
		rotations.push_back(s);
		//cout << s << endl;
	}
	
	/*
	int position;
	int count = getNumberOfBricks(rotation);	
	for (int i = 0; i < count - 1; i++) {
		position = (int)rotation.find('-');
		if (position == string::npos) {
			cout << "Sequence cannot be rotated." << endl;
			exit(1);
		}
		else {
			rotation = rotation.substr(position + 1) + "-" + rotation.substr(0, position);
			rotations.push_back(rotation);
		}
	}
	*/
}


void cCandidate::getPartialBranchCyclicRotations(const string& composition, vector<cCandidate>& branchcyclicrotations, int branchstart, int branchend) {
	cBrick b;
	string s;
	bool leftbracketput;
	vector<string> bricks;
	b.setComposition(composition, false);
	b.explodeToStringComposition(bricks);
	int count = (int)bricks.size();
	for (int i = 0; i < count; i++) {
		s = "";
		leftbracketput = false;
		for (int j = 0; j < count; j++) {
			if (branchstart == (i + j) % count) {
				//s += "(";
				if (j > 0) {
					s += "-";
				}
				leftbracketput = true;
			}
			s += bricks[(i + j) % count];
			if (branchend == (i + j) % count) {
				if (!leftbracketput) {
					break;
				}
				//s += ")";
				if (j < count - 1) {
					s += "-";
				}
			}
			else {
				if ((j < count - 1) && (branchstart != (i + j + 1) % count)) {
					s += "-";
				}
			}
		}
		
		if (leftbracketput) {
			vector<string> v;
			vector<nodeEdge> ne;
			v.push_back(s);
			cCandidate c(v, ne, startiontype, startmodifID, endmodifID, middlemodifID, (branchstart + count - i) % count, (branchend + count - i) % count);
			branchcyclicrotations.push_back(c);
			//cout << s << endl;
		}
	}
}


cCandidate::cCandidate() {
	clear();
}


cCandidate::cCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int middlepos) {
	clear();
	setCandidate(composition, path, startiontype, startmodifID, endmodifID, middlemodifID, middlepos);
}


cCandidate::cCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int branchstart, int branchend) {
	clear();
	setCandidate(composition, path, startiontype, startmodifID, endmodifID, middlemodifID, branchstart, branchend);
}


void cCandidate::setCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int middlepos) {
	clear();

	this->composition = composition;
	updateInternalComposition();

	this->startmodifID = startmodifID;
	this->endmodifID = endmodifID;
	this->middlemodifID = middlemodifID;
	this->path = path;
	this->startiontype = startiontype;

	if (middlepos == -1) {
		middlemodifID = 0;
		branchstart = -1;
		branchend = -1;
		return;
	}

	branchstart = 0;
	for (int i = 0; i < middlepos; i++) {
		branchstart += getNumberOfBricks(composition[i]);
	}
	branchend = branchstart + getNumberOfBricks(composition[middlepos]) - 1;
}


void cCandidate::setCandidate(vector<string>& composition, vector<nodeEdge>& path, eFragmentIonType startiontype, int startmodifID, int endmodifID, int middlemodifID, int branchstart, int branchend) {
	clear();

	this->composition = composition;
	updateInternalComposition();

	this->startmodifID = startmodifID;
	this->endmodifID = endmodifID;
	this->middlemodifID = middlemodifID;
	this->branchstart = branchstart;
	this->branchend = branchend;
	this->path = path;
	this->startiontype = startiontype;
}


void cCandidate::clear() {
	composition.clear();
	internalcomposition = "";
	numberofinternalbricks = 0;
	startmodifID = 0;
	endmodifID = 0;
	middlemodifID = 0;
	branchstart = -1;
	branchend = -1;
	path.clear();
	startiontype = fragmentIonTypeEnd;
	name = "";
	summary.clear();

	realpeptidename = "";
	acronympeptidename = "";
	acronyms.clear();
	backboneacronyms.clear();
	branchacronyms.clear();
	stringpath = "";
}


string& cCandidate::getComposition() {
	return internalcomposition;
}


void cCandidate::revertComposition() {
	cBrick b1, b2;
	int temp;

	for(int i = 0; i < (int)composition.size() / 2; i++) {
		b1.setComposition(composition[i], false);
		b2.setComposition(composition[(int)composition.size()-i-1], false);
		composition[i] = b2.getReverseComposition();
		composition[(int)composition.size()-i-1] = b1.getReverseComposition();
	}

	if ((int)composition.size() % 2 == 1) {
		b1.setComposition(composition[(int)composition.size() / 2], false);
		composition[(int)composition.size() / 2] = b1.getReverseComposition();
	}

	updateInternalComposition();

	temp = startmodifID;
	startmodifID = endmodifID;
	endmodifID = temp;

	if ((branchstart != -1) && (branchend != -1)) {
		temp = numberofinternalbricks - branchstart - 1;
		branchstart = numberofinternalbricks - branchend - 1;
		branchend = temp;
	}

}


void cCandidate::prepareBranchedCandidates(cCandidateSet& result, ePeptideType peptidetype, bool* terminatecomputation) {
	cCandidate c;
	result.getSet().clear();

	if (peptidetype == branchcyclic) {

		// just one from startmodifid and middlemodifid may be > 0 (checked in getCandidatesIter)
		// endmodifid is always 0 because precursor_ion is not used in initializeFragmentIonsForDeNovoGraphOfBranchCyclicPeptides
		// startmodifid is always N-terminal because only b-ions are used in the de novo graph

		if ((startmodifID > 0) && (composition.size() > 0)) {
			middlemodifID = startmodifID;
			startmodifID = 0;
			branchstart = 0;
			
			cBrick b;
			vector<int> intvector;
			b.setComposition(composition[0], false);
			b.explodeToIntComposition(intvector);
			branchend = (int)intvector.size() - 1;
		}

		// branch-cyclic series types D,H
		if ((branchstart == -1) || (branchstart == 0)) {
			for (int i = 1; i < numberofinternalbricks - 1; i++) {
				c = *this;
				c.branchstart = 0;
				c.branchend = i;
				c.revertComposition();
				result.getSet().insert(c);
			}
		}

		// branch-cyclic series types B,C,F,G
		if ((branchend == -1) || (branchend == numberofinternalbricks - 1)) {
			for (int i = 1; i < numberofinternalbricks - 1; i++) {
				c = *this;
				c.branchstart = i;
				c.branchend = numberofinternalbricks - 1;
				result.getSet().insert(c);
			}
		}

	}

	if (peptidetype == branched) {

		// below, middlemodifID is always > 0 because branchstart == 0 (or branchend == numberofinternalbricks - 1)

		// startmodifID and middlemodifID collide on the first detected combination of bricks
		if (branchstart == 0) {
			if (startmodifID > 0) {
				c = *this;
				c.startmodifID = middlemodifID;
				c.middlemodifID = startmodifID;
				attachSubBranchCandidates(c, result, peptidetype, terminatecomputation);
			}
			else {
				c = *this;
				c.startmodifID = middlemodifID;
				c.middlemodifID = 0;
				c.branchstart = -1;
				c.branchend = -1;
				attachAllBranches(c, result, peptidetype, terminatecomputation);
			}
		}

		// endmodifID and middlemodifID collide on the last detected combination of bricks
		if (branchend == numberofinternalbricks - 1) {
			if (endmodifID > 0) {
				c = *this;
				c.endmodifID = middlemodifID;
				c.middlemodifID = endmodifID;
				attachSubBranchCandidates(c, result, peptidetype, terminatecomputation);
			}
			else {
				c = *this;
				c.endmodifID = middlemodifID;
				c.middlemodifID = 0;
				c.branchstart = -1;
				c.branchend = -1;
				attachAllBranches(c, result, peptidetype, terminatecomputation);
			}
		}

	}

	// branching is unknown
	if ((branchstart == -1) && (branchend == -1)) {
		attachAllBranches(*this, result, peptidetype, terminatecomputation);
	}
	// branching is determined by a modification of the T-branch
	else {
		attachSubBranchCandidates(*this, result, peptidetype, terminatecomputation);
	}

}


string cCandidate::getTComposition() {
	vector<string> brickscomposition;
	cBrick b;
	string s = "";

	b.clear();
	b.setComposition(internalcomposition, false);
	b.explodeToStringComposition(brickscomposition);
	for (int i = 0; i < (int)brickscomposition.size(); i++) {
		if (branchstart == i) {
			s += "(";
		}
		s += brickscomposition[i];
		if (branchend == i) {
			s += ")";
		}
		else {
			if ((i < (int)brickscomposition.size() - 1) && (branchstart != i + 1)) {
				s += "-";
			}
		}
	}
	return s;
}


string cCandidate::getRevertedTComposition(bool usebrackets) {
	cBrick b, b2;
	vector<string> brickscomposition;
	b.setComposition(internalcomposition, false);
	b2.setComposition(b.getReverseComposition(), false);
	b2.explodeToStringComposition(brickscomposition);

	int bstart = numberofinternalbricks - branchend - 1;
	int bend = numberofinternalbricks - branchstart - 1;

	string s = "";
	for (int i = 0; i < (int)brickscomposition.size(); i++) {
		if (bstart == i) {
			s += "(";
		}
		s += brickscomposition[i];
		if (bend == i) {
			s += ")";
		}
		else {
			if ((i < (int)brickscomposition.size() - 1) && (bstart != i + 1)) {
				s += "-";
			}
		}
	}
	
	int p11 = (int)s.find('(');
	int p12 = (int)s.find(')') - p11 + 1;
	string t = getTComposition();
	int p21 = (int)t.find('(');
	int p22 = (int)t.find(')') - p21 + 1;
	s.replace(p11, p12, t, p21, p22);

	if (!usebrackets) {
		s[s.find('(')] = '-';
		s[s.find(')')] = '-';

		if (s[0] == '-') {
			s.erase(s.begin());
		}

		if (s[s.size() - 1] == '-') {
			s.erase(s.end() - 1);
		}
	}

	return s;
}


string cCandidate::getAcronymsTComposition(cBricksDatabase& brickdatabase) {
	vector<string> brickscomposition;
	cBrick b;
	string s = "";

	b.clear();
	b.setComposition(internalcomposition, false);
	b.explodeToStringComposition(brickscomposition);
	for (int i = 0; i < (int)brickscomposition.size(); i++) {
		if (branchstart == i) {
			s += " ( ";
		}
		s += brickdatabase.getAcronymName(brickscomposition[i], true);
		if (branchend == i) {
			s += " ) ";
		}
		else {
			if ((i < (int)brickscomposition.size() - 1) && (branchstart != i + 1)) {
				s += "-";
			}
		}
	}
	return s;
}


string cCandidate::getRealNameTComposition(cBricksDatabase& brickdatabase) {
	vector<string> brickscomposition;
	cBrick b;
	string s = "";

	b.clear();
	b.setComposition(internalcomposition, false);
	b.explodeToStringComposition(brickscomposition);
	for (int i = 0; i < (int)brickscomposition.size(); i++) {
		if (branchstart == i) {
			s += " ( ";
		}
		s += brickdatabase.getRealName(brickscomposition[i]);
		if (branchend == i) {
			s += " ) ";
		}
		else {
			if ((i < (int)brickscomposition.size() - 1) && (branchstart != i + 1)) {
				s += "-";
			}
		}
	}
	return s;
}


int cCandidate::getBranchSize() {
	if ((branchstart >= 0) && (branchend >= 0)) {
		return branchend-branchstart;
	}
	return 0;
}


int cCandidate::getBranchStart() {
	return branchstart;
}


int cCandidate::getBranchEnd() {
	return branchend;
}


void cCandidate::getPermutationsOfBranches(vector<TRotationInfo>& tpermutations) {
	tpermutations.resize(6);
	for (int i = 0; i < (int)tpermutations.size(); i++) {
		tpermutations[i].clear();
	}

	vector<int> defaultintcomposition, intcomposition, tempintvector;
	cBrick b;
	b.clear();
	b.setComposition(internalcomposition, false);
	defaultintcomposition.clear();
	b.explodeToIntComposition(defaultintcomposition);

	// 0 = n-terminal
	tpermutations[0].id = 0;
	tpermutations[0].bricks = defaultintcomposition;
	tpermutations[0].startmodifID = startmodifID;
	tpermutations[0].endmodifID = endmodifID;
	tpermutations[0].middlemodifID = middlemodifID;
	tpermutations[0].tcomposition = getTComposition();
	tpermutations[0].middlebranchstart = branchstart;
	tpermutations[0].middlebranchend = branchend;


	// first and second branch - revert order and switch
	intcomposition.clear();
	tpermutations[1].tcomposition = "";

	// push reverted second branch into intcomposition
	for (int i = branchend; i >= branchstart; i--) {
		intcomposition.push_back(defaultintcomposition[i]);

		if (i == branchstart) {
			if (tpermutations[1].tcomposition.size() > 0) {
				tpermutations[1].tcomposition.back() = '(';
			}
			else {
				tpermutations[1].tcomposition += '(';
			}
		}

		tpermutations[1].tcomposition += to_string(defaultintcomposition[i]) + "-";
	}
	tpermutations[1].middlebranchstart = (int)intcomposition.size() - 1;

	// push reverted first branch into intcomposition
	for (int i = branchstart - 1; i >= 0; i--) {
		intcomposition.push_back(defaultintcomposition[i]);
		tpermutations[1].tcomposition += to_string(defaultintcomposition[i]) + "-";
	}
	tpermutations[1].middlebranchend = (int)intcomposition.size() - 1;
	tpermutations[1].tcomposition.back() = ')';

	// push third branch into intcomposition
	for (int i = branchend + 1; i < (int)defaultintcomposition.size(); i++) {
		intcomposition.push_back(defaultintcomposition[i]);
		tpermutations[1].tcomposition += to_string(defaultintcomposition[i]) + "-";
	}

	// 1 = middle modification type dependent
	tpermutations[1].id = 1;
	tpermutations[1].bricks = intcomposition; 
	tpermutations[1].startmodifID = middlemodifID;
	tpermutations[1].endmodifID = endmodifID;
	tpermutations[1].middlemodifID = startmodifID;
	if (tpermutations[1].tcomposition.back() != ')') {
		tpermutations[1].tcomposition = tpermutations[1].tcomposition.substr(0,tpermutations[1].tcomposition.size() - 1);
	}
		

	// first and third branch - revert order and switch
	intcomposition.clear();
	tpermutations[2].tcomposition = "";
	int interpos = 0;

	for (int i = (int)defaultintcomposition.size() - 1; i >= 0; i--) {

		if (i == branchend) {
			if (tpermutations[2].tcomposition.size() > 0) {
				tpermutations[2].tcomposition.back() = '(';
			}
			else {
				tpermutations[2].tcomposition += '(';
			}
			tpermutations[2].middlebranchstart = (int)intcomposition.size();
		}

		if ((i >= branchstart) && (i <= branchend)) {
			intcomposition.push_back(defaultintcomposition[branchstart+interpos]);
			tpermutations[2].tcomposition += to_string(defaultintcomposition[branchstart+interpos]) + "-";
			interpos++;
		}
		else {
			intcomposition.push_back(defaultintcomposition[i]);
			tpermutations[2].tcomposition += to_string(defaultintcomposition[i]) + "-";
		}

		if (i == branchstart) {
			tpermutations[2].middlebranchend = (int)intcomposition.size() - 1;
			tpermutations[2].tcomposition.back() = ')';
		}

	}

	// 2 = c-terminal
	tpermutations[2].id = 2;
	tpermutations[2].bricks = intcomposition; 
	tpermutations[2].startmodifID = endmodifID;
	tpermutations[2].endmodifID = startmodifID;
	tpermutations[2].middlemodifID = middlemodifID;
	if (tpermutations[2].tcomposition.back() != ')') {
		tpermutations[2].tcomposition = tpermutations[2].tcomposition.substr(0,tpermutations[2].tcomposition.size() - 1);
	}


	// second and third branch - switch (3x)
	for (int i = 0; i < 3; i++) {
		intcomposition = tpermutations[i].bricks;
		tempintvector.clear();
		tpermutations[i+3].tcomposition = "";

		// backup the middle branch
		for (int j = tpermutations[i].middlebranchstart + 1; j <= tpermutations[i].middlebranchend; j++) {
			tempintvector.push_back(intcomposition[j]);
		}

		// begin
		for (int j = 0; j < tpermutations[i].middlebranchstart; j++) {
			tpermutations[i+3].tcomposition += to_string(intcomposition[j]) + "-";
		}
		
		if (tpermutations[i+3].tcomposition.size() > 0) {
			tpermutations[i+3].tcomposition.back() = '(';
		}
		else {
			tpermutations[i+3].tcomposition += '(';
		}

		// new middle start
		tpermutations[i+3].tcomposition += to_string(intcomposition[tpermutations[i].middlebranchstart]) + "-";

		// new middle
		for (int j = tpermutations[i].middlebranchend + 1; j < (int)intcomposition.size(); j++) {
			tpermutations[i+3].tcomposition += to_string(intcomposition[j]) + "-";
		}
		tpermutations[i+3].tcomposition.back() = ')';

		// erase the old middle branch
		intcomposition.erase(intcomposition.begin() + tpermutations[i].middlebranchstart + 1, intcomposition.begin() + tpermutations[i].middlebranchend + 1);

		tpermutations[i+3].middlebranchstart = tpermutations[i].middlebranchstart;
		tpermutations[i+3].middlebranchend = (int)intcomposition.size() - 1;

		// new end
		for (int j = 0; j < (int)tempintvector.size(); j++) {
			intcomposition.push_back(tempintvector[j]);
			tpermutations[i+3].tcomposition += to_string(tempintvector[j]) + "-";
		}
		tpermutations[i+3].id = i + 3;
		tpermutations[i+3].bricks = intcomposition;
		tpermutations[i+3].startmodifID = tpermutations[i].startmodifID;
		tpermutations[i+3].endmodifID = tpermutations[i].middlemodifID;
		tpermutations[i+3].middlemodifID = tpermutations[i].endmodifID;
		if (tpermutations[i+3].tcomposition.back() != ')') {
			tpermutations[i+3].tcomposition = tpermutations[i+3].tcomposition.substr(0,tpermutations[i+3].tcomposition.size() - 1);
		}
	}
	// 3 = n-terminal
	// 4 = middle modification type dependent
	// 5 = c-terminal

	/*
	for (int i = 0; i < 6; i++) {
		if (tpermutations[i].middlebranchstart == tpermutations[i].middlebranchend) {
			tpermutations[i].tcomposition[tpermutations[i].tcomposition.find('(')] = '-';
			tpermutations[i].tcomposition[tpermutations[i].tcomposition.find(')')] = '-';
			if (tpermutations[i].tcomposition[0] == '-') {
				tpermutations[i].tcomposition.erase(tpermutations[i].tcomposition.begin());
			}
			if (tpermutations[i].tcomposition.back() == '-') {
				tpermutations[i].tcomposition.erase(tpermutations[i].tcomposition.end() - 1);
			}
			tpermutations[i].middlebranchstart = -1;
			tpermutations[i].middlebranchend = -1;
		}
	}
	*/
}


double cCandidate::calculatePrecursorMass(cBricksDatabase& brickdatabasewithcombinations, cParameters* parameters) {
	cBrick b;
	vector<int> bricks;
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(bricks);

	if (parameters->peptidetype == linearpolyketide) {
		return 0;
	}
	
	double mass = 0;
	switch (parameters->peptidetype)
	{
	case linear:
		mass = parameters->fragmentdefinitions[precursor_ion].massdifference + parameters->searchedmodifications[startmodifID].massdifference + parameters->searchedmodifications[endmodifID].massdifference;
		break;
	case cyclic: 
		mass = parameters->fragmentdefinitions[cyclic_precursor_ion].massdifference;
		break;
	case branched:
		mass = parameters->fragmentdefinitions[precursor_ion].massdifference + parameters->searchedmodifications[startmodifID].massdifference + parameters->searchedmodifications[endmodifID].massdifference + parameters->searchedmodifications[middlemodifID].massdifference;
		break;
	case branchcyclic:
		mass = parameters->fragmentdefinitions[cyclic_precursor_ion].massdifference + parameters->searchedmodifications[middlemodifID].massdifference;
		break;
	case linearpolyketide:
		// not supported because of terminal ambiguities H- -OH, H- -H, OH- -OH
		break;
	case cyclicpolyketide:
		mass = parameters->fragmentdefinitions[cyclic_polyketide_precursor_ion].massdifference;
		break;
	case other:
		break;
	default:
		break;
	}
	
	for (int i = 0; i < (int)bricks.size(); i++) {
		mass += brickdatabasewithcombinations[bricks[i] - 1].getMass();
	}
	
	return (parameters->precursorcharge > 0)?mass:mass-2*Hplus;
}


bool cCandidate::isEqualTo(cCandidate& candidate) {
	if ((internalcomposition.compare(candidate.internalcomposition) == 0)
		&& (summary.getSummary().compare(candidate.summary.getSummary()) == 0) 
		&& (summary.isPartial() == candidate.summary.isPartial())
		&& (startmodifID == candidate.startmodifID)
		&& (endmodifID == candidate.endmodifID)
		&& (middlemodifID == candidate.middlemodifID)
		&& (branchstart == candidate.branchstart)
		&& (branchend == candidate.branchend)
		&& (startiontype == candidate.startiontype)) {
		return true;
	}
	return false;
}


void cCandidate::getPermutations(cCandidateSet& permutations, bool* terminatecomputation) {
	vector<string> currentcandidate;
	permutations.getSet().clear();
	currentcandidate.clear();
	getPermutationsIter(permutations, currentcandidate, 0, terminatecomputation);
}


int cCandidate::getStartModifID() {
	return startmodifID;
}


void cCandidate::setStartModifID(int id) {
	startmodifID = id;
}


int cCandidate::getEndModifID() {
	return endmodifID;
}


void cCandidate::setEndModifID(int id) {
	endmodifID = id;
}



int cCandidate::getMiddleModifID() {
	return middlemodifID;
}


void cCandidate::setMiddleModifID(int id) {
	middlemodifID = id;
}


bool cCandidate::compare(cCandidate& candidate) {
	if (getComposition() < candidate.getComposition()) {
		return true;
	}
	if (getComposition() > candidate.getComposition()) {
		return false;
	}

	if (startmodifID < candidate.startmodifID) {
		return true;
	}
	if (startmodifID > candidate.startmodifID) {
		return false;
	}

	if (endmodifID < candidate.endmodifID) {
		return true;
	}
	if (endmodifID > candidate.endmodifID) {
		return false;
	}

	if (middlemodifID < candidate.middlemodifID) {
		return true;
	}
	if (middlemodifID > candidate.middlemodifID) {
		return false;
	}

	if (branchstart < candidate.branchstart) {
		return true;
	}
	if (branchstart > candidate.branchstart) {
		return false;
	}

	if (branchend < candidate.branchend) {
		return true;
	}
	if (branchend > candidate.branchend) {
		return false;
	}

	return false;
}


vector<nodeEdge> cCandidate::getPath() {
	return path;
}


bool cCandidate::hasEqualTPermutations(cCandidate& candidate) {
	vector<TRotationInfo> tpermutations1, tpermutations2;
	getPermutationsOfBranches(tpermutations1);
	candidate.getPermutationsOfBranches(tpermutations2);

	for (int i = 0; i < (int)tpermutations1.size(); i++) {
		if ((tpermutations1[i].tcomposition.compare(tpermutations2[0].tcomposition) == 0) &&
			(tpermutations1[i].startmodifID == tpermutations2[0].startmodifID) &&
			(tpermutations1[i].endmodifID == tpermutations2[0].endmodifID) &&
			(tpermutations1[i].middlemodifID == tpermutations2[0].middlemodifID) &&
			(tpermutations1[i].middlebranchstart == tpermutations2[0].middlebranchstart) &&
			(tpermutations1[i].middlebranchend == tpermutations2[0].middlebranchend)) {
			return true;
		}
	}

	return false;
}


bool cCandidate::hasOnlyArtificialBricks(cBricksDatabase& brickdatabasewithcombinations) {
	if (numberofinternalbricks <= 2) {
		cBrick b;
		vector<int> intcomposition;
		b.setComposition(internalcomposition, false);
		b.explodeToIntComposition(intcomposition);
		for (int i = 0; i < (int)intcomposition.size(); i++) {
			if (!brickdatabasewithcombinations[intcomposition[i] - 1].isArtificial()) {
				return false;
			}
		}
		return true;
	}
	return false;
}


bool cCandidate::hasFirstBrickArtificial(cBricksDatabase& brickdatabasewithcombinations) {
	cBrick b;
	vector<int> intcomposition;
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(intcomposition);
	if ((intcomposition.size() > 0) && brickdatabasewithcombinations[intcomposition[0] - 1].isArtificial()) {
		return true;
	}
	return false;
}


bool cCandidate::hasLastBrickArtificial(cBricksDatabase& brickdatabasewithcombinations) {
	cBrick b;
	vector<int> intcomposition;
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(intcomposition);
	if ((intcomposition.size() > 0) && brickdatabasewithcombinations[intcomposition.back() - 1].isArtificial()) {
		return true;
	}
	return false;
}


bool cCandidate::hasLastBrickInvalid(cBricksDatabase& brickdatabasewithcombinations) {
	cBrick b;
	vector<int> intcomposition;
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(intcomposition);
	if ((intcomposition.size() > 0) && brickdatabasewithcombinations[intcomposition.back() - 1].isArtificial() && (brickdatabasewithcombinations[intcomposition.back() - 1].getMass() <= 0)) {
		return true;
	}
	return false;
}


void cCandidate::getRotations(vector<string>& rotations, bool includerevertedrotations) {
	rotations.clear();
	//rotations.push_back(composition);
	getPartialRotations(internalcomposition, rotations);
	
	if (includerevertedrotations) {
		cBrick brick;
		brick.setComposition(internalcomposition, false);
		//rotations.push_back(reversedcomposition);
		getPartialRotations(brick.getReverseComposition(), rotations);
	}
}


void cCandidate::getBranchCyclicRotations(vector<cCandidate>& branchcyclicrotations, bool includerevertedrotations) {
	branchcyclicrotations.clear();
	getPartialBranchCyclicRotations(internalcomposition, branchcyclicrotations, branchstart, branchend);
	if (includerevertedrotations) {
		getPartialBranchCyclicRotations(getRevertedTComposition(false), branchcyclicrotations, numberofinternalbricks - branchend - 1, numberofinternalbricks - branchstart - 1);
	}
}


cSummaryFormula cCandidate::calculateSummaryFormula(cParameters& parameters, ePeptideType peptidetype, double precursormass) {
	cBrick b;
	vector<int> bricks;
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(bricks);
	
	cSummaryFormula formula;
	string summary;

	if ((peptidetype == linearpolyketide) && (precursormass == 0)) {
		return formula;
	}

	switch (peptidetype)
	{
	case linear:
		summary = "H2O";
		formula.addFormula(summary);
		formula.addFormula(parameters.searchedmodifications[startmodifID].summary);
		formula.addFormula(parameters.searchedmodifications[endmodifID].summary);
		break;
	case cyclic: 
		break;
	case branched:
		summary = "H2O";
		formula.addFormula(summary);
		formula.addFormula(parameters.searchedmodifications[startmodifID].summary);
		formula.addFormula(parameters.searchedmodifications[endmodifID].summary);
		formula.addFormula(parameters.searchedmodifications[middlemodifID].summary);
		break;
	case branchcyclic:
		formula.addFormula(parameters.searchedmodifications[middlemodifID].summary);
		break;
	case linearpolyketide:
		formula.addFormula(parameters.searchedmodifications[startmodifID].summary);
		formula.addFormula(parameters.searchedmodifications[endmodifID].summary);
		break;
	case cyclicpolyketide:
		break;
	case other:
		break;
	default:
		break;
	}

	summary = parameters.precursoradduct.empty()?"":"H-1";
	formula.addFormula(summary);
	formula.addFormula(parameters.precursoradduct);

	if ((peptidetype == linear) || (peptidetype == linearpolyketide)) {
		if (parameters.cyclicnterminus) {
			summary = "H2O";
			formula.addFormula(summary, true);
		}
		if (parameters.cycliccterminus) {
			summary = "H2O";
			formula.addFormula(summary, true);
		}
	}

	bool partial = false;
	for (int i = 0; i < (int)bricks.size(); i++) {
		if (bricks[i] - 1 < (int)parameters.bricksdatabase.size()) {
			formula.addFormula(parameters.bricksdatabase[bricks[i] - 1].getSummary());
		}
		else {
			partial = true;
		}
	}

	formula.setPartial(partial);

	if (!partial && (peptidetype == linearpolyketide)) {
		summary = "H2O2";
		formula.addFormula(summary);
		
		cSummaryFormula tempformula;
		tempformula.setFormula(formula.getSummary());
		summary = "Hplus";
		tempformula.addFormula(summary);
		summary = (parameters.precursorcharge > 0)?"":"Hplus-2";
		tempformula.addFormula(summary);

		precursormass = charge(uncharge(precursormass, parameters.precursorcharge), (parameters.precursorcharge > 0)?1:-1);

		double tempmass = tempformula.getMass();
		double bestmz = fabs(precursormass - tempmass);
		int bestcase = 1;

		summary = "O-1";

		tempformula.addFormula(summary);
		tempmass = tempformula.getMass();
		if (fabs(precursormass - tempmass) < bestmz) {
			bestmz = fabs(precursormass - tempmass);
			bestcase = 2;
		}
	
		tempformula.addFormula(summary);
		tempmass = tempformula.getMass();
		if (fabs(precursormass - tempmass) < bestmz) {
			bestmz = fabs(precursormass - tempmass);
			bestcase = 3;
		}

		if (bestcase == 1) {
			return formula;
		}

		if (bestcase == 2) {
			summary = "O-1";
			formula.addFormula(summary);
			return formula;
		}

		if (bestcase == 3) {
			summary = "O-2";
			formula.addFormula(summary);
			return formula;
		}
	
	}

	return formula;
}


cSummaryFormula& cCandidate::getSummaryFormula() {
	return summary;
}



void cCandidate::setSummaryFormula(cSummaryFormula& summary) {
	this->summary = summary;
}


void cCandidate::store(ofstream& os) {
	int size;

	os.write((char *)&startmodifID, sizeof(int));
	os.write((char *)&endmodifID, sizeof(int));
	os.write((char *)&middlemodifID, sizeof(int));
	os.write((char *)&branchstart, sizeof(int));
	os.write((char *)&branchend, sizeof(int));

	size = (int)path.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)path.size(); i++) {
		path[i].store(os);
	}

	os.write((char *)&startiontype, sizeof(eFragmentIonType));
	storeString(name, os);
	summary.store(os);
	storeStringVector(composition, os);
	storeString(internalcomposition, os);

	os.write((char *)&numberofinternalbricks, sizeof(int));

	storeString(realpeptidename, os);
	storeString(acronympeptidename, os);
	storeStringVector(acronyms, os);
	storeStringVector(backboneacronyms, os);
	storeStringVector(branchacronyms, os);
	storeString(stringpath, os);
}


void cCandidate::load(ifstream& is) {
	int size;

	is.read((char *)&startmodifID, sizeof(int));
	is.read((char *)&endmodifID, sizeof(int));
	is.read((char *)&middlemodifID, sizeof(int));
	is.read((char *)&branchstart, sizeof(int));
	is.read((char *)&branchend, sizeof(int));

	is.read((char *)&size, sizeof(int));
	path.resize(size);
	for (int i = 0; i < (int)path.size(); i++) {
		path[i].load(is);
	}

	is.read((char *)&startiontype, sizeof(eFragmentIonType));
	loadString(name, is);
	summary.load(is);
	loadStringVector(composition, is);
	loadString(internalcomposition, is);

	is.read((char *)&numberofinternalbricks, sizeof(int));

	loadString(realpeptidename, is);
	loadString(acronympeptidename, is);
	loadStringVector(acronyms, is);
	loadStringVector(backboneacronyms, is);
	loadStringVector(branchacronyms, is);
	loadString(stringpath, is);
}


void cCandidate::setName(string& name) {
	this->name = name;
}


string& cCandidate::getName() {
	return name;
}


void cCandidate::setAcronyms(cBricksDatabase& bricksdatabase) {
	vector<int> bricks;
	cBrick b;
	b.clear();
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(bricks);

	acronyms.clear();
	for (int i = 0; i < (int)bricks.size(); i++) {
		acronyms.push_back(bricksdatabase[bricks[i] - 1].getAcronymsAsString());
	}
}


void cCandidate::setBackboneAcronyms(cBricksDatabase& bricksdatabase) {
	vector<int> bricks;
	cBrick b;
	b.clear();
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(bricks);

	backboneacronyms.clear();
	for (int i = 0; i < (int)bricks.size(); i++) {
		if ((branchstart >= 0) && (branchend >= 0) && ((i <= branchstart) || (i > branchend))) {
			backboneacronyms.push_back(bricksdatabase[bricks[i] - 1].getAcronymsAsString());
		}
	}
}


void cCandidate::setBranchAcronyms(cBricksDatabase& bricksdatabase) {
	vector<int> bricks;
	cBrick b;
	b.clear();
	b.setComposition(internalcomposition, false);
	b.explodeToIntComposition(bricks);

	branchacronyms.clear();
	for (int i = 0; i < (int)bricks.size(); i++) {
		if ((branchstart >= 0) && (branchend >= 0) && (i > branchstart) && (i <= branchend)) {
			branchacronyms.push_back(bricksdatabase[bricks[i] - 1].getAcronymsAsString());
		}
	}
}


vector<string>& cCandidate::getAcronyms() {
	return acronyms;
}


vector<string>& cCandidate::getBackboneAcronyms() {
	return backboneacronyms;
}


vector<string>& cCandidate::getBranchAcronyms() {
	return branchacronyms;
}


void cCandidate::setPath(cDeNovoGraph& graph, cParameters* parameters) {
	cDeNovoGraphNode* currentnode;
	cDeNovoGraphNode* targetnode;
	cEdge* currentedge;
	stringpath = "";
	for (int i = 0; i < (int)path.size(); i++) {
		currentnode = &graph[path[i].nodeid];
		currentedge = &((*currentnode)[path[i].edgeid]);
		targetnode = &graph[currentedge->targetnode];

		stringpath += to_string(currentnode->getMZRatio());
		stringpath += " -> ";
		stringpath += to_string(targetnode->getMZRatio());
		stringpath += " using brick(s): ";
		if (currentedge->composition.compare("0") == 0) {
			stringpath += "none";
		}
		else {
			stringpath += graph.getBrickDatabaseWithCombinations()->getAcronymName(currentedge->composition, true);
		}

		stringpath += " (mass difference: " + to_string(currentedge->massdifference) + ", ";
		stringpath += "source intensity: " + to_string(currentnode->getIntensity()) + ", ";
		stringpath += "target intensity: " + to_string(targetnode->getIntensity()) + ", ";
		stringpath += "ppm error: " + to_string(currentedge->ppmerror) + ", ";
		stringpath += "source charge: " + to_string(currentedge->sourcecharge) + ", ";
		stringpath += "target charge: " + to_string(currentedge->targetcharge);
		if ((parameters->peptidetype == branched) || (parameters->peptidetype == branchcyclic)) {
			if (currentedge->middlemodifID > 0) {
				stringpath += ", branch modification: " + parameters->searchedmodifications[currentedge->middlemodifID].name;
			}
		}
		if (currentedge->endmodifID > 0) {
			stringpath += ", terminal modification: " + parameters->searchedmodifications[currentedge->endmodifID].name;
		}
		stringpath += ", summary: " + currentedge->summary;
		//path += currentedge->printSourceAnnotation(fragmentdefinitions);
		//path += "->";
		//path += currentedge->printTargetAnnotation(fragmentdefinitions);
		stringpath += ")<br/>";
	}	
}


string& cCandidate::getPathAsString() {
	return stringpath;
}


void cCandidate::setRealPeptideName(cBricksDatabase& bricksdatabase, ePeptideType peptidetype) {
	switch (peptidetype)
	{
	case linear:
	case cyclic:
	case linearpolyketide:
	case cyclicpolyketide:
		realpeptidename = bricksdatabase.getRealName(internalcomposition);
		break;
	case branched:
	case branchcyclic:
		realpeptidename = getRealNameTComposition(bricksdatabase);
		break;
	case other:
	default:
		realpeptidename = "";
		break;
	}
}


void cCandidate::setAcronymPeptideNameWithHTMLReferences(cBricksDatabase& bricksdatabase, ePeptideType peptidetype) {
	switch (peptidetype)
	{
	case linear:
	case cyclic:
	case linearpolyketide:
	case cyclicpolyketide:
		acronympeptidename = bricksdatabase.getAcronymName(internalcomposition, true);
		break;
	case branched:
	case branchcyclic:
		acronympeptidename = getAcronymsTComposition(bricksdatabase);
		break;
	case other:
	default:
		acronympeptidename = "";
		break;
	}
}


string& cCandidate::getRealPeptideName() {
	return realpeptidename;
}


string& cCandidate::getAcronymPeptideNameWithHTMLReferences() {
	return acronympeptidename;
}


void cCandidate::setStartIonType(eFragmentIonType iontype) {
	startiontype = iontype;
}


eFragmentIonType cCandidate::getStartIonType() {
	return startiontype;
}


bool cCandidate::checkKetideSequence(cBricksDatabase& bricksdatabase, ePeptideType peptidetype, bool regularblocksorder) {
	
	cBrick b;
	b.setComposition(internalcomposition, false);
	vector<int> intcomposition;
	b.explodeToIntComposition(intcomposition);

	if (intcomposition.size() == 0) {
		return false;
	}

	/*
	// all blocks are water loss
	int h2o_blocks = 0;
	for (int i = 0; i < (int)intcomposition.size(); i++) {
		if (bricksdatabase[intcomposition[i] - 1].getResidueLossType() == h2o_loss) {
			h2o_blocks++;
		}
	}
	if (h2o_blocks == (int)intcomposition.size()) {
		return true;
	}
	*/

	bool hasfirstblockartificial = hasFirstBrickArtificial(bricksdatabase);
	bool haslastblockartificial = hasLastBrickArtificial(bricksdatabase);

	// linear polyketide has at least 2 building blocks
	if ((peptidetype == linearpolyketide) && !hasfirstblockartificial && !haslastblockartificial && ((int)intcomposition.size() < 2)) {
		return false;
	}

	if (!regularblocksorder) {
		return true;
	}

	// cyclic polyketide has always an even number of blocks
	if ((peptidetype == cyclicpolyketide) && !hasfirstblockartificial && !haslastblockartificial && ((int)intcomposition.size() % 2 == 1)) {
		return false;
	}

	// even blocks are water loss and odd blocks are h2 loss, or vice versa
	for (int i = (hasfirstblockartificial?2:1); i < (haslastblockartificial?(int)intcomposition.size()-1:(int)intcomposition.size()); i++) {
		if (!(((bricksdatabase[intcomposition[i] - 1].getResidueLossType() == h2_loss) && (bricksdatabase[intcomposition[i - 1] - 1].getResidueLossType() == h2o_loss))
			|| ((bricksdatabase[intcomposition[i] - 1].getResidueLossType() == h2o_loss) && (bricksdatabase[intcomposition[i - 1] - 1].getResidueLossType() == h2_loss)))) {
				return false;
		}
	}

	return true;
}


eResidueLossType cCandidate::getLeftResidueType(cBricksDatabase& bricksdatabase) {
	cBrick b;
	b.setComposition(internalcomposition, false);
	vector<int> intcomposition;
	b.explodeToIntComposition(intcomposition);

	if (intcomposition.size() == 0) {
		return h2o_loss;
	}

	return bricksdatabase[intcomposition[0] - 1].getResidueLossType();
}


eResidueLossType cCandidate::getRightResidueType(cBricksDatabase& bricksdatabase) {
	cBrick b;
	b.setComposition(internalcomposition, false);
	vector<int> intcomposition;
	b.explodeToIntComposition(intcomposition);

	if (intcomposition.size() == 0) {
		return h2o_loss;
	}

	return bricksdatabase[intcomposition.back() - 1].getResidueLossType();
}


bool cCandidate::checkKetideBlocks(cBricksDatabase& bricksdatabase, ePeptideType peptidetype, bool regularblocksorder) {
	cBrick b;
	b.setComposition(internalcomposition, false);
	vector<int> intcomposition;
	b.explodeToIntComposition(intcomposition);

	if (intcomposition.size() == 0) {
		return false;
	}

	if (!regularblocksorder) {
		return true;
	}

	bool hasfirstblockartificial = hasFirstBrickArtificial(bricksdatabase);
	bool haslastblockartificial = hasLastBrickArtificial(bricksdatabase);

	// cyclic polyketide has always an even number of blocks
	if ((peptidetype == cyclicpolyketide) && !hasfirstblockartificial && !haslastblockartificial && ((int)intcomposition.size() % 2 == 1)) {
		return false;
	}

	int h2_blocks = 0;
	int h2o_blocks = 0;
	for (int i = (hasfirstblockartificial?1:0); i < (haslastblockartificial?(int)intcomposition.size()-1:(int)intcomposition.size()); i++) {
		if (bricksdatabase[intcomposition[i] - 1].getResidueLossType() == h2_loss) {
			h2_blocks++;
		}
		if (bricksdatabase[intcomposition[i] - 1].getResidueLossType() == h2o_loss) {
			h2o_blocks++;
		}
	}

	if ((h2_blocks == h2o_blocks) || (h2_blocks == h2o_blocks + 1) || (h2_blocks + 1 == h2o_blocks)) {
		return true;
	}

	return false;
}


bool operator == (cCandidate const& a, cCandidate const& b) {
	return ((cCandidate &)a).isEqualTo((cCandidate &)b); 
}

