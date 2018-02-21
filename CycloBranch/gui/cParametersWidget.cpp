#include "gui/cParametersWidget.h"

#include <QFormLayout>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSettings>
#include <QApplication>
#include <QComboBox>
#include <QListWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>


cParametersWidget::cParametersWidget() {
	setWindowTitle("Settings...");

	settingsfile = "";
	oldsettingsfile = "";
	peaklistfilename = "";
	brickdatabasefilename = "";
	sequencedatabasefilename = "";

	stdbuttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
	stdbuttons->button(QDialogButtonBox::Ok)->setToolTip("Accept changes and hide window.");
	stdbuttons->button(QDialogButtonBox::Apply)->setToolTip("Accept changes and keep window opened.");
	stdbuttons->button(QDialogButtonBox::Cancel)->setToolTip("Drop changes and hide window.");
	load = new QPushButton(tr("Load"));
	load->setToolTip("Load settings from a file (*.ini).");
	save = new QPushButton(QString("Save"));
	save->setToolTip("Save settings in the current file (*.ini). When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");
	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save settings into a file (*.ini).");

	buttons = new QHBoxLayout();
	buttons->addWidget(stdbuttons);
	buttons->addStretch(1);
	buttons->addWidget(load);
	buttons->addWidget(save);
	buttons->addWidget(saveas);


	peaklistformlayout = new QFormLayout();

	peptidetype = new QComboBox();
	peptidetype->setToolTip("Select the type of peptide best corresponding to the peaklist.");
	peptidetype->addItem(tr("Linear"));
	peptidetype->addItem(tr("Cyclic"));
	peptidetype->addItem(tr("Branched"));
	peptidetype->addItem(tr("Lasso"));
	peptidetype->addItem(tr("Linear polysaccharide (beta version)"));
	//peptidetype->addItem(tr("Other"));
	peaklistformlayout->addRow(tr("Peptide Type: "), peptidetype);

	peaklistline = new QLineEdit();
	peaklistline->setToolTip("Select the peaklist. Following formats are supported: txt, mgf, mzML, mzXML, baf.");
	peaklistbutton = new QPushButton("Select");
	peaklistbutton->setToolTip("Select the peaklist. Following formats are supported: txt, mgf, mzML, mzXML, baf.");
	peaklistlayout = new QHBoxLayout();
	peaklistlayout->addWidget(peaklistline);
	peaklistlayout->addWidget(peaklistbutton);
	peaklistformlayout->addRow(tr("Peaklist File: "), peaklistlayout);

	precursormass = new QDoubleSpinBox();
	precursormass->setToolTip("Enter the precursor mass-to-charge (m/z) ratio (a precursor m/z ratio in the peaklist file is ignored). The value will be automatically decharged.");
	precursormass->setDecimals(6);
	precursormass->setRange(0, 100000);
	precursormass->setSingleStep(1);
	peaklistformlayout->addRow(tr("Precursor m/z Ratio: "), precursormass);

	precursorcharge = new QSpinBox();
	precursorcharge->setToolTip("Enter the precursor charge (a precursor charge in the peaklist file is ignored).");
	precursorcharge->setRange(1, 100);
	precursorcharge->setSingleStep(1);
	peaklistformlayout->addRow(tr("Precursor Charge: "), precursorcharge);

	precursormasserrortolerance = new QDoubleSpinBox();
	precursormasserrortolerance->setToolTip("Enter the precursor m/z error tolerance in ppm.");
	precursormasserrortolerance->setDecimals(3);
	precursormasserrortolerance->setRange(0, 10000);
	precursormasserrortolerance->setSingleStep(1);
	precursormasserrortolerance->setSuffix(" ppm");
	peaklistformlayout->addRow(tr("Precursor m/z Error Tolerance: "), precursormasserrortolerance);

	fragmentmasserrortolerance = new QDoubleSpinBox();
	fragmentmasserrortolerance->setToolTip("Enter the fragment m/z error tolerance in ppm.");
	fragmentmasserrortolerance->setDecimals(3);
	fragmentmasserrortolerance->setRange(0, 10000);
	fragmentmasserrortolerance->setSingleStep(1);
	fragmentmasserrortolerance->setSuffix(" ppm");
	peaklistformlayout->addRow(tr("Fragment m/z Error Tolerance: "), fragmentmasserrortolerance);

	masserrortolerancefordeisotoping = new QDoubleSpinBox();
	masserrortolerancefordeisotoping->setToolTip("Enter the fragment m/z error tolerance for deisotoping in ppm (the same value like \"Fragment m/z Error Tolerance\" is recommended by default; 0 = the deisotoping is disabled).");
	masserrortolerancefordeisotoping->setDecimals(3);
	masserrortolerancefordeisotoping->setRange(0, 10000);
	masserrortolerancefordeisotoping->setSingleStep(1);
	masserrortolerancefordeisotoping->setSuffix(" ppm");
	peaklistformlayout->addRow(tr("Fragment m/z Error Tolerance for Deisotoping: "), masserrortolerancefordeisotoping);

	minimumrelativeintensitythreshold = new QDoubleSpinBox();
	minimumrelativeintensitythreshold->setToolTip("Enter the threshold of relative intensity in %. Peaks having relative intensities below the threshold will be removed from the peaklist.");
	minimumrelativeintensitythreshold->setDecimals(3);
	minimumrelativeintensitythreshold->setRange(0, 100);
	minimumrelativeintensitythreshold->setSingleStep(1);
	peaklistformlayout->addRow(tr("Minimum Threshold of Relative Intensity: "), minimumrelativeintensitythreshold);

	minimummz = new QDoubleSpinBox();
	minimummz->setToolTip("Enter the minimum m/z ratio. Peaks having m/z ratios below the threshold will be removed from the peaklist.");
	minimummz->setDecimals(3);
	minimummz->setRange(0, 10000);
	minimummz->setSingleStep(1);
	peaklistformlayout->addRow(tr("Minimum m/z Ratio: "), minimummz);

	peaklistgroupbox = new QGroupBox("Peaklist");
	peaklistgroupbox->setLayout(peaklistformlayout);


	brickdatabaseformlayout = new QFormLayout();

	brickdatabaseline = new QLineEdit();
	brickdatabaseline->setToolTip("Select the txt file containing a list of building blocks.");
	brickdatabasebutton = new QPushButton("Select");
	brickdatabasebutton->setToolTip("Select the txt file containing a list of building blocks.");
	brickdatabaselayout = new QHBoxLayout();
	brickdatabaselayout->addWidget(brickdatabaseline);
	brickdatabaselayout->addWidget(brickdatabasebutton);
	brickdatabaseformlayout->addRow(tr("Building Blocks Database File: "), brickdatabaselayout);

	maximumbricksincombinationbegin = new QSpinBox();
	maximumbricksincombinationbegin->setToolTip("Maximum number of combined building blocks to skip a gap leading from a start point in a de novo graph. A small value speeds up the search and vice versa.");
	maximumbricksincombinationbegin->setRange(1, 20);
	maximumbricksincombinationbegin->setSingleStep(1);
	brickdatabaseformlayout->addRow(tr("Maximum Number of Combined Blocks (start): "), maximumbricksincombinationbegin);

	maximumbricksincombinationmiddle = new QSpinBox();
	maximumbricksincombinationmiddle->setToolTip("Maximum number of combined building blocks to skip a gap in a de novo graph (except gaps leading from a start point or to an end point). A small value speeds up the search and vice versa.");
	maximumbricksincombinationmiddle->setRange(1, 20);
	maximumbricksincombinationmiddle->setSingleStep(1);
	brickdatabaseformlayout->addRow(tr("Maximum Number of Combined Blocks (middle): "), maximumbricksincombinationmiddle);

	maximumbricksincombinationend = new QSpinBox();
	maximumbricksincombinationend->setToolTip("Maximum number of combined building blocks to skip a gap leading to an end point in a de novo graph. A small value speeds up the search and vice versa.");
	maximumbricksincombinationend->setRange(1, 20);
	maximumbricksincombinationend->setSingleStep(1);
	brickdatabaseformlayout->addRow(tr("Maximum Number of Combined Blocks (end): "), maximumbricksincombinationend);

	maximumcumulativemass = new QDoubleSpinBox();
	maximumcumulativemass->setToolTip("Enter the maximum cumulative mass of combined blocks (0 = the maximum mass is unlimited). A small value speeds up the search and vice versa.");
	maximumcumulativemass->setDecimals(3);
	maximumcumulativemass->setRange(0, 10000);
	maximumcumulativemass->setSingleStep(1);
	brickdatabaseformlayout->addRow(tr("Maximum Cumulative Mass of Blocks: "), maximumcumulativemass);

	generatebrickspermutations = new QCheckBox();
	generatebrickspermutations->setToolTip("Permutations of combined blocks are generated when checked (e.g., when an edge corresponds to a combination of blocks leucine, proline and valine, the order of blocks can be LPV, LVP, PVL, PLV, VLP or VPL).\nIt is recommended to enable this option by default.");
	brickdatabaseformlayout->addRow(tr("Generate Permutations of Combined Blocks: "), generatebrickspermutations);

	brickdatabasegroupbox = new QGroupBox("Database of Building Blocks");
	brickdatabasegroupbox->setLayout(brickdatabaseformlayout);


	modificationslayout = new QVBoxLayout();

	modificationstable = new QTableWidget();
	modificationstable->setColumnCount(4);
	modificationstable->setHorizontalHeaderItem(0, new QTableWidgetItem("Name"));
	modificationstable->horizontalHeaderItem(0)->setToolTip("A name of a modification.");
	modificationstable->setHorizontalHeaderItem(1, new QTableWidgetItem("Summary"));
	modificationstable->horizontalHeaderItem(1)->setToolTip("A residue summary formula of a modification (e.g., H2C2O for acetylation or HNO-1 for amidation). Negative numbers of atoms are supported (e.g., H-2O3N-5 means remove two hydrogens, add three oxygens and remove five nitrogens).");
	modificationstable->setHorizontalHeaderItem(2, new QTableWidgetItem("N-terminal"));
	modificationstable->horizontalHeaderItem(2)->setToolTip("When checked, the modification is considered to be N-terminal.");
	modificationstable->setHorizontalHeaderItem(3, new QTableWidgetItem("C-terminal"));
	modificationstable->horizontalHeaderItem(3)->setToolTip("When checked, the modification is considered to be C-terminal.");
	modificationstable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	//modificationstable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
	
	modificationsbuttoninsert = new QPushButton("Insert Modification");
	modificationsbuttoninsert->setToolTip("Insert a new row for a modification.");
	modificationsbuttoninsert->setMinimumWidth(120);
	modificationsbuttoninsert->setMaximumWidth(120);
	modificationsbuttonremove = new QPushButton("Remove Empty Rows");
	modificationsbuttonremove->setToolTip("Remove rows having empty column \"Name\".");
	modificationsbuttonremove->setMinimumWidth(120);
	modificationsbuttonremove->setMaximumWidth(120);
	modificationsbuttonslayout = new QHBoxLayout();
	modificationsbuttonslayout->addWidget(modificationsbuttoninsert);
	modificationsbuttonslayout->addWidget(modificationsbuttonremove);
	modificationsbuttonslayout->addStretch(1);
	modificationslayout->addWidget(modificationstable);
	modificationslayout->addLayout(modificationsbuttonslayout);

	modificationsgroupbox = new QGroupBox("N-terminal and C-terminal Modifications");
	modificationsgroupbox->setLayout(modificationslayout);


	applicationformlayout = new QFormLayout();
	
	mode = new QComboBox();
	mode->setToolTip("De Novo Search Engine (identification mode) - the default mode of the application.\nCompare Peaklist with Spectrum of Searched Sequence (annotation mode) - a theoretical spectrum is generated for an input \"Searched Peptide Sequence\" and it is compared with the peaklist.\nCompare Peaklist with Database - MS/MS data (identification mode) - the peaklist is compared with theoretical spectra generated from a database of sequences.\nCompare Peaklist with Database - MS data (identification mode) - the peaklist is compared with theoretical peaks generated from a database of sequences.");
	mode->addItem(tr("De Novo Search Engine"));
	mode->addItem(tr("Compare Peaklist with Spectrum of Searched Sequence"));
	mode->addItem(tr("Compare Peaklist with Database - MS/MS data"));
	mode->addItem(tr("Compare Peaklist with Database - MS data"));
	applicationformlayout->addRow(tr("Mode: "), mode);

	maximumnumberofthreads = new QSpinBox();
	maximumnumberofthreads->setToolTip("A maximum number of threads used when the peaklist is compared with theoretical spectra of peptide sequence candidates.");
	maximumnumberofthreads->setRange(1, 1024);
	maximumnumberofthreads->setSingleStep(1);
	applicationformlayout->addRow(tr("Maximum Number of Threads: "), maximumnumberofthreads);

	scoretype = new QComboBox();
	scoretype->setToolTip("A score for peptide-spectrum matches.");
	scoretype->addItem(tr("Number of b-ions"));
	scoretype->addItem(tr("Number of b-ions + dehydrated b-ions"));
	scoretype->addItem(tr("Number of b-ions + deamidated b-ions"));
	scoretype->addItem(tr("Number of y-ions + b-ions (not for cyclic peptides)"));
	scoretype->addItem(tr("Number of y-ions (not for cyclic peptides)"));
	scoretype->addItem(tr("Sum of relative intensities of matched peaks"));
	scoretype->addItem(tr("Number of matched peaks"));
	scoretype->addItem(tr("Number of matched bricks (cyclic peptides)"));
	applicationformlayout->addRow(tr("Score Type: "), scoretype);

	hitsreported = new QSpinBox();
	hitsreported->setToolTip("A maximum length of an output report with peptide sequence candidates. A great value may slow down the search and a lot of main memory may be spent.");
	hitsreported->setRange(1, 100000);
	hitsreported->setSingleStep(1);
	applicationformlayout->addRow(tr("Maximum Number of Candidate Peptides Reported: "), hitsreported);

	sequencetag = new QLineEdit();
	sequencetag->setToolTip("Each peptide sequence candidate generated from a de novo graph must fulfil the peptide sequence tag. Otherwise, its theoretical spectrum is not generated and the peptide sequence candidate is excluded from the search.\nSee the syntax of tags in the documentation.");
	sequencetag->setMaxLength(1024*1024);
	applicationformlayout->addRow(tr("Peptide Sequence Tag: "), sequencetag);

	fragmentiontypes = new cFragmentIonsListWidget(this);
	fragmentiontypes->setToolTip("Select fragment ion types which will be generated in theoretical spectra of peptide sequence candidates.");
	applicationformlayout->addRow(tr("Fragment Ion Types in Theoretical Spectra: "), fragmentiontypes);

	clearhitswithoutparent = new QCheckBox();
	clearhitswithoutparent->setToolTip("When checked, a hit of a peak is not considered when corresponding parent peak is not hit (e.g., a hit of a dehydrated b-ion is not considered as a hit when corresponding b-ion has not been hit).");
	applicationformlayout->addRow(tr("Remove Hits of Fragments without Hits of Parent Fragments: "), clearhitswithoutparent);

	cyclicnterminus = new QCheckBox();
	cyclicnterminus->setToolTip("N-terminal cyclization of a linear peptide is assumed when checked. H<sub>2</sub>O is subtracted from all theoretical N-terminal fragment ions and the theoretical precursor mass.");
	applicationformlayout->addRow(tr("Cyclic N-terminus: "), cyclicnterminus);

	cycliccterminus = new QCheckBox();
	cycliccterminus->setToolTip("C-terminal cyclization of a linear peptide is assumed when checked. H<sub>2</sub>O is subtracted from all theoretical C-terminal fragment ions and the theoretical precursor mass.");
	applicationformlayout->addRow(tr("Cyclic C-terminus: "), cycliccterminus);

	enablescrambling = new QCheckBox();
	enablescrambling->setToolTip("When checked, scrambled fragment ions of cyclic peptides are generated in theoretical spectra.");
	applicationformlayout->addRow(tr("Enable Scrambling: "), enablescrambling);

	applicationgroupbox = new QGroupBox("Application");
	applicationgroupbox->setLayout(applicationformlayout);


	denovographformlayout = new QFormLayout();

	blindedges = new QComboBox();
	blindedges->setToolTip("An operation with edges in the de novo graph forming exclusively blind paths:\nnone (edges are kept - useful when you would like to see the whole de novo graph in 'View -> Graph');\nremove (edges are removed - speeds up the search);\nconnect (edges are connected - useful when you are looking for sequence tags).");
	blindedges->addItem(tr("none (you can see a complete de novo graph)"));
	blindedges->addItem(tr("remove (speed up the search)"));
	blindedges->addItem(tr("connect (allow detection of sequence tags)"));
	denovographformlayout->addRow(tr("Action with Edges Forming Blind Paths: "), blindedges);

	denovographgroupbox = new QGroupBox("De Novo Graph");
	denovographgroupbox->setLayout(denovographformlayout);


	databasesearchformlayout = new QFormLayout();

	sequencedatabaseline = new QLineEdit();
	sequencedatabaseline->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabasebutton = new QPushButton("Select");
	sequencedatabasebutton->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabaselayout = new QHBoxLayout();
	sequencedatabaselayout->addWidget(sequencedatabaseline);
	sequencedatabaselayout->addWidget(sequencedatabasebutton);
	databasesearchformlayout->addRow(tr("Sequence Database File: "), sequencedatabaselayout);

	databasesearchgroupbox = new QGroupBox("Database Search");
	databasesearchgroupbox->setLayout(databasesearchformlayout);


	searchedsequenceformlayout = new QFormLayout();

	searchedsequence = new QLineEdit();
	searchedsequence->setToolTip("A peptide sequence which you are searching for or a peptide sequence tag.\nA peptide sequence must be entered when \"Mode\" is set up to \"Compare Peaklist with Spectrum of Searched Sequence\".\nOtherwise, it is similar to the option \"Peptide Sequence Tag\" with a difference that a peptide sequence candidate is not removed from the search but it is just highlighted in an output report of peptide sequence candidates.");
	searchedsequence->setMaxLength(1024*1024);
	searchedsequenceformlayout->addRow(tr("Sequence: "), searchedsequence);

	searchedsequenceNtermmodif = new QLineEdit();
	searchedsequenceNtermmodif->setToolTip("A name of an N-terminal modification as defined in the window \"N-terminal and C-terminal Modifications\" which belongs to the searched peptide.");
	searchedsequenceformlayout->addRow(tr("N-terminal Modification: "), searchedsequenceNtermmodif);

	searchedsequenceCtermmodif = new QLineEdit();
	searchedsequenceCtermmodif->setToolTip("A name of a C-terminal modification as defined in the window \"N-terminal and C-terminal Modifications\" which belongs to the searched peptide.");
	searchedsequenceformlayout->addRow(tr("C-terminal Modification: "), searchedsequenceCtermmodif);

	searchedsequenceTmodif = new QLineEdit();
	searchedsequenceTmodif->setToolTip("A name of an N-terminal or C-terminal modification as defined in the window \"N-terminal and C-terminal Modifications\"  which belongs to a branch of a searched peptide (branched and lasso peptides only).");
	searchedsequenceformlayout->addRow(tr("Branch Modification: "), searchedsequenceTmodif);

	searchedsequencegroupbox = new QGroupBox("Searched Peptide Sequence");
	searchedsequencegroupbox->setLayout(searchedsequenceformlayout);


	vlayout1 = new QVBoxLayout();
	vlayout1->addWidget(peaklistgroupbox);
	vlayout1->addWidget(brickdatabasegroupbox);
	vlayout1->addWidget(modificationsgroupbox);

	vlayout2 = new QVBoxLayout();
	vlayout2->addWidget(applicationgroupbox);
	vlayout2->addWidget(denovographgroupbox);
	vlayout2->addWidget(databasesearchgroupbox);
	vlayout2->addWidget(searchedsequencegroupbox);
	vlayout2->addStretch(1);
		

	hlayout = new QHBoxLayout();
	hlayout->addLayout(vlayout1);
	hlayout->addLayout(vlayout2);


	vlayout = new QVBoxLayout();
	vlayout->addLayout(hlayout);	
	vlayout->addStretch(1);
	vlayout->addLayout(buttons);

	setLayout(vlayout);

	resize(1280, 700);

	connect(load, SIGNAL(released()), this, SLOT(loadSettings()));
	connect(save, SIGNAL(released()), this, SLOT(saveSettings()));
	connect(saveas, SIGNAL(released()), this, SLOT(saveSettingsAs()));
	connect(stdbuttons, SIGNAL(accepted()), this, SLOT(updateParametersAndHide()));
	connect(stdbuttons->button(QDialogButtonBox::Apply), SIGNAL(released()), this, SLOT(updateParameters()));
	connect(stdbuttons, SIGNAL(rejected()), this, SLOT(restoreParameters()));
	connect(peaklistbutton, SIGNAL(released()), this, SLOT(peaklistButtonReleased()));
	connect(brickdatabasebutton, SIGNAL(released()), this, SLOT(brickDatabaseButtonReleased()));
	connect(modificationsbuttoninsert, SIGNAL(released()), this, SLOT(modificationsInsertButtonReleased()));
	connect(modificationsbuttonremove, SIGNAL(released()), this, SLOT(modificationsRemoveButtonReleased()));
	connect(peptidetype, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSettingsWhenPeptideTypeChanged(int)));
	connect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSettingsWhenModeChanged(int)));
	connect(fragmentiontypes, SIGNAL(resetReleased()), this, SLOT(resetFragmentIonTypes()));
	connect(sequencedatabasebutton, SIGNAL(released()), this, SLOT(sequenceDatabaseButtonReleased()));


	updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
	updateSettingsWhenModeChanged(mode->currentIndex());

	restoreParameters();


	lastdirloadsettings = "./Settings/";
	lastdirsavesettings = "./Settings/";
	lastdirselectpeaklist = "./PeakLists/";
	lastdirselectbricksdatabase = "./BrickDatabases/";
	lastdirselectsequencedatabase = "./SequenceDatabases/";
}


