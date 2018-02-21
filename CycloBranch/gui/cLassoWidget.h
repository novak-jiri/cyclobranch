/**
	\file cLassoWidget.h
	\brief Visualization of a lasso peptide.
*/


#ifndef _CLASSOWIDGET_H
#define _CLASSOWIDGET_H

#include <math.h> 
#include <QWidget>
#include <QPalette>

#include "core/cTheoreticalSpectrum.h"
#include "gui/cLinearWidget.h"
#include "gui/cCyclicWidget.h"


// forward declaration
class QPaintEvent;


/**
	\brief Generate labels of fragment ions on a branch of a lasso peptide (direction down).
	\param nterminal if true only nterminal fragment ions are drawn, if false only cterminal fragment ions are drawn
	\param rotationid id of a sequence rotation
	\param labels labels of fragment ions
	\param parameters a pointer to parameters of the application
	\param theoreticalspectrum a theoretical spectrum
	\param centerx x coordinate of a center of the ring
	\param topmargin top margin in pixels
	\param horizontalstep horizontal step (determine width of blocks)
	\param verticalstep vertical step (determine height of blocks)
	\param visiblerotationid id of a rotation whose fragment ions are visualized
	\param branchstart starting position of a branch
*/ 
void generateBranchLabelsDown(bool nterminal, int rotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int topmargin, int horizontalstep, int verticalstep, int visiblerotationid, int branchstart);


/**
	\brief Generate labels of fragment ions on a branch of a lasso peptide (direction up).
	\param nterminal if true only nterminal fragment ions are drawn, if false only cterminal fragment ions are drawn
	\param rotationid id of a sequence rotation
	\param labels labels of fragment ions
	\param parameters a pointer to parameters of the application
	\param theoreticalspectrum a theoretical spectrum
	\param centerx x coordinate of a center of the ring
	\param topmargin top margin in pixels
	\param horizontalstep horizontal step (determine width of blocks)
	\param verticalstep vertical step (determine height of blocks)
	\param visiblerotationid id of a rotation whose fragment ions are visualized
	\param branchend end position of a branch
*/ 
void generateBranchLabelsUp(bool nterminal, int rotationid, unordered_set<cIonLabel, hash_cIonLabel>& labels, cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum, int centerx, int topmargin, int horizontalstep, int verticalstep, int visiblerotationid, int branchend);


/**
	\brief Visualization of a lasso peptide.
*/
class cLassoWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cLassoWidget();


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
	int visibletrotationid;


private slots:

	void rotationChanged(int index);

	void trotationChanged(int index);

};

#endif