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