cParametersWidget::~cParametersWidget() {

	delete stdbuttons;
	delete load;
	delete save;
	delete saveas;
	delete buttons;

	delete peptidetype;
	delete peaklistline;
	delete peaklistbutton;
	delete peaklistlayout;
	delete precursormass;
	delete precursorcharge;
	delete precursormasserrortolerance;
	delete fragmentmasserrortolerance;
	delete masserrortolerancefordeisotoping;
	delete minimumrelativeintensitythreshold;
	delete minimummz;
	delete peaklistformlayout;
	delete peaklistgroupbox;

	delete brickdatabaseline;
	delete brickdatabasebutton;
	delete brickdatabaselayout;
	delete maximumbricksincombinationbegin;
	delete maximumbricksincombinationmiddle;
	delete maximumbricksincombinationend;
	delete maximumcumulativemass;
	delete generatebrickspermutations;
	delete brickdatabaseformlayout;
	delete brickdatabasegroupbox;
	
	deleteModificationsTableBody();

	for (int i = 0; i < modificationstable->columnCount(); i++) {
		delete modificationstable->horizontalHeaderItem(i);
	}

	delete modificationsbuttoninsert;
	delete modificationsbuttonremove;
	delete modificationsbuttonslayout;
	delete modificationstable;
	delete modificationslayout;
	delete modificationsgroupbox;

	delete mode;
	delete maximumnumberofthreads;
	delete scoretype;
	delete hitsreported;
	delete sequencetag;
	delete fragmentiontypes;
	delete clearhitswithoutparent;
	delete cyclicnterminus;
	delete cycliccterminus;
	delete enablescrambling;
	delete applicationformlayout;
	delete applicationgroupbox;

	delete blindedges;
	delete denovographformlayout;
	delete denovographgroupbox;

	delete sequencedatabaseline;
	delete sequencedatabasebutton;
	delete sequencedatabaselayout;
	delete databasesearchformlayout;
	delete databasesearchgroupbox;

	delete searchedsequence;
	delete searchedsequenceNtermmodif;
	delete searchedsequenceCtermmodif;
	delete searchedsequenceTmodif;
	delete searchedsequenceformlayout;
	delete searchedsequencegroupbox;

	delete vlayout1;
	delete vlayout2;
	delete hlayout;
	delete vlayout;
}


