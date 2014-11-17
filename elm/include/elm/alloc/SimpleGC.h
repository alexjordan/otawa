/*
 *	SimpleGC class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2014, IRIT UPS.
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
#ifndef ELM_ALLOC_SIMPLEGC_H_
#define ELM_ALLOC_SIMPLEGC_H_

#include <elm/alloc/StackAllocator.h>
#include <elm/util/BitVector.h>
#include <elm/stree/Tree.h>
#include <elm/genstruct/SLList.h>

namespace elm {

class SimpleGC: private StackAllocator {
public:
	SimpleGC(t::size size = 4096);
	virtual ~SimpleGC(void);
	void clear(void);
	void doGC(void);

	void *allocate(t::size size) throw(BadAlloc);
	inline void free(void *block) { }

protected:
	bool mark(void *data, t::size size);

	virtual void beginGC(void);
	virtual void collect(void) = 0;
	virtual void endGC(void);

	virtual void *chunkFilled(t::size size) throw(BadAlloc);

private:
	void *allocFromFreeList(t::size size);

	typedef struct block_t {
		block_t *next;
		t::intptr size;
	} block_t;
	block_t *free_list;
	typedef struct gc_chunk_t {
		inline gc_chunk_t(StackAllocator::chunk_t *c, t::size s): chunk(c), bits(s / sizeof(block_t)) { }
		StackAllocator::chunk_t *chunk;
		BitVector bits;
	} gc_chunk_t;
	typedef stree::Tree<void *, gc_chunk_t *> tree_t;
	tree_t *st;
	genstruct::SLList<gc_chunk_t *> chunks;
	static inline t::size round(t::size size) { return (size + sizeof(block_t) - 1) & ~(sizeof(block_t) - 1); }
};

}	// elm

#endif /* ELM_ALLOC_SIMPLEGC_H_ */
