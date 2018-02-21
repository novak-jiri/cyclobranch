/**
	\file cBricksDatabaseWidget.h
	\brief An editor of building blocks database.
*/


#ifndef _CBRICKSDATABASEWIDGET_H
#define _CBRICKSDATABASEWIDGET_H

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
#include <fstream>
#include "core/utilities.h"
#include "core/cBricksDatabase.h"
#include "gui/cBricksDatabaseProxyModel.h"
#include "gui/cMultipleButtonDelegate.h"
#include "gui/cCheckBoxDelegate.h"


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QLineEdit;
class QMenuBar;
class QMenu;


/**
	\brief Number of occurrences of a char in a string.
	\param s string
	\param c char
	\retval int number of occurrences of \a c in \a s
*/
int numberOfOccurrences(const string& s, char c);


/**
	\brief An editor of building blocks database.
*/
class cBricksDatabaseWidget : public QMainWindow
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
	QLineEdit* rowsfilterline;
	QCheckBox* rowsfiltercasesensitive;
	QPushButton* rowsfilterbutton;
	QPushButton* rowsfilterclearbutton;

	QTableView* database;
	QStandardItemModel* databasemodel;
	cBricksDatabaseProxyModel* proxymodel;
	QVBoxLayout* mainlayout;
	QWidget* mainwidget;

	QString databasefile;
	QString lastdir;
	ifstream inputstream;
	ofstream outputstream;
	cBricksDatabase bricks;

	bool datamodified;

	void deleteTable();

	void resetHeader();

	bool checkTable();

	bool checkFormula(int row, const string& summary);

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

};

#endif

