/*
 *	$Id$
 *	Main documentation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
 * 
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software 
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef ELM_DOC_H_
#define ELM_DOC_H_

/**
 * @mainpage
 * 
 * ELM is a multi-purpose library used by the OTAWA project (http://www.otawa.fr).
 * It provides :
 * @li generic data structures,
 * @li generic interface with te operating system (files, command line, etc),
 * @li a checked plugin system,
 * @li efficient string class,
 * @li other utility classes.
 * 
 * Please take a look at the module part for detailed description.
 * 
 * @par Type Facilities
 * 
 * ELM provides a lot of OS-independent simple and reliable types in @ref int
 * together with efficient complex integer functions.
 * 
 * The @ref string module provides several class to manage strings:
 * C-string wrapper (@ref elm::CString), mostly 0 cost substring extraction,
 * fast and light string class  (@ref elm::String), string buffer (@ref elm::StringBuffer) to build strings
 * and automatic string building class (@ref elm::AutoString) with very handy syntax (@c _ << a1 << a2 << ...).
 * 
 * Facilities to handle efficiently are also provided in @ref array.
 *
 * The @ref types module provides RTTI support to handle types easily.
 * Used with the <elm/meta.h> primitives, it allows to build
 * powerful template classes and functions.
 * 
 * @par Data Structures
 * 
 * One goal of ELM is to provide very powerful and efficient data structures.
 * They includes:
 * @li array management (@ref elm::genstruct::Vector, @ref elm::genstruct::AllocatedTable, @ref elm::genstruct::FixArray, @ref elm::genstruct::FragTable, @ref elm::genstruct::Table),
 * @li list management (@ref elm::genstruct::SLList, @ref elm::genstruct::DLList),
 * @li tree management (@ref elm::avl::Tree, @ref elm::genstruct::SortedBinTree, @ref elm::genstruct::Tree),
 * @li queue management (@ref elm::genstruct::VectorQueue, @ref elm::genstruct::DLList),
 * @li stack management (@ref elm::genstruct::Vector),
 * @li set management (@ref elm::avl::Set),
 * @li map management (@ref elm::genstruct::HashTable, @ref elm::avl::Map, @ref elm::genstruct::AssocList, @ref elm::genstruct::SortedBinMap, @ref elm::stree::Tree).
 * 
 * All these classes share the same syntax for iterators:
 * @code
 * 	DataStructure<MyType> data_structure;
 * 	...
 * 	for(DayaStructure<MyType>::Iterator iter; iter; iter++)
 * 		process(*iter);
 * @endcode
 * 
 * A whole bunch of ordered sets comparators (@ref elm::Comparator) or hashing algorithms (@ref elm::HashKey)
 * are proposed by ELM but they can also be specialized for your own types.
 * 
 * To make easier the use of data structure and allows inter-operations, each one implements
 * a set of interfaces known in C++ as concepts (module @ref concepts).
 *
 * Finally, ELM provides convenient classes for data handling: @ref elm::Pair, @ref elm::Option.
 *
 * @par Input-Output System
 * 
 * The @ref ios module is split in two layers. The higher layer
 * (classes @ref elm::io::Output and @ref elm::io::Input) are responsible
 * for format-aware input / output (mostly equivalent to STL istream / ostream).
 * They provide formatter class (@ref elm::io::IntFormat and @ref elm::io::FloatFormat)
 * to fine-tune their work. The IO errors are processed by rising exceptions.
 * 
 * The lower layer handles input-output as a stream of bytes but the provided
 * classes allows to pipe efficiently these classes together, offering 
 * a significant potential of control on the streams. All stream classes
 * implement either the @ref elm::io::InStream or @ref elm::io::OutStream
 * and supports error handling through synchronized error return.
 * Both allows to get the OS-error message from getLastError() method.
 * 
 * Basically, input stream and output stream gives access to the OS streams
 * but other classes allows to buffer IO (@ref elm::io::BufferedInStream or @ref elm::io::BufferedOutStream),
 * to use block of data as streams (@ref elm::io::BlockInStream, @ref elm::io::BlockOutStream),
 * to compute checksums (@ref elm::checksum::Fletcher; @ref elm::checksum::MD5). In addition,
 * input and output stream can piped with the @ref elm::io::StreamPipe or diverted with @ref elm::io::TeeOutStream.
 * 
 *
 * @par Portability Features
 * 
 * ELM implements also several classes to abstract the host OS. The following
 * features have been tested on Linux, Windows and MacOSX:
 * @li access to generic features of the system (@ref elm::sys::System)
 * @li process management (@ref elm::sys::Process),
 * @li stop watch implementation (@ref elm::sys::StopWatch),
 * @li file items (@ref elm::sys::FileItem),
 * @li command line arguments processing (@ref options module),
 * @li portable plugin system (@ref plugins module).
 *
 * In the future, classes to handle threads and sockets will be added.
 *
 * @par Memory Management
 *
 * ELM supplies several classes to handle allocation and de-allocation:
 * @li default allocation scheme (@ref elm::DefaultAllocator),
 * @li allocate from a list of fixed size (@ref elm::BlockAllocator),
 * @li stack allocation with backtrack (@ref elm::StackAllocator),
 * @li semi-automatic specialized garbage collector (@ref elm::AbstractBlockAllocatorWithGC).
 *
 * And some cleanup classes or pointer management classes (@ref elm::AutoCleaner, @ref elm::AutoDestructor, @ref AutoPtr).
 *
 * @par Other Facilities
 * @li crash management (@ref elm::CrashHandler, only for Linux),
 * @li XML and XSLT using the XOM interface (@ref xom),
 * @li basic exceptions with message (@ref elm::Exception and @ref elm::MessageException),
 * @li very light test framework (@ref elm::TestCase and @ref elm::TestSet),
 * @li automatic serialization and unserialization (@ref serial),
 * @li work-around the C++ initialization fiasco (@ref elm::Initializer),
 * bit management (@ref elm::BitVector).
 */

