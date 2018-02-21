######################################################################
# Automatically generated by qmake (3.0) Fri Dec 5 03:02:26 2014
######################################################################

TEMPLATE = app
TARGET = CycloBranch
QT += core gui widgets printsupport svg
INCLUDEPATH += . core gui parallel
QMAKE_CXXFLAGS += -std=c++0x -DUNIX -m64
QMAKE_LIBS += -lboost_regex
OBJECTS_DIR = build/
MOC_DIR = moc/

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
           core/cParameters.h \
           core/cPeak.h \
           core/cPeaksList.h \
           core/cSequence.h \
           core/cSequenceDatabase.h \
           core/cSummaryFormula.h \
           core/cTheoreticalSpectrum.h \
           core/cTheoreticalSpectrumList.h \
           core/utilities.h \
           gui/cAboutWidget.h \
           gui/cBranchedWidget.h \
           gui/cBricksDatabaseWidget.h \
           gui/cCyclicWidget.h \
           gui/cDrawPeptideWidget.h \
           gui/cEventFilter.h \
           gui/cFragmentIonsListWidget.h \
           gui/cGraphWidget.h \
           gui/cLassoWidget.h \
           gui/cLinearWidget.h \
           gui/cMainThread.h \
           gui/cMainWindow.h \
           gui/cModificationsWidget.h \
           gui/cParametersWidget.h \
           gui/cSequenceDatabaseWidget.h \
           gui/cSpectrumDetailWidget.h \
           gui/cSpectrumSceneWidget.h \
           parallel/cGraphReaderThread.h \
           parallel/cSpectrumComparatorThread.h
SOURCES += core/cBrick.cpp \
           core/cBricksDatabase.cpp \
           core/cCandidate.cpp \
           core/cCandidateSet.cpp \
           core/cDeNovoGraph.cpp \
           core/cDeNovoGraphNode.cpp \
           core/cFragmentIons.cpp \
           core/cParameters.cpp \
           core/cPeak.cpp \
           core/cPeaksList.cpp \
           core/cSequence.cpp \
           core/cSequenceDatabase.cpp \
           core/cSummaryFormula.cpp \
           core/cTheoreticalSpectrum.cpp \
           core/cTheoreticalSpectrumList.cpp \
           core/utilities.cpp \
           gui/cAboutWidget.cpp \
           gui/cBranchedWidget.cpp \
           gui/cBricksDatabaseWidget.cpp \
           gui/cCyclicWidget.cpp \
           gui/cDrawPeptideWidget.cpp \
           gui/cEventFilter.cpp \
           gui/cFragmentIonsListWidget.cpp \
           gui/cGraphWidget.cpp \
           gui/cLassoWidget.cpp \
           gui/cLinearWidget.cpp \
           gui/cMainThread.cpp \
           gui/cMainWindow.cpp \
           gui/cModificationsWidget.cpp \
           gui/cParametersWidget.cpp \
           gui/cSequenceDatabaseWidget.cpp \
           gui/cSpectrumDetailWidget.cpp \
           gui/cSpectrumSceneWidget.cpp \
           parallel/cGraphReaderThread.cpp \
           parallel/cSpectrumComparatorThread.cpp \
           main.cpp