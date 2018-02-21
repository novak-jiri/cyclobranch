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
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QCloseEvent>


cMainWindow::cMainWindow() {
	setWindowTitle(appname);
	setWindowIcon(QIcon(":/images/cb.png"));


	// main menu
	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	

	// items in the menu
	menuFile = new QMenu(tr("&File"), this);
	menuSearch = new QMenu(tr("&Search"), this);
	menuTools = new QMenu(tr("&Tools"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);


	// subitems in the menu
	actionOpenResults = new QAction(QIcon(":/images/icons/52.png"), tr("&Open Results..."), this);
	actionOpenResults->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

	actionSaveResults = new QAction(QIcon(":/images/icons/22.png"), tr("&Save Results..."), this);
	actionSaveResults->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

	actionExportToCsv = new QAction(QIcon(":/images/icons/62.png"), tr("&Export to CSV"), this);
	actionExportToCsv->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	
	actionExportToHTML = new QAction(QIcon(":/images/icons/77.png"), tr("Export to &HTML"), this);
	actionExportToHTML->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));

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

	actionSequenceDatabase = new QAction(QIcon(":/images/icons/26.png"), tr("Sequence &Database Editor"), this);
	actionSequenceDatabase->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

	actionModifications = new QAction(QIcon(":/images/icons/61.png"), tr("&Modifications Editor"), this);
	actionModifications->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));

	actionDrawPeptide = new QAction(QIcon(":/images/icons/96.png"), tr("Draw &Peptide"), this);
	actionDrawPeptide->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));

	actionNorine = new QAction(QIcon(":/images/icons/25.png"), tr("&Norine"), this);
	actionNorine->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

	actionSmilesToMonomers = new QAction(QIcon(":/images/icons/5.png"), tr("Smiles2Monome&rs"), this);
	actionSmilesToMonomers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));


	actionShowIsomers = new QAction(QIcon(":/images/icons/95.png"), tr("Show &Isomers"), this);
	actionShowIsomers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
	actionShowIsomers->setCheckable(true);

	actionGraph = new QAction(QIcon(":/images/icons/32.png"), tr("De Novo &Graph"), this);
	actionGraph->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

	actionSummaryTableOfMatchedPeaks = new QAction(QIcon(":/images/icons/43.png"), tr("S&ummary Table of Matched Peaks"), this);
	actionSummaryTableOfMatchedPeaks->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));

	actionImageWindow = new QAction(QIcon(":/images/icons/23.png"), tr("Image &Window"), this);
	actionImageWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	actionImageWindow->setDisabled(true);

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
	toolbarTools->addSeparator();
	toolbarTools->addAction(actionNorine);
	toolbarTools->addAction(actionSmilesToMonomers);

	toolbarView = addToolBar(tr("View"));
	toolbarView->addAction(actionShowIsomers);
	toolbarView->addAction(actionGraph);
	toolbarView->addAction(actionSummaryTableOfMatchedPeaks);
	toolbarView->addAction(actionImageWindow);
	toolbarView->addAction(actionLog);

	toolbarHelp = addToolBar(tr("Help"));
	toolbarHelp->addAction(actionHTMLDocumentation);
	toolbarHelp->addAction(actionPDFManual);
	toolbarHelp->addAction(actionAbout);

	rowsfilterline = new QLineEdit();
	rowsfilterline->setMinimumWidth(250);
	rowsfilterline->setToolTip("Text to Find");

	rowsfiltercasesensitive = new QCheckBox();
	rowsfiltercasesensitive->setToolTip("Case Sensitive");

	rowsfilterbutton = new QPushButton("Filter");
	rowsfilterbutton->setToolTip("Filter Search Results");
	rowsfilterbutton->setMinimumWidth(50);

	rowsfilterclearbutton = new QPushButton("Reset");
	rowsfilterclearbutton->setToolTip("Reset Search Results");
	rowsfilterclearbutton->setMinimumWidth(50);
	rowsfilterclearbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

	rowsfilterhbox = new QHBoxLayout();
	rowsfilterhbox->addWidget(rowsfilterline);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfiltercasesensitive);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfilterbutton);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfilterclearbutton);

	rowsfilterwidget = new QWidget();
	rowsfilterwidget->setLayout(rowsfilterhbox);
	rowsfilterwidget->setMaximumWidth(420);

	toolbarFilter = addToolBar(tr("Filter"));
	toolbarFilter->addWidget(rowsfilterwidget);


	// widgets
	results = new QTableWidget(0, 0, this);
	logWindow = new QTextEdit(this);
	splitter = new QSplitter(this);

	about = new cAboutWidget(this);

	graph = new cGraphWidget();
	bricksdatabasewidget = new cBricksDatabaseWidget();
	sequencedatabasewidget = new cSequenceDatabaseWidget(this);
	modificationswidget = new cModificationsWidget();
	drawpeptidewidget = new cDrawPeptideWidget(this);
	summarytableofmatchedpeaks = new cSummaryPeaksTableWidget(this);
	imagewindow = new cImageWindow(this);
	parameterswidget = new cParametersWidget(this);
	htmlexportdialog = new cHTMLExportDialog(this);
	
	// additional key shortcuts
	// actionOpen->setShortcut(QKeySequence("Ctrl+O"));

	// connection of menu items with functions
	// connect(actionOpen, SIGNAL(triggered()), this, SLOT(openDir()));
	connect(actionOpenResults, SIGNAL(triggered()), this, SLOT(openResultsFile()));
	connect(actionSaveResults, SIGNAL(triggered()), this, SLOT(saveResultsFile()));
	connect(actionExportToCsv, SIGNAL(triggered()), this, SLOT(exportToCsv()));
	connect(actionExportToHTML, SIGNAL(triggered()), this, SLOT(exportToHTML()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(quitApplication()));
	connect(actionRun, SIGNAL(triggered()), this, SLOT(run()));
	connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
	connect(actionProperties, SIGNAL(triggered()), this, SLOT(showProperties()));
	connect(actionBricksDatabase, SIGNAL(triggered()), this, SLOT(showBricksDatabase()));
	connect(actionSequenceDatabase, SIGNAL(triggered()), this, SLOT(showSequenceDatabase()));
	connect(actionModifications, SIGNAL(triggered()), this, SLOT(showModifications()));
	connect(actionDrawPeptide, SIGNAL(triggered()), this, SLOT(showDrawPeptideWidget()));
	connect(actionNorine, SIGNAL(triggered()), this, SLOT(gotoNorine()));
	connect(actionSmilesToMonomers, SIGNAL(triggered()), this, SLOT(gotoSmiles2Monomers()));
	connect(actionShowIsomers, SIGNAL(triggered()), this, SLOT(updateSpectra()));
	connect(actionGraph, SIGNAL(triggered()), this, SLOT(showGraph()));
	connect(actionSummaryTableOfMatchedPeaks, SIGNAL(triggered()), this, SLOT(showSummaryTableOfMatchedPeaks()));
	connect(actionImageWindow, SIGNAL(triggered()), this, SLOT(showImageWindow()));
	connect(actionLog, SIGNAL(triggered()), this, SLOT(showHideLog()));
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));
	connect(actionPDFManual, SIGNAL(triggered()), this, SLOT(showPDFManual()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterResults()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	connect(summarytableofmatchedpeaks, SIGNAL(tableCancelled()), this, SLOT(summaryPeaksTableCancelled()));

	// add subitems to the items in main menu
	// menuFile->addAction(actionOpen);
	menuFile->addAction(actionOpenResults);
	menuFile->addAction(actionSaveResults);
	menuFile->addSeparator();
	menuFile->addAction(actionExportToCsv);
	menuFile->addAction(actionExportToHTML);
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
	menuTools->addAction(actionNorine);
	menuTools->addAction(actionSmilesToMonomers);
	menuView->addAction(actionShowIsomers);
	menuView->addAction(actionGraph);
	menuView->addSeparator();
	menuView->addAction(actionSummaryTableOfMatchedPeaks);
	menuView->addAction(actionImageWindow);
	menuView->addSeparator();
	menuView->addAction(actionLog);
	menuHelp->addAction(actionHTMLDocumentation);
	menuHelp->addAction(actionPDFManual);
	menuHelp->addSeparator();
	menuHelp->addAction(actionAbout);

	// add items to the main menu
	menuBar->addMenu(menuFile);
	menuBar->addMenu(menuSearch);
	menuBar->addMenu(menuTools);
	menuBar->addMenu(menuView);
	menuBar->addMenu(menuHelp);

	// enable the menu
	setMenuBar(menuBar);

	results->setEditTriggers(QAbstractItemView::NoEditTriggers);
	results->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	results->horizontalHeader()->setSectionsMovable(true);
	connect(results->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerItemDoubleClicked(int)));
	connect(results, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(resultsCellClicked(int, int)));

	logWindow->setReadOnly(true);

	about->hide();
	graph->hide();
	bricksdatabasewidget->hide();
	sequencedatabasewidget->hide();
	modificationswidget->hide();
	drawpeptidewidget->hide();
	summarytableofmatchedpeaks->hide();
	imagewindow->hide();
	parameterswidget->hide();

	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(results);
	splitter->addWidget(logWindow);
	splitter->setStretchFactor(0, 1);

	// set the central widget
	setCentralWidget(splitter);

	// set the size of main window
	resize(1280, 700);

	resultsbasecolumncount = 9;
	resultsspecificcolumncount = 0;
	dbsearchspecificcolumncount = 0;

	theoreticalspectrumlist.clear();
	spectradetails.clear();

	lastdirexporttocsv = "./";
	lastdirexporttohtml = "./";
	lastdirsaveresults = "./";
	lastdiropenresults = "./";

	summarytableisprepared = false;

	quitapp = false;
}


