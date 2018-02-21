/**
	\file cPeaksList.h
	\brief The representation of a peak list.
*/


#ifndef _CPEAKSLIST_H
#define _CPEAKSLIST_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "core/cPeak.h"


using namespace std;


class cMainThread;


/**
	\brief Compare two peaks by mz ratio.
	\param a first peak
	\param b second peak
	\retval bool true when the mz ratio of the peak \a a is lower than the mz ratio of the peak \a b
*/
bool comparePeakMasses(cPeak& a, cPeak& b);


/**
	\brief Compare two peaks by intensities.
	\param a first peak
	\param b second peak
	\retval bool true when the intensity of the peak \a a is bigger than the intensity of the peak \a b
*/
bool comparePeakIntensitiesDesc(cPeak& a, cPeak& b);


/**
	\brief Check whether two masses are inside a ppm mass error tolerance.
	\param experimentalmass first mass
	\param theoreticalmass second mass
	\param tolerance fragment mass error tolerance
	\retval bool true when masses are inside a mass tolerance
*/
bool isInPpmMassErrorTolerance(double experimentalmass, double theoreticalmass, double tolerance);


/**
	\brief Compute a ppm error between two masses.
	\param experimentalmass first mass
	\param theoreticalmass second mass
	\retval double ppm error
*/
double ppmError(double experimentalmass, double theoreticalmass);


/**
	\brief The class representing a peak list.
*/
class cPeaksList {

	vector<cPeak> peaks;

public:


	/**
		\brief The constructor.
	*/ 
	cPeaksList();


	/**
		\brief Clear the peak list.
	*/ 
	void clear();


	/**
		\brief Attach a peak list.
		\param peaklist peaklist which will be attached
	*/ 
	void attach(cPeaksList& peaklist);


	/**
		\brief Add a peak.
		\param peak peak which will be added
	*/ 
	void add(cPeak& peak);


	/**
		\brief Load the spectrum from a plain text stream (tab separated values m/z ratio and intensity).
		\param stream source stream
	*/ 
	void loadFromPlainTextStream(ifstream &stream);


	/**
		\brief Load the spectrum from a .csv file converted from a .baf file by CompassXport.
		\param stream source stream
	*/ 
	void loadFromBAFStream(ifstream &stream);


	/**
		\brief Load the spectrum from a .mgf file.
		\param stream source stream
	*/ 
	void loadFromMGFStream(ifstream &stream);


	/**
		\brief The number of peaks.
		\retval int number of peaks
	*/ 
	int size();


	/**
		\brief Print the spectrum.
		\param htmlterminatelines true when html line endings must used
		\retval string string with a printed spectrum
	*/ 
	string print(bool htmlterminatelines = false);


	/**
		\brief Sort the peaks by mass.
		\param limit sort first \a limit peaks only; if \a limit == -1 then all peaks are sorted
	*/ 
	void sortbyMass(int limit = -1);


	/**
		\brief Normalize intensities of peaks.
		\retval int 0 == success, -1 == maximum intensity <= 0
	*/ 
	int normalizeIntenzity();


	/**
		\brief Crop relative intensities lower than a threshold.
		\param minimumrelativeintensitythreshold minimum threshold of relative intensity
	*/ 
	void cropIntenzity(double minimumrelativeintensitythreshold);


	/**
		\brief Crop mz ratio lower than a threshold.
		\param minimummz minimum threshold of mz ratio
	*/ 
	void cropMinimumMZRatio(double minimummz);


	/**
		\brief Crop mz ratio bigger than a threshold.
		\param maximummz maximum threshold of mz ratio
	*/ 
	void cropMaximumMZRatio(double maximummz);


	/**
		\brief Overloaded operator [].
		\param position position of a peak in the peak list
		\retval cPeak reference to the peak
	*/ 
	cPeak& operator[](int position);


	/**
		\brief Resize the peak list.
		\param size number of peaks
	*/ 
	void resize(int size);


	/**
		\brief Get the minimum mz ratio.
		\retval double minimum mz ratio
	*/ 
	double getMinimumMZRatio();


	/**
		\brief Get the maximum mz ratio.
		\retval double maximum mz ratio
	*/ 
	double getMaximumMZRatio();


	/**
		\brief Remove isotopic peaks.
		\param maximumcharge charge of the precursor
		\param fragmentmasserrortolerance mz error tolerance
		\param os reference to a main thread of the application (stream, respectively)
	*/ 
	void removeIsotopes(int maximumcharge, double fragmentmasserrortolerance, cMainThread* os = 0);


	/**
		\brief Get a position of a peak with specified mzratio.
		\param mzratio of searched peak
		\param fragmentmasserrortolerance mz error tolerance
		\retval int position; -1 when the peak is not found
	*/ 
	int find(double mzratio, double fragmentmasserrortolerance);


	/**
		\brief Erase all peaks having specified m/z ratio.
		\param mzratio mz ratio of peaks which will be erased
	*/ 
	void eraseAll(double mzratio);


	/**
		\brief Remove a peak.
		\param position position of a peak
	*/ 
	void remove(int position);


	/**
		\brief Perform a deconvolution.
		\param maximumcharge charge of the precursor
		\param fragmentmasserrortolerance mz error tolerance
	*/ 
	void removeChargeVariants(int maximumcharge, double fragmentmasserrortolerance);


	/**
		\brief Remove neutral losses.
		\param loss mass change
		\param maximumcharge charge of the precursor
		\param fragmentmasserrortolerance mz error tolerance
	*/ 
	void removeNeutralLoss(double loss, int maximumcharge, double fragmentmasserrortolerance);


	/**
		\brief Remove peaks having set up the flag \a removeme == true.
	*/ 
	void removeObsolete();


	/**
		\brief Keep only n highest peaks in a window of a specified size.
		\param maximumnumberofpeaksinwindow maximum number of peaks in a window
		\param windowsize size of a window
	*/ 
	void maxHighestPeaksInWindow(int maximumnumberofpeaksinwindow, double windowsize);


	/**
		\brief Get a maximum intensity of a peak in an interval of mz ratios.
		\param minmz minimum mz ratio
		\param maxmz maximum mz ratio
		\retval double intensity
	*/ 
	double getMaximumIntensityFromMZInterval(double minmz, double maxmz);


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