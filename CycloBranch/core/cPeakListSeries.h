/**
	\file cPeakListSeries.h
	\brief The representation of a series of peaklists.
*/


#ifndef _CPEAKLISTSERIES_H
#define _CPEAKLISTSERIES_H

#include <sstream>

#include "core/cPeaksList.h"


class cMainThread;


/**
	\brief The class representing a series of peaklists.
*/
class cPeakListSeries {

	vector<cPeaksList> peaklists;

public:


	/**
		\brief The constructor.
	*/ 
	cPeakListSeries();


	/**
		\brief The copy constructor.
		\param peaklistseries reference to a cPeakListSeries
	*/ 
	cPeakListSeries(const cPeakListSeries& peaklistseries);


	/**
		\brief Overloaded operator=.
		\param peaklistseries reference to a cPeakListSeries
	*/ 
	cPeakListSeries& operator=(const cPeakListSeries& peaklistseries);


	/**
		\brief Overloaded operator [].
		\param position position of a peaklist
		\retval cPeaksList reference to a peaklist
	*/ 
	cPeaksList& operator[](int position);


	/**
		\brief Clear the peaklist series.
	*/ 
	void clear();


	/**
		\brief Add a peaklist.
		\param peaklist peaklist which will be added
	*/ 
	void addPeakList(cPeaksList& peaklist);


	/**
		\brief Load the series of peaklists from a plain text stream.
		\param stream source stream
	*/ 
	void loadFromPlainTextStream(ifstream &stream);


	/**
		\brief Load the series of peaklists from a .csv file converted from a .baf file by CompassXport.
		\param stream source stream
	*/ 
	void loadFromBAFStream(ifstream &stream);


	/**
		\brief Load the series of peaklists from a .csv file converted from apex data by CompassXport.
		\param filename filename
		\param stream source stream
		\param titlestream title stream
		\param fwhm FWHM
		\param os pointer to the main thread of the application (output stream)
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = success; -1 = aborted by user, -2 = error
	*/ 
	int loadFromProfileApexStream(string& filename, ifstream &stream, ifstream &titlestream, double fwhm, cMainThread* os, bool& terminatecomputation);


	/**
		\brief Load the series of peaklists from .mzML file.
		\param mzmlfilename mzML filename
		\param mzmlstream mzML file stream
		\param fwhm FWHM
		\param mode program mode
		\param os pointer to the main thread of the application (output stream)
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = success; -1 = aborted by user, -2 = error
	*/ 
	int loadFromMZMLStream(string& mzmlfilename, ifstream &mzmlstream, double fwhm, eModeType mode, cMainThread* os, bool& terminatecomputation);


	/**
		\brief Load the series of peaklists from .imzML file.
		\param imzmlfilename imzML filename
		\param ibdstream ibd binary file stream
		\param fwhm FWHM
		\param defaultmaxx max count of pixel x - parsed from imzML file
		\param defaultmaxy max count of pixel y - parsed from imzML file
		\param defaultpixelsizex pixel size x - parsed from imzML file
		\param defaultpixelsizey pixel size y - parsed from imzML file
		\param vendor vendor type
		\param os pointer to the main thread of the application (output stream)
		\param terminatecomputation reference to a variable determining that the computation must be stopped
		\retval int 0 = success; -1 = aborted by user, -2 = error
	*/ 
	int loadFromIMZMLStream(string& imzmlfilename, ifstream &ibdstream, double fwhm, int& defaultmaxx, int& defaultmaxy, int& defaultpixelsizex, int& defaultpixelsizey, eVendorType& vendor, cMainThread* os, bool& terminatecomputation);


	/**
		\brief Load a spot list from a stream.
		\param stream source stream
	*/ 
	void loadSpotList(ifstream &stream);


	/**
		\brief Load the series of peaklists from a .mgf file.
		\param stream source stream
	*/ 
	void loadFromMGFStream(ifstream &stream);


	/**
		\brief The number of peaklists in the series.
		\retval int number of peaklists in the series
	*/ 
	int size();


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);

};


#endif
