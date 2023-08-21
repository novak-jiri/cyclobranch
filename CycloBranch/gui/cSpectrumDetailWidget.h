/**
	\file cSpectrumDetailWidget.h
	\brief The widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/


#ifndef _CSPECTRUMDETAILWIDGET_H
#define _CSPECTRUMDETAILWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QUrl>
#include <QFileInfo>
#include <QMainWindow>
#include <QTextDocument>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QProgressDialog>
#include <QTextStream>
#include "core/cGlobalPreferences.h"
#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"
#include "gui/cBranchedWidget.h"
#include "gui/cBranchCyclicWidget.h"
#include "gui/cSpectrumSceneWidget.h"
#include "gui/cFindDialog.h"
#include "gui/cExportDialog.h"
#include "gui/cEventFilter.h"
#include "gui/cViewButtonDelegate.h"
#include "gui/cSpectrumDetailProxyModel.h"
#include "gui/cHTMLDelegate.h"


// forward declaration
class QTextBrowser;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QCheckBox;
class QPushButton;
class QSplitter;
class QComboBox;
class QDoubleSpinBox;
class QAction;
class QLabel;


/**
	\brief Position in QTableWidget.
*/
struct cTablePosition {
	/**
		\brief A number of a row.
	*/
	int row;


	/**
		\brief A number of a column.
	*/
	int column;


	cTablePosition() {
		row = 0;
		column = 0;
	}
};


