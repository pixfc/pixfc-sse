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

#include <emmintrin.h>
#include <stdint.h>
#include <string.h>
#include <tmmintrin.h>

#include "common.h"
#include "debug_support.h"
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
#undef UNALIGNED_RGB32_INPUT_PREAMBLE
#undef UNALIGNED_RGB24_INPUT_PREAMBLE
#undef INPUT_VECT

#if GENERATE_UNALIGNED_INLINES == 1
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void unaligned_ ## fn_suffix(__VA_ARGS__)
	#define UNALIGNED_RGB32_INPUT_PREAMBLE			DECLARE_VECT_ARRAY2_N_UNALIGN_LOAD(aligned_vector, input);
	#define UNALIGNED_RGB24_INPUT_PREAMBLE			DECLARE_VECT_ARRAY3_N_UNALIGN_LOAD(aligned_vector, input);
	#define INPUT_VECT								aligned_vector
#else
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void fn_suffix(__VA_ARGS__)
	#define UNALIGNED_RGB32_INPUT_PREAMBLE
	#define UNALIGNED_RGB24_INPUT_PREAMBLE
	#define INPUT_VECT								input
#endif

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
INLINE_NAME(unpack_argb_to_ag_rb_vectors_sse2,__m128i* input, __m128i* out_4_v16i_ag_rb_vectors)
{
	CONST_M128I(mask_off_rb, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;

	out_4_v16i_ag_rb_vectors[0] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_rb));	// PAND		1	0.33
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_srli_epi16(INPUT_VECT[0], 8);					// PSRLW	1	1
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_4_v16i_ag_rb_vectors[2] = _mm_and_si128(INPUT_VECT[1], _M(mask_off_rb));	// PAND		1	0.33
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_srli_epi16(INPUT_VECT[1], 8);					// PSRLW	1	1
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
INLINE_NAME(unpack_argb_to_ag_rb_vectors_sse2_ssse3, __m128i* input, __m128i* out_4_v16i_ag_rb_vectors)
{
	CONST_M128I(shuf_out1, 0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	CONST_M128I(shuf_out2, 0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;

	out_4_v16i_ag_rb_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_out1));	// PSHUFB	1	0.5
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_4_v16i_ag_rb_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_out2));	// PSHUFB	1	0.5
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_4_v16i_ag_rb_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_out1));	// PSHUFB	1	0.5
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0

	out_4_v16i_ag_rb_vectors[3] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_out2));	// PSHUFB	1	0.5
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
INLINE_NAME(unpack_argb_to_r_g_b_vectors_sse2, __m128i* input, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(mask_off_agb, 0x0000FF000000FF00LL, 0x0000FF000000FF00LL);
	CONST_M128I(mask_off_arb, 0x00FF000000FF0000LL, 0x00FF000000FF0000LL);
	CONST_M128I(mask_off_arg, 0xFF000000FF000000LL, 0xFF000000FF000000LL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;
	M128I(scratch1, 0x0LL, 0x0LL);

	out_3_v16i_r_g_b_vectors[0] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_agb));			// PAND		1	0.33
	// 0 R1 	0 0		0 R2	0 0		0 R3 	0 0		0 R4 	0 0

	out_3_v16i_r_g_b_vectors[0] = _mm_srli_epi16(out_3_v16i_r_g_b_vectors[0], 8);			// PSRLW	1	1
	// R1  0	0 0		R2 0	0 0		R3 0	0 0		R4 0	0 0

	_M(scratch1) = _mm_and_si128(INPUT_VECT[1], _M(mask_off_agb));							// PAND		1	0.33
	// 0 R5 	0 0		0 R6	0 0		0 R7 	0 0		0 R8 	0 0

	_M(scratch1) = _mm_srli_epi16(_M(scratch1), 8);											// PSRLW	1	1
	// R5  0	0 0		R6 0	0 0		R7 0	0 0		R8 0	0 0

	out_3_v16i_r_g_b_vectors[0] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[0], _M(scratch1));//PACKSSDW	1	0.5
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0



	out_3_v16i_r_g_b_vectors[1] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_arb));			// PAND		1	0.33
	// 0 0		G1 0	0 0		G2 0	0 0		G3 0	0 0		G4 0

	out_3_v16i_r_g_b_vectors[1] = _mm_srli_epi32(out_3_v16i_r_g_b_vectors[1], 16);			// PSRLD	1	1
	// G1 0 	0 0		G2 0	0 0		G3 0	0 0		G4 0 	0 0

	_M(scratch1) = _mm_and_si128(INPUT_VECT[1], _M(mask_off_arb));							// PAND		1	0.33
	// 0 0		G5 0	0 0		G6 0	0 0		G7 0	0 0		G8 0

	_M(scratch1) = _mm_srli_epi32(_M(scratch1), 16);										// PSRLD	1	1
	// G5 0 	0 0		G6 0	0 0		G7 0	0 0		G8 0 	0 0

	out_3_v16i_r_g_b_vectors[1] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[1], _M(scratch1));//PACKSSDW	1	0.5
	// G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0



	out_3_v16i_r_g_b_vectors[2] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_arg));			// PAND		1	0.33
	// 0 0		0 B1	0 0		0 B2	0 0		0 B3	0 0		0 B4

	out_3_v16i_r_g_b_vectors[2] = _mm_srli_epi32(out_3_v16i_r_g_b_vectors[2], 24);			// PSRLD	1	1
	// B1 0 	0 0		B2 0	0 0		B3 0	0 0		B4 0 	0 0

	_M(scratch1) = _mm_and_si128(INPUT_VECT[1], _M(mask_off_arg));							// PAND		1	0.33
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
INLINE_NAME(unpack_argb_to_r_g_b_vectors_sse2_ssse3, __m128i* input, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(shuf_r1, 0xFF0DFF09FF05FF01LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0DFF09FF05FF01LL);
	CONST_M128I(shuf_g1, 0xFF0EFF0AFF06FF02LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_g2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0EFF0AFF06FF02LL);
	CONST_M128I(shuf_b1, 0xFF0FFF0BFF07FF03LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_b2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0FFF0BFF07FF03LL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;
	M128I(scratch1, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_r1));							// PSHUFB	1	0.5
	// R1 0		R2 0	R3 0	R4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_g_b_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_r2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		R5 0	R6 0	R7 0	R8 0

	out_3_v16i_r_g_b_vectors[0] = _mm_or_si128(out_3_v16i_r_g_b_vectors[0], _M(scratch1));	// POR		1	0.33
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0


	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_g1));							// PSHUFB	1	0.5
	// G1 0		G2 0	G3 0	G4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_g_b_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_g2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		G5 0	G6 0	G7 0	G8 0

	out_3_v16i_r_g_b_vectors[1] = _mm_or_si128(out_3_v16i_r_g_b_vectors[1], _M(scratch1));	// POR		1	0.33
	// G1  0	G2 0	G3  0	G4  0	G5  0	G6  0	G7  0	G8  0


	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_b1));							// PSHUFB	1	0.5
	// B1 0		B2 0	B3 0	B4 0	0 0		0 0		0 0		0 0

	out_3_v16i_r_g_b_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_b2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		B5 0	B6 0	B7 0	B8 0

	out_3_v16i_r_g_b_vectors[2] = _mm_or_si128(out_3_v16i_r_g_b_vectors[2], _M(scratch1));	// POR		1	0.33
	// B1  0	B2 0	B3  0	B4  0	B5  0	B6  0	B7  0	B8  0
};









