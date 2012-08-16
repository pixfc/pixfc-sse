/*
 * conversion_routines_from_yuyv.c
 *
 * Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
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
				UPSAMPLE_YUV422I_TO_RGB_RECIPE,\
				unpack_yuyv_to_y_uv_vectors_,\
				pack_fn,\
				convert_y_uv_vectors_to_rgb_vectors_,\
				4,\
				instr_set\
		)


#define UPSAMPLE_AND_CONVERT_TO_RGB24(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				UPSAMPLE_YUV422I_TO_RGB_RECIPE,\
				unpack_yuyv_to_y_uv_vectors_,\
				pack_fn,\
				convert_y_uv_vectors_to_rgb_vectors_,\
				3,\
				instr_set\
		)



#define CONVERT_TO_RGB32(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				YUV422I_TO_RGB_RECIPE,\
				unpack_yuyv_to_y_uv_vectors_,\
				pack_fn,\
				nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_,\
				4,\
				instr_set\
		)

#define CONVERT_TO_RGB24(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				YUV422I_TO_RGB_RECIPE,\
				unpack_yuyv_to_y_uv_vectors_,\
				pack_fn,\
				nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_,\
				3,\
				instr_set\
		)

#define UPSAMPLE_AND_CONVERT_TO_R210(...) UPSAMPLE_AND_CONVERT_TO_RGB32(__VA_ARGS__)
#define CONVERT_TO_R210(...) CONVERT_TO_RGB32(__VA_ARGS__)

/*
 *
 * 		Y U Y V
 *
 * 		to
 *
 * 		A R G B
 */
void		upsample_n_convert_yuyv_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	 UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}

void		convert_yuyv_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2,	sse2_ssse3);
}

void		upsample_n_convert_yuyv_to_argb_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}

void		convert_yuyv_to_argb_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2);
}

/*
 * 		Y U Y V
 *
 * 		to
 *
 * 		B G R A
 *
 */
void		upsample_n_convert_yuyv_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2_ssse3);
}

void		convert_yuyv_to_bgra_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2,	sse2_ssse3);
}

void		upsample_n_convert_yuyv_to_bgra_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}

void		convert_yuyv_to_bgra_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_bgra_vectors_sse2, sse2);
}


/*
 *
 * 		Y U Y V
 *
 * 		to
 *
 * 		R G B   2 4
 */
void		upsample_n_convert_yuyv_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuyv_to_rgb24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, sse2_ssse3);
}

void		upsample_n_convert_yuyv_to_rgb24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}

void		convert_yuyv_to_rgb24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, sse2);
}

/*
 *
 * 		Y U V Y
 *
 * 		to
 *
 * 		B G R 2 4
 *
 */
void		upsample_n_convert_yuyv_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		convert_yuyv_to_bgr24_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB24(pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, sse2_ssse3);
}

void		upsample_n_convert_yuyv_to_bgr24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB24(pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, sse2);
}

void		convert_yuyv_to_bgr24_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
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
void		upsample_n_convert_yuyv_to_r210_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_r_g_b_vectors_to_4_r210_sse2_ssse3, sse2_ssse3);
}

void		convert_yuyv_to_r210_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_RGB32(pack_6_r_g_b_vectors_to_4_r210_sse2_ssse3, sse2_ssse3);
}


/*
 *
 * Non SSE conversion block (nearest neighbour upsampling)
 *
 */


#define 			UNPACK_YUV422I(y1, u, v, y2, src)\
	if(src_fmt == PixFcYUYV){\
		y1 = *src++;\
		u = *src++;\
		y2 = *src++;\
		v = *src++;\
	} else if (src_fmt == PixFcUYVY) {\
		u = *src++;\
		y1 = *src++;\
		v = *src++;\
		y2 = *src++;\
	} else {\
		printf("unknown source pixel format\n");\
	}\

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
	} else {\
		printf("unknown rgb destination format\n");\
	}


