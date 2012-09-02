/*
 * conversion_routines_from_argb.c
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
#include "pixfmt_descriptions.h"

/*
 *
 * 		R G B
 *
 * 		T O
 *
 * 		Y U V 4 2 2
 */

#define UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt) \
	do {\
		if (src_fmt == PixFcARGB) {\
			src++; r1 = *(src++); g1 = *(src++); b1 = *(src++);\
			src++; r2 = *(src++); g2 = *(src++); b2 = *(src++);\
		} else if (src_fmt == PixFcBGRA) {\
			b1 = *(src++); g1 = *(src++); r1 = *(src++); src++;\
			b2 = *(src++); g2 = *(src++); r2 = *(src++); src++;\
		} else if (src_fmt == PixFcRGB24) {\
			r1 = *(src++); g1 = *(src++); b1 = *(src++);\
			r2 = *(src++); g2 = *(src++); b2 = *(src++);\
		} else if (src_fmt == PixFcBGR24) {\
			b1 = *(src++); g1 = *(src++); r1 = *(src++);\
			b2 = *(src++); g2 = *(src++); r2 = *(src++);\
		} else if (src_fmt == PixFcR210) {\
			uint32_t tmp;\
			uint8_t *p = (uint8_t *) &tmp;\
			p[3] = *src++;\
			p[2] = *src++;\
			p[1] = *src++;\
			p[0] = *src++;\
			b1 = tmp & 0x3ff;\
			g1 = (tmp >> 10) & 0x3ff;\
			r1 = (tmp >> 20) & 0x3ff;\
			p[3] = *src++;\
			p[2] = *src++;\
			p[1] = *src++;\
			p[0] = *src++;\
			b2 = tmp & 0x3ff;\
			g2 = (tmp >> 10) & 0x3ff;\
			r2 = (tmp >> 20) & 0x3ff;\
		} else\
			printf("Unknown source pixel format in non-SSE conversion from RGB\n");\
	}while(0)\

#define PACK_R210(r, g, b, dst, dest_fmt) \
	if (dest_fmt == PixFcR210) {\
		uint32_t tmp;\
		uint8_t *p = (uint8_t *) &tmp;\
		/* dprint("R: %d G: %d B: %d\n", CLIP_10BIT_PIXEL(r), CLIP_10BIT_PIXEL(g), CLIP_10BIT_PIXEL(b)); */\
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

#define PACK_YUV422(dst, y1, u, y2, v, dest_fmt) do {\
			if (dest_fmt == PixFcYUYV) {\
				*(dst++) = CLIP_PIXEL(y1);\
				*(dst++) = CLIP_PIXEL(u);\
				*(dst++) = CLIP_PIXEL(y2);\
				*(dst++) = CLIP_PIXEL(v);\
			} else if (dest_fmt == PixFcUYVY) {\
				*(dst++) = CLIP_PIXEL(u);\
				*(dst++) = CLIP_PIXEL(y1);\
				*(dst++) = CLIP_PIXEL(v);\
				*(dst++) = CLIP_PIXEL(y2);\
			} else if (dest_fmt == PixFcYUV422P) {\
				*(y_plane++) = CLIP_PIXEL(y1);\
				*(y_plane++) = CLIP_PIXEL(y2);\
				*(u_plane++) = CLIP_PIXEL(u);\
				*(v_plane++) = CLIP_PIXEL(v);\
			} else {\
				printf("Unknown output format in non-SSE conversion from RGB\n");\
			}\
		} while(0)

#define NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets) \
		y1 = (((coeffs[0][0]) * r1 + (coeffs[0][1]) * g1 + (coeffs[0][2]) * b1) >> coef_shift) + offsets[0];\
		u =  (((coeffs[1][0]) * r1 + (coeffs[1][1]) * g1 + (coeffs[1][2]) * b1) >> coef_shift) + offsets[1];\
		v =  (((coeffs[2][0]) * r1 + (coeffs[2][1]) * g1 + (coeffs[2][2]) * b1) >> coef_shift) + offsets[2];\
		y2 = (((coeffs[0][0]) * r2 + (coeffs[0][1]) * g2 + (coeffs[0][2]) * b2) >> coef_shift) + offsets[0];\

