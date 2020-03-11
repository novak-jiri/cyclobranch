#include "gui/cSummaryPeaksTableWidget.h"
#include "core/cParameters.h"
#include "core/cTheoreticalSpectrumList.h"
#include "gui/cEventFilter.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QProgressDialog>
#include <QCheckBox>
#include <QKeyEvent>
#include <QIcon>
#include <QLineEdit>
#include <QBrush>
#include <QTextStream>
#include <QMenuBar>
#include <QMenu>
#include <QClipboard>
#include <QApplication>


cSummaryPeaksTableWidget::cSummaryPeaksTableWidget(cGlobalPreferences* globalpreferences, QWidget* parent) {
	title = "Summary Table of Matched Peaks";

	this->globalpreferences = globalpreferences;
	this->parent = parent;

	setWindowTitle(title);
	setWindowIcon(QIcon(":/images/icons/table.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	menuFile = new QMenu(tr("&File"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	rowsfilteroperator = new QComboBox();
	rowsfilteroperator->setToolTip("OR = any condition must be met; AND = all conditions must be met.");
	rowsfilteroperator->addItem("OR");
	rowsfilteroperator->addItem("AND");
	rowsfilteroperator->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercombobox1 = new QComboBox();
	rowsfiltercombobox1->setToolTip("Column to be Searched");
	rowsfiltercombobox1->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercomparatorcombobox1 = new QComboBox();
	rowsfiltercomparatorcombobox1->setToolTip("Type of Comparison");
	rowsfiltercomparatorcombobox1->addItem("=");
	rowsfiltercomparatorcombobox1->addItem("<");
	rowsfiltercomparatorcombobox1->addItem("<=");
	rowsfiltercomparatorcombobox1->addItem(">");
	rowsfiltercomparatorcombobox1->addItem(">=");
	rowsfiltercomparatorcombobox1->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfilterstringlistmodel1 = new QStringListModel();
	rowsfilterlinecompleter1 = new QCompleter();
	rowsfilterlinecompleter1->setModel(rowsfilterstringlistmodel1);
	rowsfilterlinecompleter1->setCaseSensitivity(Qt::CaseInsensitive);
	rowsfilterlinecompleter1->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

	rowsfilterline1 = new QLineEdit();
	rowsfilterline1->setMinimumWidth(150);
	rowsfilterline1->setToolTip("Text to Find");
	rowsfilterline1->setCompleter(rowsfilterlinecompleter1);

	rowsfilterleft1 = new QPushButton("<");
	rowsfilterleft1->setMaximumWidth(25);

	rowsfilterright1 = new QPushButton(">");
	rowsfilterright1->setMaximumWidth(25);

	rowsfiltercombobox2 = new QComboBox();
	rowsfiltercombobox2->setToolTip("Column to be Searched");
	rowsfiltercombobox2->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercomparatorcombobox2 = new QComboBox();
	rowsfiltercomparatorcombobox2->setToolTip("Type of Comparison");
	rowsfiltercomparatorcombobox2->addItem("=");
	rowsfiltercomparatorcombobox2->addItem("<");
	rowsfiltercomparatorcombobox2->addItem("<=");
	rowsfiltercomparatorcombobox2->addItem(">");
	rowsfiltercomparatorcombobox2->addItem(">=");
	rowsfiltercomparatorcombobox2->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfilterstringlistmodel2 = new QStringListModel();
	rowsfilterlinecompleter2 = new QCompleter();
	rowsfilterlinecompleter2->setModel(rowsfilterstringlistmodel2);
	rowsfilterlinecompleter2->setCaseSensitivity(Qt::CaseInsensitive);
	rowsfilterlinecompleter2->setCompletionMode(QCompleter::UnfilteredPopupCompletion);

	rowsfilterline2 = new QLineEdit();
	rowsfilterline2->setMinimumWidth(150);
	rowsfilterline2->setToolTip("Text to Find");
	rowsfilterline2->setCompleter(rowsfilterlinecompleter2);

	rowsfilterleft2 = new QPushButton("<");
	rowsfilterleft2->setMaximumWidth(25);

	rowsfilterright2 = new QPushButton(">");
	rowsfilterright2->setMaximumWidth(25);

	rowsfiltercasesensitive = new QCheckBox();
	rowsfiltercasesensitive->setToolTip("Case Sensitive");

	rowsfilterwholeword = new QCheckBox();
	rowsfilterwholeword->setToolTip("Whole Words Only");

	rowsfilterbutton = new QPushButton("Filter");
	rowsfilterbutton->setToolTip("Filter Search Results");
	rowsfilterbutton->setMinimumWidth(75);

	rowsfilterclearbutton = new QPushButton("Reset");
	rowsfilterclearbutton->setToolTip("Reset Search Results");
	rowsfilterclearbutton->setMinimumWidth(75);
	rowsfilterclearbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

	rowsfilterhbox = new QHBoxLayout();
	rowsfilterhbox->addWidget(rowsfiltercombobox1);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercomparatorcombobox1);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterline1);
	rowsfilterhbox->addWidget(rowsfilterleft1);
	rowsfilterhbox->addWidget(rowsfilterright1);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilteroperator);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercombobox2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercomparatorcombobox2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterline2);
	rowsfilterhbox->addWidget(rowsfilterleft2);
	rowsfilterhbox->addWidget(rowsfilterright2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercasesensitive);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterwholeword);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterbutton);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterclearbutton);

	rowsfilterwidget = new QWidget();
	rowsfilterwidget->setLayout(rowsfilterhbox);

	database = new QTableView(this);
	databasemodel = new QStandardItemModel(0, 0, this);
	proxymodel = new cSummaryPeaksTableProxyModel(this);
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
	connect(database, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));

	toolbarFile = addToolBar(tr("File"));
				
	actionExportCSV = new QAction(QIcon(":/images/icons/csv.png"), tr("&Export to CSV"), this);
	actionExportCSV->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	actionExportCSV->setToolTip("Export to CSV (Ctrl + E)");
	toolbarFile->addAction(actionExportCSV);
	connect(actionExportCSV, SIGNAL(triggered()), this, SLOT(exportToCsv()));

	actionExportStatistics = new QAction(QIcon(":/images/icons/74.png"), tr("Export Stat&istics to CSV"), this);
	actionExportStatistics->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionExportStatistics->setToolTip("Export Statistics to CSV (Ctrl + I)");
	toolbarFile->addAction(actionExportStatistics);
	connect(actionExportStatistics, SIGNAL(triggered()), this, SLOT(exportStatistics()));

	actionCloseWindow = new QAction(QIcon(":/images/icons/33.png"), tr("&Close"), this);
	actionCloseWindow->setShortcut(QKeySequence(Qt::Key_Escape));
	actionCloseWindow->setToolTip("Close (Esc)");
	toolbarFile->addAction(actionCloseWindow);
	connect(actionCloseWindow, SIGNAL(triggered()), this, SLOT(closeWindow()));

	toolbarHelp = addToolBar(tr("Help"));

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));
	actionHTMLDocumentation->setToolTip("Show HTML Documentation (F1)");
	toolbarHelp->addAction(actionHTMLDocumentation);
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));

	toolbarFilter = addToolBar(tr("Filter"));
	toolbarFilter->addWidget(rowsfilterwidget);

	menuFile->addAction(actionExportCSV);
	menuFile->addAction(actionExportStatistics);
	menuFile->addSeparator();
	menuFile->addAction(actionCloseWindow);

	menuHelp->addAction(actionHTMLDocumentation);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	connect(rowsfilterleft1, SIGNAL(released()), this, SLOT(rowsFilterLeft1Slot()));
	connect(rowsfilterright1, SIGNAL(released()), this, SLOT(rowsFilterRight1Slot()));
	connect(rowsfilterleft2, SIGNAL(released()), this, SLOT(rowsFilterLeft2Slot()));
	connect(rowsfilterright2, SIGNAL(released()), this, SLOT(rowsFilterRight2Slot()));

	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);

	mainwidget = new QWidget();
	mainwidget->setLayout(mainlayout);

	setCentralWidget(mainwidget);

	resize(defaultwinsizex, defaultwinsizey);

	applyGlobalPreferences(globalpreferences);
}


