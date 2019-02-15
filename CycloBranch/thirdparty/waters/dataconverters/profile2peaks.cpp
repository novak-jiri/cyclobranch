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

		MassLynxRawProcessor RP;
		RP.SetRawPath(argv[1]);

		string outputpath = argv[1];
		outputpath = outputpath.substr(0, outputpath.size() - 4);
		outputpath += "_PEAKS.raw";

		cout << "Generating peaks ... ";

		RP.Centroid(outputpath);

		cout << "ok" << endl;

	}
	catch (const MassLynxRawException e) {

		cout << "MassLynxRawException: " << e.what() << endl;

	}

	return 0;
}

