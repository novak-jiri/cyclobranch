/**
	\file cGlobalPreferences.h
	\brief The representation of a set of peptide sequence candidates.
*/


#ifndef _CGLOBALPREFERENCES_H
#define _CGLOBALPREFERENCES_H

#include "core/utilities.h"
#include "core/cFragmentIons.h"
#include "core/cSummaryFormula.h"
#include <QString>
#include <QSettings>
#include <QFile>


/**
	\brief The struct for definition of a ion type.
*/
struct cIonType {

	/**
		\brief Name of the ion.
	*/
	string name;
	

	/**
		\brief Formula of the ion.
	*/
	string formula;


	/**
		\brief Mass difference of the ion from the neutral molecule.
	*/
	double massdifference;


	/**
		\brief Charge of the ion.
	*/
	bool positive;


	/**
		\brief The multiplier n in [nM + H]+.
	*/ 
	int multiplier;


	/**
		\brief The default constructor.
	*/
	cIonType() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/
	void clear() {
		name = "[M+H]+";
		formula = "H+";
		massdifference = H - e;
		positive = true;
		multiplier = 1;
	}


	/**
		\brief Store the structure into an output stream.
		\param os an output stream
	*/
	void store(ofstream& os) {
		storeString(name, os);
		storeString(formula, os);
		os.write((char *)&massdifference, sizeof(double));
		os.write((char *)&positive, sizeof(bool));
		os.write((char *)&multiplier, sizeof(int));
	}


	/**
		\brief Load the structure from an input stream.
		\param is an input stream
	*/
	void load(ifstream& is) {
		loadString(name, is);
		loadString(formula, is);
		is.read((char *)&massdifference, sizeof(double));
		is.read((char *)&positive, sizeof(bool));
		is.read((char *)&multiplier, sizeof(int));
	}

};


/**
	\brief The class storing global preferences.
*/
class cGlobalPreferences {

	QString settingsfile;

public:

	vector<cIonType> customions;

	int linebafprocessingmethod;
	int profilebafprocessingmethod;

	int rawdataprocessingmethod;

	QString settingsdefaultdir;
	QString peaklistsdefaultdir;
	QString blocksdefaultdir;
	QString sequencesdefaultdir;
	QString modificationsdefaultdir;
	QString openopticalimagedir;
	QString openhistologyimagedir;
	QString openmicroscopyimagedir;
	QString resultsdefaultdir;
	QString exportcsvdefaultdir;
	QString exporthtmldefaultdir;
	QString exportimagedefaultdir;

	QString bookmarkname1;
	QString bookmarkurl1;

	QString bookmarkname2;
	QString bookmarkurl2;

	QString bookmarkname3;
	QString bookmarkurl3;

	QString bookmarkname4;
	QString bookmarkurl4;

	QString bookmarkname5;
	QString bookmarkurl5;

	QString bookmarkname6;
	QString bookmarkurl6;

	QString bookmarkname7;
	QString bookmarkurl7;

	QString bookmarkname8;
	QString bookmarkurl8;

	QString bookmarkname9;
	QString bookmarkurl9;

	QString bookmarkname10;
	QString bookmarkurl10;

	bool viewbookmarks;


	/**
		\brief The constructor.
	*/
	cGlobalPreferences();


	/**
		\brief Set default ion types.
	*/
	void setDefaultIonTypes();


	/**
		\brief Set default data processing methods.
	*/
	void setDefaultDataProcessingMethods();


	/**
		\brief Set default directories.
	*/
	void setDefaultDirectories();


	/**
		\brief Set default URLs.
	*/
	void setDefaultURLs();


	/**
		\brief Load settings.
	*/
	void loadSettings();


	/**
		\brief Save settings.
	*/
	void saveSettings();


};


#endif

