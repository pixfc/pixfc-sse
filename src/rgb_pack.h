/*
 * rgb_pack.h
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
#include <stdint.h>
#include <string.h>

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
#undef M128_STORE

#if GENERATE_UNALIGNED_INLINES == 1
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void unaligned_ ## fn_suffix(__VA_ARGS__)
	#define M128_STORE(src, dst)					_mm_storeu_si128(&(dst), (src))
#else
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void fn_suffix(__VA_ARGS__)
	#define M128_STORE(src, dst)					(dst) = (src)
#endif


/* 
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 4 BGRA vectors
 *
 * Total latency:			11 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 *
 * OUTPUT:
 *
 * B1 G1	R1 A1	B2 G2	R2 A2	B3 G3	R3 A3	B4 G4	R4 A4
 * B5 G5	R5 A5	B6 G6	R6 A6	B7 G7	R7 A7	B8 G8	R8 A8
 * B9 G9	R9 A9	B10 G10	R10 A10	B11 G11	R11 A11	B12 G12	R12 A12
 * B13 G13	R13 A13	B14 G14	R14 A14	B15 G15	R15 A15	B16 G16	R16 A16
 */
INLINE_NAME(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, __m128i* in_6_r_g_b_vectors, __m128i* out_4_bgra_vectors) {
	__m128i* r1 = &in_6_r_g_b_vectors[0];
	__m128i* g1 = &in_6_r_g_b_vectors[1];
	__m128i* b1 = &in_6_r_g_b_vectors[2];
	__m128i* r2 = &in_6_r_g_b_vectors[3];
	__m128i* g2 = &in_6_r_g_b_vectors[4];
	__m128i* b2 = &in_6_r_g_b_vectors[5];
	__m128i zero = _mm_setzero_si128();

	__m128i rPacked = _mm_packus_epi16(*r1, *r2);						// PACKUSWB		1	0.5
	// R1 R2	R3 R4	R5 R6	R7 R8	R9 R10	R11 R12	R13 R14	R15 R16

	__m128i gPacked = _mm_packus_epi16(*g1, *g2);						// PACKUSWB		1	0.5
	// G1 G2	G3 G4	G5 G6	G7 G8	G9 G10	G11 G12	G13 G14	G15 G16

	__m128i bPacked = _mm_packus_epi16(*b1, *b2);						// PACKUSWB		1	0.5
	// B1 B2	B3 B4	B5 B6	B7 B8	B9 B10	B11 B12	B13 B14	B15 B16


	// unpacking lows
	__m128i bg = _mm_unpacklo_epi8(bPacked, gPacked);					// PUNPCKLBW	1	0.5
	// B1 G1	B2 G2	B3 G3	B4 G4	B5 G5	B6 G6	B7 G7	B8 G8

	__m128i ra = _mm_unpacklo_epi8(rPacked, zero);						// PUNPCKLBW	1	0.5
	// R1 A1	R2 A2	R3 A3	R4 A4	R5 A5	R6 A6	R7 A7	R8 A8

	M128_STORE(_mm_unpacklo_epi16(bg, ra), out_4_bgra_vectors[0]);		// PUNPCKLWD	1	0.5
	// B1 G1	R1 A1	B2 G2	R2 A2	B3 G3	R3 A3	B4 G4	R4 A4

	M128_STORE(_mm_unpackhi_epi16(bg, ra), out_4_bgra_vectors[1]);		// PUNPCKHWD	1	0.5
	// B5 G5	R5 A5	B6 G6	R6 A6	B7 G7	R7 A7	B8 G8	R8 A8

	// unpacking highs
	bg = _mm_unpackhi_epi8(bPacked, gPacked);							// PUNPCKHBW	1	0.5
	// B9 G9	B10 G10	B11 G11	B12 G12 B13 G13	B14 G14	B15 G15	B16 G16

	ra = _mm_unpackhi_epi8(rPacked, zero);								// PUNPCKHBW	1	0.5
	// R9 A9	R10 A10	R11 A11	R12 A12	R13 A13	R14 A14	R15 A15	R16 A16

	M128_STORE(_mm_unpacklo_epi16(bg, ra), out_4_bgra_vectors[2]);		// PUNPCKLWD	1	0.5
	// B9 G9	R9 A9	B10 G10	R10 A10	B11 G11	R11 A11	B12 G12	R12 A12

	M128_STORE(_mm_unpackhi_epi16(bg, ra), out_4_bgra_vectors[3]);		// PUNPCKHWD	1	0.5
	// B13 G13	R13 A13	B14 G14	R14 A14	B15 G15	R15 A15	B16 G16	R16 A16
};


