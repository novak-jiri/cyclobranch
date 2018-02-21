/**
	\file cSequenceDatabaseWidget.h
	\brief Visualization of the database of sequences.
*/


#ifndef _CSEQUENCEDATABASEWIDGET_H
#define _CSEQUENCEDATABASEWIDGET_H

#include <QWidget>
#include <fstream>
#include "core/utilities.h"
#include "core/cSequenceDatabase.h"
#include "core/cAllocator.h"

using namespace std;
using namespace boost;


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;


/**
	\brief Visualization of the database of sequences.
*/
class cSequenceDatabaseWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cSequenceDatabaseWidget(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cSequenceDatabaseWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Insert a new row, set its peptide type and sequence.
		\param peptidetypeindex an index of current peptide type
		\param sequence sequence
	*/ 
	void insertRow(int peptidetypeindex, QString sequence);


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
	cSequenceDatabase sequences;

	vector<int> headersort;

	cAllocator<QTableWidgetItem> widgetitemallocator;

	void deleteTable(bool enableprogress);

	void removeRow(int row);

	bool checkTable();

	bool checkFormula(int row, const string& summary);

	bool checkSequence(int row);


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
