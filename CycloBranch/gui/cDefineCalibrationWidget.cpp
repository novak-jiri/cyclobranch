#include "gui/cDefineCalibrationWidget.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QKeyEvent>
#include <QIcon>
#include <QMessageBox>
#include <QFormLayout>


cDefineCalibrationWidget::cDefineCalibrationWidget() {
	setWindowTitle("Define Samples and Calibration Curve...");
	setWindowIcon(QIcon(":/images/icons/features.png"));

	vlayout = new QVBoxLayout();
	filelistwidget = new QWidget();

	gridlayout = new QGridLayout();

	uselabel = new QLabel("Use: ");
	filelabel = new QLabel("Filename: ");
	datatypelabel = new QLabel("Data Type: ");
	grouplabel = new QLabel("Group: ");
	concentrationlabel = new QLabel("Concentration: ");
	timelabel = new QLabel("Time: ");

	filelistwidget->setLayout(gridlayout);

	compoundslabel = new QLabel("Ion Types: ");
	compoundslist = new QListWidget();
	compoundslist->setSelectionMode(QAbstractItemView::MultiSelection);
	compoundslist->setMinimumHeight(250);
	compoundslayout = new QVBoxLayout();
	compoundswidget = new QWidget();

	compoundslayout->addWidget(compoundslabel);
	compoundslayout->addWidget(compoundslist);
	compoundswidget->setLayout(compoundslayout);

	equationcombobox = new QComboBox();
	equationcombobox->setToolTip("Select the type of equation to estimate its parameters.");
	equationcombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	equationcombobox->addItem("Automatic mode (y = a*x)");
	equationcombobox->addItem("Automatic mode (y = a*x + b)");
	equationcombobox->addItem("Enter parameters manually");

	connect(equationcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(equationChanged(int)));

	doublespinboxa = new QDoubleSpinBox();
	doublespinboxa->setRange(-DBL_MAX, DBL_MAX);
	doublespinboxa->setDecimals(6);
	doublespinboxa->setValue(1);
	doublespinboxa->setDisabled(true);
	doublespinboxa->setToolTip("Enter the value of 'a' in the calibration curve 'y = a*x + b'.");

	doublespinboxb = new QDoubleSpinBox();
	doublespinboxb->setRange(-DBL_MAX, DBL_MAX);
	doublespinboxb->setDecimals(6);
	doublespinboxb->setValue(1);
	doublespinboxb->setDisabled(true);
	doublespinboxb->setToolTip("Enter the value of 'b' in the calibration curve 'y = a*x + b'.");

	eiccombobox = new QComboBox();
	eiccombobox->setToolTip("Select a method for construction of extracted ion chromatogram.\nHighest Peak = Intensity of the highest peak of a given ion in every spectrum is used.\nAll isotopes = Sums of intensities of all isotopes of a given ion in every spectrum are used.");
	eiccombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	eiccombobox->addItem("Highest Peak (e.g., 12C)");
	eiccombobox->addItem("All Isotopes (e.g., sum of 12C + 13C + 13C2 + ...)");

	peakshapecombobox = new QComboBox();
	peakshapecombobox->setToolTip("Select chromatographic peak shape to be used for area and concentration calculations:\nGaussian = basic Gaussian function,\nex-Gaussian (tailing) = Exponentially Modified Gaussian for chromatographic peaks with tailing,\nex-Gaussian (fronting) = Exponentially Modified Gaussian for chromatographic peaks with fronting.");
	peakshapecombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	peakshapecombobox->addItem("Gaussian");
	peakshapecombobox->addItem("ex-Gaussian (tailing)");
	peakshapecombobox->addItem("ex-Gaussian (fronting)");

	standardcombobox = new QComboBox();
	standardcombobox->setToolTip("Select compound standard.");
	standardcombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	formlayout = new QFormLayout();
	formwidget = new QWidget();

	formlayout->addRow(tr("Equation: "), equationcombobox);
	formlayout->addRow(tr("a: "), doublespinboxa);
	formlayout->addRow(tr("b: "), doublespinboxb);
	formlayout->addRow(tr("Extracted Ion Chromatogram: "), eiccombobox);
	formlayout->addRow(tr("Peak Shape: "), peakshapecombobox);
	formlayout->addRow(tr("Standard: "), standardcombobox);
	formwidget->setLayout(formlayout);

	okbutton = new QPushButton("Ok");
	cancelbutton = new QPushButton("Cancel");
	buttonslayout = new QHBoxLayout();
	buttonswidget = new QWidget();

	buttonslayout->addStretch(1);
	buttonslayout->addWidget(okbutton);
	buttonslayout->addWidget(cancelbutton);

	buttonswidget->setLayout(buttonslayout);

	connect(okbutton, SIGNAL(released()), this, SLOT(okButtonReleased()));
	connect(cancelbutton, SIGNAL(released()), this, SLOT(cancelButtonReleased()));

	vlayout->addWidget(filelistwidget);
	vlayout->addWidget(compoundswidget);
	vlayout->addWidget(formwidget);
	vlayout->addWidget(buttonswidget);
	vlayout->addStretch(1);

	scrollwidget = new QWidget();
	scrollwidget->setLayout(vlayout);

	scrollarea = new QScrollArea();
	scrollarea->setWidgetResizable(true);
	scrollarea->setFrameShape(QFrame::NoFrame);
	scrollarea->setWidget(scrollwidget);

	scrolllayout = new QVBoxLayout();
	scrolllayout->addWidget(scrollarea);

	setLayout(scrolllayout);

	resize(750, defaultwinsizey);

	internalusedvector.clear();
	internalfilenames.clear();
	internaldatatypesvector.clear();
	internalgroupsvector.clear();
	internalconcentrationsvector.clear();
	internaltimesvector.clear();

	internalcompounds.clear();
	internalselectedionsvector.clear();

	internalequationtype = 0;
	internala = 1;
	internalb = 0;
	internaleictype = 0;
	internalpeakshape = 0;
	internalstandard = 0;
}


