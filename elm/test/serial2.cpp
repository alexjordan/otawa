#include <elm/io.h>
#include <elm/serial2/serial.h>
#include <elm/serial2/TextSerializer.h>
#include <elm/genstruct/Vector.h>
#include <elm/serial2/collections.h>

using namespace elm;
using namespace elm::genstruct;

typedef enum color_t {
	RED,
	GREEN,
	BLUE
} color_t;
value_t color_t_values[] = {
	value("RED", RED),
	value("GREEN", GREEN),
	value("BLUE", BLUE)
};
ENUM(color_t);  // , color_t_values
ENUM_BEGIN(color_t)
	VALUE(RED),
	VALUE(GREEN),
	VALUE(BLUE)
ENUM_END


#if 0
namespace elm { namespace serial {

// Unserializer class
class Unserializer {
protected:
	virtual void beginObject(serial::Class& clazz, void *object) = 0;
	virtual void endObject(void) = 0;
	virtual void beginField(CString name) = 0;
	virtual void endField(void);
	virtual void onValue(int& v) = 0;

public:
	// Generic
	template <class T>
	Unserializer& operator&(T& v) {
		beginObject(T::__class, &v);
		v.__visit(*this);
		endObject();
		return *this;
	}
	
	template <class T>
	Unserializer& operator&(const Field<T>& field) {
		beginField(field.name());
		*this & field.value();
		endField();
		return *this;
	}
	
	template <class T>
	Unserializer& operator>>(T& v) { return *this & v; };

	// Specialized
};

template <> Unserializer& Unserializer::operator&(int& v) {
	onValue(v);
	return *this;
}
template <> Class& type(const int& data) { return TypeClass<int>::type; };



// TextSerializer class
class MyTextSerializer: public Serializer {
	genstruct::HashTable<const void *, Class *> refs;
protected:
	virtual void beginObject(serial::Class& clazz, const void *object) {
		cout << "*" << io::hex((unsigned long)object)
			 << ":" << clazz.name() << "={";
		refs.put(object, 0);
	}
	
	virtual void endObject(void) {
		cout << "};";
	}

	virtual void beginField(CString name) {
		cout << name << "=";
	}
	
	virtual void endField(void) {
	}

	virtual void onValue(const int& v) {
		cout << v << ';';
	}

	virtual void onPointer(serial::Class& clazz, const void *object) {
		cout << '&' << io::hex((unsigned long)object) << ';';
		if(!refs.get(object))
			refs.put(object, &clazz);
	}

public:
	virtual void close(void) {
		for(genstruct::HashTable<const void *, Class *>::ItemIterator ref(refs);
		ref; ref++)
			if(*ref) {
				cout << '!' << io::hex((unsigned long)ref.key()) << '=';
				ref->serialize(*this, ref.key());
			}
		refs.clear();
	}

};

} } // elm::serial


#endif





using namespace elm;

class A {
	int x;
	int y;
	int z;
public:
	A(void): x(111), y(666), z(0) { };
	virtual ~A(void) { }
	static serial2::Class<A> __class;
	virtual serial2::AbstractType& __getSerialClass(void) const { return __class; };
	template <class T> void __visit(T& e) {
		e & x & field("y", y) & z;
	}
};
serial2::Class<A> A::__class("A");


int main(void) {
	serial2::TextSerializer serializer;
	serializer << 666;
	A a, b;
	Vector<int> ints;
	for(int i = 0; i < 10; i++)
		ints.add(i);
	int z = 69;
	const int *pz = &z;
	color_t color = BLUE;
	
	serializer << a;
	serializer << &a << &b << &z << pz;
	serializer << ints;
	serializer << color;
	serializer.flush();
	cout << io::endl;
	return 0;
}
