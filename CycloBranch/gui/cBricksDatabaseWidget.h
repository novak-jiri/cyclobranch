/**
	\file cBricksDatabaseWidget.h
	\brief Visualization of the database of building blocks.
*/


#ifndef _CBRICKSDATABASEWIDGET_H
#define _CBRICKSDATABASEWIDGET_H

#include <QWidget>
#include <fstream>
#include "core/cBricksDatabase.h"

using namespace std;


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QDialogButtonBox;
class QPushButton;
class QLabel;
class QProgressDialog;


/**
	\brief Number of occurrences of a char in a string.
	\param s string
	\param c char
	\retval int number of occurrences of \a c in \a s
*/
int numberOfOccurrences(string& s, char c);


/**
	\brief The widget representing the dialog 'About'.
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
	QProgressDialog* progress;

	QString databasefile;
	QString lastdir;
	ifstream inputstream;
	ofstream outputstream;
	cBricksDatabase bricks;

	vector<int> headersort;

	void deleteTable(bool enableprogress);

	void removeRow(int row);

	bool checkTable();

	bool checkFormula(int row, string& summary);

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