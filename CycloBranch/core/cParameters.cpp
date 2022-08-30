#include "core/cParameters.h"

#include <QFile>
#include <QProcess>
#include <QDir>

#include "gui/cMainThread.h"
#include "core/cSummaryFormula.h"


bool checkSeniorRules(vector<int>& combarray, vector<int>& valences, int maxcomponents) {
	int totalvalence = 0;
	int i, size;

	i = 0;
	size = (int)combarray.size();
	while ((i < size) && (combarray[i] > 0)) {
		totalvalence += valences[combarray[i] - 1];
		i++;
	}

	// SENIOR rule 1 - the sum of valences must be even
	// SENIOR rule 3 - the sum of valences >= 2 * (atomscount - maximum number of allowed components in the graph); edges - nodes + components >= 0
	if ((totalvalence % 2 == 1) || (totalvalence < 2 * (i - maxcomponents))) {
		return false;
	}

	return true;
}


bool checkAdvancedFilteringRules(bool noratiocheck, double sumofmasses, vector<int>& countsofelements, vector<string>& namesofelements) {
	int countH = 0;
	int countC = 0;
	int countO = 0;
	int countN = 0;
	int countS = 0;
	int countP = 0;
	int countF = 0;
	int countCl = 0;
	int countBr = 0;
	int countSi = 0;

	double elementsratio;

	int size = (int)countsofelements.size();
	for (int j = 0; j < size; j++) {
		if (countsofelements[j] > 0) {
			if (namesofelements[j].compare("H") == 0) {
				countH = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("C") == 0) {
				countC = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("O") == 0) {
				countO = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("N") == 0) {
				countN = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("S") == 0) {
				countS = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("P") == 0) {
				countP = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("F") == 0) {
				countF = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("Cl") == 0) {
				countCl = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("Br") == 0) {
				countBr = countsofelements[j];
				continue;
			}
			if (namesofelements[j].compare("Si") == 0) {
				countSi = countsofelements[j];
				continue;
			}
		}
	}

	if ((countH == 0) || (countC == 0)) {
		return true;
	}

	if (noratiocheck) {
		if (countN > countO) {
			return true;
		}
	}

	if (sumofmasses < 500.0) {

		if (countC > 39) {
			return true;
		}
		if (countH > 72) {
			return true;
		}
		if (countN > 20) {
			return true;
		}
		if (countO > 20) {
			return true;
		}
		if (countP > 9) {
			return true;
		}
		if (countS > 10) {
			return true;
		}
		if (countF > 16) {
			return true;
		}
		if (countCl > 10) {
			return true;
		}
		if (countBr > 5) {
			return true;
		}
		if (countSi > 8) {
			return true;
		}

	}
	else if (sumofmasses < 1000.0) {

		if (countC > 78) {
			return true;
		}
		if (countH > 126) {
			return true;
		}
		if (countN > 25) {
			return true;
		}
		if (countO > 27) {
			return true;
		}
		if (countP > 9) {
			return true;
		}
		if (countS > 14) {
			return true;
		}
		if (countF > 34) {
			return true;
		}
		if (countCl > 12) {
			return true;
		}
		if (countBr > 8) {
			return true;
		}
		if (countSi > 14) {
			return true;
		}

	}
	else if (sumofmasses < 2000.0) {

		if (countC > 156) {
			return true;
		}
		if (countH > 236) {
			return true;
		}
		if (countN > 32) {
			return true;
		}
		if (countO > 63) {
			return true;
		}
		if (countP > 9) {
			return true;
		}
		if (countS > 14) {
			return true;
		}
		if (countF > 48) {
			return true;
		}
		if (countCl > 12) {
			return true;
		}
		if (countBr > 10) {
			return true;
		}
		if (countSi > 15) {
			return true;
		}

	}
	else if (sumofmasses < 3000.0) {

		if (countC > 162) {
			return true;
		}
		if (countH > 208) {
			return true;
		}
		if (countN > 48) {
			return true;
		}
		if (countO > 78) {
			return true;
		}
		if (countP > 9) {
			return true;
		}
		if (countS > 14) {
			return true;
		}
		if (countF > 48) {
			return true;
		}
		if (countCl > 12) {
			return true;
		}
		if (countBr > 10) {
			return true;
		}
		if (countSi > 15) {
			return true;
		}

	}

	if ((countH > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countH)) / ((double)(countC));
		if ((elementsratio < 0.2) || (elementsratio > 3.1)) {
			return true;
		}
	}

	if ((countN > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countN)) / ((double)(countC));
		if (elementsratio > 1.3) {
			return true;
		}
	}

	if ((countO > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countO)) / ((double)(countC));
		if (elementsratio > 1.2) {
			return true;
		}
	}

	if ((countP > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countP)) / ((double)(countC));
		if (elementsratio > 0.3) {
			return true;
		}
	}

	if ((countS > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countS)) / ((double)(countC));
		if (elementsratio > 0.8) {
			return true;
		}
	}

	if ((countF > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countF)) / ((double)(countC));
		if (elementsratio > 1.5) {
			return true;
		}
	}

	if ((countCl > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countCl)) / ((double)(countC));
		if (elementsratio > 0.8) {
			return true;
		}
	}

	if ((countBr > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countBr)) / ((double)(countC));
		if (elementsratio > 0.8) {
			return true;
		}
	}

	if ((countSi > 0) && (countC >= 0)) {
		if (countC == 0) {
			return true;
		}
		elementsratio = ((double)(countSi)) / ((double)(countC));
		if (elementsratio > 0.5) {
			return true;
		}
	}

	if ((countN > 1) && (countO > 1) && (countP > 1) && (countS > 1)) {
		if ((countN >= 10) || (countO >= 20) || (countP >= 4) || (countS >= 3)) {
			return true;
		}
	}

	if ((countN > 3) && (countO > 3) && (countP > 3)) {
		if ((countN >= 11) || (countO >= 22) || (countP >= 6)) {
			return true;
		}
	}

	if ((countO > 1) && (countP > 1) && (countS > 1)) {
		if ((countO >= 14) || (countP >= 3) || (countS >= 3)) {
			return true;
		}
	}

	if ((countP > 1) && (countS > 1) && (countN > 1)) {
		if ((countP >= 3) || (countS >= 3) || (countN >= 4)) {
			return true;
		}
	}

	if ((countN > 6) && (countO > 6) && (countS > 6)) {
		if ((countN >= 19) || (countO >= 14) || (countS >= 8)) {
			return true;
		}
	}

	return false;
}


void cParameters::fixIntensities(cPeaksList& centroidspectrum, cPeaksList& profilespectrum) {
	if ((centroidspectrum.size() == 0) || (profilespectrum.size() == 0)) {
		return;
	}

	double minval;
	int mem_j = 0;

	for (int i = 0; i < centroidspectrum.size(); i++) {
		minval = fabs(centroidspectrum[i].mzratio - profilespectrum[mem_j].mzratio);
		for (int j = mem_j + 1; j < profilespectrum.size(); j++) {
			if ((fabs(centroidspectrum[i].mzratio - profilespectrum[j].mzratio)) < minval) {
				minval = fabs(centroidspectrum[i].mzratio - profilespectrum[j].mzratio);
				mem_j = j;
			}
			else {
				break;
			}
		}
		centroidspectrum[i].absoluteintensity = profilespectrum[mem_j].absoluteintensity;
	}
}


void cParameters::pushLossItem(vector<neutralLoss>& lossitems, string lossstr) {
	string shortstr;
	size_t pos = lossstr.find(':');
	if (pos == string::npos) {
		shortstr = lossstr;
	}
	else {
		shortstr = lossstr.substr(0, pos);
	}

	cSummaryFormula tmpformula;
	neutralLoss lossitem;
	
	lossitem.summary = lossstr;
	addStringFormulaToMap(shortstr, lossitem.summarymap);
	tmpformula.setFormula(shortstr);
	lossitem.massdifference = tmpformula.getMass();
	lossitems.push_back(lossitem);
}


/*double cParameters::getMassAndCounts(vector<int>& combarray, vector<int>& countsofelements, vector<double>& massesofelements) {
	int i, size;

	i = 0;
	size = (int)countsofelements.size();
	for (i = 0; i < size; i++) {
		countsofelements[i] = 0;
	}

	i = 0;
	size = (int)combarray.size();
	while ((i < size) && (combarray[i] > 0)) {
		countsofelements[combarray[i] - 1]++;
		i++;
	}

	double mass = 0;
	size = (int)countsofelements.size();
	for (i = 0; i < size; i++) {
		if (countsofelements[i] > 0) {
			mass += massesofelements[i] * (double)(countsofelements[i]);
		}
	}

	return mass;
}*/


cParameters::cParameters() {
	clear();
}


