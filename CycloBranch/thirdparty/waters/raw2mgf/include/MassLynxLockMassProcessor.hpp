//-----------------------------------------------------------------------------------------------------
// FILE:			MassLynxLockmassProcessor.h
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

class MassLynxLockMassProcessor :public MassLynxRawProcessor
{
public:
	MassLynxLockMassProcessor() : MassLynxRawProcessor(MassLynxReaderType::lockmass) {}

	void RemoveLockMassCorrection()
	{
		CheckReturnCode(removeLockMassCorrection(GetRawProcessor()));
	}

	bool IsLockMassCorrected() const
	{
		bool bIsCorrected(false);
		CheckReturnCode(_isLockMassCorrected(GetRawProcessor(), &bIsCorrected));
		return bIsCorrected;
	}

	bool CanLockMassCorrect() const
	{
		bool bCanCorrect(false);
		CheckReturnCode(_canLockMassCorrect(GetRawProcessor(), &bCanCorrect));
		return bCanCorrect;
	}

	void GetLockMassValues(float& mass, float& tolerance) const
	{
		CheckReturnCode(getLockMassValues(GetRawProcessor(), &mass, &tolerance));
	}

	void GetLockMassCorrection(const float& retentionTime, float& gain) const
	{
		CheckReturnCode(getLockMassCorrection(GetRawProcessor(), retentionTime, &gain));
	}

	void SetParameters(const std::string& lockMassParameters)
	{
		CheckReturnCode(setLockMassParameters(GetRawProcessor(), lockMassParameters.c_str()));
	}

	string GetParameters() const
	{
		char* pParameters(NULL);
		CheckReturnCode(getLockMassParameter(GetRawProcessor(), &pParameters));
		return MassLynxRawReader::ToString(pParameters);
	}

	bool LockMassCorrect(const std::string& lockMassParameters)
	{
		SetParameters(lockMassParameters);
		return LockMassCorrect();
	}

	bool LockMassCorrect()
	{
		bool bCorrected(false);
		CheckReturnCode(lockMassCorrect(GetRawProcessor(), &bCorrected));
		return bCorrected;
	}

	void GetCandidates(vector<float> & masses, vector<float> & intensities)
	{
		// get the data..
		float* pMasses(NULL); float* pIntensities(NULL); int nSize(0);
		CheckReturnCode(getLockMassCandidates(GetRawProcessor(), &pMasses, &pIntensities, &nSize));

		// fill the vector and deallocate the memory
		MassLynxRawReader::ToVector(pMasses, nSize, masses,true);
		MassLynxRawReader::ToVector(pIntensities, nSize, intensities, true);
	}
};

}   // MassLynxRaw
}   // Lib
}   // Waters
