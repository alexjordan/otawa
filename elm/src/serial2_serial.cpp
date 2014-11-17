/*
 * $Id$
 * Copyright (c) 2006, IRIT UPS.
 *
 * serialization implementation.
 */

#include <elm/serial2/serial.h>
#include <elm/genstruct/HashTable.h>
#include <elm/util/Initializer.h>

using namespace elm::genstruct;

namespace elm { namespace serial2 {


/**
 * Storage of all available classes.
 */
static HashTable<CString, AbstractType *> types; 


/**
 * Initializer hook
 */
static Initializer<AbstractType> initializer;


/**
 * @class AbstractType
 * Objects representing a type in the serialization module.
 * @ingroup serial
 */


/**
 * Build an abstract type.
 * @param name	Type name (full name path with namespaces).
 * @param base	Base type for a class.
 */
AbstractType::AbstractType(CString name, AbstractType *base)
: elm::AbstractClass(name, base) {
	initializer.record(this);
}


/**
 * For internal use only.
 */
void AbstractType::initialize(void) {
	types.add(name(), this);
}


/**
 * Find a type by its name.
 * @param name	Type name.
 * @return		Found type or NULL.
 */
AbstractType *AbstractType::getType(CString name) {
	return types.get(name, 0);
}


/**
 */
class VoidType: public AbstractType {
public:
	VoidType(void): AbstractType("<void>") { }
	virtual void *instantiate(void) { return 0; } 
	virtual void unserialize(Unserializer& unserializer, void *object) { }
	virtual void serialize(Serializer& serializer, const void *object) { }
};
static VoidType void_type;


/**
 * Void type representation.
 */
AbstractType& AbstractType::T_VOID = void_type;

	
} } // elm::serial2
