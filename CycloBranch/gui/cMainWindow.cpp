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


cMainWindow::cMainWindow() {
	setWindowTitle(appname);
	setWindowIcon(QIcon(":/images/cb.png"));

	menuBar = new QMenuBar(this);
	menuBar->setNativeMenuBar(false);
	
	menuFile = new QMenu(tr("&File"), this);
	menuSearch = new QMenu(tr("&Search"), this);
	menuTools = new QMenu(tr("&Tools"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);

	actionOpenResults = new QAction(QIcon(":/images/icons/52.png"), tr("&Open Results..."), this);
	actionOpenResults->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

	actionSaveResults = new QAction(QIcon(":/images/icons/22.png"), tr("&Save Results..."), this);
	actionSaveResults->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

	actionExportToCsv = new QAction(QIcon(":/images/icons/62.png"), tr("Export to &CSV"), this);
	actionExportToCsv->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C));
	
	actionExportToHTML = new QAction(QIcon(":/images/icons/77.png"), tr("Export to &HTML"), this);
	actionExportToHTML->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_H));

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

	actionSummaryTableOfMatchedPeaks = new QAction(QIcon(":/images/icons/43.png"), tr("S&ummary Table of Matched Peaks"), this);
	actionSummaryTableOfMatchedPeaks->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_U));

	actionImageWindow = new QAction(QIcon(":/images/icons/23.png"), tr("&CrossVis"), this);
	actionImageWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
	actionImageWindow->setDisabled(true);

	actionChromatogramWindow = new QAction(QIcon(":/images/icons/chromatography.png"), tr("C&hromatogram"), this);
	actionChromatogramWindow->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));
	actionChromatogramWindow->setDisabled(true);

	actionNorine = new QAction(QIcon(":/images/icons/25.png"), tr("&Norine"), this);
	actionNorine->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));

	actionSmilesToMonomers = new QAction(QIcon(":/images/icons/5.png"), tr("Smiles2Monome&rs"), this);
	actionSmilesToMonomers->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));

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
	toolbarTools->addAction(actionChromatogramWindow);
	toolbarTools->addAction(actionImageWindow);
	toolbarTools->addSeparator();
	toolbarTools->addAction(actionNorine);
	toolbarTools->addAction(actionSmilesToMonomers);

	toolbarView = addToolBar(tr("View"));
	toolbarView->addAction(actionShowIsomers);
	toolbarView->addAction(actionGraph);
	toolbarView->addAction(actionLog);

	toolbarHelp = addToolBar(tr("Help"));
	toolbarHelp->addAction(actionHTMLDocumentation);
	toolbarHelp->addAction(actionPDFManual);
	toolbarHelp->addAction(actionAbout);

	rowsfiltercombobox = new QComboBox();
	rowsfiltercombobox->setToolTip("Column to be Searched");
	rowsfiltercombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

	rowsfiltercomparatorcombobox = new QComboBox();
	rowsfiltercomparatorcombobox->setToolTip("Type of Comparison");
	rowsfiltercomparatorcombobox->addItem("=");
	rowsfiltercomparatorcombobox->addItem("<");
	rowsfiltercomparatorcombobox->addItem("<=");
	rowsfiltercomparatorcombobox->addItem(">");
	rowsfiltercomparatorcombobox->addItem(">=");
	rowsfiltercomparatorcombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	
	rowsfilterline = new QLineEdit();
	rowsfilterline->setMinimumWidth(300);
	rowsfilterline->setToolTip("Text to Find");

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
	rowsfilterhbox->addWidget(rowsfiltercombobox);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfiltercomparatorcombobox);
	rowsfilterhbox->addSpacing(10);
	rowsfilterhbox->addWidget(rowsfilterline);
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
	toolbarFilter = addToolBar(tr("Filter"));
	toolbarFilter->addWidget(rowsfilterwidget);

	results = new QTableView(this);
	resultsmodel = new QStandardItemModel(0, 0, this);
	resultsproxymodel = new cMainWindowProxyModel(this);
	resultsproxymodel->initialize(rowsfiltercombobox, rowsfiltercomparatorcombobox);

	logWindow = new QTextEdit(this);
	logWindow->setPlainText("To start the program: click \"Search -> Settings\", configure the tool and then click \"Search -> Run\".");
	splitter = new QSplitter(this);

	about = new cAboutWidget(this);

	graph = new cGraphWidget();
	bricksdatabasewidget = new cBricksDatabaseWidget();
	sequencedatabasewidget = new cSequenceDatabaseWidget(this);
	modificationswidget = new cModificationsWidget();
	drawpeptidewidget = new cDrawPeptideWidget(this);
	summarytableofmatchedpeaks = new cSummaryPeaksTableWidget(this);
	imagewindow = new cImageWindow(this);
	chromatogramwindow = new cChromatogramWindow(theoreticalspectrumlist, this);
	parameterswidget = new cParametersWidget(this);
	htmlexportdialog = new cHTMLExportDialog(this);
	
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
	connect(actionShowIsomers, SIGNAL(triggered()), this, SLOT(showIsomersStateChanged()));
	connect(actionGraph, SIGNAL(triggered()), this, SLOT(showGraph()));
	connect(actionSummaryTableOfMatchedPeaks, SIGNAL(triggered()), this, SLOT(showSummaryTableOfMatchedPeaks()));
	connect(actionImageWindow, SIGNAL(triggered()), this, SLOT(showImageWindow()));
	connect(actionChromatogramWindow, SIGNAL(triggered()), this, SLOT(showChromatogramWindow()));
	connect(actionLog, SIGNAL(triggered()), this, SLOT(showHideLog()));
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));
	connect(actionPDFManual, SIGNAL(triggered()), this, SLOT(showPDFManual()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterResults()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	connect(summarytableofmatchedpeaks, SIGNAL(tableCancelled()), this, SLOT(summaryPeaksTableCancelled()));
	connect(summarytableofmatchedpeaks, SIGNAL(summaryPeaksTableRowDoubleClicked(int)), this, SLOT(summaryPeaksTableRowDoubleClicked(int)));
	connect(summarytableofmatchedpeaks, SIGNAL(sendFilterOptionsToImageWindow(vector<cCoordinates>, string, string, string, bool, bool)), imagewindow, SLOT(setFilterOptionsSlot(vector<cCoordinates>, string, string, string, bool, bool)));
	connect(summarytableofmatchedpeaks, SIGNAL(sendFilterOptionsToChromatogram(cPeaksList)), chromatogramwindow, SLOT(setFilterOptionsSlot(cPeaksList)));
	connect(summarytableofmatchedpeaks, SIGNAL(resetRegion()), imagewindow, SLOT(clearSelection()));

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
	menuTools->addAction(actionSummaryTableOfMatchedPeaks);
	menuTools->addAction(actionChromatogramWindow);
	menuTools->addAction(actionImageWindow);
	menuTools->addSeparator();
	menuTools->addAction(actionNorine);
	menuTools->addAction(actionSmilesToMonomers);

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

	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(results);
	splitter->addWidget(logWindow);
	splitter->setStretchFactor(0, 1);

	setCentralWidget(splitter);

	resize(1280, 780);

	resultsbasecolumncount = 9;
	resultsspecificcolumncount = 0;
	searchspecificcolumncount = 0;

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

	delete rowsfiltercombobox;
	delete rowsfiltercomparatorcombobox;
	delete rowsfilterline;
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
	delete imagewindow;
	delete chromatogramwindow;
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
	delete actionChromatogramWindow;
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
			if (results->selectionModel()->selectedIndexes().count() > 0) {
				rowDoubleClicked(results->selectionModel()->selectedIndexes()[0]);
			}
		}
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_T)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_W)) {
		rowsfilterwholeword->setChecked(!rowsfilterwholeword->isChecked());
	}

	event->accept();
}


