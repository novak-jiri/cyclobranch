/**
	\file cPreferencesIonTypeDialog.h
	\brief The implementation of ion type dialog.
*/


#ifndef _CPREFERENCESIONTYPEDIALOG_H
#define _CPREFERENCESIONTYPEDIALOG_H

#include "core/cSummaryFormula.h"
#include "core/cGlobalPreferences.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QString>


/**
	\brief Preferences dialog.
*/
class cPreferencesIonTypeDialog : public QDialog
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/
	explicit cPreferencesIonTypeDialog(QWidget *parent = 0);


	/**
		\brief Get values.
		\param ion ion
	*/
	void getValues(cIonType& ion);


	/**
		\brief Set values.
		\param ion ion
	*/
	void setValues(cIonType& ion);


	/**
		\brief The destructor.
	*/
	~cPreferencesIonTypeDialog();


private:

	QVBoxLayout *mainlayout;

	QDialogButtonBox *buttonbox;

	QWidget* gridlayoutwidget;
	QGridLayout* gridlayout;

	QLabel* labelname;
	QLineEdit* lineeditname;

	QLabel* labelformula;
	QLineEdit* lineeditformula;

	QLabel* labelmode;
	QComboBox* comboboxmode;

	QLabel* labelmultiplier;
	QSpinBox* spinboxmultiplier;

};


#endif

