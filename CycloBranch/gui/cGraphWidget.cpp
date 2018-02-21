#include "gui/cGraphWidget.h"

#include <QTextBrowser>
#include <QVBoxLayout>


cGraphWidget::cGraphWidget() {
	htmlstring = "";

	setWindowTitle("Graph");

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
	int size;

	size = (int)htmlstring.size();
	os.write((char *)&size, sizeof(int));
	os.write(htmlstring.c_str(), htmlstring.size());
}


void cGraphWidget::load(ifstream& is) {
	int size;

	is.read((char *)&size, sizeof(int));
	htmlstring.resize(size);
	is.read(&htmlstring[0], htmlstring.size());
	textbrowser->setHtml(htmlstring.c_str());
}

