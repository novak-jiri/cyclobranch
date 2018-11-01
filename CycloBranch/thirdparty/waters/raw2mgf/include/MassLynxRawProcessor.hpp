//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawProcessor.h
// DATE:			July 2017
// COPYRIGHT(C):	Waters Corporation
//
//-----------------------------------------------------------------------------------------------------
#pragma once

#include "MassLynxRawBase.hpp"

namespace Waters
{
namespace Lib
{
namespace MassLynxRaw
{
	class MassLynxRawDDAProcessor : public MassLynxRawProcessor
	{
	public:
		MassLynxRawDDAProcessor() : MassLynxRawProcessor(MassLynxReaderType::dda) {}

		void GetIndex(vector<float>& RTs, vector<float>& setmasses)
		{
			// get the data..
			float* pRTs(NULL); float* pMasses(NULL); int nSize(0);
			MassLynxRawReader::CheckReturnCode(ddaRaw(GetRawProcessor(), &pRTs, &pMasses, &nSize));

			// fill the vector and deallocate the memory
			MassLynxRawReader::ToVector(pRTs, nSize, RTs, true);
			MassLynxRawReader::ToVector(pMasses, nSize, setmasses, true);
		}

		void GetScan(const int& nWhichIndex, const MassLynxScanType& mlScanType, vector<float>& masses, vector<float>& intensities)
		{
			// get the data..
			float* pMasses(NULL); float* pIntensities(NULL); int nSize(0);
			MassLynxRawReader::CheckReturnCode(ddaGetScan(GetRawProcessor(), nWhichIndex, mlScanType, &pMasses, &pIntensities, &nSize));

			// fill the vector
			MassLynxRawReader::ToVector(pMasses, nSize, masses);
			MassLynxRawReader::ToVector(pIntensities, nSize, intensities);
		}

		bool GetNextScan(float& fRT, MassLynxScanType& mlScanType, float& fSetMass, vector<float>& masses, vector<float>& intensities) const
		{
			// get the data..
			float* pMasses(NULL); float* pIntensities(NULL); int nSize(0); bool bNext(false);
			MassLynxRawReader::CheckReturnCode(ddaGetNextScan(GetRawProcessor(), &fRT, &mlScanType, &fSetMass, &pMasses, &pIntensities, &nSize, &bNext));

			// fill the vector
			MassLynxRawReader::ToVector(pMasses, nSize, masses);
			MassLynxRawReader::ToVector(pIntensities, nSize, intensities);

			return bNext;
		}

		void Reset() const
		{
			// reset the scan iterator
			MassLynxRawReader::CheckReturnCode(ddaResetScan(GetRawProcessor()));
		}
	};

	class MassLynxRawMSeProcessor : public MassLynxRawProcessor
	{
	public:
		MassLynxRawMSeProcessor() : MassLynxRawProcessor(MassLynxReaderType::mse) {}

		bool GetNextScan(float& fRT, MassLynxScanType& mlScanType, vector<float>& masses, vector<float>& intensities) const
		{
			// get the data..
			float* pMasses(NULL); float* pIntensities(NULL); int nSize(0); bool bNext(false);
			MassLynxRawReader::CheckReturnCode(mseGetNextScan(GetRawProcessor(), &fRT, &mlScanType, &pMasses, &pIntensities, &nSize, &bNext));

			// fill the vector and deallocate the memory
			MassLynxRawReader::ToVector(pMasses, nSize, masses);
			MassLynxRawReader::ToVector(pIntensities, nSize, intensities);

			return bNext;
		}

		void Reset() const
		{
			// reset the scan iterator
			MassLynxRawReader::CheckReturnCode(mseResetScan(GetRawProcessor()));
		}
	};
}
}
}

