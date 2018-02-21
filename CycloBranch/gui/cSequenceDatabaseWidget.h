/**
	\file cSequenceDatabaseWidget.h
	\brief Visualization of the database of sequences.
*/


#ifndef _CSEQUENCEDATABASEWIDGET_H
#define _CSEQUENCEDATABASEWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <fstream>
#include "core/utilities.h"
#include "core/cSequenceDatabase.h"
#include "core/cAllocator.h"
#include "gui/cDelegate.h"


using namespace std;
using namespace boost;


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class QLineEdit;
class QCheckBox;


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

	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QLineEdit* rowsfilterline;
	QCheckBox* rowsfiltercasesensitive;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableWidget* database;
	QHBoxLayout* buttons;
	QVBoxLayout* mainlayout;

	QString databasefile;
	QString lastdir;
	ifstream inputstream;
	ofstream outputstream;
	cSequenceDatabase sequences;

	vector<int> headersort;
	cDelegate columndelegate;

	cAllocator<QTableWidgetItem> widgetitemallocator;

	bool datamodified;

	void deleteTable(bool enableprogress);

	void removeRow(int row);

	bool checkTable();

	bool checkFormula(int row, const string& summary);

	bool checkSequence(int row);

	void setDataModified(bool datamodified);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void closeWindow();

	void loadDatabase();

	bool saveDatabase();

	bool saveDatabaseAs();

	void addRow();

	void removeCheckedRows();

	void itemChanged(QTableWidgetItem* item);

	void headerItemDoubleClicked(int index);

	void filterRows();

	void resetFilter();

	void comboBoxModified(int index);

};

#endif
