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
#include "pixfmt_descriptions.h"

/*
 *
 * Non SSE conversion block (nearest neighbour upsampling)
 *
 */
#define PACK_RGB() \
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
		} else if (dest_fmt == PixFcRGB24) {\
			*(dst++) = CLIP_PIXEL(r);\
			*(dst++) = CLIP_PIXEL(g);\
			*(dst++) = CLIP_PIXEL(b);\
		} else if (dest_fmt == PixFcBGR24) {\
			*(dst++) = CLIP_PIXEL(b);\
			*(dst++) = CLIP_PIXEL(g);\
			*(dst++) = CLIP_PIXEL(r);\
		} else if (dest_fmt == PixFcR210) {\
			uint32_t temp;\
			uint8_t *tmp8 = (uint8_t *) &temp;\
			temp = ((uint32_t)CLIP_10BIT_PIXEL(b) & 0x3FF);\
			temp |= (((uint32_t)CLIP_10BIT_PIXEL(g) & 0x3FF) << 10);\
			temp |= (((uint32_t)CLIP_10BIT_PIXEL(r) & 0x3FF) << 20);\
			*dst++ = tmp8[3];\
			*dst++ = tmp8[2];\
			*dst++ = tmp8[1];\
			*dst++ = tmp8[0];\
		} else if (dest_fmt == PixFcR10k) {\
		uint32_t temp;\
		uint8_t *tmp8 = (uint8_t *) &temp;\
		temp = (((uint32_t)CLIP_10BIT_PIXEL(b) & 0x3FF) << 2);\
		temp |= (((uint32_t)CLIP_10BIT_PIXEL(g) & 0x3FF) << 12);\
		temp |= (((uint32_t)CLIP_10BIT_PIXEL(r) & 0x3FF) << 22);\
		*dst++ = tmp8[3];\
		*dst++ = tmp8[2];\
		*dst++ = tmp8[1];\
		*dst++ = tmp8[0];\
		} else {\
			printf("Unknown RGB pixel format\n");\
		}



#define CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v) \
	r = ((coeffs[0][0] * ((y) + offsets[0])) + (coeffs[0][1] * ((u) + offsets[1])) + (coeffs[0][2] * ((v) + offsets[2]))) >> coef_shift;\
	g = ((coeffs[1][0] * ((y) + offsets[0])) + (coeffs[1][1] * ((u) + offsets[1])) + (coeffs[1][2] * ((v) + offsets[2]))) >> coef_shift;\
	b = ((coeffs[2][0] * ((y) + offsets[0])) + (coeffs[2][1] * ((u) + offsets[1])) + (coeffs[2][2] * ((v) + offsets[2]))) >> coef_shift;\
	PACK_RGB()

#define CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v) \
	r = (coeffs[0][0] * ((y) + offsets[0])) + (coeffs[0][1] * ((u) + offsets[1])) + (coeffs[0][2] * ((v) + offsets[2]));\
	g = (coeffs[1][0] * ((y) + offsets[0])) + (coeffs[1][1] * ((u) + offsets[1])) + (coeffs[1][2] * ((v) + offsets[2]));\
	b = (coeffs[2][0] * ((y) + offsets[0])) + (coeffs[2][1] * ((u) + offsets[1])) + (coeffs[2][2] * ((v) + offsets[2]));\
	PACK_RGB()


/*
 * 		V 2 1 0
 *
 * 		T O
 *
 * 		A N Y   R G B
 *
 */