#define NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets) \
		y1 = coeffs[0][0] * r1 + coeffs[0][1] * g1 + coeffs[0][2] * b1 + offsets[0];\
		u =  coeffs[1][0] * r1 + coeffs[1][1] * g1 + coeffs[1][2] * b1 + offsets[1];\
		v =  coeffs[2][0] * r1 + coeffs[2][1] * g1 + coeffs[2][2] * b1 + offsets[2];\
		y2 = coeffs[0][0] * r2 + coeffs[0][1] * g2 + coeffs[0][2] * b2 + offsets[0];\


//
// NNB Integer
//
#define 	DEFINE_ANY_RGB_TO_YUV422(fn_name, coeffs, coef_shift, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		DECLARE_PADDING_BYTE_COUNT(src_padding_bytes, src_fmt, conv->width);\
		uint32_t 			line = conv->height;\
		uint32_t			pixel;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		uint8_t*			y_plane = dst;\
		uint8_t*			u_plane = dst + conv->pixel_count;\
		uint8_t*			v_plane = u_plane + conv->pixel_count / 2;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;\
		int32_t				y1, y2, u, v;\
		while(line-- > 0) {\
			pixel = conv->width;\
			while(pixel > 0){\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				PACK_YUV422(dst, y1, u, y2, v, dest_fmt);\
				pixel -= 2;\
			}\
			src += src_padding_bytes;\
		}\
	}\

// these functions assume an even number of pixels
DEFINE_ANY_RGB_TO_YUV422(convert_rgb_to_yuv422_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[0], 8, rgb_8bit_to_yuv_8bit_off[0])
DEFINE_ANY_RGB_TO_YUV422(convert_rgb_to_yuv422_bt601_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[1], 8, rgb_8bit_to_yuv_8bit_off[1])
DEFINE_ANY_RGB_TO_YUV422(convert_rgb_to_yuv422_bt709_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[2], 8, rgb_8bit_to_yuv_8bit_off[2])



//
// 	NNB Float
//
#define 	DEFINE_ANY_RGB_TO_YUV422_FLOAT(fn_name, coeffs, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		DECLARE_PADDING_BYTE_COUNT(src_padding_bytes, src_fmt, conv->width);\
		uint32_t 			line = conv->height;\
		uint32_t 			pixel_num = 0;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		uint8_t*			y_plane = dst;\
		uint8_t*			u_plane = dst + conv->pixel_count;\
		uint8_t*			v_plane = u_plane + conv->pixel_count / 2;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;\
		int32_t				y1, y2, u, v;\
		while(line-- > 0) {\
			pixel_num = conv->width;\
			while(pixel_num > 0){\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				PACK_YUV422(dst, y1, u, y2, v, dest_fmt);\
				pixel_num -= 2;\
			}\
			src += src_padding_bytes;\
		}\
	}\

// these functions assume an even number of pixels
DEFINE_ANY_RGB_TO_YUV422_FLOAT(convert_rgb_to_yuv422_nonsse_float, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0])
DEFINE_ANY_RGB_TO_YUV422_FLOAT(convert_rgb_to_yuv422_bt601_nonsse_float, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1])
DEFINE_ANY_RGB_TO_YUV422_FLOAT(convert_rgb_to_yuv422_bt709_nonsse_float, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2])





#define AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets) \
		y1 = (((coeffs[0][0]) * r1 + (coeffs[0][1]) * g1 + (coeffs[0][2]) * b1) >> coef_shift) + offsets[0];\
		u =  (((coeffs[1][0]) * (((prev_r + r2) / 2 + r1) / 2) + (coeffs[1][1]) * (((prev_g + g2) / 2 + g1) / 2) + (coeffs[1][2]) * (((prev_b + b2) / 2 + b1) / 2)) >> coef_shift) + offsets[1];\
		v =  (((coeffs[2][0]) * (((prev_r + r2) / 2 + r1) / 2) + (coeffs[2][1]) * (((prev_g + g2) / 2 + g1) / 2) + (coeffs[2][2]) * (((prev_b + b2) / 2 + b1) / 2)) >> coef_shift) + offsets[2];\
		y2 = (((coeffs[0][0]) * r2 + (coeffs[0][1]) * g2 + (coeffs[0][2]) * b2) >> coef_shift) + offsets[0];\

