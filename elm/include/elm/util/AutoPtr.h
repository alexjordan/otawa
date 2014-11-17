/*
 * $Id$
 * Copyright (c) 2006, IRIT-UPS <casse@irit.fr>
 *
 * elm/util/AutoPtr.h -- AutoPtr class.
 */
#ifndef ELM_UTIL_AUTOPTR_H
#define ELM_UTIL_AUTOPTR_H

#include <elm/assert.h>

namespace elm {

// Lock class
class Lock {
	int usage;
public:
	inline Lock(int _usage = 0);
	virtual ~Lock(void) { };
	inline void lock(void);
	inline void unlock();
};


// AutoPtr class
template <class T> class AutoPtr {
	Lock *ptr;
	static inline Lock *null(void);
public:
	inline AutoPtr(T *p = 0);
	inline AutoPtr(const AutoPtr& locked);
	inline ~AutoPtr(void);
	inline AutoPtr& operator=(const AutoPtr& locked);
	inline AutoPtr& operator=(T *p);
	inline T *operator->(void) const;
	inline T& operator*(void) const;
	inline T *operator&(void) const;
	inline bool isNull(void) const;
	inline operator bool(void) const { return !isNull(); };
	inline bool operator==(const AutoPtr<T>& ap) const;
	inline bool operator!=(const AutoPtr<T>& ap) const;
	inline bool operator>(const AutoPtr<T>& ap) const;
	inline bool operator>=(const AutoPtr<T>& ap) const;
	inline bool operator<(const AutoPtr<T>& ap) const;
	inline bool operator<=(const AutoPtr<T>& ap) const;
};

// Lock methods
Lock::Lock(int _usage): usage(_usage) {
}
void Lock::lock(void) {
	usage++;
}
void Lock::unlock() {
	if(!(--usage))
		delete this;
}


// AutoPtr methods
template <class T> Lock *AutoPtr<T>::null(void) {
	static Lock _null(1);
	return &_null;
}
template <class T> AutoPtr<T>::AutoPtr(T *p): ptr(p ? (Lock *)p : null()) {
	ptr->lock();
}
template <class T> AutoPtr<T>::~AutoPtr(void) {
	ptr->unlock();
}
template <class T> T *AutoPtr<T>::operator->(void) const {
	ASSERTP(!isNull(), "accessing null pointer");
	return (T *)ptr;
}
template <class T> AutoPtr<T>::AutoPtr(const AutoPtr& locked): ptr(locked.ptr) {
	ptr->lock();
}
template <class T> AutoPtr<T>& AutoPtr<T>::operator=(const AutoPtr& locked) {
	ptr->unlock();
	ptr = locked.ptr;
	ptr->lock();
	return *this;
}
template <class T> AutoPtr<T>& AutoPtr<T>::operator=(T *p) {
	ptr->unlock();
	ptr = p ? p : null();
	ptr->lock();
	return *this;
}
template <class T> bool AutoPtr<T>::isNull(void) const {
	return ptr == null();
}
template <class T> T& AutoPtr<T>::operator*(void) const {
	ASSERTP(!isNull(), "accessing null pointer");
	return *(T *)ptr;
}

template <class T> T *AutoPtr<T>::operator&(void) const {
	return isNull() ? 0 : (T *)ptr;
}
template <class T> bool AutoPtr<T>::operator==(const AutoPtr<T>& ap) const {
	return ptr == ap.ptr;
}
template <class T> bool AutoPtr<T>::operator!=(const AutoPtr<T>& ap) const {
	return ptr != ap.ptr;
}
template <class T> bool AutoPtr<T>::operator>(const AutoPtr<T>& ap) const {
	return ptr > ap.ptr;
}
template <class T> bool AutoPtr<T>::operator>=(const AutoPtr<T>& ap) const {
	return ptr >= ap.ptr;
}
template <class T> bool AutoPtr<T>::operator<(const AutoPtr<T>& ap) const {
	return ptr < ap.ptr;
}
template <class T> bool AutoPtr<T>::operator<=(const AutoPtr<T>& ap) const {
	return ptr <= ap.ptr;
}

} // elm

#endif // ELM_UTIL_AUTOPTR_H

