/**
	\file cParametersWidget.h
	\brief The widget for editing parameters of the application opened using the command 'Search->Settings'.
*/


#ifndef _CPARAMETERSWIDGET_H
#define _CPARAMETERSWIDGET_H

#include <iostream>
#include <string>

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QLineEdit>
#include <QPushButton>

#include "core/utilities.h"
#include "core/cSummaryFormula.h"
#include "core/cParameters.h"
#include "gui/cFragmentIonsListWidget.h"
#include "gui/cNeutralLossesListWidget.h"


using namespace std;


// forward declarations
class QFormLayout;
class QGridLayout;
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
class QScrollArea;
class QLabel;


/**
	\brief The class representing a widget for editing parameters of the application.
*/
class cParametersWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cParametersWidget(QWidget* parent = (QWidget *)0);


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


	/**
		\brief Set peptide type and searched sequence.
		\param peptidetypeindex an index of current peptide type
		\param sequence searched sequence
	*/ 
	void setSequence(int peptidetypeindex, QString sequence);


	/**
		\brief Set peptide type and sequence tag.
		\param peptidetypeindex an index of current peptide type
		\param tag sequence tag
	*/ 
	void setTag(int peptidetypeindex, QString tag);


private:

	QWidget* parent;

	cParameters parameters;

	QVBoxLayout* vlayout1;
	QVBoxLayout* vlayout2;
	QVBoxLayout* vlayout;
	QHBoxLayout* hlayout;
	QWidget* hlayoutwidget;
	QScrollArea* hlayoutscroll;

	QString settingsfile;
	QString oldsettingsfile;

	QDialogButtonBox* stdbuttons;
	QHBoxLayout* buttons;
	QPushButton* load;
	QPushButton* save;
	QPushButton* saveas;

	QGridLayout* searchgridlayout;
	QGroupBox* searchgroupbox;
	QLabel* modelabel;
	QComboBox* mode;
	eModeType oldmodetype;
	QSpinBox* maximumnumberofthreads;
	QLabel* maximumnumberofthreadslabel;

	QGridLayout* experimentalspectragridlayout;
	QGroupBox* experimentalspectragroupbox;
	QLabel* peptidetypelabel;
	QComboBox* peptidetype;
	QLabel* peaklistlabel;
	QLineEdit* peaklistline;
	QPushButton* peaklistbutton;
	QHBoxLayout* peaklistlayout;
	QWidget* peaklistwidget;
	QLabel* scannumberlabel;
	QSpinBox* scannumber;
	QLabel* precursormasslabel;
	QDoubleSpinBox* precursormass;
	QLabel* precursoradductlabel;
	QLineEdit* precursoradduct;
	QLabel* precursorchargelabel;
	QSpinBox* precursorcharge;
	QLabel* precursormasserrortolerancelabel;
	QDoubleSpinBox* precursormasserrortolerance;
	QLabel* fragmentmasserrortolerancelabel;
	QDoubleSpinBox* fragmentmasserrortolerance;
	//QDoubleSpinBox* masserrortolerancefordeisotoping;
	QLabel* minimumrelativeintensitythresholdlabel;
	QDoubleSpinBox* minimumrelativeintensitythreshold;
	QLabel* minimumabsoluteintensitythresholdlabel;
	QSpinBox* minimumabsoluteintensitythreshold;
	QLabel* minimummzlabel;
	QDoubleSpinBox* minimummz;
	QLabel* fwhmlabel;
	QDoubleSpinBox* fwhm;

	QGridLayout* brickdatabasegridlayout;
	QGroupBox* brickdatabasegroupbox;
	QLabel* brickdatabaselabel;
	QLineEdit* brickdatabaseline;
	QPushButton* brickdatabasebutton;
	QHBoxLayout* brickdatabaselayout;
	QWidget* brickdatabasewidget;
	QLabel* maximumbricksincombinatiolabel;
	QSpinBox* maximumbricksincombinationbegin;
	QSpinBox* maximumbricksincombinationmiddle;
	QSpinBox* maximumbricksincombinationend;
	QHBoxLayout* maximumbricksincombinationlayout;
	QWidget* maximumbricksincombinationwidget;
	QLabel* maximumcumulativemasslabel;
	QDoubleSpinBox* maximumcumulativemass;
	QLabel* modificationslabel;
	QLineEdit* modificationsline;
	QPushButton* modificationsbutton;
	QHBoxLayout* modificationslayout;
	QWidget* modificationswidget;

	QGroupBox* miscgroupbox;
	QGridLayout* miscgridlayout;
	QLabel* blindedgeslabel;
	QComboBox* blindedges;
	QLabel* cyclicnterminuslabel;
	QCheckBox* cyclicnterminus;
	QLabel* cycliccterminuslabel;
	QCheckBox* cycliccterminus;
	QLabel* enablescramblinglabel;
	QCheckBox* enablescrambling;
	QLabel* similaritysearchlabel;
	QCheckBox* similaritysearch;
	QLabel* regularblocksorderlabel;
	QCheckBox* regularblocksorder;

	QFormLayout* theoreticalspectraformlayout;
	QGroupBox* theoreticalspectragroupbox;
	QLineEdit* sequencedatabaseline;
	QPushButton* sequencedatabasebutton;
	QHBoxLayout* sequencedatabaselayout;
	QComboBox* scoretype;
	QSpinBox* hitsreported;
	QLineEdit* sequencetag;
	cFragmentIonsListWidget* fragmentiontypes;
	cNeutralLossesListWidget* neutrallosstypes;
	QSpinBox* maximumcombinedlosses;
	//QCheckBox* clearhitswithoutparent;
	QCheckBox* reportunmatchedtheoreticalpeaks;
	QCheckBox* generateisotopepattern;
	QSpinBox* minimumpatternsize;

	QGroupBox* searchedsequencegroupbox;
	QFormLayout* searchedsequenceformlayout;
	QLineEdit* searchedsequenceline;
	QPushButton* searchedsequencebutton;
	QHBoxLayout* searchedsequencelayout;
	QLineEdit* searchedsequenceNtermmodif;
	QLineEdit* searchedsequenceCtermmodif;
	QLineEdit* searchedsequenceTmodif;

	QString lastdirloadsettings;
	QString lastdirsavesettings;
	QString lastdirselectpeaklist;
	QString lastdirselectbricksdatabase;
	QString lastdirselectmodifications;
	QString lastdirselectsequencedatabase;


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void loadSettings();

	void saveSettings();

	void saveSettingsAs();

	void peaklistButtonReleased();

	void brickDatabaseButtonReleased();

	void modificationsButtonReleased();

	void sequenceDatabaseButtonReleased();

	bool updateParameters();

	void updateParametersAndHide();
	
	void restoreParameters();

	void updateSettingsWhenPeptideTypeChanged(int index);

	void updateSettingsWhenModeChanged(int index);

	void resetFragmentIonTypes();

	void drawPeptideButtonReleased();


signals:

	/**
		\brief Send index of peptide type and searched sequence.
		\param peptidetypeindex an index of current peptide type
		\param sequence searched sequence
	*/ 
	void sendSequenceLine(int peptidetypeindex, QString sequence);


	/**
		\brief Set list of neutral losses to HCON.
	*/
	void setHCON();


	/**
		\brief Clear the selection of ions.
	*/
	void clearIonSelection();

};

#endif