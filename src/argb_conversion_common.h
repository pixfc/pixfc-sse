/*
 * argb_conversion_common.h
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

#ifndef ARGB_CONVERSION_COMMON_H_
#define ARGB_CONVERSION_COMMON_H_

#include "common.h"

#ifndef DEBUG
/*
 * Include the unpack & pack routines twice to generate both aligned & unaligned versions
 */
#undef  GENERATE_UNALIGNED_INLINES
#define GENERATE_UNALIGNED_INLINES 0
#include "rgb_unpack.h"
#include "yuv_pack.h"

#undef  GENERATE_UNALIGNED_INLINES
#define GENERATE_UNALIGNED_INLINES 1
#include "rgb_unpack.h"
#include "yuv_pack.h"

#include "rgb_downsample.h"
#include "rgb_to_yuv_convert.h"
#endif



/*
 * 		R G B 3 2
 *
 * 		T O
 *
 * 		Y U V 4 2 2
 *
 *
 *
 * We have 2 RGB32 to YUV422 conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 */

// NNB Core conversion loop, common to RGB32 to YUV422 planar & interleaved NNB conversions
#define RGB32_TO_YUV422_NNB_LOOP_CORE(unpack_fn, downsample_fn, y_conv_fn, uv_conv_fn)\
	unpack_fn(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, convert_out);\
	downsample_fn(unpack_out, unpack_out);\
	uv_conv_fn(unpack_out, &convert_out[1]);\
	rgb_in += 2;\
	unpack_fn(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, &convert_out[2]);\
	downsample_fn(unpack_out, unpack_out);\
	uv_conv_fn(unpack_out, &convert_out[3]);\
	rgb_in += 2;\
	pixel_count -= 16;

//  NNB Interleaved conversion 1
#define CONVERT_RGB32_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				unpack_fn_prefix##instr_set,\
				nnb_422_downsample_r_g_b_vectors_##instr_set,\
				y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// NNB planar conversion 1
#define CONVERT_RGB32_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*) yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*) uplane_out + pixel_count / 2);\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				unpack_fn_prefix##instr_set,\
				nnb_422_downsample_r_g_b_vectors_##instr_set,\
				y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				unpack_fn_prefix##instr_set,\
				nnb_422_downsample_r_g_b_vectors_##instr_set,\
				y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\

// NNB interleave conversion 2
#define CONVERT2_RGB32_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[4];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				unpack_fn_prefix##instr_set,\
				nnb_422_downsample_ag_rb_vectors_##instr_set,\
				y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// NNB planar conversion 2
#define CONVERT2_RGB32_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*) yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*) uplane_out + pixel_count / 2);\
	__m128i		unpack_out[4];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				unpack_fn_prefix##instr_set,\
				nnb_422_downsample_ag_rb_vectors_##instr_set,\
				y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				unpack_fn_prefix##instr_set,\
				nnb_422_downsample_ag_rb_vectors_##instr_set,\
				y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\




//
// AVG Core conversion loop, common to RGB32 to YUV422 planar & interleaved AVG conversion 1
#define RGB32_TO_YUV422_AVG_CORE_LOOP1(unpack_fn, y_conv_fn, downsample1_fn, downsample1_out, downsample2_fn, downsample2_out, uv_conv_fn) \
	unpack_fn(rgb_in, unpack_out);\
	print_xmm8u("R 1-8:", &unpack_out[0]);\
	print_xmm8u("G 1-8:", &unpack_out[1]);\
	print_xmm8u("B 1-8:", &unpack_out[2]);\
	y_conv_fn(unpack_out, convert_out);\
	print_xmm8u("Y1-8", convert_out);\
	downsample1_fn(unpack_out, previous, downsample1_out);\
	uv_conv_fn(downsample1_out, &convert_out[1]);\
	print_xmm8u("UV1-4", &convert_out[1]);\
	rgb_in += 2;\
	unpack_fn(rgb_in, unpack_out);\
	print_xmm8u("R 1-8:", &unpack_out[0]);\
	print_xmm8u("G 1-8:", &unpack_out[1]);\
	print_xmm8u("B 1-8:", &unpack_out[2]);\
	y_conv_fn(unpack_out, &convert_out[2]);\
	print_xmm8u("Y1-8", convert_out);\
	downsample2_fn(unpack_out, previous, downsample2_out);\
	uv_conv_fn(downsample2_out, &convert_out[3]);\
	print_xmm8u("UV1-4", &convert_out[1]);\
	rgb_in += 2;\
	pixel_count -= 16;

// Average interleave conversion 1
#define AVG_DOWNSAMPLE_N_CONVERT_RGB32_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[3];\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	RGB32_TO_YUV422_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_first_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			uv_conv_fn);\
	pack_fn(convert_out, yuv_out);\
	yuv_out += 2;\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// Average planar conversion 1
