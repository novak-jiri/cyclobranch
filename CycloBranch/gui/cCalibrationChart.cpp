#include "gui/cCalibrationChart.h"

#include <QHBoxLayout>
//#include <QVBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QDoubleSpinBox>
#include <QKeyEvent>
#include <QAction>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTableView>
#include <QClipboard>
#include <QProgressDialog>
#include <QTextStream>


cCalibrationChart::cCalibrationChart(cGlobalPreferences* globalpreferences, QWidget *parent) {
	this->parent = parent;
	this->globalpreferences = globalpreferences;

	setWindowTitle("Calibration Curve");
	setWindowIcon(QIcon(":/images/icons/features.png"));

	vsplitter = new QSplitter();
	vsplitter->setOrientation(Qt::Vertical);

	chartscene = new cCalibrationChartScene(this);

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	toolbarFile = addToolBar(tr("File"));

	actionExportCSV = new QAction(QIcon(":/images/icons/csv.png"), tr("&Export to CSV"), this);
	actionExportCSV->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportCSV->setToolTip("Export to CSV (Ctrl + E)");
	toolbarFile->addAction(actionExportCSV);
	connect(actionExportCSV, SIGNAL(triggered()), this, SLOT(exportToCsv()));

	actionExportSpectrum = new QAction(QIcon(":/images/icons/66.png"), tr("Export Ima&ge"), this);
	actionExportSpectrum->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
	actionExportSpectrum->setToolTip("Export Image (Ctrl + G)");
	toolbarFile->addAction(actionExportSpectrum);
	connect(actionExportSpectrum, SIGNAL(triggered()), this, SLOT(exportScene()));

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
	connect(actionZoomIn, SIGNAL(triggered()), chartscene, SLOT(zoomIn()));

	actionZoomOut = new QAction(QIcon(":/images/icons/82.png"), tr("Zoom &Out"), this);
	actionZoomOut->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Minus));
	actionZoomOut->setToolTip("Zoom Out (Ctrl -)");
	toolbarView->addAction(actionZoomOut);
	connect(actionZoomOut, SIGNAL(triggered()), chartscene, SLOT(zoomOut()));

	actionZoomReset = new QAction(QIcon(":/images/icons/84.png"), tr("&Reset Zoom"), this);
	actionZoomReset->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	actionZoomReset->setToolTip("Zoom Reset (Ctrl + R)");
	toolbarView->addAction(actionZoomReset);
	connect(actionZoomReset, SIGNAL(triggered()), chartscene, SLOT(normalSize()));

	toolbarView->addSeparator();

	actionHideLabels = new QAction(QIcon(":/images/icons/79.png"), tr("Hide &Labels"), this);
	actionHideLabels->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	actionHideLabels->setToolTip("Hide Labels (Ctrl + L)");
	actionHideLabels->setCheckable(true);
	toolbarView->addAction(actionHideLabels);
	connect(actionHideLabels, SIGNAL(toggled(bool)), this, SLOT(hideLabels(bool)));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	labelconcentration = new QLabel(tr("Concentration: "));

	minconcentration = new QDoubleSpinBox();
	minconcentration->setDecimals(6);
	minconcentration->setRange(0, 1000000);
	minconcentration->setSingleStep(1);

	labelseparator = new QLabel(tr("-"));

	maxconcentration = new QDoubleSpinBox();
	maxconcentration->setDecimals(6);
	maxconcentration->setRange(0, 1000000);
	maxconcentration->setSingleStep(1);

	setconcentrationinterval = new QPushButton("Set");
	setconcentrationinterval->setMinimumWidth(75);
	connect(setconcentrationinterval, SIGNAL(released()), this, SLOT(setConcentrationInterval()));
	connect(this, SIGNAL(emitConcentrationInterval(double, double)), chartscene, SLOT(setConcentrationInterval(double, double)));

	resetconcentrationinterval = new QPushButton("Reset");
	resetconcentrationinterval->setMinimumWidth(75);
	connect(resetconcentrationinterval, SIGNAL(released()), chartscene, SLOT(resetConcentrationInterval()));
	connect(chartscene, SIGNAL(updateConcentrationInterval(double, double)), this, SLOT(updateConcentrationInterval(double, double)));

	hboxconcentration = new QHBoxLayout();
	hboxconcentration->addWidget(labelconcentration);
	hboxconcentration->addWidget(minconcentration);
	hboxconcentration->addWidget(labelseparator);
	hboxconcentration->addWidget(maxconcentration);
	hboxconcentration->addSpacing(5);
	hboxconcentration->addWidget(setconcentrationinterval);
	hboxconcentration->addSpacing(5);
	hboxconcentration->addWidget(resetconcentrationinterval);

	widgetconcentration = new QWidget();
	widgetconcentration->setLayout(hboxconcentration);

	toolbarConcentration = addToolBar(tr("Concentration"));

	//actionMouseConcentrationSelection = new QAction(QIcon(":/images/icons/64.png"), tr("Mouse Concentration Selection Tool"), this);
	//actionMouseConcentrationSelection->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	//actionMouseConcentrationSelection->setToolTip("Mouse Concentration Selection Tool (Ctrl + T)");
	//actionMouseConcentrationSelection->setCheckable(true);
	//actionMouseConcentrationSelection->setChecked(true);
	//actionMouseConcentrationSelection->setEnabled(true);
	//toolbarConcentration->addAction(actionMouseConcentrationSelection);
	//connect(actionMouseConcentrationSelection, SIGNAL(toggled(bool)), chartscene, SLOT(enableMouseConcentrationSelectionTool(bool)));

	toolbarConcentration->addWidget(widgetconcentration);

	database = new QTableView(this);
	databasemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cCalibrationChartProxyModel(this);
	proxymodel->setSourceModel(databasemodel);
	proxymodel->setDynamicSortFilter(false);
	database->setModel(proxymodel);
	database->setSortingEnabled(true);
	database->setSelectionBehavior(QAbstractItemView::SelectItems);
	database->setSelectionMode(QAbstractItemView::ContiguousSelection);
	database->setEditTriggers(QAbstractItemView::NoEditTriggers);
	database->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	database->horizontalHeader()->setSectionsMovable(true);
	//database->verticalHeader()->setDefaultSectionSize(30);

	QList<int> sizes;
	sizes.push_back(400);
	sizes.push_back(200);

	vsplitter->addWidget(chartscene);
	vsplitter->addWidget(database);

	vsplitter->setSizes(sizes);

	setCentralWidget(vsplitter);
	//centralWidget()->setContentsMargins(5, 5, 5, 5);

	menuFile->addAction(actionExportCSV);
	menuFile->addAction(actionExportSpectrum);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuView->addAction(actionZoomIn);
	menuView->addAction(actionZoomOut);
	menuView->addAction(actionZoomReset);
	menuView->addSeparator();
	menuView->addAction(actionHideLabels);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	resize(750, defaultwinsizey);

	//applyGlobalPreferences(globalpreferences);
}


