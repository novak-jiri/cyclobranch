#include "core/cPeak.h"


cPeak::cPeak() {
	clear();
}


void cPeak::clear() {
	mzratio = 0;
	intensity = 0;
	description = "";
	matchdescription = "";
	iontype = fragmentIonTypeEnd;
	matched = 0;
	matchedid = -1;
	charge = 1;
	isotope = false;
	removeme = false;
	scrambled = false;

	rotationid = -1;
	matchedrotations.clear();
	seriesid = -1;
}


bool cPeak::empty() {
	if ((mzratio == 0) && (intensity == 0)) return true;
	return false;
}


bool cPeak::hasMatchedRotation(int rotationid) {
	for (int i = 0; i < (int)matchedrotations.size(); i++) {
		if (rotationid == matchedrotations[i]) {
			return true;
		}
	}
	return false;
}


void cPeak::store(ofstream& os) {
	int size;

	os.write((char *)&mzratio, sizeof(double));
	os.write((char *)&intensity, sizeof(double));

	size = (int)description.size();
	os.write((char *)&size, sizeof(int));
	os.write(description.c_str(), description.size());

	size = (int)matchdescription.size();
	os.write((char *)&size, sizeof(int));
	os.write(matchdescription.c_str(), matchdescription.size());

	os.write((char *)&iontype, sizeof(fragmentIonType));
	os.write((char *)&matched, sizeof(int));
	os.write((char *)&matchedid, sizeof(int));
	os.write((char *)&charge, sizeof(int));
	os.write((char *)&rotationid, sizeof(int));

	size = (int)matchedrotations.size();
	os.write((char *)&size, sizeof(int));
	for (int i = 0; i < (int)matchedrotations.size(); i++) {
		os.write((char *)&matchedrotations[i], sizeof(int));
	}
	
	os.write((char *)&seriesid, sizeof(int));
	os.write((char *)&isotope, sizeof(bool));
	os.write((char *)&removeme, sizeof(bool));
	os.write((char *)&scrambled, sizeof(bool));
}


void cPeak::load(ifstream& is) {
	int size;

	is.read((char *)&mzratio, sizeof(double));
	is.read((char *)&intensity, sizeof(double));

	is.read((char *)&size, sizeof(int));
	description.resize(size);
	is.read(&description[0], description.size());

	is.read((char *)&size, sizeof(int));
	matchdescription.resize(size);
	is.read(&matchdescription[0], matchdescription.size());

	is.read((char *)&iontype, sizeof(fragmentIonType));
	is.read((char *)&matched, sizeof(int));
	is.read((char *)&matchedid, sizeof(int));
	is.read((char *)&charge, sizeof(int));
	is.read((char *)&rotationid, sizeof(int));

	is.read((char *)&size, sizeof(int));
	matchedrotations.resize(size);
	for (int i = 0; i < (int)matchedrotations.size(); i++) {
		is.read((char *)&matchedrotations[i], sizeof(int));
	}
	
	is.read((char *)&seriesid, sizeof(int));
	is.read((char *)&isotope, sizeof(bool));
	is.read((char *)&removeme, sizeof(bool));
	is.read((char *)&scrambled, sizeof(bool));
}

