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
#include "rgb_conversion_recipes.h"

#define CONVERT_TO_YUV422I(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				RGB32_TO_YUV422I_RECIPE,\
				unpack_argb_to_r_g_b_vectors_,\
				pack_fn,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT2_TO_YUV422I(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				RGB32_TO_YUV422I_RECIPE2,\
				unpack_argb_to_ag_rb_vectors_,\
				pack_fn,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE,\
				unpack_argb_to_r_g_b_vectors_,\
				pack_fn,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_fn, instr_set)\
		DO_CONVERSION_1U_1P(\
				AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE2,\
				unpack_argb_to_ag_rb_vectors_,\
				pack_fn,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT_TO_YUV422P(instr_set)\
		DO_CONVERSION_1U_2P(\
				RGB32_TO_YUV422P_RECIPE,\
				unpack_argb_to_r_g_b_vectors_,\
				pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
				pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT2_TO_YUV422P(instr_set)\
		DO_CONVERSION_1U_2P(\
				RGB32_TO_YUV422P_RECIPE2,\
				unpack_argb_to_ag_rb_vectors_,\
				pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
				pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define DOWNSAMPLE_N_CONVERT_TO_YUV422P(instr_set)\
		DO_CONVERSION_1U_2P(\
				AVG_DOWNSAMPLE_RGB32_TO_YUV422P_RECIPE,\
				unpack_argb_to_r_g_b_vectors_,\
				pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
				pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define DOWNSAMPLE_N_CONVERT2_TO_YUV422P(instr_set)\
		DO_CONVERSION_1U_2P(\
				AVG_DOWNSAMPLE_RGB32_TO_YUV422P_RECIPE2,\
				unpack_argb_to_ag_rb_vectors_,\
				pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2,\
				pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT_TO_YUV420P(instr_set)\
		DO_CONVERSION_1U_2P(\
				RGB32_TO_YUV420P_RECIPE,\
				unpack_argb_to_r_g_b_vectors_,\
				pack_2_y_vectors_to_1_y_vector_sse2,\
				pack_4_uv_vectors_to_yup_vectors_sse2,\
				convert_r_g_b_vectors_to_y_vector_sse2,\
				convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT2_TO_YUV420P(instr_set)\
		DO_CONVERSION_1U_2P(\
				RGB32_TO_YUV420P_RECIPE2,\
				unpack_argb_to_ag_rb_vectors_,\
				pack_2_y_vectors_to_1_y_vector_sse2,\
				pack_4_uv_vectors_to_yup_vectors_sse2,\
				convert_ag_rb_vectors_to_y_vector_sse2,\
				convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,\
				instr_set\
		)

#define CONVERT_TO_V210(instr_set)\
		DO_CONVERSION_1U_1P(\
				RGB32_TO_V210_RECIPE,\
				unpack_argb_to_r_g_b_vectors_sse2_ssse3,\
				pack_6_y_uv_vectors_to_4_v210_vectors_ ## instr_set,\
				convert_8bit_r_g_b_vectors_to_10bit_y_vector_sse2,\
				convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_sse2,\
				instr_set\
		)

#define DOWNSAMPLE_N_CONVERT_TO_V210(instr_set)\
		DO_CONVERSION_1U_1P(\
				AVG_DOWNSAMPLE_RGB32_TO_V210_RECIPE,\
				unpack_argb_to_r_g_b_vectors_sse2_ssse3,\
				pack_6_y_uv_vectors_to_4_v210_vectors_ ## instr_set,\
				convert_8bit_r_g_b_vectors_to_10bit_y_vector_sse2,\
				convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_sse2,\
				instr_set\
		)
/*
 * We have 2 RGB to YUV422 conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 *
 * The following table summarises the timing results: each value is the difference
 * between the conversion time using the first implementation and conversion time
 * using the second one. Values are in milliseconds, a negative value means
 * the first one is faster, a positive value means the second one is faster.
 *
 * 															Mac OSX - Xeon	  Linux - i5
 * ARGB to YUYV - SSE2 / SSSE3- fast downsampling 				-0.16			-0.16 		(2)
 * ARGB to YUYV - SSE2 - fast downsampling 						 0.10			 0.04 		(1)
 * ARGB to YUYV - SSE2 / SSSE3 									-0.06			 0.05 		(3)
 * ARGB to YUYV - SSE2 											 0.52			 0.59 		(1)
 * ARGB to YUYV - bt601 - SSE2 / SSSE3- fast downsampling 		-0.17			-0.58 		(2)
 * ARGB to YUYV - bt601 - SSE2 - fast downsampling 				 0.14			 0.06 		(1)
 * ARGB to YUYV - bt601 - SSE2 / SSSE3 							-0.03			 0.08 		(3)
 * ARGB to YUYV - bt601 - SSE2 									 0.40			 0.58		(1)
 * ARGB to YUYV - bt709 - SSE2 / SSSE3- fast downsampling 		-0.17			-0.15 		(2)
 * ARGB to YUYV - bt709 - SSE2 - fast downsampling 				 0.14			 0.07 		(1)
 * ARGB to YUYV - bt709 - SSE2 / SSSE3 							-0.04			 0.08 		(3)
 * ARGB to YUYV - bt709 - SSE2 									 0.39			 0.59		(1)
 *
 * As can be seen in the above table, ALL SSE2-only conversions are faster using
 * the second implementation (see lines marked (1) above), whereas half hybrid
 * SSE2-SSSE3 conversions perform faster with the first implementation
 * (lines marked (2) ), and the other half performs differently on Mac and Linux
 * (lines marked (3) ).
 *
 * Conclusion:
 * The first implementation is used for hybrid SSE2-SSSE3 routines
 * The second implementation is used for SSE2-only routines
 */

// ARGB to YUYV			SSE2 SSSE3
void		convert_argb_to_yuyv_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_argb_to_yuyv_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2_ssse3);
}


// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}

void		downsample_n_convert_argb_to_yuyv_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, sse2);
}


// ARGB to UYVY			SSE2 SSSE3
void		convert_argb_to_uyvy_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}

void		downsample_n_convert_argb_to_uyvy_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2_ssse3);
}


