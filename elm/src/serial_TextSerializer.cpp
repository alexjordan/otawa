/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * elm/serial/TextSerializer.h -- Serializer class interface.
 */

#include <elm/io.h>
#include <elm/serial/TextSerializer.h>

namespace elm { namespace serial {


/**
 * @class TextSerializer
 * Serialize object in textual readable form.
 * @deprecated
 */


/**
 * Build a text serializer.
 */
TextSerializer::TextSerializer(void): field(false) {
}


/**
 */
void TextSerializer::checkField(void) {
	if(field) {
		cout << "; ";
		field = false;
	}
}


/**
 */
void TextSerializer::write(bool val) {
	cout << (val ? "true" : "false");
	checkField();
}


/**
 */
void TextSerializer::write(char val) {
	cout << '\'' << val << '\'';
	checkField();
}


/**
 */
void TextSerializer::write(unsigned char val) {
	cout << '\'' << (unsigned int)val << '\'';
	checkField();
}


/**
 */
void TextSerializer::write(short val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(unsigned short val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(long val) {
	cout << val;
	checkField();
}

/**
 */
void TextSerializer::write(unsigned long val) {
	cout << val;
	checkField();
}

/**
 */
void TextSerializer::write(int val) {
	cout << val;
	checkField();
}

/**
 */
void TextSerializer::write(unsigned int val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(long long val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(unsigned long long val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(float val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(double val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(CString val) {
	cout << val;
	checkField();
}


/**
 */
void TextSerializer::write(String& val) {
	cout << val;
	checkField();
}


/**
 */
bool TextSerializer::writePointer(const void *ptr) {
	cout << io::hex(t::intptr(ptr));
	checkField();
	return ptr && !objects.get(ptr);
}


/**
 */
void TextSerializer::delayObject(const void *ptr, delay_t fun) {
	to_process.put(elm::Pair<const void *, delay_t>(ptr, fun));
	objects.put(ptr, 1);
}


/**
 */
void TextSerializer::beginObject(CString name, const void *ptr) {
	objects.put(ptr, 1);
		cout << name << '(' << io::hex(t::intptr(ptr)) << ") = {";
}


/**
 */
void TextSerializer::endObject(void) {
	cout << "}";
	checkField();
}


/**
 */
void TextSerializer::writeField(CString name) {
	field = true;
	cout << ' ' << name << " = ";
}
	

/**
 */	
void TextSerializer::close(void) {
	while(to_process) {
		to_process->snd(*this, to_process->fst);
		to_process.get();
	}
}

} } // elm::serial
