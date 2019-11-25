/**
	\file cSequenceDatabaseWidget.h
	\brief An editor of sequences database.
*/


#ifndef _CSEQUENCEDATABASEWIDGET_H
#define _CSEQUENCEDATABASEWIDGET_H

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
#include <QComboBox>
#include <fstream>
#include "core/utilities.h"
#include "core/cSequenceDatabase.h"
#include "gui/cSequenceDatabaseProxyModel.h"
#include "gui/cViewButtonDelegate.h"
#include "gui/cCheckBoxDelegate.h"
#include "gui/cComboBoxDelegate.h"


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QLineEdit;
class QMenuBar;
class QMenu;


/**
	\brief An editor of sequences database.
*/
class cSequenceDatabaseWidget : public QMainWindow
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
		\brief Import a new sequence.
		\param peptidetypeindex an index of current peptide type
		\param sequence sequence
	*/ 
	void importSequence(int peptidetypeindex, QString sequence);


private:

	QString editorname;

	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuEdit;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QAction* actionNewDatabase;
	QAction* actionOpenDatabase;
	QAction* actionSaveDatabase;
	QAction* actionSaveDatabaseAs;
	QAction* actionImportDatabase;
	QAction* actionCloseWindow;

	QToolBar* toolbarEdit;
	QAction* actionAddRow;
	QAction* actionRemoveSelectedRows;
	QAction* actionSelectAll;
	QAction* actionUnselectAll;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	QToolBar* toolbarFilter;
	QWidget* rowsfilterwidget;
	QHBoxLayout* rowsfilterhbox;
	QComboBox* rowsfilteroperator;
	QComboBox* rowsfiltercombobox1;
	QComboBox* rowsfiltercomparatorcombobox1;
	QLineEdit* rowsfilterline1;
	QComboBox* rowsfiltercombobox2;
	QComboBox* rowsfiltercomparatorcombobox2;
	QLineEdit* rowsfilterline2;
	QCheckBox* rowsfiltercasesensitive;
	QCheckBox* rowsfilterwholeword;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableView* database;
	QStandardItemModel* databasemodel;
	cSequenceDatabaseProxyModel* proxymodel;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString databasefile;
	QString lastdir;
	ifstream inputstream;
	ofstream outputstream;
	cSequenceDatabase sequences;

	bool datamodified;

	void deleteTable();

	void resetHeader();

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

	void openDatabase();

	bool saveDatabase();

	bool saveDatabaseAs();

	void saveDatabaseCheck();

	void addRow();

	void removeSelectedRows();

	void itemChanged(QStandardItem* item);

	void headerItemClicked(int index);

	void filterRows();

	void resetFilter();

	void createNewDatabase();

	void importDatabase();

	void selectAll();

	void unselectAll();

	void showHTMLDocumentation();

	void editItem(const QModelIndex& index);

};

#endif
