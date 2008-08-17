/**
 * Simple cyclic buffer implementation
 */

#include "CyclicBuffer.h"

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
inline bool CyclicBuffer<T>::push(const T &e) {
	if (size == maxSize)
		return false;
	buffer[start+size % maxSize] = e;
	size++;
	return true;
}

template <class T>
inline bool CyclicBuffer<T>::pop(T &e) {
	if (size == 0)
		return false;
	e = buffer[start];
	start = (start + 1) % maxSize;
	size--;
	return true;
}
