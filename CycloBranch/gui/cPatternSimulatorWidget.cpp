#include "gui/cPatternSimulatorWidget.h"

#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QScrollBar>
#include <QCheckBox>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QKeyEvent>
#include <QAction>
#include <QLabel>
#include <QList>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QClipboard>
#include <QApplication>


cPatternSimulatorWidget::cPatternSimulatorWidget(cGlobalPreferences* globalpreferences, QWidget *parent) {
	this->parent = parent;
	this->globalpreferences = globalpreferences;

	setWindowTitle("Simulate Isotopic Pattern");
	setWindowIcon(QIcon(":/images/icons/pattern.png"));

	vsplitter = new QSplitter();
	vsplitter->setOrientation(Qt::Vertical);

	hsplitter1 = new QSplitter();
	hsplitter1->setOrientation(Qt::Horizontal);

	hsplitter2 = new QSplitter();
	hsplitter2->setOrientation(Qt::Horizontal);

	spectrumscene = new cPatternSimulatorSceneWidget(this);

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	toolbarFile = addToolBar(tr("File"));

	actionExportTable = new QAction(QIcon(":/images/icons/csv.png"), tr("&Export Table to CSV"), this);
	actionExportTable->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportTable->setToolTip("Export Table to CSV (Ctrl + E)");
	toolbarFile->addAction(actionExportTable);
	connect(actionExportTable, SIGNAL(triggered()), this, SLOT(exportTableToCSV()));

	actionExportSpectrum = new QAction(QIcon(":/images/icons/66.png"), tr("Export Ima&ge"), this);
	actionExportSpectrum->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
	actionExportSpectrum->setToolTip("Export Image (Ctrl + G)");
	toolbarFile->addAction(actionExportSpectrum);
	connect(actionExportSpectrum, SIGNAL(triggered()), this, SLOT(exportSpectrum()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));

	toolbarView = addToolBar(tr("View"));

	actionZoomIn = new QAction(QIcon(":/images/icons/83.png"), tr("Zoom &In"), this);
	actionZoomIn->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Plus));
	actionZoomIn->setToolTip("Zoom In (Ctrl +)");
	toolbarView->addAction(actionZoomIn);
	connect(actionZoomIn, SIGNAL(triggered()), spectrumscene, SLOT(zoomIn()));

	actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom &Out"), this);
	actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
	actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
	toolbarView->addAction(actionZoomOut);
	connect(actionZoomOut, SIGNAL(triggered()), spectrumscene, SLOT(zoomOut()));

	actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("&Reset Zoom"), this);
	actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
	toolbarView->addAction(actionZoomReset);
	connect(actionZoomReset, SIGNAL(triggered()), spectrumscene, SLOT(normalSize()));

	toolbarView->addSeparator();

	actionAbsoluteIntensity = new QAction(QIcon(":/images/icons/11.png"), tr("&Absolute Intensity"), this);
	actionAbsoluteIntensity->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionAbsoluteIntensity->setToolTip("Absolute Intensity (Ctrl + I)");
	actionAbsoluteIntensity->setCheckable(true);
	toolbarView->addAction(actionAbsoluteIntensity);
	connect(actionAbsoluteIntensity, SIGNAL(toggled(bool)), spectrumscene, SLOT(absoluteIntensityStateChanged(bool)));

	actionRawData = new QAction(QIcon(":/images/icons/chromatography.png"), tr("&Profile Spectrum"), this);
	actionRawData->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	actionRawData->setToolTip("Profile Spectrum (Ctrl + P)");
	actionRawData->setCheckable(true);
	actionRawData->setChecked(true);
	toolbarView->addAction(actionRawData);
	connect(actionRawData, SIGNAL(toggled(bool)), this, SLOT(rawDataStateChanged(bool)));
	connect(this, SIGNAL(rawDataStateChangedSignal(bool)), spectrumscene, SLOT(rawDataStateChanged(bool)));

	toolbarView->addSeparator();

	actionHideLabels = new QAction(QIcon(":/images/icons/79.png"), tr("Hide Peak &Labels"), this);
	actionHideLabels->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	actionHideLabels->setToolTip("Hide Peak Labels (Ctrl + L)");
	actionHideLabels->setCheckable(true);
	actionHideLabels->setChecked(true);
	toolbarView->addAction(actionHideLabels);
	connect(actionHideLabels, SIGNAL(toggled(bool)), this, SLOT(hidePeakLabels(bool)));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	peakstable = new QTableView(this);
	peakstablemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cSpectrumDetailProxyModel(this);
	proxymodel->setSourceModel(peakstablemodel);
	proxymodel->setDynamicSortFilter(false);
	peakstable->setModel(proxymodel);
	peakstable->setSortingEnabled(true);

	labelmz = new QLabel(tr("m/z: "));

	minmz = new QDoubleSpinBox();
	minmz->setDecimals(6);
	minmz->setRange(0, 100000);
	minmz->setSingleStep(1);

	labelseparator = new QLabel(tr("-"));

	maxmz = new QDoubleSpinBox();
	maxmz->setDecimals(6);
	maxmz->setRange(0, 100000);
	maxmz->setSingleStep(1);

	setmzinterval = new QPushButton("Set");
	setmzinterval->setMinimumWidth(75);
	connect(setmzinterval, SIGNAL(released()), this, SLOT(setMZInterval()));
	connect(this, SIGNAL(emitMZInterval(double, double)), spectrumscene, SLOT(setMZInterval(double, double)));

	resetmzinterval = new QPushButton("Reset");
	resetmzinterval->setMinimumWidth(75);
	connect(resetmzinterval, SIGNAL(released()), spectrumscene, SLOT(resetMZInterval()));
	connect(spectrumscene, SIGNAL(updateMZInterval(double, double)), this, SLOT(updateMZInterval(double, double)));

	hboxmz = new QHBoxLayout();
	hboxmz->addWidget(labelmz);
	hboxmz->addWidget(minmz);
	hboxmz->addWidget(labelseparator);
	hboxmz->addWidget(maxmz);
	hboxmz->addSpacing(5);
	hboxmz->addWidget(setmzinterval);
	hboxmz->addSpacing(5);
	hboxmz->addWidget(resetmzinterval);

	widgetmz = new QWidget();
	widgetmz->setLayout(hboxmz);

	toolbarMz = addToolBar(tr("m/z"));

	actionMouseMzSelection = new QAction(QIcon(":/images/icons/64.png"), tr("Mouse m/z Selection Tool"), this);
	actionMouseMzSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	actionMouseMzSelection->setToolTip("Mouse m/z Selection Tool (Ctrl + T)");
	actionMouseMzSelection->setCheckable(true);
	actionMouseMzSelection->setChecked(true);
	actionMouseMzSelection->setEnabled(true);
	toolbarMz->addAction(actionMouseMzSelection);
	connect(actionMouseMzSelection, SIGNAL(toggled(bool)), spectrumscene, SLOT(enableMouseMzSelectionTool(bool)));

	toolbarMz->addWidget(widgetmz);

	addToolBarBreak();

	toolbarInput = addToolBar(tr("Input"));

	labeliontype = new QLabel(tr("Ion Type: "));
	comboboxiontype = new QComboBox();
	comboboxiontype->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	hboxiontype = new QHBoxLayout();
	hboxiontype->addWidget(labeliontype);
	hboxiontype->addWidget(comboboxiontype);
	widgetiontype = new QWidget();
	widgetiontype->setLayout(hboxiontype);
	toolbarInput->addWidget(widgetiontype);

	labelformula = new QLabel("Formula (M): ");
	lineeditformula = new QLineEdit();
	hboxformula = new QHBoxLayout();
	hboxformula->addWidget(labelformula);
	hboxformula->addWidget(lineeditformula);
	widgetformula = new QWidget();
	widgetformula->setLayout(hboxformula);
	widgetformula->setMaximumWidth(300);
	toolbarInput->addWidget(widgetformula);

	labelfwhm = new QLabel("FWHM: ");
	doublespinboxfwhm = new QDoubleSpinBox();
	//doublespinboxfwhm->setMinimumWidth(120);
	doublespinboxfwhm->setDecimals(6);
	doublespinboxfwhm->setRange(0, 1);
	doublespinboxfwhm->setSingleStep(0.001);
	doublespinboxfwhm->setSuffix(" Da");
	doublespinboxfwhm->setValue(0.05);
	hboxfwhm = new QHBoxLayout();
	hboxfwhm->addWidget(labelfwhm);
	hboxfwhm->addWidget(doublespinboxfwhm);
	widgetfwhm = new QWidget();
	widgetfwhm->setLayout(hboxfwhm);
	toolbarInput->addWidget(widgetfwhm);

	labelcharge = new QLabel("Charge: ");
	spinboxcharge = new QSpinBox();
	//spinboxcharge->setMinimumWidth(120);
	spinboxcharge->setRange(-100, 100);
	spinboxcharge->setSingleStep(1);
	spinboxcharge->setValue(1);
	hboxcharge = new QHBoxLayout();
	hboxcharge->addWidget(labelcharge);
	hboxcharge->addWidget(spinboxcharge);
	widgetcharge = new QWidget();
	widgetcharge->setLayout(hboxcharge);
	toolbarInput->addWidget(widgetcharge);

	labelabsoluteintensity = new QLabel("Max. Absolute Intensity: ");
	doublespinboxabsoluteintensity = new QDoubleSpinBox();
	//doublespinboxabsoluteintensity->setMinimumWidth(120);
	doublespinboxabsoluteintensity->setDecimals(0);
	doublespinboxabsoluteintensity->setRange(1000.0, DBL_MAX);
	doublespinboxabsoluteintensity->setSingleStep(1000000.0);
	doublespinboxabsoluteintensity->setValue(1000000.0);
	hboxabsoluteintensity = new QHBoxLayout();
	hboxabsoluteintensity->addWidget(labelabsoluteintensity);
	hboxabsoluteintensity->addWidget(doublespinboxabsoluteintensity);
	widgetabsoluteintensity = new QWidget();
	widgetabsoluteintensity->setLayout(hboxabsoluteintensity);
	toolbarInput->addWidget(widgetabsoluteintensity);

	simulatebutton = new QPushButton("Simulate");
	simulatebutton->setMinimumWidth(75);
	toolbarInput->addWidget(simulatebutton);

	connect(simulatebutton, SIGNAL(released()), this, SLOT(simulateButtonReleased()));

	hsplitter1->addWidget(spectrumscene);
	hsplitter2->addWidget(peakstable);

	QList<int> sizes;
	sizes.push_back(300);
	sizes.push_back(100);

	vsplitter->addWidget(hsplitter1);
	vsplitter->addWidget(hsplitter2);

	vsplitter->setSizes(sizes);

	setCentralWidget(vsplitter);
	centralWidget()->setContentsMargins(5, 5, 5, 5);

	exportdialog = new cExportDialog(this);

	menuFile->addAction(actionExportTable);
	menuFile->addAction(actionExportSpectrum);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuView->addAction(actionZoomIn);
	menuView->addAction(actionZoomOut);
	menuView->addAction(actionZoomReset);
	menuView->addSeparator();
	menuView->addAction(actionAbsoluteIntensity);
	menuView->addAction(actionRawData);
	menuView->addSeparator();
	menuView->addAction(actionHideLabels);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	resize(defaultwinsizex, defaultwinsizey);

	applyGlobalPreferences(globalpreferences);
}


