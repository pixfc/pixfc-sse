/*
 * conversion_routines_from_rgb24.c
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
#include "pixfc-sse.h"
#include "rgb_conversion_recipes.h"

#define CONVERT_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
				RGB24_TO_YUV422I_RECIPE,\
				unpack_rgb24_to_r_g_b_vectors_,\
				pack_fn,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
			)

#define CONVERT2_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
				RGB24_TO_YUV422I_RECIPE2,\
				unpack_rgb24_to_ag_rb_vectors_,\
				pack_fn,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
			)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
				AVG_DOWNSAMPLE_RGB24_TO_YUV422I_RECIPE,\
				unpack_rgb24_to_r_g_b_vectors_,\
				pack_fn,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
			)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
				AVG_DOWNSAMPLE_RGB24_TO_YUV422I_RECIPE2,\
				unpack_rgb24_to_ag_rb_vectors_,\
				pack_fn,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
			)

#define CONVERT_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						RGB24_TO_YUV422P_RECIPE,\
						unpack_rgb24_to_r_g_b_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_r_g_b_vectors_to_y_vector_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
						instr_set\
			)

#define CONVERT2_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						RGB24_TO_YUV422P_RECIPE2,\
						unpack_rgb24_to_ag_rb_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_ag_rb_vectors_to_y_vector_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						AVG_DOWNSAMPLE_RGB24_TO_YUV422P_RECIPE,\
						unpack_rgb24_to_r_g_b_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_r_g_b_vectors_to_y_vector_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						AVG_DOWNSAMPLE_RGB24_TO_YUV422P_RECIPE2,\
						unpack_rgb24_to_ag_rb_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_ag_rb_vectors_to_y_vector_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
						instr_set\
			)

#define CONVERT_TO_YUV420P(instr_set)\
		DO_CONVERSION_1U_2P(\
				RGB24_TO_YUV420P_RECIPE,\
				unpack_rgb24_to_r_g_b_vectors_,\
				pack_2_y_vectors_to_1_y_vector_sse2,\
				pack_4_uv_vectors_to_yup_vectors_sse2,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT2_TO_YUV420P(instr_set)\
		DO_CONVERSION_1U_2P(\
				RGB24_TO_YUV420P_RECIPE2,\
				unpack_rgb24_to_ag_rb_vectors_,\
				pack_2_y_vectors_to_1_y_vector_sse2,\
				pack_4_uv_vectors_to_yup_vectors_sse2,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
		)



// RGB24 to YUYV			SSE2 SSSE3
void		convert_rgb24_to_yuyv_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_rgb24_to_yuyv_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}


// RGB24 to YUYV			SSE2
void		convert_rgb24_to_yuyv_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}

void		downsample_n_convert_rgb24_to_yuyv_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}





// RGB24 to UYVY			SSE2 SSSE3
void		convert_rgb24_to_uyvy_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_rgb24_to_uyvy_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}


// RGB24 to UYVY			SSE2
void		convert_rgb24_to_uyvy_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}

void		downsample_n_convert_rgb24_to_uyvy_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}




// RGB24 to YUV422P			SSE2 SSSE3
void		convert_rgb24_to_yuv422p_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422P(sse2_ssse3);
}

void		downsample_n_convert_rgb24_to_yuv422p_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422P(sse2_ssse3);
}


// RGB24 to YUV422P			SSE2
void		convert_rgb24_to_yuv422p_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422P(sse2);
}

void		downsample_n_convert_rgb24_to_yuv422p_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422P(sse2);
}


// RGB24 to YUV420P			SSE2 SSSE3
void		convert_rgb24_to_yuv420p_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV420P(sse2_ssse3);
}

// RGB24 to YUV420P			SSE2
void		convert_rgb24_to_yuv420p_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV420P(sse2);
}
