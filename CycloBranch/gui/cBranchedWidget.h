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


protected:


	/**
		\brief Handle the paint event.
		\param event pointer to QPaintEvent
	*/ 
	void paintEvent(QPaintEvent *event);


private:

	cParameters* parameters;
	cTheoreticalSpectrum* theoreticalspectrum;
	int visibletrotationid;


private slots:

	void trotationChanged(int index);

};

#endif