/*
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 4 ARGB vectors
 *
 * Total latency:			11 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 *
 *
 * OUTPUT
 *
 * A1 R1	G1 B1	A2 R2	G2 B2	A3 R3	G3 B3	A4 R4	G4 B4
 * A5 R5	G5 B5	A6 R6	G6 B6	A7 R7	G7 B7	A8 R8	G8 B8
 * A9 R9	G9 B9	A10 R10	G10 B10	A11 R11	G11 B11	A12 R12	G12 B12
 * A13 R13	G13 B13	A14 R14	G14 B14	A15 R15	G15 B15	A16 R16	G16 B16
 *
 */
INLINE_NAME(pack_6_rgb_vectors_in_4_argb_vectors_sse2, __m128i* in_6_r_g_b_vectors, __m128i* out_4_argb_vectors) {
	__m128i* r1 = &in_6_r_g_b_vectors[0];
	__m128i* g1 = &in_6_r_g_b_vectors[1];
	__m128i* b1 = &in_6_r_g_b_vectors[2];
	__m128i* r2 = &in_6_r_g_b_vectors[3];
	__m128i* g2 = &in_6_r_g_b_vectors[4];
	__m128i* b2 = &in_6_r_g_b_vectors[5];
	__m128i zero = _mm_setzero_si128();

	__m128i rPacked = _mm_packus_epi16(*r1, *r2);						// PACKUSWB		1	0.5
	// R1 R2	R3 R4	R5 R6	R7 R8	R9 R10	R11 R12	R13 R14	R15 R16

	__m128i gPacked = _mm_packus_epi16(*g1, *g2);						// PACKUSWB		1	0.5
	// G1 G2	G3 G4	G5 G6	G7 G8	G9 G10	G11 G12	G13 G14	G15 G16

	__m128i bPacked = _mm_packus_epi16(*b1, *b2);						// PACKUSWB		1	0.5
	// B1 B2	B3 B4	B5 B6	B7 B8	B9 B10	B11 B12	B13 B14	B15 B16

	// unpacking lows
	__m128i ar = _mm_unpacklo_epi8(zero, rPacked);						// PUNPCKLBW	1	0.5
	// A1 R1	A2 R2	A3 R3	A4 R4	A5 R5	A6 R6	A7 R7	A8 R8

	__m128i gb = _mm_unpacklo_epi8(gPacked, bPacked);					// PUNPCKLBW	1	0.5
	// G1 B1	G2 B2	G3 B3	G4 B4	G5 B5	G6 B6	G7 B7	G8 B8

	M128_STORE(_mm_unpacklo_epi16(ar, gb), out_4_argb_vectors[0]);		// PUNPCKLWD	1	0.5
	// A1 R1	G1 B1	A2 R2	G2 B2	A3 R3	G3 B3	A4 R4	G4 B4

	M128_STORE(_mm_unpackhi_epi16(ar, gb), out_4_argb_vectors[1]);		// PUNPCKHWD	1	0.5
	// A5 R5	G5 B5	A6 R6	G6 B6	A7 R7	G7 B7	A8 R8	G8 B8

	// unpacking highs
	ar = _mm_unpackhi_epi8(zero, rPacked);								// PUNPCKHBW	1	0.5
	// A9 R9	A10 R10	A11 R11	A12 R12	A13 R13	A14 R14	A15 R15	A16 R16

	gb = _mm_unpackhi_epi8(gPacked, bPacked);							// PUNPCKHBW	1	0.5
	// G9 B9	G10 B10	G11 B11	G12 B12	G13 B13	G14 B14	G15 B15	G16 B16

	M128_STORE(_mm_unpacklo_epi16(ar, gb), out_4_argb_vectors[2]);		// PUNPCKLWD	1	0.5
	// A9 R9	G9 B9	A10 R10	G10 B10	A11 R11	G11 B11	A12 R12 G12 B12

	M128_STORE(_mm_unpackhi_epi16(ar, gb), out_4_argb_vectors[3]);		// PUNPCKHWD	1	0.5
	// A13 R13	G13 B13	A14 R14 G14 B14	A15 R15	G15 B15	A16 R16	G16 B16
};



