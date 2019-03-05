/**
	\file cNeutralLossesListWidget.h
	\brief The implementation of a widget where types of neutral losses generated in theoretical spectra are selected.
*/


#ifndef _CNEUTRALLOSSESLISTWIDGET_H
#define _CNEUTRALLOSSESLISTWIDGET_H

#include <string>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>


// forward declaration
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;


/**
	\brief The widget for selection of types of neutral losses.
*/
class cNeutralLossesListWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cNeutralLossesListWidget(QObject* parent);


	/**
		\brief The destructor.
	*/ 
	~cNeutralLossesListWidget();


	/**
		\brief Access to QListWidget.
		\retval QListWidget pointer to QListWidget
	*/ 
	QListWidget* getList();


	/**
		\brief Add a neutral loss.
		\param formula summary formula of a neutral loss
	*/
	void addItem(QString formula);


private:
	QObject* parent;

	QListWidget* list;
	QPushButton* selectallbutton;
	QPushButton* clearallbutton;
	QPushButton* addbutton;
	QPushButton* removebutton;
	QPushButton* defaultbutton;
	QPushButton* hconbutton;
	
	QHBoxLayout* hbox;
	QVBoxLayout* vbox1;
	QVBoxLayout* vbox2;


private slots:

	void selectAllItems();

	void clearAllItems();

	void addEmptyItem();

	void removeItem();

	void setDefaultItems();

	void setHCON();
	
};

#endif