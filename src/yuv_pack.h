/*
 * yuv_pack.h
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

#include "common.h"
#include "debug_support.h"
#include "platform_util.h"

#ifdef __INTEL_CPU__

#include <emmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>

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
#undef M128_LOAD
#undef CALL_INLINE

#if GENERATE_UNALIGNED_INLINES == 1
	#define CALL_INLINE(fn, ...)					unaligned_ ## fn(__VA_ARGS__)
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void unaligned_ ## fn_suffix(__VA_ARGS__)
	#define M128_STORE(src, dst)					_mm_storeu_si128(&(dst), (src))
	#define M128_LOAD(src)							_mm_loadu_si128(&(src))
#else
	#define CALL_INLINE(fn, ...)					fn(__VA_ARGS__)
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void fn_suffix(__VA_ARGS__)
	#define M128_STORE(src, dst)					(dst) = (src)
	#define M128_LOAD(src)							(src)
#endif


/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to 2 vectors YUYV
 *
 * Total latency:			4 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 *
 * Y1 U12	Y2 V12		Y3 U34		Y4 V34		Y5 U56		Y6 V56		Y7 U78		Y8 V78
 * Y9 U910	Y10 V910	Y11 U1112	Y12 V1112	Y13 U1314	Y14 V1314	Y15 U1516	Y16 V1516
 *
 */
