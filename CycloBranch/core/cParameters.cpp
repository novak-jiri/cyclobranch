#include "core/cParameters.h"

#include "gui/cMainThread.h"
#include "core/cSummaryFormula.h"


cParameters::cParameters() {
	clear();
}


void cParameters::clear() {
	string s = "";
	os = 0;
	iondefinitions.recalculateFragments(false, false, s);
	peptidetype = linear;
	peaklistfilename = "";
	useprofiledata = false;
	convertprofiledata = true;
	profiledatafilename = "";
	peaklistfileformat = txt;
	peaklistseries.clear();
	scannumber = 1;
	precursormass = 0;
	precursoradduct = "";
	metaladducts.clear();	
	precursormasserrortolerance = 5;
	precursorcharge = 1;
	fragmentmasserrortolerance = 5;
	minimumrelativeintensitythreshold = 1;
	minimumabsoluteintensitythreshold = 0;
	minimummz = 150;
	fwhm = 0.05;
	bricksdatabasefilename = "";
	bricksdatabase.clear();
	maximumbricksincombinationbegin = 1;
	maximumbricksincombinationmiddle = 1;
	maximumbricksincombinationend = 1;
	maximumbricksincombination = max(max(maximumbricksincombinationbegin, maximumbricksincombinationmiddle), maximumbricksincombinationend);
	maximumcumulativemass = 0;
	modificationsfilename = "";
	searchedmodifications.clear();
	maximumnumberofthreads = 1;
	mode = denovoengine;
	scoretype = number_of_matched_peaks;
	maximumcombinedlosses = 2;
	//clearhitswithoutparent = false;
	reportunmatchedtheoreticalpeaks = false;
	generateisotopepattern = false;
	minimumpatternsize = 1;
	cyclicnterminus = false;
	cycliccterminus = false;
	enablescrambling = false;
	similaritysearch = false;
	regularblocksorder = false;
	hitsreported = 100;
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
	ionsfortheoreticalspectra.clear();

	neutrallossesdefinitions.clear();
	neutralLoss loss;
	cSummaryFormula tmpformula;
	for (int i = 0; i < (int)defaultneutrallosses.getNeutralLosses().size(); i++) {
		loss.clear();
		loss.summary = defaultneutrallosses.getNeutralLosses()[i];
		addStringFormulaToMap(loss.summary, loss.summarymap);
		tmpformula.setFormula(loss.summary);
		loss.massdifference = tmpformula.getMass();
		neutrallossesdefinitions.push_back(loss);
	}
	neutrallossesfortheoreticalspectra.clear();
	numberofgeneratedneutrallosses = 0;

	originalneutrallossesdefinitions = neutrallossesdefinitions;
	originalneutrallossesfortheoreticalspectra = neutrallossesfortheoreticalspectra;

	peakidtodesc.clear();
	isotopeformulaidtodesc.clear();

	defaultmaxx = 1;
	defaultmaxy = 1;
	defaultpixelsizex = 1;
	defaultpixelsizey = 1;
	vendor = unknownvendor;
}


