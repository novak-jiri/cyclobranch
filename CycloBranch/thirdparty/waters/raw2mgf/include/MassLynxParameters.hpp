#pragma once

#include <vector>
#include "MassLynxRawDefs.h"
#include "MassLynxRawScanReader.hpp"

#include <sstream>

namespace Waters
{
namespace Lib
{
namespace MassLynxRaw
{
	using std::string;

	class MassLynxParameters
	{
	public:
		MassLynxParameters()
		{
			createParameters(&m_pParameters);
		}
		~MassLynxParameters()
		{
			destroyParameters(m_pParameters);
		}

		template< typename T>
		MassLynxParameters& Set(const MassLynxScanItem& key, const T& value)
		{
			const string strKey = std::to_string((int)key);
			return Set(strKey, value);
		}

		MassLynxParameters& Set(const string& key, const string& value)
		{
			setParameters(m_pParameters, key.c_str(), value.c_str());
			return *this;
		}

		MassLynxParameters& Set(const string& key, const char*& value)
		{
			setParameters(m_pParameters, key.c_str(), value);
			return *this;
		}

		MassLynxParameters& Set(const string& key, const float& value)
		{
			Set(key, std::to_string(value));
			return *this;
		}

		MassLynxParameters& Set(const string& key, const double& value)
		{
			Set(key, std::to_string(value));
			return *this;
		}

		MassLynxParameters& Set(const string& key, const int& value)
		{
			Set(key, std::to_string(value));
			return *this;
		}

		MassLynxParameters& Set(const string& key, const vector<int>& value)
		{
			std::stringstream ss;
			ss << '[';
			if (value.size() > 0)
				ss << value[0];

			for (size_t nIndex(1); nIndex < value.size(); ++nIndex)
				ss << ',' << value[nIndex];

			ss << ']';
			Set(key, ss.str());
			return *this;
		}

		//MassLynxParameters& Set(const string& key, const bool& value)
		//{

		//	if (value)
		//		Set(key, (string)"True");
		//	else
		//		Set(key, (string)"False");
		//	return *this;
		//}


		string	ToJSON() const
		{
			char* chJSON(NULL);
			MassLynxRawReader::CheckReturnCode(getParameterJSON(m_pParameters, &chJSON));
			return MassLynxRawReader::ToString(chJSON);
		}

	private:
		CMassLynxParameters m_pParameters;
	};

}   // MassLynxRaw
}   // Lib
}   // Waters