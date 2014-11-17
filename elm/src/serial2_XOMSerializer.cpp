/*
 * $Id$
 * Copyright (c) 2006, IRIT - UPS.
 *
 * src/serial_XOMSerializer.cpp -- XOMSerializer class implementation.
 */
 
#include <elm/assert.h>
#include <elm/io.h>
#include <elm/io/OutFileStream.h>
#include <elm/xom.h>
#include <elm/xom/Attribute.h>
#include <elm/xom/Serializer.h>
#include <elm/serial2/collections.h>
#include <elm/serial2/serial.h>
#include <elm/serial2/XOMSerializer.h> 

using namespace elm::genstruct;

namespace elm { namespace serial2 {

/**
 * @class XOMSerializer
 * @ingroup serial
 */



/**
 * Build a serializer to the given XOM document.
 * @param path	Path document to serialize to.
 */
XOMSerializer::XOMSerializer(elm::CString _path)
: doc(0), path(_path), ref_current(0) {
	xom::Element *elem = new xom::Element("root");	
	doc = new xom::Document(elem);	
	ASSERT(doc->getRootElement() != NULL);
	ctx.elem = elem;

}


/**
 */
XOMSerializer::~XOMSerializer(void) {
	flush();
	if(doc)
		delete doc;
}

void XOMSerializer::flush(void) {
	
	//ASSERT(doc->getRootElement() == ctx.elem);
	ASSERT(doc->getRootElement() != NULL);
	ctx.elem = doc->getRootElement();
	
	cout << "Processing des trucs dynamiques\n";
	while (!toprocess.isEmpty()) {
		Pair<AbstractType *, const void*> obj = toprocess.get();
		if (!done.hasKey(obj.snd)) {
			

			xom::Element *current = new xom::Element("dyn-obj");
			ctx.elem->appendChild(current);
			ctx.elem = current; 
			cout << "Serialization d un objet de type" << obj.fst->name() << "\n";				 
			obj.fst->serialize(*this, obj.snd);
			ctx.elem = doc->getRootElement();			
			done.put(obj.snd, ctx.elem);
		}
	}
	cout << "Fin de processing des trucs dynamiques\n";
	io::OutFileStream outfile(path.chars());
	xom::Serializer ser(outfile);
	ser.write(doc);
	ser.flush();
}


void XOMSerializer::beginObject(AbstractType& clazz, const void *object) {
	
	ASSERT(doc->getRootElement() != NULL);
	if (done.hasKey(object)) {
	
	    if (ctx.elem->getAttributeValue("class")) {
	    	cout << "Objet deja recopie, on fait rien.\n";
	    	return;
	    } else {	    
	    	cout << "Objet deja traite, on recopie!\n";
	    }
		/* move the XML node */
		xom::Element *src = done.get(object, 0);
		ASSERT(src != 0);
		
		for (int i = 0; i < src->getChildCount(); i++) {
			xom::Node *tmp = src->getChild(0);
			src->removeChild(0);
			ctx.elem->appendChild(tmp);
		}
		doc->getRootElement()->removeChild(src);
		/* delete src; XXX TODO */
		
	} else {
		
		String id = _ << refGet(object);
		
		xom::String *xom_id = new xom::String(id.toCString());
		
		
		if (ctx.elem->getAttributeValue("class")) {
			cout << "CLASSE DE BASE: " <<  (clazz.name()) << "\n";
			
			
		} else {	
			CString name = clazz.name();	
			cout << "LA CLASSE C EST: " << name << "\n";
			xom::Attribute *attr  = new xom::Attribute("class", name);	
			ctx.elem->addAttribute(attr);
		}	
	
		xom::Attribute *attr2 = new xom::Attribute("id", *xom_id);
		ctx.elem->addAttribute(attr2);
	}

}

void XOMSerializer::endObject(AbstractType& type, const void *object) {
	cout << "Fin objet!\n";
//		done.put(object, ctx.elem);

	done.put(object, ctx.elem);
	ASSERT(doc->getRootElement() != NULL);
}


void XOMSerializer::beginField(CString name) {
	ASSERT(doc->getRootElement() != NULL);
	cout << "BEGIN FIELD: " << name << "\n";
	xom::Element *current = new xom::Element(name);
	
	ctx.elem->appendChild(current);
	stack.push(ctx); 
	ctx.elem = current; 
}

void XOMSerializer::endField(void) {
	cout << "END FIELD\n";
	ctx = stack.pop();
}

void XOMSerializer::onPointer(AbstractType& clazz, const void *object) {
	ASSERT(doc->getRootElement() != NULL);
	String id = _ << refGet(object); 
	xom::String *xom_id = new xom::String(id.toCString());
	
	/* if a pointed object has never been seen, add it to the "seen" and "toprocess" lists */
	if (object && !seen.hasKey(object)) {
		seen.put(object, true);
		toprocess.put(pair(&clazz,object));		
	}
	/* in any case, put a ref to the object in the parent field */
	xom::Attribute *attr = new xom::Attribute("ref", object ? *xom_id : "NULL");
	ctx.elem->addAttribute(attr);	
		
	/* if pointed object has never been seen, add it to "seen" list and "dynamic objects" list */
	/*
	if (!seen.hasKey(object)) {
		xom::Element *current = new xom::Element("dyn-obj");
		
		seen.put(object, true);
		dyn_obj.put(object, current);		
		stack.push(ctx);
		ctx.elem = current;		
		pointer = true;
		clazz.serialize(*this, object);
		ctx = stack.pop(); 
	}
	*/		
}

void XOMSerializer::beginCompound(const void *object) {
	ASSERT(doc->getRootElement() != NULL);
	String id = _ << refGet(object);
	xom::String *xom_id = new xom::String(id.toCString());
                                  	
	xom::Attribute *attr = new xom::Attribute("id", *xom_id);
	ctx.elem->addAttribute(attr);
	xom::Element *elem = new xom::Element("item");
	ctx.elem->appendChild(elem);
	stack.push(ctx);
	ctx.elem = elem;
	ctx.firstItem = true;
}

void XOMSerializer::endCompound(const void *object) {
		ctx = stack.pop();
}

void XOMSerializer::onEnum(const void *address, int value, AbstractEnum& clazz) {
	ASSERT(doc->getRootElement() != NULL);
	ctx.elem->appendChild(clazz.nameOf(value));	
}
 
void XOMSerializer::onValue(const bool& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);
}

void XOMSerializer::onValue(const signed int& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onItem(void) {
	if (ctx.firstItem) {
	  ctx.firstItem = false;
	} else { 
	  ctx = stack.pop();
	  xom::Element *elem = new xom::Element("item");
	  ctx.elem->appendChild(elem);
	  stack.push(ctx);
	  ctx.elem = elem;
	}
}

void XOMSerializer::onValue(const unsigned int& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const char& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const signed char& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const unsigned char& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const signed short& v) {
	
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const unsigned short& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
	
}

void XOMSerializer::onValue(const signed long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const unsigned long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const signed long long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const unsigned long long& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const float& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const double& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const long double& v) {
	/* !! TODO !! */
	String str1 = _ << ((double)v);
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);
	 
}

void XOMSerializer::onValue(const CString& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}

void XOMSerializer::onValue(const String& v) {
	String str1 = _ << v;
	xom::String str2(str1.toCString());
	ctx.elem->appendChild(str2);	
}


} } // elm::serial
