#include "gui/cMultipleDatasetsTableWidget.h"
#include "core/cParameters.h"
#include "core/cTheoreticalSpectrumList.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include <QProgressDialog>
#include <QKeyEvent>
#include <QIcon>
#include <QTextStream>
#include <QMenuBar>
#include <QMenu>
#include <QClipboard>


void calculateLineParameterA(vector<double>& x, vector<double>& y, double& a) {
	size_t size = x.size();
	double tmp = 0;

	if (size < 1) {
		return;
	}

	a = 0;
	for (size_t i = 0; i < size; i++) {
		a += x[i] * y[i];
		tmp += x[i] * x[i];
	}

	if (tmp != 0) {
		a /= tmp;
	}
	else {
		a = 1;
	}
}


void calculateLineParameters(vector<double>& x, vector<double>& y, double& a, double& b) {
	int xsize = (int)x.size();
	int ysize = (int)y.size();

	a = 1;
	b = 0;

	if (xsize != ysize) {
		return;
	}

	//for (int i = 0; i < xsize; i++) {
	//	cout << x[i] << " " << y[i] << endl;
	//}
	//cout << endl;

	double sumx;
	double sumx2;

	double sumy;
	double sumxy;

	double n = xsize + 1;
	double tmp;

	sumx = 0;
	sumx2 = 0;

	sumy = 0;
	sumxy = 0;

	for (int j = 0; j < xsize; j++) {
		sumx += x[j];
		sumx2 += x[j] * x[j];

		sumy += y[j];
		sumxy += x[j] * y[j];
	}

	a = sumxy * n - sumy * sumx;
	tmp = sumx2 * n - sumx * sumx;
	if (tmp == 0) {
		return;
	}
	a /= tmp;

	b = sumy - a * sumx;
	tmp = n;
	if (tmp == 0) {
		return;
	}
	b /= tmp;

	//cout << "a: " << a << ", ";
	//cout << "b: " << b << endl;
}


void calculateLineParametersWithIterations(vector<double>& x, vector<double>& y, int iterations, double& a, double& b) {
	int xsize = (int)x.size();
	int ysize = (int)y.size();

	a = 1;
	b = 0;

	if (xsize != ysize) {
		return;
	}

	//for (int i = 0; i < xsize; i++) {
	//	cout << x[i] << " " << y[i] << endl;
	//}
	//cout << endl;

	double sumy2;

	double sumx;
	double sumx2;

	double sumy;
	double sumxy;

	double y2;
	double tmp;

	double S;
	double oldS = DBL_MAX;
	double olda, oldb;

	for (int i = 0; i < iterations; i++) {

		sumy2 = 0;

		sumx = 0;
		sumx2 = 0;

		sumy = 0;
		sumxy = 0;

		y2 = 0;

		for (int j = 0; j < xsize; j++) {
			if (i == 0) {
				y2 = y[j] * y[j];
			}
			else {
				y2 = a * x[j] + b;
				y2 *= y2;
			}

			sumy2 += y2;

			sumx += x[j] * y2;
			sumx2 += x[j] * x[j] * y2;

			sumy += y[j] * y2;
			sumxy += x[j] * y[j] * y2;
		}

		a = sumxy * sumy2 - sumy * sumx;
		tmp = sumx2 * sumy2 - sumx * sumx;
		if (tmp == 0) {
			return;
		}
		a /= tmp;

		b = sumy - a * sumx;
		tmp = sumy2;
		if (tmp == 0) {
			return;
		}
		b /= tmp;

		//b = 0;

		//cout << "iteration: " << i + 1 << ", ";
		//cout << "a: " << a << ", ";
		//cout << "b: " << b << endl;

		S = 0;
		for (int j = 0; j < xsize; j++) {
			tmp = y[j] - (a * x[j] + b);
			S += tmp * tmp;
		}

		//cout << "iteration: " << i + 1 << ", ";
		//cout << "S: " << S << ", oldS: " << oldS << endl;
		//cout << endl;

		if (i > 0) {
			if (oldS <= S) {
				a = olda;
				b = oldb;
				return;
			}
		}

		olda = a;
		oldb = b;
		oldS = S;

	}

	//cout << endl;
}


cMultipleDatasetsTableWidget::cMultipleDatasetsTableWidget(cGlobalPreferences* globalpreferences, QWidget* parent) {
	title = "Annotated Features Overview";

	this->globalpreferences = globalpreferences;
	this->parent = parent;

	parameters = 0;
	listoftheoreticalspectra = 0;

	setWindowTitle(title);
	setWindowIcon(QIcon(":/images/icons/features.png"));

	pubchemsearchwidget = new cPubChemSearchWidget();
	pubchemsearchwidget->hide();

	definecalibrationwidget = new cDefineCalibrationWidget();
	definecalibrationwidget->hide();

	connect(definecalibrationwidget, SIGNAL(calibrationRedefined()), this, SLOT(calibrationCurveRedefined()));

	calibrationchartwidget = new cCalibrationChart(globalpreferences, this);
	calibrationchartwidget->hide();

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuSearch = new QMenu(tr("&Search"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	datatypelabel = new QLabel();
	datatypelabel->setText("View: ");

	comboboxdatatype = new QComboBox();
	comboboxdatatype->setToolTip("Select the data to be displayed.");
	comboboxdatatype->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	connect(comboboxdatatype, SIGNAL(currentIndexChanged(int)), this, SLOT(dataTypeViewChanged(int)));

	datatypehboxlayout = new QHBoxLayout();
	datatypehboxlayout->addWidget(datatypelabel);
	datatypehboxlayout->addWidget(comboboxdatatype);

	datatypewidget = new QWidget();
	datatypewidget->setLayout(datatypehboxlayout);

	peakshapelabel = new QLabel();
	peakshapelabel->setText("Peak Shape: ");

	comboboxpeakshape = new QComboBox();
	comboboxpeakshape->setToolTip("Select chromatographic peak shape to be used for area and concentration calculations:\nGaussian = basic Gaussian function,\nex-Gaussian (tailing) = Exponentially Modified Gaussian for chromatographic peaks with tailing,\nex-Gaussian (fronting) = Exponentially Modified Gaussian for chromatographic peaks with fronting.");
	comboboxpeakshape->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	comboboxpeakshape->addItem("Gaussian");
	comboboxpeakshape->addItem("ex-Gaussian (tailing)");
	comboboxpeakshape->addItem("ex-Gaussian (fronting)");
	connect(comboboxpeakshape, SIGNAL(currentIndexChanged(int)), this, SLOT(peakShapeChanged(int)));

	peakshapehboxlayout = new QHBoxLayout();
	peakshapehboxlayout->addWidget(peakshapelabel);
	peakshapehboxlayout->addWidget(comboboxpeakshape);

	peakshapewidget = new QWidget();
	peakshapewidget->setLayout(peakshapehboxlayout);

	standardlabel = new QLabel();
	standardlabel->setText("Standard: ");
	
	comboboxstandard = new QComboBox();
	comboboxstandard->setToolTip("Select compound standard.");
	comboboxstandard->addItem("None");
	comboboxstandard->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	connect(comboboxstandard, SIGNAL(currentIndexChanged(int)), this, SLOT(standardChanged(int)));

	standardhboxlayout = new QHBoxLayout();
	standardhboxlayout->addWidget(standardlabel);
	standardhboxlayout->addWidget(comboboxstandard);

	standardwidget = new QWidget();
	standardwidget->setLayout(standardhboxlayout);

	calibrationlabely = new QLabel();
	calibrationlabely->setText("Calibration Curve: y = ");

	calibrationspinboxa = new QDoubleSpinBox();
	calibrationspinboxa->setRange(-DBL_MAX, DBL_MAX);
	calibrationspinboxa->setDecimals(6);
	calibrationspinboxa->setValue(1);
	calibrationspinboxa->setToolTip("Enter the value of 'a' in the calibration curve 'y = a*x + b'.");

	calibrationlabelx = new QLabel();
	calibrationlabelx->setText(" * x + ");

	calibrationspinboxb = new QDoubleSpinBox();
	calibrationspinboxb->setRange(-DBL_MAX, DBL_MAX);
	calibrationspinboxb->setDecimals(6);
	calibrationspinboxb->setValue(0);
	calibrationspinboxb->setToolTip("Enter the value of 'b' in the calibration curve 'y = a*x + b'.");

	calibrationsetbutton = new QPushButton("Set");
	calibrationsetbutton->setToolTip("Set the calibration curve and recalculate the table.");
	connect(calibrationsetbutton, SIGNAL(released()), this, SLOT(calibrationSetButtonReleased()));

	calibrationdefinebutton = new QPushButton("Define");
	calibrationdefinebutton->setToolTip("Define samples, calibration curve and recalculate the table.");
	connect(calibrationdefinebutton, SIGNAL(released()), this, SLOT(calibrationDefineButtonReleased()));

	calibrationviewbutton = new QPushButton("View");
	calibrationviewbutton->setToolTip("View the calibration curve and statistics.");
	connect(calibrationviewbutton, SIGNAL(released()), this, SLOT(calibrationViewButtonReleased()));

	calibrationhboxlayout = new QHBoxLayout();
	calibrationhboxlayout->addWidget(calibrationlabely);
	calibrationhboxlayout->addWidget(calibrationspinboxa);
	calibrationhboxlayout->addWidget(calibrationlabelx);
	calibrationhboxlayout->addWidget(calibrationspinboxb);
	calibrationhboxlayout->addWidget(calibrationsetbutton);
	calibrationhboxlayout->addWidget(calibrationdefinebutton);
	calibrationhboxlayout->addWidget(calibrationviewbutton);

	calibrationwidget = new QWidget();
	calibrationwidget->setLayout(calibrationhboxlayout);

	columnsizelabel = new QLabel();
	columnsizelabel->setText("Column size: ");

	columnsizespinbox = new QSpinBox();
	columnsizespinbox->setRange(10, 1000);
	columnsizespinbox->setValue(200);
	columnsizespinbox->setSingleStep(10);
	columnsizespinbox->setToolTip("Define the column size.");

	columnsizesetbutton = new QPushButton("Set");
	connect(columnsizesetbutton, SIGNAL(released()), this, SLOT(setColumnSize()));

	columnsizeresetbutton = new QPushButton("Reset");
	connect(columnsizeresetbutton, SIGNAL(released()), this, SLOT(resetColumnSize()));

	columnsizehboxlayout = new QHBoxLayout();
	columnsizehboxlayout->addWidget(columnsizelabel);
	columnsizehboxlayout->addWidget(columnsizespinbox);
	columnsizehboxlayout->addWidget(columnsizesetbutton);
	columnsizehboxlayout->addWidget(columnsizeresetbutton);

	columnsizewidget = new QWidget();
	columnsizewidget->setLayout(columnsizehboxlayout);

	database = new QTableView(this);
	databasemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cMultipleDatasetsTableProxyModel(this);
	proxymodel->setSourceModel(databasemodel);
	proxymodel->setDynamicSortFilter(false);
	database->setModel(proxymodel);
	database->setSortingEnabled(true);
	database->setSelectionBehavior(QAbstractItemView::SelectItems);
	database->setSelectionMode(QAbstractItemView::ContiguousSelection);
	database->setEditTriggers(QAbstractItemView::NoEditTriggers);
	database->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	database->horizontalHeader()->setSectionsMovable(true);
	database->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter | (Qt::Alignment)Qt::TextWordWrap);
	//database->verticalHeader()->setDefaultSectionSize(30);
	connect(database, SIGNAL(clicked(const QModelIndex&)), this, SLOT(rowClicked(const QModelIndex&)));

	toolbarFile = addToolBar(tr("File"));
				
	actionExportCSV = new QAction(QIcon(":/images/icons/csv.png"), tr("&Export to CSV"), this);
	actionExportCSV->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportCSV->setToolTip("Export to CSV (Ctrl + E)");
	toolbarFile->addAction(actionExportCSV);
	connect(actionExportCSV, SIGNAL(triggered()), this, SLOT(exportToCsv()));

	actionExportDatabase = new QAction(QIcon(":/images/icons/db_export.png"), tr("Export to &Database"), this);
	actionExportDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
	actionExportDatabase->setToolTip("Export to Database (Ctrl + D)");
	toolbarFile->addAction(actionExportDatabase);
	connect(actionExportDatabase, SIGNAL(triggered()), this, SLOT(exportToDatabase()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));

	toolbarSearch = addToolBar(tr("Search"));

	actionSearchPubChem = new QAction(QIcon(":/images/icons/search.png"), tr("Search &PubChem..."), this);
	actionSearchPubChem->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	actionSearchPubChem->setToolTip("Search PubChem... (Ctrl + P)");
	toolbarSearch->addAction(actionSearchPubChem);
	connect(actionSearchPubChem, SIGNAL(triggered()), this, SLOT(searchPubChem()));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	addToolBarBreak();

	toolbarView = addToolBar(tr("View"));
	toolbarView->addWidget(datatypewidget);
	toolbarView->addWidget(peakshapewidget);
	toolbarView->addWidget(standardwidget);
	toolbarView->addWidget(columnsizewidget);

	addToolBarBreak();

	toolbarCalibration = addToolBar(tr("Calibration"));
	toolbarCalibration->addWidget(calibrationwidget);

	menuFile->addAction(actionExportCSV);
	menuFile->addAction(actionExportDatabase);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuSearch->addAction(actionSearchPubChem);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuSearch);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);

	mainwidget = new QWidget();
	mainwidget->setLayout(mainlayout);

	setCentralWidget(mainwidget);

	resize(defaultwinsizex, defaultwinsizey);

	applyGlobalPreferences(globalpreferences);

	identifieditems.clear();
	identifieditemswithmultimap.clear();

	activefileid = 0;
	
	// default datatypeview cannot be relative intensity
	// updateTable causes a bug if multiple peaks have 100% relative intensity
	datatypeview = 1;
	
	peakshape = 0;
	standard = 0;
	calibrationa = 1;
	calibrationb = 0;
	systemcolumns = 0;
}


