/**
	\file cFragmentIonsListWidget.h
	\brief The implementation of a widget where types of fragment ions generated in theoretical spectra are selected.
*/


#ifndef _CFRAGMENTIONSLISTWIDGET_H
#define _CFRAGMENTIONSLISTWIDGET_H

#include <string>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>


// forward declaration
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;


/**
	\brief The widget for selection of fragment ion types.
*/
class cFragmentIonsListWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cFragmentIonsListWidget(QObject* parent);


	/**
		\brief The destructor.
	*/ 
	~cFragmentIonsListWidget();


	/**
		\brief Access to QListWidget.
		\retval QListWidget pointer to QListWidget
	*/ 
	QListWidget* getList();


private:
	QObject* parent;

	QListWidget* list;
	QPushButton* selectallbutton;
	QPushButton* clearallbutton;
	QPushButton* resetbutton;
	
	QHBoxLayout* hbox;
	QVBoxLayout* vbox1;
	QVBoxLayout* vbox2;


private slots:

	void selectAllItems();

	void clearAllItems();

	void sendResetReleased();

signals:

	/**
		\brief Reset button has been released.
	*/ 
	void resetReleased();

};

#endif