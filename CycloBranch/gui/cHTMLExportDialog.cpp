#include "gui/cHTMLExportDialog.h"


cHTMLExportDialog::cHTMLExportDialog(QWidget* parent) {
	this->parent = parent;

	checkboxparameters = new QCheckBox();
	checkboxdenovo = new QCheckBox();
	checkboxlogwindow = new QCheckBox();
	checkboxsummaryresultstable = new QCheckBox();
	checkboxsummarypeakstable = new QCheckBox();
	checkboxpeakstable = new QCheckBox();
	checkboxunmatchedexperimental = new QCheckBox();
	checkboxunmatchedtheoretical = new QCheckBox();
	checkboxdetails = new QCheckBox();

	form1 = new QFormLayout();
	form1->addRow("Parameters", checkboxparameters);
	form1->addRow("De Novo Graph", checkboxdenovo);
	form1->addRow("Log Window", checkboxlogwindow);
	form1->addRow("Output Report Table", checkboxsummaryresultstable);
	form1->addRow("Summary Table of Matched Peaks", checkboxsummarypeakstable);

	group1 = new QGroupBox("Summary");
	group1->setLayout(form1);
	
	form2 = new QFormLayout();
	form2->addRow("Peaks Table", checkboxpeakstable);
	form2->addRow("Unmatched Experimental Peaks", checkboxunmatchedexperimental);
	form2->addRow("Unmatched Theoretical Peaks", checkboxunmatchedtheoretical);
	form2->addRow("Details of Identification", checkboxdetails);

	group2 = new QGroupBox("Individual Rows in Output Report Table");
	group2->setLayout(form2);

	hbox = new QHBoxLayout();
	hbox->addWidget(group1);
	hbox->addWidget(group2);

	buttonbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);

	vbox = new QVBoxLayout();
	vbox->addLayout(hbox);
	vbox->addWidget(buttonbox);

	setLayout(vbox);
	setWindowTitle("Export HTML report");
	setMinimumWidth(500);

	connect(buttonbox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonbox, SIGNAL(rejected()), this, SLOT(reject()));
}


cHTMLExportDialog::~cHTMLExportDialog() {
	delete checkboxparameters;
	delete checkboxdenovo;
	delete checkboxlogwindow;
	delete checkboxsummaryresultstable;
	delete checkboxsummarypeakstable;
	delete checkboxpeakstable;
	delete checkboxunmatchedexperimental;
	delete checkboxunmatchedtheoretical;
	delete checkboxdetails;

	delete form1;
	delete form2;

	delete group1;
	delete group2;

	delete hbox;
	delete buttonbox;

	delete vbox;
}

