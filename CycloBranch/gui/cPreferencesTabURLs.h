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
#include <QCheckBox>


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
	QLineEdit* lineeditname1;
	QLineEdit* lineediturl1;

	QLabel* labelurl2;
	QLineEdit* lineeditname2;
	QLineEdit* lineediturl2;

	QLabel* labelurl3;
	QLineEdit* lineeditname3;
	QLineEdit* lineediturl3;

	QLabel* labelurl4;
	QLineEdit* lineeditname4;
	QLineEdit* lineediturl4;

	QLabel* labelurl5;
	QLineEdit* lineeditname5;
	QLineEdit* lineediturl5;

	QLabel* labelurl6;
	QLineEdit* lineeditname6;
	QLineEdit* lineediturl6;

	QLabel* labelurl7;
	QLineEdit* lineeditname7;
	QLineEdit* lineediturl7;

	QLabel* labelurl8;
	QLineEdit* lineeditname8;
	QLineEdit* lineediturl8;

	QLabel* labelurl9;
	QLineEdit* lineeditname9;
	QLineEdit* lineediturl9;

	QLabel* labelurl10;
	QLineEdit* lineeditname10;
	QLineEdit* lineediturl10;

	QLabel* labelviewbookmarks;
	QCheckBox* checkboxviewbookmarks;

	QWidget* hwidget;
	QHBoxLayout* hlayout;
	QPushButton* defaultbutton;


private slots:

	void defaultButtonReleased();

};


#endif

