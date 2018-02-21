#include "core/cBrick.h"


int getNumberOfBricks(const string& composition) {
	if (composition.size() == 0) {
		return 0;
	}

	int count = 0;
	for (int i = 0; i < (int)composition.size(); i++) {
		if (composition[i] == '-') {
			count++;
		}
	}
	return ++count;
}


cBrick::cBrick() {
	clear();
}


void cBrick::clear() {
	name = "";
	acronyms.clear();
	references.clear();
	summary = "";
	mass = 0;
	composition = "";
	artificial = false;
	residuelosstype = h2o_loss;
}


bool cBrick::empty() {
	if ((name.size() == 0) && (summary.size() == 0) && (mass == 0)) return true;
	return false;
}


string& cBrick::getName() {
	return name;
}


void cBrick::setName(const string& name) {
	this->name = name;
}


string& cBrick::getSummary() {
	return summary;
}


vector<string>& cBrick::getAcronyms() {
	return acronyms;
}


vector<string>& cBrick::getReferences() {
	return references;
}


string& cBrick::getComposition() {
	return composition;
}


string cBrick::getReverseComposition() {
	string reversename = "";
	vector<string> blocks;
	explodeToStringComposition(blocks);
	for (int i = 0; i < (int)blocks.size(); i++) {
		reversename += blocks[(int)blocks.size() - i - 1];
		if (i < (int)blocks.size() - 1) {
			reversename += '-';
		}
	}
	return reversename;
}


double cBrick::getMass() const {
	return mass;
}


void cBrick::setMass(double mass) {
	this->mass = mass;
}


void cBrick::setComposition(const string& composition, bool normalize) {
	this->composition = composition;
	if (normalize) {
		normalizeComposition();
	}
}


void cBrick::setSummary(const string& summary) {
	this->summary = summary;
}


void cBrick::setAcronyms(const string& acronyms) {
	this->acronyms.clear();
	string s = "";
	int i = 0;
	while (i < (int)acronyms.size()) {
		if (acronyms[i] == '/') {
			if (s.compare("") != 0) {
				this->acronyms.push_back(s);
			}
			s = "";
		}
		else {
			s += acronyms[i];
		}
		i++;
	}
	if (s.compare("") != 0) {
		this->acronyms.push_back(s);
	}
}


void cBrick::setReferences(const string& references) {
	this->references.clear();
	string s = "";
	int i = 0;
	while (i < (int)references.size()) {
		if (references[i] == '/') {
			if (s.compare("") != 0) {
				this->references.push_back(s);
			}
			s = "";
		}
		else {
			s += references[i];
		}
		i++;
	}
	if (s.compare("") != 0) {
		this->references.push_back(s);
	}
}


void cBrick::normalizeComposition() {
	vector<int> bricks;
	explodeToIntComposition(bricks);
	sort(bricks.begin(), bricks.end());
	
	composition = "";
	for (int i = 0; i < (int)bricks.size(); i++) {
		composition += to_string((long long)bricks[i]);
		if (i < (int)bricks.size() - 1) {
			composition += "-";
		}
	}
}


void cBrick::explodeToIntComposition(vector<int>& bricks) {
	if (composition.compare("") == 0) {
		return;
	}

	bricks.clear();
	int item;
	int first = 0;
	int last = (int)composition.find('-');
	while (last != (int)string::npos) {
		item = atoi(composition.substr(first, last - first).c_str());
		bricks.push_back(item);
		first = last+1;
		last = (int)composition.find('-',first);
	}
	item = atoi(composition.substr(first).c_str());
	bricks.push_back(item);
}


void cBrick::explodeToStringComposition(vector<string>& bricks) {
	if (composition.compare("") == 0) {
		return;
	}

	bricks.clear();
	string item;
	int first = 0;
	int last = (int)composition.find('-');
	while (last != (int)string::npos) {
		item = composition.substr(first, last - first);
		bricks.push_back(item);
		first = last+1;
		last = (int)composition.find('-',first);
	}
	item = composition.substr(first);
	bricks.push_back(item);
}


string cBrick::getAcronymsAsString() {
	string s = "";
	for (int i = 0; i < (int)acronyms.size(); i++) {
		s += acronyms[i];
		if (i < (int)acronyms.size() - 1) {
			s += "/";
		}
	}
	return s;
}