/*
 * Convert 2 vectors of 16 char BGRA to 4 vectors of 8 short GA1, BR1, GA2 & BR2
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
 * gaVect1
 * G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0
 *
 * brVect1
 * B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0
 *
 * gaVect2
 * G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0
 *
 * brVect3
 * B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0
 */
INLINE_NAME(unpack_bgra_to_ga_br_vectors_sse2, __m128i* input, __m128i* out_4_v16i_ga_br_vectors)
{
	CONST_M128I(mask_off_ga, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;
	
	out_4_v16i_ga_br_vectors[0] = _mm_srli_epi16(INPUT_VECT[0], 8);					// PSRLW	1	1
	// G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0

	out_4_v16i_ga_br_vectors[1] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_ga));	// PAND		1	0.33
	// B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0
	
	out_4_v16i_ga_br_vectors[2] = _mm_srli_epi16(INPUT_VECT[1], 8);					// PSRLW	1	1
	// G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0
	
	out_4_v16i_ga_br_vectors[3] = _mm_and_si128(INPUT_VECT[1], _M(mask_off_ga));	// PAND		1	0.33
	// B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0
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
 * gaVect1
 * G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0
 *
 * brVect1
 * B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0
 *
 * gaVect2
 * G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0
 *
 * brVect3
 * B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0
 */
