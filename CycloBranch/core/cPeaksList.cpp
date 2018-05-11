#include "core/cPeaksList.h"
#include "gui/cMainThread.h"


bool compareRelativePeakIntensitiesDesc(const cPeak& a, const cPeak& b) {
	return (a.relativeintensity > b.relativeintensity);
}


bool isInPpmMassErrorTolerance(double experimentalmass, double theoreticalmass, double tolerance) {
	double errortolerance = experimentalmass*tolerance*0.000001f;
	return abs(experimentalmass - theoreticalmass) <= errortolerance;
}


double ppmError(double experimentalmass, double theoreticalmass) {
	return (experimentalmass - theoreticalmass)/experimentalmass*1000000.0f;
}


cPeaksList::cPeaksList() {
	clear();
}


cPeaksList::cPeaksList(const cPeaksList& peakslist) {
	*this = peakslist;
}


cPeaksList& cPeaksList::operator=(const cPeaksList& peakslist) {
	peaks = peakslist.peaks;
	x = peakslist.x;
	y = peakslist.y;
	title = peakslist.title;
	return *this;
}


void cPeaksList::clear() {
	peaks.clear();
	x = 0;
	y = 0;
	title = "";
}


void cPeaksList::attach(cPeaksList& peaklist) {
	peaks.insert(peaks.end(), peaklist.peaks.begin(), peaklist.peaks.end());
}


void cPeaksList::add(cPeak& peak) {
	peaks.push_back(peak);
}


void cPeaksList::loadFromPlainTextStream(ifstream &stream) {
	string s;
	cPeak p;
	size_t pos;

	while (stream.good()) {
		getline(stream,s);

		// replaces commas with dots
		pos = s.find(',');
		while (pos != string::npos) {
			s.replace(pos, 1, ".");
			pos = s.find(',');
		}

		// load a row
		p.clear();
		sscanf_s(s.c_str(), "%lf\t%lf", &p.mzratio, &p.absoluteintensity);
		if (!p.empty()) {
			peaks.push_back(p);
		}
		else {
			break;
		}
	}
}


void cPeaksList::loadFromBAFStream(ifstream &stream) {
	string s;
	cPeak p;
	size_t pos;

	while (stream.good() && !(strstr(s.c_str(),"M/Z"))) {

		// replaces commas with dots
		//pos = s.find(',');
		//while (pos != string::npos) {
		//	s.replace(pos, 1, ".");
		//	pos = s.find(',');
		//}

		pos = s.find(',');
		if (pos != string::npos) {
			s.replace(pos, 1, "\t");
		}

		// load a row
		p.clear();
		sscanf_s(s.c_str(), "%lf\t%lf", &p.mzratio, &p.absoluteintensity);
		if (!p.empty()) {
			peaks.push_back(p);
		}

		getline(stream,s);
	}
}


void cPeaksList::loadFromIBDStream(cImzMLItem& imzmlitem, ifstream &ibdstream, bool use_64bit_float_mz_precision, bool use_64bit_float_intensity_precision) {
	unsigned long long start;
	float flt;
	double dbl;
	
	// mz values
	if (use_64bit_float_mz_precision) {
		peaks.resize(imzmlitem.mzlength/8);
		start = imzmlitem.mzstart;
		ibdstream.seekg(start, ibdstream.beg);
		for (int i = 0; i < (int)imzmlitem.mzlength/8; i++) {
			if (ibdstream.good()) {
				ibdstream.read((char*)&dbl, 8);
				peaks[i].mzratio = dbl;
			}
		}
	}
	else {
		peaks.resize(imzmlitem.mzlength/4);
		start = imzmlitem.mzstart;
		ibdstream.seekg(start, ibdstream.beg);
		for (int i = 0; i < (int)imzmlitem.mzlength/4; i++) {
			if (ibdstream.good()) {
				ibdstream.read((char*)&flt, 4);
				peaks[i].mzratio = flt;
			}
		}
	}

	if (imzmlitem.mzlength != imzmlitem.intensitylength) {
		return;
	}

	// intensities
	if (use_64bit_float_intensity_precision) {
		start = imzmlitem.intensitystart;
		ibdstream.seekg(start, ibdstream.beg);
		for (int i = 0; i < (int)imzmlitem.intensitylength / 8; i++) {
			if (ibdstream.good()) {
				ibdstream.read((char*)&dbl, 8);
				peaks[i].absoluteintensity = dbl;
			}
		}
	}
	else {
		start = imzmlitem.intensitystart;
		ibdstream.seekg(start, ibdstream.beg);
		for (int i = 0; i < (int)imzmlitem.intensitylength / 4; i++) {
			if (ibdstream.good()) {
				ibdstream.read((char*)&flt, 4);
				peaks[i].absoluteintensity = flt;
			}
		}
	}

	x = imzmlitem.x;
	y = imzmlitem.y;
	title = imzmlitem.title;
}


