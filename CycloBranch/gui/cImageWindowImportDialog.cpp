#include "gui/cImageWindowImportDialog.h"
#include "gui/cImageWindowTabElectron.h"
#include "gui/cImageWindowTabOptical.h"

#include <QVBoxLayout>


cImageWindowImportDialog::cImageWindowImportDialog(QWidget *parent) : QDialog(parent) {
	tabelectron = new cImageWindowTabElectron();
	taboptical = new cImageWindowTabOptical();

	tabwidget = new QTabWidget();
	tabwidget->addTab(tabelectron, tr("Scanning Electron Microscope (FEI)"));
	tabwidget->addTab(taboptical, tr("Light Microscope (Leica)"));

	buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	buttonbox->button(QDialogButtonBox::Ok)->setText("Import");

	connect(buttonbox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonbox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(tabwidget);
	mainlayout->addWidget(buttonbox);
	setLayout(mainlayout);

	resize(750, 600);

	setWindowTitle(tr("Import Images..."));
	setWindowIcon(QIcon(":/images/icons/import.png"));
}


void cImageWindowImportDialog::setFormValues(QString& lastdirmicroscopyimage, int numberoflayers) {
	tabelectron->setFormValues(lastdirmicroscopyimage, numberoflayers);
	taboptical->setFormValues(lastdirmicroscopyimage, numberoflayers);
}


int cImageWindowImportDialog::getFormValues(int& startfromindex, QStringList& tiffilenames, int& navigationindex, QString& liffilename) {
	int tabindex = tabwidget->currentIndex();
	
	switch (tabindex) {
		case 0:
			tabelectron->getFormValues(startfromindex, tiffilenames);
			break;
		case 1:
			taboptical->getFormValues(startfromindex, tiffilenames, navigationindex, liffilename);
			break;
		default:
			startfromindex = 0;
			tiffilenames.clear();
			navigationindex = 0;
			liffilename.clear();
			break;
	}

	return tabindex;
}


cImageWindowImportDialog::~cImageWindowImportDialog() {
	delete tabelectron;
	delete taboptical;
	
	delete tabwidget;
	delete buttonbox;

	delete mainlayout;
}


//void cImageWindowImportDialog::keyPressEvent(QKeyEvent *event) {
//	if (event->key() == Qt::Key_F1) {
//		#if OS_TYPE == WIN
//			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/importimagesdialog.html").absoluteFilePath()));
//		#else
//			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/importimagesdialog.html").absoluteFilePath()));
//		#endif
//	}
//
//	event->accept();
//}


void cImageWindowImportDialog::accept() {
	int startfromindex;
	QStringList tiffilenames;
	int navigationindex;
	QString liffilename;

	QMessageBox msgBox;
	QString errstr;

	int tabindex = tabwidget->currentIndex();

	switch (tabindex) {
		case 0:
			tabelectron->getFormValues(startfromindex, tiffilenames);
			if (tiffilenames.isEmpty()) {
				errstr = "At least one TIF file must be selected !";
				msgBox.setText(errstr);
				msgBox.exec();
				return;
			}
			break;
		case 1:
			taboptical->getFormValues(startfromindex, tiffilenames, navigationindex, liffilename);
			if (liffilename.isEmpty()) {
				errstr = "A LIF file must be selected !";
				msgBox.setText(errstr);
				msgBox.exec();
				return;
			}
			break;
		default:
			break;
	}

	QDialog::accept();
}

