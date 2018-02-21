/**
	\file cExportDialog.h
	\brief The implementation of a dialog to export spectrum/peptide image.
*/


#ifndef _CEXPORTDIALOG_H
#define _CEXPORTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QRadioButton>
#include <QPushButton>


/**
	\brief The implementation of a dialog to export spectrum/peptide image.
*/
class cExportDialog : public QDialog
{
	Q_OBJECT

private:

	QWidget* parent;

	QHBoxLayout* exportbox;

	QFormLayout* formlayout;
	QPushButton* exportbutton;
	QRadioButton* exportspectrum;
	QRadioButton* exportpeptide;

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cExportDialog(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cExportDialog();


private slots:


	void processExport();

};

#endif