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
/*
void		upsample_n_convert_v210_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}
*/
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
/*
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
*/

/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		R G B   2 4
 */
/*
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
*/

/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		B G R 2 4
 *
 */
/*
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
*/
/*
 *
 * Non SSE conversion block (nearest neighbour upsampling)
 *
 */
/*
void 		convert_v210_to_any_rgb_nonsse(const struct PixFcSSE* conv, void* in, void* out){
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;
	uint32_t 			which_y = 0;
	uint32_t 			pixel_num = 0;
	uint32_t			pixel_count = conv->pixel_count;
	uint8_t*			src = (uint8_t *) in;
	uint8_t*			dst = (uint8_t *) out;
	int32_t				r, g, b;
	int32_t				y, u, v;

	while(pixel_num++ < pixel_count){
		y = (! which_y) ? (src[0] << 8) : src[2] << 8;
		u = src[1] - 128;
		v = src[3] - 128;

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

		if (which_y++) {
			which_y = 0;
			src += 4;
		}
	}
}
*/