cMainWindow::~cMainWindow() {
	deleteResults();

	delete rowsfilterline;
	delete rowsfiltercasesensitive;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

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
	delete imagewindow;
	delete parameterswidget;
	delete htmlexportdialog;

	delete actionOpenResults;
	delete actionSaveResults;
	delete actionExportToCsv;
	delete actionExportToHTML;
	delete actionQuit;
	delete actionProperties;
	delete actionRun;
	delete actionBricksDatabase;
	delete actionSequenceDatabase;
	delete actionModifications;
	delete actionDrawPeptide;
	delete actionNorine;
	delete actionSmilesToMonomers;
	delete actionShowIsomers;
	delete actionGraph;
	delete actionSummaryTableOfMatchedPeaks;
	delete actionImageWindow;
	delete actionLog;
	delete actionHTMLDocumentation;
	delete actionPDFManual;
	delete actionAbout;

	delete menuFile;
	delete menuSearch;
	delete menuTools;
	delete menuView;
	delete menuHelp;

	delete menuBar;
}


void cMainWindow::keyPressEvent(QKeyEvent *event) {
	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline->hasFocus()) {
			filterResults();
		}
		else {
			if ((results->currentRow() >= 0) && !results->isRowHidden(results->currentRow())) {		
				resultsCellClicked(results->currentRow(), 0);
			}
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_N)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}
}


