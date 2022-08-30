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

// C++17 begin
#if OS_TYPE == WIN
	#include <filesystem>
#endif
// C++17 end


cParametersWidget::cParametersWidget(cGlobalPreferences* globalpreferences, QWidget* parent) {
	hideunusedparameters = true;

	this->globalpreferences = globalpreferences;
	this->parent = parent;

	int leftdefaultwidth = 420;
	int rightdefaultwidth = 420;
	int searchedsequencedefaultwidth = 510;
	int checkboxfieldwidth = 80;

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
	QString modetooltip = "'De Novo Search Engine - MS/MS': perform the de novo sequencing.\n";
	modetooltip += "'Compare Peaklist(s) with Spectrum of Searched Sequence - MS/MS': a theoretical spectrum is generated from the input 'Searched Sequence' and compared with the peaklist(s).\n";
	modetooltip += "'Compare Peaklist with Database - MS/MS': a peaklist is compared with theoretical spectra generated from a database of sequences.\n";
	modetooltip += "'Compare Peaklist(s) with Database - MS, LC-MS, MSI': dereplication; the peaklist(s) are compared with a theoretical peaklist generated from a database of compounds.\n";
	modetooltip += "'Compound Search - MS, LC-MS, MSI': the peaklist(s) are compared with a theoretical peaklist generated from a list of input chemical elements (a database-free search method).";
	mode->setToolTip(modetooltip);
	mode->addItem(tr("De Novo Search Engine - MS/MS"));
	mode->addItem(tr("Compare Peaklist(s) with Spectrum of Searched Sequence - MS/MS"));
	mode->addItem(tr("Compare Peaklist with Database - MS/MS"));
	mode->addItem(tr("Compare Peaklist(s) with Database - MS, LC-MS, MSI"));
	mode->addItem(tr("Compound Search - MS, LC-MS, MSI"));
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
	peptidetype->addItem(tr("Other"));
	peptidetype->setFixedWidth(leftdefaultwidth);
	peptidetypelabel = new QLabel("Peptide Type:");
	experimentalspectragridlayout->addWidget(peptidetypelabel, 0, 0);
	experimentalspectragridlayout->addWidget(peptidetype, 0, 1); 

	peaklistline = new QLineEdit();
	useprofiledata = new QCheckBox();
	useprofiledata->setToolTip("Use profile data (if applicable).");
	peaklistbutton = new QPushButton("Select");
	#if OS_TYPE != WIN
		peaklistline->setToolTip("Select input file(s). Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists).");
		peaklistbutton->setToolTip("Select input file(s). Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists).");
	#else
		peaklistline->setToolTip("Select input file(s). Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists),\nbaf (profile spectra),\nraw (profile spectra or peaklists),\ndat (peaklists),\nmis (deprecated),\nser (deprecated).");
		peaklistbutton->setToolTip("Select input file(s). Supported file formats:\ntxt (peaklists),\nmgf (peaklists),\nmzML (profile spectra or peaklists),\nmzXML (peaklists),\nimzML (profile spectra or peaklists),\nbaf (profile spectra),\nraw (profile spectra or peaklists),\ndat (peaklists),\nmis (deprecated),\nser (deprecated).");
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
	precursoradduct->setToolTip("Enter the formula of a precursor ion adduct (e.g., Na, K, Li).\nH is used by default if the field is empty.\nm/z ratios of theoretical peaks are shifted by mass of adduct.\nSee the manual for more details.");
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
	minimummz->setRange(0, 100000);
	minimummz->setSingleStep(1);
	minimummz->setPrefix("minimum: ");

	maximummz = new QDoubleSpinBox();
	maximummz->setToolTip("Enter the maximum m/z ratio. Peaks with m/z ratios above the threshold are removed from the peaklist (0 = disabled).");
	maximummz->setDecimals(3);
	maximummz->setRange(0, 100000);
	maximummz->setSingleStep(1);
	maximummz->setPrefix("maximum: ");

	mzratiolayout = new QHBoxLayout();
	mzratiolayout->addWidget(minimummz);
	mzratiolayout->addWidget(maximummz);
	mzratiolayout->setMargin(0);
	mzratiowidget = new QWidget();
	mzratiowidget->setLayout(mzratiolayout);
	mzratiowidget->setFixedWidth(leftdefaultwidth);
	mzratiolabel = new QLabel("m/z Ratio:");
	experimentalspectragridlayout->addWidget(mzratiolabel, 10, 0);
	experimentalspectragridlayout->addWidget(mzratiowidget, 10, 1);

	minimumrt = new QDoubleSpinBox();
	minimumrt->setToolTip("Enter the minimum retention time (0 = disabled). Units depend on your input file. Seconds or minutes can be used.");
	minimumrt->setDecimals(3);
	minimumrt->setRange(0, 100000);
	minimumrt->setSingleStep(1);
	minimumrt->setPrefix("minimum: ");

	maximumrt = new QDoubleSpinBox();
	maximumrt->setToolTip("Enter the maximum retention time (0 = disabled). Units depend on your input file. Seconds or minutes can be used.");
	maximumrt->setDecimals(3);
	maximumrt->setRange(0, 100000);
	maximumrt->setSingleStep(1);
	maximumrt->setPrefix("maximum: ");

	rtlayout = new QHBoxLayout();
	rtlayout->addWidget(minimumrt);
	rtlayout->addWidget(maximumrt);
	rtlayout->setMargin(0);
	rtwidget = new QWidget();
	rtwidget->setLayout(rtlayout);
	rtwidget->setFixedWidth(leftdefaultwidth);
	rtlabel = new QLabel("Retention Time:");
	experimentalspectragridlayout->addWidget(rtlabel, 11, 0);
	experimentalspectragridlayout->addWidget(rtwidget, 11, 1);

	fwhm = new QDoubleSpinBox();
	fwhm->setToolTip("Full width at half maximum. The value is used if the profile spectra are converted into peaklists (mzML and imzML) and if the full isotope patterns of compounds are generated.");
	fwhm->setDecimals(6);
	fwhm->setRange(0, 1);
	fwhm->setSingleStep(0.01);
	fwhm->setSuffix(" Da");
	fwhm->setFixedWidth(leftdefaultwidth);
	fwhmlabel = new QLabel("FWHM:");
	experimentalspectragridlayout->addWidget(fwhmlabel, 12, 0);
	experimentalspectragridlayout->addWidget(fwhm, 12, 1);

	minratio54Fe56Fe = new QDoubleSpinBox();
	minratio54Fe56Fe->setToolTip("Enter the minimum intensity ratio of 54Fe/56Fe.");
	minratio54Fe56Fe->setDecimals(3);
	minratio54Fe56Fe->setRange(0, 1);
	minratio54Fe56Fe->setSingleStep(0.01);
	minratio54Fe56Fe->setValue(0.01);
	minratio54Fe56Fe->setPrefix("minimum: ");

	maxratio54Fe56Fe = new QDoubleSpinBox();
	maxratio54Fe56Fe->setToolTip("Enter the maximum intensity ratio of 54Fe/56Fe.");
	maxratio54Fe56Fe->setDecimals(3);
	maxratio54Fe56Fe->setRange(0, 1);
	maxratio54Fe56Fe->setSingleStep(0.01);
	maxratio54Fe56Fe->setValue(0.1);
	maxratio54Fe56Fe->setPrefix("maximum: ");

	ratio54Fe56Felayout = new QHBoxLayout();
	ratio54Fe56Felayout->addWidget(minratio54Fe56Fe);
	ratio54Fe56Felayout->addWidget(maxratio54Fe56Fe);
	ratio54Fe56Felayout->setMargin(0);
	ratio54Fe56Fewidget = new QWidget();
	ratio54Fe56Fewidget->setLayout(ratio54Fe56Felayout);
	ratio54Fe56Fewidget->setFixedWidth(leftdefaultwidth);
	ratio54Fe56Felabel = new QLabel("54Fe/56Fe Ratio:");
	experimentalspectragridlayout->addWidget(ratio54Fe56Felabel, 13, 0);
	experimentalspectragridlayout->addWidget(ratio54Fe56Fewidget, 13, 1);

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

	blindedges = new QComboBox();
	blindedges->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	blindedges->setToolTip("The operation to be performed with edges forming incomplete paths:\nkeep (edges are kept - useful if you would like to see the whole de novo graph in 'View -> Graph');\nremove (edges are removed - speeds up the search);\nconnect (edges are connected - useful if you are looking for sequence tags).");
	blindedges->addItem(tr("keep (you can see a complete de novo graph)"));
	blindedges->addItem(tr("remove (speed up the search)"));
	blindedges->addItem(tr("connect (allow detection of sequence tags)"));
	blindedges->setFixedWidth(leftdefaultwidth);
	blindedgeslabel = new QLabel("Incomplete Paths in De Novo Graph:");
	brickdatabasegridlayout->addWidget(blindedgeslabel, 2, 0);
	brickdatabasegridlayout->addWidget(blindedges, 2, 1);

	maximumcumulativemass = new QDoubleSpinBox();
	maximumcumulativemass->setToolTip("Enter the maximum cumulative mass of combined blocks (0 = the maximum mass is unlimited). A small value speeds up the search and vice versa.");
	maximumcumulativemass->setDecimals(3);
	maximumcumulativemass->setRange(0, 10000);
	maximumcumulativemass->setSingleStep(1);
	maximumcumulativemass->setFixedWidth(leftdefaultwidth);
	maximumcumulativemasslabel = new QLabel("Maximum Cumulative Mass of Blocks:");
	brickdatabasegridlayout->addWidget(maximumcumulativemasslabel, 3, 0);
	brickdatabasegridlayout->addWidget(maximumcumulativemass, 3, 1);

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
	brickdatabasegridlayout->addWidget(modificationslabel, 4, 0);
	brickdatabasegridlayout->addWidget(modificationswidget, 4, 1);

	brickdatabasegroupbox = new QGroupBox("Database of Building Blocks");
	brickdatabasegroupbox->setLayout(brickdatabasegridlayout);


	miscgridlayout = new QGridLayout();

	similaritysearch = new QCheckBox();
	similaritysearch->setToolTip("Disable the filtering of compounds by precursor mass.");
	similaritysearch->setFixedWidth(checkboxfieldwidth);
	similaritysearchlabel = new QLabel("Disable Precursor Mass Filter:");
	miscgridlayout->addWidget(similaritysearchlabel, 0, 0);
	miscgridlayout->addWidget(similaritysearch, 0, 1);

	internalfragments = new QCheckBox();
	internalfragments->setToolTip("Generate internal fragment ions in theoretical spectra.\nThis feature is currently available only for branch-cyclic peptides.");
	internalfragments->setFixedWidth(checkboxfieldwidth);
	internalfragmentslabel = new QLabel("Internal Fragments:");
	miscgridlayout->addWidget(internalfragmentslabel, 1, 0);
	miscgridlayout->addWidget(internalfragments, 1, 1);

	enablescrambling = new QCheckBox();
	enablescrambling->setToolTip("Generate scrambled fragment ions of cyclic peptides in theoretical spectra.");
	enablescrambling->setFixedWidth(checkboxfieldwidth);
	enablescramblinglabel = new QLabel("Enable Scrambling:");
	miscgridlayout->addWidget(enablescramblinglabel, 2, 0);
	miscgridlayout->addWidget(enablescrambling, 2, 1);

	cyclicnterminus = new QCheckBox();
	cyclicnterminus->setToolTip("The water molecule is subtracted from all theoretical N-terminal fragment ions and the theoretical precursor mass.\nThis feature is useful if a linear peptide includes a small cycle close to the N-terminus.\nIf the linear polyketide is selected as the peptide type, the water molecule is subtracted only from the precursor ion.");
	cyclicnterminus->setFixedWidth(checkboxfieldwidth);
	cyclicnterminuslabel = new QLabel("Cyclic N-terminus:");
	miscgridlayout->addWidget(cyclicnterminuslabel, 0, 2);
	miscgridlayout->addWidget(cyclicnterminus, 0, 3);

	cycliccterminus = new QCheckBox();
	cycliccterminus->setToolTip("The water molecule is subtracted from all theoretical C-terminal fragment ions and the theoretical precursor mass.\nThis feature is useful if a linear peptide includes a small cycle close to the C-terminus.\nIf the linear polyketide is selected as the peptide type, the water molecule is subtracted only from the precursor ion.");
	cycliccterminus->setFixedWidth(checkboxfieldwidth);
	cycliccterminuslabel = new QLabel("Cyclic C-terminus:");
	miscgridlayout->addWidget(cycliccterminuslabel, 1, 2);
	miscgridlayout->addWidget(cycliccterminus, 1, 3);

	regularblocksorder = new QCheckBox();
	regularblocksorder->setToolTip("Keep only polyketide sequence candidates whose ketide building blocks are in the regular order [water eliminating block]-[2H eliminating block]-[water eliminating block]-[2H eliminating block], etc.");
	regularblocksorder->setFixedWidth(checkboxfieldwidth);
	regularblocksorderlabel = new QLabel("Regular Order of Ketide Blocks:");
	miscgridlayout->addWidget(regularblocksorderlabel, 2, 2);
	miscgridlayout->addWidget(regularblocksorder, 2, 3);

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
	scoretype->addItem(tr("Weighted Ratio of Matched Peaks"));
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

	neutrallosstypes = new cNeutralLossesListWidget(true, true, this);
	neutrallosstypes->setToolTip("Define and select the types of neutral losses.");
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

	elementstypes = new cNeutralLossesListWidget(false, true, this);
	elementstypes->setToolTip("Define and select chemical elements.");
	elementstypes->setFixedWidth(rightdefaultwidth);
	elementstypeslabel = new QLabel("Chemical Elements:");
	theoreticalspectragridlayout->addWidget(elementstypeslabel, 7, 0);
	theoreticalspectragridlayout->addWidget(elementstypes, 7, 1);

	maximumcombinedelements = new QSpinBox();
	maximumcombinedelements->setToolTip("Maximum number of combined chemical elements.");
	maximumcombinedelements->setRange(0, 10000);
	maximumcombinedelements->setSingleStep(1);
	maximumcombinedelements->setFixedWidth(rightdefaultwidth);
	maximumcombinedelementslabel = new QLabel("Maximum Number of Combined Elements:");
	theoreticalspectragridlayout->addWidget(maximumcombinedelementslabel, 8, 0);
	theoreticalspectragridlayout->addWidget(maximumcombinedelements, 8, 1);

	//clearhitswithoutparent = new QCheckBox();
	//clearhitswithoutparent->setToolTip("If checked, a peak is not matched if the corresponding parent peak is not matched (e.g., a dehydrated b-ion is not matched if corresponding b-ion is not matched).");
	//theoreticalspectraformlayout->addRow(tr("Remove Hits of Fragments without Hits of Parent Fragments: "), clearhitswithoutparent);

	reportunmatchedtheoreticalpeaks = new QCheckBox();
	reportunmatchedtheoreticalpeaks->setToolTip("If checked, all unmatched theoretical peaks are reported.\nIf unchecked, unmatched theoretical peaks are reported only if a corresponding isotope pattern has been matched.\nThis feature may spend a lot of main memory, keep it disabled if possible.");
	reportunmatchedtheoreticalpeaks->setFixedWidth(rightdefaultwidth);
	reportunmatchedtheoreticalpeakslabel = new QLabel("Report Unmatched Theoretical Peaks:");
	theoreticalspectragridlayout->addWidget(reportunmatchedtheoreticalpeakslabel, 9, 0);
	theoreticalspectragridlayout->addWidget(reportunmatchedtheoreticalpeaks, 9, 1);

	generateisotopepattern = new QCheckBox();
	generateisotopepattern->setChecked(true);
	generateisotopepattern->setToolTip("The full isotope patterns of compounds are generated in theoretical spectra.\nThe FWHM value is used for this purpose. If checked, the deisotoping is disabled automatically.");
	generateisotopepattern->setFixedWidth(rightdefaultwidth);
	generateisotopepatternlabel = new QLabel("Generate Full Isotope Patterns:");
	theoreticalspectragridlayout->addWidget(generateisotopepatternlabel, 10, 0);
	theoreticalspectragridlayout->addWidget(generateisotopepattern, 10, 1);

	minimumpatternsize = new QSpinBox();
	minimumpatternsize->setToolTip("The minimum number of peaks which must be annotated in an isotopic pattern. The option \"Generate Full Isotope Patterns\" must be enabled.");
	minimumpatternsize->setRange(1, 100);
	minimumpatternsize->setSingleStep(1);
	minimumpatternsize->setFixedWidth(rightdefaultwidth);
	minimumpatternsizelabel = new QLabel("Minimum Number of Isotopic Peaks:");
	theoreticalspectragridlayout->addWidget(minimumpatternsizelabel, 11, 0);
	theoreticalspectragridlayout->addWidget(minimumpatternsize, 11, 1);

	minimumfeaturesize = new QSpinBox();
	minimumfeaturesize->setToolTip("The minimum number of spectra in which a compound must be identified to be reported.\nLC-MS data = the minimum number of consecutive scans;\nMSI data = the minimum number of pixels.");
	minimumfeaturesize->setRange(1, 10000);
	minimumfeaturesize->setSingleStep(1);
	minimumfeaturesize->setFixedWidth(rightdefaultwidth);
	minimumfeaturesizelabel = new QLabel("Minimum Number of Spectra:");
	theoreticalspectragridlayout->addWidget(minimumfeaturesizelabel, 12, 0);
	theoreticalspectragridlayout->addWidget(minimumfeaturesize, 12, 1);

	minimumiontypes = new QSpinBox();
	minimumiontypes->setToolTip("The minimum number of ion types which must be matched to report a given compound.");
	minimumiontypes->setRange(1, 100);
	minimumiontypes->setSingleStep(1);
	minimumiontypes->setFixedWidth(rightdefaultwidth);
	minimumiontypeslabel = new QLabel("Minimum Number of Ion Types:");
	theoreticalspectragridlayout->addWidget(minimumiontypeslabel, 13, 0);
	theoreticalspectragridlayout->addWidget(minimumiontypes, 13, 1);

	basicformulacheck = new QCheckBox();
	basicformulacheck->setToolTip("Apply Senior's filtering rules.");
	basicformulacheck->setFixedWidth(rightdefaultwidth);
	basicformulachecklabel = new QLabel("Basic Formula Check:");
	theoreticalspectragridlayout->addWidget(basicformulachecklabel, 14, 0);
	theoreticalspectragridlayout->addWidget(basicformulacheck, 14, 1);

	advancedformulacheck = new QCheckBox();
	advancedformulacheck->setToolTip("Apply advanced filtering rules if molecular formulas of compounds are generated.");
	advancedformulacheck->setFixedWidth(rightdefaultwidth);
	advancedformulachecklabel = new QLabel("Advanced Formula Check:");
	theoreticalspectragridlayout->addWidget(advancedformulachecklabel, 15, 0);
	theoreticalspectragridlayout->addWidget(advancedformulacheck, 15, 1);

	noratiocheck = new QCheckBox();
	noratiocheck->setToolTip("Check if the number of nitrogen atoms is less or equal to the number of oxygen atoms.");
	noratiocheck->setFixedWidth(rightdefaultwidth);
	noratiochecklabel = new QLabel("N/O Ratio Check:");
	theoreticalspectragridlayout->addWidget(noratiochecklabel, 16, 0);
	theoreticalspectragridlayout->addWidget(noratiocheck, 16, 1);

	calculatefdrs = new QCheckBox();
	calculatefdrs->setToolTip("Calculate false discovery rates (experimental feature).");
	calculatefdrs->setFixedWidth(rightdefaultwidth);
	calculatefdrslabel = new QLabel("Calculate FDRs:");
	theoreticalspectragridlayout->addWidget(calculatefdrslabel, 17, 0);
	theoreticalspectragridlayout->addWidget(calculatefdrs, 17, 1);

	minimumannotationintensityrelative = new QDoubleSpinBox();
	minimumannotationintensityrelative->setToolTip("Enter the minimum relative intensity of the most intense peak in an isotopic pattern. Isotopic patterns with relative intensities below this value will be kept in the spectrum but not annotated.");
	minimumannotationintensityrelative->setDecimals(3);
	minimumannotationintensityrelative->setRange(0, 100);
	minimumannotationintensityrelative->setSingleStep(1);
	minimumannotationintensityrelative->setPrefix("relative: ");
	minimumannotationintensityrelative->setSuffix(" %");

	minimumannotationintensityabsolute = new QSpinBox();
	minimumannotationintensityabsolute->setToolTip("Enter the minimum absolute intensity of the most intense peak in an isotopic pattern. Isotopic patterns with absolute intensities below this value will be kept in the spectrum but not annotated.");
	minimumannotationintensityabsolute->setRange(0, INT32_MAX);
	minimumannotationintensityabsolute->setSingleStep(100);
	minimumannotationintensityabsolute->setPrefix("absolute: ");

	minimumannotationintensitylayout = new QHBoxLayout();
	minimumannotationintensitylayout->addWidget(minimumannotationintensityrelative);
	minimumannotationintensitylayout->addWidget(minimumannotationintensityabsolute);
	minimumannotationintensitylayout->setMargin(0);
	minimumannotationintensitywidget = new QWidget();
	minimumannotationintensitywidget->setLayout(minimumannotationintensitylayout);
	minimumannotationintensitywidget->setFixedWidth(rightdefaultwidth);
	minimumannotationintensitylabel = new QLabel("Minimum Intensity of Highest Peak in Isotopic Pattern:");
	theoreticalspectragridlayout->addWidget(minimumannotationintensitylabel, 18, 0);
	theoreticalspectragridlayout->addWidget(minimumannotationintensitywidget, 18, 1);

	mzdifftolerance = new QDoubleSpinBox();
	mzdifftolerance->setToolTip("The maximum m/z error of difference between an isotopic peak and the most intense peak in an experimental and theoretical isotopic pattern (0 = disabled) [ppm].");
	mzdifftolerance->setDecimals(3);
	mzdifftolerance->setRange(0, 10000);
	mzdifftolerance->setSingleStep(1);
	mzdifftolerance->setSuffix(" ppm");
	mzdifftolerance->setFixedWidth(rightdefaultwidth);
	mzdifftolerancelabel = new QLabel("Isotope m/z Tolerance:");
	theoreticalspectragridlayout->addWidget(mzdifftolerancelabel, 19, 0);
	theoreticalspectragridlayout->addWidget(mzdifftolerance, 19, 1);

	intensitytolerance = new QDoubleSpinBox();
	intensitytolerance->setToolTip("The maximum error tolerance of intensities of matched isotopes (0 = disabled) [in % of relative intensity of the most intense peak].\nExample:\nIsotope Intensity Tolerance = 10%, Relative Intensity of the Most Intense Peak = 100% => the tolerance of relative intensities of isotopes is 10%;\nIsotope Intensity Tolerance = 10%, Relative Intensity of the Most Intense Peak = 50% => the tolerance of relative intensities of isotopes is 5%; etc.");
	intensitytolerance->setDecimals(3);
	intensitytolerance->setRange(0, 100);
	intensitytolerance->setSingleStep(1);
	intensitytolerance->setSuffix(" %");
	intensitytolerance->setFixedWidth(rightdefaultwidth);
	intensitytolerancelabel = new QLabel("Isotope Intensity Tolerance:");
	theoreticalspectragridlayout->addWidget(intensitytolerancelabel, 20, 0);
	theoreticalspectragridlayout->addWidget(intensitytolerance, 20, 1);

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
	searchedsequenceNtermmodif->setPlaceholderText("N-terminal");

	searchedsequenceCtermmodif = new QLineEdit();
	searchedsequenceCtermmodif->setToolTip("A name of a C-terminal modification which belongs to the searched peptide. The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceCtermmodif->setPlaceholderText("C-terminal");

	searchedsequenceTmodif = new QLineEdit();
	searchedsequenceTmodif->setToolTip("A name of an N-terminal or C-terminal modification which belongs to a branch of a searched peptide (branched and branch-cyclic peptides only). The name must be defined in \"N-/C-terminal Modifications File\".");
	searchedsequenceTmodif->setPlaceholderText("Branch");

	searchedsequencemodiflayout = new QHBoxLayout();
	searchedsequencemodiflayout->addWidget(searchedsequenceNtermmodif);
	searchedsequencemodiflayout->addWidget(searchedsequenceCtermmodif);
	searchedsequencemodiflayout->addWidget(searchedsequenceTmodif);
	searchedsequencemodiflayout->setMargin(0);
	searchedsequencemodifwidget = new QWidget();
	searchedsequencemodifwidget->setLayout(searchedsequencemodiflayout);
	searchedsequencemodifwidget->setFixedWidth(searchedsequencedefaultwidth);
	searchedsequencemodiflabel = new QLabel("Modifications:");
	searchedsequencegridlayout->addWidget(searchedsequencemodiflabel, 1, 0);
	searchedsequencegridlayout->addWidget(searchedsequencemodifwidget, 1, 1);

	searchedsequenceformula = new QLineEdit();
	searchedsequenceformula->setToolTip("Formula of searched sequence.");
	searchedsequenceformula->setFixedWidth(searchedsequencedefaultwidth);
	searchedsequenceformulalabel = new QLabel("Formula:");
	searchedsequencegridlayout->addWidget(searchedsequenceformulalabel, 2, 0);
	searchedsequencegridlayout->addWidget(searchedsequenceformula, 2, 1);

	searchedsequencegroupbox = new QGroupBox("Searched Sequence/Compound");
	searchedsequencegroupbox->setLayout(searchedsequencegridlayout);


	vlayout1 = new QVBoxLayout();
	vlayout1->addWidget(searchgroupbox);
	vlayout1->addWidget(experimentalspectragroupbox);
	vlayout1->addWidget(brickdatabasegroupbox);
	vlayout1->addWidget(miscgroupbox);
	if (hideunusedparameters) {
		vlayout1->addStretch(1);
	}

	vlayout2 = new QVBoxLayout();
	vlayout2->addWidget(theoreticalspectragroupbox);
	vlayout2->addWidget(searchedsequencegroupbox);
	if (hideunusedparameters) {
		vlayout2->addStretch(1);
	}

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

	resize(defaultwinsizex, defaultwinsizey);

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

	applyGlobalPreferences(globalpreferences);

	mode->setCurrentIndex(dereplication);
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
	delete mzratiolabel;
	delete minimummz;
	delete maximummz;
	delete mzratiolayout;
	delete mzratiowidget;
	delete rtlabel;
	delete minimumrt;
	delete maximumrt;
	delete rtlayout;
	delete rtwidget;
	delete fwhmlabel;
	delete fwhm;
	delete ratio54Fe56Felabel;
	delete minratio54Fe56Fe;
	delete maxratio54Fe56Fe;
	delete ratio54Fe56Felayout;
	delete ratio54Fe56Fewidget;
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
	delete blindedgeslabel;
	delete blindedges;
	delete maximumcumulativemasslabel;
	delete maximumcumulativemass;
	delete modificationslabel;
	delete modificationsline;
	delete modificationsbutton;
	delete modificationslayout;
	delete modificationswidget;
	delete brickdatabasegridlayout;
	delete brickdatabasegroupbox;
	
	delete cyclicnterminuslabel;
	delete cyclicnterminus;
	delete cycliccterminuslabel;
	delete cycliccterminus;
	delete internalfragmentslabel;
	delete internalfragments;
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
	delete elementstypeslabel;
	delete elementstypes;
	delete maximumcombinedelementslabel;
	delete maximumcombinedelements;
	//delete clearhitswithoutparent;
	delete reportunmatchedtheoreticalpeakslabel;
	delete reportunmatchedtheoreticalpeaks;
	delete generateisotopepatternlabel;
	delete generateisotopepattern;
	delete minimumpatternsizelabel;
	delete minimumpatternsize;
	delete minimumfeaturesizelabel;
	delete minimumfeaturesize;
	delete minimumiontypeslabel;
	delete minimumiontypes;
	delete basicformulachecklabel;
	delete basicformulacheck;
	delete advancedformulachecklabel;
	delete advancedformulacheck;
	delete noratiochecklabel;
	delete noratiocheck;
	delete calculatefdrslabel;
	delete calculatefdrs;
	delete minimumannotationintensitylabel;
	delete minimumannotationintensityrelative;
	delete minimumannotationintensityabsolute;
	delete minimumannotationintensitylayout;
	delete minimumannotationintensitywidget;
	delete mzdifftolerancelabel;
	delete mzdifftolerance;
	delete intensitytolerancelabel;
	delete intensitytolerance;
	delete theoreticalspectragridlayout;
	delete theoreticalspectragroupbox;

	delete searchedsequencelabel;
	delete searchedsequenceline;
	delete searchedsequencebutton;
	delete searchedsequencelayout;
	delete searchedsequencewidget;
	delete searchedsequencemodiflabel;
	delete searchedsequenceNtermmodif;
	delete searchedsequenceCtermmodif;
	delete searchedsequenceTmodif;
	delete searchedsequencemodiflayout;
	delete searchedsequencemodifwidget;
	delete searchedsequenceformulalabel;
	delete searchedsequenceformula;
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
	this->parameters.peaklistseriesvector.clear();
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
	if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
		mode->setCurrentIndex(denovoengine);
	}
	peptidetype->setCurrentIndex(peptidetypeindex);
	searchedsequenceline->setText(sequence);
}


