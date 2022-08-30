/**
	\file cFormulaPredictor.h
	\brief Predicts the molecular formula from the exact mass.
*/


#ifndef _CFORMULAPREDICTOR_H
#define _CFORMULAPREDICTOR_H

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
#include "core/cPeaksList.h"
#include "gui/cViewButtonDelegate.h"
#include "gui/cFragmentIonsListWidget.h"
#include "gui/cNeutralLossesListWidget.h"
#include "gui/cSpectrumDetailProxyModel.h"
#include "gui/cEventFilter.h"
#include "gui/cPubChemSearchWidget.h"
#include "parallel/cFormulaPredictorThread.h"


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
class QLineEdit;


/**
	\brief Predicts the molecular formula from the exact mass.
*/
class cFormulaPredictor : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	cFormulaPredictor(cGlobalPreferences* globalpreferences, QWidget *parent);


	/**
		\brief The destructor.
	*/ 
	~cFormulaPredictor();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Apply new global preferences.
	*/
	void applyGlobalPreferences();


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:

	cGlobalPreferences* globalpreferences;
	QWidget* parent;

	cFormulaPredictorThread* formulapredictorthread;
	int chargevalue;
	int maximumcombinedelements;
	bool basicformulacheck;
	bool advancedformulacheck;
	bool noratiocheck;
	double mzvalue;
	double mzerror;
	vector<cIonType> selectedions;
	vector<string> selectedelements;
	vector<cFormulaPredictorRowItem> ionitems;
	bool success;
	bool terminatecomputation;
	string errmsg;

	map<string, int> pchemdata;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuSearch;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QToolBar* toolbarSearch;
	QToolBar* toolbarHelp;

	QAction* actionExportTable;
	QAction* actionCloseWindow;
	QAction* actionSearchPubChem; 
	QAction* actionHTMLDocumentation;

	QDoubleSpinBox* doublespinboxmz;
	QSpinBox* spinboxcharge;
	QDoubleSpinBox* doublespinboxmzerror;
	cFragmentIonsListWidget* widgetiontypes;
	cNeutralLossesListWidget* widgetelements;
	QSpinBox* spinboxmaximumcombinedelements;
	QCheckBox* checkboxbasicformula;
	QCheckBox* checkboxadvancedformula;
	QCheckBox* checkboxnoratio;
	QPushButton* predictbutton;
	QPushButton* cancelbutton;

	QFormLayout* formlayout;
	QWidget* formwidget;

	QTableView* peakstable;
	QStandardItemModel* peakstablemodel;
	cSpectrumDetailProxyModel* proxymodel;

	QSplitter* vsplitter1;
	QSplitter* vsplitter2;
	QSplitter* hsplitter;

	void preparePeaksTable();

	void deleteTable();


private slots:


	void predictButtonReleased();


	void cancelButtonReleased();


	void exportTableToCSV();


	void showHTMLDocumentation();


	void closeWindow();


	void resetIonTypes();


	void formulaPredictorThreadFinished();


	void searchPubChem();

};

#endif