cSummaryPeaksTableWidget::~cSummaryPeaksTableWidget() {
	deleteTable();
	
	delete rowsfilteroperator;
	delete rowsfiltercombobox1;
	delete rowsfiltercomparatorcombobox1;
	delete rowsfilterstringlistmodel1;
	delete rowsfilterlinecompleter1;
	delete rowsfilterline1;
	delete rowsfilterleft1;
	delete rowsfilterright1;
	delete rowsfiltercombobox2;
	delete rowsfiltercomparatorcombobox2;
	delete rowsfilterstringlistmodel2;
	delete rowsfilterlinecompleter2;
	delete rowsfilterline2;
	delete rowsfilterleft2;
	delete rowsfilterright2;
	delete rowsfiltercasesensitive;
	delete rowsfilterwholeword;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

	delete databasemodel;
	delete proxymodel;
	delete database;
	delete mainlayout;
	delete mainwidget;

	delete actionExportCSV;
	delete actionExportStatistics;
	delete actionCloseWindow;
	delete actionHTMLDocumentation;

	delete menuFile;
	delete menuHelp;

	delete menuBar;
}


void cSummaryPeaksTableWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
	event->accept();
}


bool cSummaryPeaksTableWidget::prepareToShow(QStandardItemModel* resultsstandardmodel, cMainWindowProxyModel* resultsproxymodel, cParameters* parameters, cTheoreticalSpectrumList* spectralist, bool showisomers) {
	if (!parameters || !spectralist || (resultsstandardmodel->rowCount() != spectralist->size())) {
		return false;
	}

	int spectracount = spectralist->size();
	int thpeakscount = 0;
	int spectrumindex;
	int iontypecol;
	cPeak* peak;

	for (int i = 0; i < spectracount; i++) {

		if (!resultsproxymodel->mapFromSource(resultsstandardmodel->index(i, 0)).isValid()) {
			continue;
		}

		spectrumindex = resultsstandardmodel->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

		if ((*spectralist)[spectrumindex].getNumberOfMatchedPeaks() == 0) {
			continue;
		}

		for (int j = 0; j < (int)((*spectralist)[spectrumindex].getTheoreticalPeaks()->size()); j++) {
			peak = &((*((*spectralist)[spectrumindex].getTheoreticalPeaks()))[j]);
			if (peak->matchedmz > 0) {
				thpeakscount++;
			}
		}
	}

	if (thpeakscount > 1000000) {
		QMessageBox::StandardButton reply;
		string msg = "The table contains " + to_string(thpeakscount) + " rows and might not fit into the main memory. Do you wish to continue ?\n\nTip: Re-run the search using adjusted search settings to limit the number of rows. You may increase the minimum threshold of relative peak intensity, the minimum m/z ratio, decrease the m/z error tolerance or limit the number of selected ion types in theoretical spectra.";
		reply = QMessageBox::question(this, appname, msg.c_str(), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::No) {
			return false;
		}
	}

	this->parameters = parameters;

	resetFilter();
	deleteTable();
	proxymodel->initialize(parameters->mode, parameters->peaklistfileformat, parameters->generateisotopepattern, rowsfilteroperator, rowsfiltercombobox1, rowsfiltercomparatorcombobox1, rowsfilterline1, rowsfiltercombobox2, rowsfiltercomparatorcombobox2, rowsfilterline2);

	// prepare the header
	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			if (parameters->generateisotopepattern) {
				databasemodel->setColumnCount(15);
			}
			else {
				databasemodel->setColumnCount(12);
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				databasemodel->setColumnCount(14);
			}
			else {
				databasemodel->setColumnCount(11);
			}
		}
	}
	else if (parameters->mode == denovoengine) {
		if (parameters->generateisotopepattern) {
			databasemodel->setColumnCount(11);
		}
		else {
			databasemodel->setColumnCount(10);
		}
	}
	else {
		if (parameters->generateisotopepattern) {
			databasemodel->setColumnCount(10);
		}
		else {
			databasemodel->setColumnCount(9);
		}
	}

	databasemodel->setHorizontalHeaderItem(0, new QStandardItem());
	databasemodel->horizontalHeaderItem(0)->setText("ID");
	database->setItemDelegateForColumn(0, new QItemDelegate());

	int currentcolumn = 1;
	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			iontypecol = 3;

			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Coordinate X");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;

			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Coordinate Y");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;
		}
		else {
			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Time");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;
		}

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		if (parameters->generateisotopepattern) {
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Pattern Type");
		}
		else {
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Ion Type");
		}
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	if (parameters->mode == denovoengine) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Group ID");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Fragment Type");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical m/z");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	if (parameters->generateisotopepattern) {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Theoretical Intensity [%]");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Experimental m/z");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Relative Intensity [%]");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Absolute Intensity");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
	databasemodel->horizontalHeaderItem(currentcolumn)->setText("Error [ppm]");
	database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
	currentcolumn++;

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if (parameters->generateisotopepattern) {
			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("Score");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;

			databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
			databasemodel->horizontalHeaderItem(currentcolumn)->setText("FDR");
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
			currentcolumn++;
		}

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		if (parameters->generateisotopepattern) {
			database->setItemDelegateForColumn(currentcolumn, new cHTMLDelegate());
		}
		else {
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		}
		currentcolumn++;

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Name");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Reference");
		database->setItemDelegateForColumn(currentcolumn, new cViewButtonDelegate());
		currentcolumn++;
	}
	else {
		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Summary Formula");
		if (parameters->generateisotopepattern) {
			database->setItemDelegateForColumn(currentcolumn, new cHTMLDelegate());
		}
		else {
			database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		}
		currentcolumn++;

		databasemodel->setHorizontalHeaderItem(currentcolumn, new QStandardItem());
		databasemodel->horizontalHeaderItem(currentcolumn)->setText("Sequence");
		database->setItemDelegateForColumn(currentcolumn, new QItemDelegate());
		currentcolumn++;
	}

	rowsfiltercombobox1->clear();
	rowsfiltercombobox2->clear();
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		database->resizeColumnToContents(i);
		if (((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) ||
			(((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && (i < databasemodel->columnCount() - 1))) {
			rowsfiltercombobox1->addItem(databasemodel->horizontalHeaderItem(i)->text());
			rowsfiltercombobox2->addItem(databasemodel->horizontalHeaderItem(i)->text());
		}
	}

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		rowsfiltercombobox1->setCurrentIndex(rowsfiltercombobox1->count() - 1);
		rowsfiltercombobox2->setCurrentIndex(rowsfiltercombobox2->count() - 1);

		rowsfiltercomparatorcombobox1->setCurrentIndex(0);
		rowsfiltercomparatorcombobox2->setCurrentIndex(0);
	}


	// fill new data
	QProgressDialog* progress;
	cEventFilter filter;

	progress = new QProgressDialog("Preparing the data...", "Cancel", 0, spectracount, this);
	progress->setMinimumWidth(250);
	progress->installEventFilter(&filter);
	progress->setMinimumDuration(0);
	progress->setWindowModality(Qt::ApplicationModal);
	progress->setValue(0);

	database->setModel(0);
	proxymodel->setSourceModel(0);
	database->setSortingEnabled(false);

	int secondspace, langle, rangle, tmp;
	cPeaksList* thpeaks;
	string stmp, fragmentname;
	int currentrow;

	cPeaksList eicchromatogram;
	vector<cCoordinateInfo> coordinateinfo;

	QBrush brush;
	brush.setColor(QColor(0, 0, 0));

	set<QString> completernameset;

	for (int i = 0; i < spectracount; i++) {

		if (!resultsproxymodel->mapFromSource(resultsstandardmodel->index(i, 0)).isValid()) {
			if (((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && (parameters->peaklistfileformat != mis) && (parameters->peaklistfileformat != imzML)) {
				cPeak emptypeak;
				emptypeak.mzratio = (double)(eicchromatogram.size() + 1);
				emptypeak.absoluteintensity = 0;
				eicchromatogram.add(emptypeak);
			}
			continue;
		}

		spectrumindex = resultsstandardmodel->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

		if (((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && (parameters->peaklistfileformat != mis) && (parameters->peaklistfileformat != imzML)) {
			addEICPeak(eicchromatogram, (*spectralist)[spectrumindex], (*spectralist)[spectrumindex].getExperimentalSpectrum());
		}

		if (((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML))) {
			addCoordinateInfo(spectrumindex, coordinateinfo, (*spectralist)[spectrumindex], (*spectralist)[spectrumindex].getExperimentalSpectrum());
		}

		if ((*spectralist)[spectrumindex].getNumberOfMatchedPeaks() == 0) {
			progress->setValue(i);
			continue;
		}

		thpeaks = (*spectralist)[spectrumindex].getTheoreticalPeaks();
		thpeakscount = (*spectralist)[spectrumindex].getTheoreticalPeaks()->size();

		// theoretical peaks
		for (int j = 0; j < thpeakscount; j++) {
			peak = &((*thpeaks)[j]);

			if (peak->descriptionid != -1) {
				peak->description = parameters->peakidtodesc[peak->descriptionid];
			}

			if (peak->isotopeformulaid != -1) {
				peak->isotopeformula = parameters->isotopeformulaidtodesc[peak->isotopeformulaid];
			}

			if (peak->matchedmz == 0) {
				continue;
			}

			currentrow = databasemodel->rowCount();
			currentcolumn = 0;

			databasemodel->insertRow(currentrow);

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(spectrumindex + 1), Qt::DisplayRole);
			currentcolumn++;

			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue((*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateX()), Qt::DisplayRole);
					currentcolumn++;

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue((*spectralist)[spectrumindex].getExperimentalSpectrum().getCoordinateY()), Qt::DisplayRole);
					currentcolumn++;
				}
				else {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray((*spectralist)[spectrumindex].getExperimentalSpectrum().getRetentionTime())), Qt::DisplayRole);
					currentcolumn++;
				}

				secondspace = (int)peak->description.find(' ', peak->description.find(' ') + 1);
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(0, secondspace).c_str());
				currentcolumn++;
			}

			if (parameters->mode == denovoengine) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue((*spectralist)[spectrumindex].getPathId() + 1), Qt::DisplayRole);
				currentcolumn++;
			}

			if ((parameters->mode == denovoengine) || (parameters->mode == singlecomparison) || (parameters->mode == databasesearch)) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(0, peak->description.find(':')).c_str());
				currentcolumn++;
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->mzratio)), Qt::DisplayRole);
			currentcolumn++;

			if (parameters->generateisotopepattern) {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->relativeintensity)), Qt::DisplayRole);
				currentcolumn++;
			}

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedmz)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedrelativeintensity)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropDecimalsByteArray(peak->matchedabsoluteintensity)), Qt::DisplayRole);
			currentcolumn++;

			databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
			databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
			databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(peak->matchedppm)), Qt::DisplayRole);
			currentcolumn++;

			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
				if (parameters->generateisotopepattern) {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray((*spectralist)[spectrumindex].getTargetPatternScore(peak->groupid))), Qt::DisplayRole);
					currentcolumn++;

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray((*spectralist)[spectrumindex].getTargetPatternFDR(peak->groupid))), Qt::DisplayRole);
					currentcolumn++;
				}

				stmp = peak->description.substr(peak->description.rfind('(') + 1, peak->description.rfind(')') - peak->description.rfind('(') - 1);

				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText(stmp.c_str());
				currentcolumn++;

				langle = (int)peak->description.rfind("</a>");
				rangle = (int)peak->description.find('>');
				if ((langle != string::npos) && (rangle != string::npos)) {
					stmp = peak->description.substr(rangle + 1, langle - rangle - 1);

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText(stmp.c_str());

					completernameset.insert(stmp.c_str());
					currentcolumn++;

					tmp = (int)peak->description.find('<');
					stmp = peak->description.substr(tmp, rangle - tmp + 1);
					stmp = stmp.substr(stmp.find('\"') + 1);
					stmp = stmp.substr(0, stmp.rfind('\"'));

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText(stmp.c_str());
					currentcolumn++;
				}
				else {
					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText(peak->description.substr(secondspace + 1, peak->description.rfind('(') - secondspace - 2).c_str());
					currentcolumn++;

					databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
					databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
					databasemodel->item(currentrow, currentcolumn)->setText("");
					currentcolumn++;
				}
			}
			else {
				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				if (parameters->generateisotopepattern) {
					databasemodel->item(currentrow, currentcolumn)->setText(peak->isotopeformula.c_str());
				}
				else {
					databasemodel->item(currentrow, currentcolumn)->setText(peak->formula.getFancySummary(peak->charge).c_str());
				}
				currentcolumn++;

				databasemodel->setItem(currentrow, currentcolumn, new QStandardItem());
				databasemodel->item(currentrow, currentcolumn)->setForeground(brush);
				databasemodel->item(currentrow, currentcolumn)->setText("");
				if (peak->description.find(':') + 2 < peak->description.size()) {
					fragmentname = peak->description.substr(peak->description.find(':') + 2);
					if (!showisomers) {
						stripIsomers(fragmentname);
					}
					databasemodel->item(currentrow, currentcolumn)->setText(fragmentname.c_str());
				}
				currentcolumn++;
			}

		}

		progress->setValue(i);
		if (progress->wasCanceled()) {
			deleteTable();
			emit tableCancelled();
			break;
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

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
			if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
				origcoordinateinfo = coordinateinfo;
				emit resetRegion();
				emit sendFilterOptionsToImageWindow(coordinateinfo, (bool)rowsfilteroperator->currentIndex(), rowsfiltercombobox1->currentText().toStdString(), rowsfiltercomparatorcombobox1->currentText().toStdString(), rowsfilterline1->text().toStdString(), rowsfiltercombobox2->currentText().toStdString(), rowsfiltercomparatorcombobox2->currentText().toStdString(), rowsfilterline2->text().toStdString(), rowsfiltercasesensitive->isChecked(), rowsfilterwholeword->isChecked());

				if (parameters->generateisotopepattern) {
					database->setColumnWidth(12, min(400, database->columnWidth(12)));
				}
				else {
					database->setColumnWidth(9, min(400, database->columnWidth(9)));
				}
			}
			else {
				eicchromatogram.normalizeIntenzity();
				origeicchromatogram = eicchromatogram;
				emit sendFilterOptionsToChromatogram(eicchromatogram);

				if (parameters->generateisotopepattern) {
					database->setColumnWidth(11, min(400, database->columnWidth(11)));
				}
				else {
					database->setColumnWidth(8, min(400, database->columnWidth(8)));
				}
			}
		}
		else if (parameters->mode == denovoengine) {
			if (parameters->generateisotopepattern) {
				database->setColumnWidth(9, min(400, database->columnWidth(9)));
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				database->setColumnWidth(8, min(400, database->columnWidth(8)));
			}
		}

		QStringList completerlist;
		if (completernameset.size() <= 10000) {
			for (auto& it : completernameset) {
				completerlist.push_back(it);
			}
		}
		rowsfilterstringlistmodel1->setStringList(completerlist);
		rowsfilterstringlistmodel2->setStringList(completerlist);
	}

	setWindowTitle(title);

	progress->setValue(spectracount);
	delete progress;

	return true;
}


