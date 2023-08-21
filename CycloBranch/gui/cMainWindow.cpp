#include "gui/cMainWindow.h"
#include "gui/cEventFilter.h"

#include <QApplication>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QTextStream>
#include <QTextEdit>
#include <QSplitter>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCloseEvent>
#include <QClipboard>
#include <QApplication>


cMainWindow::cMainWindow() {
	setWindowTitle(appname);
	setWindowIcon(QIcon(":/images/cb.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	
	menuFile = new QMenu(tr("&File"), this);
	menuSearch = new QMenu(tr("&Search"), this);
	menuTools = new QMenu(tr("&Tools"), this);
	menuBookmarks = new QMenu(tr("&Bookmarks"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	actionOpenResults = new QAction(QIcon(":/images/icons/52.png"), tr("&Open Results..."), this);
	actionOpenResults->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

	actionSaveResults = new QAction(QIcon(":/images/icons/22.png"), tr("&Save Results..."), this);
	actionSaveResults->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

	actionExportToCsv = new QAction(QIcon(":/images/icons/csv.png"), tr("Export to &CSV"), this);
	actionExportToCsv->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C));
	
	actionExportToHTML = new QAction(QIcon(":/images/icons/77.png"), tr("Export to &HTML"), this);
	actionExportToHTML->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_H));

	actionPreferences = new QAction(QIcon(":/images/icons/preferences.png"), tr("&Preferences"), this);
	actionPreferences->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_P));

	actionQuit = new QAction(QIcon(":/images/icons/33.png"), tr("&Quit"), this);
	actionQuit->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
	
	actionRun = new QAction(QIcon(":/images/icons/93.png"), tr("&Run"), this);
	actionRun->setShortcut(QKeySequence(Qt::Key_F5));

	actionStop = new QAction(QIcon(":/images/icons/89.png"), tr("S&top"), this);
	actionStop->setShortcut(QKeySequence(Qt::Key_F6));
	actionStop->setEnabled(false);

	actionProperties = new QAction(QIcon(":/images/icons/73.png"), tr("&Settings..."), this);
	actionProperties->setShortcut(QKeySequence(Qt::Key_F7));

	actionBricksDatabase = new QAction(QIcon(":/images/icons/68.png"), tr("&Building Blocks Editor"), this);
	actionBricksDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));

	actionSequenceDatabase = new QAction(QIcon(":/images/icons/26.png"), tr("Sequence/Compound &Database Editor"), this);
	actionSequenceDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

	actionModifications = new QAction(QIcon(":/images/icons/61.png"), tr("&Modifications Editor"), this);
	actionModifications->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));

	actionDrawPeptide = new QAction(QIcon(":/images/icons/96.png"), tr("Draw &Peptide"), this);
	actionDrawPeptide->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));

	actionSummaryTableOfMatchedPeaks = new QAction(QIcon(":/images/icons/table.png"), tr("S&ummary Table of Matched Peaks"), this);
	actionSummaryTableOfMatchedPeaks->setShortcut(QKeySequence(Qt::Key_F8));

	actionMultipleDatasets = new QAction(QIcon(":/images/icons/features.png"), tr("&Annotated Features Overview"), this);
	actionMultipleDatasets->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_F8));

	actionChromatogramWindow = new QAction(QIcon(":/images/icons/chromatography.png"), tr("C&hromatogram"), this);
	actionChromatogramWindow->setShortcut(QKeySequence(Qt::Key_F9));
	actionChromatogramWindow->setDisabled(true);

	actionImageWindow = new QAction(QIcon(":/images/icons/image_edit.png"), tr("Image &Fusion"), this);
	actionImageWindow->setShortcut(QKeySequence(Qt::Key_F10));
	actionImageWindow->setDisabled(true);

	actionPatternSimulatorWindow = new QAction(QIcon(":/images/icons/pattern.png"), tr("&Simulate Isotopic Pattern"), this);
	actionPatternSimulatorWindow->setShortcut(QKeySequence(Qt::Key_F11));

	actionFormulaPredictor = new QAction(QIcon(":/images/icons/formula.png"), tr("Calculate Molecula&r Formula"), this);
	actionFormulaPredictor->setShortcut(QKeySequence(Qt::Key_F12));

	actionBookMark1 = new QAction("", this);
	actionBookMark1->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));

	actionBookMark2 = new QAction("", this);
	actionBookMark2->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));

	actionBookMark3 = new QAction("", this);
	actionBookMark3->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));

	actionBookMark4 = new QAction("", this);
	actionBookMark4->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_4));

	actionBookMark5 = new QAction("", this);
	actionBookMark5->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_5));

	actionBookMark6 = new QAction("", this);
	actionBookMark6->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_6));

	actionBookMark7 = new QAction("", this);
	actionBookMark7->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_7));

	actionBookMark8 = new QAction("", this);
	actionBookMark8->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_8));

	actionBookMark9 = new QAction("", this);
	actionBookMark9->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_9));

	actionBookMark10 = new QAction("", this);
	actionBookMark10->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_0));

	actionShowIsomers = new QAction(QIcon(":/images/icons/95.png"), tr("Show &Isomers"), this);
	actionShowIsomers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionShowIsomers->setCheckable(true);

	actionGraph = new QAction(QIcon(":/images/icons/32.png"), tr("De Novo &Graph"), this);
	actionGraph->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

	actionLog = new QAction(QIcon(":/images/icons/2.png"), tr("&Log Window"), this);
	actionLog->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	actionLog->setCheckable(true);
	actionLog->setChecked(true);

	actionHTMLDocumentation = new QAction(QIcon(":/images/icons/3.png"), tr("&HTML Documentation"), this);
	actionHTMLDocumentation->setShortcut(QKeySequence(Qt::Key_F1));

	actionPDFManual = new QAction(QIcon(":/images/icons/4.png"), tr("&PDF Manual"), this);
	actionPDFManual->setShortcut(QKeySequence(Qt::Key_F2));

	actionAbout = new QAction(QIcon(":/images/icons/48.png"), tr("&About"), this);
	actionAbout->setShortcut(QKeySequence(Qt::Key_F3));

	toolbarFile = addToolBar(tr("File"));
	toolbarFile->addAction(actionOpenResults);
	toolbarFile->addAction(actionSaveResults);
	toolbarFile->addAction(actionExportToCsv);
	toolbarFile->addAction(actionExportToHTML);
	toolbarFile->addAction(actionPreferences);
	toolbarFile->addAction(actionQuit);

	toolbarSearch = addToolBar(tr("Search"));
	toolbarSearch->addAction(actionRun);
	toolbarSearch->addAction(actionStop);
	toolbarSearch->addAction(actionProperties);

	toolbarTools = addToolBar(tr("Tools"));
	toolbarTools->addAction(actionBricksDatabase);
	toolbarTools->addAction(actionSequenceDatabase);
	toolbarTools->addAction(actionModifications);
	toolbarTools->addAction(actionDrawPeptide);
	toolbarTools->addAction(actionSummaryTableOfMatchedPeaks);
	toolbarTools->addAction(actionMultipleDatasets);
	toolbarTools->addAction(actionChromatogramWindow);
	toolbarTools->addAction(actionImageWindow);
	toolbarTools->addAction(actionPatternSimulatorWindow);
	toolbarTools->addAction(actionFormulaPredictor);

	toolbarView = addToolBar(tr("View"));
	toolbarView->addAction(actionShowIsomers);
	toolbarView->addAction(actionGraph);
	toolbarView->addAction(actionLog);

	toolbarHelp = addToolBar(tr("Help"));
	toolbarHelp->addAction(actionHTMLDocumentation);
	toolbarHelp->addAction(actionPDFManual);
	toolbarHelp->addAction(actionAbout);

	addToolBarBreak();

	toolbarBookmarks = addToolBar(tr("Bookmarks"));
	toolbarBookmarks->addAction(actionBookMark1);
	toolbarBookmarks->addAction(actionBookMark2);
	toolbarBookmarks->addAction(actionBookMark3);
	toolbarBookmarks->addAction(actionBookMark4);
	toolbarBookmarks->addAction(actionBookMark5);
	toolbarBookmarks->addAction(actionBookMark6);
	toolbarBookmarks->addAction(actionBookMark7);
	toolbarBookmarks->addAction(actionBookMark8);
	toolbarBookmarks->addAction(actionBookMark9);
	toolbarBookmarks->addAction(actionBookMark10);

	inputfiltercombobox = new QComboBox();
	inputfiltercombobox->setToolTip("Select the input file.");
	inputfiltercombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	inputfilterbutton = new QPushButton("Select");
	inputfilterbutton->setToolTip("Select the input file.");
	inputfilterbutton->setMinimumWidth(75);

	inputfilterhbox = new QHBoxLayout();
	inputfilterhbox->addWidget(inputfiltercombobox);
	inputfilterhbox->addSpacing(10);
	inputfilterhbox->addWidget(inputfilterbutton);

	inputfilterwidget = new QWidget();
	inputfilterwidget->setLayout(inputfilterhbox);

	addToolBarBreak();

	toolbarInputFilter = addToolBar(tr("Input"));
	toolbarInputFilter->addWidget(inputfilterwidget);
	toolbarInputFilter->setVisible(false);

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

	rowsfilterline1 = new QLineEdit();
	rowsfilterline1->setMinimumWidth(150);
	rowsfilterline1->setToolTip("Text to Find");

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

	rowsfilterline2 = new QLineEdit();
	rowsfilterline2->setMinimumWidth(150);
	rowsfilterline2->setToolTip("Text to Find");

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
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilteroperator);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercombobox2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercomparatorcombobox2);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterline2);
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

	addToolBarBreak();

	toolbarRowsFilter = addToolBar(tr("Filter"));
	toolbarRowsFilter->addWidget(rowsfilterwidget);

	results = new QTableView(this);
	resultsmodel = new QStandardItemModel(0, 0, this);
	resultsproxymodel = new cMainWindowProxyModel(this);
	resultsproxymodel->initialize(rowsfilteroperator, rowsfiltercombobox1, rowsfiltercomparatorcombobox1, rowsfilterline1, rowsfiltercombobox2, rowsfiltercomparatorcombobox2, rowsfilterline2);

	logWindow = new QTextEdit(this);
	logWindow->setPlainText("To start the program: click \"Search -> Settings\", configure the tool and then click \"Search -> Run\".");
	splitter = new QSplitter(this);

	about = new cAboutWidget(this);

	graph = new cGraphWidget();

	globalpreferences.loadSettings();
	bricksdatabasewidget = new cBricksDatabaseWidget(&globalpreferences);
	sequencedatabasewidget = new cSequenceDatabaseWidget(&globalpreferences, this);
	modificationswidget = new cModificationsWidget(&globalpreferences);
	drawpeptidewidget = new cDrawPeptideWidget(this);
	summarytableofmatchedpeaks = new cSummaryPeaksTableWidget(&globalpreferences, this);
	imagewindow = new cImageWindow(&globalpreferences, this);
	chromatogramwindow = new cChromatogramWindow(&globalpreferences, listoftheoreticalspectra, this);
	parameterswidget = new cParametersWidget(&globalpreferences, this);
	htmlexportdialog = new cHTMLExportDialog(this);
	preferencesdialog = new cPreferencesDialog(globalpreferences, this);
	multipledatasetswidget = new cMultipleDatasetsTableWidget(&globalpreferences, this);
	patternsimulatorwidget = new cPatternSimulatorWidget(&globalpreferences, this);
	formulapredictor = new cFormulaPredictor(&globalpreferences, this);

	connect(actionOpenResults, SIGNAL(triggered()), this, SLOT(openResultsFile()));
	connect(actionSaveResults, SIGNAL(triggered()), this, SLOT(saveResultsFile()));
	connect(actionExportToCsv, SIGNAL(triggered()), this, SLOT(exportToCsv()));
	connect(actionExportToHTML, SIGNAL(triggered()), this, SLOT(exportToHTML()));
	connect(actionPreferences, SIGNAL(triggered()), this, SLOT(showPreferences()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(quitApplication()));
	connect(actionRun, SIGNAL(triggered()), this, SLOT(run()));
	connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
	connect(actionProperties, SIGNAL(triggered()), this, SLOT(showProperties()));
	connect(actionBricksDatabase, SIGNAL(triggered()), this, SLOT(showBricksDatabase()));
	connect(actionSequenceDatabase, SIGNAL(triggered()), this, SLOT(showSequenceDatabase()));
	connect(actionModifications, SIGNAL(triggered()), this, SLOT(showModifications()));
	connect(actionDrawPeptide, SIGNAL(triggered()), this, SLOT(showDrawPeptideWidget()));
	connect(actionBookMark1, SIGNAL(triggered()), this, SLOT(gotoBookMark1()));
	connect(actionBookMark2, SIGNAL(triggered()), this, SLOT(gotoBookMark2()));
	connect(actionBookMark3, SIGNAL(triggered()), this, SLOT(gotoBookMark3()));
	connect(actionBookMark4, SIGNAL(triggered()), this, SLOT(gotoBookMark4()));
	connect(actionBookMark5, SIGNAL(triggered()), this, SLOT(gotoBookMark5()));
	connect(actionBookMark6, SIGNAL(triggered()), this, SLOT(gotoBookMark6()));
	connect(actionBookMark7, SIGNAL(triggered()), this, SLOT(gotoBookMark7()));
	connect(actionBookMark8, SIGNAL(triggered()), this, SLOT(gotoBookMark8()));
	connect(actionBookMark9, SIGNAL(triggered()), this, SLOT(gotoBookMark9()));
	connect(actionBookMark10, SIGNAL(triggered()), this, SLOT(gotoBookMark10()));
	connect(actionShowIsomers, SIGNAL(triggered()), this, SLOT(showIsomersStateChanged()));
	connect(actionGraph, SIGNAL(triggered()), this, SLOT(showGraph()));
	connect(actionSummaryTableOfMatchedPeaks, SIGNAL(triggered()), this, SLOT(showSummaryTableOfMatchedPeaks()));
	connect(actionMultipleDatasets, SIGNAL(triggered()), this, SLOT(showMultipleDatasetsOverview()));
	connect(actionChromatogramWindow, SIGNAL(triggered()), this, SLOT(showChromatogramWindow()));
	connect(actionImageWindow, SIGNAL(triggered()), this, SLOT(showImageWindow()));
	connect(actionPatternSimulatorWindow, SIGNAL(triggered()), this, SLOT(showPatternSimulatorWindow()));
	connect(actionFormulaPredictor, SIGNAL(triggered()), this, SLOT(showFormulaPredictor()));
	connect(actionLog, SIGNAL(triggered()), this, SLOT(showHideLog()));
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));
	connect(actionPDFManual, SIGNAL(triggered()), this, SLOT(showPDFManual()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

	connect(inputfilterbutton, SIGNAL(released()), this, SLOT(inputFilterButtonReleased()));

	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterResults()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	connect(summarytableofmatchedpeaks, SIGNAL(tableCancelled()), this, SLOT(summaryPeaksTableCancelled()));
	connect(summarytableofmatchedpeaks, SIGNAL(summaryPeaksTableRowDoubleClicked(int, double)), this, SLOT(summaryPeaksTableRowDoubleClicked(int, double)));
	connect(multipledatasetswidget, SIGNAL(tableCancelled()), this, SLOT(multipleDatasetsTableCancelled()));
	connect(multipledatasetswidget, SIGNAL(multipleDatasetsTableRowClicked(int, int, int, int, int, string, string, string)), this, SLOT(multipleDatasetsTableRowClicked(int, int, int, int, int, string, string, string)));
	connect(summarytableofmatchedpeaks, SIGNAL(sendFilterOptionsToImageWindow(vector<cCoordinateInfo>, bool, string, string, string, string, string, string, bool, bool)), imagewindow, SLOT(setFilterOptionsSlot(vector<cCoordinateInfo>, bool, string, string, string, string, string, string, bool, bool)));
	connect(summarytableofmatchedpeaks, SIGNAL(sendFilterOptionsToChromatogram(cPeaksList)), chromatogramwindow, SLOT(setFilterOptionsSlot(cPeaksList)));
	connect(summarytableofmatchedpeaks, SIGNAL(resetRegion()), imagewindow, SLOT(clearSelection()));

	connect(multipledatasetswidget, SIGNAL(filterSummaryTable()), this, SLOT(filterSummaryTableIfPrepared()));

	connect(chromatogramwindow, SIGNAL(doubleClickedScanIDSignal(int)), this, SLOT(chromatogramDoubleClicked(int)));
	connect(imagewindow, SIGNAL(doubleClickedSpectrumIDSignal(int)), this, SLOT(imageWindowDoubleClicked(int)));

	connect(chromatogramwindow, SIGNAL(calculateAvgSpectrum(int, int)), this, SLOT(showAvgSpectrum(int, int)));

	menuFile->addAction(actionOpenResults);
	menuFile->addAction(actionSaveResults);
	menuFile->addSeparator();
	menuFile->addAction(actionExportToCsv);
	menuFile->addAction(actionExportToHTML);
	menuFile->addSeparator();
	menuFile->addAction(actionPreferences);
	menuFile->addSeparator();
	menuFile->addAction(actionQuit);

	menuSearch->addAction(actionRun);
	menuSearch->addAction(actionStop);
	menuSearch->addSeparator();
	menuSearch->addAction(actionProperties);

	menuTools->addAction(actionBricksDatabase);
	menuTools->addAction(actionSequenceDatabase);
	menuTools->addAction(actionModifications);
	menuTools->addSeparator();
	menuTools->addAction(actionDrawPeptide);
	menuTools->addSeparator();
	menuTools->addAction(actionSummaryTableOfMatchedPeaks);
	menuTools->addAction(actionMultipleDatasets);
	menuTools->addAction(actionChromatogramWindow);
	menuTools->addAction(actionImageWindow);
	menuTools->addSeparator();
	menuTools->addAction(actionPatternSimulatorWindow);
	menuTools->addAction(actionFormulaPredictor);

	menuBookmarks->addAction(actionBookMark1);
	menuBookmarks->addAction(actionBookMark2);
	menuBookmarks->addAction(actionBookMark3);
	menuBookmarks->addAction(actionBookMark4);
	menuBookmarks->addAction(actionBookMark5);
	menuBookmarks->addAction(actionBookMark6);
	menuBookmarks->addAction(actionBookMark7);
	menuBookmarks->addAction(actionBookMark8);
	menuBookmarks->addAction(actionBookMark9);
	menuBookmarks->addAction(actionBookMark10);

	menuView->addAction(actionShowIsomers);
	menuView->addSeparator();
	menuView->addAction(actionGraph);
	menuView->addAction(actionLog);

	menuHelp->addAction(actionHTMLDocumentation);
	menuHelp->addAction(actionPDFManual);
	menuHelp->addSeparator();
	menuHelp->addAction(actionAbout);

	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuSearch);
	menuBar->addMenu(menuTools);
	menuBar->addMenu(menuBookmarks);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	setMenuBar(menuBar);

	resultsproxymodel->setSourceModel(resultsmodel);
	resultsproxymodel->setDynamicSortFilter(false);
	results->setModel(resultsproxymodel);
	results->setSelectionBehavior(QAbstractItemView::SelectItems);
	results->setSelectionMode(QAbstractItemView::ContiguousSelection);
	results->setEditTriggers(QAbstractItemView::NoEditTriggers);
	results->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	results->horizontalHeader()->setSectionsMovable(true);
	results->setSortingEnabled(true);
	connect(results, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(rowDoubleClicked(const QModelIndex&)));

	logWindow->setReadOnly(true);

	about->hide();
	graph->hide();
	bricksdatabasewidget->hide();
	sequencedatabasewidget->hide();
	modificationswidget->hide();
	drawpeptidewidget->hide();
	summarytableofmatchedpeaks->hide();
	imagewindow->hide();
	chromatogramwindow->hide();
	parameterswidget->hide();

	multipledatasetswidget->hide();
	multipledatasetswidget->getPubChemSearchWidget()->hide();
	multipledatasetswidget->getPubChemSearchWidget()->setHTML("");
	multipledatasetswidget->getDefineCalibrationWidget()->hide();
	multipledatasetswidget->getCalibrationChartWidget()->hide();
	
	patternsimulatorwidget->hide();
	formulapredictor->hide();

	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(results);
	splitter->addWidget(logWindow);
	splitter->setStretchFactor(0, 1);

	setCentralWidget(splitter);

	resize(defaultwinsizex, defaultwinsizey);

	listoftheoreticalspectra.clear();
	eicchromatograms.clear();
	spectradetails.clear();
	avgspectra.clear();

	parameters.clear();
	
	rawdata.clear();
	imzmlprofilemetadata.clear();
	profilemz64precision = false;
	profileintensity64precision = false;

	resultsbasecolumncount = 11;
	resultsspecificcolumncount = 0;
	searchspecificcolumncount = 0;

	applyGlobalPreferences();

	summarytableisprepared = false;
	multipledatasetsisprepared = false;

	lastactivedetail = -1;
	
	othernormalgeometry = QRect();
	otherismaximized = false;
	otherprofilespectrum = false;
	otherabsoluteintensity = false;
	othermatchedpeaks = false;
	otherunmatchedpeaks = false;
	otherscrambledpeaks = false;
	otherlabelspeaks = false;
	othermzselection = true;

	activefileid = 0;

	avgspectrumused = -1;
	avgspectrumabsoluteintensityenabled = true;
	avgspectrumgeometry = QRect();
	avgspectrummaximized = false;

	quitapp = false;
}


