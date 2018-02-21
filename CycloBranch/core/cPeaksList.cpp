#include "core/cPeaksList.h"
#include "gui/cMainThread.h"


bool comparePeakMasses(const cPeak& a, const cPeak& b) {
	return (a.mzratio < b.mzratio);
}


bool comparePeakIntensitiesDesc(const cPeak& a, const cPeak& b) {
	return (a.intensity > b.intensity);
}


bool comparePeakRemoveMeFlag(const cPeak& a, const cPeak& b) {
	return (a.removeme < b.removeme);
}


bool isInPpmMassErrorTolerance(double experimentalmass, double theoreticalmass, double tolerance) {
	double errortolerance = experimentalmass*tolerance*0.000001f;
	return abs(experimentalmass - theoreticalmass) <= errortolerance;
}


double ppmError(double experimentalmass, double theoreticalmass) {
	return abs(experimentalmass - theoreticalmass)/experimentalmass*1000000.0f;
}


cPeaksList::cPeaksList() {
	clear();
}


cPeaksList::cPeaksList(const cPeaksList& peakslist) {
	*this = peakslist;
}


cPeaksList& cPeaksList::operator=(const cPeaksList& peakslist) {
	peaks = peakslist.peaks;
	return *this;
}


void cPeaksList::clear() {
	peaks.clear();
}


void cPeaksList::attach(cPeaksList& peaklist) {
	peaks.insert(peaks.end(), peaklist.peaks.begin(), peaklist.peaks.end());
	/*
	for (int i = 0; i < (int)peaklist.peaks.size(); i++) {
		peaks.push_back(peaklist.peaks[i]);
	}
	*/
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
		sscanf_s(s.c_str(), "%lf\t%lf", &p.mzratio, &p.intensity);
		if (!p.empty()) {
			peaks.push_back(p);
		}
	}
}


void cPeaksList::loadFromBAFStream(ifstream &stream) {
	string s;
	cPeak p;
	size_t pos;

	getline(stream,s);

	while (stream.good()) {
		getline(stream,s);

		// replaces commas with dots
		pos = s.find(',');
		while (pos != string::npos) {
			s.replace(pos, 1, ".");
			pos = s.find(',');
		}

		pos = s.find(':');
		if (pos != string::npos) {
			s.replace(pos, 1, "\t");
		}

		// load a row
		p.clear();
		sscanf_s(s.c_str(), "%lf\t%lf", &p.mzratio, &p.intensity);
		if (!p.empty()) {
			peaks.push_back(p);
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

	while (stream.good()) {
		getline(stream,s);
		if (!strstr(s.c_str(),"=")) {
			break;
		}
	}

	while (stream.good()) {
		getline(stream,s);
		if (s.size() > 0) {
			break;
		}
	}

	while ((stream.good()) && !(strstr(s.c_str(),"END IONS"))) {

		// replaces commas with dots
		pos = s.find(',');
		while (pos != string::npos) {
			s.replace(pos, 1, ".");
			pos = s.find(',');
		}

		// load a row
		p.clear();
		sscanf_s(s.c_str(), "%lf %lf", &p.mzratio, &p.intensity);
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

		if (peaks[i].intensity > 0) {
			s += " ";
			s += to_string(peaks[i].intensity);
		}

		if (peaks[i].matchdescription.compare("") != 0) {
			s += peaks[i].matchdescription + " ";
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
	if (limit == -1) {
		sort(peaks.begin(), peaks.end(), comparePeakMasses);
	}
	else {
		sort(peaks.begin(), peaks.begin() + limit, comparePeakMasses);
	}
}


void cPeaksList::sortbyIntensityDesc() {
	sort(peaks.begin(), peaks.end(), comparePeakIntensitiesDesc);
}


int cPeaksList::normalizeIntenzity() {
	double maximum = 0.0f;
	for (int i = 0; i < (int)peaks.size(); i++) {
		if (peaks[i].intensity > maximum) {
			maximum = peaks[i].intensity;
		}
	}
	if (maximum > 0) {
		for (int i = 0; i < (int)peaks.size(); i++) {
			peaks[i].intensity /= maximum;
			peaks[i].intensity *= 100.0f;
		}
	}
	else {
		return -1;
	}
	return 0;
}


void cPeaksList::cropIntenzity(double minimumrelativeintensitythreshold) {
	int i = 0;
	while (i < (int)peaks.size()) {
		if (peaks[i].intensity < minimumrelativeintensitythreshold) {
			peaks.erase(peaks.begin()+i);
		}
		else {
			i++;
		}
	}
}


void cPeaksList::cropMinimumMZRatio(double minimummz, double errortolerance) {
	while ((int)peaks.size() > 0) {
		if ((peaks[0].mzratio < minimummz) && (!isInPpmMassErrorTolerance(peaks[0].mzratio, minimummz, errortolerance))) {
			peaks.erase(peaks.begin());
		}
		else {
			break;
		}
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
				if (peaks[pos].intensity < peaks[i].intensity) {
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


void cPeaksList::findObsolete(double mzratio) {
	int left, right, middle;

	left = 0;
	right = (int)peaks.size() - 1;
	while (left <= right) {
		middle = (left + right) / 2;
		if (abs(mzratio - peaks[middle].mzratio) < 0.00000001) {
			while ((middle > 0) && (abs(mzratio - peaks[middle - 1].mzratio) < 0.00000001)) {
				middle--;
			}
			while ((middle < (int)peaks.size()) && (abs(mzratio - peaks[middle].mzratio) < 0.00000001)) {
				peaks[middle].removeme = true;
				middle++;
			}
			return;
		}
		if (mzratio < peaks[middle].mzratio) {
			right = middle - 1;
		}
		else {
			left = middle + 1;
		}
	}

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
			if ((found != -1) && (peaks[found].intensity < peaks[j].intensity)) {
				peaks[found].removeme = true;
			}
		}
	//}

	removeObsolete();
}


void cPeaksList::removeObsolete() {
	sort(peaks.begin(), peaks.end(), comparePeakRemoveMeFlag);

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
		sort(windows[i].begin(), windows[i].end(), comparePeakIntensitiesDesc);
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


double cPeaksList::getMaximumIntensityFromMZInterval(double minmz, double maxmz) {
	double intensity = 0;
	for (int i = 0; i < (int)peaks.size(); i++) {
		if ((peaks[i].mzratio < minmz) || (peaks[i].mzratio > maxmz)) {
			continue;
		}
		if (peaks[i].intensity > intensity) {
			intensity = peaks[i].intensity;
		}
	}
	return intensity;
}


void cPeaksList::store(ofstream& os) {
	int size;

	size = (int)peaks.size();
	os.write((char *)&size, sizeof(int));

	for (int i = 0; i < (int)peaks.size(); i++) {
		peaks[i].store(os);
	}
}


void cPeaksList::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	peaks.resize(size);

	for (int i = 0; i < (int)peaks.size(); i++) {
		peaks[i].load(is);
	}
}

