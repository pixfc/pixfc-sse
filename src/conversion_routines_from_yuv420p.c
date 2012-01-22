/*
 * conversion_routines_from_yuv420p.c
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
#include "yuv_conversion_recipes.h"
#define CONVERT_TO_RGB32(convert_fn_prefix, pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				YUV420P_TO_RGB_RECIPE,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				convert_fn_prefix,\
				4,\
				instr_set\
		)

#define CONVERT_TO_RGB24(convert_fn_prefix, pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				YUV420P_TO_RGB_RECIPE,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				convert_fn_prefix,\
				3,\
				instr_set\
		)


/*
 *
 * 		Y U V 4 2 0 P
 *
 * 		to
 *
 * 		A R G B
 */

void		convert_yuv420p_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}

void		convert_yuv420p_to_argb_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}


void		convert_yuv420p_to_argb_bt601_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}

void		convert_yuv420p_to_argb_bt601_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}


void		convert_yuv420p_to_argb_bt709_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}

void		convert_yuv420p_to_argb_bt709_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}


/*
 * 		Y U V 4 2 0 P
 *
 * 		to
 *
 * 		B G R A
 *
 */

void		convert_yuv420p_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3);
}

void		convert_yuv420p_to_bgra_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}


void		convert_yuv420p_to_bgra_bt601_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3);
}

void		convert_yuv420p_to_bgra_bt601_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}


void		convert_yuv420p_to_bgra_bt709_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3);
}

void		convert_yuv420p_to_bgra_bt709_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}


/*
 *
 * 		Y U V 4 2 0 P
 *
 * 		to
 *
 * 		R G B   2 4
 */
void		convert_yuv420p_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv420p_to_rgb24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}


void		convert_yuv420p_to_rgb24_bt601_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv420p_to_rgb24_bt601_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}


void		convert_yuv420p_to_rgb24_bt709_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv420p_to_rgb24_bt709_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}


/*
 *
 * 		Y U V 4 2 0 P
 *
 * 		to
 *
 * 		B G R 2 4
 *
 */
void		convert_yuv420p_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv420p_to_bgr24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_, pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, sse2);
}


void		convert_yuv420p_to_bgr24_bt601_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv420p_to_bgr24_bt601_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_, pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, sse2);
}


void		convert_yuv420p_to_bgr24_bt709_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv420p_to_bgr24_bt709_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_, pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, sse2);
}

/*
 *
 * Non SSE conversion block (nearest neighbour upsampling)
 *
 */
#define 	DEFINE_YUV420P_TO_ANY_RGB_NONSSE_FN(fn_name, y_off, u_off, v_off, ru_coef, rv_coef, gu_coef, gv_coef, bu_coef, bv_coef) \
void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
	uint32_t			output_stride = ((dest_fmt == PixFcARGB) || (dest_fmt == PixFcBGRA)) ? 4 : 3;\
	uint32_t 			pixels_remaining_on_line = conv->width;\
	uint32_t			lines_remaining = conv->height;\
	uint32_t			pixel_count = conv->pixel_count;\
	uint8_t*			y_src_line1 = (uint8_t *) in;\
	uint8_t*			y_src_line2 = y_src_line1 + conv->width;\
	uint8_t*			u_src = y_src_line1 + pixel_count;\
	uint8_t*			v_src = u_src + pixel_count / 4;\
	uint8_t*			dst_line1 = (uint8_t *) out;\
	uint8_t*			dst_line2 = dst_line1 + conv->width * output_stride;\
	int32_t				r_line1, g_line1, b_line1;\
	int32_t				r_line2, g_line2, b_line2;\
	int32_t				y, u, v;\
	while(lines_remaining > 0){\
		while(pixels_remaining_on_line-- > 0) {\
			y = (*y_src_line1++ + y_off) << 8;\
			u = *u_src + u_off;\
			v = *v_src + v_off;\
			r_line1 = (y + (ru_coef * u) + (rv_coef * v)) >> 8;\
			g_line1 = (y + (gu_coef * u) + (gv_coef * v)) >> 8;\
			b_line1 = (y + (bu_coef * u) + (bv_coef * v)) >> 8;\
			y = (*y_src_line2++) << 8;\
			r_line2 = (y + (ru_coef * u) + (rv_coef * v)) >> 8;\
			g_line2 = (y + (gu_coef * u) + (gv_coef * v)) >> 8;\
			b_line2 = (y + (bu_coef * u) + (bv_coef * v)) >> 8;\
			if ((pixels_remaining_on_line & 0x1) == 0) {\
				u_src++;\
				v_src++;\
			}\
			if (dest_fmt == PixFcARGB) {\
				*(dst_line1++) = 0;\
				*(dst_line1++) = CLIP_PIXEL(r_line1);\
				*(dst_line1++) = CLIP_PIXEL(g_line1);\
				*(dst_line1++) = CLIP_PIXEL(b_line1);\
				*(dst_line2++) = 0;\
				*(dst_line2++) = CLIP_PIXEL(r_line2);\
				*(dst_line2++) = CLIP_PIXEL(g_line2);\
				*(dst_line2++) = CLIP_PIXEL(b_line2);\
			} else if (dest_fmt == PixFcBGRA) {\
				*(dst_line1++) = CLIP_PIXEL(b_line1);\
				*(dst_line1++) = CLIP_PIXEL(g_line1);\
				*(dst_line1++) = CLIP_PIXEL(r_line1);\
				*(dst_line1++) = 0;\
				*(dst_line2++) = CLIP_PIXEL(b_line2);\
				*(dst_line2++) = CLIP_PIXEL(g_line2);\
				*(dst_line2++) = CLIP_PIXEL(r_line2);\
				*(dst_line2++) = 0;\
			} else  if (dest_fmt == PixFcRGB24) {\
				*(dst_line1++) = CLIP_PIXEL(r_line1);\
				*(dst_line1++) = CLIP_PIXEL(g_line1);\
				*(dst_line1++) = CLIP_PIXEL(b_line1);\
				*(dst_line2++) = CLIP_PIXEL(r_line2);\
				*(dst_line2++) = CLIP_PIXEL(g_line2);\
				*(dst_line2++) = CLIP_PIXEL(b_line2);\
			} else {\
				*(dst_line1++) = CLIP_PIXEL(b_line1);\
				*(dst_line1++) = CLIP_PIXEL(g_line1);\
				*(dst_line1++) = CLIP_PIXEL(r_line1);\
				*(dst_line2++) = CLIP_PIXEL(b_line2);\
				*(dst_line2++) = CLIP_PIXEL(g_line2);\
				*(dst_line2++) = CLIP_PIXEL(r_line2);\
			}\
		}\
		pixels_remaining_on_line = conv->width;\
		lines_remaining -= 2;\
		y_src_line1 += conv->width;\
		y_src_line2 += conv->width;\
		dst_line1 += conv->width * output_stride;\
		dst_line2 += conv->width * output_stride;\
	}\
}

DEFINE_YUV420P_TO_ANY_RGB_NONSSE_FN(convert_yuv420p_to_any_rgb_nonsse, 0, -128, -128, 0, 359, -88, -183, 454, 0);

DEFINE_YUV420P_TO_ANY_RGB_NONSSE_FN(convert_yuv420p_to_any_rgb_bt601_nonsse, -16, -128, -128, 0, 408, -100, -208, 772, 0);

DEFINE_YUV420P_TO_ANY_RGB_NONSSE_FN(convert_yuv420p_to_any_rgb_bt709_nonsse, -16, -128, -128, 0, 459, -54, -136, 540, 0);


