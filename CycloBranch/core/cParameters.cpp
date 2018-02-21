#include "core/cParameters.h"

#include "gui/cMainThread.h"


cParameters::cParameters() {
	clear();
}


void cParameters::clear() {
	os = 0;
	fragmentdefinitions.recalculateFragments(false, false);
	peptidetype = linear;
	peaklistfilename = "";
	peaklist.clear();
	precursormass = 0;
	precursormasserrortolerance = 5;
	precursorcharge = 1;
	fragmentmasserrortolerance = 5;
	masserrortolerancefordeisotoping = 5;
	minimumrelativeintensitythreshold = 0;
	minimummz = 150;
	bricksdatabasefilename = "";
	bricksdatabase.clear();
	maximumbricksincombinationbegin = 3;
	maximumbricksincombinationmiddle = 2;
	maximumbricksincombinationend = 3;
	maximumbricksincombination = max(max(maximumbricksincombinationbegin, maximumbricksincombinationmiddle), maximumbricksincombinationend);
	maximumcumulativemass = 0;
	generatebrickspermutations = true;
	maximumnumberofthreads = 1;
	mode = denovoengine;
	scoretype = b_ions;
	clearhitswithoutparent = false;
	cyclicnterminus = false;
	cycliccterminus = false;
	enablescrambling = false;
	hitsreported = 1000;
	sequencetag = "";
	originalsequencetag = "";
	searchedsequence = "";
	originalsearchedsequence = "";
	searchedsequenceNtermmodif = "";
	searchedsequenceCtermmodif = "";
	searchedsequenceTmodif = "";
	maximumnumberofcandidates = 50000;
	blindedges = 2;
	sequencedatabase.clear();
	sequencedatabasefilename = "";

	searchedmodifications.clear();
	searchedmodifications.push_back(fragmentDescription("", 0, "", true, true));

	fragmentionsfordenovograph.clear();
	fragmentionsfortheoreticalspectra.clear();
}