cDefineCalibrationWidget::~cDefineCalibrationWidget() {
	delete uselabel;
	delete filelabel;
	delete datatypelabel;
	delete grouplabel;
	delete concentrationlabel;
	delete timelabel;

	for (auto& it : checkboxes) {
		delete it;
	}

	for (auto& it : lineedits) {
		delete it;
	}

	for (auto& it : comboboxes) {
		delete it;
	}

	for (auto& it : groupnames) {
		delete it;
	}

	for (auto& it : doublespinboxes) {
		delete it;
	}

	for (auto& it : timespinboxes) {
		delete it;
	}

	delete gridlayout;
	delete filelistwidget;

	delete compoundslabel;
	delete compoundslist;
	delete compoundslayout;
	delete compoundswidget;

	delete equationcombobox;
	delete doublespinboxa;
	delete doublespinboxb;
	delete eiccombobox;
	delete peakshapecombobox;
	delete standardcombobox;

	delete formlayout;
	delete formwidget;

	delete okbutton;
	delete cancelbutton;
	delete buttonslayout;
	delete buttonswidget;

	delete vlayout;

	delete scrollwidget;
	delete scrollarea;
	delete scrolllayout;
}


void cDefineCalibrationWidget::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cDefineCalibrationWidget::store(ofstream& os) {
	storeIntVector(internalusedvector, os);
	storeStringVector(internalfilenames, os);
	storeIntVector(internaldatatypesvector, os);
	storeStringVector(internalgroupsvector, os);
	storeDoubleVector(internalconcentrationsvector, os);
	storeIntVector(internaltimesvector, os);

	storeStringVector(internalcompounds, os);
	storeIntVector(internalselectedionsvector, os);

	os.write((char *)&internalequationtype, sizeof(int));
	os.write((char *)&internaleictype, sizeof(int));
	os.write((char *)&internalpeakshape, sizeof(int));
	os.write((char *)&internalstandard, sizeof(int));

	os.write((char *)&internala, sizeof(double));
	os.write((char *)&internalb, sizeof(double));
}


