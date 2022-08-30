/**
	\file cSummaryPeaksTableWidget.h
	\brief Summary table of matched peaks.
*/


#ifndef _CSUMMARYPEAKSTABLEWIDGET_H
#define _CSUMMARYPEAKSTABLEWIDGET_H

#include <map>
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
#include <QStringListModel>
#include <QCompleter>

#include "core/utilities.h"
#include "core/cGlobalPreferences.h"
#include "gui/cViewButtonDelegate.h"
#include "gui/cSummaryPeaksTableProxyModel.h"
#include "gui/cMainWindowProxyModel.h"
#include "gui/cHTMLDelegate.h"
#include "gui/cPubChemSearchWidget.h"
#include "gui/cEventFilter.h"


// forward declaration
class cParameters;
class cTheoreticalSpectrum;
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
		\brief Identification ID subkey.
	*/
	string id;


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
		\brief Score subkey.
	*/
	string score;


	/**
		\brief FDR subkey.
	*/
	string fdr;


	/**
		\brief Clear the structure.
	*/
	void clear() {
		id.clear();
		iontype.clear();
		theoreticalmz.clear();
		summaryformula.clear();
		name.clear();
		reference.clear();
		score.clear();
		fdr.clear();
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
		\brief Theoretical intensity subkey.
	*/
	string thint;


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
		\brief summary formula subkey.
	*/
	string summary;


	/**
		\brief Clear the structure.
	*/
	void clear() {
		theoreticalmz.clear();
		thint.clear();
		experimentalmz.clear();
		relint.clear();
		absint.clear();
		ppmerror.clear();
		summary.clear();
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
		return (left.id + left.iontype + left.name + left.reference < right.id + right.iontype + right.name + right.reference);
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
		return (left.theoreticalmz + left.thint + left.experimentalmz + left.relint + left.absint + left.ppmerror + left.summary < right.theoreticalmz + right.thint + right.experimentalmz + right.relint + right.absint + right.ppmerror + right.summary);
	}

};


/**
	\brief Add a peak to an extracted ion chromatogram.
	\param parameters application parameters
	\param eicchromatogram EIC chromatogram
	\param theoreticalspectrum theoretical spectrum
	\param experimentalspectrum experimental spectrum
*/
void addEICPeak(cParameters* parameters, cPeaksList& eicchromatogram, cTheoreticalSpectrum& theoreticalspectrum, cPeaksList& experimentalspectrum);


/**
	\brief Summary table of matched peaks.
*/
class cSummaryPeaksTableWidget : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/ 
	cSummaryPeaksTableWidget(cGlobalPreferences* globalpreferences, QWidget* parent = (QWidget *)0);


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
		\param listoftheoreticalspectra lists of theoretical spectra
		\param activefileid id of the active file
		\param showisomers true if isomers of blocks are reported; false otherwise
		\retval bool true if the table was successfully prepared, false otherwise
	*/ 
	bool prepareToShow(QStandardItemModel* resultsstandardmodel, cMainWindowProxyModel* resultsproxymodel, cParameters* parameters, cTheoreticalSpectrumList* listoftheoreticalspectra, int activefileid, bool showisomers);


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


	/**
		\brief Apply new global preferences.
		\param globalpreferences global preferences of the application
	*/
	void applyGlobalPreferences(cGlobalPreferences* globalpreferences);


	/**
		\brief Get the reference to cPubChemSearchWidget.
		\retval cPubChemSearchWidget reference to cPubChemSearchWidget
	*/
	cPubChemSearchWidget* getPubChemSearchWidget();


	/**
		\brief Filter table rows.
	*/
	void filterTablerows();


	/**
		\brief Filter a specific compound.
		\param name compound name
		\param iontype ion type
		\param datatypeview type of data shown
		\param mzstr theoretical m/z value converted to a string
	*/
	void filterCompound(string name, string iontype, int datatypeview, string mzstr);


private:

	QString title;

	cGlobalPreferences* globalpreferences;

	cParameters* parameters;
	QWidget* parent;

	cPubChemSearchWidget* pubchemsearchwidget;

	QMenuBar* menuBar;
	QMenu* menuFile;
	//QMenu* menuSearch;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QAction* actionExportCSV;
	QAction* actionExportDatabase;
	QAction* actionExportStatistics;
	QAction* actionCloseWindow;

	//QToolBar* toolbarSearch;
	//QAction* actionSearchPubChem;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QToolBar* toolbarFilter;
	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;

	QComboBox* rowsfilteroperator;

	QComboBox* rowsfiltercombobox1;
	QComboBox* rowsfiltercomparatorcombobox1;
	QStringListModel* rowsfilterstringlistmodel1;
	QCompleter* rowsfilterlinecompleter1;
	QLineEdit* rowsfilterline1;
	QPushButton* rowsfilterleft1;
	QPushButton* rowsfilterright1;

	QComboBox* rowsfiltercombobox2;
	QComboBox* rowsfiltercomparatorcombobox2;
	QStringListModel* rowsfilterstringlistmodel2;
	QCompleter* rowsfilterlinecompleter2;
	QLineEdit* rowsfilterline2;
	QPushButton* rowsfilterleft2;
	QPushButton* rowsfilterright2;

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
	QString lastdirexportdatabase;
	QString lastdirexportstatisticstocsv;

	vector<double> rtimes;

	vector<cCoordinateInfo> origcoordinateinfo;
	cPeaksList origeicchromatogram;

	int activefileid;

	void addCoordinateInfo(int spectrumindex, vector<cCoordinateInfo>& coordinateinfo, cTheoreticalSpectrum& theoreticalspectrum, cPeaksList& experimentalspectrum);


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


	void exportToDatabase();


	void exportStatistics();


	void showHTMLDocumentation();


	void rowDoubleClicked(const QModelIndex& item);


	void rowsFilterLeft1Slot();


	void rowsFilterRight1Slot();


	void rowsFilterLeft2Slot();


	void rowsFilterRight2Slot();


	void searchPubChem();


signals:


	/**
		\brief The table was not generated because the process was cancelled.
	*/
	void tableCancelled();


	/**
		\brief A row was double clicked.
		\param rowid id of a row in the main window
		\param experimentalmz experimental mz of a peak
	*/
	void summaryPeaksTableRowDoubleClicked(int rowid, double experimentalmz);


	/**
		\brief Reset the region in image window.
	*/ 
	void resetRegion();

	
	/**
		\brief Send the vector of aggregated informations about coordinates to image window.
		\param coordinateinfo a vector of aggregated informations about coordinates
		\param operatortype the type of operator (0 = or; 1 = and)
		\param columnname1 name of column which was compared
		\param comparatorname1 name of used comparator
		\param filterstring1 a string used to filter the points
		\param columnname2 name of column which was compared
		\param comparatorname2 name of used comparator
		\param filterstring2 a string used to filter the points
		\param casesensitive true if the string was used as a casesensitive, false otherwise
		\param wholeword true if whole words only are compared, false otherwise
	*/ 
	void sendFilterOptionsToImageWindow(vector<cCoordinateInfo> coordinateinfo, bool operatortype, string columnname1, string comparatorname1, string filterstring1, string columnname2, string comparatorname2, string filterstring2, bool casesensitive, bool wholeword);


	/**
		\brief Send the EIC chromatogram to chromatogram window.
		\param eic eic chromatogram
	*/
	void sendFilterOptionsToChromatogram(cPeaksList eic);


};

#endif

