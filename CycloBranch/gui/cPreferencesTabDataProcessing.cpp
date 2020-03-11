#include "gui/cPreferencesTabDataProcessing.h"


cPreferencesTabDataProcessing::cPreferencesTabDataProcessing(cGlobalPreferences& globalpreferences, QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();

	labelbafprocessingmethod = new QLabel("BAF processing method : ");
	comboboxbafprocessingmethod = new QComboBox();
	comboboxbafprocessingmethod->addItem("CompassXport");
	comboboxbafprocessingmethod->addItem("CompassXtract");
	gridlayout->addWidget(labelbafprocessingmethod, 0, 0);
	gridlayout->addWidget(comboboxbafprocessingmethod, 0, 1);

	labelrawdataprocessingmethod = new QLabel("Peak Picking (non-vendor's file formats) : ");
	comboboxrawdataprocessingmethod = new QComboBox();
	comboboxrawdataprocessingmethod->addItem("OpenMS");
	comboboxrawdataprocessingmethod->addItem("Built-in");
	gridlayout->addWidget(labelrawdataprocessingmethod, 1, 0);
	gridlayout->addWidget(comboboxrawdataprocessingmethod, 1, 1);

	gridlayoutwidget = new QWidget();
	gridlayoutwidget->setLayout(gridlayout);

	defaultbutton = new QPushButton("Default");
	hlayout = new QHBoxLayout();
	hlayout->addWidget(defaultbutton);
	hlayout->addStretch(1);
	hwidget = new QWidget();
	hwidget->setLayout(hlayout);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(gridlayoutwidget);
	mainlayout->addWidget(hwidget);
	mainlayout->addStretch(1);

	setLayout(mainlayout);

	connect(defaultbutton, SIGNAL(released()), this, SLOT(defaultButtonReleased()));
}


void cPreferencesTabDataProcessing::setPreferences(cGlobalPreferences& globalpreferences) {
	globalpreferences.bafprocessingmethod = comboboxbafprocessingmethod->currentIndex();
	globalpreferences.rawdataprocessingmethod = comboboxrawdataprocessingmethod->currentIndex();
}


void cPreferencesTabDataProcessing::resetPreferences(cGlobalPreferences& globalpreferences) {
	comboboxbafprocessingmethod->setCurrentIndex(globalpreferences.bafprocessingmethod);
	comboboxrawdataprocessingmethod->setCurrentIndex(globalpreferences.rawdataprocessingmethod);
}


cPreferencesTabDataProcessing::~cPreferencesTabDataProcessing() {
	delete labelbafprocessingmethod;
	delete comboboxbafprocessingmethod;

	delete labelrawdataprocessingmethod;
	delete comboboxrawdataprocessingmethod;

	delete defaultbutton;
	delete hlayout;
	delete hwidget;

	delete gridlayout;
	delete gridlayoutwidget;

	delete mainlayout;
}


void cPreferencesTabDataProcessing::defaultButtonReleased() {
	cGlobalPreferences gp;
	comboboxbafprocessingmethod->setCurrentIndex(gp.bafprocessingmethod);
	comboboxrawdataprocessingmethod->setCurrentIndex(gp.rawdataprocessingmethod);
}

