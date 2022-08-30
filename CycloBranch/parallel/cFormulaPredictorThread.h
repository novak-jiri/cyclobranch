/**
	\file cFormulaPredictorThread.h
	\brief The thread which searches molecular formulas corresponding to m/z value.
*/


#ifndef _CFORMULAPREDICTORTHREAD_H
#define _CFORMULAPREDICTORTHREAD_H

#include <QThread>
#include "core/utilities.h"
#include "core/cSummaryFormula.h"
#include "core/cBricksDatabase.h"
#include "core/cParameters.h"


/**
	\brief An auxiliary structure representing a row in the output table.
*/
struct cFormulaPredictorRowItem {

	/**
		\brief Ion formula.
	*/
	string ionformula;


	/**
		\brief Theoretical m/z.
	*/
	double theoreticalmz;


	/**
		\brief m/z error [ppm].
	*/
	double mzerror;


	/**
		\brief Ion type.
	*/
	string iontype;


	/**
		\brief Charge.
	*/
	int charge;

};


/**
	\brief The thread which searches molecular formulas corresponding to m/z value.
*/
class cFormulaPredictorThread : public QThread {

	Q_OBJECT
	
private:

	int chargevalue;
	int maximumcombinedelements;
	bool basicformulacheck;
	bool advancedformulacheck;
	bool noratiocheck;
	double mzvalue;
	double mzerror;
	vector<cIonType>* selectedions;
	vector<string>* selectedelements;
	vector<cFormulaPredictorRowItem>* ionitems;
	bool* success;
	bool* terminatecomputation;
	string* errmsg;

	int generateCompoundsFormulaPredictor(vector<string>& formulas);


public:


	/**
		\brief The constructor.
	*/ 
	cFormulaPredictorThread();


	/**
		\brief Initialize the thread.
		\param chargevalue charge
		\param maximumcombinedelements maximum number of combined chemical elements
		\param basicformulacheck basic filtering rules
		\param advancedformulacheck advanced filtering rules
		\param noratiocheck enable N/O rule
		\param mzvalue experimental m/z value
		\param mzerror m/z error
		\param selectedions input list of selected ions
		\param selectedelements input list of selected elements
		\param ionitems output list of ions
		\param success true if the calculation was successful
		\param terminatecomputation reference to a variable determining that the thread must be stopped
		\param errmsg an error message
	*/
	void initialize(int chargevalue, int maximumcombinedelements, bool basicformulacheck, bool advancedformulacheck, bool noratiocheck, double mzvalue, double mzerror, vector<cIonType>& selectedions, vector<string>& selectedelements, vector<cFormulaPredictorRowItem>& ionitems, bool& success, bool& terminatecomputation, string& errmsg);


protected:


	/**
		\brief The main method of the thread.
	*/ 
	void run();

};


#endif