void cDefineCalibrationWidget::load(ifstream& is, int fileversionpart1, int fileversionpart2, int fileversionpart3) {
	loadIntVector(internalusedvector, is);
	loadStringVector(internalfilenames, is);

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 18)) {
		loadIntVector(internaldatatypesvector, is);

		if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 27)) {
			loadStringVector(internalgroupsvector, is);
		}
		else {
			vector<int> tmpgroupsvector;
			loadIntVector(tmpgroupsvector, is);

			internalgroupsvector.clear();
			for (auto it : tmpgroupsvector) {
				internalgroupsvector.push_back(to_string(it));
			}
		}
	}
	else {
		internaldatatypesvector.clear();
		internaldatatypesvector.resize(internalusedvector.size(), 0);

		internalgroupsvector.clear();
		internalgroupsvector.resize(internalusedvector.size());

		for (int i = 0; i < (int)internalgroupsvector.size(); i++) {
			internalgroupsvector[i] = i + 1;
		}
	}

	loadDoubleVector(internalconcentrationsvector, is);

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 25)) {
		loadIntVector(internaltimesvector, is);
	}
	else {
		internaltimesvector.clear();
		internaltimesvector.resize(internalusedvector.size(), 0);
	}

	loadStringVector(internalcompounds, is);
	loadIntVector(internalselectedionsvector, is);

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 16)) {
		is.read((char *)&internalequationtype, sizeof(int));
		is.read((char *)&internaleictype, sizeof(int));
		is.read((char *)&internalpeakshape, sizeof(int));
		is.read((char *)&internalstandard, sizeof(int));
	}
	else {
		internalequationtype = 0;
		internaleictype = 0;
		internalpeakshape = 0;
		internalstandard = 0;
	}

	if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 19)) {
		is.read((char *)&internala, sizeof(double));
		is.read((char *)&internalb, sizeof(double));
	}
	else {
		internala = 1;
		internalb = 0;
	}
}


