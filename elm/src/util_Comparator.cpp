/*
 *	$Id$
 *	Comparator class implementation
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2008, IRIT UPS.
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

#include <elm/util/Comparator.h>

namespace elm {

/**
 * @class Comparator
 * Default implementation of the concept @ref elm::concept::Comparator.
 * @param T	Type of compared values.
 */


/**
 * @fn int Comparator::compare(const T& v1, const T& v2);
 * Compare two values.
 * @param v1	First value.
 * @param v2	Second value.
 * @return		0 if both values are equal, >0 if v1 > v2, -1 else.
 */


/**
 * @fn const T& Comparator::min(const T& v1, const T& v2);
 * Get the minimum of two values.
 * @param v1	First value.
 * @param v2	Second value.
 * @return		Minimum of v1 and v2.
 */


/**
 * @fn const T& Comparator::max(const T& v1, const T& v2);
 * Get the maximum of two values.
 * @param v1	First value.
 * @param v2	Second value.
 * @return		Maximum of v1 and v2.
 */


/**
 * @class SubstractComparator
 * Implementation of the @ref elm::concept::Comparator for values supporting
 * a substraction operator returning an integer.
 * @param T	Type of the values.
 */


/**
 * @class CompareComparator
 * Implementation of the @ref elm::concept::Comparator for values providing
 * a compare() method.
 * @param T	Type of the values.
 */


/**
 * @class AssocComparator
 * An associative comparator may be used to transform an ordered data structure
 * into a map. If the items of ordered data structure are represented as pair
 * of (key, values), this comparator provides comparison of items based on
 * the key member.
 * 
 * This class is also the default comparator of pairs.
 * @param K		Key type.
 * @param T		Value type.
 * @param C		Comparator for the key.
 */

}
