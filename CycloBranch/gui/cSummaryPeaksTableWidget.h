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
//#include <fstream>
#include "core/utilities.h"
#include "core/cAllocator.h"
#include "gui/cDelegate.h"

using namespace std;


// forward declaration
class cParameters;
class cTheoreticalSpectrumList;
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class QCheckBox;
class QLineEdit;


/**
	\brief Summary table of matched peaks.
*/
class cSummaryPeaksTableWidget : public QWidget
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
		\param tablewidget a pointer to the main window table widget with the results 
		\param parameters parameters of the application
		\param spectralist list of spectra
	*/ 
	void prepareToShow(QTableWidget* tablewidget, cParameters* parameters, cTheoreticalSpectrumList* spectralist);


	/**
		\brief Delete the table content.
		\param enableprogress if true, the progress dialog is shown; if false, the dialog is hidden
	*/ 
	void deleteTable(bool enableprogress);


private:

	cParameters* parameters;
	QWidget* parent;
	QPushButton* close;
	QPushButton* exportcsv;

	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QLineEdit* rowsfilterline;
	QCheckBox* rowsfiltercasesensitive;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableWidget* database;
	QHBoxLayout* buttons;
	QVBoxLayout* mainlayout;

	vector<int> headersort;
	cDelegate columndelegate;

	QString lastdirexporttocsv;

	cAllocator<QTableWidgetItem> widgetitemallocator;


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void closeWindow();


	void headerItemDoubleClicked(int index);


	void filterRows();


	void resetFilter();


	void exportToCsv();


signals:


	/**
		\brief The table was not generated because the process was cancelled.
	*/ 
	void tableCancelled();


};

#endif
