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
	seriesid = -1;
}


bool cPeak::empty() {
	if ((mzratio == 0) && (intensity == 0)) return true;
	return false;
}


void cPeak::store(ofstream& os) {
	os.write((char *)&mzratio, sizeof(double));
	os.write((char *)&intensity, sizeof(double));
	storeString(description, os);
	storeString(matchdescription, os);
	os.write((char *)&iontype, sizeof(fragmentIonType));
	os.write((char *)&matched, sizeof(int));
	os.write((char *)&matchedid, sizeof(int));
	os.write((char *)&charge, sizeof(int));
	os.write((char *)&rotationid, sizeof(int));
	os.write((char *)&seriesid, sizeof(int));
	os.write((char *)&isotope, sizeof(bool));
	os.write((char *)&removeme, sizeof(bool));
	os.write((char *)&scrambled, sizeof(bool));
}


void cPeak::load(ifstream& is) {
	is.read((char *)&mzratio, sizeof(double));
	is.read((char *)&intensity, sizeof(double));
	loadString(description, is);
	loadString(matchdescription, is);
	is.read((char *)&iontype, sizeof(fragmentIonType));
	is.read((char *)&matched, sizeof(int));
	is.read((char *)&matchedid, sizeof(int));
	is.read((char *)&charge, sizeof(int));
	is.read((char *)&rotationid, sizeof(int));
	is.read((char *)&seriesid, sizeof(int));
	is.read((char *)&isotope, sizeof(bool));
	is.read((char *)&removeme, sizeof(bool));
	is.read((char *)&scrambled, sizeof(bool));
}

