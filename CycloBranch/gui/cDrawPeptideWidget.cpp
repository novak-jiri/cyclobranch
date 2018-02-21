#include "gui/cDrawPeptideWidget.h"

#include <QTextEdit>
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QCheckBox>
#include <QPushButton>
#include <QSplitter>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QToolBar>
#include <QKeyEvent>
#include <QIcon>


cDrawPeptideWidget::cDrawPeptideWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Draw Peptide");
	setWindowIcon(QIcon(":/images/icons/96.png"));

	drawpeptideformlayout = new QFormLayout();

	peptidetypecombobox = new QComboBox();
	peptidetypecombobox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	peptidetypecombobox->setToolTip("Select the type of peptide.");
	peptidetypecombobox->addItem(tr("Linear"));
	peptidetypecombobox->addItem(tr("Cyclic"));
	peptidetypecombobox->addItem(tr("Branched"));
	peptidetypecombobox->addItem(tr("Branch-cyclic"));
	peptidetypecombobox->addItem(tr("Linear polyketide"));
	peptidetypecombobox->addItem(tr("Cyclic polyketide"));
	//peptidetypecombobox->addItem(tr("Other"));
	drawpeptideformlayout->addRow(tr("Peptide Type: "), peptidetypecombobox);

	sequenceline = new QLineEdit();
	sequenceline->setToolTip("A peptide sequence to be drawn.\nThe tool automatically checks if the syntax of an entered sequence corresponds to the selected peptide type.\nIf the syntax is correct then a green message \"correct\" is shown else a red message \"incorrect\" is shown at the end of the input line.");

	sequencestatuslabel = new QLabel("");
	sequencestatuslabel->setMinimumWidth(50);

	sequenceactioncopy = new QAction("Copy to &Clipboard", this);
	sequenceactionsetsequence = new QAction("Set as Searched &Sequence", this);
	sequenceactionsettag = new QAction("Set as Sequence &Tag", this);
	sequenceactioninsert = new QAction("Insert into Sequence &Database", this);
	
	sequencemenu = new QMenu();
	sequencemenu->addAction(sequenceactioncopy);
	sequencemenu->addSeparator();
	sequencemenu->addAction(sequenceactionsetsequence);
	sequencemenu->addAction(sequenceactionsettag);
	sequencemenu->addAction(sequenceactioninsert);

	sequencetoolbutton = new QToolButton();
	sequencetoolbutton->setMenu(sequencemenu);
	sequencetoolbutton->setPopupMode(QToolButton::MenuButtonPopup);
	sequencetoolbutton->setDefaultAction(sequenceactioncopy);

	sequencetoolbar = new QToolBar();
	sequencetoolbar->addWidget(sequencetoolbutton);

	sequencelayout = new QHBoxLayout();
	sequencelayout->addWidget(sequenceline);
	sequencelayout->addWidget(sequencestatuslabel);
	sequencelayout->addWidget(sequencetoolbar);
	drawpeptideformlayout->addRow(tr("Sequence: "), sequencelayout);

	separateblocks = new QCheckBox();
	separateblocks->setToolTip("If the box is checked, the sequence to be drawn is specified by the fields \"Number of Blocks at Backbone\", \"Number of Blocks at Branch\", \"Branch Connection Block No.\", \"Backbone Blocks\" and \"Branch Blocks\".\nIf the box is unchecked, the sequence to be drawn is specified in the field \"Sequence\".");
	drawpeptideformlayout->addRow(tr("Define Blocks Separately: "), separateblocks);

	numberofblocksbackbone = new QSpinBox();
	numberofblocksbackbone->setToolTip("A number of blocks forming a backbone of a peptide.");
	numberofblocksbackbone->setMaximumWidth(150);
	numberofblocksbackbone->setSingleStep(1);
	drawpeptideformlayout->addRow(tr("Number of Blocks at Backbone: "), numberofblocksbackbone);

	numberofblocksbranch = new QSpinBox();
	numberofblocksbranch->setToolTip("A number of blocks forming a branch of a branched or a branch-cyclic peptide.");
	numberofblocksbranch->setMaximumWidth(150);
	numberofblocksbranch->setRange(1, 100);
	numberofblocksbranch->setSingleStep(1);
	drawpeptideformlayout->addRow(tr("Number of Blocks at Branch: "), numberofblocksbranch);

	branchposition = new QSpinBox();
	branchposition->setToolTip("A number of a block where a backbone of a branched or a branch-cyclic peptide is connected to a branch.");
	branchposition->setMaximumWidth(150);
	branchposition->setSingleStep(1);
	drawpeptideformlayout->addRow(tr("Branch Connection Block No.:"), branchposition);

	backboneblocks = new QHBoxLayout();
	backboneblocks->setContentsMargins(0, 0, 0, 0);
	backboneblockswidget = new QWidget();
	backboneblockswidget->setContentsMargins(0, 0, 0, 0);
	backboneblockswidget->setLayout(backboneblocks);
	backboneblockswidget->setToolTip("Names of building blocks forming a backbone of a peptide.\nThe number of fields is updated when the value of \"Number of Blocks at Backbone\" is changed.\nOne name of a building block per field must be entered.");
	drawpeptideformlayout->addRow(tr("Backbone Blocks: "), backboneblockswidget);

	branchblocks = new QHBoxLayout();
	branchblocks->setContentsMargins(0, 0, 0, 0);
	branchblockswidget = new QWidget();
	branchblockswidget->setContentsMargins(0, 0, 0, 0);
	branchblockswidget->setLayout(branchblocks);
	branchblockswidget->setToolTip("Names of building blocks forming a branch of a branched or a branch-cyclic peptide.\nThe number of fields is updated when the value of \"Number of Blocks at Branch\" is changed.\nOne name of a building block per field must be entered.");
	drawpeptideformlayout->addRow(tr("Branch Blocks: "), branchblockswidget);

	drawpeptideformwidget = new QWidget();
	drawpeptideformwidget->setLayout(drawpeptideformlayout);


	linearwidget = new cLinearWidget();
	linearwidget->initialize(0, 0);

	cyclicwidget = new cCyclicWidget();
	cyclicwidget->initialize(0, 0);

	branchedwidget = new cBranchedWidget();
	branchedwidget->initialize(0, 0);

	branchcyclicwidget = new cBranchCyclicWidget();
	branchcyclicwidget->initialize(0, 0);

	vbox = new QVBoxLayout();
	vbox->addWidget(drawpeptideformwidget);
	vbox->addWidget(linearwidget);
	vbox->addWidget(cyclicwidget);
	vbox->addWidget(branchedwidget);
	vbox->addWidget(branchcyclicwidget);

	vbox->setStretchFactor(drawpeptideformwidget, 1);
	vbox->setStretchFactor(linearwidget, 5);
	vbox->setStretchFactor(cyclicwidget, 5);
	vbox->setStretchFactor(branchedwidget, 5);
	vbox->setStretchFactor(branchcyclicwidget, 5);

	mainbox = new QHBoxLayout();
	mainbox->addLayout(vbox);
	setLayout(mainbox);

	resize(1280, 770);

	connect(sequenceline, SIGNAL(textEdited(const QString&)), this, SLOT(sequenceChanged()));
	connect(sequenceactioncopy, SIGNAL(triggered()), this, SLOT(sequenceCopy()));
	connect(sequenceactionsetsequence, SIGNAL(triggered()), this, SLOT(sequenceSetSequence()));
	connect(sequenceactionsettag, SIGNAL(triggered()), this, SLOT(sequenceSetTag()));
	connect(sequenceactioninsert, SIGNAL(triggered()), this, SLOT(sequenceInsert()));
	connect(peptidetypecombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(peptideTypeChanged(int)));
	connect(separateblocks, SIGNAL(stateChanged(int)), this, SLOT(separateBlocksChanged(int)));
	connect(numberofblocksbackbone, SIGNAL(valueChanged(int)), this, SLOT(numberOfBackboneBlocksChanged(int)));
	connect(numberofblocksbranch, SIGNAL(valueChanged(int)), this, SLOT(numberOfBranchBlocksChanged(int)));
	connect(branchposition, SIGNAL(valueChanged(int)), this, SLOT(blocksChanged()));
	numberOfBackboneBlocksChanged(numberofblocksbackbone->value());
	numberOfBranchBlocksChanged(numberofblocksbranch->value());
	peptideTypeChanged(peptidetypecombobox->currentIndex());

	connect(this, SIGNAL(sendSequence(int, QString)), parent, SLOT(setSequence(int, QString)));
	connect(this, SIGNAL(sendTag(int, QString)), parent, SLOT(setTag(int, QString)));
	connect(this, SIGNAL(sendInsert(int, QString)), parent, SLOT(insertSequence(int, QString)));
}