#define AVG_DOWNSAMPLE_N_CONVERT_RGB32_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixel_count / 2);\
	__m128i		previous[3];\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	RGB32_TO_YUV422_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_first_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			uv_conv_fn);\
	pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	RGB32_TO_YUV422_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			uv_conv_fn);\
	pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	uplane_out++;\
	vplane_out++;\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB32_TO_YUV422_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\


// AVG Core conversion loop, common to RGB32 to YUV422 planar & interleaved AVG conversion 1
#define RGB32_TO_YUV422_AVG_CORE_LOOP2(unpack_fn, downsample1_fn, downsample1_out, downsample2_fn, downsample2_out, y_conv_fn, uv_conv_fn) \
	unpack_fn(rgb_in, unpack_out);\
	print_xmm16u("AG 1-4:", &unpack_out[0]);\
	print_xmm16u("RB 1-4:", &unpack_out[1]);\
	print_xmm16u("AG 5-8:", &unpack_out[2]);\
	print_xmm16u("RB 5-8:", &unpack_out[3]);\
	downsample1_fn(unpack_out, previous, downsample1_out);\
	print_xmm16u("Previous AG (=5-8):", &previous[0]);\
	print_xmm16u("Previous RB (=5-8):", &previous[1]);\
	y_conv_fn(unpack_out, convert_out);\
	print_xmm16u("Y1-8:", convert_out);\
	uv_conv_fn(downsample1_out, &convert_out[1]);\
	print_xmm16u("UV1-4:", &convert_out[1]);\
	rgb_in += 2;\
	unpack_fn(rgb_in, unpack_out);\
	print_xmm16u("AG 1-4:", &unpack_out[0]);\
	print_xmm16u("RB 1-4:", &unpack_out[1]);\
	print_xmm16u("AG 5-8:", &unpack_out[2]);\
	print_xmm16u("RB 5-8:", &unpack_out[3]);\
	downsample2_fn(unpack_out, previous, downsample2_out);\
	print_xmm16u("Previous AG (=5-8):", &previous[0]);\
	print_xmm16u("Previous RB (=5-8):", &previous[1]);\
	y_conv_fn(unpack_out, &convert_out[2]);\
	print_xmm16u("Y1-8:", convert_out);\
	uv_conv_fn(downsample2_out, &convert_out[3]);\
	print_xmm16u("UV1-4:", &convert_out[1]);\
	rgb_in += 2;\
	pixel_count -= 16;\