cParameters& cParametersWidget::getParameters() {
	return parameters;
}


void cParametersWidget::setAndRestoreParameters(cParameters& parameters) {
	settingsfile = "";
	oldsettingsfile = "";

	this->parameters = parameters;
	this->parameters.bricksdatabase.clear();
	this->parameters.peaklist.clear();
	this->parameters.fragmentionsfordenovograph.clear();
	this->parameters.sequencetag = this->parameters.originalsequencetag;
	this->parameters.searchedsequence = this->parameters.originalsearchedsequence;

	restoreParameters();
}


void cParametersWidget::closeEvent(QCloseEvent *event) {
	restoreParameters();
}


void cParametersWidget::deleteRow(int number) {
	for (int i = 0; i < modificationstable->columnCount(); i++) {
		if ((i == 0) || (i == 1)) {
			delete modificationstable->item(number, i);
		}
		else {
			delete modificationstable->cellWidget(number, i);
		}
	}
	modificationstable->removeRow(number);
}


void cParametersWidget::deleteModificationsTableBody() {
	while (modificationstable->rowCount() > 0) {
		deleteRow(modificationstable->rowCount() - 1);
	}
}


void cParametersWidget::modificationsTableInsertRow() {
	int row = modificationstable->rowCount();

	modificationstable->insertRow(row);

	modificationstable->setItem(row, 0, new QTableWidgetItem());
	modificationstable->item(row, 0)->setData(Qt::DisplayRole, "");

	modificationstable->setItem(row, 1, new QTableWidgetItem());
	modificationstable->item(row, 1)->setData(Qt::DisplayRole, "");

	modificationstable->setCellWidget(row, 2, new QCheckBox());

	modificationstable->setCellWidget(row, 3, new QCheckBox());
}


