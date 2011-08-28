/*
 * yuv_pack.h
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

#ifndef YUV_REPACK_H_
#define YUV_REPACK_H_

#include <emmintrin.h>
#include <tmmintrin.h>

#include "debug_support.h"
#include "common.h"
#include "platform_util.h"


#undef UNALIGNED_YUVI_INPUT_PREAMBLE
#undef UNALIGNED_YUVI_INPUT_VECT
#undef UNALIGNED_YUVP_Y_INPUT_PREAMBLE
#undef UNALIGNED_YUVP_UV_INPUT_PREAMBLE
#undef UNALIGNED_YUVP_INPUT_PREAMBLE
#undef UNALIGNED_YUVP_Y_INPUT_VECT
#undef UNALIGNED_YUVP_U_INPUT_VECT
#undef UNALIGNED_YUVP_V_INPUT_VECT
#undef UNALIGNED_STORE
#undef ALIGNED_STORE
#undef ALIGNED_YUVI_INPUT_VECT
#undef YUVP_Y_INPUT_VECT
#undef YUVP_U_INPUT_VECT
#undef YUVP_V_INPUT_VECT
#undef NOOP


/*
 * Define a few macros to deal with unaligned input/output buffers.
 *
 */
// Interleaved YUV
#define UNALIGNED_YUVI_INPUT_PREAMBLE			DECLARE_VECT_ARRAY4_N_UNALIGN_LOAD(aligned_vector, input);
#define UNALIGNED_YUVI_INPUT_VECT				(&aligned_vector)

// Planar YUV
#define UNALIGNED_YUVP_Y_INPUT_PREAMBLE			DECLARE_VECT_ARRAY2_N_UNALIGN_LOAD(aligned_y_vector, y_input)
#define UNALIGNED_YUVP_UV_INPUT_PREAMBLE		DECLARE_VECT_N_UNALIGN_LOAD(aligned_u_vector, u_input); DECLARE_VECT_N_UNALIGN_LOAD(aligned_v_vector, v_input)
#define UNALIGNED_YUVP_INPUT_PREAMBLE			DECLARE_VECT_ARRAY2_N_UNALIGN_LOAD(aligned_y_vector, y_input);DECLARE_VECT_N_UNALIGN_LOAD(aligned_u_vector, u_input); DECLARE_VECT_N_UNALIGN_LOAD(aligned_v_vector, v_input);

#define UNALIGNED_YUVP_Y_INPUT_VECT				(&aligned_y_vector)
#define UNALIGNED_YUVP_U_INPUT_VECT				(&aligned_u_vector)
#define UNALIGNED_YUVP_V_INPUT_VECT				(&aligned_v_vector)

#define UNALIGNED_STORE(src, dst)				_mm_storeu_si128(&(dst), (src))
#define ALIGNED_STORE(src, dst)					(dst) = (src)

/*
 * And their counterparts for aligned input/output buffers.
 */
#define ALIGNED_YUVI_INPUT_VECT					input
#define YUVP_Y_INPUT_VECT						(y_input)
#define YUVP_U_INPUT_VECT						(u_input)
#define YUVP_V_INPUT_VECT						(v_input)
#define NOOP



