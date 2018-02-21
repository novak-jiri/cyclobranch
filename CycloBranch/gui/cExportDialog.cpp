#include "gui/cExportDialog.h"

#include "gui/cSpectrumDetailWidget.h"


cExportDialog::cExportDialog(QWidget* parent) {
	this->parent = parent;

	exportspectrum = new QRadioButton();
	exportspectrum->setChecked(true);
	exportpeptide = new QRadioButton();
	exportbutton = new QPushButton("Export");
	connect(exportbutton, SIGNAL(released()), this, SLOT(processExport()));

	exportbox = new QHBoxLayout();
	exportbox->addWidget(exportspectrum);
	exportbox->addWidget(exportbutton);

	formlayout = new QFormLayout();
	formlayout->addRow("Export spectrum: ", exportbox);
	formlayout->addRow("Export peptide: ", exportpeptide);
	
	setLayout(formlayout);
	setWindowTitle("Export Image");
	setMinimumSize(300, 50);
}


cExportDialog::~cExportDialog() {
	delete exportspectrum;
	delete exportbutton;
	delete exportbox;

	delete exportpeptide;

	delete formlayout;
}


void cExportDialog::processExport() {
	close();

	QTextDocument::FindFlags flags = 0;

	if (exportspectrum->isChecked()) {
		((cSpectrumDetailWidget *)parent)->exportImage(true);	
	}
	else if (exportpeptide->isChecked()) {
		((cSpectrumDetailWidget *)parent)->exportImage(false);
	}
}

