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
 * Convert 2 vectors of 16 char ARGB to 4 vectors of 8 short AG1, RB1, AG2 & RB2
 *
 * Total latency:				4
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
 * 4 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect3
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 */
EXTERN_INLINE void unpack_argb_to_ag_rb_vectors_sse2(__m128i* in_2_v8i_argb_vectors, __m128i* out_4_v16i_ag_rb_vectors)
{
	CONST_M128I(mask_off_rb, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);

	out_4_v16i_ag_rb_vectors[0] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_rb));	// PAND		1	0.33
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_srli_epi16(in_2_v8i_argb_vectors[0], 8);				// PSRLW	1	1
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_4_v16i_ag_rb_vectors[2] = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_rb));	// PAND		1	0.33
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_srli_epi16(in_2_v8i_argb_vectors[1], 8);				// PSRLW	1	1
	// R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
};


/*
 * Convert 2 vectors of 16 char ARGB to 4 vectors of 8 short AG1, RB1, AG2 & RB2
 *
 * Total latency:				4
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
 * 4 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect3
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 */
EXTERN_INLINE void unpack_argb_to_ag_rb_vectors_sse2_ssse3(__m128i* in_2_v8i_argb_vectors, __m128i* out_4_v16i_ag_rb_vectors)
{
	CONST_M128I(shuf_out1, 0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	CONST_M128I(shuf_out2, 0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);

	out_4_v16i_ag_rb_vectors[0] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_out1));// PSHUFB	1	0.5
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_out2));// PSHUFB	1	0.5
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_4_v16i_ag_rb_vectors[2] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_out1));// PSHUFB	1	0.5
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_out2));// PSHUFB	1	0.5
	// R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
};