// ARGB to UYVY			SSE2
void		convert_argb_to_uyvy_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}

void		downsample_n_convert_argb_to_uyvy_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422I(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, sse2);
}


// ARGB to YUV422P			SSE2 SSSE3
void		convert_argb_to_yuv422p_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV422P(sse2_ssse3);
}

void		downsample_n_convert_argb_to_yuv422p_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_YUV422P(sse2_ssse3);
}


// ARGB to YUV422P			SSE2
void		convert_argb_to_yuv422p_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV422P(sse2);
}

void		downsample_n_convert_argb_to_yuv422p_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT2_TO_YUV422P(sse2);
}


// ARGB to YUV420P			SSE2 SSSE3
void		convert_argb_to_yuv420p_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_YUV420P(sse2_ssse3);
}

// ARGB to YUV420P			SSE2
void		convert_argb_to_yuv420p_sse2(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT2_TO_YUV420P(sse2);
}


// ARGB to V210				SSE2 SSSE3 SSE41
void		convert_argb_to_v210_sse2_ssse3_sse41(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_V210(sse2_ssse3_sse41);
}

void		downsample_n_convert_argb_to_v210_sse2_ssse3_sse41(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_V210(sse2_ssse3_sse41);
}

// ARGB to V210				SSE2 SSSE3
void		convert_argb_to_v210_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	CONVERT_TO_V210(sse2_ssse3);
}

void		downsample_n_convert_argb_to_v210_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DOWNSAMPLE_N_CONVERT_TO_V210(sse2_ssse3);
}


// RGB to YUV422		NON SSE
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
		} else\
			printf("Unknown source pixel format in non-SSE conversion from RGB\n");\
	}while(0)\


