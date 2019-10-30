#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip> 

#include "include/MassLynxRawInfoReader.hpp"
#include "include/MassLynxRawScanReader.hpp"
#include "include/MassLynxRawChromatogramReader.hpp"


using namespace std;
using namespace Waters::Lib::MassLynxRaw;


int main(int argc, char* argv[]) {

	if (argc < 2) {
		cerr << "Error: Missing parameter - raw data folder name." << endl;
		exit(1);
	}

	try {

		MassLynxRawInfo RI(argv[1]);
		int nFuncs = RI.GetFunctionCount();

		cout << "The data has " << nFuncs << " functions." << endl << endl;

		vector<float> masses;
		vector<float> intensities;

		vector<float> chrtimes;
		vector<float> chrintensities;

		string outputfile;
		ofstream outputstream;

		outputfile = argv[1];
		outputfile = outputfile.substr(0, outputfile.rfind('.')) + ".mgf";
		outputstream.open(outputfile);

		outputstream << fixed;
		outputstream << setprecision(10);

		MassLynxRawScanReader RSR(RI);
		MassLynxRawChromatogramReader RCR(RSR);

		bool exporttime = true;

		for (int i = 0; i < nFuncs; i++) {

			cout << "Exporting scans from function no. " << i << "... ";

			chrtimes.clear();
			chrintensities.clear();

			RCR.ReadTICChromatogram(i, chrtimes, chrintensities);
			int nScans = RI.GetScansInFunction(i);

			if (nScans != (int)chrtimes.size()) {
				cout << "Warning: The chromatogram size is different from the number of scans. The retention time will not be exported." << endl;
				exporttime = false;
			}

			for (int j = 0; j < nScans; j++) {
				masses.clear();
				intensities.clear();

				outputstream << "BEGIN IONS\nTITLE=";
				outputstream << "function=" << to_string(i) << " ";
				outputstream << "scan=" << to_string(j);
				if (exporttime) {
					outputstream << "\nRTINSECONDS=" << to_string(chrtimes[j]);
				}
				outputstream << "\nCHARGE=1+\n\n";

				RSR.ReadScan(i, j, masses, intensities);
				
				for (int k = 0; k < (int)masses.size(); k++) {
					outputstream << masses[k] << " " << intensities[k] << endl;
				}

				outputstream << "END IONS\n\n";
			}

			cout << "ok" << endl;

		}

		outputstream.close();

	}
	catch (const MassLynxRawException e) {

		cout << "MassLynxRawException: " << e.what() << endl;

	}

	return 0;
}

