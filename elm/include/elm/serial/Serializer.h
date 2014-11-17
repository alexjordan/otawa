/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * elm/serial/Serializer.h -- Serializer clas interface.
 */
#ifndef ELM_SERIAL_SERIALIZER_H
#define ELM_SERIAL_SERIALIZER_H

#include <elm/string.h>

namespace elm { namespace serial {
	
// Serializer class
class Serializer {
	
	template <class T> static void delay(Serializer& serializer, const void *p) {
		serializer.process<T>(*(T *)p);
	}

	template <class T> void process(const T& val) { write(val); };

protected:
	typedef void (*delay_t)(Serializer& serializer, const void *p);
	virtual void delayObject(const void *ptr, delay_t fun) = 0;

public:
	virtual ~Serializer(void) { }
	virtual void close(void) = 0;
	
	// Write methods
	template <class T> inline void write(const T& val) { process<T>(val); };
	virtual void write(bool val) = 0;
	virtual void write(char val) = 0;
	virtual void write(unsigned char val) = 0;
	virtual void write(short val) = 0;
	virtual void write(unsigned short val) = 0;
	virtual void write(long val) = 0;
	virtual void write(unsigned long val) = 0;
	virtual void write(int val) = 0;
	virtual void write(unsigned int val) = 0;
	virtual void write(long long val) = 0;
	virtual void write(unsigned long long val) = 0;
	virtual void write(float val) = 0;
	virtual void write(double val) = 0;
	virtual void write(CString val) = 0;
	virtual void write(String& val) = 0;
	virtual bool writePointer(const void *ptr) = 0;
	virtual void beginObject(CString name, const void *ptr) = 0;
	virtual void endObject(void) = 0;
	virtual void writeField(CString name) = 0;

	// Operators 
	template <class T> inline Serializer& operator<<(const T& val) {
		write(val);
		return *this;
	}
	
	// Private
	template <class T> inline void processField(CString name, const T& val) {
		writeField(name);
		process<T>(val);
	};
};
	
} } // elm::serial

#endif // ELM_SERIAL_SERIALIZER_H
