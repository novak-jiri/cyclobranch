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
class QScrollArea;


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

	QFormLayout* peaklistformlayout;
	QGroupBox* peaklistgroupbox;
	QComboBox* peptidetype;
	QLineEdit* peaklistline;
	QPushButton* peaklistbutton;
	QHBoxLayout* peaklistlayout;
	QSpinBox* scannumber;
	QDoubleSpinBox* precursormass;
	QLineEdit* precursoradduct;
	QSpinBox* precursorcharge;
	QDoubleSpinBox* precursormasserrortolerance;
	QDoubleSpinBox* fragmentmasserrortolerance;
	QDoubleSpinBox* masserrortolerancefordeisotoping;
	QDoubleSpinBox* minimumrelativeintensitythreshold;
	QSpinBox* minimumabsoluteintensitythreshold;
	QDoubleSpinBox* minimummz;
	QDoubleSpinBox* fwhm;

	QFormLayout* brickdatabaseformlayout;
	QGroupBox* brickdatabasegroupbox;
	QLineEdit* brickdatabaseline;
	QPushButton* brickdatabasebutton;
	QHBoxLayout* brickdatabaselayout;
	QSpinBox* maximumbricksincombinationbegin;
	QSpinBox* maximumbricksincombinationmiddle;
	QSpinBox* maximumbricksincombinationend;
	QDoubleSpinBox* maximumcumulativemass;
	QLineEdit* modificationsline;
	QPushButton* modificationsbutton;
	QHBoxLayout* modificationslayout;

	QGroupBox* miscgroupbox;
	QFormLayout* miscformlayout;
	QComboBox* blindedges;
	QCheckBox* cyclicnterminus;
	QCheckBox* cycliccterminus;
	QCheckBox* enablescrambling;
	QCheckBox* similaritysearch;
	QCheckBox* regularblocksorder;

	QFormLayout* applicationformlayout;
	QGroupBox* applicationgroupbox;
	QComboBox* mode;
	eModeType oldmodetype;
	QLineEdit* sequencedatabaseline;
	QPushButton* sequencedatabasebutton;
	QHBoxLayout* sequencedatabaselayout;
	QSpinBox* maximumnumberofthreads;
	QComboBox* scoretype;
	QSpinBox* hitsreported;
	QLineEdit* sequencetag;
	cFragmentIonsListWidget* fragmentiontypes;
	QCheckBox* clearhitswithoutparent;
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

};

#endif