cPatternSimulatorWidget::~cPatternSimulatorWidget() {
	delete spectrumscene;

	deleteTable();

	delete peakstablemodel;
	delete proxymodel;
	delete peakstable;

	delete labelmz;
	delete minmz;
	delete labelseparator;
	delete maxmz;
	delete setmzinterval;
	delete resetmzinterval;
	delete hboxmz;
	delete widgetmz;

	delete labeliontype;
	delete comboboxiontype;
	delete hboxiontype;
	delete widgetiontype;

	delete labelformula;
	delete lineeditformula;
	delete hboxformula;
	delete widgetformula;

	delete labelfwhm;
	delete doublespinboxfwhm;
	delete hboxfwhm;
	delete widgetfwhm;

	delete labelcharge;
	delete spinboxcharge;
	delete hboxcharge;
	delete widgetcharge;

	delete labelabsoluteintensity;
	delete doublespinboxabsoluteintensity;
	delete hboxabsoluteintensity;
	delete widgetabsoluteintensity;

	delete simulatebutton;

	delete hsplitter1;
	delete hsplitter2;
		
	delete vsplitter;

	delete actionExportTable;
	delete actionExportSpectrum;
	delete actionCloseWindow;
	delete actionZoomIn;
	delete actionZoomOut;
	delete actionZoomReset;
	delete actionAbsoluteIntensity;
	delete actionRawData;
	delete actionHideLabels;
	delete actionMouseMzSelection;
	delete actionHTMLDocumentation;

	delete exportdialog;

	delete menuFile;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cPatternSimulatorWidget::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cPatternSimulatorWidget::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		comboboxiontype->clear();
		for (auto it : globalpreferences->customions) {
			comboboxiontype->addItem(it.name.c_str());
		}
	}
}


void cPatternSimulatorWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (minmz->hasFocus() || maxmz->hasFocus() || setmzinterval->hasFocus()) {
			setMZInterval();
		}
		else {
			simulateButtonReleased();
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
		QModelIndexList selectedindexes = peakstable->selectionModel()->selectedIndexes();
		QStandardItem* item;

		QString selectedtext;
		string itemtext;

		int selectedcount = selectedindexes.count();
		for (int i = 0; i < selectedcount; i++) {
			if (i > 0) {
				if (proxymodel->mapToSource(selectedindexes[i - 1]).row() != proxymodel->mapToSource(selectedindexes[i]).row()) {
					selectedtext += "\n";
				}
				else {
					selectedtext += "\t";
				}
			}

			item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(selectedindexes[i]));
			if (item) {
				itemtext = item->text().toStdString();
				selectedtext += stripHTML(itemtext).c_str();
			}
		}
		selectedtext += "\n";
		QApplication::clipboard()->setText(selectedtext);
	}

	event->accept();
}


void cPatternSimulatorWidget::preparePeaksTable(cPeaksList& isotopepattern) {
	deleteTable();

	peakstable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	peakstable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	peakstable->horizontalHeader()->setSectionsMovable(true);
	//peakstable->verticalHeader()->setDefaultSectionSize(30);

	peakstablemodel->setColumnCount(4);

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstablemodel->setHorizontalHeaderItem(i, new QStandardItem());
	}

	int currentcolumn = 0;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical Relative Intensity [%]");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical Absolute Intensity");
	currentcolumn++;

	peakstablemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
	peakstable->setItemDelegateForColumn(currentcolumn, new cHTMLDelegate());

	int thpeakscount = isotopepattern.size();
	peakstablemodel->setRowCount(thpeakscount);

	peakstable->setModel(0);
	proxymodel->setSourceModel(0);
	peakstable->setSortingEnabled(false);

	for (int i = 0; i < thpeakscount; i++) {
		currentcolumn = 0;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(isotopepattern[i].mzratio)), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(isotopepattern[i].relativeintensity)), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(isotopepattern[i].absoluteintensity)), Qt::DisplayRole);
		currentcolumn++;

		peakstablemodel->setItem(i, currentcolumn, new QStandardItem());
		peakstablemodel->item(i, currentcolumn)->setText(isotopepattern[i].description.c_str());
	}

	proxymodel->setSourceModel(peakstablemodel);
	peakstable->setModel(proxymodel);
	peakstable->setSortingEnabled(true);

	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		peakstable->resizeColumnToContents(i);
	}

	peakstable->setColumnWidth(3, min(1200, peakstable->columnWidth(3)));
}