#define AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets) \
		y1 = coeffs[0][0] * r1 + coeffs[0][1] * g1 + coeffs[0][2] * b1 + offsets[0];\
		u =  coeffs[1][0] * (((prev_r + r2) / 2.0f + r1) / 2.0f) + coeffs[1][1] * (((prev_g + g2) / 2.0f + g1) / 2.0f) + coeffs[1][2] * (((prev_b + b2) / 2.0f + b1) / 2.0f) + offsets[1];\
		v =  coeffs[2][0] * (((prev_r + r2) / 2.0f + r1) / 2.0f) + coeffs[2][1] * (((prev_g + g2) / 2.0f + g1) / 2.0f) + coeffs[2][2] * (((prev_b + b2) / 2.0f + b1) / 2.0f) + offsets[2];\
		y2 = coeffs[0][0] * r2 + coeffs[0][1] * g2 + coeffs[0][2] * b2 + offsets[0];\


//
// AVG Integer
//
#define 	DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422(fn_name, coeffs, coef_shift, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		DECLARE_PADDING_BYTE_COUNT(src_padding_bytes, src_fmt, conv->width);\
		uint32_t			line = conv->height;\
		uint32_t 			pixel_num;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		uint8_t*			y_plane = dst;\
		uint8_t*			u_plane = dst + conv->pixel_count;\
		uint8_t*			v_plane = u_plane + conv->pixel_count / 2;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0, prev_r = 0, prev_g = 0, prev_b = 0;\
		int32_t				y1, y2, u, v;\
		while(line-- > 0) {\
			pixel_num = conv->width - 2; /* first 2 pixels handled outside main loop */\
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
			AVG_CONVERT_RGB_TO_YUV422(r1, g1, b1, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
			prev_r = r2; prev_g = g2; prev_b = b2;\
			PACK_YUV422(dst, y1, u, y2, v, dest_fmt);\
			while(pixel_num > 0){\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				PACK_YUV422(dst, y1, u, y2, v, dest_fmt);\
				pixel_num -= 2;\
			}\
			src += src_padding_bytes;\
		}\
	}\

// these functions assume an even number of pixels
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422(downsample_n_convert_rgb_to_yuv422_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[0], 8, rgb_8bit_to_yuv_8bit_off[0])
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422(downsample_n_convert_rgb_to_yuv422_bt601_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[1], 8, rgb_8bit_to_yuv_8bit_off[1])
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422(downsample_n_convert_rgb_to_yuv422_bt709_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[2], 8, rgb_8bit_to_yuv_8bit_off[2])




//
// AVG Float
//
#define 	DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422_FLOAT(fn_name, coeffs, offsets) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		DECLARE_PADDING_BYTE_COUNT(src_padding_bytes, src_fmt, conv->width);\
		uint32_t			line = conv->height;\
		uint32_t 			pixel_num;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		uint8_t*			y_plane = dst;\
		uint8_t*			u_plane = dst + conv->pixel_count;\
		uint8_t*			v_plane = u_plane + conv->pixel_count / 2;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0, prev_r = 0, prev_g = 0, prev_b = 0;\
		int32_t				y1, y2, u, v;\
		while(line-- > 0) {\
			pixel_num = conv->width - 2; /* first 2 pixels handled outside main loop */\
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
			AVG_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
			prev_r = r2; prev_g = g2; prev_b = b2;\
			PACK_YUV422(dst, y1, u, y2, v, dest_fmt);\
			while(pixel_num > 0){\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				PACK_YUV422(dst, y1, u, y2, v, dest_fmt);\
				pixel_num -= 2;\
			}\
			src += src_padding_bytes;\
		}\
	}\

// these functions assume an even number of pixels
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422_FLOAT(downsample_n_convert_rgb_to_yuv422_nonsse_float, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0])
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422_FLOAT(downsample_n_convert_rgb_to_yuv422_bt601_nonsse_float, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1])
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_YUV422_FLOAT(downsample_n_convert_rgb_to_yuv422_bt709_nonsse_float, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2])





/*
 *
 * 		R G B
 *
 * 		T O
 *
 * 		V 2 1 0
 */

//
// NNB Integer
//
#define 	DEFINE_ANY_RGB_TO_V210_FN(fn_name, coeffs, coef_shift, offsets) \
	void 		fn_name(const struct PixFcSSE* pixfc, void* in, void* out)\
	{\
		PixFcPixelFormat 	src_fmt = pixfc->source_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint8_t*			src = (uint8_t *) in;\
		uint32_t*			dst = (uint32_t *) out;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;\
		int32_t				y1, y2, u, v;\
		while(line++ < pixfc->height){\
			/* Convert as many chunks of 6 pixels as possible */\
			while(pixel < pixfc->width - 5) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*(dst++) |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y1) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 20;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since we assumed an even number of pixels */\
			if ((pixfc->width - pixel) == 2) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
			} else if ((pixfc->width - pixel) == 4) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
			}\
		dst = (uint32_t*) ((uint8_t*)out + line * ROW_SIZE(PixFcV210, pixfc->width));\
		pixel = 0;\
		}\
	}\

