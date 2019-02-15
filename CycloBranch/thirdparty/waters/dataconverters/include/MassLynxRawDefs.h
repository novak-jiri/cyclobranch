//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawDefs.h
// COPYRIGHT(C):	Waters Corporation
//					
//-----------------------------------------------------------------------------------------------------

#pragma once

#ifdef _WIN32
#define MLYNX_RAW_API __declspec(dllexport)
#else
#define MLYNX_RAW_API
#define __stdcall
#endif

typedef void* CMassLynxParameters;
typedef void* CMassLynxRawReader;
typedef void* CMassLynxRawProcessor;
typedef void* CMassLynxRawWriter;
typedef void(__stdcall *ProgressCallBack)(void* pObject, const int& precent);

enum class MassLynxReaderType { raw = 0, scan = 1, info = 2, chrom = 3, analog = 4, lockmass = 5, centroid = 6, dda = 7, mse = 8, uninitialised = 1000 };

enum class MassLynxAcquisitionType { DDA = 0, MSE = 1, HDDDA = 2, HDMSE = 3, SONAR = 4, UNINITIALISED = 1000 };

enum class MassLynxHeaderItem {	VERSION = 0, ACQUIRED_NAME = 1, ACQUIRED_DATE = 2, ACQUIRED_TIME = 3, JOB_CODE = 4, TASK_CODE = 5, USER_NAME = 6,
	INSTRUMENT = 7, CONDITIONS = 8, LAB_NAME = 9, SAMPLE_DESCRIPTION = 10, SOLVENT_DELAY = 11, SUBMITTER = 12, SAMPLE_ID = 13, BOTTLE_NUMBER = 14,
	ANALOG_CH1_OFFSET = 15, ANALOG_CH2_OFFSET = 16, ANALOG_CH3_OFFSET = 17, ANALOG_CH4_OFFSET = 18, CAL_MS1_STATIC = 19, CAL_MS2_STATIC = 20,
	CAL_MS1_STATIC_PARAMS = 21, CAL_MS1_DYNAMIC_PARAMS = 22, CAL_MS2_STATIC_PARAMS = 23, CAL_MS2_DYNAMIC_PARAMS = 24, CAL_MS1_FAST_PARAMS = 25,
	CAL_MS2_FAST_PARAMS = 26, CAL_TIME = 27, CAL_DATE = 28, CAL_TEMPERATURE = 29, INLET_METHOD = 30, SPARE1 = 31, SPARE2 = 32, SPARE3 = 33,
	SPARE4 = 34, SPARE5 = 35,
	UNINITIALISED = 1000
};

enum class MassLynxIonMode {
	EI_POS = 0, EI_NEG = 1, CI_POS = 2, CI_NEG = 3, FB_POS = 4, FB_NEG = 5, TS_POS = 6, TS_NEG = 7, ES_POS = 8, ES_NEG = 9,
	AI_POS = 10, AI_NEG = 11, LD_POS = 12, LD_NEG = 13,
	UNINITIALISED = 1000
};

enum class MassLynxFunctionType {
	MS = 0, SIR = 1, DLY = 2, CAT = 3, OFF = 4, PAR = 5, DAU = 6, NL = 7, NG = 8, MRM = 9, Q1F = 10, MS2 = 11, DAD = 12,
	TOF = 13, PSD = 14, TOFS = 15, TOFD = 16, MTOF = 17, TOFM = 18, TOFP = 19, ASVS = 20, ASMS = 21, ASVSIR = 22, ASMSIR = 23, QUADD = 24, ASBE = 25,
	ASB2E = 26, ASCNL = 27, ASMIKES = 28, ASMRM = 29, ASNRMS = 30, ASMRMQ = 31,
	UNINITIALISED = 1000
};

