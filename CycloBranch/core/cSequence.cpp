#include "core/cSequence.h"


cSequence::cSequence() {
	clear();
}


void cSequence::clear() {
	peptidetype = linear;
	sequence = "";
	nterminalmodification = "";
	cterminalmodification = "";
	branchmodification = "";
	name = "";
	summary.clear();
	reference = "";
}


void cSequence::store(ofstream& os) {
	os.write((char *)&peptidetype, sizeof(peptideType));
	storeString(sequence, os);
	storeString(nterminalmodification, os);
	storeString(cterminalmodification, os);
	storeString(branchmodification, os);
	storeString(name, os);
	summary.store(os);
	storeString(reference, os);
}


void cSequence::load(ifstream& is) {
	is.read((char *)&peptidetype, sizeof(peptideType));
	loadString(sequence, is);
	loadString(nterminalmodification, is);
	loadString(cterminalmodification, is);
	loadString(branchmodification, is);
	loadString(name, is);
	summary.load(is);
	loadString(reference, is);
}


peptideType cSequence::getPeptideType() {
	return peptidetype;
}


string& cSequence::getSequence() {
	return sequence;
}


string& cSequence::getName() {
	return name;
}


string& cSequence::getReference() {
	return reference;
}


void cSequence::setPeptideType(peptideType peptidetype) {
	this->peptidetype = peptidetype;
}


void cSequence::setSequence(string& sequence) {
	this->sequence = sequence;
}


void cSequence::setName(string& name) {
	this->name = name;
}


void cSequence::setReference(string& reference) {
	this->reference = reference;
}


void cSequence::setNTterminalModification(string& modification) {
	nterminalmodification = modification;
}


void cSequence::setCTterminalModification(string& modification) {
	cterminalmodification = modification;
}


void cSequence::setBranchModification(string& modification) {
	branchmodification = modification;
}



string& cSequence::getNTterminalModification() {
	return nterminalmodification;
}


string& cSequence::getCTterminalModification() {
	return cterminalmodification;
}


string& cSequence::getBranchModification() {
	return branchmodification;
}


string cSequence::getNameWithReferenceAsHTMLString() {
	regex rx;	
	string s = "";
	bool correctreference = false;

	try {

		// ChemSpider
		if (!correctreference) {
			rx = "^CSID: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"http://www.chemspider.com/Chemical-Structure." + reference.substr(6) + ".html\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// PubChem
		if (!correctreference) {
			rx = "^CID: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"http://pubchem.ncbi.nlm.nih.gov/summary/summary.cgi?cid=" + reference.substr(5) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// Norine
		if (!correctreference) {
			rx = "^NOR[0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"http://bioinfo.lifl.fr/norine/result.jsp?ID=" + reference + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// Lipidmaps (undocumented)
		if (!correctreference) {
			rx = "^LM([A-Z]|[0-9])+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"http://www.lipidmaps.org/data/LMSDRecord.php?LMID=" + reference + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

	}
	catch (std::regex_error& e) {
		e;
		// nothing to do
	}

	if (!correctreference) {
		s = name;
	}

	return s;
}


void cSequence::setSummaryFormula(string& formula) {
	summary.setFormula(formula);
}


string& cSequence::getSummaryFormula() {
	return summary.getFormula();
}