/*
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 3 RGB24 vectors of 16 char
 *
 * Total latency:			18 cycles
 * Num of pixel handled:	16 pixels
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * rVect1 - 2
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 *
 * gVect1 - 2
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 *
 * bVect1 - 2
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 *
 * OUTPUT
 *
 * 3 vectors of 16 chars
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4 	G4 B4	R5 G5	B5 R6
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10B10	R11G11
 * B11R12	G12B12	R13G13	B13R14	G14B14	R15G15	B15R16	G16B16
 */
INLINE_NAME(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, __m128i* in_6_r_g_b_vectors, __m128i* out_3_rgb24_vectors) {
	CONST_M128I(mask_r1, 0xFF02FFFF01FFFF00LL, 0x05FFFF04FFFF03FFLL);
	CONST_M128I(mask_g1, 0x02FFFF01FFFF00FFLL, 0xFFFF04FFFF03FFFFLL);
	CONST_M128I(mask_b1, 0xFFFF01FFFF00FFFFLL, 0xFF04FFFF03FFFF02LL);

	CONST_M128I(mask_g2, 0xFF07FFFF06FFFF05LL, 0x0AFFFF09FFFF08FFLL);
	CONST_M128I(mask_b2, 0x07FFFF06FFFF05FFLL, 0xFFFF09FFFF08FFFFLL);
	CONST_M128I(mask_r2, 0xFFFF07FFFF06FFFFLL, 0xFF0AFFFF09FFFF08LL);

	CONST_M128I(mask_b3, 0xFF0CFFFF0BFFFF0ALL, 0x0FFFFF0EFFFF0DFFLL);
	CONST_M128I(mask_r3, 0x0DFFFF0CFFFF0BFFLL, 0xFFFF0FFFFF0EFFFFLL);
	CONST_M128I(mask_g3, 0xFFFF0CFFFF0BFFFFLL, 0xFF0FFFFF0EFFFF0DLL);

	M128I(scratch, 0x0LL, 0x0LL);

	//
	// Saturate and concatenate R1 & R2, G1 & G2, B1 & B2.
	__m128i rPacked = _mm_packus_epi16(in_6_r_g_b_vectors[0], in_6_r_g_b_vectors[3]);
	// R1 R2	R3 R4	R5 R6	R7 R8	R9 R10	R11 R12	R13 R14	R15 R16	// PACKUSWB		1	0.5

	__m128i gPacked = _mm_packus_epi16(in_6_r_g_b_vectors[1], in_6_r_g_b_vectors[4]);
	// G1 G2	G3 G4	G5 G6	G7 G8	G9 G10	G11 G12	G13 G14	G15 G16	// PACKUSWB		1	0.5

	__m128i bPacked = _mm_packus_epi16(in_6_r_g_b_vectors[2], in_6_r_g_b_vectors[5]);
	// B1 B2	B3 B4	B5 B6	B7 B8	B9 B10	B11 B12	B13 B14	B15 B16 // PACKUSWB		1	0.5

	//
	// First output vector
	// R1 G1	B1 R2	G2 B2	R3 G3	B3 R4 	G4 B4	R5 G5	B5 R6

	_M(scratch) = _mm_shuffle_epi8(rPacked, _M(mask_r1));
	// R1 0		0 R2	0 0		R3 0	0 R4	0 0		R5 0	0 R6	// PSHUFB		1	0.5

																		// PSHUFB		1	0.5
	_M(scratch) = _mm_or_si128(_M(scratch), _mm_shuffle_epi8(gPacked, _M(mask_g1)));
	// 0 G1		0 0		G2 0	0 G3	0 0		G4 0	0 G5	0 0		// POR			1	0.33

																		// PSHUFB		1	0.5
	M128_STORE(_mm_or_si128(_M(scratch), _mm_shuffle_epi8(bPacked, _M(mask_b1))), out_3_rgb24_vectors[0]);
	// 0 0		B1 0	0 B2	0 0		B3 0	0 B4	0 0		B5 0	// POR			1	0.33


	//
	// second output vector
	// G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10B10	R11G11
	_M(scratch) = _mm_shuffle_epi8(gPacked, _M(mask_g2));
	// G6 0		0 G7	0 0		G8 0	0 G9	0 0		G10 0	0 G11	// PSHUFB		1	0.5

																		// PSHUFB		1	0.5
	_M(scratch) = _mm_or_si128(_M(scratch), _mm_shuffle_epi8(bPacked, _M(mask_b2)));
	// 0 B6		0 0		B7 0	0 B8	0 0		B9 0	0 B10	0 0 	// POR			1	0.33

																		// PSHUFB		1	0.5
	M128_STORE(_mm_or_si128(_M(scratch), _mm_shuffle_epi8(rPacked, _M(mask_r2))), out_3_rgb24_vectors[1]);
	// 0 0		R7 0	0 R8	0 0		R9 0	0 R10	0 0		R11 0 	// POR			1	0.33

	//
	// Third output vector
	// B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16	G16 B16
	_M(scratch) = _mm_shuffle_epi8(bPacked, _M(mask_b3));
	// B11 0	0 B12	0 0		B13 0	0 B14	0 0		B15 0	0 B16	// PSHUFB		1	0.5

																		// PSHUFB		1	0.5
	_M(scratch) = _mm_or_si128(_M(scratch), _mm_shuffle_epi8(rPacked, _M(mask_r3)));
	// 0 R12	0 0		R13 0	0 R14	0 0		R15 0	0 R16	0 0		// POR			1	0.33

																		// PSHUFB		1	0.5
	M128_STORE(_mm_or_si128(_M(scratch), _mm_shuffle_epi8(gPacked, _M(mask_g3))), out_3_rgb24_vectors[2]);
	// 0 0		G12 0	0 G13	0 0		G14 0	0 G15	0 0		G16 0	// POR			1	0.33
}

