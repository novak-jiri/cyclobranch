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

	QString homepage = "<a href=\"https://ms.biomed.cas.cz/cyclobranch/\">https://ms.biomed.cas.cz/cyclobranch/</a><br/><a href=\"https://github.com/novak-jiri/cyclobranch/\">https://github.com/novak-jiri/cyclobranch/</a><br/><br/>";

	QString citations = "Any work based on CycloBranch shall cite any of the following references:<br/><br/>";
	citations += "Jiri Novak, Karel Lemr, Kevin A. Schug, Vladimir Havlicek:<br/>";
	citations += "CycloBranch: <i>De Novo</i> Sequencing of Nonribosomal Peptides from Accurate Product Ion Mass Spectra,<br/>";
	citations += "<i>J. Am. Soc. Mass Spectrom.</i>, vol. 26, no. 10, pp. 1780-1786, 2015. DOI: <a href=\"https://doi.org/10.1007/s13361-015-1211-1\">10.1007/s13361-015-1211-1</a>.<br/>";
	citations += "Download citation: [ <a href=\"https://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_ris.txt\"><b>ris</b></a> ], [ <a href=\"https://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_bib.txt\"><b>bib</b></a> ]<br/><br/>";

	citations += "Jiri Novak, Lucie Sokolova, Karel Lemr, Tomas Pluhacek, Andrea Palyzova, Vladimir Havlicek:<br/>";
	citations += "Batch-processing of Imaging or Liquid-chromatography Mass Spectrometry Datasets and <i>De Novo</i> Sequencing of Polyketide Siderophores,<br/>";
	citations += "<i>BBA - Proteins Proteom.</i>, vol. 1865, no. 7, pp. 768-775, 2017. DOI: <a href=\"https://doi.org/10.1016/j.bbapap.2016.12.003\">10.1016/j.bbapap.2016.12.003</a>.<br/>";
	citations += "Download citation: [ <a href=\"https://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_bba_ris.txt\"><b>ris</b></a> ], [ <a href=\"https://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_bba_bib.txt\"><b>bib</b></a> ]<br/><br/>";

	citations += "Jiri Novak, Anton Skriba, Jakub Zapal, Marek Kuzma, Vladimir Havlicek:<br/>";
	citations += "CycloBranch: an Open Tool for Fine Isotope Structures in Conventional and Product Ion Mass Spectra,<br/>";
	citations += "<i>J. Mass Spectrom.</i>, vol. 53, no. 11, pp. 1097-1103, 2018. DOI: <a href=\"https://doi.org/10.1002/jms.4285\">10.1002/jms.4285</a>.<br/>";
	citations += "Download citation: [ <a href=\"https://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_jms_ris.txt\"><b>ris</b></a> ], [ <a href=\"https://ms.biomed.cas.cz/cyclobranch/docs/cyclobranch_jms_bib.txt\"><b>bib</b></a> ]<br/><hr/><br/>";

	QString licence = "Licence:<br/><br/> This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.<br/><br/>";
	licence += "This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.<br/><br/>";
	licence += "You should have received a copy of the GNU General Public License along with this program. If not, see <a href=\"https://www.gnu.org/licenses/\">https://www.gnu.org/licenses/</a>.<br/><hr/><br/>";

	QString developers = "Developers:<br/><br/>Jiri Novak<br/>Laboratory of Molecular Structure Characterization<br/>Institute of Microbiology<br/>Czech Academy of Sciences<br/>Videnska 1083<br/>142 20 Prague<br/>Czech Republic<br/><a href=\"mailto:jiri.novak@biomed.cas.cz\">jiri.novak@biomed.cas.cz</a><br/><br/>(C) 2013 - 2020<br/><hr/><br/>";

	QString splash = "<div align=\"center\"><img src=\":/images/splash.png\"></div><br/><hr/><br/>";

	QString acknowledgement = "Acknowledgements:<br/><br/>This application uses <a href=\"https://www.qt.io/\">Qt library</a> under <a href=\"https://www.gnu.org/licenses/lgpl-3.0.en.html\">LGPLv3</a> license. Thanks to <a href=\"https://www.aha-soft.com/\">Aha-soft</a> and <a href=\"https://icons8.com/\">Icons8</a> for icons.<br/><hr/><br/>";

	message = new QTextBrowser();
	message->setReadOnly(true);
	message->setAcceptRichText(true);
	message->setOpenExternalLinks(true);
	message->setHtml("<br/>" + splash + "<b>" + appname + " " + appversion + "</b><br/><br/>" + homepage + citations + developers + licence + acknowledgement);

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

