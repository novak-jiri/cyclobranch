/**
	\file cPatternSimulatorWidget.h
	\brief The implementation of an isotopic pattern simulator (window).
*/


#ifndef _CPATTERNSIMULATORWIDGET_H
#define _CPATTERNSIMULATORWIDGET_H

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
#include "gui/cPatternSimulatorSceneWidget.h"
#include "gui/cSpectrumDetailProxyModel.h"
#include "gui/cExportDialog.h"
#include "gui/cEventFilter.h"
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
class QSpinBox;
class QAction;
class QLabel;


/**
	\brief The implementation of an isotopic pattern simulator (window).
*/
class cPatternSimulatorWidget : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	cPatternSimulatorWidget(cGlobalPreferences* globalpreferences, QWidget *parent);


	/**
		\brief The destructor.
	*/ 
	~cPatternSimulatorWidget();


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
	QToolBar* toolbarMz;
	QToolBar* toolbarInput;

	QAction* actionExportTable;
	QAction* actionExportSpectrum;
	QAction* actionCloseWindow;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionAbsoluteIntensity;
	QAction* actionRawData;
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

	QWidget* widgetiontype;
	QHBoxLayout* hboxiontype;
	QLabel* labeliontype;
	QComboBox* comboboxiontype;

	QWidget* widgetformula;
	QHBoxLayout* hboxformula;
	QLabel* labelformula;
	QLineEdit* lineeditformula;

	QWidget* widgetfwhm;
	QHBoxLayout* hboxfwhm;
	QLabel* labelfwhm;
	QDoubleSpinBox* doublespinboxfwhm;

	QWidget* widgetcharge;
	QHBoxLayout* hboxcharge;
	QLabel* labelcharge;
	QSpinBox* spinboxcharge;

	QWidget* widgetabsoluteintensity;
	QHBoxLayout* hboxabsoluteintensity;
	QLabel* labelabsoluteintensity;
	QDoubleSpinBox* doublespinboxabsoluteintensity;

	QPushButton* simulatebutton;

	QTableView* peakstable;
	QStandardItemModel* peakstablemodel;
	cSpectrumDetailProxyModel* proxymodel;

	cPatternSimulatorSceneWidget* spectrumscene;

	cExportDialog* exportdialog;


	void preparePeaksTable(cPeaksList& isotopepattern);


	void deleteTable();


signals:


	/**
		\brief The signal is emitted when the range of m/z ratios is changed.
		\param minmz a minimum threshold of m/z ratio
		\param maxmz a maximum threshold of m/z ratio
	*/ 
	void emitMZInterval(double minmz, double maxmz);


	/**
		\brief The signal is emitted when raw data state was changed.
		\param state current state
	*/
	void rawDataStateChangedSignal(bool state);


private slots:


	void simulateButtonReleased();


	void updateMZInterval(double minmz, double maxmz);


	void setMZInterval();


	void exportSpectrum();


	void exportTableToCSV();


	void rawDataStateChanged(bool state);


	void hidePeakLabels(bool hide);


	void showHTMLDocumentation();


	void closeWindow();

};

#endif

