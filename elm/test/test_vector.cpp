/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_vector.cpp -- unit tests for elm::xxx::Vector classes.
 */

#include <elm/util/test.h>
#include <elm/genstruct/Vector.h>

using namespace elm;

// test_vector()
TEST_BEGIN(vector)
	
	// Simple test
	{
		genstruct::Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(2);
		v.add(3);
		CHECK(v.count() == 4);
		CHECK(v.item(0) == 0);
		CHECK(v.item(1) == 1);
		CHECK(v.item(2) == 2);
		CHECK(v.item(3) == 3);
	}
	
	// Grow test
	{
		genstruct::Vector<int> v;
		for(int i = 0; i < 32; i++)
			v.add(i);
		bool ok = true;
		for(int i = 0; i < 32; i++)
			if(v.item(i) != i)
				ok = false;
		CHECK(ok);
	}
	
	// Insertion
	{
		genstruct::Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(3);
		v.insert(2, 2);
		CHECK(v.item(0) == 0);
		CHECK(v.item(1) == 1);
		CHECK(v.item(2) == 2);
		CHECK(v.item(3) == 3);
		v.insert(4, 4);
		CHECK(v.item(3) == 3);
		CHECK(v.item(4) == 4);
	}
	
	// Contains and index
	{
		genstruct::Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(0);
		v.add(1);
		v.add(0);
		v.add(1);
		CHECK(v.contains(1) == true);
		CHECK(v.contains(2) == false);
		CHECK(v.indexOf(1) == 1);
		CHECK(v.indexOf(2) < 0);
		CHECK(v.indexOf(1, 2) == 3);
		CHECK(v.indexOf(1, 6) < 0);
		CHECK(v.lastIndexOf(1) == 5);
		CHECK(v.lastIndexOf(2) < 0);
		CHECK(v.lastIndexOf(1, 4) == 3);
		CHECK(v.lastIndexOf(1, 1) < 0);
	}
	
	// Removal test
	{
		genstruct::Vector<int> v;
		v.add(0);
		v.add(1);
		v.add(2);
		v.add(3);
		v.add(4);
		v.removeAt(4);
		CHECK(v.count() == 4);
		CHECK(v.item(3) == 3);
		v.removeAt(1);
		CHECK(v.count() == 3);
		CHECK(v.item(0) == 0);
		CHECK(v.item(1) == 2);
		CHECK(v.item(2) == 3);
		v.remove(3);
		CHECK(v.count() == 2);
		CHECK(v.item(0) == 0);
		CHECK(v.item(1) == 2);
		v.clear();
		CHECK(v.count() == 0);
	}
	
	// Stack behaviour of the genstruct::Vector
	{
		genstruct::Vector<int> v;
		v.push(0);
		CHECK(v.top() == 0);
		v.push(1);
		CHECK(v.top() == 1);
		v.push(2);
		CHECK(v.top() == 2);
		CHECK(v.pop() == 2);
		CHECK(v.pop() == 1);
		v.push(3);
		CHECK(v.top() == 3);
		CHECK(v.pop() == 3);
		CHECK(v.pop() == 0);
		CHECK(v.length() == 0);
	}
	
	// addAll test
	{
		genstruct::Vector<int> v1, v2;
		for(int i = 0; i < 10; i++)
			v1.add(i);
		v2.addAll(v1);
		for(int i = 0; i < 10; i++)
			CHECK(v2[i] == i);
	}
	
TEST_END


