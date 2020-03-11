/**
	\file cPreferencesTabDirectories.h
	\brief The implementation of a tab used to define default directories in preferences dialog.
*/


#ifndef _CPREFERENCESTABDIRECTORIES_H
#define _CPREFERENCESTABDIRECTORIES_H

#include "core/cGlobalPreferences.h"

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>


/**
	\brief The class representing a tab of default directories.
*/
class cPreferencesTabDirectories : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/
	explicit cPreferencesTabDirectories(cGlobalPreferences& globalpreferences, QWidget *parent = 0);


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
	~cPreferencesTabDirectories();


private:

	QVBoxLayout *mainlayout;

	QWidget* gridlayoutwidget;
	QGridLayout* gridlayout;

	QLabel* labelsettingsdefaultdir;
	QLineEdit* lineeditsettingsdefaultdir;
	QPushButton* buttonsettingsdefaultdir;

	QLabel* labelpeaklistsdefaultdir;
	QLineEdit* lineeditpeaklistsdefaultdir;
	QPushButton* buttonpeaklistsdefaultdir;

	QLabel* labelblocksdefaultdir;
	QLineEdit* lineeditblocksdefaultdir;
	QPushButton* buttonblocksdefaultdir;

	QLabel* labelsequencesdefaultdir;
	QLineEdit* lineeditsequencesdefaultdir;
	QPushButton* buttonsequencesdefaultdir;

	QLabel* labelmodificationsdefaultdir;
	QLineEdit* lineeditmodificationsdefaultdir;
	QPushButton* buttonmodificationsdefaultdir;

	QLabel* labelopenopticalimagedefaultdir;
	QLineEdit* lineeditopenopticalimagedefaultdir;
	QPushButton* buttonopenopticalimagedefaultdir;

	QLabel* labelopenhistologyimagedefaultdir;
	QLineEdit* lineeditopenhistologyimagedefaultdir;
	QPushButton* buttonopenhistologyimagedefaultdir;

	QLabel* labelopenmicroscopyimagedefaultdir;
	QLineEdit* lineeditopenmicroscopyimagedefaultdir;
	QPushButton* buttonopenmicroscopyimagedefaultdir;

	QLabel* labelresultsdefaultdir;
	QLineEdit* lineeditresultsdefaultdir;
	QPushButton* buttonresultsdefaultdir;

	QLabel* labelexportcsvdefaultdir;
	QLineEdit* lineeditexportcsvdefaultdir;
	QPushButton* buttonexportcsvdefaultdir;

	QLabel* labelexporthtmldefaultdir;
	QLineEdit* lineeditexporthtmldefaultdir;
	QPushButton* buttonexporthtmldefaultdir;

	QLabel* labelexportimagedefaultdir;
	QLineEdit* lineeditexportimagedefaultdir;
	QPushButton* buttonexportimagedefaultdir;

	QWidget* hwidget;
	QHBoxLayout* hlayout;
	QPushButton* defaultbutton;


private slots:


	void settingsButtonReleased();


	void peaklistsButtonReleased();


	void blocksButtonReleased();


	void sequencesButtonReleased();


	void modificationsButtonReleased();


	void openOpticalImageButtonReleased();


	void openHistologyImageButtonReleased();


	void openMicroscopyImageButtonReleased();


	void resultsButtonReleased();


	void exportCSVButtonReleased();


	void exportHTMLButtonReleased();


	void exportImageButtonReleased();


	void defaultButtonReleased();

};


#endif

