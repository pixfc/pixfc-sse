/*
 * conversion_routines_from_v210.c
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
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

#include "common.h"
#include "pixfc-sse.h"
#include "yuv_conversion_recipes.h"

#define UPSAMPLE_AND_CONVERT_TO_RGB32(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				UPSAMPLE_V210_TO_RGB_RECIPE,\
				unpack_4v_v210_to_y_uv_vectors_,\
				pack_fn,\
				convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_,\
				4,\
				instr_set\
		)

#define UPSAMPLE_AND_CONVERT_TO_RGB24(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				UPSAMPLE_V210_TO_RGB_RECIPE,\
				unpack_4v_v210_to_y_uv_vectors_,\
				pack_fn,\
				convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_,\
				3,\
				instr_set\
		)


#define UPSAMPLE_AND_CONVERT_TO_R210(pack16_fn, pack8_fn, instr_set)\
		DO_CONVERSION_1U_2P(\
				UPSAMPLE_V210_TO_R210_RECIPE,\
				unpack_4v_v210_to_y_uv_vectors_,\
				pack16_fn,\
				pack8_fn,\
				convert_10bit_y_uv_vectors_to_10bit_rgb_vectors_,\
				3,\
				instr_set\
		)


#define CONVERT_TO_RGB32(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				V210_TO_RGB_RECIPE,\
				unpack_4v_v210_to_y_uv_vectors_,\
				pack_fn,\
				nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_,\
				4,\
				instr_set\
		)

#define CONVERT_TO_RGB24(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				V210_TO_RGB_RECIPE,\
				unpack_4v_v210_to_y_uv_vectors_,\
				pack_fn,\
				nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_,\
				3,\
				instr_set\
		)


#define CONVERT_TO_R210(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				V210_TO_R210_RECIPE,\
				unpack_4v_v210_to_y_uv_vectors_,\
				pack_fn,\
				nnb_upsample_n_convert_10bit_y_uv_vectors_to_10bit_rgb_vectors_,\
				instr_set\
		)


/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		A R G B
 */

void		upsample_n_convert_v210_to_argb_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	 UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3_sse41);
}
void		convert_v210_to_argb_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2,	sse2_ssse3_sse41);
}
void		upsample_n_convert_v210_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}
void		convert_v210_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}


/*
 * 		V 2 1 0
 *
 * 		to
 *
 * 		B G R A
 *
 */
void		upsample_n_convert_v210_to_bgra_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2_ssse3_sse41);
}

void		convert_v210_to_bgra_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3_sse41);
}

void		upsample_n_convert_v210_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2_ssse3);
}

void		convert_v210_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3);
}


/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		R G B   2 4
 */
void		upsample_n_convert_v210_to_rgb24_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3_sse41);
}

void		convert_v210_to_rgb24_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3_sse41);
}

void		upsample_n_convert_v210_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_v210_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}


/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		B G R 2 4
 *
 */
void		upsample_n_convert_v210_to_bgr24_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3_sse41);
}

void		convert_v210_to_bgr24_sse2_ssse3_sse41(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3_sse41);
}

void		upsample_n_convert_v210_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_v210_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}




/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		1 0 B I T  R G B 
 *
 */
void		upsample_n_convert_v210_to_r210_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_R210(pack_6_r_g_b_vectors_to_4_r210_sse2_ssse3, pack_3_r_g_b_vectors_to_2_r210_sse2_ssse3, sse2_ssse3);
}

void		convert_v210_to_r210_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_R210(pack_3_r_g_b_vectors_to_2_r210_sse2_ssse3, sse2_ssse3);
}

void		upsample_n_convert_v210_to_r10k_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_R210(pack_6_r_g_b_vectors_to_4_r10k_sse2_ssse3, pack_3_r_g_b_vectors_to_2_r10k_sse2_ssse3, sse2_ssse3);
}

void		convert_v210_to_r10k_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_R210(pack_3_r_g_b_vectors_to_2_r10k_sse2_ssse3, sse2_ssse3);
}




/*
 *
 *	V 2 1 0
 *
 *	T O
 *
 *	Y U V 4 2 2 I
 *
 *
 */
// V210 to YUYU
void		convert_v210_to_yuyv_sse2_ssse3_sse41(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3_sse41);
}

void		convert_v210_to_yuyv_sse2_ssse3(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

// V210 to UYVY
void		convert_v210_to_uyvy_sse2_ssse3_sse41(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3_sse41);
}

void		convert_v210_to_uyvy_sse2_ssse3(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