void cPatternSimulatorWidget::deleteTable() {
	for (int i = 0; i < peakstablemodel->columnCount(); i++) {
		if (peakstable->itemDelegateForColumn(i)) {
			delete peakstable->itemDelegateForColumn(i);
		}
	}
	peakstablemodel->clear();
}


void cPatternSimulatorWidget::simulateButtonReleased() {
	QMessageBox msgbox;
	string errmsg;
	string tmpstring;
	string proton = "H+";

	if (comboboxiontype->currentIndex() == -1) {
		errmsg = "A ion type must be defined (go to 'File->Preferences->Ion Types' in the main window) !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	cIonType iontype = globalpreferences->customions[comboboxiontype->currentIndex()];

	cSummaryFormula formula;
	formula.setFormula(lineeditformula->text().toStdString());

	double fwhm = doublespinboxfwhm->value();
	int charge = spinboxcharge->value();
	double absoluteintensity = doublespinboxabsoluteintensity->value();

	if (formula.isEmpty()) {
		errmsg = "The formula is empty !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	if (!formula.isValid(errmsg)) {
		errmsg = "The formula is not valid !\n\n";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}
	
	if (fwhm <= 0) {
		errmsg = "The FWHM value must be > 0 !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	if (charge == 0) {
		errmsg = "The charge cannot be zero !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	if (absoluteintensity < 1000.0) {
		errmsg = "The maximum absolute intensity must be >= 1000 !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	if (iontype.positive && (charge < 0)) {
		errmsg = "The selected ion type is defined for positive mode but the value of charge is negative !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	if (!iontype.positive && (charge > 0)) {
		errmsg = "The selected ion type is defined for negative mode but the value of charge is positive !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	cSummaryFormula ionformula;
	for (int i = 0; i < iontype.multiplier; i++) {
		tmpstring = formula.getSummary();
		ionformula.addFormula(tmpstring);
	}
	ionformula.addFormula(iontype.formula);

	for (int i = 1; i < abs(charge); i++) {
		if (iontype.positive) {
			ionformula.addFormula(proton);
		}
		else {
			ionformula.addFormula(proton, true);
		}
	}

	double mz = ionformula.getMass();
	if (mz > 25000) {
		errmsg = "The mass of compound exceeds the maximum limit 25000 Da !";
		msgbox.setText(errmsg.c_str());
		msgbox.exec();
		return;
	}

	cPeaksList isotopepattern = ionformula.getIsotopePattern(fwhm, abs(charge), iontype.positive, 40, true);
	for (int i = 0; i < isotopepattern.size(); i++) {
		isotopepattern[i].absoluteintensity = isotopepattern[i].relativeintensity / 100.0 * absoluteintensity;
	}

	/*
	string ionname = iontype.name.substr(0, iontype.name.size() - 1) + " ";
	ionname += to_string(abs(charge));
	if (iontype.positive) {
		ionname += "+";
	}
	else {
		ionname += "-";
	}
	*/

	preparePeaksTable(isotopepattern);

	spectrumscene->initialize(isotopepattern, fwhm);
}


void cPatternSimulatorWidget::updateMZInterval(double minmz, double maxmz) {
	this->minmz->setValue(minmz);
	this->maxmz->setValue(maxmz);
}


void cPatternSimulatorWidget::setMZInterval() {
	emit emitMZInterval(minmz->value(), maxmz->value());
}


void cPatternSimulatorWidget::exportSpectrum() {
	QString currentdir = "./";
	if (globalpreferences) {
		currentdir = globalpreferences->exportimagedefaultdir;
	}

	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Spectrum..."), currentdir, "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Spectrum..."), currentdir, "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
	#endif
	if (!filename.isEmpty()) {
		regex rx;
		bool selected = false;

		rx = ".+\\.pdf$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			spectrumscene->exportToPDF(filename, false);
			selected = true;
		}

		#if OS_TYPE == WIN
			rx = ".+\\.ps$";
			if (!selected && (regex_search(filename.toStdString(), rx))) {
				spectrumscene->exportToPDF(filename, true);
				selected = true;
			}
		#endif

		rx = ".+\\.png$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			spectrumscene->exportToPNG(filename);
			selected = true;
		}

		rx = ".+\\.svg$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			spectrumscene->exportToSVG(filename);
			selected = true;
		}

		if (!selected) {
			QMessageBox msgBox;
			QString errstr = "Unsupported file format.";
			msgBox.setText(errstr);
			msgBox.exec();
		}
	}
}