/**
	\brief The class representing a widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/
class cSpectrumDetailWidget : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cSpectrumDetailWidget();


	/**
		\brief The copy constructor.
		\param sd reference to a cSpectrumDetailWidget
	*/ 
	cSpectrumDetailWidget(const cSpectrumDetailWidget& sd);


	/**
		\brief Overloaded operator=.
		\param sd reference to a cSpectrumDetailWidget
	*/ 
	cSpectrumDetailWidget& operator=(const cSpectrumDetailWidget& sd);


	/**
		\brief The destructor.
	*/ 
	~cSpectrumDetailWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Initialize the widget.
		\param rowid id of a row
		\param scantitle scan title
		\param activefileid id of an active file
		\param globalpreferences global preferences of the application
		\param parameters a pointer to parameters
		\param theoreticalspectrum a reference to a theoretical spectrum
		\param parent pointer to a parent widget
	*/ 
	void initialize(int rowid, string scantitle, int activefileid, cGlobalPreferences* globalpreferences, cParameters* parameters, cTheoreticalSpectrum& theoreticalspectrum, QWidget* parent);


	/**
		\brief Get the theoretical spectrum.
		\retval cTheoreticalSpectrum reference to a theoretical spectrum
	*/ 
	cTheoreticalSpectrum& getTheoreticalSpectrum();


	/**
		\brief Get details about a theoretical spectrum as a HTML string.
		\retval string details about a theoretical spectrum
	*/ 
	string getDetailsAsHTMLString();


	/**
		\brief Get a table of peaks as a HTML string.
		\param unmatchedtheoreticalpeaks if true then unmatched theoretical peaks are included
		\param unmatchedexperimentalpeaks if true then unmatched experimental peaks are included
		\param exportisomers if true then isomers of building blocks are exported
		\retval string a table of peaks as a HTML string
	*/ 
	string getPeaksTableAsHTMLString(bool unmatchedtheoreticalpeaks, bool unmatchedexperimentalpeaks, bool exportisomers);


	/**
		\brief Get a partial table of peaks as a HTML string.
		\param id identifier of a spectrum
		\param exportisomers if true then isomers of building blocks are exported
		\retval string a partial table of peaks as a HTML string
	*/ 
	string getPartialPeaksTableAsHTMLString(int id, bool exportisomers);


	/**
		\brief Prepare the widget to show.
		\param actionShowIsomers reference to QAction to show isomers of building blocks
		\param rawdata a pointer to a structure handling raw data
		\param imzmlprofilemetadata imzml profile metadata
		\param profilemz64precision true if 64 mz precision is used, false if 32 bit mz precision is used
		\param profileintensity64precision true if 64 intensity precision is used, false if 32 bit intensity precision is used
		\param usedefaultsize if true, the default size is used
	*/
	void prepareToShow(QAction* actionShowIsomers, cPeakListSeries* rawdata, vector<cImzMLItem>* imzmlprofilemetadata, bool profilemz64precision, bool profileintensity64precision, bool usedefaultsize);
	

	/**
		\brief Find all occurrences of \a str and highlight them.
		\param str search string
		\param opt search options
		\param errormessage if true, an error message in a popup window is shown when no results are matched
	*/ 
	void findAll(const QString& str, QTextDocument::FindFlags opt = 0, bool errormessage = true);


	/**
		\brief Export spectrum/peptide image.
		\param exportspectrum if true, a spectrum image is exported; if false, a peptide image is exported
	*/ 
	void exportImage(bool exportspectrum);


	/**
		\brief Zoom to a peak determined by the value.
		\param value mz ratio of a peak
	*/
	void zoomToPeak(double value);


	/**
		\brief Get a set of local neutral losses.
		\retval set<int> get a set of local neutral losses
	*/
	set<string>& getLocalNeutralLosses();


	/**
		\brief Set the state of profile spectrum visualization.
		\param enable state of profile spectrum visualization
	*/
	void setProfileSpectrumEnabled(bool enable);


	/**
		\brief Check if the profile spectrum visualization is enabled.
		\retval bool true if the profile spectrum is enabled
	*/
	bool hasProfileSpectrumEnabled();


	/**
		\brief Permanently disable the visualization in profile mode.
	*/
	void disableProfileMode();


	/**
		\brief Set the state of absolute intensity mode.
		\param enable state of absolute intensity mode
	*/
	void setAbsoluteIntensityEnabled(bool enable);


	/**
		\brief Check if the absolute intensity mode is enabled.
		\retval bool true if the absolute intensity mode is enabled
	*/
	bool hasAbsoluteIntensityEnabled();


	/**
		\brief Set the state of hide matched peaks mode.
		\param enable state of hide matched peaks mode
	*/
	void setHideMatchedPeaksEnabled(bool enable);


	/**
		\brief Check if the hide matched peaks mode is enabled.
		\retval bool true if the hide matched peaks mode is enabled
	*/
	bool hasHideMatchedPeaksEnabled();


	/**
		\brief Set the state of hide unmatched peaks mode.
		\param enable state of hide unmatched peaks mode
	*/
	void setHideUnmatchedPeaksEnabled(bool enable);


	/**
		\brief Check if the hide unmatched peaks mode is enabled.
		\retval bool true if the hide unmatched peaks mode is enabled
	*/
	bool hasHideUnmatchedPeaksEnabled();


	/**
		\brief Set the state of hide scrambled peaks mode.
		\param enable state of hide scrambled peaks mode
	*/
	void setHideScrambledPeaksEnabled(bool enable);


	/**
		\brief Check if the hide scrambled peaks mode is enabled.
		\retval bool true if the hide scrambled peaks mode is enabled
	*/
	bool hasHideScrambledPeaksEnabled();


	/**
		\brief Set the state of hide peak labels mode.
		\param enable state of hide peak labels mode
	*/
	void setHidePeakLabelsEnabled(bool enable);


	/**
		\brief Check if the hide peak labels mode is enabled.
		\retval bool true if the hide peak labels mode is enabled
	*/
	bool hasHidePeakLabelsEnabled();


	/**
		\brief Set the state of mouse m/z selection tool.
		\param enable state of mouse m/z selection tool
	*/
	void setMouseMzSelectionEnabled(bool enable);


	/**
		\brief Check if the mouse m/z selection tool is enabled.
		\retval bool true if the mouse m/z selection tool is enabled
	*/
	bool hasMouseMzSelectionEnabled();


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


	/**
		\brief Handle a move event.
		\param event pointer to QMoveEvent
	*/
	void moveEvent(QMoveEvent *event);


