#include "gui/cImageWindow.h"
#include "gui/cEventFilter.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QImage>


cImageWindow::cImageWindow(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Image Window");
	setWindowIcon(QIcon(":/images/icons/23.png"));

	close = new QPushButton(tr("Close"));
	close->setToolTip("Close the window.");

	loadimagebutton = new QPushButton(tr("Load Image"));
	loadimagebutton->setToolTip("Load an image file.");
	loadimagebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));

	image = new QImage();
	imagewindowwidget = new cImageWindowWidget();

	buttons = new QHBoxLayout();
	buttons->addWidget(close);
	buttons->addStretch();
	buttons->addWidget(loadimagebutton);

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(imagewindowwidget);
	mainlayout->addLayout(buttons);

	connect(close, SIGNAL(released()), this, SLOT(closeWindow()));
	connect(loadimagebutton, SIGNAL(released()), this, SLOT(loadImage()));

	setLayout(mainlayout);

	resize(1280, 700);

	lastimagedir = "./";
}


cImageWindow::~cImageWindow() {
	delete close;
	delete loadimagebutton;

	delete image;
	delete imagewindowwidget;

	delete buttons;
	delete mainlayout;
}


void cImageWindow::closeEvent(QCloseEvent *event) {
	closeWindow();
}


void cImageWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		closeWindow();
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/imagewindow.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/imagewindow.html").absoluteFilePath()));
		#endif
	}
}


void cImageWindow::closeWindow() {
	hide();
}


void cImageWindow::loadImage() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image File..."), lastimagedir, tr("Image Files (*.jpg *.jpeg *.png *.tif *.tiff *.bmp *.gif)"));

	if (!filename.isEmpty()) {
		lastimagedir = filename;

		image->load(filename);
		imagewindowwidget->setPixmap(image);
	}
}

