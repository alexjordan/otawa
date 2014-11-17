/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * elm/serial/SerialTable.h -- SerialTable<T> class interface.
 */
#ifndef ELM_SERIAL_SERIAL_TABLE_H
#define ELM_SERIAL_SERIAL_TABLE_H

#include <elm/serial/SerialClass.h>
#include <elm/genstruct/Table.h>
#include <elm/genstruct/Vector.h>
/*#include <elm/serial/Serializer.h>
#include <elm/serial/Unerializer.h>*/

namespace elm { namespace serial {

// Extern Classes
class Serializer;
class Unserializer;

// SerialTable<T> Interface
template <class T>
class SerialTable: public genstruct::AllocatedTable<T> {
public:
	virtual ~SerialTable(void) { }
	inline SerialTable(void);
	inline SerialTable(T *table, int count);
 	inline SerialTable(const genstruct::Table<T> &table);

	static SerialClass& __class;
	virtual void __serialize(Serializer& _serializer) const;
	void __serialize(Unserializer& _serializer);
};

// SerialTable<T> Implementation
template <class T>
inline SerialTable<T>::SerialTable(void) {
}

template <class T>
inline SerialTable<T>::SerialTable(T *table, int count)
: genstruct::AllocatedTable<T>(table, count) {
}

template <class T>
inline SerialTable<T>::SerialTable(const genstruct::Table<T> &table)
: genstruct::AllocatedTable<T>(table) {
}

#ifdef ELM_SERIAL_IMPLEMENT_H

template <class T>
void SerialTable<T>::__serialize(Serializer& _serializer) const {

}

template <class T>
void SerialTable<T>::__serialize(Unserializer& _serializer) {
	genstruct::Vector<T> vec;
	for(bool next = _serializer.beginList(); next; next = _serializer.nextItem()) {
		vec.add();
		_serializer >> vec[vec.count() - 1];
	}
	_serializer.endList();
	copy(vec.detach());
}

template <class T>
elm::serial::SerialClass& SerialTable<T>::__class
	= __make_class<SerialTable<T> >("elm::serial::Table<T>");

#endif // ELM_SERIAL_IMPLEMENT_H 

} } // elm::serial

#endif // ELM_SERIAL_SERIAL_TABLE_H