cMainWindow::~cMainWindow() {
	deleteResults();

	delete inputfiltercombobox;
	delete inputfilterbutton;
	delete inputfilterhbox;
	delete inputfilterwidget;

	delete rowsfilteroperator;
	delete rowsfiltercombobox1;
	delete rowsfiltercomparatorcombobox1;
	delete rowsfilterline1;
	delete rowsfiltercombobox2;
	delete rowsfiltercomparatorcombobox2;
	delete rowsfilterline2;
	delete rowsfiltercasesensitive;
	delete rowsfilterwholeword;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

	delete resultsmodel;
	delete resultsproxymodel;
	delete results;
    
	delete logWindow;
	delete splitter;
		
	delete about;
	delete graph;
	delete bricksdatabasewidget;
	delete sequencedatabasewidget;
	delete modificationswidget;
	delete drawpeptidewidget;
	delete summarytableofmatchedpeaks;
	delete chromatogramwindow;
	delete imagewindow;
	delete parameterswidget;
	delete htmlexportdialog;
	delete preferencesdialog;
	delete multipledatasetswidget;
	delete patternsimulatorwidget;
	delete formulapredictor;

	delete actionOpenResults;
	delete actionSaveResults;
	delete actionExportToCsv;
	delete actionExportToHTML;
	delete actionPreferences;
	delete actionQuit;
	delete actionProperties;
	delete actionRun;
	delete actionBricksDatabase;
	delete actionSequenceDatabase;
	delete actionModifications;
	delete actionDrawPeptide;
	delete actionBookMark1;
	delete actionBookMark2;
	delete actionBookMark3;
	delete actionBookMark4;
	delete actionBookMark5;
	delete actionBookMark6;
	delete actionBookMark7;
	delete actionBookMark8;
	delete actionBookMark9;
	delete actionBookMark10;
	delete actionShowIsomers;
	delete actionGraph;
	delete actionSummaryTableOfMatchedPeaks;
	delete actionMultipleDatasets;
	delete actionChromatogramWindow;
	delete actionImageWindow;
	delete actionPatternSimulatorWindow;
	delete actionFormulaPredictor;
	delete actionLog;
	delete actionHTMLDocumentation;
	delete actionPDFManual;
	delete actionAbout;

	delete menuFile;
	delete menuSearch;
	delete menuTools;
	delete menuBookmarks;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cMainWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline1->hasFocus() || rowsfilterline2->hasFocus()) {
			filterResults();
		}
		else {
			if (results->selectionModel()->selectedIndexes().count() > 0) {
				rowDoubleClicked(results->selectionModel()->selectedIndexes()[0]);
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
		QModelIndexList selectedindexes = results->selectionModel()->selectedIndexes();
		QStandardItem* item;

		QString selectedtext;
		string itemtext;

		int selectedcount = selectedindexes.count();
		for (int i = 0; i < selectedcount; i++) {
			if (i > 0) {
				if (resultsproxymodel->mapToSource(selectedindexes[i - 1]).row() != resultsproxymodel->mapToSource(selectedindexes[i]).row()) {
					selectedtext += "\n";
				}
				else {
					selectedtext += "\t";
				}
			}

			item = resultsmodel->itemFromIndex(resultsproxymodel->mapToSource(selectedindexes[i]));
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


void cMainWindow::closeEvent(QCloseEvent *event) {
	quitApplication();
	event->ignore();
}


void cMainWindow::preparePeptideSequence(int row, string& peptidesequence, bool reportisomers) {
	int spectrumindex = resultsmodel->item(row, 1)->data(Qt::DisplayRole).toInt() - 1;
	peptidesequence = listoftheoreticalspectra.get(activefileid, spectrumindex).getCandidate().getAcronymPeptideNameWithHTMLReferences();
	peptidesequence = stripHTML(peptidesequence);

	if (!reportisomers) {
		stripIsomers(peptidesequence);
	}
}


void cMainWindow::reportSpectrum(int row, cTheoreticalSpectrum& theoreticalspectrum, bool reportisomers) {
	string peptidesequence;
	cSummaryFormula formula;

	string scantitle;
	scantitle.clear();

	resultsmodel->setItem(row, 0, new QStandardItem());
	resultsmodel->item(row, 0)->setText("");
	
	resultsmodel->setItem(row, 1, new QStandardItem());
	resultsmodel->item(row, 1)->setData(QVariant::fromValue(row + 1), Qt::DisplayRole);

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {

		if (parameters.mode == denovoengine) {
			resultsmodel->setItem(row, 2, new QStandardItem());
			resultsmodel->item(row, 2)->setData(QVariant::fromValue(theoreticalspectrum.getPathId() + 1), Qt::DisplayRole);
		}

		if (parameters.mode == singlecomparison) {
			resultsmodel->setItem(row, 2, new QStandardItem());
			resultsmodel->item(row, 2)->setText(theoreticalspectrum.getExperimentalSpectrum().getTitle().c_str());
		}

		if (parameters.mode == databasesearch) {
			resultsmodel->setItem(row, 2, new QStandardItem());
			resultsmodel->item(row, 2)->setText(theoreticalspectrum.getCandidate().getName().c_str());
		}

		resultsmodel->setItem(row, 2 + searchspecificcolumncount, new QStandardItem());
		preparePeptideSequence(row, peptidesequence, reportisomers);
		resultsmodel->item(row, 2 + searchspecificcolumncount)->setText(peptidesequence.c_str());

		formula = theoreticalspectrum.getCandidate().getSummaryFormula();

		resultsmodel->setItem(row, 3 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 3 + searchspecificcolumncount)->setText(formula.isPartial()?string(formula.getSummary() + " (partial)").c_str():formula.getSummary().c_str());

		resultsmodel->setItem(row, 4 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 4 + searchspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(formula.getMass())), Qt::DisplayRole);

		resultsmodel->setItem(row, 5 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 5 + searchspecificcolumncount)->setData(QVariant::fromValue(getNumberOfBricks(theoreticalspectrum.getCandidate().getComposition())), Qt::DisplayRole);

		switch (parameters.peptidetype) {
			case linear:
			case linearpolyketide:
				resultsmodel->setItem(row, 6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->item(row, 6 + searchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
				resultsmodel->setItem(row, 7 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->item(row, 7 + searchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
				break;
			case branched:
				resultsmodel->setItem(row, 6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->item(row, 6 + searchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
				resultsmodel->setItem(row, 7 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->item(row, 7 + searchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str());
				resultsmodel->setItem(row, 8 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->item(row, 8 + searchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
				break;
			case cyclic:
			case cyclicpolyketide:
				break;
			case branchcyclic:
				resultsmodel->setItem(row, 6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->item(row, 6 + searchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str());
				break;
			case other:
				break;
			default:
				break;
		}

		resultsmodel->setItem(row, 6 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 6 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfMatchedPeaks()), Qt::DisplayRole);

		resultsmodel->setItem(row, 7 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 7 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getRatioOfMatchedPeaks()*100)), Qt::DisplayRole);

		resultsmodel->setItem(row, 8 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 8 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getSumOfRelativeIntensities())), Qt::DisplayRole);

		resultsmodel->setItem(row, 9 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 9 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getWeightedRatioOfMatchedPeaks() * 100)), Qt::DisplayRole);
		
		resultsmodel->setItem(row, 10 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 10 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getCosineSimilarity())), Qt::DisplayRole);

		int index = resultsbasecolumncount + searchspecificcolumncount + resultsspecificcolumncount;
		for (int i = 0; i < (int)parameters.ionsfortheoreticalspectraMS2.size(); i++) {
			for (int j = -1; j < (int)parameters.neutrallossesfortheoreticalspectra.size(); j++) {
				resultsmodel->setItem(row, index, new QStandardItem());
				resultsmodel->item(row, index)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfMatchedPeaks(parameters.ionsfortheoreticalspectraMS2[i], (j == -1) ? -1 : parameters.neutrallossesfortheoreticalspectra[j])), Qt::DisplayRole);

				index++;
			}
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			resultsmodel->setItem(row, resultsmodel->columnCount() - 1, new QStandardItem());
			resultsmodel->item(row, resultsmodel->columnCount() - 1)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfScrambledPeaks()), Qt::DisplayRole);
		}

		if (((parameters.mode == denovoengine) || (parameters.mode == databasesearch)) && theoreticalspectrum.isValid()) {
			resultsmodel->item(row, 0)->setText("*");
			for (int i = 0; i < resultsmodel->columnCount(); i++) {
				resultsmodel->item(row, i)->setBackground(Qt::yellow);
			}
		}

	}


	if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
		int mscol = 2;

		if ((parameters.peaklistfileformats[activefileid] != mis) && (parameters.peaklistfileformats[activefileid] != imzML)) {
			resultsmodel->setItem(row, mscol, new QStandardItem());
			resultsmodel->item(row, mscol)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getExperimentalSpectrum().getRetentionTime())), Qt::DisplayRole);
			mscol++;
		}

		resultsmodel->setItem(row, mscol, new QStandardItem());
		resultsmodel->item(row, mscol)->setText(theoreticalspectrum.getExperimentalSpectrum().getTitle().c_str());
		scantitle = theoreticalspectrum.getExperimentalSpectrum().getTitle();
		mscol++;

		resultsmodel->setItem(row, mscol, new QStandardItem());
		resultsmodel->item(row, mscol)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfMatchedPeaks()), Qt::DisplayRole);
		mscol++;

		resultsmodel->setItem(row, mscol, new QStandardItem());
		resultsmodel->item(row, mscol)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getRatioOfMatchedPeaks()*100)), Qt::DisplayRole);
		mscol++;

		resultsmodel->setItem(row, mscol, new QStandardItem());
		resultsmodel->item(row, mscol)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getSumOfRelativeIntensities())), Qt::DisplayRole);
		mscol++;

		resultsmodel->setItem(row, mscol, new QStandardItem());
		resultsmodel->item(row, mscol)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getWeightedRatioOfMatchedPeaks() * 100)), Qt::DisplayRole);
		mscol++;

		//resultsmodel->setItem(row, mscol, new QStandardItem());
		//resultsmodel->item(row, mscol)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getCosineSimilarity())), Qt::DisplayRole);
		//mscol++;

		if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
			resultsmodel->setItem(row, mscol, new QStandardItem());
			resultsmodel->item(row, mscol)->setData(QVariant::fromValue(theoreticalspectrum.getExperimentalSpectrum().getCoordinateX()), Qt::DisplayRole);
			mscol++;

			resultsmodel->setItem(row, mscol, new QStandardItem());
			resultsmodel->item(row, mscol)->setData(QVariant::fromValue(theoreticalspectrum.getExperimentalSpectrum().getCoordinateY()), Qt::DisplayRole);
		}
	}


	spectradetails[row].initialize(row + 1, scantitle, activefileid, &globalpreferences, &parameters, theoreticalspectrum, this);

	connect(&spectradetails[row], SIGNAL(lastActiveDetail(int)), this, SLOT(lastActiveDetailSlot(int)));
}


