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
	neutrallosstype = peak.neutrallosstype;
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
	compoundid = peak.compoundid;

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
	neutrallosstype = -1;
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
	compoundid = -1;
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
	os.write((char *)&neutrallosstype, sizeof(int));
	os.write((char *)&matched, sizeof(int));
	os.write((char *)&matchedid, sizeof(int));
	os.write((char *)&charge, sizeof(int));
	os.write((char *)&rotationid, sizeof(int));
	os.write((char *)&seriesid, sizeof(int));
	os.write((char *)&groupid, sizeof(int));
	os.write((char *)&compoundid, sizeof(int));
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
	is.read((char *)&neutrallosstype, sizeof(int));
	is.read((char *)&matched, sizeof(int));
	is.read((char *)&matchedid, sizeof(int));
	is.read((char *)&charge, sizeof(int));
	is.read((char *)&rotationid, sizeof(int));
	is.read((char *)&seriesid, sizeof(int));
	is.read((char *)&groupid, sizeof(int));
	is.read((char *)&compoundid, sizeof(int));
	is.read((char *)&isotope, sizeof(bool));
	is.read((char *)&removeme, sizeof(bool));
	is.read((char *)&scrambled, sizeof(bool));
	is.read((char *)&decoy, sizeof(bool));
	is.read((char *)&orderid, sizeof(int));
}


bool cPeak::equals(cPeak& secondpeak) {
	if (!formula.equals(secondpeak.formula)) {
		cout << "formula does not match" << endl;
		return false;
	}

	if (isotopeformula.compare(secondpeak.isotopeformula) != 0) {
		cout << "isotopeformula does not match" << endl;
		return false;
	}

	if (isotopeformulaid != secondpeak.isotopeformulaid) {
		cout << "isotopeformulaid does not match" << endl;
		return false;
	}

	if (!compareDoubles(mzratio, secondpeak.mzratio)) {
		cout << mzratio << " " << secondpeak.mzratio << endl;
		cout << "mzratio does not match" << endl;
		return false;
	}

	if (!compareDoubles(relativeintensity, secondpeak.relativeintensity)) {
		cout << "relativeintensity does not match" << endl;
		return false;
	}

	if (!compareDoubles(absoluteintensity, secondpeak.absoluteintensity)) {
		cout << "absoluteintensity does not match" << endl;
		return false;
	}

	if (description.compare(secondpeak.description) != 0) {
		cout << "description does not match" << endl;
		return false;
	}

	if (descriptionid != secondpeak.descriptionid) {
		cout << "descriptionid does not match" << endl;
		return false;
	}

	if (!compareDoubles(matchedmz, secondpeak.matchedmz)) {
		cout << "matchedmz does not match" << endl;
		return false;
	}

	if (!compareDoubles(matchedrelativeintensity, secondpeak.matchedrelativeintensity)) {
		cout << "matchedrelativeintensity does not match" << endl;
		return false;
	}

	if (!compareDoubles(matchedabsoluteintensity, secondpeak.matchedabsoluteintensity)) {
		cout << "matchedabsoluteintensity does not match" << endl;
		return false;
	}

	if (!compareDoubles(matchedppm, secondpeak.matchedppm)) {
		cout << "matchedppm does not match" << endl;
		return false;
	}

	if (iontype != secondpeak.iontype) {
		cout << "iontype does not match" << endl;
		return false;
	}

	if (neutrallosstype != secondpeak.neutrallosstype) {
		cout << "neutrallosstype does not match" << endl;
		return false;
	}

	if (matched != secondpeak.matched) {
		cout << "matched does not match" << endl;
		return false;
	}

	if (matchedid != secondpeak.matchedid) {
		cout << "matchedid does not match" << endl;
		return false;
	}

	if (charge != secondpeak.charge) {
		cout << "charge does not match" << endl;
		return false;
	}

	if (rotationid != secondpeak.rotationid) {
		cout << "rotationid does not match" << endl;
		return false;
	}

	if (seriesid != secondpeak.seriesid) {
		cout << "seriesid does not match" << endl;
		return false;
	}

	if (groupid != secondpeak.groupid) {
		cout << "groupid does not match" << endl;
		return false;
	}

	if (compoundid != secondpeak.compoundid) {
		cout << "compoundid does not match" << endl;
		return false;
	}

	if (isotope != secondpeak.isotope) {
		cout << "isotope does not match" << endl;
		return false;
	}

	if (removeme != secondpeak.removeme) {
		cout << "removeme does not match" << endl;
		return false;
	}

	if (scrambled != secondpeak.scrambled) {
		cout << "scrambled does not match" << endl;
		return false;
	}

	if (decoy != secondpeak.decoy) {
		cout << "decoy does not match" << endl;
		return false;
	}

	if (orderid != secondpeak.orderid) {
		cout << "orderid does not match" << endl;
		return false;
	}

	return true;
}

