/**
	\file cBricksDatabaseWidget.h
	\brief Visualization of the database of building blocks.
*/


#ifndef _CBRICKSDATABASEWIDGET_H
#define _CBRICKSDATABASEWIDGET_H

#include <QWidget>
#include <fstream>
#include "core/utilities.h"
#include "core/cBricksDatabase.h"
#include "core/cAllocator.h"

using namespace std;


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;


/**
	\brief Number of occurrences of a char in a string.
	\param s string
	\param c char
	\retval int number of occurrences of \a c in \a s
*/
int numberOfOccurrences(const string& s, char c);


/**
	\brief Visualization of the database of building blocks.
*/
class cBricksDatabaseWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cBricksDatabaseWidget(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cBricksDatabaseWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


private:
	QWidget* parent;
	QPushButton* insertrow;
	QPushButton* removechecked;
	QPushButton* close;
	QPushButton* load;
	QPushButton* save;
	QPushButton* saveas;
	QTableWidget* database;
	QHBoxLayout* buttons;
	QVBoxLayout* mainlayout;

	QString databasefile;
	QString lastdir;
	ifstream inputstream;
	ofstream outputstream;
	cBricksDatabase bricks;

	vector<int> headersort;

	cAllocator<QTableWidgetItem> widgetitemallocator;

	void deleteTable(bool enableprogress);

	void removeRow(int row);

	bool checkTable();

	bool checkFormula(int row, const string& summary);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void closeWindow();

	void loadDatabase();

	void saveDatabase();

	void saveDatabaseAs();

	void addRow();

	void removeEmptyRows();

	void itemChanged(QTableWidgetItem* item);

	void headerItemDoubleClicked(int index);

};

#endif
