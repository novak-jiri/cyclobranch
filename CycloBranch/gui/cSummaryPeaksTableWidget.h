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
#include "core/utilities.h"
#include "gui/cViewButtonDelegate.h"
#include "gui/cSummaryPeaksTableProxyModel.h"
#include "gui/cMainWindowProxyModel.h"

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
	*/ 
	void prepareToShow(QStandardItemModel* resultsstandardmodel, cMainWindowProxyModel* resultsproxymodel, cParameters* parameters, cTheoreticalSpectrumList* spectralist);


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
	QAction* actionCloseWindow;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QToolBar* toolbarFilter;
	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QLineEdit* rowsfilterline;
	QCheckBox* rowsfiltercasesensitive;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableView* database;
	QStandardItemModel* databasemodel;
	cSummaryPeaksTableProxyModel* proxymodel;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString lastdirexporttocsv;

	vector<cCoordinates> coordinates;
	vector<cCoordinates> coordinates_orig;


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


	void showHTMLDocumentation();


signals:


	/**
		\brief The table was not generated because the process was cancelled.
	*/ 
	void tableCancelled();

	
	/**
		\brief Send the vector of coordinates.
		\param coordinates a vector of coordinates x and y
	*/ 
	void sendCoordinates(vector<cCoordinates> coordinates);


};

#endif
