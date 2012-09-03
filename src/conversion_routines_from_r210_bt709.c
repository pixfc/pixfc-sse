/*
 * conversion_routines_from_r210_bt709.c
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
		convert_10bit_r_g_b_vectors_to_8bit_y_vector_bt709_sse2,\
		convert_downsampled_422_10bit_r_g_b_vectors_to_8bit_uv_vector_bt709_sse2,\
		instr_set\
	)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_fn, instr_set)\
	DO_CONVERSION_1U_1P(\
		AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE,\
		unpack_2_r210_to_r_g_b_vectors_,\
		pack_fn,\
		convert_10bit_r_g_b_vectors_to_8bit_y_vector_bt709_sse2,\
		convert_downsampled_422_10bit_r_g_b_vectors_to_8bit_uv_vector_bt709_sse2,\
		instr_set\
	)

#define CONVERT_TO_V210(instr_set)\
	DO_CONVERSION_1U_1P(\
		RGB32_TO_V210_RECIPE,\
		unpack_2_r210_to_r_g_b_vectors_sse2_ssse3,\
		pack_6_y_uv_vectors_to_4_v210_vectors_ ## instr_set,\
		convert_10bit_r_g_b_vectors_to_10bit_y_vector_bt709_sse2,\
		convert_downsampled_422_10bit_r_g_b_vectors_to_10bit_uv_vector_bt709_sse2,\
		instr_set\
	)

#define DOWNSAMPLE_N_CONVERT_TO_V210(instr_set)\
	DO_CONVERSION_1U_1P(\
		AVG_DOWNSAMPLE_RGB32_TO_V210_RECIPE,\
		unpack_2_r210_to_r_g_b_vectors_sse2_ssse3,\
		pack_6_y_uv_vectors_to_4_v210_vectors_ ## instr_set,\
		convert_10bit_r_g_b_vectors_to_10bit_y_vector_bt709_sse2,\
		convert_downsampled_422_10bit_r_g_b_vectors_to_10bit_uv_vector_bt709_sse2,\
		instr_set\
	)


// r210 to YUYV			SSE2 SSSE3
void		convert_r210_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_r210_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

// r210 to UYVY			SSE2 SSSE3
void		convert_r210_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_r210_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

// r210 to v210			SSE2 SSSE3 SSE41
void		convert_r210_to_v210_bt709_sse2_ssse3_sse41(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_V210(sse2_ssse3_sse41);
}

void		downsample_n_convert_r210_to_v210_bt709_sse2_ssse3_sse41(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_V210(sse2_ssse3_sse41);
}

// r210 to v210			SSE2 SSSE3
void		convert_r210_to_v210_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_V210(sse2_ssse3);
}

void		downsample_n_convert_r210_to_v210_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_V210(sse2_ssse3);
}