void cMainWindow::closeEvent(QCloseEvent *event) {
	quitApplication();
	event->ignore();
}


void cMainWindow::preparePeptideSequence(int row, string& peptidesequence, bool reportisomers) {
	int spectrumindex = resultsmodel->item(row, 1)->data(Qt::DisplayRole).toInt() - 1;
	peptidesequence = theoreticalspectrumlist[spectrumindex].getCandidate().getAcronymPeptideNameWithHTMLReferences();
	peptidesequence = stripHTML(peptidesequence);

	if (!reportisomers) {
		stripIsomers(peptidesequence);
	}
}


void cMainWindow::reportSpectrum(int row, cTheoreticalSpectrum& theoreticalspectrum, bool reportisomers) {
	string peptidesequence;
	cSummaryFormula formula;

	resultsmodel->setItem(row, 0, new QStandardItem());
	resultsmodel->item(row, 0)->setText("");
	
	resultsmodel->setItem(row, 1, new QStandardItem());
	resultsmodel->item(row, 1)->setData(QVariant::fromValue(row + 1), Qt::DisplayRole);

	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {

		if (parameters.mode == denovoengine) {
			resultsmodel->setItem(row, 2, new QStandardItem());
			resultsmodel->item(row, 2)->setData(QVariant::fromValue(theoreticalspectrum.getPathId() + 1), Qt::DisplayRole);
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

		switch (parameters.peptidetype)
		{
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
		default:
			break;
		}

		resultsmodel->setItem(row, 6 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 6 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfMatchedPeaks()), Qt::DisplayRole);

		resultsmodel->setItem(row, 7 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 7 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getRatioOfMatchedPeaks()*100)), Qt::DisplayRole);

		resultsmodel->setItem(row, 8 + searchspecificcolumncount + resultsspecificcolumncount, new QStandardItem());
		resultsmodel->item(row, 8 + searchspecificcolumncount + resultsspecificcolumncount)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getSumOfRelativeIntensities())), Qt::DisplayRole);

		int index = resultsbasecolumncount + searchspecificcolumncount + resultsspecificcolumncount;
		for (int i = 0; i < (int)parameters.ionsfortheoreticalspectra.size(); i++) {
			for (int j = -1; j < (int)parameters.neutrallossesfortheoreticalspectra.size(); j++) {
				resultsmodel->setItem(row, index, new QStandardItem());
				resultsmodel->item(row, index)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfMatchedPeaks(parameters.ionsfortheoreticalspectra[i], (j == -1) ? -1 : parameters.neutrallossesfortheoreticalspectra[j])), Qt::DisplayRole);

				index++;
			}
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			resultsmodel->setItem(row, resultsmodel->columnCount() - 1, new QStandardItem());
			resultsmodel->item(row, resultsmodel->columnCount() - 1)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfScrambledPeaks()), Qt::DisplayRole);
		}

		if (theoreticalspectrum.isValid()) {
			resultsmodel->item(row, 0)->setText("*");
			for (int i = 0; i < resultsmodel->columnCount(); i++) {
				resultsmodel->item(row, i)->setBackground(Qt::yellow);
			}
		}

	}


	if (parameters.mode == dereplication) {
		resultsmodel->setItem(row, 2, new QStandardItem());
		resultsmodel->item(row, 2)->setText(theoreticalspectrum.getExperimentalSpectrum().getTitle().c_str());

		resultsmodel->setItem(row, 3, new QStandardItem());
		resultsmodel->item(row, 3)->setData(QVariant::fromValue(theoreticalspectrum.getNumberOfMatchedPeaks()), Qt::DisplayRole);

		resultsmodel->setItem(row, 4, new QStandardItem());
		resultsmodel->item(row, 4)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getRatioOfMatchedPeaks()*100)), Qt::DisplayRole);

		resultsmodel->setItem(row, 5, new QStandardItem());
		resultsmodel->item(row, 5)->setData(QVariant::fromValue(cropPrecisionToSixDecimalsByteArray(theoreticalspectrum.getSumOfRelativeIntensities())), Qt::DisplayRole);

		if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			resultsmodel->setItem(row, 6, new QStandardItem());
			resultsmodel->item(row, 6)->setData(QVariant::fromValue(theoreticalspectrum.getExperimentalSpectrum().getCoordinateX()), Qt::DisplayRole);	

			resultsmodel->setItem(row, 7, new QStandardItem());
			resultsmodel->item(row, 7)->setData(QVariant::fromValue(theoreticalspectrum.getExperimentalSpectrum().getCoordinateY()), Qt::DisplayRole);		
		}
	}


	spectradetails[row].initialize(&parameters, theoreticalspectrum, this);
	if (parameters.mode == dereplication) {
		spectradetails[row].setWindowTitle(("Experimental Spectrum No. " + to_string(row + 1)).c_str());
	}
	else {
		spectradetails[row].setWindowTitle(("Theoretical Spectrum No. " + to_string(row + 1)).c_str());
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
		summarytableisprepared = summarytableofmatchedpeaks->prepareToShow(resultsmodel, resultsproxymodel, &parameters, &theoreticalspectrumlist, actionShowIsomers->isChecked());
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
		imagewindow->setDefaultMaxXY(parameters.defaultmaxx, parameters.defaultmaxy, parameters.defaultpixelsizex, parameters.defaultpixelsizey, parameters.vendor);
		imagewindow->show();
		imagewindow->activateWindow();
		if (imagewindow->isMinimized()) {
			imagewindow->showNormal();
		}
	}
}


