/**
	\file cCalibrationChart.h
	\brief Calibration chart.
*/


#ifndef _CCALIBRATIONCHART_H
#define _CCALIBRATIONCHART_H

#include <QWidget>
#include <QDesktopServices>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QUrl>
#include <QFileInfo>
#include <QMainWindow>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QItemDelegate>
#include <QApplication>
#include "core/cGlobalPreferences.h"
#include "gui/cCalibrationChartScene.h"
#include "gui/cEventFilter.h"
#include "gui/cCalibrationChartProxyModel.h"


// forward declaration
class QHBoxLayout;
//class QVBoxLayout;
class QPushButton;
class QSplitter;
class QDoubleSpinBox;
class QAction;
class QLabel;
class QTableView;


/**
	\brief Calibration chart.
*/
class cCalibrationChart : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	cCalibrationChart(cGlobalPreferences* globalpreferences, QWidget *parent);


	/**
		\brief The destructor.
	*/ 
	~cCalibrationChart();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Apply new global preferences.
		\param globalpreferences global preferences of the application
	*/
	void applyGlobalPreferences(cGlobalPreferences* globalpreferences);


	/**
		\brief Set parameters of calibration curve.
		\param equationtype 0 (y = a*x), 1 (y = a*x + b), 2 (manual mode)
		\param a a
		\param b b
		\param xvalues x coordinates of points
		\param yvalues y coordinates of points
		\param sd standard deviation of y-values
	*/
	void setLineParameters(int equationtype, double a, double b, vector<double> xvalues, vector<double> yvalues, vector<double> sd);


	/**
		\brief Set data points.
		\param xvalues x coordinates of points
		\param yvalues y coordinates of points
		\param sd standard deviation of y-values
		\param datagroups vector of group ids
	*/
	void setData(vector<double> xvalues, vector<double> yvalues, vector<double> sd, vector<int> datagroups);


	/**
		\brief Create the table content.
		\param a a
		\param b b
		\param calibrationxvalues x coordinates of calibration points
		\param calibrationyvalues y coordinates of calibration points
		\param calibrationsd standard deviation of y-values of calibration points
		\param datagroups ids of groups of data points
		\param dataxvalues x coordinates of data points
		\param datayvalues y coordinates of data points
		\param datasd standard deviation of y-values of data points
	*/
	void createTable(double a, double b, vector<double>& calibrationxvalues, vector<double>& calibrationyvalues, vector<double>& calibrationsd, vector<int>& datagroups, vector<double>& dataxvalues, vector<double>& datayvalues, vector<double>& datasd);


	/**
		\brief Delete the table content.
	*/
	void deleteTable();


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:

	cGlobalPreferences* globalpreferences;
	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuView;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QToolBar* toolbarView;
	QToolBar* toolbarHelp;
	QToolBar* toolbarConcentration;

	QAction* actionExportCSV;
	QAction* actionExportSpectrum;
	QAction* actionCloseWindow;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionHideLabels;
	//QAction* actionMouseConcentrationSelection;
	QAction* actionHTMLDocumentation;

	QWidget* widgetconcentration;
	QHBoxLayout* hboxconcentration;
	QLabel* labelconcentration;
	QDoubleSpinBox* minconcentration;
	QLabel* labelseparator;
	QDoubleSpinBox* maxconcentration;
	QPushButton* setconcentrationinterval;
	QPushButton* resetconcentrationinterval;

	cCalibrationChartScene* chartscene;

	QTableView* database;
	QStandardItemModel* databasemodel;
	cCalibrationChartProxyModel* proxymodel;

	QSplitter* vsplitter;

	QString lastdirexporttocsv;

signals:


	/**
		\brief The signal is emitted when the range of concentration is changed.
		\param minconcentration a minimum threshold of concentration
		\param maxconcentration a maximum threshold of concentration
	*/ 
	void emitConcentrationInterval(double minconcentration, double maxconcentration);


private slots:


	void exportToCsv();


	void updateConcentrationInterval(double minconcentration, double maxconcentration);


	void setConcentrationInterval();


	void exportScene();


	void hideLabels(bool state);


	void showHTMLDocumentation();


	void closeWindow();

};

#endif