void cSummaryPeaksTableWidget::deleteTable() {
	for (int i = 0; i < databasemodel->columnCount(); i++) {
		if (database->itemDelegateForColumn(i)) {
			delete database->itemDelegateForColumn(i);
		}
	}
	databasemodel->clear();
}


void cSummaryPeaksTableWidget::updateFilterBySelectedRegion(int xmin, int xmax, int ymin, int ymax) {
	proxymodel->setRegion(xmin, xmax, ymin, ymax);
	filterRows();
}


void cSummaryPeaksTableWidget::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		if (lastdirexporttocsv.right(4).compare(".csv", Qt::CaseInsensitive) != 0) {
			lastdirexporttocsv = globalpreferences->exportcsvdefaultdir;
		}
		if (lastdirexportstatisticstocsv.right(4).compare(".csv", Qt::CaseInsensitive) != 0) {
			lastdirexportstatisticstocsv = globalpreferences->exportcsvdefaultdir;
		}
	}
}


void cSummaryPeaksTableWidget::addEICPeak(cPeaksList& eicchromatogram, cTheoreticalSpectrum& theoreticalspectrum, cPeaksList& experimentalspectrum) {
	double intensity = 0;
	int size;

	set<string> matchedcompounds;
	string matcheditem;
	size_t pos;

	size = experimentalspectrum.size();
	for (int i = 0; i < size; i++) {
		if (experimentalspectrum[i].matched > 0) {
			intensity += experimentalspectrum[i].absoluteintensity;
		}
	}

	size = theoreticalspectrum.getNumberOfPeaks();
	for (int i = 0; i < size; i++) {
		if (theoreticalspectrum[i].matched > 0) {
			matcheditem.clear();

			pos = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].find("</a>");
			if (pos != string::npos) {
				matcheditem = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].substr(0, pos + 4);
			}

			if (!matcheditem.empty()) {
				matchedcompounds.insert(matcheditem);
			}
		}
	}

	matcheditem.clear();
	for (auto& it : matchedcompounds) {
		matcheditem += it;
		matcheditem += "<br/>";
	}

	cPeak peak;
	peak.mzratio = (double)(eicchromatogram.size() + 1);
	peak.absoluteintensity = intensity;
	peak.description = matcheditem;
	eicchromatogram.add(peak);
}