void cPeaksList::storeToIBDStream(ofstream &ibdstream, bool use_64bit_float_mz_precision, bool use_64bit_float_intensity_precision) {
	float value;

	// store m/z values
	if (use_64bit_float_mz_precision) {
		for (int i = 0; i < (int)peaks.size(); i++) {
			ibdstream.write((char *)&(peaks[i].mzratio), 8);
		}
	}
	else {
		for (int i = 0; i < (int)peaks.size(); i++) {
			value = peaks[i].mzratio;
			ibdstream.write((char *)&value, 4);
		}
	}

	// store intensities
	if (use_64bit_float_intensity_precision) {	
		for (int i = 0; i < (int)peaks.size(); i++) {
			ibdstream.write((char *)&(peaks[i].absoluteintensity), 8);
		}
	}
	else {
		for (int i = 0; i < (int)peaks.size(); i++) {
			value = peaks[i].absoluteintensity;
			ibdstream.write((char *)&value, 4);
		}
	}
}


void cPeaksList::loadFromMGFStream(ifstream &stream) {
	string s;
	cPeak p;
	size_t pos;

	while (stream.good()) {
		getline(stream,s);
		if (strstr(s.c_str(),"BEGIN IONS")) {
			break;
		}
	}

	while (stream.good() && !(strstr(s.c_str(),"END IONS"))) {
		getline(stream,s);
		if (!strstr(s.c_str(),"=")) {
			break;
		}
		if (s.substr(0, 6).compare("TITLE=") == 0) {
			title = s.substr(6);
		}
	}

	while (stream.good() && !(strstr(s.c_str(),"END IONS"))) {
		getline(stream,s);
		if (s.size() > 0) {
			break;
		}
	}

	while (stream.good() && !(strstr(s.c_str(),"END IONS"))) {
		// replaces commas with dots
		pos = s.find(',');
		while (pos != string::npos) {
			s.replace(pos, 1, ".");
			pos = s.find(',');
		}

		// load a row
		p.clear();
		sscanf_s(s.c_str(), "%lf %lf", &p.mzratio, &p.absoluteintensity);
		if (!p.empty()) {
			peaks.push_back(p);
		}

		getline(stream,s);
	}
}


int cPeaksList::size() {
	return (int)peaks.size();
}


string cPeaksList::print(bool htmlterminatelines) {
	string s = "";

	for (int i = 0; i < size(); i++) {

		if (peaks[i].description.compare("") != 0) {
			s += peaks[i].description + " ";
		}

		s += to_string(peaks[i].mzratio);

		if (peaks[i].relativeintensity > 0) {
			s += " ";
			s += to_string(peaks[i].relativeintensity);
		}

		if (peaks[i].absoluteintensity > 0) {
			s += " ";
			s += to_string((unsigned long long)peaks[i].absoluteintensity);
		}

		if (htmlterminatelines) {
			s += "<br/>";
		}
		else {
			s += "\n";
		}

	}

	s += "Total peaks: " + to_string((int)peaks.size()) + "\n\n";
	return s;
}


void cPeaksList::sortbyMass(int limit) {
	auto cmp = [](const cPeak& a, const cPeak& b) {
		return (a.mzratio < b.mzratio);
	};

	if (limit == -1) {
		sort(peaks.begin(), peaks.end(), cmp);
	}
	else {
		sort(peaks.begin(), peaks.begin() + limit, cmp);
	}
}


void cPeaksList::sortbyRelativeIntensityDesc() {
	sort(peaks.begin(), peaks.end(), compareRelativePeakIntensitiesDesc);
}


void cPeaksList::sortbyAbsoluteIntensityDesc() {
	auto cmp = [](const cPeak& a, const cPeak& b) {
		return (a.absoluteintensity > b.absoluteintensity);
	};

	sort(peaks.begin(), peaks.end(), cmp);
}


void cPeaksList::sortbyGroupId() {
	auto cmp = [](const cPeak& a, const cPeak& b) {
		return (a.groupid < b.groupid);
	};

	sort(peaks.begin(), peaks.end(), cmp);
}


