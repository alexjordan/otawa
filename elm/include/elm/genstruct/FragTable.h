/*
 *	$Id$
 *	FragTable class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
 * 
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software 
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef ELM_GENSTRUCT_FRAGTABLE_H
#define ELM_GENSTRUCT_FRAGTABLE_H

#include <elm/assert.h>
#include <elm/genstruct/Vector.h>

namespace elm { namespace genstruct {

// FragTable class
template <class T> class FragTable {
	genstruct::Vector<T *> tab;
	int size, msk, shf, used;
public:
	inline FragTable(int size_pow = 8);
	inline FragTable(const FragTable& tab) { addAll(tab); }
	inline ~FragTable(void);
	
	// Collection concept
	inline int count (void) const { return length(); }
	inline bool isEmpty(void) const { return tab.count() == 0; }
 	inline operator bool (void) const { return !isEmpty(); }
	inline bool contains(const T &item) const
		{ for(int i = 0; i < length(); i++)
			if(get(i) == item) return true; return false; } 

	class Iterator: public PreIterator<Iterator, T> {
	public:
		inline Iterator(const FragTable<T>& array, int pos = 0);
		inline Iterator(const Iterator& iter)
			: arr(iter.arr), i(iter.i), len(iter.len) { }
		inline void next(void);
		inline const T& item(void) const;
		inline bool ended(void) const;
	protected:
		friend class FragTable;
		const FragTable<T> *arr;
		int i, len;
	};
 	
	// MutableCollection concept
	inline void clear(void);
	inline void add(const T &item);
	template <template <class _> class C >
	void addAll(const C<T> &items)
		{ for(typename C<T>::Iterator i(items); i; i++) add(i); }
	inline void remove(const T &item)
		{ for(Iterator i(*this); i; i++) if(i == item) { remove(i); break; } }
	inline void remove(const Iterator &iter) { removeAt(iter.i); }
	template <template <class _> class C >
	void removeAll(const C<T> &items)
		{ for(typename C<T>::Iterator i(items); i; i++) remove(i); }

	// Array concept
	inline int length(void) const;
	inline const T& get(int index) const;
	inline int indexOf(const T &value, int start = 0) const
		{ for(Iterator i(*this, start); i; i++)
			if(i == value) return i.i; return -1; }
	inline int lastIndexOf(const T &value, int start = -1) const
		{ for(int i = (start < 0 ? length() : start) - 1; i >= 0; i--)
			if(get(i) == value) return i; return -1; }
	inline const T& operator[](int index) const { return get(index); }
	
	// MutableArray concept
	void shrink(int length);
	inline void set(int index, const T &item);	
	void set(const Iterator &iter, const T &item) { set(iter.i, item); }
	inline T &get(int index);
	inline T &operator[](int index) { return get(index); }
	void insert(int index, const T &item);
	inline void insert(const Iterator &iter, const T &item)
		{ insert(iter.i, item); }
	void removeAt(int index);
	inline void removeAt(const Iterator &iter) { removeAt(iter.i); }
	
	// Methods
	int alloc(int count);

	// Deprecated
	inline void clean(void) { clear(); }
	inline FragTable<T>& operator+=(T value);	
};


// FragTable<T> Inlines
template <class T>
inline FragTable<T>::FragTable(int size_pow)
: size(1 << size_pow), msk(size - 1), shf(size_pow), used(size) {
	ASSERTP(size_pow > 0, "size must be greater than 0");
}

template <class T>
inline FragTable<T>::~FragTable(void) {
	for(int i = 0; i < tab.count(); i++)
		delete [] tab[i];
}

template <class T>
inline void FragTable<T>::clear(void) {
	for(int i = 0; i < tab.count(); i++)
		delete [] tab[i];
	tab.setLength(0);
	used = size;
}

template <class T>
inline int FragTable<T>::length(void) const {
	return ((tab.count() - 1) << shf) + used;
}

template <class T>
inline const T& FragTable<T>::get(int index) const {
	ASSERTP(index >= 0 && index < length(), "index out of bounds");
	return tab[index >> shf][index & msk];
}

template <class T>
inline T& FragTable<T>::get(int index) {
	ASSERTP(index >= 0 && index < length(), "index out of bounds");
	return tab[index >> shf][index & msk];
}

template <class T>
inline void FragTable<T>::set(int index, const T& value) {
	ASSERTP(index >= 0 && index < length(), "index out of bounds");
	tab[index >> shf][index & msk] = value;
}

template <class T>	
inline void FragTable<T>::add(const T& value) {
	if(used >= size) {
		tab.add(new T[size]);
		used = 0;
	}
	tab[tab.length() - 1][used++] = value;
}

template <class T>
inline FragTable<T>& FragTable<T>::operator+=(T value) {
	add(value);
	return *this;
}

template <class T>
int FragTable<T>::alloc(int count) {
	int res = length();
	while(count >= size - used) {
		count -= size - used;
		tab.add(new T[size]);
		used = 0;
	}
	used += count;
	return res;
}


template <class T>
void FragTable<T>::shrink(int length) {
	ASSERTP(length < this->length(), "length too big");
	int nl = (length + msk) >> shf;
	for(int i = nl; i < tab.count(); i++)
		delete [] tab[i];
	tab.setLength(nl);
	used = length & msk;
	if(!used)
		used = size; 
}

template <class T>
void FragTable<T>::insert(int index, const T &item) {
	ASSERTP(index >= 0 && index <= length(), "index out of bounds"); 
	int len = length();
	alloc(1);
	for(int i = len - 1; i >= index; i--)
		set(i + 1, get(i));
	set(index, item);
}


template <class T>
void FragTable<T>::removeAt(int index) {
	int len = length();
	for(int i = index + 1; i <= len; i++)
		set(i - 1, get(i));
	used--;
	if(!used) {
		delete [] tab[tab.count() - 1];
		tab.setLength(tab.count() - 1);
		used = size;
	}
}


// FragTable<T>::Iterator inlines
template <class T>
inline FragTable<T>::Iterator::Iterator(const FragTable<T>& array, int pos)
: arr(&array), i(pos), len(array.count()) {
}

template <class T>
inline void FragTable<T>::Iterator::next(void) {
	ASSERT(i < len);
	i++;
}

template <class T>
inline const T& FragTable<T>::Iterator::item(void) const {
	return arr->get(i);
}

template <class T>
inline bool FragTable<T>::Iterator::ended(void) const {
	return i >= len;
}

} } // elm::genstruct

#endif	// ELM_GENSTRUCT_FRAGTABLE_H
