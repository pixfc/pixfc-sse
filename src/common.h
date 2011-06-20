/*
 * common.h
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

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdint.h>
#include <emmintrin.h>

#include "platform_util.h"
#include "debug_support.h"

// Clamp a pixel component's value to 0-255
#define CLIP_PIXEL(x) 		(((x)>255) ? 255 : ((x)<0) ? 0 : (x))

// Run the given conversion macro with the appropriate packing & unpacking
// inlines (aligned or unaligned) based on whether the source and destination
// buffers' alignment

#ifdef WIN32
// Visual Studio's handling of variadic macros is buggy at best.
// We need the following in order to have __VA_ARGS__ handled properly.
#define LeftParenthesis (
#define RightParenthesis )
#define DO_CONVERSION(conversion_macro, unpack_fn, pack_fn, ...)\
	if (((uintptr_t)source_buffer & 0x0F) == 0) {\
		if (((uintptr_t)dest_buffer & 0x0F) == 0){\
			conversion_macro LeftParenthesis unpack_fn, pack_fn, __VA_ARGS__ RightParenthesis\
		} else {\
			conversion_macro LeftParenthesis unpack_fn, unaligned_##pack_fn, __VA_ARGS__ RightParenthesis\
		}\
	} else {\
		if (((uintptr_t)dest_buffer & 0x0F) == 0){\
			conversion_macro LeftParenthesis unaligned_##unpack_fn, pack_fn, __VA_ARGS__ RightParenthesis\
		} else {\
			conversion_macro LeftParenthesis unaligned_##unpack_fn, unaligned_##pack_fn, __VA_ARGS__ RightParenthesis\
		}\
	}
#else
#define DO_CONVERSION(conversion_macro, unpack_fn, pack_fn, ...)\
	if (((uintptr_t)source_buffer & 0x0F) == 0) {\
		if (((uintptr_t)dest_buffer & 0x0F) == 0){\
			conversion_macro(unpack_fn, pack_fn, __VA_ARGS__)\
		} else {\
			conversion_macro(unpack_fn, unaligned_##pack_fn, __VA_ARGS__)\
		}\
	} else {\
		if (((uintptr_t)dest_buffer & 0x0F) == 0){\
			conversion_macro(unaligned_##unpack_fn, pack_fn, __VA_ARGS__)\
		} else {\
			conversion_macro(unaligned_##unpack_fn, unaligned_##pack_fn, __VA_ARGS__)\
		}\
	}

#endif

// Declare a __m128i variable, and load one unaligned __m128i vector from the unaligned buffer
#define	DECLARE_VECT_N_UNALIGN_LOAD(var, unaligned_buffer_ptr)\
			__m128i (var) = _mm_loadu_si128(unaligned_buffer_ptr);

// Declare an array of 2 __m128i elements, and load 2 unaligned __m128i vectors from the unaligned buffer
#define	DECLARE_VECT_ARRAY2_N_UNALIGN_LOAD(var, unaligned_buffer_ptr)\
		__m128i (var)[2]; (var)[0] = _mm_loadu_si128(unaligned_buffer_ptr); (var)[1] = _mm_loadu_si128(&unaligned_buffer_ptr[1]);

// Same as above with 3 elements
#define	DECLARE_VECT_ARRAY3_N_UNALIGN_LOAD(var, unaligned_buffer_ptr)\
		__m128i (var)[3]; (var)[0] = _mm_loadu_si128(unaligned_buffer_ptr); (var)[1] = _mm_loadu_si128(&unaligned_buffer_ptr[1]); (var)[2] = _mm_loadu_si128(&unaligned_buffer_ptr[2]);

/*
 * This function returns the features supported by the cpu
 * as returned by the CPUID instructions. (ECX is in the higher
 * 32 bits, EDX in the lowest ones). Use the following macros to
 * determine which features are supported.
 */
INLINE uint64_t		get_cpu_features();
#define CPUID_FEATURE_SSE2	0x0000000004000000LL
#define CPUID_FEATURE_SSE3	0x0000000100000000LL
#define CPUID_FEATURE_SSSE3	0x0000020000000000LL
#define CPUID_FEATURE_NONE	0xFFFFFFFFFFFFFFFFLL	// matches all CPUs

/*
 * Check if the CPU we are running on has the given features (mask
 * of above-defined CPUID_FEATURE_* macros).
 * Return 0 if yes, -1 if not.
 */
uint32_t			does_cpu_support(uint64_t);


/*
 * Various debugging functions
 */
#ifdef DEBUG

void 						print_xmm32(char *name, __m128i *reg);
void 						print_xmm16(char *name, __m128i *reg);
void 						print_xmm16u(char *name, __m128i *reg);
void						print_xmm8u(char *name, __m128i *reg);

#else

#define print_xmm32(...)
#define print_xmm16(...)
#define print_xmm16u(...)
#define print_xmm8u(...)

#endif	// DEBUG

#endif /* COMMON_H_ */
