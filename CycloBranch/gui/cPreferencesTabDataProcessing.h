/**
	\file cPreferencesTabDataProcessing.h
	\brief The implementation of a tab used to define the data processing parameters in preferences dialog.
*/


#ifndef _CPREFERENCESTABDATAPROCESSING_H
#define _CPREFERENCESTABDATAPROCESSING_H

#include "core/cGlobalPreferences.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QFileInfo>


/**
	\brief The class representing a tab used to define the data processing parameters.
*/
class cPreferencesTabDataProcessing : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	explicit cPreferencesTabDataProcessing(cGlobalPreferences& globalpreferences, QWidget *parent = 0);


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
	~cPreferencesTabDataProcessing();


private:

	QVBoxLayout *mainlayout;

	QWidget* gridlayoutwidget;
	QGridLayout* gridlayout;

	QLabel* labellinebafprocessingmethod;
	QComboBox* comboboxlinebafprocessingmethod;

	QLabel* labelprofilebafprocessingmethod;
	QComboBox* comboboxprofilebafprocessingmethod;

	QLabel* labelrawdataprocessingmethod;
	QComboBox* comboboxrawdataprocessingmethod;

	QWidget* hwidget;
	QHBoxLayout* hlayout;
	QPushButton* defaultbutton;


private slots:


	void defaultButtonReleased();

};


#endif

