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
		if ((peaklist.size() > 0) || stream.good()) {
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
		if ((peaklist.size() > 0) || stream.good()) {
			peaklists.push_back(peaklist);
		}
	}
}


int cPeakListSeries::loadFromProfileApexStream(string& filename, ifstream &stream, ifstream &titlestream, double fwhm, cMainThread* os, bool& terminatecomputation) {
	*os << "Loading the apex data, spectrum no. : ";

	string s;
	string title;
	int count = 0;
	ofstream mgfofstream;

	char tempstring[1024];
	string peaksstring;
	peaksstring.reserve(25000000);

	int strip;

	string mgfname = filename.substr(0, (int)filename.size() - 8) + ".";
	stringstream ss;
	ss << mgfname << setw(10) << setfill('0') << 0 << ".mgf";
	mgfofstream.open(ss.str());

	while (stream.good() && !(strstr(s.c_str(),"M/Z"))) {
		getline(stream,s);
	}
	
	while (stream.good()) {
		cPeaksList peaklist;
		peaklist.loadFromBAFStream(stream);
		if ((peaklist.size() > 0) || stream.good()) {

			if ((count + 1) % 100 == 0) {
				*os << count + 1 << " ";
			}
			if ((count + 1) % 2500 == 0) {
				*os << endl;
			}

			getline(titlestream, title);

			mgfofstream << "BEGIN IONS" << endl;
			mgfofstream << "TITLE=" << title << endl;
			mgfofstream << "SCAN=" << to_string(count + 1) << endl;
			mgfofstream << "PEPMASS=1" << endl;
			mgfofstream << "RTINSECONDS=1" << endl;
			mgfofstream << "CHARGE=1+" << endl << endl;

			peaksstring.clear();
			for (int ii = 0; ii < peaklist.size(); ii++) {
				sprintf_s(tempstring, "%f %f\n\0", peaklist[ii].mzratio, peaklist[ii].absoluteintensity);
				peaksstring.append(tempstring);
			}
			mgfofstream << peaksstring;
			mgfofstream << "END IONS" << endl << endl;

			if (((count + 1) % 10 == 0) && (count > 0)) {
				mgfofstream.close();
				stringstream ss;
				strip = (count + 1) / 10;
				ss << mgfname << setw(10) << setfill('0') << strip << ".mgf";
				mgfofstream.open(ss.str());
			}

			cPeaksList emptypeaklist;
			emptypeaklist.setTitle(title);
			peaklists.push_back(emptypeaklist);
			count++;

			if (terminatecomputation) {
				peaklists.clear();
				mgfofstream.close();
				return -1;
			}

		}
	}

	mgfofstream.close();

	*os << "ok" << endl << "Total number of spectra: " << count << endl << endl << "Converting raw data to peaklists... ";

	string command = "External\\windows\\raw2peaks.bat \"" + mgfname + "\" " + to_string(fwhm);
	if (system(command.c_str()) != 0) {
		peaklists.clear();
		return -2;
	}

	*os << "ok" << endl << endl;

	ifstream mgfifstream;
	int oldpeaklistscount = (int)peaklists.size();
	vector<cPeaksList> temppeaklists;
	mgfname += "mgf";
	mgfifstream.open(mgfname);
	while (mgfifstream.good() && ((int)temppeaklists.size() < oldpeaklistscount)) {
		cPeaksList peaklist;
		peaklist.loadFromMGFStream(mgfifstream);
		temppeaklists.push_back(peaklist);
	}
	mgfifstream.close();

	if (oldpeaklistscount != (int)temppeaklists.size()) {
		peaklists.clear();
		return -2;
	}

	for (int i = 0; i < (int)temppeaklists.size(); i++) {
		temppeaklists[i].setTitle(peaklists[i].getTitle());
	}

	peaklists = temppeaklists;

	//QFile::remove(mgfname.c_str());

	return 0;
}


int cPeakListSeries::loadFromMZMLStream(string& mzmlfilename, ifstream &mzmlstream, double fwhm, eModeType mode, cMainThread* os, bool& terminatecomputation) {
	*os << "Loading the mzML file, spectrum no. : ";
	
	cMzML mzml;
	int resultcode = mzml.parse(mzmlfilename, peaklists, mode, os, terminatecomputation);
	if (resultcode != 0) {
		peaklists.clear();
		return resultcode;
	}

	if (mzml.hasProfileSpectra()) {

		ifstream mgfifstream;
		string convertedmzmlfilename;
		string mgfname = mzmlfilename.substr(0, (int)mzmlfilename.size() - 4);

		*os << "ok" << endl << "Total number of spectra: " << peaklists.size() << endl << endl << "Converting raw data to peaklists... ";

		string s;
		#if OS_TYPE == UNX
			s = installdir.toStdString() + "External/linux/raw2peaks.sh " + mgfname + "," + to_string(fwhm);
		#else
			#if OS_TYPE == OSX
				s = installdir.toStdString() + "External/macosx/raw2peaks.sh " + mgfname + "," + to_string(fwhm);
			#else		
				s = "External\\windows\\raw2peaks.bat \"" + mgfname + "\" " + to_string(fwhm);
			#endif
		#endif

		if (system(s.c_str()) != 0) {
			peaklists.clear();
			return -2;
		}

		*os << "ok" << endl << endl;

		int oldpeaklistscount = (int)peaklists.size();
		vector<cPeaksList> temppeaklists;
		mgfname += "mgf";
		mgfifstream.open(mgfname);
		while (mgfifstream.good() && ((int)temppeaklists.size() < oldpeaklistscount)) {
			cPeaksList peaklist;
			peaklist.loadFromMGFStream(mgfifstream);
			temppeaklists.push_back(peaklist);
		}
		mgfifstream.close();

		if (oldpeaklistscount != (int)temppeaklists.size()) {
			peaklists.clear();
			return -2;
		}

		for (int i = 0; i < (int)temppeaklists.size(); i++) {
			temppeaklists[i].setTitle(peaklists[i].getTitle());
		}

		peaklists = temppeaklists;

		//QFile::remove(mgfname.c_str());

	}
	else {

		*os << "ok" << endl << endl;

	}

	return 0;
}