cMultipleDatasetsTableWidget::~cMultipleDatasetsTableWidget() {
	deleteTable();

	delete datatypelabel;
	delete comboboxdatatype;
	delete datatypehboxlayout;
	delete datatypewidget;

	delete peakshapelabel;
	delete comboboxpeakshape;
	delete peakshapehboxlayout;
	delete peakshapewidget;

	delete standardlabel;
	delete comboboxstandard;
	delete standardhboxlayout;
	delete standardwidget;

	delete calibrationlabely;
	delete calibrationspinboxa;
	delete calibrationlabelx;
	delete calibrationspinboxb;
	delete calibrationsetbutton;
	delete calibrationdefinebutton;
	delete calibrationviewbutton;
	delete calibrationhboxlayout;
	delete calibrationwidget;

	delete columnsizelabel;
	delete columnsizespinbox;
	delete columnsizesetbutton;
	delete columnsizeresetbutton;
	delete columnsizehboxlayout;
	delete columnsizewidget;

	delete pubchemsearchwidget;
	delete definecalibrationwidget;
	delete calibrationchartwidget;

	delete databasemodel;
	delete proxymodel;
	delete database;
	delete mainlayout;
	delete mainwidget;

	delete actionExportCSV;
	delete actionExportDatabase;
	delete actionCloseWindow;
	delete actionSearchPubChem;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuSearch;
	delete menuHelp;

	delete menuBar;
}


void cMultipleDatasetsTableWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