INLINE_NAME(unpack_bgra_to_ga_br_vectors_sse2_ssse3, __m128i* input, __m128i* out_4_v16i_ga_br_vectors)
{
	CONST_M128I(shuf_out1, 0xFF07FF05FF03FF01LL, 0xFF0FFF0DFF0BFF09LL);
	CONST_M128I(shuf_out2, 0xFF06FF04FF02FF00LL, 0xFF0EFF0CFF0AFF08LL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;

	out_4_v16i_ga_br_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_out1));// PSHUFB	1	0.5
	// G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0

	out_4_v16i_ga_br_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_out2));// PSHUFB	1	0.5
	// B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0

	out_4_v16i_ga_br_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_out1));// PSHUFB	1	0.5
	// G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0

	out_4_v16i_ga_br_vectors[3] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_out2));// PSHUFB	1	0.5
	// B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0
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
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
INLINE_NAME(unpack_bgra_to_r_g_b_vectors_sse2, __m128i* input, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(mask_off_bga, 0x00FF000000FF0000LL, 0x00FF000000FF0000LL);
	CONST_M128I(mask_off_bra, 0x0000FF000000FF00LL, 0x0000FF000000FF00LL);
	CONST_M128I(mask_off_gra, 0x000000FF000000FFLL, 0x000000FF000000FFLL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;
	M128I(scratch1, 0x0LL, 0x0LL);
	
	out_3_v16i_r_g_b_vectors[0] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_bga));			// PAND		1	0.33
	// 0 0		R1 0	0 0		R2 0	0 0		R3 0	0 0		R4 0

	out_3_v16i_r_g_b_vectors[0] = _mm_srli_epi32(out_3_v16i_r_g_b_vectors[0], 16);			// PSRLD	1	1
	// R1  0	0 0		R2 0	0 0		R3 0	0 0		R4 0	0 0
	
	_M(scratch1) = _mm_and_si128(INPUT_VECT[1], _M(mask_off_bga));							// PAND		1	0.33
	// 0 0		R5 0	0 0		R6 0	0 0		R7 0	0 0		R8 0

	_M(scratch1) = _mm_srli_epi32(_M(scratch1), 16);										// PSRLD	1	1
	// R5  0	0 0		R6 0	0 0		R7 0	0 0		R8 0	0 0
	
	out_3_v16i_r_g_b_vectors[0] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[0], _M(scratch1));//PACKSSDW	1	0.5
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0



	out_3_v16i_r_g_b_vectors[1] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_bra));			// PAND		1	0.33
	// 0 G1		0 0		0 G2	0 0		0 G3	0 0		0 G4	0 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_srli_epi16(out_3_v16i_r_g_b_vectors[1], 8);			// PSRLW	1	1
	// G1 0 	0 0		G2 0	0 0		G3 0	0 0		G4 0 	0 0
	
	_M(scratch1) = _mm_and_si128(INPUT_VECT[1], _M(mask_off_bra));							// PAND		1	0.33
	// 0 G5		0 0		0 G6	0 0		0 G7	0 0		0 G8	0 0
	
	_M(scratch1) = _mm_srli_epi16(_M(scratch1), 8);											// PSRLW	1	1
	// G5 0 	0 0		G6 0	0 0		G7 0	0 0		G8 0 	0 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_packs_epi32(out_3_v16i_r_g_b_vectors[1], _M(scratch1));//PACKSSDW	1	0.5
	// G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
	
	

	out_3_v16i_r_g_b_vectors[2] = _mm_and_si128(INPUT_VECT[0], _M(mask_off_gra));			// PAND		1	0.33
	// B1 0		0 0		B2 0	0 0		B3 0	0 0		B4 0	0 0

	_M(scratch1) = _mm_and_si128(INPUT_VECT[1], _M(mask_off_gra));							// PAND		1	0.33
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
INLINE_NAME(unpack_bgra_to_r_g_b_vectors_sse2_ssse3, __m128i* input, __m128i* out_3_v16i_r_g_b_vectors)
{
	CONST_M128I(shuf_r1, 0xFF0EFF0AFF06FF02LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0EFF0AFF06FF02LL);
	CONST_M128I(shuf_g1, 0xFF0DFF09FF05FF01LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_g2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0DFF09FF05FF01LL);
	CONST_M128I(shuf_b1, 0xFF0CFF08FF04FF00LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_b2, 0xFFFFFFFFFFFFFFFFLL, 0xFF0CFF08FF04FF00LL);
	UNALIGNED_RGB32_INPUT_PREAMBLE;
	M128I(scratch1, 0x0LL, 0x0LL);
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_r1));							// PSHUFB	1	0.5
	// R1 0		R2 0	R3 0	R4 0	0 0		0 0		0 0		0 0
	
	out_3_v16i_r_g_b_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_r2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		R5 0	R6 0	R7 0	R8 0
	
	out_3_v16i_r_g_b_vectors[0] = _mm_or_si128(out_3_v16i_r_g_b_vectors[0], _M(scratch1));	// POR		1	0.33
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0

	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_g1));							// PSHUFB	1	0.5
	// G1 0		G2 0	G3 0	G4 0	0 0		0 0		0 0		0 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_g2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		G5 0	G6 0	G7 0	G8 0
	
	out_3_v16i_r_g_b_vectors[1] = _mm_or_si128(out_3_v16i_r_g_b_vectors[1], _M(scratch1));	// POR		1	0.33
	// G1  0	G2 0	G3  0	G4  0	G5  0	G6  0	G7  0	G8  0

	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_b1));							// PSHUFB	1	0.5
	// B1 0		B2 0	B3 0	B4 0	0 0		0 0		0 0		0 0
	
	out_3_v16i_r_g_b_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_b2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		B5 0	B6 0	B7 0	B8 0
	
	out_3_v16i_r_g_b_vectors[2] = _mm_or_si128(out_3_v16i_r_g_b_vectors[2], _M(scratch1));	// POR		1	0.33
	// B1  0	B2 0	B3  0	B4  0	B5  0	B6  0	B7  0	B8  0
};






/*
 * Convert 2 vectors of 16 char RGB24 to 7 vectors of 8 short AG1, RB1, AG2 & RB2,
 * AG3, RB3, AG4 & RB4
 *
 * Total latency:				X
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4	G4 B4	R5 G5	B5 R6
 *
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10 B10	R11 G11
 *
 * B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16 G16 B16
 *
 * OUTPUT:
 *
 * 8 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect2
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 *
 * agVect3
 * A9 0		G9 0	A10 0	G10 0	A11 0	G11 0	A12 0	G12 0
 *
 * rbVect3
 * R9 0		B9 0	R10 0	B10 0	R11 0	B11 0	R12 0	B12 0
 *
 * agVect4
 * A13 0	G13 0	A14 0	G14 0	A15 0	G15 0	A16 0	G16 0
 *
 * rbVect4
 * R13 0	B13 0	R14 0	B14 0	R15 0	B15 0	R16 0	B16 0
 */
