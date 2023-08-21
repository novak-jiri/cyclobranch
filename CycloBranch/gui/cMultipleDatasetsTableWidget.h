/**
	\file cMultipleDatasetsTableWidget.h
	\brief Aggregated results from multiple datasets.
*/


#ifndef _CMULTIPLEDATASETSTABLEWIDGET_H
#define _CMULTIPLEDATASETSTABLEWIDGET_H

#include <QDesktopServices>
#include <QUrl>
#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QItemDelegate>
#include <QMessageBox>
#include <QApplication>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>

#include "core/utilities.h"
#include "core/cGlobalPreferences.h"
#include "gui/cMultipleDatasetsTableProxyModel.h"
#include "gui/cPubChemSearchWidget.h"
#include "gui/cEventFilter.h"
#include "gui/cChromatogramWindowWidget.h"
#include "gui/cDefineCalibrationWidget.h"
#include "gui/cCalibrationChart.h"


// forward declarations
class cParameters;
class cTheoreticalSpectrumList;
class QVBoxLayout;
class QMenuBar;
class QMenu;


/**
	\brief Calculate parameter a of a line.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param a output value a
*/
void calculateLineParameterA(vector<double>& x, vector<double>& y, double& a);


/**
	\brief Calculate parameters a and b of a line.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param a output value a
	\param b output value b
*/
void calculateLineParameters(vector<double>& x, vector<double>& y, double& a, double& b);


/**
	\brief Calculate parameters a and b of a line.
	\param x input vector of x-coordinates
	\param y input vector of y-coordinates
	\param iterations number of iterations
	\param a output value a
	\param b output value b
*/
void calculateLineParametersWithIterations(vector<double>& x, vector<double>& y, int iterations, double& a, double& b);


/**
	\brief Aggregated results from multiple datasets.
*/
class cMultipleDatasetsTableWidget : public QMainWindow {

	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/ 
	cMultipleDatasetsTableWidget(cGlobalPreferences* globalpreferences, QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cMultipleDatasetsTableWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Prepare the widget to show.
		\param parameters parameters of the application
		\param listoftheoreticalspectra lists of theoretical spectra
		\param activefileid id of the active file
		\retval bool true if the table was successfully prepared, false otherwise
	*/ 
	bool prepareToShow(cParameters* parameters, cTheoreticalSpectrumList* listoftheoreticalspectra, int activefileid);


	/**
		\brief Update the table if attributes were changed.
	*/
	void updateTable();


	/**
		\brief Reset the compound standard.
	*/
	void resetStandard();


	/**
		\brief Update the list of compound standards.
	*/
	void updateListofStandards();


	/**
		\brief Delete the table content.
	*/ 
	void deleteTable();


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
		\brief Get the reference to cDefineCalibrationWidget.
		\retval cDefineCalibrationWidget reference to cDefineCalibrationWidget
	*/
	cDefineCalibrationWidget* getDefineCalibrationWidget();


	/**
		\brief Get the reference to cCalibrationChart widget.
		\retval cCalibrationChart reference to cCalibrationChart
	*/
	cCalibrationChart* getCalibrationChartWidget();


private:

	QString title;

	cGlobalPreferences* globalpreferences;
	QWidget* parent;

	cParameters* parameters;
	cTheoreticalSpectrumList* listoftheoreticalspectra;

	cPubChemSearchWidget* pubchemsearchwidget;
	cDefineCalibrationWidget* definecalibrationwidget;
	cCalibrationChart* calibrationchartwidget;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuSearch;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QAction* actionExportCSV;
	QAction* actionExportDatabase;
	QAction* actionCloseWindow;

	QToolBar* toolbarView;

	QToolBar* toolbarCalibration;

	QToolBar* toolbarSearch;
	QAction* actionSearchPubChem;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QLabel* datatypelabel;
	QComboBox* comboboxdatatype;
	QHBoxLayout* datatypehboxlayout;
	QWidget* datatypewidget;

	QLabel* peakshapelabel;
	QComboBox* comboboxpeakshape;
	QHBoxLayout* peakshapehboxlayout;
	QWidget* peakshapewidget;

	QLabel* standardlabel;
	QComboBox* comboboxstandard;
	QHBoxLayout* standardhboxlayout;
	QWidget* standardwidget;

	QLabel* calibrationlabely;
	QDoubleSpinBox* calibrationspinboxa;
	QLabel* calibrationlabelx;
	QDoubleSpinBox* calibrationspinboxb;
	QPushButton* calibrationsetbutton;
	QPushButton* calibrationdefinebutton;
	QPushButton* calibrationviewbutton;
	QHBoxLayout* calibrationhboxlayout;
	QWidget* calibrationwidget;

	QLabel* columnsizelabel;
	QSpinBox* columnsizespinbox;
	QPushButton* columnsizesetbutton;
	QPushButton* columnsizeresetbutton;
	QHBoxLayout* columnsizehboxlayout;
	QWidget* columnsizewidget;

	QTableView* database;
	QStandardItemModel* databasemodel;
	cMultipleDatasetsTableProxyModel* proxymodel;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString lastdirexporttocsv;
	QString lastdirexportdatabase;

	map<string, map<int, pair<int, int> > > identifieditems;
	map<string, multimap<int, pair<int, int> > > identifieditemswithmultimap;

	int activefileid;
	int datatypeview;
	int peakshape;
	int standard;
	double calibrationa;
	double calibrationb;
	int systemcolumns;

	void getEICFromMultiMap(int datafileid, multimap<int, pair<int, int> >& compoundmultimap, cPeaksList& eicchromatogram, bool highestpeaks);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void closeWindow();


	void exportToCsv();


	void exportToDatabase();


	void dataTypeViewChanged(int index);
	

	void peakShapeChanged(int index);


	void standardChanged(int index);


	void calibrationSetButtonReleased();


	void calibrationDefineButtonReleased();


	void calibrationViewButtonReleased();


	void calibrationCurveRedefined();


	void showHTMLDocumentation();


	void rowClicked(const QModelIndex& item);


	void searchPubChem();


	void setColumnSize();


	void resetColumnSize();


signals:


	/**
		\brief The table was not generated because the process was cancelled.
	*/
	void tableCancelled();


	/**
		\brief Update chromatogram via summary table of matched peaks.
	*/
	void filterSummaryTable();


	/**
		\brief Process a row click event.
		\param datasetid id of a dataset
		\param spectrumid id of a spectrum
		\param peakid id of a peak
		\param datatypeview type of data shown
		\param peakshape type of chromatographic peak
		\param name compound name
		\param iontype ion type
		\param mzstr theoretical m/z value converted to a string
	*/
	void multipleDatasetsTableRowClicked(int datasetid, int spectrumid, int peakid, int datatypeview, int peakshape, string name, string iontype, string mzstr);

};

#endif

