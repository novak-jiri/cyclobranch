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
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QComboBox>
#include "core/utilities.h"
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
#include "gui/cHTMLExportDialog.h"
#include "gui/cImageWindow.h"
#include "gui/cChromatogramWindow.h"
#include "gui/cMainWindowProxyModel.h"


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

	QMenuBar* menuBar;
	
	QMenu* menuFile;
	QMenu* menuSearch;
	QMenu* menuTools;
	QMenu* menuView;
	QMenu* menuHelp;
	
	QToolBar* toolbarFile;
	QToolBar* toolbarSearch;
	QToolBar* toolbarTools;
	QToolBar* toolbarView;
	QToolBar* toolbarHelp;
	QToolBar* toolbarFilter;

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
	QAction *actionBBDGNC;
	QAction* actionShowIsomers;
	QAction* actionGraph;
	QAction* actionSummaryTableOfMatchedPeaks;
	QAction* actionImageWindow;
	QAction* actionChromatogramWindow;
	QAction* actionLog;
	QAction* actionHTMLDocumentation;
	QAction* actionPDFManual;
	QAction* actionAbout;

	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QComboBox* rowsfilteroperator;
	QComboBox* rowsfiltercombobox1;
	QComboBox* rowsfiltercomparatorcombobox1;
	QLineEdit* rowsfilterline1;
	QComboBox* rowsfiltercombobox2;
	QComboBox* rowsfiltercomparatorcombobox2;
	QLineEdit* rowsfilterline2;
	QCheckBox* rowsfiltercasesensitive;
	QCheckBox* rowsfilterwholeword;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableView* results;
	QStandardItemModel* resultsmodel;
	cMainWindowProxyModel* resultsproxymodel;

	cTheoreticalSpectrumList theoreticalspectrumlist;
	vector<cSpectrumDetailWidget> spectradetails;

	cParameters parameters;

	cPeakListSeries rawdata;
	vector<cImzMLItem> imzmlprofilemetadata;
	bool profilemz64precision;
	bool profileintensity64precision;

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
	cChromatogramWindow* chromatogramwindow;
	cParametersWidget* parameterswidget;
	cHTMLExportDialog* htmlexportdialog;

	int resultsbasecolumncount;
	int resultsspecificcolumncount;
	int searchspecificcolumncount;

	QString lastdirexporttocsv;
	QString lastdirexporttohtml;
	QString lastdirsaveresults;
	QString lastdiropenresults;

	bool summarytableisprepared;

	bool quitapp;

	void closeEvent(QCloseEvent *event);

	void preparePeptideSequence(int row, string& peptidesequence, bool reportisomers);

	void reportSpectrum(int row, cTheoreticalSpectrum& theoreticalspectrum, bool reportisomers);

	void deleteResults();

	
private slots:

	void showHideLog();

	void showAbout();

	void showBricksDatabase();

	void showSequenceDatabase();

	void showModifications();

	void showDrawPeptideWidget();

	void setAndShowDrawPeptideWidget(int peptidetypeindex, QString sequence);

	bool prepareSummaryTableOfMatchedPeaks();

	void showSummaryTableOfMatchedPeaks();

	void showImageWindow();
	
	void showChromatogramWindow();

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

	void showIsomersStateChanged();

	void rowDoubleClicked(const QModelIndex& item);

	void setGraph(string s);

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

	void gotoBBDGNC();

	void summaryPeaksTableCancelled();

	void summaryPeaksTableRowDoubleClicked(int rowid, double experimentalmz);

	void updateSummaryPeaksTableFilterSlot(int xmin, int xmax, int ymin, int ymax);

	void chromatogramDoubleClicked(int scanid);

	void imageWindowDoubleClicked(int spectrumid);

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