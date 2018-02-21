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


	actionGraph = new QAction(QIcon(":/images/icons/32.png"), tr("De Novo &Graph"), this);
	actionGraph->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));

	actionLog = new QAction(QIcon(":/images/icons/2.png"), tr("&Log Window"), this);
	actionLog->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));


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

	toolbarView = addToolBar(tr("View"));
	toolbarView->addAction(actionGraph);
	toolbarView->addAction(actionLog);

	toolbarHelp = addToolBar(tr("Help"));
	toolbarHelp->addAction(actionHTMLDocumentation);
	toolbarHelp->addAction(actionPDFManual);
	toolbarHelp->addAction(actionAbout);


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
	parameterswidget = new cParametersWidget(this);
	
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
	connect(actionGraph, SIGNAL(triggered()), this, SLOT(showGraph()));
	connect(actionLog, SIGNAL(triggered()), this, SLOT(showHideLog()));
	connect(actionHTMLDocumentation, SIGNAL(triggered()), this, SLOT(showHTMLDocumentation()));
	connect(actionPDFManual, SIGNAL(triggered()), this, SLOT(showPDFManual()));
	connect(actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));

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
	menuView->addAction(actionGraph);
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
	parameterswidget->hide();

	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(results);
	splitter->addWidget(logWindow);
	splitter->setStretchFactor(0, 1);

	// set the central widget
	setCentralWidget(splitter);

	// set the size of main window
	resize(1280, 700);

	resultsbasecolumncount = 8;
	resultsspecificcolumncount = 0;
	dbsearchspecificcolumncount = 0;

	theoreticalspectrumlist.clear();
	spectradetails.clear();

	lastdirexporttocsv = "./";
	lastdirexporttohtml = "./";
	lastdirsaveresults = "./";
	lastdiropenresults = "./";

	quitapp = false;
}


void cMainWindow::closeEvent(QCloseEvent *event) {
	quitApplication();
	event->ignore();
}


