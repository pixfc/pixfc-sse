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
 * We have 2 RGB32 to YUV422 conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 */

// Core conversion loop, common to RGB32 to YUV422 planar & interleaved NNB conversions
#define RGB32_TO_YUV422_NNB_LOOP_CORE(rgb_in, unpack_fn, downsample_fn, unpack_out, y_conv_fn, uv_conv_fn, convert_out)\
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
				rgb_in,	unpack_fn_prefix##instr_set,\
				nnb_422_downsample_r_g_b_vectors_##instr_set,\
				unpack_out,	y_conv_fn, uv_conv_fn, convert_out\
		);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// NNB planar conversion 1
#define CONVERT_RGB32_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*) yplane + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*) uplane + pixel_count / 2);\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				rgb_in, unpack_fn_prefix##instr_set,\
				nnb_422_downsample_r_g_b_vectors_##instr_set,\
				unpack_out,	y_conv_fn, uv_conv_fn, convert_out\
		);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				rgb_in, unpack_fn_prefix##instr_set,\
				nnb_422_downsample_r_g_b_vectors_##instr_set,\
				unpack_out,	y_conv_fn, uv_conv_fn, convert_out\
		);\
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
				rgb_in,	unpack_fn_prefix##instr_set,\
				nnb_422_downsample_ag_rb_vectors_##instr_set,\
				unpack_out,	y_conv_fn, uv_conv_fn, convert_out\
		);\
		pack_fn(convert_out, yuv_out);\
		yuv_out += 2;\
	};\

// NNB planar conversion 2
#define CONVERT2_RGB32_TO_YUV422P(unpack_fn_prefix, pack_lo_fn, pack_hi_fn, y_conv_fn, uv_conv_fn, instr_set) \
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yplane_out = (__m128i *) dest_buffer;\
	__m128i*	uplane_out = (__m128i *) ((uint8_t*) yplane + pixel_count);\
	__m128i*	vplane_out = (__m128i *) ((uint8_t*) uplane + pixel_count / 2);\
	__m128i		unpack_out[4];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				rgb_in,	unpack_fn_prefix##instr_set,\
				nnb_422_downsample_ag_rb_vectors_##instr_set,\
				unpack_out,	y_conv_fn, uv_conv_fn, convert_out\
		);\
		pack_lo_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		RGB32_TO_YUV422_NNB_LOOP_CORE(\
				rgb_in,	unpack_fn_prefix##instr_set,\
				nnb_422_downsample_ag_rb_vectors_##instr_set,\
				unpack_out,	y_conv_fn, uv_conv_fn, convert_out\
		);\
		pack_hi_fn(convert_out, yplane_out, uplane_out, vplane_out);\
		yplane_out++;\
		uplane_out++;\
		vplane_out++;\
	};\


// Core conversion loop, common to RGB32 to YUV422 planar & interleaved AVG conversion 1
#define RGB32_TO_YUV422I_AVG_CORE_LOOP1(unpack_fn, y_conv_fn, downsample_CALL1, downsample_CALL2, uv_conv_fn) \
	unpack_fn(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, convert_out);\
	downsample_CALL1;\
	uv_conv_fn(unpack_out, &convert_out[1]);\
	rgb_in += 2;\
	unpack_fn(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, &convert_out[2]);\
	downsample_CALL2;\
	uv_conv_fn(unpack_out, &convert_out[3]);\
	rgb_in += 2;\
	pixel_count -= 16;\

// Average interleave conversion 1
#define AVG_DOWNSAMPLE_N_CONVERT_RGB32_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[3];\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	RGB32_TO_YUV422I_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_first_r_g_b_vectors_##instr_set(unpack_out, unpack_out),\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out),\
			uv_conv_fn);\
	pack_fn(convert_out, yuv_out);\
	yuv_out += 2;\
	while(pixel_count > 0) {\
		RGB32_TO_YUV422I_AVG_CORE_LOOP1(\
			unpack_fn_prefix##instr_set, y_conv_fn,\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out),\
			avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out),\
			uv_conv_fn);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 4;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\