void cMainWindow::calculateAvgSpectrum(int minscan, int maxscan) {
	int numberofspectra = maxscan - minscan + 1;
	if (numberofspectra <= 0) {
		return;
	}

	cPeaksList averagespectrum;
	for (int i = minscan - 1; i < maxscan; i++) {
		averagespectrum.attach(listoftheoreticalspectra.get(activefileid, i).getExperimentalSpectrum());
	}

	int asize = averagespectrum.size();

	QProgressDialog progress("Calculating average spectrum...", /*"Cancel"*/0, 0, 3 * asize, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);
	progress.setValue(0);

	bool quadraticapproach = false;

	if (quadraticapproach) {

		averagespectrum.sortbyAbsoluteIntensityDesc();

		// initialize clusters
		vector<cPeaksList> clusters;
		clusters.resize(averagespectrum.size());
		for (int i = 0; i < asize; i++) {
			clusters[i].add(averagespectrum[i]);

			if (i % 100 == 0) {
				progress.setValue(i);
			}
		}

		// merge clusters
		size_t csize = clusters.size();
		for (int i = 0; i < csize; i++) {
			if ((asize + i) % 100 == 0) {
				progress.setValue(asize + i);
			}

			if (clusters[i].size() == 0) {
				continue;
			}
			for (int j = i + 1; j < csize; j++) {
				if (clusters[j].size() == 0) {
					continue;
				}
				if (isInPpmMassErrorTolerance(clusters[i][0].mzratio, clusters[j][0].mzratio, parameters.fragmentmasserrortolerance)) {
					clusters[i].add(clusters[j][0]);
					clusters[j].clear();
				}
			}
		}

		// reduce peaks
		averagespectrum.clear();
		double relsum;
		int isize;
		for (int i = 0; i < csize; i++) {
			if ((2 * asize + i) % 100 == 0) {
				progress.setValue(2 * asize + i);
			}

			isize = clusters[i].size();

			if (isize == 0) {
				continue;
			}

			cPeak peak;
			relsum = 0;

			for (int j = 0; j < isize; j++) {
				relsum += clusters[i][j].absoluteintensity;
			}

			for (int j = 0; j < isize; j++) {
				peak.mzratio += clusters[i][j].mzratio*clusters[i][j].absoluteintensity;
				peak.absoluteintensity += clusters[i][j].absoluteintensity;
			}

			peak.mzratio /= relsum;

			averagespectrum.add(peak);
		}

	}
	else {

		// initialize clusters
		vector<cPeaksList> clusters;
		clusters.reserve(averagespectrum.size());

		averagespectrum.sortbyMass();
		averagespectrum.fillOrderIDs();

		cPeaksList outer = averagespectrum;
		outer.sortbyAbsoluteIntensityDesc();

		cPeaksList tmplist;

		int isize = outer.size();
		int j;

		// merge clusters
		for (int i = 0; i < isize; i++) {		
			if (averagespectrum[outer[i].orderid].orderid == -1) {
				continue;
			}

			tmplist.clear();

			tmplist.add(averagespectrum[outer[i].orderid]);
			averagespectrum[outer[i].orderid].orderid = -1;

			j = outer[i].orderid + 1;
			while (j < isize) {
				if (isInPpmMassErrorTolerance(averagespectrum[outer[i].orderid].mzratio, averagespectrum[j].mzratio, parameters.fragmentmasserrortolerance)) {
					if (averagespectrum[j].orderid != -1) {
						tmplist.add(averagespectrum[j]);
						averagespectrum[j].orderid = -1;
					}
				}
				else {
					break;
				}
				j++;
			}

			j = outer[i].orderid - 1;
			while (j >= 0) {
				if (isInPpmMassErrorTolerance(averagespectrum[outer[i].orderid].mzratio, averagespectrum[j].mzratio, parameters.fragmentmasserrortolerance)) {
					if (averagespectrum[j].orderid != -1) {
						tmplist.add(averagespectrum[j]);
						averagespectrum[j].orderid = -1;
					}
				}
				else {
					break;
				}
				j--;
			}

			clusters.push_back(tmplist);
		}

		progress.setValue(asize);

		// reduce peaks
		averagespectrum.clear();
		double relsum;
		int csize = (int)clusters.size();
		for (int i = 0; i < csize; i++) {
			isize = clusters[i].size();

			if (isize == 0) {
				continue;
			}

			cPeak peak;
			relsum = 0;

			for (int j = 0; j < isize; j++) {
				relsum += clusters[i][j].absoluteintensity;
			}

			for (int j = 0; j < isize; j++) {
				peak.mzratio += clusters[i][j].mzratio*clusters[i][j].absoluteintensity;
				peak.absoluteintensity += clusters[i][j].absoluteintensity;
			}

			peak.mzratio /= relsum;

			averagespectrum.add(peak);
		}

		progress.setValue(2 * asize);

	}

	averagespectrum.sortbyMass();
	averagespectrum.normalizeIntenzity();

	averagespectrum.cropRelativeIntenzity(parameters.minimumrelativeintensitythreshold);

	QRect geometry;
	bool usegeometry = false;
	bool ismaximized = false;

	if (avgspectra.size() > 0) {
		geometry = avgspectra[0].normalGeometry();
		usegeometry = true;
		ismaximized = avgspectra[0].isMaximized();
	}
	else {
		if (avgspectrumused >= 0) {
			geometry = avgspectrumgeometry;
			usegeometry = true;
			ismaximized = avgspectrummaximized;
		}
	}

	avgspectra.clear();
	avgspectra.resize(1);

	cTheoreticalSpectrum tsfull;
	tsfull.setParameters(&parameters);

	bool terminatecomputation = false;
	if (parameters.generateisotopepattern) {
		if (parameters.calculatefdrs) {
			tsfull.generateFineMSSpectrum(0, parameters.sequencedatabase.size() / 2, terminatecomputation);
		}
		else {
			tsfull.generateFineMSSpectrum(0, parameters.sequencedatabase.size(), terminatecomputation);
		}
	}
	else {
		tsfull.generateMSSpectrum(0, parameters.sequencedatabase.size(), terminatecomputation, true);
	}

	cTheoreticalSpectrum tsaverage;
	cPeaksList unmatchedpeaksinmatchedpatterns;
	tsaverage.setParameters(&parameters);
	tsaverage.compareAverageMSSpectrum(averagespectrum, tsfull, unmatchedpeaksinmatchedpatterns);
	tsaverage.finalizeMSSpectrum(unmatchedpeaksinmatchedpatterns, true);

	string scantitle;
	avgspectra[0].initialize(1, scantitle, activefileid, &globalpreferences, &parameters, tsaverage, this);
	avgspectra[0].prepareToShow(actionShowIsomers, &rawdata, &imzmlprofilemetadata, profilemz64precision, profileintensity64precision, true);

	string title = "Average Spectrum " + to_string(minscan) + " - " + to_string(maxscan);
	avgspectra[0].setWindowTitle(title.c_str());
	avgspectra[0].setAbsoluteIntensityEnabled(avgspectrumabsoluteintensityenabled);
	avgspectra[0].disableProfileMode();

	progress.setValue(3 * asize);

	chromatogramwindow->activateWindow();

	if (usegeometry) {
		avgspectra[0].setGeometry(geometry);
	}

	if (ismaximized) {
		avgspectra[0].showMaximized();
	}
	else {
		avgspectra[0].showNormal();
	}

	connect(&avgspectra[0], SIGNAL(absoluteIntensityStateChangedSignal(bool)), this, SLOT(avgSpectrumAbsoluteIntensityStateChanged(bool)));

	avgspectrumused = 0;
	avgspectrummaximized = avgspectra[0].isMaximized();
	avgspectrumgeometry = avgspectra[0].normalGeometry();
}


void cMainWindow::showHideLog() {
	bool state = logWindow->isVisible();

	if (state) {
		logWindow->hide();
	}
	else {
		logWindow->show();
	}
}


void cMainWindow::showAbout() {
	about->show();
	about->activateWindow();
	if (about->isMinimized()) {
		about->showNormal();
	}
}


void cMainWindow::showBricksDatabase() {
	bricksdatabasewidget->show();
	bricksdatabasewidget->activateWindow();
	if (bricksdatabasewidget->isMinimized()) {
		bricksdatabasewidget->showNormal();
	}
}


void cMainWindow::showSequenceDatabase() {
	sequencedatabasewidget->show();
	sequencedatabasewidget->activateWindow();
	if (sequencedatabasewidget->isMinimized()) {
		sequencedatabasewidget->showNormal();
	}
}


