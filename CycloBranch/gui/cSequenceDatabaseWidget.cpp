#include "gui/cSequenceDatabaseWidget.h"
#include "gui/cMainThread.h"
#include "gui/cEventFilter.h"

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
#include <QKeyEvent>
#include <QIcon>
#include <QLineEdit>


cSequenceDatabaseWidget::cSequenceDatabaseWidget(QWidget* parent) {
	this->parent = parent;

	setWindowTitle("Sequence Database Editor");
	setWindowIcon(QIcon(":/images/icons/26.png"));

	insertrow = new QPushButton(tr("Add Row"));
	insertrow->setToolTip("Add a new row.");
	insertrow->setShortcut(QKeySequence(Qt::Key_Insert));

	removechecked = new QPushButton(tr(" Remove Rows "));
	removechecked->setToolTip("Remove selected rows.");
	removechecked->setShortcut(QKeySequence(Qt::Key_Delete));

	close = new QPushButton(tr("Close"));
	close->setToolTip("Close the window.");

	load = new QPushButton(tr("Load"));
	load->setToolTip("Load the database of sequences.");
	load->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));

	save = new QPushButton(QString(" Save "));
	save->setToolTip("Save the database of sequences in the current file. When a file has not been loaded yet, the \"Save As ...\" file dialog is opened.");

	saveas = new QPushButton(tr("Save As..."));
	saveas->setToolTip("Save the database of sequences into a file.");
	saveas->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

	rowsfilterline = new QLineEdit();
	rowsfilterline->setMinimumWidth(250);
	rowsfilterline->setToolTip("Text to Find");

	rowsfiltercasesensitive = new QCheckBox();
	rowsfiltercasesensitive->setToolTip("Case Sensitive");

	rowsfilterbutton = new QPushButton("Filter");
	rowsfilterbutton->setToolTip("Filter Search Results");
	rowsfilterbutton->setMinimumWidth(50);

	rowsfilterclearbutton = new QPushButton("Reset");
	rowsfilterclearbutton->setToolTip("Reset Search Results");
	rowsfilterclearbutton->setMinimumWidth(50);
	rowsfilterclearbutton->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));

	rowsfilterhbox = new QHBoxLayout();
	rowsfilterhbox->addWidget(rowsfilterline);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfiltercasesensitive);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfilterbutton);
	rowsfilterhbox->addStretch();
	rowsfilterhbox->addWidget(rowsfilterclearbutton);

	rowsfilterwidget = new QWidget();
	rowsfilterwidget->setLayout(rowsfilterhbox);
	rowsfilterwidget->setMaximumWidth(420);

	buttons = new QHBoxLayout();
	buttons->addWidget(close);
	buttons->addStretch();
	buttons->addWidget(insertrow);
	buttons->addWidget(removechecked);
	buttons->addStretch();
	buttons->addWidget(rowsfilterwidget);
	buttons->addStretch();
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
	database->setItemDelegateForColumn(4, &columndelegate);
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

	connect(database->horizontalHeader(), SIGNAL(sectionDoubleClicked(int)), this, SLOT(headerItemDoubleClicked(int)));
	connect(database, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged(QTableWidgetItem *)));
	connect(insertrow, SIGNAL(released()), this, SLOT(addRow()));
	connect(removechecked, SIGNAL(released()), this, SLOT(removeCheckedRows()));
	connect(close, SIGNAL(released()), this, SLOT(closeWindow()));
	connect(load, SIGNAL(released()), this, SLOT(loadDatabase()));
	connect(save, SIGNAL(released()), this, SLOT(saveDatabase()));
	connect(saveas, SIGNAL(released()), this, SLOT(saveDatabaseAs()));
	connect(rowsfilterbutton, SIGNAL(released()), this, SLOT(filterRows()));
	connect(rowsfilterclearbutton, SIGNAL(released()), this, SLOT(resetFilter()));

	setLayout(mainlayout);

	resize(1280, 700);

	databasefile = "";

	#if OS_TYPE == WIN
		lastdir = "./SequenceDatabases/";
	#else
		lastdir = installdir + "SequenceDatabases/";
	#endif

	sequences.clear();

	setDataModified(false);
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

	delete rowsfilterline;
	delete rowsfiltercasesensitive;
	delete rowsfilterbutton;
	delete rowsfilterclearbutton;
	delete rowsfilterhbox;
	delete rowsfilterwidget;

	delete database;
	delete buttons;
	delete mainlayout;
}


void cSequenceDatabaseWidget::closeEvent(QCloseEvent *event) {
	closeWindow();
}


void cSequenceDatabaseWidget::insertRow(int peptidetypeindex, QString sequence) {
	addRow();
	((QComboBox *)(database->cellWidget(database->rowCount() - 1, 1)))->setCurrentIndex(peptidetypeindex);
	database->item(database->rowCount() - 1, 5)->setText(sequence);

	for (int i = 0; i < database->columnCount(); i++) {
		database->resizeColumnToContents(i);
	}
}


