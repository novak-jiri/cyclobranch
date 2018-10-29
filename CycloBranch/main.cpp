#include <QApplication>
#include <QLabel>
#include <QMetaType>
#include <QPixmap>
#include <QSplashScreen>
#include <QThread>

#include "core/utilities.h"
#include "gui/cMainWindow.h"


/**
	\brief The main function.
	\param argc number of arguments
	\param argv pointer to an array of arguments
	\retval int 0 when the application finished successfully
*/ 
int main(int argc, char** argv) {

	XMLPlatformUtils::Initialize();
	
	QApplication app(argc, argv);
	Q_INIT_RESOURCE(images);

	#if OS_TYPE != WIN
		setlocale(LC_NUMERIC,"C");
		chdir(installdir.toStdString().c_str());
	#endif
	qRegisterMetaType<cParameters>("cParameters");
	qRegisterMetaType<vector<eFragmentIonType> >("vector<eFragmentIonType>");
	qRegisterMetaType<ePeptideType>("ePeptideType");
	qRegisterMetaType<vector<fragmentDescription> >("vector<fragmentDescription>");
	qRegisterMetaType<string>("string");
	qRegisterMetaType<cFragmentIons>("cFragmentIons");
	qRegisterMetaType<vector<cCoordinates> >("vector<cCoordinates>");
	qRegisterMetaType<cPeaksList>("cPeaksList");

	QPixmap pixmap(":/images/splash.png");
	QSplashScreen splash(pixmap);
	if(!pixmap.isNull()) {
		splash.show();
		splash.showMessage(QObject::tr(QString("Please, cite us using any related paper. " + appname + " " + appversion + " is starting ...").toStdString().c_str()), Qt::AlignCenter | Qt::AlignBottom, Qt::black);
		for (int i = 0; i < 50; i++) {
			if (!splash.isVisible()) {
				break;
			}
			QThread::msleep(100);
			app.processEvents();
		}
	}

	cMainWindow mwin;
	mwin.show();

	splash.finish(&mwin);

	int resultcode = app.exec();

	XMLPlatformUtils::Terminate();

	return resultcode;

}

