//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawScanReader.h
// DATE:			Nov 2016
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
		using std::vector;

		class MassLynxRawScanReader : public MassLynxRawReader
		{
		public:

			MassLynxRawScanReader(const string & strFullPathName) : MassLynxRawReader(strFullPathName, MassLynxReaderType::scan) {}
			MassLynxRawScanReader(const MassLynxRawScanReader& massLynxScanReader) : MassLynxRawReader(massLynxScanReader, MassLynxReaderType::scan) {}
			MassLynxRawScanReader(const MassLynxRawReader& massLynxRawReader) : MassLynxRawReader(massLynxRawReader, MassLynxReaderType::scan) {}

			void ReadScan(const int& nWhichFunction, const int& nWhichScan, vector<float> & masses, vector<float> & intensities)
			{
				// get the data..
				float* pMasses(NULL); float* pIntensities(NULL); int nSize(0);
				MassLynxRawReader::CheckReturnCode(readScan(GetRawReader(), nWhichFunction, nWhichScan, &pMasses, &pIntensities, &nSize));

				// fill the vector
				ToVector(pMasses, nSize, masses);
				ToVector(pIntensities, nSize, intensities);
			}

			void ReadScan(const int& nWhichFunction, const int& nWhichScan, vector<float> & masses, vector<float> & intensities, vector<char> & flags)
			{
				// get the data..
				float* pMasses(NULL); float* pIntensities(NULL); char* pFlags(NULL); int nSize(0);
				MassLynxRawReader::CheckReturnCode(readScanFlags(GetRawReader(), nWhichFunction, nWhichScan, &pMasses, &pIntensities, &pFlags, &nSize));

				// fill the vector
				ToVector(pMasses, nSize, masses);
				ToVector(pIntensities, nSize, intensities);
				ToVector(pFlags, nSize, flags);
			}

			void ReadScan(const int& nWhichFunction, const int& nWhichScan, vector<float> & masses, vector<float> & intensities, vector<float> & productMasses)
			{
				// get the data..
				float* pMasses(NULL); float* pIntensities(NULL); float* pProductMasses(NULL); int nSize(0); int nProductSize(0);
				MassLynxRawReader::CheckReturnCode(readDaughterScan(GetRawReader(), nWhichFunction, nWhichScan, &pMasses, &pIntensities, &pProductMasses, &nSize, &nProductSize));

				// fill the vector
				masses = ToVector(pMasses, nSize);
				intensities = ToVector(pIntensities, nSize);
				productMasses = ToVector(pProductMasses, nProductSize);
			}

			void ReadScan(const int& nWhichFunction, const int& nWhichScan, const int& nWhichDrift, vector<float> & masses, vector<float> & intensities)
			{
				// get the data..
				float* pMasses(NULL); float* pIntensities(NULL); int nSize(0);
				MassLynxRawReader::CheckReturnCode(readDriftScan(GetRawReader(), nWhichFunction, nWhichScan, nWhichDrift, &pMasses, &pIntensities, &nSize));

				// fill the vector
				ToVector(pMasses, nSize, masses);
				ToVector(pIntensities, nSize, intensities);
			}
		};

	namespace Extended
	{
		class MassLynxRawScanReader : public Waters::Lib::MassLynxRaw::MassLynxRawScanReader
		{
		public:
			using Waters::Lib::MassLynxRaw::MassLynxRawScanReader::ReadScan;

			MassLynxRawScanReader(const string & strFullPathName) : Waters::Lib::MassLynxRaw::MassLynxRawScanReader(strFullPathName) {}
			MassLynxRawScanReader(const MassLynxRawScanReader& massLynxScanReader) : Waters::Lib::MassLynxRaw::MassLynxRawScanReader(massLynxScanReader) {}
			MassLynxRawScanReader(const MassLynxRawReader& massLynxRawReader) : Waters::Lib::MassLynxRaw::MassLynxRawScanReader(massLynxRawReader) {}

			void ReadScan(const int& nWhichFunction, const int& nWhichScan, const int& nWhichDrift, vector<int>& masses, vector<float>& intensities)
			{
				// get the data..
				int* pMasses(NULL); float* pIntensities(NULL); int nSize(0);
				MassLynxRawReader::CheckReturnCode(readDriftScanIndex(GetRawReader(), nWhichFunction, nWhichScan, nWhichDrift, &pMasses, &pIntensities, &nSize));

				// fill the vector
				ToVector(pMasses, nSize, masses);
				ToVector(pIntensities, nSize, intensities);
			}

			void ReadScan(const int& nWhichFunction, const int& nWhichScan, const int& nWhichDrift, vector<int>& masses, vector<float>& intensities, vector<char>& flags)
			{
				// get the data..
				int* pMasses(NULL); float* pIntensities(NULL); char* pFlags(NULL); int nSize(0);
				MassLynxRawReader::CheckReturnCode(readDriftScanFlagsIndex(GetRawReader(), nWhichFunction, nWhichScan, nWhichDrift, &pMasses, &pIntensities, &pFlags, &nSize));

				// fill the vector
				ToVector(pMasses, nSize, masses);
				ToVector(pIntensities, nSize, intensities);
				ToVector(pFlags, nSize, flags);
			}

			void GetMassScale(const int& nWhichFunction, const int& nWhichScan, vector<float>&masses)
			{
				// get the data
				int nSize(0); float* pMasses(NULL);
				MassLynxRawReader::CheckReturnCode(getDriftMassScale(GetRawReader(), nWhichFunction, nWhichScan, &pMasses, &nSize));

				// fill the vector
				ToVector(pMasses, nSize, masses);
			}
		};
	}
}   // MassLynxRaw
}   // Lib
}   // Waters

