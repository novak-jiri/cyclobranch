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
#include <QKeyEvent>
#include <QIcon>
#include <QScrollArea>


cParametersWidget::cParametersWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Settings...");
	setWindowIcon(QIcon(":/images/icons/73.png"));

	settingsfile = "";
	oldsettingsfile = "";

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
	peptidetype->addItem(tr("Branch-cyclic"));
	peptidetype->addItem(tr("Linear polysaccharide (beta version)"));
	//peptidetype->addItem(tr("Other"));
	peaklistformlayout->addRow(tr("Peptide Type: "), peptidetype);

	peaklistline = new QLineEdit();
	peaklistbutton = new QPushButton("Select");
	#if OS_TYPE != WIN
		peaklistline->setToolTip("Select the peaklist. Following formats are supported: txt, mgf, mzML, mzXML.");
		peaklistbutton->setToolTip("Select the peaklist. Following formats are supported: txt, mgf, mzML, mzXML.");
	#else
		peaklistline->setToolTip("Select the peaklist. Following formats are supported: txt, mgf, mzML, mzXML, baf.");
		peaklistbutton->setToolTip("Select the peaklist. Following formats are supported: txt, mgf, mzML, mzXML, baf.");
	#endif
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

	precursoradduct = new QLineEdit();
	precursoradduct->setToolTip("Enter the formula of a precursor ion adduct (e.g., Na, K, Li). H is used by default when empty.");
	peaklistformlayout->addRow(tr("Precursor Ion Adduct: "), precursoradduct);

	precursorcharge = new QSpinBox();
	precursorcharge->setToolTip("Enter the precursor charge (a precursor charge in the peaklist file is ignored). Negative values are allowed.\nWhen mode is set up to \"Compare Peaklist with Database - MS data\", the value determines the maximum charge of generated peaks.");
	precursorcharge->setRange(-100, 100);
	precursorcharge->setSingleStep(1);
	peaklistformlayout->addRow(tr("Charge: "), precursorcharge);

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

	modificationsline = new QLineEdit();
	modificationsline->setToolTip("Select the txt file containing a list of modifications.");
	modificationsbutton = new QPushButton("Select");
	modificationsbutton->setToolTip("Select the txt file containing a list of modifications.");
	modificationslayout = new QHBoxLayout();
	modificationslayout->addWidget(modificationsline);
	modificationslayout->addWidget(modificationsbutton);
	brickdatabaseformlayout->addRow(tr("N-/C-terminal Modifications File: "), modificationslayout);

	brickdatabasegroupbox = new QGroupBox("Database of Building Blocks");
	brickdatabasegroupbox->setLayout(brickdatabaseformlayout);


	miscformlayout = new QFormLayout();

	blindedges = new QComboBox();
	blindedges->setToolTip("An operation which is performed with edges in the de novo graph forming exclusively blind paths:\nnone (edges are kept - useful when you would like to see the whole de novo graph in 'View -> Graph');\nremove (edges are removed - speeds up the search);\nconnect (edges are connected - useful when you are looking for sequence tags).");
	blindedges->addItem(tr("none (you can see a complete de novo graph)"));
	blindedges->addItem(tr("remove (speed up the search)"));
	blindedges->addItem(tr("connect (allow detection of sequence tags)"));
	miscformlayout->addRow(tr("Action with Blind Paths in De Novo Graph: "), blindedges);

	cyclicnterminus = new QCheckBox();
	cyclicnterminus->setToolTip("N-terminal cyclization of a linear peptide is assumed when checked. H<sub>2</sub>O is subtracted from all theoretical N-terminal fragment ions and the theoretical precursor mass.");
	miscformlayout->addRow(tr("Cyclic N-terminus: "), cyclicnterminus);

	cycliccterminus = new QCheckBox();
	cycliccterminus->setToolTip("C-terminal cyclization of a linear peptide is assumed when checked. H<sub>2</sub>O is subtracted from all theoretical C-terminal fragment ions and the theoretical precursor mass.");
	miscformlayout->addRow(tr("Cyclic C-terminus: "), cycliccterminus);

	enablescrambling = new QCheckBox();
	enablescrambling->setToolTip("When checked, scrambled fragment ions of cyclic peptides are generated in theoretical spectra.");
	miscformlayout->addRow(tr("Enable Scrambling: "), enablescrambling);

	similaritysearch = new QCheckBox();
	similaritysearch->setToolTip("It enables similarity search when a peaklist is searched against a database of sequences.\nThis feature disables filtering of peptide sequence candidates by precursor mass.\nIt may be useful to determine a peptide family when a similar peptide is contained in the database.");
	miscformlayout->addRow(tr("Enable Similarity Search: "), similaritysearch);

	miscgroupbox = new QGroupBox("Miscellaneous");
	miscgroupbox->setLayout(miscformlayout);


	applicationformlayout = new QFormLayout();
	
	mode = new QComboBox();
	mode->setToolTip("De Novo Search Engine (identification mode) - the default mode of the application.\nCompare Peaklist with Spectrum of Searched Sequence (annotation mode) - a theoretical spectrum is generated for an input \"Searched Peptide Sequence\" and it is compared with the peaklist.\nCompare Peaklist with Database - MS/MS data (identification mode) - a peaklist is compared with theoretical spectra generated from a database of sequences.\nCompare Peaklist with Database - MS data (identification mode) - a peaklist is compared with theoretical peaks generated from a database of compounds/sequences.");
	mode->addItem(tr("De Novo Search Engine"));
	mode->addItem(tr("Compare Peaklist with Spectrum of Searched Sequence"));
	mode->addItem(tr("Compare Peaklist with Database - MS/MS data"));
	mode->addItem(tr("Compare Peaklist with Database - MS data"));
	applicationformlayout->addRow(tr("Mode: "), mode);

	sequencedatabaseline = new QLineEdit();
	sequencedatabaseline->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabasebutton = new QPushButton("Select");
	sequencedatabasebutton->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabaselayout = new QHBoxLayout();
	sequencedatabaselayout->addWidget(sequencedatabaseline);
	sequencedatabaselayout->addWidget(sequencedatabasebutton);
	applicationformlayout->addRow(tr("Sequence Database File: "), sequencedatabaselayout);

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
	sequencetag->setMaxLength(5000);
	applicationformlayout->addRow(tr("Peptide Sequence Tag: "), sequencetag);

	fragmentiontypes = new cFragmentIonsListWidget(this);
	fragmentiontypes->setToolTip("Select fragment ion types which will be generated in theoretical spectra of peptide sequence candidates.");
	applicationformlayout->addRow(tr("Fragment Ion Types in Theoretical Spectra: "), fragmentiontypes);

	clearhitswithoutparent = new QCheckBox();
	clearhitswithoutparent->setToolTip("When checked, a hit of a peak is not considered when corresponding parent peak is not hit (e.g., a hit of a dehydrated b-ion is not considered as a hit when corresponding b-ion has not been hit).");
	applicationformlayout->addRow(tr("Remove Hits of Fragments without Hits of Parent Fragments: "), clearhitswithoutparent);

	applicationgroupbox = new QGroupBox("Application");
	applicationgroupbox->setLayout(applicationformlayout);


	searchedsequenceformlayout = new QFormLayout();

	searchedsequenceline = new QLineEdit();
	searchedsequenceline->setToolTip("A peptide sequence which you are searching for or a peptide sequence tag.\nA peptide sequence must be entered when \"Mode\" is set up to \"Compare Peaklist with Spectrum of Searched Sequence\".\nOtherwise, it is similar to the option \"Peptide Sequence Tag\" with a difference that a peptide sequence candidate is not removed from the search but it is just highlighted in an output report of peptide sequence candidates.");
	searchedsequenceline->setMaxLength(5000);
	searchedsequencebutton = new QPushButton("Edit");
	searchedsequencebutton->setMinimumWidth(50);
	searchedsequencebutton->setToolTip("Edit the sequence using the 'Draw Peptide' tool.");
	searchedsequencelayout = new QHBoxLayout();
	searchedsequencelayout->addWidget(searchedsequenceline);
	searchedsequencelayout->addWidget(searchedsequencebutton);
	searchedsequenceformlayout->addRow(tr("Sequence: "), searchedsequencelayout);

	searchedsequenceNtermmodif = new QLineEdit();
	searchedsequenceNtermmodif->setToolTip("A name of an N-terminal modification which belongs to the searched peptide. The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceformlayout->addRow(tr("N-terminal Modification: "), searchedsequenceNtermmodif);

	searchedsequenceCtermmodif = new QLineEdit();
	searchedsequenceCtermmodif->setToolTip("A name of a C-terminal modification which belongs to the searched peptide. The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceformlayout->addRow(tr("C-terminal Modification: "), searchedsequenceCtermmodif);

	searchedsequenceTmodif = new QLineEdit();
	searchedsequenceTmodif->setToolTip("A name of an N-terminal or C-terminal modification which belongs to a branch of a searched peptide (branched and branch-cyclic peptides only). The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceformlayout->addRow(tr("Branch Modification: "), searchedsequenceTmodif);

	searchedsequencegroupbox = new QGroupBox("Searched Peptide Sequence");
	searchedsequencegroupbox->setLayout(searchedsequenceformlayout);


	vlayout1 = new QVBoxLayout();
	vlayout1->addWidget(peaklistgroupbox);
	vlayout1->addWidget(brickdatabasegroupbox);
	vlayout1->addWidget(miscgroupbox);

	vlayout2 = new QVBoxLayout();
	vlayout2->addWidget(applicationgroupbox);
	vlayout2->addWidget(searchedsequencegroupbox);
		

	hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addLayout(vlayout1);
	hlayout->addLayout(vlayout2);

	hlayoutwidget = new QWidget();
	hlayoutwidget->setLayout(hlayout);

	hlayoutscroll = new QScrollArea();
	hlayoutscroll->setWidgetResizable(true);
	hlayoutscroll->setFrameShape(QFrame::NoFrame);
	hlayoutscroll->setWidget(hlayoutwidget);


	vlayout = new QVBoxLayout();
	vlayout->addWidget(hlayoutscroll);	
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
	connect(modificationsbutton, SIGNAL(released()), this, SLOT(modificationsButtonReleased()));
	connect(peptidetype, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSettingsWhenPeptideTypeChanged(int)));
	connect(mode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSettingsWhenModeChanged(int)));
	connect(fragmentiontypes, SIGNAL(resetReleased()), this, SLOT(resetFragmentIonTypes()));
	connect(sequencedatabasebutton, SIGNAL(released()), this, SLOT(sequenceDatabaseButtonReleased()));
	connect(searchedsequencebutton, SIGNAL(released()), this, SLOT(drawPeptideButtonReleased()));
	connect(this, SIGNAL(sendSequenceLine(int, QString)), parent, SLOT(setAndShowDrawPeptideWidget(int, QString)));


	updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
	updateSettingsWhenModeChanged(mode->currentIndex());

	restoreParameters();

	#if OS_TYPE == WIN
		lastdirloadsettings = "./Settings/";
		lastdirsavesettings = "./Settings/";
		lastdirselectpeaklist = "./PeakLists/";
		lastdirselectbricksdatabase = "./BrickDatabases/";
		lastdirselectmodifications = "./Modifications/";
		lastdirselectsequencedatabase = "./SequenceDatabases/";
	#else
		lastdirloadsettings = installdir + "Settings/";
		lastdirsavesettings = installdir + "Settings/";
		lastdirselectpeaklist = installdir + "PeakLists/";
		lastdirselectbricksdatabase = installdir + "BrickDatabases/";
		lastdirselectmodifications = installdir + "Modifications/";
		lastdirselectsequencedatabase = installdir + "SequenceDatabases/";
	#endif
	
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
	delete precursoradduct;
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
	delete modificationsline;
	delete modificationsbutton;
	delete modificationslayout;
	delete brickdatabaseformlayout;
	delete brickdatabasegroupbox;
	
	delete blindedges;
	delete cyclicnterminus;
	delete cycliccterminus;
	delete enablescrambling;
	delete similaritysearch;
	delete miscformlayout;
	delete miscgroupbox;

	delete mode;
	delete sequencedatabaseline;
	delete sequencedatabasebutton;
	delete sequencedatabaselayout;
	delete maximumnumberofthreads;
	delete scoretype;
	delete hitsreported;
	delete sequencetag;
	delete fragmentiontypes;
	delete clearhitswithoutparent;
	delete applicationformlayout;
	delete applicationgroupbox;

	delete searchedsequenceline;
	delete searchedsequencebutton;
	delete searchedsequencelayout;
	delete searchedsequenceNtermmodif;
	delete searchedsequenceCtermmodif;
	delete searchedsequenceTmodif;
	delete searchedsequenceformlayout;
	delete searchedsequencegroupbox;

	delete vlayout1;
	delete vlayout2;
	delete hlayout;
	delete hlayoutwidget;
	delete hlayoutscroll;
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


void cParametersWidget::setSequence(int peptidetypeindex, QString sequence) {
	peptidetype->setCurrentIndex(peptidetypeindex);
	searchedsequenceline->setText(sequence);
}


void cParametersWidget::setTag(int peptidetypeindex, QString tag) {
	peptidetype->setCurrentIndex(peptidetypeindex);
	sequencetag->setText(tag);
}


void cParametersWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		restoreParameters();
    }
}


