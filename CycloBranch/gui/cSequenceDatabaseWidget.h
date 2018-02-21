/**
	\file cSequenceDatabaseWidget.h
	\brief Visualization of the database of sequences.
*/


#ifndef _CSEQUENCEDATABASEWIDGET_H
#define _CSEQUENCEDATABASEWIDGET_H

#include <QWidget>
#include <fstream>
#include "core/cSequenceDatabase.h"

using namespace std;


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QDialogButtonBox;
class QPushButton;
class QLabel;
class QComboBox;
class QProgressDialog;


/**
	\brief The widget representing the dialog 'About'.
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
	cSequenceDatabase sequences;

	vector<int> headersort;

	void deleteTable(bool enableprogress);

	void removeRow(int row);

	bool checkTable();

	bool checkFormula(int row, string& summary);

	bool checkSequence(int row);

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