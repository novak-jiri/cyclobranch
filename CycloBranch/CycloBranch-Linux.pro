######################################################################
# Automatically generated by qmake (3.0) Fri Dec 5 03:02:26 2014
######################################################################

TEMPLATE = app
TARGET = CycloBranch
QT += core gui widgets printsupport svg
INCLUDEPATH += . core gui parallel
QMAKE_CXXFLAGS += -std=c++11 -DLINUX -m64
QMAKE_LIBS += -lboost_regex -lxerces-c
OBJECTS_DIR = build/
MOC_DIR = moc/

# Input
RESOURCES = images.qrc
HEADERS += core/cBrick.h \
           core/cBricksDatabase.h \
           core/cCandidate.h \
           core/cCandidateSet.h \
           core/cDeNovoGraph.h \
           core/cDeNovoGraphNode.h \
           core/cFragmentIons.h \
           core/cImzML.h \
           core/cIsotopePatternCache.h \
           core/cMzML.h \
           core/cParameters.h \
           core/cPeak.h \
           core/cPeakListSeries.h \
           core/cPeaksList.h \
           core/cSequence.h \
           core/cSequenceDatabase.h \
           core/cSummaryFormula.h \
           core/cTheoreticalSpectrum.h \
           core/cTheoreticalSpectrumList.h \
           core/utilities.h \
           gui/cAboutWidget.h \
           gui/cBranchCyclicWidget.h \
           gui/cBranchedWidget.h \
           gui/cBricksDatabaseProxyModel.h \
           gui/cBricksDatabaseWidget.h \
           gui/cCheckBoxDelegate.h \
           gui/cChromatogramWindow.h \
           gui/cChromatogramWindowWidget.h \
           gui/cComboBoxDelegate.h \
           gui/cCyclicWidget.h \
           gui/cDrawPeptideWidget.h \
           gui/cEventFilter.h \
           gui/cExportDialog.h \
           gui/cFindDialog.h \
           gui/cFragmentIonsListWidget.h \
           gui/cGraphWidget.h \
           gui/cHTMLDelegate.h \
           gui/cHTMLExportDialog.h \
           gui/cImageWindow.h \
           gui/cImageWindowWidget.h \
           gui/cLinearWidget.h \
           gui/cMainThread.h \
           gui/cMainWindow.h \
           gui/cMainWindowProxyModel.h \
           gui/cModificationsProxyModel.h \
           gui/cModificationsWidget.h \
           gui/cMultipleButtonDelegate.h \
           gui/cNeutralLossesListWidget.h \
           gui/cParametersWidget.h \
           gui/cSequenceDatabaseProxyModel.h \
           gui/cSequenceDatabaseWidget.h \
           gui/cSpectrumDetailProxyModel.h \
           gui/cSpectrumDetailWidget.h \
           gui/cSpectrumSceneWidget.h \
           gui/cSummaryPeaksTableProxyModel.h \
           gui/cSummaryPeaksTableWidget.h \
           gui/cViewButtonDelegate.h \
           parallel/cGraphReaderThread.h \
           parallel/cSpectrumComparatorThread.h
SOURCES += core/cBrick.cpp \
           core/cBricksDatabase.cpp \
           core/cCandidate.cpp \
           core/cCandidateSet.cpp \
           core/cDeNovoGraph.cpp \
           core/cDeNovoGraphNode.cpp \
           core/cFragmentIons.cpp \
           core/cImzML.cpp \
           core/cIsotopePatternCache.cpp \
           core/cMzML.cpp \
           core/cParameters.cpp \
           core/cPeak.cpp \
           core/cPeakListSeries.cpp \
           core/cPeaksList.cpp \
           core/cSequence.cpp \
           core/cSequenceDatabase.cpp \
           core/cSummaryFormula.cpp \
           core/cTheoreticalSpectrum.cpp \
           core/cTheoreticalSpectrumList.cpp \
           core/utilities.cpp \
           gui/cAboutWidget.cpp \
           gui/cBranchCyclicWidget.cpp \
           gui/cBranchedWidget.cpp \
           gui/cBricksDatabaseProxyModel.cpp \
           gui/cBricksDatabaseWidget.cpp \
           gui/cCheckBoxDelegate.cpp \
           gui/cChromatogramWindow.cpp \
           gui/cChromatogramWindowWidget.cpp \
           gui/cComboBoxDelegate.cpp \
           gui/cCyclicWidget.cpp \
           gui/cDrawPeptideWidget.cpp \
           gui/cEventFilter.cpp \
           gui/cExportDialog.cpp \
           gui/cFindDialog.cpp \
           gui/cFragmentIonsListWidget.cpp \
           gui/cGraphWidget.cpp \
           gui/cHTMLDelegate.cpp \
           gui/cHTMLExportDialog.cpp \
           gui/cImageWindow.cpp \
           gui/cImageWindowWidget.cpp \
           gui/cLinearWidget.cpp \
           gui/cMainThread.cpp \
           gui/cMainWindow.cpp \
           gui/cMainWindowProxyModel.cpp \
           gui/cModificationsProxyModel.cpp \
           gui/cModificationsWidget.cpp \
           gui/cMultipleButtonDelegate.cpp \
           gui/cNeutralLossesListWidget.cpp \
           gui/cParametersWidget.cpp \
           gui/cSequenceDatabaseProxyModel.cpp \
           gui/cSequenceDatabaseWidget.cpp \
           gui/cSpectrumDetailProxyModel.cpp \
           gui/cSpectrumDetailWidget.cpp \
           gui/cSpectrumSceneWidget.cpp \
           gui/cSummaryPeaksTableProxyModel.cpp \
           gui/cSummaryPeaksTableWidget.cpp \
           gui/cViewButtonDelegate.cpp \
           parallel/cGraphReaderThread.cpp \
           parallel/cSpectrumComparatorThread.cpp \
           main.cpp
