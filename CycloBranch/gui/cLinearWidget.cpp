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
	cIonLabel label;
	label.x = x;
	label.y = y;
	label.label = description;
	label.alignright = alignright;

	if (labels.count(label) == 1) {
		auto it = labels.find(label);
		label.label = it->label + "," + label.label;
		labels.erase(it);
	}

	labels.insert(label);
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

			if (theoreticalspectrum->getVisualCoverage().size() > 0) {
				painter.setPen(QPen(Qt::black, 2, Qt::DashLine));
				painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin - 10, leftmargin + horizontalstep*(i + 1), topmargin + 30);	
				painter.drawLine(leftmargin + horizontalstep/4 + horizontalstep/8 + horizontalstep*i + horizontalstep/2, topmargin - 10, leftmargin + horizontalstep*(i + 1), topmargin + - 10);
				painter.drawLine(leftmargin + horizontalstep*(i + 1), topmargin + 30, leftmargin + horizontalstep*(i + 1) + horizontalstep/8, topmargin + 30);
			}
		}
	}
	
	if (parameters && !hidelabels && (theoreticalspectrum->getVisualCoverage().size() > 0)) {

		unordered_set<cIonLabel, hash_cIonLabel> labels;
		labels.clear();

		string name;
		int len = (int)theoreticalspectrum->getVisualCoverage()[0].series.size();
		int coverageindex = 0;
		bool skipiontype, skipneutralloss;

		for (int i = 0; i < (int)parameters->ionsfortheoreticalspectra.size(); i++) {

			skipiontype = false;
			if ((visibleionseries.compare("") != 0) && (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.compare(visibleionseries) != 0)) {
				skipiontype = true;
			}

			for (int j = -1; j < (int)parameters->neutrallossesfortheoreticalspectra.size(); j++) {

				skipneutralloss = false;
				if (visibleneutralloss.compare("all") != 0) {
					if ((j == -1) && (visibleneutralloss.compare("none") != 0)) {
						skipneutralloss = true;
					}
					if ((j >= 0) && (parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary.compare(visibleneutralloss) != 0)) {
						skipneutralloss = true;
					}
				}

				if (!skipiontype && !skipneutralloss) {

					if (((parameters->peptidetype == linear) && (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].nterminal))
						|| ((parameters->peptidetype == linearpolyketide) && ((parameters->ionsfortheoreticalspectra[i] == l1h_ion) || (parameters->ionsfortheoreticalspectra[i] == l2h_ion) || (parameters->ionsfortheoreticalspectra[i] == l1oh_ion) || (parameters->ionsfortheoreticalspectra[i] == l2oh_ion)))) {
						for (int k = 0; k < len; k++) {
							if (theoreticalspectrum->getVisualCoverage()[coverageindex].series[k] > 0) {
								if (parameters->peptidetype == linearpolyketide) {
									name = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.substr(0, 2) + to_string(k + 1) + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.substr(2);
								}
								else {
									name = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name[0] + to_string(k + 1) + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.substr(1);
								}
								if (j >= 0) {
									name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
								}
								insertLabel(labels, leftmargin + horizontalstep / 4 + horizontalstep / 8 + horizontalstep * k + horizontalstep / 2, topmargin - 35, name, false);
							}
						}
					}

					if (((parameters->peptidetype == linear) && (parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].cterminal)) ||
						((parameters->peptidetype == linearpolyketide) && ((parameters->ionsfortheoreticalspectra[i] == r1h_ion) || (parameters->ionsfortheoreticalspectra[i] == r2h_ion) || (parameters->ionsfortheoreticalspectra[i] == r1oh_ion) || (parameters->ionsfortheoreticalspectra[i] == r2oh_ion)))) {
						for (int k = len - 1; k >= 0; k--) {
							if (theoreticalspectrum->getVisualCoverage()[coverageindex].series[len - k - 1] > 0) {
								if (parameters->peptidetype == linearpolyketide) {
									name = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.substr(0, 2) + to_string(len - k) + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.substr(2);
								}
								else {
									name = parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name[0] + to_string(len - k) + parameters->iondefinitions[parameters->ionsfortheoreticalspectra[i]].name.substr(1);
								}
								if (j >= 0) {
									name += "-" + parameters->neutrallossesdefinitions[parameters->neutrallossesfortheoreticalspectra[j]].summary;
								}
								insertLabel(labels, leftmargin + horizontalstep * (k + 1), topmargin + 35, name, false);
							}
						}
					}

				}

				coverageindex++;

			}

		}

		painter.setPen(QPen(Qt::red, 2, Qt::SolidLine));
		for (auto it = labels.begin(); it != labels.end(); ++it) {
			painter.drawText(it->x, it->y, width(), 20, Qt::AlignLeft, it->label.c_str());
		}
	
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