void cParametersWidget::setTag(int peptidetypeindex, QString tag) {
	if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
		mode->setCurrentIndex(denovoengine);
	}
	peptidetype->setCurrentIndex(peptidetypeindex);
	sequencetag->setText(tag);
}


void cParametersWidget::applyGlobalPreferences(cGlobalPreferences* globalpreferences) {
	if (globalpreferences) {
		if (lastdirloadsettings.right(4).compare(".ini", Qt::CaseInsensitive) != 0) {
			lastdirloadsettings = globalpreferences->settingsdefaultdir;
		}
		if (lastdirsavesettings.right(4).compare(".ini", Qt::CaseInsensitive) != 0) {
			lastdirsavesettings = globalpreferences->settingsdefaultdir;
		}

		defaultdirselectpeaklist = globalpreferences->peaklistsdefaultdir;
		defaultdirselectbricksdatabase = globalpreferences->blocksdefaultdir;
		defaultdirselectmodifications = globalpreferences->modificationsdefaultdir;
		defaultdirselectsequencedatabase = globalpreferences->sequencesdefaultdir;

		if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
			resetFragmentIonTypes();
		}
	}
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
		if (mode->currentIndex() == (int)compoundsearch) {
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

		mode->setCurrentIndex(settings.value("mode", 3).toInt());
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
		maximummz->setValue(settings.value("maximummz", 0).toDouble());
		minimumrt->setValue(settings.value("minimumrt", 0).toDouble());
		maximumrt->setValue(settings.value("maximumrt", 0).toDouble());
		fwhm->setValue(settings.value("fwhm", 0.05).toDouble());
		minratio54Fe56Fe->setValue(settings.value("minratio54Fe56Fe", 0.01).toDouble());
		maxratio54Fe56Fe->setValue(settings.value("maxratio54Fe56Fe", 0.1).toDouble());

		brickdatabaseline->setText(settings.value("brickdatabase", "").toString());
		maximumbricksincombinationbegin->setValue(settings.value("maximumbricksincombinationbegin", 1).toInt());
		maximumbricksincombinationmiddle->setValue(settings.value("maximumbricksincombinationmiddle", 1).toInt());
		maximumbricksincombinationend->setValue(settings.value("maximumbricksincombinationend", 1).toInt());
		maximumcumulativemass->setValue(settings.value("maximumcumulativemass", 0).toDouble());
		modificationsline->setText(settings.value("modificationsfile", "").toString());

		blindedges->setCurrentIndex(settings.value("blindedges", 2).toInt());
		settings.value("cyclicnterminus", 0).toInt() == 0 ? cyclicnterminus->setChecked(false) : cyclicnterminus->setChecked(true);
		settings.value("cycliccterminus", 0).toInt() == 0 ? cycliccterminus->setChecked(false) : cycliccterminus->setChecked(true);
		settings.value("internalfragments", 0).toInt() == 0 ? internalfragments->setChecked(false) : internalfragments->setChecked(true);
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
		
		if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
			for (i = 0; i < iontypes->getList()->count(); i++) {
				iontypes->getList()->item(i)->setSelected(false);
			}

			i = 0;
			qloadstring = ("iontype_" + to_string(i)).c_str();
			while (settings.value(qloadstring, "XXX").toString().compare("XXX") != 0) {
				for (int j = 0; j < iontypes->getList()->count(); j++) {
					if (iontypes->getList()->item(j)->text().compare(settings.value(qloadstring).toString()) == 0) {
						iontypes->getList()->item(j)->setSelected(true);
						break;
					}
				}

				i++;
				qloadstring = ("iontype_" + to_string(i)).c_str();
			}
		}
		else {
			for (i = 0; i < iontypes->getList()->count(); i++) {
				qloadstring = ("fragmentiontype_" + to_string(i)).c_str();
				settings.value(qloadstring, 0).toInt() == 0 ? iontypes->getList()->item(i)->setSelected(false) : iontypes->getList()->item(i)->setSelected(true);
			}
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

		elementstypes->getList()->clear();
		i = 0;
		qloadstring = ("elementsformula_" + to_string(i)).c_str();
		while (settings.value(qloadstring, "XXX").toString().compare("XXX") != 0) {
			elementstypes->addItem(settings.value(qloadstring).toString());

			qloadstring = ("elementstype_" + to_string(i)).c_str();
			settings.value(qloadstring, 0).toInt() == 0 ? elementstypes->getList()->item(i)->setSelected(false) : elementstypes->getList()->item(i)->setSelected(true);

			i++;
			qloadstring = ("elementsformula_" + to_string(i)).c_str();
		}

		maximumcombinedelements->setValue(settings.value("maximumcombinedelements", 200).toInt());

		if ((eModeType)mode->currentIndex() == compoundsearch) {
			if (!settings.contains("maximumcombinedelements")) {
				elementstypes->getList()->clear();
				for (int i = 0; i < neutrallosstypes->getList()->count(); i++) {
					elementstypes->getList()->addItem(neutrallosstypes->getList()->item(i)->text());
					if (neutrallosstypes->getList()->item(i)->isSelected()) {
						elementstypes->getList()->item(i)->setSelected(true);
					}
				}

				maximumcombinedelements->setValue(maximumcombinedlosses->value());

				neutrallosstypes->getList()->clear();
				maximumcombinedlosses->setValue(0);
			}
		}

		//settings.value("clearhitswithoutparent", 0).toInt() == 0 ? clearhitswithoutparent->setChecked(false) : clearhitswithoutparent->setChecked(true);
		settings.value("reportunmatchedtheoreticalpeaks", 0).toInt() == 0 ? reportunmatchedtheoreticalpeaks->setChecked(false) : reportunmatchedtheoreticalpeaks->setChecked(true);
		settings.value("generateisotopepattern", 0).toInt() == 0 ? generateisotopepattern->setChecked(false) : generateisotopepattern->setChecked(true);
		minimumpatternsize->setValue(settings.value("minimumpatternsize", 1).toInt());
		minimumfeaturesize->setValue(settings.value("minimumfeaturesize", 1).toInt());
		minimumiontypes->setValue(settings.value("minimumiontypes", 1).toInt());
		settings.value("basicformulacheck", 1).toInt() == 0 ? basicformulacheck->setChecked(false) : basicformulacheck->setChecked(true);
		settings.value("advancedformulacheck", 1).toInt() == 0 ? advancedformulacheck->setChecked(false) : advancedformulacheck->setChecked(true);
		settings.value("noratiocheck", 1).toInt() == 0 ? noratiocheck->setChecked(false) : noratiocheck->setChecked(true);
		settings.value("calculatefdrs", 1).toInt() == 0 ? calculatefdrs->setChecked(false) : calculatefdrs->setChecked(true);

		minimumannotationintensityrelative->setValue(settings.value("minimumannotationintensityrelative", 0).toDouble());
		minimumannotationintensityabsolute->setValue(settings.value("minimumannotationintensityabsolute", 0).toUInt());

		mzdifftolerance->setValue(settings.value("mzdifftolerance", 0).toDouble());
		intensitytolerance->setValue(settings.value("intensitytolerance", 0).toDouble());

		searchedsequenceline->setText(settings.value("searchedsequence", "").toString());
		searchedsequenceNtermmodif->setText(settings.value("searchedsequenceNtermmodif", "").toString());
		searchedsequenceCtermmodif->setText(settings.value("searchedsequenceCtermmodif", "").toString());
		searchedsequenceTmodif->setText(settings.value("searchedsequenceTmodif", "").toString());
		searchedsequenceformula->setText(settings.value("searchedsequenceformula", "").toString());
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
	settings.setValue("maximummz", maximummz->value());
	settings.setValue("minimumrt", minimumrt->value());
	settings.setValue("maximumrt", maximumrt->value());
	settings.setValue("fwhm", fwhm->value());
	settings.setValue("minratio54Fe56Fe", minratio54Fe56Fe->value());
	settings.setValue("maxratio54Fe56Fe", maxratio54Fe56Fe->value());

	settings.setValue("brickdatabase", brickdatabaseline->text());
	settings.setValue("maximumbricksincombinationbegin", maximumbricksincombinationbegin->value());
	settings.setValue("maximumbricksincombinationmiddle", maximumbricksincombinationmiddle->value());
	settings.setValue("maximumbricksincombinationend", maximumbricksincombinationend->value());
	settings.setValue("maximumcumulativemass", maximumcumulativemass->value());
	settings.setValue("modificationsfile", modificationsline->text());

	settings.setValue("blindedges", blindedges->currentIndex());
	cyclicnterminus->isChecked() ? settings.setValue("cyclicnterminus", 1) : settings.setValue("cyclicnterminus", 0);
	cycliccterminus->isChecked() ? settings.setValue("cycliccterminus", 1) : settings.setValue("cycliccterminus", 0);
	internalfragments->isChecked() ? settings.setValue("internalfragments", 1) : settings.setValue("internalfragments", 0);
	enablescrambling->isChecked() ? settings.setValue("enablescrambling", 1) : settings.setValue("enablescrambling", 0);
	similaritysearch->isChecked() ? settings.setValue("similaritysearch", 1) : settings.setValue("similaritysearch", 0);
	regularblocksorder->isChecked() ? settings.setValue("regularblocksorder", 1) : settings.setValue("regularblocksorder", 0);

	settings.setValue("sequencedatabase", sequencedatabaseline->text());
	settings.setValue("scoretype", scoretype->currentIndex());
	settings.setValue("hitsreported", hitsreported->value());
	settings.setValue("sequencetag", sequencetag->text());

	if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
		int cnt = 0;
		for (int i = 0; i < iontypes->getList()->count(); i++) {
			if (iontypes->getList()->item(i)->isSelected()) {
				qsavestring = ("iontype_" + to_string(cnt)).c_str();
				settings.setValue(qsavestring, iontypes->getList()->item(i)->text());
				cnt++;
			}
		}
	}
	else {
		for (int i = 0; i < iontypes->getList()->count(); i++) {
			qsavestring = ("fragmentiontype_" + to_string(i)).c_str();
			iontypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);
		}
	}

	for (int i = 0; i < neutrallosstypes->getList()->count(); i++) {
		qsavestring = ("neutrallosstype_" + to_string(i)).c_str();
		neutrallosstypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

		qsavestring = ("neutrallossformula_" + to_string(i)).c_str();
		settings.setValue(qsavestring, neutrallosstypes->getList()->item(i)->text());
	}

	settings.setValue("maximumcombinedlosses", maximumcombinedlosses->value());

	for (int i = 0; i < elementstypes->getList()->count(); i++) {
		qsavestring = ("elementstype_" + to_string(i)).c_str();
		elementstypes->getList()->item(i)->isSelected() ? settings.setValue(qsavestring, 1) : settings.setValue(qsavestring, 0);

		qsavestring = ("elementsformula_" + to_string(i)).c_str();
		settings.setValue(qsavestring, elementstypes->getList()->item(i)->text());
	}

	settings.setValue("maximumcombinedelements", maximumcombinedelements->value());

	//clearhitswithoutparent->isChecked() ? settings.setValue("clearhitswithoutparent", 1) : settings.setValue("clearhitswithoutparent", 0);
	reportunmatchedtheoreticalpeaks->isChecked() ? settings.setValue("reportunmatchedtheoreticalpeaks", 1) : settings.setValue("reportunmatchedtheoreticalpeaks", 0);
	generateisotopepattern->isChecked() ? settings.setValue("generateisotopepattern", 1) : settings.setValue("generateisotopepattern", 0);
	settings.setValue("minimumpatternsize", minimumpatternsize->value());
	settings.setValue("minimumfeaturesize", minimumfeaturesize->value());
	settings.setValue("minimumiontypes", minimumiontypes->value());
	basicformulacheck->isChecked() ? settings.setValue("basicformulacheck", 1) : settings.setValue("basicformulacheck", 0);
	advancedformulacheck->isChecked() ? settings.setValue("advancedformulacheck", 1) : settings.setValue("advancedformulacheck", 0);
	noratiocheck->isChecked() ? settings.setValue("noratiocheck", 1) : settings.setValue("noratiocheck", 0);
	calculatefdrs->isChecked() ? settings.setValue("calculatefdrs", 1) : settings.setValue("calculatefdrs", 0);

	settings.setValue("minimumannotationintensityrelative", minimumannotationintensityrelative->value());
	settings.setValue("minimumannotationintensityabsolute", minimumannotationintensityabsolute->value());

	settings.setValue("mzdifftolerance", mzdifftolerance->value());
	settings.setValue("intensitytolerance", intensitytolerance->value());

	settings.setValue("searchedsequence", searchedsequenceline->text());
	settings.setValue("searchedsequenceNtermmodif", searchedsequenceNtermmodif->text());
	settings.setValue("searchedsequenceCtermmodif", searchedsequenceCtermmodif->text());
	settings.setValue("searchedsequenceTmodif", searchedsequenceTmodif->text());
	settings.setValue("searchedsequenceformula", searchedsequenceformula->text());

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
	QString currentdir = peaklistline->text();

	size_t pos = currentdir.toStdString().find(';');
	if (pos != string::npos) {
		currentdir = currentdir.toStdString().substr(0, pos).c_str();
	}

	if (!checkFile(currentdir.toStdString())) {
		currentdir = defaultdirselectpeaklist;
	}

	#if OS_TYPE != WIN
		QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select Peaklist(s)..."), currentdir, tr("Peaklists (*.txt *.mgf *.mzML *.mzXML *.imzML)"));
	#else
		QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Select Peaklist(s)..."), currentdir, tr("Peaklists (*.txt *.mgf *.mzML *.mzXML *.imzML *.baf *.raw *.dat *.mis ser)"));
	#endif

	if (!filenames.isEmpty()) {
		peaklistline->setText(filenames.join(";"));
	}
}