/* This conversion function only assumes an even number of pixels */
DEFINE_ANY_RGB_TO_V210_FN(convert_rgb_to_v210_nonsse, rgb_8bit_to_yuv_10bit_coef_lhs8[0], 8, rgb_8bit_to_yuv_10bit_off[0]);
DEFINE_ANY_RGB_TO_V210_FN(convert_rgb_to_v210_bt601_nonsse, rgb_8bit_to_yuv_10bit_coef_lhs8[1], 8, rgb_8bit_to_yuv_10bit_off[1]);
DEFINE_ANY_RGB_TO_V210_FN(convert_rgb_to_v210_bt709_nonsse,	rgb_8bit_to_yuv_10bit_coef_lhs8[2], 8, rgb_8bit_to_yuv_10bit_off[2]);




//
// NNB Float
//
#define 	DEFINE_ANY_RGB_TO_V210_FLOAT_FN(fn_name, coeffs, offsets) \
	void 		fn_name(const struct PixFcSSE* pixfc, void* in, void* out)\
	{\
		PixFcPixelFormat 	src_fmt = pixfc->source_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint8_t*			src = (uint8_t *) in;\
		uint32_t*			dst = (uint32_t *) out;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;\
		int32_t				y1, y2, u, v;\
		while(line++ < pixfc->height){\
			/* Convert as many chunks of 6 pixels as possible */\
			while(pixel < pixfc->width - 5) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*(dst++) |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y1) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 20;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since we assumed an even number of pixels */\
			if ((pixfc->width - pixel) == 2) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
			} else if ((pixfc->width - pixel) == 4) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				NNB_CONVERT_RGB_TO_YUV422_FLOAT(r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
			}\
		dst = (uint32_t*) ((uint8_t*)out + line * ROW_SIZE(PixFcV210, pixfc->width));\
		pixel = 0;\
		}\
	}\

/* This conversion function only assumes an even number of pixels */
DEFINE_ANY_RGB_TO_V210_FLOAT_FN(convert_rgb_to_v210_nonsse_float, rgb_8bit_to_yuv_10bit_coef[0], rgb_8bit_to_yuv_10bit_off[0]);
DEFINE_ANY_RGB_TO_V210_FLOAT_FN(convert_rgb_to_v210_bt601_nonsse_float, rgb_8bit_to_yuv_10bit_coef[1], rgb_8bit_to_yuv_10bit_off[1]);
DEFINE_ANY_RGB_TO_V210_FLOAT_FN(convert_rgb_to_v210_bt709_nonsse_float,	rgb_8bit_to_yuv_10bit_coef[2], rgb_8bit_to_yuv_10bit_off[2]);




//
// AVG Integer
//
#define 	DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FN(fn_name, coeffs, coef_shift, offsets) \
	void 		fn_name(const struct PixFcSSE* pixfc, void* in, void* out)\
	{\
		PixFcPixelFormat 	src_fmt = pixfc->source_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint8_t*			src = (uint8_t *) in;\
		uint32_t*			dst = (uint32_t *) out;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0, prev_r = 0, prev_g = 0, prev_b = 0;\
		int32_t				y1, y2, u, v;\
		while(line++ < pixfc->height){\
			/* Convert as many chunks of 6 pixels as possible */\
			while(pixel < pixfc->width - 5) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				if(pixel == 0) {\
					prev_r = r1; prev_g = g1; prev_b = b1; \
				}\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*(dst++) |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y1) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 20;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since we assumed an even number of pixels */\
			if ((pixfc->width - pixel) == 2) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
			} else if ((pixfc->width - pixel) == 4) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coef_shift, coeffs, offsets);\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
			}\
		dst = (uint32_t*) ((uint8_t*)out + line * ROW_SIZE(PixFcV210, pixfc->width));\
		pixel = 0;\
		}\
	}\

