//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawChromatogramReader.h
// DATE:			Nov 2016
// COPYRIGHT(C):	Waters Corporation
//
// COMMENTS:		This header contains the declaration of the MassLynxRawChromatogramReader
//					class.  The class allows the TIC and BPI chromatograms to be read as well
//                  as general mass chromatograms.
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
	using std::string;
	using std::vector;

	class MassLynxRawChromatogramReader : public MassLynxRawReader
	{
	public:

		MassLynxRawChromatogramReader(const std::string & strFullPathName) : MassLynxRawReader(strFullPathName, MassLynxReaderType::chrom) { }
		MassLynxRawChromatogramReader(const MassLynxRawReader& massLynxRawReader) : MassLynxRawReader(massLynxRawReader, MassLynxReaderType::chrom) { }
		MassLynxRawChromatogramReader(const MassLynxRawChromatogramReader& massLynxRawChromatogramReader) : MassLynxRawReader(massLynxRawChromatogramReader, MassLynxReaderType::chrom) { }

		int ReadTICChromatogram(const int& nWhichFunction, vector<float> & times, vector<float> & intensities) const
		{
			// get the data..
			float* pTimes(NULL); float* pIntensities(NULL); int nSize(0);
			CheckReturnCode(readTICChromatogram(GetRawReader(), nWhichFunction, &pTimes, &pIntensities, &nSize));

			// fill the vector and deallocate the memory
			ToVector( pTimes, nSize, times, true);
			ToVector( pIntensities, nSize, intensities, true);

			return nSize;
		}

		int ReadBPIChromatogram(const int& nWhichFunction, vector<float> & times, vector<float> & intensities)
		{
			// get the data..
			float* pTimes(NULL); float* pIntensities(NULL); int nSize(0);
			CheckReturnCode(readBPIChromatogram(GetRawReader(), nWhichFunction, &pTimes, &pIntensities, &nSize));

			// fill the vector and deallocate the memory
			ToVector(pTimes, nSize, times, true);
			ToVector(pIntensities, nSize, intensities, true);

			return nSize;
		}

		int ReadMassChromatogram(const int& nWhichFunction, const float& fMass, vector<float> & times, vector<float> & intensities, const float& fMassWindow, bool bProducts)
		{
			// get the data..
			float* pTimes(NULL); int nSize(0); float* pIntensities;
			CheckReturnCode(readMassChromatograms(GetRawReader(), nWhichFunction, &fMass, 1, &pTimes, &pIntensities, fMassWindow, bProducts, &nSize));

			// fill the vector and deallocate the memory
			ToVector(pTimes, nSize, times, true);
			ToVector(pIntensities, nSize, intensities, true);

			return nSize;
		}

		int ReadMultipleMassChromatograms(const int& nWhichFunction, const vector<float>& masses, vector<float>& times, vector<std::vector<float>>& intensities, const float& fMassWindow, bool bProducts = true)
		{
			// get the data..
			int nMasses(static_cast<int>(masses.size())); float* pTimes(NULL); vector< float*> pIntensities(nMasses, NULL); float** ppIntensities = &pIntensities[0];	int nSize(0);
			CheckReturnCode(readMassChromatograms(GetRawReader(), nWhichFunction, &masses[0], nMasses, &pTimes, ppIntensities, fMassWindow, bProducts, &nSize));

			// fill the vector and deallocate the memory
			ToVector( pTimes, nSize, times, true);
			intensities.resize(nMasses);
			for (int nIndex = 0; nIndex < nMasses; ++nIndex)
				ToVector( pIntensities[nIndex], nSize, intensities[nIndex], true);

			return nSize;
		}
		
		int ReadMRMChromatogram(const int& nWhichFunction, const int& nWhichMRM, std::vector<float> & times, std::vector<float> & intensities)
		{
			// get the data..
			float* pTimes(NULL); int nSize(0); float* pIntensities;
			CheckReturnCode(readMRMChromatograms(GetRawReader(), nWhichFunction, &nWhichMRM, 1, &pTimes, &pIntensities, &nSize));

			// fill the vector and deallocate the memory
			ToVector(pTimes, nSize, times, true);
			ToVector(pIntensities, nSize, intensities, true);

			return nSize;
		}

		int ReadMRMChromatogram(const int& nWhichFunction, const vector<int> whichMRM, vector<float> & times, vector<vector<float>>& intensities)
		{
			// get the data..
			int nMRMs(static_cast<int>(whichMRM.size())); float* pTimes(NULL); vector< float*> pIntensities(nMRMs, NULL); float** ppIntensities = &pIntensities[0];	int nSize(0);
			CheckReturnCode(readMRMChromatograms(GetRawReader(), nWhichFunction, &whichMRM[0], nMRMs, &pTimes, ppIntensities, &nSize));

			// fill the vector and deallocate the memory
			ToVector(pTimes, nSize, times, true);
			intensities.resize(nMRMs);
			for (int nIndex = 0; nIndex < nMRMs; ++nIndex)
				ToVector(pIntensities[nIndex], nSize, intensities[nIndex], true);

			return nSize;
		}

		void ReadMassChromatogram(const int& nWhichFunction, const float& fPrecursorMass, const float& fPrecursorTolerance, const float& fMass, const float& fMassTolerance, vector<float> & times, vector<float> & intensities)
		{
			// get the data..
			float* pTimes(NULL); float* pIntensities(NULL); int nSize(0);
			CheckReturnCode(readSonarMassChromatogram(GetRawReader(), nWhichFunction, fPrecursorMass, fMass, &pTimes, &pIntensities, fPrecursorTolerance * 2, fMassTolerance * 2, &nSize));

			// fill the vector and deallocate the memory
			ToVector(pTimes, nSize, times, true);
			ToVector(pIntensities, nSize, intensities, true);
		}
	};
}
}
}