cDrawPeptideWidget::~cDrawPeptideWidget() {
	delete linearwidget;
	delete cyclicwidget;
	delete branchedwidget;
	delete branchcyclicwidget;

	delete peptidetypecombobox;
	
	delete sequenceline;
	delete sequencestatuslabel;
	delete sequenceactioncopy;
	delete sequenceactionsetsequence;
	delete sequenceactionsettag;
	delete sequenceactioninsert;
	delete sequencemenu;
	delete sequencetoolbutton;
	delete sequencetoolbar;
	delete sequencelayout;

	delete separateblocks;

	delete numberofblocksbackbone;
	delete numberofblocksbranch;
	delete branchposition;

	QLayoutItem* item;

	while (backboneblocks->count() > 0) {
		item = backboneblocks->takeAt(0);
		delete item->widget();
	}

	while (branchblocks->count() > 0) {
		item = branchblocks->takeAt(0);
		delete item->widget();
	}

	delete backboneblocks;
	delete branchblocks;

	delete backboneblockswidget;
	delete branchblockswidget;

	delete drawpeptideformlayout;
	delete drawpeptideformwidget;

	delete vbox;

	delete mainbox;
}


void cDrawPeptideWidget::closeEvent(QCloseEvent *event) {
	hide();
	event->accept();
}


