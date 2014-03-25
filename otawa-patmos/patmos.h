/*
 *	sparc loader header file
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2011, IRIT UPS.
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
#ifndef OTAWA_SPARC_H
#define OTAWA_SPARC_H

#include <elm/types.h>

namespace otawa { namespace sparc {

using namespace elm;

// SPEC semantic instructions	
const int SPEC_SAVE = 1, SPEC_RESTORE = 2;

// specific flags
t::int32	IS_READ		= 0x80000000,
			IS_WRITE	= 0x40000000,
			IS_ASI		= 0x20000000,	// alternate space accesses
			IS_ATOM		= 0x10000000,	// atomical operations
			IS_NOP		= 0x08000000,	// no operation (NOP)
			IS_LOGIC	= 0x04000000,	// bit manipulation (no shift)
			IS_CC_UPD	= 0x02000000,	// condition code update
			IS_TAGGED	= 0x01000000,	// working with tagged integer
			IS_ANNUL	= 0x00400000;	// bit annul set in delayed branches

// FPU specific flags
t::int32	IS_FSQRT	= 0x00200000,
			IS_DOUBLE	= 0x00100000;

// special instruction  flags
t::int32	IS_SAVE		= 0x00200000,
			IS_RESTORE	= 0x00100000;
	
} } // otawa::sparc

#endif // OTAWA_SPARC_H
