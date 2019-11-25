#include "gui/cLinearWidget.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QMessageBox>
#include <QPrinter>
#include <QSvgGenerator>


bool operator == (cIonLabel const& a, cIonLabel const& b) {
	return ((a.x == b.x) && (a.y == b.y)); 
}


void insertLabel(unordered_set<cIonLabel, hash_cIonLabel>& labels, int x, int y, string& description, bool alignright) {
	cIonLabel newlabel;
	newlabel.x = x;
	newlabel.y = y;
	newlabel.labelset.insert(description);
	newlabel.alignright = alignright;

	if (labels.count(newlabel) == 1) {
		auto it = labels.find(newlabel);
		newlabel.labelset = it->labelset;
		newlabel.labelset.insert(description);
		labels.erase(it);
	}

	labels.insert(newlabel);
}


void drawLabels(QPainter& painter, unordered_set<cIonLabel, hash_cIonLabel>& labels, int width) {
	QFont myFont("Courier", 9);
	QFontMetrics fm(myFont);
	string labeldesc;

	painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
	for (auto& it : labels) {
		labeldesc = "";
		for (auto& it2 : it.labelset) {
			labeldesc += it2 + ",";
		}
		if (labeldesc.size() > 0) {
			labeldesc = labeldesc.substr(0, labeldesc.size() - 1);
		}
		labeldesc.erase(remove_if(begin(labeldesc), end(labeldesc), isWhiteSpace), end(labeldesc));
		if (it.alignright) {
			painter.drawText(it.x - fm.width(labeldesc.c_str()), it.y, fm.width(labeldesc.c_str()), 20, Qt::AlignLeft, labeldesc.c_str());
		}
		else {
			painter.drawText(it.x, it.y, width, 20, Qt::AlignLeft, labeldesc.c_str());
		}
	}
}


cLinearWidget::cLinearWidget() {
	parameters = 0;
	theoreticalspectrum = 0;

	visibleionseries = "";
	visibleneutralloss = "all";

	reportisomers = false;
	hidelabels = false;
}


void cLinearWidget::initialize(cParameters* parameters, cTheoreticalSpectrum* theoreticalspectrum) {
	this->parameters = parameters;
	this->theoreticalspectrum = theoreticalspectrum;
}