void cSummaryPeaksTableWidget::addCoordinateInfo(int spectrumindex, vector<cCoordinateInfo>& coordinateinfo, cTheoreticalSpectrum& theoreticalspectrum, cPeaksList& experimentalspectrum) {
	cCoordinateInfo item;
	int size;

	set<string> matchedcompounds;
	string matcheditem, tmpstr;
	size_t pos1, pos2;

	size = experimentalspectrum.size();
	for (int i = 0; i < size; i++) {
		if (experimentalspectrum[i].matched > 0) {
			item.relativeintensity += experimentalspectrum[i].relativeintensity;
			item.absoluteintensity += experimentalspectrum[i].absoluteintensity;
		}
	}

	size = theoreticalspectrum.getNumberOfPeaks();
	for (int i = 0; i < size; i++) {
		if (theoreticalspectrum[i].matched > 0) {
			matcheditem.clear();

			pos1 = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].find('<');
			if (pos1 != string::npos) {
				matcheditem = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].substr(0, pos1 - 1);

				pos1 = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].rfind("</a>");
				pos2 = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].find('>');
				if ((pos1 != string::npos) && (pos2 != string::npos)) {
					tmpstr = parameters->peakidtodesc[theoreticalspectrum[i].descriptionid].substr(pos2 + 1, pos1 - pos2 - 1);
					if (tmpstr.size() > 40) {
						matcheditem += tmpstr.substr(0, 37) + "...";
					}
					else {
						matcheditem += tmpstr;
					}
				}
				else {
					matcheditem.clear();
				}
			}

			if (!matcheditem.empty()) {
				matchedcompounds.insert(matcheditem);
			}
		}
	}

	matcheditem.clear();
	for (auto& it : matchedcompounds) {
		if (!matcheditem.empty()) {
			matcheditem += "\n";
		}
		matcheditem += it;
	}

	item.id = spectrumindex + 1;
	item.x = experimentalspectrum.getCoordinateX();
	item.y = experimentalspectrum.getCoordinateY();
	item.description = matcheditem;

	if (item.absoluteintensity > 0) {
		coordinateinfo.push_back(item);
	}
}


