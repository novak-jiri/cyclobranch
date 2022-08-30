/**
	\file cDefineCalibrationWidget.h
	\brief The widget for definition of calibration curve.
*/


#ifndef _CDEFINECALIBRATIONWIDGET_H
#define _CDEFINECALIBRATIONWIDGET_H

#include <QWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>

#include "core/utilities.h"


// forward declaration
class QScrollArea;
class QVBoxLayout;
class QHBoxLayout; 
class QGridLayout;
class QLabel;
class QListWidget;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QPushButton;
class QComboBox;
class QFormLayout;


/**
	\brief The widget for definition of calibration curve.
*/
class cDefineCalibrationWidget : public QWidget
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
	*/ 
	cDefineCalibrationWidget();


	/**
		\brief The destructor.
	*/ 
	~cDefineCalibrationWidget();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/ 
	void closeEvent(QCloseEvent *event);


	/**
		\brief Store the content into an output stream.
		\param os an output stream
	*/ 
	void store(ofstream& os);


	/**
		\brief Load the content from an input stream.
		\param is an input stream
		\param fileversionpart1 first number of .res the file version
		\param fileversionpart2 second number of .res the file version
		\param fileversionpart3 third number of .res the file version
	*/ 
	void load(ifstream& is, int fileversionpart1, int fileversionpart2, int fileversionpart3);


	/**
		\brief Prepare widget for visualization.
		\param filenames list of files
		\param compounds list of compounds
	*/
	void prepareWidget(vector<string>& filenames, vector<string>& compounds);


	/**
		\brief Get data from dialog.
		\param usedvector vector of used items
		\param datatypesvector vector of sample types
		\param groupsvector vector of groups of technical replicates
		\param concentrationsvector vector of concentrations
		\param selectedionsvector vector of selected ions
		\param equationtype type of linear equation (y = a*x, y = a*x + b or manual mode)
		\param manuala the parameter 'a' in manual mode
		\param manualb the parameter 'b' in manual mode
		\param eictype type of EIC chromatogram (highest peaks = 0, all isotopes = 1)
		\param peakshape shape of chromatographic peak
		\param standard compound standard
	*/
	void getData(vector<int>& usedvector, vector<int>& datatypesvector, vector<int>& groupsvector, vector<double>& concentrationsvector, vector<int>& selectedionsvector, int& equationtype, double& manuala, double& manualb, int& eictype, int& peakshape, int& standard);


private:

	QScrollArea* scrollarea;
	QWidget* scrollwidget;
	QVBoxLayout* scrolllayout;

	QVBoxLayout* vlayout;
	QWidget* filelistwidget;

	QGridLayout* gridlayout;

	QLabel* uselabel;
	QLabel* filelabel;
	QLabel* grouplabel;
	QLabel* datatypelabel;
	QLabel* concentrationlabel;

	vector<QCheckBox*> checkboxes;
	vector<QLineEdit*> lineedits;
	vector<QComboBox*> comboboxes;
	vector<QSpinBox*> spinboxes;
	vector<QDoubleSpinBox*> doublespinboxes;

	QLabel* compoundslabel;
	QListWidget* compoundslist;
	QVBoxLayout* compoundslayout;
	QWidget* compoundswidget;

	QFormLayout* formlayout;
	QWidget* formwidget;

	QComboBox* equationcombobox;
	QDoubleSpinBox* doublespinboxa;
	QDoubleSpinBox* doublespinboxb;
	QComboBox* eiccombobox;
	QComboBox* peakshapecombobox;
	QComboBox* standardcombobox;

	QPushButton* okbutton;
	QPushButton* cancelbutton;
	QHBoxLayout* buttonslayout;
	QWidget* buttonswidget;

	vector<int> internalusedvector;
	vector<string> internalfilenames;
	vector<int> internaldatatypesvector;
	vector<int> internalgroupsvector;
	vector<double> internalconcentrationsvector;

	vector<string> internalcompounds;
	vector<int> internalselectedionsvector;

	int internalequationtype;
	double internala;
	double internalb;
	int internaleictype;
	int internalpeakshape;
	int internalstandard;

protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/ 
	void keyPressEvent(QKeyEvent *event);


private slots:

	void okButtonReleased();


	void cancelButtonReleased();


	void dataTypeChanged(int index);

	
	void equationChanged(int index);


signals:


	/**
		\brief Parameters of calibration curve were redefined.
	*/
	void calibrationRedefined();

};

#endif