void cDefineCalibrationWidget::prepareWidget(vector<string>& filenames, vector<string>& compounds) {
	delete uselabel;
	delete filelabel;
	delete datatypelabel;
	delete grouplabel;
	delete concentrationlabel;
	delete timelabel;

	for (auto& it : checkboxes) {
		delete it;
	}

	for (auto& it : lineedits) {
		delete it;
	}

	for (auto& it : comboboxes) {
		delete it;
	}

	for (auto& it : groupnames) {
		delete it;
	}

	for (auto& it : doublespinboxes) {
		delete it;
	}

	for (auto& it : timespinboxes) {
		delete it;
	}

	checkboxes.clear();
	lineedits.clear();
	comboboxes.clear();
	groupnames.clear();
	doublespinboxes.clear();
	timespinboxes.clear();

	delete gridlayout;

	bool loadfiles = compareStringVectors(internalfilenames, filenames) &&
		(filenames.size() == internalusedvector.size()) &&
		(filenames.size() == internaldatatypesvector.size()) &&
		(filenames.size() == internalgroupsvector.size()) &&
		(filenames.size() == internalconcentrationsvector.size()) &&
		(filenames.size() == internaltimesvector.size());

	bool loadcompounds = compareStringVectors(internalcompounds, compounds) && (compounds.size() == internalselectedionsvector.size());

	if (!loadfiles) {
		internalusedvector.clear();
		internalfilenames = filenames;
		internaldatatypesvector.clear();
		internalgroupsvector.clear();
		internalconcentrationsvector.clear();
		internaltimesvector.clear();

		internalcompounds = compounds;
		internalselectedionsvector.clear();

		internalequationtype = 0;
		internala = 1;
		internalb = 0;
		internaleictype = 0;
		internalpeakshape = 0;
		internalstandard = 0;
	}
	else {
		if (!loadcompounds) {
			internalcompounds = compounds;
			internalselectedionsvector.clear();

			internalstandard = 0;
		}
	}

	gridlayout = new QGridLayout();
	filelistwidget->setLayout(gridlayout);

	uselabel = new QLabel("Use: ");
	filelabel = new QLabel("Filename: ");
	datatypelabel = new QLabel("Data Type: ");
	grouplabel = new QLabel("Group Name: ");
	concentrationlabel = new QLabel("Concentration: ");
	timelabel = new QLabel("Collection Time: ");

	gridlayout->addWidget(uselabel, 0, 0, 1, 1);
	gridlayout->addWidget(filelabel, 0, 1, 1, 10);
	gridlayout->addWidget(datatypelabel, 0, 11, 1, 2);
	gridlayout->addWidget(grouplabel, 0, 13, 1, 10);
	gridlayout->addWidget(concentrationlabel, 0, 23, 1, 1);
	gridlayout->addWidget(timelabel, 0, 24, 1, 1);

	int size = (int)filenames.size();
	for (int i = 0; i < size; i++) {
		QCheckBox* checkbox = new QCheckBox();
		checkbox->setChecked(true);
		checkboxes.push_back(checkbox);

		QLineEdit* lineedit = new QLineEdit();
		lineedit->setText(filenames[i].c_str());
		lineedit->setDisabled(true);
		lineedits.push_back(lineedit);

		QComboBox* combobox = new QComboBox();
		combobox->addItem("Sample");
		combobox->addItem("Calibration");
		comboboxes.push_back(combobox);

		connect(combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(dataTypeChanged(int)));

		QLineEdit* groupname = new QLineEdit();
		groupname->setText(QVariant(i + 1).toString());
		groupname->setToolTip("Enter a group name. Rows with the same group name form a group of technical replicates and the results are averaged.");
		groupnames.push_back(groupname);

		QDoubleSpinBox* doublespinbox = new QDoubleSpinBox();
		doublespinbox->setToolTip("Enter the concentration of compound in corresponding dataset. Units are dataset dependent. Multiple datasets can have the same value (results are averaged).");
		doublespinbox->setDecimals(3);
		doublespinbox->setRange(0, 1000000);
		doublespinbox->setSingleStep(1);
		doublespinboxes.push_back(doublespinbox);

		QSpinBox* timespinbox = new QSpinBox();
		timespinbox->setToolTip("Enter a number representing time. Items with the same number form a group of technical replicates and the results are averaged.");
		timespinbox->setRange(0, 1000000000);
		timespinbox->setSingleStep(1);
		timespinboxes.push_back(timespinbox);
		timespinbox->setValue(0);

		doublespinbox->setDisabled(true);

		gridlayout->addWidget(checkbox, i + 1, 0, 1, 1);
		gridlayout->addWidget(lineedit, i + 1, 1, 1, 10);
		gridlayout->addWidget(combobox, i + 1, 11, 1, 2);
		gridlayout->addWidget(groupname, i + 1, 13, 1, 10);
		gridlayout->addWidget(doublespinbox, i + 1, 23, 1, 1);
		gridlayout->addWidget(timespinbox, i + 1, 24, 1, 1);

		if (loadfiles) {
			checkbox->setChecked(internalusedvector[i]);
			combobox->setCurrentIndex(internaldatatypesvector[i]);
			groupname->setText(internalgroupsvector[i].c_str());
			doublespinbox->setValue(internalconcentrationsvector[i]);
			timespinbox->setValue(internaltimesvector[i]);
		}
		else {
			internalusedvector.push_back(checkbox->isChecked());
			internaldatatypesvector.push_back(combobox->currentIndex());
			internalgroupsvector.push_back(groupname->text().toStdString());
			internalconcentrationsvector.push_back(doublespinbox->value());
			internaltimesvector.push_back(timespinbox->value());
		}
	}

	compoundslist->clear();

	standardcombobox->clear();
	standardcombobox->addItem("None");

	size = (int)compounds.size();
	for (int i = 0; i < size; i++) {
		compoundslist->addItem(compounds[i].c_str());

		if (loadfiles && loadcompounds) {
			compoundslist->item(i)->setSelected(internalselectedionsvector[i]);
		}
		else {
			internalselectedionsvector.push_back(false);
		}

		standardcombobox->addItem(compounds[i].c_str());
	}

	equationcombobox->setCurrentIndex(internalequationtype);
	doublespinboxa->setValue(internala);
	doublespinboxb->setValue(internalb);
	eiccombobox->setCurrentIndex(internaleictype);
	peakshapecombobox->setCurrentIndex(internalpeakshape);
	standardcombobox->setCurrentIndex(internalstandard);
}