INLINE_NAME(unpack_rgb24_to_ag_rb_vectors_sse2, __m128i* in_3_v8i_rgb24_vectors, __m128i* out_8_v16i_ag_rb_vectors)
{
	__m128i		argb_vectors[4];
	uint32_t	num_pixels = 16;
	uint8_t*	dest = (uint8_t *) argb_vectors;
	uint8_t*	src = (uint8_t *) in_3_v8i_rgb24_vectors;

	argb_vectors[0] = _mm_setzero_si128();
	argb_vectors[1] = _mm_setzero_si128();
	argb_vectors[2] = _mm_setzero_si128();
	argb_vectors[3] = _mm_setzero_si128();
	dest++;

	// Unpack to ARGB first, then to AG / RB
	while (num_pixels-- > 0) {
		memcpy(dest, src, 3);
		dest += 4;
		src += 3;
	}

	unpack_argb_to_ag_rb_vectors_sse2(&argb_vectors[0], &out_8_v16i_ag_rb_vectors[0]);
	unpack_argb_to_ag_rb_vectors_sse2(&argb_vectors[2], &out_8_v16i_ag_rb_vectors[4]);
};


/*
 * Convert 3 vectors of 16 char RGB24 to 4 vectors of 8 short AG1, RB1, AG2 & RB2,
 * AG3, RB3, AG4 & RB4
 *
 * Total latency:				4
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4	G4 B4	R5 G5	B5 R6
 *
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10 B10	R11 G11
 *
 * B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16 G16 B16
 *
 *
 * OUTPUT:
 * 8 vectors of 8 short
 *
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect2
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 *
 * agVect3
 * A9 0		G9 0	A10 0	G10 0	A11 0	G11 0	A12 0	G12 0
 *
 * rbVect3
 * R9 0		B9 0	R10 0	B10 0	R11 0	B11 0	R12 0	B12 0
 *
 * agVect4
 * A13 0	G13 0	A14 0	G14 0	A15 0	G15 0	A16 0	G16 0
 *
 * rbVect4
 * R13 0	B13 0	R14 0	B14 0	R15 0	B15 0	R16 0	B16 0
 */
INLINE_NAME(unpack_rgb24_to_ag_rb_vectors_sse2_ssse3, __m128i* input, __m128i* out_8_v16i_ag_rb_vectors)
{
	CONST_M128I(shuf_ag1, 0xFF04FFFFFF01FFFFLL, 0xFF0AFFFFFF07FFFFLL);
	CONST_M128I(shuf_rb1, 0xFF05FF03FF02FF00LL, 0xFF0BFF09FF08FF06LL);
	CONST_M128I(shuf_ag21, 0xFFFFFFFFFF0DFFFFLL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_ag22, 0xFFF00FFFFFFFFFFFLL, 0xFFFFFF06FFFFFF03LL);
	CONST_M128I(shuf_rb21, 0xFFFFFF0FFF0EFF0CLL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_rb22, 0xFF01FFFFFFFFFFFFLL, 0xFF07FF05FF04FF02LL);
	CONST_M128I(shuf_ag31, 0xFF0CFFFFFF09FFFFLL, 0xFFFFFFFFFF0FFFFFLL);
	CONST_M128I(shuf_ag32, 0xFFFFFFFFFFFFFFFFLL, 0xFFF02FFFFFFFFFFFLL);
	CONST_M128I(shuf_rb31, 0xFF0DFF0BFF0AFF08LL, 0xFFFFFFFFFFFFFF0ELL);
	CONST_M128I(shuf_rb32, 0xFFFFFFFFFFFFFFFFLL, 0xFF03FF01FF00FFFFLL);
	CONST_M128I(shuf_ag4, 0xFF08FFFFFF05FFFFLL, 0xFF0EFFFFFF0BFFFFLL);
	CONST_M128I(shuf_rb4, 0xFF09FF07FF06FF04LL, 0xFF0FFF0DFF0CFF0ALL);
	UNALIGNED_RGB24_INPUT_PREAMBLE;


	out_8_v16i_ag_rb_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_ag1));// PSHUFB	1	0.5
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0

	out_8_v16i_ag_rb_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_rb1));// PSHUFB	1	0.5
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0

	out_8_v16i_ag_rb_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_ag21));// PSHUFB	1	0.5
	// A5 0		G5 0	0 0		0 0		0 0		0 0		0 0		0 0
	out_8_v16i_ag_rb_vectors[2] = _mm_or_si128(out_8_v16i_ag_rb_vectors[2], _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_ag22)));
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0			// POR		1	0.33
																				// PSHUFB	1	0.5

	out_8_v16i_ag_rb_vectors[3] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_rb21));// PSHUFB	1	0.5
	// R5 0		B5 0	0 0		0 0		0 0		0 0		0 0		0 0
	out_8_v16i_ag_rb_vectors[3] = _mm_or_si128(out_8_v16i_ag_rb_vectors[3], _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_rb22)));
	// R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0			// POR		1	0.33
																				// PSHUFB	1	0.5

	out_8_v16i_ag_rb_vectors[4] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_ag31));// PSHUFB	1	0.5
	// A9 0		G9 0	A10 0	G10 0	A11 0	G11 0	0 0		0 0
	out_8_v16i_ag_rb_vectors[4] = _mm_or_si128(out_8_v16i_ag_rb_vectors[4], _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_ag32)));
	// A9 0		G9 0	A10 0	G10 0	A1 0	G11 0	A12 0	G12 0			// POR		1	0.33
																				// PSHUFB	1	0.5

	out_8_v16i_ag_rb_vectors[5] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_rb31));// PSHUFB	1	0.5
	// R9 0		B9 0	R10 0	B10 0	R11 0	0 0		0 0		0 0
	out_8_v16i_ag_rb_vectors[5] = _mm_or_si128(out_8_v16i_ag_rb_vectors[5], _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_rb32)));
	// R9 0		B9 0	R10 0	B10 0	R11 0	B11 0	R12 0	B12 0			// POR		1	0.33
																				// PSHUFB	1	0.5

	out_8_v16i_ag_rb_vectors[6] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_ag4));// PSHUFB	1	0.5
	// A13 0	G13 0	A14 0	G14 0	A15 0	G15 0	A16 0	G16 0

	out_8_v16i_ag_rb_vectors[7] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_rb4));// PSHUFB	1	0.5
	// R13 0	B13 0	R14 0	B14 0	R15 0	B15 0	R16 0	B16 0
};


