#include "gui/cSequenceDatabaseWidget.h"
#include "gui/cMainThread.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QLabel>
#include <QComboBox>
#include <QProgressDialog>
#include <QCheckBox>


cSequenceDatabaseWidget::cSequenceDatabaseWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Sequence Database Editor");

	insertrow = new QPushButton(tr("Add Row"));
	insertrow->setToolTip("Add a new row.");
	removechecked = new QPushButton(tr(" Remove Rows "));
	removechecked->setToolTip("Remove selected rows.");

	close = new QPushButton(tr("Close"));
	close->setToolTip("Close the window.");
	load = new QPushButton(tr("Load"));
	load->setToolTip("Load the database of sequences.");
	save = new QPushButton(QString("Save"));
	save->setToolTip("Save the database of sequences in the current file. When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");
	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save the database of sequences into a file.");

	buttons = new QHBoxLayout();
	buttons->addWidget(close);
	buttons->addStretch(1);
	buttons->addWidget(insertrow);
	buttons->addWidget(removechecked);
	buttons->addStretch(10);
	buttons->addWidget(load);
	buttons->addWidget(save);
	buttons->addWidget(saveas);

	database = new QTableWidget(0, 0, this);
	database->setColumnCount(11);
	database->setHorizontalHeaderItem(0, new QTableWidgetItem(""));
	database->setHorizontalHeaderItem(1, new QTableWidgetItem("Type"));
	database->setHorizontalHeaderItem(2, new QTableWidgetItem("Name"));
	database->setHorizontalHeaderItem(3, new QTableWidgetItem("Summary Formula"));
	database->setHorizontalHeaderItem(4, new QTableWidgetItem("Monoisotopic Mass"));
	database->setHorizontalHeaderItem(5, new QTableWidgetItem("Sequence"));
	database->setHorizontalHeaderItem(6, new QTableWidgetItem("N-terminal Modification"));
	database->setHorizontalHeaderItem(7, new QTableWidgetItem("C-terminal Modification"));
	database->setHorizontalHeaderItem(8, new QTableWidgetItem("Branch Modification"));
	database->setHorizontalHeaderItem(9, new QTableWidgetItem("Reference"));
	database->setHorizontalHeaderItem(10, new QTableWidgetItem("Preview"));
	database->horizontalHeader()->setStretchLastSection(true);
	for (int i = 0; i < database->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}

	headersort.resize(database->columnCount());
	for (int i = 0; i < database->columnCount(); i++) {
		headersort[i] = -1;
	}

	mainlayout = new QVBoxLayout();
	mainlayout->addWidget(database);
	mainlayout->addLayout(buttons);

	progress = new QProgressDialog(this);
	progress->setCancelButton(0);
	progress->setMinimumDuration(1000);
	progress->setWindowModality(Qt::WindowModal);

	connect(database->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerItemDoubleClicked(int)));
	connect(database, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged(QTableWidgetItem *)));
	connect(insertrow, SIGNAL(released()), this, SLOT(addRow()));
	connect(removechecked, SIGNAL(released()), this, SLOT(removeEmptyRows()));
	connect(close, SIGNAL(released()), this, SLOT(closeWindow()));
	connect(load, SIGNAL(released()), this, SLOT(loadDatabase()));
	connect(save, SIGNAL(released()), this, SLOT(saveDatabase()));
	connect(saveas, SIGNAL(released()), this, SLOT(saveDatabaseAs()));

	setLayout(mainlayout);

	resize(1280, 700);

	databasefile = "";
	lastdir = "./SequenceDatabases/";
	sequences.clear();
}


cSequenceDatabaseWidget::~cSequenceDatabaseWidget() {
	deleteTable(false);

	for (int i = 0; i < database->columnCount(); i++) {
		delete database->horizontalHeaderItem(i);
	}

	database->setColumnCount(0);

	delete insertrow;
	delete removechecked;
	delete close;
	delete load;
	delete save;
	delete saveas;
	delete database;
	delete buttons;
	delete mainlayout;
	delete progress;
}


void cSequenceDatabaseWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
}


void cSequenceDatabaseWidget::deleteTable(bool enableprogress) {
	if (enableprogress) {
		progress->setLabelText("Clearing the table...");
		progress->setMinimum(0);
		progress->setValue(1);
		progress->setMaximum(database->rowCount());
		progress->show();
	}

	for (int i = 0; i < database->rowCount(); i++) {
		for (int j = 0; j < database->columnCount(); j++) {
			if ((j == 0) || (j == 1) || (j == 10)) {
				delete database->cellWidget(i, j);
			}
			else {
				delete database->item(i, j);
			}		
		}

		if (enableprogress && ((i == 0) || ((i - 1)/100 != i/100))) {
			progress->setValue(i);
		}
	}

	if (enableprogress) {
		progress->setValue(database->rowCount());
		progress->hide();
	}
	database->setRowCount(0);
}


void cSequenceDatabaseWidget::removeRow(int row) {
	for (int i = 0; i < database->columnCount(); i++) {
		if ((i == 0) || (i == 1) || (i == 10)) {
			delete database->cellWidget(row, i);
		}
		else {
			delete database->item(row, i);
		}
	}
	database->removeRow(row);
}


bool cSequenceDatabaseWidget::checkTable() {

	// check summary formulas
	for (int i = 0; i < database->rowCount(); i++) {
		if (!checkFormula(i, database->item(i, 3)->text().toStdString())) {
			return false;
		}
	}

	// check sequences
	for (int i = 0; i < database->rowCount(); i++) {
		if (!checkSequence(i)) {
			return false;
		}
	}

	return true;
}


bool cSequenceDatabaseWidget::checkFormula(int row, string& summary) {
	cSummaryFormula formula;
	string errmsg;
	formula.setFormula(summary);
	if (!formula.isValid(errmsg)) {
		QMessageBox msgBox;
		QString errstr = "Syntax error in the row no. ";
		errstr += to_string(row + 1).c_str();
		errstr += ": ";
		errstr += errmsg.c_str();
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}
	if (database->item(row, 4)) {
		database->item(row, 4)->setData(Qt::DisplayRole, formula.getMass());
	}
	return true;
}