bool cMultipleDatasetsTableWidget::prepareToShow(cParameters* parameters, cTheoreticalSpectrumList* listoftheoreticalspectra, int activefileid) {
	QMessageBox msgBox;
	QString errstr;

	if (!parameters || !listoftheoreticalspectra || (listoftheoreticalspectra->size() == 0) || (activefileid >= listoftheoreticalspectra->size())) {
		QString errstr = "Nothing to show. Perform an analysis first (MS, LC-MS or MSI) !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		QString errstr = "This feature is available only for MS, LC-MS, and MSI data.";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	this->parameters = parameters;
	this->listoftheoreticalspectra = listoftheoreticalspectra;
	this->activefileid = activefileid;

	int numberofdatasets = listoftheoreticalspectra->size();

	deleteTable();

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		comboboxdatatype->blockSignals(true);

		comboboxdatatype->clear();
		comboboxdatatype->addItem("Maximum Relative Intensity");
		comboboxdatatype->addItem("Maximum Absolute Intensity");

		if ((parameters->peaklistfileformats[activefileid] != mis) && (parameters->peaklistfileformats[activefileid] != imzML)) {
			comboboxdatatype->addItem("Peak Area (Chromatogram) - All Isotopes");
			comboboxdatatype->addItem("Concentration - All Isotopes");
			comboboxdatatype->addItem("Peak Area (Chromatogram) - Highest Peak");
			comboboxdatatype->addItem("Concentration - Highest Peak");
			comboboxdatatype->addItem("Retention Time");

			standardlabel->show();
			comboboxstandard->show();

			peakshapelabel->show();
			comboboxpeakshape->show();

			toolbarCalibration->show();

			calibrationlabely->show();
			calibrationspinboxa->show();
			calibrationlabelx->show();
			calibrationspinboxb->show();
			calibrationsetbutton->show();
			calibrationdefinebutton->show();
			calibrationviewbutton->show();
		}
		else {
			standardlabel->hide();
			comboboxstandard->hide();

			peakshapelabel->hide();
			comboboxpeakshape->hide();

			toolbarCalibration->hide();

			calibrationlabely->hide();
			calibrationspinboxa->hide();
			calibrationlabelx->hide();
			calibrationspinboxb->hide();
			calibrationsetbutton->hide();
			calibrationdefinebutton->hide();
			calibrationviewbutton->hide();
		}

		// default datatypeview cannot be relative intensity
		// updateTable causes a bug if multiple peaks have 100% relative intensity
		if (datatypeview == 0) {
			datatypeview = 1;
		}

		if (datatypeview < comboboxdatatype->count()) {
			comboboxdatatype->setCurrentIndex(datatypeview);
		}

		comboboxdatatype->blockSignals(false);
	}

	string tmpstr;
	size_t pos;

	if (parameters->mode == dereplication) {
		systemcolumns = 6;
	}
	else {
		systemcolumns = 5;
	}

	int currentcolumn = 0;

	databasemodel->setColumnCount(numberofdatasets + systemcolumns);

	if (parameters->mode == dereplication) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Name");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Ion Type");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Formula (M)");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("PubChem Results");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Num. of Datasets");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	for (int i = 0; i < numberofdatasets; i++) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());

		tmpstr = parameters->peaklistfilenames[i];
		pos = tmpstr.rfind('/');
		if (pos != string::npos) {
			pos = tmpstr.substr(0, pos).rfind('/');
			if (pos != string::npos) {
				tmpstr = tmpstr.substr(pos + 1);
			}
		}

		databasemodel->horizontalHeaderItem(currentcolumn)->setText(tmpstr.c_str());
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	// prepare the data
	identifieditems.clear();
	identifieditemswithmultimap.clear();

	vector< vector<double> > rtimes;
	vector<int> rtimeunits;

	cTheoreticalSpectrum* currentspectrum;
	cPeak* currentpeak;
	int sizei, sizej, sizek;
	string compounddesc;

	sizei = listoftheoreticalspectra->size();
	rtimes.resize(sizei);
	rtimeunits.resize(sizei);

	for (int i = 0; i < sizei; i++) {

		sizej = listoftheoreticalspectra->size(i);
		rtimes[i].resize(sizej);

		for (int j = 0; j < sizej; j++) {
			rtimes[i][j] = listoftheoreticalspectra->get(i, j).getExperimentalSpectrum().getRetentionTime();

			if (j == 0) {
				rtimeunits[i] = listoftheoreticalspectra->get(i, j).getExperimentalSpectrum().getRetentionTimeUnit();
			}

			currentspectrum = &(listoftheoreticalspectra->get(i, j));
			sizek = currentspectrum->getNumberOfPeaks();
			for (int k = 0; k < sizek; k++) {
				currentpeak = &((*currentspectrum)[k]);
				if (currentpeak->matched) {
					if (parameters->generateisotopepattern) {
						// nothing to do - the highest intensity peak (non-isotopic) is selected automatically below
					}
					else {
						if (currentpeak->isotope) {
							continue;
						}
					}

					compounddesc.clear();
					if (currentpeak->descriptionid != -1) {
						currentpeak->description = parameters->peakidtodesc[currentpeak->descriptionid];

						pos = currentpeak->description.find('>');
						if (pos != string::npos) {
							tmpstr = currentpeak->description.substr(pos + 1);
							pos = tmpstr.rfind('<');
							if (pos != string::npos) {
								compounddesc = tmpstr.substr(0, pos);
							}
						}

						pos = currentpeak->description.find(" <");
						if (pos != string::npos) {
							compounddesc += "\t";
							compounddesc += currentpeak->description.substr(0, pos);
						}

						if (parameters->mode == dereplication) {
							if ((currentpeak->compoundid >= 0) && (currentpeak->compoundid < parameters->sequencedatabase.size())) {
								compounddesc += "\t";
								compounddesc += parameters->sequencedatabase[currentpeak->compoundid].getSummaryFormula();
							}
						}

						compounddesc += "\t";

						identifieditemswithmultimap[compounddesc].insert(make_pair(i, make_pair(j, k)));
					}
				}
			}
		}
	}

	map<int, int> tempmap;

	for (auto& it : identifieditemswithmultimap) {

		for (int i = 0; i < sizei; i++) {

			pair< multimap<int, pair<int, int> >::iterator, multimap<int, pair<int, int> >::iterator> result = identifieditemswithmultimap[it.first].equal_range(i);
			tempmap.clear();

			for (multimap<int, pair<int, int> >::iterator it2 = result.first; it2 != result.second; it2++) {
				if (tempmap.count(it2->second.first) == 1) {
					if (listoftheoreticalspectra->get(i, it2->second.first)[it2->second.second].relativeintensity > listoftheoreticalspectra->get(i, it2->second.first)[tempmap[it2->second.first]].relativeintensity) {
						tempmap[it2->second.first] = it2->second.second;
					}
				}
				else {
					tempmap[it2->second.first] = it2->second.second;
				}
			}

			for (auto& it2 : tempmap) {
				if (identifieditems[it.first].count(i) == 1) {
					if (listoftheoreticalspectra->get(i, it2.first)[it2.second].matchedabsoluteintensity > listoftheoreticalspectra->get(i, identifieditems[it.first][i].first)[identifieditems[it.first][i].second].matchedabsoluteintensity) {
						identifieditems[it.first][i] = make_pair(it2.first, it2.second);
					}
				}
				else {
					identifieditems[it.first][i] = make_pair(it2.first, it2.second);
				}
			}

		}

	}

	int numberofrows = (int)identifieditems.size();
	if (numberofrows > 50000) {
		QMessageBox::StandardButton reply;
		string msg = "The table contains " + to_string(numberofrows) + " rows. Do you wish to continue ?\n\nTip: Re-run the search using adjusted search settings to limit the number of rows. You may increase the minimum threshold of relative peak intensity, the minimum m/z ratio, decrease the m/z error tolerance or limit the number of selected ion types in theoretical spectra.";
		reply = QMessageBox::question(this, appname, msg.c_str(), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return false;
		}
	}

	// fill the data
	QProgressDialog* progress;
	cEventFilter filter;

	progress = new QProgressDialog("Preparing the data...", "Cancel", 0, numberofrows, this);
	progress->setMinimumWidth(250);
	progress->installEventFilter(&filter);
	progress->setMinimumDuration(0);
	progress->setWindowModality(Qt::ApplicationModal);
	progress->setValue(0);

	database->setModel(0);
	proxymodel->setSourceModel(0);
	database->setSortingEnabled(false);

	double value;
	int hits;
	int currentrow = 0;
	string datastr;

	cPeaksList eicchromatogram;
	vector<double> nys;
	vector<double> sigmas;
	vector<double> as;
	vector<double> bases;
	vector<double> taus;

	double maxheight;
	int tmpsize;
	bool badhit;

	bool fronting = false;
	if (peakshape == 2) {
		fronting = true;
	}

	for (auto& it : identifieditems) {

		progress->setValue(currentrow);

		currentrow = databasemodel->rowCount();
		currentcolumn = 0;

		databasemodel->insertRow(currentrow);

		if (parameters->mode == dereplication) {
			datastr.clear();
			tmpstr = it.first;
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setText(datastr.c_str());
			currentcolumn++;

			datastr.clear();
			tmpstr = tmpstr.substr(pos + 1);
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setText(datastr.c_str());
			currentcolumn++;

			datastr.clear();
			tmpstr = tmpstr.substr(pos + 1);
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setText(datastr.c_str());
			currentcolumn++;
		}
		else {
			datastr.clear();
			tmpstr = it.first;
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->setItem(currentrow, 1, new QStandardItem());
			databasemodel->item(currentrow, 1)->setText(datastr.c_str());

			datastr.clear();
			tmpstr = tmpstr.substr(pos + 1);
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->setItem(currentrow, 0, new QStandardItem());
			databasemodel->item(currentrow, 0)->setText(datastr.c_str());

			currentcolumn += 2;
		}

		value = listoftheoreticalspectra->get(it.second.begin()->first, it.second.begin()->second.first)[it.second.begin()->second.second].mzratio;
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		datastr = databasemodel->item(currentrow, currentcolumn - 2)->text().toStdString();
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		if (parameters->pchemresults.count(datastr) == 1) {
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(parameters->pchemresults[datastr]), Qt::DisplayRole);
		}
		currentcolumn++;
		
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		currentcolumn++;

		hits = 0;
		for (int i = systemcolumns; i < numberofdatasets + systemcolumns; i++) {
			value = 0;
			if (it.second.count(i - systemcolumns) > 0) {
				switch (datatypeview) {
					case 0:
						value = listoftheoreticalspectra->get(i - systemcolumns, it.second[i - systemcolumns].first)[it.second[i - systemcolumns].second].matchedrelativeintensity;
						break;
					case 1:
						value = listoftheoreticalspectra->get(i - systemcolumns, it.second[i - systemcolumns].first)[it.second[i - systemcolumns].second].matchedabsoluteintensity;
						break;
					case 2:
					case 3:
						getEICFromMultiMap(i - systemcolumns, identifieditemswithmultimap[it.first], eicchromatogram, false);

						calculateGaussianParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, nys, sigmas, as);
						
						if (peakshape > 0) {
							calculateExponentialParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, fronting, 1.0, bases, taus);
						}

						badhit = false;

						maxheight = 0;
						tmpsize = eicchromatogram.size();
						for (int j = 0; j < tmpsize; j++) {
							if (eicchromatogram[j].absoluteintensity > maxheight) {
								maxheight = eicchromatogram[j].absoluteintensity;
							}
						}

						tmpsize = (int)nys.size();
						for (int j = 0; j < tmpsize; j++) {
							if (as[j] > 2 * maxheight) {
								nys[j] = 0;
								sigmas[j] = 0;
								as[j] = 0;
								badhit = true;
							}
						}

						if (peakshape == 0) {
							value = getGaussianAreas(sigmas, as);
						}
						else {
							value = getExponentiallyModifiedGaussianAreas(nys, as, nys, sigmas, taus, fronting);
							if (value == 0) {
								badhit = true;
							}
						}

						if (badhit) {
							value = -1;
						}
						break;
					case 4:
					case 5:
						getEICFromMultiMap(i - systemcolumns, identifieditemswithmultimap[it.first], eicchromatogram, true);
						calculateGaussianParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, nys, sigmas, as);

						if (peakshape > 0) {
							calculateExponentialParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, fronting, 1.0, bases, taus);
						}

						badhit = false;

						maxheight = 0;
						tmpsize = eicchromatogram.size();
						for (int j = 0; j < tmpsize; j++) {
							if (eicchromatogram[j].absoluteintensity > maxheight) {
								maxheight = eicchromatogram[j].absoluteintensity;
							}
						}

						tmpsize = (int)nys.size();
						for (int j = 0; j < tmpsize; j++) {
							if (as[j] > 2 * maxheight) {
								nys[j] = 0;
								sigmas[j] = 0;
								as[j] = 0;
								badhit = true;
							}
						}

						if (peakshape == 0) {
							value = getGaussianAreas(sigmas, as);
						}
						else {
							value = getExponentiallyModifiedGaussianAreas(nys, as, nys, sigmas, taus, fronting);
							if (value == 0) {
								badhit = true;
							}
						}

						if (badhit) {
							value = -1;
						}
						break;
					case 6:
						value = listoftheoreticalspectra->get(i - systemcolumns, it.second[i - systemcolumns].first).getExperimentalSpectrum().getRetentionTime();
						break;
					default:
						break;
				}
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());

			if (value != 0) {
				//databasemodel->item(currentrow, currentcolumn)->setBackground(Qt::yellow);
				hits++;
			}

			if (value != 0) {
				switch (datatypeview) {
					case 0:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 1:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 2:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 3:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 4:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 5:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 6:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					default:
						break;
				}
			}

			currentcolumn++;
		}

		databasemodel->item(currentrow, systemcolumns - 1)->setData(QVariant::fromValue(hits), Qt::DisplayRole);

		if (progress->wasCanceled()) {
			deleteTable();
			emit tableCancelled();
			break;
		}

	}

	double stdvalue;
	if (!progress->wasCanceled()) {

		if ((datatypeview == 2) || (datatypeview == 4)) {
			if (standard > 0) {
				for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
					stdvalue = 0;
					if (databasemodel->item(standard - 1, j) && (databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble() > 0)) {
						stdvalue = databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble();
					}

					for (int i = 0; i < databasemodel->rowCount(); i++) {
						if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() > 0)) {
							if (stdvalue > 0) {
								value = databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() / stdvalue;
								databasemodel->item(i, j)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
							}
							else {
								databasemodel->item(i, j)->setText("DETECTED");
							}
						}
					}
				}
			}
		}

		if ((datatypeview == 3) || (datatypeview == 5)) {
			// standard selected
			if (standard > 0) {
				for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
					stdvalue = 0;
					if (databasemodel->item(standard - 1, j) && (databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble() > 0)) {
						stdvalue = databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble();
					}

					for (int i = 0; i < databasemodel->rowCount(); i++) {
						if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() > 0)) {
							if ((stdvalue > 0) && (calibrationa > 0)) {
								value = databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() / stdvalue;
								value = (value - calibrationb) / calibrationa;
								databasemodel->item(i, j)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
							}
							else {
								databasemodel->item(i, j)->setText("DETECTED");
							}
						}
					}
				}
			}
			// standard not selected
			else {
				for (int i = 0; i < databasemodel->rowCount(); i++) {
					for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
						value = 0;
						if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() > 0)) {
							value = databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble();

							if (calibrationa > 0) {
								value = (value - calibrationb) / calibrationa;
								databasemodel->item(i, j)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
							}
							else {
								databasemodel->item(i, j)->setText("DETECTED");
							}
						}
					}
				}
			}
		}

		// clear badhits + concentrations below zero
		for (int i = 0; i < databasemodel->rowCount(); i++) {
			for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
				if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() < 0)) {
					databasemodel->item(i, j)->setText("DETECTED");
				}
			}
		}

	}

	proxymodel->setSourceModel(databasemodel);
	database->setModel(proxymodel);
	database->setSortingEnabled(true);
	proxymodel->sort(-1);

	if (!progress->wasCanceled()) {
		for (int i = 0; i < databasemodel->columnCount(); i++) {
			database->resizeColumnToContents(i);
		}
	}

	vector<string> filenames;
	for (int i = systemcolumns; i < numberofdatasets + systemcolumns; i++) {
		filenames.push_back(databasemodel->horizontalHeaderItem(i)->text().toStdString());
	}

	vector<string> compounds;
	string compound;
	int rowcount = databasemodel->rowCount();
	for (int i = 0; i < rowcount; i++) {
		compound.clear();
		if (parameters->mode == dereplication) {
			compound = databasemodel->item(i, 0)->text().toStdString();
			compound += ", ";
			compound += databasemodel->item(i, 1)->text().toStdString();
			compound += ", ";
			compound += databasemodel->item(i, 2)->text().toStdString();
		}
		else {
			compound = databasemodel->item(i, 0)->text().toStdString();
			compound += ", ";
			compound += databasemodel->item(i, 1)->text().toStdString();
		}
		compounds.push_back(compound);
	}

	definecalibrationwidget->prepareWidget(filenames, compounds);

	vector<double> tmpx;
	vector<double> tmpy;

	vector<double> tmpsd;

	calibrationchartwidget->setLineParameters(0, calibrationa, calibrationb, tmpx, tmpy, tmpsd);

	progress->setValue(numberofrows);
	delete progress;

	return true;
}


