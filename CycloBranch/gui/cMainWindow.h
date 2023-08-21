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
#include "gui/cPreferencesDialog.h"
#include "gui/cPatternSimulatorWidget.h"
#include "gui/cFormulaPredictor.h"
#include "gui/cMultipleDatasetsTableWidget.h"


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
	QMenu* menuBookmarks;
	QMenu* menuView;
	QMenu* menuHelp;
	
	QToolBar* toolbarFile;
	QToolBar* toolbarSearch;
	QToolBar* toolbarTools;
	QToolBar* toolbarBookmarks;
	QToolBar* toolbarView;
	QToolBar* toolbarHelp;
	QToolBar* toolbarInputFilter;
	QToolBar* toolbarRowsFilter;

	QAction* actionOpenResults;
	QAction* actionSaveResults;
	QAction* actionExportToCsv;
	QAction* actionExportToHTML;
	QAction* actionPreferences;
	QAction* actionQuit;
	QAction* actionRun;
	QAction* actionStop;
	QAction* actionProperties;
	QAction* actionBricksDatabase;
	QAction* actionSequenceDatabase;
	QAction* actionModifications;
	QAction *actionDrawPeptide;
	QAction *actionBookMark1;
	QAction *actionBookMark2;
	QAction *actionBookMark3;
	QAction *actionBookMark4;
	QAction *actionBookMark5;
	QAction *actionBookMark6;
	QAction *actionBookMark7;
	QAction *actionBookMark8;
	QAction *actionBookMark9;
	QAction *actionBookMark10;
	QAction* actionShowIsomers;
	QAction* actionGraph;
	QAction* actionSummaryTableOfMatchedPeaks;
	QAction* actionMultipleDatasets;
	QAction* actionChromatogramWindow;
	QAction* actionImageWindow;
	QAction* actionPatternSimulatorWindow;
	QAction* actionFormulaPredictor;
	QAction* actionLog;
	QAction* actionHTMLDocumentation;
	QAction* actionPDFManual;
	QAction* actionAbout;

	QWidget* inputfilterwidget;
	QHBoxLayout* inputfilterhbox;
	QComboBox* inputfiltercombobox;
	QPushButton* inputfilterbutton;

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

	cTheoreticalSpectrumList listoftheoreticalspectra;
	cPeakListSeries eicchromatograms;
	vector<cSpectrumDetailWidget> spectradetails;
	vector<cSpectrumDetailWidget> avgspectra;

	cGlobalPreferences globalpreferences;
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
	cPreferencesDialog* preferencesdialog;
	cMultipleDatasetsTableWidget* multipledatasetswidget;
	cPatternSimulatorWidget* patternsimulatorwidget;
	cFormulaPredictor* formulapredictor;

	int resultsbasecolumncount;
	int resultsspecificcolumncount;
	int searchspecificcolumncount;

	QString lastdirexporttocsv;
	QString lastdirexporttohtml;

	QString lastdirsaveresults;
	QString lastdiropenresults;

	bool summarytableisprepared;
	bool multipledatasetsisprepared;

	int lastactivedetail;

	QRect othernormalgeometry;
	bool otherismaximized;
	bool otherprofilespectrum;
	bool otherabsoluteintensity;
	bool othermatchedpeaks;
	bool otherunmatchedpeaks;
	bool otherscrambledpeaks;
	bool otherlabelspeaks;
	bool othermzselection;

	int activefileid;

	int avgspectrumused;
	bool avgspectrumabsoluteintensityenabled;
	QRect avgspectrumgeometry;
	bool avgspectrummaximized;

	bool quitapp;

	void closeEvent(QCloseEvent *event);

	void preparePeptideSequence(int row, string& peptidesequence, bool reportisomers);

	void reportSpectrum(int row, cTheoreticalSpectrum& theoreticalspectrum, bool reportisomers);

	void deleteResults();

	void applyGlobalPreferences();

	void openDetail(int rowid);

	void calculateAvgSpectrum(int minscan, int maxscan);

	
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

	void showMultipleDatasetsOverview();

	void showPatternSimulatorWindow();

	void showFormulaPredictor();

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

	void showPreferences();

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

	void lastActiveDetailSlot(int rowid);

	void gotoBookMark1();

	void gotoBookMark2();

	void gotoBookMark3();

	void gotoBookMark4();

	void gotoBookMark5();

	void gotoBookMark6();

	void gotoBookMark7();

	void gotoBookMark8();

	void gotoBookMark9();

	void gotoBookMark10();

	void summaryPeaksTableCancelled();

	void multipleDatasetsTableCancelled();

	void summaryPeaksTableRowDoubleClicked(int rowid, double experimentalmz);

	void multipleDatasetsTableRowClicked(int datasetid, int spectrumid, int peakid, int datatypeview, int peakshape, string name, string iontype, string mzstr);

	void updateSummaryPeaksTableFilterSlot(int xmin, int xmax, int ymin, int ymax);

	void chromatogramDoubleClicked(int scanid);

	void imageWindowDoubleClicked(int spectrumid);

	void inputFilterButtonReleased();

	void filterSummaryTableIfPrepared();

	void showAvgSpectrum(int minscan, int maxscan);

	void avgSpectrumAbsoluteIntensityStateChanged(bool state);

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