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

};


/**
	\brief The class storing global preferences.
*/
class cGlobalPreferences {

	QString settingsfile;

public:

	vector<cIonType> customions;

	int bafprocessingmethod;
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

	QString bookmarkurl1;
	QString bookmarkurl2;


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

