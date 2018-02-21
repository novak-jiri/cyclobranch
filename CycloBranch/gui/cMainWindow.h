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
#include <QFile>
#include "core/utilities.h"
#include "core/cAllocator.h"
#include "core/cTheoreticalSpectrum.h"
#include "gui/cAboutWidget.h"
#include "gui/cGraphWidget.h"
#include "gui/cParametersWidget.h"
#include "gui/cSpectrumDetailWidget.h"
#include "gui/cBricksDatabaseWidget.h"
#include "gui/cSequenceDatabaseWidget.h"
#include "gui/cModificationsWidget.h"
#include "gui/cDrawPeptideWidget.h"
#include "gui/cSummaryPeaksTableWidget.h"
#include "gui/cMainThread.h"
#include "gui/cDelegate.h"
#include "gui/cHTMLExportDialog.h"
#include "gui/cImageWindow.h"


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


protected:

	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:

	// main menu
	QMenuBar* menuBar;
	
	// items in the menu
	QMenu* menuFile;
	QMenu* menuSearch;
	QMenu* menuTools;
	QMenu* menuView;
	QMenu* menuHelp;
	
	// toolbars
	QToolBar* toolbarFile;
	QToolBar* toolbarSearch;
	QToolBar* toolbarTools;
	QToolBar* toolbarView;
	QToolBar* toolbarHelp;
	QToolBar* toolbarFilter;

	// subitems in the menu
	QAction* actionOpenResults;
	QAction* actionSaveResults;
	QAction* actionExportToCsv;
	QAction* actionExportToHTML;
	QAction* actionQuit;
	QAction* actionRun;
	QAction* actionStop;
	QAction* actionProperties;
	QAction* actionBricksDatabase;
	QAction* actionSequenceDatabase;
	QAction* actionModifications;
	QAction *actionDrawPeptide;
	QAction *actionNorine;
	QAction *actionSmilesToMonomers;
	QAction* actionShowIsomers;
	QAction* actionGraph;
	QAction* actionSummaryTableOfMatchedPeaks;
	QAction* actionImageWindow;
	QAction* actionLog;
	QAction* actionHTMLDocumentation;
	QAction* actionPDFManual;
	QAction* actionAbout;

	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QLineEdit* rowsfilterline;
	QCheckBox* rowsfiltercasesensitive;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableWidget* results;
	cTheoreticalSpectrumList theoreticalspectrumlist;
	vector<cSpectrumDetailWidget> spectradetails;

	cParameters parameters;

	QTextEdit* logWindow;
	QSplitter* splitter;
		
	cAboutWidget* about;
	cGraphWidget* graph;
	cBricksDatabaseWidget* bricksdatabasewidget;
	cSequenceDatabaseWidget* sequencedatabasewidget;
	cModificationsWidget* modificationswidget;
	cDrawPeptideWidget* drawpeptidewidget;
	cSummaryPeaksTableWidget* summarytableofmatchedpeaks;
	cImageWindow* imagewindow;
	cParametersWidget* parameterswidget;
	cHTMLExportDialog* htmlexportdialog;

	int resultsbasecolumncount;
	int resultsspecificcolumncount;
	int dbsearchspecificcolumncount;

	vector<int> resultsheadersort;
	cDelegate columndelegate;

	QString lastdirexporttocsv;
	QString lastdirexporttohtml;
	QString lastdirsaveresults;
	QString lastdiropenresults;

	bool summarytableisprepared;

	cAllocator<QTableWidgetItem> widgetitemallocator;

	bool quitapp;

	void closeEvent(QCloseEvent *event);

	void preparePeptideSequence(int row, string& peptidesequence, bool reportisomers);

	void reportSpectrum(int id, cTheoreticalSpectrum& theoreticalspectrum, bool reportisomers);

	void deleteResults();

	
private slots:

	void showHideLog();

	void showAbout();

	void showBricksDatabase();

	void showSequenceDatabase();

	void showModifications();

	void showDrawPeptideWidget();

	void setAndShowDrawPeptideWidget(int peptidetypeindex, QString sequence);

	void showSummaryTableOfMatchedPeaks();

	void showImageWindow();
	
	void showGraph();

	void showProperties();

	void run();

	void stop();

	void updateLog(QString qstr);

	void enableRunButtonAndSettings(bool enable);

	void enableStopButton(bool enable);

	void enableButtonsHandlingResults(bool enable);

	void sendParameters(cParameters parameters);

	void reportSpectra();

	void updateSpectra();

	void resultsCellClicked(int row, int column);

	void setGraph(string s);

	void headerItemDoubleClicked(int);

	void exportToCsv();

	void exportToHTML();

	void showHTMLDocumentation();

	void showPDFManual();

	void saveResultsFile();

	void openResultsFile();

	void setSequence(int peptidetypeindex, QString sequence);

	void setTag(int peptidetypeindex, QString tag);

	void insertSequence(int peptidetypeindex, QString tag);

	void quitApplication();

	void filterResults();

	void resetFilter();

	void gotoNorine();

	void gotoSmiles2Monomers();

	void summaryPeaksTableCancelled();

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