enum class MassLynxScanItem { LINEAR_DETECTOR_VOLTAGE = 1, LINEAR_SENSITIVITY = 2, REFLECTRON_LENS_VOLTAGE = 3, REFLECTRON_DETECTOR_VOLTAGE = 4, REFLECTRON_SENSITIVITY = 5,
	LASER_REPETITION_RATE = 6, COURSE_LASER_CONTROL = 7, FINE_LASER_CONTROL = 8, LASERAIM_XPOS = 9, LASERAIM_YPOS = 10, NUM_SHOTS_SUMMED = 11, NUM_SHOTS_PERFORMED = 12,
	SEGMENT_NUMBER = 13, LCMP_TFM_WELL = 14, SEGMENT_TYPE = 15, SOURCE_REGION1 = 16, SOURCE_REGION2 = 17, REFLECTRON_FIELD_LENGTH = 18, REFLECTRON_LENGTH = 19,
	REFLECTRON_VOLT = 20, SAMPLE_PLATE_VOLT = 21, REFLECTRON_FIELD_LENGTH_ALT = 22, REFLECTRON_LENGTH_ALT = 23, PSD_STEP_MAJOR = 24, PSD_STEP_MINOR = 25, PSD_FACTOR_1 = 26,
	NEEDLE = 50, COUNTER_ELECTRODE_VOLTAGE = 51, SAMPLING_CONE_VOLTAGE = 52, SKIMMER_LENS = 53, SKIMMER = 54, PROBE_TEMPERATURE = 55, SOURCE_TEMPERATURE = 56, RF_VOLTAGE = 57,
	SOURCE_APERTURE = 58, SOURCE_CODE = 59, LM_RESOLUTION = 60, HM_RESOLUTION = 61, COLLISION_ENERGY = 62, ION_ENERGY = 63, MULTIPLIER1 = 64, MULTIPLIER2 = 65, TRANSPORTDC = 66,
	TOF_APERTURE = 67, ACC_VOLTAGE = 68, STEERING = 69, FOCUS = 70, ENTRANCE = 71, GUARD = 72, TOF = 73, REFLECTRON = 74, COLLISION_RF = 75, TRANSPORT_RF = 76, SET_MASS = 77,
	COLLISION_ENERGY2 = 78, SONAR_ENABLED = 81, QUAD_START_MASS = 82, QUAD_STOP_MASS = 83, QUAD_PEAK_WIDTH = 84, REFERENCE_SCAN = 100, USE_LOCKMASS_CORRECTION = 101,
	LOCKMASS_CORRECTION = 102, USETEMP_CORRECTION = 103, TEMP_CORRECTION = 104, TEMP_COEFFICIENT = 105, FAIMS_COMPENSATION_VOLTAGE = 106, TIC_TRACE_A = 107, TIC_TRACE_B = 108,
	RAW_EE_CV = 110, RAW_EE_CE = 111, ACCURATE_MASS = 112, ACCURATE_MASS_FLAGS = 113, SCAN_ERROR_FLAG = 114, DRE_TRANSMISSION = 115, SCAN_PUSH_COUNT = 116,
	RAW_STAT_SWAVE_NORMALISATION_FACTOR = 117, TOTAL_ION_CURRENT = 252, BASE_PEAK_MASS = 253, BASE_PEAK_INTENSITY = 254, PEAKS_IN_SCAN = 255,
	UNINITIALISED = 1000
};

enum class MassLynxScanType {
	MS1 = 1, MS2 = 2, UNINITIALISED = 1000
};


