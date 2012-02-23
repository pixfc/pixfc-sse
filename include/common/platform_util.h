/*
 * platform_util.h
 *
 * Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public  License as published by the
 * Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PLATFORM_UTIL_H
#define PLATFORM_UTIL_H

#include <emmintrin.h>

#if defined(__linux__) || defined(__APPLE__)

/*
 * We need the following because Visual Sudio cant handle assignment to a __m128i
 * variable. So instead, we have to declare the variable as a 16-byte aligned
 * array of __int64 and we have to cast it to __m128i when used. This is SHIT !!!!!
 * To add to the confusion, VS also requires all aligned variables to be declared at 
 * the top of a function.
 * http://permalink.gmane.org/gmane.comp.lib.cairo/14414
 */
#define CONST_M128I_ARRAY(var, num_elem)		const __m128i var[(num_elem)]
#define CONST_M128I(var, val64_1, val64_2)		static const __m128i var = { val64_1, val64_2 }
#define M128I(var, val64_1, val64_2)			__m128i var = { val64_1, val64_2 }
#define	_M(var)									(var)
#define _PRAGMA_(...)   						_Pragma( #__VA_ARGS__ )

#else

/*
 * Here starts the Microsoft __m128i awesomeness
 */

#define CONST_M128I_ARRAY(var, num_elem)		__declspec(align(16)) const __int64 var[2][(num_elem)]
#define CONST_M128I(var, val64_1, val64_2)		__declspec(align(16)) static const __int64 var[] = { (val64_1), (val64_2) }
#define M128I(var, val64_1, val64_2)			__declspec(align(16)) __int64 var[] = { (val64_1), (val64_2)}
#define	_M(var)									(*((__m128i *)(var)))
#define _PRAGMA_(...)   						__pragma( #__VA_ARGS__ )

#endif

#endif	// #ifndef PLATFORM_UTIL_H
