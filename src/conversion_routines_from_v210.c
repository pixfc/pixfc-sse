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
void		upsample_n_convert_v210_to_argb_sse2_ssse3(const struct PixFcSSE * pixfc, void* source_buffer, void* dest_buffer) {
	UPSAMPLE_AND_CONVERT_TO_RGB32(pack_6_rgb_vectors_in_4_argb_vectors_sse2, sse2_ssse3);
}
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


/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		R G B   2 4
 */
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


/*
 *
 * 		V 2 1 0
 *
 * 		to
 *
 * 		B G R 2 4
 *
 */
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


/*
 *
 * Non SSE conversion block (nearest neighbour upsampling)
 *
 */
#define CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef) \
	r = (yRCoef * y + (uRCoef * u) + (vRCoef * v)) >> 8;\
	g = (yGCoef * y + (uGCoef * u) + (vGCoef * v)) >> 8;\
	b = (yBCoef * y + (uBCoef * u) + (vBCoef * v)) >> 8;\
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
	} else  {	/* PixFcBGR24 */\
		*(dst++) = CLIP_PIXEL(b);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(r);\
	}\

#define DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(fn_name, yOffset, uvOffset, yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint32_t*			src = (uint32_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y, u, v;\
		/* This conversion routines only assumes there is an even number of pixels. */\
		while(line++ < conv->height) {\
			/* Convert as many chunks of 6 pixels as possible,
			   until less than 6 pixels remain. */\
			while(pixel < (conv->width - 5)) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF) - yOffset;\
				u = (*src & 0x3FF) - uvOffset;\
				v = ((*src >> 20) & 0x3FF) - uvOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				y = (src[1] & 0x3FF) - yOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF) - yOffset;\
				u = ((src[1] >> 10) & 0x3FF) - uvOffset;\
				v = (src[2] & 0x3FF) - uvOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				y = ((src[2] >> 10) & 0x3FF) - yOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF) - yOffset;\
				u = ((src[2] >> 20) & 0x3FF) - uvOffset;\
				v = ((src[3] >> 10) & 0x3FF) - uvOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				y = ((src[3] >> 20) & 0x3FF) - yOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				src += 4;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since yuv422
			   formats (inc v210) carry at the very least an even
			   number of pixels. */\
			if ((conv->width - pixel) == 2) {\
				y = ((*src >> 10) & 0x3FF) - yOffset;\
				u = (*src & 0x3FF) - uvOffset;\
				v = ((*src >> 20) & 0x3FF) - uvOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				y = (src[1] & 0x3FF) - yOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
			} else if ((conv->width - pixel) == 4) {\
				y = ((*src >> 10) & 0x3FF) - yOffset;\
				u = (*src & 0x3FF) - uvOffset;\
				v = ((*src >> 20) & 0x3FF) - uvOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				y = (src[1] & 0x3FF) - yOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF) - yOffset;\
				u = ((src[1] >> 10) & 0x3FF) - uvOffset;\
				v = (src[2] & 0x3FF) - uvOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
				y = ((src[2] >> 10) & 0x3FF) - yOffset;\
				CONVERT_N_STORE(yRCoef, uRCoef, vRCoef, yGCoef, uGCoef, vGCoef, yBCoef, uBCoef, vBCoef);\
			}\
			src = (uint32_t*) ((uint8_t*)in + line * ROW_SIZE(PixFcV210, conv->width));\
			pixel = 0;\
		}\
	}

// Full range
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_rgb_nonsse,
		0, 512,
		64, 0, 90,
		64, -22, -46,
		64, 113, 0)

// bt601
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_rgb_bt601_nonsse,
		64, 512,
		74, 0, 102,
		74, -25, -52,
		74, 129, 0)

// bt709
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_rgb_bt709_nonsse,
		64, 512,
		74, 0, 115,
		74, -14, -34,
		74, 135, 0)





