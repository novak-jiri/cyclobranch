#include "core/cPeak.h"


cPeak::cPeak() {
	clear();
}


void cPeak::clear() {
	mzratio = 0;
	relativeintensity = 0;
	absoluteintensity = 0;
	description = "";
	descriptionid = -1;
	matchedmz = 0;
	matchedrelativeintensity = 0;
	matchedabsoluteintensity = 0;
	matchedppm = 0;
	iontype = fragmentIonTypeEnd;
	matched = 0;
	matchedid = -1;
	charge = 1;
	isotope = false;
	removeme = false;
	scrambled = false;
	decoy = false;
	orderid = -1;

	rotationid = -1;
	seriesid = -1;
	groupid = -1;
}


bool cPeak::empty() {
	if ((mzratio == 0) && (relativeintensity == 0) && (absoluteintensity == 0)) return true;
	return false;
}


void cPeak::store(ofstream& os) {
	os.write((char *)&mzratio, sizeof(double));
	os.write((char *)&relativeintensity, sizeof(double));
	os.write((char *)&absoluteintensity, sizeof(double));
	storeString(description, os);
	os.write((char *)&descriptionid, sizeof(int));
	os.write((char *)&matchedmz, sizeof(double));
	os.write((char *)&matchedrelativeintensity, sizeof(double));
	os.write((char *)&matchedabsoluteintensity, sizeof(double));
	os.write((char *)&matchedppm, sizeof(double));
	os.write((char *)&iontype, sizeof(eFragmentIonType));
	os.write((char *)&matched, sizeof(int));
	os.write((char *)&matchedid, sizeof(int));
	os.write((char *)&charge, sizeof(int));
	os.write((char *)&rotationid, sizeof(int));
	os.write((char *)&seriesid, sizeof(int));
	os.write((char *)&groupid, sizeof(int));
	os.write((char *)&isotope, sizeof(bool));
	os.write((char *)&removeme, sizeof(bool));
	os.write((char *)&scrambled, sizeof(bool));
	os.write((char *)&decoy, sizeof(bool));
	os.write((char *)&orderid, sizeof(int));
}


void cPeak::load(ifstream& is) {
	is.read((char *)&mzratio, sizeof(double));
	is.read((char *)&relativeintensity, sizeof(double));
	is.read((char *)&absoluteintensity, sizeof(double));
	loadString(description, is);
	is.read((char *)&descriptionid, sizeof(int));
	is.read((char *)&matchedmz, sizeof(double));
	is.read((char *)&matchedrelativeintensity, sizeof(double));
	is.read((char *)&matchedabsoluteintensity, sizeof(double));
	is.read((char *)&matchedppm, sizeof(double));
	is.read((char *)&iontype, sizeof(eFragmentIonType));
	is.read((char *)&matched, sizeof(int));
	is.read((char *)&matchedid, sizeof(int));
	is.read((char *)&charge, sizeof(int));
	is.read((char *)&rotationid, sizeof(int));
	is.read((char *)&seriesid, sizeof(int));
	is.read((char *)&groupid, sizeof(int));
	is.read((char *)&isotope, sizeof(bool));
	is.read((char *)&removeme, sizeof(bool));
	is.read((char *)&scrambled, sizeof(bool));
	is.read((char *)&decoy, sizeof(bool));
	is.read((char *)&orderid, sizeof(int));
}

