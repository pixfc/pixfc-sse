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
#include "yuv_conversion_recipes.h"

#define UPSAMPLE_AND_CONVERT_TO_RGB32(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				UPSAMPLE_YUV422P_TO_RGB_RECIPE,\
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
				UPSAMPLE_YUV422P_TO_RGB_RECIPE,\
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
				YUV422P_TO_RGB_RECIPE,\
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
				YUV422P_TO_RGB_RECIPE,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_,\
				3,\
				instr_set\
		)

#define UPSAMPLE_AND_CONVERT_TO_RGB10(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				UPSAMPLE_YUV422P_TO_RGB_RECIPE,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				convert_8bit_y_uv_vectors_to_10bit_rgb_vectors_,\
				4,\
				instr_set\
		)

#define CONVERT_TO_RGB10(pack_fn, instr_set)\
		DO_CONVERSION_3U_1P(\
				YUV422P_TO_RGB_RECIPE,\
				unpack_yuv42Xp_to_2_y_vectors_sse2,\
				unpack_low_yuv42Xp_to_uv_vector_sse2,\
				unpack_high_yuv42Xp_to_uv_vector_sse2,\
				pack_fn,\
				nnb_upsample_n_convert_8bit_y_uv_vectors_to_10bit_rgb_vectors_,\
				4,\
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
 * 		Y U V Y
 *
 * 		to
 *
 * 		R 2 1 0
 *
 */
void		upsample_n_convert_yuv422p_to_r210_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB10(pack_6_r_g_b_vectors_to_4_r210_sse2_ssse3, sse2_ssse3);
}

void		convert_yuv422p_to_r210_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB10(pack_6_r_g_b_vectors_to_4_r210_sse2_ssse3, sse2_ssse3);
}


/*
 *
 * Non SSE conversion blocks (nearest neighbour upsampling)
 *
 */
#define PACK_RGB(r, g, b, dst) \
	if (dest_fmt == PixFcARGB) {\
		*(dst++) = 0;\
		*(dst++) = CLIP_PIXEL(r);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(b);\
	} else if (dest_fmt == PixFcBGRA) {\
		*(dst++) = CLIP_PIXEL(b);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(r);\
		*(dst++) = 0;\
	} else  if (dest_fmt == PixFcRGB24) {\
		*(dst++) = CLIP_PIXEL(r);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(b);\
	} else if (dest_fmt == PixFcBGR24) {\
		*(dst++) = CLIP_PIXEL(b);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(r);\
	} else if (dest_fmt == PixFcR210) {\
		uint32_t tmp;\
		uint8_t *p = (uint8_t *) &tmp;\
		dprint("R: %d G: %d B: %d\n", CLIP_10BIT_PIXEL(r), CLIP_10BIT_PIXEL(g), CLIP_10BIT_PIXEL(b));\
		tmp = CLIP_10BIT_PIXEL(b);\
		tmp |= ((CLIP_10BIT_PIXEL(g)) << 10);\
		tmp |= ((CLIP_10BIT_PIXEL(r)) << 20);\
		*(dst++) = p[3];\
		*(dst++) = p[2];\
		*(dst++) = p[1];\
		*(dst++) = p[0];\
	} else {\
		printf("unknown rgb destination format\n");\
	}

// These conversion routines make no assumption on the number of pixels
#define DEFINE_YUV422P_TO_ANY_RGB(fn_name, coeffs, coef_shift, offsets)\
	void	fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		uint32_t            pixels_remaining;\
		uint32_t            lines_remaining = conv->height;\
		DECLARE_PADDING_BYTE_COUNT(padding_bytes, dest_fmt, conv->width);\
		uint8_t*			y_src = (uint8_t *) in;\
		uint8_t*			u_src = y_src + conv->pixel_count;\
		uint8_t*			v_src = u_src + conv->pixel_count / 2;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y, u, v;\
		while(lines_remaining-- > 0) {\
			pixels_remaining = conv->width;\
			while(pixels_remaining > 0){\
				y = *y_src++ + offsets[0];\
				u = *u_src + offsets[1];\
				v = *v_src + offsets[2];\
				\
				if ((pixels_remaining & 0x1) != 0) {\
					u_src++;\
					v_src++;\
				}\
				\
				r = ((y * coeffs[0][0])+ (u * coeffs[0][1]) +(v * coeffs[0][2])) >> coef_shift;\
				g = ((y * coeffs[1][0])+ (u * coeffs[1][1]) +(v * coeffs[1][2])) >> coef_shift;\
				b = ((y * coeffs[2][0])+ (u * coeffs[2][1]) +(v * coeffs[2][2])) >> coef_shift;\
				\
				PACK_RGB(r, g, b, dst);\
				pixels_remaining--;\
			}\
			dst += padding_bytes;\
		}\
	}