/*
 * Pack 4 ARGB vectors of 16 char in 3 RGB vectors of 16 chars
 *
 * INPUT:
 *
 * 4 vectors of 16 chars:
 *
 * A1 R1	G1 B1	A2 R2	G2 B2	A3 R3	G3 B3	A4 R4	G4 B4
 * A5 R5	G5 B5	A6 R6	G6 B6	A7 R7	G7 B7	A8 R8	G8 B8
 * A9 R9	G9 B9	A10 R10	G10 B10	A11 R11	G11 B11	A12 R12	G12 B12
 * A13 R13	G13 B13	A14 R14	G14 B14	A15 R15	G15 B15	A16 R16	G16 B16
 *
 *
 *
 * OUTPUT
 *
 * 3 vectors of 16 chars
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4 	G4 B4	R5 G5	B5 R6
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10B10	R11G11
 * B11R12	G12B12	R13G13	B13R14	G14B14	R15G15	B15R16	G16B16
 */
INLINE_NAME(repack_4_argb_vectors_to_3_rgb24_vectors_nonsse, __m128i* src, __m128i* dst){
	uint8_t*		argb_buffer = ((uint8_t *) src) + 1;	// skip 1st alpha byte
	uint8_t*		rgb_buffer = (uint8_t *) dst;
	uint32_t		num_pixels = 16;

	// TODO: improve me
	// If anyone has a better way of doing this, I am all ear.
	while (num_pixels-- > 0) {
		memcpy(rgb_buffer, argb_buffer, 3);

		argb_buffer += 4;
		rgb_buffer += 3;
	}
}

INLINE_NAME(pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, __m128i* in_6_r_g_b_vectors, __m128i* out_3_rgb24_vectors) {
	__m128i			pack_out[4];

	// This is not as inefficient as it may seem, as shown in unit testing.
	// We first pack in 4 ARGB vectors, and then repack that into RGB24.

	// pack both sets of 8 pixels
	pack_6_rgb_vectors_in_4_argb_vectors_sse2(in_6_r_g_b_vectors, pack_out);

	// repack ARGB into RGB24
	repack_4_argb_vectors_to_3_rgb24_vectors_nonsse(pack_out, out_3_rgb24_vectors);
}