cCalibrationChart::~cCalibrationChart() {
	delete labelconcentration;
	delete minconcentration;
	delete labelseparator;
	delete maxconcentration;
	delete setconcentrationinterval;
	delete resetconcentrationinterval;
	delete hboxconcentration;
	delete widgetconcentration;

	delete chartscene;

	delete databasemodel;
	delete proxymodel;
	delete database;

	delete vsplitter;

	delete actionExportCSV;
	delete actionExportSpectrum;
	delete actionCloseWindow;
	delete actionZoomIn;
	delete actionZoomOut;
	delete actionZoomReset;
	delete actionHideLabels;
	//delete actionMouseConcentrationSelection;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cCalibrationChart::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cCalibrationChart::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		if (lastdirexporttocsv.right(4).compare(".csv", Qt::CaseInsensitive) != 0) {
			lastdirexporttocsv = globalpreferences->exportcsvdefaultdir;
		}
	}
}


void cCalibrationChart::setLineParameters(int equationtype, double a, double b, vector<double> xvalues, vector<double> yvalues, vector<double> sd) {
	if (chartscene) {
		chartscene->setLineParameters(equationtype, a, b, xvalues, yvalues, sd);
	}

	deleteTable();
}


void cCalibrationChart::setData(vector<double> xvalues, vector<double> yvalues, vector<double> sd, vector<int> datagroups) {
	if (chartscene) {
		chartscene->setData(xvalues, yvalues, sd, datagroups);
	}
}