/*
 *
 *	V 2 1 0
 *
 *	T O
 *
 *	Y U V 4 2 2 I
 *
 *
 */
// V210 to YUYU
void		convert_v210_to_yuyv_sse2_ssse3_sse41(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3_sse41);
}

void		convert_v210_to_yuyv_sse2_ssse3(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

// V210 to UYVY
void		convert_v210_to_uyvy_sse2_ssse3_sse41(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3_sse41);
}

void		convert_v210_to_uyvy_sse2_ssse3(const struct PixFcSSE* pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(V210_TO_YUV422I_RECIPE, unpack_4v_v210_to_y_uv_vectors_, pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

#define PACK_TO_YUV422P(y1, u, v, y2, dst) \
	if (dest_fmt == PixFcYUYV) {\
		*dst++ = y1;\
		*dst++ = u;\
		*dst++ = y2;\
		*dst++ = v;\
	} else if (dest_fmt == PixFcUYVY) {\
		*dst++ = u;\
		*dst++ = y1;\
		*dst++ = v;\
		*dst++ = y2;\
	} else {\
		dprint("Unknown destination pixel format\n");\
	}

void		convert_v210_to_yuv422i_nonsse(const struct PixFcSSE* conv, void* srcBuffer, void* dstBuffer) {
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;
	uint32_t 			pixel = 0;
	uint32_t			line = conv->height;
	uint32_t*			src = (uint32_t *) srcBuffer;
	uint8_t*			dst = (uint8_t *) dstBuffer;
	int32_t				y1, y2, u, v;

	// This conversion routines only assumes there is an even number of pixels.

	while(line++ < conv->height) {
		// Convert as many chunks of 6 pixels as possible,
		// until less than 6 pixels remain.
		while(pixel < (conv->width - 5)) {
			u = (*src >> 2) & 0xFF;
			y1 = (*src >> 12) & 0xFF;
			v = (*src >> 22) & 0xFF;
			y2 = (src[1] >> 2) & 0xFF;
			PACK_TO_YUV422P(y1, u, v, y2, dst);

			u = (src[1] >> 12) & 0xFF;
			y1 = (src[1] >> 22) & 0xFF;
			v = (src[2] >> 2) & 0xFF;
			y2 = (src[2] >> 12) & 0xFF;
			PACK_TO_YUV422P(y1, u, v, y2, dst);

			u = (src[2] >> 22) & 0xFF;
			y1 = (src[3] >> 2) & 0xFF;
			v = (src[3] >> 12) & 0xFF;
			y2 = (src[3] >> 22) & 0xFF;
			PACK_TO_YUV422P(y1, u, v, y2, dst);

			pixel += 6;
			src += 4;
		}

		// There can only be 2 or 4 pixels left since yuv422
		// formats (inc v210) carry at the very least an even
		// number of pixels.
		if ((conv->width - pixel) == 2) {
			u = (*src >> 2) & 0xFF;
			y1 = (*src >> 12) & 0xFF;
			v = (*src >> 22) & 0xFF;
			y2 = (src[1] >> 2) & 0xFF;
			PACK_TO_YUV422P(y1, u, v, y2, dst);
		} else if ((conv->width - pixel) == 4) {
			u = (*src >> 2) & 0xFF;
			y1 = (*src >> 12) & 0xFF;
			v = (*src >> 22) & 0xFF;
			y2 = (src[1] >> 2) & 0xFF;
			PACK_TO_YUV422P(y1, u, v, y2, dst);

			u = (src[1] >> 12) & 0xFF;
			y1 = (src[1] >> 22) & 0xFF;
			v = (src[2] >> 2) & 0xFF;
			y2 = (src[2] >> 12) & 0xFF;
			PACK_TO_YUV422P(y1, u, v, y2, dst);
		}

		src = (uint32_t*) ((uint8_t*)srcBuffer + line * ROW_SIZE(PixFcV210, conv->width));
		pixel = 0;
	}

}