bool cSequenceDatabaseWidget::checkSequence(int row) {
	regex rx;
	// [^\\[\\]]+ is used instead of .+ to prevent from a too complex regex error
	switch ((peptideType)((QComboBox *)database->cellWidget(row, 1))->currentIndex())
	{
	case linear:
	case cyclic:
	case linearpolysaccharide:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case branched:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case lasso:
		rx = "(^(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$|^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\)(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?$)";
		break;
	case other:
	default:
		rx = ".*";
		break;
	}

	try {
		if (!(regex_search(database->item(row, 5)->text().toStdString(), rx))) {
			QMessageBox msgBox;
			QString errstr = "Row no. ";
			errstr += to_string(row + 1).c_str();
			errstr += ". The format of sequence '";
			errstr += database->item(row, 5)->text().toStdString().c_str();
			errstr += "' does not correspond to the sequence type '";
			errstr += getStringFromPeptideType((peptideType)((QComboBox *)database->cellWidget(row, 1))->currentIndex()).c_str();
			errstr += "'.";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}
	catch (std::regex_error& e) {
		QMessageBox msgBox;
		QString errstr = "Row no. ";
		errstr += to_string(row + 1).c_str();
		errstr += ". cSequenceDatabaseWidget::checkSequence: regex_search failed, error no. ";
		errstr += to_string(e.code()).c_str();
		errstr += ". Please, check the sequence: '";
		errstr += database->item(row, 5)->text().toStdString().c_str();
		errstr += "'.";
		msgBox.setText(errstr);
		msgBox.exec();
		return false;
	}

	return true;
}


void cSequenceDatabaseWidget::closeWindow() {
	hide();
}


void cSequenceDatabaseWidget::loadDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load the Database of Sequences"), lastdir, tr("Database of Sequences (*.txt)"));
	lastdir = filename;

	if (filename.toStdString().compare("") != 0) {

		databasefile = filename;
		save->setText(QString("  Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("'  "));
		
		inputstream.open(filename.toStdString().c_str());

		if (!inputstream.good()) {
			QMessageBox msgBox;
			msgBox.setText("Cannot open the file '" + databasefile + "'.");
			msgBox.exec();
		}
		else {

			deleteTable(true);

			sequences.clear();
			sequences.loadFromPlainTextStream(inputstream);

			progress->setLabelText("Loading the Sequence Databatase...");
			progress->setMinimum(0);
			progress->setValue(1);
			progress->setMaximum(sequences.size());
			progress->show();

            cSummaryFormula formula;
            database->setRowCount(sequences.size());
            for (int i = 0; i < sequences.size(); i++) {
				QCheckBox* checkbox = new QCheckBox();
				database->setCellWidget(i, 0, checkbox);

                QComboBox* combo = new QComboBox();
				for (int j = 0; j <= (int)other; j++) {
                    combo->addItem(QString(getStringFromPeptideType((peptideType)j).c_str()));
                }
                combo->setCurrentIndex((int)sequences[i].getPeptideType());
                database->setCellWidget(i, 1, combo);

                database->setItem(i, 2, new QTableWidgetItem(sequences[i].getName().c_str()));
                database->setItem(i, 3, new QTableWidgetItem(sequences[i].getSummaryFormula().c_str()));
                formula.setFormula(sequences[i].getSummaryFormula());
                database->setItem(i, 4, new QTableWidgetItem());
                database->item(i, 4)->setData(Qt::DisplayRole, formula.getMass());
                database->setItem(i, 5, new QTableWidgetItem(sequences[i].getSequence().c_str()));
                database->setItem(i, 6, new QTableWidgetItem(sequences[i].getNTterminalModification().c_str()));
                database->setItem(i, 7, new QTableWidgetItem(sequences[i].getCTterminalModification().c_str()));
                database->setItem(i, 8, new QTableWidgetItem(sequences[i].getBranchModification().c_str()));
                database->setItem(i, 9, new QTableWidgetItem(sequences[i].getReference().c_str()));

                database->setCellWidget(i, 10, new QLabel(sequences[i].getNameWithReferenceAsHTMLString().c_str()));
                ((QLabel *)database->cellWidget(i, 10))->setTextFormat(Qt::RichText);
                ((QLabel *)database->cellWidget(i, 10))->setTextInteractionFlags(Qt::TextBrowserInteraction);
                ((QLabel *)database->cellWidget(i, 10))->setOpenExternalLinks(true);

                if ((i == 0) || ((i - 1)/100 != i/100)) {
                    progress->setValue(i);
                }
            }

			for (int i = 0; i < database->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}

			progress->setValue(sequences.size());
			progress->hide();
			
		}

		inputstream.close();

	}
}


void cSequenceDatabaseWidget::saveDatabase() {

	if (!checkTable()) {
		return;
	}

	if (databasefile.compare("") == 0) {
		saveDatabaseAs();
		return;
	}

	outputstream.open(databasefile.toStdString().c_str());
	if (!outputstream.good()) {
		QMessageBox msgBox;
		msgBox.setText("Cannot open the file '" + databasefile + "'.");
		msgBox.exec();
	}
	else {

		progress->setLabelText("Saving the Sequence Databatase...");
		progress->setMinimum(0);
		progress->setMaximum(100);
		progress->setValue(1);
		progress->show();

		cSequence seq;
		sequences.clear();

		removeEmptyRows();

		progress->setMaximum(database->rowCount());

		for (int i = 0; i < database->rowCount(); i++) {
			seq.clear();
			for (int j = 0; j < database->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					seq.setPeptideType((peptideType)(((QComboBox *)database->cellWidget(i,j))->currentIndex()));
					break;
				case 2:
					seq.setName(database->item(i,j)->text().toStdString());
					break;
				case 3:
					seq.setSummaryFormula(database->item(i,j)->text().toStdString());
					break;
				case 4:
					// nothing to do
					break;
				case 5:
					seq.setSequence(database->item(i,j)->text().toStdString());
					break;
				case 6:
					seq.setNTterminalModification(database->item(i,j)->text().toStdString());
					break;
				case 7:
					seq.setCTterminalModification(database->item(i,j)->text().toStdString());
					break;
				case 8:
					seq.setBranchModification(database->item(i,j)->text().toStdString());
					break;
				case 9:
					seq.setReference(database->item(i,j)->text().toStdString());
					break;
				default:
					break;
				}
			}
			sequences.push_back(seq);
			
			if ((i == 0) || ((i - 1)/100 != i/100)) {
				progress->setValue(i);
			}
		}

		sequences.storeToPlainTextStream(outputstream);

		progress->setValue(database->rowCount());
		progress->hide();
		
	}
	outputstream.close();
}


void cSequenceDatabaseWidget::saveDatabaseAs() {

	if (!checkTable()) {
		return;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Settings As..."), lastdir, tr("Database of Sequences (*.txt)"));
	lastdir = filename;

	if (filename.toStdString().compare("") != 0) {
		databasefile = filename;
		save->setText(QString("  Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("'  "));
		saveDatabase();
	}
}


void cSequenceDatabaseWidget::addRow() {
	int row = database->rowCount();
	database->insertRow(row);

	QCheckBox* checkbox = new QCheckBox();
	database->setCellWidget(row, 0, checkbox);

	QComboBox* combo = new QComboBox();
	for (int i = 0; i <= (int)other; i++) {
		combo->addItem(QString(getStringFromPeptideType((peptideType)i).c_str()));
	}
	combo->setCurrentIndex((int)other);
	database->setCellWidget(row, 1, combo);

	database->setItem(row, 2, new QTableWidgetItem());
	database->setItem(row, 3, new QTableWidgetItem());
	database->setItem(row, 4, new QTableWidgetItem());
	database->setItem(row, 5, new QTableWidgetItem());
	database->setItem(row, 6, new QTableWidgetItem());
	database->setItem(row, 7, new QTableWidgetItem());
	database->setItem(row, 8, new QTableWidgetItem());
	database->setItem(row, 9, new QTableWidgetItem());

	database->setCellWidget(row, 10, new QLabel());
	((QLabel *)database->cellWidget(row, 10))->setTextFormat(Qt::RichText);
	((QLabel *)database->cellWidget(row, 10))->setTextInteractionFlags(Qt::TextBrowserInteraction);
	((QLabel *)database->cellWidget(row, 10))->setOpenExternalLinks(true);
}


void cSequenceDatabaseWidget::removeEmptyRows() {
	int i = 0;
	while (i < database->rowCount()) {
		if (((QCheckBox *)(database->cellWidget(i, 0)))->isChecked()) {
			removeRow(i);
		}
		else {
			i++;
		}
	}
}


void cSequenceDatabaseWidget::itemChanged(QTableWidgetItem* item) {
	// recalculate mass when formula is changed
	if (item->column() == 3) {
		checkFormula(item->row(), item->text().toStdString());
	}

	// check sequence
	if (item->column() == 5) {
		checkSequence(item->row());
	}

	// update references preview
	if (((item->column() == 2) || (item->column() == 9)) && database->cellWidget(item->row(), 10)) {
		cSequence seq;
		seq.setName(database->item(item->row(), 2)->text().toStdString());
		seq.setReference(database->item(item->row(), 9)->text().toStdString());
		((QLabel *)database->cellWidget(item->row(), 10))->setText(seq.getNameWithReferenceAsHTMLString().c_str());
	}
}


void cSequenceDatabaseWidget::headerItemDoubleClicked(int index) {
	if (headersort[index] == -1) {
		database->sortByColumn(index, Qt::AscendingOrder);
		headersort[index] = 1;
		return;
	}

	if (headersort[index] == 0) {
		database->sortByColumn(index, Qt::AscendingOrder);
		headersort[index] = 1;
	}
	else {
		database->sortByColumn(index, Qt::DescendingOrder);
		headersort[index] = 0;
	}
}