void cSequenceDatabaseWidget::deleteTable(bool enableprogress) {
	QProgressDialog* progress;
	cEventFilter filter;
	int rowcount = database->rowCount();

	if (enableprogress) {
		progress = new QProgressDialog("Clearing the table...", 0, 0, rowcount, this);
		progress->setMinimumWidth(250);
		progress->installEventFilter(&filter);
		progress->setMinimumDuration(0);
		progress->setWindowModality(Qt::WindowModal);
		progress->setValue(0);
	}

	widgetitemallocator.reset();

	for (int i = 0; i < rowcount; i++) {
		delete database->cellWidget(rowcount - i - 1, 0);
		delete database->cellWidget(rowcount - i - 1, 1);
		delete database->cellWidget(rowcount - i - 1, 10);

		if (enableprogress) {
			progress->setValue(i);
		}
	}

	database->setRowCount(0);

	if (enableprogress) {
		progress->setValue(rowcount);
		delete progress;
	}
}


void cSequenceDatabaseWidget::removeRow(int row) {
	for (int i = 0; i < database->columnCount(); i++) {
		if ((i == 0) || (i == 1) || (i == 10)) {
			delete database->cellWidget(row, i);
		}
		else {
			database->takeItem(row, i);
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


bool cSequenceDatabaseWidget::checkFormula(int row, const string& summary) {
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
		database->item(row, 4)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(formula.getMass()));
	}
	return true;
}


bool cSequenceDatabaseWidget::checkSequence(int row) {

	if (database->item(row, 5)->text().toStdString().compare("") == 0) {
		return true;
	}

	regex rx;
	// [^\\[\\]]+ is used instead of .+ to prevent from a too complex regex error
	switch ((ePeptideType)((QComboBox *)database->cellWidget(row, 1))->currentIndex())
	{
	case linear:
	case linearpolysaccharide:
#if OLIGOKETIDES == 1
	case linearoligoketide:
#endif
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case cyclic:
#if OLIGOKETIDES == 1
	case cyclicoligoketide:
#endif
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+$";
		break;
	case branched:
		rx = "^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$";
		break;
	case branchcyclic:
		rx = "(^(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*$|^\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*\\\\\\(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])+\\\\\\)(\\[[^\\[\\]]+\\](-\\[[^\\[\\]]+\\])*)?$)";
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
			errstr += getStringFromPeptideType((ePeptideType)((QComboBox *)database->cellWidget(row, 1))->currentIndex()).c_str();
			errstr += "'.";
			msgBox.setText(errstr);
			msgBox.exec();
			return false;
		}
	}
	catch (regex_error& e) {
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


void cSequenceDatabaseWidget::setDataModified(bool datamodified) {
	if (datamodified == this->datamodified) {
		return;
	}

	this->datamodified = datamodified;

	if (datamodified) {
		save->setText("*" + save->text());
	}
	else {
		if ((save->text().size() > 0) && (save->text().at(0) == '*')) {
			save->setText(save->text().toStdString().substr(1).c_str());
		}
	}
}


void cSequenceDatabaseWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
		closeWindow();
    }

	if ((event->key() == Qt::Key_Enter) || (event->key() == Qt::Key_Return)) {
		if (rowsfilterline->hasFocus()) {
			filterRows();
		}
    }

	if (event->key() == Qt::Key_F1) {
		#if OS_TYPE == WIN
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo("docs/html/sequenceeditor.html").absoluteFilePath()));
		#else
			QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(installdir + "docs/html/sequenceeditor.html").absoluteFilePath()));
		#endif
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_F)) {
		rowsfilterline->setFocus();
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_N)) {
		rowsfiltercasesensitive->setChecked(!rowsfiltercasesensitive->isChecked());
	}

	if ((event->modifiers() == Qt::ControlModifier) && (event->key() == Qt::Key_S)) {
		saveDatabase();
	}
}


void cSequenceDatabaseWidget::closeWindow() {
	if (datamodified) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, appname, "Save changes ?", QMessageBox::Yes|QMessageBox::No);
	
		if (reply == QMessageBox::Yes) {
			if (!saveDatabase()) {
				return;
			}
		}
	}

	hide();
}