int cPeakListSeries::loadFromIMZMLStream(string& imzmlfilename, ifstream &ibdstream, double fwhm, int& defaultmaxx, int& defaultmaxy, int& pixelsize, eVendorType& vendor, cMainThread* os, bool& terminatecomputation) {
	*os << "Loading the imzML file... ";

	cImzML imzml;
	if (imzml.parse(imzmlfilename, defaultmaxx, defaultmaxy, pixelsize, vendor)) {
		return -3;
	}

	*os << "ok" << endl << endl;

	ofstream mgfofstream;
	ifstream mgfifstream;
	string mgfname;
	string convertedimzmlfilename;
	string convertedibdfilename;

	char tempstring[1024];
	string peaksstring;
	peaksstring.reserve(25000000);

	int strip;
	bool rawdata = imzml.hasProfileSpectra();
	bool use_64bit_float_mz_precision = imzml.use64BitMzPrecision();
	bool use_64bit_float_intensity_precision = imzml.use64BitIntensityPrecision();

	if (rawdata) {
		mgfname = imzmlfilename.substr(0, (int)imzmlfilename.size() - 5);
		stringstream ss;
		ss << mgfname << setw(10) << setfill('0') << 0 << ".mgf";
		mgfofstream.open(ss.str());
	}

	*os << "Loading the ibd file, spectrum no. : ";

	for (int i = 0; i < (int)imzml.getItems().size(); i++) {
		if ((i + 1) % 100 == 0) {
			*os << i + 1 << " ";
		}
		if ((i + 1) % 2500 == 0) {
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
		peaklist.loadFromIBDStream(imzml.getItems()[i], ibdstream, use_64bit_float_mz_precision, use_64bit_float_intensity_precision);
		if (!rawdata) {
			peaklists.push_back(peaklist);
		}

		if (rawdata) {
			peaksstring.clear();
			for (int j = 0; j < peaklist.size(); j++) {
				sprintf_s(tempstring, "%f %f\n\0", peaklist[j].mzratio, peaklist[j].absoluteintensity);
				peaksstring.append(tempstring);
			}
			mgfofstream << peaksstring;
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

	*os << " ok" << endl << "Total number of spectra: " << imzml.getItems().size() << endl << endl;

	if (rawdata) {

		mgfofstream.close();
		
		*os << "Converting raw data to peaklists... ";

		string s;
		#if OS_TYPE == UNX
			s = installdir.toStdString() + "External/linux/raw2peaks.sh " + mgfname + "," + to_string(fwhm);
		#else
			#if OS_TYPE == OSX
				s = installdir.toStdString() + "External/macosx/raw2peaks.sh " + mgfname + "," + to_string(fwhm);
			#else		
				s = "External\\windows\\raw2peaks.bat \"" + mgfname + "\" " + to_string(fwhm);
			#endif
		#endif

		if (system(s.c_str()) != 0) {
			peaklists.clear();
			return -2;
		}

		*os << "ok" << endl << endl;

		mgfname += "mgf";
		mgfifstream.open(mgfname);
		while (mgfifstream.good() && (peaklists.size() < imzml.getItems().size())) {
			cPeaksList peaklist;
			peaklist.loadFromMGFStream(mgfifstream);
			peaklists.push_back(peaklist);
		}
		mgfifstream.close();

		if (imzml.getItems().size() != peaklists.size()) {
			peaklists.clear();
			return -2;
		}
		
		QFile::remove(mgfname.c_str());

		for (int i = 0; i < (int)imzml.getItems().size(); i++) {
			peaklists[i].setCoordinates(imzml.getItems()[i].x, imzml.getItems()[i].y);
			peaklists[i].setTitle(imzml.getItems()[i].title);
		}

		convertedimzmlfilename = imzmlfilename.substr(0, (int)imzmlfilename.size() - 6);
		convertedibdfilename = convertedimzmlfilename + "_converted_fwhm_" + to_string(fwhm) + ".ibd";
		convertedimzmlfilename += "_converted_fwhm_";
		convertedimzmlfilename += to_string(fwhm);
		convertedimzmlfilename += ".imzML";

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
			peaklists[i].storeToIBDStream(ofibdstream, use_64bit_float_mz_precision, use_64bit_float_intensity_precision);
		}
			
		ofibdstream.close();
		
		imzml.updateRawDataToPeakList(peaklists, convertedibdfilename);
		imzml.write(convertedimzmlfilename);

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
		if ((peaklist.size() > 0) || stream.good()) {
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

