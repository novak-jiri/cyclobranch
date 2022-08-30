/**
	\file cImageWindowImportDialog.h
	\brief The implementation of import dialog in image window.
*/


#ifndef _CIMAGEWINDOWIMPORTDIALOG_H
#define _CIMAGEWINDOWIMPORTDIALOG_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMessageBox>


class cImageWindowTabElectron;
class cImageWindowTabOptical;


/**
	\brief Import dialog in image window.
*/
class cImageWindowImportDialog : public QDialog
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/
	explicit cImageWindowImportDialog(QWidget *parent = 0);


	/**
		\brief Initialize values in the form.
		\param lastdirmicroscopyimage a default directory with microscopy images
		\param numberoflayers number of layers
	*/
	void setFormValues(QString& lastdirmicroscopyimage, int numberoflayers);


	/**
		\brief Get the values from the form.
		\param startfromindex starting index to open microscopy images
		\param tiffilenames a list of tif images to be opened
		\param navigationindex index of navigation image
		\param liffilename a lif file from which the metadata are read
		\retval int tab index
	*/
	int getFormValues(int& startfromindex, QStringList& tiffilenames, int& navigationindex, QString& liffilename);


	/**
		\brief The destructor.
	*/
	~cImageWindowImportDialog();


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/
	//void keyPressEvent(QKeyEvent *event);


private:

	cImageWindowTabElectron* tabelectron;
	cImageWindowTabOptical* taboptical;

	QTabWidget *tabwidget;
	QDialogButtonBox *buttonbox;

	QVBoxLayout *mainlayout;


public slots:

	/**
		\brief Override the accept slot.
	*/
	void accept();

};


#endif