void cMainWindow::showModifications() {
	modificationswidget->show();
	modificationswidget->activateWindow();
	if (modificationswidget->isMinimized()) {
		modificationswidget->showNormal();
	}
}


void cMainWindow::showDrawPeptideWidget() {
	drawpeptidewidget->show();
	drawpeptidewidget->activateWindow();
	if (drawpeptidewidget->isMinimized()) {
		drawpeptidewidget->showNormal();
	}
}


void cMainWindow::setAndShowDrawPeptideWidget(int peptidetypeindex, QString sequence) {
	drawpeptidewidget->setSequence(peptidetypeindex, sequence);
	showDrawPeptideWidget();
}


bool cMainWindow::prepareSummaryTableOfMatchedPeaks() {
	bool imagewindowstate = actionImageWindow->isEnabled();
	actionImageWindow->setEnabled(false);
	actionSummaryTableOfMatchedPeaks->setEnabled(false);

	if (!summarytableisprepared) {
		rowsfilterwidget->setEnabled(false);
		summarytableisprepared = summarytableofmatchedpeaks->prepareToShow(resultsmodel, resultsproxymodel, &parameters, &listoftheoreticalspectra, activefileid, actionShowIsomers->isChecked());
		rowsfilterwidget->setEnabled(true);
	}

	actionImageWindow->setEnabled(imagewindowstate);
	actionSummaryTableOfMatchedPeaks->setEnabled(true);

	return summarytableisprepared;
}


void cMainWindow::showSummaryTableOfMatchedPeaks() {
	if (prepareSummaryTableOfMatchedPeaks()) {
		summarytableofmatchedpeaks->show();
		summarytableofmatchedpeaks->activateWindow();
		if (summarytableofmatchedpeaks->isMinimized()) {
			summarytableofmatchedpeaks->showNormal();
		}
	}
}


void cMainWindow::showImageWindow() {
	if (prepareSummaryTableOfMatchedPeaks()) {
		imagewindow->setDefaultValues(parameters.peaklistfilenames[activefileid], parameters.defaultmaxx, parameters.defaultmaxy, parameters.defaultpixelsizex, parameters.defaultpixelsizey, parameters.vendor);
		imagewindow->show();
		imagewindow->activateWindow();
		if (imagewindow->isMinimized()) {
			imagewindow->showNormal();
		}
	}
}


void cMainWindow::showChromatogramWindow() {
	if (prepareSummaryTableOfMatchedPeaks()) {
		chromatogramwindow->recalculateTICChromatogram(activefileid);
		chromatogramwindow->show();
		chromatogramwindow->activateWindow();
		if (chromatogramwindow->isMinimized()) {
			chromatogramwindow->showNormal();
		}
	}
}


void cMainWindow::showGraph() {
	graph->show();
	graph->activateWindow();
	if (graph->isMinimized()) {
		graph->showNormal();
	}
}


void cMainWindow::showProperties() {
	parameterswidget->show();
	parameterswidget->activateWindow();
	if (parameterswidget->isMinimized()) {
		parameterswidget->showNormal();
	}
}


void cMainWindow::showMultipleDatasetsOverview() {
	if (!multipledatasetsisprepared) {
		multipledatasetswidget->resetStandard();
		multipledatasetsisprepared = multipledatasetswidget->prepareToShow(&parameters, &listoftheoreticalspectra, activefileid);
		multipledatasetswidget->updateListofStandards();
	}
	if (multipledatasetsisprepared) {
		multipledatasetswidget->show();
		multipledatasetswidget->activateWindow();
		if (multipledatasetswidget->isMinimized()) {
			multipledatasetswidget->showNormal();
		}
	}
}


void cMainWindow::showPatternSimulatorWindow() {
	patternsimulatorwidget->show();
	patternsimulatorwidget->activateWindow();
	if (patternsimulatorwidget->isMinimized()) {
		patternsimulatorwidget->showNormal();
	}
}


void cMainWindow::showFormulaPredictor() {
	formulapredictor->show();
	formulapredictor->activateWindow();
	if (formulapredictor->isMinimized()) {
		formulapredictor->showNormal();
	}
}


void cMainWindow::run() {
	enableRunButtonAndSettings(false);
	enableStopButton(true);
	enableButtonsHandlingResults(false);

	logWindow->clear();
	
	resetFilter();

	deleteResults();
	
	listoftheoreticalspectra.clear();
	eicchromatograms.clear();
	spectradetails.clear();

	if (avgspectra.size() > 0) {
		avgspectrumused = 0;
		avgspectrummaximized = avgspectra[0].isMaximized();
		avgspectrumgeometry = avgspectra[0].normalGeometry();
	}

	avgspectra.clear();

	parameters.clear();
	
	rawdata.clear();
	imzmlprofilemetadata.clear();
	profilemz64precision = false;
	profileintensity64precision = false;

	cParameters localparameters = parameterswidget->getParameters();

	regex rx;
	rx = "\\.[iI][mM][zZ][mM][lL]$";
	localparameters.profiledatafilename = "";
	if (regex_search(localparameters.peaklistfilenames[0], rx)) {
		localparameters.profiledatafilename = localparameters.peaklistfilenames[0];

		string convertedimzml = localparameters.peaklistfilenames[0].substr(0, (int)localparameters.peaklistfilenames[0].size() - 6);
		string convertedibd = convertedimzml;
		convertedimzml += "_converted_fwhm_";
		convertedimzml += to_string(localparameters.fwhm);
		convertedimzml += ".imzML";
		convertedibd += "_converted_fwhm_";
		convertedibd += to_string(localparameters.fwhm);
		convertedibd += ".ibd";

		if (QFile::exists(convertedimzml.c_str()) && QFile::exists(convertedibd.c_str())) {
			QMessageBox::StandardButton reply;
			string s = "The converted imzML file '" + convertedimzml.substr(convertedimzml.rfind('/') + 1) + "' has been found.\n\nDo you want to proceed with this file instead of the original imzML file '";
			s += localparameters.peaklistfilenames[0].substr(localparameters.peaklistfilenames[0].rfind('/') + 1) + "' ?";
			s += "\n\nClick 'Yes' to proceed the converted file.\nClick 'No' to convert the original file again. Note: The conversion may be time-consuming.";

			reply = QMessageBox::question(this, "Do you want to proceed the converted file ?", s.c_str(), QMessageBox::Yes|QMessageBox::No);

			if (reply == QMessageBox::Yes) {
				localparameters.peaklistfilenames[0] = convertedimzml;
			}
		}
	}

	/*
	rx = "\\.[mM][zZ][mM][lL]$";
	if (regex_search(localparameters.peaklistfilename, rx)) {
		string convertedmzml = localparameters.peaklistfilename.substr(0, (int)localparameters.peaklistfilename.size() - 5);
		convertedmzml += "_converted_fwhm_";
		convertedmzml += to_string(localparameters.fwhm);
		convertedmzml += ".mzML";

		if (QFile::exists(convertedmzml.c_str())) {
			QMessageBox::StandardButton reply;
			string s = "The converted mzML file '" + convertedmzml.substr(convertedmzml.rfind('/') + 1) + "' has been found.\n\nDo you want to proceed with this file instead of the original mzML file '";
			s += localparameters.peaklistfilename.substr(localparameters.peaklistfilename.rfind('/') + 1) + "' ?";
			s += "\n\nClick 'Yes' to proceed the converted file.\nClick 'No' to convert the original file again. Note: The conversion may be time-consuming.";

			reply = QMessageBox::question(this, "Do you want to proceed the converted file ?", s.c_str(), QMessageBox::Yes|QMessageBox::No);

			if (reply == QMessageBox::Yes) {
				localparameters.peaklistfilename = convertedmzml;
			}
		}
	}
	*/

	#if OS_TYPE == WIN
		if (localparameters.useprofiledata) {

			rx = "\\.[bB][aA][fF]$";
			if (regex_search(localparameters.peaklistfilenames[0], rx)) {

				if (globalpreferences.profilebafprocessingmethod == 0) {
					if (localparameters.peaklistfilenames.size() == 1) {
						int i = 1;
						string profiledata = localparameters.peaklistfilenames[0] + ".profile.1.csv";
						while (QFile::exists(profiledata.c_str())) {
							i++;
							profiledata = localparameters.peaklistfilenames[0] + ".profile." + to_string(i) + ".csv";
						}
						i--;

						localparameters.convertprofiledata = true;
						if (i > 0) {
							QMessageBox::StandardButton reply;
							string s = "The file '" + localparameters.peaklistfilenames[0].substr(localparameters.peaklistfilenames[0].rfind('/') + 1) + ".profile.1.csv'";
							if (i > 1) {
								s += " and " + to_string(i - 1) + " other files containing profile spectra have been found.";
							}
							else {
								s += " containing a profile spectrum has been found.";
							}

							s += "\n\nDo you want to proceed the existing file(s) ?";
							s += "\n\nClick 'Yes' to proceed the existing file(s).\nClick 'No' to convert the profile data again. Note: The conversion of many profile spectra may be time-consuming.";

							reply = QMessageBox::question(this, "Do you want to proceed the existing file(s) ?", s.c_str(), QMessageBox::Yes | QMessageBox::No);

							if (reply == QMessageBox::Yes) {
								localparameters.convertprofiledata = false;
							}
						}
					}
					else {
						localparameters.convertprofiledata = false;

						QMessageBox msgBox;
						QString errstr = "You tried to convert the profile spectra from multiple *.baf files using CompassXport. ";
						errstr += "This feature may be time-consuming and it will be disabled. ";
						errstr += "If you would like to see the profile spectra, go to 'Preferences->Data Processing' and select CompassXtract as a profile data converter or use only one input *.baf file.";
						msgBox.setText(errstr);
						msgBox.exec();
					}
				}

				if (globalpreferences.profilebafprocessingmethod == 1) {
					localparameters.convertprofiledata = false;
				}

			}

		}

		localparameters.linebafprocessing = globalpreferences.linebafprocessingmethod;
	#endif

	cMainThread* thread = new cMainThread(localparameters, listoftheoreticalspectra, true, false);
	connect(thread, SIGNAL(message(QString)), this, SLOT(updateLog(QString)));
	connect(thread, SIGNAL(enableRunButtonAndSettings(bool)), this, SLOT(enableRunButtonAndSettings(bool)));
	connect(thread, SIGNAL(enableStopButton(bool)), this, SLOT(enableStopButton(bool)));
	connect(thread, SIGNAL(enableButtonsHandlingResults(bool)), this, SLOT(enableButtonsHandlingResults(bool)));
	connect(thread, SIGNAL(sendParameters(cParameters)), this, SLOT(sendParameters(cParameters)));
	connect(thread, SIGNAL(reportSpectra()), this, SLOT(reportSpectra()));
	connect(thread, SIGNAL(setGraph(string)), this, SLOT(setGraph(string)));
	connect(this, SIGNAL(stopComputation()), thread, SLOT(stopComputation()));

	summarytableofmatchedpeaks->deleteTable();
	summarytableofmatchedpeaks->hide();
	summarytableofmatchedpeaks->getPubChemSearchWidget()->setHTML("");
	summarytableisprepared = false;

	lastactivedetail = -1;

	othernormalgeometry = QRect();
	otherismaximized = false;
	otherprofilespectrum = false;
	otherabsoluteintensity = false;
	othermatchedpeaks = false;
	otherunmatchedpeaks = false;
	otherscrambledpeaks = false;
	otherlabelspeaks = false;
	othermzselection = true;

	activefileid = 0;

	imagewindow->hide();
	chromatogramwindow->hide();

	multipledatasetswidget->deleteTable();
	multipledatasetswidget->hide();
	multipledatasetswidget->getPubChemSearchWidget()->hide();
	multipledatasetswidget->getPubChemSearchWidget()->setHTML("");
	multipledatasetswidget->getDefineCalibrationWidget()->hide();
	multipledatasetswidget->getCalibrationChartWidget()->hide();
	multipledatasetsisprepared = false;

	thread->start();
}


void cMainWindow::stop() {
	emit stopComputation(); 
}


void cMainWindow::updateLog(QString qstr) {
	QTextCursor cursor(logWindow->textCursor());
    cursor.movePosition(QTextCursor::End);
	logWindow->setTextCursor(cursor);
	logWindow->insertPlainText(qstr);
	QScrollBar *sb = logWindow->verticalScrollBar();
	sb->setValue(sb->maximum());
}


void cMainWindow::enableRunButtonAndSettings(bool enable) {

	if (enable && quitapp) {
		qApp->quit();
	}

	actionRun->setEnabled(enable);

	actionProperties->setEnabled(enable);
	parameterswidget->close();
}


void cMainWindow::enableStopButton(bool enable) {
	actionStop->setEnabled(enable);
}


void cMainWindow::enableButtonsHandlingResults(bool enable) {
	actionOpenResults->setEnabled(enable);
	actionSaveResults->setEnabled(enable);
	actionExportToCsv->setEnabled(enable);
	actionExportToHTML->setEnabled(enable);
	actionPreferences->setEnabled(enable);
	rowsfilterwidget->setEnabled(enable);
	actionSummaryTableOfMatchedPeaks->setEnabled(enable);
	actionMultipleDatasets->setEnabled(enable);
	
	if (enable) {
		if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
			actionShowIsomers->setEnabled(false);
			if ((parameters.peaklistfileformats[0] == mis) || (parameters.peaklistfileformats[0] == imzML)) {
				actionImageWindow->setEnabled(true);
				actionChromatogramWindow->setEnabled(false);
			}
			else {
				actionImageWindow->setEnabled(false);
				actionChromatogramWindow->setEnabled(true);
			}
		}
		else {
			actionShowIsomers->setEnabled(true);
			actionImageWindow->setEnabled(false);
			actionChromatogramWindow->setEnabled(false);
		}
	}
	else {
		actionShowIsomers->setEnabled(false);
		actionImageWindow->setEnabled(false);
		actionChromatogramWindow->setEnabled(false);
	}
}