void cParameters::clear() {
	string s = "";
	os = 0;
	iondefinitions.recalculateFragments(false, false, s);
	peptidetype = linear;
	peaklistfilenames.clear();
	originalpeaklistfilenames.clear();
	useprofiledata = false;
	convertprofiledata = true;
	profiledatafilename = "";
	linebafprocessing = 0;
	peaklistfileformats.clear();
	peaklistseriesvector.clear();
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
	maximummz = 0;
	minimumrt = 0;
	maximumrt = 0;
	fwhm = 0.05;
	minratio54Fe56Fe = 0.01;
	maxratio54Fe56Fe = 0.1;
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
	mode = dereplication;
	scoretype = number_of_matched_peaks;
	maximumcombinedlosses = 2;
	maximumcombinedelements = 200;
	//clearhitswithoutparent = false;
	basicformulacheck = true;
	advancedformulacheck = true;
	noratiocheck = true;
	calculatefdrs = true;
	minimumannotationintensityrelative = 0;
	minimumannotationintensityabsolute = 0;
	mzdifftolerance = 0;
	intensitytolerance = 0;
	reportunmatchedtheoreticalpeaks = false;
	generateisotopepattern = false;
	minimumpatternsize = 1;
	minimumfeaturesize = 1;
	minimumiontypes = 1;
	cyclicnterminus = false;
	cycliccterminus = false;
	internalfragments = false;
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
	ionsfortheoreticalspectraMS1.clear();
	ionsfortheoreticalspectraMS2.clear();

	cSummaryFormula tmpformula;
	neutralLoss lossitem;

	neutrallossesdefinitions.clear();
	for (int i = 0; i < (int)defaultneutrallosses.getNeutralLosses().size(); i++) {
		pushLossItem(neutrallossesdefinitions, defaultneutrallosses.getNeutralLosses()[i]);
	}
	neutrallossesfortheoreticalspectra.clear();
	numberofgeneratedneutrallosses = 0;

	originalneutrallossesdefinitions = neutrallossesdefinitions;
	originalneutrallossesfortheoreticalspectra = neutrallossesfortheoreticalspectra;

	originalelementsdefinitions.clear();
	originalelementsfortheoreticalspectra.clear();

	pushLossItem(originalelementsdefinitions, "H");
	pushLossItem(originalelementsdefinitions, "C");
	pushLossItem(originalelementsdefinitions, "O");
	pushLossItem(originalelementsdefinitions, "N");
	pushLossItem(originalelementsdefinitions, "S:1");
	pushLossItem(originalelementsdefinitions, "P:1");

	originalelementsfortheoreticalspectra.push_back(0);
	originalelementsfortheoreticalspectra.push_back(1);
	originalelementsfortheoreticalspectra.push_back(2);
	originalelementsfortheoreticalspectra.push_back(3);

	peakidtodesc.clear();
	isotopeformulaidtodesc.clear();
	pchemresults.clear();

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
	string linespectramgfname;

	cPeaksList profilelist;
	ifstream profilestream;
	string profilemgfname;
	string peaksfoldername;

	int hrs, mins, secs;
	QTime time;
	bool good;

	if (peaklistfilenames.empty()) {
		error = true;
		errormessage = "A peaklist must be selected. Have you configured the engine (Search -> Settings...) ?\n";
	}

	peaklistseriesvector.clear();
	peaklistseriesvector.resize(peaklistfilenames.size());
	peaklistfileformats.resize(peaklistfilenames.size());

	// peaklist check
	if (!error) {

		for (int h = 0; h < (int)peaklistfilenames.size(); h++) {

			if (terminatecomputation) {
				error = true;
				errormessage = "Aborted by user.\n";
				break;
			}

			peaklistfileformats[h] = txt;
			
			try {

				rx = "\\.[mM][gG][fF]$";
				// Mascot Generic Format
				if (regex_search(peaklistfilenames[h], rx)) {
					peaklistfileformats[h] = mgf;
				}
			
				rx = "\\.[mM][zZ][mM][lL]$";
				// mzML
				if (regex_search(peaklistfilenames[h], rx)) {
					peaklistfileformats[h] = mzML;
				}
			
				rx = "\\.[mM][zZ][xX][mM][lL]$";
				// mzXML
				if (regex_search(peaklistfilenames[h], rx)) {
					peaklistfileformats[h] = mzXML;
				}
			
				#if OS_TYPE == WIN
					rx = "\\.[bB][aA][fF]$";
					// Bruker Analysis File
					if (regex_search(peaklistfilenames[h], rx)) {
						peaklistfileformats[h] = baf;
					}

					rx = "\\.[rR][aA][wW]$";
					// Thermo RAW file
					if (regex_search(peaklistfilenames[h], rx)) {
						peaklistfileformats[h] = raw;
					}

					rx = "\\.[dD][aA][tT]$";
					// dat file in Waters raw directory
					if (regex_search(peaklistfilenames[h], rx)) {
						peaklistfileformats[h] = dat;
					}

					rx = "\\.[mM][iI][sS]$";
					// flexImaging File
					if (regex_search(peaklistfilenames[h], rx)) {
						peaklistfileformats[h] = mis;
					}

					rx = "ser$";
					// apex File
					if (regex_search(peaklistfilenames[h], rx)) {
						peaklistfileformats[h] = ser;
					}
				#endif

				rx = "\\.[iI][mM][zZ][mM][lL]$";
				// imzML File
				if (regex_search(peaklistfilenames[h], rx)) {
					peaklistfileformats[h] = imzML;
				}

			}
			catch (regex_error& e) {
				error = true;
				errormessage = "cParameters::checkAndPrepare: regex_search failed, error no. " + to_string((int)e.code()) + "\n";
			}

			if (!error) {

				QDir peaksdir;

				switch (peaklistfileformats[h]) {
					case txt:
						peakliststream.open(peaklistfilenames[h]);
						break;
					case mgf:
						peakliststream.open(peaklistfilenames[h]);
						break;
					case mzXML:
						*os << "Converting the file " + peaklistfilenames[h] + " to mzML ... ";
				
						#if OS_TYPE == UNX
							s = installdir.toStdString() + "External/linux/any2mzml.sh " + peaklistfilenames[h];
							if (system(s.c_str()) != 0) {
								error = true;
								errormessage = "The file cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
								errormessage += "Do you have FileConverter installed (OpenMS 2.x must be installed) ?\n";
								errormessage += "Do you have 'any2mzml.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
								errormessage += "Is the file 'any2mzml.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/any2mzml.sh) ? \n";
							}
						#else
							#if OS_TYPE == OSX
								s = installdir.toStdString() + "External/macosx/any2mzml.sh " + peaklistfilenames[h];
								if (system(s.c_str()) != 0) {
									error = true;
									errormessage = "The file cannot be converted.\n";
									errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									errormessage += "Do you have FileConverter installed (OpenMS 2.x must be installed) ?\n";
									errormessage += "Do you have 'any2mzml.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
									errormessage += "Is the file 'any2mzml.sh' executable ? \n";
								}
							#else		
								s = "External\\windows\\any2mzml.bat \"" + peaklistfilenames[h] + "\"";
								if (system(s.c_str()) != 0) {
									error = true;
									errormessage = "The file cannot be converted.\n";
									errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									errormessage += "Do you have FileConverter installed (OpenMS 2.x must be installed) ?\n";
									errormessage += "Do you have a path to FileConverter in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
									errormessage += "Do you have 'any2mzml.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
								}
							#endif
						#endif

						if (!error) {
							*os << "ok" << endl << endl;
							mzmlname = peaklistfilenames[h] + ".mzML";
							peakliststream.open(mzmlname);
						}
						break;
					case baf:
						#if OS_TYPE == WIN
							time.start();

							*os << "Processing the file " + peaklistfilenames[h] + ":" << endl;

							/*
							s = "External\\windows\\baf2csv.bat \"" + peaklistfilenames[h] + "\"";
							if (system(s.c_str()) != 0) {
								error = true;
								errormessage = "The file cannot be converted.\n";
								errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
								errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
								errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
								errormessage += "Do you have 'baf2csv.bat' file located in the 'External/windows' folder ?\n";
							}

							if (!error) {
								*os << "ok" << endl;
								peakliststream.open(peaklistfilenames[h] + ".csv");
							}
							*/

							if (linebafprocessing == 0) {
								mzmlname = peaklistfilenames[h] + ".mzML";
								peakliststream.open(mzmlname);
								good = peakliststream.good();
								peakliststream.close();

								if (good) {
									*os << "The previously converted centroid spectra were found." << endl;
									*os << "The following file was used: " << mzmlname << endl;
								}
								else {
									*os << "centroid spectra ... ";

									//s = "External\\windows\\baf2mzml.bat \"" + peaklistfilenames[h] + "\"";
									//if (system(s.c_str()) != 0) {
									//	error = true;
									//	errormessage = "The file cannot be converted.\n";
									//	errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									//	errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
									//	errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
									//	errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									//	errormessage += "Do you have 'baf2mzml.bat' file located in the 'External/windows' folder ?\n";
									//}

									QString command = "External\\windows\\baf2mzml.bat";
									QString param = peaklistfilenames[h].c_str();
									
									QStringList params;
									params << param;
									
									QProcess convert;
									convert.start(command, params);

									if (!convert.waitForStarted(-1)) {
										error = true;
									}
							
									if (!error) {
										convert.closeWriteChannel();
										if (!convert.waitForFinished(-1)) {
											error = true;
										}
									}

									if (!error) {
										*os << "ok" << endl;
									}
									else {
										errormessage = "The file cannot be converted.\n";
										errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
										errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
										errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
										errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
										errormessage += "Do you have 'baf2mzml.bat' file located in the 'External/windows' folder ?\n";									
									}
								}

								if (!error) {
									peakliststream.open(mzmlname);
								}
							}
							else {
								linespectramgfname = peaklistfilenames[h] + ".mgf";

								peakliststream.open(linespectramgfname, ifstream::binary);
								good = peakliststream.good();

								string endstr;
								if (good) {
									peakliststream.seekg(0, ios::end);
									unsigned long long spos = (unsigned long long)peakliststream.tellg();
									if (spos > 12) {
										spos -= 12;
										peakliststream.seekg(spos, ios::beg);
									}

									char c;
									while (peakliststream.good()) {
										peakliststream.get(c);
										endstr += c;
									}
								}

								peakliststream.close();
								peakliststream.clear();
							
								if (endstr.find("END IONS") != string::npos) {
									*os << "The previously converted centroid spectra were found." << endl;
									*os << "The following file was used: " << linespectramgfname << endl;
								}
								else {
									*os << "centroid spectra ... ";

									if (!QFile(peaklistfilenames[h].c_str()).exists()) {
										error = true;
										errormessage = "The file '" + peaklistfilenames[h] + "' does not exist.\n";
									}
									else {
										//s = "External\\windows\\linespectrumreader.exe \"" + peaklistfilenames[h] + "\"";
										//if (system(s.c_str()) != 0) {
										//	error = true;
										//	errormessage = "The file cannot be converted.\n";
										//	errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
										//	errormessage += "Do you have Bruker Daltonik's CompassXtract installed ?\n";
										//	//errormessage += "Do you have path to the CompassXtract.exe in your PATH variable ?\n";
										//	errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
										//	errormessage += "Do you have 'linespectrumreader.exe' file located in the 'External/windows' folder ?\n";
										//}

										QString command = "External\\windows\\linespectrumreader.exe";
										QString param = peaklistfilenames[h].c_str();

										QStringList params;
										params << param;

										QProcess convert;
										convert.start(command, params);

										if (!convert.waitForStarted(-1)) {
											error = true;
										}

										if (!error) {
											convert.closeWriteChannel();
											if (!convert.waitForFinished(-1)) {
												error = true;
											}
										}

										if (error) {
											errormessage = "The file cannot be converted.\n";
											errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
											errormessage += "Do you have Bruker Daltonik's CompassXtract installed ?\n";
											//errormessage += "Do you have path to the CompassXtract.exe in your PATH variable ?\n";
											errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
											errormessage += "Do you have 'linespectrumreader.exe' file located in the 'External/windows' folder ?\n";
										}
									}

									if (!error) {
										*os << "ok" << endl;
									}
								}

								if (!error) {
									peakliststream.open(linespectramgfname);
								}
							}

							if (!error && useprofiledata && convertprofiledata) {
								*os << "profile spectra ... ";

								s = "External\\windows\\baf2profile.bat \"" + peaklistfilenames[h] + "\"";
								if (system(s.c_str()) != 0) {
									error = true;
									errormessage = "The file cannot be converted.\n";
									errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
									errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
									errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									errormessage += "Do you have 'baf2profile.bat' file located in the 'External/windows' folder ?\n";
								}

								if (!error) {
									*os << "ok" << endl;
								}
							}

							*os << endl;

							secs = time.elapsed() / 1000;
							mins = (secs / 60) % 60;
							hrs = (secs / 3600);
							secs = secs % 60;

							*os << "The data conversion took: " << to_string(hrs) << " hrs, " << to_string(mins) << " min, " << to_string(secs) << " sec." << endl << endl;
						#endif
						break;
					case raw:
						#if OS_TYPE == WIN
							*os << "Converting the file " + peaklistfilenames[h] + " ... ";

							s = "External\\windows\\raw2mzmlpeaks.bat \"" + peaklistfilenames[h] + "\"";
							if (system(s.c_str()) != 0) {
								error = true;
								errormessage = "The file cannot be converted.\n";
								errormessage += "Is the file '" + peaklistfilenames[h] + "' opened elsewhere ?\n";
								errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
								errormessage += "Do you have msconvert.exe installed (OpenMS 2.x including ProteoWizard must be installed) ?\n";
								errormessage += "Do you have a path to msconvert.exe in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/share/OpenMS/THIRDPARTY/pwiz-bin') ?\n";
								errormessage += "Do you have 'raw2mzmlpeaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
							}

							if (!error && useprofiledata) {
								s = "External\\windows\\raw2mzml.bat \"" + peaklistfilenames[h] + "\"";
								if (system(s.c_str()) != 0) {
									error = true;
									errormessage = "The file cannot be converted.\n";
									errormessage += "Is the file '" + peaklistfilenames[h] + "' opened elsewhere ?\n";
									errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									errormessage += "Do you have msconvert.exe installed (OpenMS 2.x including ProteoWizard must be installed) ?\n";
									errormessage += "Do you have a path to msconvert.exe in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/share/OpenMS/THIRDPARTY/pwiz-bin') ?\n";
									errormessage += "Do you have 'raw2mzml.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
								}
							}

							if (!error) {
								*os << "ok" << endl << endl;
								mzmlname = peaklistfilenames[h].substr(0, peaklistfilenames[h].rfind('.')) + "_converted.mzML";
								peakliststream.open(mzmlname);
							}
						#endif
						break;
					case dat:
						#if OS_TYPE == WIN
							foldername = peaklistfilenames[h].substr(0, peaklistfilenames[h].rfind('/'));
							peaksfoldername = foldername.substr(0, foldername.size() - 4) + "_PEAKS.raw";

							peaksdir.setPath(peaksfoldername.c_str());
							if (!peaksdir.exists()) {
								*os << "Generating centroid data folder from " + foldername + " ... ";
								s = "External\\windows\\waters\\profile2peaks.exe \"" + foldername + "\"";
								if (system(s.c_str()) != 0) {
									error = true;
									errormessage = "The raw data folder cannot be converted.\n";
									errormessage += "Does the folder '" + foldername + "' exist ?\n";
									errormessage += "Is the folder with the folder '" + foldername + "' writable ?\n";
									errormessage += "Do you have 'profile2peaks.exe' file located in the 'External/windows/waters' folder ?\n";
								}
								else {
									*os << "ok" << endl << endl;
									*os << "Centroid data folder " + peaksfoldername + " successfully created." << endl << endl;
								}
							}
							else {
								*os << "Previously generated centroid data folder " + peaksfoldername + " has been found." << endl << endl;
							}

							if (!error) {
								string tmpfilename = foldername.substr(0, foldername.rfind('.')) + ".mgf";
								if (!QFile(tmpfilename.c_str()).exists()) {
									*os << "Converting profile data " + foldername + " ... ";
									s = "External\\windows\\waters\\raw2mgf.exe \"" + foldername + "\"";
									if (system(s.c_str()) != 0) {
										error = true;
										errormessage = "The raw data folder cannot be converted.\n";
										errormessage += "Does the folder '" + foldername + "' exist ?\n";
										errormessage += "Is the folder with the folder '" + foldername + "' writable ?\n";
										errormessage += "Do you have 'raw2mgf.exe' file located in the 'External/windows/waters' folder ?\n";
									}
									else {
										*os << "ok" << endl << endl;
									}
								}
								else {
									*os << "Previously converted profile data file " + tmpfilename + " has been found." << endl << endl;
								}
							}

							if (!error) {
								string mgfname = peaksfoldername.substr(0, peaksfoldername.rfind('.')) + ".mgf";
								if (!QFile(mgfname.c_str()).exists()) {
									*os << "Converting centroid data " + peaksfoldername + " ... ";
									s = "External\\windows\\waters\\raw2mgf.exe \"" + peaksfoldername + "\"";
									if (system(s.c_str()) != 0) {
										error = true;
										errormessage = "The raw data folder cannot be converted.\n";
										errormessage += "Does the folder '" + peaksfoldername + "' exist ?\n";
										errormessage += "Is the folder with the folder '" + peaksfoldername + "' writable ?\n";
										errormessage += "Do you have 'raw2mgf.exe' file located in the 'External/windows/waters' folder ?\n";
									}
									else {
										*os << "ok" << endl << endl;
									}
								}
								else {
									*os << "Previously converted centroid data file " + mgfname + " has been found." << endl << endl;
								}
							}

							if (!error) {
								string mgfname = peaksfoldername.substr(0, peaksfoldername.rfind('.')) + ".mgf";
								peakliststream.open(mgfname);
							}
						#endif
						break;
					case mis:
						#if OS_TYPE == WIN
							foldername = peaklistfilenames[h].substr(0, peaklistfilenames[h].rfind('.'));
							*os << "Converting flexImaging data folder " + foldername + " ... ";
							s = "External\\windows\\mis2csv.bat \"" + foldername + "\"";
							if (system(s.c_str()) != 0) {
								error = true;
								errormessage = "The folder cannot be converted.\n";
								errormessage += "Does the folder '" + foldername + "' exist ?\n";
								errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
								errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
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
							foldername = peaklistfilenames[h].substr(0, peaklistfilenames[h].length() - 4);
							*os << "Converting apex data folder " + foldername + " ... ";
							s = "External\\windows\\ser2csv.bat \"" + foldername + "\"";
							if (system(s.c_str()) != 0) {
								error = true;
								errormessage = "The folder cannot be converted.\n";
								errormessage += "Does the folder '" + foldername + "' exist ?\n";
								errormessage += "Do you have Bruker Daltonik's CompassXport installed ?\n";
								errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
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
						peakliststream.open(peaklistfilenames[h]);
						break;
					case imzML:
						ibdfilename = peaklistfilenames[h].substr(0, (int)peaklistfilenames[h].size() - 5);
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
					if ((peaklistfileformats[h] == mis) || (peaklistfileformats[h] == ser) || (peaklistfileformats[h] == dat)) {
						errormessage = "Cannot open the folder '" + foldername + "'.";
					}
					else {
						errormessage = "Cannot open the file '" + peaklistfilenames[h] + "'.";
						if (peaklistfileformats[h] == baf) {
							if (linebafprocessing == 0) {
								errormessage += "\n\nDo you have Bruker Daltonik's CompassXport installed ?\n";
								errormessage += "Do you have path to the CompassXport.exe in your PATH variable ?\n";
							}
							else {
								errormessage += "\n\nDoes the file '" + peaklistfilenames[h] + "' exist ?\n";
								errormessage += "Do you have Bruker Daltonik's CompassXtract installed ?\n";
								//errormessage += "Do you have path to the CompassXtract.exe in your PATH variable ?\n";
								errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
								errormessage += "Do you have 'linespectrumreader.exe' file located in the 'External/windows' folder ?\n";
							}
						}
					}
				}
				else {
					if (os && (peaklistfileformats[h] != mzML) && (peaklistfileformats[h] != mzXML) && (peaklistfileformats[h] != imzML) && (peaklistfileformats[h] != baf) && (peaklistfileformats[h] != raw) && (peaklistfileformats[h] != ser)) {
						*os << "Loading the peaklist(s)... ";
					}
					switch (peaklistfileformats[h]) {
						case txt:
							peaklistseriesvector[h].loadFromPlainTextStream(peakliststream);
							break;
						case mzXML:
							errtype = peaklistseriesvector[h].loadFromMZMLStream(mzmlname, peakliststream, fwhm, mode, os, terminatecomputation);
							if (errtype == -1) {
								error = true;
								errormessage = "Aborted by user.\n";
							}
							if (errtype == -2) {
								error = true;
								#if OS_TYPE == UNX
									errormessage = "Raw data cannot be converted.\n";
									errormessage += "Does the file '" + mzmlname + "' exist ?\n";
									errormessage += "Is the directory with the file '" + mzmlname + "' writable ?\n";
									errormessage += "Do you have enough space on your hard drive ?\n";
									errormessage += "Do you have OpenMS 2.x installed ?\n";
									errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
									errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/raw2peaks.sh) ? \n";
								#else
									#if OS_TYPE == OSX
										errormessage = "Raw data cannot be converted.\n";
										errormessage += "Does the file '" + mzmlname + "' exist ?\n";
										errormessage += "Is the directory with the file '" + mzmlname + "' writable ?\n";
										errormessage += "Do you have enough space on your hard drive ?\n";
										errormessage += "Do you have OpenMS 2.x installed ?\n";
										errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
										errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/macosx/raw2peaks.sh) ? \n";
									#else		
										errormessage = "Raw data cannot be converted.\n";
										errormessage += "Does the file '" + mzmlname + "' exist ?\n";
										errormessage += "Is the directory with the file '" + mzmlname + "' writable ?\n";
										errormessage += "Do you have enough space on your hard drive ?\n";
										errormessage += "Do you have OpenMS 2.x installed ?\n";
										errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
										errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
									#endif
								#endif
							}
							break;
						case mgf:
							peaklistseriesvector[h].loadFromMGFStream(peakliststream, 0);
							break;
						case baf:
							/*
							#if OS_TYPE == WIN
								peaklistseries.loadFromBAFStream(peakliststream);
							#endif
							*/
							#if OS_TYPE == WIN
								if (linebafprocessing == 0) {
									errtype = peaklistseriesvector[h].loadFromMZMLStream(mzmlname, peakliststream, fwhm, mode, os, terminatecomputation);
									if (errtype == -1) {
										error = true;
										errormessage = "Aborted by user.\n";
									}
								}
								else {
									peaklistseriesvector[h].loadFromMGFStream(peakliststream, 1);
								}
							#endif
							break;
						case raw:
							#if OS_TYPE == WIN
								errtype = peaklistseriesvector[h].loadFromMZMLStream(mzmlname, peakliststream, fwhm, mode, os, terminatecomputation);
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
							#endif
							break;
						case dat:
							#if OS_TYPE == WIN
								peaklistseriesvector[h].loadFromMGFStream(peakliststream, 1);

								profilemgfname = foldername.substr(0, foldername.rfind('.')) + ".mgf";
								profilestream.open(profilemgfname);

								for (int i = 0; i < peaklistseriesvector[h].size(); i++) {
									profilelist.clear();
									profilelist.loadFromMGFStream(profilestream, 1);

									if (peaklistseriesvector[h][i].getTitle().compare(profilelist.getTitle()) != 0) {
										error = true;
										errormessage = "The number of spectra in " + foldername + " and " + peaksfoldername + " is different.\n";
										break;
									}

									fixIntensities(peaklistseriesvector[h][i], profilelist);
								}

								profilestream.close();
							#endif
							break;
						case mis:
							#if OS_TYPE == WIN
								peaklistseriesvector[h].loadFromBAFStream(peakliststream);
								peaklistseriesvector[h].loadSpotList(spotliststream);
								spotliststream.close();
							#endif
							break;
						case ser:
							#if OS_TYPE == WIN
								errtype = peaklistseriesvector[h].loadFromProfileApexStream(peaklistfilenames[h], peakliststream, titleliststream, fwhm, os, terminatecomputation);
								titleliststream.close();
								if (errtype == -1) {
									error = true;
									errormessage = "Aborted by user.\n";
								}
								if (errtype == -2) {
									error = true;
									errormessage = "Raw data cannot be converted.\n";
									errormessage += "Does the folder '" + peaklistfilenames[h].substr(0, peaklistfilenames[h].length() - 4) + "' exist ?\n";
									errormessage += "Is the directory '" + peaklistfilenames[h].substr(0, peaklistfilenames[h].length() - 4) + "' writable ?\n";
									errormessage += "Do you have enough space on your hard drive ?\n";
									errormessage += "Do you have OpenMS 2.x installed ?\n";
									errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
									errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
								}
							#endif
							break;
						case mzML:
							errtype = peaklistseriesvector[h].loadFromMZMLStream(peaklistfilenames[h], peakliststream, fwhm, mode, os, terminatecomputation);
							if (errtype == -1) {
								error = true;
								errormessage = "Aborted by user.\n";
							}
							if (errtype == -2) {
								error = true;
								#if OS_TYPE == UNX
									errormessage = "Raw data cannot be converted.\n";
									errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									errormessage += "Do you have enough space on your hard drive ?\n";
									errormessage += "Do you have OpenMS 2.x installed ?\n";
									errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
									errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/raw2peaks.sh) ? \n";
								#else
									#if OS_TYPE == OSX
										errormessage = "Raw data cannot be converted.\n";
										errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
										errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
										errormessage += "Do you have enough space on your hard drive ?\n";
										errormessage += "Do you have OpenMS 2.x installed ?\n";
										errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
										errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/macosx/raw2peaks.sh) ? \n";
									#else		
										errormessage = "Raw data cannot be converted.\n";
										errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
										errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
										errormessage += "Do you have enough space on your hard drive ?\n";
										errormessage += "Do you have OpenMS 2.x installed ?\n";
										errormessage += "Do you have a path to OpenMS binaries folder in your PATH variable (e.g., 'C:/Program Files/OpenMS-2.3.0/bin') ?\n";
										errormessage += "Do you have 'raw2peaks.bat' file located in the '" + appname.toStdString() + "/External/windows' folder ?\n";
									#endif
								#endif
							}
							break;
						case imzML:
							errtype = peaklistseriesvector[h].loadFromIMZMLStream(peaklistfilenames[h], peakliststream, fwhm, defaultmaxx, defaultmaxy, defaultpixelsizex, defaultpixelsizey, vendor, os, terminatecomputation);
							if (errtype == -1) {
								error = true;
								errormessage = "Aborted by user.\n";
							}
							if (errtype == -2) {
								error = true;
								#if OS_TYPE == UNX
									errormessage = "Raw data cannot be converted.\n";
									errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
									errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
									errormessage += "Do you have enough space on your hard drive ?\n";
									errormessage += "Do you have OpenMS 2.x installed ?\n";
									errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/linux' folder ?\n";
									errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/linux/raw2peaks.sh) ? \n";
								#else
									#if OS_TYPE == OSX
										errormessage = "Raw data cannot be converted.\n";
										errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
										errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
										errormessage += "Do you have enough space on your hard drive ?\n";
										errormessage += "Do you have OpenMS 2.x installed ?\n";
										errormessage += "Do you have 'raw2peaks.sh' file located in '" + installdir.toStdString() + "External/macosx' folder ?\n";
										errormessage += "Is the file 'raw2peaks.sh' executable (sudo chmod +x " + installdir.toStdString() + "External/macosx/raw2peaks.sh) ? \n";
									#else		
										errormessage = "Raw data cannot be converted.\n";
										errormessage += "Does the file '" + peaklistfilenames[h] + "' exist ?\n";
										errormessage += "Is the directory with the file '" + peaklistfilenames[h] + "' writable ?\n";
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
					if (os && (peaklistfileformats[h] != mzML) && (peaklistfileformats[h] != mzXML) && (peaklistfileformats[h] != imzML) && (peaklistfileformats[h] != baf) && (peaklistfileformats[h] != raw) && (peaklistfileformats[h] != ser)) {
						*os << "ok" << endl << endl;
					}
				}
				peakliststream.close();

			}

		}

	}


	// bricksdatabase check
	if (!error && ((mode == denovoengine) || ((mode == singlecomparison) && (peptidetype != other)) || ((mode == databasesearch) && (peptidetype != other)))) {

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
	if (!error && ((mode == denovoengine) || (mode == singlecomparison) || ((mode == databasesearch) && (peptidetype != other)))) {

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

		cFragmentIonType fiontype;
		fiontype.nterminal = true;
		fiontype.cterminal = true;
		searchedmodifications.insert(searchedmodifications.begin(), fiontype);

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
			sequencedatabase.loadFromPlainTextStream(sequencedatabasestream, false);
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
	if (!error && ((mode == dereplication) || (mode == compoundsearch))) {
		i = 0;
		while (i < (int)ionsfortheoreticalspectraMS1.size()) {
			if (ionsfortheoreticalspectraMS1[i].positive != (precursorcharge > 0)) {
				ionsfortheoreticalspectraMS1.erase(ionsfortheoreticalspectraMS1.begin() + i);
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


int cParameters::prepareLossesAndCompounds(bool& terminatecomputation) {
	bool error = false;
	string errormessage = "";
	int errtype;


	// calculate combinations of neutral losses or generate compounds
	neutrallossesdefinitions.clear();
	neutrallossesfortheoreticalspectra.clear();
	numberofgeneratedneutrallosses = 0;
	if (!error) {
		if ((mode == denovoengine) || (mode == singlecomparison) || (mode == databasesearch) || (mode == dereplication)) {
			errtype = calculateNeutralLosses(terminatecomputation, errormessage);
			if (errtype == -1) {
				error = true;
			}
		}
		else if (mode == compoundsearch) {
			errtype = generateCompounds(terminatecomputation, errormessage); // uses ionsfortheoreticalspectraMS1
			if (errtype == -1) {
				error = true;
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


bool cParameters::checkModifications(cSequence& sequence, int& startmodifid, int& endmodifid, int& middlemodifid, string& errormessage) {
	startmodifid = 0;
	endmodifid = 0;
	middlemodifid = 0;
	errormessage = "";

	if ((sequence.getPeptideType() == linear) || (sequence.getPeptideType() == branched) || (sequence.getPeptideType() == branchcyclic) || (sequence.getPeptideType() == linearpolyketide)) {

		if ((sequence.getPeptideType() == linear) || (sequence.getPeptideType() == branched) || (sequence.getPeptideType() == linearpolyketide)) {
			startmodifid = -1;
			endmodifid = -1;
		}

		if ((sequence.getPeptideType() == branched) || (sequence.getPeptideType() == branchcyclic)) {
			middlemodifid = -1;
		}

		for (int i = 0; i < (int)searchedmodifications.size(); i++) {

			if ((sequence.getPeptideType() == linear) || (sequence.getPeptideType() == branched) || (sequence.getPeptideType() == linearpolyketide)) {
				if (searchedmodifications[i].name.compare(sequence.getNTterminalModification()) == 0) {
					startmodifid = i;
				}

				if (searchedmodifications[i].name.compare(sequence.getCTterminalModification()) == 0) {
					endmodifid = i;
				}
			}

			if ((sequence.getPeptideType() == branched) || (sequence.getPeptideType() == branchcyclic)) {
				if (searchedmodifications[i].name.compare(sequence.getBranchModification()) == 0) {
					middlemodifid = i;
				}
			}

		}

		if (startmodifid == -1) {
			errormessage = "The N-terminal modification in the sequence " + sequence.getSequence() + " is not defined: " + sequence.getNTterminalModification();
			return false;
		}

		if (endmodifid == -1) {
			errormessage = "The C-terminal modification in the sequence " + sequence.getSequence() + " is not defined: " + sequence.getCTterminalModification();
			return false;
		}

		if (middlemodifid == -1) {
			errormessage = "The branch modification in the sequence " + sequence.getSequence() + " is not defined: " + sequence.getBranchModification();
			return false;
		}

	}

	return true;
}


string cParameters::printToString() {
	string s = "";

	s += "Mode: ";
	switch ((eModeType)mode) {
		case denovoengine:
			s += "De Novo Search Engine - MS/MS";
			break;
		case singlecomparison:
			s += "Compare Peaklist(s) with Spectrum of Searched Sequence - MS/MS";
			break;
		case databasesearch:
			s += "Compare Peaklist with Database - MS/MS";
			break;
		case dereplication:
			s += "Compare Peaklist(s) with Database - MS, LC-MS, MSI";
			break;
		case compoundsearch:
			s += "Compound Search - MS, LC-MS, MSI";
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

	s += "File(s): \n";
	for (int i = 0; i < (int)peaklistfilenames.size(); i++) {
		s += peaklistfilenames[i] + "\n";
	}

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
	s += "Maximum m/z Ratio: " + to_string(maximummz) + "\n";
	s += "Minimum Retention Time: " + to_string(minimumrt) + "\n";
	s += "Maximum Retention Time: " + to_string(maximumrt) + "\n";
	s += "FWHM: " + to_string(fwhm) + "\n";
	s += "Minimum Ratio 54Fe/56Fe: " + to_string(minratio54Fe56Fe) + "\n";
	s += "Maximum Ratio 54Fe/56Fe: " + to_string(maxratio54Fe56Fe) + "\n";
	s += "Building Blocks Database File: " + bricksdatabasefilename + "\n";
	s += "Maximum Number of Combined Blocks (start, middle, end): " + to_string(maximumbricksincombinationbegin) + ", " + to_string(maximumbricksincombinationmiddle) + ", " + to_string(maximumbricksincombinationend) + "\n";

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

	s += "Maximum Cumulative Mass of Blocks: " + to_string(maximumcumulativemass) + "\n";
	s += "N-/C-terminal Modifications File: " + modificationsfilename + "\n";

	s += "Disable Precursor Mass Filter: ";
	s += similaritysearch ? "on" : "off";
	s += "\n";

	s += "Internal Fragments: ";
	s += internalfragments ? "on" : "off";
	s += "\n";

	s += "Enable Scrambling: ";
	s += enablescrambling ? "on" : "off";
	s += "\n";

	s += "Cyclic N-terminus: ";
	s += cyclicnterminus ? "on" : "off";
	s += "\n";

	s += "Cyclic C-terminus: ";
	s += cycliccterminus ? "on" : "off";
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
		case weighted_ratio_of_matched_peaks:
			s += "Weighted Ratio of Matched Peaks";
			break;
		default:
			s += "undefined";
			break;
	}
	s += "\n";

	s += "Maximum Number of Reported Sequence Candidates: " + to_string(hitsreported) + "\n";
	s += "Peptide Sequence Tag: " + originalsequencetag + "\n";

	s += "Ion Types: ";
	if ((mode == dereplication) || (mode == compoundsearch)) {
		for (int i = 0; i < (int)ionsfortheoreticalspectraMS1.size(); i++) {
			s += ionsfortheoreticalspectraMS1[i].name;
			if (i < (int)ionsfortheoreticalspectraMS1.size() - 1) {
				s += ", ";
			}
		}
	}
	else {
		for (int i = 0; i < (int)ionsfortheoreticalspectraMS2.size(); i++) {
			s += iondefinitions[ionsfortheoreticalspectraMS2[i]].name;
			if (i < (int)ionsfortheoreticalspectraMS2.size() - 1) {
				s += ", ";
			}
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

	s += "Maximum Number of Combined Losses: " + to_string(maximumcombinedlosses) + "\n";

	s += "Chemical Elements: ";
	for (int i = 0; i < (int)originalelementsfortheoreticalspectra.size(); i++) {
		s += originalelementsdefinitions[originalelementsfortheoreticalspectra[i]].summary;
		if (i < (int)originalelementsfortheoreticalspectra.size() - 1) {
			s += ", ";
		}
	}
	s += "\n";

	s += "Maximum Number of Combined Elements: " + to_string(maximumcombinedelements) + "\n";

	//s += "Remove Hits of Fragments without Hits of Parent Fragments: ";
	//s += clearhitswithoutparent ? "on" : "off";
	//s += "\n";

	s += "Report Unmatched Theoretical Peaks: ";
	s += reportunmatchedtheoreticalpeaks ? "on" : "off";
	s += "\n";

	s += "Generate Full Isotope Patterns: ";
	s += generateisotopepattern ? "on" : "off";
	s += "\n";
	
	s += "Minimum Number of Isotopic Peaks: " + to_string(minimumpatternsize) + "\n";

	s += "Minimum Number of Spectra: " + to_string(minimumfeaturesize) + "\n";

	s += "Minimum Number of Ion Types: " + to_string(minimumiontypes) + "\n";

	s += "Basic Formula Check: ";
	s += basicformulacheck ? "on" : "off";
	s += "\n";

	s += "Advanced Formula Check: ";
	s += advancedformulacheck ? "on" : "off";
	s += "\n";

	s += "N/O Ratio Check: ";
	s += noratiocheck ? "on" : "off";
	s += "\n";

	s += "Calculate FDRs: ";
	s += calculatefdrs ? "on" : "off";
	s += "\n";

	s += "Minimum Relative Intensity of Highest Peak in Isotopic Cluster: " + to_string(minimumannotationintensityrelative) + "\n";
	s += "Minimum Absolute Intensity of Highest Peak in Isotopic Cluster: " + to_string(minimumannotationintensityabsolute) + "\n";

	s += "Isotope m/z Tolerance: " + to_string(mzdifftolerance) + "\n";

	s += "Isotope Intensity Tolerance: " + to_string(intensitytolerance) + "\n";

	s += "Searched Sequence: " + originalsearchedsequence + "\n";
	s += "N-terminal Modification: " + searchedsequenceNtermmodif + "\n";
	s += "C-terminal Modification: " + searchedsequenceCtermmodif + "\n";
	s += "Branch Modification: " + searchedsequenceTmodif + "\n";
	s += "Formula: " + searchedsequenceformula + "\n";

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


int cParameters::calculateNeutralLosses(bool& terminatecomputation, string& errormessage) {
	neutrallossesdefinitions.clear();
	neutrallossesfortheoreticalspectra.clear();
	numberofgeneratedneutrallosses = 0;
	errormessage = "";

	if (maximumcombinedlosses == 0) {
		return 0;
	}

	cSummaryFormula tmpformula;
	string tmpstring;
	neutralLoss loss;

	cBricksDatabase neutrallossesbrickdatabase;
	cBrick tmpbrick;
	string tmpstr;
	vector<int> valences;
	bool validvalences = false;

	vector<int> limitsofelements;
	vector<int> countsofelements;
	vector<double> massesofelements;

	int numberofbasicbricks = 0;

	double sumofmasses;

	int compressionlimit = 100000;
	bool compressformulas;

	//int stringsizeest = 0;
	//int mapsizeest = 0;
	//int doublesizeest = 0;
	//int pruned = 0;

	if (os) {
		*os << "Calculating combinations of neutral losses... " << endl;
	}

	for (int i = 0; i < (int)originalneutrallossesfortheoreticalspectra.size(); i++) {
		tmpstr = originalneutrallossesdefinitions[originalneutrallossesfortheoreticalspectra[i]].summary;

		if (tmpstr.rfind(':') != string::npos) {
			tmpstr = tmpstr.substr(0, tmpstr.rfind(':'));
		}

		tmpformula.setFormula(tmpstr, false);

		tmpbrick.clear();
		tmpbrick.setName(originalneutrallossesdefinitions[originalneutrallossesfortheoreticalspectra[i]].summary);
		tmpbrick.setComposition(to_string(numberofbasicbricks + 1), false);
		tmpbrick.setMass(tmpformula.getMass());
		tmpbrick.setSummary(tmpstr);
		tmpbrick.createSummaryMap();

		neutrallossesbrickdatabase.push_back(tmpbrick);

		numberofbasicbricks++;
	}

	neutrallossesbrickdatabase.sortbyMass();

	for (int i = 0; i < neutrallossesbrickdatabase.size(); i++) {
		countsofelements.push_back(0);
		massesofelements.push_back(neutrallossesbrickdatabase[i].getMass());

		tmpstr = neutrallossesbrickdatabase[i].getName();
		if (tmpstr.rfind(':') != string::npos) {
			tmpstr = tmpstr.substr(tmpstr.rfind(':') + 1);
			limitsofelements.push_back(QVariant(tmpstr.c_str()).toInt());
		}
		else {
			limitsofelements.push_back(0);
		}

		tmpstr = neutrallossesbrickdatabase[i].getSummary();

		if (tmpstr.compare("H") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("C") == 0) {
			valences.push_back(4);
		}

		if (tmpstr.compare("O") == 0) {
			valences.push_back(2);
		}

		if (tmpstr.compare("N") == 0) {
			valences.push_back(3);
		}

		if (tmpstr.compare("S") == 0) {
			valences.push_back(6);
		}

		if (tmpstr.compare("P") == 0) {
			valences.push_back(5);
		}

		if (tmpstr.compare("Li") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Na") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("K") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("F") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Cl") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Br") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("I") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Si") == 0) {
			valences.push_back(4);
		}
	}

	if (basicformulacheck && (neutrallossesbrickdatabase.size() > 0) && (neutrallossesbrickdatabase.size() == valences.size())) {
		validvalences = true;

		if (os) {
			*os << "Filtering using Senior's rules has been enabled." << endl;
		}
	}

	vector<int> combarray;
	for (int i = 0; i < maximumcombinedlosses; i++) {
		combarray.push_back(0);
	}

	map<string, int> atoms;
	vector<int> max_counts;
	bool skipcombination;
	bool bruteforce = false;

	if ((mode == singlecomparison) || (mode == databasesearch)) {

		if ((peptidetype == other) && (neutrallossesbrickdatabase.size() > 0)) {

			bruteforce = true;
			for (int i = 0; i < neutrallossesbrickdatabase.size(); i++) {
				if (neutrallossesbrickdatabase[i].getSummaryMap().size() == 1) {
					auto it = neutrallossesbrickdatabase[i].getSummaryMap().begin();
					if (it->second != 1) {
						bruteforce = false;
						break;
					}
				}
				else {
					bruteforce = false;
					break;
				}
			}

		}

	}

	if (mode == singlecomparison) {
			
		// calculate molecular formula of searched sequence
		if (bruteforce) {

			*os << "Brute force fragmentation enabled. Maximum numbers of elements: " << endl;

			string formulastr;
			if (peptidetype == other) {
				formulastr = searchedsequenceformula;
			}
			else {
				string upperboundsequence = searchedsequence;

				//if (!checkRegex(peptidetype, upperboundsequence, errormessage)) {
				//	return -1;
				//}

				//if (!bricksdatabase.replaceAcronymsByIDs(upperboundsequence, errormessage)) {
				//	return -1;
				//}

				cSequence tmpsequence;
				tmpsequence.setNTterminalModification(searchedsequenceNtermmodif);
				tmpsequence.setCTterminalModification(searchedsequenceCtermmodif);
				tmpsequence.setBranchModification(searchedsequenceTmodif);
				tmpsequence.setPeptideType(peptidetype);

				int startmodifid, endmodifid, middlemodifid;
				if (!checkModifications(tmpsequence, startmodifid, endmodifid, middlemodifid, errormessage)) {
					return -1;
				}

				int branchstart, branchend;
				vector<string> v;
				cCandidate c;
				vector<nodeEdge> netmp;

				parseBranch(peptidetype, upperboundsequence, v, branchstart, branchend);
				// startmodifid, endmodifid and middlemodifid were filled up by checkModifications
				c.setCandidate(v, netmp, fragmentIonTypeEnd, startmodifid, endmodifid, middlemodifid, branchstart, branchend);
				cSummaryFormula formula = c.calculateSummaryFormulaFromBricks(*this, peptidetype, precursormass);

				formulastr = formula.getSummary();
			}

			addStringFormulaToMap(formulastr, atoms);

			for (int i = 0; i < neutrallossesbrickdatabase.size(); i++) {
				tmpstr = neutrallossesbrickdatabase[i].getSummary();
				if (atoms.count(tmpstr) == 1) {
					max_counts.push_back(atoms[tmpstr]);
				}
				else {
					max_counts.push_back(0);
				}
				*os << tmpstr << ": " << max_counts[i] << endl;
			}

		}

	}

	if ((mode == databasesearch) && (peptidetype == other)) {

		if (bruteforce) {

			*os << "Brute force fragmentation enabled. Maximum numbers of elements: " << endl;

			for (int j = 0; j < neutrallossesbrickdatabase.size(); j++) {
				max_counts.push_back(0);
			}
		
			int i = 0;
			while (i < sequencedatabase.size()) {
				tmpformula.setFormula(sequencedatabase[i].getSummaryFormula());
				tmpstr = "H+";
				tmpformula.addFormula(tmpstr);
				tmpstr = (precursorcharge > 0) ? "" : "H-2+-2";
				tmpformula.addFormula(tmpstr);
				tmpstr = precursoradduct.empty() ? "" : "H-1";
				tmpformula.addFormula(tmpstr);
				tmpstr = precursoradduct;
				tmpformula.addFormula(tmpstr);

				if (similaritysearch || isInPpmMassErrorTolerance(charge(uncharge(precursormass, precursorcharge), (precursorcharge > 0) ? 1 : -1), tmpformula.getMass(), precursormasserrortolerance)) {
					atoms.clear();
					tmpstr = tmpformula.getSummary();
					tmpstr += (precursorcharge > 0) ? "H-1+-1" : "H+";
					addStringFormulaToMap(tmpstr, atoms);

					for (int j = 0; j < neutrallossesbrickdatabase.size(); j++) {
						tmpstr = neutrallossesbrickdatabase[j].getSummary();
						if ((atoms.count(tmpstr) == 1) && (atoms[tmpstr] > max_counts[j])) {
							max_counts[j] = atoms[tmpstr];
						}
					}

					i++;
				}
				else {
					sequencedatabase.erase(i);
				}
			}

			for (int j = 0; j < neutrallossesbrickdatabase.size(); j++) {
				tmpstr = neutrallossesbrickdatabase[j].getSummary();
				*os << tmpstr << ": " << max_counts[j] << endl;
			}
	
		}
	
	}

	// to do - change to unsigned long long
	int ii = 0;
	compressformulas = false;

	if (bruteforce && (peptidetype == other) && ((mode == databasesearch) || (mode == singlecomparison))) {
		compressionlimit = 0;
		compressformulas = true;
	}

	double crop = uncharge(precursormass, precursorcharge) - minimummz;
	if (mode == dereplication) {
		crop = 0;
	}

	//while (neutrallossesbrickdatabase.nextCombination(combarray, numberofbasicbricks, maximumcombinedlosses, 0, uncharge(precursormass, precursorcharge) - minimummz)) {
	while (neutrallossesbrickdatabase.nextCombinationFastLimited(combarray, countsofelements, limitsofelements, massesofelements, sumofmasses, numberofbasicbricks, maximumcombinedlosses, 0, crop)) {
		if (terminatecomputation) {
			neutrallossesdefinitions.clear();
			neutrallossesfortheoreticalspectra.clear();
			numberofgeneratedneutrallosses = 0;
			errormessage = "Aborted by user.";
			return -1;
		}

		if (bruteforce) {
			skipcombination = false;
			for (int j = 0; j < numberofbasicbricks; j++) {
				if (countsofelements[j] > max_counts[j]) {
					skipcombination = true;
					break;
				}
			}

			if (skipcombination) {
				continue;
			}
		}

		if (validvalences) {
			if (!checkSeniorRules(combarray, valences, 10)) {
				continue;
			}
		}

		loss.clear();
		loss.massdifference = -sumofmasses;

		for (int j = 0; j < (int)combarray.size(); j++) {
			if ((combarray[j] > 0) && (combarray[j] <= numberofbasicbricks)) {
				loss.summary += neutrallossesbrickdatabase[combarray[j] - 1].getSummary();
			}
			else {
				break;
			}
		}

		if (compressformulas) {
			tmpformula.clear();
			tmpformula.addFormula(loss.summary);
			loss.summary = tmpformula.getSummary();
		}

		tmpformula.clear();
		tmpformula.addFormula(loss.summary, true);
		tmpstring = tmpformula.getSummary();
		addStringFormulaToMap(tmpstring, loss.summarymap);

		//stringsizeest += sizeof(string) + loss.summary.size();
		//mapsizeest += sizeof(loss.summarymap) + loss.summarymap.size() * (sizeof(string) /* the length of string is 1 for HCONS */ + sizeof(int));
		//doublesizeest += sizeof(double);

		neutrallossesdefinitions.push_back(loss);
		neutrallossesfortheoreticalspectra.push_back(ii);

		ii++;

		if (ii == compressionlimit) {
			compressformulas = true;
		}

		if (ii % 100000 == 0) {
			if (os) {
				*os << ii << " ";
			}

			//cout << ii << " " << pruned << " - " << stringsizeest << " " << mapsizeest << " " << doublesizeest << " " << stringsizeest + mapsizeest + doublesizeest << endl;
		}

		if (ii % 1000000 == 0) {
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
				errormessage = "Aborted by user.";
				return -1;
			}
		}
	}

	return 0;
}


int cParameters::generateCompounds(bool& terminatecomputation, string& errormessage) {
	sequencedatabase.clear();
	unsigned long long compoundsgenerated = 0;
	unsigned long long compoundsused = 0;
	unsigned long long compoundslimit = 5000000;

	errormessage = "";

	if (maximumcombinedelements == 0) {
		return 0;
	}

	cSequence seq;
	cSummaryFormula tmpformula;
	int size;

	cBricksDatabase elementsbrickdatabase;
	cBrick tmpbrick;
	string tmpstr, tmpstr2;
	vector<int> valences;
	bool validvalences = false;

	vector<int> limitsofelements;
	vector<int> countsofelements;
	vector<double> massesofelements;
	vector<string> namesofelements;

	int numberofbasicbricks = 0;

	double sumofmasses;
	double tmpmzdifference;
	bool alloutofmz;
	int featureshint;
	int compoundshint;
	bool hintend;

	double minadd = 0;
	//double maxadd = 0;

	bool lcms;
	bool rtavailable;

	int kstart;
	int kend;

	if (os) {
		*os << "Generating compounds... " << endl;
	}

	for (int i = 0; i < (int)originalelementsfortheoreticalspectra.size(); i++) {
		tmpstr = originalelementsdefinitions[originalelementsfortheoreticalspectra[i]].summary;

		if (tmpstr.rfind(':') != string::npos) {
			tmpstr = tmpstr.substr(0, tmpstr.rfind(':'));
		}

		tmpformula.setFormula(tmpstr, false);

		tmpbrick.clear();
		tmpbrick.setName(originalelementsdefinitions[originalelementsfortheoreticalspectra[i]].summary);
		tmpbrick.setComposition(to_string(numberofbasicbricks + 1), false);
		tmpbrick.setMass(tmpformula.getMass());
		tmpbrick.setSummary(tmpstr);
		tmpbrick.createSummaryMap();

		if ((tmpbrick.getSummaryMap().size() != 1)) {
			errormessage = "Bad input element: " + tmpstr + ". Only single elements can be used in this mode e.g. H, C, O, N, P, S.";
			return -1;
		}
		else {
			auto it = tmpbrick.getSummaryMap().begin();
			if (it->second != 1) {
				errormessage = "Bad input element: " + tmpstr + ". Only single elements can be used in this mode e.g. H, C, O, N, P, S.";
				return -1;
			}
		}

		elementsbrickdatabase.push_back(tmpbrick);

		numberofbasicbricks++;
	}

	elementsbrickdatabase.sortbyMass();

	int carbonpos = -1;
	//bool enablelimitfilter = false;
	for (int i = 0; i < elementsbrickdatabase.size(); i++) {
		countsofelements.push_back(0);
		massesofelements.push_back(elementsbrickdatabase[i].getMass());

		tmpstr = elementsbrickdatabase[i].getName();
		if (tmpstr.rfind(':') != string::npos) {
			tmpstr = tmpstr.substr(tmpstr.rfind(':') + 1);
			limitsofelements.push_back(QVariant(tmpstr.c_str()).toInt());
			//enablelimitfilter = true;
		}
		else {
			limitsofelements.push_back(0);
		}

		tmpstr = elementsbrickdatabase[i].getSummary();
		namesofelements.push_back(tmpstr);

		if (tmpstr.compare("C") == 0) {
			carbonpos = i;
		}

		if (tmpstr.compare("H") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("C") == 0) {
			valences.push_back(4);
		}

		if (tmpstr.compare("O") == 0) {
			valences.push_back(2);
		}

		if (tmpstr.compare("N") == 0) {
			valences.push_back(3);
		}

		if (tmpstr.compare("S") == 0) {
			valences.push_back(6);
		}

		if (tmpstr.compare("P") == 0) {
			valences.push_back(5);
		}

		if (tmpstr.compare("Li") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Na") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("K") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("F") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Cl") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Br") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("I") == 0) {
			valences.push_back(1);
		}

		if (tmpstr.compare("Si") == 0) {
			valences.push_back(4);
		}
	}

	if (basicformulacheck && (elementsbrickdatabase.size() > 0) && (elementsbrickdatabase.size() == valences.size())) {
		validvalences = true;
	}

	vector<int> combarray;
	for (int i = 0; i < maximumcombinedelements; i++) {
		combarray.push_back(0);
	}

	if (ionsfortheoreticalspectraMS1.size() > 0) {
		minadd = ionsfortheoreticalspectraMS1[0].massdifference;
		//maxadd = ionsfortheoreticalspectraMS1[0].massdifference;
		for (int i = 1; i < (int)ionsfortheoreticalspectraMS1.size(); i++) {
			if (ionsfortheoreticalspectraMS1[i].massdifference < minadd) {
				minadd = ionsfortheoreticalspectraMS1[i].massdifference;
			}
			//if (ionsfortheoreticalspectraMS1[i].massdifference > maxadd) {
			//	maxadd = ionsfortheoreticalspectraMS1[i].massdifference;
			//}
		}
	}

	double unchargedmaximummz = charge(uncharge(maximummz, precursorcharge), (precursorcharge > 0) ? 1 : -1) - minadd;
	//double unchargedmaximummz = maximummz - minadd;

	//bool skipcombination;
	//while (elementsbrickdatabase.nextCombinationFast(combarray, countsofelements, massesofelements, sumofmasses, numberofbasicbricks, maximumcombinedelements, 0, maximummz)) {
	while (elementsbrickdatabase.nextCombinationFastLimited(combarray, countsofelements, limitsofelements, massesofelements, sumofmasses, numberofbasicbricks, maximumcombinedelements, 0, unchargedmaximummz)) {
		if (terminatecomputation) {
			sequencedatabase.clear();
			errormessage = "Aborted by user.";
			return -1;
		}

		/*if (enablelimitfilter) {
			skipcombination = false;
			size = (int)countsofelements.size();
			for (int j = 0; j < size; j++) {
				if ((limitsofelements[j] > 0) && (countsofelements[j] > limitsofelements[j])) {
					skipcombination = true;
					break;
				}
			}

			if (skipcombination) {
				continue;
			}
		}*/

		if (validvalences) {
			if (!checkSeniorRules(combarray, valences, 1)) {
				continue;
			}
		}

		//sumofmasses = getMassAndCounts(combarray, countsofelements, massesofelements);

		alloutofmz = true;
		for (auto& it : ionsfortheoreticalspectraMS1) {
			for (int j = 0; j < abs(precursorcharge); j++) {
				tmpmzdifference = sumofmasses + it.massdifference;
				if (precursorcharge > 0) {
					tmpmzdifference += j * (H - e);
				}
				else {
					tmpmzdifference -= j * (H - e);
				}
				if (j > 0) {
					tmpmzdifference /= (double)(j + 1);
				}
				if ((tmpmzdifference >= minimummz) && (tmpmzdifference <= maximummz)) {
					alloutofmz = false;
				}
			}
		}

		if (alloutofmz) {
			continue;
		}

		if (advancedformulacheck) {
			if (checkAdvancedFilteringRules(noratiocheck, sumofmasses, countsofelements, namesofelements)) {
				continue;
			}
		}

		compoundsgenerated++;

		if (!reportunmatchedtheoreticalpeaks) {

			for (int h = 0; h < (int)peaklistseriesvector.size(); h++) {

				lcms = (peaklistseriesvector[h].size() > 1) && !((peaklistfileformats[h] == mis) || (peaklistfileformats[h] == imzML));
				rtavailable = (peaklistseriesvector[h].size() > 1) && (peaklistseriesvector[h][peaklistseriesvector[h].size() - 1].getRetentionTime() > 0);

				compoundshint = 0;

				size = peaklistseriesvector[h].size();

				kstart = 0;
				kend = size;

				if (lcms && rtavailable) {

					if (minimumrt != 0) {
						while (kstart < size) {
							if (peaklistseriesvector[h][kstart].getRetentionTime() >= minimumrt) {
								break;
							}
							kstart++;
						}

						if (kstart == size) {
							kstart--;
						}
					}

					if (maximumrt != 0) {
						while (kend >= 0) {
							if (peaklistseriesvector[h][kend].getRetentionTime() <= maximumrt) {
								break;
							}
							kend--;
						}

						if (kend == -1) {
							kend++;
						}
					}

				}

				for (auto& it : ionsfortheoreticalspectraMS1) {

					for (int j = 0; j < abs(precursorcharge); j++) {

						tmpmzdifference = sumofmasses + it.massdifference;
						if (precursorcharge > 0) {
							tmpmzdifference += j * (H - e);
						}
						else {
							tmpmzdifference -= j * (H - e);
						}
						if (j > 0) {
							tmpmzdifference /= (double)(j + 1);
						}

						featureshint = 0;

						for (int k = kstart; k < kend; k++) {

							if (searchHint(tmpmzdifference, peaklistseriesvector[h][k], fragmentmasserrortolerance)) {
								featureshint++;
							}
							else {
								if (lcms) {
									featureshint = 0;
								}
							}

							hintend = false;
							if (lcms || (peaklistfileformats[h] == imzML) || (peaklistfileformats[h] == mis)) {
								if (featureshint >= minimumfeaturesize) {
									compoundshint++;
									hintend = true;
								}
							}
							else {
								if (featureshint > 0) {
									compoundshint++;
									hintend = true;
								}
							}

							if (hintend) {
								break;
							}

						}

						if (compoundshint >= minimumiontypes) {
							break;
						}

					}

					if (compoundshint >= minimumiontypes) {
						break;
					}

				}

				if (compoundshint >= minimumiontypes) {
					break;
				}

			}

			if (compoundshint < minimumiontypes) {
				continue;
			}

		}

		tmpstr.clear();
		tmpstr2.clear();
		size = (int)countsofelements.size();
		for (int j = 0; j < size; j++) {
			if (countsofelements[j] > 0) {
				if (j == carbonpos) {
					tmpstr2 = namesofelements[j] + to_string(countsofelements[j]);
				}
				else {
					tmpstr += namesofelements[j] + to_string(countsofelements[j]);
				}
			}
		}
		tmpstr = tmpstr2 + tmpstr;

		seq.setName(tmpstr);
		seq.setSummaryFormula(tmpstr);
		if (compoundsused <= compoundslimit) {
			sequencedatabase.push_back(seq);
		}
		compoundsused++;

		if (compoundsused % 100000 == 0) {
			if (os) {
				*os << compoundsused << " ";
			}
		}

		if (compoundsused % 1000000 == 0) {
			if (os) {
				*os << endl;
			}
		}
	}

	if (os) {
		*os << "ok" << endl;
		*os << "Number of generated compounds: " << compoundsgenerated << endl;
		*os << "Number of used compounds: " << compoundsused << endl << endl;
	}

	return 0;
}


void cParameters::store(ofstream& os) {
	int size;
	string s;
	
	iondefinitions.store(os);

	os.write((char *)&peptidetype, sizeof(ePeptideType));

	size = (int)peaklistfilenames.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < size; i++) {
		storeString(peaklistfilenames[i], os);
	}

	storeString(originalpeaklistfilenames, os);

	size = (int)peaklistfileformats.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < size; i++) {
		os.write((char *)&peaklistfileformats[i], sizeof(ePeakListFileFormat));
	}

	size = (int)peaklistseriesvector.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < size; i++) {
		peaklistseriesvector[i].store(os);
	}

	os.write((char *)&useprofiledata, sizeof(bool));
	os.write((char *)&convertprofiledata, sizeof(bool));

	storeString(profiledatafilename, os);

	os.write((char *)&linebafprocessing, sizeof(int));

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
	os.write((char *)&maximummz, sizeof(double));
	os.write((char *)&minimumrt, sizeof(double));
	os.write((char *)&maximumrt, sizeof(double));
	os.write((char *)&fwhm, sizeof(double));

	os.write((char *)&minratio54Fe56Fe, sizeof(double));
	os.write((char *)&maxratio54Fe56Fe, sizeof(double));

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
	os.write((char *)&maximumcombinedelements, sizeof(int));
	//os.write((char *)&clearhitswithoutparent, sizeof(bool));
	os.write((char *)&reportunmatchedtheoreticalpeaks, sizeof(bool));
	os.write((char *)&generateisotopepattern, sizeof(bool));
	os.write((char *)&minimumpatternsize, sizeof(int));
	os.write((char *)&minimumfeaturesize, sizeof(int));
	os.write((char *)&minimumiontypes, sizeof(int));
	os.write((char *)&basicformulacheck, sizeof(bool));
	os.write((char *)&advancedformulacheck, sizeof(bool));
	os.write((char *)&noratiocheck, sizeof(bool));
	os.write((char *)&calculatefdrs, sizeof(bool));
	os.write((char *)&minimumannotationintensityrelative, sizeof(double));
	os.write((char *)&minimumannotationintensityabsolute, sizeof(unsigned));
	os.write((char *)&mzdifftolerance, sizeof(double));
	os.write((char *)&intensitytolerance, sizeof(double));
	os.write((char *)&cyclicnterminus, sizeof(bool));
	os.write((char *)&cycliccterminus, sizeof(bool));
	os.write((char *)&internalfragments, sizeof(bool));
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
	storeString(searchedsequenceformula, os);

	os.write((char *)&maximumnumberofcandidates, sizeof(int));
	os.write((char *)&blindedges, sizeof(int));

	storeString(sequencedatabasefilename, os);
	sequencedatabase.store(os);

	size = (int)fragmentionsfordenovograph.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)fragmentionsfordenovograph.size(); i++) {
		os.write((char *)&fragmentionsfordenovograph[i], sizeof(eFragmentIonType));
	}

	size = (int)ionsfortheoreticalspectraMS1.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)ionsfortheoreticalspectraMS1.size(); i++) {
		ionsfortheoreticalspectraMS1[i].store(os);
	}

	size = (int)ionsfortheoreticalspectraMS2.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)ionsfortheoreticalspectraMS2.size(); i++) {
		os.write((char *)&ionsfortheoreticalspectraMS2[i], sizeof(eFragmentIonType));
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

	size = (int)originalelementsdefinitions.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)originalelementsdefinitions.size(); i++) {
		originalelementsdefinitions[i].store(os);
	}

	size = (int)originalelementsfortheoreticalspectra.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)originalelementsfortheoreticalspectra.size(); i++) {
		os.write((char *)&originalelementsfortheoreticalspectra[i], sizeof(int));
	}

	storeStringVector(peakidtodesc, os);
	storeStringVector(isotopeformulaidtodesc, os);
	storeStringIntMap(pchemresults, os);

	os.write((char *)&defaultmaxx, sizeof(int));
	os.write((char *)&defaultmaxy, sizeof(int));
	os.write((char *)&defaultpixelsizex, sizeof(int));
	os.write((char *)&defaultpixelsizey, sizeof(int));
	os.write((char *)&vendor, sizeof(eVendorType));
}


