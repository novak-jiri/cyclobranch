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
#include "core/cGlobalPreferences.h"
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
		\param globalpreferences global preferences of the application
		\param parent pointer to a parent widget
	*/ 
	cParametersWidget(cGlobalPreferences* globalpreferences, QWidget* parent = (QWidget *)0);


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


	/**
		\brief Apply new global preferences.
		\param globalpreferences global preferences of the application
	*/
	void applyGlobalPreferences(cGlobalPreferences* globalpreferences);


private:

	bool hideunusedparameters;

	QWidget* parent;

	cGlobalPreferences* globalpreferences;
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

	QGroupBox* searchgroupbox;
	QGridLayout* searchgridlayout;
	QLabel* modelabel;
	QComboBox* mode;
	eModeType oldmodetype;
	QSpinBox* maximumnumberofthreads;
	QLabel* maximumnumberofthreadslabel;

	QGroupBox* experimentalspectragroupbox;
	QGridLayout* experimentalspectragridlayout;
	QLabel* peptidetypelabel;
	QComboBox* peptidetype;
	QLabel* peaklistlabel;
	QLineEdit* peaklistline;
	QCheckBox* useprofiledata;
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
	QLabel* minimumrelativeintensitythresholdlabel;
	QDoubleSpinBox* minimumrelativeintensitythreshold;
	QLabel* minimumabsoluteintensitythresholdlabel;
	QSpinBox* minimumabsoluteintensitythreshold;
	QLabel* mzratiolabel;
	QDoubleSpinBox* minimummz;
	QDoubleSpinBox* maximummz;
	QHBoxLayout* mzratiolayout;
	QWidget* mzratiowidget;
	QLabel* rtlabel;
	QDoubleSpinBox* minimumrt;
	QDoubleSpinBox* maximumrt;
	QHBoxLayout* rtlayout;
	QWidget* rtwidget;
	QLabel* fwhmlabel;
	QDoubleSpinBox* fwhm;

	QGroupBox* isotoperatiosgroupbox;
	QGridLayout* isotoperatiosgridlayout;

	QLabel* ratio54Fe56FeLabel;
	QCheckBox* ratio54Fe56FeEnabled;
	QDoubleSpinBox* minratio54Fe56Fe;
	QDoubleSpinBox* maxratio54Fe56Fe;
	QHBoxLayout* ratio54Fe56FeLayout;
	QWidget* ratio54Fe56FeWidget;

	QLabel* ratio60Ni58NiLabel;
	QCheckBox* ratio60Ni58NiEnabled;
	QDoubleSpinBox* minratio60Ni58Ni;
	QDoubleSpinBox* maxratio60Ni58Ni;
	QHBoxLayout* ratio60Ni58NiLayout;
	QWidget* ratio60Ni58NiWidget;

	QLabel* ratio62Ni58NiLabel;
	QCheckBox* ratio62Ni58NiEnabled;
	QDoubleSpinBox* minratio62Ni58Ni;
	QDoubleSpinBox* maxratio62Ni58Ni;
	QHBoxLayout* ratio62Ni58NiLayout;
	QWidget* ratio62Ni58NiWidget;

	QLabel* ratio65Cu63CuLabel;
	QCheckBox* ratio65Cu63CuEnabled;
	QDoubleSpinBox* minratio65Cu63Cu;
	QDoubleSpinBox* maxratio65Cu63Cu;
	QHBoxLayout* ratio65Cu63CuLayout;
	QWidget* ratio65Cu63CuWidget;

	QLabel* ratio66Zn64ZnLabel;
	QCheckBox* ratio66Zn64ZnEnabled;
	QDoubleSpinBox* minratio66Zn64Zn;
	QDoubleSpinBox* maxratio66Zn64Zn;
	QHBoxLayout* ratio66Zn64ZnLayout;
	QWidget* ratio66Zn64ZnWidget;

	QLabel* ratio67Zn64ZnLabel;
	QCheckBox* ratio67Zn64ZnEnabled;
	QDoubleSpinBox* minratio67Zn64Zn;
	QDoubleSpinBox* maxratio67Zn64Zn;
	QHBoxLayout* ratio67Zn64ZnLayout;
	QWidget* ratio67Zn64ZnWidget;

	QLabel* ratio68Zn64ZnLabel;
	QCheckBox* ratio68Zn64ZnEnabled;
	QDoubleSpinBox* minratio68Zn64Zn;
	QDoubleSpinBox* maxratio68Zn64Zn;
	QHBoxLayout* ratio68Zn64ZnLayout;
	QWidget* ratio68Zn64ZnWidget;

	QGroupBox* brickdatabasegroupbox;
	QGridLayout* brickdatabasegridlayout;
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
	QLabel* internalfragmentslabel;
	QCheckBox* internalfragments;
	QLabel* enablescramblinglabel;
	QCheckBox* enablescrambling;
	QLabel* similaritysearchlabel;
	QCheckBox* similaritysearch;
	QLabel* regularblocksorderlabel;
	QCheckBox* regularblocksorder;

	QGroupBox* theoreticalspectragroupbox;
	QGridLayout* theoreticalspectragridlayout;
	QLabel* sequencedatabaselabel;
	QLineEdit* sequencedatabaseline;
	QPushButton* sequencedatabasebutton;
	QHBoxLayout* sequencedatabaselayout;
	QWidget* sequencedatabasewidget;
	QLabel* scoretypelabel;
	QComboBox* scoretype;
	QLabel* hitsreportedlabel;
	QSpinBox* hitsreported;
	QLabel* sequencetaglabel;
	QLineEdit* sequencetag;
	QLabel* iontypeslabel;
	cFragmentIonsListWidget* iontypes;
	QLabel* neutrallosstypeslabel;
	cNeutralLossesListWidget* neutrallosstypes;
	QLabel* maximumcombinedlosseslabel;
	QSpinBox* maximumcombinedlosses;
	QLabel* elementstypeslabel;
	cNeutralLossesListWidget* elementstypes;
	QLabel* maximumcombinedelementslabel;
	QSpinBox* maximumcombinedelements;
	//QCheckBox* clearhitswithoutparent;
	QLabel* reportunmatchedtheoreticalpeakslabel;
	QCheckBox* reportunmatchedtheoreticalpeaks;
	QLabel* generateisotopepatternlabel;
	QCheckBox* generateisotopepattern;
	QLabel* minimumpatternsizelabel;
	QSpinBox* minimumpatternsize;
	QLabel* minimumfeaturesizelabel;
	QSpinBox* minimumfeaturesize;
	QLabel* minimumiontypeslabel;
	QSpinBox* minimumiontypes;
	QLabel* basicformulachecklabel;
	QCheckBox* basicformulacheck;
	QLabel* advancedformulachecklabel;
	QCheckBox* advancedformulacheck;
	QLabel* noratiochecklabel;
	QCheckBox* noratiocheck;
	QLabel* calculatefdrslabel;
	QCheckBox* calculatefdrs;

	QLabel* minimumannotationintensitylabel;
	QDoubleSpinBox* minimumannotationintensityrelative;
	QSpinBox* minimumannotationintensityabsolute;
	QHBoxLayout* minimumannotationintensitylayout;
	QWidget* minimumannotationintensitywidget;

	QLabel* mzdifftolerancelabel;
	QDoubleSpinBox* mzdifftolerance;
	QLabel* intensitytolerancelabel;
	QDoubleSpinBox* intensitytolerance;

	QGroupBox* searchedsequencegroupbox;
	QGridLayout* searchedsequencegridlayout;
	QLabel* searchedsequencelabel;
	QLineEdit* searchedsequenceline;
	QPushButton* searchedsequencebutton;
	QHBoxLayout* searchedsequencelayout;
	QWidget* searchedsequencewidget;
	QLabel* searchedsequencemodiflabel;
	QLineEdit* searchedsequenceNtermmodif;
	QLineEdit* searchedsequenceCtermmodif;
	QLineEdit* searchedsequenceTmodif;
	QHBoxLayout* searchedsequencemodiflayout;
	QWidget* searchedsequencemodifwidget;
	QLabel* searchedsequenceformulalabel;
	QLineEdit* searchedsequenceformula;

	QString lastdirloadsettings;
	QString lastdirsavesettings;

	QString defaultdirselectpeaklist;
	QString defaultdirselectbricksdatabase;
	QString defaultdirselectmodifications;
	QString defaultdirselectsequencedatabase;


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