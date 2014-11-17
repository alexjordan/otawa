/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * elm/serial/implement.h -- implementation for unserialization.
 */
#ifndef ELM_SERIAL_IMPLEMENT_H
#define ELM_SERIAL_IMPLEMENT_H

#include <elm/string.h>
#include <elm/serial/SerialClass.h>
#include <elm/serial/Serializer.h>
#include <elm/serial/Unserializer.h>

namespace elm { namespace serial {

template <class T>
elm::serial::SerialClass& __make_class(elm::CString name) {
	static elm::serial::GenSerialClass<T> clazz(name);
	return clazz;
}

// Macros
#define FIELD(name) 			_serializer.processField(#name, name)
#define ON_SERIAL(code) 		if(__serial) { code; }
#define ON_UNSERIAL(code)		if(!__serial) { code; }
#define SERIALIZE_BASE(clazz)	clazz::__serialize(_serializer)

#define REFLEXIVITY(clazz) \
	elm::serial::SerialClass& clazz::__class = elm::serial::__make_class<clazz>(#clazz);

#define SERIALIZE(clazz, fields) \
	REFLEXIVITY(clazz) \
	void clazz::__serialize(elm::serial::Serializer& _serializer) const  { \
		bool __serial; \
		__serial = true; \
		fields; \
	} \
	void clazz::__serialize(elm::serial::Unserializer& _serializer) { \
		bool __serial; \
		__serial = false; \
		fields; \
	} \
	namespace elm { namespace serial { \
	template <> void Serializer::process(const clazz& _obj) { \
		beginObject(#clazz, &_obj); \
		_obj.__serialize(*this); \
		endObject(); \
	} \
	template <> inline void Serializer::process<clazz *>(clazz * const &val) { \
		if(writePointer(val)) \
			delayObject(val, Serializer::delay<clazz>); \
	} \
	} }

#define ENUM_VALUE(n)	elm::Pair<elm::CString, int>(#n, n)

#define SERIALIZE_ENUM(t, vals...) \
	namespace elm { namespace serial { \
	template <> \
	inline void elm::serial::Unserializer::read<t>(t& val) { \
		static elm::Pair<CString, int> values[] = { vals, Pair<CString, int>("", 0) }; \
		val = (t)readEnum(values); \
	} \
	} }

} } // elm::serial

#endif // ELM_SERIAL_IMPLEMENT_H
