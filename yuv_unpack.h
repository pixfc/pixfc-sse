/*
 * yuv_unpack.h
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

#include <emmintrin.h>
#include <tmmintrin.h>

#include "common.h"
#include "platform_util.h"


#ifndef GENERATE_UNALIGNED_INLINES
#error "The GENERATE_UNALIGNED_INLINES macro is not defined"
#endif


/*
 * This header file expects the GENERATE_UNALIGNED_INLINES macro to be set to 1
 * to generate inlines for unaligned input buffers.
 * This header file does not have the usual #ifndef / #define / #endif barrier
 * preventing it from being included multiple times in a single source file.
 * This is done so this header file CAN be included multiple times, once with
 * GENERATE_UNALIGNED_INLINES set to 0 to generate inlines for aligned input buffers
 * and once with GENERATE_UNALIGNED_INLINES set to 1 to generate inlines for
 * unaligned input buffers
 */

#undef INLINE_NAME
#undef UNALIGNED_YUV422_INPUT_PREAMBLE
#undef INPUT_VECT

#if GENERATE_UNALIGNED_INLINES == 1
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void unaligned_ ## fn_suffix(__VA_ARGS__)
	#define UNALIGNED_YUV422_INPUT_PREAMBLE			DECLARE_VECT_N_UNALIGN_LOAD(aligned_vector, input);
	#define INPUT_VECT								(&aligned_vector)
#else
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void fn_suffix(__VA_ARGS__)
	#define UNALIGNED_YUV422_INPUT_PREAMBLE
	#define INPUT_VECT								input
#endif


/*
 * Convert 1 vector of 16 char YUYV to 2 vectors of 8 short Y & UV
 *
 * Total latency:				4
 * Number of pixels handled:	8
 *
 * INPUT
 * 1 vector of 16 char
 * Y1 U12	Y2 V12	Y3 U34	Y4 V34	Y5 U56	Y6 V56	Y7 U78	Y8 V78
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
INLINE_NAME(unpack_yuyv_to_y_uv_vectors_sse2, __m128i* input, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(mask_off_chromas, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	UNALIGNED_YUV422_INPUT_PREAMBLE;

    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_and_si128(*INPUT_VECT, _M(mask_off_chromas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PAND             1	0.33

    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_srli_epi16(*INPUT_VECT,  8);
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSRLW            1	1
};

/*
 * Convert 1 vector of 16 char YUYV to 2 vectors of 8 short Y & UV
 *
 * Total latency:				2
 * Number of pixels handled:	8
 *
 * INPUT
 * 1 vector of 16 char
 * Y1 U12	Y2 V12	Y3 U34	Y4 V34	Y5 U56	Y6 V56	Y7 U78	Y8 V78
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
INLINE_NAME(unpack_yuyv_to_y_uv_vectors_sse2_ssse3, __m128i* input, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(shuffle_lumas,   0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	CONST_M128I(shuffle_chromas, 0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);
	UNALIGNED_YUV422_INPUT_PREAMBLE;
	
    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_shuffle_epi8 (*INPUT_VECT, _M(shuffle_lumas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PSHUFB			1	0.5
	
    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_shuffle_epi8 (*INPUT_VECT, _M(shuffle_chromas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSHUFB			1	0.5
};

/*
 * Convert 1 vector of 16 char YUYV to 3 vectors of 8 short Y, U & V
 *
 * Total latency:				8
 * Number of pixels handled:	8
 * 
 * INPUT:
 *
 * 1 vector of 16 char
 * Y1 U12	Y2 V12	Y3 U34	Y4 V34	Y5 U56	Y6 V56	Y7 U78	Y8 V78
 *
 * OUTPUT:
 *
 * 3 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	U12 0	U34 0	U34 0	U56 0	U56 0	U78 0	U78 0
 *
 * vVect
 * V12 0	V12 0	V34 0	V34 0	V56 0	V56 0	V78 0	V78 0
 */
