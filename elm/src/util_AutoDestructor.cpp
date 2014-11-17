/*
 *	$Id$
 *	delegate classes implementation
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

#include <elm/util/AutoDestructor.h>

namespace elm {

/**
 * @class AutoDestructor
 * This class is wrapper around allocated pointer to manager its deletion.
 * This type supports all operation on pointer and ensures that the pointed
 * memory will be fried when the the pointer will be deleted. Notice that it
 * takes as much memory as a usual pointer.
 * 
 * It may be useful in a generic data structure where the user has no access
 * to the destructor.
 * 
 * @param T	Type of pointed data. 
 */


/**
 * @fn AutoDestructor::AutoDestructor(void);
 * Build an autodestructor with a null pointer.
 */


/**
 * @fn AutoDestructor::AutoDestructor(T *ptr);
 * Build an autodestructor with the given pointer.
 * @param ptr	Used pointer.
 */


/**
 * @fn AutoDestructor::AutoDestructor(AutoDestructor& ad);
 * Build the auto-destructor using the pointer from the given auto-destructor.
 * To avoid double memory release, the original ad auto-destructor pointer is
 * set to null. 
 * @param ad	Auto-destructor to take the pointer from.
 */


/**
 * @fn bool AutoDestructor::isNull(void) const;
 * Test if the pointer is null.
 * @return	True if the pointer is null, false else.
 */


/**
 * @fn void AutoDestructor::clean(void);
 * Free the pointed memory and set the pointer to null.
 */


/**
 * @fn T *AutoDestructor::detach(void);
 * Return the pointer and set it in the object to null (it will no more be
 * automatically fried).
 * @return	Stored pointer.
 */


/**
 * @fn AutoDestructor& AutoDestructor::operator=(T *ptr);
 * Change the pointer contained in the auto-destructor, the old pointer begin
 * fried.
 * Same as set().
 * @return	Pointer to set.
 */


/**
 * @fn AutoDestructor& AutoDestructor::operator=(AutoDestructor& ad);
 * Change the current pointer with the one of the given auto-destructor. The old
 * pointer is first fried and the ad pointer is set to null.
 * @param ad	Auto-destructor to set the pointer form.
 */


/**
 * @fn AutoDestructor::operator T *(void) const;
 * Same as get().
 */


/**
 * @fn T *AutoDestructor::operator->(void) const;
 * Indirect access to the members of stored pointer.
 * @return	Stored pointer.
 */


/**
 * @fn AutoDestructor::operator bool(void) const;
 * Same as !isNull().
 */

};