// AVG interleave conversion 2
#define AVG_DOWNSAMPLE_N_CONVERT2_RGB32_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[2];\
	__m128i		unpack_out[4];\
	__m128i		downsample_out[2];\
	__m128i		convert_out[4];\
	RGB32_TO_YUV422_AVG_CORE_LOOP2(\
		unpack_fn_prefix##instr_set,\
		avg_422_downsample_first_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
		y_conv_fn, uv_conv_fn);\
	pack_fn(convert_out, yuv_out);\
	yuv_out += 2;\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_AVG_CORE_LOOP2(\
			unpack_fn_prefix##instr_set,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	}

// AVG planar conversion 2
#define AVG_DOWNSAMPLE_N_CONVERT2_RGB32_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixel_count / 2);\
	__m128i		previous[2];\
	__m128i		unpack_out[4];\
	__m128i		downsample_out[2];\
	__m128i		convert_out[4];\
	RGB32_TO_YUV422_AVG_CORE_LOOP2(\
		unpack_fn_prefix##instr_set,\
		avg_422_downsample_first_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, unpack_out,\
		y_conv_fn, uv_conv_fn);\
	pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	RGB32_TO_YUV422_AVG_CORE_LOOP2(\
			unpack_fn_prefix##instr_set,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, unpack_out,\
			y_conv_fn, uv_conv_fn);\
	pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	uplane_out++;\
	vplane_out++;\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_AVG_CORE_LOOP2(\
			unpack_fn_prefix##instr_set,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, unpack_out,\
			y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB32_TO_YUV422_AVG_CORE_LOOP2(\
			unpack_fn_prefix##instr_set, \
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, unpack_out,\
			y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\





/*
 * 		R G B 2 4
 *
 * 		T O
 *
 * 		Y U V 4 2 2
 *
 *
 * We have 2 RGB24 to YUV422 conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 *
 */

// NNB Core conversion loop, common to RGB24 to YUV422 planar & interleaved NNB conversions
#define RGB24_TO_YUV422_NNB_LOOP_CORE(offset_2nd_grp, unpack_fn, downsample_fn, y_conv_fn, uv_conv_fn)\
		unpack_fn(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		downsample_fn(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		y_conv_fn(&unpack_out[offset_2nd_grp], &convert_out[2]);\
		downsample_fn(&unpack_out[offset_2nd_grp], &unpack_out[offset_2nd_grp]);\
		uv_conv_fn(&unpack_out[offset_2nd_grp], &convert_out[3]);\
		rgb_in += 3;\
		pixel_count -= 16;\

// NNB interleave conversion 1
#define CONVERT_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[6];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_NNB_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			nnb_422_downsample_r_g_b_vectors_##instr_set,\
			y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// NNB planar conversion 1
#define CONVERT_RGB24_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixel_count / 2);\
	__m128i		unpack_out[6];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_NNB_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			nnb_422_downsample_r_g_b_vectors_##instr_set,\
			y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB24_TO_YUV422_NNB_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			nnb_422_downsample_r_g_b_vectors_##instr_set,\
			y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\

// NNB interleave conversion 2
#define CONVERT2_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[8];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_NNB_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			nnb_422_downsample_ag_rb_vectors_##instr_set,\
			y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// NNB planar conversion 2
#define CONVERT2_RGB24_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixel_count / 2);\
	__m128i		unpack_out[8];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_NNB_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			nnb_422_downsample_ag_rb_vectors_##instr_set,\
			y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB24_TO_YUV422_NNB_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			nnb_422_downsample_ag_rb_vectors_##instr_set,\
			y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\




// AVG Core conversion loop, common to RGB24 to YUV422 planar & interleaved AVG conversion 1 & 2
#define RGB24_TO_YUV422_AVG_LOOP_CORE(offset_2nd_grp, unpack_fn, downsample1_fn, downsample1_out, downsample2_fn, downsample2_out, y_conv_fn, uv_conv_fn)\
	unpack_fn(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, convert_out);\
	downsample1_fn(unpack_out, previous, downsample1_out);\
	uv_conv_fn(downsample1_out, &convert_out[1]);\
	y_conv_fn(&unpack_out[offset_2nd_grp], &convert_out[2]);\
	downsample2_fn(&unpack_out[offset_2nd_grp], previous, downsample2_out);\
	uv_conv_fn(downsample2_out, &convert_out[3]);\
	rgb_in += 3;\
	pixel_count -= 16;\

// AVG interleave conversion 1
#define AVG_DOWNSAMPLE_N_CONVERT_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[3];\
	__m128i		unpack_out[6];\
	__m128i		convert_out[4];\
	RGB24_TO_YUV422_AVG_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			avg_422_downsample_first_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, &unpack_out[3],\
			y_conv_fn, uv_conv_fn);\
	pack_fn(convert_out, yuv_out);\
	yuv_out += 2;\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_AVG_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, &unpack_out[3],\
			y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// AVG planar conversion 1
#define AVG_DOWNSAMPLE_N_CONVERT_RGB24_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t *) yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t *) uplane_out + pixel_count / 2);\
	__m128i		previous[3];\
	__m128i		unpack_out[6];\
	__m128i		convert_out[4];\
	RGB24_TO_YUV422_AVG_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			avg_422_downsample_first_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, &unpack_out[3],\
			y_conv_fn, uv_conv_fn);\
	pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	RGB24_TO_YUV422_AVG_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, &unpack_out[3],\
			y_conv_fn, uv_conv_fn);\
	pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	uplane_out++;\
	vplane_out++;\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_AVG_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, &unpack_out[3],\
			y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB24_TO_YUV422_AVG_LOOP_CORE(\
			3, unpack_fn_prefix##instr_set,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, unpack_out,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set, &unpack_out[3],\
			y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\

// AVG interleave conversion 2
#define AVG_DOWNSAMPLE_N_CONVERT2_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[2];\
	__m128i		unpack_out[8];\
	__m128i		downsample_out[2];\
	__m128i		convert_out[4];\
	RGB24_TO_YUV422_AVG_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			avg_422_downsample_first_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			y_conv_fn, uv_conv_fn);\
	pack_fn(convert_out, yuv_out);\
	yuv_out += 2;\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_AVG_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, unpack_out,\
			y_conv_fn, uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// AVG planar conversion 2
#define AVG_DOWNSAMPLE_N_CONVERT2_RGB24_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t *) yplane_out + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t *) uplane_out + pixel_count / 2);\
	__m128i		previous[2];\
	__m128i		unpack_out[8];\
	__m128i		downsample_out[2];\
	__m128i		convert_out[4];\
	RGB24_TO_YUV422_AVG_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			avg_422_downsample_first_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			y_conv_fn, uv_conv_fn);\
	pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	RGB24_TO_YUV422_AVG_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			y_conv_fn, uv_conv_fn);\
	pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
	yplane_out++;\
	uplane_out++;\
	vplane_out++;\
	while(pixel_count > 0) {\
		RGB24_TO_YUV422_AVG_LOOP_CORE(\
			4, unpack_fn_prefix##instr_set,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
			avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, unpack_out,\
			y_conv_fn, uv_conv_fn);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB24_TO_YUV422_AVG_LOOP_CORE(\
				4, unpack_fn_prefix##instr_set,\
				avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
				avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set, downsample_out,\
				y_conv_fn, uv_conv_fn);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\

#endif /* ARGB_CONVERSION_COMMON_H_ */
