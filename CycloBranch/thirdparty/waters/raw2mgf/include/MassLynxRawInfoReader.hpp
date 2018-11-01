//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawInfo.h
// DATE:			Nov 2009
// COPYRIGHT(C):	Waters Corporation
//
// COMMENTTS:		This header contains the declaration of the MassLynxRawInfo
//					class.
//-----------------------------------------------------------------------------------------------------
#pragma once

#include "MassLynxRawBase.hpp"

namespace Waters
{
namespace Lib
{
namespace MassLynxRaw
{
	using std::string;
	using std::vector;

	class MassLynxRawInfo : public MassLynxRawReader
	{

	public:
		MassLynxRawInfo(const std::string & strFullPathName) : MassLynxRawReader(strFullPathName, MassLynxReaderType::info) {}
		MassLynxRawInfo(const MassLynxRawReader& rawReader) : MassLynxRawReader(rawReader, MassLynxReaderType::info) {}
		MassLynxRawInfo(const MassLynxRawInfo& rawReader) : MassLynxRawReader(rawReader, MassLynxReaderType::info) {}

		int GetFunctionCount() const
		{
			unsigned int nFunction(0);
			MassLynxRawReader::CheckReturnCode(getFunctionCount(GetRawReader(), &nFunction));
			return nFunction;
		}

		string GetFunctionTypeString(const MassLynxFunctionType& functionType) const
		{
			char* chFunctionType(NULL);
			MassLynxRawReader::CheckReturnCode(getFunctionTypeString(GetRawReader(), functionType, &chFunctionType));
			return ToString(chFunctionType);
		}

		MassLynxAcquisitionType GetAcquisitionType() const
		{
			MassLynxAcquisitionType acquisitionType(MassLynxAcquisitionType::UNINITIALISED);
			CheckReturnCode(getAcquisitionType(GetRawReader(), &acquisitionType));
			return acquisitionType;
		}

		MassLynxFunctionType GetFunctionType(const int& nWhichFunction) const
		{
			MassLynxFunctionType functionType(MassLynxFunctionType::UNINITIALISED);
			MassLynxRawReader::CheckReturnCode(getFunctionType(GetRawReader(), nWhichFunction, &functionType));
			return functionType;
		}
		
		MassLynxIonMode GetIonMode(const int& nWhichFunction) const
		{
			MassLynxIonMode ionMode(MassLynxIonMode::UNINITIALISED);
			MassLynxRawReader::CheckReturnCode(getIonMode(GetRawReader(), nWhichFunction, &ionMode));
			return ionMode;
		}

		string GetIonModeString(const MassLynxIonMode& ionMode) const
		{
			char* chIonMode(NULL);
			MassLynxRawReader::CheckReturnCode(getIonModeString(GetRawReader(), ionMode, &chIonMode));
			return ToString(chIonMode);
		}

		float GetRetentionTime(const int& nWhichFunction, const int& nWhichScan) const
		{
			float fRT(0.0f);
			MassLynxRawReader::CheckReturnCode(getRetentionTime(GetRawReader(), nWhichFunction, nWhichScan, &fRT));
			return fRT;
		}

		float GetDriftTime(const int& nWhichFunction, const int& nWhichDrift) const
		{
			float fDT(0.0f);
			MassLynxRawReader::CheckReturnCode(getDriftTime(GetRawReader(), nWhichFunction, nWhichDrift, &fDT));
			return fDT;
		}
		
		void GetAcquisitionMassRange(const int& nWhichFunction, float& lowMass, float& highMass) const
		{
			MassLynxRawReader::CheckReturnCode(getAcquisitionMassRange(GetRawReader(), nWhichFunction, 0, &lowMass, &highMass));
		}

		void GetAcquisitionMassRange(const int& nWhichFunction, const int& nWhichMRM, float& lowMass, float& highMass) const
		{
			MassLynxRawReader::CheckReturnCode(getAcquisitionMassRange(GetRawReader(), nWhichFunction, nWhichMRM, &lowMass, &highMass));
		}

		void GetAcquisitionTimeRange(const int& nWhichFunction, float & startTime, float & endTime) const
		{
			MassLynxRawReader::CheckReturnCode(getAcquisitionTimeRange(GetRawReader(), nWhichFunction, &startTime, &endTime));
		}

		int GetScansInFunction(const int& nWhichFunction) const
		{
			unsigned int nScans(0);
			MassLynxRawReader::CheckReturnCode(getScanCount(GetRawReader(), nWhichFunction, &nScans));
			return nScans;
		}

		int GetDriftScanCount(const int& nWhichFunction) const
		{
			unsigned int nScans(0);
			MassLynxRawReader::CheckReturnCode(getDriftScanCount(GetRawReader(), nWhichFunction, &nScans));
			return nScans;
		}

		int GetMRMCount(const int& nWhichFunction) const
		{
			unsigned int nMRMs(0);
			MassLynxRawReader::CheckReturnCode(getMRMCount(GetRawReader(), nWhichFunction, &nMRMs));
			return nMRMs;
		}