void cPeaksList::sortbyOrderId() {
	auto cmp = [](const cPeak& a, const cPeak& b) {
		return (a.orderid < b.orderid);
	};

	sort(peaks.begin(), peaks.end(), cmp);
}


int cPeaksList::normalizeIntenzity() {
	if (peaks.size() == 0) {
		return -1;
	}

	double maximum = 0.0f;

	for (int i = 0; i < (int)peaks.size(); i++) {
		if (peaks[i].absoluteintensity > maximum) {
			maximum = peaks[i].absoluteintensity;
		}
	}

	if (maximum > 0) {
		for (int i = 0; i < (int)peaks.size(); i++) {
			peaks[i].relativeintensity = peaks[i].absoluteintensity/maximum*100.0f;
		}
	}

	return 0;
}


void cPeaksList::normalizeIntenzityByValue(double value) {
	if (peaks.size() == 0) {
		return;
	}

	if (value > 0) {
		for (int i = 0; i < (int)peaks.size(); i++) {
			peaks[i].relativeintensity = peaks[i].absoluteintensity / value * 100.0f;
		}
	}
}


void cPeaksList::cropRelativeIntenzity(double minimumrelativeintensitythreshold) {
	sortbyRelativeIntensityDesc();

	int i = (int)peaks.size() - 1;
	while ((i >= 0) && (peaks[i].relativeintensity < minimumrelativeintensitythreshold)) {
		i--;
	}
	peaks.resize(i + 1);

	sortbyMass();
}


void cPeaksList::cropAbsoluteIntenzity(unsigned minimumabsoluteintensitythreshold) {
	sortbyAbsoluteIntensityDesc();

	int i = (int)peaks.size() - 1;
	while ((i >= 0) && (peaks[i].absoluteintensity < (double)minimumabsoluteintensitythreshold)) {
		i--;
	}
	peaks.resize(i + 1);

	sortbyMass();
}


void cPeaksList::cropMinimumMZRatio(double minimummz, double errortolerance) {
	int position = 0;
	while (position < (int)peaks.size()) {
		if ((peaks[position].mzratio < minimummz) && (!isInPpmMassErrorTolerance(peaks[position].mzratio, minimummz, errortolerance))) {
			position++;
		}
		else {
			break;
		}
	}
	if (position > 0) {
		peaks.erase(peaks.begin(), peaks.begin() + position);
	}
}


void cPeaksList::cropMaximumMZRatio(double maximummz, double errortolerance) {
	while ((int)peaks.size() > 0) {
		if ((peaks.back().mzratio > maximummz) && (!isInPpmMassErrorTolerance(peaks.back().mzratio, maximummz, errortolerance))) {
			peaks.erase(peaks.begin() + peaks.size() - 1);
		}
		else {
			break;
		}
	}
}


cPeak& cPeaksList::operator[](int position) {
	return peaks[position];
}


void cPeaksList::resize(int size) {
	peaks.resize(size);
}


double cPeaksList::getMinimumMZRatio() {
	if (peaks.size() == 0) {
		return 0;	
	}

	double minimum = peaks[0].mzratio;
	for (int i = 1; i < (int)peaks.size(); i++) {
		if (peaks[i].mzratio < minimum) {
			minimum = peaks[i].mzratio;
		}
	}
	return minimum;
}


double cPeaksList::getMaximumMZRatio() {
	double maximum = 0;
	for (int i = 0; i < (int)peaks.size(); i++) {
		if (peaks[i].mzratio > maximum) {
			maximum = peaks[i].mzratio;
		}
	}
	return maximum;
}


void cPeaksList::removeIsotopes(int maximumcharge, double fragmentmasserrortolerance, cMainThread* os) {

	if (os) {
		*os << "Removed isotopes:" << endl;
	}

	int pos, i;
	for (i = 0; i < (int)peaks.size(); i++) {
		peaks[i].isotope = false;
	}
	for (i = 0; i < (int)peaks.size(); i++) {
		for (int j = 1; j <= abs(maximumcharge); j++) {
			pos = find(charge(peaks[i].mzratio, (maximumcharge > 0)?j:-j), fragmentmasserrortolerance);
			if (pos != -1) {
				if (peaks[pos].relativeintensity < peaks[i].relativeintensity) {
					peaks[pos].isotope = true;
				}
				else {
					peaks[i].isotope = true;
				}
			}
		}
	}
	i = 0;
	while (i < (int)peaks.size()) {
		if (peaks[i].isotope) {
			if (os) {
				*os << peaks[i].mzratio << endl;
			}
			peaks.erase(peaks.begin()+i);
		}
		else {
			i++;
		}
	}

	if (os) {
		*os << endl;
	}

}


