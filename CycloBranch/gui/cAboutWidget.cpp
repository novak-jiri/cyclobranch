#include "gui/cAboutWidget.h"
#include "gui/cMainThread.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDialogButtonBox>



cAboutWidget::cAboutWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("About...");

	layout = new QVBoxLayout();

	message = new QTextBrowser();
	message->setReadOnly(true);
	message->setAcceptRichText(true);
	message->setOpenExternalLinks(true);
	message->setHtml(appname + " " + appversion + "<br/><br/>Developers:<br/><br/>Jiri Novak<br/>Laboratory of Molecular Structure Characterization<br/>Institute of Microbiology<br/>Academy of Sciences of the Czech Republic<br/>Videnska 1083<br/>142 20 Prague<br/>Czech Republic<br/><a href=\"mailto:jiri.novak@biomed.cas.cz\">jiri.novak@biomed.cas.cz</a><br/><a href=\"http://ms.biomed.cas.cz/staff-novak_jiri.php\">http://ms.biomed.cas.cz/staff-novak_jiri.php</a><br/><a href=\"https://cas-cz.academia.edu/JiriNovak\">https://cas-cz.academia.edu/JiriNovak</a><br/><br/>(C) 2013-2014");

	buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok);
	//buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    //connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(message);
	layout->addWidget(buttonbox);

	setLayout(layout);

	resize(600, 400);
}


cAboutWidget::~cAboutWidget() {
	delete layout;
	delete message;
	delete buttonbox;
}


void cAboutWidget::closeEvent(QCloseEvent *event) {
	accept();
}


void cAboutWidget::accept() {
	hide();
	//parent->setEnabled(true);
}