void cMainWindow::closeEvent(QCloseEvent *event) {
	quitApplication();
	event->ignore();
}


void cMainWindow::preparePeptideSequence(int row, string& peptidesequence, bool reportisomers) {
	int spectrumindex = results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1;
	string tmpsequence;
	bool copy;

	if (!reportisomers) {
		tmpsequence = stripHTML(theoreticalspectrumlist[spectrumindex].getCandidate().getAcronymPeptideNameWithHTMLReferences());
		peptidesequence = "";
		copy = true;
		for (int j = 0; j < (int)tmpsequence.size(); j++) {
			if (tmpsequence[j] == '/') {
				copy = false;
			}
			if (tmpsequence[j] == ']') {
				copy = true;
			}
			if (copy) {
				peptidesequence += tmpsequence[j];
			}
		}
	}
	else {
		peptidesequence = stripHTML(theoreticalspectrumlist[spectrumindex].getCandidate().getAcronymPeptideNameWithHTMLReferences());
	}

}


void cMainWindow::reportSpectrum(int id, cTheoreticalSpectrum& theoreticalspectrum, bool reportisomers) {
	string peptidesequence;
	cSummaryFormula formula;

	int row = results->rowCount();
    results->insertRow(row);
	
	results->setItem(row, 0, widgetitemallocator.getNewItem());
	
	results->setItem(row, 1, widgetitemallocator.getNewItem());
	results->item(row, 1)->setData(Qt::DisplayRole, results->rowCount());

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {

		if (parameters.mode == databasesearch) {
			results->setItem(row, 2, widgetitemallocator.getNewItem());
			results->item(row, 2)->setText(theoreticalspectrum.getCandidate().getName().c_str());
		}

		results->setItem(row, 2 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		preparePeptideSequence(row, peptidesequence, reportisomers);
		results->item(row, 2 + dbsearchspecificcolumncount)->setText(peptidesequence.c_str());

		formula = theoreticalspectrum.getCandidate().getSummaryFormula(parameters, parameters.peptidetype);

		results->setItem(row, 3 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 3 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, formula.isPartial()?string(formula.getSummary() + " (partial)").c_str():formula.getSummary().c_str());

		results->setItem(row, 4 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 4 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(formula.getMass()));

		results->setItem(row, 5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 5 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, getNumberOfBricks(theoreticalspectrum.getCandidate().getComposition()));

		switch (parameters.peptidetype)
		{
		case linear:
#if OLIGOKETIDES == 1
		case linearoligoketide:
#endif
		case linearpolysaccharide:
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
			results->setItem(row, 7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 7 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
			break;
		case branched:
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
			results->setItem(row, 7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 7 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str());
			results->setItem(row, 8 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 8 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
			break;
		case cyclic:
#if OLIGOKETIDES == 1
		case cyclicoligoketide:
#endif
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedBricks());	
			break;
		case branchcyclic:
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str());
			break;
		case other:
		default:
			break;
		}

		results->setItem(row, 6 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 6 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks());

		results->setItem(row, 7 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 7 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(theoreticalspectrum.getRatioOfMatchedPeaks()*100));

		results->setItem(row, 8 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 8 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(theoreticalspectrum.getWeightedIntensityScore()));

		for (int i = 0; i < (int)parameters.fragmentionsfortheoreticalspectra.size(); i++) {
			results->setItem(row, resultsbasecolumncount  + dbsearchspecificcolumncount + resultsspecificcolumncount + i, widgetitemallocator.getNewItem());
			results->item(row, resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks(parameters.fragmentionsfortheoreticalspectra[i]));
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			results->setItem(row, results->columnCount() - 1, widgetitemallocator.getNewItem());
			results->item(row, results->columnCount() - 1)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfScrambledPeaks());
		}

		if (theoreticalspectrum.isValid()) {
			results->item(row, 0)->setData(Qt::DisplayRole, "*");
			for (int i = 0; i < results->columnCount(); i++) {
				results->item(row, i)->setBackground(Qt::yellow);
			}
		}

	}


	if (parameters.mode == dereplication) {
		results->setItem(row, 2, widgetitemallocator.getNewItem());
		results->item(row, 2)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks());

		results->setItem(row, 3, widgetitemallocator.getNewItem());
		results->item(row, 3)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(theoreticalspectrum.getRatioOfMatchedPeaks()*100));

		results->setItem(row, 4, widgetitemallocator.getNewItem());
		results->item(row, 4)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(theoreticalspectrum.getWeightedIntensityScore()));

		if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			results->setItem(row, 5, widgetitemallocator.getNewItem());
			results->item(row, 5)->setData(Qt::DisplayRole, theoreticalspectrum.getExperimentalSpectrum().getCoordinateX());	

			results->setItem(row, 6, widgetitemallocator.getNewItem());
			results->item(row, 6)->setData(Qt::DisplayRole, theoreticalspectrum.getExperimentalSpectrum().getCoordinateY());		
		}
	}


	spectradetails[id].initialize(&parameters, theoreticalspectrum, this);
	if (parameters.mode == dereplication) {
		spectradetails[id].setWindowTitle(("Experimental Spectrum No. " + to_string(row+1)).c_str());
	}
	else {
		spectradetails[id].setWindowTitle(("Theoretical Spectrum No. " + to_string(row+1)).c_str());
	}
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
}