INLINE_NAME(unpack_yuyv_to_y_u_v_vectors_sse2, __m128i* input, __m128i* out_3_v16i_y_u_v_vectors)
{
	CONST_M128I(mask_off_chromas, 0x5C4600785F692350LL, 0x780063402A520043LL);
	CONST_M128I(mask_cb, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	UNALIGNED_YUV422_INPUT_PREAMBLE;
	__m128i uv, uLo, uHi, vLo, vHi;

    // Y unpacking
    out_3_v16i_y_u_v_vectors[0] = _mm_and_si128(*INPUT_VECT, _M(mask_off_chromas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0        // PAND             1	0.33

    //
    // U unpacking
    uv = _mm_srli_epi16(*INPUT_VECT,  8);
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0       // PSRLW            1	1

    uLo =  _mm_and_si128(uv, _M(mask_off_chromas));
    // U12 0    0 0     U34 0   0 0     U56 0   0 0     U78 0   0 0         // PAND             1	0.33

    uHi = _mm_slli_epi32(uLo, 16);
    // 0 0      U12 0   0 0     U34 0   0 0     U56 0   0 0     U78 0       // PSLLD            1	1

    out_3_v16i_y_u_v_vectors[1] = _mm_or_si128(uLo, uHi);
    // U12 0    U12 0   U34 0   U34 0   U56 0   U56 0   U78 0   U78 0       // POR              1	0.33


    //
    // V unpacking
    vHi =  _mm_and_si128(uv, _M(mask_cb));
    // 0 0      V12 0   0 0     V34 0   0 0     V56 0   0 0     V78 0		// PAND             1	0.33

    vLo = _mm_srli_epi32(vHi, 16);								            // PSRLD            1	1
    // V12 0    0 0     V34 0   0 0     V56 0   0 0     V78 0   0 0

    out_3_v16i_y_u_v_vectors[2] = _mm_or_si128(vLo, vHi);
    // V12 0    V12 0   V34 0   V34 0   V56 0   V56 0   V78 0   V78 0       // POR              1	0.33
};


/*
 * Convert 1 vector of 16 char UYVY to 2 vectors of 8 short Y & UV
 *
 * Total latency:				2
 * Number of pixels handled:	8
 *
 * INPUT
 * 1 vector of 16 char
 * U12 Y1	V12 Y2	U34	Y3	V34 Y4	U56 Y5	V56 Y6	U78 Y7	V78 Y8
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
INLINE_NAME(unpack_uyvy_to_y_uv_vectors_sse2, __m128i* input, __m128i* out_2_v16i_y_uv_vectors)
{
    CONST_M128I(mask_off_lumas, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
    UNALIGNED_YUV422_INPUT_PREAMBLE;

    // Y unpacking
    out_2_v16i_y_uv_vectors[0] = _mm_srli_epi16(*INPUT_VECT, 8);
    // Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0		// PSRLW		1	1

    //
    // U V unpacking
	out_2_v16i_y_uv_vectors[1] = _mm_and_si128(*INPUT_VECT, _M(mask_off_lumas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PAND			1	0.33
};

/*
 * Convert 1 vector of 16 char UYVY to 2 vectors of 8 short Y & UV
 *
 * Total latency:				2
 * Number of pixels handled:	8
 *
 * INPUT
 * 1 vector of 16 char
 * U12 Y1	V12 Y2	U34	Y3	V34 Y4	U56 Y5	V56 Y6	U78 Y7	V78 Y8
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
INLINE_NAME(unpack_uyvy_to_y_uv_vectors_sse2_ssse3, __m128i* input, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(shuffle_chromas, 0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	CONST_M128I(shuffle_lumas,   0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);
    UNALIGNED_YUV422_INPUT_PREAMBLE;
	
    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_shuffle_epi8 (*INPUT_VECT, _M(shuffle_lumas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PSHUFB	1	0.5
	
    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_shuffle_epi8 (*INPUT_VECT, _M(shuffle_chromas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSHUFB	1	0.5
};