void cSummaryPeaksTableWidget::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline1->hasFocus() || rowsfilterline2->hasFocus()) {
			filterRows();
		}
		else {
			if (database->selectionModel()->selectedIndexes().count() > 0) {
				rowDoubleClicked(database->selectionModel()->selectedIndexes()[0]);
			}
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline1->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_T)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_W)) {
		rowsfilterwholeword->setChecked(!rowsfilterwholeword->isChecked());
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


void cSummaryPeaksTableWidget::closeWindow() {
	hide();
}


void cSummaryPeaksTableWidget::filterRows() {
	setWindowTitle(title);

	Qt::CaseSensitivity casesensitive = rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
	QString str = "";
	int i, id, x, y;
	int iontypecol, mzcol, relintcol, absintcol, namecol, referencecol;

	cPeaksList eicchromatogram;
	eicchromatogram.clear();

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			iontypecol = 3;
			if (parameters->generateisotopepattern) {
				mzcol = 6;
				relintcol = 7;
				absintcol = 8;
				namecol = 13;
			}
			else {
				mzcol = 5;
				relintcol = 6;
				absintcol = 7;
				namecol = 10;
			}
		}
		else {
			iontypecol = 2;
			if (parameters->generateisotopepattern) {
				mzcol = 5;
				absintcol = 7;
				namecol = 12;
				referencecol = 13;
			}
			else {
				mzcol = 4;
				absintcol = 6;
				namecol = 9;
				referencecol = 10;
			}

			eicchromatogram = origeicchromatogram;
			for (i = 0; i < eicchromatogram.size(); i++) {
				eicchromatogram[i].absoluteintensity = 0;
				eicchromatogram[i].relativeintensity = 0;
				eicchromatogram[i].description = "";
			}
		}
	}

	QProgressDialog progress("Updating...", "Cancel", 0, databasemodel->rowCount(), this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);

	// filter table
	proxymodel->setWholeWord(rowsfilterwholeword->isChecked());
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterCaseSensitivity(casesensitive);
	proxymodel->setFilterFixedString(str);

	// prepare data for visualization
	vector<cCoordinateInfo> coordinateinfo;
	map<int, cCoordinateInfo> coordinatemap;
	map<int, set<string> > experimentalmz;
	string tmpdescription;

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			int proxymodelrowcount = proxymodel->rowCount();
			for (i = 0; i < proxymodelrowcount; i++) {
				x = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 1)))->data(Qt::DisplayRole).toInt();
				y = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 2)))->data(Qt::DisplayRole).toInt();

				if (((proxymodel->xmin == -1) && (proxymodel->xmax == -1) && (proxymodel->ymin == -1) && (proxymodel->ymax == -1)) || ((x >= proxymodel->xmin) && (x <= proxymodel->xmax) && (y >= proxymodel->ymin) && (y <= proxymodel->ymax))) {

					id = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->data(Qt::DisplayRole).toInt(); // ok

					tmpdescription = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, mzcol)))->data(Qt::DisplayRole).toString().toStdString();
					if (experimentalmz[id].count(tmpdescription) == 0) {
						if (coordinatemap.count(id) == 1) {
							coordinatemap[id].relativeintensity += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, relintcol)))->data(Qt::DisplayRole).toDouble();
							coordinatemap[id].absoluteintensity += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, absintcol)))->data(Qt::DisplayRole).toDouble();
						}
						else {
							coordinatemap[id].id = id;
							coordinatemap[id].x = x;
							coordinatemap[id].y = y;
							coordinatemap[id].relativeintensity = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, relintcol)))->data(Qt::DisplayRole).toDouble();
							coordinatemap[id].absoluteintensity = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, absintcol)))->data(Qt::DisplayRole).toDouble();
						}
						experimentalmz[id].insert(tmpdescription);
					}

					tmpdescription = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, iontypecol)))->text().toStdString() + " ";
					if (databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, namecol)))->text().length() > 40) {
						tmpdescription += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, namecol)))->text().toStdString().substr(0, 37) + "...";
					}
					else {
						tmpdescription += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, namecol)))->text().toStdString();
					}

					if (coordinatemap[id].description.find(tmpdescription) == string::npos) {
						if (!coordinatemap[id].description.empty()) {
							coordinatemap[id].description += "\n";
						}
						coordinatemap[id].description += tmpdescription;
					}

				}

				progress.setValue(i);

				if (progress.wasCanceled()) {
					resetFilter();
					break;
				}
			}

			if (!progress.wasCanceled()) {
				for (auto& it : coordinatemap) {
					if (it.second.absoluteintensity > 0) {
						coordinateinfo.push_back(it.second);
					}
				}
			}
		}
		else {
			int proxymodelrowcount = proxymodel->rowCount();
			for (i = 0; i < proxymodelrowcount; i++) {
				id = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, 0)))->data(Qt::DisplayRole).toInt() - 1; // ok

				tmpdescription = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, mzcol)))->data(Qt::DisplayRole).toString().toStdString();
				if (experimentalmz[id].count(tmpdescription) == 0) {
					eicchromatogram[id].absoluteintensity += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, absintcol)))->data(Qt::DisplayRole).toDouble();
					experimentalmz[id].insert(tmpdescription);
				}

				tmpdescription = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, iontypecol)))->text().toStdString();
				tmpdescription += " <a href=\"";
				tmpdescription += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, referencecol)))->text().toStdString();
				tmpdescription += "\">";
				tmpdescription += databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, namecol)))->text().toStdString();
				tmpdescription += "</a>";

				if (eicchromatogram[id].description.find(tmpdescription) == string::npos) {
					eicchromatogram[id].description += tmpdescription;
					eicchromatogram[id].description += "<br/>";
				}

				progress.setValue(i);

				if (progress.wasCanceled()) {
					resetFilter();
					break;
				}
			}
		}
	}

	if (((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) && !progress.wasCanceled()) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			emit sendFilterOptionsToImageWindow(coordinateinfo, (bool)rowsfilteroperator->currentIndex(), rowsfiltercombobox1->currentText().toStdString(), rowsfiltercomparatorcombobox1->currentText().toStdString(), rowsfilterline1->text().toStdString(), rowsfiltercombobox2->currentText().toStdString(), rowsfiltercomparatorcombobox2->currentText().toStdString(), rowsfilterline2->text().toStdString(), rowsfiltercasesensitive->isChecked(), rowsfilterwholeword->isChecked());
		}
		else {
			eicchromatogram.normalizeIntenzity();
			emit sendFilterOptionsToChromatogram(eicchromatogram);
		}
	}

	progress.setValue(databasemodel->rowCount());
}


void cSummaryPeaksTableWidget::resetFilter() {
	setWindowTitle(title);

	rowsfilterline1->setText("");
	rowsfilterline2->setText("");

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			vector<cCoordinateInfo> coordinateinfo;
			
			for (auto& it : origcoordinateinfo) {
				if (((proxymodel->xmin == -1) && (proxymodel->xmax == -1) && (proxymodel->ymin == -1) && (proxymodel->ymax == -1)) || ((it.x >= proxymodel->xmin) && (it.x <= proxymodel->xmax) && (it.y >= proxymodel->ymin) && (it.y <= proxymodel->ymax))) {
					coordinateinfo.push_back(it);
				}
			}

			emit sendFilterOptionsToImageWindow(coordinateinfo, (bool)rowsfilteroperator->currentIndex(), rowsfiltercombobox1->currentText().toStdString(), rowsfiltercomparatorcombobox1->currentText().toStdString(), rowsfilterline1->text().toStdString(), rowsfiltercombobox2->currentText().toStdString(), rowsfiltercomparatorcombobox2->currentText().toStdString(), rowsfilterline2->text().toStdString(), rowsfiltercasesensitive->isChecked(), rowsfilterwholeword->isChecked());
		}
		else {
			emit sendFilterOptionsToChromatogram(origeicchromatogram);
		}
	}

	database->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
	proxymodel->sort(-1);

	proxymodel->setWholeWord(false);
	proxymodel->setFilterKeyColumn(-1);
	proxymodel->setFilterFixedString("");
}


