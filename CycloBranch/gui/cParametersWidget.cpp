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
	load->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));

	save = new QPushButton(QString("Save"));
	save->setToolTip("Save settings in the current file (*.ini). When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");
	
	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save settings into a file (*.ini).");
	saveas->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

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
	peptidetype->addItem(tr("Linear polyketide"));
	peptidetype->addItem(tr("Cyclic polyketide"));
	//peptidetype->addItem(tr("Other"));
	peaklistformlayout->addRow(tr("Peptide Type: "), peptidetype);

	peaklistline = new QLineEdit();
	peaklistbutton = new QPushButton("Select");
	#if OS_TYPE != WIN
		peaklistline->setToolTip("Select a file. Supported formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists).");
		peaklistbutton->setToolTip("Select a file. Supported formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists).");
	#else
		peaklistline->setToolTip("Select a file. Supported formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (peaklists),\nmzXML (peaklists),\nbaf (profile spectra),\nimzML (profile spectra or peaklists),\nmis (deprecated).");
		peaklistbutton->setToolTip("Select a file. Supported formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (peaklists),\nmzXML (peaklists),\nbaf (profile spectra),\nimzML (profile spectra or peaklists),\nmis (deprecated).");
	#endif
	peaklistbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	peaklistlayout = new QHBoxLayout();
	peaklistlayout->addWidget(peaklistline);
	peaklistlayout->addWidget(peaklistbutton);
	peaklistformlayout->addRow(tr("File: "), peaklistlayout);

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
	precursorcharge->setToolTip("Enter the precursor charge (a precursor charge in the peaklist file is ignored). Negative values are allowed.\nWhen mode is set up to \"Compare Peaklist(s) with Database - MS or MSI data\", the value determines the maximum charge of generated peaks.");
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
	fragmentmasserrortolerance->setToolTip("Enter the m/z error tolerance in MS mode or the fragment m/z error tolerance in MS/MS mode [ppm].");
	fragmentmasserrortolerance->setDecimals(3);
	fragmentmasserrortolerance->setRange(0, 10000);
	fragmentmasserrortolerance->setSingleStep(1);
	fragmentmasserrortolerance->setSuffix(" ppm");
	peaklistformlayout->addRow(tr("m/z Error Tolerance: "), fragmentmasserrortolerance);

	masserrortolerancefordeisotoping = new QDoubleSpinBox();
	masserrortolerancefordeisotoping->setToolTip("Enter the m/z error tolerance for deisotoping in MS mode or the fragment m/z error tolerance for deisotoping in MS/MS mode [ppm]\n(the same value like \"m/z Error Tolerance\" is recommended by default; 0 = the deisotoping is disabled).");
	masserrortolerancefordeisotoping->setDecimals(3);
	masserrortolerancefordeisotoping->setRange(0, 10000);
	masserrortolerancefordeisotoping->setSingleStep(1);
	masserrortolerancefordeisotoping->setSuffix(" ppm");
	peaklistformlayout->addRow(tr("m/z Error Tolerance for Deisotoping: "), masserrortolerancefordeisotoping);

	minimumrelativeintensitythreshold = new QDoubleSpinBox();
	minimumrelativeintensitythreshold->setToolTip("Enter the threshold of relative intensity in %. Peaks having relative intensities below the threshold will be removed from the peaklist.");
	minimumrelativeintensitythreshold->setDecimals(3);
	minimumrelativeintensitythreshold->setRange(0, 100);
	minimumrelativeintensitythreshold->setSingleStep(1);
	minimumrelativeintensitythreshold->setSuffix(" %");
	peaklistformlayout->addRow(tr("Minimum Threshold of Relative Intensity: "), minimumrelativeintensitythreshold);

	minimummz = new QDoubleSpinBox();
	minimummz->setToolTip("Enter the minimum m/z ratio. Peaks having m/z ratios below the threshold will be removed from the peaklist.");
	minimummz->setDecimals(3);
	minimummz->setRange(0, 10000);
	minimummz->setSingleStep(1);
	peaklistformlayout->addRow(tr("Minimum m/z Ratio: "), minimummz);

	fwhm = new QDoubleSpinBox();
	fwhm->setToolTip("Full width at half maximum. The value is used if the profile spectra are converted into peaklists (imzML).");
	fwhm->setDecimals(6);
	fwhm->setRange(0, 1);
	fwhm->setSingleStep(0.01);
	peaklistformlayout->addRow(tr("FWHM: "), fwhm);

	peaklistgroupbox = new QGroupBox("Spectrum");
	peaklistgroupbox->setLayout(peaklistformlayout);


	brickdatabaseformlayout = new QFormLayout();

	brickdatabaseline = new QLineEdit();
	brickdatabaseline->setToolTip("Select the txt file containing a list of building blocks.");
	brickdatabasebutton = new QPushButton("Select");
	brickdatabasebutton->setToolTip("Select the txt file containing a list of building blocks.");
	brickdatabasebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
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
	modificationsbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	modificationslayout = new QHBoxLayout();
	modificationslayout->addWidget(modificationsline);
	modificationslayout->addWidget(modificationsbutton);
	brickdatabaseformlayout->addRow(tr("N-/C-terminal Modifications File: "), modificationslayout);

	brickdatabasegroupbox = new QGroupBox("Database of Building Blocks");
	brickdatabasegroupbox->setLayout(brickdatabaseformlayout);


	miscformlayout = new QFormLayout();

	blindedges = new QComboBox();
	blindedges->setToolTip("The operation to be performed with edges forming incomplete paths:\nkeep (edges are kept - useful when you would like to see the whole de novo graph in 'View -> Graph');\nremove (edges are removed - speeds up the search);\nconnect (edges are connected - useful when you are looking for sequence tags).");
	blindedges->addItem(tr("keep (you can see a complete de novo graph)"));
	blindedges->addItem(tr("remove (speed up the search)"));
	blindedges->addItem(tr("connect (allow detection of sequence tags)"));
	miscformlayout->addRow(tr("Incomplete Paths in De Novo Graph: "), blindedges);

	cyclicnterminus = new QCheckBox();
	cyclicnterminus->setToolTip("The water molecule is subtracted from all theoretical N-terminal fragment ions and the theoretical precursor mass.\nThis feature is useful when a linear peptide includes a small cycle close to the N-terminus.\nIf the linear polyketide is selected as the peptide type, the water molecule is subtracted only from the precursor ion.");
	miscformlayout->addRow(tr("Cyclic N-terminus: "), cyclicnterminus);

	cycliccterminus = new QCheckBox();
	cycliccterminus->setToolTip("The water molecule is subtracted from all theoretical C-terminal fragment ions and the theoretical precursor mass.\nThis feature is useful when a linear peptide includes a small cycle close to the C-terminus.\nIf the linear polyketide is selected as the peptide type, the water molecule is subtracted only from the precursor ion.");
	miscformlayout->addRow(tr("Cyclic C-terminus: "), cycliccterminus);

	enablescrambling = new QCheckBox();
	enablescrambling->setToolTip("Generate scrambled fragment ions of cyclic peptides in theoretical spectra.");
	miscformlayout->addRow(tr("Enable Scrambling: "), enablescrambling);

	similaritysearch = new QCheckBox();
	similaritysearch->setToolTip("Enable the similarity search when a peaklist is searched against a database of sequences.\nThis feature disables filtering of peptide sequence candidates by precursor mass and can be useful to determine a peptide family when a similar peptide is included in a database of sequences.");
	miscformlayout->addRow(tr("Enable Similarity Search: "), similaritysearch);

	regularblocksorder = new QCheckBox();
	regularblocksorder->setToolTip("Keep only polyketide sequence candidates whose ketide building blocks are in the regular order [water eliminating block]-[2H eliminating block]-[water eliminating block]-[2H eliminating block], etc.");
	miscformlayout->addRow(tr("Regular Order of Ketide Blocks: "), regularblocksorder);

	miscgroupbox = new QGroupBox("Miscellaneous");
	miscgroupbox->setLayout(miscformlayout);


	applicationformlayout = new QFormLayout();
	
	mode = new QComboBox();
	mode->setToolTip("'De Novo Search Engine' - the default mode of the application.\n'Compare Peaklist with Spectrum of Searched Sequence' - a theoretical spectrum is generated for the input 'Searched Peptide Sequence' and is compared with the peaklist.\n'Compare Peaklist with Database - MS/MS data' - a peaklist is compared with theoretical spectra generated from a database of sequences.\n'Compare Peaklist(s) with Database - MS or MSI data' - dereplication; compounds search; the peaklists are compared with theoretical peaks generated from a database of compounds/sequences.");
	mode->addItem(tr("De Novo Search Engine"));
	mode->addItem(tr("Compare Peaklist with Spectrum of Searched Sequence"));
	mode->addItem(tr("Compare Peaklist with Database - MS/MS data"));
	mode->addItem(tr("Compare Peaklist(s) with Database - MS or MSI data"));
	applicationformlayout->addRow(tr("Mode: "), mode);
	oldmodetype = (eModeType)mode->currentIndex();

	sequencedatabaseline = new QLineEdit();
	sequencedatabaseline->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabasebutton = new QPushButton("Select");
	sequencedatabasebutton->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabasebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
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
	fragmentiontypes->setToolTip("Select ion types which will be generated in theoretical spectra.");
	applicationformlayout->addRow(tr("Ion Types in Theoretical Spectra: "), fragmentiontypes);

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
	searchedsequencebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
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
	hlayoutwidget->setMinimumWidth(1250);
	hlayoutwidget->setLayout(hlayout);

	hlayoutscroll = new QScrollArea();
	hlayoutscroll->setWidgetResizable(false);
	hlayoutscroll->setFrameShape(QFrame::NoFrame);
	hlayoutscroll->setWidget(hlayoutwidget);


	vlayout = new QVBoxLayout();
	vlayout->addWidget(hlayoutscroll);	
	vlayout->addLayout(buttons);

	setLayout(vlayout);

	resize(1280, 750);

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
	delete fwhm;
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
	delete regularblocksorder;
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
	this->parameters.peaklistseries.clear();
	this->parameters.fragmentionsfordenovograph.clear();
	this->parameters.sequencetag = this->parameters.originalsequencetag;
	this->parameters.searchedsequence = this->parameters.originalsearchedsequence;

	restoreParameters();
}


