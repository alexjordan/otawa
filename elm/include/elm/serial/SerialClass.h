/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * elm/serial/SerialClass.h -- SerialClass and GenSerialClass interfaces.
 */
#ifndef ELM_SERIAL_SERIAL_CLASS_H
#define ELM_SERIAL_SERIAL_CLASS_H

#include <elm/string.h>
#include <elm/genstruct/HashTable.h>

namespace elm { namespace serial {

// External classes
class Unserializer;

// SerialClass class
class SerialClass {
private:
	CString _name;

	SerialClass *next;
	static bool initialized;
	static SerialClass *first;
	static genstruct::HashTable<CString, SerialClass *> classes;
	static void initialize(void);
	
public:
	SerialClass(elm::CString name);
	virtual ~SerialClass(void) { }
	inline CString name(void) const { return _name; };
	static SerialClass *find(CString name);
	
	virtual void *create(void) = 0;
	virtual void unserialize(void *ptr, Unserializer& unserializer) = 0;
};


// Templated serialized class
template <class T>
class GenSerialClass: public SerialClass {
public:
	inline GenSerialClass(CString name): SerialClass(name) { }
	
	// SerialClass overload
	virtual void *create(void) {
		return new T;
	}
	
	virtual void unserialize(void *ptr, Unserializer& unserializer) {
		((T *)ptr)->__serialize(unserializer);
	}
	
};

} } // elm::serial

#endif // ELM_SERIAL_SERIAL_CLASS_H