void cSummaryPeaksTableWidget::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));

	if (!filename.isEmpty()) {
		lastdirexporttocsv = filename;

		QProgressDialog progress("Exporting CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);

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


void cSummaryPeaksTableWidget::exportStatistics() {

	if (!parameters) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export Statistics"), lastdirexportstatisticstocsv, tr("Files (*.csv)"));

	if (!filename.isEmpty()) {
		lastdirexportstatisticstocsv = filename;

		QProgressDialog progress("Exporting statistics to CSV file...", "Cancel", 0, proxymodel->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);

		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		int iontypecol;
		int theoreticalmzcol;
		int summaryformulacol;
		int namecol;
		int referencecol;

		int idcol;
		int xcol, ycol;
		int relintcol, absintcol;
		int scorecol, fdrcol;

		int experimentalmzcol;
		int ppmcol;

		int thintcol;

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {

			if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
				if (parameters->generateisotopepattern) {
					iontypecol = 3;
					theoreticalmzcol = 4;
					summaryformulacol = 12;
					namecol = 13;
					referencecol = 14;
					scorecol = 10;
					fdrcol = 11;
					idcol = 0;
					xcol = 1;
					ycol = 2;
					relintcol = 7;
					absintcol = 8;
				}
				else {
					iontypecol = 3;
					theoreticalmzcol = 4;
					summaryformulacol = 9;
					namecol = 10;
					referencecol = 11;
					idcol = 0;
					xcol = 1;
					ycol = 2;
					relintcol = 6;
					absintcol = 7;
				}
			}
			else {
				if (parameters->generateisotopepattern) {
					iontypecol = 2;
					theoreticalmzcol = 3;
					summaryformulacol = 11;
					namecol = 12;
					referencecol = 13;
					scorecol = 9;
					fdrcol = 10;
					idcol = 0;
					relintcol = 6;
					absintcol = 7;
				}
				else {
					iontypecol = 2;
					theoreticalmzcol = 3;
					summaryformulacol = 8;
					namecol = 9;
					referencecol = 10;
					idcol = 0;
					relintcol = 5;
					absintcol = 6;
				}
			}

		}
		else if (parameters->mode == denovoengine) {
			if (parameters->generateisotopepattern) {
				theoreticalmzcol = 3;
				thintcol = 4;
				experimentalmzcol = 5;
				relintcol = 6;
				absintcol = 7;
				ppmcol = 8;
				summaryformulacol = 9;
			}
			else {
				theoreticalmzcol = 3;
				experimentalmzcol = 4;
				relintcol = 5;
				absintcol = 6;
				ppmcol = 7;
				summaryformulacol = 8;
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				theoreticalmzcol = 2;
				thintcol = 3;
				experimentalmzcol = 4;
				relintcol = 5;
				absintcol = 6;
				ppmcol = 7;
				summaryformulacol = 8;
			}
			else {
				theoreticalmzcol = 2;
				experimentalmzcol = 3;
				relintcol = 4;
				absintcol = 5;
				ppmcol = 6;
				summaryformulacol = 7;
			}
		}

		map<cSummaryTableKeyMS, vector<int>, cIonSummaryTableKeyMS_comp> ionmapms;
		map<cSummaryTableKeyMS, vector<int>, cEnvelopeSummaryTableKeyMS_comp> envelopemapms;
		cSummaryTableKeyMS keyms;

		map<cSummaryTableKeyMSMS, vector<int>, cSummaryTableKeyMSMS_comp> mapmsms;
		cSummaryTableKeyMSMS keymsms;

		QStandardItem* item;
		for (int i = 0; i < proxymodel->rowCount(); i++) {

			if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {

				keyms.clear();

				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, idcol)));
				if (item) {
					keyms.id = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, iontypecol)));
				if (item) {
					keyms.iontype = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, theoreticalmzcol)));
				if (item) {
					keyms.theoreticalmz = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, summaryformulacol)));
				if (item) {
					keyms.summaryformula = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, namecol)));
				if (item) {
					keyms.name = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, referencecol)));
				if (item) {
					keyms.reference = item->text().toStdString();
				}
				if (parameters->generateisotopepattern) {
					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, scorecol)));
					if (item) {
						keyms.score = item->text().toStdString();
					}
					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, fdrcol)));
					if (item) {
						keyms.fdr = item->text().toStdString();
					}
				}

				auto obj = ionmapms.find(keyms);
				if (obj != ionmapms.end()) {
					obj->second.push_back(i);
				}
				else {
					vector<int> v;
					v.push_back(i);
					ionmapms.insert(std::pair< cSummaryTableKeyMS, vector<int> >(keyms, v));
				}

				if (parameters->generateisotopepattern) {
					obj = envelopemapms.find(keyms);
					if (obj != envelopemapms.end()) {
						obj->second.push_back(i);
					}
					else {
						vector<int> v;
						v.push_back(i);
						envelopemapms.insert(std::pair< cSummaryTableKeyMS, vector<int> >(keyms, v));
					}
				}

			}
			else {

				keymsms.clear();

				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, theoreticalmzcol)));
				if (item) {
					keymsms.theoreticalmz = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, thintcol)));
				if (item) {
					keymsms.thint = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, experimentalmzcol)));
				if (item) {
					keymsms.experimentalmz = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, relintcol)));
				if (item) {
					keymsms.relint = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, absintcol)));
				if (item) {
					keymsms.absint = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, ppmcol)));
				if (item) {
					keymsms.ppmerror = item->text().toStdString();
				}
				item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(i, summaryformulacol)));
				if (item) {
					keymsms.summary = item->text().toStdString();
				}

				auto obj = mapmsms.find(keymsms);
				if (obj != mapmsms.end()) {
					obj->second.push_back(i);
				}
				else {
					vector<int> v;
					v.push_back(i);
					mapmsms.insert(std::pair< cSummaryTableKeyMSMS, vector<int> >(keymsms, v));
				}

			}

			progress.setValue(i);
			if (progress.wasCanceled()) {
				removefile = true;
				break;
			}

		}
		
		int idmin, idmax;
		int xmin, xmax, ymin, ymax;
		double relintmax, relintavg, relintmed, relintcur;
		double absintmax, absintavg, absintmed, absintcur;
		double scoremin, scoreavg, scoremed, scoremax, scorecur;
		double fdrmin, fdravg, fdrmed, fdrmax, fdrcur;
		vector<double> relintmedianvector;
		vector<double> absintmedianvector;
		vector<double> scoremedianvector;
		vector<double> fdrmedianvector;

		if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {

			auto cmpms = [](pair<cSummaryTableKeyMS, vector<int> > const &a, pair<cSummaryTableKeyMS, vector<int> > const &b) {
				if (a.first.name < b.first.name) {
					return true;
				}

				if (a.first.name > b.first.name) {
					return false;
				}

				if (a.first.iontype < b.first.iontype) {
					return true;
				}

				if (a.first.iontype > b.first.iontype) {
					return false;
				}

				if (a.first.theoreticalmz < b.first.theoreticalmz) {
					return true;
				}

				if (a.first.theoreticalmz > b.first.theoreticalmz) {
					return false;
				}

				return true;
			};

			if (parameters->generateisotopepattern) {

				vector< pair<cSummaryTableKeyMS, vector<int> > > envelopestatisticsms;

				for (auto it = envelopemapms.begin(); it != envelopemapms.end(); ++it) {
					envelopestatisticsms.push_back(*it);
				}

				sort(envelopestatisticsms.begin(), envelopestatisticsms.end(), cmpms);

				out << "\"Whole Pattern Statistics\"" << endl << endl;

				out << "\"Name\",\"Pattern Type\",\"Number of Patterns\",";
				out << "\"Minimum Score\",\"Average Score\",\"Median Score\",\"Maximum Score\",";
				out << "\"Minimum FDR\",\"Average FDR\",\"Median FDR\",\"Maximum FDR\",";
				out << "\"Reference\"" << endl;

				scoremin = DBL_MAX;
				scoremax = 0;
				scoreavg = 0;
				scoremedianvector.clear();

				fdrmin = DBL_MAX;
				fdrmax = 0;
				fdravg = 0;
				fdrmedianvector.clear();

				int envelopesstatisticssize = (int)envelopestatisticsms.size();
				int cnt = 0;
				for (int i = 0; i < envelopesstatisticssize; i++) {

					scorecur = QString(envelopestatisticsms[i].first.score.c_str()).toDouble();
					if (scorecur > scoremax) {
						scoremax = scorecur;
					}
					if (scorecur < scoremin) {
						scoremin = scorecur;
					}
					scoreavg += scorecur;
					scoremedianvector.push_back(scorecur);

					fdrcur = QString(envelopestatisticsms[i].first.fdr.c_str()).toDouble();
					if (fdrcur > fdrmax) {
						fdrmax = fdrcur;
					}
					if (fdrcur < fdrmin) {
						fdrmin = fdrcur;
					}
					fdravg += fdrcur;
					fdrmedianvector.push_back(fdrcur);

					cnt++;

					if ((i + 1 < envelopesstatisticssize) && (envelopestatisticsms[i].first.iontype.compare(envelopestatisticsms[i + 1].first.iontype) == 0)
						&& (envelopestatisticsms[i].first.name.compare(envelopestatisticsms[i + 1].first.name) == 0)
						&& (envelopestatisticsms[i].first.reference.compare(envelopestatisticsms[i + 1].first.reference) == 0)) {
						continue;
					}

					scoreavg /= (double)cnt;
					fdravg /= (double)cnt;

					scoremed = median(scoremedianvector);
					fdrmed = median(fdrmedianvector);

					out << "\"" << envelopestatisticsms[i].first.name.c_str() << "\",\"" << envelopestatisticsms[i].first.iontype.c_str() << "\",\"" << cnt << "\",\"";

					stringstream ss;
					ss << std::fixed << std::setprecision(6) << scoremin << "\",\"" << scoreavg << "\",\"" << scoremed << "\",\"" << scoremax << "\",\"";
					ss << std::fixed << std::setprecision(6) << fdrmin << "\",\"" << fdravg << "\",\"" << fdrmed << "\",\"" << fdrmax << "\",\"";
					out << ss.str().c_str();

					out << envelopestatisticsms[i].first.reference.c_str() << "\"" << endl;

					scoremin = DBL_MAX;
					scoremax = 0;
					scoreavg = 0;
					scoremedianvector.clear();

					fdrmin = DBL_MAX;
					fdrmax = 0;
					fdravg = 0;
					fdrmedianvector.clear();

					cnt = 0;

				}

				out << endl << endl;

			}

			out << "\"Single Peak Statistics\"" << endl << endl;

			out << "\"Name\",";
			if (parameters->generateisotopepattern) {
				out << "\"Pattern Type\",";
			}
			else {
				out << "\"Ion Type\",";
			}
			out << "\"Number of Spectra\",\"Theoretical m/z\",";
			out << "\"Average Relative Intensity [%]\",\"Median Relative Intensity [%]\",\"Maximum Relative Intensity [%]\",";
			out << "\"Average Absolute Intensity\",\"Median Absolute Intensity\",\"Maximum Absolute Intensity\",";
			out << "\"ID (min)\",\"ID (max)\",";
			if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
				out << "\"Coordinate X (min)\",\"Coordinate X (max)\",\"Coordinate Y (min)\",\"Coordinate Y (max)\",";
			}
			out << "\"Summary Formula\"" << endl;

			vector< pair<cSummaryTableKeyMS, vector<int> > > ionstatisticsms;

			for (auto it = ionmapms.begin(); it != ionmapms.end(); ++it) {
				ionstatisticsms.push_back(*it);
			}

			sort(ionstatisticsms.begin(), ionstatisticsms.end(), cmpms);

			for (auto it = ionstatisticsms.begin(); it != ionstatisticsms.end(); ++it) {

				idmin = INT32_MAX;
				idmax = 0;

				xmin = INT32_MAX;
				xmax = 0;

				ymin = INT32_MAX;
				ymax = 0;

				relintmax = 0;
				relintavg = 0;
				relintmedianvector.clear();

				absintmax = 0;
				absintavg = 0;
				absintmedianvector.clear();

				for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2) {
					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, idcol)));
					if (item) {
						if (item->data(Qt::DisplayRole).toInt() < idmin) {
							idmin = item->data(Qt::DisplayRole).toInt();
						}
						if (item->data(Qt::DisplayRole).toInt() > idmax) {
							idmax = item->data(Qt::DisplayRole).toInt();
						}
					}

					if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {

						item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, xcol)));
						if (item) {
							if (item->data(Qt::DisplayRole).toInt() < xmin) {
								xmin = item->data(Qt::DisplayRole).toInt();
							}
							if (item->data(Qt::DisplayRole).toInt() > xmax) {
								xmax = item->data(Qt::DisplayRole).toInt();
							}
						}

						item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, ycol)));
						if (item) {
							if (item->data(Qt::DisplayRole).toInt() < ymin) {
								ymin = item->data(Qt::DisplayRole).toInt();
							}
							if (item->data(Qt::DisplayRole).toInt() > ymax) {
								ymax = item->data(Qt::DisplayRole).toInt();
							}
						}

					}

					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, relintcol)));
					if (item) {
						relintcur = item->data(Qt::DisplayRole).toDouble();
						if (relintcur > relintmax) {
							relintmax = relintcur;
						}
						relintavg += relintcur;
						relintmedianvector.push_back(relintcur);
					}

					item = databasemodel->itemFromIndex(proxymodel->mapToSource(proxymodel->index(*it2, absintcol)));
					if (item) {
						absintcur = item->data(Qt::DisplayRole).toDouble();
						if (absintcur > absintmax) {
							absintmax = absintcur;
						}
						absintavg += absintcur;
						absintmedianvector.push_back(absintcur);
					}
				}

				relintavg /= (double)it->second.size();
				absintavg /= (double)it->second.size();

				relintmed = median(relintmedianvector);
				absintmed = median(absintmedianvector);

				out << "\"" << it->first.name.c_str() << "\",\"" << it->first.iontype.c_str() << "\",\"" << it->second.size() << "\",\"" << it->first.theoreticalmz.c_str() << "\",\"";

				stringstream ss;
				ss << std::fixed << std::setprecision(2) << relintavg << "\",\"" << relintmed << "\",\"" << relintmax << "\",\"";
				ss << std::fixed << std::setprecision(0) << absintavg << "\",\"" << absintmed << "\",\"" << absintmax << "\",\"";
				out << ss.str().c_str();

				out << idmin << "\",\"" << idmax << "\",\"";
				if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
					out << xmin << "\",\"" << xmax << "\",\"" << ymin << "\",\"" << ymax << "\",\"";
				}

				out << stripHTML(it->first.summaryformula).c_str() << "\"" << endl;

			}

		}
		else {

			out << "\"Number of Matches\",\"Theoretical m/z\",";
			if (parameters->generateisotopepattern) {
				out << "\"Theoretical Intensity [%]\",";
			}
			out << "\"Experimental m/z\",\"Relative Intensity [%]\",\"Absolute Intensity\",\"Error [ppm]\",\"Summary Formula\"" << endl;

			vector< pair<cSummaryTableKeyMSMS, vector<int> > > statisticsmsms;

			for (auto it = mapmsms.begin(); it != mapmsms.end(); ++it) {
				statisticsmsms.push_back(*it);
			}

			auto cmpmsms = [](pair<cSummaryTableKeyMSMS, vector<int> > const &a, pair<cSummaryTableKeyMSMS, vector<int> > const &b) {
				return a.second.size() > b.second.size();
			};
			sort(statisticsmsms.begin(), statisticsmsms.end(), cmpmsms);

			for (auto it = statisticsmsms.begin(); it != statisticsmsms.end(); ++it) {
				out << "\"" << it->second.size() << "\",\"" << it->first.theoreticalmz.c_str() << "\",";
				if (parameters->generateisotopepattern) {
					out << "\"" << it->first.thint.c_str() << "\",";
				}
				out << "\"" << it->first.experimentalmz.c_str() << "\",\"" << it->first.relint.c_str() << "\",\"" << it->first.absint.c_str() << "\",\"" << it->first.ppmerror.c_str() << "\",\"" << stripHTML(it->first.summary).c_str() << "\"" << endl;
			}

		}
	
		file.close();

		if (removefile) {
			file.remove();
		}

		progress.setValue(proxymodel->rowCount());
	}
}