void cParametersWidget::closeEvent(QCloseEvent *event) {
	restoreParameters();
	event->accept();
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

	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		updateParametersAndHide();
    }

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_S)) {
		saveSettings();
	}

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/settings.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/settings.html").absoluteFilePath()));
		#endif
	}

	event->accept();
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
		fwhm->setValue(settings.value("fwhm", 0.1).toDouble());

		brickdatabaseline->setText(settings.value("brickdatabase", "").toString());
		maximumbricksincombinationbegin->setValue(settings.value("maximumbricksincombinationbegin", 1).toInt());
		maximumbricksincombinationmiddle->setValue(settings.value("maximumbricksincombinationmiddle", 1).toInt());
		maximumbricksincombinationend->setValue(settings.value("maximumbricksincombinationend", 1).toInt());
		maximumcumulativemass->setValue(settings.value("maximumcumulativemass", 0).toDouble());
		settings.value("generatebrickspermutations", 1).toInt() == 0 ? generatebrickspermutations->setChecked(false) : generatebrickspermutations->setChecked(true);
		modificationsline->setText(settings.value("modificationsfile", "").toString());

		blindedges->setCurrentIndex(settings.value("blindedges", 2).toInt());
		settings.value("cyclicnterminus", 0).toInt() == 0 ? cyclicnterminus->setChecked(false) : cyclicnterminus->setChecked(true);
		settings.value("cycliccterminus", 0).toInt() == 0 ? cycliccterminus->setChecked(false) : cycliccterminus->setChecked(true);
		settings.value("enablescrambling", 0).toInt() == 0 ? enablescrambling->setChecked(false) : enablescrambling->setChecked(true);
		settings.value("similaritysearch", 0).toInt() == 0 ? similaritysearch->setChecked(false) : similaritysearch->setChecked(true);
		settings.value("regularblocksorder", 0).toInt() == 0 ? regularblocksorder->setChecked(false) : regularblocksorder->setChecked(true);

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
	settings.setValue("fwhm", fwhm->value());

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
	regularblocksorder->isChecked() ? settings.setValue("regularblocksorder", 1) : settings.setValue("regularblocksorder", 0);

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
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Peaklist..."), lastdirselectpeaklist, tr("Peak Lists (*.txt *.mgf *.mzML *.mzXML *.imzML)"));
	#else
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Peaklist..."), lastdirselectpeaklist, tr("Peak Lists (*.txt *.mgf *.mzML *.mzXML *.baf *.imzML *.mis)"));
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

	if ((brickdatabaseline->text().toStdString().compare("") == 0) && (((eModeType)mode->currentIndex() == denovoengine) || ((eModeType)mode->currentIndex() == singlecomparison) || ((eModeType)mode->currentIndex() == databasesearch))) {
		errstr = "A database of building blocks must be specified!";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((sequencedatabaseline->text().toStdString().compare("") == 0) && (((eModeType)mode->currentIndex() == databasesearch) || ((eModeType)mode->currentIndex() == dereplication))) {
		errstr = "A sequence database must be specified!";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((precursormass->value() == 0) && (((eModeType)mode->currentIndex() == denovoengine) || ((eModeType)mode->currentIndex() == singlecomparison) || ((eModeType)mode->currentIndex() == databasesearch))) {
		errstr = "The precursor m/z ratio cannot be zero!";
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
	if ((maximumbricksincombinationmiddle->value() < 2) && ((eModeType)mode->currentIndex() == denovoengine) && (((ePeptideType)peptidetype->currentIndex() == branched) || ((ePeptideType)peptidetype->currentIndex() == branchcyclic))) {
		errstr = "'Maximum Number of Combined Blocks (middle)' must be at least 2 when a branched or a branch-cyclic peptide is searched! (One block represents a branched residue, the other block(s) corresponds to a branch.)";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}
	*/

	parameters.peptidetype = (ePeptideType)peptidetype->currentIndex();
	parameters.peaklistfilename = peaklistline->text().toStdString();
	parameters.precursormass = precursormass->value();
	parameters.precursoradduct = precursoradduct->text().toStdString();
	parameters.precursormasserrortolerance = precursormasserrortolerance->value();
	parameters.precursorcharge = precursorcharge->value();
	parameters.fragmentmasserrortolerance = fragmentmasserrortolerance->value();
	parameters.masserrortolerancefordeisotoping = masserrortolerancefordeisotoping->value();
	parameters.minimumrelativeintensitythreshold = minimumrelativeintensitythreshold->value();
	parameters.minimummz = minimummz->value();
	parameters.fwhm = fwhm->value();

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
	parameters.regularblocksorder = regularblocksorder->isChecked();

	parameters.mode = (eModeType)mode->currentIndex();
	parameters.sequencedatabasefilename = sequencedatabaseline->text().toStdString();
	parameters.maximumnumberofthreads = maximumnumberofthreads->value();
	parameters.scoretype = (eScoreType)scoretype->currentIndex();
	parameters.hitsreported = hitsreported->value();
	parameters.sequencetag = sequencetag->text().toStdString();
	parameters.originalsequencetag = parameters.sequencetag;

	parameters.fragmentionsfortheoreticalspectra.clear();
	int start;

	if ((eModeType)mode->currentIndex() == dereplication) {
		start = ms_hplus;
	}
	else {
		switch ((ePeptideType)peptidetype->currentIndex())
		{
		case linear:
		case branched:
		case cyclic:
		case branchcyclic:
			start = a_ion;
			break;
		case linearpolyketide:
			start = l1h_ion;
			break;
		case cyclicpolyketide:
			start = l1h_ion; // l0h_ion;
			break;
		case other:
		default:
			break;
		}
	}

	for (int i = 0; i < fragmentiontypes->getList()->count(); i++) {
		if (fragmentiontypes->getList()->item(i)->isSelected()) {
			parameters.fragmentionsfortheoreticalspectra.push_back((eFragmentIonType)(i + start));
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
	fwhm->setValue(parameters.fwhm);

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
	regularblocksorder->setChecked(parameters.regularblocksorder);

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
		case cyclic:
		case branchcyclic:
			start = a_ion;
			break;
		case linearpolyketide:
			start = l1h_ion;
			break;
		case cyclicpolyketide:
			start = l1h_ion; // l0h_ion;
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

	if (!(((eModeType)mode->currentIndex() != oldmodetype) && ((eModeType)mode->currentIndex() != dereplication) && (oldmodetype != dereplication))) {
		resetFragmentIonTypes();
	}

	switch ((ePeptideType)index)
	{
	case linear:
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(false);
		cycliccterminus->setDisabled(false);
		enablescrambling->setDisabled(true);
		regularblocksorder->setDisabled(true);
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
		regularblocksorder->setDisabled(true);
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
		regularblocksorder->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(false);
		break;
	case branchcyclic:
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		regularblocksorder->setDisabled(true);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(false);
		break;
	case linearpolyketide:
		modificationsline->setDisabled(false);
		modificationsbutton->setDisabled(false);
		cyclicnterminus->setDisabled(false);
		cycliccterminus->setDisabled(false);
		enablescrambling->setDisabled(true);
		regularblocksorder->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(false);
		searchedsequenceCtermmodif->setDisabled(false);
		searchedsequenceTmodif->setDisabled(true);
		break;
	case cyclicpolyketide:
		modificationsline->setDisabled(true);
		modificationsbutton->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		regularblocksorder->setDisabled(false);
		searchedsequenceNtermmodif->setDisabled(true);
		searchedsequenceCtermmodif->setDisabled(true);
		searchedsequenceTmodif->setDisabled(true);
		break;
	case other:
		break;
	default:
		break;
	}
}


void cParametersWidget::updateSettingsWhenModeChanged(int index) {

	switch ((eModeType)index)
	{
	case denovoengine:
		peptidetype->setDisabled(false);
		precursormass->setDisabled(false);
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		fwhm->setDisabled(true);
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
		fwhm->setDisabled(true);
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
		fwhm->setDisabled(true);
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
		fwhm->setDisabled(false);
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

	oldmodetype = (eModeType)mode->currentIndex();
}


void cParametersWidget::resetFragmentIonTypes() {
	fragmentiontypes->getList()->clear();
	eFragmentIonType start, end;
	
	if ((eModeType)mode->currentIndex() == dereplication) {
		start = ms_hplus;
		end = ms_MGa4H;
	}
	else {
		switch ((ePeptideType)peptidetype->currentIndex()) {
		case linear:
		case branched:
			start = a_ion;
			end = z_ion_dehydrated_and_deamidated;
			break;
		case cyclic:
			start = a_ion;
			end = c_ion_dehydrated_and_deamidated;
			break;
		case branchcyclic:
			start = a_ion;
			end = z_ion_dehydrated_and_deamidated;
			break;
		case linearpolyketide:
			start = l1h_ion;
			end = r2oh_ion_co_loss_dehydrated_and_deamidated;
			break;
		case cyclicpolyketide:
			start = l1h_ion; // l0h_ion;
			end = l2h_ion_co_loss_dehydrated_and_deamidated;
			break;
		case other:
			break;
		default:
			break;
		}
	}

	for (int i = (int)start; i <= (int)end; i++) {

		fragmentiontypes->getList()->addItem(tr(parameters.fragmentdefinitions[(eFragmentIonType)i].name.c_str()));

		if ((eModeType)mode->currentIndex() == dereplication) {
			if ((eFragmentIonType)i == ms_hplus) {
				fragmentiontypes->getList()->item(i-start)->setSelected(true);
			}
		}
		else {
			switch ((ePeptideType)peptidetype->currentIndex()) {
			case linear:
			case branched:
				if (((eFragmentIonType)i == b_ion) || ((eFragmentIonType)i == y_ion)) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case cyclic:
				if ((eFragmentIonType)i == b_ion) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case branchcyclic:
				if (((eFragmentIonType)i == b_ion) || ((eFragmentIonType)i == y_ion)) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case linearpolyketide:
				if (((eFragmentIonType)i == l1h_ion) || ((eFragmentIonType)i == l2h_ion) || ((eFragmentIonType)i == r1h_ion) || ((eFragmentIonType)i == r2h_ion) ||
					((eFragmentIonType)i == l1oh_ion) || ((eFragmentIonType)i == l2oh_ion) || ((eFragmentIonType)i == r1oh_ion) || ((eFragmentIonType)i == r2oh_ion)
					) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case cyclicpolyketide:
				if (/*((eFragmentIonType)i == l0h_ion) ||*/ ((eFragmentIonType)i == l1h_ion) || ((eFragmentIonType)i == l2h_ion)) {
					fragmentiontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case other:
				break;
			default:
				break;
			}
		}

	}

}


void cParametersWidget::drawPeptideButtonReleased() {
	emit sendSequenceLine(peptidetype->currentIndex(), searchedsequenceline->text());
}

