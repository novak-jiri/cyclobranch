/**
	\file cParametersWidget.h
	\brief The widget for editing parameters of the application opened using the command 'Search->Settings'.
*/


#ifndef _CPARAMETERSWIDGET_H
#define _CPARAMETERSWIDGET_H

#include <iostream>
#include <string>

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

#include "core/cSummaryFormula.h"
#include "core/cParameters.h"
#include "gui/cFragmentIonsListWidget.h"


using namespace std;


// forward declarations
class QFormLayout;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;
class QComboBox;
class QSpinBox;
class QDoubleSpinBox;
class QDialogButtonBox;
class QGroupBox;
class QCheckBox;
class QTableWidget;
class QListWidget;
class QMessageBox;


/**
	\brief The class representing a widget for editing parameters of the application.
*/
class cParametersWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cParametersWidget();


	/**
		\brief The destructor.
	*/ 
	~cParametersWidget();


	/**
		\brief Get parameters.
		\retval cParameters reference to parameters
	*/ 
	cParameters& getParameters();


	/**
		\brief Set and restore parameters.
		\param parameters reference to parameters
	*/ 
	void setAndRestoreParameters(cParameters& parameters);


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


private:
	cParameters parameters;

	QVBoxLayout* vlayout1;
	QVBoxLayout* vlayout2;
	QVBoxLayout* vlayout;
	QHBoxLayout* hlayout;

	QString settingsfile;
	QString oldsettingsfile;

	QDialogButtonBox* stdbuttons;
	QHBoxLayout* buttons;
	QPushButton* load;
	QPushButton* save;
	QPushButton* saveas;

	QFormLayout* peaklistformlayout;
	QGroupBox* peaklistgroupbox;
	QComboBox* peptidetype;
	QLineEdit* peaklistline;
	QPushButton* peaklistbutton;
	QHBoxLayout* peaklistlayout;
	string peaklistfilename;
	QDoubleSpinBox* precursormass;
	QSpinBox* precursorcharge;
	QDoubleSpinBox* precursormasserrortolerance;
	QDoubleSpinBox* fragmentmasserrortolerance;
	QDoubleSpinBox* masserrortolerancefordeisotoping;
	QDoubleSpinBox* minimumrelativeintensitythreshold;
	QDoubleSpinBox* minimummz;

	QFormLayout* brickdatabaseformlayout;
	QGroupBox* brickdatabasegroupbox;
	QLineEdit* brickdatabaseline;
	QPushButton* brickdatabasebutton;
	QHBoxLayout* brickdatabaselayout;
	string brickdatabasefilename;
	QSpinBox* maximumbricksincombinationbegin;
	QSpinBox* maximumbricksincombinationmiddle;
	QSpinBox* maximumbricksincombinationend;
	QDoubleSpinBox* maximumcumulativemass;
	QCheckBox* generatebrickspermutations;

	QGroupBox* modificationsgroupbox;
	QVBoxLayout* modificationslayout;
	QTableWidget* modificationstable;
	QHBoxLayout* modificationsbuttonslayout;
	QPushButton* modificationsbuttoninsert;
	QPushButton* modificationsbuttonremove;

	QFormLayout* applicationformlayout;
	QGroupBox* applicationgroupbox;
	QComboBox* mode;
	QSpinBox* maximumnumberofthreads;
	QComboBox* scoretype;
	QSpinBox* hitsreported;
	QLineEdit* sequencetag;
	cFragmentIonsListWidget* fragmentiontypes;
	QCheckBox* clearhitswithoutparent;
	QCheckBox* cyclicnterminus;
	QCheckBox* cycliccterminus;
	QCheckBox* enablescrambling;

	QGroupBox* denovographgroupbox;
	QFormLayout* denovographformlayout;
	QComboBox* blindedges;

	QGroupBox* databasesearchgroupbox;
	QFormLayout* databasesearchformlayout;
	QLineEdit* sequencedatabaseline;
	QPushButton* sequencedatabasebutton;
	QHBoxLayout* sequencedatabaselayout;
	string sequencedatabasefilename;
	
	QGroupBox* searchedsequencegroupbox;
	QFormLayout* searchedsequenceformlayout;
	QLineEdit* searchedsequence;
	QLineEdit* searchedsequenceNtermmodif;
	QLineEdit* searchedsequenceCtermmodif;
	QLineEdit* searchedsequenceTmodif;

	QString lastdirloadsettings;
	QString lastdirsavesettings;
	QString lastdirselectpeaklist;
	QString lastdirselectbricksdatabase;
	QString lastdirselectsequencedatabase;

	void deleteRow(int number);

	void deleteModificationsTableBody();

	void modificationsTableInsertRow();

	void modificationsTableRemoveEmptyRows();


private slots:

	void loadSettings();

	void saveSettings();

	void saveSettingsAs();

	void peaklistButtonReleased();

	void brickDatabaseButtonReleased();

	void sequenceDatabaseButtonReleased();

	void modificationsInsertButtonReleased();

	void modificationsRemoveButtonReleased();

	bool updateParameters();

	void updateParametersAndHide();
	
	void restoreParameters();

	void updateSettingsWhenPeptideTypeChanged(int index);

	void updateSettingsWhenModeChanged(int index);

	void resetFragmentIonTypes();

};

#endif