void cMultipleDatasetsTableWidget::updateTable() {
	QMessageBox msgBox;
	QString errstr;

	if (!parameters || !listoftheoreticalspectra || (listoftheoreticalspectra->size() == 0) || (activefileid >= listoftheoreticalspectra->size())) {
		QString errstr = "Nothing to show. Perform an analysis first (MS, LC-MS or MSI) !";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		QString errstr = "This feature is available only for MS, LC-MS, and MSI data.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	int numberofdatasets = listoftheoreticalspectra->size();

	string tmpstr;
	size_t pos;

	if (parameters->mode == dereplication) {
		systemcolumns = 6;
	}
	else {
		systemcolumns = 5;
	}

	int currentcolumn = 0;

	vector< vector<double> > rtimes;
	vector<int> rtimeunits;

	int sizei, sizej;
	string compounddesc;

	sizei = listoftheoreticalspectra->size();
	rtimes.resize(sizei);
	rtimeunits.resize(sizei);
	for (int i = 0; i < sizei; i++) {
		sizej = listoftheoreticalspectra->size(i);
		rtimes[i].resize(sizej);
		for (int j = 0; j < sizej; j++) {
			rtimes[i][j] = listoftheoreticalspectra->get(i, j).getExperimentalSpectrum().getRetentionTime();

			if (j == 0) {
				rtimeunits[i] = listoftheoreticalspectra->get(i, j).getExperimentalSpectrum().getRetentionTimeUnit();
			}
		}
	}

	int numberofrows = (int)identifieditems.size();

	// fill the data
	QProgressDialog* progress;
	cEventFilter filter;

	progress = new QProgressDialog("Updating the data...", /*"Cancel"*/0, 0, numberofrows, this);
	progress->setMinimumWidth(250);
	progress->installEventFilter(&filter);
	progress->setMinimumDuration(0);
	progress->setWindowModality(Qt::ApplicationModal);
	progress->setValue(0);

	double value;
	int hits;
	int currentrow = -1;
	string datastr;

	cPeaksList eicchromatogram;
	vector<double> nys;
	vector<double> sigmas;
	vector<double> as;
	vector<double> bases;
	vector<double> taus;

	double maxheight;
	int tmpsize;
	bool badhit;

	bool fronting = false;
	if (peakshape == 2) {
		fronting = true;
	}

	for (auto& it : identifieditems) {

		progress->setValue(currentrow);

		currentrow++;
		currentcolumn = 0;

		if (parameters->mode == dereplication) {
			datastr.clear();
			tmpstr = it.first;
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->item(currentrow, currentcolumn)->setText(datastr.c_str());
			currentcolumn++;

			datastr.clear();
			tmpstr = tmpstr.substr(pos + 1);
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->item(currentrow, currentcolumn)->setText(datastr.c_str());
			currentcolumn++;

			datastr.clear();
			tmpstr = tmpstr.substr(pos + 1);
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->item(currentrow, currentcolumn)->setText(datastr.c_str());
			currentcolumn++;
		}
		else {
			datastr.clear();
			tmpstr = it.first;
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->item(currentrow, 1)->setText(datastr.c_str());

			datastr.clear();
			tmpstr = tmpstr.substr(pos + 1);
			pos = tmpstr.find("\t");
			if (pos != string::npos) {
				datastr = tmpstr.substr(0, pos);
			}

			databasemodel->item(currentrow, 0)->setText(datastr.c_str());

			currentcolumn += 2;
		}

		value = listoftheoreticalspectra->get(it.second.begin()->first, it.second.begin()->second.first)[it.second.begin()->second.second].mzratio;
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		datastr = databasemodel->item(currentrow, currentcolumn - 2)->text().toStdString();
		if (parameters->pchemresults.count(datastr) == 1) {
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(parameters->pchemresults[datastr]), Qt::DisplayRole);
		}

		currentcolumn += 2;

		hits = 0;
		for (int i = systemcolumns; i < numberofdatasets + systemcolumns; i++) {
			value = 0;
			if (it.second.count(i - systemcolumns) > 0) {
				switch (datatypeview) {
					case 0:
						value = listoftheoreticalspectra->get(i - systemcolumns, it.second[i - systemcolumns].first)[it.second[i - systemcolumns].second].matchedrelativeintensity;
						break;
					case 1:
						value = listoftheoreticalspectra->get(i - systemcolumns, it.second[i - systemcolumns].first)[it.second[i - systemcolumns].second].matchedabsoluteintensity;
						break;
					case 2:
					case 3:
						getEICFromMultiMap(i - systemcolumns, identifieditemswithmultimap[it.first], eicchromatogram, false);

						calculateGaussianParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, nys, sigmas, as);

						if (peakshape > 0) {
							calculateExponentialParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, fronting, 1.0, bases, taus);
						}

						badhit = false;

						maxheight = 0;
						tmpsize = eicchromatogram.size();
						for (int j = 0; j < tmpsize; j++) {
							if (eicchromatogram[j].absoluteintensity > maxheight) {
								maxheight = eicchromatogram[j].absoluteintensity;
							}
						}

						tmpsize = (int)nys.size();
						for (int j = 0; j < tmpsize; j++) {
							if (as[j] > 2 * maxheight) {
								nys[j] = 0;
								sigmas[j] = 0;
								as[j] = 0;
								badhit = true;
							}
						}

						if (peakshape == 0) {
							value = getGaussianAreas(sigmas, as);
						}
						else {
							value = getExponentiallyModifiedGaussianAreas(nys, as, nys, sigmas, taus, fronting);
							if (value == 0) {
								badhit = true;
							}
						}

						if (badhit) {
							value = -1;
						}
						break;
					case 4:
					case 5:
						getEICFromMultiMap(i - systemcolumns, identifieditemswithmultimap[it.first], eicchromatogram, true);
						calculateGaussianParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, nys, sigmas, as);

						if (peakshape > 0) {
							calculateExponentialParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, fronting, 1.0, bases, taus);
						}

						badhit = false;

						maxheight = 0;
						tmpsize = eicchromatogram.size();
						for (int j = 0; j < tmpsize; j++) {
							if (eicchromatogram[j].absoluteintensity > maxheight) {
								maxheight = eicchromatogram[j].absoluteintensity;
							}
						}

						tmpsize = (int)nys.size();
						for (int j = 0; j < tmpsize; j++) {
							if (as[j] > 2 * maxheight) {
								nys[j] = 0;
								sigmas[j] = 0;
								as[j] = 0;
								badhit = true;
							}
						}

						if (peakshape == 0) {
							value = getGaussianAreas(sigmas, as);
						}
						else {
							value = getExponentiallyModifiedGaussianAreas(nys, as, nys, sigmas, taus, fronting);
							if (value == 0) {
								badhit = true;
							}
						}

						if (badhit) {
							value = -1;
						}
						break;
					case 6:
						value = listoftheoreticalspectra->get(i - systemcolumns, it.second[i - systemcolumns].first).getExperimentalSpectrum().getRetentionTime();
						break;
					default:
						break;
				}
			}

			if (value != 0) {
				hits++;
			}

			if (value != 0) {
				switch (datatypeview) {
					case 0:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 1:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 2:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 3:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 4:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 5:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					case 6:
						databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
						break;
					default:
						break;
				}
			}

			currentcolumn++;
		}

		databasemodel->item(currentrow, systemcolumns - 1)->setData(QVariant::fromValue(hits), Qt::DisplayRole);

		//if (progress->wasCanceled()) {
		//	break;
		//}

	}

	double stdvalue;
	//if (!progress->wasCanceled()) {

		if ((datatypeview == 2) || (datatypeview == 4)) {
			if (standard > 0) {
				for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
					stdvalue = 0;
					if (databasemodel->item(standard - 1, j) && (databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble() > 0)) {
						stdvalue = databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble();
					}

					for (int i = 0; i < databasemodel->rowCount(); i++) {
						if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() > 0)) {
							if (stdvalue > 0) {
								value = databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() / stdvalue;
								databasemodel->item(i, j)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
							}
							else {
								databasemodel->item(i, j)->setText("DETECTED");
							}
						}
					}
				}
			}
		}

		if ((datatypeview == 3) || (datatypeview == 5)) {
			// standard selected
			if (standard > 0) {
				for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
					stdvalue = 0;
					if (databasemodel->item(standard - 1, j) && (databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble() > 0)) {
						stdvalue = databasemodel->item(standard - 1, j)->data(Qt::DisplayRole).toDouble();
					}

					for (int i = 0; i < databasemodel->rowCount(); i++) {
						if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() > 0)) {
							if ((stdvalue > 0) && (calibrationa > 0)) {
								value = databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() / stdvalue;
								value = (value - calibrationb) / calibrationa;
								databasemodel->item(i, j)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
							}
							else {
								databasemodel->item(i, j)->setText("DETECTED");
							}
						}
					}
				}
			}
			// standard not selected
			else {
				for (int i = 0; i < databasemodel->rowCount(); i++) {
					for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
						value = 0;
						if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() > 0)) {
							value = databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble();

							if (calibrationa > 0) {
								value = (value - calibrationb) / calibrationa;
								databasemodel->item(i, j)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
							}
							else {
								databasemodel->item(i, j)->setText("DETECTED");
							}
						}
					}
				}
			}
		}

		// clear badhits + concentrations below zero
		for (int i = 0; i < databasemodel->rowCount(); i++) {
			for (int j = systemcolumns; j < databasemodel->columnCount(); j++) {
				if (databasemodel->item(i, j) && (databasemodel->item(i, j)->data(Qt::DisplayRole).toDouble() < 0)) {
					databasemodel->item(i, j)->setText("DETECTED");
				}
			}
		}

	//}

	progress->setValue(numberofrows);
	delete progress;

	if (database->selectionModel()->selectedIndexes().count() > 0) {
		if (proxymodel->mapToSource(database->selectionModel()->selectedIndexes()[0]).column() >= systemcolumns) {
			rowClicked(database->selectionModel()->selectedIndexes()[0]);
		}
	}
}


