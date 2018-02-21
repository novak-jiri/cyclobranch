#include "gui/cAboutWidget.h"
#include "gui/cMainThread.h"

#include <QTextBrowser>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QKeyEvent>
#include <QIcon>


cAboutWidget::cAboutWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("About...");
	setWindowIcon(QIcon(":/images/icons/48.png"));

	layout = new QVBoxLayout();

	QString homepage = "Homepage: <a href=\"http://ms.biomed.cas.cz/cyclobranch/\">http://ms.biomed.cas.cz/cyclobranch/</a><br/><br/>";

	QString citation = "Any work based on CycloBranch shall cite the following reference:<br/><br/>";
	citation += "<b>Jiri Novak, Karel Lemr, Kevin A. Schug and Vladimir Havlicek.<br/>";
	citation += "CycloBranch: De Novo Sequencing of Nonribosomal Peptides from Accurate Product Ion Mass Spectra.<br/>";
	citation += "<i>J. Am. Soc. Mass Spectrom.</i>, vol. 26, no. 10, pp. 1780-1786, 2015. DOI: <a href=\"http://link.springer.com/article/10.1007/s13361-015-1211-1\">10.1007/s13361-015-1211-1</a>.<br/>";
	citation += "Download citation: [ <a href=\"http://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_ris.txt\"><b>ris</b></a> ], [ <a href=\"http://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_bib.txt\"><b>bib</b></a> ]</b><br/><hr/><br/>";

	QString licence = "Licence:<br/><br/> This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.<br/><br/>";
	licence += "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.<br/><br/>";
	licence += "You should have received a copy of the GNU General Public License along with this program. If not, see <a href=\"http://www.gnu.org/licenses/\">http://www.gnu.org/licenses/</a>.<br/><hr/><br/>";

	QString developers = "Developers:<br/><br/>Jiri Novak<br/>Laboratory of Molecular Structure Characterization<br/>Institute of Microbiology<br/>Academy of Sciences of the Czech Republic<br/>Videnska 1083<br/>142 20 Prague<br/>Czech Republic<br/><a href=\"mailto:jiri.novak@biomed.cas.cz\">jiri.novak@biomed.cas.cz</a><br/><a href=\"http://ms.biomed.cas.cz/staff-novak_jiri.php\">http://ms.biomed.cas.cz/staff-novak_jiri.php</a><br/><a href=\"https://cas-cz.academia.edu/JiriNovak\">https://cas-cz.academia.edu/JiriNovak</a><br/><br/>(C) 2013 - 2016<br/><hr/><br/>";

	QString splash = "<div align=\"center\"><img src=\":/images/splash.png\"></div><br/><hr/><br/>";

	QString acknowledgement = "Acknowledgements:<br/><br/><a href=\"http://www.aha-soft.com/\">http://www.aha-soft.com/</a><br/><hr/><br/>";

	message = new QTextBrowser();
	message->setReadOnly(true);
	message->setAcceptRichText(true);
	message->setOpenExternalLinks(true);
	message->setHtml("<br/>" + splash + "<b>" + appname + " " + appversion + "</b><br/><br/>" + homepage + citation + developers + licence + acknowledgement);

	buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok);
	//buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
    //connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	layout->addWidget(message);
	layout->addWidget(buttonbox);

	setLayout(layout);

	resize(800, 600);
}


cAboutWidget::~cAboutWidget() {
	delete layout;
	delete message;
	delete buttonbox;
}


void cAboutWidget::closeEvent(QCloseEvent *event) {
	accept();
	event->accept();
}


void cAboutWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		hide();
    }
	event->accept();
}


void cAboutWidget::accept() {
	hide();
	//parent->setEnabled(true);
}

