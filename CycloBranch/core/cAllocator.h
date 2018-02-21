/**
	\file cAllocator.h
	\brief An allocator of objects.
*/


#ifndef _CALLOCATOR_H
#define _CALLOCATOR_H

#include <vector>

using namespace std;


/**
	\brief An allocator of objects.
*/
template <class T>
class cAllocator
{

	vector<T*> strips;
	int itemsperstrip;

	int currentfreestrip;
	int currentfreeitem;

public:


	/**
		\brief The constructor.
	*/ 
	cAllocator();


	/**
		\brief The destructor.
	*/ 
	~cAllocator();


	/**
		\brief Remove all allocated items.
	*/ 
	void reset();


	/**
		\brief Get a reference to a new item.
		\retval reference to a new item
	*/ 
	T* getNewItem();

};


template <class T>
cAllocator<T>::cAllocator() {
	strips.clear();
	itemsperstrip = 500;
	currentfreestrip = 0;
	currentfreeitem = 0;

	T* newstrip = new T[itemsperstrip];
	strips.push_back(newstrip);
}


template <class T>
cAllocator<T>::~cAllocator() {
	for (int i = 0; i < (int)strips.size(); i++) {
		delete [] strips[i];
	}
}


template <class T>
void cAllocator<T>::reset() {
	for (int i = 0; i < (int)strips.size(); i++) {
		delete [] strips[i];
	}

	strips.clear();
	itemsperstrip = 500;
	currentfreestrip = 0;
	currentfreeitem = 0;

	T* newstrip = new T[itemsperstrip];
	strips.push_back(newstrip);
}


template <class T>
T* cAllocator<T>::getNewItem() {
	T* freeitem = &strips[currentfreestrip][currentfreeitem];

	currentfreeitem++;
	if (currentfreeitem == itemsperstrip) {
		currentfreestrip++;
		currentfreeitem = 0;
		T* newstrip = new T[itemsperstrip];
		strips.push_back(newstrip);
	}

	return freeitem;
}


#endif

