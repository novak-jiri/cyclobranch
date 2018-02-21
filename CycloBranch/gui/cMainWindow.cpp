#include "gui/cMainWindow.h"

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


cMainWindow::cMainWindow() {

	// main menu
	menuBar = new QMenuBar(this);
	
	// items in the menu
	menuFile = new QMenu(tr("&File"), this);
	menuSearch = new QMenu(tr("&Search"), this);
	menuTools = new QMenu(tr("&Tools"), this);
	menuView = new QMenu(tr("&View"), this);
	menuHelp = new QMenu(tr("&Help"), this);
	
	// subitems in the menu
	// actionOpen = new QAction(tr("&Open"), this);
	actionOpenResults = new QAction(tr("&Open Results..."), this);
	actionSaveResults = new QAction(tr("&Save Results..."), this);
	actionExportToCsv = new QAction(tr("&Export to CSV"), this);
	actionExportToHTML = new QAction(tr("&Export to HTML"), this);
	actionQuit = new QAction(tr("&Quit"), this);
	actionRun = new QAction(tr("&Run"), this);
	actionStop = new QAction(tr("&Stop"), this);
	actionStop->setEnabled(false);
	actionProperties = new QAction(tr("&Settings..."), this);
	actionBricksDatabase = new QAction(tr("&Building Blocks Editor"), this);
	actionSequenceDatabase = new QAction(tr("&Sequence Database Editor"), this);
	actionGraph = new QAction(tr("&Graph"), this);
	actionLog = new QAction(tr("&Log Window"), this);
	actionHTMLDocumentation = new QAction(tr("&HTML Documentation"), this);
	actionPDFManual = new QAction(tr("&PDF Manual"), this);
	actionAbout = new QAction(tr("&About"), this);

	// widgets
	results = new QTableWidget(0, 0, this);
	logWindow = new QTextEdit(this);
	splitter = new QSplitter(this);

	about = new cAboutWidget(this);

	graph = new cGraphWidget();
	bricksdatabasewidget = new cBricksDatabaseWidget();
	sequencedatabasewidget = new cSequenceDatabaseWidget();
	parameterswidget = new cParametersWidget();
	
	// additional key shortcuts
	// actionOpen->setShortcut(QKeySequence("Ctrl+O"));

	// connection of menu items with functions
	// connect(actionOpen, SIGNAL(triggered()), this, SLOT(openDir()));
	connect(actionOpenResults, SIGNAL(triggered()), this, SLOT(openResultsFile()));
	connect(actionSaveResults, SIGNAL(triggered()), this, SLOT(saveResultsFile()));
	connect(actionExportToCsv, SIGNAL(triggered()), this, SLOT(exportToCsv()));
	connect(actionExportToHTML, SIGNAL(triggered()), this, SLOT(exportToHTML()));
	connect(actionQuit, SIGNAL(triggered()), this, SLOT(stop()));
	connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actionRun, SIGNAL(triggered()), this, SLOT(run()));
	connect(actionStop, SIGNAL(triggered()), this, SLOT(stop()));
	connect(actionProperties, SIGNAL(triggered()), this, SLOT(showProperties()));
	connect(actionBricksDatabase, SIGNAL(triggered()), this, SLOT(showBricksDatabase()));
	connect(actionSequenceDatabase, SIGNAL(triggered()), this, SLOT(showSequenceDatabase()));
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
	parameterswidget->hide();

	splitter->setOrientation(Qt::Vertical);
	splitter->addWidget(results);
	splitter->addWidget(logWindow);
	splitter->setStretchFactor(0, 1);

	// set the central widget
	setCentralWidget(splitter);

	// set the size of main window
	resize(1280, 700);

	setWindowTitle(appname);

	resultsbasecolumncount = 8;
	resultsspecificcolumncount = 0;
	dbsearchspecificcolumncount = 0;

	resultsDetails.clear();

	lastdirexporttocsv = "./";
	lastdirexporttohtml = "./";
	lastdirsaveresults = "./";
	lastdiropenresults = "./";
}