#ifdef __cplusplus
extern "C" {
#endif
	// error code support
	MLYNX_RAW_API int __stdcall getErrorMessage(const int nErrorCode, char** ppErrorMessage );
	MLYNX_RAW_API int __stdcall releaseMemory(void* pMemory);

	// Base reader
	MLYNX_RAW_API int __stdcall createRawReaderFromPath(const char *path, CMassLynxRawReader* mlRawReader, MassLynxReaderType nType);
	MLYNX_RAW_API int __stdcall createRawReaderFromReader(CMassLynxRawReader mlSourceRawReader, CMassLynxRawReader* mlRawReader, MassLynxReaderType nType);
	MLYNX_RAW_API int __stdcall destroyRawReader(CMassLynxRawReader mlRawReader);

	// Information reader
	MLYNX_RAW_API int __stdcall getFunctionCount(CMassLynxRawReader mlInfoReader, unsigned int* pFunctions);
	MLYNX_RAW_API int __stdcall getScanCount(CMassLynxRawReader mlInfoReader, const int nWhichFunction, unsigned int* pScans);
	MLYNX_RAW_API int __stdcall getAcquisitionMassRange(CMassLynxRawReader mlInfoReader, const int nWhichFunction, const int nWhichSegment, float* lowMass, float* highMass);
	MLYNX_RAW_API int __stdcall getAcquisitionTimeRange(CMassLynxRawReader mlInfoReader, const int nWhichFunction, float* startTime, float* endTime);
	MLYNX_RAW_API int __stdcall getFunctionType(CMassLynxRawReader mlInfoReader, const int nWhichFunction, MassLynxFunctionType* pFunctionType);
	MLYNX_RAW_API int __stdcall getFunctionTypeString(CMassLynxRawReader mlInfoReader, const MassLynxFunctionType functionType, char** chFunctionType);
	MLYNX_RAW_API int __stdcall isContinuum(CMassLynxRawReader mlInfoReader, const int nWhichFunction, bool& bContinuum);
	MLYNX_RAW_API int __stdcall getIonMode(CMassLynxRawReader mlInfoReader, const int nWhichFunction, MassLynxIonMode* ionMode);
	MLYNX_RAW_API int __stdcall getIonModeString(CMassLynxRawReader mlInfoReader, const MassLynxIonMode ionMode, char** chIonMode);
	MLYNX_RAW_API int __stdcall getHeaderItems(CMassLynxRawReader mlInfoReader, const MassLynxHeaderItem* pItems, char** ppItems, int nItems, char* pDelimiter);
	MLYNX_RAW_API int __stdcall getScanItems(CMassLynxRawReader mlInfoReader, const int nWhichFunction, const int nWhichScan, const MassLynxScanItem* pItems, char** ppInfo, int nItems, char* pDelimiter);
	MLYNX_RAW_API int __stdcall getScanItemNames(CMassLynxRawReader mlInfoReader, const MassLynxScanItem* pItems, char** ppStatsName, int nItems, char* pDelimiter);
	MLYNX_RAW_API int __stdcall getItemsInFunction(CMassLynxRawReader mlInfoReader, const int nWhichFunction, MassLynxScanItem** ppItems, int* pSize);
	MLYNX_RAW_API int __stdcall getRetentionTime(CMassLynxRawReader mlInfoReader, const int nWhichFunction, const int nWhichScan, float* fRT);
	MLYNX_RAW_API int __stdcall getDriftTime(CMassLynxRawReader mlInfoReader, const int nWhichFunction, const int nWhichDrift, float* fRT);
	MLYNX_RAW_API int __stdcall getDriftScanCount(CMassLynxRawReader mlInfoReader, const int nWhichFunction, unsigned int* pScans);
	MLYNX_RAW_API int __stdcall getMRMCount(CMassLynxRawReader mlInfoReader, const int nWhichFunction, unsigned int* pMRMs);
	MLYNX_RAW_API int __stdcall isLockMassCorrected(CMassLynxRawReader mlInfoReader, bool* pIsApplied);
	MLYNX_RAW_API int __stdcall canLockMassCorrect(CMassLynxRawReader mlInfoReader, bool* pCanApply);
	MLYNX_RAW_API int __stdcall getAcquisitionType(CMassLynxRawReader mlInfoReader, MassLynxAcquisitionType* acquisitionType);
	MLYNX_RAW_API int __stdcall getSonarIndexRange(CMassLynxRawReader mlRawReader, const int nWhichFunction, const float preCursorMass, const float preCursorTolerance, int* pStartIndex, int* pEndIndex);

	// deprecate
	MLYNX_RAW_API int __stdcall getHeaderItem(CMassLynxRawReader mlInfoReader, const char* pTag, char** ppMetaData);
	MLYNX_RAW_API int __stdcall getScanItem(CMassLynxRawReader mlInfoReader, const int nWhichFunction, const int nWhichScan, const MassLynxScanItem* pItems, const char*** pppInfo, int nItems);
	MLYNX_RAW_API int __stdcall getScanItemName(CMassLynxRawReader mlInfoReader, const MassLynxScanItem* pItems, void** ppStatsName, int nItems);
	
	// Scan Reader functions
	MLYNX_RAW_API int __stdcall readScan(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, float** ppMasses, float** ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall readScanFlags(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, float** pMasses, float** pIntensities, char** pFlags, int* pSize);
	MLYNX_RAW_API int __stdcall readDriftScan(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, const int nWhichDrift, float** ppMasses, float** ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall readDaughterScan(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, float** ppMasses, float** ppIntensities, float** ppProductMasses, int* pSize, int* pProductSize);
	MLYNX_RAW_API int __stdcall readDriftScanIndex(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, const int nWhichDrift, int** ppMasses, float** ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall readDriftScanFlagsIndex(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, const int nWhichDrift, int** ppMasses, float** ppIntensities, char** pFlags, int* pSize);
	MLYNX_RAW_API int __stdcall getDriftMassScale(CMassLynxRawReader mlRawScanreader, const int nWhichFunction, const int nWhichScan, float** ppMasses, int* pSize);

	// Chromatogram functions
	MLYNX_RAW_API int __stdcall readBPIChromatogram(CMassLynxRawReader mlChromatogramReader, int nWhichFunction, float** ppTimes, float** ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall readTICChromatogram(CMassLynxRawReader mlChromatogramReader, int nWhichFunction, float** ppTimes, float** ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall readMassChromatograms(CMassLynxRawReader mlChromatogramReader, const int nWhichFunction, const float* massList, const int massListSize, float** ppTimes, float** const ppIntensities, float massWindow, bool bDaughters, int* pSize);
	MLYNX_RAW_API int __stdcall readMRMChromatograms(CMassLynxRawReader mlChromatogramReader, const int nWhichFunction, const int* mrmList, const int nMRM, float** ppTimes, float ** const ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall readSonarMassChromatogram(CMassLynxRawReader mlChromatogramReader, const int nWhichFunction, const float preCursorMass, const float pMass, float** ppTimes, float** ppIntensities, const float precursorMassWindow, const float massWindow, int* pSize);

	// Analog reader functions
	MLYNX_RAW_API int __stdcall getChannelCount(CMassLynxRawReader mlAnalogReader, int* nChannels);
	MLYNX_RAW_API int __stdcall readChannel(CMassLynxRawReader mlAnalogReader, const int nWhichChannel, float** pTimes, float** pIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall getChannelDesciption(CMassLynxRawReader mlAnalogReader, const int nWhichChannel, char** ppDescription);
	MLYNX_RAW_API int __stdcall getChannelUnits(CMassLynxRawReader mlAnalogReader, const int nWhichChannel, char** ppUnits);

	// Base processor
	MLYNX_RAW_API int __stdcall createRawProcessor(CMassLynxRawProcessor* mlRawProcessor, MassLynxReaderType nType, ProgressCallBack pCallback, void* pCaller);
	MLYNX_RAW_API int __stdcall destroyRawProcessor(CMassLynxRawProcessor mlRawProcessor);
	MLYNX_RAW_API int __stdcall getProcessorMessage(CMassLynxRawProcessor mlRawProcessor, const int nCode, char** ppMessage);
	MLYNX_RAW_API int __stdcall setRawReader(CMassLynxRawProcessor mlRawProcessor, CMassLynxRawReader mlRawReader);
	MLYNX_RAW_API int __stdcall setRawPath(CMassLynxRawProcessor mlRawProcessor, const char *path);

	// Lock mass processor
	MLYNX_RAW_API int __stdcall setLockMassParameters(CMassLynxRawProcessor mlLockMassProcessor, const char* pParameters);
	MLYNX_RAW_API int __stdcall getLockMassParameter(CMassLynxRawProcessor mlLockMassProcessor, char** ppParameters);
	MLYNX_RAW_API int __stdcall lockMassCorrect(CMassLynxRawProcessor mlLockMassProcessor, bool* pApplied);
	MLYNX_RAW_API int __stdcall removeLockMassCorrection(CMassLynxRawProcessor mlLockMassProcessor);
	MLYNX_RAW_API int __stdcall getLockMassCandidates(CMassLynxRawProcessor mlLockMassProcessor, float** ppMasses, float** ppIntensities, int* nSize);
	MLYNX_RAW_API int __stdcall _isLockMassCorrected(CMassLynxRawProcessor mlLockMassProcessor, bool* pApplied);
	MLYNX_RAW_API int __stdcall _canLockMassCorrect(CMassLynxRawProcessor mlLockMassProcessor, bool* pCanApply);
	MLYNX_RAW_API int __stdcall getLockMassValues(CMassLynxRawProcessor mlLockMassProcessor, float* pMass, float* pTolerance);
	MLYNX_RAW_API int __stdcall getLockMassCorrection(CMassLynxRawProcessor mlLockMassProcessor, const float retentionTime, float* pGain);

	// Scan processor
	MLYNX_RAW_API int __stdcall getScan(CMassLynxRawProcessor mlScanProcessor, float** ppMasses, float** ppIntensities, int* nSize);

	// combine
	MLYNX_RAW_API int __stdcall combineScan(CMassLynxRawProcessor mlScanProcessor, const int nWhichFunction, const int* pScans, const int nScans);

	// smooth
	MLYNX_RAW_API int __stdcall smoothScan(CMassLynxRawProcessor mlScanProcessor);
	MLYNX_RAW_API int __stdcall setSmoothParameter(CMassLynxRawProcessor mlScanProcessor, const char* pParameters);
	MLYNX_RAW_API int __stdcall getSmoothParameter(CMassLynxRawProcessor mlScanProcessor, char** ppParameters);

	// centroid
	MLYNX_RAW_API int __stdcall centroidScan(CMassLynxRawProcessor mlScanProcessor);

	// theshold
	MLYNX_RAW_API int __stdcall thresholdScan(CMassLynxRawProcessor mlScanProcessor);
	MLYNX_RAW_API int __stdcall setThresholdParameter(CMassLynxRawProcessor mlScanProcessor, const char* pParameters);
	MLYNX_RAW_API int __stdcall getThresholdParameter(CMassLynxRawProcessor mlScanProcessor, char** ppParameters);

	// chromatogram processor
	MLYNX_RAW_API int __stdcall getProcessedChromatograms(CMassLynxRawProcessor mlChromatogramProcessor, const int nChromatograms, float** ppTimes, float** const ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall getProcessedChromatogramCount(CMassLynxRawProcessor mlChromatogramProcessor, int* pChromatograms);
	MLYNX_RAW_API int __stdcall extract(CMassLynxRawProcessor mlChromatogramProcessor);
	MLYNX_RAW_API int __stdcall setExtractParameter(CMassLynxRawProcessor mlChromatogramProcessor, const char* pParameters);
	MLYNX_RAW_API int __stdcall getExtractParameter(CMassLynxRawProcessor mlChromatogramProcessor, char** ppParameters);

	// smooth
	MLYNX_RAW_API int __stdcall smoothChromatogram(CMassLynxRawProcessor mlChromatogramProcessor);
	MLYNX_RAW_API int __stdcall setChromatogramSmoothParameter(CMassLynxRawProcessor mlChromatogramProcessor, const char* pParameters);
	MLYNX_RAW_API int __stdcall getChromatogramSmoothParameter(CMassLynxRawProcessor mlChromatogramProcessor, char** ppParameters);

	// Parameter helper
	MLYNX_RAW_API int __stdcall createParameters(CMassLynxParameters* mlParameters);
	MLYNX_RAW_API int __stdcall destroyParameters(CMassLynxParameters mlParameters);
	MLYNX_RAW_API int __stdcall setParameters(CMassLynxParameters mlParameters, const char* pKey, const char* pValue);
	MLYNX_RAW_API int __stdcall getParameterJSON(CMassLynxParameters mlParameters, char** ppJSON);

	// RAW folder processes
	MLYNX_RAW_API int __stdcall centroidRaw(CMassLynxRawProcessor mlRawProcessor, const char *path);

	// Raw Writer
	MLYNX_RAW_API int __stdcall createRawWriter(CMassLynxRawWriter* mlRawWriter, const char* pTarget);
	MLYNX_RAW_API int __stdcall destroyRawWriter(CMassLynxRawWriter mlRawWriter);
	MLYNX_RAW_API int __stdcall appendRawFunction(CMassLynxRawWriter mlRawWriter, const bool bCentroid );
	MLYNX_RAW_API int __stdcall appendRawFunctionWithStats(CMassLynxRawWriter mlRawWriter, const bool bCentroid, const MassLynxScanItem* pStats, const int size);
	MLYNX_RAW_API int __stdcall appendRawScan(CMassLynxRawWriter mlRawWriter, const float* pMasses, const float* pIntensities, const int nSize, const float fRT);
	MLYNX_RAW_API int __stdcall appendRawScanWithStats(CMassLynxRawWriter mlRawWriter, const float* pMasses, const float* pIntensities, const int nSize, const float fRT, const char* pStats);
	MLYNX_RAW_API int __stdcall setRawHeaderItem(CMassLynxRawWriter mlRawWriter, const MassLynxHeaderItem headerItem, const char* pValue);
	MLYNX_RAW_API int __stdcall closeRawWriter(CMassLynxRawWriter mlRawWriter);

	// DDA processor
	MLYNX_RAW_API int __stdcall ddaRaw(const CMassLynxRawProcessor mlRawProcessor, float** ppRT, float** ppMasses, int* nSize);
	MLYNX_RAW_API int __stdcall ddaGetScan(const CMassLynxRawProcessor mlRawProcessor, const int nWhichPrecursor, const MassLynxScanType scanType, float** ppMasses, float** ppIntensities, int* pSize);
	MLYNX_RAW_API int __stdcall ddaGetNextScan(const CMassLynxRawProcessor mlRawProcessor, float* pRT, MassLynxScanType* scanType, float* pSetMass, float** ppMasses, float** ppIntensities, int* pSize, bool* pNext);
	MLYNX_RAW_API int __stdcall ddaResetScan(const CMassLynxRawProcessor mlRawProcessor);

	// MSe processor
	MLYNX_RAW_API int __stdcall mseGetNextScan(const CMassLynxRawProcessor mlRawProcessor, float* pRT, MassLynxScanType* scanType, float** ppMasses, float** ppIntensities, int* pSize, bool* pNext);
	MLYNX_RAW_API int __stdcall mseResetScan(const CMassLynxRawProcessor mlRawProcessor);


#ifdef __cplusplus
}
#endif