/* This conversion function only assumes an even number of pixels */
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FN(downsample_n_convert_rgb_to_v210_nonsse, rgb_8bit_to_yuv_10bit_coef_lhs8[0], 8, rgb_8bit_to_yuv_10bit_off[0]);
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FN(downsample_n_convert_rgb_to_v210_bt601_nonsse, rgb_8bit_to_yuv_10bit_coef_lhs8[1], 8, rgb_8bit_to_yuv_10bit_off[1]);
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FN(downsample_n_convert_rgb_to_v210_bt709_nonsse,	rgb_8bit_to_yuv_10bit_coef_lhs8[2], 8, rgb_8bit_to_yuv_10bit_off[2]);




//
// AVG Float
//
#define 	DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FLOAT_FN(fn_name, coeffs, offsets) \
	void 		fn_name(const struct PixFcSSE* pixfc, void* in, void* out)\
	{\
		PixFcPixelFormat 	src_fmt = pixfc->source_fmt;\
		uint32_t 			pixel = 0;\
		uint32_t			line = 0;\
		uint8_t*			src = (uint8_t *) in;\
		uint32_t*			dst = (uint32_t *) out;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0, prev_r = 0, prev_g = 0, prev_b = 0;\
		int32_t				y1 = 0, y2 = 0, u = 0, v = 0;\
		while(line++ < pixfc->height){\
			/* Convert as many chunks of 6 pixels as possible */\
			while(pixel < pixfc->width - 5) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				if(pixel == 0) {\
					prev_r = r1; prev_g = g1; prev_b = b1; \
				}\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*(dst++) |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y1) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 20;\
				pixel += 6;\
			}\
			/* There can only be 2 or 4 pixels left since we assumed an even number of pixels */\
			if ((pixfc->width - pixel) == 2) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
			} else if ((pixfc->width - pixel) == 4) {\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				prev_r = r2; prev_g = g2; prev_b = b2;\
				*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
				UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
				AVG_CONVERT_RGB_TO_YUV422_FLOAT(prev_r, prev_g, prev_b, r1, g1, b1, r2, g2, b2, y1, u, v, y2, coeffs, offsets);\
				*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
				*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
				*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
				*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
			}\
		dst = (uint32_t*) ((uint8_t*)out + line * ROW_SIZE(PixFcV210, pixfc->width));\
		pixel = 0;\
		}\
	}\

/* This conversion function only assumes an even number of pixels */
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FLOAT_FN(downsample_n_convert_rgb_to_v210_nonsse_float, rgb_8bit_to_yuv_10bit_coef[0], rgb_8bit_to_yuv_10bit_off[0]);
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FLOAT_FN(downsample_n_convert_rgb_to_v210_bt601_nonsse_float, rgb_8bit_to_yuv_10bit_coef[1], rgb_8bit_to_yuv_10bit_off[1]);
DEFINE_DOWNSAMPLE_N_CONVERT_ANY_RGB_TO_V210_FLOAT_FN(downsample_n_convert_rgb_to_v210_bt709_nonsse_float,	rgb_8bit_to_yuv_10bit_coef[2], rgb_8bit_to_yuv_10bit_off[2]);





/*
 *
 * 		R G B
 *
 * 		T O
 *
 * 		Y U V 4 2 0
 */

//
// NNB Integer
//
#define NNB_CONVERT_RGB_TO_YUV420(r1_line1, g1_line1, b1_line1, r2_line1, g2_line1, b2_line1, r1_line2, g1_line2, b1_line2, r2_line2, g2_line2, b2_line2, y1_line1, y2_line1, y1_line2, y2_line2 , u, v, coef_shift, coeffs, offsets) \
		y1_line1 = ((coeffs[0][0] * r1_line1 + coeffs[0][1] * g1_line1 + coeffs[0][2] * b1_line1) >> coef_shift) + offsets[0];\
		y2_line1 = ((coeffs[0][0] * r2_line1 + coeffs[0][1] * g2_line1 + coeffs[0][2] * b2_line1) >> coef_shift) + offsets[0];\
		y1_line2 = ((coeffs[0][0] * r1_line2 + coeffs[0][1] * g1_line2 + coeffs[0][2] * b1_line2) >> coef_shift) + offsets[0];\
		y2_line2 = ((coeffs[0][0] * r2_line2 + coeffs[0][1] * g2_line2 + coeffs[0][2] * b2_line2) >> coef_shift) + offsets[0];\
		r1_line1 = (r1_line1 + r2_line1 + r1_line2 + r2_line2) / 4;\
		g1_line1 = (g1_line1 + g2_line1 + g1_line2 + g2_line2) / 4;\
		b1_line1 = (b1_line1 + b2_line1 + b1_line2 + b2_line2) / 4;\
		u = ((coeffs[1][0] * r1_line1 + coeffs[1][1] * g1_line1 + coeffs[1][2] * b1_line1) >> coef_shift) + offsets[1];\
		v = ((coeffs[2][0] * r1_line1 + coeffs[2][1] * g1_line1 + coeffs[2][2]  * b1_line1) >> coef_shift) + offsets[2];\