void cMainWindow::closeEvent(QCloseEvent *event) {
	emit stopComputation();
	qApp->quit();
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


void cMainWindow::showGraph() {
	graph->show();
	graph->activateWindow();
}


void cMainWindow::showProperties() {
	parameterswidget->show();
	parameterswidget->activateWindow();
}


void cMainWindow::run() {
	logWindow->clear();
	deleteResults();
	resultsDetails.clear();

	cMainThread* thread = new cMainThread(parameterswidget->getParameters(), true, false);
	connect(thread, SIGNAL(message(QString)), this, SLOT(updateLog(QString)));
	connect(thread, SIGNAL(enableRunButtonAndSettings(bool)), this, SLOT(enableRunButtonAndSettings(bool)));
	connect(thread, SIGNAL(enableStopButton(bool)), this, SLOT(enableStopButton(bool)));
	connect(thread, SIGNAL(enableButtonsHandlingResults(bool)), this, SLOT(enableButtonsHandlingResults(bool)));
	connect(thread, SIGNAL(sendTheoreticalSpectrum(cTheoreticalSpectrum)), this, SLOT(sendTheoreticalSpectrum(cTheoreticalSpectrum)));
	connect(thread, SIGNAL(sendParameters(cParameters)), this, SLOT(sendParameters(cParameters)));
	connect(thread, SIGNAL(prepareColumns()), this, SLOT(prepareColumns()));
	connect(thread, SIGNAL(fitColumns()), this, SLOT(fitColumns()));
	connect(thread, SIGNAL(setGraph(string)), this, SLOT(setGraph(string)));
	connect(this, SIGNAL(stopComputation()), thread, SLOT(stopComputation()));
	connect(thread, SIGNAL(safeExit()), this, SLOT(safeExit()));

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


void cMainWindow::sendTheoreticalSpectrum(cTheoreticalSpectrum theoreticalspectrum) {
	int row = results->rowCount();
    results->insertRow(row);
	
	results->setItem(row, 0, new QTableWidgetItem(""));
	
	results->setItem(row, 1, new QTableWidgetItem());
	results->item(row, 1)->setData(Qt::DisplayRole, results->rowCount());


	if ((parameters.mode == denovoengine) || (parameters.mode == singlecomparison) || (parameters.mode == databasesearch)) {

		if (parameters.mode == databasesearch) {
			results->setItem(row, 2, new QTableWidgetItem(theoreticalspectrum.getCandidate().getName().c_str()));
		}

		results->setItem(row, 2 + dbsearchspecificcolumncount, new QTableWidgetItem(stripHTML(theoreticalspectrum.getAcronymPeptideNameWithHTMLReferences()).c_str()));

		results->setItem(row, 3 + dbsearchspecificcolumncount, new QTableWidgetItem());
		results->item(row, 3 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getCandidate().getSummaryFormula(parameters, parameters.peptidetype).c_str());

		results->setItem(row, 4 + dbsearchspecificcolumncount, new QTableWidgetItem());
		results->item(row, 4 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, getNumberOfBricks(theoreticalspectrum.getCandidate().getComposition()));

		switch (parameters.peptidetype)
		{
		case linear:
			results->setItem(row, 5 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str()));
			results->setItem(row, 6 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str()));
			break;
		case branched:
			results->setItem(row, 5 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str()));
			results->setItem(row, 6 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str()));
			results->setItem(row, 7 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str()));
			break;
		case cyclic:
			results->setItem(row, 5 + dbsearchspecificcolumncount, new QTableWidgetItem());
			results->item(row, 5 + dbsearchspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedBricks());	
			break;
		case lasso:
			results->setItem(row, 5 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getMiddleModifID()].name.c_str()));
			break;
		case linearpolysaccharide:
			results->setItem(row, 5 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getStartModifID()].name.c_str()));
			results->setItem(row, 6 + dbsearchspecificcolumncount, new QTableWidgetItem(parameters.searchedmodifications[theoreticalspectrum.getCandidate().getEndModifID()].name.c_str()));
			break;
		case other:
		default:
			break;
		}

		results->setItem(row, 5 + dbsearchspecificcolumncount + resultsspecificcolumncount, new QTableWidgetItem());
		results->item(row, 5 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks());

		results->setItem(row, 6 + dbsearchspecificcolumncount + resultsspecificcolumncount, new QTableWidgetItem());
		results->item(row, 6 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getRatioOfMatchedPeaks()*100);

		results->setItem(row, 7 + dbsearchspecificcolumncount + resultsspecificcolumncount, new QTableWidgetItem());
		results->item(row, 7 + dbsearchspecificcolumncount + resultsspecificcolumncount)->setData(Qt::DisplayRole, theoreticalspectrum.getWeightedIntensityScore());

		for (int i = 0; i < (int)parameters.fragmentionsfortheoreticalspectra.size(); i++) {
			results->setItem(row, resultsbasecolumncount  + dbsearchspecificcolumncount + resultsspecificcolumncount + i, new QTableWidgetItem());
			results->item(row, resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks(parameters.fragmentionsfortheoreticalspectra[i]));
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			results->setItem(row, results->columnCount() - 1, new QTableWidgetItem());
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
		results->setItem(row, 2, new QTableWidgetItem());
		results->item(row, 2)->setData(Qt::DisplayRole, theoreticalspectrum.getNumberOfMatchedPeaks());

		results->setItem(row, 3, new QTableWidgetItem());
		results->item(row, 3)->setData(Qt::DisplayRole, theoreticalspectrum.getRatioOfMatchedPeaks()*100);

		results->setItem(row, 4, new QTableWidgetItem());
		results->item(row, 4)->setData(Qt::DisplayRole, theoreticalspectrum.getWeightedIntensityScore());
	}


	cSpectrumDetailWidget sd;
	sd.initialize(&parameters, theoreticalspectrum);
	sd.setWindowTitle(("Theoretical Spectrum No. " + to_string(row+1)).c_str());
	resultsDetails.push_back(sd);
}