void cMainWindow::showBricksDatabase() {
	bricksdatabasewidget->show();
	bricksdatabasewidget->activateWindow();
}


void cMainWindow::showSequenceDatabase() {
	sequencedatabasewidget->show();
	sequencedatabasewidget->activateWindow();
}


void cMainWindow::showModifications() {
	modificationswidget->show();
	modificationswidget->activateWindow();
}


void cMainWindow::showDrawPeptideWidget() {
	drawpeptidewidget->show();
	drawpeptidewidget->activateWindow();
}


void cMainWindow::setAndShowDrawPeptideWidget(int peptidetypeindex, QString sequence) {
	drawpeptidewidget->setSequence(peptidetypeindex, sequence);
	showDrawPeptideWidget();
}


void cMainWindow::showSummaryTableOfMatchedPeaks() {
	summarytableofmatchedpeaks->show();
	summarytableofmatchedpeaks->activateWindow();
	if (!summarytableisprepared) {
		summarytableisprepared = true;
		rowsfilterwidget->setEnabled(false);
		summarytableofmatchedpeaks->prepareToShow(results, &parameters, &theoreticalspectrumlist);
		rowsfilterwidget->setEnabled(true);
	}
}


void cMainWindow::showImageWindow() {
	imagewindow->show();
	imagewindow->activateWindow();
}


void cMainWindow::showGraph() {
	graph->show();
	graph->activateWindow();
}


void cMainWindow::showProperties() {
	parameterswidget->show();
	parameterswidget->activateWindow();
}


