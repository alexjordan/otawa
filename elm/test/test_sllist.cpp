/*
 * $Id$
 * Copyright (c) 2005, IRIT UPS.
 *
 * test/test_SLList/test_SLList.cpp -- test program for SLList class.
 */

#include <elm/util/test.h>
#include <elm/genstruct/SLList.h>
#include <elm/genstruct/Vector.h>

using namespace elm;
using namespace genstruct;

TEST_BEGIN(sllist)

	// Test initial
	{
		SLList<int> list;
		CHECK(list.count() == 0);
	}
	
	// Test addFirst
	{
		SLList<int> list;
		for(int i = 0; i < 10; i++)
			list.addFirst(i);
		CHECK(list.count() == 10);
		int i = 9;
		bool good = true;
		for(SLList<int>::Iterator iter(list); iter; iter++, i--)
			if(iter.item() != i) {
				good = false;
				break;
			}
		CHECK(good);
	}
	

	// List copy test
	{
		SLList<int> list1, list2;
		list1.add(1);
		list1.add(2);
		list1.add(3);
		CHECK(list2.isEmpty());
		list2 = list1;
		CHECK(!list2.isEmpty());
		CHECK(list2.contains(1));
		CHECK(list2.contains(2));
		CHECK(list2.contains(3));
		CHECK(!list1.isEmpty());
		CHECK(list1.contains(1));
		CHECK(list1.contains(2));
		CHECK(list1.contains(3));
		list2.remove(2);
		CHECK(!list2.contains(2));
		CHECK(list1.contains(2));
		list2.remove(3);
		CHECK(!list2.contains(3));
		CHECK(list1.contains(3));
		list2.remove(1);
		CHECK(!list2.contains(1));
		CHECK(list1.contains(1));
	}

	// test addAll, removeAll
	{
		genstruct::Vector<int> v;
		v.add(1);
		v.add(2);
		v.add(3);
		genstruct::SLList<int> l;
		l.addAll(v);
		CHECK(l.contains(1));
		CHECK(l.contains(2));
		CHECK(l.contains(3));
	}

	// test remove with iterator
	{
		genstruct::SLList<int> l;
		for(int i = 0; i < 10; i++)
			l.add(i);
		for(genstruct::SLList<int>::Iterator i(l); i; i && i++)
			if(*i % 2 == 0)
				l.remove(i);
		int pairs = 0;
		for(genstruct::SLList<int>::Iterator i(l); i; i++)
			if(*i % 2 == 0)
				pairs++;
		CHECK_EQUAL(pairs, 0);
	}

	// test for null
	{
		CHECK(genstruct::SLList<int>::null.isEmpty());
	}
TEST_END

