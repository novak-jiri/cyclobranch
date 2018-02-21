#include "core/cBrick.h"


int getNumberOfBricks(string& composition) {
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


string stripHTML(string& htmlstring) {
	string s;
	bool add = true;

	for (int i = 0; i < (int)htmlstring.size(); i++) {
		if (htmlstring[i] == '<') {
			add = false;
			continue;
		}
		if (htmlstring[i] == '>') {
			add = true;
			continue;
		}
		if (add) {
			s += htmlstring[i];
		}
	}
	return s;
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
}


bool cBrick::empty() {
	if ((name.size() == 0) && (summary.size() == 0) && (mass == 0)) return true;
	return false;
}


string& cBrick::getName() {
	return name;
}


void cBrick::setName(string& name) {
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


double cBrick::getMass() {
	return mass;
}


void cBrick::setMass(double mass) {
	this->mass = mass;
}


void cBrick::setComposition(string& composition, bool normalize) {
	this->composition = composition;
	if (normalize) {
		normalizeComposition();
	}
}


void cBrick::setSummary(string& summary) {
	this->summary = summary;
}


void cBrick::setAcronyms(string& acronyms) {
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


void cBrick::setReferences(string& references) {
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
	while (last != string::npos) {
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
	while (last != string::npos) {
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


string cBrick::getFirstAcronymAsString() {
	if (acronyms.size() == 0) {
		return "";
	}
	return acronyms[0];
}


string cBrick::getAcronymsWithReferencesAsHTMLString() {
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
				rx = "^PDB: [A-Z]+$";
				if (regex_search(references[i], rx)) {
					s += "<a href=\"http://www.pdb.org/pdb/ligand/ligandsummary.do?hetId=" + references[i].substr(5) + "\">";
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
		catch (std::regex_error& e) {
			e;
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


void cBrick::store(ofstream& os) {
	int size;

	size = (int)name.size();
	os.write((char *)&size, sizeof(int));
	os.write(name.c_str(), name.size());

	size = (int)acronyms.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)acronyms.size(); i++) {
		size = (int)acronyms[i].size();
		os.write((char *)&size, sizeof(int));
		os.write(acronyms[i].c_str(), acronyms[i].size());
	}

	size = (int)references.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)references.size(); i++) {
		size = (int)references[i].size();
		os.write((char *)&size, sizeof(int));
		os.write(references[i].c_str(), references[i].size());
	}

	size = (int)summary.size();
	os.write((char *)&size, sizeof(int));
	os.write(summary.c_str(), summary.size());

	os.write((char *)&mass, sizeof(double));

	size = (int)composition.size();
	os.write((char *)&size, sizeof(int));
	os.write(composition.c_str(), composition.size());

	os.write((char *)&artificial, sizeof(bool));
}


void cBrick::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	name.resize(size);
	is.read(&name[0], name.size());

	is.read((char *)&size, sizeof(int));
	acronyms.resize(size);
	for (int i = 0; i < (int)acronyms.size(); i++) {
		is.read((char *)&size, sizeof(int));
		acronyms[i].resize(size);
		is.read(&acronyms[i][0], acronyms[i].size());
	}

	is.read((char *)&size, sizeof(int));
	references.resize(size);
	for (int i = 0; i < (int)references.size(); i++) {
		is.read((char *)&size, sizeof(int));
		references[i].resize(size);
		is.read(&references[i][0], references[i].size());
	}

	is.read((char *)&size, sizeof(int));
	summary.resize(size);
	is.read(&summary[0], summary.size());

	is.read((char *)&mass, sizeof(double));

	is.read((char *)&size, sizeof(int));
	composition.resize(size);
	is.read(&composition[0], composition.size());

	is.read((char *)&artificial, sizeof(bool));
}