#define NNB_CONVERT_RGB_TO_YUV420_FLOAT(r1_line1, g1_line1, b1_line1, r2_line1, g2_line1, b2_line1, r1_line2, g1_line2, b1_line2, r2_line2, g2_line2, b2_line2, y1_line1, y2_line1, y1_line2, y2_line2 , u, v, coeffs, offsets) \
		y1_line1 = (coeffs[0][0] * r1_line1 + coeffs[0][1] * g1_line1 + coeffs[0][2] * b1_line1) + offsets[0];\
		y2_line1 = (coeffs[0][0] * r2_line1 + coeffs[0][1] * g2_line1 + coeffs[0][2] * b2_line1) + offsets[0];\
		y1_line2 = (coeffs[0][0] * r1_line2 + coeffs[0][1] * g1_line2 + coeffs[0][2] * b1_line2) + offsets[0];\
		y2_line2 = (coeffs[0][0] * r2_line2 + coeffs[0][1] * g2_line2 + coeffs[0][2] * b2_line2) + offsets[0];\
		r1_line1 = (r1_line1 + r2_line1 + r1_line2 + r2_line2) / 4.0f;\
		g1_line1 = (g1_line1 + g2_line1 + g1_line2 + g2_line2) / 4.0f;\
		b1_line1 = (b1_line1 + b2_line1 + b1_line2 + b2_line2) / 4.0f;\
		u = (coeffs[1][0] * r1_line1 + coeffs[1][1] * g1_line1 + coeffs[1][2] * b1_line1) + offsets[1];\
		v = (coeffs[2][0] * r1_line1 + coeffs[2][1] * g1_line1 + coeffs[2][2] * b1_line1) + offsets[2];\


#define 	DEFINE_ANY_RGB_TO_YUV420_FN(fn_name, coeffs, coef_shift, offsets) \
void fn_name(const struct PixFcSSE* conv, void* in, void* out)\
{\
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
	PixFcPixelFormat 	src_fmt = conv->source_fmt;\
	uint8_t				input_stride = ((src_fmt == PixFcARGB) || (src_fmt == PixFcBGRA)) ? 4 : 3;\
	uint32_t			pixel_count = conv->pixel_count;\
	uint8_t*			src_line1 = (uint8_t *) in;\
	uint8_t*			src_line2 = src_line1 + conv->width * input_stride;\
	uint8_t*			dst = (uint8_t *) out;\
	uint8_t*			y_line1 = dst;\
	uint8_t*			y_line2 = y_line1 + conv->width;\
	uint8_t*			u_plane = dst + pixel_count;\
	uint8_t*			v_plane = u_plane + pixel_count / 4;\
	int32_t				r1_line1 = 0, g1_line1 = 0, b1_line1 = 0, r2_line1 = 0, g2_line1 = 0, b2_line1 = 0;\
	int32_t				r1_line2 = 0, g1_line2 = 0, b1_line2 = 0, r2_line2 = 0, g2_line2 = 0, b2_line2 = 0;\
	int32_t				y1_line1, y2_line1, y1_line2, y2_line2, u, v;\
	uint32_t			line = conv->height;\
	uint32_t			col = conv->width;\
	while(line > 0){\
		while(col > 0){\
			if (src_fmt == PixFcARGB) {\
				src_line1++;\
				r1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				b1_line1 = *(src_line1++);\
				src_line1++;\
				r2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				b2_line1 = *(src_line1++);\
				src_line2++;\
				r1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				b1_line2 = *(src_line2++);\
				src_line2++;\
				r2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				b2_line2 = *(src_line2++);\
			} else if (src_fmt == PixFcBGRA) {\
				b1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				r1_line1 = *(src_line1++);\
				src_line1++;\
				b2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				r2_line1 = *(src_line1++);\
				src_line1++;\
				b1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				r1_line2 = *(src_line2++);\
				src_line2++;\
				b2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				r2_line2 = *(src_line2++);\
				src_line2++;\
			} else if (src_fmt == PixFcRGB24) {\
				r1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				b1_line1 = *(src_line1++);\
				r2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				b2_line1 = *(src_line1++);\
				r1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				b1_line2 = *(src_line2++);\
				r2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				b2_line2 = *(src_line2++);\
			} else if (src_fmt == PixFcBGR24) {\
				b1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				r1_line1 = *(src_line1++);\
				b2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				r2_line1 = *(src_line1++);\
				b1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				r1_line2 = *(src_line2++);\
				b2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				r2_line2 = *(src_line2++);\
			} else\
				printf("Unknown source pixel format in non-SSE conversion from RGB\n");\
			NNB_CONVERT_RGB_TO_YUV420(r1_line1, g1_line1, b1_line1, r2_line1, g2_line1, b2_line1, r1_line2, g1_line2, b1_line2, r2_line2, g2_line2, b2_line2, y1_line1, y2_line1, y1_line2, y2_line2 , u, v, coef_shift, coeffs, offsets); \
			if (dest_fmt == PixFcYUV420P) {\
				*(y_line1++) = CLIP_PIXEL(y1_line1);\
				*(y_line1++) = CLIP_PIXEL(y2_line1);\
				*(y_line2++) = CLIP_PIXEL(y1_line2);\
				*(y_line2++) = CLIP_PIXEL(y2_line2);\
				*(u_plane++) = CLIP_PIXEL(u);\
				*(v_plane++) = CLIP_PIXEL(v);\
			} else {\
				printf("Unknown output format in non-SSE conversion from RGB\n");\
			}\
			col -= 2;\
		}\
		src_line1 += conv->width * input_stride;\
		src_line2 += conv->width * input_stride;\
		y_line1 += conv->width;\
		y_line2 += conv->width;\
		line -= 2;\
		col = conv->width;\
	}\
}