/*
 * The following macro expands to the following:
 *
 * EXTERN_INLINE void inline_fn_name( __m128i* y_input, __m128i* u_input, __m128i* v_input, __m128i* yuyv_out)
 * {
 * 	M128I(scratch, 0x0LL, 0x0LL);
 *
 * 	UNALIGNED_YUVP_INPUT_PREAMBLE;
 *
 *  // UV  interleaving
 * 	_M(scratch) = _mm_unpacklo_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
 * 	// U1 V1	U2 V2	U3 V4	U4 V4	U5 V5	U6 V6	U7 V7	U8 V8		// PUNPCKLBW       1	0.5
 *
 * 	// Y - UV interleaving
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[0], _mm_unpacklo_epi8(YUVP_Y_INPUT_VECT[0], _M(scratch)));
 * 	// Y1 U1	Y2 V1	Y3 U2	Y4 V2	Y5 U3	Y6 V3	Y7 U4	Y8 V4		// PUNPCKLBW       1	0.5
 *
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[1], _mm_unpackhi_epi8(YUVP_Y_INPUT_VECT[0], _M(scratch)));
 * 	// Y9 U5	Y10 V5	Y11 U6	Y12 V6	Y13 U7	Y14 V7	Y15 U8	Y16 V8		// PUNPCKHBW       1	0.5
 *
 *
 * 	// UV  interleaving
 * 	_M(scratch) = _mm_unpackhi_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
 * 	// U9 V9	U10 V10	U11 V11	U12 V12	U13 V13	U14 V14	U15 V15	U16 V16		// PUNPCKHBW       1	0.5
 *
 * 	// Y - UV interleaving
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[2], _mm_unpacklo_epi8(YUVP_Y_INPUT_VECT[1], _M(scratch)));
 * 	// Y17 U9	Y18 V9	Y19 U10	Y20 V10	Y21 U11	Y22 V11	Y23 U12	Y24 V12		// PUNPCKLBW       1	0.5
 *
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[3], _mm_unpackhi_epi8(YUVP_Y_INPUT_VECT[1], _M(scratch)));
 * 	// Y25 U13	Y26 V13	Y27 U14	Y28 V14	Y29 U15	Y30 V15	Y31 U16	Y32 V16		// PUNPCKHBW       1	0.5
 * };
 *
 */
#define DEFINE_SSE2_YUV422P_TO_YUYV_REPACK_INLINE(inline_fn_name, preamble, y_vect, u_vect, v_vect, store_op)\
EXTERN_INLINE void inline_fn_name( __m128i* y_input, __m128i* u_input, __m128i* v_input, __m128i* yuyv_out)\
{\
	M128I(scratch, 0x0LL, 0x0LL);\
	(preamble);\
	_M(scratch) = _mm_unpacklo_epi8(*(u_vect), *(v_vect));\
	store_op(_mm_unpacklo_epi8((y_vect)[0], _M(scratch)), yuyv_out[0]);\
	store_op(_mm_unpackhi_epi8((y_vect)[0], _M(scratch)), yuyv_out[1]);\
	_M(scratch) = _mm_unpackhi_epi8(*(u_vect), *(v_vect));\
	store_op(_mm_unpacklo_epi8((y_vect)[1], _M(scratch)), yuyv_out[2]);\
	store_op(_mm_unpackhi_epi8((y_vect)[1], _M(scratch)), yuyv_out[3]);\
}


/*
 * Convert YUV422P to YUYV:
 * 4 vectors of 16 char: 2xY, 1xU, 1xV to 4 vectors of 16 char YUYV
 *
 * Total latency:				6
 * Number of pixels handled:	32
 *
 * INPUT
 * 4 vectors of 16 char
 * y_input
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 * Y17 Y18	Y19 Y20	Y21 Y22	Y23 Y24	Y25 Y26	Y27 Y28 Y29 Y30	Y31 Y32
 *
 * u_input
 * U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14	U15 U16
 *
 * v_input
 * V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14	V15 V16
 *
 *
 * OUTPUT:
 *
 * 4 vectors of 16 char
 * Y1 U1	Y2 V1	Y3 U2	Y4 V2	Y5 U3	Y6 V3	Y7 U4	Y8 V4
 * Y9 U5	Y10 V5	Y11 U6	Y12 V6	Y13 U7	Y14 V7	Y15 U8	Y16 V8
 * Y17 U9	Y18 V9	Y19 U10	Y20 V10	Y21 U11	Y22 V11	Y23 U12	Y24 V12
 * Y25 U13	Y26 V13	Y27 U14	Y28 V14	Y29 U15	Y30 V15	Y31 U16	Y32 V16
 *
 */
