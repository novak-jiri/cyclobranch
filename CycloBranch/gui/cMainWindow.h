/**
	\file cMainWindow.h
	\brief The main window of the application.
*/


#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include "core/cTheoreticalSpectrum.h"
#include "gui/cAboutWidget.h"
#include "gui/cGraphWidget.h"
#include "gui/cParametersWidget.h"
#include "gui/cSpectrumDetailWidget.h"
#include "gui/cMainThread.h"

// forward declaration
class QTextEdit;
class QSplitter;
class QTableWidget;


/**
	\brief The class representing the main window of the application.
*/
class cMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	

	/**
		\brief The constructor.
	*/ 
	cMainWindow();


	/**
		\brief The destructor.
	*/ 
	~cMainWindow();


private:

	// main menu
	QMenuBar* menuBar;
	
	// items in the menu
	QMenu* menuFile;
	QMenu* menuSearch;
	QMenu* menuView;
	QMenu* menuHelp;
	
	// subitems in the menu
	QAction* actionOpenResults;
	QAction* actionSaveResults;
	QAction* actionExportToCsv;
	QAction* actionExportToHTML;
	QAction* actionQuit;
	QAction* actionRun;
	QAction* actionStop;
	QAction* actionProperties;
	QAction* actionGraph;
	QAction* actionLog;
	QAction* actionHTMLDocumentation;
	QAction* actionPDFManual;
	QAction* actionAbout;

	QTableWidget* results;

	vector<cSpectrumDetailWidget> resultsDetails;
	cParameters parameters;

	QTextEdit* logWindow;
	QSplitter* splitter;
		
	cAboutWidget* about;
	cGraphWidget* graph;
	cParametersWidget* parameterswidget;

	int resultsbasecolumncount;
	int resultsspecificcolumncount;

	vector<int> resultsheadersort;

	QString lastdirexporttocsv;
	QString lastdirexporttohtml;
	QString lastdirsaveresults;
	QString lastdiropenresults;

	void closeEvent(QCloseEvent *event);


private slots:

	void showHideLog();

	void showAbout();

	void showGraph();

	void showProperties();

	void run();

	void stop();

	void updateLog(QString qstr);

	void enableRunButtonAndSettings(bool enable);

	void enableStopButton(bool enable);

	void enableButtonsHandlingResults(bool enable);

	void sendTheoreticalSpectrum(cTheoreticalSpectrum theoreticalspectrum);

	void sendParameters(cParameters parameters);

	void prepareColumns();

	void fitColumns();

	void deleteResults();

	void resultsCellClicked(int row, int column);

	void setGraph(string s);

	void headerItemDoubleClicked(int);

	void safeExit();

	void exportToCsv();

	void exportToHTML();

	void showHTMLDocumentation();

	void showPDFManual();

	void saveResultsFile();

	void openResultsFile();

	//void showContextMenu(const QPoint &pt);

signals:

	/**
		\brief The option 'Search->Stop' has been selected.
	*/ 
	void stopComputation();

};


/**
	\brief Register string by Qt.
*/
Q_DECLARE_METATYPE(string);


#endif