// These functions assume an even number of pixels and lines
DEFINE_ANY_RGB_TO_YUV420_FN(convert_rgb_to_yuv420_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[0], 8, rgb_8bit_to_yuv_8bit_off[0]);
DEFINE_ANY_RGB_TO_YUV420_FN(convert_rgb_to_yuv420_bt601_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[1], 8, rgb_8bit_to_yuv_8bit_off[1]);
DEFINE_ANY_RGB_TO_YUV420_FN(convert_rgb_to_yuv420_bt709_nonsse, rgb_8bit_to_yuv_8bit_coef_lhs8[2], 8, rgb_8bit_to_yuv_8bit_off[2]);


//
// NNB Float
//
#define 	DEFINE_ANY_RGB_TO_YUV420_FLOAT_FN(fn_name, coeffs, offsets) \
void fn_name(const struct PixFcSSE* conv, void* in, void* out)\
{\
	PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
	PixFcPixelFormat 	src_fmt = conv->source_fmt;\
	uint8_t				input_stride = ((src_fmt == PixFcARGB) || (src_fmt == PixFcBGRA)) ? 4 : 3;\
	uint32_t			pixel_count = conv->pixel_count;\
	uint8_t*			src_line1 = (uint8_t *) in;\
	uint8_t*			src_line2 = src_line1 + conv->width * input_stride;\
	uint8_t*			dst = (uint8_t *) out;\
	uint8_t*			y_line1 = dst;\
	uint8_t*			y_line2 = y_line1 + conv->width;\
	uint8_t*			u_plane = dst + pixel_count;\
	uint8_t*			v_plane = u_plane + pixel_count / 4;\
	int32_t				r1_line1 = 0, g1_line1 = 0, b1_line1 = 0, r2_line1 = 0, g2_line1 = 0, b2_line1 = 0;\
	int32_t				r1_line2 = 0, g1_line2 = 0, b1_line2 = 0, r2_line2 = 0, g2_line2 = 0, b2_line2 = 0;\
	int32_t				y1_line1, y2_line1, y1_line2, y2_line2, u, v;\
	uint32_t			line = conv->height;\
	uint32_t			col = conv->width;\
	while(line > 0){\
		while(col > 0){\
			if (src_fmt == PixFcARGB) {\
				src_line1++;\
				r1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				b1_line1 = *(src_line1++);\
				src_line1++;\
				r2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				b2_line1 = *(src_line1++);\
				src_line2++;\
				r1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				b1_line2 = *(src_line2++);\
				src_line2++;\
				r2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				b2_line2 = *(src_line2++);\
			} else if (src_fmt == PixFcBGRA) {\
				b1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				r1_line1 = *(src_line1++);\
				src_line1++;\
				b2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				r2_line1 = *(src_line1++);\
				src_line1++;\
				b1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				r1_line2 = *(src_line2++);\
				src_line2++;\
				b2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				r2_line2 = *(src_line2++);\
				src_line2++;\
			} else if (src_fmt == PixFcRGB24) {\
				r1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				b1_line1 = *(src_line1++);\
				r2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				b2_line1 = *(src_line1++);\
				r1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				b1_line2 = *(src_line2++);\
				r2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				b2_line2 = *(src_line2++);\
			} else if (src_fmt == PixFcBGR24) {\
				b1_line1 = *(src_line1++);\
				g1_line1 = *(src_line1++);\
				r1_line1 = *(src_line1++);\
				b2_line1 = *(src_line1++);\
				g2_line1 = *(src_line1++);\
				r2_line1 = *(src_line1++);\
				b1_line2 = *(src_line2++);\
				g1_line2 = *(src_line2++);\
				r1_line2 = *(src_line2++);\
				b2_line2 = *(src_line2++);\
				g2_line2 = *(src_line2++);\
				r2_line2 = *(src_line2++);\
			} else\
				printf("Unknown source pixel format in non-SSE conversion from RGB\n");\
			NNB_CONVERT_RGB_TO_YUV420_FLOAT(r1_line1, g1_line1, b1_line1, r2_line1, g2_line1, b2_line1, r1_line2, g1_line2, b1_line2, r2_line2, g2_line2, b2_line2, y1_line1, y2_line1, y1_line2, y2_line2 , u, v, coeffs, offsets); \
			if (dest_fmt == PixFcYUV420P) {\
				*(y_line1++) = CLIP_PIXEL(y1_line1);\
				*(y_line1++) = CLIP_PIXEL(y2_line1);\
				*(y_line2++) = CLIP_PIXEL(y1_line2);\
				*(y_line2++) = CLIP_PIXEL(y2_line2);\
				*(u_plane++) = CLIP_PIXEL(u);\
				*(v_plane++) = CLIP_PIXEL(v);\
			} else {\
				printf("Unknown output format in non-SSE conversion from RGB\n");\
			}\
			col -= 2;\
		}\
		src_line1 += conv->width * input_stride;\
		src_line2 += conv->width * input_stride;\
		y_line1 += conv->width;\
		y_line2 += conv->width;\
		line -= 2;\
		col = conv->width;\
	}\
}

