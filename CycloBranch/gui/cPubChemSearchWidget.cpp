#include "gui/cPubChemSearchWidget.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QIcon>


cPubChemSearchWidget::cPubChemSearchWidget() {
	htmlstring = "";

	setWindowTitle("Search PubChem...");
	setWindowIcon(QIcon(":/images/icons/search.png"));

	layout = new QVBoxLayout();

	textbrowser = new QTextBrowser();
	textbrowser->setReadOnly(true);
	textbrowser->setFont(QFont("Courier", 9));
	textbrowser->setLineWrapMode(QTextEdit::NoWrap);
	textbrowser->setAcceptRichText(true);
	textbrowser->setOpenExternalLinks(true);

	layout->addWidget(textbrowser);

	setLayout(layout);

	resize(defaultwinsizex, defaultwinsizey);
}


cPubChemSearchWidget::~cPubChemSearchWidget() {
	delete textbrowser;

	delete layout;
}


void cPubChemSearchWidget::setHTML(string html) {
	htmlstring = html;
	textbrowser->setHtml(html.c_str());
}


string cPubChemSearchWidget::getHTML() {
	return htmlstring;
}


void cPubChemSearchWidget::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cPubChemSearchWidget::store(ofstream& os) {
	storeString(htmlstring, os);
}


void cPubChemSearchWidget::load(ifstream& is) {
	loadString(htmlstring, is);
	textbrowser->setHtml(htmlstring.c_str());
}


void cPubChemSearchWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		hide();
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/summarypeakstable.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/summarypeakstable.html").absoluteFilePath()));
		#endif
	}

	event->accept();
}

