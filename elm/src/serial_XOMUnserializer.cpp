/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * src/serial_XOMUnserializer.cpp -- XOMUnserializer class implementation.
 */
#include <elm/assert.h>
#include <elm/xom.h>
#include <elm/io/BlockInStream.h>
#include <elm/serial/SerialClass.h>
#include <elm/serial/XOMUnserializer.h>

namespace elm { namespace serial {

/**
 * @class XOMUnserializer
 * Unserializer from XML using XOM API.
 * @deprecated
 */


/**
 */
void XOMUnserializer::ref_t::put(void *&_ptr) {
	if(ptr)
		_ptr = ptr;
	else {
		patch_t *patch = new patch_t(_ptr);
		patch->next = patches;
		patches = patch;
	}
}


/**
 */		
void XOMUnserializer::ref_t::record(void *_ptr) {
	ptr = _ptr;
	for(patch_t *patch = patches, *next; patch; patch = next) {
		next = patch->next;
		patch->ptr = ptr;
		delete patch;
	}
}


/**
 * Build the unserializer for using the given element.
 * @param element	XOM element to use.
 */				
XOMUnserializer::XOMUnserializer(xom::Element *element)
: doc(0) {
	ctx.elem = element;
}


/**
 * Build an unserializer from the given XOM document.
 * @param path	Path document to unserialize from.
 */
XOMUnserializer::XOMUnserializer(elm::CString path)
: doc(0) {
	ctx.elem = 0;
	xom::Builder builder;
	doc = builder.build(path);
	if(!doc)
		throw io::IOException(_ << "cannot open \"" << path << "\"");
	ctx.elem = doc->getRootElement();
	ASSERT(ctx.elem);
}


/**
 */
XOMUnserializer::~XOMUnserializer(void) {
	if(doc)
		delete doc;
}


/**
 */	
void XOMUnserializer::close(void) {
	for(genstruct::HashTable<CString,  ref_t *>::Iterator ref(refs); ref; ref++)
		if(!ref->ptr) {
			CString id = ref.key();
			throw io::IOException(_ << "unsolved reference \"" << id << "\"");
		}
}


/**
 */
void XOMUnserializer::readPointer(SerialClass& clazz, void *&ptr) {
	SerialClass *uclass = &clazz;
	
	// Is there a reference ?
	Option<xom::String> id = ctx.elem->getAttributeValue("ref");
	if(id) {
		ref_t *ref = refs.get(id, 0);
		if(!ref) {
			ref = new ref_t();
			refs.put(id, ref);
		}
		ref->put(ptr);
	}
	
	// Else it should be embedded
	else {
		
		// Find the class
		string clazz_name;
		Option<xom::String> name = ctx.elem->getAttributeValue("class");
		if(name) {
			clazz_name = name;
			uclass = SerialClass::find(&clazz_name);
			if(!uclass)
				throw io::IOException(_ << "no class " << clazz_name);
		}
		
		// Build the object
		ptr = uclass->create();
		beginObject(&clazz_name, ptr);
		uclass->unserialize(ptr, *this);
		endObject();
	}
}


/**
 */
void XOMUnserializer::beginObject(CString name, void *ptr) {
	Option<xom::String> id = ctx.elem->getAttributeValue("id");
	if(id) {
		ref_t *ref = refs.get(id, 0);
		if(ref)
			ref->record(ptr);
		else
			refs.put(id, new ref_t(ptr));
	}
}


/**
 */
void XOMUnserializer::endObject(void) {
}


/**
 */
bool XOMUnserializer::beginField(CString name) {
	xom::Element *felem = ctx.elem->getFirstChildElement(xom::String(name));
	if(!felem)
		return false;
	stack.push(ctx);
	ctx.elem = felem;
	return true;
}


/**
 */
void XOMUnserializer::endField(void) {
	ctx = stack.pop();
}


/**
 */
void XOMUnserializer::read(bool& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(char& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(unsigned char& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(short& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(unsigned short& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(unsigned long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(int& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(unsigned int& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(long long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(unsigned long long& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(float& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(double& val) {
	xom::String text = ctx.elem->getValue();
	io::BlockInStream block(text);
	in.setStream(block);
	in >> val;
	text.free();
}


/**
 */
void XOMUnserializer::read(CString& val) {
	val = ctx.elem->getValue();
}


/**
 */
void XOMUnserializer::read(String& val) {
	xom::String text = ctx.elem->getValue();
	val = String(text);
	text.free();
}


/**
 */
bool XOMUnserializer::beginList(void) {
	int cnt = ctx.elem->getChildCount();
	for(ctx.i = 0; ctx.i < cnt; ctx.i++) {
		xom::Node *node = ctx.elem->getChild(ctx.i);
		if(node->kind() == xom::Node::ELEMENT) {
			stack.push(ctx);
			ctx.elem = (xom::Element *)node;
			return true;
		}
	}
	return false;
}


/**
 */
void XOMUnserializer::endList(void) {
}


/**
 */
bool XOMUnserializer::nextItem(void) {
	ctx = stack.pop();
	int cnt = ctx.elem->getChildCount();
	for(ctx.i++; ctx.i < cnt; ctx.i++) {
		xom::Node *node = ctx.elem->getChild(ctx.i);
		if(node->kind() == xom::Node::ELEMENT) {
			stack.push(ctx);
			ctx.elem = (xom::Element *)node;
			return true;
		}
	}
	return false;
}


/**
 */
int XOMUnserializer::readEnum(Pair<elm::CString, int> values[]) {
	xom::String text = ctx.elem->getValue();
	for(int i = 0; values[i].fst; i++) {
		CString value = values[i].fst;
		int pos = value.lastIndexOf(':');
		if(pos >= 0)
			value = value.chars() + pos + 1;
		if(value == text) {
			text.free();
			return values[i].snd;
		}
	}
	String name = text;
	text.free();
	throw io::IOException(_ << "bad enumerated value \"" << name << "\"");
}

} } // elm::serial