void cMainWindow::reportSpectrum(int id, cTheoreticalSpectrum& theoreticalspectrum) {
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
		results->item(row, 2 + dbsearchspecificcolumncount)->setText(stripHTML(theoreticalspectrum.getCandidate().getAcronymPeptideNameWithHTMLReferences()).c_str());

		results->setItem(row, 3 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 3 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getCandidate().getSummaryFormula(parameters, parameters.peptidetype).c_str());

		results->setItem(row, 4 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 4 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, getNumberOfBricks(theoreticalspectrum.getCandidate().getComposition()));

		switch (parameters.peptidetype)
		{
		case linear:
			results->setItem(row, 5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 5 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
			break;
		case branched:
			results->setItem(row, 5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 5 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str());
			results->setItem(row, 7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 7 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
			break;
		case cyclic:
			results->setItem(row, 5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 5 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedBricks());	
			break;
		case lasso:
			results->setItem(row, 5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 5 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str());
			break;
		case linearpolysaccharide:
			results->setItem(row, 5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 5 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str());
			results->setItem(row, 6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->item(row, 6 + dbsearchspecificcolumncount)->setText(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str());
			break;
		case other:
		default:
			break;
		}

		results->setItem(row, 5 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 5 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks());

		results->setItem(row, 6 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 6 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getRatioOfMatchedPeaks()*100);

		results->setItem(row, 7 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->item(row, 7 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getWeightedIntensityScore());

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
		results->item(row, 3)->setData(Qt::DisplayRole, theoreticalspectrum.getRatioOfMatchedPeaks()*100);

		results->setItem(row, 4, widgetitemallocator.getNewItem());
		results->item(row, 4)->setData(Qt::DisplayRole, theoreticalspectrum.getWeightedIntensityScore());
	}


	spectradetails[id].initialize(&parameters, theoreticalspectrum);
	spectradetails[id].setWindowTitle(("Theoretical Spectrum No. " + to_string(row+1)).c_str());
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

	cMainThread* thread = new cMainThread(parameterswidget->getParameters(), theoreticalspectrumlist, true, false);
	connect(thread, SIGNAL(message(QString)), this, SLOT(updateLog(QString)));
	connect(thread, SIGNAL(enableRunButtonAndSettings(bool)), this, SLOT(enableRunButtonAndSettings(bool)));
	connect(thread, SIGNAL(enableStopButton(bool)), this, SLOT(enableStopButton(bool)));
	connect(thread, SIGNAL(enableButtonsHandlingResults(bool)), this, SLOT(enableButtonsHandlingResults(bool)));
	connect(thread, SIGNAL(sendParameters(cParameters)), this, SLOT(sendParameters(cParameters)));
	connect(thread, SIGNAL(reportSpectra()), this, SLOT(reportSpectra()));
	connect(thread, SIGNAL(setGraph(string)), this, SLOT(setGraph(string)));
	connect(this, SIGNAL(stopComputation()), thread, SLOT(stopComputation()));

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
		resultsspecificcolumncount = 2;
		break;
	case branched:
		resultsspecificcolumncount = 3;
		break;
	case cyclic:
		resultsspecificcolumncount = 1;
		break;
	case lasso:
		resultsspecificcolumncount = 1;
		break;
	case linearpolysaccharide:
		resultsspecificcolumncount = 2;
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
		results->horizontalHeaderItem(4 + dbsearchspecificcolumncount)->setText("Number of Bricks");

		switch (parameters.peptidetype)
		{
		case linear:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(5 + dbsearchspecificcolumncount)->setText("N-terminal Modification");
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("C-terminal Modification");
			break;
		case branched:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(5 + dbsearchspecificcolumncount)->setText("N-terminal Modification");
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("Branch Modification");
			results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(7 + dbsearchspecificcolumncount)->setText("C-terminal Modification");
			break;
		case cyclic:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(5 + dbsearchspecificcolumncount)->setText("Matched Bricks");
			break;
		case lasso:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(5 + dbsearchspecificcolumncount)->setText("Branch Modification");
			break;
		case linearpolysaccharide:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(5 + dbsearchspecificcolumncount)->setText("N-terminal Modification");
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(6 + dbsearchspecificcolumncount)->setText("C-terminal Modification");
			break;
		case other:
		default:
			break;
		}
		
		results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(5 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setText("Matched Peaks");

		results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(6 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setText("Ratio of Matched Peaks [%]");
				
		results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount + resultsspecificcolumncount, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(7 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setText("Sum of Relative Intensities");

		for (int i = 0; i < (int)parameters.fragmentionsfortheoreticalspectra.size(); i++) {
			results->setHorizontalHeaderItem(resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i)->setText(parameters.fragmentdefinitions[(fragmentIonType)parameters.fragmentionsfortheoreticalspectra[i]].name.c_str());
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			results->setHorizontalHeaderItem(results->columnCount() - 1, widgetitemallocator.getNewItem());
			results->horizontalHeaderItem(results->columnCount() - 1)->setText("Scrambled Peaks");
		}

	}


	if (parameters.mode == dereplication) {
		results->setColumnCount(5);
		results->setHorizontalHeaderItem(0, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(0)->setText("*");
		results->setHorizontalHeaderItem(1, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(1)->setText("Result ID");
		results->setHorizontalHeaderItem(2, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(2)->setText("Matched Peaks");
		results->setHorizontalHeaderItem(3, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(3)->setText("Ratio of Matched Peaks [%]");
		results->setHorizontalHeaderItem(4, widgetitemallocator.getNewItem());
		results->horizontalHeaderItem(4)->setText("Sum of Relative Intensities");
	}


	results->resizeColumnsToContents();

	resultsheadersort.resize(results->columnCount());
	for (int i = 0; i < results->columnCount(); i++) {
		resultsheadersort[i] = -1;
	}


	QProgressDialog progress("Preparing the report...", /*"Cancel"*/0, 0, theoreticalspectrumlist.size(), this);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	spectradetails.clear();
	spectradetails.resize(theoreticalspectrumlist.size());
	for (int i = 0; i < theoreticalspectrumlist.size(); i++) {
		reportSpectrum(i, theoreticalspectrumlist[i]);
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

		QProgressDialog progress("Exporting the CSV file...", /*"Cancel"*/0, 0, results->rowCount(), this);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);
		
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
			for (int j = 0; j < results->columnCount(); j++) {
				out << "\"" << results->item(i, j)->data(Qt::DisplayRole).toString() << "\"";
				if (j < results->columnCount() - 1) {
					out << ",";
				}
			}
			out << endl;

			progress.setValue(i);
			//if (progress.wasCanceled()) {
			//	break;
			//}
		}

		file.close();

		progress.setValue(results->rowCount());
	}

}


void cMainWindow::exportToHTML() {

	QString filename = QFileDialog::getSaveFileName(this, tr("Export to HTML"), lastdirexporttohtml, tr("HTML Files (*.htm *.html)"));
	
	if (!filename.isEmpty()) {
		lastdirexporttohtml = filename;
		
		QProgressDialog progress("Exporting the HTML report...", /*"Cancel"*/0, 0, results->rowCount(), this);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

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
			out << "<style> table.results td { font-family: Courier; font-size: 10pt;} </style>\n";

			out << "<script language=\"javascript\">\n";
			out << "function showHide(id) { if (document.getElementById(id).style.display!='none') { document.getElementById(id).style.display='none'; } else { document.getElementById(id).style.display='table-row'; } return false; }\n";
			out << "</script>";

			out << "</head>\n";
			out << "<body style=\"font-family: Verdana, Arial; font-size: 10pt\">\n";

			out << "<h1>" << QString(title.c_str()) << "</h1>\n";

			out << "<h2>Results</h2>\n";

			out << "<p>Hint: Click on a row to expand details about a peptide sequence candidate.</p>";

			out << "<p><font face=\"Courier\"><table class=\"results\" cellpadding=\"0\" cellspacing=\"0\" border=\"1\" width=\"100%\">\n<tr>\n";
			for (int i = 0; i < results->columnCount(); i++) {
				out << "<td><b>" << results->horizontalHeaderItem(i)->text() << "</b></td>\n";
			}
			out << "</tr>\n";

			for (int i = 0; i < results->rowCount(); i++) {
				out << "<tr onclick=\"return showHide('row";
				out << i;
				out << "');\">\n";
			
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

				out << "<tr style=\"display: none;\" id=\"row";
				out << i;
				out << "\"><td colspan=\"";
				out << results->columnCount();
				out << "\">\n";

				out << "<p style=\"margin: 20px\">" << spectradetails[i].getTheoreticalSpectrum().getCoverageBySeries().c_str();
				out << "<br/>" << spectradetails[i].getDetailsAsHTMLString().c_str() << "</p>\n";

				out << "</td></tr>\n";

				progress.setValue(i);
				//if (progress.wasCanceled()) {
				//	break;
				//}
			}
			out << "</table></font></p>\n";

			out << "<h2>Parameters</h2>\n";

			out << "<p><pre>\n" << parameters.printToString().c_str() << "</pre></p>";

			if (parameters.mode == denovoengine) {

				out << "<h2>De Novo Graph</h2>\n";

				out << "<p><font face=\"Courier\">\n" << graph->getHTML().c_str() << "</font></p>";

			}

			out << "</body>\n";
			out << "</html>\n";

		}
		
		file.close();

		progress.setValue(results->rowCount());

	}

}


void cMainWindow::showHTMLDocumentation() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/userguide.html").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(linuxinstalldir + "docs/html/userguide.html").absoluteFilePath()));
	#endif
}


void cMainWindow::showPDFManual() {
	#if OS_TYPE == WIN
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/refman.pdf").absoluteFilePath()));
	#else
		QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(linuxinstalldir + "docs/refman.pdf").absoluteFilePath()));
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

		QProgressDialog progress("Saving the report...", /*"Cancel"*/0, 0, theoreticalspectrumlist.size(), this);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);
		
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
				//if (progress.wasCanceled()) {
				//	break;
				//}
			}

			// close file
			outfile.close();
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

			// load theoretical spectra
			infile.read((char *)&size, sizeof(int));

			QProgressDialog progress("Loading the report...", /*"Cancel"*/0, 0, size, this);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

			for (int i = 0; i < size; i++) {
				theoreticalspectrum.load(infile);
				theoreticalspectrumlist.add(theoreticalspectrum);
				progress.setValue(i);
				//if (progress.wasCanceled()) {
				//	break;
				//}
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
	emit stopComputation();
	quitapp = true;

	if (actionRun->isEnabled()) {
		qApp->quit();
	}
}


/*
void cMainWindow::showContextMenu(const QPoint &pt) {
    QMenu *menu = logWindow->createStandardContextMenu();
    menu->addAction(clear);
	menu->exec(logWindow->mapToGlobal(pt));
    delete menu;
}
*/


cMainWindow::~cMainWindow() {
	deleteResults();

	delete menuBar;
	
	delete menuFile;
	delete menuSearch;
	delete menuTools;
	delete menuView;
	delete menuHelp;
	
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
	delete actionGraph;
	delete actionLog;
	delete actionHTMLDocumentation;
	delete actionPDFManual;
	delete actionAbout;

	delete results;
    delete logWindow;
	delete splitter;
		
	delete about;
	delete graph;
	delete bricksdatabasewidget;
	delete sequencedatabasewidget;
	delete modificationswidget;
	delete drawpeptidewidget;
	delete parameterswidget;
}