void cMainWindow::sendParameters(cParameters parameters) {
	this->parameters = parameters;
}


void cMainWindow::reportSpectra() {

	if (quitapp) {
		return;
	}

	switch (parameters.peptidetype) {
		case linear:
		case linearpolyketide:
			resultsspecificcolumncount = 2;
			break;
		case branched:
			resultsspecificcolumncount = 3;
			break;
		case cyclic:
		case cyclicpolyketide:
			resultsspecificcolumncount = 0;
			break;
		case branchcyclic:
			resultsspecificcolumncount = 1;
			break;
		case other:
			resultsspecificcolumncount = 0;
			break;
		default:
			break;
	}


	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
		searchspecificcolumncount = 1;
	}
	else {
		searchspecificcolumncount = 0;
	}


	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) { 

		resultsmodel->setColumnCount(resultsbasecolumncount + searchspecificcolumncount + resultsspecificcolumncount + ((int)parameters.ionsfortheoreticalspectraMS2.size() * ((int)parameters.neutrallossesfortheoreticalspectra.size() + 1)));

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			resultsmodel->setColumnCount(resultsmodel->columnCount() + 1);
		}

		resultsmodel->setHorizontalHeaderItem(0, new QStandardItem());
		resultsmodel->horizontalHeaderItem(0)->setText("*");
		results->setItemDelegateForColumn(0, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(1, new QStandardItem());
		if (parameters.mode == singlecomparison) {
			resultsmodel->horizontalHeaderItem(1)->setText("Scan ID");
		}
		else {
			resultsmodel->horizontalHeaderItem(1)->setText("Result ID");
		}
		results->setItemDelegateForColumn(1, new QItemDelegate());

		if (parameters.mode == denovoengine) {
			resultsmodel->setHorizontalHeaderItem(2, new QStandardItem());
			resultsmodel->horizontalHeaderItem(2)->setText("Group ID");
			results->setItemDelegateForColumn(2, new QItemDelegate());
		}

		if (parameters.mode == singlecomparison) {
			resultsmodel->setHorizontalHeaderItem(2, new QStandardItem());
			resultsmodel->horizontalHeaderItem(2)->setText("Title");
			results->setItemDelegateForColumn(2, new QItemDelegate());
		}
		
		if (parameters.mode == databasesearch) {
			resultsmodel->setHorizontalHeaderItem(2, new QStandardItem());
			resultsmodel->horizontalHeaderItem(2)->setText("Name");
			results->setItemDelegateForColumn(2, new QItemDelegate());
		}
		
		resultsmodel->setHorizontalHeaderItem(2 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(2 + searchspecificcolumncount)->setText("Peptide Sequence");
		results->setItemDelegateForColumn(2 + searchspecificcolumncount, new QItemDelegate());
		
		resultsmodel->setHorizontalHeaderItem(3 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(3 + searchspecificcolumncount)->setText("Summary Formula");
		results->setItemDelegateForColumn(3 + searchspecificcolumncount, new QItemDelegate());
		
		resultsmodel->setHorizontalHeaderItem(4 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(4 + searchspecificcolumncount)->setText("Monoisotopic Mass");
		results->setItemDelegateForColumn(4 + searchspecificcolumncount, new QItemDelegate());
		
		resultsmodel->setHorizontalHeaderItem(5 + searchspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(5 + searchspecificcolumncount)->setText("Number of Bricks");
		results->setItemDelegateForColumn(5 + searchspecificcolumncount, new QItemDelegate());

		switch (parameters.peptidetype) {
			case linear:
				resultsmodel->setHorizontalHeaderItem(6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(6 + searchspecificcolumncount)->setText("N-terminal Modification");
				results->setItemDelegateForColumn(6 + searchspecificcolumncount, new QItemDelegate());

				resultsmodel->setHorizontalHeaderItem(7 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(7 + searchspecificcolumncount)->setText("C-terminal Modification");
				results->setItemDelegateForColumn(7 + searchspecificcolumncount, new QItemDelegate());
				break;
			case branched:
				resultsmodel->setHorizontalHeaderItem(6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(6 + searchspecificcolumncount)->setText("N-terminal Modification");
				results->setItemDelegateForColumn(6 + searchspecificcolumncount, new QItemDelegate());

				resultsmodel->setHorizontalHeaderItem(7 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(7 + searchspecificcolumncount)->setText("Branch Modification");
				results->setItemDelegateForColumn(7 + searchspecificcolumncount, new QItemDelegate());

				resultsmodel->setHorizontalHeaderItem(8 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(8 + searchspecificcolumncount)->setText("C-terminal Modification");
				results->setItemDelegateForColumn(8 + searchspecificcolumncount, new QItemDelegate());
				break;
			case cyclic:
			case cyclicpolyketide:
				break;
			case branchcyclic:
				resultsmodel->setHorizontalHeaderItem(6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(6 + searchspecificcolumncount)->setText("Branch Modification");
				results->setItemDelegateForColumn(6 + searchspecificcolumncount, new QItemDelegate());
				break;
			case linearpolyketide:
				resultsmodel->setHorizontalHeaderItem(6 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(6 + searchspecificcolumncount)->setText("Left Terminal Modification");
				results->setItemDelegateForColumn(6 + searchspecificcolumncount, new QItemDelegate());

				resultsmodel->setHorizontalHeaderItem(7 + searchspecificcolumncount, new QStandardItem());
				resultsmodel->horizontalHeaderItem(7 + searchspecificcolumncount)->setText("Right Terminal Modification");
				results->setItemDelegateForColumn(7 + searchspecificcolumncount, new QItemDelegate());
				break;
			case other:
				break;
			default:
				break;
		}
		
		resultsmodel->setHorizontalHeaderItem(6 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(6 + searchspecificcolumncount + resultsspecificcolumncount)->setText("Matched Peaks");
		results->setItemDelegateForColumn(6 + searchspecificcolumncount + resultsspecificcolumncount, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(7 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(7 + searchspecificcolumncount + resultsspecificcolumncount)->setText("Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(7 + searchspecificcolumncount + resultsspecificcolumncount, new QItemDelegate());
				
		resultsmodel->setHorizontalHeaderItem(8 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(8 + searchspecificcolumncount + resultsspecificcolumncount)->setText("Sum of Relative Intensities");
		results->setItemDelegateForColumn(8 + searchspecificcolumncount + resultsspecificcolumncount, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(9 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(9 + searchspecificcolumncount + resultsspecificcolumncount)->setText("Weighted Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(9 + searchspecificcolumncount + resultsspecificcolumncount, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(10 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->horizontalHeaderItem(10 + searchspecificcolumncount + resultsspecificcolumncount)->setText("Cosine Similarity");
		results->setItemDelegateForColumn(10 + searchspecificcolumncount + resultsspecificcolumncount, new QItemDelegate());

		string name;
		int index = resultsbasecolumncount + searchspecificcolumncount + resultsspecificcolumncount;
		for (int i = 0; i < (int)parameters.ionsfortheoreticalspectraMS2.size(); i++) {
			for (int j = -1; j < (int)parameters.neutrallossesfortheoreticalspectra.size(); j++) {
				resultsmodel->setHorizontalHeaderItem(index, new QStandardItem());
				name = parameters.iondefinitions[(eFragmentIonType)parameters.ionsfortheoreticalspectraMS2[i]].name;
				if (j >= 0) {
					name += "-" + parameters.neutrallossesdefinitions[parameters.neutrallossesfortheoreticalspectra[j]].summary;
				}
				resultsmodel->horizontalHeaderItem(index)->setText(name.c_str());
				results->setItemDelegateForColumn(index, new QItemDelegate());

				index++;
			}
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			resultsmodel->setHorizontalHeaderItem(resultsmodel->columnCount() - 1, new QStandardItem());
			resultsmodel->horizontalHeaderItem(resultsmodel->columnCount() - 1)->setText("Scrambled Peaks");
			results->setItemDelegateForColumn(resultsmodel->columnCount() - 1, new QItemDelegate());
		}

	}


	if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
		if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
			resultsmodel->setColumnCount(/*10*/9);
		}
		else {
			resultsmodel->setColumnCount(/*9*/8);
		}

		int mscol = 0;

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("*");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("Spectrum ID");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		if ((parameters.peaklistfileformats[activefileid] != mis) && (parameters.peaklistfileformats[activefileid] != imzML)) {
			resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
			resultsmodel->horizontalHeaderItem(mscol)->setText("Time");
			results->setItemDelegateForColumn(mscol, new QItemDelegate());
			mscol++;
		}

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("Title");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("Matched Peaks");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("Sum of Relative Intensities");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		resultsmodel->horizontalHeaderItem(mscol)->setText("Weighted Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(mscol, new QItemDelegate());
		mscol++;

		//resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
		//resultsmodel->horizontalHeaderItem(mscol)->setText("Cosine Similarity");
		//results->setItemDelegateForColumn(mscol, new QItemDelegate());
		//mscol++;

		if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
			resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
			resultsmodel->horizontalHeaderItem(mscol)->setText("Coordinate X");
			results->setItemDelegateForColumn(mscol, new QItemDelegate());
			mscol++;

			resultsmodel->setHorizontalHeaderItem(mscol, new QStandardItem());
			resultsmodel->horizontalHeaderItem(mscol)->setText("Coordinate Y");
			results->setItemDelegateForColumn(mscol, new QItemDelegate());
		}
	}

	rowsfiltercombobox1->clear();
	rowsfiltercombobox2->clear();
	for (int i = 0; i < resultsmodel->columnCount(); i++) {
		rowsfiltercombobox1->addItem(resultsmodel->horizontalHeaderItem(i)->text());
		rowsfiltercombobox2->addItem(resultsmodel->horizontalHeaderItem(i)->text());
	}

	results->resizeColumnsToContents();

	if (listoftheoreticalspectra.size() == 0) {
		listoftheoreticalspectra.resize(1);
		listoftheoreticalspectra.clear(0);
		activefileid = 0;
	}

	if (parameters.useprofiledata) {

		if (parameters.peaklistfileformats[activefileid] == baf) {
			rawdata.resize(listoftheoreticalspectra.size(activefileid));
		}

		if (parameters.peaklistfileformats[activefileid] == dat) {
			rawdata.resize(listoftheoreticalspectra.size(activefileid));
		}

		if ((parameters.peaklistfileformats[activefileid] == mzML) || (parameters.peaklistfileformats[activefileid] == raw)) {
			rawdata.resize(listoftheoreticalspectra.size(activefileid));
		}

		if ((parameters.peaklistfileformats[activefileid] == imzML) && ((parameters.mode == dereplication) || (parameters.mode == compoundsearch))) {
			rawdata.resize(listoftheoreticalspectra.size(activefileid));
			imzmlprofilemetadata.clear();
			profilemz64precision = false;
			profileintensity64precision = false;
			
			cImzML imzml;
			int defaultmaxx;
			int defaultmaxy;
			int defaultpixelsizex;
			int defaultpixelsizey;
			eVendorType vendor;

			if (imzml.parse(parameters.profiledatafilename, defaultmaxx, defaultmaxy, defaultpixelsizex, defaultpixelsizey, vendor) == 0) {
				if (imzml.hasProfileSpectra()) {
					imzmlprofilemetadata = imzml.getItems();
					profilemz64precision = imzml.use64BitMzPrecision();
					profileintensity64precision = imzml.use64BitIntensityPrecision();
				}
			}
		}

	}

	QProgressDialog progress("Preparing the report...", /*"Cancel"*/0, 0, listoftheoreticalspectra.size(activefileid), this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);
	progress.setValue(0);

	results->setModel(0);
	resultsproxymodel->setSourceModel(0);
	results->setSortingEnabled(false);
	
	spectradetails.clear();
	spectradetails.resize(listoftheoreticalspectra.size(activefileid));
	resultsmodel->setRowCount(listoftheoreticalspectra.size(activefileid));
	bool reportisomers = actionShowIsomers->isChecked();
	for (int i = 0; i < listoftheoreticalspectra.size(activefileid); i++) {
		reportSpectrum(i, listoftheoreticalspectra.get(activefileid, i), reportisomers);
		progress.setValue(i);
		//if (progress.wasCanceled()) {
		//	break;
		//}
	}

	if (avgspectra.size() > 0) {
		avgspectrumused = 0;
		avgspectrummaximized = avgspectra[0].isMaximized();
		avgspectrumgeometry = avgspectra[0].normalGeometry();
	}

	avgspectra.clear();

	resultsproxymodel->setSourceModel(resultsmodel);
	results->setModel(resultsproxymodel);
	results->setSortingEnabled(true);
	resultsproxymodel->sort(-1);

	for (int i = 0; i < resultsmodel->columnCount(); i++) {
		results->resizeColumnToContents(i);
	}

	if (parameters.peaklistseriesvector.size() > 1) {
		toolbarInputFilter->setVisible(true);
	}
	else {
		toolbarInputFilter->setVisible(false);
	}

	inputfiltercombobox->clear();
	for (int i = 0; i < parameters.peaklistseriesvector.size(); i++) {
		inputfiltercombobox->addItem(parameters.peaklistfilenames[i].c_str());
	}
	inputfiltercombobox->setCurrentIndex(activefileid);

	//eicchromatograms.clear();
	//if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
	//	if ((parameters.peaklistfileformats[activefileid] != mis) && (parameters.peaklistfileformats[activefileid] != imzML)) {
	//		int sizei = listoftheoreticalspectra.size();
	//		int sizej;
	//		eicchromatograms.resize(sizei);
	//		for (int i = 0; i < sizei; i++) {
	//			eicchromatograms[i].clear();
	//			sizej = listoftheoreticalspectra.size(i);
	//			for (int j = 0; j < sizej; j++) {
	//				addEICPeak(&parameters, eicchromatograms[i], listoftheoreticalspectra.get(i, j), listoftheoreticalspectra.get(i, j).getExperimentalSpectrum());
	//			}
	//			eicchromatograms[i].normalizeIntenzity();
	//		}
	//	}	
	//}

	progress.setValue(listoftheoreticalspectra.size(activefileid));
}


void cMainWindow::showIsomersStateChanged() {

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {

		if (!summarytableofmatchedpeaks->isHidden()) {
			summarytableofmatchedpeaks->deleteTable();
			summarytableisprepared = false;

			prepareSummaryTableOfMatchedPeaks();
		}
		else {
			summarytableofmatchedpeaks->deleteTable();
			summarytableisprepared = false;
		}

		bool reportisomers = actionShowIsomers->isChecked();
		string peptidesequence;

		QProgressDialog progress("Updating the report...", 0, 0, listoftheoreticalspectra.size(activefileid), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);
		progress.setValue(0);

		for (int i = 0; i < resultsmodel->rowCount(); i++) {
			preparePeptideSequence(i, peptidesequence, reportisomers);
			resultsmodel->item(i, 2 + searchspecificcolumncount)->setText(peptidesequence.c_str());

			progress.setValue(i);
		}

		for (int i = 0; i < resultsmodel->columnCount(); i++) {
			results->resizeColumnToContents(i);
		}

		graph->updateView(reportisomers);

		progress.setValue(listoftheoreticalspectra.size(activefileid));

	}

}


void cMainWindow::deleteResults() {
	for (int i = 0; i < resultsmodel->columnCount(); i++) {
		if (results->itemDelegateForColumn(i)) {
			delete results->itemDelegateForColumn(i);
		}
	}
	resultsmodel->clear();
	resultsmodel->setColumnCount(0);
	resultsmodel->setRowCount(0);
}


void cMainWindow::applyGlobalPreferences() {
	if (lastdirexporttocsv.right(4).compare(".csv", Qt::CaseInsensitive) != 0) {
		lastdirexporttocsv = globalpreferences.exportcsvdefaultdir;
	}

	if ((lastdirexporttohtml.right(4).compare(".htm", Qt::CaseInsensitive) != 0) && (lastdirexporttohtml.right(5).compare(".html", Qt::CaseInsensitive) != 0)) {
		lastdirexporttohtml = globalpreferences.exporthtmldefaultdir;
	}

	if (lastdiropenresults.right(4).compare(".res", Qt::CaseInsensitive) != 0) {
		lastdiropenresults = globalpreferences.resultsdefaultdir;
	}

	if (lastdirsaveresults.right(4).compare(".res", Qt::CaseInsensitive) != 0) {
		lastdirsaveresults = globalpreferences.resultsdefaultdir;
	}

	actionBookMark1->setText(globalpreferences.bookmarkname1);
	if (globalpreferences.bookmarkname1.isEmpty() || globalpreferences.bookmarkurl1.isEmpty()) {
		actionBookMark1->setEnabled(false);
		actionBookMark1->setVisible(false);
	}
	else {
		actionBookMark1->setEnabled(true);
		actionBookMark1->setVisible(true);
	}

	actionBookMark2->setText(globalpreferences.bookmarkname2);
	if (globalpreferences.bookmarkname2.isEmpty() || globalpreferences.bookmarkurl2.isEmpty()) {
		actionBookMark2->setEnabled(false);
		actionBookMark2->setVisible(false);
	}
	else {
		actionBookMark2->setEnabled(true);
		actionBookMark2->setVisible(true);
	}

	actionBookMark3->setText(globalpreferences.bookmarkname3);
	if (globalpreferences.bookmarkname3.isEmpty() || globalpreferences.bookmarkurl3.isEmpty()) {
		actionBookMark3->setEnabled(false);
		actionBookMark3->setVisible(false);
	}
	else {
		actionBookMark3->setEnabled(true);
		actionBookMark3->setVisible(true);
	}

	actionBookMark4->setText(globalpreferences.bookmarkname4);
	if (globalpreferences.bookmarkname4.isEmpty() || globalpreferences.bookmarkurl4.isEmpty()) {
		actionBookMark4->setEnabled(false);
		actionBookMark4->setVisible(false);
	}
	else {
		actionBookMark4->setEnabled(true);
		actionBookMark4->setVisible(true);
	}

	actionBookMark5->setText(globalpreferences.bookmarkname5);
	if (globalpreferences.bookmarkname5.isEmpty() || globalpreferences.bookmarkurl5.isEmpty()) {
		actionBookMark5->setEnabled(false);
		actionBookMark5->setVisible(false);
	}
	else {
		actionBookMark5->setEnabled(true);
		actionBookMark5->setVisible(true);
	}

	actionBookMark6->setText(globalpreferences.bookmarkname6);
	if (globalpreferences.bookmarkname6.isEmpty() || globalpreferences.bookmarkurl6.isEmpty()) {
		actionBookMark6->setEnabled(false);
		actionBookMark6->setVisible(false);
	}
	else {
		actionBookMark6->setEnabled(true);
		actionBookMark6->setVisible(true);
	}

	actionBookMark7->setText(globalpreferences.bookmarkname7);
	if (globalpreferences.bookmarkname7.isEmpty() || globalpreferences.bookmarkurl7.isEmpty()) {
		actionBookMark7->setEnabled(false);
		actionBookMark7->setVisible(false);
	}
	else {
		actionBookMark7->setEnabled(true);
		actionBookMark7->setVisible(true);
	}

	actionBookMark8->setText(globalpreferences.bookmarkname8);
	if (globalpreferences.bookmarkname8.isEmpty() || globalpreferences.bookmarkurl8.isEmpty()) {
		actionBookMark8->setEnabled(false);
		actionBookMark8->setVisible(false);
	}
	else {
		actionBookMark8->setEnabled(true);
		actionBookMark8->setVisible(true);
	}

	actionBookMark9->setText(globalpreferences.bookmarkname9);
	if (globalpreferences.bookmarkname9.isEmpty() || globalpreferences.bookmarkurl9.isEmpty()) {
		actionBookMark9->setEnabled(false);
		actionBookMark9->setVisible(false);
	}
	else {
		actionBookMark9->setEnabled(true);
		actionBookMark9->setVisible(true);
	}

	actionBookMark10->setText(globalpreferences.bookmarkname10);
	if (globalpreferences.bookmarkname10.isEmpty() || globalpreferences.bookmarkurl10.isEmpty()) {
		actionBookMark10->setEnabled(false);
		actionBookMark10->setVisible(false);
	}
	else {
		actionBookMark10->setEnabled(true);
		actionBookMark10->setVisible(true);
	}

	if (globalpreferences.viewbookmarks) {
		toolbarBookmarks->setVisible(true);
	}
	else {
		toolbarBookmarks->setVisible(false);
	}
}


void cMainWindow::openDetail(int rowid) {
	int last = lastactivedetail;

	QRect geometry;
	bool usedefaultsize = true;
	bool ishidden = spectradetails[rowid].isHidden();

	if ((last != -1) /*&& (last != rowid)*/) {
		if (last == -2) {
			geometry = othernormalgeometry;
		}
		else {
			geometry = spectradetails[last].normalGeometry();
		}

		if (ishidden) {
			spectradetails[rowid].move(geometry.x(), geometry.y());
			spectradetails[rowid].setGeometry(geometry);
		}

		usedefaultsize = false;
	}

	spectradetails[rowid].prepareToShow(actionShowIsomers, &rawdata, &imzmlprofilemetadata, profilemz64precision, profileintensity64precision, usedefaultsize);

	spectradetails[rowid].show();
	spectradetails[rowid].activateWindow();
	if (spectradetails[rowid].isMinimized()) {
		spectradetails[rowid].showNormal();
	}

	if ((last != -1) /*&& (last != rowid)*/) {
		if (ishidden) {
			spectradetails[rowid].setGeometry(geometry);
		}

		if (last == -2) {
			if (otherismaximized) {
				spectradetails[rowid].showMaximized();
			}
			else {
				spectradetails[rowid].showNormal();
			}
			spectradetails[rowid].setProfileSpectrumEnabled(otherprofilespectrum);
			spectradetails[rowid].setAbsoluteIntensityEnabled(otherabsoluteintensity);
			spectradetails[rowid].setHideMatchedPeaksEnabled(othermatchedpeaks);
			spectradetails[rowid].setHideUnmatchedPeaksEnabled(otherunmatchedpeaks);
			spectradetails[rowid].setHideScrambledPeaksEnabled(otherscrambledpeaks);
			spectradetails[rowid].setHidePeakLabelsEnabled(otherlabelspeaks);
			spectradetails[rowid].setMouseMzSelectionEnabled(othermzselection);
		}
		else {
			if (spectradetails[last].isMaximized()) {
				spectradetails[rowid].showMaximized();
			}
			else {
				spectradetails[rowid].showNormal();
			}
			spectradetails[rowid].setProfileSpectrumEnabled(spectradetails[last].hasProfileSpectrumEnabled());
			spectradetails[rowid].setAbsoluteIntensityEnabled(spectradetails[last].hasAbsoluteIntensityEnabled());
			spectradetails[rowid].setHideMatchedPeaksEnabled(spectradetails[last].hasHideMatchedPeaksEnabled());
			spectradetails[rowid].setHideUnmatchedPeaksEnabled(spectradetails[last].hasHideUnmatchedPeaksEnabled());
			spectradetails[rowid].setHideScrambledPeaksEnabled(spectradetails[last].hasHideScrambledPeaksEnabled());
			spectradetails[rowid].setHidePeakLabelsEnabled(spectradetails[last].hasHidePeakLabelsEnabled());
			spectradetails[rowid].setMouseMzSelectionEnabled(spectradetails[last].hasMouseMzSelectionEnabled());
		}
	}
}


void cMainWindow::rowDoubleClicked(const QModelIndex& item) {
	int row = resultsproxymodel->mapToSource(item).row();
	int rowid = resultsmodel->item(row, 1)->data(Qt::DisplayRole).toInt() - 1;
	openDetail(rowid);
}


void cMainWindow::setGraph(string s) {
	graph->setHTML(s, actionShowIsomers->isChecked());
}


void cMainWindow::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));
	
	if (!filename.isEmpty()) {
		lastdirexporttocsv = filename;

		QProgressDialog progress("Exporting CSV file...", "Cancel", 0, resultsproxymodel->rowCount(), this);
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

		for (int i = 0; i < resultsmodel->columnCount(); i++) {
			out << "\"" << resultsmodel->horizontalHeaderItem(i)->text() << "\"";
			if (i < resultsmodel->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		QStandardItem* item;
		for (int i = 0; i < resultsproxymodel->rowCount(); i++) {

			for (int j = 0; j < resultsmodel->columnCount(); j++) {
				item = resultsmodel->itemFromIndex(resultsproxymodel->mapToSource(resultsproxymodel->index(i, j)));
				out << "\"" << item->data(Qt::DisplayRole).toString() << "\"";
				if (j < resultsmodel->columnCount() - 1) {
					out << ",";
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

		progress.setValue(resultsproxymodel->rowCount());
	}
}


void cMainWindow::exportToHTML() {

	if (htmlexportdialog->exec() != QDialog::Accepted) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export to HTML"), lastdirexporttohtml, tr("HTML Files (*.htm *.html)"));
	
	if (!filename.isEmpty()) {			
		lastdirexporttohtml = filename;

		QProgressDialog progress("Exporting HTML report...", "Cancel", 0, resultsproxymodel->rowCount(), this);
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

		bool reportisomers = actionShowIsomers->isChecked();

		QTextStream out(&file);

		if (resultsmodel->rowCount() > 0) {

			bool matchedrow;
			QStandardItem* item;
			string title = appname.toStdString() + " " + appversion.toStdString() + " - Output Report";

			out << "<html>\n";
			out << "<head>\n";
			out << "<title>" << QString(title.c_str()) << "</title>\n";
		
			out << "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.4/jquery.min.js\"></script>\n";
			out << "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jquery.tablesorter/2.23.3/js/jquery.tablesorter.min.js\"></script>\n";
			out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"https://cdnjs.cloudflare.com/ajax/libs/jquery.tablesorter/2.23.3/css/theme.green.min.css\">\n";
			out << "<script>$(document).ready(function() { $(\".tablesorter\").tablesorter( { theme: 'green'} ); } ); </script>";

			out << "</head>\n";
			out << "<body style=\"font-family: Verdana, Arial; font-size: 10pt\">\n";

			out << "<h1><a href=\"https://ms.biomed.cas.cz/cyclobranch/\">" << QString(title.c_str()) << "</a></h1>\n";

			if (htmlexportdialog->checkboxparameters->isChecked()) {	
				out << "<h2>Parameters</h2>\n";
				out << "<p><pre>\n" << parameters.printToString().c_str() << "</pre></p>\n";
				out << "<hr size=\"1\"/>\n";
			}

			if (htmlexportdialog->checkboxdenovo->isChecked() && (parameters.mode == denovoengine)) {
				out << "<h2>De Novo Graph</h2>\n";
				out << "<p><font face=\"Courier\">\n";
				string graphhtml = graph->getHTML();
				if (!reportisomers) {
					stripIsomers(graphhtml);
				}
				out << graphhtml.c_str();
				out << "</font></p>\n";
				out << "<hr size=\"1\"/>\n";
			}

			if (htmlexportdialog->checkboxlogwindow->isChecked()) {
				out << "<h2>Log Window</h2>\n";
				out << "<p><pre>\n" << logWindow->toPlainText() << "</pre></p>\n";
				out << "<hr size=\"1\"/>\n";
			}

			if (htmlexportdialog->checkboxsummaryresultstable->isChecked()) {
				out << "<h2>Output Report Table</h2>\n";

				out << "<p><table class=\"tablesorter\" cellpadding=\"0\" cellspacing=\"0\" border=\"1\" width=\"100%\">\n<thead><tr>\n";
				for (int i = 0; i < resultsmodel->columnCount(); i++) {
					out << "<th><b>" << resultsmodel->horizontalHeaderItem(i)->text() << "</b></th>\n";
				}
				out << "</tr></thead><tbody>\n";

				for (int i = 0; i < resultsproxymodel->rowCount(); i++) {

					out << "<tr>\n";
			
					matchedrow = false;
					if (resultsmodel->itemFromIndex(resultsproxymodel->mapToSource(resultsproxymodel->index(i, 0)))->data(Qt::DisplayRole).toString().compare("*") == 0) {
						matchedrow = true;
					}

					for (int j = 0; j < resultsmodel->columnCount(); j++) {
						item = resultsmodel->itemFromIndex(resultsproxymodel->mapToSource(resultsproxymodel->index(i, j)));
						out << "<td";
						if (matchedrow) {
							out << " bgcolor=\"yellow\"";
						}
						out << ">" << item->data(Qt::DisplayRole).toString() << "</td>\n";
					}

					out << "</tr>\n";

				}
				out << "</tbody></table></p>\n";
				out << "<br/><hr size=\"1\"/>\n";
			}

			if (htmlexportdialog->checkboxsummarypeakstable->isChecked()) {
				out << "<h2>Summary Table of Matched Peaks</h2>\n";

				out << "<p><table class=\"tablesorter\" cellpadding=\"0\" cellspacing=\"0\" border=\"1\" width=\"100%\"><thead><tr>";

				int columncount;
				string tdwidth;
				if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
					if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
						if (parameters.generateisotopepattern) {
							if (parameters.calculatefdrs) {
								columncount = 15;
							}
							else {
								columncount = 13;
							}
						}
						else {
							columncount = 12;
						}
					}
					else {
						if (parameters.generateisotopepattern) {
							if (parameters.calculatefdrs) {
								columncount = 14;
							}
							else {
								columncount = 12;
							}
						}
						else {
							columncount = 11;
						}
					}
				}
				else if (parameters.mode == denovoengine) {
					if (parameters.generateisotopepattern) {
						columncount = 11;
					}
					else {
						columncount = 10;
					}
				}
				else {
					if (parameters.generateisotopepattern) {
						columncount = 10;
					}
					else {
						columncount = 9;
					}
				}

				tdwidth = to_string(100/columncount);

				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>ID</b></th>";

				if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
					if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Coordinate X</b></th>";
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Coordinate Y</b></th>";
					}
					else {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Time</b></th>";
					}
					if (parameters.generateisotopepattern) {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Pattern Type</b></th>";
					}
					else {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Ion Type</b></th>";
					}
				}

				if (parameters.mode == denovoengine) {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Group ID</b></th>";
				}

				if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Fragment Type</b></th>";
				}

				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Theoretical m/z</b></th>";

				if (parameters.generateisotopepattern) {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Theoretical Intensity [%]</b></th>";
				}

				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Experimental m/z</b></th>";
				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Relative Intensity [%]</b></th>";
				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Absolute Intensity</b></th>";
				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Error [ppm]</b></th>";

				if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
					if (parameters.generateisotopepattern && parameters.calculatefdrs) {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Score</b></th>";
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>FDR</b></th>";
					}
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Summary Formula</b></th>";
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Name</b></th>";
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Reference</b></th>";
				}
				else {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Summary Formula</b></th>";
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Sequence</b></th>";
				}

				out << "</tr></thead><tbody>";

				int spectrumindex;
				for (int i = 0; i < resultsproxymodel->rowCount(); i++) {

					spectrumindex = resultsmodel->itemFromIndex(resultsproxymodel->mapToSource(resultsproxymodel->index(i, 1)))->data(Qt::DisplayRole).toInt() - 1;

					if (spectradetails[spectrumindex].getTheoreticalSpectrum().getNumberOfMatchedPeaks() == 0) {
						continue;
					}

					out << spectradetails[spectrumindex].getPartialPeaksTableAsHTMLString(spectrumindex, reportisomers).c_str();
				
				}

				out << "</tbody></table></p>";
				out << "<br/><hr size=\"1\"/>\n";

			}

			if (htmlexportdialog->checkboxdetails->isChecked() || htmlexportdialog->checkboxpeakstable->isChecked()) {
				out << "<h2>Individual Rows in Output Report</h2>\n";

				int spectrumindex;
				for (int i = 0; i < resultsproxymodel->rowCount(); i++) {

					spectrumindex = resultsmodel->itemFromIndex(resultsproxymodel->mapToSource(resultsproxymodel->index(i, 1)))->data(Qt::DisplayRole).toInt() - 1;

					if ((spectradetails[spectrumindex].getTheoreticalSpectrum().getNumberOfMatchedPeaks() == 0) && !htmlexportdialog->checkboxunmatchedtheoretical->isChecked() && !htmlexportdialog->checkboxunmatchedexperimental->isChecked()) {
						continue;
					}

					out << "<h3>ID: ";
					out << spectrumindex + 1;
					out << "</h3>\n";

					if (htmlexportdialog->checkboxpeakstable->isChecked()) {
						out << "<p align=\"center\">";
						out << spectradetails[spectrumindex].getPeaksTableAsHTMLString(htmlexportdialog->checkboxunmatchedtheoretical->isChecked(), htmlexportdialog->checkboxunmatchedexperimental->isChecked(), reportisomers).c_str();
						out << "</p>\n";
					}

					if (htmlexportdialog->checkboxdetails->isChecked()) {
						out << "<font face=\"Courier\">";
						out << "<p>";
						string textinfo = spectradetails[spectrumindex].getDetailsAsHTMLString() + spectradetails[spectrumindex].getTheoreticalSpectrum().getCoverageBySeries();
						if (!reportisomers) {
							stripIsomers(textinfo);
						}
						out << textinfo.c_str();
						out << "</p>";
						out << "</font>\n";
					}

					out << "<br/><hr size=\"1\"/>\n";

					progress.setValue(i);
					if (progress.wasCanceled()) {
						removefile = true;
						break;
					}
				}
				
			}

			out << "</body>\n";
			out << "</html>\n";

		}
		
		file.close();

		if (removefile) {
			file.remove();
		}

		progress.setValue(resultsproxymodel->rowCount());

	}

}


void cMainWindow::showPreferences() {
	if (preferencesdialog->exec() != QDialog::Accepted) {
		preferencesdialog->resetPreferences(globalpreferences);
		return;
	}

	preferencesdialog->setPreferences(globalpreferences);

	parameterswidget->applyGlobalPreferences(&globalpreferences);
	summarytableofmatchedpeaks->applyGlobalPreferences(&globalpreferences);
	chromatogramwindow->applyGlobalPreferences(&globalpreferences);
	imagewindow->applyGlobalPreferences(&globalpreferences);
	bricksdatabasewidget->applyGlobalPreferences(&globalpreferences);
	sequencedatabasewidget->applyGlobalPreferences(&globalpreferences);
	modificationswidget->applyGlobalPreferences(&globalpreferences);
	multipledatasetswidget->applyGlobalPreferences(&globalpreferences);
	patternsimulatorwidget->applyGlobalPreferences(&globalpreferences);
	formulapredictor->applyGlobalPreferences();
	
	applyGlobalPreferences();

	globalpreferences.saveSettings();
}


void cMainWindow::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/userguide.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/userguide.html").absoluteFilePath()));
	#endif
}


void cMainWindow::showPDFManual() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/refman.pdf").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/refman.pdf").absoluteFilePath()));
	#endif	
}


void cMainWindow::saveResultsFile() {

	#if OS_TYPE == WIN
		string filetypes = appname.toStdString() + " " + appversion.toStdString() + " Results Files (*.res)";
	#else
		string filetypes = "Results Files (*.res)";
	#endif
	
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Results..."), lastdirsaveresults, tr(filetypes.c_str()));
	
	if (!filename.isEmpty()) {
		lastdirsaveresults = filename;

		int savesize = 0;
		for (int h = 0; h < listoftheoreticalspectra.size(); h++) {
			savesize += listoftheoreticalspectra.size(h);
		}

		QProgressDialog progress("Saving the report...", "Cancel", 0, savesize, this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);
		progress.setValue(0);
		
		bool removefile = false;
		ofstream outfile;
		outfile.open(filename.toStdString().c_str(), ios::out | ios::binary | ios::trunc);

		if (outfile.is_open()) {
			int size;

			// write the application name and version
			QString appinfo = appname + " " + appversion;
			size = (int)appinfo.toStdString().size();
			outfile.write((char *)&size, sizeof(int));
			outfile.write(appinfo.toStdString().c_str(), size);

			// store graph window
			graph->store(outfile);

			// store log window
			size = logWindow->toPlainText().size();
			outfile.write((char *)&size, sizeof(int));
			outfile.write(logWindow->toPlainText().toStdString().c_str(), logWindow->toPlainText().size());

			// store parameters
			parameters.store(outfile);

			// store an auxiliary value for progress dialog
			outfile.write((char *)&savesize, sizeof(int));

			// store theoretical spectra
			int progresscount = 0;
			size = listoftheoreticalspectra.size();
			outfile.write((char *)&size, sizeof(int));
			for (int h = 0; h < listoftheoreticalspectra.size(); h++) {
				size = listoftheoreticalspectra.size(h);
				outfile.write((char *)&size, sizeof(int));
				for (int i = 0; i < listoftheoreticalspectra.size(h); i++) {
					listoftheoreticalspectra.get(h, i).store(outfile);
					progress.setValue(progresscount);
					progresscount++;
					if (progress.wasCanceled()) {
						removefile = true;
						break;
					}
				}
			}

			// store results from PubChem
			summarytableofmatchedpeaks->getPubChemSearchWidget()->store(outfile);
			multipledatasetswidget->getPubChemSearchWidget()->store(outfile);

			// store calibration data
			multipledatasetswidget->getDefineCalibrationWidget()->store(outfile);

			// close file
			outfile.close();
			
			if (removefile) {
				QFile::remove(filename);
			}
		}
	
		progress.setValue(savesize);

	}

}


void cMainWindow::openResultsFile() {
	string filetypes = "Results Files (*.res)";

	QString filename = QFileDialog::getOpenFileName(this, tr("Open Results..."), lastdiropenresults, tr(filetypes.c_str()));
	
	if (!filename.isEmpty()) {
		lastdiropenresults = filename;

		ifstream infile;
		infile.open(filename.toStdString().c_str(), ios::in | ios::binary);

		if (infile.is_open()) {
			int size;
			string s;
			cTheoreticalSpectrum theoreticalspectrum;

			// read the application name and version
			infile.read((char *)&size, sizeof(int));
			s.resize(size);
			infile.read(&s[0], s.size());


			string fileversion = s;
			fileversion = fileversion.substr(fileversion.find('.') + 2);
			fileversion = fileversion.substr(0, fileversion.rfind('(') - 1);

			int fileversionpart1 = QString(fileversion.substr(0, fileversion.find('.')).c_str()).toInt();

			fileversion = fileversion.substr(fileversion.find('.') + 1);
			int fileversionpart2 = QString(fileversion.substr(0, fileversion.find('.')).c_str()).toInt();

			fileversion = fileversion.substr(fileversion.find('.') + 1);
			int fileversionpart3 = QString(fileversion.c_str()).toInt();


			string lastcompatible = lastcompatibleappversion.toStdString();
			lastcompatible = lastcompatible.substr(lastcompatible.find('.') + 2);
			lastcompatible = lastcompatible.substr(0, lastcompatible.rfind('(') - 1);

			int lastcompatiblepart1 = QString(lastcompatible.substr(0, lastcompatible.find('.')).c_str()).toInt();

			lastcompatible = lastcompatible.substr(lastcompatible.find('.') + 1);
			int lastcompatiblepart2 = QString(lastcompatible.substr(0, lastcompatible.find('.')).c_str()).toInt();

			lastcompatible = lastcompatible.substr(lastcompatible.find('.') + 1);
			int lastcompatiblepart3 = QString(lastcompatible.c_str()).toInt();


			string currentversion = appversion.toStdString();
			currentversion = currentversion.substr(currentversion.find('.') + 2);
			currentversion = currentversion.substr(0, currentversion.rfind('(') - 1);

			int currentversionpart1 = QString(currentversion.substr(0, currentversion.find('.')).c_str()).toInt();

			currentversion = currentversion.substr(currentversion.find('.') + 1);
			int currentversionpart2 = QString(currentversion.substr(0, currentversion.find('.')).c_str()).toInt();

			currentversion = currentversion.substr(currentversion.find('.') + 1);
			int currentversionpart3 = QString(currentversion.c_str()).toInt();


			if (!isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, lastcompatiblepart1, lastcompatiblepart2, lastcompatiblepart3)) {
				QMessageBox msgBox;
				QString errstr = "The results file cannot be opened. The file was created in ";
				errstr += s.c_str();
				errstr += " but the minimum required version is " + appname + " " + lastcompatibleappversion + ".";
				msgBox.setText(errstr);
				msgBox.exec();

				infile.close();
				return;
			}


			bool oldversion = false;
			if (currentversionpart3 < fileversionpart3) {
				oldversion = true;
			}
			if (currentversionpart3 > fileversionpart3) {
				oldversion = false;
			}
			if (currentversionpart2 < fileversionpart2) {
				oldversion = true;
			}
			if (currentversionpart2 > fileversionpart2) {
				oldversion = false;
			}
			if (currentversionpart1 < fileversionpart1) {
				oldversion = true;
			}
			if (currentversionpart1 > fileversionpart1) {
				oldversion = false;
			}

			if (oldversion) {
				QMessageBox msgBox;
				QString errstr = "The results file cannot be opened. The file was created in ";
				errstr += s.c_str();
				errstr += " but your current version is only " + appname + " " + appversion + ".";
				msgBox.setText(errstr);
				msgBox.exec();

				infile.close();
				return;
			}

			deleteResults();

			summarytableofmatchedpeaks->deleteTable();
			summarytableofmatchedpeaks->hide();
			summarytableisprepared = false;

			imagewindow->hide();
			chromatogramwindow->hide();

			multipledatasetswidget->deleteTable();
			multipledatasetswidget->hide();
			multipledatasetswidget->getPubChemSearchWidget()->hide();
			multipledatasetswidget->getPubChemSearchWidget()->setHTML("");
			multipledatasetswidget->getDefineCalibrationWidget()->hide();
			multipledatasetswidget->getCalibrationChartWidget()->hide();
			multipledatasetsisprepared = false;
			
			listoftheoreticalspectra.clear();
			eicchromatograms.clear();
			spectradetails.clear();

			if (avgspectra.size() > 0) {
				avgspectrumused = 0;
				avgspectrummaximized = avgspectra[0].isMaximized();
				avgspectrumgeometry = avgspectra[0].normalGeometry();
			}

			avgspectra.clear();

			lastactivedetail = -1;

			othernormalgeometry = QRect();
			otherismaximized = false;
			otherprofilespectrum = false;
			otherabsoluteintensity = false;
			othermatchedpeaks = false;
			otherunmatchedpeaks = false;
			otherscrambledpeaks = false;
			otherlabelspeaks = false;
			othermzselection = true;

			activefileid = 0;

			parameters.clear();

			rawdata.clear();
			imzmlprofilemetadata.clear();
			profilemz64precision = false;
			profileintensity64precision = false;

			// load graph window
			graph->load(infile);
			graph->updateView(actionShowIsomers->isChecked());

			// load log window
			infile.read((char *)&size, sizeof(int));
			s.resize(size);
			infile.read(&s[0], s.size());
			logWindow->setPlainText(s.c_str());
			QScrollBar *sb = logWindow->verticalScrollBar();
			sb->setValue(sb->maximum());

			// load parameters
			parameters.load(infile, fileversionpart1, fileversionpart2, fileversionpart3);
			parameterswidget->setAndRestoreParameters(parameters);

			if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
				actionShowIsomers->setEnabled(false);
				if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
					actionImageWindow->setEnabled(true);
					actionChromatogramWindow->setEnabled(false);
				}
				else {
					actionImageWindow->setEnabled(false);
					actionChromatogramWindow->setEnabled(true);
				}
			}
			else {
				actionShowIsomers->setEnabled(true);
				actionImageWindow->setEnabled(false);
				actionChromatogramWindow->setEnabled(false);
			}

			int loadsize;

			// load theoretical spectra
			infile.read((char *)&loadsize, sizeof(int));

			QProgressDialog progress("Loading the report...", 0, 0, loadsize, this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::ApplicationModal);
			progress.setValue(0);

			if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 0)) {
				int progresscount = 0;
				infile.read((char *)&size, sizeof(int));
				listoftheoreticalspectra.resize(size);

				for (int h = 0; h < listoftheoreticalspectra.size(); h++) {
					infile.read((char *)&size, sizeof(int));
					for (int i = 0; i < size; i++) {
						theoreticalspectrum.load(infile, fileversionpart1, fileversionpart2, fileversionpart3);
						listoftheoreticalspectra.add(h, theoreticalspectrum);
						progress.setValue(progresscount);
						progresscount++;
					}
				}
			}
			else {
				listoftheoreticalspectra.resize(1);
				for (int i = 0; i < loadsize; i++) {
					theoreticalspectrum.load(infile, fileversionpart1, fileversionpart2, fileversionpart3);
					listoftheoreticalspectra.add(0, theoreticalspectrum);
					progress.setValue(i);
				}
			}

			progress.setValue(loadsize);

			reportSpectra();

			// load results from PubChem
			if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 0, 22)) {
				summarytableofmatchedpeaks->getPubChemSearchWidget()->load(infile);
			}
			else {
				summarytableofmatchedpeaks->getPubChemSearchWidget()->setHTML("");
			}

			if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 6)) {
				multipledatasetswidget->getPubChemSearchWidget()->load(infile);
			}
			else {
				multipledatasetswidget->getPubChemSearchWidget()->setHTML("");
			}

			if (isCompatibleVersion(fileversionpart1, fileversionpart2, fileversionpart3, 2, 1, 14)) {
				multipledatasetswidget->getDefineCalibrationWidget()->load(infile, fileversionpart1, fileversionpart2, fileversionpart3);
			}
			else {
				// nothing to do
			}

			// close file
			infile.close();
		}
	
	}

}


