/**
	\file cPreferencesDialog.h
	\brief The implementation of preferences dialog.
*/


#ifndef _CPREFERENCESDIALOG_H
#define _CPREFERENCESDIALOG_H

#include "core/cGlobalPreferences.h"

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>


class cPreferencesTabIonTypes;
class cPreferencesTabDataProcessing;
class cPreferencesTabDirectories;
class cPreferencesTabURLs;


/**
	\brief Preferences dialog.
*/
class cPreferencesDialog : public QDialog
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	explicit cPreferencesDialog(cGlobalPreferences& globalpreferences, QWidget *parent = 0);


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
	~cPreferencesDialog();


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/
	void keyPressEvent(QKeyEvent *event);


private:

	cPreferencesTabIonTypes* tabiontypes;
	cPreferencesTabDataProcessing* tabdataprocessing;
	cPreferencesTabDirectories* tabdirectories;
	cPreferencesTabURLs* taburls;

	QTabWidget *tabwidget;
	QDialogButtonBox *buttonbox;

	QVBoxLayout *mainlayout;

};


#endif