void cMainWindow::run() {
	enableRunButtonAndSettings(false);
	enableStopButton(true);
	enableButtonsHandlingResults(false);

	logWindow->clear();
	
	deleteResults();
	
	theoreticalspectrumlist.clear();
	spectradetails.clear();

	cParameters localparameters = parameterswidget->getParameters();

	regex rx;
	rx = "\\.[iI][mM][zZ][mM][lL]$";
	if (regex_search(localparameters.peaklistfilename, rx)) {
		string convertedimzml = localparameters.peaklistfilename.substr(0, (int)localparameters.peaklistfilename.size() - 6);
		string convertedibd = convertedimzml;
		convertedimzml += "_converted.imzML";
		convertedibd += "_converted.ibd";

		if (QFile::exists(convertedimzml.c_str()) && QFile::exists(convertedibd.c_str())) {
			QMessageBox::StandardButton reply;
			string s = "The converted imzML file '" + convertedimzml.substr(convertedimzml.rfind('/') + 1) + "' has been found.\n\nDo you want to proceed with this file instead of the original imzML file '";
			s += localparameters.peaklistfilename.substr(localparameters.peaklistfilename.rfind('/') + 1) + "' ?";
			s += "\n\nClick 'Yes' to proceed the converted file.\nClick 'No' to convert the original file again. Note: The conversion may be time-consuming.";

			reply = QMessageBox::question(this, "Do you want to proceed the converted file ?", s.c_str(), QMessageBox::Yes|QMessageBox::No);

			if (reply == QMessageBox::Yes) {
				localparameters.peaklistfilename = convertedimzml;
			}
		}
	}

	cMainThread* thread = new cMainThread(localparameters, theoreticalspectrumlist, true, false);
	connect(thread, SIGNAL(message(QString)), this, SLOT(updateLog(QString)));
	connect(thread, SIGNAL(enableRunButtonAndSettings(bool)), this, SLOT(enableRunButtonAndSettings(bool)));
	connect(thread, SIGNAL(enableStopButton(bool)), this, SLOT(enableStopButton(bool)));
	connect(thread, SIGNAL(enableButtonsHandlingResults(bool)), this, SLOT(enableButtonsHandlingResults(bool)));
	connect(thread, SIGNAL(sendParameters(cParameters)), this, SLOT(sendParameters(cParameters)));
	connect(thread, SIGNAL(reportSpectra()), this, SLOT(reportSpectra()));
	connect(thread, SIGNAL(setGraph(string)), this, SLOT(setGraph(string)));
	connect(this, SIGNAL(stopComputation()), thread, SLOT(stopComputation()));

	summarytableofmatchedpeaks->deleteTable(true);
	summarytableofmatchedpeaks->hide();
	summarytableisprepared = false;

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
	rowsfilterwidget->setEnabled(enable);
	actionSummaryTableOfMatchedPeaks->setEnabled(enable);
	
	if (parameters.mode == dereplication) {
		actionShowIsomers->setEnabled(false);
	}
	else {
		actionShowIsomers->setEnabled(enable);
	}	
}


void cMainWindow::sendParameters(cParameters parameters) {
	this->parameters = parameters;
}