void cParametersWidget::modificationsTableRemoveEmptyRows() {
	int i = 0;
	while (i < modificationstable->rowCount()) {
		if (modificationstable->item(i, 0)->text().toStdString().compare("") == 0) {
			deleteRow(i);
		}
		else {
			i++;
		}
	}
}


void cParametersWidget::loadSettings() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Settings"), lastdirloadsettings, tr("Settings Files (*.ini)"));
	lastdirloadsettings = filename;

	if (filename.toStdString().compare("") != 0) {
		int loadedrows = 0;
		QString qloadstring;
		QCheckBox* tmpcheckbox;

		settingsfile = filename;
		save->setText(QString("  Save '") + QString(settingsfile.toStdString().substr(settingsfile.toStdString().rfind('/') + 1, settingsfile.toStdString().size()).c_str()) + QString("'  "));
		QSettings settings(settingsfile, QSettings::IniFormat);

		peptidetype->setCurrentIndex(settings.value("peptidetype", 0).toInt());
		peaklistline->setText(settings.value("peaklist", "").toString());
		precursormass->setValue(settings.value("precursormass", 0.0).toDouble());
		precursormasserrortolerance->setValue(settings.value("precursormasserrortolerance", 5.0).toDouble());
		precursorcharge->setValue(settings.value("precursorcharge", 1).toInt());
		fragmentmasserrortolerance->setValue(settings.value("fragmentmasserrortolerance", 5.0).toDouble());
		masserrortolerancefordeisotoping->setValue(settings.value("masserrortolerancefordeisotoping", 5.0).toDouble());
		minimumrelativeintensitythreshold->setValue(settings.value("minimumrelativeintensitythreshold", 0).toDouble());
		minimummz->setValue(settings.value("minimummz", 150).toDouble());

		brickdatabaseline->setText(settings.value("brickdatabase", "").toString());
		maximumbricksincombinationbegin->setValue(settings.value("maximumbricksincombinationbegin", 3).toInt());
		maximumbricksincombinationmiddle->setValue(settings.value("maximumbricksincombinationmiddle", 2).toInt());
		maximumbricksincombinationend->setValue(settings.value("maximumbricksincombinationend", 3).toInt());
		maximumcumulativemass->setValue(settings.value("maximumcumulativemass", 0).toDouble());
		settings.value("generatebrickspermutations", 1).toInt() == 0 ? generatebrickspermutations->setChecked(false) : generatebrickspermutations->setChecked(true);

		deleteModificationsTableBody();

		qloadstring = ("modification_" + to_string(loadedrows) + "_0").c_str();
		while (settings.value(qloadstring, "").toString().toStdString().compare("") != 0) {
			modificationsTableInsertRow();

			modificationstable->item(loadedrows, 0)->setText(settings.value(qloadstring, "").toString());

			qloadstring = ("modification_" + to_string(loadedrows) + "_1").c_str();
			modificationstable->item(loadedrows, 1)->setText(settings.value(qloadstring, "").toString());

			tmpcheckbox = (QCheckBox *)(modificationstable->cellWidget(loadedrows, 2));
			qloadstring = ("modification_" + to_string(loadedrows) + "_2").c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? tmpcheckbox->setChecked(false) : tmpcheckbox->setChecked(true);

			tmpcheckbox = (QCheckBox *)(modificationstable->cellWidget(loadedrows, 3));
			qloadstring = ("modification_" + to_string(loadedrows) + "_3").c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? tmpcheckbox->setChecked(false) : tmpcheckbox->setChecked(true);

			loadedrows++;
			qloadstring = ("modification_" + to_string(loadedrows) + "_0").c_str();
		}

		mode->setCurrentIndex(settings.value("mode", 0).toInt());
		maximumnumberofthreads->setValue(settings.value("maximumnumberofthreads", 1).toInt());
		scoretype->setCurrentIndex(settings.value("scoretype", 0).toInt());
		hitsreported->setValue(settings.value("hitsreported", 1000).toInt());
		sequencetag->setText(settings.value("sequencetag", "").toString());
		
		for (int i = 0; i < fragmentiontypes->getList()->count(); i++) {
			qloadstring = ("fragmentiontype_" + to_string(i)).c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? fragmentiontypes->getList()->item(i)->setSelected(false) : fragmentiontypes->getList()->item(i)->setSelected(true);
		}

		settings.value("clearhitswithoutparent", 0).toInt() == 0 ? clearhitswithoutparent->setChecked(false) : clearhitswithoutparent->setChecked(true);
		settings.value("cyclicnterminus", 0).toInt() == 0 ? cyclicnterminus->setChecked(false) : cyclicnterminus->setChecked(true);
		settings.value("cycliccterminus", 0).toInt() == 0 ? cycliccterminus->setChecked(false) : cycliccterminus->setChecked(true);
		settings.value("enablescrambling", 0).toInt() == 0 ? enablescrambling->setChecked(false) : enablescrambling->setChecked(true);

		blindedges->setCurrentIndex(settings.value("blindedges", 2).toInt());

		sequencedatabaseline->setText(settings.value("sequencedatabase", "").toString());

		searchedsequence->setText(settings.value("searchedsequence", "").toString());
		searchedsequenceNtermmodif->setText(settings.value("searchedsequenceNtermmodif", "").toString());
		searchedsequenceCtermmodif->setText(settings.value("searchedsequenceCtermmodif", "").toString());
		searchedsequenceTmodif->setText(settings.value("searchedsequenceTmodif", "").toString());

	}

}
 

