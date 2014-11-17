/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * src/serial_Serializer.h -- Serializer class implementation.
 */

#include <elm/serial/Serializer.h>

namespace elm { namespace serial {

/**
 * @class Serializer
 * Interface implemented by objets providing serialization facilities.
 * @deprecated
 */


/**
 * @fn void Serializer::delayObject(const void *ptr, delay_t fun);
 * When an object cannot be serialized immediately, this function is called
 * to record it for future serialization.
 * @param ptr	Delayed objet pointer.
 * @param fun	fun used to perform the serialization.
 */


/**
 * @fn void Serializer::close(void);
 * Stop the serialization. Delayed serialization may be performed.
 */


/**
 * @fn void Serializer::write(const T& val);
 * Add the given value to the serialization.
 * @param val	Value to serialize.
 * @note There is specialization of this method for each base type. 
 */


/**
 * @fn bool Serializer::writePointer(const void *ptr);
 * Serialize a pointer.
 * @param ptr	Pointer of the object to serialize.
 * @return		True if the pointer has been serialized, false if the object
 * serialization needs to be delayed.
 */


/**
 * @fn void Serializer::beginObject(CString name, const void *ptr);
 * Serialization of an object begins.
 * @param name	Object class name.
 * @param ptr	Pointer to the object.
 */


/**
 * @fn void Serializer::endObject(void);
 * End of the current object serialization.
 */


/**
 * @fn void Serializer::writeField(CString name);
 * A field will be serialized.
 * @param name	Field name.
 */

} } // elm::serial
