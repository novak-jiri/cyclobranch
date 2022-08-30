#include "core/cGlobalPreferences.h"


cGlobalPreferences::cGlobalPreferences() {
	settingsfile = "CycloBranch.ini";

	#if OS_TYPE != WIN
		char *homedir = getenv("HOME");
		if (homedir != 0) {
			settingsfile = homedir;
			settingsfile += "/CycloBranch.ini";
		}
	#endif

	setDefaultIonTypes();
	setDefaultDataProcessingMethods();
	setDefaultDirectories();
	setDefaultURLs();
}


void cGlobalPreferences::loadSettings() {
	if (!QFile(settingsfile).exists()) {
		return;
	}

	QSettings settings(settingsfile, QSettings::IniFormat);

	int customionscount = settings.value("customionscount", 0).toInt();
	QString customionvalue;

	customions.clear();
	for (int i = 0; i < customionscount; i++) {
		cIonType ion;

		customionvalue = "customion_name_" + QVariant(i).toString();
		ion.name = settings.value(customionvalue, ion.name.c_str()).toString().toStdString();

		customionvalue = "customion_formula_" + QVariant(i).toString();
		ion.formula = settings.value(customionvalue, ion.formula.c_str()).toString().toStdString();

		customionvalue = "customion_positive_" + QVariant(i).toString();
		ion.positive = settings.value(customionvalue, (int)ion.positive).toInt() == 0 ? false : true;

		customionvalue = "customion_multiplier_" + QVariant(i).toString();
		ion.multiplier = settings.value(customionvalue, ion.multiplier).toInt();

		cSummaryFormula formula;
		formula.setFormula(ion.formula);
		ion.massdifference = formula.getMass();

		customions.push_back(ion);
	}

	if (settings.contains("linebafprocessingmethod")) {
		linebafprocessingmethod = settings.value("linebafprocessingmethod").toInt();
	}

	if (settings.contains("profilebafprocessingmethod")) {
		profilebafprocessingmethod = settings.value("profilebafprocessingmethod").toInt();
	}

	if (settings.contains("rawdataprocessingmethod")) {
		rawdataprocessingmethod = settings.value("rawdataprocessingmethod").toInt();
	}

	if (settings.contains("settingsdefaultdir")) {
		settingsdefaultdir = settings.value("settingsdefaultdir").toString();
	}

	if (settings.contains("peaklistsdefaultdir")) {
		peaklistsdefaultdir = settings.value("peaklistsdefaultdir").toString();
	}

	if (settings.contains("blocksdefaultdir")) {
		blocksdefaultdir = settings.value("blocksdefaultdir").toString();
	}

	if (settings.contains("sequencesdefaultdir")) {
		sequencesdefaultdir = settings.value("sequencesdefaultdir").toString();
	}

	if (settings.contains("modificationsdefaultdir")) {
		modificationsdefaultdir = settings.value("modificationsdefaultdir").toString();
	}

	if (settings.contains("openopticalimagedir")) {
		openopticalimagedir = settings.value("openopticalimagedir").toString();
	}

	if (settings.contains("openhistologyimagedir")) {
		openhistologyimagedir = settings.value("openhistologyimagedir").toString();
	}

	if (settings.contains("openmicroscopyimagedir")) {
		openmicroscopyimagedir = settings.value("openmicroscopyimagedir").toString();
	}

	if (settings.contains("resultsdefaultdir")) {
		resultsdefaultdir = settings.value("resultsdefaultdir").toString();
	}

	if (settings.contains("exportcsvdefaultdir")) {
		exportcsvdefaultdir = settings.value("exportcsvdefaultdir").toString();
	}

	if (settings.contains("exporthtmldefaultdir")) {
		exporthtmldefaultdir = settings.value("exporthtmldefaultdir").toString();
	}

	if (settings.contains("exportimagedefaultdir ")) {
		exportimagedefaultdir = settings.value("exportimagedefaultdir ").toString();
	}

	if (settings.contains("bookmarkname1")) {
		bookmarkname1 = settings.value("bookmarkname1").toString();
	}

	if (settings.contains("bookmarkurl1")) {
		bookmarkurl1 = settings.value("bookmarkurl1").toString();
	}

	if (settings.contains("bookmarkname2")) {
		bookmarkname2 = settings.value("bookmarkname2").toString();
	}

	if (settings.contains("bookmarkurl2")) {
		bookmarkurl2 = settings.value("bookmarkurl2").toString();
	}

	if (settings.contains("bookmarkname3")) {
		bookmarkname3 = settings.value("bookmarkname3").toString();
	}

	if (settings.contains("bookmarkurl3")) {
		bookmarkurl3 = settings.value("bookmarkurl3").toString();
	}

	if (settings.contains("bookmarkname4")) {
		bookmarkname4 = settings.value("bookmarkname4").toString();
	}

	if (settings.contains("bookmarkurl4")) {
		bookmarkurl4 = settings.value("bookmarkurl4").toString();
	}

	if (settings.contains("bookmarkname5")) {
		bookmarkname5 = settings.value("bookmarkname5").toString();
	}

	if (settings.contains("bookmarkurl5")) {
		bookmarkurl5 = settings.value("bookmarkurl5").toString();
	}

	if (settings.contains("bookmarkname6")) {
		bookmarkname6 = settings.value("bookmarkname6").toString();
	}

	if (settings.contains("bookmarkurl6")) {
		bookmarkurl6 = settings.value("bookmarkurl6").toString();
	}

	if (settings.contains("bookmarkname7")) {
		bookmarkname7 = settings.value("bookmarkname7").toString();
	}

	if (settings.contains("bookmarkurl7")) {
		bookmarkurl7 = settings.value("bookmarkurl7").toString();
	}

	if (settings.contains("bookmarkname8")) {
		bookmarkname8 = settings.value("bookmarkname8").toString();
	}

	if (settings.contains("bookmarkurl8")) {
		bookmarkurl8 = settings.value("bookmarkurl8").toString();
	}

	if (settings.contains("bookmarkname9")) {
		bookmarkname9 = settings.value("bookmarkname9").toString();
	}

	if (settings.contains("bookmarkurl9")) {
		bookmarkurl9 = settings.value("bookmarkurl9").toString();
	}

	if (settings.contains("bookmarkname10")) {
		bookmarkname10 = settings.value("bookmarkname10").toString();
	}

	if (settings.contains("bookmarkurl10")) {
		bookmarkurl10 = settings.value("bookmarkurl10").toString();
	}

	viewbookmarks = settings.value("viewbookmarks", 0).toInt() == 0 ? false : true;
}


