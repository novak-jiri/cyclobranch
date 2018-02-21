#include <QApplication>
#include <QLabel>
#include <QMetaType>

#include "gui/cMainWindow.h"


/**
	\brief The main function.
	\param argc number of arguments
	\param argv pointer to an array of arguments
	\retval int 0 when the application finished successfully
*/ 
int main(int argc, char** argv) {

	QApplication app(argc, argv);
	qRegisterMetaType<cTheoreticalSpectrum>("cTheoreticalSpectrum");
	qRegisterMetaType<cParameters>("cParameters");
	qRegisterMetaType<vector<fragmentIonType> >("vector<fragmentIonType>");
	qRegisterMetaType<peptideType>("peptideType");
	qRegisterMetaType<vector<fragmentDescription> >("vector<fragmentDescription>");
	qRegisterMetaType<string>("string");
	qRegisterMetaType<cFragmentIons>("cFragmentIons");

	cMainWindow mwin;
	mwin.show();

	return app.exec();

}

