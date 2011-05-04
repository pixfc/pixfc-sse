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

#ifndef YUV_UNPACK_H_
#define YUV_UNPACK_H_

#include <emmintrin.h>
#include <tmmintrin.h>

#include "platform_util.h"

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
EXTERN_INLINE void unpack_yuyv_to_y_uv_vectors_sse2(__m128i* in_1_v8i_yuyv_vector, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(mask_off_chromas, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_and_si128(*in_1_v8i_yuyv_vector, _M(mask_off_chromas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PAND             2   2

    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_srli_epi16(*in_1_v8i_yuyv_vector,  8);
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSRLW            2   2
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
EXTERN_INLINE void unpack_yuyv_to_y_uv_vectors_sse2_ssse3(__m128i* in_1_v8i_yuyv_vector, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(shuffle_lumas,   0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	CONST_M128I(shuffle_chromas, 0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);
	
    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_shuffle_epi8 (*in_1_v8i_yuyv_vector, _M(shuffle_lumas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PSHUFB			1 1 3	0.5	1 2
	
    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_shuffle_epi8 (*in_1_v8i_yuyv_vector, _M(shuffle_chromas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSHUFB			1 1 3	0.5	1 2
};

/*
 * Convert 1 vector of 16 char YUYV to 3 vectors of 8 short Y, U & V
 *
 * Total latency:				16
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
EXTERN_INLINE void unpack_yuyv_to_y_u_v_vectors_sse2(__m128i* in_1_v8i_yuyv_vector, __m128i* out_3_v16i_y_u_v_vectors)
{
	CONST_M128I(mask_off_chromas, 0x5C4600785F692350LL, 0x780063402A520043LL);
	CONST_M128I(mask_cb, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	__m128i uv, uLo, uHi, vLo, vHi;

    // Y unpacking
    out_3_v16i_y_u_v_vectors[0] = _mm_and_si128(*in_1_v8i_yuyv_vector, _M(mask_off_chromas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0        // PAND             2   2

    //
    // U unpacking
    uv = _mm_srli_epi16(*in_1_v8i_yuyv_vector,  8);
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0       // PSRLW            2   2

    uLo =  _mm_and_si128(uv, _M(mask_off_chromas));
    // U12 0    0 0     U34 0   0 0     U56 0   0 0     U78 0   0 0         // PAND             2   2

    uHi = _mm_slli_epi32(uLo, 16);
    // 0 0      U12 0   0 0     U34 0   0 0     U56 0   0 0     U78 0       // PSLLD            2   2

    out_3_v16i_y_u_v_vectors[1] = _mm_or_si128(uLo, uHi);
    // U12 0    U12 0   U34 0   U34 0   U56 0   U56 0   U78 0   U78 0       // POR              2   2


    //
    // V unpacking
    vHi =  _mm_and_si128(uv, _M(mask_cb));
    // 0 0      V12 0   0 0     V34 0   0 0     V56 0   0 0     V78 0		// PAND             2   2

    vLo = _mm_srli_epi32(vHi, 16);								            // PSRLD            2   2
    // V12 0    0 0     V34 0   0 0     V56 0   0 0     V78 0   0 0

    out_3_v16i_y_u_v_vectors[2] = _mm_or_si128(vLo, vHi);
    // V12 0    V12 0   V34 0   V34 0   V56 0   V56 0   V78 0   V78 0       // POR              2   2
};


/*
 * Convert 1 vector of 16 char UYVY to 2 vectors of 8 short Y & UV
 *
 * Total latency:				4
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
EXTERN_INLINE void unpack_uyvy_to_y_uv_vectors_sse2(__m128i* in_1_v8i_yuyv_vector, __m128i* out_2_v16i_y_uv_vectors)
{
    CONST_M128I(mask_off_lumas, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);

    // Y unpacking
    out_2_v16i_y_uv_vectors[0] = _mm_srli_epi16(*in_1_v8i_yuyv_vector, 8);
    // Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0		// PSRLW            2   2

    //
    // U V unpacking
	out_2_v16i_y_uv_vectors[1] = _mm_and_si128(*in_1_v8i_yuyv_vector, _M(mask_off_lumas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PAND             2   2
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
EXTERN_INLINE void unpack_uyvy_to_y_uv_vectors_sse2_ssse3(__m128i* in_1_v8i_yuyv_vector, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(shuffle_chromas, 0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	CONST_M128I(shuffle_lumas,   0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);
	
    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_shuffle_epi8 (*in_1_v8i_yuyv_vector, _M(shuffle_lumas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PSHUFB			1 1 3	0.5	1 2
	
    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_shuffle_epi8 (*in_1_v8i_yuyv_vector, _M(shuffle_chromas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSHUFB			1 1 3	0.5	1 2
};


/*
 * Calculate missing chroma values for the last 8 pix in an image
 * by averaging previous and current chroma values. The last pixel reuses
 * the chroma values of the before-last pix.
 *
 *
 * TOTAL LATENCY:			9
 *
 * INPUT:
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * OUTPUT:
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 */
EXTERN_INLINE void	reconstruct_last_missing_uv_sse2(__m128i* current_uv, __m128i* out) {
	M128I(avgB, 0x0LL, 0x0LL);

	_M(avgB) = _mm_srli_si128(*current_uv, 4);							// PSRLDQ	4	2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	0 0		0 0

	_M(avgB) = _mm_shufflehi_epi16(_M(avgB), 0x44);						// PSHUFHW	1	1
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	U7 0	V7 0

	_M(avgB) = _mm_or_si128(_M(avgB), _M(avgB));						// POR		2	2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	U7 0	V7 0

	*out = _mm_avg_epu16(*current_uv, _M(avgB));						// PAVGW	2	2
}

/*
 * Calculate missing chroma values by averaging previous and current chroma values
 *
 * TOTAL LATENCY:			12
 *
 * INPUT:
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 * U9 0		V9 0	U11 0 	V11 0	U13 0	V13 0	U15 0	V15 0
 *
 * OUTPUT:
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 */
EXTERN_INLINE void	reconstruct_missing_uv_sse2(__m128i* current_uv, __m128i* next_uv, __m128i* out) {
	M128I(avgB, 0x0LL, 0x0LL);
	M128I(tmp, 0x0LL, 0x0LL);

	_M(avgB) = _mm_srli_si128(*current_uv, 4);							// PSRLDQ	4	2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	0 0		0 0

	_M(tmp) = _mm_slli_si128(*next_uv, 12);								// PSLLDQ	4	2
	// 0 0		0 0		0 0		0 0		0 0		0 0		U9 0	V9 0

	_M(avgB) = _mm_or_si128(_M(tmp), _M(avgB));							// POR		2	2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	U9 0	V9 0

	*out = _mm_avg_epu16(*current_uv, _M(avgB));						// PAVGW	2	2
}

/*
 * Calculate missing chroma values for the last 8 pix in an image
 * by averaging previous and current chroma values. The last pixel reuses
 * the chroma values of the before-last pix.
 *
 *
 * TOTAL LATENCY:			3
 *
 * INPUT:
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * OUTPUT:
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 */
EXTERN_INLINE void	reconstruct_last_missing_uv_sse2_ssse3(__m128i* current_uv, __m128i* out) {
	CONST_M128I(shuff1, 0xFF0AFF08FF06FF04LL, 0xFF0EFF0CFF0EFF0CLL);
	M128I(avgB, 0x0LL, 0x0LL);
	
	_M(avgB) = _mm_shuffle_epi8(*current_uv, _M(shuff1));				// PSHUFB	1 1	3	0.5	1 2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	U7 0	V7 0
		
	*out = _mm_avg_epu16(*current_uv, _M(avgB));						// PAVGW	2	2
}

/*
 * Calculate missing chroma values by averaging previous and current chroma values
 *
 * TOTAL LATENCY:			6
 *
 * INPUT:
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 * U9 0		V9 0	U11 0 	V11 0	U13 0	V13 0	U15 0	V15 0
 *
 * OUTPUT:
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 */
EXTERN_INLINE void	reconstruct_missing_uv_sse2_ssse3(__m128i* current_uv, __m128i* next_uv, __m128i* out) {
	CONST_M128I(shuff1, 0xFF0AFF08FF06FF04LL, 0xFFFFFFFFFF0EFF0CLL);
	CONST_M128I(shuff2, 0xFFFFFFFFFFFFFFFFLL, 0xFF02FF00FFFFFFFFLL);
	M128I(avgB, 0x0LL, 0x0LL);
	M128I(tmp, 0x0LL, 0x0LL);
	
	_M(avgB) = _mm_shuffle_epi8(*current_uv, _M(shuff1));				// PSHUFB	1 1	3	0.5	1 2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	0 0		0 0
	
	_M(tmp) = _mm_shuffle_epi8(*next_uv, _M(shuff2));					// PSHUFB	1 1	3	0.5	1 2
	// 0 0		0 0		0 0		0 0		0 0		0 0		U9 0	V9 0
	
	_M(avgB) = _mm_or_si128(_M(tmp), _M(avgB));							// POR		2	2
	// U3 0		V3 0	U5 0	V5 0	U7 0	V7 0	U9 0	V9 0
	
	*out = _mm_avg_epu16(*current_uv, _M(avgB));						// PAVGW	2	2
}

#endif /* YUV_UNPACK_H_ */