#define DEFINE_YUV422I_TO_ANY_RGB(fn_name, coeffs, coef_shift, offsets)\
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		uint32_t 			pixel_num = 0;\
		uint32_t			pixel_count = conv->pixel_count;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y1, u, v, y2;\
		while(pixel_num < pixel_count){\
			UNPACK_YUV422I(y1, u, v, y2, src);\
			y1 += offsets[0];\
			y2 += offsets[0];\
			u += offsets[1];\
			v += offsets[2];\
			r = ((coeffs[0][0] * y1) + (coeffs[0][1] * u) + (coeffs[0][2] * v)) >> coef_shift;\
			g = ((coeffs[1][0] * y1) + (coeffs[1][1] * u) + (coeffs[1][2] * v)) >> coef_shift;\
			b = ((coeffs[2][0] * y1) + (coeffs[2][1] * u) + (coeffs[2][2] * v)) >> coef_shift;\
			PACK_RGB(r, g, b, dst);\
			r = ((coeffs[0][0] * y2) + (coeffs[0][1] * u) + (coeffs[0][2] * v)) >> coef_shift;\
			g = ((coeffs[1][0] * y2) + (coeffs[1][1] * u) + (coeffs[1][2] * v)) >> coef_shift;\
			b = ((coeffs[2][0] * y2) + (coeffs[2][1] * u) + (coeffs[2][2] * v)) >> coef_shift;\
			PACK_RGB(r, g, b, dst);\
			pixel_num += 2;\
		}\
	}

DEFINE_YUV422I_TO_ANY_RGB(convert_yuv422i_to_any_rgb_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[0], 8, yuv_8bit_to_rgb_8bit_off[0]);
DEFINE_YUV422I_TO_ANY_RGB(convert_yuv422i_to_any_rgb_bt601_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[1], 8, yuv_8bit_to_rgb_8bit_off[1]);
DEFINE_YUV422I_TO_ANY_RGB(convert_yuv422i_to_any_rgb_bt709_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[2], 8, yuv_8bit_to_rgb_8bit_off[2]);


#define DEFINE_UPSAMPLE_N_CONVERT_YUV422I_TO_ANY_RGB(fn_name, coeffs, coef_shift, offsets)\
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		uint32_t 			pixel_num = 0;\
		uint32_t			pixel_count = conv->pixel_count;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y1, u, v, y2;\
		int32_t				next_y1, next_u, next_v, next_y2;\
		UNPACK_YUV422I(y1, u, v, y2, src);\
		while(pixel_num < pixel_count - 2){ /* handle the last 2 pixels outside the loop*/\
			r = ((coeffs[0][0] * (y1 + offsets[0])) + (coeffs[0][1] * (u + offsets[1])) + (coeffs[0][2] * (v + offsets[2]))) >> coef_shift;\
			g = ((coeffs[1][0] * (y1 + offsets[0])) + (coeffs[1][1] * (u + offsets[1])) + (coeffs[1][2] * (v + offsets[2]))) >> coef_shift;\
			b = ((coeffs[2][0] * (y1 + offsets[0])) + (coeffs[2][1] * (u + offsets[1])) + (coeffs[2][2] * (v + offsets[2]))) >> coef_shift;\
			PACK_RGB(r, g, b, dst);\
			UNPACK_YUV422I(next_y1, next_u, next_v, next_y2, src);\
			r = ((coeffs[0][0] * (y2 + offsets[0])) + (coeffs[0][1] * ((u + next_u) / 2 + offsets[1])) + (coeffs[0][2] * ((v + next_v) / 2 + offsets[2]))) >> coef_shift;\
			g = ((coeffs[1][0] * (y2 + offsets[0])) + (coeffs[1][1] * ((u + next_u) / 2 + offsets[1])) + (coeffs[1][2] * ((v + next_v) / 2 + offsets[2]))) >> coef_shift;\
			b = ((coeffs[2][0] * (y2 + offsets[0])) + (coeffs[2][1] * ((u + next_u) / 2 + offsets[1])) + (coeffs[2][2] * ((v + next_v) / 2 + offsets[2]))) >> coef_shift;\
			PACK_RGB(r, g, b, dst);\
			y1 = next_y1;\
			y2 = next_y2;\
			u = next_u;\
			v = next_v;\
			pixel_num += 2;\
		}\
		r = ((coeffs[0][0] * (y1 + offsets[0])) + (coeffs[0][1] * (u + offsets[1])) + (coeffs[0][2] * (v + offsets[2]))) >> coef_shift;\
		g = ((coeffs[1][0] * (y1 + offsets[0])) + (coeffs[1][1] * (u + offsets[1])) + (coeffs[1][2] * (v + offsets[2]))) >> coef_shift;\
		b = ((coeffs[2][0] * (y1 + offsets[0])) + (coeffs[2][1] * (u + offsets[1])) + (coeffs[2][2] * (v + offsets[2]))) >> coef_shift;\
		PACK_RGB(r, g, b, dst);\
		r = ((coeffs[0][0] * (y2 + offsets[0])) + (coeffs[0][1] * (u + offsets[1])) + (coeffs[0][2] * (v + offsets[2]))) >> coef_shift;\
		g = ((coeffs[1][0] * (y2 + offsets[0])) + (coeffs[1][1] * (u + offsets[1])) + (coeffs[1][2] * (v + offsets[2]))) >> coef_shift;\
		b = ((coeffs[2][0] * (y2 + offsets[0])) + (coeffs[2][1] * (u + offsets[1])) + (coeffs[2][2] * (v + offsets[2]))) >> coef_shift;\
		PACK_RGB(r, g, b, dst);\
	}