int cParameters::checkAndPrepare() {

	bool error = false;
	string errormessage = "";
	ifstream peakliststream;
	ifstream bricksdatabasestream;
	ifstream sequencedatabasestream;
	regex rx;
	ePeakListFileFormat peaklistfileformat;
	string s;


	// peaklist check
	if (!error) {

		peaklistfileformat = txt;
			
		try {

			rx = "\\.[mM][gG][fF]$";
			// Mascot Generic Format
			if (regex_search(peaklistfilename, rx)) {
				peaklistfileformat = mgf;
			}
			
			rx = "\\.[mM][zZ][mM][lL]$";
			// mzML
			if (regex_search(peaklistfilename, rx)) {
				peaklistfileformat = mzML;
			}
			
			rx = "\\.[mM][zZ][xX][mM][lL]$";
			// mzXML
			if (regex_search(peaklistfilename, rx)) {
				peaklistfileformat = mzXML;
			}
			
			rx = "\\.[bB][aA][fF]$";
			// Bruker Analysis File
			if (regex_search(peaklistfilename, rx)) {
				peaklistfileformat = baf;
			}

		}
		catch (std::regex_error& e) {
			error = true;
			errormessage = "cParameters::checkAndPrepare: regex_search failed, error no. " + to_string((int)e.code()) + "\n";
		}

		if (!error) {

			switch (peaklistfileformat) 
			{
			case txt:
				peakliststream.open(peaklistfilename);
				break;
			case mgf:
				peakliststream.open(peaklistfilename);
				break;
			case mzML:
			case mzXML:
				*os << "Converting the file " + peaklistfilename + " to .mgf ... ";
				s = "External\\any2mgf.bat \"" + peaklistfilename + "\"";
				if (system(s.c_str()) != 0) {
					error = true;
					errormessage = "The file cannot be converted.\n";
					errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
					errormessage += "Do you have OpenMS installed ?\n";
					errormessage += "Do you have path to the OpenMS/bin/FileConverter.exe in your PATH variable ?\n";
					errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
					errormessage += "Do you have 'any2mgf.bat' file located in the 'External' folder ?\n";
				}

				if (!error) {
					*os << "ok" << endl << endl;
					peakliststream.open(peaklistfilename + ".mgf");
				}
				break;
			case baf:
				*os << "Converting the file " + peaklistfilename + " ... ";
				s = "External\\baf2csv.bat \"" + peaklistfilename + "\"";
				if (system(s.c_str()) != 0) {
					error = true;
					errormessage = "The file cannot be converted.\n";
					errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
					errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
					errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
					errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
					errormessage += "Do you have 'baf2csv.bat' file located in the 'External' folder ?\n";
				}

				if (!error) {
					*os << "ok" << endl << endl;
					peakliststream.open(peaklistfilename + ".csv");
				}
				break;
			default:
				break;
			}

		}
			

		if (!error) {
			if (!peakliststream.good()) {
				error = true;
				errormessage = "Cannot open the file '" + peaklistfilename + "'.";
			}
			else {
				if (os) {
					*os << "Loading the peak list... ";
				}
				switch (peaklistfileformat) {
				case txt:
					peaklist.loadFromPlainTextStream(peakliststream);
					break;
				case mzML:
				case mzXML:
				case mgf:
					peaklist.loadFromMGFStream(peakliststream);
					break;
				case baf:
					peaklist.loadFromBAFStream(peakliststream);
					break;
				default:
					break;
				}
				if (os) {
					*os << "ok" << endl << endl;
				}
			}
			peakliststream.close();
		}

	}


	// bricksdatabase check
	if (!error && ((mode == denovoengine) || (mode == singlecomparison) || (mode == databasesearch))) {

		bricksdatabasestream.open(bricksdatabasefilename);
		if (!bricksdatabasestream.good()) {
			error = true;
			errormessage = "Cannot open the file '" + bricksdatabasefilename + "'.";
		}
		else {
			if (os) {
				*os << "Loading the database of bricks... ";
			}
			if (bricksdatabase.loadFromPlainTextStream(bricksdatabasestream, errormessage) == -1) {
				error = true;
			}
			else {
				if (os) {
					*os << "ok" << endl << endl;
				}
			}
		}
		bricksdatabasestream.close();

		bricksdatabase.sortbyMass();

		// check for redundant acronyms and []
		for (int i = 0; i < (int)bricksdatabase.size(); i++) {
			for (int j = 0; j < (int)bricksdatabase[i].getAcronyms().size(); j++) {

				if ((bricksdatabase[i].getAcronyms()[j].find('[') != string::npos) || (bricksdatabase[i].getAcronyms()[j].find(']') != string::npos)) {
					error = true;
					errormessage = "Forbidden symbol '[' or ']' used in the acronym '" + bricksdatabase[i].getAcronyms()[j] + "' of the brick no. " + to_string(i + 1) + ".\n\n";
				}

				for (int k = i; k < (int)bricksdatabase.size(); k++) {
					for (int m = j; m < (int)bricksdatabase[k].getAcronyms().size(); m++) {
						if ((i == k) && (j == m)) {
							continue;
						}
						if (bricksdatabase[i].getAcronyms()[j].compare(bricksdatabase[k].getAcronyms()[m]) == 0) {
							error = true;
							errormessage = "Redundant acronym '" + bricksdatabase[i].getAcronyms()[j] + "' used in brick no. " + to_string(i + 1) + " and brick no. " + to_string(k + 1) + ".\n\n";
						}
					}
				}
			}
		}

	}


	// sequencedatabase check
	if (!error && ((mode == databasesearch) || (mode == dereplication))) {

		sequencedatabasestream.open(sequencedatabasefilename);
		if (!sequencedatabasestream.good()) {
			error = true;
			errormessage = "Cannot open the file '" + sequencedatabasefilename + "'.";
		}
		else {
			if (os) {
				*os << "Loading the database of sequences... ";
			}
			sequencedatabase.loadFromPlainTextStream(sequencedatabasestream);
			if (os) {
				*os << "ok" << endl << endl;
			}
		}
		sequencedatabasestream.close();

	}


	// initialize fragment ions for de novo graph
	fragmentionsfordenovograph.clear();
	if (!error) {
		switch (peptidetype)
		{
		case linear:
			initializeFragmentIonsForDeNovoGraphOfLinearPeptides(fragmentionsfordenovograph);
			break;
		case cyclic:
			initializeFragmentIonsForDeNovoGraphOfCyclicPeptides(fragmentionsfordenovograph);
			break;
		case branched:
			initializeFragmentIonsForDeNovoGraphOfTPeptides(fragmentionsfordenovograph);
			break;
		case lasso:
			initializeFragmentIonsForDeNovoGraphOfLassoPeptides(fragmentionsfordenovograph);
			break;
		case linearpolysaccharide:
			initializeFragmentIonsForDeNovoGraphOfLinearPolysaccharide(fragmentionsfordenovograph);
			break;
		case other:
			error = true;
			errormessage = "Invalid peptide type 'other'.\n\n";
			break;
		default:
			error = true;
			errormessage = "Undefined peptide type.\n\n";
			break;
		}
	}

	
	// report errors and return
	if (error) {
		if (os) {
			*os << endl << endl;
			*os << "Error: " << errormessage.c_str() << endl;
			*os << endl;
			*os << endl;
		}
		return -1;
	}


	return 0;
}