void cParametersWidget::saveSettings() {

	if (settingsfile.compare("") == 0) {
		saveSettingsAs();
		return;
	}

	int savedrows = 0;
	QString qsavestring;
	QCheckBox* tmpcheckbox;

	QSettings settings(settingsfile, QSettings::IniFormat);
	settings.clear();

	settings.setValue("peptidetype", peptidetype->currentIndex());
	settings.setValue("peaklist", peaklistline->text());
	settings.setValue("precursormass", precursormass->value());
	settings.setValue("precursormasserrortolerance", precursormasserrortolerance->value());
	settings.setValue("precursorcharge", precursorcharge->value());
	settings.setValue("fragmentmasserrortolerance", fragmentmasserrortolerance->value());
	settings.setValue("masserrortolerancefordeisotoping", masserrortolerancefordeisotoping->value());
	settings.setValue("minimumrelativeintensitythreshold", minimumrelativeintensitythreshold->value());
	settings.setValue("minimummz", minimummz->value());

	settings.setValue("brickdatabase", brickdatabaseline->text());
	settings.setValue("maximumbricksincombinationbegin", maximumbricksincombinationbegin->value());
	settings.setValue("maximumbricksincombinationmiddle", maximumbricksincombinationmiddle->value());
	settings.setValue("maximumbricksincombinationend", maximumbricksincombinationend->value());
	settings.setValue("maximumcumulativemass", maximumcumulativemass->value());
	generatebrickspermutations->isChecked() ? settings.setValue("generatebrickspermutations", 1) : settings.setValue("generatebrickspermutations", 0);

	modificationsTableRemoveEmptyRows();

	for (int i = 0; i < modificationstable->rowCount(); i++) {
		qsavestring = ("modification_" + to_string(savedrows) + "_0").c_str();
		settings.setValue(qsavestring, modificationstable->item(i, 0)->text());
		
		qsavestring = ("modification_" + to_string(savedrows) + "_1").c_str();
		settings.setValue(qsavestring, modificationstable->item(i, 1)->text());

		qsavestring = ("modification_" + to_string(savedrows) + "_2").c_str();
		tmpcheckbox = (QCheckBox *)(modificationstable->cellWidget(i, 2));
		tmpcheckbox->isChecked() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);
		
		qsavestring = ("modification_" + to_string(savedrows) + "_3").c_str();
		tmpcheckbox = (QCheckBox *)(modificationstable->cellWidget(i, 3));
		tmpcheckbox->isChecked() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

		savedrows++;
	}

	settings.setValue("mode", mode->currentIndex());
	settings.setValue("maximumnumberofthreads", maximumnumberofthreads->value());
	settings.setValue("scoretype", scoretype->currentIndex());
	settings.setValue("hitsreported", hitsreported->value());
	settings.setValue("sequencetag", sequencetag->text());

	for (int i = 0; i < fragmentiontypes->getList()->count(); i++) {
		qsavestring = ("fragmentiontype_" + to_string(i)).c_str();
		fragmentiontypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);
	}

	clearhitswithoutparent->isChecked() ? settings.setValue("clearhitswithoutparent", 1) : settings.setValue("clearhitswithoutparent", 0);
	cyclicnterminus->isChecked() ? settings.setValue("cyclicnterminus", 1) : settings.setValue("cyclicnterminus", 0);
	cycliccterminus->isChecked() ? settings.setValue("cycliccterminus", 1) : settings.setValue("cycliccterminus", 0);
	enablescrambling->isChecked() ? settings.setValue("enablescrambling", 1) : settings.setValue("enablescrambling", 0);

	settings.setValue("blindedges", blindedges->currentIndex());

	settings.setValue("sequencedatabase", sequencedatabaseline->text());

	settings.setValue("searchedsequence", searchedsequence->text());
	settings.setValue("searchedsequenceNtermmodif", searchedsequenceNtermmodif->text());
	settings.setValue("searchedsequenceCtermmodif", searchedsequenceCtermmodif->text());
	settings.setValue("searchedsequenceTmodif", searchedsequenceTmodif->text());

}