DEFINE_UPSAMPLE_N_CONVERT_YUV422I_TO_ANY_RGB(upsample_n_convert_yuv422i_to_any_rgb_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[0], 8, yuv_8bit_to_rgb_8bit_off[0]);
DEFINE_UPSAMPLE_N_CONVERT_YUV422I_TO_ANY_RGB(upsample_n_convert_yuv422i_to_any_rgb_bt601_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[1], 8, yuv_8bit_to_rgb_8bit_off[1]);
DEFINE_UPSAMPLE_N_CONVERT_YUV422I_TO_ANY_RGB(upsample_n_convert_yuv422i_to_any_rgb_bt709_nonsse, yuv_8bit_to_rgb_8bit_coef_lhs8[2], 8, yuv_8bit_to_rgb_8bit_off[2]);


/*
 * Original yuv to rgb conversion - left here for ref
 */
#ifdef __INTEL_CPU__
void 		convert_yuyv_to_rgb_original(const struct PixFcSSE* conv, void* in, void* out){
	__m128i*	yuyv_8pixels = (__m128i *) in;
	__m128i*	rgb_out_buf = (__m128i *) out;
	uint32_t	pixel_count = conv->pixel_count;
	__m128i		unpack_out[2];
	__m128i		convert_out[6];
	while(pixel_count > 0) {
		unpack_yuyv_to_y_u_v_vectors_sse2(yuyv_8pixels, unpack_out);
		nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_sse2(unpack_out, convert_out);
		unpack_yuyv_to_y_u_v_vectors_sse2(&yuyv_8pixels[1], unpack_out);
		nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_sse2(unpack_out, &convert_out[3]);
		pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking(convert_out, rgb_out_buf);
		yuyv_8pixels += 2;
		rgb_out_buf += 3;
		pixel_count -= 16;
	};
}
#endif


/*
 *  	Y U Y V
 *
 * 		T O
 *
 * 		Y U V 4 2 2 P
 */
void		convert_yuyv_to_yuv422p_sse2(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK(YUV422I_TO_YUV422P_RECIPE, repack_yuyv_to_yuv422p_, sse2);
}

void		convert_yuyv_to_yuv422p_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK(YUV422I_TO_YUV422P_RECIPE, repack_yuyv_to_yuv422p_, sse2_ssse3);
}