#define 	DEFINE_ANY_RGB_TO_YUV422(fn_name, y_offset, u_offset, v_offset, yr_coef, yg_coef, yb_coef, ur_coef, ug_coef, ub_coef, vr_coef, vg_coef, vb_coef) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out) {\
		PixFcPixelFormat 	dest_fmt = conv->dest_fmt;\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		uint32_t 			pixel_num = 0;\
		uint32_t			pixel_count = conv->pixel_count;\
		uint8_t*			src = (uint8_t *) in;\
		uint8_t*			dst = (uint8_t *) out;\
		uint8_t*			y_plane = dst;\
		uint8_t*			u_plane = dst + pixel_count;\
		uint8_t*			v_plane = u_plane + pixel_count / 2;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;\
		int32_t				y1, y2, u, v;\
		while(pixel_num < pixel_count){\
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
			y1 = (((yr_coef) * r1 + (yg_coef) * g1 + (yb_coef) * b1) >> 8) + y_offset;\
			u =  (((ur_coef) * r1 + (ug_coef) * g1 + (ub_coef) * b1) >> 8) + u_offset;\
			v =  (((vr_coef) * r1 + (vg_coef) * g1 + (vb_coef) * b1) >> 8) + v_offset;\
			y2 = (((yr_coef) * r2 + (yg_coef) * g2 + (yb_coef) * b2) >> 8) + y_offset;\
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
			pixel_num += 2;\
		}\
	}\

DEFINE_ANY_RGB_TO_YUV422(convert_rgb_to_yuv422_nonsse, 0, 128, 128, 77, 150, 29, -43, -85, 128, 128, -107, -21)
DEFINE_ANY_RGB_TO_YUV422(convert_rgb_to_yuv422_bt601_nonsse, 16, 128, 128, 66, 129, 25, -38, -74, 112, 112, -94, -18)
DEFINE_ANY_RGB_TO_YUV422(convert_rgb_to_yuv422_bt709_nonsse, 16, 128, 128, 47, 157, 16, -26, -87, 112, 112, -102, -10)

// RGB to v210		NON SSE

#define 	DEFINE_ANY_RGB_TO_V210_FN(fn_name, y_offset, u_offset, v_offset, yr_coef, yg_coef, yb_coef, ur_coef, ug_coef, ub_coef, vr_coef, vg_coef, vb_coef) \
	void 		fn_name(const struct PixFcSSE* conv, void* in, void* out)\
	{\
		PixFcPixelFormat 	src_fmt = conv->source_fmt;\
		uint32_t 			pixel_num = 0;\
		uint32_t			pixel_count = conv->pixel_count;\
		uint8_t*			src = (uint8_t *) in;\
		uint32_t*			dst = (uint32_t *) out;\
		int32_t				r1 = 0, g1 = 0, b1 = 0, r2 = 0, g2 = 0, b2 = 0;\
		int32_t				y1, y2, u, v;\
		while(pixel_num < pixel_count){\
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
			y1 = (((yr_coef) * r1 + (yg_coef) * g1 + (yb_coef) * b1) >> 8) + y_offset;\
			u = (((ur_coef) * r1 + (ug_coef) * g1 + (ub_coef) * b1) >> 8) + u_offset;\
			v = (((vr_coef) * r1 + (vg_coef) * g1 + (vb_coef) * b1) >> 8) + v_offset;\
			y2 = (((yr_coef) * r2 + (yg_coef) * g2 + (yb_coef) * b2) >> 8) + y_offset;\
			*dst = CLIP_10BIT_PIXEL(u) & 0x3FF;\
			*dst |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 10;\
			*(dst++) |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 20;\
			*dst = CLIP_10BIT_PIXEL(y2) & 0x3FF;\
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
			y1 = (((yr_coef) * r1 + (yg_coef) * g1 + (yb_coef) * b1) >> 8) + y_offset;\
			u = (((ur_coef) * r1 + (ug_coef) * g1 + (ub_coef) * b1) >> 8) + u_offset;\
			v = (((vr_coef) * r1 + (vg_coef) * g1 + (vb_coef) * b1) >> 8) + v_offset;\
			y2 = (((yr_coef) * r2 + (yg_coef) * g2 + (yb_coef) * b2) >> 8) + y_offset;\
			*dst |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 10;\
			*(dst++) |= (CLIP_10BIT_PIXEL(y1) & 0x3FF) << 20;\
			*dst = CLIP_10BIT_PIXEL(v) & 0x3FF;\
			*dst |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 10;\
			UNPACK_RGB(src, r1, g1, b1, r2, g2, b2, src_fmt);\
			y1 = (((yr_coef) * r1 + (yg_coef) * g1 + (yb_coef) * b1) >> 8) + y_offset;\
			u = (((ur_coef) * r1 + (ug_coef) * g1 + (ub_coef) * b1) >> 8) + u_offset;\
			v = (((vr_coef) * r1 + (vg_coef) * g1 + (vb_coef) * b1) >> 8) + v_offset;\
			y2 = (((yr_coef) * r2 + (yg_coef) * g2 + (yb_coef) * b2) >> 8) + y_offset;\
			*(dst++) |= (CLIP_10BIT_PIXEL(u) & 0x3FF) << 20;\
			*dst = CLIP_10BIT_PIXEL(y1) & 0x3FF;\
			*dst |= (CLIP_10BIT_PIXEL(v) & 0x3FF) << 10;\
			*(dst++) |= (CLIP_10BIT_PIXEL(y2) & 0x3FF) << 20;\
			pixel_num += 6;\
		}\
	}\

