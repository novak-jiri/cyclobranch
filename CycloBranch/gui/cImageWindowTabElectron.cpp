#include "gui/cImageWindowTabElectron.h"


cImageWindowTabElectron::cImageWindowTabElectron(QWidget *parent) : QWidget(parent) {
	gridlayout = new QGridLayout();

	labelstartfrom = new QLabel("Begin import at : ");

	comboboxstartfrom = new QComboBox();

	gridlayout->addWidget(labelstartfrom, 0, 0, 1, 1);
	gridlayout->addWidget(comboboxstartfrom, 0, 1, 1, 9);

	labelfilenames = new QLabel("TIF Images : ");

	listwidgetfilenames = new QListWidget();
	listwidgetfilenames->setSelectionMode(QAbstractItemView::NoSelection);

	buttonfilenames = new QPushButton("Browse");

	gridlayout->addWidget(labelfilenames, 1, 0, 1, 1);
	gridlayout->addWidget(listwidgetfilenames, 1, 1, 1, 9);
	gridlayout->addWidget(buttonfilenames, 2, 9, 1, 1);

	setLayout(gridlayout);

	connect(buttonfilenames, SIGNAL(released()), this, SLOT(browseButtonReleased()));
}


void cImageWindowTabElectron::setFormValues(QString& lastdirmicroscopyimage, int numberoflayers) {
	this->lastdirmicroscopyimage = &lastdirmicroscopyimage;

	QString layername = "Microscopy Image ";
	QString tmpname;
	comboboxstartfrom->clear();
	for (int i = 0; i < numberoflayers - 4; i++) {
		tmpname = layername + QVariant(i + 1).toString();
		comboboxstartfrom->insertItem(i, tmpname);
	}

	listwidgetfilenames->clear();
	imagenames.clear();
}


void cImageWindowTabElectron::getFormValues(int& index, QStringList& filenames) {
	index = comboboxstartfrom->currentIndex();
	filenames = imagenames;
}


cImageWindowTabElectron::~cImageWindowTabElectron() {
	delete labelstartfrom;
	delete comboboxstartfrom;

	delete labelfilenames;
	delete listwidgetfilenames;
	delete buttonfilenames;

	delete gridlayout;
}


void cImageWindowTabElectron::browseButtonReleased() {
	QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select Images..."), *lastdirmicroscopyimage, tr("Images (*.tif *.tiff)"));
	if (!filenames.isEmpty()) {
		listwidgetfilenames->clear();
		for (auto& it : filenames) {
			listwidgetfilenames->addItem(it);
		}
		imagenames = filenames;
	}
}

