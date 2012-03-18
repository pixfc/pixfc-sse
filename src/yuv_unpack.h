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

#include "debug_support.h"
#include "common.h"
#include "platform_util.h"

#ifdef __INTEL_CPU__

#include <emmintrin.h>
#include <tmmintrin.h>



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
#undef UNALIGNED_YUV422I_INPUT_PREAMBLE
#undef INPUT_VECT
#undef UNALIGNED_YUVP_Y_INPUT_PREAMBLE
#undef UNALIGNED_YUVP_UV_INPUT_PREAMBLE
#undef YUVP_Y_INPUT_VECT
#undef YUVP_U_INPUT_VECT
#undef YUVP_V_INPUT_VECT


//#define GENERATE_UNALIGNED_INLINES 1
#if GENERATE_UNALIGNED_INLINES == 1
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void unaligned_ ## fn_suffix(__VA_ARGS__)

	// Interleaved YUV
	#define UNALIGNED_YUV422I_INPUT_PREAMBLE		DECLARE_VECT_N_UNALIGN_LOAD(aligned_vector, input);
	#define INPUT_VECT								(&aligned_vector)

	// Planar YUV
	#define UNALIGNED_YUVP_Y_INPUT_PREAMBLE			DECLARE_VECT_N_UNALIGN_LOAD(aligned_y_vector, y_input)
	#define UNALIGNED_YUVP_UV_INPUT_PREAMBLE		DECLARE_VECT_N_UNALIGN_LOAD(aligned_u_vector, u_input); DECLARE_VECT_N_UNALIGN_LOAD(aligned_v_vector, v_input)
	#define YUVP_Y_INPUT_VECT						(&aligned_y_vector)
	#define YUVP_U_INPUT_VECT						(&aligned_u_vector)
	#define YUVP_V_INPUT_VECT						(&aligned_v_vector)

#else
	#define INLINE_NAME(fn_suffix, ...)				EXTERN_INLINE void fn_suffix(__VA_ARGS__)

	// Interleaved YUV
	#define UNALIGNED_YUV422I_INPUT_PREAMBLE
	#define INPUT_VECT								input

	// Planar YUV
	#define UNALIGNED_YUVP_Y_INPUT_PREAMBLE
	#define UNALIGNED_YUVP_UV_INPUT_PREAMBLE
	#define YUVP_Y_INPUT_VECT						(y_input)
	#define YUVP_U_INPUT_VECT						(u_input)
	#define YUVP_V_INPUT_VECT						(v_input)
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
	UNALIGNED_YUV422I_INPUT_PREAMBLE;

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
	UNALIGNED_YUV422I_INPUT_PREAMBLE;
	
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
	__m128i uv, uLo, uHi, vLo, vHi;
	UNALIGNED_YUV422I_INPUT_PREAMBLE;

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
    UNALIGNED_YUV422I_INPUT_PREAMBLE;

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
    UNALIGNED_YUV422I_INPUT_PREAMBLE;
	
    // Y unpacking
	out_2_v16i_y_uv_vectors[0] = _mm_shuffle_epi8 (*INPUT_VECT, _M(shuffle_lumas));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PSHUFB	1	0.5
	
    //
    // U V unpacking
    out_2_v16i_y_uv_vectors[1] = _mm_shuffle_epi8 (*INPUT_VECT, _M(shuffle_chromas));
    // U12 0    V12 0   U34 0   V34 0   U56 0   V56 0   U78 0   V78 0		// PSHUFB	1	0.5
};



/*
 * Convert 1 vector of 16 char Y (as found in YUV422P / YUV420P) to 2 vectors of 8 short Y
 *
 * Total latency:				2
 *
 * INPUT
 * 1 vector of 16 char
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12		Y13 Y14		Y15 Y16
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect1
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * yVect2
 * Y9  0	Y10  0	Y11  0	Y12  0	Y13  0	Y14  0	Y15  0	Y16  0
 */
