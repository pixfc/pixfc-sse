/*
 * conversion_routines_from_r210.c
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
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
#include "pixfc-sse.h"
#include "rgb_conversion_recipes.h"


#define CONVERT_TO_YUV422I(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				RGB32_TO_YUV422I_RECIPE,\
				unpack_2_r210_to_r_g_b_vectors_,\
				pack_fn,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)


#define DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE,\
				unpack_2_r210_to_r_g_b_vectors_,\
				pack_fn,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)


// r210 to YUYV			SSE2 SSSE3
void		convert_r210_to_yuyv_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_r210_to_yuyv_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}



/*
 *
 * 		R 2 1 0
 *
 * 		T O
 *
 * 		R G B 3 2  /  2 4
 *
 */
void		convert_r210_to_argb_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_4_argb_vectors_sse2, 4);
}

void		convert_r210_to_bgra_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, 4);
}

void		convert_r210_to_rgb24_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, 3);
}

void		convert_r210_to_bgr24_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, 3);
}

