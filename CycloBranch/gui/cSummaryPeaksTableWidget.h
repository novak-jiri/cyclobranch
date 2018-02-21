/**
	\file cSummaryPeaksTableWidget.h
	\brief Summary table of matched peaks.
*/


#ifndef _CSUMMARYPEAKSTABLEWIDGET_H
#define _CSUMMARYPEAKSTABLEWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QMessageBox>
#include <QComboBox>
#include <map>
#include "core/utilities.h"
#include "gui/cViewButtonDelegate.h"
#include "gui/cSummaryPeaksTableProxyModel.h"
#include "gui/cMainWindowProxyModel.h"
#include "gui/cHTMLDelegate.h"

using namespace std;


// forward declaration
class cParameters;
class cTheoreticalSpectrumList;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QMenuBar;
class QMenu;


/**
	\brief Key to reduce the summary table when exporting statistics for MS and MSI data.
*/
struct cSummaryTableKeyMS {

	/**
		\brief Ion type subkey.
	*/
	string iontype;


	/**
		\brief Theoretical m/z subkey.
	*/
	string theoreticalmz;


	/**
		\brief Summary formula subkey.
	*/
	string summaryformula;


	/**
		\brief Compound name subkey.
	*/
	string name;


	/**
		\brief Reference subkey.
	*/
	string reference;


	/**
		\brief Clear the structure.
	*/
	void clear() {
		iontype.clear();
		theoreticalmz.clear();
		summaryformula.clear();
		name.clear();
		reference.clear();
	}

};


/**
	\brief Key to reduce the summary table when exporting statistics for MS/MS data.
*/
struct cSummaryTableKeyMSMS {

	/**
		\brief Theoretical m/z subkey.
	*/
	string theoreticalmz;


	/**
		\brief Experimental m/z subkey.
	*/
	string experimentalmz;


	/**
		\brief Relative intensity subkey.
	*/
	string relint;


	/**
	\brief Absolute intensity subkey.
	*/
	string absint;


	/**
		\brief ppm error subkey.
	*/
	string ppmerror;


	/**
		\brief Clear the structure.
	*/
	void clear() {
		theoreticalmz.clear();
		experimentalmz.clear();
		relint.clear();
		absint.clear();
		ppmerror.clear();
	}

};


/**
	\brief Comparison object for cSummaryTableKeyMS (independent ions).
*/
struct cIonSummaryTableKeyMS_comp {

	/**
		\brief Operator to define the order of two items.
		\param left first key
		\param right second key
		\retval bool true if the first object is less than the second object
	*/
	bool operator()(const cSummaryTableKeyMS& left, const cSummaryTableKeyMS& right) const {
		return (left.iontype + left.theoreticalmz + left.summaryformula + left.name + left.reference < right.iontype + right.theoreticalmz + right.summaryformula + right.name + right.reference);
	}

};


/**
	\brief Comparison object for cSummaryTableKeyMS (whole envelope).
*/
struct cEnvelopeSummaryTableKeyMS_comp {

	/**
		\brief Operator to define the order of two items.
		\param left first key
		\param right second key
		\retval bool true if the first object is less than the second object
	*/
	bool operator()(const cSummaryTableKeyMS& left, const cSummaryTableKeyMS& right) const {
		return (left.iontype + left.name + left.reference < right.iontype + right.name + right.reference);
	}

};


/**
	\brief Comparison object for cSummaryTableKeyMSMS.
*/
struct cSummaryTableKeyMSMS_comp {

	/**
		\brief Operator to define the order of two items.
		\param left first key
		\param right second key
		\retval bool true if the first object is less than the second object
	*/
	bool operator()(const cSummaryTableKeyMSMS& left, const cSummaryTableKeyMSMS& right) const {
		return (left.theoreticalmz + left.experimentalmz + left.relint + left.absint + left.ppmerror < right.theoreticalmz + right.experimentalmz + right.relint + right.absint + right.ppmerror);
	}

};


/**
	\brief Summary table of matched peaks.
*/
class cSummaryPeaksTableWidget : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cSummaryPeaksTableWidget(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cSummaryPeaksTableWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Prepare the widget to show.
		\param resultsstandardmodel standard model of the tableview in the main application window 
		\param resultsproxymodel proxy model of the tableview in the main application window 
		\param parameters parameters of the application
		\param spectralist list of spectra
		\param showisomers true if isomers of blocks are reported; false otherwise
		\retval bool true if the table was successfully prepared, false otherwise
	*/ 
	bool prepareToShow(QStandardItemModel* resultsstandardmodel, cMainWindowProxyModel* resultsproxymodel, cParameters* parameters, cTheoreticalSpectrumList* spectralist, bool showisomers);


	/**
		\brief Delete the table content.
	*/ 
	void deleteTable();


	/**
		\brief Update the filter using an image region.
		\param xmin minimum x coordinate
		\param xmax maximum x coordinate
		\param ymin minimum y coordinate
		\param ymax maximum y coordinate
	*/ 
	void updateFilterBySelectedRegion(int xmin, int xmax, int ymin, int ymax);


private:

	cParameters* parameters;
	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QAction* actionExportCSV;
	QAction* actionExportStatistics;
	QAction* actionCloseWindow;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QToolBar* toolbarFilter;
	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QComboBox* rowsfiltercombobox;
	QComboBox* rowsfiltercomparatorcombobox;
	QLineEdit* rowsfilterline;
	QCheckBox* rowsfiltercasesensitive;
	QCheckBox* rowsfilterwholeword;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableView* database;
	QStandardItemModel* databasemodel;
	cSummaryPeaksTableProxyModel* proxymodel;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString lastdirexporttocsv;
	QString lastdirexportstatisticstocsv;

	vector<cCoordinates> coordinates;
	cPeaksList origeicchromatogram;

	void addEICPeak(cPeaksList& eicchromatogram, cPeaksList& experimentalspectrum);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void closeWindow();


	void filterRows();


	void resetFilter();


	void exportToCsv();


	void exportStatistics();


	void showHTMLDocumentation();


	void rowDoubleClicked(const QModelIndex& item);


signals:


	/**
		\brief The table was not generated because the process was cancelled.
	*/
	void tableCancelled();


	/**
		\brief A row was double clicked.
		\param rowid id of a row in the main window
	*/
	void summaryPeaksTableRowDoubleClicked(int rowid);


	/**
		\brief Reset the region in imagw window.
	*/ 
	void resetRegion();

	
	/**
		\brief Send the vector of coordinates to image window.
		\param coordinates a vector of coordinates x and y
		\param columnname name of column which was compared
		\param comparatorname name of used comparator
		\param filterstring a string used to filter the points
		\param casesensitive true if the string was used as a casesensitive, false otherwise
		\param wholeword true if whole words only are compared, false otherwise
	*/ 
	void sendFilterOptionsToImageWindow(vector<cCoordinates> coordinates, string columnname, string comparatorname, string filterstring, bool casesensitive, bool wholeword);


	/**
		\brief Send the EIC chromatogram to chromatogram window.
		\param eic eic chromatogram
	*/
	void sendFilterOptionsToChromatogram(cPeaksList eic);


};

#endif