string cParameters::printToString() {
	string s = "";
	s += "Peptide Type: ";
	switch (peptidetype) {
	case linear:
		s += "Linear\n";
		break;
	case cyclic:
		s += "Cyclic\n";
		break;
	case branched:
		s += "Branched\n";
		break;
	case lasso:
		s += "Lasso\n";
		break;
	case linearpolysaccharide:
		s += "Linear polysaccharide (beta version)\n";
		break;
	case other:
		s += "Other\n";
		break;
	default:
		break;
	}
	s += "Peaklist File: " + peaklistfilename + "\n";
	s += "Precursor Mass: " + to_string(precursormass) + "\n";
	s += "Precursor Charge: " + to_string(precursorcharge) + "\n";
	s += "Precursor Mass Error Tolerance: " + to_string(precursormasserrortolerance) + "\n";
	s += "Fragment Mass Error Tolerance: " + to_string(fragmentmasserrortolerance) + "\n";
	s += "Fragment Mass Error Tolerance for Deisotoping: " + to_string(masserrortolerancefordeisotoping) + "\n";
	s += "Minimum Threshold of Relative Intensity: " + to_string(minimumrelativeintensitythreshold) + "\n";
	s += "Minimum m/z Ratio: " + to_string(minimummz) + "\n";
	s += "Brick Database File: " + bricksdatabasefilename + "\n";
	s += "Maximum Number of Combined Blocks (start, middle, end): " + to_string(maximumbricksincombinationbegin) + ", " + to_string(maximumbricksincombinationmiddle) + ", " + to_string(maximumbricksincombinationend) + "\n";
	s += "Maximum Cumulative Mass of Blocks: " + to_string(maximumcumulativemass) + "\n";

	s += "Generate Permutations of Combined Bricks: ";
	s += generatebrickspermutations ? "on" : "off";
	s += "\n";

	s += "N-terminal and C-terminal Modifications:\n";
	for (int i = 1; i < (int)searchedmodifications.size(); i++) {
		s += searchedmodifications[i].name + ", " + to_string(searchedmodifications[i].massdifference) + ", ";
		s += searchedmodifications[i].nterminal ? "N-terminal, " : "-, ";
		s += searchedmodifications[i].cterminal ? "C-terminal" : "-";
		s += "\n";
	}

	s += "Mode: ";
	switch ((modeType)mode) {
	case denovoengine:
		s += "De Novo Search Engine";
		break;
	case singlecomparison:
		s += "Compare Peaklist with Spectrum of Searched Sequence";
		break;
	case databasesearch:
		s += "Compare Peaklist with Database - MS/MS data";
		break;
	case dereplication:
		s += "Compare Peaklist with Database - MS data";
		break;
	default:
		break;
	}
	s += "\n";

	s += "Maximum Number of Threads: " + to_string(maximumnumberofthreads) + "\n";

	s += "Score Type: ";
	switch (scoretype) {
	case b_ions:
		s += "Number of b-ions";
		break;
	case b_ions_and_b_water_loss_ions:
		s += "Number of b-ions + dehydrated b-ions";
		break;
	case b_ions_and_b_ammonia_loss_ions:
		s += "Number of b-ions + deamidated b-ions";
		break;
	case y_ions_and_b_ions:
		s += "Number of y-ions + b-ions (not for cyclic peptides)";
		break;
	case y_ions:
		s += "Number of y-ions (not for cyclic peptides)";
		break;
	case weighted_intensity:
		s += "Sum of relative intensities of matched peaks";
		break;
	case matched_peaks:
		s += "Number of matched peaks";
		break;
	case matched_bricks:
		s += "Number of matched bricks (cyclic peptides)";
		break;
	default:
		s += "undefined";
		break;
	}
	s += "\n";

	s += "Maximum Number of Candidate Peptides Reported: " + to_string(hitsreported) + "\n";
	s += "Peptide Sequence Tag: " + originalsequencetag + "\n";

	s += "Fragment Ion Types in Theoretical Spectra: ";
	for (int i = 0; i < (int)fragmentionsfortheoreticalspectra.size(); i++) {
		s += fragmentdefinitions[fragmentionsfortheoreticalspectra[i]].name;
		if (i < (int)fragmentionsfortheoreticalspectra.size() - 1) {
			s += ", ";
		}
	}
	s += "\n";

	s += "Remove Hits of Fragments without Hits of Parent Fragments: ";
	s += clearhitswithoutparent ? "on" : "off";
	s += "\n";

	s += "Cyclic N-terminus: ";
	s += cyclicnterminus ? "on" : "off";
	s += "\n";

	s += "Cyclic C-terminus: ";
	s += cycliccterminus ? "on" : "off";
	s += "\n";

	s += "Enable Scrambling: ";
	s += enablescrambling ? "on" : "off";
	s += "\n";

	s += "Action with Edges Forming Blind Paths: ";
	switch (blindedges) {
	case 0:
		s += "none (you can see a complete de novo graph)";
		break;
	case 1:
		s += "remove (speed up the search)";
		break;
	case 2:
		s += "connect (allow detection of sequence tags)";
		break;
	default:
		break;
	}
	s += "\n";

	s += "Sequence Database File: " + sequencedatabasefilename + "\n";

	s += "Searched Peptide Sequence: " + originalsearchedsequence + "\n";
	s += "N-terminal Modification of Searched Peptide Sequence: " + searchedsequenceNtermmodif + "\n";
	s += "C-terminal Modification of Searched Peptide Sequence: " + searchedsequenceCtermmodif + "\n";
	s += "Branch Modification of Searched Peptide Sequence: " + searchedsequenceTmodif + "\n";

	s += "\n";

	return s;
}


