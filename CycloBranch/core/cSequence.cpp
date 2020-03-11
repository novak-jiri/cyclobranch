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
	decoy = false;
}


void cSequence::store(ofstream& os) {
	os.write((char *)&peptidetype, sizeof(ePeptideType));
	storeString(sequence, os);
	storeString(nterminalmodification, os);
	storeString(cterminalmodification, os);
	storeString(branchmodification, os);
	storeString(name, os);
	summary.store(os);
	storeString(reference, os);
	os.write((char *)&decoy, sizeof(bool));
}


void cSequence::load(ifstream& is) {
	is.read((char *)&peptidetype, sizeof(ePeptideType));
	loadString(sequence, is);
	loadString(nterminalmodification, is);
	loadString(cterminalmodification, is);
	loadString(branchmodification, is);
	loadString(name, is);
	summary.load(is);
	loadString(reference, is);
	is.read((char *)&decoy, sizeof(bool));
}


ePeptideType cSequence::getPeptideType() {
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


void cSequence::setPeptideType(ePeptideType peptidetype) {
	this->peptidetype = peptidetype;
}


void cSequence::setSequence(const string& sequence) {
	this->sequence = sequence;
}


void cSequence::setName(const string& name) {
	this->name = name;
}


void cSequence::setReference(const string& reference) {
	this->reference = reference;
}


void cSequence::setNTterminalModification(const string& modification) {
	nterminalmodification = modification;
}


void cSequence::setCTterminalModification(const string& modification) {
	cterminalmodification = modification;
}


void cSequence::setBranchModification(const string& modification) {
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
	// performance improvement - quick return without regex_search
	string s = "";
	if (reference.empty()) {
		s += "<a href=\"https://pubchem.ncbi.nlm.nih.gov/#query=" + name + "\">";
		s += name;
		s += "</a>";
		return s;
	}

	regex rx;	
	bool correctreference = false;

	try {

		// ChemSpider
		if (!correctreference) {
			rx = "^CSID: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://www.chemspider.com/Chemical-Structure." + reference.substr(6) + ".html\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// PubChem
		if (!correctreference) {
			rx = "^CID: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://pubchem.ncbi.nlm.nih.gov/summary/summary.cgi?cid=" + reference.substr(5) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// ChEBI
		if (!correctreference) {
			rx = "^CHEBI: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://www.ebi.ac.uk/chebi/searchId.do?chebiId=CHEBI:" + reference.substr(7) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// Norine
		if (!correctreference) {
			rx = "^NOR[0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://bioinfo.lifl.fr/norine/result.jsp?ID=" + reference + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// Lipidmaps
		if (!correctreference) {
			rx = "^LM([A-Z]|[0-9])+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://www.lipidmaps.org/data/LMSDRecord.php?LMID=" + reference + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// KEGG Compounds (undocumented)
		if (!correctreference) {
			rx = "^C[0-9]{5}$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://www.genome.jp/dbget-bin/www_bget?cpd:" + reference + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// CAYMAN (undocumented)
		if (!correctreference) {
			rx = "^CAYMAN: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://www.caymanchem.com/product/" + reference.substr(8) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// PAMDB (undocumented)
		if (!correctreference) {
			rx = "^PAMDB[0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"http://pseudomonas.umaryland.edu/PAMDB?MetID=" + reference + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// Siderophore Base (undocumented)
		if (!correctreference) {
			rx = "^SB: [0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"http://bertrandsamuel.free.fr/siderophore_base/siderophore.php?id=" + reference.substr(4) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// MIBIG (undocumented)
		if (!correctreference) {
			rx = "^MIBIG: BGC[0-9]+$";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://mibig.secondarymetabolites.org/repository/" + reference.substr(7) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

		// DOI
		if (!correctreference) {
			rx = "^DOI: ";
			if (regex_search(reference, rx)) {
				s += "<a href=\"https://doi.org/" + reference.substr(5) + "\">";
				s += name;
				s += "</a>";
				correctreference = true;
			}
		}

	}
	catch (regex_error& /*e*/) {
		// nothing to do
	}

	if (!correctreference) {
		s += "<a href=\"https://pubchem.ncbi.nlm.nih.gov/#query=" + name + "\">";
		s += name;
		s += "</a>";
	}

	return s;
}


void cSequence::setSummaryFormula(const string& formula) {
	summary.setFormula(formula);
}


string cSequence::getSummaryFormula() {
	return summary.getSummary();
}


bool cSequence::isDecoy() {
	return decoy;
}


void cSequence::setDecoy(bool decoy) {
	this->decoy = decoy;
}

