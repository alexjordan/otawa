/*
 *	$Id$
 *	HashKey test file
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2010, IRIT UPS.
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

#include <elm/util/test.h>
#include <elm/util/HashKey.h>

using namespace elm;

TEST_BEGIN(hashkey)

	// generic hash
	{
		typedef int t[3];
		t t1 = { 0, 1, 2 };
		t t2 = { 0, 1, 2 };
		t t3 = { 2, 1, 0 };
		typedef HashKey<t> ht;
		CHECK_EQUAL(ht::hash(t1), ht::hash(t1));
		CHECK_EQUAL(ht::hash(t1), ht::hash(t2));
		CHECK(ht::equals(t1, t1));
		CHECK(ht::equals(t1, t2));
		CHECK(!ht::equals(t1, t3));
	}

TEST_END

