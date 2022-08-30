/**
	\file cImageWindowTabElectron.h
	\brief The implementation of a tab used to import images from electron microscopy.
*/


#ifndef _CIMAGEWINDOWTABELECTRON_H
#define _CIMAGEWINDOWTABELECTRON_H

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>
#include <QFileDialog>


/**
	\brief The class representing a tab used to import images from electron microscopy.
*/
class cImageWindowTabElectron : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/
	explicit cImageWindowTabElectron(QWidget *parent = 0);


	/**
		\brief Initialize values in the form.
		\param lastdirmicroscopyimage a default directory with microscopy images
		\param numberoflayers number of layers
	*/
	void setFormValues(QString& lastdirmicroscopyimage, int numberoflayers);


	/**
		\brief Get the values from the form.
		\param index starting index to open microscopy images
		\param filenames list of images to open
	*/
	void getFormValues(int& index, QStringList& filenames);


	/**
		\brief The destructor.
	*/
	~cImageWindowTabElectron();


private:

	QString* lastdirmicroscopyimage;

	QGridLayout* gridlayout;

	QLabel* labelstartfrom;
	QComboBox* comboboxstartfrom;

	QLabel* labelfilenames;
	QListWidget* listwidgetfilenames;
	QPushButton* buttonfilenames;
	QStringList imagenames;


private slots:

	void browseButtonReleased();

};


#endif