//
// NNB Integer
//
#define DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(fn_name, coeffs, coef_shift, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint32_t*			src = (uint32_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y, u, v;\
		uint32_t			padding_bytes = PADDING_BYTE_COUNT(dest_fmt, conv->width);\
		\
		while(line++ < conv->height) {\
			/* Convert as many chunks of 6 pixels as possible,
			   until less than 6 pixels remain. */\
			while(pixel < (conv->width - 5)) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF);\
				u = (*src & 0x3FF);\
				v = ((*src >> 20) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = ((src[1] >> 10) & 0x3FF);\
				v = (src[2] & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF);\
				u = ((src[2] >> 20) & 0x3FF);\
				v = ((src[3] >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[3] >> 20) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				src += 4;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since yuv422
			   formats (inc v210) carry at the very least an even
			   number of pixels. */\
			if ((conv->width - pixel) == 2) {\
				y = ((*src >> 10) & 0x3FF);\
				u = (*src & 0x3FF);\
				v = ((*src >> 20) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
			} else if ((conv->width - pixel) == 4) {\
				y = ((*src >> 10) & 0x3FF);\
				u = (*src & 0x3FF);\
				v = ((*src >> 20) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = ((src[1] >> 10) & 0x3FF);\
				v = (src[2] & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
			}\
			src = (uint32_t*) ((uint8_t*)in + line * ROW_SIZE(PixFcV210, conv->width));\
			dst += padding_bytes;\
			pixel = 0;\
		}\
	}

// These conversion routines only assume an even number of pixels.
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_rgb_nonsse, yuv_10bit_to_rgb_8bit_coef_lhs8[0], 8, yuv_10bit_to_rgb_8bit_off[0])
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_rgb_bt601_nonsse, yuv_10bit_to_rgb_8bit_coef_lhs8[1], 8, yuv_10bit_to_rgb_8bit_off[1])
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_rgb_bt709_nonsse, yuv_10bit_to_rgb_8bit_coef_lhs8[2], 8, yuv_10bit_to_rgb_8bit_off[2])
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_10bit_rgb_nonsse, yuv_10bit_to_rgb_10bit_coef_lhs8[0], 8, yuv_10bit_to_rgb_10bit_off[0])
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_10bit_rgb_bt601_nonsse, yuv_10bit_to_rgb_10bit_coef_lhs8[1], 8, yuv_10bit_to_rgb_10bit_off[1])
DEFINE_V210_TO_ANY_RGB_NONSSE_CONVERSION(convert_v210_to_any_10bit_rgb_bt709_nonsse, yuv_10bit_to_rgb_10bit_coef_lhs8[2], 8, yuv_10bit_to_rgb_10bit_off[2])


//
// NNB Float
//
#define DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(fn_name, coeffs, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint32_t*			src = (uint32_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y, u, v;\
		uint32_t			padding_bytes = PADDING_BYTE_COUNT(dest_fmt, conv->width);\
		\
		while(line++ < conv->height) {\
			/* Convert as many chunks of 6 pixels as possible,
			   until less than 6 pixels remain. */\
			while(pixel < (conv->width - 5)) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF);\
				u = (*src & 0x3FF);\
				v = ((*src >> 20) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = ((src[1] >> 10) & 0x3FF);\
				v = (src[2] & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF);\
				u = ((src[2] >> 20) & 0x3FF);\
				v = ((src[3] >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[3] >> 20) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				src += 4;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since yuv422
			   formats (inc v210) carry at the very least an even
			   number of pixels. */\
			if ((conv->width - pixel) == 2) {\
				y = ((*src >> 10) & 0x3FF);\
				u = (*src & 0x3FF);\
				v = ((*src >> 20) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
			} else if ((conv->width - pixel) == 4) {\
				y = ((*src >> 10) & 0x3FF);\
				u = (*src & 0x3FF);\
				v = ((*src >> 20) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = ((src[1] >> 10) & 0x3FF);\
				v = (src[2] & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
			}\
			src = (uint32_t*) ((uint8_t*)in + line * ROW_SIZE(PixFcV210, conv->width));\
			dst += padding_bytes;\
			pixel = 0;\
		}\
	}

// These conversion routines only assume an even number of pixels.
DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(convert_v210_to_any_rgb_nonsse_float, yuv_10bit_to_rgb_8bit_coef[0], yuv_10bit_to_rgb_8bit_off[0])
DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(convert_v210_to_any_rgb_bt601_nonsse_float, yuv_10bit_to_rgb_8bit_coef[1], yuv_10bit_to_rgb_8bit_off[1])
DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(convert_v210_to_any_rgb_bt709_nonsse_float, yuv_10bit_to_rgb_8bit_coef[2], yuv_10bit_to_rgb_8bit_off[2])
DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(convert_v210_to_any_10bit_rgb_nonsse_float, yuv_10bit_to_rgb_10bit_coef[0], yuv_10bit_to_rgb_10bit_off[0])
DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(convert_v210_to_any_10bit_rgb_bt601_nonsse_float, yuv_10bit_to_rgb_10bit_coef[1], yuv_10bit_to_rgb_10bit_off[1])
DEFINE_V210_TO_ANY_RGB_NONSSE_FLOAT_CONVERSION(convert_v210_to_any_10bit_rgb_bt709_nonsse_float, yuv_10bit_to_rgb_10bit_coef[2], yuv_10bit_to_rgb_10bit_off[2])



//
// AVG Integer
//
#define DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(fn_name, coeffs, coef_shift, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint32_t*			src = (uint32_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y, u, v, next_u, next_v;\
		uint32_t			padding_bytes = PADDING_BYTE_COUNT(dest_fmt, conv->width);\
		\
		while(line++ < conv->height) {\
			u = (*src & 0x3FF);\
			v = ((*src >> 20) & 0x3FF);\
			/* Convert as many chunks of 6 pixels as possible,
			   until 6 or less pixels remain as they must be handled seperately. */\
			while(pixel < (conv->width - 6)) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				next_u = ((src[1] >> 10) & 0x3FF); /* belongs to pix 3 & 4 */\
				next_v = (src[2] & 0x3FF); /* belongs to pix 3 & 4 */\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, ((u + next_u) / 2), ((v + next_v) / 2));\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				next_u = ((src[2] >> 20) & 0x3FF); /* belongs to pix 5 & 6 */\
				next_v = ((src[3] >> 10) & 0x3FF);/* belongs to pix 5 & 6 */\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, ((u + next_u) / 2), ((v + next_v) / 2));\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[3] >> 20) & 0x3FF);\
				next_u = (src[4] & 0x3FF); /* belongs to pixels 1 and 2 in next set of 6 v210 pix */\
				next_v = ((src[4] >> 20) & 0x3FF); /* belongs to pixels 1 and 2 in next set of 6 v210 pix */\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, ((u + next_u) / 2), ((v + next_v) / 2));\
				u = next_u; v = next_v;\
				src += 4;\
				pixel += 6;\
			}\
			/* There can only be 2, 4 or 6 pixels left since yuv422
			   formats (inc v210) carry at the very least an even
			   number of pixels. */\
			if ((conv->width - pixel) == 2) {\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
			} else if ((conv->width - pixel) == 4) {\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				next_u = ((src[1] >> 10) & 0x3FF); /* belongs to pix 3 & 4 */\
				next_v = (src[2] & 0x3FF); /* belongs to pix 3 & 4 */\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, ((u + next_u) / 2), ((v + next_v) / 2));\
				u = next_u; v = next_v;\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
			} else if ((conv->width - pixel) == 6) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				next_u = ((src[1] >> 10) & 0x3FF); /* belongs to pix 3 & 4 */\
				next_v = (src[2] & 0x3FF); /* belongs to pix 3 & 4 */\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, ((u + next_u) / 2), ((v + next_v) / 2));\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				next_u = ((src[2] >> 20) & 0x3FF); /* belongs to pix 5 & 6 */\
				next_v = ((src[3] >> 10) & 0x3FF);/* belongs to pix 5 & 6 */\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, ((u + next_u) / 2), ((v + next_v) / 2));\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
				y = ((src[3] >> 20) & 0x3FF);\
				CONVERT_N_STORE(coeffs, coef_shift, offsets, y, u, v);\
			}\
			src = (uint32_t*) ((uint8_t*)in + line * ROW_SIZE(PixFcV210, conv->width));\
			dst += padding_bytes;\
			pixel = 0;\
		}\
	}

