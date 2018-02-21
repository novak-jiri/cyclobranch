/**
	\file cHTMLExportDialog.h
	\brief The implementation of HTML export dialog.
*/


#ifndef _CHTMLEXPORTDIALOG_H
#define _CHTMLEXPORTDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QCheckBox>


/**
	\brief The implementation of HTML export dialog.
*/
class cHTMLExportDialog : public QDialog
{
	Q_OBJECT

private:

	QWidget* parent;

	QFormLayout* form1;
	QFormLayout* form2;

	QGroupBox* group1;
	QGroupBox* group2;

	QHBoxLayout* hbox;
	QDialogButtonBox* buttonbox;
	QVBoxLayout* vbox;

public:

	/**
		\brief Export parameters.
    */
	QCheckBox* checkboxparameters;


	/**
		\brief Export de novo graph.
    */
	QCheckBox* checkboxdenovo;


	/**
		\brief Export log window.
    */
	QCheckBox* checkboxlogwindow;


	/**
		\brief Export output report table.
    */
	QCheckBox* checkboxsummaryresultstable;


	/**
		\brief Export summary table of matched peaks.
    */
	QCheckBox* checkboxsummarypeakstable;


	/**
		\brief Export peakss table.
    */
	QCheckBox* checkboxpeakstable;


	/**
		\brief Export unmatched experimental peaks.
    */
	QCheckBox* checkboxunmatchedexperimental;


	/**
		\brief Export matched experimental peaks.
    */
	QCheckBox* checkboxunmatchedtheoretical;


	/**
		\brief Export details of identification.
    */
	QCheckBox* checkboxdetails;


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cHTMLExportDialog(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cHTMLExportDialog();

};

#endif

