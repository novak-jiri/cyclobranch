/**
	\file cPreferencesTabIonTypes.h
	\brief The implementation of a tab used to define ion types in preferences dialog.
*/


#ifndef _CPREFERENCESTABIONTYPES_H
#define _CPREFERENCESTABIONTYPES_H

#include "core/cGlobalPreferences.h"
#include "gui/cPreferencesIonTypeDialog.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QMessageBox>


/**
	\brief The class representing a tab of ion types.
*/
class cPreferencesTabIonTypes : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	explicit cPreferencesTabIonTypes(cGlobalPreferences& globalpreferences, QWidget *parent = 0);


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
	~cPreferencesTabIonTypes();


private:

	vector<cIonType> localcustomions;

	cPreferencesIonTypeDialog* iontypedialog;

	QListWidget* list;

	QHBoxLayout *hlayout;
	QWidget* hwidget;

	QVBoxLayout *vlayout;

	QPushButton* addbutton;
	QPushButton* editbutton;
	QPushButton* removebutton;
	QPushButton* defaultbutton;


	bool isValid(cIonType& ion, int row, string& errormessage);


private slots:

	void listItemDoubleClicked(QListWidgetItem* item);


	void addButtonReleased();


	void editButtonReleased();


	void removeButtonReleased();


	void defaultButtonReleased();

};


#endif

