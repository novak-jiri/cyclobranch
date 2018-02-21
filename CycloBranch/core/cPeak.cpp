#include "core/cPeak.h"


cPeak::cPeak() {
	clear();
}


cPeak::cPeak(const cPeak& peak) {
	*this = peak;
}


cPeak& cPeak::operator=(const cPeak& peak) {
	formula = peak.formula;
	isotopeformula = peak.isotopeformula;
	isotopeformulaid = peak.isotopeformulaid;
	mzratio = peak.mzratio;
	relativeintensity = peak.relativeintensity;
	absoluteintensity = peak.absoluteintensity;
	description = peak.description;
	descriptionid = peak.descriptionid;
	matchedmz = peak.matchedmz;
	matchedrelativeintensity = peak.matchedrelativeintensity;
	matchedabsoluteintensity = peak.matchedabsoluteintensity;
	matchedppm = peak.matchedppm;
	iontype = peak.iontype;
	matched = peak.matched;
	matchedid = peak.matchedid;
	charge = peak.charge;
	isotope = peak.isotope;
	removeme = peak.removeme;
	scrambled = peak.scrambled;
	decoy = peak.decoy;
	orderid = peak.orderid;

	rotationid = peak.rotationid;
	seriesid = peak.seriesid;
	groupid = peak.groupid;

	return *this;
}


void cPeak::clear() {
	formula.clear();
	isotopeformula = "";
	isotopeformulaid = -1;
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
	formula.store(os);
	storeString(isotopeformula, os);
	os.write((char *)&isotopeformulaid, sizeof(int));
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
	formula.load(is);
	loadString(isotopeformula, is);
	is.read((char *)&isotopeformulaid, sizeof(int));
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