void cParametersWidget::saveSettingsAs() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Settings As..."), lastdirsavesettings, tr("Files (*.ini)"));
	lastdirsavesettings = filename;
	
	if (filename.toStdString().compare("") != 0) {
		settingsfile = filename;
		save->setText(QString("  Save '") + QString(settingsfile.toStdString().substr(settingsfile.toStdString().rfind('/') + 1, settingsfile.toStdString().size()).c_str()) + QString("'  "));
		saveSettings();
	}
}


void cParametersWidget::peaklistButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Peaklist..."), lastdirselectpeaklist, tr("Peak Lists (*.txt *.mgf *.mzML *.mzXML *.baf)"));
	lastdirselectpeaklist = filename;

	if (filename.toStdString().compare("") != 0) {
		peaklistfilename = filename.toStdString();
		peaklistline->setText(filename);
	}
}


void cParametersWidget::brickDatabaseButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Building Blocks Database..."), lastdirselectbricksdatabase, tr("Text Files (*.txt)"));
	lastdirselectbricksdatabase = filename;

	if (filename.toStdString().compare("") != 0) {
		brickdatabasefilename = filename.toStdString();
		brickdatabaseline->setText(filename);
	}
}


void cParametersWidget::sequenceDatabaseButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Sequence Database..."), lastdirselectsequencedatabase, tr("Text Files (*.txt)"));
	lastdirselectsequencedatabase = filename;

	if (filename.toStdString().compare("") != 0) {
		sequencedatabasefilename = filename.toStdString();
		sequencedatabaseline->setText(filename);
	}
}


void cParametersWidget::modificationsInsertButtonReleased() {
	modificationsTableInsertRow();
}


void cParametersWidget::modificationsRemoveButtonReleased() {
	modificationsTableRemoveEmptyRows();
}