int cPeaksList::find(double mzratio, double fragmentmasserrortolerance) {
	int left, right, middle;
	for (int i = 0; i < (int)peaks.size(); i++) {
		left = 0;
		right = (int)peaks.size() - 1;
		while (left <= right) {
			middle = (left + right) / 2;
			if (isInPpmMassErrorTolerance(mzratio, peaks[middle].mzratio, fragmentmasserrortolerance)) {
				return middle;
			}
			if (mzratio < peaks[middle].mzratio) {
				right = middle - 1;
			}
			else {
				left = middle + 1;
			}
		}
	}
	return -1;
}


void cPeaksList::remove(int position) {
	if (position < (int)peaks.size()) {
		peaks.erase(peaks.begin() + position);	
	}
}


void cPeaksList::removeChargeVariants(int maximumcharge, double fragmentmasserrortolerance) {
	if (abs(maximumcharge) == 1) {
		return;
	}

	sortbyMass();

	double charged;
	for (int i = abs(maximumcharge); i > 1; i--) {
		for (int j = i - 1; j >= 1; j--) {
			for (int k = 0; k < (int)peaks.size(); k++) {
				if (peaks[k].removeme) {
					continue;
				}

				charged = charge(uncharge(peaks[k].mzratio, (maximumcharge > 0)?i:-i), (maximumcharge > 0)?j:-j);

				for (int m = k + 1; m < (int)peaks.size(); m++) {
					if (peaks[m].removeme) {
						continue;
					}

					if (isInPpmMassErrorTolerance(charged, peaks[m].mzratio, fragmentmasserrortolerance)) {
						peaks[k].removeme = true;
						break;
					}
				}
			}
		}
	}

	removeObsolete();
}


void cPeaksList::removeNeutralLoss(double loss, int maximumcharge, double fragmentmasserrortolerance) {
	sortbyMass();

	int found = -1;
	//for (int i = 1; i <= maximumcharge; i++) {
		for (int j = 0; j < (int)peaks.size(); j++) {
			found = find(peaks[j].mzratio + loss, fragmentmasserrortolerance);
			if ((found != -1) && (peaks[found].relativeintensity < peaks[j].relativeintensity)) {
				peaks[found].removeme = true;
			}
		}
	//}

	removeObsolete();
}


void cPeaksList::removeObsolete() {
	auto cmp = [](const cPeak& a, const cPeak& b) {
		return (a.removeme < b.removeme);
	};

	sort(peaks.begin(), peaks.end(), cmp);

	int items = 0;
	int size = (int)peaks.size();
	while ((items < size) && (peaks[size - items - 1].removeme)) {
		items++;
	}

	if (items > 0) {
		peaks.erase(peaks.begin() + size - items, peaks.end());
	}

	sortbyMass();
}


void cPeaksList::maxHighestPeaksInWindow(int maximumnumberofpeaksinwindow, double windowsize) {

	if ((peaks.size() == 0) || (maximumnumberofpeaksinwindow == 0) || (windowsize <= 0)) {
		return;
	}

	sortbyMass();

	int wincount = (int)(peaks.back().mzratio/windowsize) + 1;
	vector<vector<cPeak> > windows;
	windows.resize(wincount);

	// create windows of peaks
	for (int i = 0; i < (int)peaks.size(); i++) {
		windows[int(peaks[i].mzratio/windowsize)].push_back(peaks[i]);
	}

	// crop windows
	int peaksinwindow;
	for (int i = 0; i < wincount; i++) {
		sort(windows[i].begin(), windows[i].end(), compareRelativePeakIntensitiesDesc);
		peaksinwindow = min(maximumnumberofpeaksinwindow, (int)windows[i].size());
		windows[i].resize(peaksinwindow);
	}

	// merge windows
	peaks.clear();
	for (int i = 0; i < wincount; i++) {
		peaks.insert(peaks.end(), windows[i].begin(), windows[i].end());
	}

	sortbyMass();

}


