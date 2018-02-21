#include "core/cPeakListSeries.h"

#include <QFile>
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
	*os << endl << endl << "Parsing the imzML file..." << endl;

	cImzML imzml;
	imzml.parse(imzmlfilename);

	ofstream mgfofstream;
	ifstream mgfifstream;
	string mgfname;
	string convertedimzmlfilename;
	string convertedibdfilename;
	int strip;
	bool rawdata = imzml.hasProfileSpectra();

	if (rawdata) {
		mgfname = imzmlfilename.substr(0, (int)imzmlfilename.size() - 5);
		stringstream ss;
		ss << mgfname << setw(10) << setfill('0') << 0 << ".mgf";
		mgfofstream.open(ss.str());
	}

	*os << "Loading the peaklist no. : " << endl;

	for (int i = 0; i < (int)imzml.getItems().size(); i++) {
		*os << i + 1 << " ";
		if ((i + 1) % 25 == 0) {
			*os << endl;
		}

		if (rawdata) {
			mgfofstream << "BEGIN IONS" << endl;
			mgfofstream << "TITLE=" << endl;
			mgfofstream << "SCAN=" << to_string(i + 1) << endl;
			mgfofstream << "PEPMASS=1" << endl;
			mgfofstream << "RTINSECONDS=1" << endl;
			mgfofstream << "CHARGE=1+" << endl << endl;
		}

		cPeaksList peaklist;
		peaklist.loadFromIBDStream(imzml.getItems()[i], ibdstream);
		if ((peaklist.size() > 0) && !rawdata) {
			peaklists.push_back(peaklist);
		}

		if (rawdata) {
			for (int j = 0; j < peaklist.size(); j++) {
				mgfofstream << peaklist[j].mzratio << " " << peaklist[j].intensity << endl;
			}
			mgfofstream << "END IONS" << endl << endl;

			if (((i + 1) % 100 == 0) && (i > 0)) {
				mgfofstream.close();
				stringstream ss;
				strip = (i + 1) / 100;
				ss << mgfname << setw(10) << setfill('0') << strip << ".mgf";
				mgfofstream.open(ss.str());
			}
		}

		if (terminatecomputation) {
			peaklists.clear();
			return -1;
		}
	}

	*os << " ok" << endl << endl;

	if (rawdata) {

		mgfofstream.close();
		
		*os << endl << "Converting raw data to peaklists... ";

		string s;
		#if OS_TYPE == UNX
			s = installdir.toStdString() + "External/linux/raw2peaks.sh " + mgfname;
		#else
			#if OS_TYPE == OSX
				s = installdir.toStdString() + "External/macosx/raw2peaks.sh " + mgfname;
			#else		
				s = "External\\windows\\raw2peaks.bat \"" + mgfname + "\"";
			#endif
		#endif

		if (system(s.c_str()) != 0) {
			return -2;
		}

		mgfname += "mgf";
		mgfifstream.open(mgfname);
		while (mgfifstream.good() && (peaklists.size() < imzml.getItems().size())) {
			cPeaksList peaklist;
			peaklist.loadFromMGFStream(mgfifstream);
			peaklists.push_back(peaklist);
		}
		mgfifstream.close();
		QFile::remove(mgfname.c_str());

		if (imzml.getItems().size() != peaklists.size()) {
			return -2;
		}
		
		for (int i = 0; i < (int)imzml.getItems().size(); i++) {
			peaklists[i].setCoordinates(imzml.getItems()[i].x, imzml.getItems()[i].y);
		}

		imzml.updateRawDataToPeakList(peaklists);

		convertedimzmlfilename = imzmlfilename.substr(0, (int)imzmlfilename.size() - 6);
		convertedibdfilename = convertedimzmlfilename + "_converted.ibd";
		convertedimzmlfilename += "_converted.imzML";
		imzml.write(convertedimzmlfilename);

		ofstream ofibdstream;
		char headerbyte;
		ofibdstream.open(convertedibdfilename, std::ofstream::binary);
			
		// copy ibd header
		ibdstream.seekg(0, ibdstream.beg);
		for (int i = 0; i < 16; i++) {
			ibdstream.read((char*)&headerbyte, 1);
			ofibdstream.write((char*)&headerbyte, 1);
		}
			
		// write peaklists
		for (int i = 0; i < (int)peaklists.size(); i++) {
			peaklists[i].storeToIBDStream(ofibdstream);
		}
			
		ofibdstream.close();
		
		int i = 0;
		while (i < (int)peaklists.size()) {
			if (peaklists[i].size() == 0) {
				peaklists.erase(peaklists.begin() + i);
			}
			else {
				i++;
			}
		}
	}

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

