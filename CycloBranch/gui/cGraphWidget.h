/**
	\file cGraphWidget.h
	\brief The implementation of the dialog 'View->Graph'.
*/


#ifndef _CGRAPHWIDGET_H
#define _CGRAPHWIDGET_H

#include <string>
#include <fstream>
#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include "core/utilities.h"


using namespace std;


// forward declaration
class QTextBrowser;
class QVBoxLayout;


/**
	\brief The widget representing the dialog 'View->Graph'.
*/
class cGraphWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cGraphWidget();


	/**
		\brief The destructor.
	*/ 
	~cGraphWidget();


	/**
		\brief Set the HTML text into the widget.
		\param s string with the HTML text
	*/ 
	void setHTML(string s);


	/**
		\brief Get the HTML text from the widget.
		\retval string HTML string
	*/ 
	string getHTML();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Store the content into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the content from an input stream.
		\param is an input stream
	*/ 
	void load(ifstream& is);


private:
	QVBoxLayout* layout;
	QTextBrowser* textbrowser;
	string htmlstring;


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


};

#endif