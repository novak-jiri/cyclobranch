#include "core/cParameters.h"

#include "gui/cMainThread.h"


cParameters::cParameters() {
	clear();
}


void cParameters::clear() {
	string s = "";
	os = 0;
	fragmentdefinitions.recalculateFragments(false, false, s);
	peptidetype = linear;
	peaklistfilename = "";
	peaklistfileformat = txt;
	peaklistseries.clear();
	precursormass = 0;
	precursoradduct = "";
	precursorAdductHasLi = false;
	precursorAdductHasNa = false;
	precursorAdductHasMg = false;
	precursorAdductHasAl = false;
	precursorAdductHasK = false;
	precursorAdductHasCa = false;
	precursorAdductHasMn = false;
	precursorAdductHasFe = false;
	precursorAdductHasCo = false;
	precursorAdductHasNi = false;
	precursorAdductHasCu = false;
	precursorAdductHasZn = false;
	precursorAdductHasGa = false;
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
	modificationsfilename = "";
	searchedmodifications.clear();
	maximumnumberofthreads = 1;
	mode = denovoengine;
	scoretype = b_ions;
	clearhitswithoutparent = false;
	cyclicnterminus = false;
	cycliccterminus = false;
	enablescrambling = false;
	similaritysearch = false;
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

	fragmentionsfordenovograph.clear();
	fragmentionsfortheoreticalspectra.clear();
}