void cDefineCalibrationWidget::getData(vector<int>& usedvector, vector<int>& datatypesvector, vector<string>& groupsvector, vector<double>& concentrationsvector, vector<int>& timesvector, vector<int>& selectedionsvector, int& equationtype, double& manuala, double& manualb, int& eictype, int& peakshape, int& standard) {
	usedvector = internalusedvector;
	datatypesvector = internaldatatypesvector;
	groupsvector = internalgroupsvector;
	concentrationsvector = internalconcentrationsvector;
	timesvector = internaltimesvector;
	selectedionsvector = internalselectedionsvector;

	equationtype = internalequationtype;
	manuala = internala;
	manualb = internalb;
	eictype = internaleictype;
	peakshape = internalpeakshape;
	standard = internalstandard;
}


void cDefineCalibrationWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		hide();
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/annotatedfeatures.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/annotatedfeatures.html").absoluteFilePath()));
		#endif
	}

	event->accept();
}


void cDefineCalibrationWidget::okButtonReleased() {
	int firstvalue = -1;
	bool allzeros = true;
	int size;
	string groupname;
	int timeval;

	map<string, int> grouptime;

	if (equationcombobox->currentIndex() < 2) {

		size = (int)checkboxes.size();
		for (int i = 0; i < size; i++) {
			if (checkboxes[i]->isChecked() && (comboboxes[i]->currentIndex() == 1)) {
				if (firstvalue == -1) {
					firstvalue = doublespinboxes[i]->value() * 1000.0;
				}
				else {
					if (firstvalue != doublespinboxes[i]->value() * 1000.0) {
						allzeros = false;
						break;
					}
				}
			}
		}

		if (allzeros) {
			QMessageBox msgBox;
			QString errstr = "At least two different values of concentration must be defined !";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}

	}
	else {

		if (doublespinboxa->value() == 0) {
			QMessageBox msgBox;
			QString errstr = "The parameter 'a' cannot be zero !";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}

	}

	size = (int)checkboxes.size();
	for (int i = 0; i < size; i++) {
		if (checkboxes[i]->isChecked() && (comboboxes[i]->currentIndex() == 0)) {
			groupname = groupnames[i]->text().toStdString();
			timeval = timespinboxes[i]->value();

			if (groupname.empty()) {
				QMessageBox msgBox;
				QString errstr = "The group name in the row no. ";
				errstr += QVariant(i + 1).toString();
				errstr += " cannot be empty !";
				msgBox.setText(errstr);
				msgBox.exec();
				return;
			}

			if (grouptime.count(groupname) == 0) {
				grouptime[groupname] = timeval;
			}
			else {
				if (grouptime[groupname] != timeval) {
					QMessageBox msgBox;
					QString errstr = "The data files in the group ";
					errstr += groupname.c_str();
					errstr += " must have the same collection time value !";
					msgBox.setText(errstr);
					msgBox.exec();
					return;
				}
			}
		}
	}

	allzeros = true;

	size = (int)compoundslist->count();
	for (int i = 0; i < size; i++) {
		if (compoundslist->item(i)->isSelected()) {
			allzeros = false;
			break;
		}
	}

	if (allzeros) {
		QMessageBox msgBox;
		QString errstr = "At least one ion type must be selected !";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	internalusedvector.clear();
	internalfilenames.clear();
	internaldatatypesvector.clear();
	internalgroupsvector.clear();
	internalconcentrationsvector.clear();
	internaltimesvector.clear();
	
	size = (int)lineedits.size();
	for (int i = 0; i < size; i++) {
		internalusedvector.push_back(checkboxes[i]->isChecked());
		internalfilenames.push_back(lineedits[i]->text().toStdString());
		internaldatatypesvector.push_back(comboboxes[i]->currentIndex());
		internalgroupsvector.push_back(groupnames[i]->text().toStdString());
		internalconcentrationsvector.push_back(doublespinboxes[i]->value());
		internaltimesvector.push_back(timespinboxes[i]->value());
	}

	internalcompounds.clear();
	internalselectedionsvector.clear();
	
	size = (int)compoundslist->count();
	for (int i = 0; i < size; i++) {
		internalcompounds.push_back(compoundslist->item(i)->text().toStdString());
		internalselectedionsvector.push_back(compoundslist->item(i)->isSelected());
	}

	internalequationtype = equationcombobox->currentIndex();
	internala = doublespinboxa->value();
	internalb = doublespinboxb->value();
	internaleictype = eiccombobox->currentIndex();
	internalpeakshape = peakshapecombobox->currentIndex();
	internalstandard = standardcombobox->currentIndex();

	hide();

	emit calibrationRedefined();
}


void cDefineCalibrationWidget::cancelButtonReleased() {
	int size = (int)internalfilenames.size();
	for (int i = 0; i < size; i++) {
		checkboxes[i]->setChecked(internalusedvector[i]);
		lineedits[i]->setText(internalfilenames[i].c_str());
		comboboxes[i]->setCurrentIndex(internaldatatypesvector[i]);
		groupnames[i]->setText(internalgroupsvector[i].c_str());
		doublespinboxes[i]->setValue(internalconcentrationsvector[i]);
		timespinboxes[i]->setValue(internaltimesvector[i]);
	}

	compoundslist->clear();

	size = (int)internalcompounds.size();
	for (int i = 0; i < size; i++) {
		compoundslist->addItem(internalcompounds[i].c_str());
		compoundslist->item(i)->setSelected(internalselectedionsvector[i]);
	}

	equationcombobox->setCurrentIndex(internalequationtype);
	doublespinboxa->setValue(internala);
	doublespinboxb->setValue(internalb);
	eiccombobox->setCurrentIndex(internaleictype);
	peakshapecombobox->setCurrentIndex(internalpeakshape);
	standardcombobox->setCurrentIndex(internalstandard);

	hide();
}


void cDefineCalibrationWidget::dataTypeChanged(int index) {
	QComboBox* combobox = qobject_cast<QComboBox*>(sender());
	int position = gridlayout->indexOf(combobox);

	int row, column, rowspan, colspan;
	gridlayout->getItemPosition(position, &row, &column, &rowspan, &colspan);

	row--;

	if (row < 0) {
		return;
	}

	if (index == 0) {
		groupnames[row]->setDisabled(false);
		doublespinboxes[row]->setDisabled(true);
		timespinboxes[row]->setDisabled(false);
	}

	if (index == 1) {
		groupnames[row]->setDisabled(true);
		doublespinboxes[row]->setDisabled(false);
		timespinboxes[row]->setDisabled(true);
	}
}


void cDefineCalibrationWidget::equationChanged(int index) {
	if (index == 2) {
		doublespinboxa->setEnabled(true);
		doublespinboxb->setEnabled(true);
	}
	else {
		doublespinboxa->setEnabled(false);
		doublespinboxb->setEnabled(false);
	}
}