void cGlobalPreferences::saveSettings() {
	QSettings settings(settingsfile, QSettings::IniFormat);
	settings.clear();

	settings.setValue("customionscount", (int)customions.size());

	QString customionvalue;
	for (int i = 0; i < (int)customions.size(); i++) {
		customionvalue = "customion_name_" + QVariant(i).toString();
		settings.setValue(customionvalue, customions[i].name.c_str());

		customionvalue = "customion_formula_" + QVariant(i).toString();
		settings.setValue(customionvalue, customions[i].formula.c_str());

		customionvalue = "customion_positive_" + QVariant(i).toString();
		settings.setValue(customionvalue, (int)customions[i].positive);

		customionvalue = "customion_multiplier_" + QVariant(i).toString();
		settings.setValue(customionvalue, customions[i].multiplier);
	}

	settings.setValue("linebafprocessingmethod", linebafprocessingmethod);
	settings.setValue("profilebafprocessingmethod", profilebafprocessingmethod);

	settings.setValue("rawdataprocessingmethod", rawdataprocessingmethod);

	settings.setValue("settingsdefaultdir", settingsdefaultdir);
	settings.setValue("peaklistsdefaultdir", peaklistsdefaultdir);
	settings.setValue("blocksdefaultdir", blocksdefaultdir);
	settings.setValue("sequencesdefaultdir", sequencesdefaultdir);
	settings.setValue("modificationsdefaultdir", modificationsdefaultdir);
	settings.setValue("openopticalimagedir", openopticalimagedir);
	settings.setValue("openhistologyimagedir", openhistologyimagedir);
	settings.setValue("openmicroscopyimagedir", openmicroscopyimagedir);
	settings.setValue("resultsdefaultdir", resultsdefaultdir);
	settings.setValue("exportcsvdefaultdir", exportcsvdefaultdir);
	settings.setValue("exporthtmldefaultdir", exporthtmldefaultdir);
	settings.setValue("exportimagedefaultdir", exportimagedefaultdir);

	settings.setValue("bookmarkname1", bookmarkname1);
	settings.setValue("bookmarkurl1", bookmarkurl1);

	settings.setValue("bookmarkname2", bookmarkname2);
	settings.setValue("bookmarkurl2", bookmarkurl2);

	settings.setValue("bookmarkname3", bookmarkname3);
	settings.setValue("bookmarkurl3", bookmarkurl3);

	settings.setValue("bookmarkname4", bookmarkname4);
	settings.setValue("bookmarkurl4", bookmarkurl4);

	settings.setValue("bookmarkname5", bookmarkname5);
	settings.setValue("bookmarkurl5", bookmarkurl5);

	settings.setValue("bookmarkname6", bookmarkname6);
	settings.setValue("bookmarkurl6", bookmarkurl6);

	settings.setValue("bookmarkname7", bookmarkname7);
	settings.setValue("bookmarkurl7", bookmarkurl7);

	settings.setValue("bookmarkname8", bookmarkname8);
	settings.setValue("bookmarkurl8", bookmarkurl8);

	settings.setValue("bookmarkname9", bookmarkname9);
	settings.setValue("bookmarkurl9", bookmarkurl9);

	settings.setValue("bookmarkname10", bookmarkname10);
	settings.setValue("bookmarkurl10", bookmarkurl10);

	viewbookmarks ? settings.setValue("viewbookmarks", 1) : settings.setValue("viewbookmarks", 0);
}


