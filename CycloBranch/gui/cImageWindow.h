/**
	\file cImageWindow.h
	\brief Image window.
*/


#ifndef _CIMAGEWINDOW_H
#define _CIMAGEWINDOW_H

#include <QWidget>
#include <QDesktopServices>
#include "core/utilities.h"
#include "gui/cImageWindowWidget.h"


// forward declaration
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QImage;


/**
	\brief Image window.
*/
class cImageWindow : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cImageWindow(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cImageWindow();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


private:

	QWidget* parent;
	QPushButton* close;
	QPushButton* loadimagebutton;

	QImage* image;
	cImageWindowWidget* imagewindowwidget;
	QHBoxLayout* buttons;
	QVBoxLayout* mainlayout;

	QString lastimagedir;


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:


	void closeWindow();

	void loadImage();

};

#endif