INLINE_NAME(unpack_yuv42Xp_to_2_y_vectors_sse2, __m128i* y_input, __m128i* out_1_v16i_y_vector1, __m128i* out_1_v16i_y_vector2)
{
	CONST_M128I(zero_vect, 0x0LL, 0x0LL);
	UNALIGNED_YUVP_Y_INPUT_PREAMBLE;

    // Y1 unpacking
	*out_1_v16i_y_vector1 = _mm_unpacklo_epi8(*YUVP_Y_INPUT_VECT, _M(zero_vect));
    // Y1 0     Y2 0    Y3 0    Y4 0    Y5 0    Y6 0    Y7 0    Y8 0		// PUNPCKLBW       1	0.5

    //
    // Y2 unpacking
    *out_1_v16i_y_vector2 = _mm_unpackhi_epi8(*YUVP_Y_INPUT_VECT, _M(zero_vect));
    // Y9  0	Y10  0	Y11  0	Y12  0	Y13  0	Y14  0	Y15  0	Y16  0		// PUNPCKHBW       1	0.5
};

/*
 * Convert the first 8 chroma values in 2 vectors of 16 char UV
 * (as found in YUV422P / YUV420P) to 2 vectors of 8 interleaved short UV
 *
 * Total latency:				3
 *
 * INPUT
 * 2 vectors of 16 char
 * u_input
 * U1 U2	U3 U4	U5 U6	U7 U8	U9 U10	U11 U12		U13 U14		U15 U16
 *
 * v_input
 * V1 V2	V3 V4	V5 V6	V7 V8	V9 V10	V11 V12		V13 V14		V15 V16
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 * uvVect1
 * U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4	0
 *
 * uvVect2
 * U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8	0
 */
INLINE_NAME(unpack_low_yuv42Xp_to_uv_vector_sse2, __m128i* u_input, __m128i* v_input, __m128i* out_1_v16i_uv_vector1, __m128i* out_1_v16i_uv_vector2)
{
	CONST_M128I(zero_vect, 0x0LL, 0x0LL);
	M128I(scratch, 0x0LL, 0x0LL);
	UNALIGNED_YUVP_UV_INPUT_PREAMBLE;

	_M(scratch) = _mm_unpacklo_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
    // U1 V1	U2 V2	U3 V3	U4 V4	U5 V5	U6 V6	U7 V7	U8 V8		// PUNPCKLBW       1	0.5

	*out_1_v16i_uv_vector1 = _mm_unpacklo_epi8(_M(scratch), _M(zero_vect));
	// U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4	0		// PUNPCKLBW       1	0.5

	*out_1_v16i_uv_vector2 = _mm_unpackhi_epi8(_M(scratch), _M(zero_vect));
	// U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8	0		// PUNPCKHBW       1	0.5
};


/*
 * Convert the last 8 chroma values in 2 vectors of 16 char UV
 * (as found in YUV422P / YUV420P) to 1 vector of 8 interleaved short UV
 *
 * Total latency:				3
 *
 * INPUT
 * 2 vectors of 16 char
 * u_input
 * U1 U2	U3 U4	U5 U6	U7 U8	U9 U10	U11 U12		U13 U14		U15 U16
 *
 * v_input
 * V1 V2	V3 V4	V5 V6	V7 V8	V9 V10	V11 V12		V13 V14		V15 V16
 *
 * OUTPUT:
 *
 * 1 vector of 16 char
 * uvVect1
 * U9 0		V9 0	U10 0 	V10 0	U11 0 	V11 0	U12 0 	V12 0
 *
 * uvVect2
 * U13 0	V13 0	U14 0 	V14 0	U15 0 	V15 0	U16 0	V16 0
 */
INLINE_NAME(unpack_high_yuv42Xp_to_uv_vector_sse2, __m128i* u_input, __m128i* v_input, __m128i* out_1_v16i_uv_vector1, __m128i* out_1_v16i_uv_vector2)
{
	CONST_M128I(zero_vect, 0x0LL, 0x0LL);
	M128I(scratch, 0x0LL, 0x0LL);
	UNALIGNED_YUVP_UV_INPUT_PREAMBLE;

    _M(scratch) = _mm_unpackhi_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
    // U9 V9	U10 V10	U11 V11	U12 V12	U13 V13	U14 V14	U15 V15	U16 V16		// PUNPCKHBW       1	0.5

	*out_1_v16i_uv_vector1 = _mm_unpacklo_epi8(_M(scratch), _M(zero_vect));
	// U9 0		V9 0	U10 0 	V10 0	U11 0 	V11 0	U12 0 	V12 0		// PUNPCKLBW       1	0.5

	*out_1_v16i_uv_vector2 = _mm_unpackhi_epi8(_M(scratch), _M(zero_vect));
	// U13 0	V13 0	U14 0 	V14 0	U15 0 	V15 0	U16 0	V16 0		// PUNPCKHBW       1	0.5

};

#endif	// __INTEL_CPU__

