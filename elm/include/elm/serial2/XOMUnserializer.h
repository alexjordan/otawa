/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * XOMUnserializer class interface.
 */
#ifndef ELM_SERIAL2_XOM_UNSERIALIZER_H
#define ELM_SERIAL2_XOM_UNSERIALIZER_H

#include <elm/genstruct/HashTable.h>
#include <elm/genstruct/Vector.h>
#include <elm/serial2/Unserializer.h>
#include <elm/util/Pair.h>

namespace elm {

// Extern classes
namespace xom {
	class Document;
	class Element;
}

namespace serial2 {

// XOMUnserializer class
class XOMUnserializer: public Unserializer {
public:
	XOMUnserializer(xom::Element *element);
	XOMUnserializer(elm::CString path);
	~XOMUnserializer(void);

	// XOMUnserializer overload
	virtual void flush(void);
	virtual void beginObject(AbstractType& clazz, void *object);
	virtual void endObject(AbstractType& clazz, void *object);
	virtual bool beginField(CString name);
	virtual void endField(void);
	virtual void onPointer(AbstractType& clazz, void **object);
	virtual bool beginCompound(void*);
	virtual int countItems(void);
	virtual bool nextItem(void);
	virtual void endCompound(void*);
	virtual int onEnum(AbstractEnum& clazz); 
	virtual void onValue(bool& v);
	virtual void onValue(signed int& v);
	virtual void onValue(unsigned int& v);
	virtual void onValue(char& v);
	virtual void onValue(signed char& v);
	virtual void onValue(unsigned char& v);
	virtual void onValue(signed short& v);
	virtual void onValue(unsigned short& v);
	virtual void onValue(signed long& v);
	virtual void onValue(unsigned long& v);
	virtual void onValue(signed long long& v);
	virtual void onValue(unsigned long long& v);
	virtual void onValue(float& v);
	virtual void onValue(double& v);
	virtual void onValue(long double& v);
	virtual void onValue(CString& v);
	virtual void onValue(String& v);

private:
	typedef struct patch_t {
		struct patch_t *next;
		void **ptr;
		inline patch_t(void **_ptr): ptr(_ptr) { };
	} patch_t;

	typedef struct ref_t {
		AbstractType& t;
		void *ptr;
		patch_t *patches;
		inline ref_t(AbstractType& type, void *_ptr = 0) : t(type), ptr(_ptr), patches(0) { };
		void put(void **_ptr);
		void record(void *_ptr);
	} ref_t;

	typedef struct context_t {
		int i;
		xom::Element *elem;
	} context_t;

	xom::Document *doc;
	context_t ctx;
	elm::io::Input in;
	elm::genstruct::HashTable<CString,  ref_t *> refs;
	elm::genstruct::Vector<context_t> stack;
	elm::genstruct::Vector<Pair<cstring, ref_t *> > pending;
	
	void embed(AbstractType& clazz, void **ptr);
}; 

} } // elm::serial2

#endif // ELM_SERIAL2_XOM_UNSERIALIZER_H