int cParameters::checkAndPrepare(bool& terminatecomputation) {

	bool error = false;
	string errormessage = "";
	ifstream peakliststream;
	ifstream spotliststream;
	ifstream titleliststream;
	ifstream bricksdatabasestream;
	ifstream modificationsstream;
	ifstream sequencedatabasestream;
	regex rx;
	string s;
	int i, errtype;
	string foldername;
	string ibdfilename;
	string mzmlname;

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

				rx = "\\.[rR][aA][wW]$";
				// Thermo RAW file
				if (regex_search(peaklistfilename, rx)) {
					peaklistfileformat = raw;
				}

				rx = "\\.[dD][aA][tT]$";
				// dat file in Waters raw directory
				if (regex_search(peaklistfilename, rx)) {
					peaklistfileformat = dat;
				}

				rx = "\\.[mM][iI][sS]$";
				// flexImaging File
				if (regex_search(peaklistfilename, rx)) {
					peaklistfileformat = mis;
				}

				rx = "ser$";
				// apex File
				if (regex_search(peaklistfilename, rx)) {
					peaklistfileformat = ser;
				}
			#endif

			rx = "\\.[iI][mM][zZ][mM][lL]$";
			// imzML File
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
			case mzXML:
				*os << "Converting the file " + peaklistfilename + " to mgf ... ";
				
				#if OS_TYPE == UNX
					s = installdir.toStdString() + "External/linux/any2mgf.sh " + peaklistfilename;
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The file cannot be converted.\n";
						errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
						errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
						errormessage += "Do you have FileConverter installed (OpenMS 2.x must be installed) ?\n";
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
							errormessage += "Do you have FileConverter installed (OpenMS 2.x must be installed) ?\n";
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
							errormessage += "Do you have FileConverter installed (OpenMS 2.x must be installed) ?\n";
							errormessage += "Do you have a path to FileConverter in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
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
					*os << "Processing the file " + peaklistfilename + ":" << endl;

					*os << "centroid spectra ... ";
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
						*os << "ok" << endl;
						peakliststream.open(peaklistfilename + ".csv");
					}

					if (useprofiledata && convertprofiledata) {
						*os << "profile spectra ... ";
						s = "External\\windows\\baf2profile.bat \"" + peaklistfilename + "\"";
						if (system(s.c_str()) != 0) {
							error = true;
							errormessage = "The file cannot be converted.\n";
							errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
							errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
							errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
							errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
							errormessage += "Do you have 'baf2profile.bat' file located in the 'External/windows' folder ?\n";
						}

						if (!error) {
							*os << "ok" << endl;
						}
					}

					*os << endl;
				#endif
				break;
			case raw:
				#if OS_TYPE == WIN
					*os << "Converting the file " + peaklistfilename + " ... ";
					s = "External\\windows\\raw2mzml.bat \"" + peaklistfilename + "\"";
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The file cannot be converted.\n";
						errormessage += "Is the file '" + peaklistfilename + "' opened elsewhere ?\n";
						errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
						errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
						errormessage += "Do you have msconvert.exe installed (OpenMS 2.x including ProteoWizard must be installed) ?\n";
						errormessage += "Do you have a path to msconvert.exe in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/share/OpenMS/THIRDPARTY/pwiz-bin') ?\n";
						errormessage += "Do you have 'raw2mzml.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
					}

					if (!error) {
						*os << "ok" << endl << endl;
						mzmlname = peaklistfilename.substr(0, peaklistfilename.rfind('.')) + ".mzML";
						peakliststream.open(mzmlname);
					}
				#endif
				break;
			case dat:
				#if OS_TYPE == WIN
					foldername = peaklistfilename.substr(0, peaklistfilename.rfind('/'));
					*os << "Converting the raw data folder " + foldername + " ... ";
					s = "External\\windows\\waters\\raw2mgf.exe \"" + foldername + "\"";
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The raw data folder cannot be converted.\n";
						errormessage += "Does the folder '" + foldername + "' exist ?\n";
						errormessage += "Is the folder with the folder '" + foldername + "' writable ?\n";
						errormessage += "Do you have 'raw2mgf.exe' file located in the 'External/windows/waters' folder ?\n";
					}

					if (!error) {
						*os << "ok" << endl << endl;
						string mgfname = foldername.substr(0, foldername.rfind('.')) + ".mgf";
						peakliststream.open(mgfname);
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
			case ser:
				#if OS_TYPE == WIN
					foldername = peaklistfilename.substr(0, peaklistfilename.length() - 4);
					*os << "Converting apex data folder " + foldername + " ... ";
					s = "External\\windows\\ser2csv.bat \"" + foldername + "\"";
					if (system(s.c_str()) != 0) {
						error = true;
						errormessage = "The folder cannot be converted.\n";
						errormessage += "Does the folder '" + foldername + "' exist ?\n";
						errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
						errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
						errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
						errormessage += "Do you have 'ser2csv.bat' file located in the 'External/windows' folder ?\n";
					}

					if (!error) {
						*os << "ok" << endl << endl;
						peakliststream.open(foldername + ".csv");
						titleliststream.open(foldername + ".txt");
					}
				#endif
				break;
			case mzML:
				peakliststream.open(peaklistfilename);
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
				if ((peaklistfileformat == mis) || (peaklistfileformat == ser) || (peaklistfileformat == dat)) {
					errormessage = "Cannot open the folder '" + foldername + "'.";
				}
				else {
					errormessage = "Cannot open the file '" + peaklistfilename + "'.";
					if (peaklistfileformat == baf) {
						errormessage += "\n\nDo you have Bruker Daltonik's CompassXport installed ?\n";
						errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
					}
				}
			}
			else {
				if (os && (peaklistfileformat != mzML) && (peaklistfileformat != imzML) && (peaklistfileformat != raw) && (peaklistfileformat != ser)) {
					*os << "Loading the peaklist(s)... ";
				}
				switch (peaklistfileformat) {
				case txt:
					peaklistseries.loadFromPlainTextStream(peakliststream);
					break;
				case mzXML:
				case mgf:
					peaklistseries.loadFromMGFStream(peakliststream);
					break;
				case baf:
					#if OS_TYPE == WIN
						peaklistseries.loadFromBAFStream(peakliststream);
					#endif
					break;
				case raw:
					#if OS_TYPE == WIN
						errtype = peaklistseries.loadFromMZMLStream(mzmlname, peakliststream, fwhm, mode, os, terminatecomputation);
						if (errtype == -1) {
							error = true;
							errormessage = "Aborted by user.\n";
						}
						if (errtype == -2) {
							error = true;
							errormessage = "Raw data cannot be converted.\n";
							errormessage += "Does the file '" + mzmlname + "' exist ?\n";
							errormessage += "Is the directory with the file '" + mzmlname + "' writable ?\n";
							errormessage += "Do you have enough space on your hard drive ?\n";
							errormessage += "Do you have OpenMS 2.x installed ?\n";
							errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
							errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
						}
						if (errtype == -3) {
							error = true;
							errormessage = "Failed to load the mzML file, zlib compression is not supported. The spectra must be stored in the mzML file with the attribute \"no compression\".\n";
						}
					#endif
					break;
				case dat:
					#if OS_TYPE == WIN
						peaklistseries.loadFromMGFStream(peakliststream);
					#endif
					break;
				case mis:
					#if OS_TYPE == WIN
						peaklistseries.loadFromBAFStream(peakliststream);
						peaklistseries.loadSpotList(spotliststream);
						spotliststream.close();
					#endif
					break;
				case ser:
					#if OS_TYPE == WIN
						errtype = peaklistseries.loadFromProfileApexStream(peaklistfilename, peakliststream, titleliststream, fwhm, os, terminatecomputation);
						titleliststream.close();
						if (errtype == -1) {
							error = true;
							errormessage = "Aborted by user.\n";
						}
						if (errtype == -2) {
							error = true;
							errormessage = "Raw data cannot be converted.\n";
							errormessage += "Does the folder '" + peaklistfilename.substr(0, peaklistfilename.length() - 4) + "' exist ?\n";
							errormessage += "Is the directory '" + peaklistfilename.substr(0, peaklistfilename.length() - 4) + "' writable ?\n";
							errormessage += "Do you have enough space on your hard drive ?\n";
							errormessage += "Do you have OpenMS 2.x installed ?\n";
							errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
							errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
						}
					#endif
					break;
				case mzML:
					errtype = peaklistseries.loadFromMZMLStream(peaklistfilename, peakliststream, fwhm, mode, os, terminatecomputation);
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
							errormessage += "Do you have OpenMS 2.x installed ?\n";
							errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
							errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/raw2peaks.sh) ? \n";
						#else
							#if OS_TYPE == OSX
								errormessage = "Raw data cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
								errormessage += "Do you have enough space on your hard drive ?\n";
								errormessage += "Do you have OpenMS 2.x installed ?\n";
								errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
								errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/macosx/raw2peaks.sh) ? \n";
							#else		
								errormessage = "Raw data cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
								errormessage += "Do you have enough space on your hard drive ?\n";
								errormessage += "Do you have OpenMS 2.x installed ?\n";
								errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
								errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
							#endif
						#endif
					}
					if (errtype == -3) {
						error = true;
						errormessage = "Failed to load the mzML file, zlib compression is not supported. The spectra must be stored in the mzML file with the attribute \"no compression\".\n";
					}
					break;
				case imzML:
					errtype = peaklistseries.loadFromIMZMLStream(peaklistfilename, peakliststream, fwhm, defaultmaxx, defaultmaxy, defaultpixelsizex, defaultpixelsizey, vendor, os, terminatecomputation);
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
							errormessage += "Do you have OpenMS 2.x installed ?\n";
							errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
							errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/raw2peaks.sh) ? \n";
						#else
							#if OS_TYPE == OSX
								errormessage = "Raw data cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
								errormessage += "Do you have enough space on your hard drive ?\n";
								errormessage += "Do you have OpenMS 2.x installed ?\n";
								errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
								errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/macosx/raw2peaks.sh) ? \n";
							#else		
								errormessage = "Raw data cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilename + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilename + "' writable ?\n";
								errormessage += "Do you have enough space on your hard drive ?\n";
								errormessage += "Do you have OpenMS 2.x installed ?\n";
								errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
								errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
							#endif
						#endif
					}
					if (errtype == -3) {
						error = true;
						errormessage = "Failed to load the imzML file, zlib compression is not supported. The spectra must be stored in the imzML file with the attribute \"no compression\".\n";
					}
					break;
				default:
					break;
				}
				if (os && (peaklistfileformat != mzML) && (peaklistfileformat != imzML) && (peaklistfileformat != raw) && (peaklistfileformat != ser)) {
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
			bool skiph2blocks = !((peptidetype == linearpolyketide) || (peptidetype == cyclicpolyketide));
			if (bricksdatabase.loadFromPlainTextStream(bricksdatabasestream, errormessage, false, skiph2blocks) == -1) {
				error = true;
			}
			else {
				if (os) {
					*os << "ok" << endl << endl;
				}
			}
		}
		bricksdatabasestream.close();

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

		bricksdatabase.sortbyMass();
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

		metaladducts.clear();

		for (int i = 0; i < (int)periodictablemap.getAdductElements().size(); i++) {
			rx = periodictablemap.getAdductElements()[i] + "[^a-z]";
			if (regex_search(precursoradduct, rx)) {
				metaladducts.push_back(periodictablemap.getAdductElements()[i]);
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
	if (!error && (mode == denovoengine)) {
		switch (peptidetype) {
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
			case linearpolyketide:
				initializeFragmentIonsForDeNovoGraphOfLinearPolyketide(fragmentionsfordenovograph);
				break;
			case cyclicpolyketide:
				initializeFragmentIonsForDeNovoGraphOfCyclicPolyketide(fragmentionsfordenovograph);
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
		while (i < (int)ionsfortheoreticalspectra.size()) {
			if (iondefinitions[ionsfortheoreticalspectra[i]].positive != (precursorcharge > 0)) {
				ionsfortheoreticalspectra.erase(ionsfortheoreticalspectra.begin() + i);
			}
			else {
				i++;
			}
		}
	}


	// calculate combinations of neutral losses
	if (!error) {
		errtype = calculateNeutralLosses(terminatecomputation);
		if (errtype == -1) {
			error = true;
			errormessage = "Aborted by user.\n";
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

	s += "Mode: ";
	switch ((eModeType)mode) {
	case denovoengine:
		s += "De Novo Search Engine";
		break;
	case singlecomparison:
		s += "Compare Peaklist(s) with Spectrum of Searched Sequence";
		break;
	case databasesearch:
		s += "Compare Peaklist with Database - MS/MS data";
		break;
	case dereplication:
		s += "Compare Peaklist(s) with Database - MS or MSI data";
		break;
	default:
		break;
	}
	s += "\n";

	s += "Maximum Number of Threads: " + to_string(maximumnumberofthreads) + "\n";

	s += "Peptide Type: ";
	switch (peptidetype) {
		case linear:
			s += "Linear";
			break;
		case cyclic:
			s += "Cyclic";
			break;
		case branched:
			s += "Branched";
			break;
		case branchcyclic:
			s += "Branch-cyclic";
			break;
		case linearpolyketide:
			s += "Linear polyketide";
			break;
		case cyclicpolyketide:
			s += "Cyclic polyketide";
			break;
		case other:
			s += "Other";
			break;
		default:
			break;
	}
	s += "\n";

	s += "File: " + peaklistfilename + "\n";

	s += "Use Profile Data: ";
	s += useprofiledata ? "on" : "off";
	s += "\n";

	s += "Scan no.: " + to_string(scannumber) + "\n";
	s += "Precursor m/z Ratio: " + to_string(precursormass) + "\n";
	s += "Precursor Ion Adduct: " + precursoradduct + "\n";
	s += "Charge: " + to_string(precursorcharge) + "\n";
	s += "Precursor m/z Error Tolerance: " + to_string(precursormasserrortolerance) + "\n";
	s += "m/z Error Tolerance: " + to_string(fragmentmasserrortolerance) + "\n";
	s += "Minimum Threshold of Relative Intensity: " + to_string(minimumrelativeintensitythreshold) + "\n";
	s += "Minimum Threshold of Absolute Intensity: " + to_string(minimumabsoluteintensitythreshold) + "\n";
	s += "Minimum m/z Ratio: " + to_string(minimummz) + "\n";
	s += "FWHM: " + to_string(fwhm) + "\n";
	s += "Building Blocks Database File: " + bricksdatabasefilename + "\n";
	s += "Maximum Number of Combined Blocks (start, middle, end): " + to_string(maximumbricksincombinationbegin) + ", " + to_string(maximumbricksincombinationmiddle) + ", " + to_string(maximumbricksincombinationend) + "\n";
	s += "Maximum Cumulative Mass of Blocks: " + to_string(maximumcumulativemass) + "\n";

	s += "N-/C-terminal Modifications File: " + modificationsfilename + "\n";

	s += "Incomplete Paths in De Novo Graph: ";
	switch (blindedges) {
		case 0:
			s += "keep (you can see a complete de novo graph)";
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

	s += "Disable Precursor Mass Filter: ";
	s += similaritysearch ? "on" : "off";
	s += "\n";

	s += "Regular Order of Ketide Blocks: ";
	s += regularblocksorder ? "on" : "off";
	s += "\n";

	s += "Sequence/Compound Database File: " + sequencedatabasefilename + "\n";

	s += "Score Type: ";
	switch (scoretype) {
		case number_of_matched_peaks:
			s += "Number of matched peaks";
			break;
		case sum_of_relative_intensities:
			s += "Sum of relative intensities of matched peaks";
			break;
		case number_of_b_ions:
			s += "Number of b-ions";
			break;
		case number_of_y_ions:
			s += "Number of y-ions";
			break;
		case number_of_b_and_y_ions:
			s += "Number of b-ions and y-ions";
			break;
		default:
			s += "undefined";
			break;
	}
	s += "\n";

	s += "Maximum Number of Reported Sequence Candidates: " + to_string(hitsreported) + "\n";
	s += "Peptide Sequence Tag: " + originalsequencetag + "\n";

	s += "Ion Types: ";
	for (int i = 0; i < (int)ionsfortheoreticalspectra.size(); i++) {
		s += iondefinitions[ionsfortheoreticalspectra[i]].name;
		if (i < (int)ionsfortheoreticalspectra.size() - 1) {
			s += ", ";
		}
	}
	s += "\n";

	s += "Neutral Losses: ";
	for (int i = 0; i < (int)originalneutrallossesfortheoreticalspectra.size(); i++) {
		s += originalneutrallossesdefinitions[originalneutrallossesfortheoreticalspectra[i]].summary;
		if (i < (int)originalneutrallossesfortheoreticalspectra.size() - 1) {
			s += ", ";
		}
	}
	s += "\n";

	s += "Maximum Number of Combined Neutral Losses: " + to_string(maximumcombinedlosses) + "\n";

	//s += "Remove Hits of Fragments without Hits of Parent Fragments: ";
	//s += clearhitswithoutparent ? "on" : "off";
	//s += "\n";

	s += "Report Unmatched Theoretical Peaks: ";
	s += reportunmatchedtheoreticalpeaks ? "on" : "off";
	s += "\n";

	s += "Generate Full Isotope Patterns: ";
	s += generateisotopepattern ? "on" : "off";
	s += "\n";
	
	s += "Minimum Pattern Size: " + to_string(minimumpatternsize) + "\n";

	s += "Searched Sequence: " + originalsearchedsequence + "\n";
	s += "N-terminal Modification: " + searchedsequenceNtermmodif + "\n";
	s += "C-terminal Modification: " + searchedsequenceCtermmodif + "\n";
	s += "Branch Modification: " + searchedsequenceTmodif + "\n";

	s += "\n";

	return s;
}


void cParameters::setOutputStream(cMainThread& os) {
	this->os = &os;
}


void cParameters::updateFragmentDefinitions() {
	if ((peptidetype == linear) || (peptidetype == linearpolyketide)) {
		iondefinitions.recalculateFragments(cyclicnterminus, cycliccterminus, precursoradduct);
	}
	else {
		iondefinitions.recalculateFragments(false, false, precursoradduct);
	}
}


int cParameters::calculateNeutralLosses(bool& terminatecomputation) {
	neutrallossesdefinitions.clear();
	neutrallossesfortheoreticalspectra.clear();
	numberofgeneratedneutrallosses = 0;

	if (maximumcombinedlosses == 0) {
		return 0;
	}

	cSummaryFormula tmpformula;
	string tmpstring;
	neutralLoss loss;
	int i;

	cBricksDatabase neutrallossesbrickdatabase;
	cBrick tmpbrick;
	int numberofbasicbricks = 0;
	string compositionname;
	vector<int> intcomposition;

	int compressionlimit = 100000;
	bool compressformulas;

	bool undefinedelement;
	int valence;
	int atomscount;

	//int stringsizeest = 0;
	//int mapsizeest = 0;
	//int doublesizeest = 0;
	//int pruned = 0;

	if (os) {
		*os << "Calculating combinations of neutral losses... " << endl;
	}

	for (i = 0; i < (int)originalneutrallossesfortheoreticalspectra.size(); i++) {
		tmpformula.setFormula(originalneutrallossesdefinitions[originalneutrallossesfortheoreticalspectra[i]].summary, false);

		tmpbrick.clear();
		tmpbrick.setComposition(to_string(numberofbasicbricks + 1), false);
		tmpbrick.setMass(tmpformula.getMass());
		tmpbrick.setSummary(originalneutrallossesdefinitions[originalneutrallossesfortheoreticalspectra[i]].summary);

		neutrallossesbrickdatabase.push_back(tmpbrick);

		numberofbasicbricks++;
	}

	neutrallossesbrickdatabase.sortbyMass();

	vector<int> combarray;
	for (i = 0; i < maximumcombinedlosses; i++) {
		combarray.push_back(0);
	}

	i = 0;
	compressformulas = false;
	while (neutrallossesbrickdatabase.nextCombination(combarray, numberofbasicbricks, maximumcombinedlosses, 0, uncharge(precursormass, precursorcharge) - minimummz)) {
		if (terminatecomputation) {
			neutrallossesdefinitions.clear();
			neutrallossesfortheoreticalspectra.clear();
			numberofgeneratedneutrallosses = 0;
			return -1;
		}

		getNameOfCompositionFromIntVector(compositionname, combarray);

		tmpbrick.clear();
		tmpbrick.setComposition(compositionname, true);
		tmpbrick.setMass(neutrallossesbrickdatabase.getMassOfComposition(combarray, numberofbasicbricks));

		loss.clear();
		loss.massdifference = -tmpbrick.getMass();

		intcomposition.clear();
		tmpbrick.explodeToIntComposition(intcomposition);
		for (int j = 0; j < (int)intcomposition.size(); j++) {
			loss.summary += neutrallossesbrickdatabase[intcomposition[j] - 1].getSummary();
		}

		tmpformula.clear();
		tmpformula.addFormula(loss.summary, true);
		tmpstring = tmpformula.getSummary();
		addStringFormulaToMap(tmpstring, loss.summarymap);

		undefinedelement = false;
		for (auto it = loss.summarymap.begin(); it != loss.summarymap.end(); ++it) {
			if ((it->first.compare("H") != 0) && (it->first.compare("C") != 0) && (it->first.compare("O") != 0) && (it->first.compare("N") != 0) && (it->first.compare("S") != 0)) {
				undefinedelement = true;
				break;
			}
		}

		valence = 0;
		atomscount = 0;
		if (loss.summarymap.count("H") > 0) {
			valence += -loss.summarymap["H"];
			atomscount += -loss.summarymap["H"];
		}
		if (loss.summarymap.count("C") > 0) {
			valence += -loss.summarymap["C"] * 4;
			atomscount += -loss.summarymap["C"];
		}
		if (loss.summarymap.count("O") > 0) {
			valence += -loss.summarymap["O"] * 2;
			atomscount += -loss.summarymap["O"];
		}
		if (loss.summarymap.count("N") > 0) {
			valence += -loss.summarymap["N"] * 3;
			atomscount += -loss.summarymap["N"];
		}
		if (loss.summarymap.count("S") > 0) {
			valence += -loss.summarymap["S"] * 6; /* the maximum valence state is used */
			atomscount += -loss.summarymap["S"];
		}

		// SENIOR rule 1 - the sum of valences must be even
		// SENIOR rule 3 - the sum of valences >= 2 * (atomscount - maximum number of allowed components in the graph); edges - nodes + components >= 0
		if (!undefinedelement && ((valence % 2 == 1) || (valence < 2 * (atomscount - 10)))) {
			//pruned++;
			continue;
		}

		if (compressformulas) {
			tmpformula.clear();
			tmpformula.addFormula(loss.summary);
			loss.summary = tmpformula.getSummary();
		}

		//stringsizeest += sizeof(string) + loss.summary.size();
		//mapsizeest += sizeof(loss.summarymap) + loss.summarymap.size() * (sizeof(string) /* the length of string is 1 for HCONS */ + sizeof(int));
		//doublesizeest += sizeof(double);

		neutrallossesdefinitions.push_back(loss);
		neutrallossesfortheoreticalspectra.push_back(i);

		i++;

		if (i == compressionlimit) {
			compressformulas = true;
		}

		if (i % 100000 == 0) {
			if (os) {
				*os << i << " ";
			}

			//cout << i << " " << pruned << " - " << stringsizeest << " " << mapsizeest << " " << doublesizeest << " " << stringsizeest + mapsizeest + doublesizeest << endl;
		}

		if (i % 1000000 == 0) {
			if (os) {
				*os << endl;
			}
		}
	}

	numberofgeneratedneutrallosses = (int)neutrallossesfortheoreticalspectra.size();

	if (os) {
		*os << "ok" << endl;
		*os << "Number of combined losses: " << neutrallossesfortheoreticalspectra.size() << endl << endl;
	}

	if (neutrallossesfortheoreticalspectra.size() >= compressionlimit) {
		for (int i = 0; i < compressionlimit; i++) {
			tmpformula.clear();
			tmpformula.addFormula(neutrallossesdefinitions[neutrallossesfortheoreticalspectra[i]].summary);
			neutrallossesdefinitions[neutrallossesfortheoreticalspectra[i]].summary = tmpformula.getSummary();

			if (terminatecomputation) {
				neutrallossesdefinitions.clear();
				neutrallossesfortheoreticalspectra.clear();
				numberofgeneratedneutrallosses = 0;
				return -1;
			}
		}
	}

	return 0;
}


void cParameters::store(ofstream& os) {
	int size;
	string s;
	
	iondefinitions.store(os);

	os.write((char *)&peptidetype, sizeof(ePeptideType));

	storeString(peaklistfilename, os);
	os.write((char *)&peaklistfileformat, sizeof(ePeakListFileFormat));
	peaklistseries.store(os);

	os.write((char *)&useprofiledata, sizeof(bool));
	os.write((char *)&convertprofiledata, sizeof(bool));

	storeString(profiledatafilename, os);

	os.write((char *)&scannumber, sizeof(int));
	os.write((char *)&precursormass, sizeof(double));

	storeString(precursoradduct, os);
	storeStringVector(metaladducts, os);

	os.write((char *)&precursormasserrortolerance, sizeof(double));
	os.write((char *)&precursorcharge, sizeof(int));
	os.write((char *)&fragmentmasserrortolerance, sizeof(double));
	os.write((char *)&minimumrelativeintensitythreshold, sizeof(double));
	os.write((char *)&minimumabsoluteintensitythreshold, sizeof(unsigned));
	os.write((char *)&minimummz, sizeof(double));
	os.write((char *)&fwhm, sizeof(double));

	storeString(bricksdatabasefilename, os);
	bricksdatabase.store(os);

	os.write((char *)&maximumbricksincombinationbegin, sizeof(int));
	os.write((char *)&maximumbricksincombinationmiddle, sizeof(int));
	os.write((char *)&maximumbricksincombinationend, sizeof(int));
	os.write((char *)&maximumbricksincombination, sizeof(int));
	os.write((char *)&maximumcumulativemass, sizeof(double));
	storeString(modificationsfilename, os);

	size = (int)searchedmodifications.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)searchedmodifications.size(); i++) {
		searchedmodifications[i].store(os);
	}

	os.write((char *)&maximumnumberofthreads, sizeof(int));
	os.write((char *)&mode, sizeof(eModeType));
	os.write((char *)&scoretype, sizeof(eScoreType));
	os.write((char *)&maximumcombinedlosses, sizeof(int));
	//os.write((char *)&clearhitswithoutparent, sizeof(bool));
	os.write((char *)&reportunmatchedtheoreticalpeaks, sizeof(bool));
	os.write((char *)&generateisotopepattern, sizeof(bool));
	os.write((char *)&minimumpatternsize, sizeof(int));
	os.write((char *)&cyclicnterminus, sizeof(bool));
	os.write((char *)&cycliccterminus, sizeof(bool));
	os.write((char *)&enablescrambling, sizeof(bool));
	os.write((char *)&similaritysearch, sizeof(bool));
	os.write((char *)&regularblocksorder, sizeof(bool));
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

	size = (int)ionsfortheoreticalspectra.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)ionsfortheoreticalspectra.size(); i++) {
		os.write((char *)&ionsfortheoreticalspectra[i], sizeof(eFragmentIonType));
	}

	size = (int)neutrallossesdefinitions.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)neutrallossesdefinitions.size(); i++) {
		neutrallossesdefinitions[i].store(os);
	}

	size = (int)originalneutrallossesdefinitions.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)originalneutrallossesdefinitions.size(); i++) {
		originalneutrallossesdefinitions[i].store(os);
	}

	size = (int)neutrallossesfortheoreticalspectra.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)neutrallossesfortheoreticalspectra.size(); i++) {
		os.write((char *)&neutrallossesfortheoreticalspectra[i], sizeof(int));
	}

	size = (int)originalneutrallossesfortheoreticalspectra.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)originalneutrallossesfortheoreticalspectra.size(); i++) {
		os.write((char *)&originalneutrallossesfortheoreticalspectra[i], sizeof(int));
	}

	os.write((char *)&numberofgeneratedneutrallosses, sizeof(int));

	storeStringVector(peakidtodesc, os);
	storeStringVector(isotopeformulaidtodesc, os);

	os.write((char *)&defaultmaxx, sizeof(int));
	os.write((char *)&defaultmaxy, sizeof(int));
	os.write((char *)&defaultpixelsizex, sizeof(int));
	os.write((char *)&defaultpixelsizey, sizeof(int));
	os.write((char *)&vendor, sizeof(eVendorType));
}