void cPatternSimulatorWidget::exportTableToCSV() {
	QString currentdir = "./";
	if (globalpreferences) {
		currentdir = globalpreferences->exportcsvdefaultdir;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export Table to CSV..."), currentdir, "Files (*.csv)");
	
	if (!filename.isEmpty()) {

		QProgressDialog progress("Exporting the CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);
		progress.setValue(0);
		
		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		for (int i = 0; i < peakstablemodel->columnCount(); i++) {
			out << "\"" << peakstablemodel->horizontalHeaderItem(i)->text() << "\"";
			if (i < peakstablemodel->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		QStandardItem* item;
		string str;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			for (int j = 0; j < peakstablemodel->columnCount(); j++) {
				item = peakstablemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));
				if (item) {
					str = item->data(Qt::DisplayRole).toString().toStdString();
					out << "\"" << stripHTML(str).c_str() << "\"";
					if (j < peakstablemodel->columnCount() - 1) {
						out << ",";
					}
				}
				else {
					if (j < peakstablemodel->columnCount() - 1) {
						out << ",";
					}
				}
			}
			out << endl;

			progress.setValue(i);
			if (progress.wasCanceled()) {
				removefile = true;
				break;
			}
		}

		file.close();

		if (removefile) {
			file.remove();
		}

		progress.setValue(proxymodel->rowCount());

	}
}


void cPatternSimulatorWidget::rawDataStateChanged(bool state) {
	emit rawDataStateChangedSignal(state);
}


void cPatternSimulatorWidget::hidePeakLabels(bool hide) {
	spectrumscene->hidePeakLabels(hide);
}


void cPatternSimulatorWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/simulateisotopicpattern.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/simulateisotopicpattern.html").absoluteFilePath()));
	#endif
}


void cPatternSimulatorWidget::closeWindow() {
	hide();
}