void cDrawPeptideWidget::setSequence(int peptidetypeindex, QString sequence) {
	separateblocks->setChecked(false);
	peptidetypecombobox->setCurrentIndex(peptidetypeindex);
	sequenceline->setText(sequence);
	sequenceChanged();
	separateblocks->setChecked(true);
}


void cDrawPeptideWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		hide();
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/drawpeptide.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/drawpeptide.html").absoluteFilePath()));
		#endif
	}

	event->accept();
}


void cDrawPeptideWidget::separateBlocksChanged(int state) {
	if (state == Qt::Checked) {
		sequenceline->setDisabled(true);
		numberofblocksbackbone->setDisabled(false);
		backboneblockswidget->setDisabled(false);
		if (((ePeptideType)(peptidetypecombobox->currentIndex()) == branched) || ((ePeptideType)(peptidetypecombobox->currentIndex()) == branchcyclic)) {
			numberofblocksbranch->setDisabled(false);
			branchblockswidget->setDisabled(false);
			branchposition->setDisabled(false);
		}
		else {
			numberofblocksbranch->setDisabled(true);
			branchblockswidget->setDisabled(true);
			branchposition->setDisabled(true);
		}
	}
	else {
		sequenceline->setDisabled(false);
		numberofblocksbackbone->setDisabled(true);
		backboneblockswidget->setDisabled(true);
		numberofblocksbranch->setDisabled(true);
		branchblockswidget->setDisabled(true);
		branchposition->setDisabled(true);
	}

	blocksChanged();
	sequenceChanged();
}


void cDrawPeptideWidget::numberOfBackboneBlocksChanged(int numberofblocks) {
	switch ((ePeptideType)peptidetypecombobox->currentIndex()) {
		case linear:
		case linearpolyketide:
			numberofblocksbackbone->setRange(1, 100);
			branchposition->setRange(1, 1);
			break;
		case cyclic:
		case cyclicpolyketide:
			numberofblocksbackbone->setRange(2, 100);
			branchposition->setRange(1, 1);
			break;
		case branched:
			numberofblocksbackbone->setRange(3, 100);
			branchposition->setRange(2, numberofblocks - 1);
			break;
		case branchcyclic:
			numberofblocksbackbone->setRange(2, 100);
			branchposition->setRange(1, numberofblocks);
			break;
		case other:
			break;
		default:
			break;
	}

	if (numberofblocks > backboneblocks->count()) {
		int itemscount = numberofblocks - backboneblocks->count();
		QLineEdit* line;
		for (int i = 0; i < itemscount; i++) {
			line = new QLineEdit();
			connect(line, SIGNAL(textEdited(const QString&)), this, SLOT(blocksChanged()));
			backboneblocks->addWidget(line);
		}
	}
	else if (numberofblocks < backboneblocks->count()) {
		QLayoutItem* item;
		while (numberofblocks < backboneblocks->count()) {
			item = backboneblocks->takeAt(backboneblocks->count() - 1);
			delete item->widget();
		}
	}

	blocksChanged();
}