void cParameters::load(ifstream& is, int fileversionpart1, int fileversionpart2, int fileversionpart3) {
	int size;
	string s;

	os = 0;

	iondefinitions.load(is);

	is.read((char *)&peptidetype, sizeof(ePeptideType));

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 0)) {
		is.read((char *)&size, sizeof(int));
		peaklistfilenames.resize(size);
		for (int i = 0; i < size; i++) {
			loadString(peaklistfilenames[i], is);
		}

		loadString(originalpeaklistfilenames, is);

		is.read((char *)&size, sizeof(int));
		peaklistfileformats.resize(size);
		for (int i = 0; i < size; i++) {
			is.read((char *)&peaklistfileformats[i], sizeof(ePeakListFileFormat));
		}

		is.read((char *)&size, sizeof(int));
		peaklistseriesvector.resize(size);
		for (int i = 0; i < size; i++) {
			peaklistseriesvector[i].load(is, fileversionpart1, fileversionpart2, fileversionpart3);
		}
	}
	else {
		peaklistfilenames.resize(1);
		loadString(peaklistfilenames[0], is);

		originalpeaklistfilenames = peaklistfilenames[0];

		peaklistfileformats.resize(1);
		is.read((char *)&peaklistfileformats[0], sizeof(ePeakListFileFormat));

		peaklistseriesvector.resize(1);
		peaklistseriesvector[0].load(is, fileversionpart1, fileversionpart2, fileversionpart3);
	}

	is.read((char *)&useprofiledata, sizeof(bool));
	is.read((char *)&convertprofiledata, sizeof(bool));

	loadString(profiledatafilename, is);

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 18)) {
		is.read((char *)&linebafprocessing, sizeof(int));
	}
	else {
		linebafprocessing = 0;
	}

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
	is.read((char *)&maximummz, sizeof(double));

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 29)) {
		is.read((char *)&minimumrt, sizeof(double));
		is.read((char *)&maximumrt, sizeof(double));
	}
	else {
		minimumrt = 0;
		maximumrt = 0;
	}

	is.read((char *)&fwhm, sizeof(double));

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 37)) {
		is.read((char *)&minratio54Fe56Fe, sizeof(double));
		is.read((char *)&maxratio54Fe56Fe, sizeof(double));
	}
	else {
		minratio54Fe56Fe = 0;
		maxratio54Fe56Fe = 0.1;
	}

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

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 33)) {
		is.read((char *)&maximumcombinedelements, sizeof(int));
	}
	else {
		if (mode == compoundsearch) {
			maximumcombinedelements = maximumcombinedlosses;
		}
		else {
			maximumcombinedelements = 200;
		}
	}

	//is.read((char *)&clearhitswithoutparent, sizeof(bool));
	is.read((char *)&reportunmatchedtheoreticalpeaks, sizeof(bool));
	is.read((char *)&generateisotopepattern, sizeof(bool));
	is.read((char *)&minimumpatternsize, sizeof(int));
	is.read((char *)&minimumfeaturesize, sizeof(int));
	is.read((char *)&minimumiontypes, sizeof(int));
	is.read((char *)&basicformulacheck, sizeof(bool));
	is.read((char *)&advancedformulacheck, sizeof(bool));
	is.read((char *)&noratiocheck, sizeof(bool));

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 11)) {
		is.read((char *)&calculatefdrs, sizeof(bool));
	}
	else {
		calculatefdrs = true;
	}

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 1)) {
		is.read((char *)&minimumannotationintensityrelative, sizeof(double));
		is.read((char *)&minimumannotationintensityabsolute, sizeof(unsigned));
	}
	else {
		minimumannotationintensityrelative = 0;
		minimumannotationintensityabsolute = 0;
	}

	is.read((char *)&mzdifftolerance, sizeof(double));
	is.read((char *)&intensitytolerance, sizeof(double));
	is.read((char *)&cyclicnterminus, sizeof(bool));
	is.read((char *)&cycliccterminus, sizeof(bool));
	is.read((char *)&internalfragments, sizeof(bool));
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
	loadString(searchedsequenceformula, is);

	is.read((char *)&maximumnumberofcandidates, sizeof(int));
	is.read((char *)&blindedges, sizeof(int));

	loadString(sequencedatabasefilename, is);
	sequencedatabase.load(is, fileversionpart1, fileversionpart2, fileversionpart3);

	is.read((char *)&size, sizeof(int));
	fragmentionsfordenovograph.resize(size);
	for (int i = 0; i < (int)fragmentionsfordenovograph.size(); i++) {
		is.read((char *)&fragmentionsfordenovograph[i], sizeof(eFragmentIonType));
	}

	is.read((char *)&size, sizeof(int));
	ionsfortheoreticalspectraMS1.resize(size);
	for (int i = 0; i < (int)ionsfortheoreticalspectraMS1.size(); i++) {
		if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 23)) {
			ionsfortheoreticalspectraMS1[i].load(is);
		}
		else {
			// bug before version 2.0.23
			is.read((char *)&ionsfortheoreticalspectraMS1[i], sizeof(cIonType));
		}
	}

	is.read((char *)&size, sizeof(int));
	ionsfortheoreticalspectraMS2.resize(size);
	for (int i = 0; i < (int)ionsfortheoreticalspectraMS2.size(); i++) {
		is.read((char *)&ionsfortheoreticalspectraMS2[i], sizeof(eFragmentIonType));
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

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 34)) {
		is.read((char *)&size, sizeof(int));
		originalelementsdefinitions.resize(size);
		for (int i = 0; i < (int)originalelementsdefinitions.size(); i++) {
			originalelementsdefinitions[i].load(is);
		}

		is.read((char *)&size, sizeof(int));
		originalelementsfortheoreticalspectra.resize(size);
		for (int i = 0; i < (int)originalelementsfortheoreticalspectra.size(); i++) {
			is.read((char *)&originalelementsfortheoreticalspectra[i], sizeof(int));
		}
	}
	else {
		if (mode == compoundsearch) {
			originalelementsdefinitions = originalneutrallossesdefinitions;
			originalelementsfortheoreticalspectra = originalneutrallossesfortheoreticalspectra;

			originalneutrallossesdefinitions.clear();
			originalneutrallossesfortheoreticalspectra.clear();
			maximumcombinedlosses = 0;
		}
		else {
			originalelementsdefinitions.clear();
			originalelementsfortheoreticalspectra.clear();

			pushLossItem(originalelementsdefinitions, "H");
			pushLossItem(originalelementsdefinitions, "C");
			pushLossItem(originalelementsdefinitions, "O");
			pushLossItem(originalelementsdefinitions, "N");
			pushLossItem(originalelementsdefinitions, "S:1");
			pushLossItem(originalelementsdefinitions, "P:1");

			originalelementsfortheoreticalspectra.push_back(0);
			originalelementsfortheoreticalspectra.push_back(1);
			originalelementsfortheoreticalspectra.push_back(2);
			originalelementsfortheoreticalspectra.push_back(3);
		}
	}

	loadStringVector(peakidtodesc, is);
	loadStringVector(isotopeformulaidtodesc, is);

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 21)) {
		loadStringIntMap(pchemresults, is);
	}
	else {
		pchemresults.clear();
	}

	is.read((char *)&defaultmaxx, sizeof(int));
	is.read((char *)&defaultmaxy, sizeof(int));
	is.read((char *)&defaultpixelsizex, sizeof(int));
	is.read((char *)&defaultpixelsizey, sizeof(int));
	is.read((char *)&vendor, sizeof(eVendorType));
}