private:

	QWidget* parent;
	QAction* actionShowIsomers;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuFind;
	QMenu* menuView;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QToolBar* toolbarFind;
	QToolBar* toolbarView;
	QToolBar* toolbarHelp;
	QToolBar* toolbarMz;
	QToolBar* toolbarIonSeries;
	QToolBar* toolbarNeutralLoss;
	QToolBar* toolbarRotation;
	QToolBar* toolbarTrotation;

	QAction* actionExportTable;
	QAction* actionExportSpectrum;
	QAction* actionCloseWindow;
	QAction* actionFind;
	QAction* actionPrevious;
	QAction* actionNext;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionAbsoluteIntensity;
	QAction* actionRawData;
	QAction* actionHideMatched;
	QAction* actionHideUnmatched;
	QAction* actionHideScrambled;
	QAction* actionHideLabels;
	QAction* actionMouseMzSelection;
	QAction* actionHTMLDocumentation;
	QSplitter* hsplitter1;
	QSplitter* hsplitter2;
	QSplitter* vsplitter;

	QWidget* widgetmz;
	QHBoxLayout* hboxmz;
	QLabel* labelmz;
	QDoubleSpinBox* minmz;
	QLabel* labelseparator;
	QDoubleSpinBox* maxmz;
	QPushButton* setmzinterval;
	QPushButton* resetmzinterval;

	QWidget* ionserieswidget;
	QHBoxLayout* ionserieshbox;
	QLabel* ionserieslabel;
	QComboBox* ionseriescombobox;

	QWidget* neutrallosswidget;
	QHBoxLayout* neutrallosshbox;
	QLabel* neutrallosslabel;
	QComboBox* neutrallosscombobox;

	QWidget* rotationwidget;
	QHBoxLayout* rotationhbox;
	QLabel* rotationlabel;
	QComboBox* rotationcombobox;

	QWidget* trotationwidget;
	QHBoxLayout* trotationhbox;
	QLabel* trotationlabel;
	QComboBox* trotationcombobox;

	QTextBrowser* textbrowser;

	QTableView* peakstable;
	QStandardItemModel* peakstablemodel;
	cSpectrumDetailProxyModel* proxymodel;

	vector<cTablePosition> tablematches;

	cLinearWidget* linearwidget;
	cCyclicWidget* cyclicwidget;
	cBranchedWidget* branchedwidget;
	cBranchCyclicWidget* branchcyclicwidget;

	cSpectrumSceneWidget* spectrumscene;

	cTheoreticalSpectrum* theoreticalspectrum;
	
	int rowid;
	string scantitle;
	bool preparedToShow;

	set<string> localneutralosses;

	cGlobalPreferences* globalpreferences;
	cParameters* parameters;

	cPeakListSeries* rawdata;
	vector<cImzMLItem>* imzmlprofilemetadata;
	bool profilemz64precision;
	bool profileintensity64precision;

	cFindDialog* finddialog;
	cExportDialog* exportdialog;
	int currentfinditem;

	bool reportisomers;

	int activefileid;

	void preparePeaksTable(QRect geometry);

	string printHTMLTableCell(string text, bool red);	


signals:


	/**
		\brief The signal is emitted when the range of m/z ratios has been changed.
		\param minmz a minimum threshold of m/z ratio
		\param maxmz a maximum threshold of m/z ratio
	*/ 
	void emitMZInterval(double minmz, double maxmz);


	/**
		\brief The signal is emitted when raw data state was changed.
		\param state current state
	*/
	void rawDataStateChangedSignal(bool state);


	/**
		\brief The signal is emitted when the absolute intensity state was changed.
		\param state current state
	*/
	void absoluteIntensityStateChangedSignal(bool state);


	/**
		\brief The signal is emitted when the widget has been moved.
		\param rowid rowid of the widget
	*/
	void lastActiveDetail(int rowid);


private slots:


	void updateMZInterval(double minmz, double maxmz);


	void setMZInterval();


	void exportSpectrum();


	void exportPeptide();


	void openFindDialog();


	void openExportImageDialog();


	void exportTableToCSV();


	void movePrevious();


	void moveNext();


	void headerItemClicked(int index);


	void filterPeaksTable();


	void rawDataStateChanged(bool state);


	void absoluteIntensityStateChanged(bool state);


	void hideMatchedPeaks(bool hide);


	void hideUnmatchedPeaks(bool hide);


	void hideScrambledPeaks(bool hide);


	void hidePeakLabels(bool hide);


	void filterTableAfterIonSeriesChanged(int index);


	void filterTableAfterNeutralLossChanged(int index);


	void filterTableAfterRotationChanged(int index);


	void filterTableAfterTRotationChanged(int index);


	void showIsomersStateChanged();


	void showHTMLDocumentation();


	void closeWindow();

	
	void tableDoubleClicked(const QModelIndex& index);

};

#endif

