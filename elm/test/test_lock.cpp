/*
 * $Id$
 * Copyright (c) 2005, IRIT-UPS.
 *
 * test/test_lock.cpp -- unit tests for elm::Lock class.
 */

#include <elm/util/test.h>
#include <elm/utility.h>
#include <elm/util/AutoPtr.h>

using namespace elm;

// Utility class
class LockTest: public Lock {
	bool *alloc;
public:
	LockTest(bool *_alloc): alloc(_alloc) { *alloc = true; };
	~LockTest(void) { *alloc = false; };
	inline bool getAlloc(void) const { return *alloc; };
};


// test_lock()
TEST_BEGIN(lock)
	
	// Simple test
	{
		bool alloc = false;
		{
			AutoPtr<LockTest> p = new LockTest(&alloc);
			CHECK(alloc);
		}
		CHECK(!alloc);
	}
	
	// Null assignment test
	{
		bool alloc = false;
		{
			AutoPtr<LockTest> p;
			CHECK(!alloc);
			p = new LockTest(&alloc);
			CHECK(alloc);
		}
		CHECK(!alloc);
	}
	
	// Passing test
	{
		bool alloc = false;
		{
			AutoPtr<LockTest> p1;
			{
				AutoPtr<LockTest> p2 = new LockTest(&alloc);
				CHECK(alloc);
				p1 = p2;
				CHECK(alloc);
			}
			CHECK(alloc);
		}
		CHECK(!alloc);
	}
	
	// Passing null
	{
		bool alloc = false;
		AutoPtr<LockTest> p = new LockTest(&alloc);
		CHECK(alloc);
		p = 0;
		CHECK(!alloc);
	}
	
	// Transitivity
	{
		bool alloc = false;
		AutoPtr<LockTest> p1 = new LockTest(&alloc);
		CHECK(alloc);
		AutoPtr<LockTest> p2 = p1;
		CHECK(alloc);
		AutoPtr<LockTest> p3 = p2;
		CHECK(alloc);
		p1 = 0;
		CHECK(alloc);
		p3 = 0;
		CHECK(alloc);
		p2 = 0;
		CHECK(!alloc);
	}
	
	// Usage test
	{
		bool alloc = false;
		AutoPtr<LockTest> p = new LockTest(&alloc);
		CHECK(p->getAlloc() == alloc);
	}
	
TEST_END



