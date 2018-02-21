#include "core/cCandidateSet.h"


cCandidateSet::cCandidateSet() {
	candidates.clear();
}


int cCandidateSet::size() {
	return (int)candidates.size();
}


unordered_set<cCandidate, hash_cCandidate>& cCandidateSet::getSet() {
	return candidates;
}