void cMultipleDatasetsTableWidget::resetStandard() {
	standard = 0;
}


void cMultipleDatasetsTableWidget::updateListofStandards() {
	if (!parameters) {
		return;
	}

	comboboxstandard->blockSignals(true);
	comboboxstandard->clear();
	comboboxstandard->addItem("None");

	string tmpstr;
	for (int i = 0; i < databasemodel->rowCount(); i++) {
		tmpstr.clear();

		if (parameters->mode == dereplication) {
			if (databasemodel->item(i, 0)) {
				tmpstr += databasemodel->item(i, 0)->text().toStdString();
				tmpstr += ", ";
			}
			if (databasemodel->item(i, 1)) {
				tmpstr += databasemodel->item(i, 1)->text().toStdString();
				tmpstr += ", ";
			}
			if (databasemodel->item(i, 2)) {
				tmpstr += databasemodel->item(i, 2)->text().toStdString();
			}

			comboboxstandard->addItem(tmpstr.c_str());
		}

		if (parameters->mode == compoundsearch) {
			if (databasemodel->item(i, 0)) {
				tmpstr += databasemodel->item(i, 0)->text().toStdString();
				tmpstr += ", ";
			}
			if (databasemodel->item(i, 1)) {
				tmpstr += databasemodel->item(i, 1)->text().toStdString();
			}

			comboboxstandard->addItem(tmpstr.c_str());
		}
	}

	comboboxstandard->blockSignals(false);
}


void cMultipleDatasetsTableWidget::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}
	databasemodel->clear();
}


void cMultipleDatasetsTableWidget::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		if (lastdirexporttocsv.right(4).compare(".csv", Qt::CaseInsensitive) != 0) {
			lastdirexporttocsv = globalpreferences->exportcsvdefaultdir;
		}
		if (lastdirexportdatabase.right(4).compare(".txt", Qt::CaseInsensitive) != 0) {
			lastdirexportdatabase = globalpreferences->sequencesdefaultdir;
		}
	}
}


cPubChemSearchWidget* cMultipleDatasetsTableWidget::getPubChemSearchWidget() {
	return pubchemsearchwidget;
}


cDefineCalibrationWidget* cMultipleDatasetsTableWidget::getDefineCalibrationWidget() {
	return definecalibrationwidget;
}


cCalibrationChart* cMultipleDatasetsTableWidget::getCalibrationChartWidget() {
	return calibrationchartwidget;
}


void cMultipleDatasetsTableWidget::getEICFromMultiMap(int datafileid, multimap<int, pair<int, int> >& compoundmultimap, cPeaksList& eicchromatogram, bool highestpeaks) {
	eicchromatogram.clear();

	int size = listoftheoreticalspectra->size(datafileid);
	eicchromatogram.resize(size);
	for (int i = 0; i < size; i++) {
		eicchromatogram[i].clear();
		eicchromatogram[i].mzratio = (double)(i + 1);
		eicchromatogram[i].absoluteintensity = 0;
	}

	pair< multimap<int, pair<int, int> >::iterator, multimap<int, pair<int, int> >::iterator> result = compoundmultimap.equal_range(datafileid);
	map<int, int> tempmap;

	if (highestpeaks) {
		for (multimap<int, pair<int, int> >::iterator it = result.first; it != result.second; it++) {
			if (tempmap.count(it->second.first) == 1) {
				if (listoftheoreticalspectra->get(datafileid, it->second.first)[it->second.second].relativeintensity > listoftheoreticalspectra->get(datafileid, it->second.first)[tempmap[it->second.first]].relativeintensity) {
					tempmap[it->second.first] = it->second.second;
				}
			}
			else {
				tempmap[it->second.first] = it->second.second;
			}
		}

		for (auto& it : tempmap) {
			if (listoftheoreticalspectra->get(datafileid, it.first)[it.second].matchedabsoluteintensity > eicchromatogram[it.first].absoluteintensity) {
				eicchromatogram[it.first].absoluteintensity = listoftheoreticalspectra->get(datafileid, it.first)[it.second].matchedabsoluteintensity;
			}
		}
	}
	else {
		for (multimap<int, pair<int, int> >::iterator it = result.first; it != result.second; it++) {
			eicchromatogram[it->second.first].absoluteintensity += listoftheoreticalspectra->get(datafileid, it->second.first)[it->second.second].matchedabsoluteintensity;
		}
	}

	eicchromatogram.normalizeIntenzity();
}


void cMultipleDatasetsTableWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (calibrationspinboxa->hasFocus() || calibrationspinboxb->hasFocus() || calibrationsetbutton->hasFocus()) {
			calibrationSetButtonReleased();
		}
		else {
			if (database->selectionModel()->selectedIndexes().count() > 0) {
				rowClicked(database->selectionModel()->selectedIndexes()[0]);
			}
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_C)) {
		QModelIndexList selectedindexes = database->selectionModel()->selectedIndexes();
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

			item = databasemodel->itemFromIndex(proxymodel->mapToSource(selectedindexes[i]));
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


void cMultipleDatasetsTableWidget::closeWindow() {
	pubchemsearchwidget->hide();
	definecalibrationwidget->hide();
	calibrationchartwidget->hide();
	hide();
}


void cMultipleDatasetsTableWidget::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));

	if (!filename.isEmpty()) {
		lastdirexporttocsv = filename;

		QProgressDialog progress("Exporting CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
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

		for (int i = 0; i < databasemodel->columnCount(); i++) {
			out << "\"" << databasemodel->horizontalHeaderItem(i)->text() << "\"";
			if (i < databasemodel->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		QStandardItem* item;
		string str;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			for (int j = 0; j < databasemodel->columnCount(); j++) {
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, j)));
				if (item) {
					str = item->data(Qt::DisplayRole).toString().toStdString();
					out << "\"" << stripHTML(str).c_str() << "\"";
					if (j < databasemodel->columnCount() - 1) {
						out << ",";
					}
				}
				else {
					if (j < databasemodel->columnCount() - 1) {
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


void cMultipleDatasetsTableWidget::exportToDatabase() {
	
	if (!parameters) {
		return;
	}

	if (parameters->mode != compoundsearch) {
		QMessageBox msgBox;
		QString msg = "The feature is available only in 'Compound Search - MS, LC-MS, MSI' mode !";
		msgBox.setText(msg);
		msgBox.exec();
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export Database"), lastdirexportdatabase, tr("Files (*.txt)"));

	if (!filename.isEmpty()) {
		lastdirexportdatabase = filename;

		set<string> formulas;
		QStandardItem* item;

		for (int i = 0; i < proxymodel->rowCount(); i++) {
			item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 1)));
			if (item) {
				formulas.insert(item->text().toStdString());
			}
		}

		cSequenceDatabase seqdb;
		cSequence seq;

		seq.setPeptideType(other);
		for (auto& it : formulas) {
			seq.setName(it);
			seq.setSummaryFormula(it);
			seqdb.push_back(seq);
		}

		ofstream output;
		output.open(filename.toStdString());
		seqdb.storeToPlainTextStream(output, false);
		output.close();
	}

}


void cMultipleDatasetsTableWidget::dataTypeViewChanged(int index) {
	datatypeview = index;
	updateTable();

	vector<double> tmpx;
	vector<double> tmpy;

	vector<double> tmpsd;

	calibrationchartwidget->setLineParameters(0, calibrationa, calibrationb, tmpx, tmpy, tmpsd);
}


void cMultipleDatasetsTableWidget::peakShapeChanged(int index) {
	peakshape = index;
	updateTable();

	vector<double> tmpx;
	vector<double> tmpy;

	vector<double> tmpsd;

	calibrationchartwidget->setLineParameters(0, calibrationa, calibrationb, tmpx, tmpy, tmpsd);
}


void cMultipleDatasetsTableWidget::standardChanged(int index) {
	standard = index;
	updateTable();

	vector<double> tmpx;
	vector<double> tmpy;

	vector<double> tmpsd;

	calibrationchartwidget->setLineParameters(0, calibrationa, calibrationb, tmpx, tmpy, tmpsd);
}


void cMultipleDatasetsTableWidget::calibrationSetButtonReleased() {
	calibrationa = calibrationspinboxa->value();
	calibrationb = calibrationspinboxb->value();
	updateTable();

	vector<double> tmpx;
	vector<double> tmpy;

	vector<double> tmpsd;

	calibrationchartwidget->setLineParameters(0, calibrationa, calibrationb, tmpx, tmpy, tmpsd);
}


void cMultipleDatasetsTableWidget::calibrationDefineButtonReleased() {
	definecalibrationwidget->show();
	definecalibrationwidget->activateWindow();
	if (definecalibrationwidget->isMinimized()) {
		definecalibrationwidget->showNormal();
	}
}


void cMultipleDatasetsTableWidget::calibrationViewButtonReleased() {
	calibrationchartwidget->show();
	calibrationchartwidget->activateWindow();
	if (calibrationchartwidget->isMinimized()) {
		calibrationchartwidget->showNormal();
	}
}


void cMultipleDatasetsTableWidget::calibrationCurveRedefined() {
	vector<int> usedvector;
	vector<int> datatypes;
	vector<string> groupsvector;
	vector<double> concentrationsvector;
	vector<int> timesvector;
	vector<int> selectedionsvector;

	int equationtype;
	double manuala;
	double manualb;
	int eictype;
	int peakshapetype;
	int standardtype;

	definecalibrationwidget->getData(usedvector, datatypes, groupsvector, concentrationsvector, timesvector, selectedionsvector, equationtype, manuala, manualb, eictype, peakshapetype, standardtype);

	comboboxdatatype->blockSignals(true);
	comboboxpeakshape->blockSignals(true);
	comboboxstandard->blockSignals(true);

	if (eictype == 0) {
		comboboxdatatype->setCurrentIndex(5);
		datatypeview = 5;
	}
	// eictype == 1
	else {
		comboboxdatatype->setCurrentIndex(3);
		datatypeview = 3;
	}

	comboboxpeakshape->setCurrentIndex(peakshapetype);
	peakshape = peakshapetype;
	
	comboboxstandard->setCurrentIndex(standardtype);
	standard = standardtype;

	comboboxdatatype->blockSignals(false);
	comboboxpeakshape->blockSignals(false);
	comboboxstandard->blockSignals(false);

	int selectedions = 0;
	for (int i = 0; i < selectedionsvector.size(); i++) {
		if (selectedionsvector[i]) {
			selectedions++;
		}
	}

	if (selectedions == 0) {
		return;
	}

	int numberofdatasets = listoftheoreticalspectra->size();

	vector< vector<double> > rtimes;
	vector<int> rtimeunits;

	int sizei, sizej;

	sizei = listoftheoreticalspectra->size();
	rtimes.resize(sizei);
	rtimeunits.resize(sizei);
	for (int i = 0; i < sizei; i++) {
		sizej = listoftheoreticalspectra->size(i);
		rtimes[i].resize(sizej);
		for (int j = 0; j < sizej; j++) {
			rtimes[i][j] = listoftheoreticalspectra->get(i, j).getExperimentalSpectrum().getRetentionTime();

			if (j == 0) {
				rtimeunits[i] = listoftheoreticalspectra->get(i, j).getExperimentalSpectrum().getRetentionTimeUnit();
			}
		}
	}

	double value;

	cPeaksList eicchromatogram;
	vector<double> nys;
	vector<double> sigmas;
	vector<double> as;
	vector<double> bases;
	vector<double> taus;

	double maxheight;
	int tmpsize;
	bool badhit;

	bool fronting = false;
	if (peakshape == 2) {
		fronting = true;
	}

	map<int, double> calibrationareas;
	multimap<int, double> calibrationsubareas;
	map<int, int> calibrationcounts;

	map<string, double> dataareas;
	multimap<string, double> datasubareas;
	map<string, int> datacounts;
	map<string, int> datatimes;

	double stdarea;
	double cumarea;

	int c, k;
	string g;
	
	for (int i = systemcolumns; i < numberofdatasets + systemcolumns; i++) {

		if (usedvector[i - systemcolumns] == 0) {
			continue;
		}

		stdarea = 0;
		cumarea = 0;

		k = 0;

		for (auto& it : identifieditems) {
			
			value = 0;
			if (it.second.count(i - systemcolumns) > 0) {
				
				switch (datatypeview) {
					case 2:
					case 3:
						getEICFromMultiMap(i - systemcolumns, identifieditemswithmultimap[it.first], eicchromatogram, false);

						calculateGaussianParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, nys, sigmas, as);

						if (peakshape > 0) {
							calculateExponentialParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, fronting, 1.0, bases, taus);
						}

						badhit = false;

						maxheight = 0;
						tmpsize = eicchromatogram.size();
						for (int j = 0; j < tmpsize; j++) {
							if (eicchromatogram[j].absoluteintensity > maxheight) {
								maxheight = eicchromatogram[j].absoluteintensity;
							}
						}

						tmpsize = (int)nys.size();
						for (int j = 0; j < tmpsize; j++) {
							if (as[j] > 2 * maxheight) {
								nys[j] = 0;
								sigmas[j] = 0;
								as[j] = 0;
								badhit = true;
							}
						}

						if (peakshape == 0) {
							value = getGaussianAreas(sigmas, as);
						}
						else {
							value = getExponentiallyModifiedGaussianAreas(nys, as, nys, sigmas, taus, fronting);
							if (value == 0) {
								badhit = true;
							}
						}

						if (badhit) {
							value = -1;
						}
						break;
					case 0:
					case 1:
					case 4:
					case 5:
					case 6:
						getEICFromMultiMap(i - systemcolumns, identifieditemswithmultimap[it.first], eicchromatogram, true);
						calculateGaussianParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, nys, sigmas, as);

						if (peakshape > 0) {
							calculateExponentialParameters(eicchromatogram, rtimes[i - systemcolumns], rtimeunits[i - systemcolumns], true, true, fronting, 1.0, bases, taus);
						}

						badhit = false;

						maxheight = 0;
						tmpsize = eicchromatogram.size();
						for (int j = 0; j < tmpsize; j++) {
							if (eicchromatogram[j].absoluteintensity > maxheight) {
								maxheight = eicchromatogram[j].absoluteintensity;
							}
						}

						tmpsize = (int)nys.size();
						for (int j = 0; j < tmpsize; j++) {
							if (as[j] > 2 * maxheight) {
								nys[j] = 0;
								sigmas[j] = 0;
								as[j] = 0;
								badhit = true;
							}
						}

						if (peakshape == 0) {
							value = getGaussianAreas(sigmas, as);
						}
						else {
							value = getExponentiallyModifiedGaussianAreas(nys, as, nys, sigmas, taus, fronting);
							if (value == 0) {
								badhit = true;
							}
						}

						if (badhit) {
							value = -1;
						}
						break;
					default:
						break;
				}

			}

			if (value < 0) {
				value = 0;
			}

			if ((standard > 0) && (standard - 1 == k)) {
				stdarea = value;
			}

			if (selectedionsvector[k]) {
				cumarea += value;
			}

			k++;

		}

		if (datatypes[i - systemcolumns] == 0) {
			
			g = groupsvector[i - systemcolumns];

			if (dataareas.count(g) == 0) {
				if (stdarea > 0) {
					dataareas[g] = cumarea / stdarea;
				}
				else {
					dataareas[g] = cumarea;
				}
			}
			else {
				if (stdarea > 0) {
					dataareas[g] += cumarea / stdarea;
				}
				else {
					dataareas[g] += cumarea;
				}
			}

			if (stdarea > 0) {
				datasubareas.insert(pair<string, double>(g, cumarea / stdarea));
			}
			else {
				datasubareas.insert(pair<string, double>(g, cumarea));
			}

			if (datacounts.count(g) == 0) {
				datacounts[g] = 1;
			}
			else {
				datacounts[g] += 1;
			}

			if (datatimes.count(g) == 0) {
				datatimes[g] = timesvector[i - systemcolumns];
			}
		}
		else {
			c = (int)(concentrationsvector[i - systemcolumns] * 1000.0);

			if (calibrationareas.count(c) == 0) {
				if (stdarea > 0) {
					calibrationareas[c] = cumarea / stdarea;
				}
				else {
					calibrationareas[c] = cumarea;
				}
			}
			else {
				if (stdarea > 0) {
					calibrationareas[c] += cumarea / stdarea;
				}
				else {
					calibrationareas[c] += cumarea;
				}
			}

			if (stdarea > 0) {
				calibrationsubareas.insert(pair<int, double>(c, cumarea / stdarea));
			}
			else {
				calibrationsubareas.insert(pair<int, double>(c, cumarea));
			}

			if (calibrationcounts.count(c) == 0) {
				calibrationcounts[c] = 1;
			}
			else {
				calibrationcounts[c] += 1;
			}
		}

	}

	double xvalue, yvalue, dev;
	double avg;

	vector<double> calibrationxvalues;
	vector<double> calibrationyvalues;
	vector<double> calibrationsd;

	//calibrationxvalues.push_back(0);
	//calibrationyvalues.push_back(0);

	for (auto& it : calibrationareas) {
		xvalue = (double)it.first / 1000.0;
		calibrationxvalues.push_back(xvalue);
		
		yvalue = it.second / (double)(calibrationcounts[it.first]);
		calibrationyvalues.push_back(yvalue);

		if (calibrationcounts[it.first] > 1) {
			avg = 0;
			auto range = calibrationsubareas.equal_range(it.first);
			for (auto it2 = range.first; it2 != range.second; ++it2) {
				avg += it2->second;
			}

			avg /= (double)(calibrationcounts[it.first]);

			dev = 0;
			for (auto it2 = range.first; it2 != range.second; ++it2) {
				dev += (it2->second - avg) * (it2->second - avg);
			}

			dev /= (double)(calibrationcounts[it.first] - 1);
			dev = sqrt(dev);

			calibrationsd.push_back(dev);
		}
		else {
			calibrationsd.push_back(0);
		}
	}

	double a = 1;
	double b = 0;

	if (equationtype == 0) {
		calculateLineParameterA(calibrationxvalues, calibrationyvalues, a);
	}

	if (equationtype == 1) {
		calculateLineParameters(calibrationxvalues, calibrationyvalues, a, b);
		//calculateLineParametersWithIterations(xvalues, yvalues, 10, a, b);
	}

	if (equationtype == 2) {
		a = manuala;
		b = manualb;
	}

	calibrationspinboxa->setValue(a);
	calibrationspinboxb->setValue(b);

	calibrationsetbutton->click();

	// fix xvalues and yvalues
	calibrationchartwidget->setLineParameters(equationtype, a, b, calibrationxvalues, calibrationyvalues, calibrationsd);

	vector<double> dataxvalues;
	vector<double> datayvalues;
	vector<double> datasd;
	vector<string> datagroups;
	vector<int> datatimevalues;

	for (auto& it : dataareas) {
		yvalue = it.second / (double)(datacounts[it.first]);
		datayvalues.push_back(yvalue);

		xvalue = yvalue - b;

		if (a != 0) {
			xvalue /= a;
		}
		else {
			xvalue = 0;
		}

		dataxvalues.push_back(xvalue);

		if (datacounts[it.first] > 1) {
			avg = 0;
			auto range = datasubareas.equal_range(it.first);
			for (auto it2 = range.first; it2 != range.second; ++it2) {
				avg += it2->second;
			}

			avg /= (double)(datacounts[it.first]);

			dev = 0;
			for (auto it2 = range.first; it2 != range.second; ++it2) {
				dev += (it2->second - avg) * (it2->second - avg);
			}

			dev /= (double)(datacounts[it.first] - 1);
			dev = sqrt(dev);

			datasd.push_back(dev);
		}
		else {
			datasd.push_back(0);
		}

		datagroups.push_back(it.first);
		datatimevalues.push_back(datatimes[it.first]);
	}

	calibrationchartwidget->setData(dataxvalues, datayvalues, datasd, datagroups, datatimevalues);

	calibrationchartwidget->createTable(a, b, calibrationxvalues, calibrationyvalues, calibrationsd, datagroups, dataxvalues, datayvalues, datasd, datatimevalues);
}


void cMultipleDatasetsTableWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/annotatedfeatures.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/annotatedfeatures.html").absoluteFilePath()));
	#endif
}


void cMultipleDatasetsTableWidget::rowClicked(const QModelIndex& item) {
	if (!parameters || (systemcolumns == 0)) {
		return;
	}

	int row = proxymodel->mapToSource(item).row();
	int column = proxymodel->mapToSource(item).column();

	if (database->selectionModel()->selectedIndexes().count() > 0) {
		database->selectionModel()->select(item, QItemSelectionModel::ClearAndSelect);
	}

	int datasetid;
	int spectrumid;
	int peakid;

	string clickedcompound;
	if (parameters->mode == dereplication) {
		clickedcompound = databasemodel->item(row, 0)->text().toStdString();
		clickedcompound += "\t";
		clickedcompound += databasemodel->item(row, 1)->text().toStdString();
		clickedcompound += "\t";
		clickedcompound += databasemodel->item(row, 2)->text().toStdString();
		clickedcompound += "\t";
	}
	else {
		clickedcompound = databasemodel->item(row, 1)->text().toStdString();
		clickedcompound += "\t";
		clickedcompound += databasemodel->item(row, 0)->text().toStdString();
		clickedcompound += "\t";
	}

	if (column < systemcolumns) {
		string url = "https://pubchem.ncbi.nlm.nih.gov/#query=";
		
		if ((parameters->mode == dereplication) && (column == 0)) {
			if (identifieditems.count(clickedcompound) > 0) {
				datasetid = identifieditems[clickedcompound].begin()->first;
				spectrumid = identifieditems[clickedcompound].begin()->second.first;
				peakid = identifieditems[clickedcompound].begin()->second.second;

				string stmp = parameters->sequencedatabase[listoftheoreticalspectra->get(datasetid, spectrumid)[peakid].compoundid].getNameWithReferenceAsHTMLString();
				size_t pos = stmp.find('\"');
				if (pos + 1 != string::npos) {
					stmp = stmp.substr(pos + 1);
					pos = stmp.rfind('\"');
					if (pos != string::npos) {
						stmp = stmp.substr(0, pos);

						// crop RT info
						pos = stmp.rfind('@');
						if (pos != string::npos) {
							stmp = stmp.substr(0, pos);
						}

						QDesktopServices::openUrl(QUrl(stmp.c_str()));
					}
				}
			}
		}

		if ((parameters->mode == dereplication) && ((column == 2) || (column == 4))) {
			url += databasemodel->item(row, 2)->text().toStdString();
			QDesktopServices::openUrl(QUrl(url.c_str()));
		}

		if ((parameters->mode == compoundsearch) && ((column == 1) || (column == 3))) {
			url += databasemodel->item(row, 1)->text().toStdString();
			QDesktopServices::openUrl(QUrl(url.c_str()));
		}

		return;
	}

	column -= systemcolumns;

	if (identifieditems.count(clickedcompound) > 0) {
		if (identifieditems[clickedcompound].count(column) > 0) {
			spectrumid = identifieditems[clickedcompound][column].first;
			peakid = identifieditems[clickedcompound][column].second;

			if (parameters->mode == dereplication) {
				emit multipleDatasetsTableRowClicked(column, spectrumid, peakid, datatypeview, peakshape, databasemodel->item(row, 0)->text().toStdString(), databasemodel->item(row, 1)->text().toStdString(), databasemodel->item(row, 3)->text().toStdString());
			}
			else {
				emit multipleDatasetsTableRowClicked(column, spectrumid, peakid, datatypeview, peakshape, databasemodel->item(row, 1)->text().toStdString(), databasemodel->item(row, 0)->text().toStdString(), databasemodel->item(row, 2)->text().toStdString());
			}
		}
	}
}


