/**
	\file cIsotopePatternCache.h
	\brief Isotope pattern cache (LRU cache is implemented).
*/


#ifndef _CISOTOPEPATTERNCACHE_H
#define _CISOTOPEPATTERNCACHE_H


#include <QMutex>
#include <unordered_map>
#include <list>
#include "core/utilities.h"
#include "core/cPeaksList.h"


/**
	\brief Isotope pattern cache (LRU cache is implemented).
*/
class cIsotopePatternCache : public QMutex {

	unordered_map<string, list<pair<string, cPeaksList> >::iterator > cache_map;
	list<pair<string, cPeaksList> > cache_list;

	int maxnumberofitems;

public:


	/**
		\brief The constructor.
		\param size maximum number of cached items
	*/
	cIsotopePatternCache(int size = 10000);


	/**
		\brief The destructor.
	*/
	~cIsotopePatternCache();


	/**
		\brief Get the isotope pattern corresponding to the formula from the cache.
		\param formula input summary formula
		\param isotopepattern output isotope pattern
		\retval bool true if the isotope pattern was found; false otherwise.
	*/
	bool get(string& formula, cPeaksList& isotopepattern);


	/**
		\brief Put the isotope pattern corresponding to the formula into the cache.
		\param formula input summary formula
		\param isotopepattern input isotope pattern
	*/
	void put(string& formula, cPeaksList& isotopepattern);


	/**
		\brief Clear the cache.
	*/
	void clear();

};


#endif