DEFINE_SSE2_YUV422P_TO_YUYV_REPACK_INLINE(repack_yuv422p_to_yuyv_sse2, NOOP, YUVP_Y_INPUT_VECT, YUVP_U_INPUT_VECT, YUVP_V_INPUT_VECT, ALIGNED_STORE)
DEFINE_SSE2_YUV422P_TO_YUYV_REPACK_INLINE(unaligned_src_repack_yuv422p_to_yuyv_sse2, UNALIGNED_YUVP_INPUT_PREAMBLE, UNALIGNED_YUVP_Y_INPUT_VECT, UNALIGNED_YUVP_U_INPUT_VECT, UNALIGNED_YUVP_V_INPUT_VECT, ALIGNED_STORE)
DEFINE_SSE2_YUV422P_TO_YUYV_REPACK_INLINE(unaligned_dst_repack_yuv422p_to_yuyv_sse2, NOOP, YUVP_Y_INPUT_VECT, YUVP_U_INPUT_VECT, YUVP_V_INPUT_VECT, UNALIGNED_STORE)
DEFINE_SSE2_YUV422P_TO_YUYV_REPACK_INLINE(unaligned_src_unaligned_dst_repack_yuv422p_to_yuyv_sse2, UNALIGNED_YUVP_INPUT_PREAMBLE, UNALIGNED_YUVP_Y_INPUT_VECT, UNALIGNED_YUVP_U_INPUT_VECT, UNALIGNED_YUVP_V_INPUT_VECT, UNALIGNED_STORE)





/*
 * The following macro expands to the following:
 *
 * EXTERN_INLINE void inline_fn_name( __m128i* y_input, __m128i* u_input, __m128i* v_input, __m128i* yuyv_out)
 * {
 * 	M128I(scratch, 0x0LL, 0x0LL);
 *
 * 	UNALIGNED_YUVP_INPUT_PREAMBLE;
 *
 *  // UV  interleaving
 * 	_M(scratch) = _mm_shuffle_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
 * 	// U1 V1	U2 V2	U3 V4	U4 V4	U5 V5	U6 V6	U7 V7	U8 V8		// PUNPCKLBW       1	0.5
 *
 * 	// Y - UV interleaving
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[0], _mm_unpacklo_epi8(_M(scratch), YUVP_Y_INPUT_VECT[0]));
 * 	// Y1 U1	Y2 V1	Y3 U2	Y4 V2	Y5 U3	Y6 V3	Y7 U4	Y8 V4		// PUNPCKLBW       1	0.5
 *
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[1], _mm_unpackhi_epi8(_M(scratch), YUVP_Y_INPUT_VECT[0]));
 * 	// Y9 U5	Y10 V5	Y11 U6	Y12 V6	Y13 U7	Y14 V7	Y15 U8	Y16 V8		// PUNPCKHBW       1	0.5
 *
 *
 * 	// UV  interleaving
 * 	_M(scratch) = _mm_unpackhi_epi8(*YUVP_U_INPUT_VECT, *YUVP_V_INPUT_VECT);
 * 	// U9 V9	U10 V10	U11 V11	U12 V12	U13 V13	U14 V14	U15 V15	U16 V16		// PUNPCKHBW       1	0.5
 *
 * 	// Y - UV interleaving
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[2], _mm_unpacklo_epi8(_M(scratch), YUVP_Y_INPUT_VECT[1]));
 * 	// Y17 U9	Y18 V9	Y19 U10	Y20 V10	Y21 U11	Y22 V11	Y23 U12	Y24 V12		// PUNPCKLBW       1	0.5
 *
 * 	UNALIGNED_STORE(out_4_v16i_yuyv_vectors[3], _mm_unpackhi_epi8(_M(scratch), YUVP_Y_INPUT_VECT[1]));
 * 	// Y25 U13	Y26 V13	Y27 U14	Y28 V14	Y29 U15	Y30 V15	Y31 U16	Y32 V16		// PUNPCKHBW       1	0.5
 * };
 *
 */