void cMainWindow::showChromatogramWindow() {
	if (prepareSummaryTableOfMatchedPeaks()) {
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


void cMainWindow::run() {
	enableRunButtonAndSettings(false);
	enableStopButton(true);
	enableButtonsHandlingResults(false);

	logWindow->clear();
	
	resetFilter();

	deleteResults();
	
	theoreticalspectrumlist.clear();
	spectradetails.clear();

	cParameters localparameters = parameterswidget->getParameters();

	regex rx;
	rx = "\\.[iI][mM][zZ][mM][lL]$";
	if (regex_search(localparameters.peaklistfilename, rx)) {
		string convertedimzml = localparameters.peaklistfilename.substr(0, (int)localparameters.peaklistfilename.size() - 6);
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
			s += localparameters.peaklistfilename.substr(localparameters.peaklistfilename.rfind('/') + 1) + "' ?";
			s += "\n\nClick 'Yes' to proceed the converted file.\nClick 'No' to convert the original file again. Note: The conversion may be time-consuming.";

			reply = QMessageBox::question(this, "Do you want to proceed the converted file ?", s.c_str(), QMessageBox::Yes|QMessageBox::No);

			if (reply == QMessageBox::Yes) {
				localparameters.peaklistfilename = convertedimzml;
			}
		}
	}

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

	cMainThread* thread = new cMainThread(localparameters, theoreticalspectrumlist, true, false);
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
	summarytableisprepared = false;
	imagewindow->hide();
	chromatogramwindow->hide();

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
	
	if (enable) {
		if (parameters.mode == dereplication) {
			actionShowIsomers->setEnabled(false);
			if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
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

	switch (parameters.peptidetype)
	{
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
	default:
		break;
	}


	if ((parameters.mode == denovoengine) || (parameters.mode == databasesearch)) {
		searchspecificcolumncount = 1;
	}
	else {
		searchspecificcolumncount = 0;
	}


	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) { 

		resultsmodel->setColumnCount(resultsbasecolumncount + searchspecificcolumncount + resultsspecificcolumncount + ((int)parameters.ionsfortheoreticalspectra.size() * ((int)parameters.neutrallossesfortheoreticalspectra.size() + 1)));

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			resultsmodel->setColumnCount(resultsmodel->columnCount() + 1);
		}

		resultsmodel->setHorizontalHeaderItem(0, new QStandardItem());
		resultsmodel->horizontalHeaderItem(0)->setText("*");
		results->setItemDelegateForColumn(0, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(1, new QStandardItem());
		resultsmodel->horizontalHeaderItem(1)->setText("Result ID");
		results->setItemDelegateForColumn(1, new QItemDelegate());

		if (parameters.mode == denovoengine) {
			resultsmodel->setHorizontalHeaderItem(2, new QStandardItem());
			resultsmodel->horizontalHeaderItem(2)->setText("Group ID");
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

		switch (parameters.peptidetype)
		{
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

		string name;
		int index = resultsbasecolumncount + searchspecificcolumncount + resultsspecificcolumncount;
		for (int i = 0; i < (int)parameters.ionsfortheoreticalspectra.size(); i++) {
			for (int j = -1; j < (int)parameters.neutrallossesfortheoreticalspectra.size(); j++) {
				resultsmodel->setHorizontalHeaderItem(index, new QStandardItem());
				name = parameters.iondefinitions[(eFragmentIonType)parameters.ionsfortheoreticalspectra[i]].name;
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


	if (parameters.mode == dereplication) {
		if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			resultsmodel->setColumnCount(8);
		}
		else {
			resultsmodel->setColumnCount(6);
		}

		resultsmodel->setHorizontalHeaderItem(0, new QStandardItem());
		resultsmodel->horizontalHeaderItem(0)->setText("*");
		results->setItemDelegateForColumn(0, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(1, new QStandardItem());
		resultsmodel->horizontalHeaderItem(1)->setText("Spectrum ID");
		results->setItemDelegateForColumn(1, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(2, new QStandardItem());
		resultsmodel->horizontalHeaderItem(2)->setText("Title");
		results->setItemDelegateForColumn(2, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(3, new QStandardItem());
		resultsmodel->horizontalHeaderItem(3)->setText("Matched Peaks");
		results->setItemDelegateForColumn(3, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(4, new QStandardItem());
		resultsmodel->horizontalHeaderItem(4)->setText("Ratio of Matched Peaks [%]");
		results->setItemDelegateForColumn(4, new QItemDelegate());

		resultsmodel->setHorizontalHeaderItem(5, new QStandardItem());
		resultsmodel->horizontalHeaderItem(5)->setText("Sum of Relative Intensities");
		results->setItemDelegateForColumn(5, new QItemDelegate());

		if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
			resultsmodel->setHorizontalHeaderItem(6, new QStandardItem());
			resultsmodel->horizontalHeaderItem(6)->setText("Coordinate X");
			results->setItemDelegateForColumn(6, new QItemDelegate());

			resultsmodel->setHorizontalHeaderItem(7, new QStandardItem());
			resultsmodel->horizontalHeaderItem(7)->setText("Coordinate Y");
			results->setItemDelegateForColumn(7, new QItemDelegate());			
		}
	}

	rowsfiltercombobox->clear();
	for (int i = 0; i < resultsmodel->columnCount(); i++) {
		rowsfiltercombobox->addItem(resultsmodel->horizontalHeaderItem(i)->text());
	}

	results->resizeColumnsToContents();

	QProgressDialog progress("Preparing the report...", /*"Cancel"*/0, 0, theoreticalspectrumlist.size(), this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::ApplicationModal);

	results->setModel(0);
	resultsproxymodel->setSourceModel(0);
	results->setSortingEnabled(false);
	
	spectradetails.clear();
	spectradetails.resize(theoreticalspectrumlist.size());
	resultsmodel->setRowCount(theoreticalspectrumlist.size());
	bool reportisomers = actionShowIsomers->isChecked();
	for (int i = 0; i < theoreticalspectrumlist.size(); i++) {
		reportSpectrum(i, theoreticalspectrumlist[i], reportisomers);
		progress.setValue(i);
		//if (progress.wasCanceled()) {
		//	break;
		//}
	}

	resultsproxymodel->setSourceModel(resultsmodel);
	results->setModel(resultsproxymodel);
	results->setSortingEnabled(true);
	resultsproxymodel->sort(-1);

	for (int i = 0; i < resultsmodel->columnCount(); i++) {
		results->resizeColumnToContents(i);
	}

	progress.setValue(theoreticalspectrumlist.size());

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

		QProgressDialog progress("Updating the report...", 0, 0, theoreticalspectrumlist.size(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::ApplicationModal);

		for (int i = 0; i < resultsmodel->rowCount(); i++) {
			preparePeptideSequence(i, peptidesequence, reportisomers);
			resultsmodel->item(i, 2 + searchspecificcolumncount)->setText(peptidesequence.c_str());

			progress.setValue(i);
		}

		for (int i = 0; i < resultsmodel->columnCount(); i++) {
			results->resizeColumnToContents(i);
		}

		graph->updateView(reportisomers);

		progress.setValue(theoreticalspectrumlist.size());

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


void cMainWindow::rowDoubleClicked(const QModelIndex& item) {
	int row = resultsproxymodel->mapToSource(item).row();
	int rowid = resultsmodel->item(row, 1)->data(Qt::DisplayRole).toInt() - 1;

	spectradetails[rowid].prepareToShow(parameters.peptidetype, actionShowIsomers);
	spectradetails[rowid].show();
	spectradetails[rowid].activateWindow();
	if (spectradetails[rowid].isMinimized()) {
		spectradetails[rowid].showNormal();
	}
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

			out << "<h1><a href=\"http://ms.biomed.cas.cz/cyclobranch/\">" << QString(title.c_str()) << "</a></h1>\n";

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
				if (parameters.mode == dereplication) {
					if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
						if (parameters.generateisotopepattern) {
							columncount = 15;
						}
						else {
							columncount = 12;
						}
					}
					else {
						if (parameters.generateisotopepattern) {
							columncount = 13;
						}
						else {
							columncount = 10;
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

				if (parameters.mode == dereplication) {
					if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Coordinate X</b></th>";
						out << "<th width=\"" << tdwidth.c_str() << "%\"><b>Coordinate Y</b></th>";
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

				if (parameters.mode != dereplication) {
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

				if (parameters.mode == dereplication) {
					if (parameters.generateisotopepattern) {
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
		progress.setWindowModality(Qt::ApplicationModal);
		
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
				QString errstr = "The results file cannot be opened because it was created in the different version of " + appname + " (";
				errstr += s.c_str();
				errstr += ").";
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
			
			theoreticalspectrumlist.clear();
			spectradetails.clear();
			
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
			parameters.load(infile);
			parameterswidget->setAndRestoreParameters(parameters);

			if (parameters.mode == dereplication) {
				actionShowIsomers->setEnabled(false);
				if ((parameters.peaklistfileformat == mis) || (parameters.peaklistfileformat == imzML)) {
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

			// load theoretical spectra
			infile.read((char *)&size, sizeof(int));

			QProgressDialog progress("Loading the report...", 0, 0, size, this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::ApplicationModal);

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
	resultsproxymodel->setFilterFixedString(rowsfilterline->text());

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
	rowsfilterline->setText("");

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


void cMainWindow::gotoNorine() {
	QDesktopServices::openUrl(QUrl("http://bioinfo.lifl.fr/norine/"));
}


void cMainWindow::gotoSmiles2Monomers() {
	QDesktopServices::openUrl(QUrl("http://bioinfo.lifl.fr/norine/smiles2monomers.jsp"));
}


void cMainWindow::summaryPeaksTableCancelled() {
	summarytableisprepared = false;
}


void cMainWindow::summaryPeaksTableRowDoubleClicked(int rowid) {
	spectradetails[rowid].prepareToShow(parameters.peptidetype, actionShowIsomers);
	spectradetails[rowid].show();
	spectradetails[rowid].activateWindow();
	if (spectradetails[rowid].isMinimized()) {
		spectradetails[rowid].showNormal();
	}
}


void cMainWindow::updateSummaryPeaksTableFilterSlot(int xmin, int xmax, int ymin, int ymax) {
	summarytableofmatchedpeaks->updateFilterBySelectedRegion(xmin, xmax, ymin, ymax);
}


/*
void cMainWindow::showContextMenu(const QPoint &pt) {
    QMenu *menu = logWindow->createStandardContextMenu();
    menu->addAction(clear);
	menu->exec(logWindow->mapToGlobal(pt));
    delete menu;
}
*/

