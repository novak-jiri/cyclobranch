//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxRawReader.hpp
// DATE:			Nov 2016
// COPYRIGHT(C):	Waters Corporation
//
// COMMENTS:		This header contains the delclaration of the MassLynxRawReader
//					class.
//					
//-----------------------------------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include "MassLynxRawDefs.h"

///<sumary>
///Check a MassLynxRaw result code
///<\summary>

namespace Waters
{
namespace Lib
{
namespace MassLynxRaw
{
	using std::string;
	using std::vector;

	class MassLynxRawProcessor;
	static void __stdcall processor_callback(void* pObject, const int& percent);

	class MassLynxRawException : public std::exception
	{
	public:
		MassLynxRawException(const int& nCode, const string& message) : m_nCode(nCode), m_strMessage(message) {}
		virtual const char* what() const throw() { return m_strMessage.c_str(); }

		int code() const { return m_nCode; }

	private:
		std::string m_strMessage;
		int m_nCode;
	};

	class MassLynxRawReader
	{
	protected:
		MassLynxRawReader(const string & strFullPathName,  MassLynxReaderType nType = MassLynxReaderType::raw)
		{
			CheckReturnCode(createRawReaderFromPath(strFullPathName.c_str(), &m_pRawReader, nType));
		}

		MassLynxRawReader(const MassLynxRawReader& massLynxRawReader, MassLynxReaderType nType)
		{
			CheckReturnCode(createRawReaderFromReader(massLynxRawReader.GetRawReader(), &m_pRawReader, nType));
		}

		virtual ~MassLynxRawReader()
		{
			destroyRawReader(m_pRawReader);
			m_pRawReader = NULL;
		}


	public:
		CMassLynxRawReader const GetRawReader() const { return m_pRawReader; }

		static void CheckReturnCode(const int& nCode)
		{
			if (0 != nCode)
			{
				char* pErrorMessage(NULL);
				getErrorMessage(nCode, &pErrorMessage);
				throw MassLynxRawException(nCode, ToString(pErrorMessage));
			}
		}

		vector<string> ToVector(const string &s, char delim) const
		{
			// check if empty - return empty vector 
			if (s.empty())
				return vector<string>(0);

			auto pos = s.find(delim);

			// check if just one item
			if (string::npos == pos)
				return vector<string>{s};

			size_t i = 0;
			vector<string> elems;
			while (pos != string::npos) {
				elems.push_back(s.substr(i, pos - i));
				i = ++pos;
				pos = s.find(delim, pos);

				if (pos == string::npos)
					elems.push_back(s.substr(i, s.length()));
			}

			return elems;
		}

		static string ToString(const char* chString)
		{
			if (NULL == chString)
				return "";

			string strValue(chString);
			releaseMemory((void*&)chString);
			chString = NULL;

			return strValue;
		}

		template< typename T>
		static vector<T> ToVector(const T* pArray, const int& nSize, const bool& bRelease = false)
		{
			// do we have data?
			if (pArray == NULL)
				return vector<T>(0);

			// something wrong so just clean up
			if (nSize < 1)
			{
				if (bRelease)
					releaseMemory((void*)pArray);
				return vector<T>(0);
			}

			vector<T>templateVector(nSize);
			memcpy(&templateVector[0], pArray, nSize * sizeof(T));

			if (bRelease)
				releaseMemory((void*)pArray);

			return templateVector;
		}

		template< typename T>
		static int ToVector(const T* pArray, const int& nSize, vector<T>& templateVector, const bool& bRelease = false)
		{
			// do we have data?
			if (pArray == NULL)
			{
				templateVector.clear();
				return 0;
			}

			// something wrong so just clean up
			if (nSize < 1)
			{
				if (bRelease)
					releaseMemory((void*)pArray);

				templateVector.clear();
				return 0;
			}

			templateVector.resize(nSize);
			memcpy(&templateVector[0], pArray, nSize * sizeof(T));

			if (bRelease)
				releaseMemory((void*)pArray);

			return 0;
		}

	private:
		CMassLynxRawReader m_pRawReader;
	};

	using std::string;




	class MassLynxRawProcessor
	{

	public:
		MassLynxRawProcessor(const MassLynxReaderType& nType)
		{

			MassLynxRawReader::CheckReturnCode(createRawProcessor(&m_pMassLynxProcessor, nType, processor_callback, this));
		}

		MassLynxRawProcessor()
		{
			MassLynxRawReader::CheckReturnCode(createRawProcessor(&m_pMassLynxProcessor, MassLynxReaderType::centroid, processor_callback, this));
		}

		virtual ~MassLynxRawProcessor()
		{
			MassLynxRawReader::CheckReturnCode(destroyRawProcessor(m_pMassLynxProcessor));
		}

		void CheckReturnCode(const int& nCode) const
		{
			if (0 != nCode)
			{
				char* pErrorMessage(NULL);
				getProcessorMessage(m_pMassLynxProcessor, nCode, &pErrorMessage);
				throw MassLynxRawException(nCode, MassLynxRawReader::ToString(pErrorMessage));
			}
		}

		void SetRawReader(const MassLynxRawReader& mlRawReader)
		{
			MassLynxRawReader::CheckReturnCode(setRawReader(m_pMassLynxProcessor, mlRawReader.GetRawReader()));
		}

		void SetRawPath(const string& strFullPathName)
		{
			MassLynxRawReader::CheckReturnCode(setRawPath(m_pMassLynxProcessor, strFullPathName.c_str()));
		}

		virtual void Progress(const int& percent) {}

		void Centroid(const string& strOutputPathName) const
		{
			MassLynxRawReader::CheckReturnCode(centroidRaw(m_pMassLynxProcessor, strOutputPathName.c_str()));
		}

	protected:
		CMassLynxRawProcessor const GetRawProcessor() const { return m_pMassLynxProcessor; }

	private:
		CMassLynxRawProcessor m_pMassLynxProcessor;
	};

	static void __stdcall processor_callback(void* pObject, const int& percent)
	{
		// call the virtual update fn
		MassLynxRawProcessor* self = (MassLynxRawProcessor*)pObject;
		self->Progress(percent);
	}
}
}
}
