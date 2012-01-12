/*
 * rgb_conversion_recipes.h
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

#ifndef RGB_CONVERSION_RECIPES_H_
#define RGB_CONVERSION_RECIPES_H_

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
#define RGB32_TO_YUV422I_RECIPE(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define RGB32_TO_YUV422P_RECIPE(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define RGB32_TO_YUV422I_RECIPE2(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define RGB32_TO_YUV422P_RECIPE2(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB32_TO_YUV422P_RECIPE(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB32_TO_YUV422I_RECIPE2(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB32_TO_YUV422P_RECIPE2(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
 * 		R G B 3 2
 *
 * 		T O
 *
 * 		Y U V 4 2 0 P
 *
 *
 *
 * We have 2 RGB32 to YUV420P conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 */


//
// NNB Core conversion loop for RGB32 to YUV420 planar NNB conversion 1
#define RGB32_TO_YUV420_NNB_CORE_LOOP1(unpack_fn, y_conv_fn, downsample_fn, uv_conv_fn, y_pack_fn, uv_pack_fn) \
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 R 1-8:", &unpack_out[0]);\
	print_xmm8u("l1 G 1-8:", &unpack_out[1]);\
	print_xmm8u("l1 B 1-8:", &unpack_out[2]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y1-8", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[3]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 R 1-8:", &unpack_out[3]);\
	print_xmm8u("l2 G 1-8:", &unpack_out[4]);\
	print_xmm8u("l2 B 1-8:", &unpack_out[5]);\
	y_conv_fn(&unpack_out[3], &convert_out[4]);\
	print_xmm8u("l2 Y1-8", &convert_out[4]);\
	downsample_fn(unpack_out, &unpack_out[3], downsample_out);\
	print_xmm8u("l12 420 downsampled R 1-4:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 1-4:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 1-4:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, convert_out);\
	print_xmm8u("l12 UV1-4", convert_out);\
	/* 												 	*/\
	/* Next set of 8 pixels (9-16) along the same line 	*/\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 R 9-16:", &unpack_out[0]);\
	print_xmm8u("l1 G 9-16:", &unpack_out[1]);\
	print_xmm8u("l1 B 9-16:", &unpack_out[2]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(unpack_out, downsample_out);\
	print_xmm8u("l1 Y9-16", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	unpack_fn(rgb_line2, &unpack_out[3]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 R 9-16:", &unpack_out[3]);\
	print_xmm8u("l2 G 9-16:", &unpack_out[4]);\
	print_xmm8u("l2 B 9-16:", &unpack_out[5]);\
	y_conv_fn(&unpack_out[3], &downsample_out[1]);\
	print_xmm8u("l2 Y9-16", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(unpack_out, &unpack_out[3], downsample_out);\
	print_xmm8u("l12 420 downsampled R 5-8:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 5-8:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 5-8:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, &convert_out[1]);\
	print_xmm8u("l12 UV5-8", &convert_out[1]);\
	/* 												 	*/\
	/* Next set of 8 pixels (17-24) along the same line */\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 R 17-24:", &unpack_out[0]);\
	print_xmm8u("l1 G 17-24:", &unpack_out[1]);\
	print_xmm8u("l1 B 17-24:", &unpack_out[2]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y17-24", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[3]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 R 17-24:", &unpack_out[3]);\
	print_xmm8u("l2 G 17-24:", &unpack_out[4]);\
	print_xmm8u("l2 B 17-24:", &unpack_out[5]);\
	y_conv_fn(&unpack_out[3], &convert_out[4]);\
	print_xmm8u("l2 Y17-24", &convert_out[4]);\
	downsample_fn(unpack_out, &unpack_out[3], downsample_out);\
	print_xmm8u("l12 420 downsampled R 9-12:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 9-12:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 9-12:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, &convert_out[2]);\
	/* so far, convert_out[0] holds UV1-4, [1] UV5-8, [2] UV9-12*/\
	print_xmm8u("l12 UV9-12", &convert_out[2]);\
	/* 											 		*/\
	/* Next set of 8 pixels (25-32) along the same line */\
	/* 											 		*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 R 25-32:", &unpack_out[0]);\
	print_xmm8u("l1 G 25-32:", &unpack_out[1]);\
	print_xmm8u("l1 B 25-32:", &unpack_out[2]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(unpack_out, downsample_out);\
	print_xmm8u("l1 Y25-32", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	unpack_fn(rgb_line2, &unpack_out[3]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 R 25-32:", &unpack_out[3]);\
	print_xmm8u("l2 G 25-32:", &unpack_out[4]);\
	print_xmm8u("l2 B 25-32:", &unpack_out[5]);\
	y_conv_fn(&unpack_out[3], &downsample_out[1]);\
	print_xmm8u("l2 Y25-32", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(unpack_out, &unpack_out[3], downsample_out);\
	print_xmm8u("l12 420 downsampled R 13-16:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 13-16:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 13-16:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, &convert_out[3]);\
	print_xmm8u("l12 UV13-16", &convert_out[3]);\
	uv_pack_fn(convert_out, uplane_out, vplane_out);\
	uplane_out++;\
	vplane_out++;



// NNB planar conversion 1
#define RGB32_TO_YUV420P_RECIPE(unpack_fn_prefix, y_pack_fn, uv_pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	lines_remaining = pixfc->height;\
	uint32_t	pixels_remaining_on_line = pixfc->width;\
	__m128i*	rgb_line1 = (__m128i *) source_buffer;\
	__m128i*	rgb_line2 = (__m128i *) ((uint8_t*)source_buffer + pixfc->width*4);\
	__m128i*	yplane_line1 = (__m128i *) dest_buffer;\
	__m128i*	yplane_line2 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixfc->pixel_count / 4);\
	__m128i		unpack_out[6];\
	__m128i		downsample_out[3];\
	/* [0]: l12   	UV 1-4*/\
	/* [1]: l12   	UV 5-8*/\
	/* [2]: l12   	UV 9-12*/\
	/* [3]: l12   	UV 13-16*/\
	/*  OR  l1		Y  1-8*/\
	/* [4]: l2   	Y  1-8*/\
	__m128i		convert_out[5];\
	while(lines_remaining > 0){\
		while(pixels_remaining_on_line > 0) {\
			RGB32_TO_YUV420_NNB_CORE_LOOP1(\
					unpack_fn_prefix##instr_set, y_conv_fn,\
					avg_420_downsample_r_g_b_vectors_##instr_set,\
					uv_conv_fn, y_pack_fn, uv_pack_fn);\
			/* The above macro effectively handles 64 pixels (32 on two adjacent lines)*/ \
			/* but we need to run it only for as many pixels as there is on one line */ \
			pixels_remaining_on_line -= 32;\
		};\
		/* the inner while loop handles two lines at a time */\
		lines_remaining -= 2;\
		rgb_line1 = (__m128i *) ((uint8_t*)rgb_line1 + pixfc->width * 4);\
		rgb_line2 = (__m128i *) ((uint8_t*)rgb_line2 + pixfc->width * 4);\
		yplane_line1 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
		yplane_line2 = (__m128i *) ((uint8_t*)yplane_line2 + pixfc->width);\
		pixels_remaining_on_line = pixfc->width;\
	};\


// NNB Core conversion loop for RGB32 to YUV420 planar NNB conversion 2
#define RGB32_TO_YUV420_NNB_CORE_LOOP2(unpack_fn, y_conv_fn, downsample_fn, uv_conv_fn, y_pack_fn, uv_pack_fn) \
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 1-4:", &unpack_out[0]);\
	print_xmm8u("l1 RB 1-4:", &unpack_out[1]);\
	print_xmm8u("l1 AG 5-8:", &unpack_out[2]);\
	print_xmm8u("l1 RB 5-8:", &unpack_out[3]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y1-8", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 1-4:", &unpack_out[4]);\
	print_xmm8u("l2 RB 1-4:", &unpack_out[5]);\
	print_xmm8u("l2 AG 5-8:", &unpack_out[6]);\
	print_xmm8u("l2 RB 5-8:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &convert_out[4]);\
	print_xmm8u("l2 Y1-8", &convert_out[4]);\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled AG 1-4:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled RB 1-4:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, convert_out);\
	print_xmm8u("l12 UV1-4", convert_out);\
	/* 												 	*/\
	/* Next set of 8 pixels (9-16) along the same line 	*/\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 9-12:", &unpack_out[0]);\
	print_xmm8u("l1 RB 9-12:", &unpack_out[1]);\
	print_xmm8u("l1 AG 13-16:", &unpack_out[2]);\
	print_xmm8u("l1 RB 13-16:", &unpack_out[3]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(unpack_out, downsample_out);\
	print_xmm8u("l1 Y9-16", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 9-12:", &unpack_out[4]);\
	print_xmm8u("l2 RB 9-12:", &unpack_out[5]);\
	print_xmm8u("l2 AG 12-16:", &unpack_out[6]);\
	print_xmm8u("l2 RB 12-16:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &downsample_out[1]);\
	print_xmm8u("l2 Y9-16", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled AG 5-8:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled RB 5-8:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, &convert_out[1]);\
	print_xmm8u("l12 UV5-8", &convert_out[1]);\
	/* 												 	*/\
	/* Next set of 8 pixels (17-24) along the same line */\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 17-20:", &unpack_out[0]);\
	print_xmm8u("l1 RB 17-20:", &unpack_out[1]);\
	print_xmm8u("l1 AG 21-24:", &unpack_out[2]);\
	print_xmm8u("l1 RB 21-24:", &unpack_out[3]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y17-24", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 17-20:", &unpack_out[4]);\
	print_xmm8u("l2 RB 17-20:", &unpack_out[5]);\
	print_xmm8u("l2 AG 21-24:", &unpack_out[6]);\
	print_xmm8u("l2 RB 21-24:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &convert_out[4]);\
	print_xmm8u("l2 Y17-24", &convert_out[4]);\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled AG 9-12:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled RB 9-12:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, &convert_out[2]);\
	/* so far, convert_out[0] holds UV1-4, [1] UV5-8, [2] UV9-12*/\
	print_xmm8u("l12 UV9-12", &convert_out[2]);\
	/* 											 		*/\
	/* Next set of 8 pixels (25-32) along the same line */\
	/* 											 		*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 25-28:", &unpack_out[0]);\
	print_xmm8u("l1 RB 25-28:", &unpack_out[1]);\
	print_xmm8u("l1 AG 29-32:", &unpack_out[2]);\
	print_xmm8u("l1 RB 29-32:", &unpack_out[3]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(unpack_out, downsample_out);\
	print_xmm8u("l1 Y25-32", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 25-28:", &unpack_out[4]);\
	print_xmm8u("l2 RB 25-28:", &unpack_out[5]);\
	print_xmm8u("l2 AG 28-32:", &unpack_out[6]);\
	print_xmm8u("l2 RB 28-32:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &downsample_out[1]);\
	print_xmm8u("l2 Y24-32", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled R 13-16:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 13-16:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, &convert_out[3]);\
	print_xmm8u("l12 UV13-16", &convert_out[3]);\
	uv_pack_fn(convert_out, uplane_out, vplane_out);\
	uplane_out++;\
	vplane_out++;

// NNB planar conversion 2
#define RGB32_TO_YUV420P_RECIPE2(unpack_fn_prefix, y_pack_fn, uv_pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	lines_remaining = pixfc->height;\
	uint32_t	pixels_remaining_on_line = pixfc->width;\
	__m128i*	rgb_line1 = (__m128i *) source_buffer;\
	__m128i*	rgb_line2 = (__m128i *) ((uint8_t*)source_buffer + pixfc->width*4);\
	__m128i*	yplane_line1 = (__m128i *) dest_buffer;\
	__m128i*	yplane_line2 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixfc->pixel_count / 4);\
	__m128i		unpack_out[8];\
	__m128i		downsample_out[2];\
	/* [0]: l12   	UV 1-4*/\
	/* [1]: l12   	UV 5-8*/\
	/* [2]: l12   	UV 9-12*/\
	/* [3]: l12   	UV 13-16*/\
	/*  OR  l1		Y  1-8*/\
	/* [4]: l2   	Y  1-8*/\
	__m128i		convert_out[5];\
	while(lines_remaining > 0){\
		while(pixels_remaining_on_line > 0) {\
			RGB32_TO_YUV420_NNB_CORE_LOOP2(\
					unpack_fn_prefix##instr_set, y_conv_fn,\
					avg_420_downsample_ag_rb_vectors_##instr_set,\
					uv_conv_fn, y_pack_fn, uv_pack_fn);\
			/* The above macro effectively handles 64 pixels (32 on two adjacent lines)*/ \
			/* but we need to run it only for as many pixels as there is on one line */ \
			pixels_remaining_on_line -= 32;\
		};\
		/* the inner while loop handles two lines at a time */\
		lines_remaining -= 2;\
		rgb_line1 = (__m128i *) ((uint8_t*)rgb_line1 + pixfc->width * 4);\
		rgb_line2 = (__m128i *) ((uint8_t*)rgb_line2 + pixfc->width * 4);\
		yplane_line1 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
		yplane_line2 = (__m128i *) ((uint8_t*)yplane_line2 + pixfc->width);\
		pixels_remaining_on_line = pixfc->width;\
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
#define RGB24_TO_YUV422I_RECIPE(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define RGB24_TO_YUV422P_RECIPE(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define RGB24_TO_YUV422I_RECIPE2(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define RGB24_TO_YUV422P_RECIPE2(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB24_TO_YUV422I_RECIPE(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB24_TO_YUV422P_RECIPE(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB24_TO_YUV422I_RECIPE2(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
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
#define AVG_DOWNSAMPLE_RGB24_TO_YUV422P_RECIPE2(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
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




/*
 * 		R G B 2 4
 *
 * 		T O
 *
 * 		Y U V 4 2 0 P
 *
 *
 *
 * We have 2 RGB24 to YUV420P conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 */


//
// NNB Core conversion loop for RGB24 to YUV420 planar NNB conversion 1
#define RGB24_TO_YUV420_NNB_CORE_LOOP1(unpack_fn, y_conv_fn, downsample_fn, uv_conv_fn, y_pack_fn, uv_pack_fn) \
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 3;\
	print_xmm8u("l1 R 1-8:", &unpack_out[0]);\
	print_xmm8u("l1 G 1-8:", &unpack_out[1]);\
	print_xmm8u("l1 B 1-8:", &unpack_out[2]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y1-8", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[6]);\
	rgb_line2 += 3;\
	print_xmm8u("l2 R 1-8:", &unpack_out[6]);\
	print_xmm8u("l2 G 1-8:", &unpack_out[7]);\
	print_xmm8u("l2 B 1-8:", &unpack_out[8]);\
	y_conv_fn(&unpack_out[6], &convert_out[4]);\
	print_xmm8u("l2 Y1-8", &convert_out[4]);\
	downsample_fn(unpack_out, &unpack_out[6], downsample_out);\
	print_xmm8u("l12 420 downsampled R 1-4:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 1-4:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 1-4:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, convert_out);\
	print_xmm8u("l12 UV1-4", convert_out);\
	/* 												 	*/\
	/* Next set of 8 pixels (9-16) along the same line 	*/\
	/* 												 	*/\
	print_xmm8u("l1 R 9-16:", &unpack_out[3]);\
	print_xmm8u("l1 G 9-16:", &unpack_out[4]);\
	print_xmm8u("l1 B 9-16:", &unpack_out[5]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(&unpack_out[3], downsample_out);\
	print_xmm8u("l1 Y9-16", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	print_xmm8u("l2 R 9-16:", &unpack_out[9]);\
	print_xmm8u("l2 G 9-16:", &unpack_out[10]);\
	print_xmm8u("l2 B 9-16:", &unpack_out[11]);\
	y_conv_fn(&unpack_out[9], &downsample_out[1]);\
	print_xmm8u("l2 Y9-16", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(&unpack_out[3], &unpack_out[9], downsample_out);\
	print_xmm8u("l12 420 downsampled R 5-8:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 5-8:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 5-8:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, &convert_out[1]);\
	print_xmm8u("l12 UV5-8", &convert_out[1]);\
	/* 												 	*/\
	/* Next set of 8 pixels (17-24) along the same line */\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 3;\
	print_xmm8u("l1 R 17-24:", &unpack_out[0]);\
	print_xmm8u("l1 G 17-24:", &unpack_out[1]);\
	print_xmm8u("l1 B 17-24:", &unpack_out[2]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y17-24", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[6]);\
	rgb_line2 += 3;\
	print_xmm8u("l2 R 17-24:", &unpack_out[6]);\
	print_xmm8u("l2 G 17-24:", &unpack_out[7]);\
	print_xmm8u("l2 B 17-24:", &unpack_out[8]);\
	y_conv_fn(&unpack_out[6], &convert_out[4]);\
	print_xmm8u("l2 Y17-24", &convert_out[4]);\
	downsample_fn(unpack_out, &unpack_out[6], downsample_out);\
	print_xmm8u("l12 420 downsampled R 9-12:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 9-12:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 9-12:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, &convert_out[2]);\
	/* so far, convert_out[0] holds UV1-4, [1] UV5-8, [2] UV9-12 [3]: l1 Y17-24 [4]: l2 Y17-24*/ \
	print_xmm8u("l12 UV9-12", &convert_out[2]);\
	/* 											 		*/\
	/* Next set of 8 pixels (25-32) along the same line */\
	/* 											 		*/\
	print_xmm8u("l1 R 25-32:", &unpack_out[3]);\
	print_xmm8u("l1 G 25-32:", &unpack_out[4]);\
	print_xmm8u("l1 B 25-32:", &unpack_out[5]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(&unpack_out[3], downsample_out);\
	print_xmm8u("l1 Y25-32", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	print_xmm8u("l2 R 25-32:", &unpack_out[9]);\
	print_xmm8u("l2 G 25-32:", &unpack_out[10]);\
	print_xmm8u("l2 B 25-32:", &unpack_out[11]);\
	y_conv_fn(&unpack_out[9], &downsample_out[1]);\
	print_xmm8u("l2 Y25-32", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(&unpack_out[3], &unpack_out[9], downsample_out);\
	print_xmm8u("l12 420 downsampled R 13-16:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 13-16:", &downsample_out[1]);\
	print_xmm8u("l12 420 downsampled B 13-16:", &downsample_out[2]);\
	uv_conv_fn(downsample_out, &convert_out[3]);\
	print_xmm8u("l12 UV13-16", &convert_out[3]);\
	uv_pack_fn(convert_out, uplane_out, vplane_out);\
	uplane_out++;\
	vplane_out++;



// NNB planar conversion 1
#define RGB24_TO_YUV420P_RECIPE(unpack_fn_prefix, y_pack_fn, uv_pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	lines_remaining = pixfc->height;\
	uint32_t	pixels_remaining_on_line = pixfc->width;\
	__m128i*	rgb_line1 = (__m128i *) source_buffer;\
	__m128i*	rgb_line2 = (__m128i *) ((uint8_t*)source_buffer + pixfc->width*3);\
	__m128i*	yplane_line1 = (__m128i *) dest_buffer;\
	__m128i*	yplane_line2 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixfc->pixel_count / 2);\
	__m128i		unpack_out[12];\
	/* [0]: l1	R 1-8		[6]:  l2	R 1-8*/\
	/* [1]: l1	G 1-8		[7]:  l2	G 1-8*/\
	/* [2]: l1	B 1-8		[8]:  l2	B 1-8*/\
	/* [3]: l1	R 9-16		[9]:  l2	R 9-16*/\
	/* [4]: l1	G 9-16		[10]: l2	G 9-16*/\
	/* [5]: l1	B 9-16		[11]: l2	B 9-16*/\
	__m128i		downsample_out[3];\
	/* [0]: l12   	UV 1-4*/\
	/* [1]: l12   	UV 5-8*/\
	/* [2]: l12   	UV 9-12*/\
	/* [3]: l12   	UV 13-16*/\
	/*  OR  l1		Y  1-8*/\
	/* [4]: l2   	Y  1-8*/\
	__m128i		convert_out[5];\
	while(lines_remaining > 0){\
		while(pixels_remaining_on_line > 0) {\
			RGB32_TO_YUV420_NNB_CORE_LOOP1(\
					unpack_fn_prefix##instr_set, y_conv_fn,\
					avg_420_downsample_r_g_b_vectors_##instr_set,\
					uv_conv_fn, y_pack_fn, uv_pack_fn);\
			/* The above macro effectively handles 64 pixels (32 on two adjacent lines)*/ \
			/* but we need to run it only for as many pixels as there is on one line */ \
			pixels_remaining_on_line -= 32;\
		};\
		/* the inner while loop handles two lines at a time */\
		lines_remaining -= 2;\
		rgb_line1 = (__m128i *) ((uint8_t*)rgb_line1 + pixfc->width * 3);\
		rgb_line2 = (__m128i *) ((uint8_t*)rgb_line2 + pixfc->width * 3);\
		yplane_line1 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
		yplane_line2 = (__m128i *) ((uint8_t*)yplane_line2 + pixfc->width);\
		pixels_remaining_on_line = pixfc->width;\
	};\


// NNB Core conversion loop for RGB24 to YUV420 planar NNB conversion 2
#define RGB24_TO_YUV420_NNB_CORE_LOOP2(unpack_fn, y_conv_fn, downsample_fn, uv_conv_fn, y_pack_fn, uv_pack_fn) \
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 1-4:", &unpack_out[0]);\
	print_xmm8u("l1 RB 1-4:", &unpack_out[1]);\
	print_xmm8u("l1 AG 5-8:", &unpack_out[2]);\
	print_xmm8u("l1 RB 5-8:", &unpack_out[3]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y1-8", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 1-4:", &unpack_out[4]);\
	print_xmm8u("l2 RB 1-4:", &unpack_out[5]);\
	print_xmm8u("l2 AG 5-8:", &unpack_out[6]);\
	print_xmm8u("l2 RB 5-8:", &unpack_out[7);\
	y_conv_fn(&unpack_out[4], &convert_out[4]);\
	print_xmm8u("l2 Y1-8", &convert_out[4]);\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled AG 1-4:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled RB 1-4:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, convert_out);\
	print_xmm8u("l12 UV1-4", convert_out);\
	/* 												 	*/\
	/* Next set of 8 pixels (9-16) along the same line 	*/\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 9-12:", &unpack_out[0]);\
	print_xmm8u("l1 RB 9-12:", &unpack_out[1]);\
	print_xmm8u("l1 AG 13-16:", &unpack_out[2]);\
	print_xmm8u("l1 RB 13-16:", &unpack_out[3]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(unpack_out, downsample_out);\
	print_xmm8u("l1 Y9-16", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 9-12:", &unpack_out[4]);\
	print_xmm8u("l2 RB 9-12:", &unpack_out[5]);\
	print_xmm8u("l2 AG 12-16:", &unpack_out[6]);\
	print_xmm8u("l2 RB 12-16:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &downsample_out[1]);\
	print_xmm8u("l2 Y9-16", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled AG 5-8:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled RB 5-8:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, &convert_out[1]);\
	print_xmm8u("l12 UV5-8", &convert_out[1]);\
	/* 												 	*/\
	/* Next set of 8 pixels (17-24) along the same line */\
	/* 												 	*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 17-20:", &unpack_out[0]);\
	print_xmm8u("l1 RB 17-20:", &unpack_out[1]);\
	print_xmm8u("l1 AG 21-24:", &unpack_out[2]);\
	print_xmm8u("l1 RB 21-24:", &unpack_out[3]);\
	y_conv_fn(unpack_out, &convert_out[3]);\
	print_xmm8u("l1 Y17-24", &convert_out[3]);\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 17-20:", &unpack_out[4]);\
	print_xmm8u("l2 RB 17-20:", &unpack_out[5]);\
	print_xmm8u("l2 AG 21-24:", &unpack_out[6]);\
	print_xmm8u("l2 RB 21-24:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &convert_out[4]);\
	print_xmm8u("l2 Y17-24", &convert_out[4]);\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled AG 9-12:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled RB 9-12:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, &convert_out[2]);\
	/* so far, convert_out[0] holds UV1-4, [1] UV5-8, [2] UV9-12*/\
	print_xmm8u("l12 UV9-12", &convert_out[2]);\
	/* 											 		*/\
	/* Next set of 8 pixels (25-32) along the same line */\
	/* 											 		*/\
	unpack_fn(rgb_line1, unpack_out);\
	rgb_line1 += 2;\
	print_xmm8u("l1 AG 25-28:", &unpack_out[0]);\
	print_xmm8u("l1 RB 25-28:", &unpack_out[1]);\
	print_xmm8u("l1 AG 29-32:", &unpack_out[2]);\
	print_xmm8u("l1 RB 29-32:", &unpack_out[3]);\
	/* storing y samples in downsample array to avoid having to create another stack-allocated __m128i array */\
	y_conv_fn(unpack_out, downsample_out);\
	print_xmm8u("l1 Y25-32", downsample_out);\
	y_pack_fn(&convert_out[3], downsample_out, yplane_line1);\
	yplane_line1++;\
	unpack_fn(rgb_line2, &unpack_out[4]);\
	rgb_line2 += 2;\
	print_xmm8u("l2 AG 25-28:", &unpack_out[4]);\
	print_xmm8u("l2 RB 25-28:", &unpack_out[5]);\
	print_xmm8u("l2 AG 28-32:", &unpack_out[6]);\
	print_xmm8u("l2 RB 28-32:", &unpack_out[7]);\
	y_conv_fn(&unpack_out[4], &downsample_out[1]);\
	print_xmm8u("l2 Y24-32", &downsample_out[1]);\
	y_pack_fn(&convert_out[4], &downsample_out[1], yplane_line2);\
	yplane_line2++;\
	downsample_fn(unpack_out, downsample_out);\
	print_xmm8u("l12 420 downsampled R 13-16:", &downsample_out[0]);\
	print_xmm8u("l12 420 downsampled G 13-16:", &downsample_out[1]);\
	uv_conv_fn(downsample_out, &convert_out[3]);\
	print_xmm8u("l12 UV13-16", &convert_out[3]);\
	uv_pack_fn(convert_out, uplane_out, vplane_out);\
	uplane_out++;\
	vplane_out++;

// NNB planar conversion 2
#define RGB24_TO_YUV420P_RECIPE2(unpack_fn_prefix, y_pack_fn, uv_pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	lines_remaining = pixfc->height;\
	uint32_t	pixels_remaining_on_line = pixfc->width;\
	__m128i*	rgb_line1 = (__m128i *) source_buffer;\
	__m128i*	rgb_line2 = (__m128i *) ((uint8_t*)source_buffer + pixfc->width*4);\
	__m128i*	yplane_line1 = (__m128i *) dest_buffer;\
	__m128i*	yplane_line2 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*)uplane_out + pixfc->pixel_count / 4);\
	__m128i		unpack_out[8];\
	__m128i		downsample_out[2];\
	/* [0]: l12   	UV 1-4*/\
	/* [1]: l12   	UV 5-8*/\
	/* [2]: l12   	UV 9-12*/\
	/* [3]: l12   	UV 13-16*/\
	/*  OR  l1		Y  1-8*/\
	/* [4]: l2   	Y  1-8*/\
	__m128i		convert_out[5];\
	while(lines_remaining > 0){\
		while(pixels_remaining_on_line > 0) {\
			RGB32_TO_YUV420_NNB_CORE_LOOP2(\
					unpack_fn_prefix##instr_set, y_conv_fn,\
					avg_420_downsample_ag_rb_vectors_##instr_set,\
					uv_conv_fn, y_pack_fn, uv_pack_fn);\
			/* The above macro effectively handles 64 pixels (32 on two adjacent lines)*/ \
			/* but we need to run it only for as many pixels as there is on one line */ \
			pixels_remaining_on_line -= 32;\
		};\
		/* the inner while loop handles two lines at a time */\
		lines_remaining -= 2;\
		rgb_line1 = (__m128i *) ((uint8_t*)rgb_line1 + pixfc->width * 4);\
		rgb_line2 = (__m128i *) ((uint8_t*)rgb_line2 + pixfc->width * 4);\
		yplane_line1 = (__m128i *) ((uint8_t*)yplane_line1 + pixfc->width);\
		yplane_line2 = (__m128i *) ((uint8_t*)yplane_line2 + pixfc->width);\
		pixels_remaining_on_line = pixfc->width;\
	};\


#endif /* RGB_CONVERSION_RECIPES_H_ */