bool cParametersWidget::updateParameters() {
	QMessageBox msgBox;
	QString errstr;

	if (peaklistline->text().toStdString().compare("") == 0) {
		errstr = "A peaklist must be specified.";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((brickdatabaseline->text().toStdString().compare("") == 0) && (((modeType)mode->currentIndex() == denovoengine) || ((modeType)mode->currentIndex() == singlecomparison) || ((modeType)mode->currentIndex() == databasesearch))) {
		errstr = "A database of building blocks must be specified.";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((sequencedatabaseline->text().toStdString().compare("") == 0) && (((modeType)mode->currentIndex() == databasesearch) || ((modeType)mode->currentIndex() == dereplication))) {
		errstr = "A sequence database must be specified.";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	parameters.peptidetype = (peptideType)peptidetype->currentIndex();
	parameters.peaklistfilename = peaklistline->text().toStdString();
	parameters.precursormass = precursormass->value();
	parameters.precursormasserrortolerance = precursormasserrortolerance->value();
	parameters.precursorcharge = precursorcharge->value();
	parameters.fragmentmasserrortolerance = fragmentmasserrortolerance->value();
	parameters.masserrortolerancefordeisotoping = masserrortolerancefordeisotoping->value();
	parameters.minimumrelativeintensitythreshold = minimumrelativeintensitythreshold->value();
	parameters.minimummz = minimummz->value();

	parameters.bricksdatabasefilename = brickdatabaseline->text().toStdString();
	parameters.maximumbricksincombinationbegin = maximumbricksincombinationbegin->value();
	parameters.maximumbricksincombinationmiddle = maximumbricksincombinationmiddle->value();
	parameters.maximumbricksincombinationend = maximumbricksincombinationend->value();
	parameters.maximumcumulativemass = maximumcumulativemass->value();
	parameters.maximumbricksincombination = max(max(parameters.maximumbricksincombinationbegin, parameters.maximumbricksincombinationmiddle), parameters.maximumbricksincombinationend);
	parameters.generatebrickspermutations = generatebrickspermutations->isChecked();

	QCheckBox* tmpcheckbox1;
	QCheckBox* tmpcheckbox2;

	parameters.searchedmodifications.clear();
	parameters.searchedmodifications.push_back(fragmentDescription("", 0, "", true, true));

	modificationsTableRemoveEmptyRows();
	
	int i = 0;
	while (i < modificationstable->rowCount()) {
		tmpcheckbox1 = (QCheckBox *)(modificationstable->cellWidget(i, 2));
		tmpcheckbox2 = (QCheckBox *)(modificationstable->cellWidget(i, 3));

		parameters.searchedmodifications.push_back(fragmentDescription(modificationstable->item(i, 0)->text().toStdString(), 0, modificationstable->item(i, 1)->text().toStdString(), tmpcheckbox1->isChecked(), tmpcheckbox2->isChecked()));

		i++;
	}

	parameters.mode = (modeType)mode->currentIndex();
	parameters.maximumnumberofthreads = maximumnumberofthreads->value();
	parameters.scoretype = (scoreType)scoretype->currentIndex();
	parameters.hitsreported = hitsreported->value();
	parameters.sequencetag = sequencetag->text().toStdString();
	parameters.originalsequencetag = parameters.sequencetag;

	parameters.fragmentionsfortheoreticalspectra.clear();
	int start;

	if ((modeType)mode->currentIndex() == dereplication) {
		start = ms_hplus;
	}
	else {
		switch ((peptideType)peptidetype->currentIndex())
		{
		case linear:
		case branched:
			start = b_ion;
			break;
		case cyclic:
			start = b_ion;
			break;
		case lasso:
			start = b_ion;
			break;
		case linearpolysaccharide:
			start = ms_nterminal_ion_hplus;
			break;
		case other:
		default:
			break;
		}
	}

	for (int i = 0; i < fragmentiontypes->getList()->count(); i++) {
		if (fragmentiontypes->getList()->item(i)->isSelected()) {
			parameters.fragmentionsfortheoreticalspectra.push_back((fragmentIonType)(i + start));
		}
	}

	parameters.clearhitswithoutparent = clearhitswithoutparent->isChecked();
	parameters.cyclicnterminus = cyclicnterminus->isChecked();
	parameters.cycliccterminus = cycliccterminus->isChecked();
	parameters.updateFragmentDefinitions();

	parameters.enablescrambling = enablescrambling->isChecked();

	parameters.blindedges = blindedges->currentIndex();

	parameters.sequencedatabasefilename = sequencedatabaseline->text().toStdString();

	parameters.searchedsequence = searchedsequence->text().toStdString();
	parameters.originalsearchedsequence = parameters.searchedsequence;
	parameters.searchedsequenceNtermmodif = searchedsequenceNtermmodif->text().toStdString();
	parameters.searchedsequenceCtermmodif = searchedsequenceCtermmodif->text().toStdString();
	parameters.searchedsequenceTmodif = searchedsequenceTmodif->text().toStdString();

	oldsettingsfile = settingsfile;

	return true;
}


void cParametersWidget::updateParametersAndHide() {
	if (updateParameters()) {
		hide();
	}
}


void cParametersWidget::restoreParameters() {
	peptidetype->setCurrentIndex(parameters.peptidetype);
	peaklistline->setText(parameters.peaklistfilename.c_str());
	precursormass->setValue(parameters.precursormass);
	precursormasserrortolerance->setValue(parameters.precursormasserrortolerance);
	precursorcharge->setValue(parameters.precursorcharge);
	fragmentmasserrortolerance->setValue(parameters.fragmentmasserrortolerance);
	masserrortolerancefordeisotoping->setValue(parameters.masserrortolerancefordeisotoping);
	minimumrelativeintensitythreshold->setValue(parameters.minimumrelativeintensitythreshold);
	minimummz->setValue(parameters.minimummz);

	brickdatabaseline->setText(parameters.bricksdatabasefilename.c_str());
	maximumbricksincombinationbegin->setValue(parameters.maximumbricksincombinationbegin);
	maximumbricksincombinationmiddle->setValue(parameters.maximumbricksincombinationmiddle);
	maximumbricksincombinationend->setValue(parameters.maximumbricksincombinationend);
	maximumcumulativemass->setValue(parameters.maximumcumulativemass);
	generatebrickspermutations->setChecked(parameters.generatebrickspermutations);

	QCheckBox* tmpcheckbox;

	deleteModificationsTableBody();

	int i = 1;
	while (i < (int)parameters.searchedmodifications.size()) {
		modificationsTableInsertRow();

		modificationstable->item(i - 1, 0)->setText(parameters.searchedmodifications[i].name.c_str());

		modificationstable->item(i - 1, 1)->setText(parameters.searchedmodifications[i].summary.c_str());

		tmpcheckbox = (QCheckBox *)(modificationstable->cellWidget(i - 1, 2));
		tmpcheckbox->setChecked(parameters.searchedmodifications[i].nterminal);

		tmpcheckbox = (QCheckBox *)(modificationstable->cellWidget(i - 1, 3));
		tmpcheckbox->setChecked(parameters.searchedmodifications[i].cterminal);

		i++;
	}

	mode->setCurrentIndex(parameters.mode);
	maximumnumberofthreads->setValue(parameters.maximumnumberofthreads);
	scoretype->setCurrentIndex(parameters.scoretype);
	hitsreported->setValue(parameters.hitsreported);
	sequencetag->setText(parameters.sequencetag.c_str());

	int start;
	if (parameters.mode == dereplication) {
		start = ms_hplus;
	}
	else {
		switch (parameters.peptidetype)
		{
		case linear:
		case branched:
			start = b_ion;
			break;
		case cyclic:
			start = b_ion;
			break;
		case lasso:
			start = b_ion;
			break;
		case linearpolysaccharide:
			start = ms_nterminal_ion_hplus;
			break;
		case other:
		default:
			break;
		}
	}

	for (int i = 0; i < (int)parameters.fragmentionsfortheoreticalspectra.size(); i++) {
		fragmentiontypes->getList()->item(parameters.fragmentionsfortheoreticalspectra[i] - start)->setSelected(true);
	}

	clearhitswithoutparent->setChecked(parameters.clearhitswithoutparent);
	cyclicnterminus->setChecked(parameters.cyclicnterminus);
	cycliccterminus->setChecked(parameters.cycliccterminus);
	enablescrambling->setChecked(parameters.enablescrambling);
	
	blindedges->setCurrentIndex(parameters.blindedges);

	sequencedatabaseline->setText(parameters.sequencedatabasefilename.c_str());

	searchedsequence->setText(parameters.searchedsequence.c_str());
	searchedsequenceNtermmodif->setText(parameters.searchedsequenceNtermmodif.c_str());
	searchedsequenceCtermmodif->setText(parameters.searchedsequenceCtermmodif.c_str());
	searchedsequenceTmodif->setText(parameters.searchedsequenceTmodif.c_str());

	settingsfile = oldsettingsfile;
	if (settingsfile.toStdString().compare("") == 0) {
		save->setText(QString("Save"));
	}
	else {
		save->setText(QString("  Save '") + QString(settingsfile.toStdString().substr(settingsfile.toStdString().rfind('/') + 1, settingsfile.toStdString().size()).c_str()) + QString("'  "));
	}

	hide();
}


void cParametersWidget::updateSettingsWhenPeptideTypeChanged(int index) {

	resetFragmentIonTypes();

	switch ((peptideType)index)
	{
	case linear:
		modificationsgroupbox->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(true);
		cyclicnterminus->setDisabled(false);
		cycliccterminus->setDisabled(false);
		enablescrambling->setDisabled(true);
		break;
	case cyclic:
		modificationsgroupbox->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(false);
		break;
	case branched:
		modificationsgroupbox->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(false);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		break;
	case lasso:
		modificationsgroupbox->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(false);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		break;
	case linearpolysaccharide:
		modificationsgroupbox->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		break;
	case other:
	default:
		break;
	}
}


void cParametersWidget::updateSettingsWhenModeChanged(int index) {

	switch ((modeType)index)
	{
	case denovoengine:
		peptidetype->setDisabled(false);
		precursormass->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(false);
		maximumbricksincombinationmiddle->setDisabled(false);
		maximumbricksincombinationend->setDisabled(false);
		maximumcumulativemass->setDisabled(false);
		generatebrickspermutations->setDisabled(false);
		maximumnumberofthreads->setDisabled(false);
		scoretype->setDisabled(false);
		hitsreported->setDisabled(false);
		sequencetag->setDisabled(false);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(false);
		denovographgroupbox->setDisabled(false);
		databasesearchgroupbox->setDisabled(true);
		searchedsequence->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case singlecomparison:
		peptidetype->setDisabled(false);
		precursormass->setDisabled(true);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(true);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
		generatebrickspermutations->setDisabled(true);
		maximumnumberofthreads->setDisabled(true);
		scoretype->setDisabled(true);
		hitsreported->setDisabled(true);
		sequencetag->setDisabled(true);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(false);
		denovographgroupbox->setDisabled(true);
		databasesearchgroupbox->setDisabled(true);
		searchedsequence->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case databasesearch:
		peptidetype->setDisabled(false);
		precursormass->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
		generatebrickspermutations->setDisabled(true);
		maximumnumberofthreads->setDisabled(false);
		scoretype->setDisabled(false);
		hitsreported->setDisabled(false);
		sequencetag->setDisabled(false);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(false);
		denovographgroupbox->setDisabled(true);
		databasesearchgroupbox->setDisabled(false);
		searchedsequence->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case dereplication:
		peptidetype->setDisabled(true);
		precursormass->setDisabled(true);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(true);
		brickdatabaseline->setDisabled(true);
		brickdatabasebutton->setDisabled(true);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
		generatebrickspermutations->setDisabled(true);
		maximumnumberofthreads->setDisabled(true);
		scoretype->setDisabled(true);
		hitsreported->setDisabled(true);
		sequencetag->setDisabled(true);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(true);
		denovographgroupbox->setDisabled(true);
		databasesearchgroupbox->setDisabled(false);
		searchedsequence->setDisabled(true);

		resetFragmentIonTypes();
		
		modificationsgroupbox->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		break;
	default:
		break;
	}

}


void cParametersWidget::resetFragmentIonTypes() {
	fragmentiontypes->getList()->clear();
	fragmentIonType start, end;


	if ((modeType)mode->currentIndex() == dereplication) {
		start = ms_hplus;
		end = ms_MFe4H;
	}
	else {
		switch ((peptideType)peptidetype->currentIndex()) {
		case linear:
		case branched:
			start = b_ion;
			end = y_ion_water_and_ammonia_loss;
			break;
		case cyclic:
			start = b_ion;
			end = a_ion_water_and_ammonia_loss;
			break;
		case lasso:
			start = b_ion;
			end = y_ion_water_and_ammonia_loss;
			break;
		case linearpolysaccharide:
			start = ms_nterminal_ion_hplus;
			end = ms_cterminal_ion_kplus;
			break;
		case other:
		default:
			break;
		}
	}

	for (int i = start; i <= end; i++) {
		fragmentiontypes->getList()->addItem(tr(parameters.fragmentdefinitions[(fragmentIonType)i].name.c_str()));

		if (parameters.fragmentdefinitions[(fragmentIonType)i].parent == (fragmentIonType)i) {
			fragmentiontypes->getList()->item(i-start)->setSelected(true);
		}
	}

}