/*
 * Convert 2 vectors of 16 char RGB24 to 6 vectors of 8 short:
 *
 * Total latency:				x
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4	G4 B4	R5 G5	B5 R6
 *
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10 B10	R11 G11
 *
 * B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16 G16 B16
 *
 * OUTPUT:
 * 6 vectors of 8 short
  *
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * rVect
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 *
 * gVect
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 *
 * bVect
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 */
INLINE_NAME(unpack_rgb24_to_r_g_b_vectors_sse2, __m128i* in_3_v8i_rgb24_vectors, __m128i* out_6_v16i_r_g_b_vectors)
{
	__m128i		argb_vectors[4];
	uint32_t	index = 16;
	uint8_t*	dest = (uint8_t *) argb_vectors;
	uint8_t*	src = (uint8_t *) in_3_v8i_rgb24_vectors;
	
	argb_vectors[0] = _mm_setzero_si128();
	argb_vectors[1] = _mm_setzero_si128();
	argb_vectors[2] = _mm_setzero_si128();
	argb_vectors[3] = _mm_setzero_si128();
	dest++;

	// Unpack to ARGB first, then to AG / RB
	while (index < 16) {
		memcpy(dest, src, 3);
		dest += 4;
		src += 3;
	}
	
	unpack_argb_to_r_g_b_vectors_sse2(&argb_vectors[0], &out_6_v16i_r_g_b_vectors[0]);
	unpack_argb_to_r_g_b_vectors_sse2(&argb_vectors[2], &out_6_v16i_r_g_b_vectors[3]);
};


/*
 * Convert 3 vectors of 16 char RGB24 to 6 vectors of 8 short:
 *
 * Total latency:				18
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4	G4 B4	R5 G5	B5 R6
 *
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10 B10	R11 G11
 *
 * B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16 G16 B16
 *
 * OUTPUT:
 *
 * 6 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
INLINE_NAME(unpack_rgb24_to_r_g_b_vectors_sse2_ssse3, __m128i* input, __m128i* out_6_v16i_r_g_b_vectors)
{
	CONST_M128I(shuf_r1, 0xFF09FF06FF03FF00LL, 0xFFFFFFFFFF0FFF0CLL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF05FF02FFFFFFFFLL);
	CONST_M128I(shuf_g1, 0xFF0AFF07FF04FF01LL, 0xFFFFFFFFFFFFFF0DLL);
	CONST_M128I(shuf_g2, 0xFFFFFFFFFFFFFFFFLL, 0xFF06FF03FF00FFFFLL);
	CONST_M128I(shuf_b1, 0xFF0BFF08FF05FF02LL, 0xFFFFFFFFFFFFFF0ELL);
	CONST_M128I(shuf_b2, 0xFFFFFFFFFFFFFFFFLL, 0xFF07FF04FF01FFFFLL);
	
	CONST_M128I(shuf_r3, 0xFFFFFF0EFF0BFF08LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r4, 0xFF01FFFFFFFFFFFFLL, 0xFF0DFF0AFF07FF04LL);
	CONST_M128I(shuf_g3, 0xFFFFFF0FFF0CFF09LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_g4, 0xFF02FFFFFFFFFFFFLL, 0xFF0EFF0BFF08FF05LL);
	CONST_M128I(shuf_b3, 0xFFFFFFFFFF0DFF0ALL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_b4, 0xFF03FF00FFFFFFFFLL, 0xFF0FFF0CFF09FF06LL);

	UNALIGNED_RGB24_INPUT_PREAMBLE;
	M128I(scratch1, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_r1));							// PSHUFB	1	0.5
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	0 0		0 0

	out_6_v16i_r_g_b_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_r2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		0 0		0 0		R7 0	R8 0

	out_6_v16i_r_g_b_vectors[0] = _mm_or_si128(out_6_v16i_r_g_b_vectors[0], _M(scratch1));	// POR		1	0.33
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0


	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_g1));							// PSHUFB	1	0.5
	// G1 0		G2 0	G3 0	G4 0	0 0		0 0		0 0		0 0

	out_6_v16i_r_g_b_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_g2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		G5 0	G6 0	G7 0	G8 0

	out_6_v16i_r_g_b_vectors[1] = _mm_or_si128(out_6_v16i_r_g_b_vectors[1], _M(scratch1));	// POR		1	0.33
	// G1  0	G2 0	G3  0	G4  0	G5  0	G6  0	G7  0	G8  0


	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_b1));							// PSHUFB	1	0.5
	// B1 0		B2 0	B3 0	B4 0	0 0		0 0		0 0		0 0

	out_6_v16i_r_g_b_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_b2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		B5 0	B6 0	B7 0	B8 0

	out_6_v16i_r_g_b_vectors[2] = _mm_or_si128(out_6_v16i_r_g_b_vectors[2], _M(scratch1));	// POR		1	0.33
	// B1  0	B2 0	B3  0	B4  0	B5  0	B6  0	B7  0	B8  0
	
	
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_r3));							// PSHUFB	1	0.5
	// R9 0		R10 0	R11 0	0 0		0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[3] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_r4));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		R12 0	R13 0	R14 0	R15 0	R16 0
	
	out_6_v16i_r_g_b_vectors[3] = _mm_or_si128(out_6_v16i_r_g_b_vectors[3], _M(scratch1));	// POR		1	0.33
	// R9  0	R10  0	R11 0	R12  0	R13  0	R14  0	R15  0	R16  0
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_g3));							// PSHUFB	1	0.5
	// G9 0		G10 0	G11 0	0 0		0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[4] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_g4));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		G12 0	G13 0	G14 0	G15 0	G16 0
	
	out_6_v16i_r_g_b_vectors[4] = _mm_or_si128(out_6_v16i_r_g_b_vectors[4], _M(scratch1));	// POR		1	0.33
	// G9  0	G10 0	G11  0	G12  0	G13  0	G14  0	G15  0	G16  0
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_b3));							// PSHUFB	1	0.5
	// B9 0		B10 0	0 0		0 0		0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[5] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_b4));				// PSHUFB	1	0.5
	// 0 0		0 0		B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
	
	out_6_v16i_r_g_b_vectors[5] = _mm_or_si128(out_6_v16i_r_g_b_vectors[5], _M(scratch1));	// POR		1	0.33
	// B9  0	B10 0	B11  0	B12  0	B13  0	B14  0	B15  0	B16  0
};





/*
 * Convert 2 vectors of 16 char BGR24 to 7 vectors of 8 short AG1, RB1, AG2 & RB2,
 * AG3, RB3, AG4 & RB4
 *
 * Total latency:				X
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4	G4 R4	B5 G5	R5 B6
 *
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10 R10	B11 G11
 *
 * R11 B12	G12 R12	B13 G13	R13 B14	G14 R14	B15 G15	R15 B16 G16 R16
 *
 * OUTPUT:
 *
 * 8 vectors of 8 short
 * gaVect1
 * G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0
 *
 * brVect1
 * B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0
 *
 * gaVect2
 * G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0
 *
 * brVect2
 * B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0
 *
 * agVect3
 * G9 0		A9 0	G10 0	A10 0	G11 0	A11 0	G12 0	A12 0
 *
 * brVect3
 * B9 0		R9 0	B10 0	R10 0	B11 0	R11 0	B12 0	R12 0
 *
 * gaVect4
 * G13 0	A13 0	G14 0	A14 0	G15 0	A15 0	G16 0	A16 0
 *
 * brVect4
 * B13 0	R13 0	B14 0	R14 0	B15 0	R15 0	B16 0	R16 0
 */