void cMainWindow::setSequence(int peptidetypeindex, QString sequence) {
	showProperties();
	parameterswidget->setSequence(peptidetypeindex, sequence);
}


void cMainWindow::setTag(int peptidetypeindex, QString tag) {
	showProperties();
	parameterswidget->setTag(peptidetypeindex, tag);
}


void cMainWindow::insertSequence(int peptidetypeindex, QString sequence) {
	showSequenceDatabase();
	sequencedatabasewidget->importSequence(peptidetypeindex, sequence);
}


void cMainWindow::quitApplication() {
	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, appname, "Quit ?", QMessageBox::Yes|QMessageBox::No);
	
	if (reply != QMessageBox::Yes) {
		return;
	}
 
	emit stopComputation();
	quitapp = true;

	if (actionRun->isEnabled()) {
		qApp->quit();
	}
}


void cMainWindow::filterResults() {
	resultsproxymodel->setWholeWord(rowsfilterwholeword->isChecked());
	resultsproxymodel->setFilterKeyColumn(-1);
	resultsproxymodel->setFilterCaseSensitivity(rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive);
	resultsproxymodel->setFilterFixedString("");

	if (!summarytableofmatchedpeaks->isHidden() || !imagewindow->isHidden() || !chromatogramwindow->isHidden()) {
		summarytableofmatchedpeaks->deleteTable();
		summarytableisprepared = false;

		prepareSummaryTableOfMatchedPeaks();
	}
	else {
		summarytableofmatchedpeaks->deleteTable();
		summarytableisprepared = false;
	}
}