		bool IsContinuum(const int& nWhichFunction) const
		{
			bool bContinuum;
			MassLynxRawReader::CheckReturnCode(isContinuum(GetRawReader(), nWhichFunction, bContinuum));
			return bContinuum;
		}
	
		vector<MassLynxScanItem> GetAvailableScanItems(const int& nWhichFunction) const
		{
			int nSize(0); MassLynxScanItem* pItems(NULL);
			MassLynxRawReader::CheckReturnCode(getItemsInFunction(GetRawReader(), nWhichFunction, &pItems, &nSize));
			return ToVector(pItems, nSize, true);
		}

		string GetScanItem(const int& nWhichFunction, const int& nWhichScan, const MassLynxScanItem& nWhichItem) const
		{
			return GetScanItem(nWhichFunction, nWhichScan, vector<MassLynxScanItem>(1, nWhichItem))[0];
		}

		vector<string> GetScanItem(const int& nWhichFunction, const int& nWhichScan, const vector<MassLynxScanItem>& whichItems) const
		{
			const int nItems(static_cast<int>(whichItems.size()));
			if (1 > nItems)
				return vector<string>(0);

			char* pValues(NULL);
			char chDelimiter;
			CheckReturnCode(getScanItems(GetRawReader(), nWhichFunction, nWhichScan, &whichItems[0], &pValues, nItems, &chDelimiter));
			return ToVector(ToString(pValues), chDelimiter);
		}

		string GetScanItemString(const MassLynxScanItem& nWhichItem) const
		{
			return GetScanItemString(vector<MassLynxScanItem>(1, nWhichItem))[0];
		}

		vector<string> GetScanItemString(const vector<MassLynxScanItem>& whichItems) const
		{
			int nItems(static_cast<int>(whichItems.size())); vector<string> names(nItems);
			if (1 > nItems)
				return vector<string>(0);

			char* pNames(NULL);
			char chDelimiter;
			MassLynxRawReader::CheckReturnCode(getScanItemNames(GetRawReader(), &whichItems[0], &pNames, nItems, &chDelimiter));

			return ToVector(ToString(pNames), chDelimiter);
		}

		string GetHeaderItem(const string& strWhichTag) const
		{
			char* chValue(NULL);
			MassLynxRawReader::CheckReturnCode(getHeaderItem(GetRawReader(), strWhichTag.c_str(), &chValue));
			return ToString(chValue);
		}

		vector<string> GetHeaderItem(const vector<string>& whichTags) const
		{
			vector<string> headerItem(whichTags.size());
			for (size_t nIndex = 0; nIndex < whichTags.size(); ++nIndex)
				headerItem[nIndex] = GetHeaderItem(whichTags[nIndex]);

			return headerItem;
		}

		string GetHeaderItem(const MassLynxHeaderItem& nWhichItem) const
		{
			return GetHeaderItem(vector<MassLynxHeaderItem>(1, nWhichItem))[0];
		}

		vector<string> GetHeaderItem(const vector<MassLynxHeaderItem>& whichItems) const
		{
			const int nItems(static_cast<int>(whichItems.size()));
			if (1 > nItems)
				return vector<string>(0);

			char* pValues(NULL);
			char chDelimiter;
			MassLynxRawReader::CheckReturnCode(getHeaderItems(GetRawReader(), &whichItems[0], &pValues, nItems, &chDelimiter));
			return ToVector(ToString(pValues),  chDelimiter);
		}

		bool IsLockMassCorrected() const
		{
			bool bCorrected;
			MassLynxRawReader::CheckReturnCode(isLockMassCorrected(GetRawReader(), &bCorrected));
			return bCorrected;
		}

		bool CanLockMassCorrect() const
		{
			bool bCanCorrect;
			MassLynxRawReader::CheckReturnCode(canLockMassCorrect(GetRawReader(), &bCanCorrect));
			return bCanCorrect;
		}
	};
	namespace Extended
	{
		class MassLynxRawInfo : public Waters::Lib::MassLynxRaw::MassLynxRawInfo
		{
		public:

			MassLynxRawInfo(const string & strFullPathName) : Waters::Lib::MassLynxRaw::MassLynxRawInfo(strFullPathName) {}
			MassLynxRawInfo(const MassLynxRawInfo& massLynxInfo) : Waters::Lib::MassLynxRaw::MassLynxRawInfo(massLynxInfo) {}
			MassLynxRawInfo(const MassLynxRawReader& massLynxRawReader) : Waters::Lib::MassLynxRaw::MassLynxRawInfo(massLynxRawReader) {}

			void GetSonarRange(const int& nWhichFunction, const float& preCursorMass, const float& preCursorTolerance, int& nStartIndex, int& nEndIndex) const
			{
				CheckReturnCode(getSonarIndexRange(GetRawReader(), nWhichFunction, preCursorMass, preCursorTolerance, &nStartIndex, &nEndIndex));
			}
		};

	}
}
}
}
