######################################################################
# Automatically generated by qmake (3.0) Fri Dec 5 03:02:26 2014
######################################################################

TEMPLATE = app
TARGET = CycloBranch
QT += core gui widgets printsupport svg
INCLUDEPATH += . core gui parallel /usr/local/Cellar/boost/1.57.0/include /usr/local/Cellar/xerces-c/3.1.1/include
QMAKE_CXXFLAGS += -std=c++0x -DMACOSX -m64
QMAKE_LIBS += -lboost_regex -lxerces-c
QMAKE_LIBDIR = /usr/local/Cellar/boost/1.57.0/lib /usr/local/Cellar/xerces-c/3.1.1/lib
OBJECTS_DIR = build/
MOC_DIR = moc/
ICON = images/cb.icns

# The following variables must be manually fixed in the 'Makefile' generated by qmake.
# The variables QMAKE_CC, QMAKE_CXX and QMAKE_LINK are ignored by qmake.
#CC            = /usr/local/Cellar/gcc/4.9.2_1/bin/gcc-4.9
#CXX           = /usr/local/Cellar/gcc/4.9.2_1/bin/g++-4.9
#LINK          = /usr/local/Cellar/gcc/4.9.2_1/bin/g++-4.9

# Input
RESOURCES = images.qrc
HEADERS += core/cAllocator.h \
           core/cBrick.h \
           core/cBricksDatabase.h \
           core/cCandidate.h \
           core/cCandidateSet.h \
           core/cDeNovoGraph.h \
           core/cDeNovoGraphNode.h \
           core/cFragmentIons.h \
           core/cImzML.h \
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
           gui/cBricksDatabaseWidget.h \
           gui/cCyclicWidget.h \
           gui/cDelegate.h \
           gui/cDrawPeptideWidget.h \
           gui/cEventFilter.h \
           gui/cExportDialog.h \
           gui/cFindDialog.h \
           gui/cFragmentIonsListWidget.h \
           gui/cGraphWidget.h \
           gui/cHTMLExportDialog.h \
           gui/cImageWindow.h \
           gui/cImageWindowWidget.h \
           gui/cLinearWidget.h \
           gui/cMainThread.h \
           gui/cMainWindow.h \
           gui/cModificationsWidget.h \
           gui/cParametersWidget.h \
           gui/cSequenceDatabaseWidget.h \
           gui/cSpectrumDetailWidget.h \
           gui/cSpectrumSceneWidget.h \
           gui/cSummaryPeaksTableWidget.h \
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
           gui/cBricksDatabaseWidget.cpp \
           gui/cCyclicWidget.cpp \
           gui/cDelegate.cpp \
           gui/cDrawPeptideWidget.cpp \
           gui/cEventFilter.cpp \
           gui/cExportDialog.cpp \
           gui/cFindDialog.cpp \
           gui/cFragmentIonsListWidget.cpp \
           gui/cGraphWidget.cpp \
           gui/cHTMLExportDialog.cpp \
           gui/cImageWindow.cpp \
           gui/cImageWindowWidget.cpp \
           gui/cLinearWidget.cpp \
           gui/cMainThread.cpp \
           gui/cMainWindow.cpp \
           gui/cModificationsWidget.cpp \
           gui/cParametersWidget.cpp \
           gui/cSequenceDatabaseWidget.cpp \
           gui/cSpectrumDetailWidget.cpp \
           gui/cSpectrumSceneWidget.cpp \
           gui/cSummaryPeaksTableWidget.cpp \
           parallel/cGraphReaderThread.cpp \
           parallel/cSpectrumComparatorThread.cpp \
           main.cpp