#define AVG_DOWNSAMPLE_N_CONVERT2_RGB32_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[2];\
	__m128i		unpack_out[4];\
	__m128i		downsample_out[2];\
	__m128i		convert_out[4];\
	unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, convert_out);\
	avg_422_downsample_first_ag_rb_vectors_##instr_set(unpack_out, downsample_out);\
	uv_conv_fn(downsample_out, &convert_out[1]);\
	previous[0] = _mm_load_si128(&unpack_out[2]);\
	previous[1] = _mm_load_si128(&unpack_out[3]);\
	unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
	y_conv_fn(unpack_out, &convert_out[2]);\
	avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out);\
	uv_conv_fn(unpack_out, &convert_out[3]);\
	pack_fn(convert_out, yuv_out);\
	rgb_in += 4;\
	yuv_out += 2;\
	pixel_count -= 16;\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(unpack_out, previous, downsample_out);\
		y_conv_fn(unpack_out, convert_out);\
		uv_conv_fn(downsample_out, &convert_out[1]);\
		unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
		y_conv_fn(unpack_out, &convert_out[2]);\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 4;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\






/*
 * We have 2 RGB24 to YUV422 conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 *
 */
#define CONVERT_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[6];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		nnb_422_downsample_r_g_b_vectors_##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		y_conv_fn(&unpack_out[3], &convert_out[2]);\
		nnb_422_downsample_r_g_b_vectors_##instr_set(&unpack_out[3], &unpack_out[3]);\
		uv_conv_fn(&unpack_out[3], &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 3;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\

#define CONVERT2_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[8];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		nnb_422_downsample_ag_rb_vectors_##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		y_conv_fn(&unpack_out[4], &convert_out[2]);\
		nnb_422_downsample_ag_rb_vectors_##instr_set(&unpack_out[4], &unpack_out[4]);\
		uv_conv_fn(&unpack_out[4], &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 3;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\

#define AVG_DOWNSAMPLE_N_CONVERT_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[3];\
	__m128i		unpack_out[6];\
	__m128i		convert_out[4];\
	unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, convert_out);\
	avg_422_downsample_first_r_g_b_vectors_##instr_set(unpack_out, unpack_out);\
	uv_conv_fn(unpack_out, &convert_out[1]);\
	y_conv_fn(&unpack_out[3], &convert_out[2]);\
	previous[0] = _mm_load_si128(&unpack_out[0]);\
	previous[1] = _mm_load_si128(&unpack_out[1]);\
	previous[2] = _mm_load_si128(&unpack_out[2]);\
	avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(&unpack_out[3], previous, &unpack_out[3]);\
	uv_conv_fn(&unpack_out[3], &convert_out[3]);\
	pack_fn(convert_out, yuv_out);\
	rgb_in += 3;\
	yuv_out += 2;\
	pixel_count -= 16;\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		y_conv_fn(&unpack_out[3], &convert_out[2]);\
		avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(&unpack_out[3], previous, &unpack_out[3]);\
		uv_conv_fn(&unpack_out[3], &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 3;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\


#define AVG_DOWNSAMPLE_N_CONVERT2_RGB24_TO_YUV422I(unpack_fn_prefix, pack_fn, y_conv_fn, uv_conv_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[2];\
	__m128i		unpack_out[8];\
	__m128i		downsample_out[2];\
	__m128i		convert_out[4];\
	unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
	y_conv_fn(unpack_out, convert_out);\
	avg_422_downsample_first_ag_rb_vectors_##instr_set(unpack_out, downsample_out);\
	uv_conv_fn(downsample_out, &convert_out[1]);\
	previous[0] = _mm_load_si128(&unpack_out[2]);\
	previous[1] = _mm_load_si128(&unpack_out[3]);\
	y_conv_fn(&unpack_out[4], &convert_out[2]);\
	avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(&unpack_out[4], previous, downsample_out);\
	uv_conv_fn(downsample_out, &convert_out[3]);\
	pack_fn(convert_out, yuv_out);\
	rgb_in += 3;\
	yuv_out += 2;\
	pixel_count -= 16;\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(unpack_out, previous, downsample_out);\
		uv_conv_fn(downsample_out, &convert_out[1]);\
		y_conv_fn(&unpack_out[4], &convert_out[2]);\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(&unpack_out[4], previous, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 3;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\

#endif /* ARGB_CONVERSION_COMMON_H_ */
