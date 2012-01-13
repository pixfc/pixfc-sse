/*
 * conversion_routines_from_argb_bt709.c
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
						RGB32_TO_YUV422I_RECIPE,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_fn,\
						convert_r_g_b_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define CONVERT2_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						RGB32_TO_YUV422I_RECIPE2,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_fn,\
						convert_ag_rb_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_fn,\
						convert_r_g_b_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_fn, instr_set)\
			DO_CONVERSION_1U_1P(\
						AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE2,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_fn,\
						convert_ag_rb_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)


#define CONVERT_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						RGB32_TO_YUV422P_RECIPE,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_r_g_b_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define CONVERT2_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						RGB32_TO_YUV422P_RECIPE2,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_ag_rb_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						AVG_DOWNSAMPLE_RGB32_TO_YUV422P_RECIPE,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_r_g_b_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422P(instr_set)\
			DO_CONVERSION_1U_2P(\
						AVG_DOWNSAMPLE_RGB32_TO_YUV422P_RECIPE2,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
						pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
						convert_ag_rb_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define CONVERT_TO_YUV420P(instr_set)\
			DO_CONVERSION_1U_2P(\
						RGB32_TO_YUV420P_RECIPE,\
						unpack_argb_to_r_g_b_vectors_,\
						pack_2_y_vectors_to_1_y_vector_sse2,\
						pack_4_uv_vectors_to_yup_vectors_sse2,\
						convert_r_g_b_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)

#define CONVERT2_TO_YUV420P(instr_set)\
			DO_CONVERSION_1U_2P(\
						RGB32_TO_YUV420P_RECIPE2,\
						unpack_argb_to_ag_rb_vectors_,\
						pack_2_y_vectors_to_1_y_vector_sse2,\
						pack_4_uv_vectors_to_yup_vectors_sse2,\
						convert_ag_rb_vectors_to_y_vector_bt709_sse2,\
						convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709_sse2,\
						instr_set\
			)


// ARGB to YUYV			SSE2 SSSE3
void		convert_argb_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_argb_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}


// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}

void		downsample_n_convert_argb_to_yuyv_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}



// ARGB to UYVY			SSE2 SSSE3
void		convert_argb_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_argb_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}


// ARGB to UYVY			SSE2
void		convert_argb_to_uyvy_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}

void		downsample_n_convert_argb_to_uyvy_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}



// ARGB to YUV422P			SSE2 SSSE3
void		convert_argb_to_yuv422p_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422P(sse2_ssse3);
}

void		downsample_n_convert_argb_to_yuv422p_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422P(sse2_ssse3);
}


// ARGB to YUV422P			SSE2
void		convert_argb_to_yuv422p_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422P(sse2);
}

void		downsample_n_convert_argb_to_yuv422p_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422P(sse2);
}


// ARGB to YUV420P			SSE2 SSSE3
void		convert_argb_to_yuv420p_bt709_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV420P(sse2_ssse3);
}

// ARGB to YUV420P			SSE2
void		convert_argb_to_yuv420p_bt709_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV420P(sse2);
}



void 		convert_rgb_to_yuv422_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out)
{
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;
	PixFcPixelFormat 	src_fmt = conv->source_fmt;
	uint32_t 			pixel_num = 0;
	uint32_t			pixel_count = conv->pixel_count;
	uint8_t*			src = (uint8_t *) in;
	uint8_t*			dst = (uint8_t *) out;
	uint8_t*			y_plane = dst;
	uint8_t*			u_plane = dst + pixel_count;
	uint8_t*			v_plane = u_plane + pixel_count / 2;
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
		y1 = ((47 * r1 + 157 * g1 + 16 * b1) >> 8) + 16;
		u = ((-26 * r1 - 87 * g1 + 112 * b1) >> 8) + 128;
		v = ((112 * r1 - 102  * g1 - 10 * b1) >> 8) + 128;
		y2 = ((47 * r2 + 157 * g2 + 16 * b2) >> 8) + 16;

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
		} else if (dest_fmt == PixFcYUV422P) {
			*(y_plane++) = CLIP_PIXEL(y1);
			*(y_plane++) = CLIP_PIXEL(y2);
			*(u_plane++) = CLIP_PIXEL(u);
			*(v_plane++) = CLIP_PIXEL(v);
		} else {
			printf("Unknown output format in non-SSE conversion from RGB\n");
		}

		pixel_num += 2; // 2 pixels processed per loop
	}
}

// RGB to YUV420		NON SSE
void 		convert_rgb_to_yuv420_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out)
{
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;
	PixFcPixelFormat 	src_fmt = conv->source_fmt;
	uint8_t				input_stride = ((src_fmt == PixFcARGB) || (src_fmt == PixFcBGRA)) ? 4 : 3;
	uint32_t			pixel_count = conv->pixel_count;
	uint8_t*			src_line1 = (uint8_t *) in;
	uint8_t*			src_line2 = src_line1 + conv->width * input_stride;
	uint8_t*			dst = (uint8_t *) out;
	uint8_t*			y_line1 = dst;
	uint8_t*			y_line2 = y_line1 + conv->width;
	uint8_t*			u_plane = dst + pixel_count;
	uint8_t*			v_plane = u_plane + pixel_count / 4;
	int32_t				r1_line1 = 0, g1_line1 = 0, b1_line1 = 0, r2_line1 = 0, g2_line1 = 0, b2_line1 = 0;
	int32_t				r1_line2 = 0, g1_line2 = 0, b1_line2 = 0, r2_line2 = 0, g2_line2 = 0, b2_line2 = 0;
	int32_t				y1_line1, y2_line1, y1_line2, y2_line2, u, v;
	uint32_t			line = conv->height;
	uint32_t			col = conv->width;

	while(line > 0){
		while(col > 0){
			if (src_fmt == PixFcARGB) {
				src_line1++;	// A
				r1_line1 = *(src_line1++);
				g1_line1 = *(src_line1++);
				b1_line1 = *(src_line1++);
				src_line1++;	// A
				r2_line1 = *(src_line1++);
				g2_line1 = *(src_line1++);
				b2_line1 = *(src_line1++);
				src_line2++;	// A
				r1_line2 = *(src_line2++);
				g1_line2 = *(src_line2++);
				b1_line2 = *(src_line2++);
				src_line2++;	// A
				r2_line2 = *(src_line2++);
				g2_line2 = *(src_line2++);
				b2_line2 = *(src_line2++);
			} else if (src_fmt == PixFcBGRA) {
				b1_line1 = *(src_line1++);
				g1_line1 = *(src_line1++);
				r1_line1 = *(src_line1++);
				src_line1++;	// A
				b2_line1 = *(src_line1++);
				g2_line1 = *(src_line1++);
				r2_line1 = *(src_line1++);
				src_line1++;	// A
				b1_line2 = *(src_line2++);
				g1_line2 = *(src_line2++);
				r1_line2 = *(src_line2++);
				src_line2++;	// A
				b2_line2 = *(src_line2++);
				g2_line2 = *(src_line2++);
				r2_line2 = *(src_line2++);
				src_line2++;	// A
			} else if (src_fmt == PixFcRGB24) {
				r1_line1 = *(src_line1++);
				g1_line1 = *(src_line1++);
				b1_line1 = *(src_line1++);
				r2_line1 = *(src_line1++);
				g2_line1 = *(src_line1++);
				b2_line1 = *(src_line1++);

				r1_line2 = *(src_line2++);
				g1_line2 = *(src_line2++);
				b1_line2 = *(src_line2++);
				r2_line2 = *(src_line2++);
				g2_line2 = *(src_line2++);
				b2_line2 = *(src_line2++);
			} else if (src_fmt == PixFcBGR24) {
				b1_line1 = *(src_line1++);
				g1_line1 = *(src_line1++);
				r1_line1 = *(src_line1++);
				b2_line1 = *(src_line1++);
				g2_line1 = *(src_line1++);
				r2_line1 = *(src_line1++);

				b1_line2 = *(src_line2++);
				g1_line2 = *(src_line2++);
				r1_line2 = *(src_line2++);
				b2_line2 = *(src_line2++);
				g2_line2 = *(src_line2++);
				r2_line2 = *(src_line2++);
			} else
				printf("Unknown source pixel format in non-SSE conversion from RGB\n");

			//
			y1_line1 = ((47 * r1_line1 + 157 * g1_line1 + 16 * b1_line1) >> 8) + 16;
			y2_line1 = ((47 * r2_line1 + 157 * g2_line1 + 16 * b2_line1) >> 8) + 16;
			y1_line2 = ((47 * r1_line2 + 157 * g1_line2 + 16 * b1_line2) >> 8) + 16;
			y2_line2 = ((47 * r2_line2 + 157 * g2_line2 + 16 * b2_line2) >> 8) + 16;

			// Average all input componenents
			r1_line1 = (r1_line1 + r2_line1 + r1_line2 + r2_line2) / 4;
			g1_line1 = (g1_line1 + g2_line1 + g1_line2 + g2_line2) / 4;
			b1_line1 = (b1_line1 + b2_line1 + b1_line2 + b2_line2) / 4;
			u = ((-26 * r1_line1 - 87 * g1_line1 + 112 * b1_line1) >> 8) + 128;
			v = ((112 * r1_line1 - 102  * g1_line1 - 10 * b1_line1) >> 8) + 128;

			if (dest_fmt == PixFcYUV420P) {
				*(y_line1++) = CLIP_PIXEL(y1_line1);
				*(y_line1++) = CLIP_PIXEL(y2_line1);
				*(y_line2++) = CLIP_PIXEL(y1_line2);
				*(y_line2++) = CLIP_PIXEL(y2_line2);
				*(u_plane++) = CLIP_PIXEL(u);
				*(v_plane++) = CLIP_PIXEL(v);
			} else {
				printf("Unknown output format in non-SSE conversion from RGB\n");
			}

			col += 2;	// 2 pixels (on two lines) are processed per inner loop
		}

		src_line1 += conv->width * input_stride;
		src_line1 += conv->width * input_stride;
		y_line1 += conv->width;
		y_line2 += conv->width;

		line -= 2; // two lines are processed per outer loop
	}
}
