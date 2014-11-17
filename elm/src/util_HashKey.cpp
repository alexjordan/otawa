/*
 *	$Id$
 *	HashKey class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2006-07, IRIT UPS.
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

#include <string.h>
#include <elm/util/HashKey.h>

namespace elm {

/**
 * @class HashKey
 * This is the default implementation of the Hash concept in ELM. It provides
 * a generic Jenkins hash function but provides also easier or more adapted
 * hash functions for curren types like integers, void pointer or strings.
 * 
 * Refer to @ref hash concept for more details.
 * 
 * @ingroup utility
 */ 


/**
 * Perform hashing according Jenkins approach
 * (http://www.burtleburtle.net/bob/hash/doobs.html).
 */
t::hash hash_jenkins(const void *block, int size) {
	t::hash hash = 0;
	const unsigned char *p = (const unsigned char *)block;
	for(int i = 0; i < size; i++) {
		hash += p[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}


/**
 * Use a classical compiler string hashing algorithm (see "The Compilers"
 * by Aho, Sethi, Ullman).
 */
t::hash hash_string(const char *chars, int length) {
	t::hash h = 0, g;
	for(int i = 0; i < length; i++) {
		h = (h << 4) + chars[i];
		if((g = h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}


/**
 * Use a classical compiler string hashing algorithm (see "The Compilers"
 * by Aho, Sethi, Ullman).
 */
t::hash hash_cstring(const char *chars) {
	t::hash h = 0, g;
	for(; *chars; chars++) {
		h = (h << 4) + *chars;
		if((g = h & 0xf0000000)) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}


/**
 * Test equality of two memory blocks.
 * @param p1	First memory block.
 * @param p2	Second memory block.
 * @param size	Block size.
 * @return		True if they equals, byte to byte, false else.
 */
bool hash_equals(const void *p1, const void *p2, int size) {
	return !memcmp(p1, p2, size);
}

};	// elm