/**
 * @defgroup serial Serialization / unserialization
 * 
 * ELM provides support for pseudo-automatic serialization / unserialization
 * of objects.
 * 
 * The following C++ features are supported in serialization:
 * @li scalar types (integer, floats)
 * @li atomic types (strings),
 * @li class type with inheritance,
 * @li pointer and reference types with circularity detection,
 * @li array types,
 * @li ELM collection types,
 * @li enumeration with user support for value names.
 * 
 * In addition, the user can provides its own custom serialization code in
 * a serializer / unserializer independent format.
 * 
 * @par A simple example
 * 
 * The serialization or the unserialization is a very simple to perform using
 * the same interface as usual C++ input/output.
 * 
 * @code
 * #include <elm/serial2/serial.h>
 * #include <elm/serial2/TextSerializer.h>
 * #include <elm/serial2/collections.h>
 * 
 * using namespace elm;
 * 
 * int main(void) {
 *	serial2::TextSerializer serializer;
 *	serializer << "Hello, World !\n" << 666 << true;
 * 
 * 	MyClass my_object;
 *	serializer << my_object;
 * 
 *	Vector<MyClass> objects;
 *	for(int i = 0; i < 10; i++)
 *		objects.add(my_object);
 *	serializer << objects; 
 * }
 * @endcode
 * 
 * In the example above, we show how serializable objects (scalar values, string,
 * MyClass and Vector) are easily serialized by a serializer providing a text
 * output. As a default, the output is performed to the standard output. Notice
 * that the unserialization of data from the bytes produced by this example must
 * be performed in the same order. 
 * 
 * @code
 * #include <elm/serial2/serial.h>
 * #include <elm/serial2/XOMUnserializer.h>
 * #include <elm/serial2/collections.h>
 * 
 * using namespace elm;
 * 
 * int main(void) {
 *  serial2::XOMUnserializer unser("unser.xml");
 *	string str;
 *	int x;
 *	bool boolean;
 *	serializer >> str >> x >> boolean;
 * 
 * 	MyClass my_object;
 *	serializer >> my_object;
 * 
 *	Vector<MyClass> objects;
 *	serializer >> objects; 
 * }
 * }
 * @endcode
 * 
 * The unserialization above looks much like the previous serialization
 * example. It works in the same way as the input stream of C++ standard library.
 * The read values must be put in variable. Notice that in the case of the Vector,
 * the unserializer automatically allocate enough space to store the serialized
 * collection of objects.
 * @p
 * This examples illustrates also the use of an XML unserializer taking its input
 * from the file "unser.xml". This file is usual XML text that may be modified
 * by hand and that contains very few serialization-systems items.
 *
 * ELM provides serializer / unserializer for usual types (like scalars, strings
 * or data collections). Yet, to serializer custom classes, the user has to
 * add some information about the fields to work with. In our example, we use
 * the following declaration for MyClass in MyClass.h:
 * 
 * @code
 * #include <elm/serial2/macros.h>
 * #include <elm/serial2/collections.h>
 * 
 * class MyClass {
 * 	SERIALIZABLE(MyClass, FIELD(name) & FIELD(value) & FIELD(attrs));
 *	string name;
 *	int value;
 *	Vector<string> attrs;
 * public:
 *	...
 * };
 * @endcode
 * 
 * The macro SERIALIZABLE is used to add to the class some RTTI information
 * that is used by the serialization system. After the name of the class,
 * this macro takes the list of fields to serialize separated by the '&' operator
 * and embedded in the FIELD macro. The field macro is not mandatory, it allows
 * only to provide better reading on textual output by providing the name of
 * the field. This macro is used both by serialization and unserialization. 
 * 
 * @par Making a class serializable
 * 
 * In ELM, making a class serializable is very easy. One has to add some
 * RTTI information in the class declaration and add a macro in the class
 * definition file.
 * 
 * The declaration ".h" file must includes serialization headers:
 * @code
 * #include <elm/serial2/macros.h>
 * #include <elm/serial2/collections.h>
 * @endcode
 * 
 * Then, we have to add the macro SERIALIZE in the class declaration including
 * the name of the class and the list of the field to serialize separated by
 * '&'.
 * @code
 * class ClassName {
 *	SERIALIZABLE(ClassName, field1 & field2 & field3 & ...);
 *	...
 * };
 * @endcode
 *
 * In the definition ".cpp" file, we have just to put the following macro that
 * provides the implementation of the RTTI information of the class:
 * @code
 *	SERIALIZE(ClassName)
 * @endcode 
 * The passed class name must be the same between the SERIALIZABLE and the SERIALIZE
 * macro and fully qualified to avoid ambiguities.
 * 
 * Declaring a field serializable is as easy as passing its name in the SERIALIZABLE
 * list. Actually, a reference to the field is taken and used to read or write
 * the serialized values. In human readable formats (like XML or text),
 * it may be useful to provide also the identifier of the field to the
 * serialization system. This is easily done using the FIELD macro in place
 * of the field name:
 * @code
 *	SERIALIZE(MyClass, FIELD(field1) & field2 & FIELD(field3) & ...);
 * @endcode
 * 
 * Some serialization formats supports optional field definition. In this case,
 * a default value may be provided with the DFIELD macro:
 * @code
 *	SERIALIZE(MyClass, DFIELD(field1, default_value) & field2 & FIELD(field3) & ...);
 * @endcode
 * 
 * Finally, if the serialized class inherit from a serializable class, the
 * base class must be added to the list of field with the BASE macro:
 * @code
 * class MyBaseClass {
 *	SERIALIZABLE(MyBaseClass, ...);
 *	...
 * };
 * 
 * class MyClass: public MyBaseClass {
 *	SERIALIZABLE(MyClass, BASE(MyBaseClass) & field1 & ...);
 *	...
 * };
 * @endcode
 * 
 * @par Collection serialization
 * 
 * @par Customizing the serialization
 * 
 * @par Available serializers / unserializers
 * 
 * @par Writing a serialiazer
 * 
 */