double cPeaksList::getMaximumRelativeIntensityFromMZInterval(double minmz, double maxmz, bool hidematched, bool hideunmatched, ePeptideType peptidetype, bool hidescrambled) {
	double intensity = 0;
	for (int i = 0; i < (int)peaks.size(); i++) {

		// skip peaks which are out of range
		if ((peaks[i].mzratio < minmz) || (peaks[i].mzratio > maxmz)) {
			continue;
		}

		// skip unmatched peaks if selected
		if (hideunmatched && (peaks[i].matched <= 0)) {
			continue;
		}

		// skip matched peaks if selected
		if (hidematched && (peaks[i].matched > 0)) {
			continue;
		}

		// skip scrambled peaks if selected
		if ((peptidetype == cyclic) && hidescrambled && peaks[i].scrambled) {
			continue;
		}

		if (peaks[i].relativeintensity > intensity) {
			intensity = peaks[i].relativeintensity;
		}
	}
	return intensity;
}


double cPeaksList::getMaximumAbsoluteIntensityFromMZInterval(double minmz, double maxmz, bool hidematched, bool hideunmatched, ePeptideType peptidetype, bool hidescrambled) {
	double intensity = 0;
	for (int i = 0; i < (int)peaks.size(); i++) {
		// skip peaks which are out of range
		if ((peaks[i].mzratio < minmz) || (peaks[i].mzratio > maxmz)) {
			continue;
		}

		// skip unmatched peaks if selected
		if (hideunmatched && (peaks[i].matched <= 0)) {
			continue;
		}

		// skip matched peaks if selected
		if (hidematched && (peaks[i].matched > 0)) {
			continue;
		}

		// skip scrambled peaks if selected
		if ((peptidetype == cyclic) && hidescrambled && peaks[i].scrambled) {
			continue;
		}

		if (peaks[i].absoluteintensity > intensity) {
			intensity = peaks[i].absoluteintensity;
		}
	}
	return intensity;
}


double cPeaksList::getMaximumAbsoluteIntensity() {
	double intensity = 0;
	for (int i = 0; i < (int)peaks.size(); i++) {
		if (peaks[i].absoluteintensity > intensity) {
			intensity = peaks[i].absoluteintensity;
		}
	}
	return intensity;
}


void cPeaksList::setCoordinates(int x, int y) {
	this->x = x;
	this->y = y;
}


int cPeaksList::getCoordinateX() {
	return x;
}


int cPeaksList::getCoordinateY() {
	return y;
}


void cPeaksList::store(ofstream& os) {
	int size;

	size = (int)peaks.size();
	os.write((char *)&size, sizeof(int));

	for (int i = 0; i < (int)peaks.size(); i++) {
		peaks[i].store(os);
	}

	os.write((char *)&x, sizeof(int));
	os.write((char *)&y, sizeof(int));

	storeString(title, os);
}


void cPeaksList::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	peaks.resize(size);

	for (int i = 0; i < (int)peaks.size(); i++) {
		peaks[i].load(is);
	}

	is.read((char *)&x, sizeof(int));
	is.read((char *)&y, sizeof(int));

	loadString(title, is);
}


string& cPeaksList::getTitle() {
	return title;
}


void cPeaksList::setTitle(string& title) {
	this->title = title;
}


void cPeaksList::reducePeakDescriptions(unordered_map<string, int>& peakdesctoid) {
	//static int test = 0;
	int descid;
	for (int i = 0; i < (int)peaks.size(); i++) {
		//test++;
		if (peakdesctoid.count(peaks[i].description) > 0) {
			peaks[i].descriptionid = peakdesctoid[peaks[i].description];
		}
		else {
			descid = (int)peakdesctoid.size();
			peaks[i].descriptionid = descid;
			peakdesctoid.insert(make_pair(peaks[i].description, descid));
		}
		peaks[i].description.clear();
	}
	//cout << "desc: " << test << " " << peakdesctoid.size() << endl;
}


void cPeaksList::reduceIsotopeFormulaDescriptions(unordered_map<string, int>& isotopeformuladesctoid) {
	//static int test = 0;
	int descid;
	for (int i = 0; i < (int)peaks.size(); i++) {
		//test++;
		if (isotopeformuladesctoid.count(peaks[i].isotopeformula) > 0) {
			peaks[i].isotopeformulaid = isotopeformuladesctoid[peaks[i].isotopeformula];
		}
		else {
			descid = (int)isotopeformuladesctoid.size();
			peaks[i].isotopeformulaid = descid;
			isotopeformuladesctoid.insert(make_pair(peaks[i].isotopeformula, descid));
		}
		peaks[i].isotopeformula.clear();
	}
	//cout << "isotope: " << test << " " << isotopeformuladesctoid.size() << endl;
}


void cPeaksList::fillOrderIDs() {
	int size = (int)peaks.size();
	for (int i = 0; i < size; i++) {
		peaks[i].orderid = i;
	}
}