void cMainWindow::sendParameters(cParameters parameters) {
	this->parameters = parameters;
}


void cMainWindow::prepareColumns() {

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

		results->setHorizontalHeaderItem(0, new QTableWidgetItem("*"));
		results->setHorizontalHeaderItem(1, new QTableWidgetItem("Result ID"));
		if (parameters.mode == databasesearch) {
			results->setHorizontalHeaderItem(2, new QTableWidgetItem("Name"));
		}
		results->setHorizontalHeaderItem(2 + dbsearchspecificcolumncount, new QTableWidgetItem("Peptide Sequence"));
		results->setHorizontalHeaderItem(3 + dbsearchspecificcolumncount, new QTableWidgetItem("Summary Formula"));
		results->setHorizontalHeaderItem(4 + dbsearchspecificcolumncount, new QTableWidgetItem("Number of Bricks"));

		switch (parameters.peptidetype)
		{
		case linear:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, new QTableWidgetItem("N-terminal Modification"));
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, new QTableWidgetItem("C-terminal Modification"));
			break;
		case branched:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, new QTableWidgetItem("N-terminal Modification"));
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, new QTableWidgetItem("Branch Modification"));
			results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount, new QTableWidgetItem("C-terminal Modification"));
			break;
		case cyclic:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, new QTableWidgetItem("Matched Bricks"));
			break;
		case lasso:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, new QTableWidgetItem("Branch Modification"));
			break;
		case linearpolysaccharide:
			results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount, new QTableWidgetItem("N-terminal Modification"));
			results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount, new QTableWidgetItem("C-terminal Modification"));
			break;
		case other:
		default:
			break;
		}
		
		results->setHorizontalHeaderItem(5 + dbsearchspecificcolumncount + resultsspecificcolumncount, new QTableWidgetItem("Matched Peaks"));
		results->setHorizontalHeaderItem(6 + dbsearchspecificcolumncount + resultsspecificcolumncount, new QTableWidgetItem("Ratio of Matched Peaks [%]"));
		results->setHorizontalHeaderItem(7 + dbsearchspecificcolumncount + resultsspecificcolumncount, new QTableWidgetItem("Sum of Relative Intensities"));

		for (int i = 0; i < (int)parameters.fragmentionsfortheoreticalspectra.size(); i++) {
			results->setHorizontalHeaderItem(resultsbasecolumncount + dbsearchspecificcolumncount + resultsspecificcolumncount + i, new QTableWidgetItem(parameters.fragmentdefinitions[(fragmentIonType)parameters.fragmentionsfortheoreticalspectra[i]].name.c_str()));
		}

		if ((parameters.peptidetype == cyclic) && parameters.enablescrambling) {
			results->setHorizontalHeaderItem(results->columnCount() - 1, new QTableWidgetItem("Scrambled Peaks"));
		}

	}


	if (parameters.mode == dereplication) {
		results->setColumnCount(5);
		results->setHorizontalHeaderItem(0, new QTableWidgetItem("*"));
		results->setHorizontalHeaderItem(1, new QTableWidgetItem("Result ID"));
		results->setHorizontalHeaderItem(2, new QTableWidgetItem("Matched Peaks"));
		results->setHorizontalHeaderItem(3, new QTableWidgetItem("Ratio of Matched Peaks [%]"));
		results->setHorizontalHeaderItem(4, new QTableWidgetItem("Sum of Relative Intensities"));
	}


	results->resizeColumnsToContents();

	resultsheadersort.resize(results->columnCount());
	for (int i = 0; i < results->columnCount(); i++) {
		resultsheadersort[i] = -1;
	}

}