void cParameters::setOutputStream(cMainThread& os) {
	this->os = &os;
}


void cParameters::updateFragmentDefinitions() {
	if (peptidetype == linear) {
		fragmentdefinitions.recalculateFragments(cyclicnterminus, cycliccterminus);
	}
	else {
		fragmentdefinitions.recalculateFragments(false, false);
	}
}


void cParameters::store(ofstream& os) {
	int size;
	
	fragmentdefinitions.store(os);

	os.write((char *)&peptidetype, sizeof(peptideType));

	storeString(peaklistfilename, os);
	peaklist.store(os);

	os.write((char *)&precursormass, sizeof(double));
	os.write((char *)&precursormasserrortolerance, sizeof(double));
	os.write((char *)&precursorcharge, sizeof(int));
	os.write((char *)&fragmentmasserrortolerance, sizeof(double));
	os.write((char *)&masserrortolerancefordeisotoping, sizeof(double));
	os.write((char *)&minimumrelativeintensitythreshold, sizeof(double));
	os.write((char *)&minimummz, sizeof(double));

	storeString(bricksdatabasefilename, os);
	bricksdatabase.store(os);

	os.write((char *)&maximumbricksincombinationbegin, sizeof(int));
	os.write((char *)&maximumbricksincombinationmiddle, sizeof(int));
	os.write((char *)&maximumbricksincombinationend, sizeof(int));
	os.write((char *)&maximumbricksincombination, sizeof(int));
	os.write((char *)&maximumcumulativemass, sizeof(double));
	os.write((char *)&generatebrickspermutations, sizeof(bool));
	os.write((char *)&maximumnumberofthreads, sizeof(int));
	os.write((char *)&mode, sizeof(modeType));
	os.write((char *)&scoretype, sizeof(scoreType));
	os.write((char *)&clearhitswithoutparent, sizeof(bool));
	os.write((char *)&cyclicnterminus, sizeof(bool));
	os.write((char *)&cycliccterminus, sizeof(bool));
	os.write((char *)&enablescrambling, sizeof(bool));
	os.write((char *)&hitsreported, sizeof(int));

	storeString(sequencetag, os);
	storeString(originalsequencetag, os);
	storeString(searchedsequence, os);
	storeString(originalsearchedsequence, os);
	storeString(searchedsequenceNtermmodif, os);
	storeString(searchedsequenceCtermmodif, os);
	storeString(searchedsequenceTmodif, os);

	os.write((char *)&maximumnumberofcandidates, sizeof(int));
	os.write((char *)&blindedges, sizeof(int));

	storeString(sequencedatabasefilename, os);
	sequencedatabase.store(os);

	size = (int)searchedmodifications.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)searchedmodifications.size(); i++) {
		searchedmodifications[i].store(os);
	}

	size = (int)fragmentionsfordenovograph.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)fragmentionsfordenovograph.size(); i++) {
		os.write((char *)&fragmentionsfordenovograph[i], sizeof(fragmentIonType));
	}

	size = (int)fragmentionsfortheoreticalspectra.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)fragmentionsfortheoreticalspectra.size(); i++) {
		os.write((char *)&fragmentionsfortheoreticalspectra[i], sizeof(fragmentIonType));
	}
}


