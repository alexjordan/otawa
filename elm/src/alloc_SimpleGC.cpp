/*
 *	SimpleGC class implementation
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

#include <elm/alloc/SimpleGC.h>
#include <elm/stree/SegmentBuilder.h>

namespace elm {

/**
 * @class SimpleGC
 * Basically, this allocator is a stack allocator
 * that support monitored reference collection for garbage collection.
 * This means that the user is responsible to provide the live references
 * at garbage collection time. This is done by overloading the @ref collect()
 * method and calling @ref mark() on each live block.
 */

/**
 * Initialize the allocator.
 * @param size	Size of chunks.
 */
SimpleGC::SimpleGC(t::size size)
: StackAllocator(round(size)), free_list(0), st(0) {
	newChunk();
}


/**
 */
SimpleGC::~SimpleGC(void) {
}


/**
 * Reset the allocator.
 */
void SimpleGC::clear(void) {
	StackAllocator::clear();
}


/**
 */
void SimpleGC::doGC(void) {
	beginGC();
	collect();
	endGC();
}


/**
 * Allocate memory from free block list.
 * @param size	Size of block to allocate.
 * @return		Allocated block or null if no block available.
 */
void *SimpleGC::allocFromFreeList(t::size size) {
	for(block_t *block = free_list, **prev = &free_list; block; prev = &block->next, block = block->next)
		if(block->size >= size) {
			void *res = (t::uint8 *)block + block->size - size;
			if(res == block)
				*prev = block->next;
			return res;
		}
	return 0;
}


/**
 * Perform an allocation of the given size.
 * @param size	Size of allocated memory.
 */
void *SimpleGC::allocate(t::size size) throw(BadAlloc) {

	// round the size
	size = round(size);

	// look for a free block
	void *res = allocFromFreeList(size);
	if(res)
		return res;

	// else allocate from stack
	return StackAllocator::allocate(size);
}


/**
 */
void *SimpleGC::chunkFilled(t::size size) throw(BadAlloc) {

	// collect garbage and try to allocate in free blocks
	doGC();
	void *res = allocFromFreeList(size);
	if(res)
		return res;

	// allocate a new chunk
	newChunk();
	return StackAllocator::allocate(size);
}


/**
 * Called to mark a block as alive.
 * @param data	Alive data block base.
 * @param size	Size of block.
 * @return		True if the block has already been marked, false else.
 * @warning		This function must only be called from the @ref collect() context!
 */
bool SimpleGC::mark(void *data, t::size size) {

	// find the chunk
	gc_chunk_t *gcc = st->get(data);
	ASSERTP(gcc, _ << "during GC, block out of chunks: " << (void *)data << ":" << io::hex(size) << "!");
	int p = (static_cast<char *>(data) - gcc->chunk->buffer) / sizeof(block_t);
	int s = size / sizeof(block_t);

	// mark it and make result
	bool res = true;
	int i;
	for(i = p; i < p + s; i++)
		if(!gcc->bits[i]) {
			res = false;
			gcc->bits.set(i);
		}
	return res;
}


/**
 * Called before a GC starts. Overriding methods must call this one.
 */
void SimpleGC::beginGC(void) {

	// build the data structure
	stree::SegmentBuilder<void *, gc_chunk_t *> builder(0);
	for(ChunkIter c(*this); c; c++) {
		gc_chunk_t *gcc = new gc_chunk_t(*c, chunkSize());
		chunks.add(gcc);
		builder.add(c->buffer, c->buffer + chunkSize(), gcc);
	}

	// finalize the tree
	st = new tree_t();
	builder.make(*st);
}


/**
 * @fn void SimpleGC::collect(void);
 * This function is called when references for a garbage collection are needed.
 * It must call the @ref mark() with each alive reference.
 *
 * This function must be override in each use site in order to provide alive references.
 */


/**
 */
void SimpleGC::endGC(void) {

	// build the list of free blocks
	for(genstruct::SLList<gc_chunk_t *>::Iterator gcc(chunks); gcc; gcc++) {

		// traverse the bits
		int i = 0, b = -1, cs = chunkSize() / sizeof(block_t);
		while(i < cs) {

			// skip ones
			while(i < cs && gcc->bits[i])
				i++;

			// if zero found
			if(i < cs) {
				b = i;
				i++;

				// skip zeroes
				while(i < cs && gcc->bits[i])
					i++;

				// create the free block
				// if b == 0, we could remove the block (if facility was available)
				block_t *blk = static_cast<block_t *>(static_cast<void *>(gcc->chunk->buffer + i * sizeof(block_t)));
				blk->size = (i - b) * sizeof(block_t);
				blk->next = free_list;
				free_list = blk;
			}
		}
	}

	// free the GC resources
	for(genstruct::SLList<gc_chunk_t *>::Iterator c(chunks); c; c++)
		delete *c;
	chunks.clear();
	delete st;
}

}	// elm
