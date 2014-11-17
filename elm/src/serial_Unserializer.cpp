/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * src/serial_Unserializer.cpp -- Unserializer clas implementation.
 */
#ifndef ELM_SERIAL_UNSERIALIZER_H
#define ELM_SERIAL_UNSERIALIZER_H

#include <elm/serial/Unserializer.h>

namespace elm { namespace serial {

/**
 * @class Unserializer
 * This interface must be implemented by unserializer engines.
 * @deprecated
 */


/**
 * @fn void Unserializer::close(void);
 * This method must be called at the end of the unserialization in order
 * to resolve lacking refercnes.
 */


/**
 * @fn void Unserializer::read(T& val);
 * Unserialize the given object. There is specialization for each base type.
 * @param val	Variable to unserialize in.
 */


/**
 * @fn void Unserializer::readPointer(void *&ptr);
 * Unserialize a pointer.
 * @param ptr	Pointer to record in.
 */


/**
 * @fn void Unserializer::beginObject(CString name);
 * Begin to unserialize an object.
 * @param name	Name of the object class.
 */


/**
 * @fn void Unserializer::endObject(void);
 * End of unserialization of an object.
 */


/**
 * @fn bool Unserializer::beginField(CString name);
 * A field begin to be unserialized.
 * @name	Name of the field.
 * @return	True if the field may be unserialised, false else.
 */


/**
 * @fn void Unserializer::endField(void);
 * The current field unserialization is ended.
 */


/**
 * @fn Unserializer& Unserializer::operator>>(T& val);
 * Perform unserialization of the given value.
 * @param val	Value to unserialize in.
 */
	
} } // elm::serial

#endif // ELM_SERIAL_UNSERIALIZER_H