// These conversion routines only assume an even number of pixels.
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(upsample_n_convert_v210_to_any_rgb_nonsse, yuv_10bit_to_rgb_8bit_coef_lhs8[0], 8, yuv_10bit_to_rgb_8bit_off[0])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(upsample_n_convert_v210_to_any_rgb_bt601_nonsse, yuv_10bit_to_rgb_8bit_coef_lhs8[1], 8, yuv_10bit_to_rgb_8bit_off[1])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(upsample_n_convert_v210_to_any_rgb_bt709_nonsse, yuv_10bit_to_rgb_8bit_coef_lhs8[2], 8, yuv_10bit_to_rgb_8bit_off[2])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(upsample_n_convert_v210_to_any_10bit_rgb_nonsse, yuv_10bit_to_rgb_10bit_coef_lhs8[0], 8, yuv_10bit_to_rgb_10bit_off[0])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(upsample_n_convert_v210_to_any_10bit_rgb_bt601_nonsse, yuv_10bit_to_rgb_10bit_coef_lhs8[1], 8, yuv_10bit_to_rgb_10bit_off[1])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION(upsample_n_convert_v210_to_any_10bit_rgb_bt709_nonsse, yuv_10bit_to_rgb_10bit_coef_lhs8[2], 8, yuv_10bit_to_rgb_10bit_off[2])



