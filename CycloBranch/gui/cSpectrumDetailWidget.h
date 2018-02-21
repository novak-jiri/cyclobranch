/**
	\file cSpectrumDetailWidget.h
	\brief The widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/


#ifndef _CSPECTRUMDETAILWIDGET_H
#define _CSPECTRUMDETAILWIDGET_H

#include <QWidget>
#include <QMainWindow>
#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"
#include "gui/cBranchedWidget.h"
#include "gui/cLassoWidget.h"
#include "gui/cSpectrumSceneWidget.h"


// forward declaration
class QTextEdit;
class QTextBrowser;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QCheckBox;
class QPushButton;
class QSplitter;
class QComboBox;
class QDoubleSpinBox;
class QToolBar;
class QAction;
class QLabel;


/**
	\brief The class representing a widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/
class cSpectrumDetailWidget : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cSpectrumDetailWidget();


	/**
		\brief The copy constructor.
		\param sd reference to a cSpectrumDetailWidget
	*/ 
	cSpectrumDetailWidget(const cSpectrumDetailWidget& sd);


	/**
		\brief Overloaded operator=.
		\param sd reference to a cSpectrumDetailWidget
	*/ 
	cSpectrumDetailWidget& operator=(const cSpectrumDetailWidget& sd);


	/**
		\brief The destructor.
	*/ 
	~cSpectrumDetailWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Initialize the widget.
		\param parameters a pointer to parameters
		\param theoreticalspectrum a reference to a theoretical spectrum
	*/ 
	void initialize(cParameters* parameters, cTheoreticalSpectrum& theoreticalspectrum);


	/**
		\brief Get the theoretical spectrum.
		\retval cTheoreticalSpectrum reference to a theoretical spectrum
	*/ 
	cTheoreticalSpectrum& getTheoreticalSpectrum();


	/**
		\brief Get details about a theoretical spectrum as a HTML string.
		\retval string details about a theoretical spectrum
	*/ 
	string getDetailsAsHTMLString();


	/**
		\brief Prepare the widget to show.
		\param peptidetype a type of peptide
	*/ 
	void prepareToShow(peptideType peptidetype);


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private:
	QToolBar* toolbarExport;
	QToolBar* toolbarZoom;
	QToolBar* toolbarHide;
	QToolBar* toolbarMz;
	QToolBar* toolbarRotation;
	QToolBar* toolbarTrotation;

	QAction* actionExportSpectrum;
	QAction* actionExportPeptide;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionHideMatched;
	QAction* actionHideUnmatched;
	QAction* actionHideScrambled;
	QAction* actionMouseMzSelection;

	QSplitter* hsplitter1;
	QSplitter* hsplitter2;
	QSplitter* vsplitter;

	QWidget* widgetmz;
	QHBoxLayout* hboxmz;
	QLabel* labelmz;
	QDoubleSpinBox* minmz;
	QDoubleSpinBox* maxmz;
	QPushButton* setmzinterval;
	QPushButton* resetmzinterval;

	QWidget* widgetrotation;
	QHBoxLayout* hboxrotation;
	QLabel* labelrotation;
	QComboBox* rotation;

	QWidget* widgettrotation;
	QHBoxLayout* hboxtrotation;
	QLabel* labeltrotation;
	QComboBox* trotation;

	QTextEdit* textedit;
	QTextBrowser* textbrowser;
	cLinearWidget* linearwidget;
	cCyclicWidget* cyclicwidget;
	cBranchedWidget* branchedwidget;
	cLassoWidget* lassowidget;

	cSpectrumSceneWidget* spectrumscene;

	cTheoreticalSpectrum* theoreticalspectrum;

	bool preparedToShow;
	cParameters* parameters;


signals:

	/**
		\brief The signal is emitted when the range of m/z ratios has been changed.
		\param minmz a minimum threshold of m/z ratio
		\param maxmz a maximum threshold of m/z ratio
	*/ 
	void emitMZInterval(double minmz, double maxmz);


private slots:

	void updateMZInterval(double minmz, double maxmz);

	void setMZInterval();

	void exportSpectrum();

	void exportPeptide();

};

#endif