/*
 * Convert 2 vectors of 16 char ARGB to 3 vectors of 8 short:
 *
 * Total latency:				15
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
 * 3 vectors of 8 short
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gb1Vect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * gb2Vect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
EXTERN_INLINE void unpack_argb_to_r_g_b_vectors_sse2(__m128i* in_2_v8i_argb_vectors, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(mask_off_agb, 0x0000FF000000FF00LL, 0x0000FF000000FF00LL);
	CONST_M128I(mask_off_arb, 0x00FF000000FF0000LL, 0x00FF000000FF0000LL);
	CONST_M128I(mask_off_arg, 0xFF000000FF000000LL, 0xFF000000FF000000LL);
	M128I(scratch1, 0x0LL, 0x0LL);

	out_3_v16i_r_g_b_vectors[0] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_agb));// PAND		1	0.33
	// 0 R1 	0 0		0 R2	0 0		0 R3 	0 0		0 R4 	0 0

	out_3_v16i_r_g_b_vectors[0] = _mm_srli_epi16(out_3_v16i_r_g_b_vectors[0], 8);			// PSRLW	1	1
	// R1  0	0 0		R2 0	0 0		R3 0	0 0		R4 0	0 0

	_M(scratch1) = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_agb));				// PAND		1	0.33
	// 0 R5 	0 0		0 R6	0 0		0 R7 	0 0		0 R8 	0 0

	_M(scratch1) = _mm_srli_epi16(_M(scratch1), 8);											// PSRLW	1	1
	// R5  0	0 0		R6 0	0 0		R7 0	0 0		R8 0	0 0

	out_3_v16i_r_g_b_vectors[0] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[0], _M(scratch1));//PACKSSDW	1	0.5
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0



	out_3_v16i_r_g_b_vectors[1] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_arb));// PAND		1	0.33
	// 0 0		G1 0	0 0		G2 0	0 0		G3 0	0 0		G4 0

	out_3_v16i_r_g_b_vectors[1] = _mm_srli_epi32(out_3_v16i_r_g_b_vectors[1], 16);			// PSRLD	1	1
	// G1 0 	0 0		G2 0	0 0		G3 0	0 0		G4 0 	0 0

	_M(scratch1) = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_arb));				// PAND		1	0.33
	// 0 0		G5 0	0 0		G6 0	0 0		G7 0	0 0		G8 0

	_M(scratch1) = _mm_srli_epi32(_M(scratch1), 16);										// PSRLD	1	1
	// G5 0 	0 0		G6 0	0 0		G7 0	0 0		G8 0 	0 0

	out_3_v16i_r_g_b_vectors[1] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[1], _M(scratch1));//PACKSSDW	1	0.5
	// G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0



	out_3_v16i_r_g_b_vectors[2] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_arg));// PAND		1	0.33
	// 0 0		0 B1	0 0		0 B2	0 0		0 B3	0 0		0 B4

	out_3_v16i_r_g_b_vectors[2] = _mm_srli_epi32(out_3_v16i_r_g_b_vectors[2], 24);			// PSRLD	1	1
	// B1 0 	0 0		B2 0	0 0		B3 0	0 0		B4 0 	0 0

	_M(scratch1) = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_arg));				// PAND		1	0.33
	// 0 0		0 B5	0 0		0 B6	0 0		0 B7	0 0		0 B8

	_M(scratch1) = _mm_srli_epi32(_M(scratch1), 24);										// PSRLD	1	1
	// B5 0 	0 0		B6 0	0 0		B7 0	0 0		B8 0 	0 0

	out_3_v16i_r_g_b_vectors[2] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[2], _M(scratch1));//PACKSSDW	1	0.5
	// B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
};


/*
 * Convert 2 vectors of 16 char ARGB to 3 vectors of 8 short:
 *
 * Total latency:				9
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
 * 3 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
EXTERN_INLINE void unpack_argb_to_r_g_b_vectors_sse2_ssse3(__m128i* in_2_v8i_argb_vectors, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(shuf_r1, 0xFF0DFF09FF05FF01LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0DFF09FF05FF01LL);
	CONST_M128I(shuf_g1, 0xFF0EFF0AFF06FF02LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_g2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0EFF0AFF06FF02LL);
	CONST_M128I(shuf_b1, 0xFF0FFF0BFF07FF03LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_b2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0FFF0BFF07FF03LL);

	M128I(scratch1, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_r1));					// PSHUFB	1	0.5
	// R1 0		R2 0	R3 0	R4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_g_b_vectors[0] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_r2));	// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		R5 0	R6 0	R7 0	R8 0

	out_3_v16i_r_g_b_vectors[0] = _mm_or_si128(out_3_v16i_r_g_b_vectors[0], _M(scratch1));	// POR		1	0.33
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0


	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_g1));					// PSHUFB	1	0.5
	// G1 0		G2 0	G3 0	G4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_g_b_vectors[1] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_g2));	// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		G5 0	G6 0	G7 0	G8 0

	out_3_v16i_r_g_b_vectors[1] = _mm_or_si128(out_3_v16i_r_g_b_vectors[1], _M(scratch1));	// POR		1	0.33
	// G1  0	G2 0	G3  0	G4  0	G5  0	G6  0	G7  0	G8  0


	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_b1));					// PSHUFB	1	0.5
	// B1 0		B2 0	B3 0	B4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_g_b_vectors[2] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_b2));	// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		B5 0	B6 0	B7 0	B8 0

	out_3_v16i_r_g_b_vectors[2] = _mm_or_si128(out_3_v16i_r_g_b_vectors[2], _M(scratch1));	// POR		1	0.33
	// B1  0	B2 0	B3  0	B4  0	B5  0	B6  0	B7  0	B8  0
};









/*
 * Convert 2 vectors of 16 char BGRA to 4 vectors of 8 short AG1, RB1, AG2 & RB2
 *
 * Total latency:				12
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * B1  G1	R1  A1	B2  G2	R2  A2	B3  G3	R3  A3	B4  G4	R4  A4
 *
 * B5  G5	R5  A5	B6  G6	R6  A6	B7  G7	R7  A7	B8  G8	R8  A8
 *
 * OUTPUT:
 *
 * 4 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect3
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 */
EXTERN_INLINE void unpack_bgra_to_ag_rb_vectors_sse2(__m128i* in_2_v8i_argb_vectors, __m128i* out_4_v16i_ag_rb_vectors)
{
	CONST_M128I(mask_off_ag, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);

	out_4_v16i_ag_rb_vectors[1] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_ag));	// PAND		1	0.33
	// B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_shufflelo_epi16(out_4_v16i_ag_rb_vectors[1], 0xB1);	// PSHUFLW	1	0.5
	// R1 0		B1 0	R2 0	B2 0	B3 0	R3 0	B4 0	R4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_shufflehi_epi16(out_4_v16i_ag_rb_vectors[1], 0xB1);	// PSHUFHW	1	0.5
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_4_v16i_ag_rb_vectors[0] = _mm_srli_epi16(in_2_v8i_argb_vectors[0], 8);				// PSRLW	1	1
	// G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0

	out_4_v16i_ag_rb_vectors[0] = _mm_shufflelo_epi16(out_4_v16i_ag_rb_vectors[0], 0xB1);	// PSHUFLW	1	0.5
	// A1 0		G1 0	A2 0	G2 0	G3 0	A3 0	G4 0	A4 0

	out_4_v16i_ag_rb_vectors[0] = _mm_shufflehi_epi16(out_4_v16i_ag_rb_vectors[0], 0xB1);	// PSHUFHW	1	0.5
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_4_v16i_ag_rb_vectors[3] = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_ag));	// PAND		1	0.33
	// B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_shufflelo_epi16(out_4_v16i_ag_rb_vectors[3], 0xB1);	// PSHUFLW	1	0.5
	// R5 0		B5 0	R6 0	B6 0	B7 0	R7 0	B8 0	R8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_shufflehi_epi16(out_4_v16i_ag_rb_vectors[3], 0xB1);	// PSHUFHW	1	0.5
	// R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0

	out_4_v16i_ag_rb_vectors[2] = _mm_srli_epi16(in_2_v8i_argb_vectors[1], 8);				// PSRLW	1	1
	// G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0

	out_4_v16i_ag_rb_vectors[2] = _mm_shufflelo_epi16(out_4_v16i_ag_rb_vectors[2], 0xB1);	// PSHUFLW	1	0.5
	// A5 0		G5 0	A6 0	G6 0	G7 0	A7 0	G8 0	A8 0

	out_4_v16i_ag_rb_vectors[2] = _mm_shufflehi_epi16(out_4_v16i_ag_rb_vectors[2], 0xB1);	// PSHUFHW	1	0.5
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
};