void cLinearWidget::exportToPDF(QString filename, bool postscript) {
	QPrinter printer;
	if (postscript) {
		printer.setPaperSize(QSizeF(width() + 100, height() + 100), QPrinter::DevicePixel);
		printer.setPageMargins (50, 50, 50, 50, QPrinter::DevicePixel);
	}
	else {
		printer.setPaperSize(QSizeF(width(), height()), QPrinter::DevicePixel);
		printer.setPageMargins (0, 0, 0, 0, QPrinter::DevicePixel);
	}
	printer.setOutputFormat(QPrinter::NativeFormat);
	printer.setOutputFileName(filename);

	QPainter painter;
	if (!painter.begin(&printer)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		#if OS_TYPE == UNX
			errstr += "\nDo you have a properly configured print server (sudo apt-get install cups-pdf) ?";
		#endif
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
	paint(painter);
	painter.end();
}


void cLinearWidget::exportToPNG(QString filename) {
	QImage image(width(), height(), QImage::Format_ARGB32);
	image.fill(Qt::white);

	QPainter painter;
	if (!painter.begin(&image)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::SmoothPixmapTransform);
	paint(painter);
	painter.end();
	image.save(filename);
}


void cLinearWidget::exportToSVG(QString filename) {
	QSvgGenerator generator;
	generator.setFileName(filename);
	generator.setSize(QSize(width(), height()));
	generator.setViewBox(QRect(0, 0, width(), height()));

	QPainter painter;
	if (!painter.begin(&generator)) {
		QMessageBox msgBox;
		QString errstr = "The file cannot be created.";
		msgBox.setText(errstr);
		msgBox.exec();
		return;
	}

	painter.fillRect(QRect(0, 0, width(), height()), Qt::white);
	paint(painter);
	painter.end();
}


void cLinearWidget::setReportIsomers(bool reportisomers) {
	this->reportisomers = reportisomers;
	repaint();
}


void cLinearWidget::hidePeakLabels(bool state) {
	this->hidelabels = state;
	repaint();
}


void cLinearWidget::paintEvent(QPaintEvent *event) {
	QPainter painter;
	painter.begin(this);
	paint(painter);
	painter.end();
	event->accept();
}


void cLinearWidget::paint(QPainter& painter) {

	if (!theoreticalspectrum) {
		return;
	}

	const int topmargin = max(20,(height() - 80)/2);
	const int leftmargin = 20;
	const int bottommargin = 80;
	const int rightmargin = 20;

	vector<string> acronyms = theoreticalspectrum->getCandidate().getAcronyms();
	int size = (int)acronyms.size();

	if (!reportisomers) {
		stripIsomersFromStringVector(acronyms);
	}

	const int horizontalstep = (width() - leftmargin - rightmargin)/std::max(size, 1);

	QFont myFont("Courier", 9);
	QFontMetrics fm(myFont);
	painter.setFont(myFont);

	for (int i = 0; i < size; i++) {
		painter.setPen(QPen(Qt::blue, 2, Qt::SolidLine));
		if (fm.width(acronyms[i].c_str()) > horizontalstep/2) {
			painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*i + 2, topmargin, horizontalstep/2 - 2, 20, Qt::AlignLeft|Qt::AlignVCenter, acronyms[i].c_str());
		}
		else {
			painter.drawText(leftmargin + horizontalstep/4 + horizontalstep*i, topmargin, horizontalstep/2, 20, Qt::AlignCenter, acronyms[i].c_str());
		}

		painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
		painter.drawRect(leftmargin + horizontalstep/4 + horizontalstep*i, topmargin, horizontalstep/2, 20);

		if (i < size - 1) {
			painter.setPen(QPen(Qt::black, 2, Qt::SolidLine));
			painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep*i + horizontalstep/2, topmargin + 11, leftmargin + horizontalstep/4 + horizontalstep*(i + 1), topmargin + 11);

			if (theoreticalspectrum->getTheoreticalPeaks()->size() > 0) {
				painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
				painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin - 10, leftmargin + horizontalstep*(i + 1), topmargin + 30);	
				painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*i + horizontalstep/2, topmargin - 10, leftmargin + horizontalstep*(i + 1), topmargin + - 10);
				painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin + 30, leftmargin + horizontalstep*(i + 1) + horizontalstep/8, topmargin + 30);
			}
		}
	}
	
	if (parameters && !hidelabels && theoreticalspectrum->getTheoreticalPeaks()) {

		unordered_set<cIonLabel, hash_cIonLabel> labels;
		labels.clear();

		cPeaksList* thpeaks = theoreticalspectrum->getTheoreticalPeaks();
		string description;
		size_t pos;
		for (int i = 0; i < thpeaks->size(); i++) {

			if ((*thpeaks)[i].matched > 0) {

				description = (*thpeaks)[i].description;
				description = description.substr(0, description.find(':'));

				if (description.find('[') == string::npos) {
					if (parameters->precursorcharge > 0) {
						pos = description.find("+ ");
						if (pos != string::npos) {
							description = description.substr(pos + 2);
						}
						else {
							continue;
						}
					}
					else {
						pos = description.find("- ");
						if (pos != string::npos) {
							description = description.substr(pos + 2);
						}
						else {
							continue;
						}
					}

					if (visibleionseries.empty() || (!visibleionseries.empty() && (parameters->iondefinitions[(*thpeaks)[i].iontype].name.compare(visibleionseries) == 0))) {

						if ((visibleneutralloss.compare("all") == 0) 
							|| ((visibleneutralloss.compare("none") == 0) && (description.find(" -") == string::npos))
							|| ((visibleneutralloss.compare("all") != 0) && (visibleneutralloss.compare("none") != 0) && (description.substr(description.find(" -") + 2).compare(visibleneutralloss) == 0))) {

							if (((parameters->peptidetype == linear) && (parameters->iondefinitions[(*thpeaks)[i].iontype].nterminal))
								|| ((parameters->peptidetype == linearpolyketide) && (((*thpeaks)[i].iontype == l1h_ion) || ((*thpeaks)[i].iontype == l2h_ion) || ((*thpeaks)[i].iontype == l1oh_ion) || ((*thpeaks)[i].iontype == l2oh_ion)))) {

								if ((*thpeaks)[i].seriesid >= 0) {
									insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * (*thpeaks)[i].seriesid + horizontalstep / 2, topmargin - 35, description, false);
								}

							}

							if (((parameters->peptidetype == linear) && (parameters->iondefinitions[(*thpeaks)[i].iontype].cterminal)) ||
								((parameters->peptidetype == linearpolyketide) && (((*thpeaks)[i].iontype == r1h_ion) || ((*thpeaks)[i].iontype == r2h_ion) || ((*thpeaks)[i].iontype == r1oh_ion) || ((*thpeaks)[i].iontype == r2oh_ion)))) {

								if ((*thpeaks)[i].seriesid >= 0) {
									insertLabel(labels, leftmargin + horizontalstep * (size - (*thpeaks)[i].seriesid - 1), topmargin + 35, description, false);
								}

							}

						}

					}

				}

			}

		}

		drawLabels(painter, labels, width());
	
	}
	
}


void cLinearWidget::ionSeriesChanged(QString text) {
	if (!parameters) {
		visibleionseries = "";
		return;
	}

	if (text.compare("all") == 0) {
		visibleionseries = "";
	}
	else {
		visibleionseries = text.toStdString();
	}

	repaint();
}


void cLinearWidget::neutralLossChanged(QString text) {
	if (!parameters) {
		visibleneutralloss = "all";
		return;
	}

	visibleneutralloss = text.toStdString();

	repaint();
}