#define DEFINE_SSE2_YUV422P_TO_UYVY_REPACK_INLINE(inline_fn_name, preamble, y_vect, u_vect, v_vect, store_op)\
EXTERN_INLINE void inline_fn_name( __m128i* y_input, __m128i* u_input, __m128i* v_input, __m128i* uyvy_out)\
{\
	M128I(scratch, 0x0LL, 0x0LL);\
	(preamble);\
	_M(scratch) = _mm_unpacklo_epi8(*(u_vect), *(v_vect));\
	store_op(_mm_unpacklo_epi8(_M(scratch), (y_vect)[0]), yuyv_out[0]);\
	store_op(_mm_unpackhi_epi8(_M(scratch), (y_vect)[0]), yuyv_out[1]);\
	_M(scratch) = _mm_unpackhi_epi8(*(u_vect), *(v_vect));\
	store_op(_mm_unpacklo_epi8(_M(scratch), (y_vect)[1]), yuyv_out[2]);\
	store_op(_mm_unpackhi_epi8(_M(scratch), (y_vect)[1]), yuyv_out[3]);\
}


/*
 * Convert YUV422P to UYVY:
 * 4 vectors of 16 char: 2xY, 1xU, 1xV to 4 vectors of 16 char YUYV
 *
 * Total latency:				4
 * Number of pixels handled:	32
 *
 * INPUT
 * 4 vectors of 16 char
 * y_input
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 * Y17 Y18	Y19 Y20	Y21 Y22	Y23 Y24	Y25 Y26	Y27 Y28 Y29 Y30	Y31 Y32
 *
 * u_input
 * U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14	U15 U16
 *
 * v_input
 * V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14	V15 V16
 *
 *
 * OUTPUT:
 *
 * 4 vectors of 16 char
 * U1 Y1	V1 Y2	U2 Y3	V2 Y4	U3 Y5	V3 Y6	U4 Y7	V4 Y8
 * U5 Y9	V5 Y10	U6 Y11	V6 Y12	U7 Y13	V7 Y14	U8 Y15	V8 Y16
 * U9 Y17	V9 Y18	U10 Y19	V10 Y20	U11 Y21	V11 Y22	U12 Y23	V12 Y24
 * U13 Y25	V13 Y26	U14 Y27	V14 Y28	U15 Y29	V15 Y30	U16 Y31	V16 Y32
 *
 */
DEFINE_SSE2_YUV422P_TO_UYVY_REPACK_INLINE(repack_yuv422p_to_uyvy_sse2, NOOP, YUVP_Y_INPUT_VECT, YUVP_U_INPUT_VECT, YUVP_V_INPUT_VECT, ALIGNED_STORE)
DEFINE_SSE2_YUV422P_TO_UYVY_REPACK_INLINE(unaligned_src_repack_yuv422p_to_uyvy_sse2, UNALIGNED_YUVP_INPUT_PREAMBLE, UNALIGNED_YUVP_Y_INPUT_VECT, UNALIGNED_YUVP_U_INPUT_VECT, UNALIGNED_YUVP_V_INPUT_VECT, ALIGNED_STORE)
DEFINE_SSE2_YUV422P_TO_UYVY_REPACK_INLINE(unaligned_dst_repack_yuv422p_to_uyvy_sse2, NOOP, YUVP_Y_INPUT_VECT, YUVP_U_INPUT_VECT, YUVP_V_INPUT_VECT, UNALIGNED_STORE)
DEFINE_SSE2_YUV422P_TO_UYVY_REPACK_INLINE(unaligned_src_unaligned_dst_repack_yuv422p_to_uyvy_sse2, UNALIGNED_YUVP_INPUT_PREAMBLE, UNALIGNED_YUVP_Y_INPUT_VECT, UNALIGNED_YUVP_U_INPUT_VECT, UNALIGNED_YUVP_V_INPUT_VECT, UNALIGNED_STORE)