/*
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 3 BGR24 vectors of 16 char
 *
 * Total latency:			18 cycles
 * Num of pixel handled:	16 pixels
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * rVect1 - 2
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 *
 * gVect1 - 2
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 *
 * bVect1 - 2
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 *
 * OUTPUT
 *
 * 3 vectors of 16 chars
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4 	G4 R4	B5 G5	R5 B6
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10R10	B11G11
 * R11B12	G12R12	B13G13	R13B14	G14R14	B15G15	R15B16	G16R16
 */
INLINE_NAME(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, __m128i* in_6_r_g_b_vectors, __m128i* out_3_bgr24_vectors) {
	CONST_M128I(mask_b1, 0xFF02FFFF01FFFF00LL, 0x05FFFF04FFFF03FFLL);
	CONST_M128I(mask_g1, 0x02FFFF01FFFF00FFLL, 0xFFFF04FFFF03FFFFLL);
	CONST_M128I(mask_r1, 0xFFFF01FFFF00FFFFLL, 0xFF04FFFF03FFFF02LL);

	CONST_M128I(mask_g2, 0xFF07FFFF06FFFF05LL, 0x0AFFFF09FFFF08FFLL);
	CONST_M128I(mask_r2, 0x07FFFF06FFFF05FFLL, 0xFFFF09FFFF08FFFFLL);
	CONST_M128I(mask_b2, 0xFFFF07FFFF06FFFFLL, 0xFF0AFFFF09FFFF08LL);

	CONST_M128I(mask_r3, 0xFF0CFFFF0BFFFF0ALL, 0x0FFFFF0EFFFF0DFFLL);
	CONST_M128I(mask_b3, 0x0DFFFF0CFFFF0BFFLL, 0xFFFF0FFFFF0EFFFFLL);
	CONST_M128I(mask_g3, 0xFFFF0CFFFF0BFFFFLL, 0xFF0FFFFF0EFFFF0DLL);

	M128I(scratch, 0x0LL, 0x0LL);

	//
	// Saturate and concatenate R1 & R2, G1 & G2, B1 & B2.
	__m128i rPacked = _mm_packus_epi16(in_6_r_g_b_vectors[0], in_6_r_g_b_vectors[3]);
	// R1 R2	R3 R4	R5 R6	R7 R8	R9 R10	R11 R12	R13 R14	R15 R16	// PACKUSWB		1	0.5

	__m128i gPacked = _mm_packus_epi16(in_6_r_g_b_vectors[1], in_6_r_g_b_vectors[4]);
	// G1 G2	G3 G4	G5 G6	G7 G8	G9 G10	G11 G12	G13 G14	G15 G16	// PACKUSWB		1	0.5

	__m128i bPacked = _mm_packus_epi16(in_6_r_g_b_vectors[2], in_6_r_g_b_vectors[5]);
	// B1 B2	B3 B4	B5 B6	B7 B8	B9 B10	B11 B12	B13 B14	B15 B16 // PACKUSWB		1	0.5

	//
	// First output vector
	// B1 G1	R1 B2	G2 R2	B3 G3	R3 B4 	G4 R4	B5 G5	R5 B6
	_M(scratch) = _mm_shuffle_epi8(bPacked, _M(mask_b1));
	// B1 0		0 B2	0 0		B3 0	0 B4	0 0		B5 0	0 B6	// PSHUFB		1	0.5

																		// PSHUFB		1	0.5
	_M(scratch) = _mm_or_si128(_M(scratch), _mm_shuffle_epi8(gPacked, _M(mask_g1)));
	// 0 G1		0 0		G2 0	0 G3	0 0		G4 0	0 G5	0 0		// POR			1	0.33

																		// PSHUFB		1	0.5
	M128_STORE(_mm_or_si128(_M(scratch), _mm_shuffle_epi8(rPacked, _M(mask_r1))), out_3_bgr24_vectors[0]);
	// 0 0		R1 0	0 R2	0 0		R3 0	0 R4	0 0		R5 0	// POR			1	0.33


	//
	// second output vector
	// G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10R10	B11G11
	_M(scratch) = _mm_shuffle_epi8(gPacked, _M(mask_g2));
	// G6 0		0 G7	0 0		G8 0	0 G9	0 0		G10 0	0 G11	// PSHUFB		1	0.5

																		// PSHUFB		1	0.5
	_M(scratch) = _mm_or_si128(_M(scratch), _mm_shuffle_epi8(rPacked, _M(mask_r2)));
	// 0 R6		0 0		R7 0	0 R8	0 0		R9 0	0 R10	0 0 	// POR			1	0.33

																		// PSHUFB		1	0.5
	M128_STORE(_mm_or_si128(_M(scratch), _mm_shuffle_epi8(bPacked, _M(mask_b2))), out_3_bgr24_vectors[1]);
	// 0 0		B7 0	0 B8	0 0		B9 0	0 B10	0 0		B11 0 	// POR			1	0.33


	//
	// Third output vector
	// R11 B12	G12 R12	B13 G13	R13 B14	G14 R14	B15 G15	R15 B16	G16 R16
	_M(scratch) = _mm_shuffle_epi8(rPacked, _M(mask_r3));
	// R11 0	0 R12	0 0		R13 0	0 R14	0 0		R15 0	0 R16	// PSHUFB		1	0.5

																		// PSHUFB		1	0.5
	_M(scratch) = _mm_or_si128(_M(scratch), _mm_shuffle_epi8(bPacked, _M(mask_b3)));
	// 0 B12	0 0		B13 0	0 B14	0 0		B15 0	0 B16	0 0		// POR			1	0.33

																		// PSHUFB		1	0.5
	M128_STORE(_mm_or_si128(_M(scratch), _mm_shuffle_epi8(gPacked, _M(mask_g3))), out_3_bgr24_vectors[2]);
	// 0 0		G12 0	0 G13	0 0		G14 0	0 G15	0 0		G16 0	// POR			1	0.33
}

