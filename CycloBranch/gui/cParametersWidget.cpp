#include "gui/cParametersWidget.h"

#include "core/cSummaryFormula.h"

#include <QGridLayout>
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
#include <QLabel>


cParametersWidget::cParametersWidget(QWidget* parent) {
	this->parent = parent;

	int leftdefaultwidth = 370;
	int rightdefaultwidth = 370;
	int searchedsequencedefaultwidth = 510;

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


	searchgridlayout = new QGridLayout();

	mode = new QComboBox();
	mode->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	mode->setToolTip("'De Novo Search Engine' - the default mode of the application.\n'Compare Peaklist(s) with Spectrum of Searched Sequence' - a theoretical spectrum is generated for the input 'Searched Sequence' and is compared with the peaklist(s).\n'Compare Peaklist with Database - MS/MS data' - a peaklist is compared with theoretical spectra generated from a database of sequences.\n'Compare Peaklist(s) with Database - MS or MSI data' - compound search; dereplication; the peaklists are compared with theoretical peaks generated from a database of compounds/sequences.");
	mode->addItem(tr("De Novo Search Engine"));
	mode->addItem(tr("Compare Peaklist(s) with Spectrum of Searched Sequence"));
	mode->addItem(tr("Compare Peaklist with Database - MS/MS data"));
	mode->addItem(tr("Compare Peaklist(s) with Database - MS or MSI data"));
	mode->setFixedWidth(leftdefaultwidth);
	modelabel = new QLabel("Mode:");
	searchgridlayout->addWidget(modelabel, 0, 0);
	searchgridlayout->addWidget(mode, 0, 1);
	oldmodetype = (eModeType)mode->currentIndex();

	maximumnumberofthreads = new QSpinBox();
	maximumnumberofthreads->setToolTip("A maximum number of threads used if an experimental peaklist is compared with theoretical spectra of peptide sequence candidates.");
	maximumnumberofthreads->setRange(1, 1024);
	maximumnumberofthreads->setSingleStep(1);
	maximumnumberofthreads->setFixedWidth(leftdefaultwidth);
	maximumnumberofthreadslabel = new QLabel("Maximum Number of Threads:");
	searchgridlayout->addWidget(maximumnumberofthreadslabel, 1, 0);
	searchgridlayout->addWidget(maximumnumberofthreads, 1, 1);

	searchgroupbox = new QGroupBox("Search");
	searchgroupbox->setLayout(searchgridlayout);


	experimentalspectragridlayout = new QGridLayout();

	peptidetype = new QComboBox();
	peptidetype->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	peptidetype->setToolTip("Select the type of peptide or polyketide.");
	peptidetype->addItem(tr("Linear"));
	peptidetype->addItem(tr("Cyclic"));
	peptidetype->addItem(tr("Branched"));
	peptidetype->addItem(tr("Branch-cyclic"));
	peptidetype->addItem(tr("Linear polyketide"));
	peptidetype->addItem(tr("Cyclic polyketide"));
	//peptidetype->addItem(tr("Other"));
	peptidetype->setFixedWidth(leftdefaultwidth);
	peptidetypelabel = new QLabel("Peptide Type:");
	experimentalspectragridlayout->addWidget(peptidetypelabel, 0, 0);
	experimentalspectragridlayout->addWidget(peptidetype, 0, 1); 

	peaklistline = new QLineEdit();
	useprofiledata = new QCheckBox();
	useprofiledata->setToolTip("Use profile data (if applicable).");
	peaklistbutton = new QPushButton("Select");
	#if OS_TYPE != WIN
		peaklistline->setToolTip("Select a file. Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists).");
		peaklistbutton->setToolTip("Select a file. Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists).");
	#else
		peaklistline->setToolTip("Select a file. Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists),\nbaf (profile spectra),\nraw (profile spectra or peaklists),\ndat (peaklists),\nmis (deprecated),\nser (deprecated).");
		peaklistbutton->setToolTip("Select a file. Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists),\nbaf (profile spectra),\nraw (profile spectra or peaklists),\ndat (peaklists),\nmis (deprecated),\nser (deprecated).");
	#endif
	peaklistbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
	peaklistlayout = new QHBoxLayout();
	peaklistlayout->addWidget(peaklistline);
	peaklistlayout->addWidget(useprofiledata);
	peaklistlayout->addWidget(peaklistbutton);
	peaklistlayout->setMargin(0);
	peaklistwidget = new QWidget();
	peaklistwidget->setLayout(peaklistlayout);
	peaklistwidget->setFixedWidth(leftdefaultwidth);
	peaklistlabel = new QLabel("File:");
	experimentalspectragridlayout->addWidget(peaklistlabel, 1, 0);
	experimentalspectragridlayout->addWidget(peaklistwidget, 1, 1);

	scannumber = new QSpinBox();
	scannumber->setToolTip("Number of a spectrum to be processed in a LC-MS/MS data file.\nThe number may be different from an internal scan number stored in the input data file.");
	scannumber->setRange(1, 100000000);
	scannumber->setSingleStep(1);
	scannumber->setFixedWidth(leftdefaultwidth);
	scannumberlabel = new QLabel("Scan no.:");
	experimentalspectragridlayout->addWidget(scannumberlabel, 2, 0);
	experimentalspectragridlayout->addWidget(scannumber, 2, 1);

	precursormass = new QDoubleSpinBox();
	precursormass->setToolTip("Enter the precursor mass-to-charge (m/z) ratio.\nThe value is decharged automatically.\nThe precursor m/z ratio in the input peaklist file is ignored.");
	precursormass->setDecimals(6);
	precursormass->setRange(0, 100000);
	precursormass->setSingleStep(1);
	precursormass->setFixedWidth(leftdefaultwidth);
	precursormasslabel = new QLabel("Precursor m/z Ratio:");
	experimentalspectragridlayout->addWidget(precursormasslabel, 3, 0);
	experimentalspectragridlayout->addWidget(precursormass, 3, 1);

	precursoradduct = new QLineEdit();
	precursoradduct->setToolTip("Enter the formula of a precursor ion adduct (e.g., Na, K, Li).\nH is used by default if the field is empty.\nSee the manual for more options.");
	precursoradduct->setFixedWidth(leftdefaultwidth);
	precursoradductlabel = new QLabel("Precursor Ion Adduct:");
	experimentalspectragridlayout->addWidget(precursoradductlabel, 4, 0);
	experimentalspectragridlayout->addWidget(precursoradduct, 4, 1);

	precursorcharge = new QSpinBox();
	precursorcharge->setToolTip("Enter the charge of precursor ion. Negative values are allowed. The charge of precursor ion in the input peaklist file is ignored.\nIf the mode \"Compare Peaklist(s) with Database - MS or MSI data\" is used, the value determines the maximum charge of generated theoretical peaks.\nFor example, 3 means that theoretical peaks of compouds are generated with charges 1+, 2+, and 3+. The value -1 means that theoretical peaks of compounds are generated with charge 1-.");
	precursorcharge->setRange(-100, 100);
	precursorcharge->setSingleStep(1);
	precursorcharge->setFixedWidth(leftdefaultwidth);
	precursorchargelabel = new QLabel("Charge:");
	experimentalspectragridlayout->addWidget(precursorchargelabel, 5, 0);
	experimentalspectragridlayout->addWidget(precursorcharge, 5, 1);

	precursormasserrortolerance = new QDoubleSpinBox();
	precursormasserrortolerance->setToolTip("Enter the precursor m/z error tolerance in ppm.");
	precursormasserrortolerance->setDecimals(3);
	precursormasserrortolerance->setRange(0, 10000);
	precursormasserrortolerance->setSingleStep(1);
	precursormasserrortolerance->setSuffix(" ppm");
	precursormasserrortolerance->setFixedWidth(leftdefaultwidth);
	precursormasserrortolerancelabel = new QLabel("Precursor m/z Error Tolerance:");
	experimentalspectragridlayout->addWidget(precursormasserrortolerancelabel, 6, 0);
	experimentalspectragridlayout->addWidget(precursormasserrortolerance, 6, 1);

	fragmentmasserrortolerance = new QDoubleSpinBox();
	fragmentmasserrortolerance->setToolTip("Enter the m/z error tolerance in MS mode or the fragment m/z error tolerance in MS/MS mode [ppm].");
	fragmentmasserrortolerance->setDecimals(3);
	fragmentmasserrortolerance->setRange(0, 10000);
	fragmentmasserrortolerance->setSingleStep(1);
	fragmentmasserrortolerance->setSuffix(" ppm");
	fragmentmasserrortolerance->setFixedWidth(leftdefaultwidth);
	fragmentmasserrortolerancelabel = new QLabel("m/z Error Tolerance:");
	experimentalspectragridlayout->addWidget(fragmentmasserrortolerancelabel, 7, 0);
	experimentalspectragridlayout->addWidget(fragmentmasserrortolerance, 7, 1);

	minimumrelativeintensitythreshold = new QDoubleSpinBox();
	minimumrelativeintensitythreshold->setToolTip("Enter the minimum threshold of relative intensity in %. Peaks with relative intensities below the threshold are removed from the peaklist.");
	minimumrelativeintensitythreshold->setDecimals(3);
	minimumrelativeintensitythreshold->setRange(0, 100);
	minimumrelativeintensitythreshold->setSingleStep(1);
	minimumrelativeintensitythreshold->setSuffix(" %");
	minimumrelativeintensitythreshold->setFixedWidth(leftdefaultwidth);
	minimumrelativeintensitythresholdlabel = new QLabel("Minimum Threshold of Relative Intensity:");
	experimentalspectragridlayout->addWidget(minimumrelativeintensitythresholdlabel, 8, 0);
	experimentalspectragridlayout->addWidget(minimumrelativeintensitythreshold, 8, 1);

	minimumabsoluteintensitythreshold = new QSpinBox();
	minimumabsoluteintensitythreshold->setToolTip("Enter the minimum threshold of absolute intensity. Peaks with absolute intensities below the threshold are removed from the peaklist.");
	minimumabsoluteintensitythreshold->setRange(0, INT32_MAX);
	minimumabsoluteintensitythreshold->setSingleStep(100);
	minimumabsoluteintensitythreshold->setFixedWidth(leftdefaultwidth);
	minimumabsoluteintensitythresholdlabel = new QLabel("Minimum Threshold of Absolute Intensity:");
	experimentalspectragridlayout->addWidget(minimumabsoluteintensitythresholdlabel, 9, 0);
	experimentalspectragridlayout->addWidget(minimumabsoluteintensitythreshold, 9, 1);

	minimummz = new QDoubleSpinBox();
	minimummz->setToolTip("Enter the minimum m/z ratio. Peaks with m/z ratios below the threshold are removed from the peaklist.");
	minimummz->setDecimals(3);
	minimummz->setRange(0, 10000);
	minimummz->setSingleStep(1);
	minimummz->setFixedWidth(leftdefaultwidth);
	minimummzlabel = new QLabel("Minimum m/z Ratio:");
	experimentalspectragridlayout->addWidget(minimummzlabel, 10, 0);
	experimentalspectragridlayout->addWidget(minimummz, 10, 1);

	fwhm = new QDoubleSpinBox();
	fwhm->setToolTip("Full width at half maximum. The value is used if the profile spectra are converted into peaklists (mzML and imzML) and if the full isotope patterns of compounds are generated.");
	fwhm->setDecimals(6);
	fwhm->setRange(0, 1);
	fwhm->setSingleStep(0.01);
	fwhm->setSuffix(" Da");
	fwhm->setFixedWidth(leftdefaultwidth);
	fwhmlabel = new QLabel("FWHM:");
	experimentalspectragridlayout->addWidget(fwhmlabel, 11, 0);
	experimentalspectragridlayout->addWidget(fwhm, 11, 1);

	experimentalspectragroupbox = new QGroupBox("Experimental Spectrum/Spectra");
	experimentalspectragroupbox->setLayout(experimentalspectragridlayout);


	brickdatabasegridlayout = new QGridLayout();

	brickdatabaseline = new QLineEdit();
	brickdatabaseline->setToolTip("Select the txt file containing a list of building blocks.");
	brickdatabasebutton = new QPushButton("Select");
	brickdatabasebutton->setToolTip("Select the txt file containing a list of building blocks.");
	brickdatabasebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
	brickdatabaselayout = new QHBoxLayout();
	brickdatabaselayout->addWidget(brickdatabaseline);
	brickdatabaselayout->addWidget(brickdatabasebutton);
	brickdatabaselayout->setMargin(0);
	brickdatabasewidget = new QWidget();
	brickdatabasewidget->setLayout(brickdatabaselayout);
	brickdatabasewidget->setFixedWidth(leftdefaultwidth);
	brickdatabaselabel = new QLabel("Building Blocks Database File:");
	brickdatabasegridlayout->addWidget(brickdatabaselabel, 0, 0);
	brickdatabasegridlayout->addWidget(brickdatabasewidget, 0, 1);

	maximumbricksincombinationbegin = new QSpinBox();
	maximumbricksincombinationbegin->setToolTip("Maximum number of combined building blocks to skip a gap leading from a start point in a de novo graph. A small value speeds up the search and vice versa.");
	maximumbricksincombinationbegin->setRange(1, 20);
	maximumbricksincombinationbegin->setSingleStep(1);
	maximumbricksincombinationbegin->setPrefix("start: ");

	maximumbricksincombinationmiddle = new QSpinBox();
	maximumbricksincombinationmiddle->setToolTip("Maximum number of combined building blocks to skip a gap in a de novo graph (except gaps leading from a start point or to an end point). A small value speeds up the search and vice versa.");
	maximumbricksincombinationmiddle->setRange(1, 20);
	maximumbricksincombinationmiddle->setSingleStep(1);
	maximumbricksincombinationmiddle->setPrefix("middle: ");

	maximumbricksincombinationend = new QSpinBox();
	maximumbricksincombinationend->setToolTip("Maximum number of combined building blocks to skip a gap leading to an end point in a de novo graph. A small value speeds up the search and vice versa.");
	maximumbricksincombinationend->setRange(1, 20);
	maximumbricksincombinationend->setSingleStep(1);
	maximumbricksincombinationend->setPrefix("end: ");

	maximumbricksincombinationlayout = new QHBoxLayout();
	maximumbricksincombinationlayout->addWidget(maximumbricksincombinationbegin);
	maximumbricksincombinationlayout->addWidget(maximumbricksincombinationmiddle);
	maximumbricksincombinationlayout->addWidget(maximumbricksincombinationend);
	maximumbricksincombinationlayout->setMargin(0);
	maximumbricksincombinationwidget = new QWidget();
	maximumbricksincombinationwidget->setLayout(maximumbricksincombinationlayout);
	maximumbricksincombinationwidget->setFixedWidth(leftdefaultwidth);
	maximumbricksincombinatiolabel = new QLabel("Maximum Number of Combined Blocks:");
	brickdatabasegridlayout->addWidget(maximumbricksincombinatiolabel, 1, 0);
	brickdatabasegridlayout->addWidget(maximumbricksincombinationwidget, 1, 1);

	maximumcumulativemass = new QDoubleSpinBox();
	maximumcumulativemass->setToolTip("Enter the maximum cumulative mass of combined blocks (0 = the maximum mass is unlimited). A small value speeds up the search and vice versa.");
	maximumcumulativemass->setDecimals(3);
	maximumcumulativemass->setRange(0, 10000);
	maximumcumulativemass->setSingleStep(1);
	maximumcumulativemass->setFixedWidth(leftdefaultwidth);
	maximumcumulativemasslabel = new QLabel("Maximum Cumulative Mass of Blocks:");
	brickdatabasegridlayout->addWidget(maximumcumulativemasslabel, 2, 0);
	brickdatabasegridlayout->addWidget(maximumcumulativemass, 2, 1);

	modificationsline = new QLineEdit();
	modificationsline->setToolTip("Select the txt file containing a list of modifications.");
	modificationsbutton = new QPushButton("Select");
	modificationsbutton->setToolTip("Select the txt file containing a list of modifications.");
	modificationsbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_M));
	modificationslayout = new QHBoxLayout();
	modificationslayout->addWidget(modificationsline);
	modificationslayout->addWidget(modificationsbutton);
	modificationslayout->setMargin(0);
	modificationswidget = new QWidget();
	modificationswidget->setLayout(modificationslayout);
	modificationswidget->setFixedWidth(leftdefaultwidth);
	modificationslabel = new QLabel("N-/C-terminal Modifications File:");
	brickdatabasegridlayout->addWidget(modificationslabel, 3, 0);
	brickdatabasegridlayout->addWidget(modificationswidget, 3, 1);

	brickdatabasegroupbox = new QGroupBox("Database of Building Blocks");
	brickdatabasegroupbox->setLayout(brickdatabasegridlayout);


	miscgridlayout = new QGridLayout();

	blindedges = new QComboBox();
	blindedges->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	blindedges->setToolTip("The operation to be performed with edges forming incomplete paths:\nkeep (edges are kept - useful if you would like to see the whole de novo graph in 'View -> Graph');\nremove (edges are removed - speeds up the search);\nconnect (edges are connected - useful if you are looking for sequence tags).");
	blindedges->addItem(tr("keep (you can see a complete de novo graph)"));
	blindedges->addItem(tr("remove (speed up the search)"));
	blindedges->addItem(tr("connect (allow detection of sequence tags)"));
	blindedges->setFixedWidth(leftdefaultwidth);
	blindedgeslabel = new QLabel("Incomplete Paths in De Novo Graph:");
	miscgridlayout->addWidget(blindedgeslabel, 0, 0);
	miscgridlayout->addWidget(blindedges, 0, 1);

	cyclicnterminus = new QCheckBox();
	cyclicnterminus->setToolTip("The water molecule is subtracted from all theoretical N-terminal fragment ions and the theoretical precursor mass.\nThis feature is useful if a linear peptide includes a small cycle close to the N-terminus.\nIf the linear polyketide is selected as the peptide type, the water molecule is subtracted only from the precursor ion.");
	cyclicnterminus->setFixedWidth(leftdefaultwidth);
	cyclicnterminuslabel = new QLabel("Cyclic N-terminus:");
	miscgridlayout->addWidget(cyclicnterminuslabel, 1, 0);
	miscgridlayout->addWidget(cyclicnterminus, 1, 1);

	cycliccterminus = new QCheckBox();
	cycliccterminus->setToolTip("The water molecule is subtracted from all theoretical C-terminal fragment ions and the theoretical precursor mass.\nThis feature is useful if a linear peptide includes a small cycle close to the C-terminus.\nIf the linear polyketide is selected as the peptide type, the water molecule is subtracted only from the precursor ion.");
	cycliccterminus->setFixedWidth(leftdefaultwidth);
	cycliccterminuslabel = new QLabel("Cyclic C-terminus:");
	miscgridlayout->addWidget(cycliccterminuslabel, 2, 0);
	miscgridlayout->addWidget(cycliccterminus, 2, 1);

	enablescrambling = new QCheckBox();
	enablescrambling->setToolTip("Generate scrambled fragment ions of cyclic peptides in theoretical spectra.");
	enablescrambling->setFixedWidth(leftdefaultwidth);
	enablescramblinglabel = new QLabel("Enable Scrambling:");
	miscgridlayout->addWidget(enablescramblinglabel, 3, 0);
	miscgridlayout->addWidget(enablescrambling, 3, 1);

	similaritysearch = new QCheckBox();
	similaritysearch->setToolTip("Disable the filtering of sequence candidates by precursor mass. This option can be used to determine a peptide family when a modified peptide is included in a sequence database.");
	similaritysearch->setFixedWidth(leftdefaultwidth);
	similaritysearchlabel = new QLabel("Disable Precursor Mass Filter:");
	miscgridlayout->addWidget(similaritysearchlabel, 4, 0);
	miscgridlayout->addWidget(similaritysearch, 4, 1);

	regularblocksorder = new QCheckBox();
	regularblocksorder->setToolTip("Keep only polyketide sequence candidates whose ketide building blocks are in the regular order [water eliminating block]-[2H eliminating block]-[water eliminating block]-[2H eliminating block], etc.");
	regularblocksorder->setFixedWidth(leftdefaultwidth);
	regularblocksorderlabel = new QLabel("Regular Order of Ketide Blocks:");
	miscgridlayout->addWidget(regularblocksorderlabel, 5, 0);
	miscgridlayout->addWidget(regularblocksorder, 5, 1);

	miscgroupbox = new QGroupBox("Miscellaneous");
	miscgroupbox->setLayout(miscgridlayout);


	theoreticalspectragridlayout = new QGridLayout();
	
	sequencedatabaseline = new QLineEdit();
	sequencedatabaseline->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabasebutton = new QPushButton("Select");
	sequencedatabasebutton->setToolTip("Select the txt file containing a database of sequences.");
	sequencedatabasebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
	sequencedatabaselayout = new QHBoxLayout();
	sequencedatabaselayout->addWidget(sequencedatabaseline);
	sequencedatabaselayout->addWidget(sequencedatabasebutton);
	sequencedatabaselayout->setMargin(0);
	sequencedatabasewidget = new QWidget();
	sequencedatabasewidget->setLayout(sequencedatabaselayout);
	sequencedatabasewidget->setFixedWidth(rightdefaultwidth);
	sequencedatabaselabel = new QLabel("Sequence/Compound Database File:");
	theoreticalspectragridlayout->addWidget(sequencedatabaselabel, 0, 0);
	theoreticalspectragridlayout->addWidget(sequencedatabasewidget, 0, 1);

	scoretype = new QComboBox();
	scoretype->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	scoretype->setToolTip("A score for peptide-spectrum matches.");
	scoretype->addItem(tr("Number of matched peaks"));
	scoretype->addItem(tr("Sum of relative intensities of matched peaks"));
	scoretype->addItem(tr("Number of b-ions"));
	scoretype->addItem(tr("Number of y-ions"));
	scoretype->addItem(tr("Number of b-ions and y-ions"));
	scoretype->setFixedWidth(rightdefaultwidth);
	scoretypelabel = new QLabel("Score Type:");
	theoreticalspectragridlayout->addWidget(scoretypelabel, 1, 0);
	theoreticalspectragridlayout->addWidget(scoretype, 1, 1);

	hitsreported = new QSpinBox();
	hitsreported->setToolTip("A maximum length of an output report with peptide sequence candidates.");
	hitsreported->setRange(1, 100000);
	hitsreported->setSingleStep(1);
	hitsreported->setFixedWidth(rightdefaultwidth);
	hitsreportedlabel = new QLabel("Maximum Number of Reported Sequence Candidates:");
	theoreticalspectragridlayout->addWidget(hitsreportedlabel, 2, 0);
	theoreticalspectragridlayout->addWidget(hitsreported, 2, 1);

	sequencetag = new QLineEdit();
	sequencetag->setToolTip("Each peptide sequence candidate generated from a de novo graph must fulfil the peptide sequence tag. Otherwise, its theoretical spectrum is not generated and the peptide sequence candidate is excluded from the search.\nSee the syntax of tags in the documentation.");
	sequencetag->setMaxLength(5000);
	sequencetag->setFixedWidth(rightdefaultwidth);
	sequencetaglabel = new QLabel("Peptide Sequence Tag:");
	theoreticalspectragridlayout->addWidget(sequencetaglabel, 3, 0);
	theoreticalspectragridlayout->addWidget(sequencetag, 3, 1);

	iontypes = new cFragmentIonsListWidget(this);
	iontypes->setToolTip("Select ion types which will be generated in theoretical spectra.");
	iontypes->setFixedWidth(rightdefaultwidth);
	iontypeslabel = new QLabel("Ion Types:");
	theoreticalspectragridlayout->addWidget(iontypeslabel, 4, 0);
	theoreticalspectragridlayout->addWidget(iontypes, 4, 1);

	neutrallosstypes = new cNeutralLossesListWidget(this);
	neutrallosstypes->setToolTip("Define and select the types of neutral losses which will be generated in theoretical spectra.");
	neutrallosstypes->setFixedWidth(rightdefaultwidth);
	neutrallosstypeslabel = new QLabel("Neutral Losses:");
	theoreticalspectragridlayout->addWidget(neutrallosstypeslabel, 5, 0);
	theoreticalspectragridlayout->addWidget(neutrallosstypes, 5, 1);

	maximumcombinedlosses = new QSpinBox();
	maximumcombinedlosses->setToolTip("Maximum number of combined neutral losses.");
	maximumcombinedlosses->setRange(0, 10000);
	maximumcombinedlosses->setSingleStep(1);
	maximumcombinedlosses->setFixedWidth(rightdefaultwidth);
	maximumcombinedlosseslabel = new QLabel("Maximum Number of Combined Losses:");
	theoreticalspectragridlayout->addWidget(maximumcombinedlosseslabel, 6, 0);
	theoreticalspectragridlayout->addWidget(maximumcombinedlosses, 6, 1);

	//clearhitswithoutparent = new QCheckBox();
	//clearhitswithoutparent->setToolTip("If checked, a peak is not matched if the corresponding parent peak is not matched (e.g., a dehydrated b-ion is not matched if corresponding b-ion is not matched).");
	//theoreticalspectraformlayout->addRow(tr("Remove Hits of Fragments without Hits of Parent Fragments: "), clearhitswithoutparent);

	reportunmatchedtheoreticalpeaks = new QCheckBox();
	reportunmatchedtheoreticalpeaks->setToolTip("If checked, all unmatched theoretical peaks are reported.\nIf unchecked, unmatched theoretical peaks are reported only if a corresponding isotope pattern has been matched.\nThis feature may spend a lot of main memory, keep it disabled if possible.");
	reportunmatchedtheoreticalpeaks->setFixedWidth(rightdefaultwidth);
	reportunmatchedtheoreticalpeakslabel = new QLabel("Report Unmatched Theoretical Peaks:");
	theoreticalspectragridlayout->addWidget(reportunmatchedtheoreticalpeakslabel, 7, 0);
	theoreticalspectragridlayout->addWidget(reportunmatchedtheoreticalpeaks, 7, 1);

	generateisotopepattern = new QCheckBox();
	generateisotopepattern->setChecked(true);
	generateisotopepattern->setToolTip("The full isotope patterns of compounds are generated in theoretical spectra.\nThe FWHM value is used for this purpose. If checked, the deisotoping is disabled automatically.");
	generateisotopepattern->setFixedWidth(rightdefaultwidth);
	generateisotopepatternlabel = new QLabel("Generate Full Isotope Patterns:");
	theoreticalspectragridlayout->addWidget(generateisotopepatternlabel, 8, 0);
	theoreticalspectragridlayout->addWidget(generateisotopepattern, 8, 1);

	minimumpatternsize = new QSpinBox();
	minimumpatternsize->setToolTip("The minimum number of matched peaks in an isotope pattern to be reported. The option \"Generate Full Isotope Patterns\" must be enabled.");
	minimumpatternsize->setRange(1, 100);
	minimumpatternsize->setSingleStep(1);
	minimumpatternsize->setFixedWidth(rightdefaultwidth);
	minimumpatternsizelabel = new QLabel("Minimum Pattern Size:");
	theoreticalspectragridlayout->addWidget(minimumpatternsizelabel, 9, 0);
	theoreticalspectragridlayout->addWidget(minimumpatternsize, 9, 1);

	theoreticalspectragroupbox = new QGroupBox("Theoretical Spectrum/Spectra");
	theoreticalspectragroupbox->setLayout(theoreticalspectragridlayout);


	searchedsequencegridlayout = new QGridLayout();

	searchedsequenceline = new QLineEdit();
	searchedsequenceline->setToolTip("A peptide sequence which you are searching for or a peptide sequence tag.\nA peptide sequence must be entered if \"Mode\" is set up to \"Compare Peaklist(s) with Spectrum of Searched Sequence\".\nOtherwise, the option is similar to \"Peptide Sequence Tag\" with a difference that a peptide sequence candidate is not removed from the search but it is just highlighted in the output list of peptide sequence candidates.");
	searchedsequenceline->setMaxLength(5000);
	searchedsequencebutton = new QPushButton("Edit");
	searchedsequencebutton->setMinimumWidth(50);
	searchedsequencebutton->setToolTip("Edit the sequence using the 'Draw Peptide' tool.");
	searchedsequencebutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
	searchedsequencelayout = new QHBoxLayout();
	searchedsequencelayout->addWidget(searchedsequenceline);
	searchedsequencelayout->addWidget(searchedsequencebutton);
	searchedsequencelayout->setMargin(0);
	searchedsequencewidget = new QWidget();
	searchedsequencewidget->setLayout(searchedsequencelayout);
	searchedsequencewidget->setFixedWidth(searchedsequencedefaultwidth);
	searchedsequencelabel = new QLabel("Sequence:");
	searchedsequencegridlayout->addWidget(searchedsequencelabel, 0, 0);
	searchedsequencegridlayout->addWidget(searchedsequencewidget, 0, 1);

	searchedsequenceNtermmodif = new QLineEdit();
	searchedsequenceNtermmodif->setToolTip("A name of an N-terminal modification which belongs to the searched peptide. The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceNtermmodif->setFixedWidth(searchedsequencedefaultwidth);
	searchedsequenceNtermmodiflabel = new QLabel("N-terminal Modification:");
	searchedsequencegridlayout->addWidget(searchedsequenceNtermmodiflabel, 1, 0);
	searchedsequencegridlayout->addWidget(searchedsequenceNtermmodif, 1, 1);

	searchedsequenceCtermmodif = new QLineEdit();
	searchedsequenceCtermmodif->setToolTip("A name of a C-terminal modification which belongs to the searched peptide. The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceCtermmodif->setFixedWidth(searchedsequencedefaultwidth);
	searchedsequenceCtermmodiflabel = new QLabel("C-terminal Modification:");
	searchedsequencegridlayout->addWidget(searchedsequenceCtermmodiflabel, 2, 0);
	searchedsequencegridlayout->addWidget(searchedsequenceCtermmodif, 2, 1);

	searchedsequenceTmodif = new QLineEdit();
	searchedsequenceTmodif->setToolTip("A name of an N-terminal or C-terminal modification which belongs to a branch of a searched peptide (branched and branch-cyclic peptides only). The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceTmodif->setFixedWidth(searchedsequencedefaultwidth);
	searchedsequenceTmodiflabel = new QLabel("Branch Modification:");
	searchedsequencegridlayout->addWidget(searchedsequenceTmodiflabel, 3, 0);
	searchedsequencegridlayout->addWidget(searchedsequenceTmodif, 3, 1);

	searchedsequencegroupbox = new QGroupBox("Searched Sequence");
	searchedsequencegroupbox->setLayout(searchedsequencegridlayout);


	vlayout1 = new QVBoxLayout();
	vlayout1->addWidget(searchgroupbox);
	vlayout1->addWidget(experimentalspectragroupbox);
	vlayout1->addWidget(brickdatabasegroupbox);
	vlayout1->addWidget(miscgroupbox);

	vlayout2 = new QVBoxLayout();
	vlayout2->addWidget(theoreticalspectragroupbox);
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

	resize(1280, 780);

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
	connect(iontypes, SIGNAL(resetReleased()), this, SLOT(resetFragmentIonTypes()));
	connect(sequencedatabasebutton, SIGNAL(released()), this, SLOT(sequenceDatabaseButtonReleased()));
	connect(searchedsequencebutton, SIGNAL(released()), this, SLOT(drawPeptideButtonReleased()));
	connect(this, SIGNAL(sendSequenceLine(int, QString)), parent, SLOT(setAndShowDrawPeptideWidget(int, QString)));
	connect(this, SIGNAL(clearIonSelection()), iontypes, SLOT(clearAllItems()));
	connect(this, SIGNAL(setHCON()), neutrallosstypes, SLOT(setHCON()));

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

	delete modelabel;
	delete mode;
	delete maximumnumberofthreadslabel;
	delete maximumnumberofthreads;
	delete searchgridlayout;
	delete searchgroupbox;

	delete peptidetypelabel;
	delete peptidetype;
	delete peaklistlabel;
	delete peaklistline;
	delete useprofiledata;
	delete peaklistbutton;
	delete peaklistlayout;
	delete peaklistwidget;
	delete scannumberlabel;
	delete scannumber;
	delete precursormasslabel;
	delete precursormass;
	delete precursoradductlabel;
	delete precursoradduct;
	delete precursorchargelabel;
	delete precursorcharge;
	delete precursormasserrortolerancelabel;
	delete precursormasserrortolerance;
	delete fragmentmasserrortolerancelabel;
	delete fragmentmasserrortolerance;
	delete minimumrelativeintensitythresholdlabel;
	delete minimumrelativeintensitythreshold;
	delete minimumabsoluteintensitythresholdlabel;
	delete minimumabsoluteintensitythreshold;
	delete minimummzlabel;
	delete minimummz;
	delete fwhmlabel;
	delete fwhm;
	delete experimentalspectragridlayout;
	delete experimentalspectragroupbox;

	delete brickdatabaselabel;
	delete brickdatabaseline;
	delete brickdatabasebutton;
	delete brickdatabaselayout;
	delete brickdatabasewidget;
	delete maximumbricksincombinatiolabel;
	delete maximumbricksincombinationbegin;
	delete maximumbricksincombinationmiddle;
	delete maximumbricksincombinationend;
	delete maximumbricksincombinationlayout;
	delete maximumbricksincombinationwidget;
	delete maximumcumulativemasslabel;
	delete maximumcumulativemass;
	delete modificationslabel;
	delete modificationsline;
	delete modificationsbutton;
	delete modificationslayout;
	delete modificationswidget;
	delete brickdatabasegridlayout;
	delete brickdatabasegroupbox;
	
	delete blindedgeslabel;
	delete blindedges;
	delete cyclicnterminuslabel;
	delete cyclicnterminus;
	delete cycliccterminuslabel;
	delete cycliccterminus;
	delete enablescramblinglabel;
	delete enablescrambling;
	delete similaritysearchlabel;
	delete similaritysearch;
	delete regularblocksorderlabel;
	delete regularblocksorder;
	delete miscgridlayout;
	delete miscgroupbox;

	delete sequencedatabaselabel;
	delete sequencedatabaseline;
	delete sequencedatabasebutton;
	delete sequencedatabaselayout;
	delete sequencedatabasewidget;
	delete scoretypelabel;
	delete scoretype;
	delete hitsreportedlabel;
	delete hitsreported;
	delete sequencetaglabel;
	delete sequencetag;
	delete iontypeslabel;
	delete iontypes;
	delete neutrallosstypeslabel;
	delete neutrallosstypes;
	delete maximumcombinedlosseslabel;
	delete maximumcombinedlosses;
	//delete clearhitswithoutparent;
	delete reportunmatchedtheoreticalpeakslabel;
	delete reportunmatchedtheoreticalpeaks;
	delete generateisotopepatternlabel;
	delete generateisotopepattern;
	delete minimumpatternsizelabel;
	delete minimumpatternsize;
	delete theoreticalspectragridlayout;
	delete theoreticalspectragroupbox;

	delete searchedsequencelabel;
	delete searchedsequenceline;
	delete searchedsequencebutton;
	delete searchedsequencelayout;
	delete searchedsequencewidget;
	delete searchedsequenceNtermmodiflabel;
	delete searchedsequenceNtermmodif;
	delete searchedsequenceCtermmodiflabel;
	delete searchedsequenceCtermmodif;
	delete searchedsequenceTmodiflabel;
	delete searchedsequenceTmodif;
	delete searchedsequencegridlayout;
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
	if ((eModeType)mode->currentIndex() == dereplication) {
		mode->setCurrentIndex(denovoengine);
	}
	peptidetype->setCurrentIndex(peptidetypeindex);
	searchedsequenceline->setText(sequence);
}


void cParametersWidget::setTag(int peptidetypeindex, QString tag) {
	if ((eModeType)mode->currentIndex() == dereplication) {
		mode->setCurrentIndex(denovoengine);
	}
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

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_H)) {
		if ((mode->currentIndex() == (int)denovoengine) || (mode->currentIndex() == (int)singlecomparison) || (mode->currentIndex() == (int)databasesearch)) {
			emit clearIonSelection();
			emit setHCON();
		}
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
	int i;

	if (!filename.isEmpty()) {
		lastdirloadsettings = filename;

		QString qloadstring;

		settingsfile = filename;
		save->setText(QString("  Save '") + QString(settingsfile.toStdString().substr(settingsfile.toStdString().rfind('/') + 1, settingsfile.toStdString().size()).c_str()) + QString("'  "));
		QSettings settings(settingsfile, QSettings::IniFormat);

		mode->setCurrentIndex(settings.value("mode", 0).toInt());
		maximumnumberofthreads->setValue(settings.value("maximumnumberofthreads", 1).toInt());

		peptidetype->setCurrentIndex(settings.value("peptidetype", 0).toInt());

		// both - mode and peptide type are now defined
		updateSettingsWhenModeChanged(mode->currentIndex());

		peaklistline->setText(settings.value("peaklist", "").toString());
		settings.value("useprofiledata", 0).toInt() == 0 ? useprofiledata->setChecked(false) : useprofiledata->setChecked(true);
		scannumber->setValue(settings.value("scannumber", 1).toInt());
		precursormass->setValue(settings.value("precursormass", 0.0).toDouble());
		precursoradduct->setText(settings.value("precursoradduct", "").toString());
		precursormasserrortolerance->setValue(settings.value("precursormasserrortolerance", 5.0).toDouble());
		precursorcharge->setValue(settings.value("precursorcharge", 1).toInt());
		fragmentmasserrortolerance->setValue(settings.value("fragmentmasserrortolerance", 5.0).toDouble());
		minimumrelativeintensitythreshold->setValue(settings.value("minimumrelativeintensitythreshold", 1).toDouble());
		minimumabsoluteintensitythreshold->setValue(settings.value("minimumabsoluteintensitythreshold", 0).toUInt());
		minimummz->setValue(settings.value("minimummz", 150).toDouble());
		fwhm->setValue(settings.value("fwhm", 0.05).toDouble());

		brickdatabaseline->setText(settings.value("brickdatabase", "").toString());
		maximumbricksincombinationbegin->setValue(settings.value("maximumbricksincombinationbegin", 1).toInt());
		maximumbricksincombinationmiddle->setValue(settings.value("maximumbricksincombinationmiddle", 1).toInt());
		maximumbricksincombinationend->setValue(settings.value("maximumbricksincombinationend", 1).toInt());
		maximumcumulativemass->setValue(settings.value("maximumcumulativemass", 0).toDouble());
		modificationsline->setText(settings.value("modificationsfile", "").toString());

		blindedges->setCurrentIndex(settings.value("blindedges", 2).toInt());
		settings.value("cyclicnterminus", 0).toInt() == 0 ? cyclicnterminus->setChecked(false) : cyclicnterminus->setChecked(true);
		settings.value("cycliccterminus", 0).toInt() == 0 ? cycliccterminus->setChecked(false) : cycliccterminus->setChecked(true);
		settings.value("enablescrambling", 0).toInt() == 0 ? enablescrambling->setChecked(false) : enablescrambling->setChecked(true);
		settings.value("similaritysearch", 0).toInt() == 0 ? similaritysearch->setChecked(false) : similaritysearch->setChecked(true);
		settings.value("regularblocksorder", 0).toInt() == 0 ? regularblocksorder->setChecked(false) : regularblocksorder->setChecked(true);

		sequencedatabaseline->setText(settings.value("sequencedatabase", "").toString());
		scoretype->setCurrentIndex(settings.value("scoretype", (int)number_of_matched_peaks).toInt());
		if ((scoretype->currentIndex() < (int)number_of_matched_peaks) || (scoretype->currentIndex() > (int)number_of_b_and_y_ions)) {
			scoretype->setCurrentIndex((int)number_of_matched_peaks);
		}
		hitsreported->setValue(settings.value("hitsreported", 100).toInt());
		sequencetag->setText(settings.value("sequencetag", "").toString());
		
		for (i = 0; i < iontypes->getList()->count(); i++) {
			qloadstring = ("fragmentiontype_" + to_string(i)).c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? iontypes->getList()->item(i)->setSelected(false) : iontypes->getList()->item(i)->setSelected(true);
		}

		neutrallosstypes->getList()->clear();
		i = 0;
		qloadstring = ("neutrallossformula_" + to_string(i)).c_str();
		while (settings.value(qloadstring, "XXX").toString().compare("XXX") != 0) {
			neutrallosstypes->addItem(settings.value(qloadstring).toString());

			qloadstring = ("neutrallosstype_" + to_string(i)).c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? neutrallosstypes->getList()->item(i)->setSelected(false) : neutrallosstypes->getList()->item(i)->setSelected(true);

			i++;
			qloadstring = ("neutrallossformula_" + to_string(i)).c_str();
		}

		maximumcombinedlosses->setValue(settings.value("maximumcombinedlosses", 2).toInt());
		//settings.value("clearhitswithoutparent", 0).toInt() == 0 ? clearhitswithoutparent->setChecked(false) : clearhitswithoutparent->setChecked(true);
		settings.value("reportunmatchedtheoreticalpeaks", 0).toInt() == 0 ? reportunmatchedtheoreticalpeaks->setChecked(false) : reportunmatchedtheoreticalpeaks->setChecked(true);
		settings.value("generateisotopepattern", 0).toInt() == 0 ? generateisotopepattern->setChecked(false) : generateisotopepattern->setChecked(true);
		minimumpatternsize->setValue(settings.value("minimumpatternsize", 1).toInt());

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

	settings.setValue("mode", mode->currentIndex());
	settings.setValue("maximumnumberofthreads", maximumnumberofthreads->value());

	settings.setValue("peptidetype", peptidetype->currentIndex());
	settings.setValue("peaklist", peaklistline->text());
	useprofiledata->isChecked() ? settings.setValue("useprofiledata", 1) : settings.setValue("useprofiledata", 0);
	settings.setValue("scannumber", scannumber->value());
	settings.setValue("precursormass", precursormass->value());
	settings.setValue("precursoradduct", precursoradduct->text());
	settings.setValue("precursormasserrortolerance", precursormasserrortolerance->value());
	settings.setValue("precursorcharge", precursorcharge->value());
	settings.setValue("fragmentmasserrortolerance", fragmentmasserrortolerance->value());
	settings.setValue("minimumrelativeintensitythreshold", minimumrelativeintensitythreshold->value());
	settings.setValue("minimumabsoluteintensitythreshold", minimumabsoluteintensitythreshold->value());
	settings.setValue("minimummz", minimummz->value());
	settings.setValue("fwhm", fwhm->value());

	settings.setValue("brickdatabase", brickdatabaseline->text());
	settings.setValue("maximumbricksincombinationbegin", maximumbricksincombinationbegin->value());
	settings.setValue("maximumbricksincombinationmiddle", maximumbricksincombinationmiddle->value());
	settings.setValue("maximumbricksincombinationend", maximumbricksincombinationend->value());
	settings.setValue("maximumcumulativemass", maximumcumulativemass->value());
	settings.setValue("modificationsfile", modificationsline->text());

	settings.setValue("blindedges", blindedges->currentIndex());
	cyclicnterminus->isChecked() ? settings.setValue("cyclicnterminus", 1) : settings.setValue("cyclicnterminus", 0);
	cycliccterminus->isChecked() ? settings.setValue("cycliccterminus", 1) : settings.setValue("cycliccterminus", 0);
	enablescrambling->isChecked() ? settings.setValue("enablescrambling", 1) : settings.setValue("enablescrambling", 0);
	similaritysearch->isChecked() ? settings.setValue("similaritysearch", 1) : settings.setValue("similaritysearch", 0);
	regularblocksorder->isChecked() ? settings.setValue("regularblocksorder", 1) : settings.setValue("regularblocksorder", 0);

	settings.setValue("sequencedatabase", sequencedatabaseline->text());
	settings.setValue("scoretype", scoretype->currentIndex());
	settings.setValue("hitsreported", hitsreported->value());
	settings.setValue("sequencetag", sequencetag->text());

	for (int i = 0; i < iontypes->getList()->count(); i++) {
		qsavestring = ("fragmentiontype_" + to_string(i)).c_str();
		iontypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);
	}

	for (int i = 0; i < neutrallosstypes->getList()->count(); i++) {
		qsavestring = ("neutrallosstype_" + to_string(i)).c_str();
		neutrallosstypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

		qsavestring = ("neutrallossformula_" + to_string(i)).c_str();
		settings.setValue(qsavestring, neutrallosstypes->getList()->item(i)->text());
	}

	settings.setValue("maximumcombinedlosses", maximumcombinedlosses->value());
	//clearhitswithoutparent->isChecked() ? settings.setValue("clearhitswithoutparent", 1) : settings.setValue("clearhitswithoutparent", 0);
	reportunmatchedtheoreticalpeaks->isChecked() ? settings.setValue("reportunmatchedtheoreticalpeaks", 1) : settings.setValue("reportunmatchedtheoreticalpeaks", 0);
	generateisotopepattern->isChecked() ? settings.setValue("generateisotopepattern", 1) : settings.setValue("generateisotopepattern", 0);
	settings.setValue("minimumpatternsize", minimumpatternsize->value());

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
		QString filename = QFileDialog::getOpenFileName(this, tr("Select Peaklist..."), lastdirselectpeaklist, tr("Peak Lists (*.txt *.mgf *.mzML *.mzXML *.imzML *.baf *.raw *.dat *.mis ser)"));
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

	cSummaryFormula tmpformula;
	string tmpstring;
	string errmsg;

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

	for (int i = 0; i < neutrallosstypes->getList()->count(); i++) {
		tmpstring = neutrallosstypes->getList()->item(i)->text().toStdString();
		tmpformula.setFormula(tmpstring, false);

		if (!tmpformula.isValid(errmsg)) {
			errstr = "A molecular formula of a neutral loss is not valid!\n\n";
			errstr += errmsg.c_str();
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}

	parameters.mode = (eModeType)mode->currentIndex();
	parameters.maximumnumberofthreads = maximumnumberofthreads->value();

	parameters.peptidetype = (ePeptideType)peptidetype->currentIndex();
	parameters.peaklistfilename = peaklistline->text().toStdString();
	parameters.useprofiledata = useprofiledata->isChecked();
	parameters.scannumber = scannumber->value();
	parameters.precursormass = precursormass->value();
	parameters.precursoradduct = precursoradduct->text().toStdString();
	parameters.precursormasserrortolerance = precursormasserrortolerance->value();
	parameters.precursorcharge = precursorcharge->value();
	parameters.fragmentmasserrortolerance = fragmentmasserrortolerance->value();
	parameters.minimumrelativeintensitythreshold = minimumrelativeintensitythreshold->value();
	parameters.minimumabsoluteintensitythreshold = minimumabsoluteintensitythreshold->value();
	parameters.minimummz = minimummz->value();
	parameters.fwhm = fwhm->value();

	parameters.bricksdatabasefilename = brickdatabaseline->text().toStdString();
	parameters.maximumbricksincombinationbegin = maximumbricksincombinationbegin->value();
	parameters.maximumbricksincombinationmiddle = maximumbricksincombinationmiddle->value();
	parameters.maximumbricksincombinationend = maximumbricksincombinationend->value();
	parameters.maximumcumulativemass = maximumcumulativemass->value();
	parameters.maximumbricksincombination = max(max(parameters.maximumbricksincombinationbegin, parameters.maximumbricksincombinationmiddle), parameters.maximumbricksincombinationend);
	parameters.modificationsfilename = modificationsline->text().toStdString();

	parameters.blindedges = blindedges->currentIndex();
	parameters.cyclicnterminus = cyclicnterminus->isChecked();
	parameters.cycliccterminus = cycliccterminus->isChecked();
	parameters.enablescrambling = enablescrambling->isChecked();
	parameters.similaritysearch = similaritysearch->isChecked();
	parameters.regularblocksorder = regularblocksorder->isChecked();

	parameters.sequencedatabasefilename = sequencedatabaseline->text().toStdString();
	parameters.scoretype = (eScoreType)scoretype->currentIndex();
	parameters.hitsreported = hitsreported->value();
	parameters.sequencetag = sequencetag->text().toStdString();
	parameters.originalsequencetag = parameters.sequencetag;

	parameters.ionsfortheoreticalspectra.clear();
	int start;

	if ((eModeType)mode->currentIndex() == dereplication) {
		start = ms_Hplus;
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

	for (int i = 0; i < iontypes->getList()->count(); i++) {
		if (iontypes->getList()->item(i)->isSelected()) {
			parameters.ionsfortheoreticalspectra.push_back((eFragmentIonType)(i + start));
		}
	}

	parameters.originalneutrallossesdefinitions.clear();
	parameters.originalneutrallossesfortheoreticalspectra.clear();
	neutralLoss loss;
	for (int i = 0; i < neutrallosstypes->getList()->count(); i++) {
		loss.clear();
		loss.summary = neutrallosstypes->getList()->item(i)->text().toStdString();
		parameters.originalneutrallossesdefinitions.push_back(loss);
		if (neutrallosstypes->getList()->item(i)->isSelected()) {
			parameters.originalneutrallossesfortheoreticalspectra.push_back(i);
		}
	}

	parameters.maximumcombinedlosses = maximumcombinedlosses->value();
	//parameters.clearhitswithoutparent = clearhitswithoutparent->isChecked();
	parameters.reportunmatchedtheoreticalpeaks = reportunmatchedtheoreticalpeaks->isChecked();
	parameters.generateisotopepattern = generateisotopepattern->isChecked();
	parameters.minimumpatternsize = minimumpatternsize->value();

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
	mode->setCurrentIndex(parameters.mode);
	maximumnumberofthreads->setValue(parameters.maximumnumberofthreads);

	peptidetype->setCurrentIndex(parameters.peptidetype);
	peaklistline->setText(parameters.peaklistfilename.c_str());
	useprofiledata->setChecked(parameters.useprofiledata);
	scannumber->setValue(parameters.scannumber);
	precursormass->setValue(parameters.precursormass);
	precursoradduct->setText(parameters.precursoradduct.c_str());
	precursormasserrortolerance->setValue(parameters.precursormasserrortolerance);
	precursorcharge->setValue(parameters.precursorcharge);
	fragmentmasserrortolerance->setValue(parameters.fragmentmasserrortolerance);
	minimumrelativeintensitythreshold->setValue(parameters.minimumrelativeintensitythreshold);
	minimumabsoluteintensitythreshold->setValue(parameters.minimumabsoluteintensitythreshold);
	minimummz->setValue(parameters.minimummz);
	fwhm->setValue(parameters.fwhm);

	brickdatabaseline->setText(parameters.bricksdatabasefilename.c_str());
	maximumbricksincombinationbegin->setValue(parameters.maximumbricksincombinationbegin);
	maximumbricksincombinationmiddle->setValue(parameters.maximumbricksincombinationmiddle);
	maximumbricksincombinationend->setValue(parameters.maximumbricksincombinationend);
	maximumcumulativemass->setValue(parameters.maximumcumulativemass);
	modificationsline->setText(parameters.modificationsfilename.c_str());

	blindedges->setCurrentIndex(parameters.blindedges);
	cyclicnterminus->setChecked(parameters.cyclicnterminus);
	cycliccterminus->setChecked(parameters.cycliccterminus);
	enablescrambling->setChecked(parameters.enablescrambling);
	similaritysearch->setChecked(parameters.similaritysearch);
	regularblocksorder->setChecked(parameters.regularblocksorder);

	sequencedatabaseline->setText(parameters.sequencedatabasefilename.c_str());
	scoretype->setCurrentIndex(parameters.scoretype);
	hitsreported->setValue(parameters.hitsreported);
	sequencetag->setText(parameters.sequencetag.c_str());

	int start;
	if (parameters.mode == dereplication) {
		start = ms_Hplus;
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

	for (int i = 0; i < (int)parameters.ionsfortheoreticalspectra.size(); i++) {
		iontypes->getList()->item(parameters.ionsfortheoreticalspectra[i] - start)->setSelected(true);
	}

	neutrallosstypes->getList()->clear();
	for (int i = 0; i < (int)parameters.originalneutrallossesdefinitions.size(); i++) {
		neutrallosstypes->addItem(parameters.originalneutrallossesdefinitions[i].summary.c_str());
	}

	for (int i = 0; i < (int)parameters.originalneutrallossesfortheoreticalspectra.size(); i++) {
		neutrallosstypes->getList()->item(parameters.originalneutrallossesfortheoreticalspectra[i])->setSelected(true);
	}

	maximumcombinedlosses->setValue(parameters.maximumcombinedlosses);
	//clearhitswithoutparent->setChecked(parameters.clearhitswithoutparent);
	reportunmatchedtheoreticalpeaks->setChecked(parameters.reportunmatchedtheoreticalpeaks);
	generateisotopepattern->setChecked(parameters.generateisotopepattern);
	minimumpatternsize->setValue(parameters.minimumpatternsize);
	
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
		scannumber->setDisabled(false);
		precursormass->setDisabled(false);
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		fwhm->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(false);
		maximumbricksincombinationmiddle->setDisabled(false);
		maximumbricksincombinationend->setDisabled(false);
		maximumcumulativemass->setDisabled(false);
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
		iontypes->setDisabled(false);
		neutrallosstypes->setDisabled(false);
		maximumcombinedlosses->setDisabled(false);
		//clearhitswithoutparent->setDisabled(false);
		reportunmatchedtheoreticalpeaks->setDisabled(false);
		generateisotopepattern->setDisabled(false);
		minimumpatternsize->setDisabled(false);
		searchedsequenceline->setDisabled(false);
		searchedsequencebutton->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case singlecomparison:
		peptidetype->setDisabled(false);
		scannumber->setDisabled(true);
		precursormass->setDisabled(false);
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		fwhm->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
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
		iontypes->setDisabled(false);
		neutrallosstypes->setDisabled(false);
		maximumcombinedlosses->setDisabled(false);
		//clearhitswithoutparent->setDisabled(false);
		reportunmatchedtheoreticalpeaks->setDisabled(false);
		generateisotopepattern->setDisabled(false);
		minimumpatternsize->setDisabled(false);
		searchedsequenceline->setDisabled(false);
		searchedsequencebutton->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case databasesearch:
		peptidetype->setDisabled(false);
		scannumber->setDisabled(false);
		precursormass->setDisabled(false);
		precursoradduct->setDisabled(false);
		precursorcharge->setDisabled(false);
		precursormasserrortolerance->setDisabled(false);
		fwhm->setDisabled(false);
		brickdatabaseline->setDisabled(false);
		brickdatabasebutton->setDisabled(false);
		maximumbricksincombinationbegin->setDisabled(true);
		maximumbricksincombinationmiddle->setDisabled(true);
		maximumbricksincombinationend->setDisabled(true);
		maximumcumulativemass->setDisabled(true);
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
		iontypes->setDisabled(false);
		neutrallosstypes->setDisabled(false);
		maximumcombinedlosses->setDisabled(false);
		//clearhitswithoutparent->setDisabled(false);
		reportunmatchedtheoreticalpeaks->setDisabled(false);
		generateisotopepattern->setDisabled(false);
		minimumpatternsize->setDisabled(false);
		searchedsequenceline->setDisabled(false);
		searchedsequencebutton->setDisabled(false);

		updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());
		break;
	case dereplication:
		peptidetype->setDisabled(true);
		scannumber->setDisabled(true);
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
		iontypes->setDisabled(false);
		neutrallosstypes->setDisabled(false);
		maximumcombinedlosses->setDisabled(false);
		//clearhitswithoutparent->setDisabled(true);
		reportunmatchedtheoreticalpeaks->setDisabled(false);
		generateisotopepattern->setDisabled(false);
		minimumpatternsize->setDisabled(false);
		searchedsequenceline->setDisabled(true);
		searchedsequencebutton->setDisabled(true);

		resetFragmentIonTypes();
		
		modificationsline->setDisabled(true);
		modificationsbutton->setDisabled(true);
		cyclicnterminus->setDisabled(true);
		cycliccterminus->setDisabled(true);
		enablescrambling->setDisabled(true);
		regularblocksorder->setDisabled(true);
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
	iontypes->getList()->clear();
	eFragmentIonType start, end;
	
	if ((eModeType)mode->currentIndex() == dereplication) {
		start = ms_Hplus;
		end = ms_MGa4H;
	}
	else {
		switch ((ePeptideType)peptidetype->currentIndex()) {
		case linear:
		case branched:
			start = a_ion;
			end = z_ion;
			break;
		case cyclic:
			start = a_ion;
			end = c_ion;
			break;
		case branchcyclic:
			start = a_ion;
			end = z_ion;
			break;
		case linearpolyketide:
			start = l1h_ion;
			end = r2oh_ion;
			break;
		case cyclicpolyketide:
			start = l1h_ion; // l0h_ion;
			end = l2h_ion;
			break;
		case other:
			break;
		default:
			break;
		}
	}

	for (int i = (int)start; i <= (int)end; i++) {

		iontypes->getList()->addItem(tr(parameters.iondefinitions[(eFragmentIonType)i].name.c_str()));

		if ((eModeType)mode->currentIndex() == dereplication) {
			if ((eFragmentIonType)i == ms_Hplus) {
				iontypes->getList()->item(i-start)->setSelected(true);
			}
		}
		else {
			switch ((ePeptideType)peptidetype->currentIndex()) {
			case linear:
			case branched:
				if (((eFragmentIonType)i == b_ion) || ((eFragmentIonType)i == y_ion)) {
					iontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case cyclic:
				if ((eFragmentIonType)i == b_ion) {
					iontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case branchcyclic:
				if (((eFragmentIonType)i == b_ion) || ((eFragmentIonType)i == y_ion)) {
					iontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case linearpolyketide:
				if (((eFragmentIonType)i == l1h_ion) || ((eFragmentIonType)i == l2h_ion) || ((eFragmentIonType)i == r1h_ion) || ((eFragmentIonType)i == r2h_ion) ||
					((eFragmentIonType)i == l1oh_ion) || ((eFragmentIonType)i == l2oh_ion) || ((eFragmentIonType)i == r1oh_ion) || ((eFragmentIonType)i == r2oh_ion)
					) {
					iontypes->getList()->item(i-start)->setSelected(true);
				}
				break;
			case cyclicpolyketide:
				if (/*((eFragmentIonType)i == l0h_ion) ||*/ ((eFragmentIonType)i == l1h_ion) || ((eFragmentIonType)i == l2h_ion)) {
					iontypes->getList()->item(i-start)->setSelected(true);
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

