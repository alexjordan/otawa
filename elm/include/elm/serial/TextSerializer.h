/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * elm/serial/TextSerializer.h -- Serializer class interface.
 */
#ifndef ELM_SERIAL_TEXT_SERIALIZER_H
#define ELM_SERIAL_TEXT_SERIALIZER_H

#include <elm/serial/Serializer.h>
#include <elm/genstruct/HashTable.h>
#include <elm/genstruct/VectorQueue.h>
#include <elm/util/Pair.h>

namespace elm { namespace serial {

// TextSerializer class
class TextSerializer: public Serializer {
	bool field;
	elm::genstruct::HashTable<const void *, int> objects;
	elm::genstruct::VectorQueue<elm::Pair<const void *, delay_t> > to_process;
	void checkField(void);

protected:
	virtual void delayObject(const void *ptr, delay_t fun);

public:
	TextSerializer(void);
	
	// Serializer overload
	virtual void close(void);
	virtual void write(bool val);
	virtual void write(char val);
	virtual void write(unsigned char val);
	virtual void write(short val);
	virtual void write(unsigned short val);
	virtual void write(long val);
	virtual void write(unsigned long val);
	virtual void write(int val);
	virtual void write(unsigned int val);
	virtual void write(long long val);
	virtual void write(unsigned long long val);
	virtual void write(float val);
	virtual void write(double val);
	virtual void write(CString val);
	virtual void write(String& val);
	virtual bool writePointer(const void *ptr);
	virtual void beginObject(CString name, const void *ptr);
	virtual void endObject(void);
	virtual void writeField(CString name);
};

} } // elm::serial

#endif // ELM_SERIAL_TEXT_SERIALIZER_H