INLINE_NAME(unpack_bgr24_to_ga_br_vectors_sse2, __m128i* in_3_v8i_bgr24_vectors, __m128i* out_8_v16i_ga_br_vectors)
{
	__m128i		bgra_vectors[4];
	uint32_t	num_pixels = 16;
	uint8_t*	dest = (uint8_t *) bgra_vectors;
	uint8_t*	src = (uint8_t *) in_3_v8i_bgr24_vectors;

	bgra_vectors[0] = _mm_setzero_si128();
	bgra_vectors[1] = _mm_setzero_si128();
	bgra_vectors[2] = _mm_setzero_si128();
	bgra_vectors[3] = _mm_setzero_si128();

	// Unpack to BGRA first, then to GA / BR
	while (num_pixels-- > 0) {
		memcpy(dest, src, 3);
		dest += 4;
		src += 3;
	}
	
	unpack_bgra_to_ga_br_vectors_sse2(&bgra_vectors[0], &out_8_v16i_ga_br_vectors[0]);
	unpack_bgra_to_ga_br_vectors_sse2(&bgra_vectors[2], &out_8_v16i_ga_br_vectors[4]);
};


/*
 * Convert 3 vectors of 16 char BGR24 to 4 vectors of 8 short AG1, RB1, AG2 & RB2,
 * AG3, RB3, AG4 & RB4
 *
 * Total latency:				4
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4	G4 R4	B5 G5	R5 B6
 *
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10 R10	B11 G11
 *
 * R11 B12	G12 R12	B13 G13	R13 B14	G14 R14	B15 G15	R15 B16 G16 R16
 *
 *
 * OUTPUT:
 * 8 vectors of 8 short
 *
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect2
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 *
 * agVect3
 * A9 0		G9 0	A10 0	G10 0	A11 0	G11 0	A12 0	G12 0
 *
 * rbVect3
 * R9 0		B9 0	R10 0	B10 0	R11 0	B11 0	R12 0	B12 0
 *
 * agVect4
 * A13 0	G13 0	A14 0	G14 0	A15 0	G15 0	A16 0	G16 0
 *
 * rbVect4
 * R13 0	B13 0	R14 0	B14 0	R15 0	B15 0	R16 0	B16 0
 */