void cDrawPeptideWidget::numberOfBranchBlocksChanged(int numberofblocks) {
	if (numberofblocks > branchblocks->count()) {
		int itemscount = numberofblocks - branchblocks->count();
		QLineEdit* line;
		for (int i = 0; i < itemscount; i++) {
			line = new QLineEdit();
			connect(line, SIGNAL(textEdited(const QString&)), this, SLOT(blocksChanged()));
			branchblocks->addWidget(line);
		}
	}
	else if (numberofblocks < branchblocks->count()) {
		QLayoutItem* item;
		while (numberofblocks < branchblocks->count()) {
			item = branchblocks->takeAt(branchblocks->count() - 1);
			delete item->widget();
		}
	}

	blocksChanged();
}


void cDrawPeptideWidget::peptideTypeChanged(int index) {

	linearwidget->hide();
	cyclicwidget->hide();
	branchedwidget->hide();
	branchcyclicwidget->hide();

	switch ((ePeptideType)index) {
		case linear:
		case linearpolyketide:
			linearwidget->show();
			numberOfBackboneBlocksChanged(max(backboneblocks->count(), 1));
			break;
		case cyclic:
		case cyclicpolyketide:
			cyclicwidget->show();
			numberOfBackboneBlocksChanged(max(backboneblocks->count(), 2));
			break;
		case branched:
			branchedwidget->show();
			numberOfBackboneBlocksChanged(max(backboneblocks->count(), 3));
			break;
		case branchcyclic:
			branchcyclicwidget->show();
			numberOfBackboneBlocksChanged(max(backboneblocks->count(), 2));
			break;
		case other:
			break;
		default:
			break;
	}

	separateBlocksChanged(separateblocks->checkState());
}


void cDrawPeptideWidget::sequenceCopy() {
	QApplication::clipboard()->setText(sequenceline->text());
}


void cDrawPeptideWidget::sequenceSetSequence() {
	emit sendSequence(peptidetypecombobox->currentIndex(), sequenceline->text());
}


void cDrawPeptideWidget::sequenceSetTag() {
	emit sendTag(peptidetypecombobox->currentIndex(), sequenceline->text());
}


void cDrawPeptideWidget::sequenceInsert() {
	emit sendInsert(peptidetypecombobox->currentIndex(), sequenceline->text());
}