void		convert_yuv422i_to_yuv422p_nonsse(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	uint32_t 	pixel_count = pixfc->pixel_count;
	uint8_t *	src = (uint8_t *)source_buffer;
	uint8_t *	y_plane = (uint8_t *) dest_buffer;
	uint8_t *	u_plane = y_plane + pixel_count;
	uint8_t *	v_plane = u_plane + pixel_count / 2;

	// Do conversion
	if (pixfc->source_fmt == PixFcYUYV)
		while(pixel_count > 0) {
			*y_plane++ = *src++;
			*u_plane++ = *src++;
			*y_plane++ = *src++;
			*v_plane++ = *src++;

			pixel_count -= 2;
		}
	else if (pixfc->source_fmt == PixFcUYVY)
		while(pixel_count > 0) {
			*u_plane++ = *src++;
			*y_plane++ = *src++;
			*v_plane++ = *src++;
			*y_plane++ = *src++;

			pixel_count -= 2;
		}
	else
		printf("unknown source pixel format\n");
}


/*
 *
 *	Y U Y V
 *
 *	T O
 *
 *	V 2 1 0
 *
 *
 */
// YUYV to V210
void		convert_yuyv_to_v210_sse2_ssse3_sse41(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(YUV422I_TO_V210_RECIPE, unpack_yuyv_to_y_uv_vectors_sse2_ssse3, pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3_sse41);
}

void		convert_yuyv_to_v210_sse2_ssse3(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(YUV422I_TO_V210_RECIPE, unpack_yuyv_to_y_uv_vectors_sse2_ssse3, pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3);
}

void		convert_yuv422i_to_v210_nonsse(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	PixFcPixelFormat 	src_fmt = pixfc->source_fmt;
	uint32_t 			line = 0;
	uint32_t 			pixel = 0;
	uint8_t*			src = (uint8_t *) source_buffer;
	uint32_t*			dst = (uint32_t *) dest_buffer;
	int32_t				y1, y2, u, v;

	// This conversion function only assumes an even number of pixels

	while(line++ < pixfc->height){
		// Convert as many chunks of 6 pixels as possible
		while(pixel < pixfc->width - 5) {
			UNPACK_YUV422I(y1, u, v, y2, src);
			*dst = u << 2;
			//printf("M6 %d %d %d %d\n", (u<<2), (y1<<2), (v<<2), (y2<<2));
			*dst |= y1 << 12;
			*(dst++) |= v << 22;
			*dst = y2 << 2;
			UNPACK_YUV422I(y1, u, v, y2, src);
			*dst |= u << 12;
			*(dst++) |= y1 << 22;
			*dst = v << 2;
			*dst |= y2 << 12;
			//printf("M6 %d %d %d %d\n", (u<<2), (y1<<2), (v<<2), (y2<<2));
			UNPACK_YUV422I(y1, u, v, y2, src);
			*(dst++) |= u << 22;
			*dst = y1 << 2;
			*dst |= v << 12;
			*(dst++) |= y2 << 22;
			//printf("M6 %d %d %d %d\n", (u<<2), (y1<<2), (v<<2), (y2<<2));
			pixel += 6;
		}

		// There can only be 2 or 4 pixels left since yuv422i
		// formats carry at the very least an even number of pixels.
		if ((pixfc->width - pixel) == 2) {
			UNPACK_YUV422I(y1, u, v, y2, src);
			*dst = u << 2;
			*dst |= y1 << 12;
			*(dst++) |= v << 22;
			*dst = y2 << 2;
			//printf("L2 %d %d %d %d\n", (u<<2), (y1<<2), (v<<2), (y2<<2));
		} else if ((pixfc->width - pixel) == 4) {
			UNPACK_YUV422I(y1, u, v, y2, src);
			*dst = u << 2;
			*dst |= y1 << 12;
			*(dst++) |= v << 22;
			*dst = y2 << 2;
			//printf("L4 %d %d %d %d\n", (u<<2), (y1<<2), (v<<2), (y2<<2));
			UNPACK_YUV422I(y1, u, v, y2, src);
			*dst |= u << 12;
			*(dst++) |= y1 << 22;
			*dst = v << 2;
			*dst |= y2 << 12;
			//printf("L4 %d %d %d %d\n", (u<<2), (y1<<2), (v<<2), (y2<<2));
		}

		dst = (uint32_t*) ((uint8_t*)dest_buffer + line * ROW_SIZE(PixFcV210, pixfc->width));
		pixel = 0;
	}
}