void cSummaryPeaksTableWidget::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/summarypeakstable.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/summarypeakstable.html").absoluteFilePath()));
	#endif
}


void cSummaryPeaksTableWidget::rowDoubleClicked(const QModelIndex& item) {
	int size;

	int idcolumn = 0;
	int experimentalmzcolumn = 0;

	if ((parameters->mode == dereplication) || (parameters->mode == compoundsearch)) {
		if ((parameters->peaklistfileformat == mis) || (parameters->peaklistfileformat == imzML)) {
			if (parameters->generateisotopepattern) {
				size = 14;
				experimentalmzcolumn = 6;
			}
			else {
				size = 11;
				experimentalmzcolumn = 5;
			}
		}
		else {
			if (parameters->generateisotopepattern) {
				size = 13;
				experimentalmzcolumn = 5;
			}
			else {
				size = 10;
				experimentalmzcolumn = 4;
			}
		}
	}
	else if (parameters->mode == denovoengine) {
		if (parameters->generateisotopepattern) {
			size = 11;
			experimentalmzcolumn = 5;
		}
		else {
			size = 10;
			experimentalmzcolumn = 4;
		}
	}
	else {
		if (parameters->generateisotopepattern) {
			size = 10;
			experimentalmzcolumn = 4;
		}
		else {
			size = 9;
			experimentalmzcolumn = 3;
		}
	}

	int row = proxymodel->mapToSource(item).row();
	int column = proxymodel->mapToSource(item).column();

	if (column < size) {
		emit summaryPeaksTableRowDoubleClicked(databasemodel->item(row, idcolumn)->data(Qt::DisplayRole).toInt() - 1, databasemodel->item(row, experimentalmzcolumn)->data(Qt::DisplayRole).toDouble());
	}
}