void cParameters::load(ifstream& is) {
	int size;
	string s;

	os = 0;

	iondefinitions.load(is);

	is.read((char *)&peptidetype, sizeof(ePeptideType));

	loadString(peaklistfilename, is);
	is.read((char *)&peaklistfileformat, sizeof(ePeakListFileFormat));
	peaklistseries.load(is);

	is.read((char *)&useprofiledata, sizeof(bool));
	is.read((char *)&convertprofiledata, sizeof(bool));

	loadString(profiledatafilename, is);

	is.read((char *)&scannumber, sizeof(int));
	is.read((char *)&precursormass, sizeof(double));

	loadString(precursoradduct, is);
	loadStringVector(metaladducts, is);
	
	is.read((char *)&precursormasserrortolerance, sizeof(double));
	is.read((char *)&precursorcharge, sizeof(int));
	is.read((char *)&fragmentmasserrortolerance, sizeof(double));
	is.read((char *)&minimumrelativeintensitythreshold, sizeof(double));
	is.read((char *)&minimumabsoluteintensitythreshold, sizeof(unsigned));
	is.read((char *)&minimummz, sizeof(double));
	is.read((char *)&fwhm, sizeof(double));

	loadString(bricksdatabasefilename, is);
	bricksdatabase.load(is);

	is.read((char *)&maximumbricksincombinationbegin, sizeof(int));
	is.read((char *)&maximumbricksincombinationmiddle, sizeof(int));
	is.read((char *)&maximumbricksincombinationend, sizeof(int));
	is.read((char *)&maximumbricksincombination, sizeof(int));
	is.read((char *)&maximumcumulativemass, sizeof(double));
	loadString(modificationsfilename, is);

	is.read((char *)&size, sizeof(int));
	searchedmodifications.resize(size);
	for (int i = 0; i < (int)searchedmodifications.size(); i++) {
		searchedmodifications[i].load(is);
	}

	is.read((char *)&maximumnumberofthreads, sizeof(int));
	is.read((char *)&mode, sizeof(eModeType));
	is.read((char *)&scoretype, sizeof(eScoreType));
	is.read((char *)&maximumcombinedlosses, sizeof(int));
	//is.read((char *)&clearhitswithoutparent, sizeof(bool));
	is.read((char *)&reportunmatchedtheoreticalpeaks, sizeof(bool));
	is.read((char *)&generateisotopepattern, sizeof(bool));
	is.read((char *)&minimumpatternsize, sizeof(int));
	is.read((char *)&cyclicnterminus, sizeof(bool));
	is.read((char *)&cycliccterminus, sizeof(bool));
	is.read((char *)&enablescrambling, sizeof(bool));
	is.read((char *)&similaritysearch, sizeof(bool));
	is.read((char *)&regularblocksorder, sizeof(bool));
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
	ionsfortheoreticalspectra.resize(size);
	for (int i = 0; i < (int)ionsfortheoreticalspectra.size(); i++) {
		is.read((char *)&ionsfortheoreticalspectra[i], sizeof(eFragmentIonType));
	}

	is.read((char *)&size, sizeof(int));
	neutrallossesdefinitions.resize(size);
	for (int i = 0; i < (int)neutrallossesdefinitions.size(); i++) {
		neutrallossesdefinitions[i].load(is);
	}

	is.read((char *)&size, sizeof(int));
	originalneutrallossesdefinitions.resize(size);
	for (int i = 0; i < (int)originalneutrallossesdefinitions.size(); i++) {
		originalneutrallossesdefinitions[i].load(is);
	}

	is.read((char *)&size, sizeof(int));
	neutrallossesfortheoreticalspectra.resize(size);
	for (int i = 0; i < (int)neutrallossesfortheoreticalspectra.size(); i++) {
		is.read((char *)&neutrallossesfortheoreticalspectra[i], sizeof(int));
	}

	is.read((char *)&size, sizeof(int));
	originalneutrallossesfortheoreticalspectra.resize(size);
	for (int i = 0; i < (int)originalneutrallossesfortheoreticalspectra.size(); i++) {
		is.read((char *)&originalneutrallossesfortheoreticalspectra[i], sizeof(int));
	}

	is.read((char *)&numberofgeneratedneutrallosses, sizeof(int));

	loadStringVector(peakidtodesc, is);
	loadStringVector(isotopeformulaidtodesc, is);

	is.read((char *)&defaultmaxx, sizeof(int));
	is.read((char *)&defaultmaxy, sizeof(int));
	is.read((char *)&defaultpixelsizex, sizeof(int));
	is.read((char *)&defaultpixelsizey, sizeof(int));
	is.read((char *)&vendor, sizeof(eVendorType));
}