int cParameters::checkAndPrepare(bool& terminatecomputation) {

	bool error = false;
	string errormessage = "";
	ifstream peakliststream;
	ifstream spotliststream;
	ifstream bricksdatabasestream;
	ifstream modificationsstream;
	ifstream sequencedatabasestream;
	regex rx;
	string s;
	int i, errtype;
	string foldername;
	string ibdfilename;

	if (peaklistfilename.empty()) {
		error = true;
		errormessage = "A peaklist is not specified. Have you configured the engine (Search -> Settings...) ?\n";
	}

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
			
			#if OS_TYPE == WIN
				rx = "\\.[bB][aA][fF]$";
				// Bruker Analysis File
				if (regex_search(peaklistfilename, rx)) {
					peaklistfileformat = baf;
				}

				rx = "\\.[mM][iI][sS]$";
				// flexImaging File
				if (regex_search(peaklistfilename, rx)) {
					peaklistfileformat = mis;
				}
			#endif

			rx = "\\.[iI][mM][zZ][mM][lL]$";
			// flexImaging File
			if (regex_search(peaklistfilename, rx)) {
				peaklistfileformat = imzML;
			}

		}
		catch (regex_error& e) {
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
				*os << "Converting the file " + peaklistfilename + " to mgf ... ";
				
				#if OS_TYPE == UNX
					s = installdir.toStdString() + "External/linux/any2mgf.sh " + peaklistfilename;
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The file cannot be converted.\n";
						errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
						errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
						errormessage += "Do you have FileConverter installed (sudo apt-get install topp) ?\n";
						errormessage += "Do you have 'any2mgf.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
						errormessage += "Is the file 'any2mgf.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/any2mgf.sh) ? \n";
					}
				#else
					#if OS_TYPE == OSX
						s = installdir.toStdString() + "External/macosx/any2mgf.sh " + peaklistfilename;
						if (system(s.c_str()) != 0) {
							error = true;
							errormessage = "The file cannot be converted.\n";
							errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
							errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
							errormessage += "Do you have FileConverter installed (OpenMS 1.11) ?\n";
							errormessage += "Do you have 'any2mgf.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
							errormessage += "Is the file 'any2mgf.sh' executable ? \n";
						}
					#else		
						s = "External\\windows\\any2mgf.bat \"" + peaklistfilename + "\"";
						if (system(s.c_str()) != 0) {
							error = true;
							errormessage = "The file cannot be converted.\n";
							errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
							errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
							errormessage += "Do you have FileConverter installed (OpenMS 1.11) ?\n";
							errormessage += "Do you have a path to FileConverter in your PATH variable (e.g., 'C:/Program Files/OpenMS-1.11/bin') ?\n";
							errormessage += "Do you have 'any2mgf.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
						}
					#endif
				#endif

				if (!error) {
					*os << "ok" << endl << endl;
					peakliststream.open(peaklistfilename + ".mgf");
				}
				break;
			case baf:
				#if OS_TYPE == WIN
					*os << "Converting the file " + peaklistfilename + " ... ";
					s = "External\\windows\\baf2csv.bat \"" + peaklistfilename + "\"";
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The file cannot be converted.\n";
						errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
						errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
						errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
						errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
						errormessage += "Do you have 'baf2csv.bat' file located in the 'External/windows' folder ?\n";
					}

					if (!error) {
						*os << "ok" << endl << endl;
						peakliststream.open(peaklistfilename + ".csv");
					}
				#endif
				break;
			case mis:
				#if OS_TYPE == WIN
					foldername = peaklistfilename.substr(0, peaklistfilename.rfind('.'));
					*os << "Converting flexImaging data folder " + foldername + " ... ";
					s = "External\\windows\\mis2csv.bat \"" + foldername + "\"";
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The folder cannot be converted.\n";
						errormessage += "Does the folder '" + foldername + "' exist ?\n";
						errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
						errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
						errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
						errormessage += "Do you have 'mis2csv.bat' file located in the 'External/windows' folder ?\n";
					}

					if (!error) {
						*os << "ok" << endl << endl;
						peakliststream.open(foldername + ".baf.csv");
						spotliststream.open(foldername + ".baf.txt");
					}
				#endif
				break;
			case imzML:
				ibdfilename = peaklistfilename.substr(0, (int)peaklistfilename.size() - 5);
				ibdfilename += "ibd";
				peakliststream.open(ibdfilename, std::ifstream::binary);
				break;
			default:
				break;
			}

		}
			

		if (!error) {
			if (!peakliststream.good()) {
				error = true;
				if (peaklistfileformat == mis) {
					errormessage = "Cannot open the folder '" + foldername + "'.";
				}
				else {
					errormessage = "Cannot open the file '" + peaklistfilename + "'.";
				}
			}
			else {
				if (os) {
					*os << "Loading the peak list... ";
				}
				switch (peaklistfileformat) {
				case txt:
					peaklistseries.loadFromPlainTextStream(peakliststream);
					break;
				case mzML:
				case mzXML:
				case mgf:
					peaklistseries.loadFromMGFStream(peakliststream);
					break;
				case baf:
					#if OS_TYPE == WIN
						peaklistseries.loadFromBAFStream(peakliststream);
					#endif
					break;
				case mis:
					#if OS_TYPE == WIN
						peaklistseries.loadFromBAFStream(peakliststream);
						peaklistseries.loadSpotList(spotliststream);
						spotliststream.close();
					#endif
					break;
				case imzML:
					errtype = peaklistseries.loadFromIMZMLStream(peaklistfilename, peakliststream, minimumrelativeintensitythreshold, os, terminatecomputation);
					if (errtype == -1) {
						error = true;
						errormessage = "Aborted by user.\n";
					}
					if (errtype == -2) {
						error = true;
						#if OS_TYPE == UNX
							errormessage = "Raw data cannot be converted.\n";
							errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
							errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
							errormessage += "Do you have enough space on your hard drive ?\n";
							errormessage += "Do you have OpenMS 1.11 installed (sudo apt-get install topp) ?\n";
							errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
							errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/raw2peaks.sh) ? \n";
						#else
							#if OS_TYPE == OSX
								errormessage = "Raw data cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
								errormessage += "Do you have enough space on your hard drive ?\n";
								errormessage += "Do you have OpenMS 1.11 installed ?\n";
								errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
								errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/macosx/raw2peaks.sh) ? \n";
							#else		
								errormessage = "Raw data cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
								errormessage += "Do you have enough space on your hard drive ?\n";
								errormessage += "Do you have OpenMS 1.11 installed ?\n";
								errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-1.11/bin') ?\n";
								errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
							#endif
						#endif
					}
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
			if (bricksdatabase.loadFromPlainTextStream(bricksdatabasestream, errormessage, false) == -1) {
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


	// modifications check
	if (!error && ((mode == denovoengine) || (mode == singlecomparison) || (mode == databasesearch))) {

		searchedmodifications.clear();

		if ((peptidetype != cyclic) && (modificationsfilename.compare("") != 0)) {
			modificationsstream.open(modificationsfilename);
			if (!modificationsstream.good()) {
				error = true;
				errormessage = "Cannot open the file '" + modificationsfilename + "'.";
			}
			else {
				if (os) {
					*os << "Loading modifications... ";
				}
				if (loadModificationsFromPlainTextStream(modificationsstream, searchedmodifications, errormessage, false) == -1) {
					error = true;
				}
				else {
					if (os) {
						*os << "ok" << endl << endl;
					}
				}
			}
			modificationsstream.close();
		}

		fragmentDescription fd;
		fd.nterminal = true;
		fd.cterminal = true;
		searchedmodifications.insert(searchedmodifications.begin(), fd);

	}


	// precursor adduct check
	if (!error && ((mode == denovoengine) || (mode == singlecomparison) || (mode == databasesearch))) {
		cSummaryFormula formula;
		formula.clear();
		formula.setFormula(precursoradduct);
		if (!formula.isValid(errormessage)) {
			error = true;
			errormessage = "Precursor Ion Adduct: " + errormessage;
		}

		precursorAdductHasLi = false;
		precursorAdductHasNa = false;
		precursorAdductHasMg = false;
		precursorAdductHasAl = false;
		precursorAdductHasK = false;
		precursorAdductHasCa = false;
		precursorAdductHasMn = false;
		precursorAdductHasFe = false;
		precursorAdductHasCo = false;
		precursorAdductHasNi = false;
		precursorAdductHasCu = false;
		precursorAdductHasZn = false;
		precursorAdductHasGa = false;

		rx = "Li";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasLi = true;
		}
		rx = "Na";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasNa = true;
		}
		rx = "Mg";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasMg = true;
		}
		rx = "Al";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasAl = true;
		}
		rx = "K";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasK = true;
		}
		rx = "Ca";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasCa = true;
		}
		rx = "Mn";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasMn = true;
		}
		rx = "Fe";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasFe = true;
		}
		rx = "Co";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasCo = true;
		}
		rx = "Ni";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasNi = true;
		}
		rx = "Cu";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasCu = true;
		}
		rx = "Zn";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasZn = true;
		}
		rx = "Ga";
		if (regex_search(precursoradduct, rx)) {
			precursorAdductHasGa = true;
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
		case branchcyclic:
			initializeFragmentIonsForDeNovoGraphOfBranchCyclicPeptides(fragmentionsfordenovograph);
			break;
#if OLIGOKETIDES == 1
		case linearoligoketide:
			initializeFragmentIonsForDeNovoGraphOfLinearOligoketide(fragmentionsfordenovograph);
			break;
		case cyclicoligoketide:
			initializeFragmentIonsForDeNovoGraphOfCyclicOligoketide(fragmentionsfordenovograph);
			break;
#endif
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


	// check theoretical fragments in positive/negative mode
	if (!error && (mode == dereplication)) {
		i = 0;
		while (i < (int)fragmentionsfortheoreticalspectra.size()) {
			if (fragmentdefinitions[fragmentionsfortheoreticalspectra[i]].positive != (precursorcharge > 0)) {
				fragmentionsfortheoreticalspectra.erase(fragmentionsfortheoreticalspectra.begin() + i);
			}
			else {
				i++;
			}
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
	case branchcyclic:
		s += "Branch-cyclic\n";
		break;
#if OLIGOKETIDES == 1
	case linearoligoketide:
		s += "Linear oligoketide\n";
		break;
	case cyclicoligoketide:
		s += "Cyclic oligoketide\n";
		break;
#endif
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
	s += "Precursor Ion Adduct: " + precursoradduct + "\n";
	s += "Charge: " + to_string(precursorcharge) + "\n";
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

	s += "N-/C-terminal Modifications File: " + modificationsfilename + "\n";

	s += "Action with Blind Paths in De Novo Graph: ";
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

	s += "Cyclic N-terminus: ";
	s += cyclicnterminus ? "on" : "off";
	s += "\n";

	s += "Cyclic C-terminus: ";
	s += cycliccterminus ? "on" : "off";
	s += "\n";

	s += "Enable Scrambling: ";
	s += enablescrambling ? "on" : "off";
	s += "\n";

	s += "Similarity Search: ";
	s += similaritysearch ? "on" : "off";
	s += "\n";

	s += "Mode: ";
	switch ((eModeType)mode) {
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

	s += "Sequence Database File: " + sequencedatabasefilename + "\n";

	s += "Maximum Number of Threads: " + to_string(maximumnumberofthreads) + "\n";

	s += "Score Type: ";
	switch (scoretype) {
	case b_ions:
		s += "Number of b-ions";
		break;
	case b_ions_and_b_dehydrated_ions:
		s += "Number of b-ions + dehydrated b-ions";
		break;
	case b_ions_and_b_deamidated_ions:
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

	s += "Ion Types in Theoretical Spectra: ";
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
		fragmentdefinitions.recalculateFragments(cyclicnterminus, cycliccterminus, precursoradduct);
	}
	else {
		fragmentdefinitions.recalculateFragments(false, false, precursoradduct);
	}
}


void cParameters::store(ofstream& os) {
	int size;
	
	fragmentdefinitions.store(os);

	os.write((char *)&peptidetype, sizeof(ePeptideType));

	storeString(peaklistfilename, os);
	os.write((char *)&peaklistfileformat, sizeof(ePeakListFileFormat));
	peaklistseries.store(os);

	os.write((char *)&precursormass, sizeof(double));

	storeString(precursoradduct, os);
	os.write((char *)&precursorAdductHasLi, sizeof(bool));
	os.write((char *)&precursorAdductHasNa, sizeof(bool));
	os.write((char *)&precursorAdductHasMg, sizeof(bool));
	os.write((char *)&precursorAdductHasAl, sizeof(bool));
	os.write((char *)&precursorAdductHasK, sizeof(bool));
	os.write((char *)&precursorAdductHasCa, sizeof(bool));
	os.write((char *)&precursorAdductHasMn, sizeof(bool));
	os.write((char *)&precursorAdductHasFe, sizeof(bool));
	os.write((char *)&precursorAdductHasCo, sizeof(bool));
	os.write((char *)&precursorAdductHasNi, sizeof(bool));
	os.write((char *)&precursorAdductHasCu, sizeof(bool));
	os.write((char *)&precursorAdductHasZn, sizeof(bool));
	os.write((char *)&precursorAdductHasGa, sizeof(bool));

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
	storeString(modificationsfilename, os);

	size = (int)searchedmodifications.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)searchedmodifications.size(); i++) {
		searchedmodifications[i].store(os);
	}

	os.write((char *)&maximumnumberofthreads, sizeof(int));
	os.write((char *)&mode, sizeof(eModeType));
	os.write((char *)&scoretype, sizeof(eScoreType));
	os.write((char *)&clearhitswithoutparent, sizeof(bool));
	os.write((char *)&cyclicnterminus, sizeof(bool));
	os.write((char *)&cycliccterminus, sizeof(bool));
	os.write((char *)&enablescrambling, sizeof(bool));
	os.write((char *)&similaritysearch, sizeof(bool));
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

	size = (int)fragmentionsfordenovograph.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)fragmentionsfordenovograph.size(); i++) {
		os.write((char *)&fragmentionsfordenovograph[i], sizeof(eFragmentIonType));
	}

	size = (int)fragmentionsfortheoreticalspectra.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)fragmentionsfortheoreticalspectra.size(); i++) {
		os.write((char *)&fragmentionsfortheoreticalspectra[i], sizeof(eFragmentIonType));
	}
}