/**
 * @defgroup utilities Development Utilities
 *
 * The entities defined in this section are targeted to help development and testing instead
 * of representing usual classes embedded in your application.
 *
 * @section deprecated Deprecation Management
 *
 * ELM provides a small macro found in <elm/deprecated.h>, called DEPRECATED.
 *
 * When put at the start of a function body, it displays a warning deprecation
 * message the first time it is called.
 *
 * @code
 * #include <elm/deprecated.h>
 *
 * class MyClass {
 * public:
 *
 * 	void myDeprecatedFunction(void) {
 * 		DEPRECATED
 * 		// body of the function
 * 	}
 * @endcode
 *
 * @section helper	Helper Methods
 *
 * ELM provides several helper method for using the different classes. They are prefixed by a double underscore and
 * all only compiled if a matching define is not provided.
 *
 * @li @c __size(): gives the full size of the data structure (removed if @c NSIZE is defined)
 * @li @c __print(): display an internal representation of the class (removed if @c NDEBUG is defined)
 *
 * @section test	Testing
 *
 * This module provides a poor, but existing, solution to unit testing in C++.
 * It provides macros and classes to perform statistics on unit testing
 * and makes automatic some test procedures.
 *
 * The example show how the module works:
 * @code
 * #include <elm/util/test.h>
 *
 * void my_test(void) {
 * 		CHECK_BEGIN("my test")
 *
 * 			// test 1
 * 			CHECK(result1 == expected1);
 *
 * 			// test 2
 * 			CHECK(result2 == expected2);
 *
 * 			...
 *
 * 		CHECK_END
 * }
 * @endcode
 *
 * The test must be enclosed in @c CHECK_BEGIN / @c CHECK_END pair and the test case name must be given.
 * Then the test are performed ending with a call to a macro allowing to test and record result of the test.
 *
 * The existing macro are:
 * @li @c CHECK(x) -- check if x is true
 * @li @c CHECK_EQUAL(result, expected) -- test equality and, if it fails, display the failed result value
 * @li @c CHECK_EXCEPTION(exception, action) -- perform the action and the test fails if the exception is not thrown
 * @li @c FAIL_ON_EXCEPTION(exception, action) -- perform the action and the test fails if the exception is thrown
 *
 * There are also some useful macros as:
 * @li @c REQUIRE(cnd, action) -- perform the action only if the given condition is true, else stop the test.
 *
 */

#endif /*ELM_DOC_H_*/
