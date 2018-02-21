/**
	\file cSpectrumDetailWidget.h
	\brief The widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/


#ifndef _CSPECTRUMDETAILWIDGET_H
#define _CSPECTRUMDETAILWIDGET_H

#include <QWidget>
#include <QMainWindow>
#include <QTextDocument>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressDialog>
#include <QTextStream>
#include "core/cAllocator.h"
#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"
#include "gui/cBranchedWidget.h"
#include "gui/cBranchCyclicWidget.h"
#include "gui/cSpectrumSceneWidget.h"
#include "gui/cFindDialog.h"
#include "gui/cExportDialog.h"
#include "gui/cEventFilter.h"
#include "gui/cDelegate.h"


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
class QToolBar;
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
		\param parameters a pointer to parameters
		\param theoreticalspectrum a reference to a theoretical spectrum
		\param parent pointer to a parent widget
	*/ 
	void initialize(cParameters* parameters, cTheoreticalSpectrum& theoreticalspectrum, QWidget* parent);


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
		\retval string a table of peaks as a HTML string
	*/ 
	string getPeaksTableAsHTMLString(bool unmatchedtheoreticalpeaks, bool unmatchedexperimentalpeaks);


	/**
		\brief Get a partial table of peaks as a HTML string.
		\param id identifier of a spectrum
		\retval string a partial table of peaks as a HTML string
	*/ 
	string getPartialPeaksTableAsHTMLString(int id);


	/**
		\brief Prepare the widget to show.
		\param peptidetype a type of peptide
	*/ 
	void prepareToShow(ePeptideType peptidetype);
	

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


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:
	QWidget* parent;

	QToolBar* toolbarExport;
	QToolBar* toolbarFind;
	QToolBar* toolbarZoom;
	QToolBar* toolbarHide;
	QToolBar* toolbarMz;
	QToolBar* toolbarRotation;
	QToolBar* toolbarTrotation;

	QAction* actionExportTable;
	QAction* actionExportSpectrum;
	QAction* actionFind;
	QAction* actionPrevious;
	QAction* actionNext;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionHideMatched;
	QAction* actionHideUnmatched;
	QAction* actionHideScrambled;
	QAction* actionMouseMzSelection;

	QSplitter* hsplitter1;
	QSplitter* hsplitter2;
	QSplitter* vsplitter;

	QWidget* widgetmz;
	QHBoxLayout* hboxmz;
	QLabel* labelmz;
	QDoubleSpinBox* minmz;
	QDoubleSpinBox* maxmz;
	QPushButton* setmzinterval;
	QPushButton* resetmzinterval;

	QWidget* widgetrotation;
	QHBoxLayout* hboxrotation;
	QLabel* labelrotation;
	QComboBox* rotation;

	QWidget* widgettrotation;
	QHBoxLayout* hboxtrotation;
	QLabel* labeltrotation;
	QComboBox* trotation;

	QTextBrowser* textbrowser;

	QTableWidget* peakstable;
	vector<int> peakstableheadersort;
	cAllocator<QTableWidgetItem> widgetitemallocator;
	vector<cTablePosition> tablematches;
	cDelegate columndelegate;

	cLinearWidget* linearwidget;
	cCyclicWidget* cyclicwidget;
	cBranchedWidget* branchedwidget;
	cBranchCyclicWidget* branchcyclicwidget;

	cSpectrumSceneWidget* spectrumscene;

	cTheoreticalSpectrum* theoreticalspectrum;

	bool preparedToShow;
	cParameters* parameters;

	cFindDialog* finddialog;
	cExportDialog* exportdialog;
	int currentfinditem;


	void preparePeaksTable();

	string printHTMLTableCell(string text, bool red);	


signals:

	/**
		\brief The signal is emitted when the range of m/z ratios has been changed.
		\param minmz a minimum threshold of m/z ratio
		\param maxmz a maximum threshold of m/z ratio
	*/ 
	void emitMZInterval(double minmz, double maxmz);


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

	void headerItemDoubleClicked(int);

	void filterPeaksTable();

	void hideMatchedPeaks(bool hide);

	void hideUnmatchedPeaks(bool hide);

	void hideScrambledPeaks(bool hide);

};

#endif