void cParameters::load(ifstream& is) {
	int size;

	os = 0;

	fragmentdefinitions.load(is);

	is.read((char *)&peptidetype, sizeof(ePeptideType));

	loadString(peaklistfilename, is);
	is.read((char *)&peaklistfileformat, sizeof(ePeakListFileFormat));
	peaklistseries.load(is);

	is.read((char *)&precursormass, sizeof(double));

	loadString(precursoradduct, is);
	is.read((char *)&precursorAdductHasLi, sizeof(bool));
	is.read((char *)&precursorAdductHasNa, sizeof(bool));
	is.read((char *)&precursorAdductHasMg, sizeof(bool));
	is.read((char *)&precursorAdductHasAl, sizeof(bool));
	is.read((char *)&precursorAdductHasK, sizeof(bool));
	is.read((char *)&precursorAdductHasCa, sizeof(bool));
	is.read((char *)&precursorAdductHasMn, sizeof(bool));
	is.read((char *)&precursorAdductHasFe, sizeof(bool));
	is.read((char *)&precursorAdductHasCo, sizeof(bool));
	is.read((char *)&precursorAdductHasNi, sizeof(bool));
	is.read((char *)&precursorAdductHasCu, sizeof(bool));
	is.read((char *)&precursorAdductHasZn, sizeof(bool));
	is.read((char *)&precursorAdductHasGa, sizeof(bool));

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
	loadString(modificationsfilename, is);

	is.read((char *)&size, sizeof(int));
	searchedmodifications.resize(size);
	for (int i = 0; i < (int)searchedmodifications.size(); i++) {
		searchedmodifications[i].load(is);
	}

	is.read((char *)&maximumnumberofthreads, sizeof(int));
	is.read((char *)&mode, sizeof(eModeType));
	is.read((char *)&scoretype, sizeof(eScoreType));
	is.read((char *)&clearhitswithoutparent, sizeof(bool));
	is.read((char *)&cyclicnterminus, sizeof(bool));
	is.read((char *)&cycliccterminus, sizeof(bool));
	is.read((char *)&enablescrambling, sizeof(bool));
	is.read((char *)&similaritysearch, sizeof(bool));
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
	fragmentionsfordenovograph.resize(size);
	for (int i = 0; i < (int)fragmentionsfordenovograph.size(); i++) {
		is.read((char *)&fragmentionsfordenovograph[i], sizeof(eFragmentIonType));
	}

	is.read((char *)&size, sizeof(int));
	fragmentionsfortheoreticalspectra.resize(size);
	for (int i = 0; i < (int)fragmentionsfortheoreticalspectra.size(); i++) {
		is.read((char *)&fragmentionsfortheoreticalspectra[i], sizeof(eFragmentIonType));
	}
}