void cGlobalPreferences::setDefaultIonTypes() {
	cIonType ion;

	customions.clear();

	/*
	cFragmentIons iondefinitions;

	int start = (int)ms_Hplus;
	int end = (int)ms_MGa4H;

	for (int i = start; i <= end; i++) {
		ion.clear();
		ion.name = iondefinitions[(eFragmentIonType)i].name.c_str();
		ion.formula = iondefinitions[(eFragmentIonType)i].summary.c_str();
		ion.positive = iondefinitions[(eFragmentIonType)i].positive;
		ion.multiplier = iondefinitions[(eFragmentIonType)i].multiplier;
		customions.push_back(ion);
	}
	*/

	// initialize ion [M+H]+
	ion.clear();
	ion.name = "[M+H]+";
	ion.formula = "H+";
	ion.massdifference = H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Na]+
	ion.clear();
	ion.name = "[M+Na]+";
	ion.formula = "Na+";
	ion.massdifference = Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+K]+
	ion.clear();
	ion.name = "[M+K]+";
	ion.formula = "K+";
	ion.massdifference = K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M-H]-
	ion.clear();
	ion.name = "[M-H]-";
	ion.formula = "H-1+-1";
	ion.massdifference = -H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Na-2H]-
	ion.clear();
	ion.name = "[M+Na-2H]-";
	ion.formula = "NaH-2+-1";
	ion.massdifference = Na - 2 * H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+K-2H]-
	ion.clear();
	ion.name = "[M+K-2H]-";
	ion.formula = "KH-2+-1";
	ion.massdifference = K - 2 * H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Fe-2H]+
	ion.clear();
	ion.name = "[M+Fe-2H]+";
	ion.formula = "FeH-3H+";
	ion.massdifference = Fe - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Fe-3H+Na]+
	ion.clear();
	ion.name = "[M+Fe-3H+Na]+";
	ion.formula = "FeH-3Na+";
	ion.massdifference = Fe - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Fe-3H+K]+
	ion.clear();
	ion.name = "[M+Fe-3H+K]+";
	ion.formula = "FeH-3K+";
	ion.massdifference = Fe - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [2M+Fe-2H]+
	ion.clear();
	ion.name = "[2M+Fe-2H]+";
	ion.formula = "FeH-3H+";
	ion.massdifference = Fe - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 2;
	customions.push_back(ion);

	// initialize ion [2M+Fe-3H+Na]+
	ion.clear();
	ion.name = "[2M+Fe-3H+Na]+";
	ion.formula = "FeH-3Na+";
	ion.massdifference = Fe - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 2;
	customions.push_back(ion);

	// initialize ion [2M+Fe-3H+K]+
	ion.clear();
	ion.name = "[2M+Fe-3H+K]+";
	ion.formula = "FeH-3K+";
	ion.massdifference = Fe - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 2;
	customions.push_back(ion);

	// initialize ion [3M+Fe-2H]+
	ion.clear();
	ion.name = "[3M+Fe-2H]+";
	ion.formula = "FeH-3H+";
	ion.massdifference = Fe - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [3M+Fe-3H+Na]+
	ion.clear();
	ion.name = "[3M+Fe-3H+Na]+";
	ion.formula = "FeH-3Na+";
	ion.massdifference = Fe - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [3M+Fe-3H+K]+
	ion.clear();
	ion.name = "[3M+Fe-3H+K]+";
	ion.formula = "FeH-3K+";
	ion.massdifference = Fe - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [3M+2Fe-5H]+
	ion.clear();
	ion.name = "[3M+2Fe-5H]+";
	ion.formula = "Fe2H-6H+";
	ion.massdifference = 2 * Fe - 6 * H + H - e;
	ion.positive = true;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [3M+2Fe-6H+Na]+
	ion.clear();
	ion.name = "[3M+2Fe-6H+Na]+";
	ion.formula = "Fe2H-6Na+";
	ion.massdifference = 2 * Fe - 6 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [3M+2Fe-6H+K]+
	ion.clear();
	ion.name = "[3M+2Fe-6H+K]+";
	ion.formula = "Fe2H-6K+";
	ion.massdifference = 2 * Fe - 6 * H + K - e;
	ion.positive = true;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [M+Fe-4H]-
	ion.clear();
	ion.name = "[M+Fe-4H]-";
	ion.formula = "FeH-3H-1+-1";
	ion.massdifference = Fe - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [2M+Fe-4H]-
	ion.clear();
	ion.name = "[2M+Fe-4H]-";
	ion.formula = "FeH-3H-1+-1";
	ion.massdifference = Fe - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 2;
	customions.push_back(ion);

	// initialize ion [3M+Fe-4H]-
	ion.clear();
	ion.name = "[3M+Fe-4H]-";
	ion.formula = "FeH-3H-1+-1";
	ion.massdifference = Fe - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [3M+2Fe-7H]-
	ion.clear();
	ion.name = "[3M+2Fe-7H]-";
	ion.formula = "Fe2H-6H-1+-1";
	ion.massdifference = 2 * Fe - 6 * H - H + e;
	ion.positive = false;
	ion.multiplier = 3;
	customions.push_back(ion);

	// initialize ion [M+NH4]+
	ion.clear();
	ion.name = "[M+NH4]+";
	ion.formula = "NH4+";
	ion.massdifference = H + NH3 - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cl]-
	ion.clear();
	ion.name = "[M+Cl]-";
	ion.formula = "Cl+-1";
	ion.massdifference = Cl + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M]+
	ion.clear();
	ion.name = "[M]+";
	ion.formula = "+";
	ion.massdifference = -e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M]-
	ion.clear();
	ion.name = "[M]-";
	ion.formula = "+-1";
	ion.massdifference = e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion Li+
	ion.clear();
	ion.name = "[M+Li]+";
	ion.formula = "Li+";
	ion.massdifference = Li - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mg-H]+
	ion.clear();
	ion.name = "[M+Mg-H]+";
	ion.formula = "MgH-2H+";
	ion.massdifference = Mg - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mg-2H+Na]+
	ion.clear();
	ion.name = "[M+Mg-2H+Na]+";
	ion.formula = "MgH-2Na+";
	ion.massdifference = Mg - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mg-2H+K]+
	ion.clear();
	ion.name = "[M+Mg-2H+K]+";
	ion.formula = "MgH-2K+";
	ion.massdifference = Mg - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mg-3H]-
	ion.clear();
	ion.name = "[M+Mg-3H]-";
	ion.formula = "MgH-2H-1+-1";
	ion.massdifference = Mg - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Al-2H]+
	ion.clear();
	ion.name = "[M+Al-2H]+";
	ion.formula = "AlH-3H+";
	ion.massdifference = Al - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Al-3H+Na]+
	ion.clear();
	ion.name = "[M+Al-3H+Na]+";
	ion.formula = "AlH-3Na+";
	ion.massdifference = Al - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Al-3H+K]+
	ion.clear();
	ion.name = "[M+Al-3H+K]+";
	ion.formula = "AlH-3K+";
	ion.massdifference = Al - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Al-4H]-
	ion.clear();
	ion.name = "[M+Al-4H]-";
	ion.formula = "AlH-3H-1+-1";
	ion.massdifference = Al - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Si-3H]+
	ion.clear();
	ion.name = "[M+Si-3H]+";
	ion.formula = "SiH-4H+";
	ion.massdifference = Si - 4 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Si-4H+Na]+
	ion.clear();
	ion.name = "[M+Si-4H+Na]+";
	ion.formula = "SiH-4Na+";
	ion.massdifference = Si - 4 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Si-4H+K]+
	ion.clear();
	ion.name = "[M+Si-4H+K]+";
	ion.formula = "SiH-4K+";
	ion.massdifference = Si - 4 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Si-5H]-
	ion.clear();
	ion.name = "[M+Si-5H]-";
	ion.formula = "SiH-4H-1+-1";
	ion.massdifference = Si - 4 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ca-H]+
	ion.clear();
	ion.name = "[M+Ca-H]+";
	ion.formula = "CaH-2H+";
	ion.massdifference = Ca - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ca-2H+Na]+
	ion.clear();
	ion.name = "[M+Ca-2H+Na]+";
	ion.formula = "CaH-2Na+";
	ion.massdifference = Ca - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ca-2H+K]+
	ion.clear();
	ion.name = "[M+Ca-2H+K]+";
	ion.formula = "CaH-2K+";
	ion.massdifference = Ca - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ca-3H]-
	ion.clear();
	ion.name = "[M+Ca-3H]-";
	ion.formula = "CaH-2H-1+-1";
	ion.massdifference = Ca - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Sc-2H]+
	ion.clear();
	ion.name = "[M+Sc-2H]+";
	ion.formula = "ScH-3H+";
	ion.massdifference = Sc - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Sc-3H+Na]+
	ion.clear();
	ion.name = "[M+Sc-3H+Na]+";
	ion.formula = "ScH-3Na+";
	ion.massdifference = Sc - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Sc-3H+K]+
	ion.clear();
	ion.name = "[M+Sc-3H+K]+";
	ion.formula = "ScH-3K+";
	ion.massdifference = Sc - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Sc-4H]-
	ion.clear();
	ion.name = "[M+Sc-4H]-";
	ion.formula = "ScH-3H-1+-1";
	ion.massdifference = Sc - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cr-2H]+
	ion.clear();
	ion.name = "[M+Cr-2H]+";
	ion.formula = "CrH-3H+";
	ion.massdifference = Cr - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cr-3H+Na]+
	ion.clear();
	ion.name = "[M+Cr-3H+Na]+";
	ion.formula = "CrH-3Na+";
	ion.massdifference = Cr - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cr-3H+K]+
	ion.clear();
	ion.name = "[M+Cr-3H+K]+";
	ion.formula = "CrH-3K+";
	ion.massdifference = Cr - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cr-4H]-
	ion.clear();
	ion.name = "[M+Cr-4H]-";
	ion.formula = "CrH-3H-1+-1";
	ion.massdifference = Cr - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mn-H]+
	ion.clear();
	ion.name = "[M+Mn-H]+";
	ion.formula = "MnH-2H+";
	ion.massdifference = Mn - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mn-2H+Na]+
	ion.clear();
	ion.name = "[M+Mn-2H+Na]+";
	ion.formula = "MnH-2Na+";
	ion.massdifference = Mn - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mn-2H+K]+
	ion.clear();
	ion.name = "[M+Mn-2H+K]+";
	ion.formula = "MnH-2K+";
	ion.massdifference = Mn - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Mn-3H]-
	ion.clear();
	ion.name = "[M+Mn-3H]-";
	ion.formula = "MnH-2H-1+-1";
	ion.massdifference = Mn - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Co-H]+
	ion.clear();
	ion.name = "[M+Co-H]+";
	ion.formula = "CoH-2H+";
	ion.massdifference = Co - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Co-2H+Na]+
	ion.clear();
	ion.name = "[M+Co-2H+Na]+";
	ion.formula = "CoH-2Na+";
	ion.massdifference = Co - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Co-2H+K]+
	ion.clear();
	ion.name = "[M+Co-2H+K]+";
	ion.formula = "CoH-2K+";
	ion.massdifference = Co - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Co-3H]-
	ion.clear();
	ion.name = "[M+Co-3H]-";
	ion.formula = "CoH-2H-1+-1";
	ion.massdifference = Co - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ni-H]+
	ion.clear();
	ion.name = "[M+Ni-H]+";
	ion.formula = "NiH-2H+";
	ion.massdifference = Ni - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ni-2H+Na]+
	ion.clear();
	ion.name = "[M+Ni-2H+Na]+";
	ion.formula = "NiH-2Na+";
	ion.massdifference = Ni - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ni-2H+K]+
	ion.clear();
	ion.name = "[M+Ni-2H+K]+";
	ion.formula = "NiH-2K+";
	ion.massdifference = Ni - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ni-3H]-
	ion.clear();
	ion.name = "[M+Ni-3H]-";
	ion.formula = "NiH-2H-1+-1";
	ion.massdifference = Ni - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cu-H]+
	ion.clear();
	ion.name = "[M+Cu-H]+";
	ion.formula = "CuH-2H+";
	ion.massdifference = Cu - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cu-2H+Na]+
	ion.clear();
	ion.name = "[M+Cu-2H+Na]+";
	ion.formula = "CuH-2Na+";
	ion.massdifference = Cu - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cu-2H+K]+
	ion.clear();
	ion.name = "[M+Cu-2H+K]+";
	ion.formula = "CuH-2K+";
	ion.massdifference = Cu - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Cu-3H]-
	ion.clear();
	ion.name = "[M+Cu-3H]-";
	ion.formula = "CuH-2H-1+-1";
	ion.massdifference = Cu - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Zn-H]+
	ion.clear();
	ion.name = "[M+Zn-H]+";
	ion.formula = "ZnH-2H+";
	ion.massdifference = Zn - 2 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Zn-2H+Na]+
	ion.clear();
	ion.name = "[M+Zn-2H+Na]+";
	ion.formula = "ZnH-2Na+";
	ion.massdifference = Zn - 2 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Zn-2H+K]+
	ion.clear();
	ion.name = "[M+Zn-2H+K]+";
	ion.formula = "ZnH-2K+";
	ion.massdifference = Zn - 2 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Zn-3H]-
	ion.clear();
	ion.name = "[M+Zn-3H]-";
	ion.formula = "ZnH-2H-1+-1";
	ion.massdifference = Zn - 2 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ga-2H]+
	ion.clear();
	ion.name = "[M+Ga-2H]+";
	ion.formula = "GaH-3H+";
	ion.massdifference = Ga - 3 * H + H - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ga-3H+Na]+
	ion.clear();
	ion.name = "[M+Ga-3H+Na]+";
	ion.formula = "GaH-3Na+";
	ion.massdifference = Ga - 3 * H + Na - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ga-3H+K]+
	ion.clear();
	ion.name = "[M+Ga-3H+K]+";
	ion.formula = "GaH-3K+";
	ion.massdifference = Ga - 3 * H + K - e;
	ion.positive = true;
	ion.multiplier = 1;
	customions.push_back(ion);

	// initialize ion [M+Ga-4H]-
	ion.clear();
	ion.name = "[M+Ga-4H]-";
	ion.formula = "GaH-3H-1+-1";
	ion.massdifference = Ga - 3 * H - H + e;
	ion.positive = false;
	ion.multiplier = 1;
	customions.push_back(ion);
}


