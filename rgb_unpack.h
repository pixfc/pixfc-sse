/*
 * rgb_unpack.h
 *
 * Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public  License as published by the
 * Free Software Foundation, either version 3 of the License, or
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

#ifndef RGB_UNPACK_H_
#define RGB_UNPACK_H_


#include <emmintrin.h>
#include <tmmintrin.h>

#include "platform_util.h"


/*
 * Convert 2 vectors of 16 char ARGB to 3 vectors of 8 short:
 *
 * Total latency:				28
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * A1  R1	G1  B1	A2  R2	G2  B2	A3  R3	G3  B3	A4  R4	G4  B4
 *
 * A5  R5	G5  B5	A6  R6	G6  B6	A7  R7	G7  B7	A8  R8	G8  B8
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gb1Vect
 * G1 0		B1 0	G2 0	B2 0	G3 0	B3 0	G4 0	B4 0
 *
 * gb2Vect
 * G5 0		B5 0	G6 0	B6 0	G7 0	B7 0	G8 0	B8 0
 */
EXTERN_INLINE void unpack_argb_to_r_gb_vectors_sse2(__m128i* in_2_v8i_argb_vectors, __m128i* out_3_v16i_r_gb_vectors)
{
	CONST_M128I(mask_off_gb, 0x000000FF000000FFLL, 0x000000FF000000FFLL);
	CONST_M128I(mask_off_rb, 0x0000FF000000FF00LL, 0x0000FF000000FF00LL);
	CONST_M128I(mask_off_rg, 0x00FF000000FF0000LL, 0x00FF000000FF0000LL);
	M128I(scratch1, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_srli_epi32(in_2_v8i_argb_vectors[0], 8);								// PSRLD	2	2
	// R1 G1	B1 0	R2 G2	B2 0	R3 G3	B3 0	R4 G4	B4 0

	out_3_v16i_r_gb_vectors[0] = _mm_and_si128(_M(scratch1), _M(mask_off_gb));				// PAND		2   2
	// R1 0		0 0		R2 0	0 0		R3 0	0 0		R4 0	0 0

	out_3_v16i_r_gb_vectors[1] = _mm_and_si128(_M(scratch1), _M(mask_off_rb));				// PAND		2   2
	// 0 G1 	0 0		0 G2	0 0		0 G3	0 0		0 G4 	0 0

	out_3_v16i_r_gb_vectors[1] = _mm_srli_epi32(out_3_v16i_r_gb_vectors[1], 8);				// PSRLD	2	2
	// G1 0 	0 0		G2 0	0 0		G3 0	0 0		G4 0 	0 0

	_M(scratch1) = _mm_and_si128(_M(scratch1), _M(mask_off_rg));							// PAND		2   2
	// 0 0		B1 0	0 0		B2 0	0 0		B3 0	0 0		B4 0

	out_3_v16i_r_gb_vectors[1] = _mm_or_si128(out_3_v16i_r_gb_vectors[1], _M(scratch1));	// POR		2   2
	// G1 0		B1 0	G2 0	B2 0	G3 0	B3 0	G4 0	B4 0

	_M(scratch1) = _mm_srli_epi32(in_2_v8i_argb_vectors[1], 8);								// PSRLD	2	2
	// R5 G5	B5 0	R6 G6	B6 0	R7 G7	B7 0	R8 G8	B8 0

	_M(scratch2) = _mm_and_si128(_M(scratch1), _M(mask_off_gb));							// PAND		2   2
	// R5 0		0 0		R6 0	0 0		R7 0	0 0		R8 0	0 0

	out_3_v16i_r_gb_vectors[0] = _mm_packs_epi32(out_3_v16i_r_gb_vectors[0], _M(scratch2));//  PACKSSDW	4 4 2 2
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0

	out_3_v16i_r_gb_vectors[2] = _mm_and_si128(_M(scratch1), _M(mask_off_rb));				// PAND		2   2
	// 0 G5 	0 0		0 G6	0 0		0 G7	0 0		0 G8 	0 0

	out_3_v16i_r_gb_vectors[2] = _mm_srli_epi32(out_3_v16i_r_gb_vectors[2], 8);				// PSRLD	2	2
	// G5 0 	0 0		G6 0	0 0		G7 0	0 0		G8 0 	0 0

	_M(scratch1) = _mm_and_si128(_M(scratch1), _M(mask_off_rg));							// PAND 	2   2
	// 0 0		B5 0	0 0		B6 0	0 0		B7 0	0 0		B8 0

	out_3_v16i_r_gb_vectors[2] = _mm_or_si128(out_3_v16i_r_gb_vectors[2], _M(scratch1));	// POR		2   2
	// G5 0		B5 0	G6 0	B6 0	G7 0	B7 0	G8 0	B8 0
};


/*
 * Convert 2 vectors of 16 char ARGB to 3 vectors of 8 short:
 *
 * Total latency:				6
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * A1  R1	G1  B1	A2  R2	G2  B2	A3  R3	G3  B3	A4  R4	G4  B4
 *
 * A5  R5	G5  B5	A6  R6	G6  B6	A7  R7	G7  B7	A8  R8	G8  B8
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gb1Vect
 * G1 0		B1 0	G2 0	B2 0	G3 0	B3 0	G4 0	B4 0
 *
 * gb2Vect
 * G5 0		B5 0	G6 0	B6 0	G7 0	B7 0	G8 0	B8 0
 */
EXTERN_INLINE void unpack_argb_to_r_gb_vectors_sse2_ssse3(__m128i* in_2_v8i_argb_vectors, __m128i* out_3_v16i_r_gb_vectors)
{
	CONST_M128I(shuf_r1, 0xFF0DFF09FF05FF01LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0DFF09FF05FF01LL);
	CONST_M128I(shuf_gb, 0xFF07FF06FF03FF02LL, 0xFF0FFF0EFF0BFF0ALL);

	M128I(scratch1, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_r1));					// PSHUFB	1 1 3 0 1 2
	// R1 0		R2 0	R3 0	R4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_gb_vectors[0] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_r2));	// PSHUFB	1 1 3 0 1 2
	// 0 0		0 0		0 0		0 0		R5 0	R6 0	R7 0	R8 0

	out_3_v16i_r_gb_vectors[0] = _mm_or_si128(out_3_v16i_r_gb_vectors[0], _M(scratch1));	// POR		2   2
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0

	out_3_v16i_r_gb_vectors[1] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_gb));	// PSHUFB	1 1 3 0 1 2
	// G1 0		B1 0	G2 0	B2 0	G3 0	B3 0	G4 0	B4 0

	out_3_v16i_r_gb_vectors[2] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_gb));	// PSHUFB	1 1 3 0 1 2
	// G5 0		B5 0	G6 0	B6 0	G7 0	B7 0	G8 0	B8 0
};


#endif /* RGB_UNPACK_H_ */