INLINE_NAME(unpack_bgr24_to_ag_rb_vectors_sse2_ssse3, __m128i* input, __m128i* out_8_v16i_ag_rb_vectors)
{
	CONST_M128I(shuf_ag1, 0xFF04FFFFFF01FFFFLL, 0xFF0AFFFFFF07FFFFLL);
	CONST_M128I(shuf_rb1, 0xFF03FF05FF00FF02LL, 0xFF09FF0BFF06FF08LL);
	CONST_M128I(shuf_ag21, 0xFFFFFFFFFF0DFFFFLL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_ag22, 0xFFF00FFFFFFFFFFFLL, 0xFFFFFF06FFFFFF03LL);
	CONST_M128I(shuf_rb21, 0xFF0FFFFFFF0CFF0ELL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_rb22, 0xFFFFFF01FFFFFFFFLL, 0xFF05FF07FF02FF04LL);
	CONST_M128I(shuf_ag31, 0xFF0CFFFFFF09FFFFLL, 0xFFFFFFFFFF0FFFFFLL);
	CONST_M128I(shuf_ag32, 0xFFFFFFFFFFFFFFFFLL, 0xFFF02FFFFFFFFFFFLL);
	CONST_M128I(shuf_rb31, 0xFF0BFF0DFF08FF0ALL, 0xFFFFFFFFFF0EFFFFLL);
	CONST_M128I(shuf_rb32, 0xFFFFFFFFFFFFFFFFLL, 0xFF01FF03FFFFFF00LL);
	CONST_M128I(shuf_ag4, 0xFF08FFFFFF05FFFFLL, 0xFF0EFFFFFF0BFFFFLL);
	CONST_M128I(shuf_rb4, 0xFF07FF09FF04FF06LL, 0xFF0DFF0FFF0AFF0CLL);
	UNALIGNED_RGB24_INPUT_PREAMBLE;

	out_8_v16i_ag_rb_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_ag1));// PSHUFB	1	0.5
	// A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
	
	out_8_v16i_ag_rb_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_rb1));// PSHUFB	1	0.5
	// R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
	
	out_8_v16i_ag_rb_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_ag21));// PSHUFB	1	0.5
	// A5 0		G5 0	0 0		0 0		0 0		0 0		0 0		0 0
	out_8_v16i_ag_rb_vectors[2] = _mm_or_si128(out_8_v16i_ag_rb_vectors[2], _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_ag22)));
	// A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0			// POR		1	0.33
																				// PSHUFB	1	0.5
	
	out_8_v16i_ag_rb_vectors[3] = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_rb21));// PSHUFB	1	0.5
	// R5 0		B5 0	0 0		B6 0		0 0		0 0		0 0		0 0
	out_8_v16i_ag_rb_vectors[3] = _mm_or_si128(out_8_v16i_ag_rb_vectors[3], _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_rb22)));
	// R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0			// POR		1	0.33
																				// PSHUFB	1	0.5
	
	out_8_v16i_ag_rb_vectors[4] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_ag31));// PSHUFB	1	0.5
	// A9 0		G9 0	A10 0	G10 0	A11 0	G11 0	0 0		0 0
	out_8_v16i_ag_rb_vectors[4] = _mm_or_si128(out_8_v16i_ag_rb_vectors[4], _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_ag32)));
	// A9 0		G9 0	A10 0	G10 0	A1 0	G11 0	A12 0	G12 0			// POR		1	0.33
																				// PSHUFB	1	0.5
	
	out_8_v16i_ag_rb_vectors[5] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_rb31));// PSHUFB	1	0.5
	// R9 0		B9 0	R10 0	B10 0	R11 0	0 0		0 0		0 0
	out_8_v16i_ag_rb_vectors[5] = _mm_or_si128(out_8_v16i_ag_rb_vectors[5], _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_rb32)));
	// R9 0		B9 0	R10 0	B10 0	R11 0	B11 0	R12 0	B12 0			// POR		1	0.33
																				// PSHUFB	1	0.5
	
	out_8_v16i_ag_rb_vectors[6] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_ag4));// PSHUFB	1	0.5
	// A13 0	G13 0	A14 0	G14 0	A15 0	G15 0	A16 0	G16 0
	
	out_8_v16i_ag_rb_vectors[7] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_rb4));// PSHUFB	1	0.5
	// R13 0	B13 0	R14 0	B14 0	R15 0	B15 0	R16 0	B16 0
};


/*
 * Convert 2 vectors of 16 char BGR24 to 6 vectors of 8 short:
 *
 * Total latency:				x
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4	G4 R4	B5 G5	R5 B6
 *
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10 R10	B11 G11
 *
 * R11 B12	G12 R12	B13 G13	R13 B14	G14 R14	B15 G15	R15 B16 G16 R16
 *
 * OUTPUT:
 * 6 vectors of 8 short
 *
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * rVect
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 *
 * gVect
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 *
 * bVect
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 */
INLINE_NAME(unpack_bgr24_to_r_g_b_vectors_sse2, __m128i* in_3_v8i_bgr24_vectors, __m128i* out_6_v16i_r_g_b_vectors)
{
	__m128i		bgra_vectors[4];
	uint32_t	num_pixels = 16;
	uint8_t*	dest = (uint8_t *) bgra_vectors;
	uint8_t*	src = (uint8_t *) in_3_v8i_bgr24_vectors;

	bgra_vectors[0] = _mm_setzero_si128();
	bgra_vectors[1] = _mm_setzero_si128();
	bgra_vectors[2] = _mm_setzero_si128();
	bgra_vectors[3] = _mm_setzero_si128();

	// Unpack to BGRA first, then to GA / BR
	while (num_pixels-- > 0) {
		memcpy(dest, src, 3);
		dest +=4;
		src += 3;
	}
	
	unpack_bgra_to_r_g_b_vectors_sse2(&bgra_vectors[0], &out_6_v16i_r_g_b_vectors[0]);
	unpack_bgra_to_r_g_b_vectors_sse2(&bgra_vectors[2], &out_6_v16i_r_g_b_vectors[3]);
};