void cParametersWidget::loadSettings() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load Settings"), lastdirloadsettings, tr("Settings Files (*.ini)"));

	if (!filename.isEmpty()) {
		lastdirloadsettings = filename;

		QString qloadstring;

		settingsfile = filename;
		save->setText(QString("  Save '") + QString(settingsfile.toStdString().substr(settingsfile.toStdString().rfind('/') + 1, settingsfile.toStdString().size()).c_str()) + QString("'  "));
		QSettings settings(settingsfile, QSettings::IniFormat);

		peptidetype->setCurrentIndex(settings.value("peptidetype", 0).toInt());
		peaklistline->setText(settings.value("peaklist", "").toString());
		precursormass->setValue(settings.value("precursormass", 0.0).toDouble());
		precursoradduct->setText(settings.value("precursoradduct", "").toString());
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
		modificationsline->setText(settings.value("modificationsfile", "").toString());

		blindedges->setCurrentIndex(settings.value("blindedges", 2).toInt());
		settings.value("cyclicnterminus", 0).toInt() == 0 ? cyclicnterminus->setChecked(false) : cyclicnterminus->setChecked(true);
		settings.value("cycliccterminus", 0).toInt() == 0 ? cycliccterminus->setChecked(false) : cycliccterminus->setChecked(true);
		settings.value("enablescrambling", 0).toInt() == 0 ? enablescrambling->setChecked(false) : enablescrambling->setChecked(true);
		settings.value("similaritysearch", 0).toInt() == 0 ? similaritysearch->setChecked(false) : similaritysearch->setChecked(true);

		mode->setCurrentIndex(settings.value("mode", 0).toInt());
		sequencedatabaseline->setText(settings.value("sequencedatabase", "").toString());
		maximumnumberofthreads->setValue(settings.value("maximumnumberofthreads", 1).toInt());
		scoretype->setCurrentIndex(settings.value("scoretype", 0).toInt());
		hitsreported->setValue(settings.value("hitsreported", 1000).toInt());
		sequencetag->setText(settings.value("sequencetag", "").toString());
		
		for (int i = 0; i < fragmentiontypes->getList()->count(); i++) {
			qloadstring = ("fragmentiontype_" + to_string(i)).c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? fragmentiontypes->getList()->item(i)->setSelected(false) : fragmentiontypes->getList()->item(i)->setSelected(true);
		}

		settings.value("clearhitswithoutparent", 0).toInt() == 0 ? clearhitswithoutparent->setChecked(false) : clearhitswithoutparent->setChecked(true);

		searchedsequenceline->setText(settings.value("searchedsequence", "").toString());
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

	QString qsavestring;

	QSettings settings(settingsfile, QSettings::IniFormat);
	settings.clear();

	settings.setValue("peptidetype", peptidetype->currentIndex());
	settings.setValue("peaklist", peaklistline->text());
	settings.setValue("precursormass", precursormass->value());
	settings.setValue("precursoradduct", precursoradduct->text());
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
	settings.setValue("modificationsfile", modificationsline->text());

	settings.setValue("blindedges", blindedges->currentIndex());
	cyclicnterminus->isChecked() ? settings.setValue("cyclicnterminus", 1) : settings.setValue("cyclicnterminus", 0);
	cycliccterminus->isChecked() ? settings.setValue("cycliccterminus", 1) : settings.setValue("cycliccterminus", 0);
	enablescrambling->isChecked() ? settings.setValue("enablescrambling", 1) : settings.setValue("enablescrambling", 0);
	similaritysearch->isChecked() ? settings.setValue("similaritysearch", 1) : settings.setValue("similaritysearch", 0);

	settings.setValue("mode", mode->currentIndex());
	settings.setValue("sequencedatabase", sequencedatabaseline->text());
	settings.setValue("maximumnumberofthreads", maximumnumberofthreads->value());
	settings.setValue("scoretype", scoretype->currentIndex());
	settings.setValue("hitsreported", hitsreported->value());
	settings.setValue("sequencetag", sequencetag->text());

	for (int i = 0; i < fragmentiontypes->getList()->count(); i++) {
		qsavestring = ("fragmentiontype_" + to_string(i)).c_str();
		fragmentiontypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);
	}

	clearhitswithoutparent->isChecked() ? settings.setValue("clearhitswithoutparent", 1) : settings.setValue("clearhitswithoutparent", 0);

	settings.setValue("searchedsequence", searchedsequenceline->text());
	settings.setValue("searchedsequenceNtermmodif", searchedsequenceNtermmodif->text());
	settings.setValue("searchedsequenceCtermmodif", searchedsequenceCtermmodif->text());
	settings.setValue("searchedsequenceTmodif", searchedsequenceTmodif->text());

}