/*
 * REPack 4 BGRA vectors of 16 char in 48 BGR chars
 *
 * INPUT:
 *
 * 4 vectors of 16 chars:
 *
 * B1 G1	R1 A1	B2 G2	R2 A2	B3 G3	R3 A3	B4 G4	R4 A4
 * B5 G5	R5 A5	B6 G6	R6 A6	B7 G7	R7 A7	B8 G8	R8 A8
 * B9 G9	R9 A9	B10 G10	R10 A10	B11 G11	R11 A11	B12 G12	R12 A12
 * B13 G13	R13 A13	B14 G14	R14 A14	B15 G15	R15 A15	B16 G16	R16 A16
 *
 *
 *
 * OUTPUT
 *
 * 3 vectors of 16 chars:
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4 	G4 R4	B5 G5	R5 B6
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10R10	B11G11
 * R11B12	G12R12	B13G13	R13B14	G14R14	B15G15	R15B16	G16R16
 */
INLINE_NAME(repack_4_bgra_vectors_to_3_bgr24_vectors_nonsse, __m128i * src, __m128i * dst){
	uint8_t *		bgra_buffer = (uint8_t *) src;
	uint8_t *		bgr_buffer = (uint8_t *) dst;
	uint32_t	num_pixels = 16;

	// TODO: improve me
	// If anyone has a better way of doing this, I am all ear.
	while (num_pixels-- > 0) {
		memcpy(bgr_buffer, bgra_buffer, 3);

		bgra_buffer += 4;
		bgr_buffer += 3;
	}
}

INLINE_NAME(pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, __m128i* in_6_r_g_b_vectors, __m128i* out_3_bgr24_vectors) {
	__m128i			pack_out[4];

	// This is not as inefficient as it may seem, as shown in unit testing.
	// We first pack in 4 BGRA vectors, and then repack that into BGR24.

	pack_6_rgb_vectors_in_4_bgra_vectors_sse2(in_6_r_g_b_vectors, pack_out);

	// repack ARGB into RGB24
	repack_4_bgra_vectors_to_3_bgr24_vectors_nonsse(pack_out, out_3_bgr24_vectors);

}