void cSequenceDatabaseWidget::loadDatabase() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Load the Database of Sequences"), lastdir, tr("Database of Sequences (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;

		databasefile = filename;
		save->setText(QString(" Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("' "));
		
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

			QProgressDialog progress("Loading the Sequence Database...", "Cancel", 0, sequences.size(), this);
			progress.setMinimumWidth(250);
			cEventFilter filter;
			progress.installEventFilter(&filter);
			progress.setMinimumDuration(0);
			progress.setWindowModality(Qt::WindowModal);

            cSummaryFormula formula;
            database->setRowCount(sequences.size());
            for (int i = 0; i < sequences.size(); i++) {
				QCheckBox* checkbox = new QCheckBox();
				database->setCellWidget(i, 0, checkbox);

                QComboBox* combo = new QComboBox();
				for (int j = 0; j <= (int)other; j++) {
                    combo->addItem(QString(getStringFromPeptideType((ePeptideType)j).c_str()));
                }
                combo->setCurrentIndex((int)sequences[i].getPeptideType());
				connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxModified(int)));
                database->setCellWidget(i, 1, combo);

                database->setItem(i, 2, widgetitemallocator.getNewItem());
				database->item(i, 2)->setText(sequences[i].getName().c_str());

                database->setItem(i, 3, widgetitemallocator.getNewItem());
				database->item(i, 3)->setText(sequences[i].getSummaryFormula().c_str());

				formula.setFormula(sequences[i].getSummaryFormula());
                database->setItem(i, 4, widgetitemallocator.getNewItem());
				database->item(i, 4)->setData(Qt::DisplayRole, cropPrecisionToSixDecimals(formula.getMass()));

				database->setItem(i, 5, widgetitemallocator.getNewItem());
				database->item(i, 5)->setText(sequences[i].getSequence().c_str());

				database->setItem(i, 6, widgetitemallocator.getNewItem());
				database->item(i, 6)->setText(sequences[i].getNTterminalModification().c_str());

				database->setItem(i, 7, widgetitemallocator.getNewItem());
				database->item(i, 7)->setText(sequences[i].getCTterminalModification().c_str());

				database->setItem(i, 8, widgetitemallocator.getNewItem());
				database->item(i, 8)->setText(sequences[i].getBranchModification().c_str());

				database->setItem(i, 9, widgetitemallocator.getNewItem());
				database->item(i, 9)->setText(sequences[i].getReference().c_str());

                database->setCellWidget(i, 10, new QLabel(sequences[i].getNameWithReferenceAsHTMLString().c_str()));
                ((QLabel *)database->cellWidget(i, 10))->setTextFormat(Qt::RichText);
                ((QLabel *)database->cellWidget(i, 10))->setTextInteractionFlags(Qt::TextBrowserInteraction);
                ((QLabel *)database->cellWidget(i, 10))->setOpenExternalLinks(true);

				progress.setValue(i);
				if (progress.wasCanceled()) {
					deleteTable(true);
					sequences.clear();
					databasefile = "";
					save->setText(" Save ");
					break;
				}
			}

			for (int i = 0; i < database->columnCount(); i++) {
				database->resizeColumnToContents(i);
			}

			progress.setValue(sequences.size());

			setDataModified(false);
						
		}

		inputstream.close();

	}
}


bool cSequenceDatabaseWidget::saveDatabase() {
	bool saved = false;

	if (!checkTable()) {
		return saved;
	}

	if (databasefile.compare("") == 0) {
		return saveDatabaseAs();
	}

	outputstream.open(databasefile.toStdString().c_str());
	if (!outputstream.good()) {
		QMessageBox msgBox;
		msgBox.setText("Cannot open the file '" + databasefile + "'.");
		msgBox.exec();
	}
	else {

		QProgressDialog progress("Saving the Sequence Database...", 0, 0, database->rowCount(), this);
		progress.setMinimumWidth(250);
		cEventFilter filter;
		progress.installEventFilter(&filter);
		progress.setMinimumDuration(0);
		progress.setWindowModality(Qt::WindowModal);

		cSequence seq;
		sequences.clear();
		string s;

		removeCheckedRows();

		for (int i = 0; i < database->rowCount(); i++) {

			if (database->isRowHidden(i)) {
				progress.setValue(i);
				continue;
			}

			seq.clear();
			for (int j = 0; j < database->columnCount(); j++) {
				switch (j)
				{
				case 0:
					// nothing to do
					break;
				case 1:
					seq.setPeptideType((ePeptideType)(((QComboBox *)database->cellWidget(i,j))->currentIndex()));
					break;
				case 2:
					s = database->item(i,j)->text().toStdString();
					seq.setName(removeWhiteSpacesExceptSpaces(s));
					break;
				case 3:
					s = database->item(i,j)->text().toStdString();
					seq.setSummaryFormula(removeWhiteSpacesExceptSpaces(s));
					break;
				case 4:
					// nothing to do
					break;
				case 5:
					s = database->item(i,j)->text().toStdString();
					seq.setSequence(removeWhiteSpacesExceptSpaces(s));
					break;
				case 6:
					s = database->item(i,j)->text().toStdString();
					seq.setNTterminalModification(removeWhiteSpacesExceptSpaces(s));
					break;
				case 7:
					s = database->item(i,j)->text().toStdString();
					seq.setCTterminalModification(removeWhiteSpacesExceptSpaces(s));
					break;
				case 8:
					s = database->item(i,j)->text().toStdString();
					seq.setBranchModification(removeWhiteSpacesExceptSpaces(s));
					break;
				case 9:
					s = database->item(i,j)->text().toStdString();
					seq.setReference(removeWhiteSpacesExceptSpaces(s));
					break;
				default:
					break;
				}
			}
			sequences.push_back(seq);
			
			progress.setValue(i);
		}

		sequences.storeToPlainTextStream(outputstream);

		progress.setValue(progress.maximum());
		
		setDataModified(false);
		
		saved = true;

	}
	outputstream.close();

	return saved;
}


