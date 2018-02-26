/**
	\file cBranchedWidget.h
	\brief Visualization of a branched peptide.
*/


#ifndef _CBRANCHEDWIDGET_H
#define _CBRANCHEDWIDGET_H

#include <QWidget>

#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"

// forward declaration
class QPaintEvent;


/**
	\brief Visualization of a branched peptide.
*/
class cBranchedWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cBranchedWidget();


	/**
		\brief Initialize the widget.
		\param parameters a pointer to parameters
		\param theoreticalspectrum a pointer to a theoretical spectrum
	*/ 
	void initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum);


	/**
		\brief Export peptide into a PDF or a PS file.
		\param filename filename
		\param postscript if true then PS file is generated instead of PDF
	*/ 
	void exportToPDF(QString filename, bool postscript);


	/**
		\brief Export peptide scene into a PNG file.
		\param filename filename
	*/ 
	void exportToPNG(QString filename);


	/**
		\brief Export peptide scene into a SVG file.
		\param filename filename
	*/ 
	void exportToSVG(QString filename);


	/**
		\brief Set the report isomers state.
		\param reportisomers true if isomers are reported; false otherwise
	*/
	void setReportIsomers(bool reportisomers);


protected:


	/**
		\brief Handle the paint event.
		\param event pointer to QPaintEvent
	*/ 
	void paintEvent(QPaintEvent *event);


private:

	void paint(QPainter& painter);

	cParameters* parameters;
	cTheoreticalSpectrum* theoreticalspectrum;

	string visibleionseries;
	string visibleneutralloss;
	int visibletrotationid;

	bool reportisomers;


private slots:

	void ionSeriesChanged(QString text);

	void neutralLossChanged(QString text);

	void trotationChanged(int index);

};

#endif