void cParametersWidget::saveSettingsAs() {
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Settings As..."), lastdirsavesettings, tr("Files (*.ini)"));
	
	if (!filename.isEmpty()) {
		lastdirsavesettings = filename;
	
		settingsfile = filename;
		save->setText(QString("  Save '") + QString(settingsfile.toStdString().substr(settingsfile.toStdString().rfind('/') + 1, settingsfile.toStdString().size()).c_str()) + QString("'  "));
		saveSettings();
	}
}


void cParametersWidget::peaklistButtonReleased() {
	#if OS_TYPE != WIN
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Peaklist..."), lastdirselectpeaklist, tr("Peak Lists (*.txt *.mgf *.mzML *.mzXML)"));
	#else
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Peaklist..."), lastdirselectpeaklist, tr("Peak Lists (*.txt *.mgf *.mzML *.mzXML *.baf)"));
	#endif

	if (!filename.isEmpty()) {
		lastdirselectpeaklist = filename;
		peaklistline->setText(filename);
	}
}


void cParametersWidget::brickDatabaseButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Building Blocks Database..."), lastdirselectbricksdatabase, tr("Text Files (*.txt)"));
	
	if (!filename.isEmpty()) {
		lastdirselectbricksdatabase = filename;
		brickdatabaseline->setText(filename);
	}
}