/*
 * Convert 3 vectors of 16 char BGR24 to 6 vectors of 8 short:
 *
 * Total latency:				18
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4	G4 R4	B5 G5	R5 B6
 *
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10 R10	B11 G11
 *
 * R11 B12	G12 R12	B13 G13	R13 B14	G14 R14	B15 G15	R15 B16 G16 R16
 *
 * OUTPUT:
 *
 * 6 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
INLINE_NAME(unpack_bgr24_to_r_g_b_vectors_sse2_ssse3, __m128i* input, __m128i* out_6_v16i_r_g_b_vectors)
{
	CONST_M128I(shuf_b1, 0xFF09FF06FF03FF00LL, 0xFFFFFFFFFF0FFF0CLL);
	CONST_M128I(shuf_b2, 0xFFFFFFFFFFFFFFFFLL, 0xFF05FF02FFFFFFFFLL);
	CONST_M128I(shuf_g1, 0xFF0AFF07FF04FF01LL, 0xFFFFFFFFFFFFFF0DLL);
	CONST_M128I(shuf_g2, 0xFFFFFFFFFFFFFFFFLL, 0xFF06FF03FF00FFFFLL);
	CONST_M128I(shuf_r1, 0xFF0BFF08FF05FF02LL, 0xFFFFFFFFFFFFFF0ELL);
	CONST_M128I(shuf_r2, 0xFFFFFFFFFFFFFFFFLL, 0xFF07FF04FF01FFFFLL);
	
	CONST_M128I(shuf_b3, 0xFFFFFF0EFF0BFF08LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_b4, 0xFF01FFFFFFFFFFFFLL, 0xFF0DFF0AFF07FF04LL);
	CONST_M128I(shuf_g3, 0xFFFFFF0FFF0CFF09LL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_g4, 0xFF02FFFFFFFFFFFFLL, 0xFF0EFF0BFF08FF05LL);
	CONST_M128I(shuf_r3, 0xFFFFFFFFFF0DFF0ALL, 0xFFFFFFFFFFFFFFFFLL);
	CONST_M128I(shuf_r4, 0xFF03FF00FFFFFFFFLL, 0xFF0FFF0CFF09FF06LL);
	
	UNALIGNED_RGB24_INPUT_PREAMBLE;
	M128I(scratch1, 0x0LL, 0x0LL);
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_r1));							// PSHUFB	1	0.5
	// R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	0 0		0 0
	
	out_6_v16i_r_g_b_vectors[0] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_r2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		0 0		0 0		R7 0	R8 0
	
	out_6_v16i_r_g_b_vectors[0] = _mm_or_si128(out_6_v16i_r_g_b_vectors[0], _M(scratch1));	// POR		1	0.33
	// R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_g1));							// PSHUFB	1	0.5
	// G1 0		G2 0	G3 0	G4 0	0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[1] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_g2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		G5 0	G6 0	G7 0	G8 0
	
	out_6_v16i_r_g_b_vectors[1] = _mm_or_si128(out_6_v16i_r_g_b_vectors[1], _M(scratch1));	// POR		1	0.33
	// G1  0	G2 0	G3  0	G4  0	G5  0	G6  0	G7  0	G8  0
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[0], _M(shuf_b1));							// PSHUFB	1	0.5
	// B1 0		B2 0	B3 0	B4 0	0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[2] = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_b2));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		0 0		B5 0	B6 0	B7 0	B8 0
	
	out_6_v16i_r_g_b_vectors[2] = _mm_or_si128(out_6_v16i_r_g_b_vectors[2], _M(scratch1));	// POR		1	0.33
	// B1  0	B2 0	B3  0	B4  0	B5  0	B6  0	B7  0	B8  0
	
	
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_r3));							// PSHUFB	1	0.5
	// R9 0		R10 0	R11 0	0 0		0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[3] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_r4));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		R12 0	R13 0	R14 0	R15 0	R16 0
	
	out_6_v16i_r_g_b_vectors[3] = _mm_or_si128(out_6_v16i_r_g_b_vectors[3], _M(scratch1));	// POR		1	0.33
	// R9  0	R10  0	R11 0	R12  0	R13  0	R14  0	R15  0	R16  0
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_g3));							// PSHUFB	1	0.5
	// G9 0		G10 0	G11 0	0 0		0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[4] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_g4));				// PSHUFB	1	0.5
	// 0 0		0 0		0 0		G12 0	G13 0	G14 0	G15 0	G16 0
	
	out_6_v16i_r_g_b_vectors[4] = _mm_or_si128(out_6_v16i_r_g_b_vectors[4], _M(scratch1));	// POR		1	0.33
	// G9  0	G10 0	G11  0	G12  0	G13  0	G14  0	G15  0	G16  0
	
	
	_M(scratch1) = _mm_shuffle_epi8(INPUT_VECT[1], _M(shuf_b3));							// PSHUFB	1	0.5
	// B9 0		B10 0	0 0		0 0		0 0		0 0		0 0		0 0
	
	out_6_v16i_r_g_b_vectors[5] = _mm_shuffle_epi8(INPUT_VECT[2], _M(shuf_b4));				// PSHUFB	1	0.5
	// 0 0		0 0		B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
	
	out_6_v16i_r_g_b_vectors[5] = _mm_or_si128(out_6_v16i_r_g_b_vectors[5], _M(scratch1));	// POR		1	0.33
	// B9  0	B10 0	B11  0	B12  0	B13  0	B14  0	B15  0	B16  0
};