DEFINE_ANY_RGB_TO_V210_FN(convert_rgb_to_v210_nonsse, 0, 512, 512, 306, 601, 117, -173, -339, 512, 512, -429, -83);
DEFINE_ANY_RGB_TO_V210_FN(convert_rgb_to_v210_bt601_nonsse, 64, 512, 512, 263, 516, 100, -152, -298, 450, 450, -377, -73);
DEFINE_ANY_RGB_TO_V210_FN(convert_rgb_to_v210_bt709_nonsse,	64, 512, 512, 187, 629, 63,	-103, -347, 450, 450, -409, -41);

// RGB to YUV420		NON SSE
#define 	DEFINE_ANY_RGB_TO_YUV420_FN(fn_name, y_offset, u_offset, v_offset, yr_coef, yg_coef, yb_coef, ur_coef, ug_coef, ub_coef, vr_coef, vg_coef, vb_coef) \
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
			y1_line1 = ((yr_coef * r1_line1 + yg_coef * g1_line1 + yb_coef * b1_line1) >> 8) + y_offset;\
			y2_line1 = ((yr_coef * r2_line1 + yg_coef * g2_line1 + yb_coef * b2_line1) >> 8) + y_offset;\
			y1_line2 = ((yr_coef * r1_line2 + yg_coef * g1_line2 + yb_coef * b1_line2) >> 8) + y_offset;\
			y2_line2 = ((yr_coef * r2_line2 + yg_coef * g2_line2 + yb_coef * b2_line2) >> 8) + y_offset;\
			r1_line1 = (r1_line1 + r2_line1 + r1_line2 + r2_line2) / 4;\
			g1_line1 = (g1_line1 + g2_line1 + g1_line2 + g2_line2) / 4;\
			b1_line1 = (b1_line1 + b2_line1 + b1_line2 + b2_line2) / 4;\
			u = ((ur_coef * r1_line1 + ug_coef * g1_line1 + ub_coef * b1_line1) >> 8) + u_offset;\
			v = ((vr_coef * r1_line1 + vg_coef * g1_line1 + vb_coef  * b1_line1) >> 8) + v_offset;\
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


DEFINE_ANY_RGB_TO_YUV420_FN(convert_rgb_to_yuv420_nonsse, 0, 128, 128, 77, 150, 29, -43, -85, 128, 128, -107, -21);
DEFINE_ANY_RGB_TO_YUV420_FN(convert_rgb_to_yuv420_bt601_nonsse, 16, 128, 128, 66, 129, 25, -38, -74, 112, 112, -94, -18);
DEFINE_ANY_RGB_TO_YUV420_FN(convert_rgb_to_yuv420_bt709_nonsse, 16, 128, 128, 47, 157, 16, -26, -87, 112, 112, -102, -10);