// These functions assume an even number of pixels and lines
DEFINE_ANY_RGB_TO_YUV420_FLOAT_FN(convert_rgb_to_yuv420_nonsse_float, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0]);
DEFINE_ANY_RGB_TO_YUV420_FLOAT_FN(convert_rgb_to_yuv420_bt601_nonsse_float, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1]);
DEFINE_ANY_RGB_TO_YUV420_FLOAT_FN(convert_rgb_to_yuv420_bt709_nonsse_float, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2]);



/*
 *
 * 		any R G B
 *
 * 		T O
 *
 * 		R 2 1 0
 *
 */
// This conversion assumes an even number of pixels
void		convert_rgb_to_10bit_rgb_nonsse(const struct PixFcSSE* pixfc, void* in, void* out) {
	PixFcPixelFormat 	src_fmt = pixfc->source_fmt;
	PixFcPixelFormat 	dst_fmt = pixfc->dest_fmt;
	uint32_t 			pixel = 0;
	uint32_t			line = 0;
	uint8_t*			src = (uint8_t *) in;
	uint8_t*			dst = (uint8_t *) out;
	uint32_t			dst_row_byte_count = ROW_SIZE(dst_fmt, pixfc->width);
	int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;
	while(line++ < pixfc->height){
		while(pixel < pixfc->width) {
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);
			r1 <<= 2;
			g1 <<= 2;
			b1 <<= 2;
			PACK_R210(r1, g1, b1, dst, dst_fmt);
			r2 <<= 2;
			g2 <<= 2;
			b2 <<= 2;
			PACK_R210(r2, g2, b2, dst, dst_fmt);
			pixel += 2;
		}
		dst = (uint8_t*)out + line * dst_row_byte_count;
		pixel = 0;
	}
}
