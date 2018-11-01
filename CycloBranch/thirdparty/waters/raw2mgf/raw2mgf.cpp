#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip> 

#include "include/MassLynxRawInfoReader.hpp"
#include "include/MassLynxRawScanReader.hpp"


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

		string outputfile;
		ofstream outputstream;

		outputfile = argv[1];
		outputfile = outputfile.substr(0, outputfile.rfind('.')) + ".mgf";
		outputstream.open(outputfile);

		outputstream << fixed;
		outputstream << setprecision(10);

		MassLynxRawScanReader RSR(RI);

		for (int i = 0; i < nFuncs; i++) {

			cout << "Exporting scan no. " << i << "... ";

			int nScans = RI.GetScansInFunction(i);
			for (int j = 0; j < nScans; j++) {
				masses.clear();
				intensities.clear();

				outputstream << "BEGIN IONS\nTITLE=";
				outputstream << "function=" << to_string(i) << " ";
				outputstream << "scan=" << to_string(j);
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

