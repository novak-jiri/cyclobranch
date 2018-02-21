/**
	\file cAboutWidget.h
	\brief The implementation of the dialog 'Help->About'.
*/


#ifndef _CABOUTWIDGET_H
#define _CABOUTWIDGET_H

#include <QWidget>


// forward declaration
class QVBoxLayout;
class QTextBrowser;
class QDialogButtonBox;


/**
	\brief The widget representing the dialog 'About'.
*/
class cAboutWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cAboutWidget(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cAboutWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:
	QWidget* parent;
	QVBoxLayout* layout;
	QTextBrowser* message;
	QDialogButtonBox* buttonbox;


private slots:

	void accept();

};

#endif