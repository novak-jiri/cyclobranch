/**
	\file cChromatogramWindow.h
	\brief Chromatogram window.
*/


#ifndef _CCHROMATOGRAMWINDOW_H
#define _CCHROMATOGRAMWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QToolBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include "core/utilities.h"
#include "core/cGlobalPreferences.h"
#include "gui/cChromatogramWindowWidget.h"


/**
	\brief Chromatogram window.
*/
class cChromatogramWindow : public QMainWindow
{
	Q_OBJECT

public:


	/**
		\brief The constructor.
		\param globalpreferences global preferences of the application
		\param listoftheoreticalspectra lists of theoretical spectra
		\param parent pointer to a parent widget
	*/
	cChromatogramWindow(cGlobalPreferences* globalpreferences, cTheoreticalSpectrumList& listoftheoreticalspectra, QWidget* parent = (QWidget *)0);


	/**
		\brief The destructor.
	*/
	~cChromatogramWindow();


	/**
		\brief Handle the window close event.
		\param event pointer to QCloseEvent
	*/
	void closeEvent(QCloseEvent *event);


	/**
		\brief Recalculate TIC chromatogram.
		\param activefileid id of an active file
	*/
	void recalculateTICChromatogram(int activefileid);


	/**
		\brief Enable/disable absolute intensity mode.
		\param enabled if true the absolute intensity is used, if false the relative intensity is used
	*/
	void setAbsoluteIntensityEnabled(bool enabled);


	/**
		\brief Set chromatographic peak shape value.
		\param peakshape 0 = Gaussian, 1 = ex-Gaussian (tailing), 2 = ex-Gaussian (fronting)
	*/
	void setPeakShape(int peakshape);


	/**
		\brief Apply new global preferences.
		\param globalpreferences global preferences of the application
	*/
	void applyGlobalPreferences(cGlobalPreferences* globalpreferences);


private:

	cGlobalPreferences* globalpreferences;
	cTheoreticalSpectrumList* listoftheoreticalspectra;
	QWidget* parent;

	QMenuBar* menuBar;
	QMenu* menuFile;
	QMenu* menuView;
	QMenu* menuHelp;

	QToolBar* toolbarFile;
	QAction* actionExportImage;
	QAction* actionCloseWindow;

	QToolBar* toolbarView;
	QAction* actionZoomIn;
	QAction* actionZoomOut;
	QAction* actionZoomReset;
	QAction* actionMouseSelection;
	QAction* actionRetentionTime;
	QAction* actionAbsoluteIntensity;
	QAction* actionRawData;
	QAction* actionHideTIC;
	QAction* actionHideEIC;
	QAction* actionHideLabels;

	QWidget* peakshapewidget;
	QHBoxLayout* peakshapehboxlayout;
	QLabel* peakshapelabel;
	QComboBox* comboboxpeakshape;

	QToolBar* toolbarTime;

	QWidget* widgetretentiontime;
	QHBoxLayout* hboxretentiontime;
	QLabel* labelretentiontime;
	QDoubleSpinBox* minretentiontime;
	QLabel* labelseparatorretentiontime;
	QDoubleSpinBox* maxretentiontime;
	QPushButton* setretentiontimeinterval;
	QPushButton* resetretentiontimeinterval;

	QWidget* widgetscanid;
	QHBoxLayout* hboxscanid;
	QLabel* labelscanid;
	QSpinBox* minscanid;
	QLabel* labelseparatorscanid;
	QSpinBox* maxscanid;
	QPushButton* setscanidinterval;
	QPushButton* resetscanidinterval;
	QPushButton* sumofspectra;

	QToolBar* toolbarHelp;
	QAction* actionHTMLDocumentation;

	cChromatogramWindowWidget* chromatogramwindowwidget;

	QString lastdirexportimage;


protected:


	/**
		\brief Handle a key press event.
		\param event pointer to QKeyEvent
	*/
	void keyPressEvent(QKeyEvent *event);


signals:


	/**
		\brief The signal is emitted when the range of retention time has been changed.
		\param mintime minimum retention time
		\param maxtime maximum retention time
	*/
	void emitRetentionTimeInterval(double mintime, double maxtime);


	/**
		\brief The signal is emitted when the range of scan numbers has been changed.
		\param minid minimum scan id
		\param maxid maximum scan id
	*/
	void emitScanIDInterval(int minid, int maxid);


	/**
		\brief The signal is emitted a chromatographic peak was double clicked.
		\param scanid scan id
	*/
	void doubleClickedScanIDSignal(int scanid);


	/**
		\brief The signal is emitted if the average spectrum is calculated.
		\param minid minimum scan id
		\param maxid maximum scan id
	*/
	void calculateAvgSpectrum(int minid, int maxid);


private slots:


	void exportImage();


	void closeWindow();


	void updateRetentionTimeInterval(double mintime, double maxtime);


	void setRetentionTimeInterval();


	void updateScanIDInterval(int minid, int maxid);


	void setScanIDInterval();


	void setFilterOptionsSlot(cPeaksList eic);


	void showHTMLDocumentation();


	void chromatogramDoubleClickedSlot(int scanid);


	void showAvgSpectrum();

};

#endif