void cMainWindow::fitColumns() {
	for (int i = 0; i < results->columnCount(); i++) {
		results->resizeColumnToContents(i);
	}
}


void cMainWindow::deleteResults() {
	while (results->rowCount() > 0) {
		for (int i = 0; i < results->columnCount(); i++) {
			delete results->item(results->rowCount() - 1, i);
		}
		results->removeRow(results->rowCount() - 1);
	}

	for (int i = 0; i < results->columnCount(); i++) {
		delete results->horizontalHeaderItem(i);
	}
	results->setColumnCount(0);
}


void cMainWindow::resultsCellClicked(int row, int column) {
	resultsDetails[results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1].prepareToShow(parameters.peptidetype);
	resultsDetails[results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1].show();
	resultsDetails[results->item(row, 1)->data(Qt::DisplayRole).toInt() - 1].activateWindow();
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


void cMainWindow::safeExit() {
	enableRunButtonAndSettings(true);
	enableStopButton(false);
	enableButtonsHandlingResults(true);
}


void cMainWindow::exportToCsv() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to CSV"), lastdirexporttocsv, tr("Files (*.csv)"));
	lastdirexporttocsv = filename;
	
	if (filename.toStdString().compare("") != 0) {
		
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
		}

		file.close();
	}
}


void cMainWindow::exportToHTML() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Export to HTML"), lastdirexporttohtml, tr("HTML Files (*.htm *.html)"));
	lastdirexporttohtml = filename;
	
	if (filename.toStdString().compare("") != 0) {
		
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

				out << "<p style=\"margin: 20px\">" << resultsDetails[i].getTheoreticalSpectrum().getCoverageBySeries().c_str();
				out << "<br/>" << resultsDetails[i].getDetailsAsHTMLString().c_str() << "</p>\n";

				out << "</td></tr>\n";
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

	}

}


void cMainWindow::showHTMLDocumentation() {
	QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/userguide.html").absoluteFilePath()));
}


void cMainWindow::showPDFManual() {
	QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/refman.pdf").absoluteFilePath()));
}


void cMainWindow::saveResultsFile() {
	
	string filetypes = appname.toStdString() + " " + appversion.toStdString() + " Results Files (*.res)";
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Results..."), lastdirsaveresults, tr(filetypes.c_str()));
	lastdirsaveresults = filename;
	
	if (filename.toStdString().compare("") != 0) {
		
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
			size = (int)resultsDetails.size();
			outfile.write((char *)&size, sizeof(int));
			for (int i = 0; i < (int)resultsDetails.size(); i++) {
				resultsDetails[i].getTheoreticalSpectrum().store(outfile);
			}

			// close file
			outfile.close();
		}
	
	}

}


void cMainWindow::openResultsFile() {

	string filetypes = appname.toStdString() + " " + appversion.toStdString() + " Results Files (*.res)";
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Results..."), lastdiropenresults, tr(filetypes.c_str()));
	lastdiropenresults = filename;
	
	if (filename.toStdString().compare("") != 0) {

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
			resultsDetails.clear();
			
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
			prepareColumns();
			infile.read((char *)&size, sizeof(int));
			for (int i = 0; i < size; i++) {
				theoreticalspectrum.load(infile);
				sendTheoreticalSpectrum(theoreticalspectrum);
			}
			fitColumns();

			// close file
			infile.close();
		}
	
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
	delete parameterswidget;
}