void cParametersWidget::brickDatabaseButtonReleased() {
	QString currentdir = brickdatabaseline->text();
	if (!checkFile(currentdir.toStdString())) {
		currentdir = defaultdirselectbricksdatabase;
	}

	QString filename = QFileDialog::getOpenFileName(this, tr("Select Building Blocks Database..."), currentdir, tr("Text Files (*.txt)"));
	
	if (!filename.isEmpty()) {
		brickdatabaseline->setText(filename);
	}
}


void cParametersWidget::modificationsButtonReleased() {
	QString currentdir = modificationsline->text();
	if (!checkFile(currentdir.toStdString())) {
		currentdir = defaultdirselectmodifications;
	}

	QString filename = QFileDialog::getOpenFileName(this, tr("Select Modifications File..."), currentdir, tr("Text Files (*.txt)"));
	
	if (!filename.isEmpty()) {
		modificationsline->setText(filename);
	}
}


void cParametersWidget::sequenceDatabaseButtonReleased() {
	QString currentdir = sequencedatabaseline->text();
	if (!checkFile(currentdir.toStdString())) {
		currentdir = defaultdirselectsequencedatabase;
	}

	QString filename = QFileDialog::getOpenFileName(this, tr("Select Sequence Database..."), currentdir, tr("Text Files (*.txt)"));

	if (!filename.isEmpty()) {
		sequencedatabaseline->setText(filename);
	}
}