/*
 * Convert 2 vectors of 16 char BGRA to 4 vectors of 8 short AG1, RB1, AG2 & RB2
 *
 * Total latency:				4
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * B1  G1	R1  A1	B2  G2	R2  A2	B3  G3	R3  A3	B4  G4	R4  A4
 *
 * B5  G5	R5  A5	B6  G6	R6  A6	B7  G7	R7  A7	B8  G8	R8  A8
 *
 * OUTPUT:
 *
 * 4 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect3
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 */
EXTERN_INLINE void unpack_bgra_to_ag_rb_vectors_sse2_ssse3(__m128i* in_2_v8i_argb_vectors, __m128i* out_4_v16i_ag_rb_vectors)
{
	CONST_M128I(shuf_out1, 0xFF05FF07FF01FF03LL, 0xFF0DFF0FFF09FF0BLL);
	CONST_M128I(shuf_out2, 0xFF04FF06FF00FF02LL, 0xFF0CFF0EFF08FF0ALL);

	out_4_v16i_ag_rb_vectors[0] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_out1));// PSHUFB	1	0.5
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_out2));// PSHUFB	1	0.5
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_4_v16i_ag_rb_vectors[2] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_out1));// PSHUFB	1	0.5
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_out2));// PSHUFB	1	0.5
	// R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
};

