/*
 * $Id$
 * Copyright (c) 2004, Alfheim Corporation.
 *
 * elm/string/StringBuffer.h -- interface for StringBuffer class.
 */
#ifndef ELM_STRING_STRING_BUFFER_H
#define ELM_STRING_STRING_BUFFER_H

#include <elm/string/CString.h>
#include <elm/string/String.h>
#include <elm/io/BlockOutStream.h>
#include <elm/io/Output.h>

namespace elm {

// StringBuffer class
class StringBuffer: public io::Output {
	io::BlockOutStream _stream;
public:
	inline StringBuffer(int capacity = 64, int increment = 32);
	inline String toString(void);
	inline String copyString(void);
	inline int length(void) const;
	inline void reset(void);
	inline io::OutStream& stream(void);
};


// Inlines
inline StringBuffer::StringBuffer(int capacity, int increment)
: io::Output(_stream), _stream(capacity, increment) {
	String::buffer_t str = { 0 };
	_stream.write((char *)&str, sizeof(short));
}

inline String StringBuffer::toString(void) {
	int len = length();
	_stream.write('\0');
	return String((String::buffer_t *)_stream.detach(), sizeof(short), len);
}

inline int StringBuffer::length(void) const {
	return _stream.size() - sizeof(short);
}

inline void StringBuffer::reset(void) {
	_stream.setSize(sizeof(short));
}

inline io::OutStream& StringBuffer::stream(void) {
	return _stream;
}

inline String StringBuffer::copyString(void) {
	return String(
		_stream.block() + sizeof(short),
		_stream.size() - sizeof(short));
}

} // elm

#endif // ELM_STRING_STRING_BUFFER_H