void cParameters::load(ifstream& is) {
	int size;

	os = 0;

	fragmentdefinitions.load(is);

	is.read((char *)&peptidetype, sizeof(peptideType));

	loadString(peaklistfilename, is);
	peaklist.load(is);

	is.read((char *)&precursormass, sizeof(double));
	is.read((char *)&precursormasserrortolerance, sizeof(double));
	is.read((char *)&precursorcharge, sizeof(int));
	is.read((char *)&fragmentmasserrortolerance, sizeof(double));
	is.read((char *)&masserrortolerancefordeisotoping, sizeof(double));
	is.read((char *)&minimumrelativeintensitythreshold, sizeof(double));
	is.read((char *)&minimummz, sizeof(double));

	loadString(bricksdatabasefilename, is);
	bricksdatabase.load(is);

	is.read((char *)&maximumbricksincombinationbegin, sizeof(int));
	is.read((char *)&maximumbricksincombinationmiddle, sizeof(int));
	is.read((char *)&maximumbricksincombinationend, sizeof(int));
	is.read((char *)&maximumbricksincombination, sizeof(int));
	is.read((char *)&maximumcumulativemass, sizeof(double));
	is.read((char *)&generatebrickspermutations, sizeof(bool));
	is.read((char *)&maximumnumberofthreads, sizeof(int));
	is.read((char *)&mode, sizeof(modeType));
	is.read((char *)&scoretype, sizeof(scoreType));
	is.read((char *)&clearhitswithoutparent, sizeof(bool));
	is.read((char *)&cyclicnterminus, sizeof(bool));
	is.read((char *)&cycliccterminus, sizeof(bool));
	is.read((char *)&enablescrambling, sizeof(bool));
	is.read((char *)&hitsreported, sizeof(int));

	loadString(sequencetag, is);
	loadString(originalsequencetag, is);
	loadString(searchedsequence, is);
	loadString(originalsearchedsequence, is);
	loadString(searchedsequenceNtermmodif, is);
	loadString(searchedsequenceCtermmodif, is);
	loadString(searchedsequenceTmodif, is);

	is.read((char *)&maximumnumberofcandidates, sizeof(int));
	is.read((char *)&blindedges, sizeof(int));

	loadString(sequencedatabasefilename, is);
	sequencedatabase.load(is);

	is.read((char *)&size, sizeof(int));
	searchedmodifications.resize(size);
	for (int i = 0; i < (int)searchedmodifications.size(); i++) {
		searchedmodifications[i].load(is);
	}

	is.read((char *)&size, sizeof(int));
	fragmentionsfordenovograph.resize(size);
	for (int i = 0; i < (int)fragmentionsfordenovograph.size(); i++) {
		is.read((char *)&fragmentionsfordenovograph[i], sizeof(fragmentIonType));
	}

	is.read((char *)&size, sizeof(int));
	fragmentionsfortheoreticalspectra.resize(size);
	for (int i = 0; i < (int)fragmentionsfortheoreticalspectra.size(); i++) {
		is.read((char *)&fragmentionsfortheoreticalspectra[i], sizeof(fragmentIonType));
	}
}

