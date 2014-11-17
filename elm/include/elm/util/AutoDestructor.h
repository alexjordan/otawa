/*
 *	$Id$
 *	delegate classes interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2007, IRIT UPS.
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
#ifndef ELM_UTIL_AUTODESTRUCTOR_H
#define ELM_UTIL_AUTODESTRUCTOR_H

namespace elm {

// AutoDestructor class
template <class T>
class AutoDestructor {
public:
	inline AutoDestructor(void): p(0) { }
	inline AutoDestructor(T *ptr): p(ptr) { };
	inline ~AutoDestructor(void) { clean(); }

	inline bool isNull(void) const { return !p; }
	inline void clean(void) { if(p) delete p; p = 0; }
	inline T *detach(void) { T *res = p; p = 0; return res; }
	inline void set(T *ptr) { clean(); p = ptr; }
	inline T *get(void) const { return p; }

	inline AutoDestructor& operator=(T *ptr) { set(ptr); return *this; } 
	inline AutoDestructor& operator=(AutoDestructor& ad)
		{ clean(); p = ad.p; ad.p = 0; return *this; }
	inline operator T *(void) const { return get(); }
	inline T *operator->(void) const { return get(); }
	inline operator bool(void) const { return !isNull(); }

private:
	T *p;
};

};

#endif // ELM_UTIL_AUTODESTRUCTOR_H