void cMainWindow::resetFilter() {
	rowsfilterline1->setText("");
	rowsfilterline2->setText("");

	results->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
	resultsproxymodel->sort(-1);

	resultsproxymodel->setWholeWord(false);
	resultsproxymodel->setFilterKeyColumn(-1);
	resultsproxymodel->setFilterFixedString("");

	if (!summarytableofmatchedpeaks->isHidden() || !imagewindow->isHidden() || !chromatogramwindow->isHidden()) {
		summarytableofmatchedpeaks->deleteTable();
		summarytableisprepared = false;

		prepareSummaryTableOfMatchedPeaks();
	}
	else {
		summarytableofmatchedpeaks->deleteTable();
		summarytableisprepared = false;
	}
}


void cMainWindow::lastActiveDetailSlot(int rowid) {
	lastactivedetail = rowid - 1;
}


void cMainWindow::gotoBookMark1() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl1));
}


void cMainWindow::gotoBookMark2() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl2));
}


void cMainWindow::gotoBookMark3() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl3));
}


void cMainWindow::gotoBookMark4() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl4));
}


void cMainWindow::gotoBookMark5() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl5));
}


void cMainWindow::gotoBookMark6() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl6));
}


void cMainWindow::gotoBookMark7() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl7));
}


void cMainWindow::gotoBookMark8() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl8));
}