#define DEFINE_SSE2_YUYV_TO_YUV422P_REPACK_INLINE(inline_fn_name, preamble, yuyv_input, store_op)\
EXTERN_INLINE void inline_fn_name(__m128i* input, __m128i* y_output, __m128i* u_output, __m128i* v_output)\
{\
	CONST_M128I(mask_luma, 0x00FF00FF00FF00FFLL, 0x00FF00FF00FF00FFLL);\
	CONST_M128I(mask_cb,   0x0000FF000000FF00LL, 0x0000FF000000FF00LL);\
	CONST_M128I(mask_cr,   0xFF000000FF000000LL, 0xFF000000FF000000LL);\
	M128I(scratch, 0x0LL, 0x0LL);\
	M128I(scratch2, 0x0LL, 0x0LL);\
	M128I(scratch3, 0x0LL, 0x0LL);\
	M128I(scratch4, 0x0LL, 0x0LL);\
	(preamble);\
	_M(scratch) = _mm_and_si128((yuyv_input)[0], _M(mask_luma));\
	_M(scratch2) = _mm_and_si128(*(yuyv_input)[1], _M(mask_luma));\
	store_op(_mm_packus_epi16(_M(scratch), _M(scratch2)), y_output[0]);\
	_M(scratch) = _mm_and_si128((yuyv_input)[2], _M(mask_luma));\
	_M(scratch2) = _mm_and_si128(*(yuyv_input)[3], _M(mask_luma));\
	store_op(_mm_packus_epi16(_M(scratch), _M(scratch2)), y_output[1]);\
	_M(scratch) = _mm_and_si128((yuyv_input)[0], _M(mask_cb));\
	_M(scratch) = _mm_srli_si128(_M(scratch), 1);\
	_M(scratch2) = _mm_and_si128((yuyv_input)[1], _M(mask_cb));\
	_M(scratch2) = _mm_srli_si128((_M(scratch2), 1);\
	_M(scratch) = _mm_packs_epi32(_M(scratch), _M(scratch2));\
	_M(scratch2) = _mm_and_si128((yuyv_input)[2], _M(mask_cb));\
	_M(scratch2) = _mm_srli_si128(_M(scratch2), 1);\
	_M(scratch3) = _mm_and_si128((yuyv_input)[3], _M(mask_cb));\
	_M(scratch3) = _mm_srli_si128((_M(scratch3), 1);\
	_M(scratch2) = _mm_packs_epi32(_M(scratch2), _M(scratch3));\
	store_op(_mm_packus_epi16(_M(scratch), _M(scratch2)), *u_output);\
	_M(scratch) = _mm_and_si128((yuyv_input)[0], _M(mask_cr));\
	_M(scratch) = _mm_srli_si128(_M(scratch), 3);\
	_M(scratch2) = _mm_and_si128((yuyv_input)[1], _M(mask_cr));\
	_M(scratch2) = _mm_srli_si128((_M(scratch2), 3);\
	_M(scratch) = _mm_packs_epi32(_M(scratch), _M(scratch2));\
	_M(scratch2) = _mm_and_si128((yuyv_input)[2], _M(mask_cr));\
	_M(scratch2) = _mm_srli_si128(_M(scratch2), 3);\
	_M(scratch3) = _mm_and_si128((yuyv_input)[3], _M(mask_cr));\
	_M(scratch3) = _mm_srli_si128((_M(scratch3), 3);\
	_M(scratch2) = _mm_packs_epi32(_M(scratch2), _M(scratch3));\
	store_op(_mm_packus_epi16(_M(scratch), _M(scratch2)), *v_output);\
}
#endif // YUV_REPACK_H_

