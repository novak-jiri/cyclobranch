/**
	\file cDrawPeptideWidget.h
	\brief Draw a peptide from a sequence of building blocks.
*/


#ifndef _CDRAWPEPTIDEWIDGET_H
#define _CDRAWPEPTIDEWIDGET_H

#include <QWidget>
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"
#include "gui/cBranchedWidget.h"
#include "gui/cLassoWidget.h"


// forward declaration
class QTextEdit;
class QTextBrowser;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QScrollArea;
class QCheckBox;
class QPushButton;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;
class QLabel;
class QAction;
class QMenu;
class QToolButton;
class QToolBar;


/**
	\brief Draw a peptide from a sequence of building blocks.
*/
class cDrawPeptideWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param parent pointer to a parent widget
	*/ 
	cDrawPeptideWidget(QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/ 
	~cDrawPeptideWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Set peptide sequence.
		\param peptidetypeindex an index of current peptide type
		\param sequence searched sequence
	*/ 
	void setSequence(int peptidetypeindex, QString sequence);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:

	QWidget* parent;

	QWidget* drawpeptideformwidget;
	QFormLayout* drawpeptideformlayout;
	QComboBox* peptidetypecombobox;

	QLineEdit* sequenceline;
	QLabel* sequencestatuslabel;
	QAction *sequenceactioncopy;
	QAction *sequenceactionsetsequence;
	QAction *sequenceactionsettag;
	QAction *sequenceactioninsert;
	QMenu *sequencemenu;
	QToolButton* sequencetoolbutton;
	QToolBar *sequencetoolbar;
	QHBoxLayout* sequencelayout;

	QCheckBox* separateblocks;
	QSpinBox* numberofblocksbackbone;
	QSpinBox* numberofblocksbranch;
	QSpinBox* branchposition;
	QHBoxLayout* backboneblocks;
	QWidget* backboneblockswidget;
	QHBoxLayout* branchblocks;
	QWidget* branchblockswidget;

	cTheoreticalSpectrum theoreticalspectrum;
	cLinearWidget* linearwidget;
	cCyclicWidget* cyclicwidget;
	cBranchedWidget* branchedwidget;
	cLassoWidget* lassowidget;

	QVBoxLayout* vbox;
	QHBoxLayout* mainbox;


	void drawPeptide(vector<string>& composition, cBricksDatabase& bricksdb, int branchstart, int branchend);


private slots:

	void separateBlocksChanged(int state);

	void numberOfBackboneBlocksChanged(int numberofblocks);

	void numberOfBranchBlocksChanged(int numberofblocks);

	void peptideTypeChanged(int index);

	void sequenceCopy();

	void sequenceSetSequence();

	void sequenceSetTag();

	void sequenceInsert();

	void blocksChanged();

	void sequenceChanged();


signals:

	/**
		\brief Send index of peptide type and searched sequence (a request for cParametersWidget).
		\param peptidetypeindex an index of current peptide type
		\param sequence searched sequence
	*/ 
	void sendSequence(int peptidetypeindex, QString sequence);


	/**
		\brief Send index of peptide type and sequence tag (a request for cParametersWidget).
		\param peptidetypeindex an index of current peptide type
		\param tag sequence tag
	*/ 
	void sendTag(int peptidetypeindex, QString tag);


	/**
		\brief Send index of peptide type and sequence (a request for cSequenceDatabaseWidget).
		\param peptidetypeindex an index of current peptide type
		\param sequence sequence
	*/ 
	void sendInsert(int peptidetypeindex, QString sequence);

};

#endif