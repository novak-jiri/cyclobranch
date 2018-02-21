/**
	\file cModificationsWidget.h
	\brief Editor of N-terminal and C-terminal modifications.
*/


#ifndef _CMODIFICATIONSWIDGET_H
#define _CMODIFICATIONSWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <fstream>
#include "core/utilities.h"
#include "core/cFragmentIons.h"
#include "core/cSummaryFormula.h"
#include "core/cAllocator.h"
#include "gui/cDelegate.h"


using namespace std;


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QTableWidget;
class QTableWidgetItem;
class QPushButton;
class QLineEdit;
class QCheckBox;


/**
	\brief Editor of N-terminal and C-terminal modifications.
*/
class cModificationsWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cModificationsWidget(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cModificationsWidget();


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
	vector<fragmentDescription> modifications;

	vector<int> headersort;
	cDelegate columndelegate;

	cAllocator<QTableWidgetItem> widgetitemallocator;

	bool datamodified;

	void deleteTable(bool enableprogress);

	void removeRow(int row);

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

	void loadDatabase();

	bool saveDatabase();

	bool saveDatabaseAs();

	void addRow();

	void removeCheckedRows();

	void itemChanged(QTableWidgetItem* item);

	void headerItemDoubleClicked(int index);

	void filterRows();

	void resetFilter();

	void checkBoxModified(int state);

};

#endif
