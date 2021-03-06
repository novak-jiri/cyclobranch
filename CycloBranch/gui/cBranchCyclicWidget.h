/**
	\file cBranchCyclicWidget.h
	\brief Visualization of a branch-cyclic peptide.
*/


#ifndef _CBRANCHCYCLICWIDGET_H
#define _CBRANCHCYCLICWIDGET_H

#include <math.h> 
#include <QWidget>
#include <QPalette>

#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"


// forward declaration
class QPaintEvent;


/**
	\brief Generate labels of fragment ions on a branch of a branch-cyclic peptide (direction down).
	\param nterminal if true only nterminal fragment ions are drawn, if false only cterminal fragment ions are drawn
	\param rotationid id of a sequence rotation
	\param labels labels of fragment ions
	\param parameters a pointer to parameters of the application
	\param theoreticalspectrum a theoretical spectrum
	\param centerx x coordinate of a center of the ring
	\param topmargin top margin in pixels
	\param horizontalstep horizontal step (determine width of blocks)
	\param verticalstep vertical step (determine height of blocks)
	\param visibleionseries name of a ion series to be visualized
	\param visibleneutralloss name of a neutral loss to be visualized
	\param visiblerotationid id of a rotation whose fragment ions are visualized
	\param branchstart starting position of a branch
*/ 
void generateBranchLabelsDown(bool nterminal, int rotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int topmargin, int horizontalstep, int verticalstep, string visibleionseries, string visibleneutralloss, int visiblerotationid, int branchstart);


/**
	\brief Generate labels of fragment ions on a branch of a branch-cyclic peptide (direction up).
	\param nterminal if true only nterminal fragment ions are drawn, if false only cterminal fragment ions are drawn
	\param rotationid id of a sequence rotation
	\param labels labels of fragment ions
	\param parameters a pointer to parameters of the application
	\param theoreticalspectrum a theoretical spectrum
	\param centerx x coordinate of a center of the ring
	\param topmargin top margin in pixels
	\param horizontalstep horizontal step (determine width of blocks)
	\param verticalstep vertical step (determine height of blocks)
	\param visibleionseries name of a ion series to be visualized
	\param visibleneutralloss name of a neutral loss to be visualized
	\param visiblerotationid id of a rotation whose fragment ions are visualized
	\param branchend end position of a branch
	\param fragmentend last fragment to draw
*/ 
void generateBranchLabelsUp(bool nterminal, int rotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int topmargin, int horizontalstep, int verticalstep, string visibleionseries, string visibleneutralloss, int visiblerotationid, int branchend, int fragmentend);


/**
	\brief Visualization of a branch-cyclic peptide.
*/
class cBranchCyclicWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cBranchCyclicWidget();


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


	/**
		\brief Hide/Show labels of peaks.
		\param state the peak labels are hidden when \a state is true
	*/
	void hidePeakLabels(bool state);


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
	int visiblerotationid;
	int visibletrotationid;

	bool reportisomers;
	bool hidelabels;


private slots:

	void ionSeriesChanged(QString text);

	void neutralLossChanged(QString text);

	void rotationChanged(int index);

	void trotationChanged(int index);

};

#endif