string cBrick::getReferencesAsString() {
	string s = "";
	for (int i = 0; i < (int)references.size(); i++) {
		s += references[i];
		if (i < (int)references.size() - 1) {
			s += "/";
		}
	}
	return s;
}


string cBrick::getFirstAcronymAsString() {
	if (acronyms.size() == 0) {
		return "";
	}
	return acronyms[0];
}


string cBrick::getAcronymsWithReferencesAsHTMLString() {
	if (acronyms.size() != references.size()) {
		return getAcronymsAsString();
	}

	string s = "";
	regex rx;
	bool correctreference;
	
	for (int i = 0; i < (int)acronyms.size(); i++) {

		correctreference = false;

		try {

			// ChemSpider
			if (!correctreference) {
				rx = "^CSID: [0-9]+$";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://www.chemspider.com/Chemical-Structure." + references[i].substr(6) + ".html\">";
					s += acronyms[i];
					s += "</a>";
					correctreference = true;
				}
			}

			// PubChem
			if (!correctreference) {
				rx = "^CID: [0-9]+$";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://pubchem.ncbi.nlm.nih.gov/summary/summary.cgi?cid=" + references[i].substr(5) + "\">";
					s += acronyms[i];
					s += "</a>";
					correctreference = true;
				}
			}

			// PDB
			if (!correctreference) {
				rx = "^PDB: ([A-Z]|[0-9])+$";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://www.ebi.ac.uk/pdbe-srv/pdbechem/chemicalCompound/show/" + references[i].substr(5) + "\">";
					s += acronyms[i];
					s += "</a>";
					correctreference = true;
				}
			}

			// smiles in ChemSpider
			if (!correctreference) {
				rx = " in CSID: [0-9]+$";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://www.ebi.ac.uk/pdbe-site/pdbemotif/smilesstats.jsp?smiles=" + references[i].substr(0, references[i].find(':') - 8) + "\">";
					s += acronyms[i];
					s += "</a>";
					correctreference = true;
				}
			}

			// smiles in PubChem
			if (!correctreference) {
				rx = " in CID: [0-9]+$";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://www.ebi.ac.uk/pdbe-site/pdbemotif/smilesstats.jsp?smiles=" + references[i].substr(0, references[i].find(':') - 7) + "\">";
					s += acronyms[i];
					s += "</a>";
					correctreference = true;
				}
			}

			// smiles in Norine
			if (!correctreference) {
				rx = " in: NOR";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://www.ebi.ac.uk/pdbe-site/pdbemotif/smilesstats.jsp?smiles=" + references[i].substr(0, references[i].find(':') - 3) + "\">";
					s += acronyms[i];
					s += "</a>";
					correctreference = true;
				}
			}

		}
		catch (regex_error& /*e*/) {
			// nothing to do
		}

		if (!correctreference) {
			s += acronyms[i];
		}

		if (i < (int)acronyms.size() - 1) {
			s += "/";
		}
	}
	return s;
}


void cBrick::setArtificial(bool artificial) {
	this->artificial = artificial;
}


bool cBrick::isArtificial() {
	return artificial;
}


void cBrick::setResidueLossType(eResidueLossType residuelosstype) {
	this->residuelosstype = residuelosstype;
}


eResidueLossType cBrick::getResidueLossType() {
	return residuelosstype;
}


void cBrick::store(ofstream& os) {
	storeString(name, os);
	storeStringVector(acronyms, os);
	storeStringVector(references, os);
	storeString(summary, os);
	os.write((char *)&mass, sizeof(double));
	storeString(composition, os);
	os.write((char *)&artificial, sizeof(bool));
	os.write((char *)&residuelosstype, sizeof(eResidueLossType));
}


void cBrick::load(ifstream& is) {
	loadString(name, is);
	loadStringVector(acronyms, is);
	loadStringVector(references, is);
	loadString(summary, is);
	is.read((char *)&mass, sizeof(double));
	loadString(composition, is);
	is.read((char *)&artificial, sizeof(bool));
	is.read((char *)&residuelosstype, sizeof(eResidueLossType));
}