void cCalibrationChart::createTable(double a, double b, vector<double>& calibrationxvalues, vector<double>& calibrationyvalues, vector<double>& calibrationsd, vector<int>& datagroups, vector<double>& dataxvalues, vector<double>& datayvalues, vector<double>& datasd) {
	int currentrow = 0;
	int currentcolumn = 0;
	size_t size;
	string s;
	double value;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Data Point");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Total (x-axis)");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Std. Dev. (x-axis)");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Total (y-axis)");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Std. Dev. (y-axis)");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	size = calibrationxvalues.size();
	for (size_t i = 0; i < size; i++) {
		currentrow = databasemodel->rowCount();
		databasemodel->insertRow(currentrow);

		currentcolumn = 0;

		s = "Calib. ";
		s += to_string(calibrationxvalues[i]);
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setText(s.c_str());
		currentcolumn++;

		if (a != 0) {
			value = (calibrationyvalues[i] - b) / a;
		}
		else {
			value = 0;
		}
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		if (a != 0) {
			value = (calibrationsd[i] - b) / a;
		}
		else {
			value = 0;
		}
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		value = calibrationyvalues[i];
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		value = calibrationsd[i];
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;
	}

	size = dataxvalues.size();
	for (size_t i = 0; i < size; i++) {
		currentrow = databasemodel->rowCount();
		databasemodel->insertRow(currentrow);

		currentcolumn = 0;

		s = "Group ";
		s += to_string(datagroups[i]);
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setText(s.c_str());
		currentcolumn++;

		value = dataxvalues[i];
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		if (a != 0) {
			value = (datasd[i] - b) / a;
		}
		else {
			value = 0;
		}
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		value = datayvalues[i];
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;

		value = datasd[i];
		databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
		databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(value)), Qt::DisplayRole);
		currentcolumn++;
	}

	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
}


void cCalibrationChart::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}
	databasemodel->clear();
}


void cCalibrationChart::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (minconcentration->hasFocus() || maxconcentration->hasFocus() || setconcentrationinterval->hasFocus()) {
			setConcentrationInterval();
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


void cCalibrationChart::exportToCsv() {
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


void cCalibrationChart::updateConcentrationInterval(double minconcentration, double maxconcentration) {
	this->minconcentration->setValue(minconcentration);
	this->maxconcentration->setValue(maxconcentration);
}


void cCalibrationChart::setConcentrationInterval() {
	emit emitConcentrationInterval(minconcentration->value(), maxconcentration->value());
}


void cCalibrationChart::exportScene() {
	QString currentdir = "./";
	if (globalpreferences) {
		currentdir = globalpreferences->exportimagedefaultdir;
	}

	#if OS_TYPE == WIN
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Image..."), currentdir, "PDF Files (*.pdf);; PS Files (*.ps);; PNG Files (*.png);; SVG Files (*.svg)");
	#else
		QString filename = QFileDialog::getSaveFileName(this, tr("Export Image..."), currentdir, "PDF Files (*.pdf);; PNG Files (*.png);; SVG Files (*.svg)");
	#endif
	if (!filename.isEmpty()) {
		regex rx;
		bool selected = false;

		rx = ".+\\.pdf$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chartscene->exportToPDF(filename, false);
			selected = true;
		}

		#if OS_TYPE == WIN
			rx = ".+\\.ps$";
			if (!selected && (regex_search(filename.toStdString(), rx))) {
				chartscene->exportToPDF(filename, true);
				selected = true;
			}
		#endif

		rx = ".+\\.png$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chartscene->exportToPNG(filename);
			selected = true;
		}

		rx = ".+\\.svg$";
		if (!selected && (regex_search(filename.toStdString(), rx))) {
			chartscene->exportToSVG(filename);
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


void cCalibrationChart::hideLabels(bool state) {
	chartscene->hideLabels(state);
}


void cCalibrationChart::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/annotatedfeatures.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/annotatedfeatures.html").absoluteFilePath()));
	#endif
}


void cCalibrationChart::closeWindow() {
	hide();
}

