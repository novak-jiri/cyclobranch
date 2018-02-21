/**
	\file cLinearWidget.h
	\brief Visualization of a linear peptide.
*/


#ifndef _CLINEARWIDGET_H
#define _CLINEARWIDGET_H

#include <QWidget>

#include "core/cTheoreticalSpectrum.h"


// forward declaration
class QPaintEvent;


/**
	\brief Auxiliary structure for visualization of labels of fragment ions.
*/
struct cIonLabel {

	/**
		\brief x coordinate.
	*/
	int x;


	/**
		\brief y coordinate.
	*/
	int y;


	/**
		\brief Label of a ion.
	*/ 
	string label;


	/**
		\brief True when the label is aligned right.
	*/ 
	bool alignright;


	/**
		\brief The constructor.
	*/ 
	cIonLabel() {
		clear();
	}


	/**
		\brief Clear the structure.
	*/ 
	void clear() {
		x = 0;
		y = 0;
		label = "";
		alignright = false;
	}

};


/**
	\brief The structure defining a hash function of cIonLabel.
*/
struct hash_cIonLabel {

	/**
		\brief Hash a ion label.
		\param label reference to a ion label
		\retval size_t hashed ion label
	*/
	size_t operator()(const cIonLabel& label) const {
		return std::hash<string>()(to_string(label.x) + to_string(label.y));
	}

};


/**
	\brief Overloaded operator ==.
	\param a first ion label
	\param b second ion label
	\retval bool true when labels have equal coordinates
*/
bool operator == (cIonLabel const& a, cIonLabel const& b);


/**
	\brief Insert a fragment ion label into an unordered set of labels and concatenate descriptions of labels on the same coordinates.
	\param labels unordered set of labels
	\param x x coordinate
	\param y y coordinate
	\param description a description of a fragment ion
	\param alignright true when the description is aligned right
*/
void insertLabel(unordered_set<cIonLabel, hash_cIonLabel>& labels, int x, int y, string& description, bool alignright);


/**
	\brief Visualization of a linear peptide.
*/
class cLinearWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cLinearWidget();


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

};

#endif