INLINE_NAME(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, __m128i* in_4_y_uv_422_vectors, __m128i* out_2_yuyv_vectors) {
	M128I(scratch1, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_packus_epi16(in_4_y_uv_422_vectors[0], in_4_y_uv_422_vectors[2]);// PACKUSWB		1	0.5
	// Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12	Y13 Y14	Y15 Y16

	_M(scratch2) = _mm_packus_epi16(in_4_y_uv_422_vectors[1], in_4_y_uv_422_vectors[3]);// PACKUSWB		1	0.5
	// U12 V12	U34 V34	U56 V56	U78 V78	U910 V910	U1112 V1112	U1314 V1314	U1516 V1516

	M128_STORE(_mm_unpacklo_epi8(_M(scratch1), _M(scratch2)), out_2_yuyv_vectors[0]);	// PUNPCKLBW	1	0.5

	M128_STORE(_mm_unpackhi_epi8(_M(scratch1), _M(scratch2)), out_2_yuyv_vectors[1]);	// PUNPCKHBW	1	0.5
}


/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to 2 vectors UYVY
 *
 * Total latency:			4 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 *
 * U12 Y1 	V12 Y2		U34 Y3 		V34 Y4		U56 Y5		V56 Y6		U78 Y7		V78 Y8
 * U910 Y9	V910 Y10	U1112 Y11	V1112 Y12	U1314 Y13	V1314 Y14	U1516 Y15	V1516 Y16
 *
 */
INLINE_NAME(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, __m128i* in_4_y_uv_422_vectors, __m128i* out_2_uyvy_vectors) {
	M128I(scratch1, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_packus_epi16(in_4_y_uv_422_vectors[0], in_4_y_uv_422_vectors[2]);// PACKUSWB		1	0.5
	// Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12	Y13 Y14	Y15 Y16

	_M(scratch2) = _mm_packus_epi16(in_4_y_uv_422_vectors[1], in_4_y_uv_422_vectors[3]);// PACKUSWB		1	0.5
	// U12 V12	U34 V34	U56 V56	U78 V78	U910 V910	U1112 V1112	U1314 V1314	U1516 V1516

	M128_STORE(_mm_unpacklo_epi8(_M(scratch2), _M(scratch1)), out_2_uyvy_vectors[0]);	// PUNPCKLBW	1	0.5

	M128_STORE(_mm_unpackhi_epi8(_M(scratch2), _M(scratch1)), out_2_uyvy_vectors[1]);	// PUNPCKHBW	1	0.5
}

/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to YUV422p - the lowest 4 bytes are
 * filled in the U and V planes
 *
 *
 * Total latency:			6 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 *
 * OUTPUT:
 *
 * 3 vectors of 16 char
 *
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 *
 * U1 U2	U3 U4	U5 U6	U7 U8	0 0		0 0		0 0		0 0
 *
 * V1 V2	V3 V4	V5 V6	V7 V8	0 0		0 0		0 0		0 0
 */
INLINE_NAME(pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2, __m128i* in_4_y_uv_422_vectors, __m128i* out_y_plane, __m128i* out_u_plane, __m128i* out_v_plane) {
	CONST_M128I(zero, 0x0LL, 0x0LL);
	CONST_M128I(mask_cr, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	M128I(scratch, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);

	M128_STORE(_mm_packus_epi16(in_4_y_uv_422_vectors[0], in_4_y_uv_422_vectors[2]), *out_y_plane);
	// Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12	Y13 Y14	Y15 Y16			// PACKUSWB		1	0.5

	_M(scratch) = _mm_packus_epi16(in_4_y_uv_422_vectors[1], in_4_y_uv_422_vectors[3]);
	// U1 V1 	U2 V2	U3 V3	U4 V4	U5 V5 	U6 V6	U7 V7	U8 V8			// PACKUSWB		1	0.5

	_M(scratch2) = _mm_and_si128(_M(scratch), _M(mask_cr));						// PAND			1	0.33
	// U1 0 	U2 0	U3 0	U4 0	U5 0 	U6 0	U7 0	U8 0

	M128_STORE(_mm_packus_epi16(_M(scratch2), _M(zero)), *out_u_plane);		// PACKUSWB		1	0.5
	// U1 U2	U3 U4 	U5 U6	U7 U8	0 0		0 0		0 0		0 0

	_M(scratch2) = _mm_srli_epi16(_M(scratch), 8);								// PSRLW		1	1
	// V1 0		V2 0	V3 0	V4 0	V5 0	V6 0	V7 0	V8 0

	M128_STORE(_mm_packus_epi16(_M(scratch2), _M(zero)), *out_v_plane);		// PACKUSWB		1	0.5
	// V1 V2	V3 V4 	V5 V6	V7 V8	0 0		0 0		0 0		0 0
}

/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to YUV422p - the highest 4 bytes are
 * filled in the U and V planes (lowest ones remain untouched)
 *
 *
 * Total latency:			6 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 *
 * OUTPUT:
 *
 * 3 vectors of 16 char
 *
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 *
 * x x		x x		x x		x x		U1 U2	U3 U4	U5 U6	U7 U8
 *
 * x x		x x		x x		x x		V1 V2	V3 V4	V5 V6	V7 V8
 *
 */
INLINE_NAME(pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2, __m128i* in_4_y_uv_422_vectors, __m128i* out_y_plane, __m128i* out_u_plane, __m128i* out_v_plane) {
	CONST_M128I(zero, 0x0LL, 0x0LL);
	CONST_M128I(mask_cr, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	M128I(scratch, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);

	M128_STORE(_mm_packus_epi16(in_4_y_uv_422_vectors[0], in_4_y_uv_422_vectors[2]), *out_y_plane);
	// Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12	Y13 Y14	Y15 Y16			// PACKUSWB		1	0.5

	_M(scratch) = _mm_packus_epi16(in_4_y_uv_422_vectors[1], in_4_y_uv_422_vectors[3]);
	// U1 V1 	U2 V2	U3 V3	U4 V4	U5 V5 	U6 V6	U7 V7	U8 V8			// PACKUSWB		1	0.5

	_M(scratch2) = _mm_and_si128(_M(scratch), _M(mask_cr));						// PAND			1	0.33
	// U1 0 	U2 0	U3 0	U4 0	U5 0 	U6 0	U7 0	U8 0

	_M(scratch2) = _mm_packus_epi16(_M(zero), _M(scratch2));					// PACKUSWB		1	0.5
	// 0 0		0 0		0 0		0 0		U1 U2	U3 U4 	U5 U6	U7 U8

	M128_STORE(_mm_or_si128(M128_LOAD(*out_u_plane), _M(scratch2)), *out_u_plane);
	// x x		x x		x x		x x		U1 U2	U3 U4	U5 U6	U7 U8			// POR			1	0.33

	_M(scratch2) = _mm_srli_epi16(_M(scratch), 8);								// PSRLW		1	1
	// V1 0		V2 0	V3 0	V4 0	V5 0	V6 0	V7 0	V8 0

	_M(scratch2) = _mm_packus_epi16(_M(zero), _M(scratch2));					// PACKUSWB		1	0.5
	// 0 0		0 0		0 0		0 0		V1 V2	V3 V4 	V5 V6	V7 V8

	M128_STORE(_mm_or_si128(M128_LOAD(*out_v_plane), _M(scratch2)), *out_v_plane);
	// x x		x x		x x		x x		V1 V2	V3 V4	V5 V6	V7 V8			// POR			1	0.33
}



/*
 * Pack 2 pairs of Y to one Y vector
 *
 *
 * Total latency:			1 cycle
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 2 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 *
 *
 * OUTPUT:
 *
 * 1 vector of 16 char
 *
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 *
 */
INLINE_NAME(pack_2_y_vectors_to_1_y_vector_sse2, __m128i* in_y_vector1, __m128i* in_y_vector2, __m128i* out_y_plane) {
	M128_STORE(_mm_packus_epi16(*in_y_vector1, *in_y_vector2), *out_y_plane);
	// Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12	Y13 Y14	Y15 Y16			// PACKUSWB		1	0.5
}


/*
 * Pack 4 UV vectors UV vectors to U plane and V plane
 *
 * Total latency:			8 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * uvVect1
 * U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4 0
 *
 * uvVect2
 * U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8 0
 *
 * uvVect3
 * U9 0		V9 0	U10 0	V10 0	U11 0	V11 0	U12 0	V12 0
 *
 * uvVect4
 * U13 0	V13 0	U14 0	V14 0	U15 0	V15 0	U16 0	V16 0
 *
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 *
 *
 * U1 U2	U3 U4	U5 U6	U7 U8	U9 U10	U11	U12	U13	U14	U15	U16
 *
 * V1 V2	V3 V4	V5 V6	V7 V8	V9 V10	V11 V12	V13	V14	V14	V16
 *
 */
INLINE_NAME(pack_4_uv_vectors_to_yup_vectors_sse2, __m128i* in_4_uv_vectors, __m128i* out_u_plane, __m128i* out_v_plane) {
	CONST_M128I(mask_v, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);
	M128I(scratch1, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);
	M128I(scratch3, 0x0LL, 0x0LL);
	M128I(scratch4, 0x0LL, 0x0LL);

	_M(scratch1) = _mm_packus_epi16(in_4_uv_vectors[0], in_4_uv_vectors[1]);
	// U1 V1 	U2 V2	U3 V3	U4 V4	U5 V5 	U6 V6	U7 V7	U8 V8			// PACKUSWB		1	0.5

	_M(scratch2) = _mm_packus_epi16(in_4_uv_vectors[2], in_4_uv_vectors[3]);
	// U9 V9 	U10 V10	U11 V11	U12 V12	U13 V13 U14 V14	U15 V15	U16 V16			// PACKUSWB		1	0.5

	_M(scratch3) = _mm_and_si128(_M(scratch1), _M(mask_v));						// PAND			1	0.33
	// U1 0 	U2 0	U3 0	U4 0	U5 0 	U6 0	U7 0	U8 0

	_M(scratch4) = _mm_and_si128(_M(scratch2), _M(mask_v));						// PAND			1	0.33
	// U9 0 	U10 0	U11 0	U12 0	U13 0 	U14 0	U15 0	U16 0

	M128_STORE(_mm_packus_epi16(_M(scratch3), _M(scratch4)), *out_u_plane);		// PACKUSWB		1	0.5
	// U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14 U15 U16

	_M(scratch3) = _mm_srli_epi16(_M(scratch1), 8);								// PSRLW		1	1
	// V1 0 	V2 0	V3 0	V4 0	V5 0 	V6 0	V7 0	V8 0

	_M(scratch4) = _mm_srli_epi16(_M(scratch2), 8);								// PSRLW		1	1
	// V9 0 	V10 0	V11 0	V12 0	V13 0 	V14 0	V15 0	V16 0

	M128_STORE(_mm_packus_epi16(_M(scratch3), _M(scratch4)), *out_v_plane);		// PACKUSWB		1	0.5
	// V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14 V15 V16
}


/*
 * Pack 2 Y, UV vectors into 1 v210 vector (6 pixels)
 *
 * Total latency:				12
 * Number of pixels handled:	16
 *
 * INPUT
 *
 * 2 vectors of 8 short
 * yVect
 * Y0		Y1		Y2		Y3		Y4		Y5		0		0
 *
 * uvVect
 * U01		V01		U23		V23		U45		V45		0		0
 *
 * OUTPUT:
 *
 * 1 vector of 6 v210 pixels
 * U01	Y0	V01	Y1	... U45	Y4	V45	Y5
 *
 *
 */
INLINE_NAME(pack_2_y_uv_vectors_to_1_v210_vector_sse2_ssse3_sse41, __m128i* input, __m128i* output) {
	CONST_M128I(max_value, 0x03FF03FF03FF03FFLL, 0x03FF03FF03FF03FFLL);
	CONST_M128I(shuffle_y, 0x050403020100FFFFLL, 0x0B0A09080706FFFFLL);
	CONST_M128I(shuffle_uv, 0x0504FFFF03020100LL, 0x0B0AFFFF09080706LL);
	CONST_M128I(blend_mask1, 0x00000000FFFFFFFFLL, 0x00000000FFFFFFFFLL);
	CONST_M128I(blend_mask2, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);	
	M128I(scratch, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);
	M128I(scratch3, 0x0LL, 0x0LL);
	
	// Clamp to 0 - 1023
	_M(scratch) = _mm_max_epi16(_mm_min_epi16(input[0], _M(max_value)), _M(scratch));//PMIN/PMAX	2	1
	_M(scratch2) = _mm_max_epi16(_mm_min_epi16(input[1], _M(max_value)), _M(scratch2));//PMIN/PMAX	2	1
	
	_M(scratch) = _mm_shuffle_epi8(_M(scratch), _M(shuffle_y));						//	PSHUFB		1	0.5
	//	0		Y0		Y1		Y2		0		Y3		Y4		Y5

	_M(scratch2) = _mm_shuffle_epi8(_M(scratch2), _M(shuffle_uv));					//	PSHUFB		1	0.5
	//	U01		V01		0		U23		V23		U45		0		V45
	
	//
	_M(scratch3) = _mm_blendv_epi8(_M(scratch), _M(scratch2), _M(blend_mask1));		// PBLENDVB		1	1	
	// U01		V01		Y1		Y2		V23		U45		Y4		Y5

	_M(scratch2) = _mm_blendv_epi8(_M(scratch2), _M(scratch), _M(blend_mask1));		// PBLENDVB		1	1
	// 0		Y0		0		U23		0		Y3		0		V45
	
	//
	// 0		V01		0		Y2		0		U45		0		Y5	
	_M(scratch) = _mm_slli_epi16(_M(scratch3), 4);									// PSLLW		1	1
	_M(scratch) = _mm_blendv_epi8(_M(scratch3), _M(scratch), _M(blend_mask2));		// PBLENDVB		1	1
	// (10-bit words)
	// U01	0	V01		Y1	0	Y2		V23	0	U45		Y4	0	Y5
	
	//
	_M(scratch2) = _mm_srli_epi32(_M(scratch2), 6);									// PSRLD        1	1
	M128_STORE(_mm_or_si128(_M(scratch), _M(scratch2)), *output);					// POR			1	0.33
	// (10-bit words)
	// U01	Y0	V01		Y1	U23	Y2		V23	Y3	U45		Y4	V45	Y5
}

/*
 * Pack 2 Y, UV vectors into 1 v210 vector (6 pixels)
 *
 * Total latency:				16
 * Number of pixels handled:	16
 *
 * INPUT
 *
 * 2 vectors of 8 short
 * yVect
 * Y0		Y1		Y2		Y3		Y4		Y5		0		0
 *
 * uvVect
 * U01		V01		U23		V23		U45		V45		0		0
 *
 * OUTPUT:
 *
 * 1 vector of 6 v210 pixels
 * U01	Y0	V01	Y1	... U45	Y4	V45	Y5
 *
 *
 */
INLINE_NAME(pack_2_y_uv_vectors_to_1_v210_vector_sse2_ssse3, __m128i* input, __m128i* output) {
	CONST_M128I(max_value, 0x03FF03FF03FF03FFLL, 0x03FF03FF03FF03FFLL);
	CONST_M128I(shuffle_11, 0xFFFF0302FFFFFFFFLL, 0xFFFF0908FFFFFFFFLL);
	CONST_M128I(shuffle_12, 0xFFFFFFFFFFFF0100LL, 0xFFFFFFFFFFFF0706LL);
	CONST_M128I(shuffle_21, 0x0504FFFFFFFFFFFFLL, 0x0B0AFFFFFFFFFFFFLL);
	CONST_M128I(shuffle_22, 0xFFFFFFFF0302FFFFLL, 0xFFFFFFFF0908FFFFLL);
	CONST_M128I(shuffle_31, 0xFFFFFFFF0100FFFFLL, 0xFFFFFFFF0706FFFFLL);
	CONST_M128I(shuffle_32, 0x0504FFFFFFFFFFFFLL, 0x0B0AFFFFFFFFFFFFLL);
	M128I(scratch, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);
	M128I(scratch3, 0x0LL, 0x0LL);
	
	// Clamp to 0 - 1023
	input[0] = _mm_max_epi16(_mm_min_epi16(input[0], _M(max_value)), _M(scratch));//PMIN/PMAX	2	1
	input[1] = _mm_max_epi16(_mm_min_epi16(input[1], _M(max_value)), _M(scratch2));//PMIN/PMAX	2	1
	
	//
	_M(scratch) = _mm_shuffle_epi8(input[0], _M(shuffle_11));		//	PSHUFB		1	0.5
	//	0		0		Y1		0		0		0		Y4		0
	
	_M(scratch2) = _mm_shuffle_epi8(input[1], _M(shuffle_12));		//	PSHUFB		1	0.5
	//	U01		0		0		0		V23		0		0		0

	_M(scratch3) = _mm_or_si128(_M(scratch), _M(scratch2));			//	POR			1	0.33
	//	U01		0		Y1		0		V23		0		Y4		0
	
	//
	_M(scratch) = _mm_shuffle_epi8(input[0], _M(shuffle_21));		//	PSHUFB		1	0.5
	//	0		0		0		Y2		0		0		0		Y5
	
	_M(scratch2) = _mm_shuffle_epi8(input[1], _M(shuffle_22));		//	PSHUFB		1	0.5
	//	0		V01		0		0		0		U45		0		0
	
	_M(scratch) = _mm_or_si128(_M(scratch), _M(scratch2));			//	POR			1	0.33
	_M(scratch) = _mm_slli_epi16(_M(scratch), 4);					//	PSLLW		1	1
	//	0		V01		0		Y2		0		U45		0		Y5
	
	_M(scratch3) = _mm_or_si128(_M(scratch3), _M(scratch));			//	POR			1	0.33
	// (10-bit words)
	// U01	0	V01		Y1	0	Y2		V23	0	U45		Y4	0	Y5
	
	
	//
	_M(scratch) = _mm_shuffle_epi8(input[0], _M(shuffle_31));		//	PSHUFB		1	0.5
	//	0		Y0		0		0		0		Y3		0		0
	
	_M(scratch2) = _mm_shuffle_epi8(input[1], _M(shuffle_32));		//	PSHUFB		1	0.5
	//	0		0		0		U23		0		0		0		V45
	
	_M(scratch2) = _mm_or_si128(_M(scratch), _M(scratch2));			//	POR			1	0.33
	// 0		Y0		0		U23		0		Y3		0		V45
	
	_M(scratch2) = _mm_srli_epi32(_M(scratch2), 6);					// PSRLD        1	1
	M128_STORE(_mm_or_si128(_M(scratch3), _M(scratch2)), *output);	// POR			1	0.33
	// (10-bit words)
	// U01	Y0	V01		Y1	U23	Y2		V23	Y3	U45		Y4	V45	Y5
}


#undef DEFINE_PACK_4_V210_VECTORS
#define DEFINE_PACK_4_V210_VECTORS(instr_set) \
INLINE_NAME(pack_6_y_uv_vectors_to_4_v210_vectors_ ## instr_set, __m128i* input, __m128i* output) {\
	__m128i scratch[2];\
	CALL_INLINE(pack_2_y_uv_vectors_to_1_v210_vector_ ## instr_set, input, output);\
	scratch[0] = _mm_alignr_epi8(input[2], input[0], 12);		/*	PALIGNR		1	1 */\
	scratch[1] = _mm_alignr_epi8(input[3], input[1], 12);		/*	PALIGNR		1	1 */\
	CALL_INLINE(pack_2_y_uv_vectors_to_1_v210_vector_ ## instr_set, scratch, &output[1]);\
	scratch[0] = _mm_alignr_epi8(input[4], input[2], 8);		/*	PALIGNR		1	1 */\
	scratch[1] = _mm_alignr_epi8(input[5], input[3], 8);		/*	PALIGNR		1	1 */\
	CALL_INLINE(pack_2_y_uv_vectors_to_1_v210_vector_ ## instr_set, scratch, &output[2]);\
	scratch[0] = _mm_srli_si128(input[4], 4);					/*	PSRLDQ		1	0.5 */\
	scratch[1] = _mm_srli_si128(input[5], 4);					/*	PSRLDQ		1	0.5 */\
	CALL_INLINE(pack_2_y_uv_vectors_to_1_v210_vector_ ## instr_set, scratch, &output[3]);\
}

/*
 * Pack 6 vectors (Y, UV) into 4 v210 vectors (24 pixels) 
 *
 * Total latency:				18 (sse41) / 22 (ssse3)
 * Number of pixels handled:	24
 *
 * INPUT
 * 6 vectors of 8 short
 * yVect1
 * Y0		Y1		Y2		Y3		Y4		Y5		Y6		Y7
 *
 * uvVect1
 * U01		V01		U23		V23		U45		V45		U67		V67
 *
 * yVect2
 * Y8		Y9		Y10		Y11		Y12		Y13		Y14		Y15
 *
 * uvVect2
 * U89		V89		U1011	V1011	U1213	V1213	U1415	V1516
 *
 * yVect3
 * Y16		Y17		Y18		Y19		Y20		Y21		Y22		Y23
 *
 * uvVect3
 * U1617	V1617	U1819	V1819	U2021	V2021	U2223	V2223
 *
 *
 * OUTPUT:
 * 4 vectors of 24 v210 pixels
 * U01		Y0		V01		Y1	... U45	  Y4	V45	    Y5
 * U67		Y6		V67		Y7	... U1011 Y10	V1011	Y11
 * U1213	Y12		V1213	Y13	... U1617 Y16	V1617	Y17
 * U1819	Y18		V1819	Y19	... U2223 Y22	V2223	Y23 
 *
 *
 */
DEFINE_PACK_4_V210_VECTORS(sse2_ssse3_sse41);
DEFINE_PACK_4_V210_VECTORS(sse2_ssse3);


#endif	// __INTEL_CPU__