void cSummaryPeaksTableWidget::rowsFilterLeft1Slot() {
	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		return;
	}

	QStringList list = ((QStringListModel*)rowsfilterlinecompleter1->model())->stringList();
	int index = list.indexOf(rowsfilterline1->text());
	int comboindex = rowsfiltercombobox1->findText("Name");

	if ((index > 0) && (comboindex >= 0)) {
		rowsfiltercombobox1->setCurrentIndex(comboindex);
		rowsfiltercomparatorcombobox1->setCurrentIndex(0);
		rowsfilterline1->setText(list[index - 1]);
		filterRows();
		setWindowTitle(title + " - compound " + QVariant(index).toString() + " of " + QVariant(((QStringListModel*)rowsfilterlinecompleter1->model())->stringList().size()).toString());
	}
	else {
		setWindowTitle(title);
	}
}


void cSummaryPeaksTableWidget::rowsFilterRight1Slot() {
	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		return;
	}

	QStringList list = ((QStringListModel*)rowsfilterlinecompleter1->model())->stringList();
	int index = list.indexOf(rowsfilterline1->text());
	int comboindex = rowsfiltercombobox1->findText("Name");

	if ((index < list.size() - 1) && (comboindex >= 0)) {
		rowsfiltercombobox1->setCurrentIndex(comboindex);
		rowsfiltercomparatorcombobox1->setCurrentIndex(0);
		rowsfilterline1->setText(list[index + 1]);
		filterRows();
		setWindowTitle(title + " - compound " + QVariant(index + 2).toString() + " of " + QVariant(((QStringListModel*)rowsfilterlinecompleter1->model())->stringList().size()).toString());
	}
	else {
		setWindowTitle(title);
	}
}


void cSummaryPeaksTableWidget::rowsFilterLeft2Slot() {
	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		return;
	}

	QStringList list = ((QStringListModel*)rowsfilterlinecompleter2->model())->stringList();
	int index = list.indexOf(rowsfilterline2->text());
	int comboindex = rowsfiltercombobox2->findText("Name");

	if ((index > 0) && (comboindex >= 0)) {
		rowsfiltercombobox2->setCurrentIndex(comboindex);
		rowsfiltercomparatorcombobox2->setCurrentIndex(0);
		rowsfilterline2->setText(list[index - 1]);
		filterRows();
		setWindowTitle(title + " - compound " + QVariant(index).toString() + " of " + QVariant(((QStringListModel*)rowsfilterlinecompleter2->model())->stringList().size()).toString());
	}
	else {
		setWindowTitle(title);
	}
}


void cSummaryPeaksTableWidget::rowsFilterRight2Slot() {
	if ((parameters->mode != dereplication) && (parameters->mode != compoundsearch)) {
		return;
	}

	QStringList list = ((QStringListModel*)rowsfilterlinecompleter2->model())->stringList();
	int index = list.indexOf(rowsfilterline2->text());
	int comboindex = rowsfiltercombobox2->findText("Name");

	if ((index < list.size() - 1) && (comboindex >= 0)) {
		rowsfiltercombobox2->setCurrentIndex(comboindex);
		rowsfiltercomparatorcombobox2->setCurrentIndex(0);
		rowsfilterline2->setText(list[index + 1]);
		filterRows();
		setWindowTitle(title + " - compound " + QVariant(index + 2).toString() + " of " + QVariant(((QStringListModel*)rowsfilterlinecompleter2->model())->stringList().size()).toString());
	}
	else {
		setWindowTitle(title);
	}
}

