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


const double pi = 3.141592653589793;


/**
	\brief Paint the circle of a cyclic or a lasso peptide.
	\param painter a reference to current QPainter
	\param brushcolor a background color for rectangles with the acronyms of building blocks
	\param acronymsofblocks acronyms of building blocks
	\param centerx x coordinate of a center of the ring
	\param centery y coordinate of a center of the ring
	\param radius radius of the ring
	\param angle an angle between two building blocks
	\param horizontalstep determine width of rectangles with the acronyms of building blocks
	\param linesize length of separators between building blocks
	\param cornerlinesize length of endings of separators between building blocks
*/ 
void paintCircle(QPainter& painter, QColor& brushcolor, vector<string>& acronymsofblocks, int centerx, int centery, int radius, double angle, int horizontalstep, int linesize, int cornerlinesize);


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


protected:


	/**
		\brief Handle the paint event.
		\param event pointer to QPaintEvent
	*/ 
	void paintEvent(QPaintEvent *event);


private:

	cParameters* parameters;
	cTheoreticalSpectrum* theoreticalspectrum;
	int visiblerotationid;


private slots:

	void rotationChanged(int index);

};

#endif