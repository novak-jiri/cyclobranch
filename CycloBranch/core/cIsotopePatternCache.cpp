#include "core/cIsotopePatternCache.h"


cIsotopePatternCache::cIsotopePatternCache(int size) {
	clear();
	maxnumberofitems = max(size, 1);
}


cIsotopePatternCache::~cIsotopePatternCache() {
}


bool cIsotopePatternCache::get(string& formula, cPeaksList& isotopepattern) {
	auto found = cache_map.find(formula);
	
	// pattern is not in the cache
	if (found == cache_map.end()) {
		return false;
	}

	// read the pattern from the list
	isotopepattern = found->second->second;

	// pattern is at the end of the list
	if (found->second == prev(cache_list.end())) {
		return true;
	}

	// move the used pattern to the end of the list (LRU cache)
	cache_list.splice(cache_list.end(), cache_list, found->second);
	found->second = prev(cache_list.end());

	return true;
}


void cIsotopePatternCache::put(string& formula, cPeaksList& isotopepattern) {
	// remove the least recently used item from the map and from the head of the list
	if ((int)cache_map.size() >= maxnumberofitems) {
		cache_map.erase(cache_list.front().first);
		cache_list.pop_front();
	}

	// insert new item into the map and at the tail of the list
	cache_list.push_back(make_pair(formula, isotopepattern));
	cache_map.insert(make_pair(formula, prev(cache_list.end())));
}


void cIsotopePatternCache::clear() {
	cache_map.clear();
	cache_list.clear();
}