/*
 * Convert 2 vectors of 16 char BGRA to 3 vectors of 8 short:
 *
 * Total latency:				13
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * B1  G1	R1  A1	B2  G2	R2  A2	B3  G3	R3  A3	B4  G4	R4  A4
 *
 * B5  G5	R5  A5	B6  G6	R6  A6	B7  G7	R7  A7	B8  G8	R8  A8
 *
 * OUTPUT:
 *
 * 3 vectors of 8 short
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gb1Vect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * gb2Vect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
EXTERN_INLINE void unpack_bgra_to_r_g_b_vectors_sse2(__m128i* in_2_v8i_argb_vectors, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(mask_off_bga, 0x00FF000000FF0000LL, 0x00FF000000FF0000LL);
	CONST_M128I(mask_off_bra, 0x0000FF000000FF00LL, 0x0000FF000000FF00LL);
	CONST_M128I(mask_off_gra, 0x000000FF000000FFLL, 0x000000FF000000FFLL);
	M128I(scratch1, 0x0LL, 0x0LL);
	
	out_3_v16i_r_g_b_vectors[0] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_bga));// PAND		1	0.33
	// 0 0		R1 0	0 0		R2 0	0 0		R3 0	0 0		R4 0

	out_3_v16i_r_g_b_vectors[0] = _mm_srli_epi32(out_3_v16i_r_g_b_vectors[0], 16);			// PSRLD	1	1
	// R1  0	0 0		R2 0	0 0		R3 0	0 0		R4 0	0 0
	
	_M(scratch1) = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_bga));				// PAND		1	0.33
	// 0 0		R5 0	0 0		R6 0	0 0		R7 0	0 0		R8 0

	_M(scratch1) = _mm_srli_epi32(_M(scratch1), 16);										// PSRLD	1	1
	// R5  0	0 0		R6 0	0 0		R7 0	0 0		R8 0	0 0
	
	out_3_v16i_r_g_b_vectors[0] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[0], _M(scratch1));//PACKSSDW	1	0.5
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0



	out_3_v16i_r_g_b_vectors[1] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_bra));// PAND		1	0.33
	// 0 G1		0 0		0 G2	0 0		0 G3	0 0		0 G4	0 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_srli_epi16(out_3_v16i_r_g_b_vectors[1], 8);			// PSRLW	1	1
	// G1 0 	0 0		G2 0	0 0		G3 0	0 0		G4 0 	0 0
	
	_M(scratch1) = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_bra));				// PAND		1	0.33
	// 0 G5		0 0		0 G6	0 0		0 G7	0 0		0 G8	0 0
	
	_M(scratch1) = _mm_srli_epi16(_M(scratch1), 8);											// PSRLW	1	1
	// G5 0 	0 0		G6 0	0 0		G7 0	0 0		G8 0 	0 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[1], _M(scratch1));//PACKSSDW	1	0.5
	// G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
	
	

	out_3_v16i_r_g_b_vectors[2] = _mm_and_si128(in_2_v8i_argb_vectors[0], _M(mask_off_gra));// PAND		1	0.33
	// B1 0		0 0		B2 0	0 0		B3 0	0 0		B4 0	0 0

	_M(scratch1) = _mm_and_si128(in_2_v8i_argb_vectors[1], _M(mask_off_gra));				// PAND		1	0.33
	// B5 0		0 0		B6 0	0 0		B7 0	0 0		B8 0	0 0

	out_3_v16i_r_g_b_vectors[2] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[2], _M(scratch1));//PACKSSDW	1	0.5
	// B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
};


/*
 * Convert 2 vectors of 16 char BGRA to 3 vectors of 8 short:
 *
 * Total latency:				9
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * B1  G1	R1  A1	B2  G2	R2  A2	B3  G3	R3  A3	B4  G4	R4  A4
 *
 * B5  G5	R5  A5	B6  G6	R6  A6	B7  G7	R7  A7	B8  G8	R8  A8
 *
 * OUTPUT:
 *
 * 3 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
EXTERN_INLINE void unpack_bgra_to_r_g_b_vectors_sse2_ssse3(__m128i* in_2_v8i_argb_vectors, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(shuf_r1, 0xFF0EFF0AFF06FF02LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0EFF0AFF06FF02LL);
	CONST_M128I(shuf_g1, 0xFF0DFF09FF05FF01LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_g2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0DFF09FF05FF01LL);
	CONST_M128I(shuf_b1, 0xFF0CFF08FF04FF00LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_b2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0CFF08FF04FF00LL);
	
	M128I(scratch1, 0x0LL, 0x0LL);
	
	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_r1));					// PSHUFB	1	0.5
	// R1 0		R2 0	R3 0	R4 0	0 0		0 0		0 0		0 0
	
	out_3_v16i_r_g_b_vectors[0] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_r2));	// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		R5 0	R6 0	R7 0	R8 0
	
	out_3_v16i_r_g_b_vectors[0] = _mm_or_si128(out_3_v16i_r_g_b_vectors[0], _M(scratch1));	// POR		1	0.33
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0

	
	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_g1));					// PSHUFB	1	0.5
	// G1 0		G2 0	G3 0	G4 0	0 0		0 0		0 0		0 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_g2));	// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		G5 0	G6 0	G7 0	G8 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_or_si128(out_3_v16i_r_g_b_vectors[1], _M(scratch1));	// POR		1	0.33
	// G1  0	G2 0	G3  0	G4  0	G5  0	G6  0	G7  0	G8  0

	
	_M(scratch1) = _mm_shuffle_epi8(in_2_v8i_argb_vectors[0], _M(shuf_b1));					// PSHUFB	1	0.5
	// B1 0		B2 0	B3 0	B4 0	0 0		0 0		0 0		0 0
	
	out_3_v16i_r_g_b_vectors[2] = _mm_shuffle_epi8(in_2_v8i_argb_vectors[1], _M(shuf_b2));	// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		B5 0	B6 0	B7 0	B8 0
	
	out_3_v16i_r_g_b_vectors[2] = _mm_or_si128(out_3_v16i_r_g_b_vectors[2], _M(scratch1));	// POR		1	0.33
	// B1  0	B2 0	B3  0	B4  0	B5  0	B6  0	B7  0	B8  0
};


#endif /* RGB_UNPACK_H_ */
