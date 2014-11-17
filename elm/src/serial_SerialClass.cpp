/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * src/serial_SerialClass.cpp -- SerialClass and GenSerialClass implementation.
 */

#include <elm/io.h>
#include <elm/serial/SerialClass.h>

namespace elm { namespace serial {

/**
 * @class SerialClass
 * A serial class records the list of existing serializable classes.
 * @deprecated
 */


/**
 * True if the classes has been initialized.
 */
bool SerialClass::initialized = false;


/**
 * List head of classes to initialize.
 */
SerialClass *SerialClass::first = 0;


/**
 * Name map to serializable classes.
 */
genstruct::HashTable<CString, SerialClass *> SerialClass::classes;


/**
 * Initialize the serial classes declared during application initialization.
 */
void SerialClass::initialize(void) {
	for(SerialClass *clazz = first; clazz; clazz = clazz->next)
		classes.put(clazz->_name, clazz);
	initialized = true;
}


/**
 * Build a serial class.
 * @param name	Class name.
 */
SerialClass::SerialClass(elm::CString name) : _name(name) {
	//cout << "CLASS " << name << " DECLARED (" << first << ") !\n";
	if(initialized)
		classes.put(name, this);
	else {
		next = first;
		first = this;
	}
}


/**
 * @fn CString SerialClass::name(void) const;
 * Get class name.
 * @return	Class name.
 */


/**
 * Find a class by its name.
 * @param name	Name of looked class.
 * @return		Found class or null.
 */
SerialClass *SerialClass::find(CString name) {
	if(!initialized)
		initialize();
	return classes.get(name, 0);
}


/**
 * @fn void *SerialClass::create(void);
 * Called to create an object of this class.
 * @return	Created object.
 */


/**
 * @fn void SerialClass::unserialize(void *ptr, Unserializer& unserializer);
 * Proceed to unserialization from the given unserializer on the given object.
 */


/**
 * @class GenSerialClass
 * Auto-builder for serializable class.
 * @param T	Class to be recorded.
 */


/**
 * @fn GenSerialClass::GenSerialClass(CString name);
 * Build the serial class.
 * @param name	Name of the class.
 */


/*
 * !!TODO!!
 * Current serialization implementation is very, very harmful !
 * It must replaced quickly before it will be largely used. The future
 * serialization must meet the following requirements :
 * (1) only one function to handle the serialization of members,
 * (2) checking of type consistency with polymorphic pointer / reference,
 * (3) full support of base class serialization with management of possible
 * name clashes,
 * (4) simple support of specialization through simple template specialization,
 * (5) real implementation of serialization must be hidden as possible,
 * (6) ways to implement classical symbolic types (enum, set of bits, binary
 * blocks and so one) must be provided.
 */

} } // elm::serial