bool cParametersWidget::updateParameters() {
	QMessageBox msgBox;
	QString errstr;

	cSummaryFormula tmpformula;
	string tmpstring;
	string errmsg;

	QStringList multiplefiles = peaklistline->text().split(";");

	if ((peaklistline->text().toStdString().compare("") == 0) || (multiplefiles.size() == 0)) {
		errstr = "A peaklist must be specified !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	// C++17 begin
	#if OS_TYPE == WIN
		QStringList tmpmultiplefiles;

		regex tmprx;
		string tmppath;

		for (auto& it : multiplefiles) {
			QFileInfo fileinfo(it);
			if (fileinfo.exists()) {
				if (fileinfo.isDir()) {
					for (auto& p : filesystem::recursive_directory_iterator(it.toStdString())) {
						tmppath = filesystem::absolute(p.path()).string();
						replace(tmppath.begin(), tmppath.end(), '\\', '/');

						tmprx = "\\.[bB][aA][fF]$";
						if (regex_search(tmppath, tmprx)) {
							tmpmultiplefiles.push_back(tmppath.c_str());
						}

						tmprx = "PEAKS.[rR][aA][wW]\\/\\_[fF][uU][nN][cC]001.[dD][aA][tT]$";
						if (!regex_search(tmppath, tmprx)) {
							tmprx = "\\_[fF][uU][nN][cC]001.[dD][aA][tT]$";
							if (regex_search(tmppath, tmprx)) {
								tmpmultiplefiles.push_back(tmppath.c_str());
							}
						}
					}
				}
				else {
					tmpmultiplefiles.push_back(it);
				}
			}
			else {
				errstr = "The path '" + it + "' does not exist !";
				msgBox.setText(errstr);
				msgBox.exec();
				return false;
			}
		}

		multiplefiles = tmpmultiplefiles;
	#endif
	// C++17 end

	int msi = 0;
	regex rx;
	for (auto& it : multiplefiles) {
		#if OS_TYPE == WIN
			rx = "\\.[mM][iI][sS]$";
			// flexImaging File
			if (regex_search(it.toStdString(), rx)) {
				msi++;
			}
		#endif

		rx = "\\.[iI][mM][zZ][mM][lL]$";
		// imzML File
		if (regex_search(it.toStdString(), rx)) {
			msi++;
		}
	}

	if ((msi == multiplefiles.size()) && (multiplefiles.size() > 1)) {
		errstr = "Please, select only one mass spectrometry imaging input file !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((msi > 0) && (msi < multiplefiles.size()) && (multiplefiles.size() > 1)) {
		errstr = "Please, do not mix conventional/LC-MS input files with mass spectrometry imaging files !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if (((eModeType)mode->currentIndex() == denovoengine) || ((eModeType)mode->currentIndex() == databasesearch) || ((eModeType)mode->currentIndex() == singlecomparison)) {
		if (multiplefiles.size() > 1) {
			errstr = "Multiple files cannot be selected in this mode !";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}

	if ((brickdatabaseline->text().toStdString().compare("") == 0) && (((eModeType)mode->currentIndex() == denovoengine) || (((eModeType)mode->currentIndex() == databasesearch) && ((ePeptideType)peptidetype->currentIndex() != other)) || (((eModeType)mode->currentIndex() == singlecomparison) && ((ePeptideType)peptidetype->currentIndex() != other)))) {
		errstr = "A database of building blocks must be specified !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if ((sequencedatabaseline->text().toStdString().compare("") == 0) && (((eModeType)mode->currentIndex() == databasesearch) || ((eModeType)mode->currentIndex() == dereplication))) {
		errstr = "A sequence database must be specified !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if (((eModeType)mode->currentIndex() == databasesearch) || ((eModeType)mode->currentIndex() == dereplication)) {
		int notselected = 0;

		for (int i = 0; i < iontypes->getList()->count(); i++) {
			if (iontypes->getList()->item(i)->isSelected()) {
				break;
			}
			else {
				notselected++;
			}
		}

		if (iontypes->getList()->count() == notselected) {
			errstr = "A ion type must be selected (e.g., [M+H]+) !";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}

	if ((precursormass->value() == 0) && (((eModeType)mode->currentIndex() == denovoengine) || ((eModeType)mode->currentIndex() == singlecomparison) || ((eModeType)mode->currentIndex() == databasesearch))) {
		errstr = "The precursor m/z ratio cannot be zero !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if (precursorcharge->value() == 0) {
		errstr = "The charge cannot be zero !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if (((eModeType)mode->currentIndex() == compoundsearch) && (maximummz->value() <= minimummz->value())) {
		errstr = "The maximum m/z ratio must be bigger than the minimum m/z ratio !";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	if (((eModeType)mode->currentIndex() == compoundsearch) || ((eModeType)mode->currentIndex() == dereplication)) {
		if ((minimumrt->value() != 0) && (maximumrt->value() != 0)) {
			if (maximumrt->value() <= minimumrt->value()) {	
				regex rx;
				bool lcms;

				for (auto& it : multiplefiles) {
					lcms = true;

					rx = "\\.[iI][mM][zZ][mM][lL]$";
					// imzML File
					if (regex_search(it.toStdString(), rx)) {
						lcms = false;
					}

					#if OS_TYPE == WIN
						rx = "\\.[mM][iI][sS]$";
						// flexImaging File
						if (regex_search(it.toStdString(), rx)) {
							lcms = false;
						}
					#endif

					if (lcms) {
						errstr = "The maximum retention time must be bigger than the minimum retention time !";
						msgBox.setText(errstr);
						msgBox.exec();
						return false;
					}
				}
			}
		}

		if (minratio54Fe56Fe->value() >= maxratio54Fe56Fe->value()) {
			errstr = "The minimum ratio 54Fe/56Fe must be lower than the maximum ratio of 54Fe/56Fe !";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}

	if ((eModeType)mode->currentIndex() == denovoengine) {
		if ((ePeptideType)peptidetype->currentIndex() == other) {
			errstr = "The peptide type 'Other' cannot be used in this mode !";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}

	if ((eModeType)mode->currentIndex() == singlecomparison) {
		if ((ePeptideType)peptidetype->currentIndex() == other) {
			if (searchedsequenceformula->text().isEmpty()) {
				errstr = "The field 'Formula' is empty !";
				msgBox.setText(errstr);
				msgBox.exec();
				return false;
			}

			tmpstring = searchedsequenceformula->text().toStdString();
			tmpformula.setFormula(tmpstring, false);
			if (!tmpformula.isValid(errmsg)) {
				errstr = "The field 'Formula' is not valid !\n\n";
				errstr += errmsg.c_str();
				msgBox.setText(errstr);
				msgBox.exec();
				return false;
			}
		}
	}

	/*
	if ((maximumbricksincombinationmiddle->value() < 2) && ((eModeType)mode->currentIndex() == denovoengine) && (((ePeptideType)peptidetype->currentIndex() == branched) || ((ePeptideType)peptidetype->currentIndex() == branchcyclic))) {
		errstr = "'Maximum Number of Combined Blocks (middle)' must be at least 2 when a branched or a branch-cyclic peptide is searched! (One block represents a branched residue, the other block(s) corresponds to a branch.)";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}
	*/

	if ((eModeType)mode->currentIndex() == compoundsearch) {
		if (elementstypes->getList()->count() == 0) {
			errstr = "The list of chemical elements is empty!";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}

		if (maximumcombinedelements->value() == 0) {
			errstr = "The maximum number of combined elements is zero (use e.g. 150 or 200)!";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}

		bool somethingselected = false;
		for (int i = 0; i < elementstypes->getList()->count(); i++) {
			tmpstring = elementstypes->getList()->item(i)->text().toStdString();

			if (tmpstring.rfind(':') != string::npos) {
				tmpstring = tmpstring.substr(0, tmpstring.rfind(':'));
			}

			tmpformula.setFormula(tmpstring, false);
			if (!tmpformula.isValid(errmsg)) {
				errstr = "A formula in the list of elements is not valid!\n\n";
				errstr += errmsg.c_str();
				msgBox.setText(errstr);
				msgBox.exec();
				return false;
			}

			if (elementstypes->getList()->item(i)->isSelected()) {
				somethingselected = true;
			}
		}

		if (!somethingselected) {
			errstr = "Please, select some chemical elements!";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}
	else {
		for (int i = 0; i < neutrallosstypes->getList()->count(); i++) {
			tmpstring = neutrallosstypes->getList()->item(i)->text().toStdString();

			if (tmpstring.rfind(':') != string::npos) {
				tmpstring = tmpstring.substr(0, tmpstring.rfind(':'));
			}

			tmpformula.setFormula(tmpstring, false);
			if (!tmpformula.isValid(errmsg)) {
				errstr = "A molecular formula of a neutral loss is not valid!\n\n";
				errstr += errmsg.c_str();
				msgBox.setText(errstr);
				msgBox.exec();
				return false;
			}
		}
	}

	parameters.mode = (eModeType)mode->currentIndex();
	parameters.maximumnumberofthreads = maximumnumberofthreads->value();

	parameters.peptidetype = (ePeptideType)peptidetype->currentIndex();

	parameters.peaklistfilenames.clear();
	for (auto& it : multiplefiles) {
		parameters.peaklistfilenames.push_back(it.toStdString());
	}

	parameters.originalpeaklistfilenames = peaklistline->text().toStdString();

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
	parameters.maximummz = maximummz->value();
	parameters.minimumrt = minimumrt->value();
	parameters.maximumrt = maximumrt->value();
	parameters.fwhm = fwhm->value();
	parameters.minratio54Fe56Fe = minratio54Fe56Fe->value();
	parameters.maxratio54Fe56Fe = maxratio54Fe56Fe->value();

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
	parameters.internalfragments = internalfragments->isChecked();
	parameters.enablescrambling = enablescrambling->isChecked();
	parameters.similaritysearch = similaritysearch->isChecked();
	parameters.regularblocksorder = regularblocksorder->isChecked();

	parameters.sequencedatabasefilename = sequencedatabaseline->text().toStdString();
	parameters.scoretype = (eScoreType)scoretype->currentIndex();
	parameters.hitsreported = hitsreported->value();
	parameters.sequencetag = sequencetag->text().toStdString();
	parameters.originalsequencetag = parameters.sequencetag;

	parameters.ionsfortheoreticalspectraMS1.clear();
	parameters.ionsfortheoreticalspectraMS2.clear();

	if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
		for (int i = 0; i < iontypes->getList()->count(); i++) {
			if (iontypes->getList()->item(i)->isSelected()) {
				parameters.ionsfortheoreticalspectraMS1.push_back(globalpreferences->customions[i]);
			}
		}
	}
	else {
		int start = -1;

		switch ((ePeptideType)peptidetype->currentIndex()) {
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
				break;
			default:
				break;
		}

		if (start != -1) {
			for (int i = 0; i < iontypes->getList()->count(); i++) {
				if (iontypes->getList()->item(i)->isSelected()) {
					parameters.ionsfortheoreticalspectraMS2.push_back((eFragmentIonType)(i + start));
				}
			}
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

	parameters.originalelementsdefinitions.clear();
	parameters.originalelementsfortheoreticalspectra.clear();
	neutralLoss element;
	for (int i = 0; i < elementstypes->getList()->count(); i++) {
		element.clear();
		element.summary = elementstypes->getList()->item(i)->text().toStdString();
		parameters.originalelementsdefinitions.push_back(element);
		if (elementstypes->getList()->item(i)->isSelected()) {
			parameters.originalelementsfortheoreticalspectra.push_back(i);
		}
	}

	parameters.maximumcombinedelements = maximumcombinedelements->value();

	//parameters.clearhitswithoutparent = clearhitswithoutparent->isChecked();
	parameters.reportunmatchedtheoreticalpeaks = reportunmatchedtheoreticalpeaks->isChecked();
	parameters.generateisotopepattern = generateisotopepattern->isChecked();
	parameters.minimumpatternsize = minimumpatternsize->value();
	parameters.minimumfeaturesize = minimumfeaturesize->value();
	parameters.minimumiontypes = minimumiontypes->value();
	parameters.basicformulacheck = basicformulacheck->isChecked();
	parameters.advancedformulacheck = advancedformulacheck->isChecked();
	parameters.noratiocheck = noratiocheck->isChecked();
	parameters.calculatefdrs = calculatefdrs->isChecked();

	parameters.minimumannotationintensityrelative = minimumannotationintensityrelative->value();
	parameters.minimumannotationintensityabsolute = minimumannotationintensityabsolute->value();

	parameters.mzdifftolerance = mzdifftolerance->value();
	parameters.intensitytolerance = intensitytolerance->value();

	parameters.searchedsequence = searchedsequenceline->text().toStdString();
	parameters.originalsearchedsequence = parameters.searchedsequence;
	parameters.searchedsequenceNtermmodif = searchedsequenceNtermmodif->text().toStdString();
	parameters.searchedsequenceCtermmodif = searchedsequenceCtermmodif->text().toStdString();
	parameters.searchedsequenceTmodif = searchedsequenceTmodif->text().toStdString();
	parameters.searchedsequenceformula = searchedsequenceformula->text().toStdString();

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

	//if (parameters.peaklistfilenames.size() > 0) {
	//	QStringList strlist;
	//	for (auto& it : parameters.peaklistfilenames) {
	//		strlist << it.c_str();
	//	}
	//	peaklistline->setText(strlist.join(";"));
	//}
	//else {
	//	peaklistline->setText("");
	//}

	peaklistline->setText(parameters.originalpeaklistfilenames.c_str());

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
	maximummz->setValue(parameters.maximummz);
	minimumrt->setValue(parameters.minimumrt);
	maximumrt->setValue(parameters.maximumrt);
	fwhm->setValue(parameters.fwhm);
	minratio54Fe56Fe->setValue(parameters.minratio54Fe56Fe);
	maxratio54Fe56Fe->setValue(parameters.maxratio54Fe56Fe);

	brickdatabaseline->setText(parameters.bricksdatabasefilename.c_str());
	maximumbricksincombinationbegin->setValue(parameters.maximumbricksincombinationbegin);
	maximumbricksincombinationmiddle->setValue(parameters.maximumbricksincombinationmiddle);
	maximumbricksincombinationend->setValue(parameters.maximumbricksincombinationend);
	maximumcumulativemass->setValue(parameters.maximumcumulativemass);
	modificationsline->setText(parameters.modificationsfilename.c_str());

	blindedges->setCurrentIndex(parameters.blindedges);
	cyclicnterminus->setChecked(parameters.cyclicnterminus);
	cycliccterminus->setChecked(parameters.cycliccterminus);
	internalfragments->setChecked(parameters.internalfragments);
	enablescrambling->setChecked(parameters.enablescrambling);
	similaritysearch->setChecked(parameters.similaritysearch);
	regularblocksorder->setChecked(parameters.regularblocksorder);

	sequencedatabaseline->setText(parameters.sequencedatabasefilename.c_str());
	scoretype->setCurrentIndex(parameters.scoretype);
	hitsreported->setValue(parameters.hitsreported);
	sequencetag->setText(parameters.sequencetag.c_str());

	for (int i = 0; i < iontypes->getList()->count(); i++) {
		iontypes->getList()->item(i)->setSelected(false);
	}

	if ((parameters.mode == dereplication) || (parameters.mode == compoundsearch)) {
		for (int i = 0; i < (int)parameters.ionsfortheoreticalspectraMS1.size(); i++) {
			for (int j = 0; j < iontypes->getList()->count(); j++) {
				if (iontypes->getList()->item(j)->text().toStdString().compare(parameters.ionsfortheoreticalspectraMS1[i].name) == 0) {
					iontypes->getList()->item(j)->setSelected(true);
					break;
				}
			}			
		}
	}
	else {
		int start = -1;

		switch (parameters.peptidetype) {
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
				break;
			default:
				break;
		}

		if (start != -1) {
			for (int i = 0; i < (int)parameters.ionsfortheoreticalspectraMS2.size(); i++) {
				iontypes->getList()->item(parameters.ionsfortheoreticalspectraMS2[i] - start)->setSelected(true);
			}
		}
	}

	neutrallosstypes->getList()->clear();
	for (int i = 0; i < (int)parameters.originalneutrallossesdefinitions.size(); i++) {
		neutrallosstypes->addItem(parameters.originalneutrallossesdefinitions[i].summary.c_str());
	}

	for (int i = 0; i < (int)parameters.originalneutrallossesfortheoreticalspectra.size(); i++) {
		neutrallosstypes->getList()->item(parameters.originalneutrallossesfortheoreticalspectra[i])->setSelected(true);
	}

	maximumcombinedlosses->setValue(parameters.maximumcombinedlosses);

	elementstypes->getList()->clear();
	for (int i = 0; i < (int)parameters.originalelementsdefinitions.size(); i++) {
		elementstypes->addItem(parameters.originalelementsdefinitions[i].summary.c_str());
	}

	for (int i = 0; i < (int)parameters.originalelementsfortheoreticalspectra.size(); i++) {
		elementstypes->getList()->item(parameters.originalelementsfortheoreticalspectra[i])->setSelected(true);
	}

	maximumcombinedelements->setValue(parameters.maximumcombinedelements);

	//clearhitswithoutparent->setChecked(parameters.clearhitswithoutparent);
	reportunmatchedtheoreticalpeaks->setChecked(parameters.reportunmatchedtheoreticalpeaks);
	generateisotopepattern->setChecked(parameters.generateisotopepattern);
	minimumpatternsize->setValue(parameters.minimumpatternsize);
	minimumfeaturesize->setValue(parameters.minimumfeaturesize);
	minimumiontypes->setValue(parameters.minimumiontypes);
	basicformulacheck->setChecked(parameters.basicformulacheck);
	advancedformulacheck->setChecked(parameters.advancedformulacheck);
	noratiocheck->setChecked(parameters.noratiocheck);
	calculatefdrs->setChecked(parameters.calculatefdrs);

	minimumannotationintensityrelative->setValue(parameters.minimumannotationintensityrelative);
	minimumannotationintensityabsolute->setValue(parameters.minimumannotationintensityabsolute);

	mzdifftolerance->setValue(parameters.mzdifftolerance);
	intensitytolerance->setValue(parameters.intensitytolerance);

	searchedsequenceline->setText(parameters.searchedsequence.c_str());
	searchedsequenceNtermmodif->setText(parameters.searchedsequenceNtermmodif.c_str());
	searchedsequenceCtermmodif->setText(parameters.searchedsequenceCtermmodif.c_str());
	searchedsequenceTmodif->setText(parameters.searchedsequenceTmodif.c_str());
	searchedsequenceformula->setText(parameters.searchedsequenceformula.c_str());

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
	if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
		return;
	}

	if (!(((eModeType)mode->currentIndex() != oldmodetype) 
		&& (((eModeType)mode->currentIndex() == denovoengine) || ((eModeType)mode->currentIndex() == singlecomparison) || ((eModeType)mode->currentIndex() == databasesearch))
		&& ((oldmodetype == denovoengine) || (oldmodetype == singlecomparison) || (oldmodetype == databasesearch))
		)) {
		resetFragmentIonTypes();
	}

	bool denovostate = (mode->currentIndex() == denovoengine) ? false : true;
	bool sequencetagstate = (mode->currentIndex() == denovoengine) || (mode->currentIndex() == databasesearch) ? false : true;

	switch ((ePeptideType)index) {
		case linear:
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(denovostate);
			maximumbricksincombinationmiddle->setDisabled(denovostate);
			maximumbricksincombinationend->setDisabled(denovostate);
			blindedges->setDisabled(denovostate);
			maximumcumulativemass->setDisabled(denovostate);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
			cyclicnterminus->setDisabled(false);
			cycliccterminus->setDisabled(false);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(sequencetagstate);
			iontypes->setDisabled(false);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(false);
			searchedsequenceCtermmodif->setDisabled(false);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula ->setDisabled(true);
			break;
		case cyclic:
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(denovostate);
			maximumbricksincombinationmiddle->setDisabled(denovostate);
			maximumbricksincombinationend->setDisabled(denovostate);
			blindedges->setDisabled(denovostate);
			maximumcumulativemass->setDisabled(denovostate);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(false);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(sequencetagstate);
			iontypes->setDisabled(false);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(true);
			searchedsequenceCtermmodif->setDisabled(true);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula->setDisabled(true);
			break;
		case branched:
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(denovostate);
			maximumbricksincombinationmiddle->setDisabled(denovostate);
			maximumbricksincombinationend->setDisabled(denovostate);
			blindedges->setDisabled(denovostate);
			maximumcumulativemass->setDisabled(denovostate);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(sequencetagstate);
			iontypes->setDisabled(false);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(false);
			searchedsequenceCtermmodif->setDisabled(false);
			searchedsequenceTmodif->setDisabled(false);
			searchedsequenceformula->setDisabled(true);
			break;
		case branchcyclic:
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(denovostate);
			maximumbricksincombinationmiddle->setDisabled(denovostate);
			maximumbricksincombinationend->setDisabled(denovostate);
			blindedges->setDisabled(denovostate);
			maximumcumulativemass->setDisabled(denovostate);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(false);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(sequencetagstate);
			iontypes->setDisabled(false);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(true);
			searchedsequenceCtermmodif->setDisabled(true);
			searchedsequenceTmodif->setDisabled(false);
			searchedsequenceformula->setDisabled(true);
			break;
		case linearpolyketide:
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(denovostate);
			maximumbricksincombinationmiddle->setDisabled(denovostate);
			maximumbricksincombinationend->setDisabled(denovostate);
			blindedges->setDisabled(denovostate);
			maximumcumulativemass->setDisabled(denovostate);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
			cyclicnterminus->setDisabled(false);
			cycliccterminus->setDisabled(false);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(false);
			sequencetag->setDisabled(sequencetagstate);
			iontypes->setDisabled(false);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(false);
			searchedsequenceCtermmodif->setDisabled(false);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula->setDisabled(true);
			break;
		case cyclicpolyketide:
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(denovostate);
			maximumbricksincombinationmiddle->setDisabled(denovostate);
			maximumbricksincombinationend->setDisabled(denovostate);
			blindedges->setDisabled(denovostate);
			maximumcumulativemass->setDisabled(denovostate);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(false);
			sequencetag->setDisabled((mode->currentIndex() == denovoengine) || (mode->currentIndex() == databasesearch) ? false : true);
			iontypes->setDisabled(false);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(true);
			searchedsequenceCtermmodif->setDisabled(true);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula->setDisabled(true);
			break;
		case other:
			brickdatabaseline->setDisabled(true);
			brickdatabasebutton->setDisabled(true);
			maximumbricksincombinationbegin->setDisabled(true);
			maximumbricksincombinationmiddle->setDisabled(true);
			maximumbricksincombinationend->setDisabled(true);
			blindedges->setDisabled(true);
			maximumcumulativemass->setDisabled(true);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(true);
			iontypes->setDisabled(true);
			searchedsequenceline->setDisabled(true);
			searchedsequencebutton->setDisabled(true);
			searchedsequenceNtermmodif->setDisabled(true);
			searchedsequenceCtermmodif->setDisabled(true);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula->setDisabled(false);
			break;
		default:
			break;
	}
}


void cParametersWidget::updateSettingsWhenModeChanged(int index) {
	switch ((eModeType)index) {
		case denovoengine:
			peptidetype->setDisabled(false);
			scannumber->setDisabled(false);
			precursormass->setDisabled(false);
			precursoradduct->setDisabled(false);
			precursorcharge->setDisabled(false);
			precursormasserrortolerance->setDisabled(false);
			maximummz->setDisabled(true);
			minimumrt->setDisabled(true);
			maximumrt->setDisabled(true);
			fwhm->setDisabled(false);
			minratio54Fe56Fe->setDisabled(true);
			maxratio54Fe56Fe->setDisabled(true);
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(false);
			maximumbricksincombinationmiddle->setDisabled(false);
			maximumbricksincombinationend->setDisabled(false);
			blindedges->setDisabled(false);
			maximumcumulativemass->setDisabled(false);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
			sequencedatabaseline->setDisabled(true);
			sequencedatabasebutton->setDisabled(true);
			similaritysearch->setDisabled(true);
			maximumnumberofthreads->setDisabled(false);
			scoretype->setDisabled(false);
			hitsreported->setDisabled(false);
			sequencetag->setDisabled(false);
			iontypes->setDisabled(false);
			neutrallosstypes->setDisabled(false);
			maximumcombinedlosses->setDisabled(false);
			elementstypes->setDisabled(true);
			maximumcombinedelements->setDisabled(true);
			//clearhitswithoutparent->setDisabled(false);
			basicformulacheck->setDisabled(false);
			advancedformulacheck->setDisabled(true);
			noratiocheck->setDisabled(true);
			calculatefdrs->setDisabled(true);
			minimumannotationintensityrelative->setDisabled(true);
			minimumannotationintensityabsolute->setDisabled(true);
			mzdifftolerance->setDisabled(true);
			intensitytolerance->setDisabled(true);
			reportunmatchedtheoreticalpeaks->setDisabled(false);
			generateisotopepattern->setDisabled(false);
			minimumpatternsize->setDisabled(false);
			minimumfeaturesize->setDisabled(true);
			minimumiontypes->setDisabled(true);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);

			updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());

			if (hideunusedparameters) {

				maximumnumberofthreadslabel->setHidden(false);
				maximumnumberofthreads->setHidden(false);

				peptidetypelabel->setHidden(false);
				peptidetype->setHidden(false);

				scannumberlabel->setHidden(false);
				scannumber->setHidden(false);

				precursormasslabel->setHidden(false);
				precursormass->setHidden(false);

				precursoradductlabel->setHidden(false);
				precursoradduct->setHidden(false);

				precursormasserrortolerancelabel->setHidden(false);
				precursormasserrortolerance->setHidden(false);

				rtlabel->setHidden(true);
				minimumrt->setHidden(true);
				maximumrt->setHidden(true);

				ratio54Fe56Felabel->setHidden(true);
				minratio54Fe56Fe->setHidden(true);
				maxratio54Fe56Fe->setHidden(true);

				maximumbricksincombinatiolabel->setHidden(false);
				maximumbricksincombinationbegin->setHidden(false);
				maximumbricksincombinationmiddle->setHidden(false);
				maximumbricksincombinationend->setHidden(false);

				blindedgeslabel->setHidden(false);
				blindedges->setHidden(false);

				maximumcumulativemasslabel->setHidden(false);
				maximumcumulativemass->setHidden(false);

				sequencedatabaselabel->setHidden(true);
				sequencedatabaseline->setHidden(true);
				sequencedatabasebutton->setHidden(true);

				scoretypelabel->setHidden(false);
				scoretype->setHidden(false);

				hitsreportedlabel->setHidden(false);
				hitsreported->setHidden(false);

				sequencetaglabel->setHidden(false);
				sequencetag->setHidden(false);

				neutrallosstypeslabel->setHidden(false);
				neutrallosstypes->setHidden(false);

				maximumcombinedlosseslabel->setHidden(false);
				maximumcombinedlosses->setHidden(false);

				elementstypeslabel->setHidden(true);
				elementstypes->setHidden(true);

				maximumcombinedelementslabel->setHidden(true);
				maximumcombinedelements->setHidden(true);

				minimumfeaturesizelabel->setHidden(true);
				minimumfeaturesize->setHidden(true);

				minimumiontypeslabel->setHidden(true);
				minimumiontypes->setHidden(true);

				advancedformulachecklabel->setHidden(true);
				advancedformulacheck->setHidden(true);

				noratiochecklabel->setHidden(true);
				noratiocheck->setHidden(true);

				calculatefdrslabel->setHidden(true);
				calculatefdrs->setHidden(true);

				minimumannotationintensitylabel->setHidden(true);
				minimumannotationintensityrelative->setHidden(true);
				minimumannotationintensityabsolute->setHidden(true);

				mzdifftolerancelabel->setHidden(true);
				mzdifftolerance->setHidden(true);

				intensitytolerancelabel->setHidden(true);
				intensitytolerance->setHidden(true);

				brickdatabasegroupbox->setHidden(false);
				miscgroupbox->setHidden(false);
				searchedsequencegroupbox->setHidden(false);

			}

			break;
		case singlecomparison:
			peptidetype->setDisabled(false);
			scannumber->setDisabled(true);
			precursormass->setDisabled(false);
			precursoradduct->setDisabled(false);
			precursorcharge->setDisabled(false);
			precursormasserrortolerance->setDisabled(false);
			maximummz->setDisabled(true);
			minimumrt->setDisabled(true);
			maximumrt->setDisabled(true);
			fwhm->setDisabled(false);
			minratio54Fe56Fe->setDisabled(true);
			maxratio54Fe56Fe->setDisabled(true);
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(true);
			maximumbricksincombinationmiddle->setDisabled(true);
			maximumbricksincombinationend->setDisabled(true);
			blindedges->setDisabled(true);
			maximumcumulativemass->setDisabled(true);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
			sequencedatabaseline->setDisabled(true);
			sequencedatabasebutton->setDisabled(true);
			similaritysearch->setDisabled(true);
			maximumnumberofthreads->setDisabled(true);
			scoretype->setDisabled(true);
			hitsreported->setDisabled(true);
			sequencetag->setDisabled(true);
			iontypes->setDisabled(false);
			neutrallosstypes->setDisabled(false);
			maximumcombinedlosses->setDisabled(false);
			elementstypes->setDisabled(true);
			maximumcombinedelements->setDisabled(true);
			//clearhitswithoutparent->setDisabled(false);
			basicformulacheck->setDisabled(false);
			advancedformulacheck->setDisabled(true);
			noratiocheck->setDisabled(true);
			calculatefdrs->setDisabled(true);
			minimumannotationintensityrelative->setDisabled(true);
			minimumannotationintensityabsolute->setDisabled(true);
			mzdifftolerance->setDisabled(true);
			intensitytolerance->setDisabled(true);
			reportunmatchedtheoreticalpeaks->setDisabled(false);
			generateisotopepattern->setDisabled(false);
			minimumpatternsize->setDisabled(false);
			minimumfeaturesize->setDisabled(true);
			minimumiontypes->setDisabled(true);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);

			updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());

			if (hideunusedparameters) {

				maximumnumberofthreadslabel->setHidden(true);
				maximumnumberofthreads->setHidden(true);

				peptidetypelabel->setHidden(false);
				peptidetype->setHidden(false);

				scannumberlabel->setHidden(true);
				scannumber->setHidden(true);

				precursormasslabel->setHidden(false);
				precursormass->setHidden(false);

				precursoradductlabel->setHidden(false);
				precursoradduct->setHidden(false);

				precursormasserrortolerancelabel->setHidden(false);
				precursormasserrortolerance->setHidden(false);

				rtlabel->setHidden(true);
				minimumrt->setHidden(true);
				maximumrt->setHidden(true);

				ratio54Fe56Felabel->setHidden(true);
				minratio54Fe56Fe->setHidden(true);
				maxratio54Fe56Fe->setHidden(true);

				maximumbricksincombinatiolabel->setHidden(true);
				maximumbricksincombinationbegin->setHidden(true);
				maximumbricksincombinationmiddle->setHidden(true);
				maximumbricksincombinationend->setHidden(true);

				blindedgeslabel->setHidden(true);
				blindedges->setHidden(true);

				maximumcumulativemasslabel->setHidden(true);
				maximumcumulativemass->setHidden(true);

				sequencedatabaselabel->setHidden(true);
				sequencedatabaseline->setHidden(true);
				sequencedatabasebutton->setHidden(true);

				scoretypelabel->setHidden(true);
				scoretype->setHidden(true);

				hitsreportedlabel->setHidden(true);
				hitsreported->setHidden(true);

				sequencetaglabel->setHidden(true);
				sequencetag->setHidden(true);

				neutrallosstypeslabel->setHidden(false);
				neutrallosstypes->setHidden(false);

				maximumcombinedlosseslabel->setHidden(false);
				maximumcombinedlosses->setHidden(false);

				elementstypeslabel->setHidden(true);
				elementstypes->setHidden(true);

				maximumcombinedelementslabel->setHidden(true);
				maximumcombinedelements->setHidden(true);

				minimumfeaturesizelabel->setHidden(true);
				minimumfeaturesize->setHidden(true);

				minimumiontypeslabel->setHidden(true);
				minimumiontypes->setHidden(true);

				advancedformulachecklabel->setHidden(true);
				advancedformulacheck->setHidden(true);

				noratiochecklabel->setHidden(true);
				noratiocheck->setHidden(true);

				calculatefdrslabel->setHidden(true);
				calculatefdrs->setHidden(true);

				minimumannotationintensitylabel->setHidden(true);
				minimumannotationintensityrelative->setHidden(true);
				minimumannotationintensityabsolute->setHidden(true);

				mzdifftolerancelabel->setHidden(true);
				mzdifftolerance->setHidden(true);

				intensitytolerancelabel->setHidden(true);
				intensitytolerance->setHidden(true);

				brickdatabasegroupbox->setHidden(false);
				miscgroupbox->setHidden(false);
				searchedsequencegroupbox->setHidden(false);

			}

			break;
		case databasesearch:
			peptidetype->setDisabled(false);
			scannumber->setDisabled(false);
			precursormass->setDisabled(false);
			precursoradduct->setDisabled(false);
			precursorcharge->setDisabled(false);
			precursormasserrortolerance->setDisabled(false);
			maximummz->setDisabled(true);
			minimumrt->setDisabled(true);
			maximumrt->setDisabled(true);
			fwhm->setDisabled(false);
			minratio54Fe56Fe->setDisabled(true);
			maxratio54Fe56Fe->setDisabled(true);
			brickdatabaseline->setDisabled(false);
			brickdatabasebutton->setDisabled(false);
			maximumbricksincombinationbegin->setDisabled(true);
			maximumbricksincombinationmiddle->setDisabled(true);
			maximumbricksincombinationend->setDisabled(true);
			blindedges->setDisabled(true);
			maximumcumulativemass->setDisabled(true);
			modificationsline->setDisabled(false);
			modificationsbutton->setDisabled(false);
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
			elementstypes->setDisabled(true);
			maximumcombinedelements->setDisabled(true);
			//clearhitswithoutparent->setDisabled(false);
			basicformulacheck->setDisabled(false);
			advancedformulacheck->setDisabled(true);
			noratiocheck->setDisabled(true);
			calculatefdrs->setDisabled(true);
			minimumannotationintensityrelative->setDisabled(true);
			minimumannotationintensityabsolute->setDisabled(true);
			mzdifftolerance->setDisabled(true);
			intensitytolerance->setDisabled(true);
			reportunmatchedtheoreticalpeaks->setDisabled(false);
			generateisotopepattern->setDisabled(false);
			minimumpatternsize->setDisabled(false);
			minimumfeaturesize->setDisabled(true);
			minimumiontypes->setDisabled(true);
			searchedsequenceline->setDisabled(false);
			searchedsequencebutton->setDisabled(false);

			updateSettingsWhenPeptideTypeChanged(peptidetype->currentIndex());

			if (hideunusedparameters) {

				maximumnumberofthreadslabel->setHidden(false);
				maximumnumberofthreads->setHidden(false);

				peptidetypelabel->setHidden(false);
				peptidetype->setHidden(false);

				scannumberlabel->setHidden(false);
				scannumber->setHidden(false);

				precursormasslabel->setHidden(false);
				precursormass->setHidden(false);

				precursoradductlabel->setHidden(false);
				precursoradduct->setHidden(false);

				precursormasserrortolerancelabel->setHidden(false);
				precursormasserrortolerance->setHidden(false);

				rtlabel->setHidden(true);
				minimumrt->setHidden(true);
				maximumrt->setHidden(true);

				ratio54Fe56Felabel->setHidden(true);
				minratio54Fe56Fe->setHidden(true);
				maxratio54Fe56Fe->setHidden(true);

				maximumbricksincombinatiolabel->setHidden(true);
				maximumbricksincombinationbegin->setHidden(true);
				maximumbricksincombinationmiddle->setHidden(true);
				maximumbricksincombinationend->setHidden(true);

				blindedgeslabel->setHidden(true);
				blindedges->setHidden(true);

				maximumcumulativemasslabel->setHidden(true);
				maximumcumulativemass->setHidden(true);

				sequencedatabaselabel->setHidden(false);
				sequencedatabaseline->setHidden(false);
				sequencedatabasebutton->setHidden(false);

				scoretypelabel->setHidden(false);
				scoretype->setHidden(false);

				hitsreportedlabel->setHidden(false);
				hitsreported->setHidden(false);

				sequencetaglabel->setHidden(false);
				sequencetag->setHidden(false);

				neutrallosstypeslabel->setHidden(false);
				neutrallosstypes->setHidden(false);

				maximumcombinedlosseslabel->setHidden(false);
				maximumcombinedlosses->setHidden(false);

				elementstypeslabel->setHidden(true);
				elementstypes->setHidden(true);

				maximumcombinedelementslabel->setHidden(true);
				maximumcombinedelements->setHidden(true);

				minimumfeaturesizelabel->setHidden(true);
				minimumfeaturesize->setHidden(true);

				minimumiontypeslabel->setHidden(true);
				minimumiontypes->setHidden(true);

				advancedformulachecklabel->setHidden(true);
				advancedformulacheck->setHidden(true);

				noratiochecklabel->setHidden(true);
				noratiocheck->setHidden(true);

				calculatefdrslabel->setHidden(true);
				calculatefdrs->setHidden(true);

				minimumannotationintensitylabel->setHidden(true);
				minimumannotationintensityrelative->setHidden(true);
				minimumannotationintensityabsolute->setHidden(true);

				mzdifftolerancelabel->setHidden(true);
				mzdifftolerance->setHidden(true);

				intensitytolerancelabel->setHidden(true);
				intensitytolerance->setHidden(true);

				brickdatabasegroupbox->setHidden(false);
				miscgroupbox->setHidden(false);
				searchedsequencegroupbox->setHidden(false);

			}

			break;
		case dereplication:
			peptidetype->setDisabled(true);
			scannumber->setDisabled(true);
			precursormass->setDisabled(true);
			precursoradduct->setDisabled(true);
			precursorcharge->setDisabled(false);
			precursormasserrortolerance->setDisabled(true);
			maximummz->setDisabled(false);
			minimumrt->setDisabled(false);
			maximumrt->setDisabled(false);
			fwhm->setDisabled(false);
			minratio54Fe56Fe->setDisabled(false);
			maxratio54Fe56Fe->setDisabled(false);
			brickdatabaseline->setDisabled(true);
			brickdatabasebutton->setDisabled(true);
			maximumbricksincombinationbegin->setDisabled(true);
			maximumbricksincombinationmiddle->setDisabled(true);
			maximumbricksincombinationend->setDisabled(true);
			blindedges->setDisabled(true);
			maximumcumulativemass->setDisabled(true);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			similaritysearch->setDisabled(true);
			sequencedatabaseline->setDisabled(false);
			sequencedatabasebutton->setDisabled(false);
			maximumnumberofthreads->setDisabled(false);
			scoretype->setDisabled(true);
			hitsreported->setDisabled(true);
			sequencetag->setDisabled(true);
			iontypes->setDisabled(false);
			neutrallosstypes->setDisabled(false);
			maximumcombinedlosses->setDisabled(false);
			elementstypes->setDisabled(true);
			maximumcombinedelements->setDisabled(true);
			//clearhitswithoutparent->setDisabled(true);
			basicformulacheck->setDisabled(false);
			advancedformulacheck->setDisabled(true);
			noratiocheck->setDisabled(true);
			calculatefdrs->setDisabled(false);
			minimumannotationintensityrelative->setDisabled(false);
			minimumannotationintensityabsolute->setDisabled(false);
			mzdifftolerance->setDisabled(false);
			intensitytolerance->setDisabled(false);
			reportunmatchedtheoreticalpeaks->setDisabled(false);
			generateisotopepattern->setDisabled(false);
			minimumpatternsize->setDisabled(false);
			minimumfeaturesize->setDisabled(false);
			minimumiontypes->setDisabled(false);
			searchedsequenceline->setDisabled(true);
			searchedsequencebutton->setDisabled(true);

			if ((oldmodetype == denovoengine) || (oldmodetype == singlecomparison) || (oldmodetype == databasesearch)) {
				resetFragmentIonTypes();
			}
		
			brickdatabaseline->setDisabled(true);
			brickdatabasebutton->setDisabled(true);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(true);
			iontypes->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(true);
			searchedsequenceCtermmodif->setDisabled(true);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula->setDisabled(true);

			if (hideunusedparameters) {

				maximumnumberofthreadslabel->setHidden(false);
				maximumnumberofthreads->setHidden(false);

				peptidetypelabel->setHidden(true);
				peptidetype->setHidden(true);

				scannumberlabel->setHidden(true);
				scannumber->setHidden(true);

				precursormasslabel->setHidden(true);
				precursormass->setHidden(true);

				precursoradductlabel->setHidden(true);
				precursoradduct->setHidden(true);

				precursormasserrortolerancelabel->setHidden(true);
				precursormasserrortolerance->setHidden(true);

				rtlabel->setHidden(false);
				minimumrt->setHidden(false);
				maximumrt->setHidden(false);

				ratio54Fe56Felabel->setHidden(false);
				minratio54Fe56Fe->setHidden(false);
				maxratio54Fe56Fe->setHidden(false);

				maximumbricksincombinatiolabel->setHidden(true);
				maximumbricksincombinationbegin->setHidden(true);
				maximumbricksincombinationmiddle->setHidden(true);
				maximumbricksincombinationend->setHidden(true);

				blindedgeslabel->setHidden(true);
				blindedges->setHidden(true);

				maximumcumulativemasslabel->setHidden(true);
				maximumcumulativemass->setHidden(true);

				sequencedatabaselabel->setHidden(false);
				sequencedatabaseline->setHidden(false);
				sequencedatabasebutton->setHidden(false);

				scoretypelabel->setHidden(true);
				scoretype->setHidden(true);

				hitsreportedlabel->setHidden(true);
				hitsreported->setHidden(true);

				sequencetaglabel->setHidden(true);
				sequencetag->setHidden(true);

				neutrallosstypeslabel->setHidden(false);
				neutrallosstypes->setHidden(false);

				maximumcombinedlosseslabel->setHidden(false);
				maximumcombinedlosses->setHidden(false);

				elementstypeslabel->setHidden(true);
				elementstypes->setHidden(true);

				maximumcombinedelementslabel->setHidden(true);
				maximumcombinedelements->setHidden(true);

				minimumfeaturesizelabel->setHidden(false);
				minimumfeaturesize->setHidden(false);

				minimumiontypeslabel->setHidden(false);
				minimumiontypes->setHidden(false);

				advancedformulachecklabel->setHidden(true);
				advancedformulacheck->setHidden(true);

				noratiochecklabel->setHidden(true);
				noratiocheck->setHidden(true);

				calculatefdrslabel->setHidden(false);
				calculatefdrs->setHidden(false);

				minimumannotationintensitylabel->setHidden(false);
				minimumannotationintensityrelative->setHidden(false);
				minimumannotationintensityabsolute->setHidden(false);

				mzdifftolerancelabel->setHidden(false);
				mzdifftolerance->setHidden(false);

				intensitytolerancelabel->setHidden(false);
				intensitytolerance->setHidden(false);

				brickdatabasegroupbox->setHidden(true);
				miscgroupbox->setHidden(true);
				searchedsequencegroupbox->setHidden(true);

			}

			break;
		case compoundsearch:
			peptidetype->setDisabled(true);
			scannumber->setDisabled(true);
			precursormass->setDisabled(true);
			precursoradduct->setDisabled(true);
			precursorcharge->setDisabled(false);
			precursormasserrortolerance->setDisabled(true);
			maximummz->setDisabled(false);
			minimumrt->setDisabled(false);
			maximumrt->setDisabled(false);
			fwhm->setDisabled(false);
			minratio54Fe56Fe->setDisabled(false);
			maxratio54Fe56Fe->setDisabled(false);
			brickdatabaseline->setDisabled(true);
			brickdatabasebutton->setDisabled(true);
			maximumbricksincombinationbegin->setDisabled(true);
			maximumbricksincombinationmiddle->setDisabled(true);
			maximumbricksincombinationend->setDisabled(true);
			blindedges->setDisabled(true);
			maximumcumulativemass->setDisabled(true);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			similaritysearch->setDisabled(true);
			sequencedatabaseline->setDisabled(true);
			sequencedatabasebutton->setDisabled(true);
			maximumnumberofthreads->setDisabled(false);
			scoretype->setDisabled(true);
			hitsreported->setDisabled(true);
			sequencetag->setDisabled(true);
			iontypes->setDisabled(false);
			neutrallosstypes->setDisabled(true);
			maximumcombinedlosses->setDisabled(true);
			elementstypes->setDisabled(false);
			maximumcombinedelements->setDisabled(false);
			//clearhitswithoutparent->setDisabled(true);
			basicformulacheck->setDisabled(false);
			advancedformulacheck->setDisabled(false);
			noratiocheck->setDisabled(false);
			calculatefdrs->setDisabled(false);
			minimumannotationintensityrelative->setDisabled(false);
			minimumannotationintensityabsolute->setDisabled(false);
			mzdifftolerance->setDisabled(false);
			intensitytolerance->setDisabled(false);
			reportunmatchedtheoreticalpeaks->setDisabled(false);
			generateisotopepattern->setDisabled(false);
			minimumpatternsize->setDisabled(false);
			minimumfeaturesize->setDisabled(false);
			minimumiontypes->setDisabled(false);
			searchedsequenceline->setDisabled(true);
			searchedsequencebutton->setDisabled(true);

			if ((oldmodetype == denovoengine) || (oldmodetype == singlecomparison) || (oldmodetype == databasesearch)) {
				resetFragmentIonTypes();
			}

			brickdatabaseline->setDisabled(true);
			brickdatabasebutton->setDisabled(true);
			modificationsline->setDisabled(true);
			modificationsbutton->setDisabled(true);
			cyclicnterminus->setDisabled(true);
			cycliccterminus->setDisabled(true);
			internalfragments->setDisabled(true);
			enablescrambling->setDisabled(true);
			regularblocksorder->setDisabled(true);
			sequencetag->setDisabled(true);
			iontypes->setDisabled(false);
			searchedsequenceNtermmodif->setDisabled(true);
			searchedsequenceCtermmodif->setDisabled(true);
			searchedsequenceTmodif->setDisabled(true);
			searchedsequenceformula->setDisabled(true);

			if (hideunusedparameters) {

				maximumnumberofthreadslabel->setHidden(false);
				maximumnumberofthreads->setHidden(false);

				peptidetypelabel->setHidden(true);
				peptidetype->setHidden(true);

				scannumberlabel->setHidden(true);
				scannumber->setHidden(true);

				precursormasslabel->setHidden(true);
				precursormass->setHidden(true);

				precursoradductlabel->setHidden(true);
				precursoradduct->setHidden(true);

				precursormasserrortolerancelabel->setHidden(true);
				precursormasserrortolerance->setHidden(true);

				rtlabel->setHidden(false);
				minimumrt->setHidden(false);
				maximumrt->setHidden(false);

				ratio54Fe56Felabel->setHidden(false);
				minratio54Fe56Fe->setHidden(false);
				maxratio54Fe56Fe->setHidden(false);

				maximumbricksincombinatiolabel->setHidden(true);
				maximumbricksincombinationbegin->setHidden(true);
				maximumbricksincombinationmiddle->setHidden(true);
				maximumbricksincombinationend->setHidden(true);

				blindedgeslabel->setHidden(true);
				blindedges->setHidden(true);

				maximumcumulativemasslabel->setHidden(true);
				maximumcumulativemass->setHidden(true);

				sequencedatabaselabel->setHidden(true);
				sequencedatabaseline->setHidden(true);
				sequencedatabasebutton->setHidden(true);

				scoretypelabel->setHidden(true);
				scoretype->setHidden(true);

				hitsreportedlabel->setHidden(true);
				hitsreported->setHidden(true);

				sequencetaglabel->setHidden(true);
				sequencetag->setHidden(true);

				neutrallosstypeslabel->setHidden(true);
				neutrallosstypes->setHidden(true);

				maximumcombinedlosseslabel->setHidden(true);
				maximumcombinedlosses->setHidden(true);

				elementstypeslabel->setHidden(false);
				elementstypes->setHidden(false);

				maximumcombinedelementslabel->setHidden(false);
				maximumcombinedelements->setHidden(false);

				minimumfeaturesizelabel->setHidden(false);
				minimumfeaturesize->setHidden(false);

				minimumiontypeslabel->setHidden(false);
				minimumiontypes->setHidden(false);

				advancedformulachecklabel->setHidden(false);
				advancedformulacheck->setHidden(false);

				noratiochecklabel->setHidden(false);
				noratiocheck->setHidden(false);

				calculatefdrslabel->setHidden(false);
				calculatefdrs->setHidden(false);

				minimumannotationintensitylabel->setHidden(false);
				minimumannotationintensityrelative->setHidden(false);
				minimumannotationintensityabsolute->setHidden(false);

				mzdifftolerancelabel->setHidden(false);
				mzdifftolerance->setHidden(false);

				intensitytolerancelabel->setHidden(false);
				intensitytolerance->setHidden(false);

				brickdatabasegroupbox->setHidden(true);
				miscgroupbox->setHidden(true);
				searchedsequencegroupbox->setHidden(true);

			}

			break;
		default:
			break;
	}

	oldmodetype = (eModeType)mode->currentIndex();
}


void cParametersWidget::resetFragmentIonTypes() {

	iontypes->getList()->clear();
	
	if (((eModeType)mode->currentIndex() == dereplication) || ((eModeType)mode->currentIndex() == compoundsearch)) {
		if (globalpreferences) {
			for (int i = 0; i < (int)globalpreferences->customions.size(); i++) {
				iontypes->getList()->addItem(globalpreferences->customions[i].name.c_str());
				if (globalpreferences->customions[i].name.compare("[M+H]+") == 0) {
					iontypes->getList()->item(i)->setSelected(true);
				}
			}
		}
	}
	else {
		eFragmentIonType start, end;

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
				return;
			default:
				break;
		}

		for (int i = (int)start; i <= (int)end; i++) {

			iontypes->getList()->addItem(tr(parameters.iondefinitions[(eFragmentIonType)i].name.c_str()));

			switch ((ePeptideType)peptidetype->currentIndex()) {
				case linear:
				case branched:
					if (((eFragmentIonType)i == b_ion) || ((eFragmentIonType)i == y_ion)) {
						iontypes->getList()->item(i - start)->setSelected(true);
					}
					break;
				case cyclic:
					if ((eFragmentIonType)i == b_ion) {
						iontypes->getList()->item(i - start)->setSelected(true);
					}
					break;
				case branchcyclic:
					if (((eFragmentIonType)i == b_ion) || ((eFragmentIonType)i == y_ion)) {
						iontypes->getList()->item(i - start)->setSelected(true);
					}
					break;
				case linearpolyketide:
					if (((eFragmentIonType)i == l1h_ion) || ((eFragmentIonType)i == l2h_ion) || ((eFragmentIonType)i == r1h_ion) || ((eFragmentIonType)i == r2h_ion) ||
						((eFragmentIonType)i == l1oh_ion) || ((eFragmentIonType)i == l2oh_ion) || ((eFragmentIonType)i == r1oh_ion) || ((eFragmentIonType)i == r2oh_ion)
						) {
						iontypes->getList()->item(i - start)->setSelected(true);
					}
					break;
				case cyclicpolyketide:
					if (/*((eFragmentIonType)i == l0h_ion) ||*/ ((eFragmentIonType)i == l1h_ion) || ((eFragmentIonType)i == l2h_ion)) {
						iontypes->getList()->item(i - start)->setSelected(true);
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