void cParametersWidget::modificationsButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Modifications File..."), lastdirselectmodifications, tr("Text Files (*.txt)"));
	
	if (!filename.isEmpty()) {
		lastdirselectmodifications = filename;
		modificationsline->setText(filename);
	}
}


void cParametersWidget::sequenceDatabaseButtonReleased() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Select Sequence Database..."), lastdirselectsequencedatabase, tr("Text Files (*.txt)"));

	if (!filename.isEmpty()) {
		lastdirselectsequencedatabase = filename;
		sequencedatabaseline->setText(filename);
	}
}


bool cParametersWidget::updateParameters() {
	QMessageBox msgBox;
	QString errstr;

	if (peaklistline->text().toStdString().compare("") == 0) {
		errstr = "A peaklist must be specified!";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((brickdatabaseline->text().toStdString().compare("") == 0) && (((modeType)mode->currentIndex() == denovoengine) || ((modeType)mode->currentIndex() == singlecomparison) || ((modeType)mode->currentIndex() == databasesearch))) {
		errstr = "A database of building blocks must be specified!";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((sequencedatabaseline->text().toStdString().compare("") == 0) && (((modeType)mode->currentIndex() == databasesearch) || ((modeType)mode->currentIndex() == dereplication))) {
		errstr = "A sequence database must be specified!";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if (precursorcharge->value() == 0) {
		errstr = "The charge cannot be zero!";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	/*
	if ((maximumbricksincombinationmiddle->value() < 2) && ((modeType)mode->currentIndex() == denovoengine) && (((peptideType)peptidetype->currentIndex() == branched) || ((peptideType)peptidetype->currentIndex() == lasso))) {
		errstr = "'Maximum Number of Combined Blocks (middle)' must be at least 2 when a branched or a branch-cyclic peptide is searched! (One block represents a branched residue, the other block(s) corresponds to a branch.)";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}
	*/

	parameters.peptidetype = (peptideType)peptidetype->currentIndex();
	parameters.peaklistfilename = peaklistline->text().toStdString();
	parameters.precursormass = precursormass->value();
	parameters.precursoradduct = precursoradduct->text().toStdString();
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
	parameters.modificationsfilename = modificationsline->text().toStdString();

	parameters.blindedges = blindedges->currentIndex();
	parameters.cyclicnterminus = cyclicnterminus->isChecked();
	parameters.cycliccterminus = cycliccterminus->isChecked();
	parameters.enablescrambling = enablescrambling->isChecked();
	parameters.similaritysearch = similaritysearch->isChecked();

	parameters.mode = (modeType)mode->currentIndex();
	parameters.sequencedatabasefilename = sequencedatabaseline->text().toStdString();
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
			start = a_ion;
			break;
		case cyclic:
			start = a_ion;
			break;
		case lasso:
			start = a_ion;
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

	parameters.searchedsequence = searchedsequenceline->text().toStdString();
	parameters.originalsearchedsequence = parameters.searchedsequence;
	parameters.searchedsequenceNtermmodif = searchedsequenceNtermmodif->text().toStdString();
	parameters.searchedsequenceCtermmodif = searchedsequenceCtermmodif->text().toStdString();
	parameters.searchedsequenceTmodif = searchedsequenceTmodif->text().toStdString();

	parameters.updateFragmentDefinitions();

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
	precursoradduct->setText(parameters.precursoradduct.c_str());
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
	modificationsline->setText(parameters.modificationsfilename.c_str());

	blindedges->setCurrentIndex(parameters.blindedges);
	cyclicnterminus->setChecked(parameters.cyclicnterminus);
	cycliccterminus->setChecked(parameters.cycliccterminus);
	enablescrambling->setChecked(parameters.enablescrambling);
	similaritysearch->setChecked(parameters.similaritysearch);

	mode->setCurrentIndex(parameters.mode);
	sequencedatabaseline->setText(parameters.sequencedatabasefilename.c_str());
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
			start = a_ion;
			break;
		case cyclic:
			start = a_ion;
			break;
		case lasso:
			start = a_ion;
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
	
	searchedsequenceline->setText(parameters.searchedsequence.c_str());
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
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(false);
		cycliccterminus->setDisabled(false);
		enablescrambling->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(true);
		break;
	case cyclic:
		modificationsline->setDisabled(true);
		modificationsbutton->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(true);
		break;
	case branched:
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(false);
		break;
	case lasso:
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(false);
		break;
	case linearpolysaccharide:
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(true);
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
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(false);
		maximumbricksincombinationmiddle->setDisabled(false);
		maximumbricksincombinationend->setDisabled(false);
		maximumcumulativemass->setDisabled(false);
		generatebrickspermutations->setDisabled(false);
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		sequencedatabaseline->setDisabled(true);
		sequencedatabasebutton->setDisabled(true);
		blindedges->setDisabled(false);
		similaritysearch->setDisabled(true);
		maximumnumberofthreads->setDisabled(false);
		scoretype->setDisabled(false);
		hitsreported->setDisabled(false);
		sequencetag->setDisabled(false);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(false);
		searchedsequenceline->setDisabled(false);
		searchedsequencebutton->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case singlecomparison:
		peptidetype->setDisabled(false);
		precursormass->setDisabled(false);
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
		generatebrickspermutations->setDisabled(true);
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		sequencedatabaseline->setDisabled(true);
		sequencedatabasebutton->setDisabled(true);
		blindedges->setDisabled(true);
		similaritysearch->setDisabled(true);
		maximumnumberofthreads->setDisabled(true);
		scoretype->setDisabled(true);
		hitsreported->setDisabled(true);
		sequencetag->setDisabled(true);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(false);
		searchedsequenceline->setDisabled(false);
		searchedsequencebutton->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case databasesearch:
		peptidetype->setDisabled(false);
		precursormass->setDisabled(false);
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
		generatebrickspermutations->setDisabled(true);
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		blindedges->setDisabled(true);
		similaritysearch->setDisabled(false);
		sequencedatabaseline->setDisabled(false);
		sequencedatabasebutton->setDisabled(false);
		maximumnumberofthreads->setDisabled(false);
		scoretype->setDisabled(false);
		hitsreported->setDisabled(false);
		sequencetag->setDisabled(false);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(false);
		searchedsequenceline->setDisabled(false);
		searchedsequencebutton->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case dereplication:
		peptidetype->setDisabled(true);
		precursormass->setDisabled(true);
		precursoradduct->setDisabled(true);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(true);
		brickdatabaseline->setDisabled(true);
		brickdatabasebutton->setDisabled(true);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
		generatebrickspermutations->setDisabled(true);
		modificationsline->setDisabled(true);
		modificationsbutton->setDisabled(true);
		blindedges->setDisabled(true);
		similaritysearch->setDisabled(true);
		sequencedatabaseline->setDisabled(false);
		sequencedatabasebutton->setDisabled(false);
		maximumnumberofthreads->setDisabled(true);
		scoretype->setDisabled(true);
		hitsreported->setDisabled(true);
		sequencetag->setDisabled(true);
		fragmentiontypes->setDisabled(false);
		clearhitswithoutparent->setDisabled(true);
		searchedsequenceline->setDisabled(true);
		searchedsequencebutton->setDisabled(true);

		resetFragmentIonTypes();
		
		modificationsline->setDisabled(true);
		modificationsbutton->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(true);
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
			start = a_ion;
			end = z_ion_dehydrated_and_deamidated;
			break;
		case cyclic:
			start = a_ion;
			end = c_ion_dehydrated_and_deamidated;
			break;
		case lasso:
			start = a_ion;
			end = z_ion_dehydrated_and_deamidated;
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

	for (int i = (int)start; i <= (int)end; i++) {

		fragmentiontypes->getList()->addItem(tr(parameters.fragmentdefinitions[(fragmentIonType)i].name.c_str()));

		if ((modeType)mode->currentIndex() == dereplication) {
			if ((fragmentIonType)i == ms_hplus) {
				fragmentiontypes->getList()->item(i-start)->setSelected(true);
			}
		}
		else {
			switch ((peptideType)peptidetype->currentIndex()) {
			case linear:
			case branched:
				if (((fragmentIonType)i == b_ion) || ((fragmentIonType)i == y_ion)) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case cyclic:
				if ((fragmentIonType)i == b_ion) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case lasso:
				if (((fragmentIonType)i == b_ion) || ((fragmentIonType)i == y_ion)) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case linearpolysaccharide:
				if (((fragmentIonType)i == ms_nterminal_ion_hplus) || ((fragmentIonType)i == ms_cterminal_ion_hplus)) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case other:
			default:
				break;
			}
		}

	}

}


void cParametersWidget::drawPeptideButtonReleased() {
	emit sendSequenceLine(peptidetype->currentIndex(), searchedsequenceline->text());
}