void cMultipleDatasetsTableWidget::searchPubChem() {
	if (!parameters) {
		return;
	}

	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		QMessageBox msgBox;
		QString msg = "This feature is available only in 'Compare Peaklist(s) with Database - MS, LC-MS, MSI' and 'Compound Search - MS, LC-MS, MSI' modes !";
		msgBox.setText(msg);
		msgBox.exec();
		return;
	}

	bool repeatsearch = false;
	if (!pubchemsearchwidget->getHTML().empty()) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Search PubChem...", "Search again ?", QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes) {
			repeatsearch = true;
		}
	}

	if (repeatsearch || pubchemsearchwidget->getHTML().empty()) {

		//resetFilter();

		int iontypecol;
		int formulacol;
		int theoreticalmzcol;

		if (parameters->mode == compoundsearch) {
			iontypecol = 0;
			formulacol = 1;
			theoreticalmzcol = 2;
		}
		else {
			iontypecol = 1;
			formulacol = 2;
			theoreticalmzcol = 3;
		}

		set<string> formulas;
		map<string, cRowItemInfo> formulasmap;
		multimap<double, string> invertedmultimap;
		//map<string, set<int> > scanids;
		set<string> errorset;
		//QStandardItem* itemid;
		QStandardItem* itemiontype;
		QStandardItem* itemtheoreticalmz;
		//QStandardItem* itemexperimentalint;
		QStandardItem* itemname;
		ostringstream outhtml;

		cRowItemInfo rowitem;
		string tmpstr;

		for (int i = 0; i < proxymodel->rowCount(); i++) {
			//itemid = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, idcol)));
			itemiontype = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, iontypecol)));
			itemtheoreticalmz = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, theoreticalmzcol)));
			//itemexperimentalint = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, experimentalintcol)));
			itemname = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, formulacol)));

			if (/*itemid &&*/ itemiontype && itemtheoreticalmz && /*itemexperimentalint &&*/ itemname) {
				formulas.insert(itemname->text().toStdString());

				rowitem.id = 0; //itemid->text().toInt();
				rowitem.iontype = itemiontype->text().toStdString();
				rowitem.theoreticalmz = itemtheoreticalmz->text().toDouble();
				rowitem.experimentalint = 0; //itemexperimentalint->text().toDouble();

				tmpstr = itemname->text().toStdString() + ", " + rowitem.iontype;

				if (formulasmap.count(tmpstr) == 0) {
					formulasmap[tmpstr] = rowitem;
				}
				//else {
				//	if (formulasmap[tmpstr].id == rowitem.id) {
				//		if (rowitem.experimentalint > formulasmap[tmpstr].experimentalint) {
				//			formulasmap[tmpstr].experimentalint = rowitem.experimentalint;
				//			formulasmap[tmpstr].theoreticalmz = rowitem.theoreticalmz;
				//		}
				//	}
				//}

				//if (scanids.count(tmpstr) == 0) {
				//	set<int> tmpset;
				//	tmpset.insert(rowitem.id);
				//	scanids[tmpstr] = tmpset;
				//}
				//else {
				//	scanids[tmpstr].insert(rowitem.id);
				//}
			}
		}

		if (formulas.size() == 0) {
			return;
		}

		const int limit = 500;

		if (formulas.size() > limit) {
			QMessageBox msgBox;
			QString errstr = "The maximum number of formulas to be searched is " + QVariant(limit).toString() + ". You generated " + QVariant((int)formulas.size()).toString() + " formulas. ";
			errstr += "Please, optimize the search settings.";
			msgBox.setText(errstr);
			msgBox.exec();
			return;
		}

		for (auto& it : formulasmap) {
			invertedmultimap.insert(make_pair(it.second.theoreticalmz, it.first));
		}

		parameters->pchemresults.clear();
		vector<string> cidsvector;

		//vector<string> allcids;

		bool allempty = true;

		bool writelog = false;
		ofstream pchemlog;

		if (writelog) {
			pchemlog.open("pubchem.log", ofstream::out);
		}

		int maxthreadsbackup = QThreadPool::globalInstance()->maxThreadCount();
		int maxparallelqueries = 2;

		QThreadPool::globalInstance()->setMaxThreadCount(maxparallelqueries);

		vector<string> vformulas;
		for (auto& it : formulas) {
			vformulas.push_back(it);
		}

		QProgressDialog firstdialog("Searching compounds (CIDs)...", "Cancel", 0, (int)formulas.size(), this);
		firstdialog.setMinimumWidth(250);
		cEventFilter firstfilter;
		firstdialog.installEventFilter(&firstfilter);
		firstdialog.setMinimumDuration(0);
		firstdialog.setWindowModality(Qt::ApplicationModal);
		firstdialog.setValue(0);

		QFutureWatcher<string> firstfuturewatcher;
		QObject::connect(&firstfuturewatcher, SIGNAL(finished()), &firstdialog, SLOT(reset()));
		QObject::connect(&firstdialog, SIGNAL(canceled()), &firstfuturewatcher, SLOT(cancel()));
		QObject::connect(&firstfuturewatcher, SIGNAL(progressRangeChanged(int, int)), &firstdialog, SLOT(setRange(int, int)));
		QObject::connect(&firstfuturewatcher, SIGNAL(progressValueChanged(int)), &firstdialog, SLOT(setValue(int)));

		QFuture<string> firstfuture = QtConcurrent::mapped(vformulas.begin(), vformulas.end(), cPubchemCIDReader());

		firstfuturewatcher.setFuture(firstfuture);

		firstdialog.exec();

		if (firstfuturewatcher.isCanceled()) {
			QThreadPool::globalInstance()->clear();
		}

		firstfuturewatcher.waitForFinished();

		if (!firstfuturewatcher.isCanceled()) {

			cidsvector.resize(vformulas.size());

			cPubchemCIDReader pubchemformulareader;
			int count;
			string firstfutureresult;
			for (int i = 0; i < (int)vformulas.size(); i++) {
				firstfutureresult = firstfuture.resultAt(i);

				if (firstfutureresult.substr(0, 6).compare("@ERROR") == 0) {
					if (writelog) {
						pchemlog << firstfutureresult;
					}
					firstfutureresult.clear();
				}

				if (!firstfutureresult.empty()) {
					allempty = false;
				}

				count = pubchemformulareader.getNumberOfCIDs(firstfutureresult);
				if (count > 0) {
					parameters->pchemresults.insert(make_pair(vformulas[i], count));
					pubchemformulareader.getCommaSeparatedCIDs(firstfutureresult, cidsvector[i]);
					//pubchemformulareader.attachVectorOfCIDs(firstfutureresult, allcids);
				}

				if (firstfutureresult.find("Status:") != string::npos) {
					if (firstfutureresult.find("Status: 404") == string::npos) {
						errorset.insert(vformulas[i]);
					}
				}

				if (writelog) {
					pchemlog << "Formula: \"" << vformulas[i] << "\"" << endl;
					pchemlog << "Result: \"" << firstfutureresult << "\"" << endl;
					pchemlog << "Reduced result: \"" << cidsvector[i] << "\"" << endl;
					pchemlog << "-----" << endl << endl;
				}
			}

			QProgressDialog seconddialog("Searching compounds (Names)...", "Cancel", 0, (int)cidsvector.size(), this);
			seconddialog.setMinimumWidth(250);
			cEventFilter secondfilter;
			seconddialog.installEventFilter(&secondfilter);
			seconddialog.setMinimumDuration(0);
			seconddialog.setWindowModality(Qt::ApplicationModal);
			seconddialog.setValue(0);

			QFutureWatcher<string> secondfuturewatcher;
			QObject::connect(&secondfuturewatcher, SIGNAL(finished()), &seconddialog, SLOT(reset()));
			QObject::connect(&seconddialog, SIGNAL(canceled()), &secondfuturewatcher, SLOT(cancel()));
			QObject::connect(&secondfuturewatcher, SIGNAL(progressRangeChanged(int, int)), &seconddialog, SLOT(setRange(int, int)));
			QObject::connect(&secondfuturewatcher, SIGNAL(progressValueChanged(int)), &seconddialog, SLOT(setValue(int)));

			QFuture<string> secondfuture = QtConcurrent::mapped(cidsvector.begin(), cidsvector.end(), cPubchemNameReader());

			secondfuturewatcher.setFuture(secondfuture);

			seconddialog.exec();

			if (secondfuturewatcher.isCanceled()) {
				QThreadPool::globalInstance()->clear();
			}

			secondfuturewatcher.waitForFinished();

			if (!secondfuturewatcher.isCanceled()) {

				cPubchemNameReader pubchemnamereader;
				set<string> reducednames;
				string link;

				map<string, set<string> > namesmap;
				string secondfutureresult;

				bool servererror = false;

				for (int i = 0; i < (int)vformulas.size(); i++) {
					if (cidsvector[i].empty()) {
						continue;
					}

					reducednames.clear();
					secondfutureresult = secondfuture.resultAt(i);

					if (secondfutureresult.substr(0, 6).compare("@ERROR") == 0) {
						if (writelog) {
							pchemlog << secondfutureresult;
						}
						secondfutureresult.clear();
					}

					pubchemnamereader.getReducedNames(secondfutureresult, reducednames);

					if (!reducednames.empty()) {
						namesmap[vformulas[i]] = reducednames;
					}

					if (secondfutureresult.find("Status:") != string::npos) {
						if (secondfutureresult.find("Status: 404") == string::npos) {
							set<string> tmperrset;
							string status;

							if (secondfutureresult.find("Status: 414") != string::npos) {
								tmperrset.insert("Too many names, this item must be checked manually.");
								namesmap[vformulas[i]] = tmperrset;
							}
							else {
								tmperrset.insert("No names found, try again or later.");
								namesmap[vformulas[i]] = tmperrset;

								servererror = true;

								status = vformulas[i];
								status += ", <a href=\"https://pubchem.ncbi.nlm.nih.gov/#query=" + vformulas[i] + "\">";
								status += "https://pubchem.ncbi.nlm.nih.gov/#query=" + vformulas[i];
								status += "</a>";

								errorset.insert(status);
							}
						}
					}

					if (writelog) {
						pchemlog << "Formula: \"" << vformulas[i] << "\"" << endl;
						pchemlog << "Result: \"" << secondfutureresult << "\"" << endl;
						pchemlog << "Reduced result: " << endl;
						for (auto& it : reducednames) {
							pchemlog << it << endl;
						}
						pchemlog << "-----" << endl << endl;
					}
				}

				if (errorset.size() > 0) {
					outhtml << "<div>";
					outhtml << "The queries on the following formulas returned an error:<br/>";
					for (auto& it : errorset) {
						outhtml << it << "<br/>";
					}
					outhtml << "</div>";
				}

				if (servererror) {
					outhtml << "<div>Try to repeat the search process again or later !</div>";
				}
				else {
					if (allempty) {
						outhtml << "<div>Failed to connect to the server. Please, check your Internet connection, firewall or antivirus settings !</div>";
					}
					else {
						outhtml << "<div>The search process has been completed successfully !</div>";
					}
				}

				outhtml << "<div><br/>---<br/></div>";

				for (auto& it : invertedmultimap) {
					tmpstr = it.second.substr(0, it.second.find(","));

					if (namesmap[tmpstr].size() > 0) {
						link = "https://pubchem.ncbi.nlm.nih.gov/#query=" + tmpstr;

						outhtml << "<div>";
						outhtml << std::fixed << std::setprecision(6);
						outhtml << "m/z " << it.first << ", " << it.second << ", <a href=\"" << link << "\">" << link << "</a> (";
						outhtml << parameters->pchemresults[tmpstr];
						outhtml << " result";
						if (parameters->pchemresults[tmpstr] > 1) {
							outhtml << "s";
						}
						outhtml << ")<br/>";

						//vector<int> scanidsvector;
						//if (scanids.count(it.second) == 1) {
						//	scanidsvector.assign(scanids[it.second].begin(), scanids[it.second].end());
						//}

						//outhtml << "Scans: ";
						//int scanssize = (int)scanidsvector.size();
						//for (int i = 0; i < scanssize; i++) {
						//	if (i == 0) {
						//		outhtml << scanidsvector[i];
						//		continue;
						//	}
						//	if (i == scanssize - 1) {
						//		if (scanidsvector[i - 1] + 1 == scanidsvector[i]) {
						//			outhtml << "-";
						//		}
						//		else {
						//			outhtml << ", ";
						//		}
						//		outhtml << scanidsvector[i];
						//		break;
						//	}
						//	if (scanidsvector[i - 1] + 1 == scanidsvector[i]) {
						//		if (scanidsvector[i] + 1 != scanidsvector[i + 1]) {
						//			outhtml << "-" << scanidsvector[i];
						//		}
						//	}
						//	else {
						//		outhtml << ", " << scanidsvector[i];
						//	}
						//}
						//outhtml << "<br/>";

						//if ((rtimes.size() > 0) && (rtimes[rtimes.size() - 1] > 0)) {
						//	outhtml << "RT: ";
						//	outhtml << std::fixed << std::setprecision(3);
						//	for (int i = 0; i < scanssize; i++) {
						//		if (i == 0) {
						//			outhtml << rtimes[scanidsvector[i] - 1];
						//			continue;
						//		}
						//		if (i == scanssize - 1) {
						//			if (scanidsvector[i - 1] + 1 == scanidsvector[i]) {
						//				outhtml << "-";
						//			}
						//			else {
						//				outhtml << ", ";
						//			}
						//			outhtml << rtimes[scanidsvector[i] - 1];
						//			break;
						//		}
						//		if (scanidsvector[i - 1] + 1 == scanidsvector[i]) {
						//			if (scanidsvector[i] + 1 != scanidsvector[i + 1]) {
						//				outhtml << "-" << rtimes[scanidsvector[i] - 1];
						//			}
						//		}
						//		else {
						//			outhtml << ", " << rtimes[scanidsvector[i] - 1];
						//		}
						//	}
						//	outhtml << "<br/>";
						//}

						outhtml << "Names: " << "<br/>";
						for (auto& it2 : namesmap[tmpstr]) {
							outhtml << it2 << "<br/>";
						}
						outhtml << "</div>";
					}
				}

			}

		}

		//ofstream of("out.sh");
		//for (auto& it : allcids) {
		//	of << "wget https://pubchem.ncbi.nlm.nih.gov/rest/pug_view/data/compound/" << it << "/xml -O " << it << ".xml" << endl;
		//}

		QThreadPool::globalInstance()->setMaxThreadCount(maxthreadsbackup);

		if (writelog) {
			pchemlog.close();
		}

		//resetFilter();

		prepareToShow(parameters, listoftheoreticalspectra, activefileid);

		emit filterSummaryTable();

		pubchemsearchwidget->setHTML(outhtml.str());
	}

	pubchemsearchwidget->show();
	pubchemsearchwidget->activateWindow();
	if (pubchemsearchwidget->isMinimized()) {
		pubchemsearchwidget->showNormal();
	}
}


void cMultipleDatasetsTableWidget::setColumnSize() {
	for (int i = systemcolumns; i < databasemodel->columnCount(); i++) {
		database->setColumnWidth(i, columnsizespinbox->value());
	}
}


void cMultipleDatasetsTableWidget::resetColumnSize() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
}

