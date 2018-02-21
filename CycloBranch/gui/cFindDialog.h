/**
	\file cFindDialog.h
	\brief The implementation of a find dialog.
*/


#ifndef _CFINDDIALOG_H
#define _CFINDDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTextDocument>


/**
	\brief The implementation of a find dialog.
*/
class cFindDialog : public QDialog
{
	Q_OBJECT

private:

	QWidget* parent;

	QHBoxLayout* searchbox;

	QFormLayout* formlayout;
	QLineEdit* searchline;
	QPushButton* searchbutton;
	QCheckBox* matchcase;
	QCheckBox* matchwholeword;

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cFindDialog(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cFindDialog();


private slots:


	void findAll();

};

#endif