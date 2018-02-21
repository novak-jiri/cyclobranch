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
	
	QApplication app(argc, argv);
	Q_INIT_RESOURCE(images);

	#if OS_TYPE != WIN
		setlocale(LC_NUMERIC,"C");
		chdir(installdir.toStdString().c_str());
	#endif
	qRegisterMetaType<cParameters>("cParameters");
	qRegisterMetaType<vector<fragmentIonType> >("vector<fragmentIonType>");
	qRegisterMetaType<peptideType>("peptideType");
	qRegisterMetaType<vector<fragmentDescription> >("vector<fragmentDescription>");
	qRegisterMetaType<string>("string");
	qRegisterMetaType<cFragmentIons>("cFragmentIons");

	QPixmap pixmap(":/images/splash.png");
	QSplashScreen splash(pixmap);
	if(!pixmap.isNull()) {
		splash.show();
		splash.showMessage(QObject::tr(QString(appname + " " + appversion + " is starting ...").toStdString().c_str()), Qt::AlignLeft | Qt::AlignBottom, Qt::black);
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

	return app.exec();

}

