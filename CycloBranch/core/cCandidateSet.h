/**
	\file cCandidateSet.h
	\brief The representation of a set of peptide sequence candidates.
*/


#ifndef _CCANDIDATESET_H
#define _CCANDIDATESET_H

#include <QMutex>

#include "core/cCandidate.h"


/**
	\brief The class representing a set of peptide sequence candidates.
*/
class cCandidateSet : public QMutex {

	unordered_set<cCandidate, hash_cCandidate> candidates;

public:

	/**
		\brief The constructor.
	*/ 
	cCandidateSet();


	/**
		\brief Get the size of the set.
		\retval int size of the set
	*/ 
	int size();


	/**
		\brief Access to the set of candidates.
		\retval unordered_set<cCandidate, hash_cCandidate> reference to the set of candidates
	*/ 
	unordered_set<cCandidate, hash_cCandidate>& getSet();

};


#endif

