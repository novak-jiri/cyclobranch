/**
	\file cSpectrumDetailWidget.h
	\brief The widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/


#ifndef _CSPECTRUMDETAILWIDGET_H
#define _CSPECTRUMDETAILWIDGET_H

#include <QWidget>
#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"
#include "gui/cBranchedWidget.h"
#include "gui/cGraphicalSpectrumWidget.h"


// forward declaration
class QTextEdit;
class QTextBrowser;
class QHBoxLayout;
class QVBoxLayout;
class QFormLayout;
class QScrollArea;
class QCheckBox;
class QPushButton;
class QSplitter;
class QComboBox;
class QDoubleSpinBox;


/**
	\brief The class representing a widget for visualisation of a detail of a spectrum (opened after doubleclick on a row when results are reported).
*/
class cSpectrumDetailWidget : public QWidget
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


private:
	QHBoxLayout* zoom;
	QHBoxLayout* mzinterval;

	QSplitter* hsplitter;
	QSplitter* vsplitter1;
	QSplitter* vsplitter2;

	QWidget* formwidget;
	QFormLayout* formlayout;
	QPushButton* zoomin;
	QPushButton* zoomout;
	QPushButton* normalsize;
	QDoubleSpinBox* minmz;
	QDoubleSpinBox* maxmz;
	QPushButton* setmzinterval;
	QPushButton* resetmzinterval;
	QCheckBox* hideunmatched;
	QCheckBox* hidematched;
	QComboBox* rotation;

	QTextEdit* textedit;
	QTextBrowser* textbrowser;
	cLinearWidget* linearwidget;
	cCyclicWidget* cyclicwidget;
	cBranchedWidget* branchedwidget;

	QScrollArea* graphicalspectrumscroll;
	cGraphicalSpectrumWidget* graphicalspectrum;

	cTheoreticalSpectrum* theoreticalspectrum;

	QHBoxLayout* mainbox;

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

};

#endif