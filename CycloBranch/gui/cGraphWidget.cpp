#include "gui/cGraphWidget.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QIcon>


cGraphWidget::cGraphWidget() {
	htmlstring = "";

	setWindowTitle("De Novo Graph");
	setWindowIcon(QIcon(":/images/icons/32.png"));

	layout = new QVBoxLayout();

	textbrowser = new QTextBrowser();
	textbrowser->setReadOnly(true);
	textbrowser->setFont(QFont("Courier", 9));
	textbrowser->setLineWrapMode(QTextEdit::NoWrap);
	textbrowser->setAcceptRichText(true);
	textbrowser->setOpenExternalLinks(true);

	layout->addWidget(textbrowser);

	setLayout(layout);

	resize(1280, 700);
}


cGraphWidget::~cGraphWidget() {
	delete textbrowser;

	delete layout;
}


void cGraphWidget::setHTML(string s) {
	textbrowser->setHtml(s.c_str());
	htmlstring = s;
}


string cGraphWidget::getHTML() {
	return htmlstring;
}


void cGraphWidget::closeEvent(QCloseEvent *event) {
	hide();
}


void cGraphWidget::store(ofstream& os) {
	storeString(htmlstring, os);
}


void cGraphWidget::load(ifstream& is) {
	loadString(htmlstring, is);
	textbrowser->setHtml(htmlstring.c_str());
}


void cGraphWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		hide();
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/menubar.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/menubar.html").absoluteFilePath()));
		#endif
	}
}