void cMainWindow::gotoBookMark9() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl9));
}


void cMainWindow::gotoBookMark10() {
	QDesktopServices::openUrl(QUrl(globalpreferences.bookmarkurl10));
}


void cMainWindow::summaryPeaksTableCancelled() {
	summarytableisprepared = false;
}


void cMainWindow::multipleDatasetsTableCancelled() {
	multipledatasetsisprepared = false;
}


void cMainWindow::summaryPeaksTableRowDoubleClicked(int rowid, double experimentalmz) {
	openDetail(rowid);
	spectradetails[rowid].zoomToPeak(experimentalmz);
}


void cMainWindow::multipleDatasetsTableRowClicked(int datasetid, int spectrumid, int peakid, int datatypeview, int peakshape, string name, string iontype, string mzstr) {
	bool ctrlpressed = false;
	if (QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
		ctrlpressed = true;
	}

	bool activatetable = true;
	int last = lastactivedetail;
	
	if (last == -1) {
		activatetable = false;
	}

	if (datasetid != activefileid) {
		inputfiltercombobox->setCurrentIndex(datasetid);
		inputFilterButtonReleased();
	}

	if (prepareSummaryTableOfMatchedPeaks()) {
		summarytableofmatchedpeaks->filterCompound(name, iontype, datatypeview, mzstr);

		if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
			if ((parameters.peaklistfileformats[activefileid] == mis) || (parameters.peaklistfileformats[activefileid] == imzML)) {
				if (imagewindow->isHidden()) {
					showImageWindow();
					activatetable = false;
				}
			}
			else {
				if (chromatogramwindow->isHidden()) {
					showChromatogramWindow();
					activatetable = false;
				}

				//if (datatypeview == 0) {
				//	chromatogramwindow->setAbsoluteIntensityEnabled(false);
				//}
				//else {
				//	chromatogramwindow->setAbsoluteIntensityEnabled(true);
				//}

				chromatogramwindow->setAbsoluteIntensityEnabled(true);

				chromatogramwindow->setPeakShape(peakshape);
			}
		}
	}

	if (ctrlpressed) {
		openDetail(spectrumid);
		spectradetails[spectrumid].zoomToPeak(listoftheoreticalspectra.get(datasetid, spectrumid)[peakid].matchedmz);
	}

	if (activatetable) {
		multipledatasetswidget->activateWindow();
	}
}


void cMainWindow::updateSummaryPeaksTableFilterSlot(int xmin, int xmax, int ymin, int ymax) {
	summarytableofmatchedpeaks->updateFilterBySelectedRegion(xmin, xmax, ymin, ymax);
}


void cMainWindow::chromatogramDoubleClicked(int scanid) {
	if ((scanid >= 0) && (scanid < spectradetails.size())) {
		openDetail(scanid);
	}
}


void cMainWindow::imageWindowDoubleClicked(int spectrumid) {
	if ((spectrumid >= 0) && (spectrumid < spectradetails.size())) {
		openDetail(spectrumid);
	}
}


void cMainWindow::inputFilterButtonReleased() {
	if (inputfiltercombobox->count() > 0) {
		if (lastactivedetail >= 0) {
			othernormalgeometry = spectradetails[lastactivedetail].normalGeometry();
			otherismaximized = spectradetails[lastactivedetail].isMaximized();
			otherprofilespectrum = spectradetails[lastactivedetail].hasProfileSpectrumEnabled();
			otherabsoluteintensity = spectradetails[lastactivedetail].hasAbsoluteIntensityEnabled();
			othermatchedpeaks = spectradetails[lastactivedetail].hasHideMatchedPeaksEnabled();
			otherunmatchedpeaks = spectradetails[lastactivedetail].hasHideUnmatchedPeaksEnabled();
			otherscrambledpeaks = spectradetails[lastactivedetail].hasHideScrambledPeaksEnabled();
			otherlabelspeaks = spectradetails[lastactivedetail].hasHidePeakLabelsEnabled();
			othermzselection = spectradetails[lastactivedetail].hasMouseMzSelectionEnabled();
			lastactivedetail = -2;
		}

		summarytableofmatchedpeaks->deleteTable();
		summarytableisprepared = false;

		spectradetails.clear();

		if (avgspectra.size() > 0) {
			avgspectrumused = 0;
			avgspectrummaximized = avgspectra[0].isMaximized();
			avgspectrumgeometry = avgspectra[0].normalGeometry();
		}

		avgspectra.clear();

		rawdata.clear();

		activefileid = inputfiltercombobox->currentIndex();

		reportSpectra();

		resetFilter();

		if (chromatogramwindow->isVisible() || imagewindow->isVisible() || summarytableofmatchedpeaks->isVisible()) {
			if (prepareSummaryTableOfMatchedPeaks()) {
				if (chromatogramwindow->isVisible()) {
					chromatogramwindow->recalculateTICChromatogram(activefileid);
				}
			}
		}
	}
}


void cMainWindow::filterSummaryTableIfPrepared() {
	if (summarytableisprepared) {
		summarytableofmatchedpeaks->filterTablerows();
	}
}


void cMainWindow::showAvgSpectrum(int minscan, int maxscan) {
	calculateAvgSpectrum(minscan, maxscan);
}


void cMainWindow::avgSpectrumAbsoluteIntensityStateChanged(bool state) {
	avgspectrumabsoluteintensityenabled = state;
}


/*
void cMainWindow::showContextMenu(const QPoint &pt) {
    QMenu *menu = logWindow->createStandardContextMenu();
    menu->addAction(clear);
	menu->exec(logWindow->mapToGlobal(pt));
    delete menu;
}
*/