//
// AVG Float
//
#define DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(fn_name, coeffs, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out){\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint32_t*			src = (uint32_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		int32_t				r, g, b;\
		int32_t				y, u, v, next_u, next_v;\
		uint32_t			padding_bytes = PADDING_BYTE_COUNT(dest_fmt, conv->width);\
		\
		while(line++ < conv->height) {\
			u = (*src & 0x3FF);\
			v = ((*src >> 20) & 0x3FF);\
			/* Convert as many chunks of 6 pixels as possible,
			   until 6 or less pixels remain as they must be handled seperately. */\
			while(pixel < (conv->width - 6)) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				next_u = ((src[1] >> 10) & 0x3FF); /* belongs to pix 3 & 4 */\
				next_v = (src[2] & 0x3FF); /* belongs to pix 3 & 4 */\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, ((u + next_u) / 2.0f), ((v + next_v) / 2.0f));\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				next_u = ((src[2] >> 20) & 0x3FF); /* belongs to pix 5 & 6 */\
				next_v = ((src[3] >> 10) & 0x3FF);/* belongs to pix 5 & 6 */\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, ((u + next_u) / 2.0f), ((v + next_v) / 2.0f));\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[3] >> 20) & 0x3FF);\
				next_u = (src[4] & 0x3FF); /* belongs to pixels 1 and 2 in next set of 6 v210 pix */\
				next_v = ((src[4] >> 20) & 0x3FF); /* belongs to pixels 1 and 2 in next set of 6 v210 pix */\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, ((u + next_u) / 2.0f), ((v + next_v) / 2.0f));\
				u = next_u; v = next_v;\
				src += 4;\
				pixel += 6;\
			}\
			/* There can only be 2, 4 or 6 pixels left since yuv422
			   formats (inc v210) carry at the very least an even
			   number of pixels. */\
			if ((conv->width - pixel) == 2) {\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
			} else if ((conv->width - pixel) == 4) {\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				next_u = ((src[1] >> 10) & 0x3FF); /* belongs to pix 3 & 4 */\
				next_v = (src[2] & 0x3FF); /* belongs to pix 3 & 4 */\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, ((u + next_u) / 2.0f), ((v + next_v) / 2.0f));\
				u = next_u; v = next_v;\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
			} else if ((conv->width - pixel) == 6) {\
				/* Pixel 1 and 2 */\
				y = ((*src >> 10) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = (src[1] & 0x3FF);\
				next_u = ((src[1] >> 10) & 0x3FF); /* belongs to pix 3 & 4 */\
				next_v = (src[2] & 0x3FF); /* belongs to pix 3 & 4 */\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, ((u + next_u) / 2.0f), ((v + next_v) / 2.0f));\
				/* Pixel 3 and 4 */\
				y = ((src[1] >> 20) & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[2] >> 10) & 0x3FF);\
				next_u = ((src[2] >> 20) & 0x3FF); /* belongs to pix 5 & 6 */\
				next_v = ((src[3] >> 10) & 0x3FF);/* belongs to pix 5 & 6 */\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, ((u + next_u) / 2.0f), ((v + next_v) / 2.0f));\
				/* Pixel 5 and 6 */\
				y = (src[3] & 0x3FF);\
				u = next_u; v = next_v;\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
				y = ((src[3] >> 20) & 0x3FF);\
				CONVERT_N_STORE_FLOAT(coeffs, offsets, y, u, v);\
			}\
			src = (uint32_t*) ((uint8_t*)in + line * ROW_SIZE(PixFcV210, conv->width));\
			dst += padding_bytes;\
			pixel = 0;\
		}\
	}

// These conversion routines only assume an even number of pixels.
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(upsample_n_convert_v210_to_any_rgb_nonsse_float, yuv_10bit_to_rgb_8bit_coef[0], yuv_10bit_to_rgb_8bit_off[0])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(upsample_n_convert_v210_to_any_rgb_bt601_nonsse_float, yuv_10bit_to_rgb_8bit_coef[1], yuv_10bit_to_rgb_8bit_off[1])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(upsample_n_convert_v210_to_any_rgb_bt709_nonsse_float, yuv_10bit_to_rgb_8bit_coef[2], yuv_10bit_to_rgb_8bit_off[2])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(upsample_n_convert_v210_to_any_10bit_rgb_nonsse_float, yuv_10bit_to_rgb_10bit_coef[0], yuv_10bit_to_rgb_10bit_off[0])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(upsample_n_convert_v210_to_any_10bit_rgb_bt601_nonsse_float, yuv_10bit_to_rgb_10bit_coef[1], yuv_10bit_to_rgb_10bit_off[1])
DEFINE_UPSAMPLE_V210_TO_ANY_RGB_NONSSE_CONVERSION_FLOAT(upsample_n_convert_v210_to_any_10bit_rgb_bt709_nonsse_float, yuv_10bit_to_rgb_10bit_coef[2], yuv_10bit_to_rgb_10bit_off[2])



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
	uint32_t			line = 0;
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