void cMainWindow::reportSpectra() {

	if (quitapp) {
		return;
	}

	switch (parameters.peptidetype)
	{
	case linear:
#if OLIGOKETIDES == 1
	case linearoligoketide:
#endif
	case linearpolysaccharide:
		resultsspecificcolumncount = 2;
		break;
	case branched:
		resultsspecificcolumncount = 3;
		break;
	case cyclic:
#if OLIGOKETIDES == 1
	case cyclicoligoketide:
#endif
		resultsspecificcolumncount = 1;
		break;
	case branchcyclic:
		resultsspecificcolumncount = 1;
		break;
	case other:
	default:
		break;
	}


	if (parameters.mode == databasesearch) {
		dbsearchspecificcolumncount = 1;
	}
	else {
		dbsearchspecificcolumncount = 0;
	}


	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) { 

		results->setColumnCount(resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + (int)parameters.fragmentionsfortheoreticalspectra.size());

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			results->setColumnCount(results->columnCount() + 1);
		}

		results->setHorizontalHeaderItem(0, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(0)->setText("*");

		results->setHorizontalHeaderItem(1, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(1)->setText("Result ID");

		if (parameters.mode == databasesearch) {
			results->setHorizontalHeaderItem(2, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(2)->setText("Name");
		}
		
		results->setHorizontalHeaderItem(2 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(2 + dbsearchspecificcolumncount)->setText("Peptide Sequence");
		
		results->setHorizontalHeaderItem(3 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(3 + dbsearchspecificcolumncount)->setText("Summary Formula");
		
		results->setHorizontalHeaderItem(4 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(4 + dbsearchspecificcolumncount)->setText("Monoisotopic Mass");
		results->setItemDelegateForColumn(4 + dbsearchspecificcolumncount, &columndelegate);
		
		results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(5 + dbsearchspecificcolumncount)->setText("Number of Bricks");

		switch (parameters.peptidetype)
		{
		case linear:
		case linearpolysaccharide:
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("N-terminal Modification");
			results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(7 + dbsearchspecificcolumncount)->setText("C-terminal Modification");
			break;
		case branched:
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("N-terminal Modification");
			results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(7 + dbsearchspecificcolumncount)->setText("Branch Modification");
			results->setHorizontalHeaderItem(8 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(8 + dbsearchspecificcolumncount)->setText("C-terminal Modification");
			break;
		case cyclic:
#if OLIGOKETIDES == 1
		case cyclicoligoketide:
#endif
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("Matched Bricks");
			break;
		case branchcyclic:
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("Branch Modification");
			break;
#if OLIGOKETIDES == 1
		case linearoligoketide:
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("Left Terminal Modification");
			results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(7 + dbsearchspecificcolumncount)->setText("Right Terminal Modification");
			break;
#endif
		case other:
		default:
			break;
		}
		
		results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(6 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setText("Matched Peaks");

		results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(7 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setText("Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(7 + dbsearchspecificcolumncount + resultsspecificcolumncount, &columndelegate);
				
		results->setHorizontalHeaderItem(8 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(8 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setText("Sum of Relative Intensities");
		results->setItemDelegateForColumn(8 + dbsearchspecificcolumncount + resultsspecificcolumncount, &columndelegate);

		for (int i = 0; i < (int)parameters.fragmentionsfortheoreticalspectra.size(); i++) {
			results->setHorizontalHeaderItem(resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i)->setText(parameters.fragmentdefinitions[(eFragmentIonType)parameters.fragmentionsfortheoreticalspectra[i]].name.c_str());
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			results->setHorizontalHeaderItem(results->columnCount() - 1, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(results->columnCount() - 1)->setText("Scrambled Peaks");
		}

	}


	if (parameters.mode == dereplication) {
		if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			results->setColumnCount(7);
		}
		else {
			results->setColumnCount(5);
		}

		results->setHorizontalHeaderItem(0, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(0)->setText("*");

		results->setHorizontalHeaderItem(1, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(1)->setText("Spectrum ID");

		results->setHorizontalHeaderItem(2, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(2)->setText("Matched Peaks");

		results->setHorizontalHeaderItem(3, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(3)->setText("Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(3, &columndelegate);

		results->setHorizontalHeaderItem(4, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(4)->setText("Sum of Relative Intensities");
		results->setItemDelegateForColumn(4, &columndelegate);

		if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			results->setHorizontalHeaderItem(5, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(5)->setText("Coordinate X");

			results->setHorizontalHeaderItem(6, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6)->setText("Coordinate Y");
		}
	}


	results->resizeColumnsToContents();

	resultsheadersort.resize(results->columnCount());
	for (int i = 0; i < results->columnCount(); i++) {
		resultsheadersort[i] = -1;
	}

	QProgressDialog progress("Preparing the report...", /*"Cancel"*/0, 0, theoreticalspectrumlist.size(), this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	spectradetails.clear();
	spectradetails.resize(theoreticalspectrumlist.size());
	bool reportisomers = actionShowIsomers->isChecked();
	for (int i = 0; i < theoreticalspectrumlist.size(); i++) {
		reportSpectrum(i, theoreticalspectrumlist[i], reportisomers);
		progress.setValue(i);
		//if (progress.wasCanceled()) {
		//	break;
		//}
	}

	for (int i = 0; i < results->columnCount(); i++) {
		results->resizeColumnToContents(i);
	}

	progress.setValue(theoreticalspectrumlist.size());

}


void cMainWindow::updateSpectra() {

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {

		bool reportisomers = actionShowIsomers->isChecked();
		string peptidesequence;

		QProgressDialog progress("Updating the report...", 0, 0, theoreticalspectrumlist.size(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		for (int i = 0; i < results->rowCount(); i++) {
			preparePeptideSequence(i, peptidesequence, reportisomers);
			results->item(i, 2 + dbsearchspecificcolumncount)->setText(peptidesequence.c_str());

			progress.setValue(i);
		}

		for (int i = 0; i < results->columnCount(); i++) {
			results->resizeColumnToContents(i);
		}

		progress.setValue(theoreticalspectrumlist.size());

	}

}


void cMainWindow::deleteResults() {
	widgetitemallocator.reset();
	results->setColumnCount(0);
	results->setRowCount(0);
}


void cMainWindow::resultsCellClicked(int row, int column) {
	spectradetails[results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1].prepareToShow(parameters.peptidetype);
	spectradetails[results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1].show();
	spectradetails[results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1].activateWindow();
}


void cMainWindow::setGraph(string s) {
	graph->setHTML(s);
}


void cMainWindow::headerItemDoubleClicked(int index) {
	if (resultsheadersort[index] == -1) {
		results->sortByColumn(index, Qt::DescendingOrder);
		resultsheadersort[index] = 0;
		return;
	}

	if (resultsheadersort[index] == 0) {
		results->sortByColumn(index, Qt::AscendingOrder);
		resultsheadersort[index] = 1;
	}
	else {
		results->sortByColumn(index, Qt::DescendingOrder);
		resultsheadersort[index] = 0;
	}
}


void cMainWindow::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));
	
	if (!filename.isEmpty()) {
		lastdirexporttocsv = filename;

		QProgressDialog progress("Exporting CSV file...", "Cancel", 0, results->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);
		
		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		for (int i = 0; i < results->columnCount(); i++) {
			out << "\"" << results->horizontalHeaderItem(i)->text() << "\"";
			if (i < results->columnCount() - 1) {
				out << ",";
			}
		}
		out << endl;

		for (int i = 0; i < results->rowCount(); i++) {

			if (results->isRowHidden(i)) {
				continue;
			}

			for (int j = 0; j < results->columnCount(); j++) {
				out << "\"" << results->item(i, j)->data(Qt::DisplayRole).toString() << "\"";
				if (j < results->columnCount() - 1) {
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

		progress.setValue(results->rowCount());
	}
}


void cMainWindow::exportToHTML() {

	if (htmlexportdialog->exec() != QDialog::Accepted) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Export to HTML"), lastdirexporttohtml, tr("HTML Files (*.htm *.html)"));
	
	if (!filename.isEmpty()) {			
		lastdirexporttohtml = filename;

		QProgressDialog progress("Exporting HTML report...", "Cancel", 0, results->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		bool removefile = false;
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			return;
		}

		QTextStream out(&file);

		if (results->rowCount() > 0) {

			bool matchedrow;
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

			out << "<h1>" << QString(title.c_str()) << "</h1>\n";

			if (htmlexportdialog->checkboxparameters->isChecked()) {	
				out << "<h2>Parameters</h2>\n";
				out << "<p><pre>\n" << parameters.printToString().c_str() << "</pre></p>\n";
				out << "<hr size=\"1\"/>\n";
			}

			if (htmlexportdialog->checkboxdenovo->isChecked() && (parameters.mode == denovoengine)) {
				out << "<h2>De Novo Graph</h2>\n";
				out << "<p><font face=\"Courier\">\n" << graph->getHTML().c_str() << "</font></p>\n";
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
				for (int i = 0; i < results->columnCount(); i++) {
					out << "<th><b>" << results->horizontalHeaderItem(i)->text() << "</b></th>\n";
				}
				out << "</tr></thead><tbody>\n";

				for (int i = 0; i < results->rowCount(); i++) {

					if (results->isRowHidden(i)) {
						continue;
					}

					out << "<tr>\n";
			
					matchedrow = false;
					if (results->item(i, 0)->data(Qt::DisplayRole).toString().compare("*") == 0) {
						matchedrow = true;
					}

					for (int j = 0; j < results->columnCount(); j++) {
						out << "<td";
						if (matchedrow) {
							out << " bgcolor=\"yellow\"";
						}
						out << ">" << results->item(i, j)->data(Qt::DisplayRole).toString() << "</td>\n";
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
				if (parameters.mode == dereplication) {
					if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
						columncount = 11;
					}
					else {
						columncount = 9;
					}
				}
				else {
					columncount = 7;
				}

				tdwidth = to_string(100/columncount);

				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>ID</b></th>";

				if (parameters.mode == dereplication) {
					if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Coordinate X</b></th>";
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Coordinate Y</b></th>";
					}
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Ion Type</b></th>";
				}
				else {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Fragment Type</b></th>";
				}

				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Theoretical m/z</b></th>";
				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Experimental m/z</b></th>";
				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Intensity [%]</b></th>";
				out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Error [ppm]</b></th>";

				if (parameters.mode == dereplication) {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Summary Formula</b></th>";
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Name</b></th>";
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Reference</b></th>";
				}
				else {
					out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Sequence</b></th>";
				}

				out << "</tr></thead><tbody>";

				int spectrumindex;
				for (int i = 0; i < results->rowCount(); i++) {

					if (results->isRowHidden(i)) {
						continue;
					}

					spectrumindex = results->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

					if (spectradetails[spectrumindex].getTheoreticalSpectrum().getNumberOfMatchedPeaks() == 0) {
						continue;
					}

					out << spectradetails[spectrumindex].getPartialPeaksTableAsHTMLString(spectrumindex).c_str();
				
				}

				out << "</tbody></table></p>";
				out << "<br/><hr size=\"1\"/>\n";

			}

			if (htmlexportdialog->checkboxdetails->isChecked() || htmlexportdialog->checkboxpeakstable->isChecked()) {
				out << "<h2>Individual Rows in Output Report Table</h2>\n";

				int spectrumindex;
				for (int i = 0; i < results->rowCount(); i++) {

					if (results->isRowHidden(i)) {
						continue;
					}

					spectrumindex = results->item(i, 1)->data(Qt::DisplayRole).toInt() - 1;

					if ((spectradetails[spectrumindex].getTheoreticalSpectrum().getNumberOfMatchedPeaks() == 0) && !htmlexportdialog->checkboxunmatchedtheoretical->isChecked() && !htmlexportdialog->checkboxunmatchedexperimental->isChecked()) {
						continue;
					}

					out << "<h3>ID: ";
					out << spectrumindex + 1;
					out << "</h3>\n";

					if (htmlexportdialog->checkboxpeakstable->isChecked()) {
						out << "<p align=\"center\">";
						out << spectradetails[spectrumindex].getPeaksTableAsHTMLString(htmlexportdialog->checkboxunmatchedtheoretical->isChecked(), htmlexportdialog->checkboxunmatchedexperimental->isChecked()).c_str();
						out << "</p>\n";
					}

					if (htmlexportdialog->checkboxdetails->isChecked()) {
						out << "<font face=\"Courier\">";
						out << "<p>";
						out << spectradetails[spectrumindex].getDetailsAsHTMLString().c_str();
						out << spectradetails[spectrumindex].getTheoreticalSpectrum().getCoverageBySeries().c_str();
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

		progress.setValue(results->rowCount());

	}

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

		QProgressDialog progress("Saving the report...", "Cancel", 0, theoreticalspectrumlist.size(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);
		
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

			// store theoretical spectra
			size = theoreticalspectrumlist.size();
			outfile.write((char *)&size, sizeof(int));
			for (int i = 0; i < theoreticalspectrumlist.size(); i++) {
				theoreticalspectrumlist[i].store(outfile);
				progress.setValue(i);
				if (progress.wasCanceled()) {
					removefile = true;
					break;
				}
			}

			// close file
			outfile.close();
			
			if (removefile) {
				QFile::remove(filename);
			}
		}
	
		progress.setValue(theoreticalspectrumlist.size());

	}

}


void cMainWindow::openResultsFile() {
	
	#if OS_TYPE == WIN
		string filetypes = appname.toStdString() + " " + appversion.toStdString() + " Results Files (*.res)";
	#else
		string filetypes = "Results Files (*.res)";
	#endif
	
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
			QString appinfo = appname + " " + appversion;
			if (appinfo.toStdString().compare(s) != 0) {
				QMessageBox msgBox;
				QString errstr = "The results file cannot be read because it was created by different version ";
				errstr += s.c_str();
				errstr += ".";
				msgBox.setText(errstr);
				msgBox.exec();

				infile.close();
				return;
			}

			deleteResults();

			summarytableofmatchedpeaks->deleteTable(true);
			summarytableofmatchedpeaks->hide();
			summarytableisprepared = false;
			
			theoreticalspectrumlist.clear();
			spectradetails.clear();
			
			// load graph window
			graph->load(infile);

			// load log window
			infile.read((char *)&size, sizeof(int));
			s.resize(size);
			infile.read(&s[0], s.size());
			logWindow->setPlainText(s.c_str());
			QScrollBar *sb = logWindow->verticalScrollBar();
			sb->setValue(sb->maximum());

			// load parameters
			parameters.load(infile);
			parameterswidget->setAndRestoreParameters(parameters);

			if (parameters.mode == dereplication) {
				actionShowIsomers->setEnabled(false);
			}
			else {
				actionShowIsomers->setEnabled(true);
			}

			// load theoretical spectra
			infile.read((char *)&size, sizeof(int));

			QProgressDialog progress("Loading the report...", 0, 0, size, this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			for (int i = 0; i < size; i++) {
				theoreticalspectrum.load(infile);
				theoreticalspectrumlist.add(theoreticalspectrum);
				progress.setValue(i);
			}

			progress.setValue(size);

			reportSpectra();

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
	sequencedatabasewidget->insertRow(peptidetypeindex, sequence);
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
	summarytableofmatchedpeaks->deleteTable(true);
	summarytableofmatchedpeaks->hide();
	summarytableisprepared = false;

	Qt::CaseSensitivity casesensitive = rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
	QString str = rowsfilterline->text();
	int rowcount = results->rowCount();
	bool match;
	int i, j;

	QProgressDialog progress("Updating the report...", "Cancel", 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (i = 0; i < rowcount; i++) {
		match = false;
		for (j = 0; j < results->columnCount(); j++) {
			if (results->item(i, j)->text().contains(str, casesensitive)) {
				match = true;
				break;
			}
		}
		results->setRowHidden(i, !match);
		progress.setValue(i);

		if (progress.wasCanceled()) {
			resetFilter();
			break;
		}

	}

	progress.setValue(rowcount);
}


void cMainWindow::resetFilter() {
	summarytableofmatchedpeaks->deleteTable(true);
	summarytableofmatchedpeaks->hide();
	summarytableisprepared = false;

	rowsfilterline->setText("");
	int rowcount = results->rowCount();

	QProgressDialog progress("Updating the report...", 0, 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < rowcount; i++) {
		results->setRowHidden(i, false);
		progress.setValue(i);
	}

	progress.setValue(rowcount);
}


void cMainWindow::gotoNorine() {
	QDesktopServices::openUrl(QUrl("http://bioinfo.lifl.fr/norine/"));
}


void cMainWindow::gotoSmiles2Monomers() {
	QDesktopServices::openUrl(QUrl("http://bioinfo.lifl.fr/norine/smiles2monomers.jsp"));
}


void cMainWindow::summaryPeaksTableCancelled() {
	summarytableisprepared = false;
}

/*
void cMainWindow::showContextMenu(const QPoint &pt) {
    QMenu *menu = logWindow->createStandardContextMenu();
    menu->addAction(clear);
	menu->exec(logWindow->mapToGlobal(pt));
    delete menu;
}
*/

