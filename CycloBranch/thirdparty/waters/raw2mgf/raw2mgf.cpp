#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip> 

#include "include/MassLynxRawDataFile.h"
#include "include/MassLynxRawReader.h"
#include "include/MassLynxRawScanReader.h"
#include "include/MassLynxRawChromatogramReader.h"
#include "include/MassLynxRawInfo.h"
#include "include/MassLynxRawScanStatsReader.h"

using std::vector;
using std::string;
using std::cout;
using std::endl;

using namespace std;
using namespace Waters::Lib::MassLynxRaw;

#include <stdio.h>


int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Error: Missing parameter - raw data folder name." << endl;
		exit(1);
	}

	try {

		// get a raw reader
		MassLynxRawReader RR(argv[1]);

		// use it to get raw info
		MassLynxRawInfo RI(RR);
		int nFuncs = RI.GetFunctionCount();

		cout << "The data has " << nFuncs << " functions." << endl << endl;

		// now read some raw spectra
		MassLynxRawScanReader RSR(RR);

		vector<float> masses;
		vector<float> intensities;

		string outputfile;
		ofstream outputstream;

		outputfile = argv[1];
		outputfile = outputfile.substr(0, outputfile.rfind('.')) + ".mgf";
		outputstream.open(outputfile);

		outputstream << fixed;
		outputstream << setprecision(10);

		// basic scan stats
		MassLynxRawScanStatsReader RSSR(RR);

		vector<MSScanStats> stats;

		for (int i = 0; i < nFuncs; i++) {

			cout << "Exporting scan no. " << i << "... ";

			stats.clear();
			RSSR.readScanStats(i, stats);

			for (int j = 0; j < (int)stats.size(); j++) {
				masses.clear();
				intensities.clear();

				outputstream << "BEGIN IONS\nTITLE=";
				outputstream << "function=" << to_string(i) << " ";
				outputstream << "scan=" << to_string(j);
				outputstream << "\nCHARGE=1+\n\n";

				RSR.readSpectrum(i, j, masses, intensities);
				
				for (int k = 0; k < (int)masses.size(); k++) {
					outputstream << masses[k] << " " << intensities[k] << endl;
				}

				outputstream << "END IONS\n\n";
			}

			cout << "ok" << endl;

		}

		outputstream.close();

	}
	catch (MassLynxRawException &e) {
		cout << "MassLynxRawException: " << e.what() << endl;
	}

	return 0;
}

