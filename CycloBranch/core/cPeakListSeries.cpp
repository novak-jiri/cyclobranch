#include "core/cPeakListSeries.h"

#include "gui/cMainThread.h"


cPeakListSeries::cPeakListSeries() {
	clear();
}


cPeakListSeries::cPeakListSeries(const cPeakListSeries& peaklistseries) {
	*this = peaklistseries;
}


cPeakListSeries& cPeakListSeries::operator=(const cPeakListSeries& peaklistseries) {
	peaklists = peaklistseries.peaklists;
	return *this;
}


cPeaksList& cPeakListSeries::operator[](int position) {
	return peaklists[position];
}


void cPeakListSeries::clear() {
	peaklists.clear();
}


void cPeakListSeries::addPeakList(cPeaksList& peaklist) {
	peaklists.push_back(peaklist);
}


void cPeakListSeries::loadFromPlainTextStream(ifstream &stream) {
	while (stream.good()) {
		cPeaksList peaklist;
		peaklist.loadFromPlainTextStream(stream);
		
		if (peaklist.size() > 0) {
			peaklists.push_back(peaklist);
		}
	}
}


void cPeakListSeries::loadFromBAFStream(ifstream &stream) {
	string s;
	while (stream.good() && !(strstr(s.c_str(),"M/Z"))) {
		getline(stream,s);
	}
	
	while (stream.good()) {
		cPeaksList peaklist;
		peaklist.loadFromBAFStream(stream);
		
		if (peaklist.size() > 0) {
			peaklists.push_back(peaklist);
		}
	}
}


int cPeakListSeries::loadFromIMZMLStream(string& imzmlfilename, ifstream &ibdstream, double minimumrelativeintensitythreshold, cMainThread* os, bool& terminatecomputation) {
	*os << "Parsing the imzML file ..." << endl;

	cImzML imzml;
	imzml.parse(imzmlfilename);

	*os << "Loading the peaklist no. : " << endl;

	for (int i = 0; i < (int)imzml.getItems().size(); i++) {
		*os << i + 1 << " ";
		if ((i + 1) % 25 == 0) {
			*os << endl;
		}

		cPeaksList peaklist;
		peaklist.loadFromIBDStream(imzml.getItems()[i], ibdstream);
		if (peaklist.size() > 0) {
			peaklist.normalizeIntenzity(); // to do - redundant operation
			peaklist.cropIntenzity(minimumrelativeintensitythreshold); // to do - redundant operation
			peaklists.push_back(peaklist);
		}

		if (terminatecomputation) {
			peaklists.clear();
			return -1;
		}
	}

	*os << " ok" << endl;

	return 0;
}


void cPeakListSeries::loadSpotList(ifstream &stream) {
	string s;
	vector<string> spots;
	while (stream.good()) {
		getline(stream,s);
		if (!s.empty()) {
			spots.push_back(s);
		}
	}

	if (peaklists.size() != spots.size()) {
		return;
	}
	
	size_t px, py;
	int x, y;
	for (int i = 0; i < (int)peaklists.size(); i++) {
		px = spots[i].find('X');
		py = spots[i].find('Y');

		if ((px == string::npos) || (py == string::npos)) {
			return;
		}

		x = stoi(spots[i].substr(0, py).substr(px + 1));
		y = stoi(spots[i].substr(py + 1));

		peaklists[i].setCoordinates(x, y);
	}
}


void cPeakListSeries::loadFromMGFStream(ifstream &stream) {
	while (stream.good()) {
		cPeaksList peaklist;
		peaklist.loadFromMGFStream(stream);
		
		if (peaklist.size() > 0) {
			peaklists.push_back(peaklist);
		}
	}
}


int cPeakListSeries::size() {
	return (int)peaklists.size();
}


void cPeakListSeries::store(ofstream& os) {
	int size;

	size = (int)peaklists.size();
	os.write((char *)&size, sizeof(int));

	for (int i = 0; i < size; i++) {
		peaklists[i].store(os);
	}
}


void cPeakListSeries::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	peaklists.resize(size);

	for (int i = 0; i < size; i++) {
		peaklists[i].load(is);
	}
}