void cDrawPeptideWidget::blocksChanged() {
	if (!separateblocks->isChecked()) {
		return;
	}

	QString s = "";
	string composition = "";
	int compositioncount = 0;
	cBricksDatabase bricksdb;
	cBrick b;

	int backbonecount = backboneblocks->count();
	while ((backbonecount > 0) && (((QLineEdit *)(backboneblocks->itemAt(backbonecount - 1)->widget()))->text().toStdString().compare("") == 0)) {
		backbonecount--;
	}

	int branchcount = branchblocks->count();
	while ((branchcount > 1) && (((QLineEdit *)(branchblocks->itemAt(branchcount - 1)->widget()))->text().toStdString().compare("") == 0)) {
		branchcount--;
	}

	ePeptideType peptidetype = (ePeptideType)peptidetypecombobox->currentIndex();

	if ((peptidetype == linear) || ((backbonecount > 1) && (peptidetype == cyclic))
		|| (peptidetype == linearpolyketide) || ((backbonecount > 1) && (peptidetype == cyclicpolyketide))) {
		for (int i = 0; i < backbonecount; i++) {
			if (i > 0) {
				s += "-";
				composition += "-";
			}
			s += "[";
			s += ((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->text();
			s += "]";
			composition += to_string(++compositioncount);
			b.clear();
			b.setAcronyms(((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->text().toStdString());
			bricksdb.push_back(b);
		}
	}

	if (((backbonecount > branchposition->value()) && (peptidetype == branched)) || ((backbonecount >= branchposition->value()) && (peptidetype == branchcyclic))) {
		for (int i = 0; i < backbonecount; i++) {
			if (i == branchposition->value() - 1) {
				s += "\\([";
				s += ((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->text();
				s += "]";
				if (composition.size() > 0) {
					composition += "-";
				}
				composition += to_string(++compositioncount);
				b.clear();
				b.setAcronyms(((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->text().toStdString());
				bricksdb.push_back(b);

				for (int j = 0; j < branchcount; j++) {
					s += "-[";
					s += ((QLineEdit *)(branchblocks->itemAt(j)->widget()))->text();
					s += "]";
					if (composition.size() > 0) {
						composition += "-";
					}
					composition += to_string(++compositioncount);
					b.clear();
					b.setAcronyms(((QLineEdit *)(branchblocks->itemAt(j)->widget()))->text().toStdString());
					bricksdb.push_back(b);

				}

				s += "\\)";
			}
			else {
				if ((i > 0) && (i != branchposition->value())) {
					s += "-";
				}
				s += "[";
				s += ((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->text();
				s += "]";
				if (composition.size() > 0) {
					composition += "-";
				}
				composition += to_string(++compositioncount);
				b.clear();
				b.setAcronyms(((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->text().toStdString());
				bricksdb.push_back(b);
			}
		}
	}

	sequenceline->setText(s);
	
	string errormessage = "";
	string sequence = sequenceline->text().toStdString();
	if (checkRegex(peptidetype, sequence, errormessage)) {
		sequencestatuslabel->setText("<font color='green'>correct</font>");
		vector<string> compositionvector;
		compositionvector.push_back(composition);
		drawPeptide(compositionvector, bricksdb, branchposition->value() - 1, branchposition->value() - 1 + branchcount);
	}
	else {
		sequencestatuslabel->setText("<font color='red'>incorrect</font>");
	}
}


void cDrawPeptideWidget::sequenceChanged() {
	if (separateblocks->isChecked()) {
		return;
	}

	string errormessage = "";
	ePeptideType peptidetype = (ePeptideType)peptidetypecombobox->currentIndex();
	string sequence = sequenceline->text().toStdString();
	int start1, start2;

	if (checkRegex(peptidetype, sequence, errormessage)) {

		cBricksDatabase bricksdb;
		cBrick b;
		string s = "";
		string acronym;
		bool insidebrick = false;
		bool found = false;

		for (int i = 0; i < (int)sequence.size(); i++) {
			if (sequence[i] == '[') {
				acronym = "";
				insidebrick = true;
				continue;
			}

			if (sequence[i] == ']') {
				b.clear();
				b.setAcronyms(acronym);
				bricksdb.push_back(b);
				s += to_string(bricksdb.size());
				insidebrick = false;
				continue;
			}

			if (insidebrick) {
				acronym += sequence[i];
			}
			else {
				s += sequence[i];
			}
		}
		sequence = s;

		int branchstart, branchend;
		vector<string> v;
		parseBranch(peptidetype, sequence, v, branchstart, branchend);

		sequencestatuslabel->setText("<font color='green'>correct</font>");
		// the candidate is also initialized
		drawPeptide(v, bricksdb, branchstart, branchend);

		vector<int> intcomposition;
		b.clear();
		b.setComposition(theoreticalspectrum.getCandidate().getComposition(), false);
		b.explodeToIntComposition(intcomposition);

		if ((peptidetype == linear) || (peptidetype == cyclic)
			|| (peptidetype == linearpolyketide) || (peptidetype == cyclicpolyketide)) {
			numberofblocksbackbone->setValue((int)v.size());
			for (int i = 0; i < backboneblocks->count(); i++) {
				if (i < (int)v.size()) {
					((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->setText(bricksdb[intcomposition[i] - 1].getAcronymsAsString().c_str());

				}
				else {
					((QLineEdit *)(backboneblocks->itemAt(i)->widget()))->setText("");
				}
			}
		}

		if ((peptidetype == branched) || (peptidetype == branchcyclic)) {
			branchstart = theoreticalspectrum.getCandidate().getBranchStart();
			branchend = theoreticalspectrum.getCandidate().getBranchEnd();

			numberofblocksbackbone->setValue((int)v.size() - branchend + branchstart);
			numberofblocksbranch->setValue(branchend - branchstart);
			branchposition->setValue(branchstart + 1);

			start1 = 0;
			start2 = 0;
			for (int i = 0; i < (int)v.size(); i++) {
				if ((i > branchstart) && (i <= branchend)) {
					((QLineEdit *)(branchblocks->itemAt(start1)->widget()))->setText(bricksdb[intcomposition[i] - 1].getAcronymsAsString().c_str());
					start1++;
				}
				else {
					((QLineEdit *)(backboneblocks->itemAt(start2)->widget()))->setText(bricksdb[intcomposition[i] - 1].getAcronymsAsString().c_str());
					start2++;
				}
			}

			while (start1 < branchblocks->count()) {
				((QLineEdit *)(branchblocks->itemAt(start1)->widget()))->setText("");
				start1++;
			}

			while (start2 < backboneblocks->count()) {
				((QLineEdit *)(backboneblocks->itemAt(start2)->widget()))->setText("");
				start2++;
			}
		}

	}
	else {
		sequencestatuslabel->setText("<font color='red'>incorrect</font>");
	}
}


void cDrawPeptideWidget::drawPeptide(vector<string>& composition, cBricksDatabase& bricksdb, int branchstart, int branchend) {
	vector<nodeEdge> netmp;
	vector<cCandidate> branchcyclicrotations;
	int numberofbricks;

	switch ((ePeptideType)peptidetypecombobox->currentIndex()) {
		case linear:
		case linearpolyketide:
			theoreticalspectrum.getCandidate().setCandidate(composition, netmp, fragmentIonTypeEnd, 0, 0, 0, -1, -1);
			theoreticalspectrum.getCandidate().setAcronyms(bricksdb);
			linearwidget->initialize(0, &theoreticalspectrum);
			linearwidget->repaint();
			break;
		case cyclic:
		case cyclicpolyketide:
			theoreticalspectrum.getCandidate().setCandidate(composition, netmp, fragmentIonTypeEnd, 0, 0, 0, -1, -1);
			theoreticalspectrum.getCandidate().setAcronyms(bricksdb);
			cyclicwidget->initialize(0, &theoreticalspectrum);
			cyclicwidget->repaint();
			break;
		case branched:
			theoreticalspectrum.getCandidate().setCandidate(composition, netmp, fragmentIonTypeEnd, 0, 0, 0, branchstart, branchend);
			theoreticalspectrum.getCandidate().setBackboneAcronyms(bricksdb);
			theoreticalspectrum.getCandidate().setBranchAcronyms(bricksdb);
			branchedwidget->initialize(0, &theoreticalspectrum);
			branchedwidget->repaint();
			break;
		case branchcyclic:
			theoreticalspectrum.getCandidate().setCandidate(composition, netmp, fragmentIonTypeEnd, 0, 0, 0, branchstart, branchend);

			// normalize the candidate
			theoreticalspectrum.getCandidate().getBranchCyclicRotations(branchcyclicrotations, false);
			numberofbricks = getNumberOfBricks(theoreticalspectrum.getCandidate().getComposition());
			for (int i = 0; i < (int)branchcyclicrotations.size(); i++) {
				if (branchcyclicrotations[i].getBranchEnd() == numberofbricks - 1) {
					vector<string> v;
					v.push_back(branchcyclicrotations[i].getComposition());
					vector<nodeEdge> cpath = theoreticalspectrum.getCandidate().getPath();
					theoreticalspectrum.getCandidate().setCandidate(v, cpath, theoreticalspectrum.getCandidate().getStartIonType(), theoreticalspectrum.getCandidate().getStartModifID(), theoreticalspectrum.getCandidate().getEndModifID(), theoreticalspectrum.getCandidate().getMiddleModifID(), branchcyclicrotations[i].getBranchStart(), branchcyclicrotations[i].getBranchEnd());
					break;
				}
			}

			theoreticalspectrum.getCandidate().setBackboneAcronyms(bricksdb);
			theoreticalspectrum.getCandidate().setBranchAcronyms(bricksdb);
			branchcyclicwidget->initialize(0, &theoreticalspectrum);
			branchcyclicwidget->repaint();
			break;
		case other:
			break;
		default:
			break;
	}
}

