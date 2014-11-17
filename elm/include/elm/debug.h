/*
 * $Id$
 * Copyright (c) 2004-07, IRIT - UPS
 *
 * Debugging macros
 */
#ifndef ELM_DEBUG_H
#define ELM_DEBUG_H

#include <elm/io.h>

namespace elm {

// Debugging macros
#ifdef NDEBUG
#	define TRACE
#	define BREAK(cond)
#	define HERE
#else
#	define TRACE elm::trace(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#	define BREAK(cond)	if(cond)
#	define HERE { int x = 0; };
#endif

// Inlines
inline void trace(CString file, int line, CString fun) {
	elm::cerr << "TRACE(" << file << ':' << line << ":" << fun << ")\n";
	elm::cerr.flush();
}
	
}	// elm
	
#endif	// ELM_DEBUG_H
