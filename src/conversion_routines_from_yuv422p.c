/*
 * conversion_routines_from_yuv422p.c
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
#include "yuyv_conversion_common.h"

#define UPSAMPLE_AND_CONVERT_TO_RGB32(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				UPSAMPLE_AND_CONVERT_YUV422P_TO_RGB,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				convert_y_uv_vectors_to_rgb_vectors_,\
				4,\
				instr_set\
		)

#define UPSAMPLE_AND_CONVERT_TO_RGB24(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				UPSAMPLE_AND_CONVERT_YUV422P_TO_RGB,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				convert_y_uv_vectors_to_rgb_vectors_,\
				3,\
				instr_set\
		)



#define CONVERT_TO_RGB32(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				CONVERT_YUV422P_TO_RGB,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_,\
				4,\
				instr_set\
		)

#define CONVERT_TO_RGB24(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				CONVERT_YUV422P_TO_RGB,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_,\
				3,\
				instr_set\
		)


/*
 *
 * 		Y U V 4 2 2 P
 *
 * 		to
 *
 * 		A R G B
 */
void		upsample_n_convert_yuv422p_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	 UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}

void		convert_yuv422p_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2,	sse2_ssse3);
}

void		upsample_n_convert_yuv422p_to_argb_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}

void		convert_yuv422p_to_argb_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}

/*
 * 		Y U V 4 2 2 P
 *
 * 		to
 *
 * 		B G R A
 *
 */
void		upsample_n_convert_yuv422p_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2_ssse3);
}

void		convert_yuv422p_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3);
}

void		upsample_n_convert_yuv422p_to_bgra_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}

void		convert_yuv422p_to_bgra_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}


/*
 *
 * 		Y U V 4 2 2 P
 *
 * 		to
 *
 * 		R G B   2 4
 */
void		upsample_n_convert_yuv422p_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv422p_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		upsample_n_convert_yuv422p_to_rgb24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}

void		convert_yuv422p_to_rgb24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}

/*
 *
 * 		Y U V 4 2 2 P
 *
 * 		to
 *
 * 		B G R 2 4
 *
 */
void		upsample_n_convert_yuv422p_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv422p_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		upsample_n_convert_yuv422p_to_bgr24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, sse2);
}

void		convert_yuv422p_to_bgr24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, sse2);
}

/*
 *
 * Non SSE conversion block (nearest neighbour upsampling)
 *
 */
void 		convert_yuv422p_to_any_rgb_nonsse(const struct PixFcSSE* conv, void* in, void* out){
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;
	uint32_t 			pixel_num = 0;
	uint32_t			pixel_count = conv->pixel_count;
	uint8_t*			y_src = (uint8_t *) in;
	uint8_t*			u_src = y_src + pixel_count;
	uint8_t*			v_src = u_src + pixel_count / 2;
	uint8_t*			dst = (uint8_t *) out;
	int32_t				r, g, b;
	int32_t				y, u, v;

	while(pixel_num++ < pixel_count){
		y = (*y_src++) << 8;
		u = *u_src - 128;
		v = *v_src - 128;

		if ((pixel_num & 0x1) == 0) {
			u_src++;
			v_src++;
		}

		r = (y + (359 * v)) >> 8;
		g = (y - (88 * u) - (183 * v)) >> 8;
		b = (y + (454 * u)) >> 8;

		if (dest_fmt == PixFcARGB) {
			*(dst++) = 0;		//A
			*(dst++) = CLIP_PIXEL(r);
			*(dst++) = CLIP_PIXEL(g);
			*(dst++) = CLIP_PIXEL(b);
		} else if (dest_fmt == PixFcBGRA) {
			*(dst++) = CLIP_PIXEL(b);
			*(dst++) = CLIP_PIXEL(g);
			*(dst++) = CLIP_PIXEL(r);
			*(dst++) = 0;		//A
		} else  if (dest_fmt == PixFcRGB24) {
			*(dst++) = CLIP_PIXEL(r);
			*(dst++) = CLIP_PIXEL(g);
			*(dst++) = CLIP_PIXEL(b);
		} else  {	// PixFcBGR24
			*(dst++) = CLIP_PIXEL(b);
			*(dst++) = CLIP_PIXEL(g);
			*(dst++) = CLIP_PIXEL(r);
		}
	}
}