bool cSequenceDatabaseWidget::saveDatabaseAs() {
	if (!checkTable()) {
		return false;
	}

	QString filename = QFileDialog::getSaveFileName(this, tr("Save the Database of Sequences As..."), lastdir, tr("Database of Sequences (*.txt)"));

	if (!filename.isEmpty()) {
		lastdir = filename;
	
		databasefile = filename;
		save->setText(QString(" Save '") + QString(databasefile.toStdString().substr(databasefile.toStdString().rfind('/') + 1, databasefile.toStdString().size()).c_str()) + QString("' "));
		return saveDatabase();
	}

	return false;
}


void cSequenceDatabaseWidget::addRow() {
	int row = database->rowCount();
	database->insertRow(row);

	QCheckBox* checkbox = new QCheckBox();
	database->setCellWidget(row, 0, checkbox);

	QComboBox* combo = new QComboBox();
	for (int i = 0; i <= (int)other; i++) {
		combo->addItem(QString(getStringFromPeptideType((ePeptideType)i).c_str()));
	}
	combo->setCurrentIndex((int)other);
	connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxModified(int)));
	database->setCellWidget(row, 1, combo);

	database->setItem(row, 2, widgetitemallocator.getNewItem());
	database->setItem(row, 3, widgetitemallocator.getNewItem());
	database->setItem(row, 4, widgetitemallocator.getNewItem());
	database->setItem(row, 5, widgetitemallocator.getNewItem());
	database->setItem(row, 6, widgetitemallocator.getNewItem());
	database->setItem(row, 7, widgetitemallocator.getNewItem());
	database->setItem(row, 8, widgetitemallocator.getNewItem());
	database->setItem(row, 9, widgetitemallocator.getNewItem());

	database->setCellWidget(row, 10, new QLabel());
	((QLabel *)database->cellWidget(row, 10))->setTextFormat(Qt::RichText);
	((QLabel *)database->cellWidget(row, 10))->setTextInteractionFlags(Qt::TextBrowserInteraction);
	((QLabel *)database->cellWidget(row, 10))->setOpenExternalLinks(true);
}


void cSequenceDatabaseWidget::removeCheckedRows() {
	int i = 0;
	while (i < database->rowCount()) {
		if (((QCheckBox *)(database->cellWidget(i, 0)))->isChecked()) {
			removeRow(i);
			setDataModified(true);
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

	setDataModified(true);
}


void cSequenceDatabaseWidget::headerItemDoubleClicked(int index) {
	setDataModified(true);

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


void cSequenceDatabaseWidget::filterRows() {
	Qt::CaseSensitivity casesensitive = rowsfiltercasesensitive->isChecked()?Qt::CaseSensitive:Qt::CaseInsensitive;
	QString str = rowsfilterline->text();
	int rowcount = database->rowCount();
	bool match;
	int i, j;

	QProgressDialog progress("Updating...", "Cancel", 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (i = 0; i < rowcount; i++) {

		match = false;
		for (j = 0; j < database->columnCount(); j++) {
			// ignore non-text fields
			if ((j == 0) || (j == 1) || (j == 10)) {
				continue;
			}

			if (database->item(i, j)->text().contains(str, casesensitive)) {
				match = true;
				break;
			}
		}
		database->setRowHidden(i, !match);
		progress.setValue(i);

		if (progress.wasCanceled()) {
			resetFilter();
			break;
		}

	}

	progress.setValue(rowcount);
}


void cSequenceDatabaseWidget::resetFilter() {
	rowsfilterline->setText("");
	int rowcount = database->rowCount();

	QProgressDialog progress("Updating...", 0, 0, rowcount, this);
	progress.setMinimumWidth(250);
	cEventFilter filter;
	progress.installEventFilter(&filter);
	progress.setMinimumDuration(0);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < rowcount; i++) {
		database->setRowHidden(i, false);
		progress.setValue(i);
	}

	progress.setValue(rowcount);
}


void cSequenceDatabaseWidget::comboBoxModified(int index) {
	setDataModified(true);
}

