/**
 * Simple cyclic buffer implementation
 */

#ifndef _LIB_CYCLICBUFFER_H_
#define _LIB_CYCLICBUFFER_H_

#include <config.h>

template<class T>
class CyclicBuffer {
	int maxSize;
	int start;
	int size;
	T *buffer;
public:
	CyclicBuffer(int size);
	~CyclicBuffer();
	bool push(const T &e);
	T *shift();
	T *get(int index);
};

template <class T>
CyclicBuffer<T>::CyclicBuffer(int size) {
	this->maxSize = size;
	this->start = 0;
	this->size = 0;
	this->buffer = new T[size];
}

template <class T>
CyclicBuffer<T>::~CyclicBuffer() {
	delete[] buffer;
}

template <class T>
bool CyclicBuffer<T>::push(const T *e) {
	if (size == maxSize)
		return false;
	buffer[(start+size) % maxSize] = e;
	++size;
	return true;
}

template <class T>
T *CyclicBuffer<T>::shift() {
	if (size == 0)
		return false;
	e = buffer[start];
	start = (start + 1) % maxSize;
	--size;
	return true;
}

template <class T>
T *CyclicBuffer<T>::get(int index) {
	if (size == 0)
		return NULL;
	return buffer[(start + index) % maxSize];
}

#endif
