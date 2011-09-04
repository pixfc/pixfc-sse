/*
 * conversion_routines_from_argb_bt601.c
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
#include "argb_conversion_common.h"

#define CONVERT_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						CONVERT_RGB32_TO_YUV422I,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_fn,\
						convert_r_g_b_vectors_to_y_vector_bt601_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt601_sse2,\
						instr_set\
			)

#define CONVERT2_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						CONVERT2_RGB32_TO_YUV422I,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_fn,\
						convert_ag_rb_vectors_to_y_vector_bt601_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt601_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						AVG_DOWNSAMPLE_N_CONVERT_RGB32_TO_YUV422I,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_fn,\
						convert_r_g_b_vectors_to_y_vector_bt601_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt601_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						AVG_DOWNSAMPLE_N_CONVERT2_RGB32_TO_YUV422I,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_fn,\
						convert_ag_rb_vectors_to_y_vector_bt601_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt601_sse2,\
						instr_set\
			)



// ARGB to YUYV			SSE2 SSSE3
void		convert_argb_to_yuyv_bt601_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_argb_to_yuyv_bt601_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}


// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_bt601_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}

void		downsample_n_convert_argb_to_yuyv_bt601_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}




// ARGB to UYVY			SSE2 SSSE3
void		convert_argb_to_uyvy_bt601_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_argb_to_uyvy_bt601_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}


// ARGB to UYVY			SSE2
void		convert_argb_to_uyvy_bt601_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}

void		downsample_n_convert_argb_to_uyvy_bt601_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}




void 		convert_rgb_to_yuv422_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out)
{
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;
	PixFcPixelFormat 	src_fmt = conv->source_fmt;
	uint32_t 			pixel_num = 0;
	uint32_t			pixel_count = conv->pixel_count;
	uint8_t*			src = (uint8_t *) in;
	uint8_t*			dst = (uint8_t *) out;
	int32_t				r1, g1, b1, r2, g2, b2;
	int32_t				y1, y2, u, v;

	while(pixel_num < pixel_count){
		if (src_fmt == PixFcARGB) {
			src++;	// A
			r1 = *(src++);
			g1 = *(src++);
			b1 = *(src++);
			src++;	// A
			r2 = *(src++);
			g2 = *(src++);
			b2 = *(src++);
		} else if (src_fmt == PixFcBGRA) {
			b1 = *(src++);
			g1 = *(src++);
			r1 = *(src++);
			src++;	// A
			b2 = *(src++);
			g2 = *(src++);
			r2 = *(src++);
			src++;	// A
		} else if (src_fmt == PixFcRGB24) {
			r1 = *(src++);
			g1 = *(src++);
			b1 = *(src++);
			r2 = *(src++);
			g2 = *(src++);
			b2 = *(src++);
		} else if (src_fmt == PixFcBGR24) {
			b1 = *(src++);
			g1 = *(src++);
			r1 = *(src++);
			b2 = *(src++);
			g2 = *(src++);
			r2 = *(src++);
		} else
			printf("Unknown source pixel format in non-SSE conversion from RGB\n");


		//
		y1 = ((66 * r1 + 129 * g1 + 25 * b1) >> 8) + 16;
		u = ((-38 * r1 - 74 * g1 + 112 * b1) >> 8) + 128;
		v = ((112 * r1 - 94  * g1 - 18 * b1) >> 8) + 128;
		y2 = ((66 * r2 + 129 * g2 + 25 * b2) >> 8) + 16;

		if (dest_fmt == PixFcYUYV) {
			*(dst++) = CLIP_PIXEL(y1);
			*(dst++) = CLIP_PIXEL(u);
			*(dst++) = CLIP_PIXEL(y2);
			*(dst++) = CLIP_PIXEL(v);
		} else if (dest_fmt == PixFcUYVY) {
			*(dst++) = CLIP_PIXEL(u);
			*(dst++) = CLIP_PIXEL(y1);
			*(dst++) = CLIP_PIXEL(v);
			*(dst++) = CLIP_PIXEL(y2);
		} else {
			printf("Unknown output format in non-SSE conversion from RGB\n");
		}

		pixel_num += 2; // 2 pixels processed per loop
	}
}


