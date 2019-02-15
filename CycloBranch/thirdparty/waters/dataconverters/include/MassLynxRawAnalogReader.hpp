//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawAnalogReader.hpp
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
	using std::string;
	using std::vector;

	class MassLynxRawAnalogReader : public MassLynxRawReader
	{
	public:
		MassLynxRawAnalogReader(const std::string & strFullPathName) : MassLynxRawReader(strFullPathName, MassLynxReaderType::analog) { }
		MassLynxRawAnalogReader(const MassLynxRawReader& massLynxRawReader) : MassLynxRawReader(massLynxRawReader, MassLynxReaderType::analog) {}
		MassLynxRawAnalogReader(const MassLynxRawAnalogReader& massLynxRawAnalogReader) : MassLynxRawReader(massLynxRawAnalogReader, MassLynxReaderType::analog) {}

		int GetChannelCount() const
		{
			int nChannels(0);
			CheckReturnCode(getChannelCount(GetRawReader(), &nChannels));
			return nChannels;
		}

		string GetChannelDescription(const int& nWhichChannel) const
		{
			char* chValue(NULL);
			CheckReturnCode(getChannelDesciption(GetRawReader(), nWhichChannel, &chValue));
			return ToString(chValue);
		}

		string GetChannelUnits(const int& nWhichChannel) const
		{
			char* chValue(NULL);
			CheckReturnCode(getChannelUnits(GetRawReader(), nWhichChannel, &chValue));
			return ToString(chValue);
		}

		int ReadChannel(const int& nWhichChannel, vector<float>& times, vector<float>& intensities) const
		{
			// get the data..
			float* pTimes(NULL); float* pIntensities(NULL); int nSize(0);
			CheckReturnCode(readChannel(GetRawReader(), nWhichChannel, &pTimes, &pIntensities, &nSize));

			// fill the vector and deallocate the memory
			ToVector( pTimes, nSize, times, true);
			ToVector( pIntensities, nSize, intensities, true);

			return nSize;
		}
	};
}
}
}