void cGlobalPreferences::setDefaultDataProcessingMethods() {
	linebafprocessingmethod = 0;
	profilebafprocessingmethod = 0;

	rawdataprocessingmethod = 0;
}


void cGlobalPreferences::setDefaultDirectories() {
	#if OS_TYPE == WIN
		settingsdefaultdir = "./Settings/";
		peaklistsdefaultdir = "./PeakLists/";
		blocksdefaultdir = "./BrickDatabases/";
		sequencesdefaultdir = "./SequenceDatabases/";
		modificationsdefaultdir = "./Modifications/";
		openopticalimagedir = "./";
		openhistologyimagedir = "./";
		openmicroscopyimagedir = "./";
		resultsdefaultdir = "./";
		exportcsvdefaultdir = "./";
		exporthtmldefaultdir = "./";
		exportimagedefaultdir = "./";
	#else
		settingsdefaultdir = installdir + "Settings/";
		peaklistsdefaultdir = installdir + "PeakLists/";
		blocksdefaultdir = installdir + "BrickDatabases/";
		sequencesdefaultdir = installdir + "SequenceDatabases/";
		modificationsdefaultdir = installdir + "Modifications/";
		openopticalimagedir = "./";
		openhistologyimagedir = "./";
		openmicroscopyimagedir = "./";
		resultsdefaultdir = "./";
		exportcsvdefaultdir = "./";
		exporthtmldefaultdir = "./";
		exportimagedefaultdir = "./";
	#endif
}


void cGlobalPreferences::setDefaultURLs() {
	bookmarkname1 = appname;
	bookmarkurl1 = "https://ms.biomed.cas.cz/cyclobranch/";

	bookmarkname2 = "MassSpecBlocks";
	bookmarkurl2 = "https://ms.biomed.cas.cz/msb/";

	bookmarkname3 = "PubChem";
	bookmarkurl3 = "https://pubchem.ncbi.nlm.nih.gov/";

	bookmarkname4 = "ChemSpider";
	bookmarkurl4 = "https://www.chemspider.com/";

	bookmarkname5 = "ChEBI";
	bookmarkurl5 = "https://www.ebi.ac.uk/chebi/";

	bookmarkname6 = "Lipid Maps";
	bookmarkurl6 = "https://lipidmaps.org/";

	bookmarkname7 = "Reaxys";
	bookmarkurl7 = "https://www.reaxys.com/";

	bookmarkname8 = "Norine";
	bookmarkurl8 = "https://bioinfo.lifl.fr/norine/";

	bookmarkname9 = "";
	bookmarkurl9 = "";

	bookmarkname10 = "";
	bookmarkurl10 = "";

	viewbookmarks = false;
}