DEFINE_YUV422P_TO_ANY_RGB(convert_yuv422p_to_any_rgb_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[0], 8, yuv_8bit_to_rgb_8bit_off[0])
DEFINE_YUV422P_TO_ANY_RGB(convert_yuv422p_to_any_rgb_bt601_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[1], 8, yuv_8bit_to_rgb_8bit_off[1])
DEFINE_YUV422P_TO_ANY_RGB(convert_yuv422p_to_any_rgb_bt709_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[2], 8, yuv_8bit_to_rgb_8bit_off[2])
DEFINE_YUV422P_TO_ANY_RGB(convert_yuv422p_to_any_10bit_rgb_nonsse, yuv_8bit_to_rgb_10bit_coef_lhs8[0], 8, yuv_8bit_to_rgb_10bit_off[0])
DEFINE_YUV422P_TO_ANY_RGB(convert_yuv422p_to_any_10bit_rgb_bt601_nonsse, yuv_8bit_to_rgb_10bit_coef_lhs8[1], 8, yuv_8bit_to_rgb_10bit_off[1])
DEFINE_YUV422P_TO_ANY_RGB(convert_yuv422p_to_any_10bit_rgb_bt709_nonsse, yuv_8bit_to_rgb_10bit_coef_lhs8[2], 8, yuv_8bit_to_rgb_10bit_off[2])



// These conversion routines only assume an even number of pixels
#define DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(fn_name, coeffs, coef_scale, offsets)\
	void	fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		DECLARE_PADDING_BYTE_COUNT(padding_bytes, dest_fmt, conv->width);\
		uint32_t 			lines_remaining = conv->height;\
		uint32_t			pixels_remaining;\
		uint8_t*			y_src = (uint8_t *) in;\
		uint8_t*			u_src = y_src + conv->pixel_count;\
		uint8_t*			v_src = u_src + conv->pixel_count / 2;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y1, y2, u, v, next_u, next_v;\
		while(lines_remaining-- > 0){\
			pixels_remaining = conv->width - 2; /* handle the last 2 pixels outside the loop*/\
			u = *u_src++;\
			v = *v_src++;\
			while(pixels_remaining > 0){\
				y1 = *y_src++ + offsets[0];\
				y2 = *y_src++ + offsets[0];\
				\
				r = ((y1 * coeffs[0][0])+ ((u + offsets[1]) * coeffs[0][1]) +((v + offsets[2]) * coeffs[0][2])) / coef_scale;\
				g = ((y1 * coeffs[1][0])+ ((u + offsets[1]) * coeffs[1][1]) +((v + offsets[2]) * coeffs[1][2])) / coef_scale;\
				b = ((y1 * coeffs[2][0])+ ((u + offsets[1]) * coeffs[2][1]) +((v + offsets[2]) * coeffs[2][2])) / coef_scale;\
				PACK_RGB(r, g, b, dst);\
				\
				next_u = *u_src++;\
				next_v = *v_src++;\
				r = ((y2 * coeffs[0][0])+ (((u + next_u) / 2.0f + offsets[1]) * coeffs[0][1]) +(((v + next_v) / 2.0f + offsets[2]) * coeffs[0][2])) / coef_scale;\
				g = ((y2 * coeffs[1][0])+ (((u + next_u) / 2.0f + offsets[1]) * coeffs[1][1]) +(((v + next_v) / 2.0f + offsets[2]) * coeffs[1][2])) / coef_scale;\
				b = ((y2 * coeffs[2][0])+ (((u + next_u) / 2.0f + offsets[1]) * coeffs[2][1]) +(((v + next_v) / 2.0f + offsets[2]) * coeffs[2][2])) / coef_scale;\
				PACK_RGB(r, g, b, dst);\
				\
				u = next_u;\
				v = next_v;\
				pixels_remaining -= 2;\
			}\
			y1 = *y_src++ + offsets[0];\
			y2 = *y_src++ + offsets[0];\
			\
			r = ((y1 * coeffs[0][0])+ ((u + offsets[1]) * coeffs[0][1]) +((v + offsets[2]) * coeffs[0][2])) / coef_scale;\
			g = ((y1 * coeffs[1][0])+ ((u + offsets[1]) * coeffs[1][1]) +((v + offsets[2]) * coeffs[1][2])) / coef_scale;\
			b = ((y1 * coeffs[2][0])+ ((u + offsets[1]) * coeffs[2][1]) +((v + offsets[2]) * coeffs[2][2])) / coef_scale;\
			PACK_RGB(r, g, b, dst);\
			\
			r = ((y2 * coeffs[0][0])+ ((u + offsets[1]) * coeffs[0][1]) +((v + offsets[2]) * coeffs[0][2])) / coef_scale;\
			g = ((y2 * coeffs[1][0])+ ((u + offsets[1]) * coeffs[1][1]) +((v + offsets[2]) * coeffs[1][2])) / coef_scale;\
			b = ((y2 * coeffs[2][0])+ ((u + offsets[1]) * coeffs[2][1]) +((v + offsets[2]) * coeffs[2][2])) / coef_scale;\
			PACK_RGB(r, g, b, dst);\
			dst += padding_bytes;\
		}\
	}

DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(upsample_n_convert_yuv422p_to_any_rgb_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[0], 256.0f, yuv_8bit_to_rgb_8bit_off[0])
DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(upsample_n_convert_yuv422p_to_any_rgb_bt601_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[1], 256.0f, yuv_8bit_to_rgb_8bit_off[1])
DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(upsample_n_convert_yuv422p_to_any_rgb_bt709_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[2], 256.0f, yuv_8bit_to_rgb_8bit_off[2])
DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(upsample_n_convert_yuv422p_to_any_10bit_rgb_nonsse, yuv_8bit_to_rgb_10bit_coef_lhs8[0], 256.0f, yuv_8bit_to_rgb_10bit_off[0])
DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(upsample_n_convert_yuv422p_to_any_10bit_rgb_bt601_nonsse, yuv_8bit_to_rgb_10bit_coef_lhs8[1], 256.0f, yuv_8bit_to_rgb_10bit_off[1])
DEFINE_UPSAMPLE_N_YUV422P_TO_ANY_RGB(upsample_n_convert_yuv422p_to_any_10bit_rgb_bt709_nonsse, yuv_8bit_to_rgb_10bit_coef_lhs8[2], 256.0f, yuv_8bit_to_rgb_10bit_off[2])


/*
 * 		Y U V 4 2 2 P
 *
 * 		T O
 *
 * 		Y U Y V  /  U V Y V
 *
 */
void		convert_yuv422p_to_yuyv_sse2(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer){
	DO_REPACK(YUV422P_TO_YUV422I_RECIPE, repack_yuv422p_to_yuyv_, sse2);
}

void		convert_yuv422p_to_yuyv_nonsse(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer){
	uint32_t 	pixel_count = pixfc->pixel_count;
	uint8_t *	y_plane = (uint8_t *) source_buffer;
	uint8_t *	u_plane = y_plane + pixel_count;
	uint8_t *	v_plane = u_plane + pixel_count / 2;
	uint8_t *	dst = (uint8_t *) dest_buffer;

	// Do conversion
	while(pixel_count > 0) {
		*dst++ = *y_plane++;
		*dst++ = *u_plane++;
		*dst++ = *y_plane++;
		*dst++ = *v_plane++;

		pixel_count -= 2;
	}
}

void		convert_yuv422p_to_uyvy_sse2(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer){
	DO_REPACK(YUV422P_TO_YUV422I_RECIPE, repack_yuv422p_to_uyvy_, sse2);
}

void		convert_yuv422p_to_uyvy_nonsse(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer){
	uint32_t 	pixel_count = pixfc->pixel_count;
	uint8_t *	y_plane = (uint8_t *) source_buffer;
	uint8_t *	u_plane = y_plane + pixel_count;
	uint8_t *	v_plane = u_plane + pixel_count / 2;
	uint8_t *	dst = (uint8_t *) dest_buffer;

	// Do conversion
	while(pixel_count > 0) {
		*dst++ = *u_plane++;
		*dst++ = *y_plane++;
		*dst++ = *v_plane++;
		*dst++ = *y_plane++;

		pixel_count -= 2;
	}
}


