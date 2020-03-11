/**
	\file cPreferencesTabURLs.h
	\brief The implementation of a tab used to define URLs in preferences dialog.
*/


#ifndef _CPREFERENCESTABURLS_H
#define _CPREFERENCESTABURLS_H

#include "core/cGlobalPreferences.h"

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


/**
	\brief The class representing a tab of URLs.
*/
class cPreferencesTabURLs : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	explicit cPreferencesTabURLs(cGlobalPreferences& globalpreferences, QWidget *parent = 0);


	/**
		\brief Set preferences.
		\param globalpreferences global preferences of the application
	*/
	void setPreferences(cGlobalPreferences& globalpreferences);


	/**
		\brief Reset preferences.
		\param globalpreferences global preferences of the application
	*/
	void resetPreferences(cGlobalPreferences& globalpreferences);


	/**
		\brief The destructor.
	*/
	~cPreferencesTabURLs();


private:

	QVBoxLayout *mainlayout;

	QWidget* gridlayoutwidget;
	QGridLayout* gridlayout;

	QLabel* labelurl1;
	QLineEdit* lineediturl1;

	QLabel* labelurl2;
	QLineEdit* lineediturl2;

	QWidget* hwidget;
	QHBoxLayout* hlayout;
	QPushButton* defaultbutton;


private slots:

	void defaultButtonReleased();

};


#endif

