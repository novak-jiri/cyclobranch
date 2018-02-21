/**
	\file cCyclicWidget.h
	\brief Visualization of a cyclic peptide.
*/


#ifndef _CCYCLICWIDGET_H
#define _CCYCLICWIDGET_H

#include <math.h> 
#include <QWidget>
#include <QPalette>

#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"


// forward declaration
class QPaintEvent;


/**
	\brief Paint the circle of a cyclic or a branch-cyclic peptide.
	\param painter a reference to current QPainter
	\param acronymsofblocks acronyms of building blocks
	\param centerx x coordinate of a center of the ring
	\param centery y coordinate of a center of the ring
	\param radius radius of the ring
	\param angle an angle between two building blocks
	\param horizontalstep determine width of rectangles with the acronyms of building blocks
	\param linesize length of separators between building blocks
	\param cornerlinesize length of endings of separators between building blocks
	\param drawdashlines true when dashed lines between blocks and numbers of blocks are painted, false otherwise
	\param visiblerotationid an identifier of a rotation of a cyclic peptide
	\param labels labels of fragment ions
*/ 
void paintCircle(QPainter& painter, vector<string>& acronymsofblocks, int centerx, int centery, int radius, double angle, int horizontalstep, int linesize, int cornerlinesize, bool drawdashlines, int visiblerotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels);


/**
	\brief Generate labels of fragment ions of a cyclic peptide (direction right).
	\param nterminal if true only nterminal fragment ions are drawn, if false only cterminal fragment ions are drawn
	\param rotationid id of a sequence rotation
	\param rotationstart starting point on a ring to draw a series
	\param fragmentstart first fragment to draw
	\param fragmentend last fragment to draw
	\param numberofringblocks number of building blocks on the ring
	\param labels labels of fragment ions
	\param parameters a pointer to parameters of the application
	\param theoreticalspectrum a theoretical spectrum
	\param centerx x coordinate of a center of the ring
	\param centery y coordinate of a center of the ring
	\param radius radius of the ring
	\param angle an angle between two building blocks
	\param linesize length of separators between building blocks
	\param cornerlinesize length of endings of separators between building blocks
	\param visiblerotationid id of a rotation whose fragment ions are visualized
	\param branchstart starting position of a branch
	\param branchend end position of a branch
*/ 
void generateCyclicLabelsToRight(bool nterminal, int rotationid, int rotationstart, int fragmentstart, int fragmentend, int numberofringblocks, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int centery, int radius, double angle, int linesize, int cornerlinesize, int visiblerotationid, int branchstart, int branchend);


/**
	\brief Generate labels of fragment ions of a cyclic peptide (direction left).
	\param nterminal if true only nterminal fragment ions are drawn, if false only cterminal fragment ions are drawn
	\param rotationid id of a sequence rotation
	\param rotationstart starting point on a ring to draw a series
	\param fragmentstart first fragment to draw
	\param fragmentend last fragment to draw
	\param numberofringblocks number of building blocks on the ring
	\param labels labels of fragment ions
	\param parameters a pointer to parameters of the application
	\param theoreticalspectrum a theoretical spectrum
	\param centerx x coordinate of a center of the ring
	\param centery y coordinate of a center of the ring
	\param radius radius of the ring
	\param angle an angle between two building blocks
	\param linesize length of separators between building blocks
	\param cornerlinesize length of endings of separators between building blocks
	\param visiblerotationid id of a rotation whose fragment ions are visualized
	\param branchstart starting position of a branch
	\param branchend end position of a branch
*/ 
void generateCyclicLabelsToLeft(bool nterminal, int rotationid, int rotationstart, int fragmentstart, int fragmentend, int numberofringblocks, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int centery, int radius, double angle, int linesize, int cornerlinesize, int visiblerotationid, int branchstart, int branchend);


/**
	\brief Visualization of a cyclic peptide.
*/
class cCyclicWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cCyclicWidget();


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
	int visiblerotationid;

	bool reportisomers;


private slots:

	void rotationChanged(int index);

};

#endif