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
#include "rgb_unpack.h"
#include "rgb_to_yuv_convert.h"
#include "yuv_pack.h"


/*
 * We have 2 RGB to YUV422 conversion implementations:
 * - The first one unpacks 8 pixels into 3 16bit vectors R,G & B.
 * - The second one unpacks 8 pixels into 4 16bit AG & RB vectors.
 *
 */
#define CONVERT_RGB_TO_YUV422(unpack_fn_prefix, y_conv_fn, uv_conv_fn, pack_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		nnb_422_downsample_r_g_b_vectors_##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
		y_conv_fn(unpack_out, &convert_out[2]);\
		nnb_422_downsample_r_g_b_vectors_##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 4;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\

#define CONVERT2_RGB_TO_YUV422(unpack_fn_prefix, y_conv_fn, uv_conv_fn, pack_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[4];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		nnb_422_downsample_ag_rb_vectors_##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
		y_conv_fn(unpack_out, &convert_out[2]);\
		nnb_422_downsample_ag_rb_vectors_##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 4;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\

#define AVG_DOWNSAMPLE_N_CONVERT_RGB_TO_YUV422(unpack_fn_prefix, y_conv_fn, uv_conv_fn, pack_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		previous[3];\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	unpack_fn_prefix##instr_set(rgb_in, previous);\
	y_conv_fn(previous, convert_out);\
	avg_422_downsample_first_r_g_b_vectors_##instr_set(previous, unpack_out);\
	uv_conv_fn(unpack_out, &convert_out[1]);\
	unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
	y_conv_fn(unpack_out, &convert_out[2]);\
	avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out);\
	uv_conv_fn(unpack_out, &convert_out[3]);\
	pack_fn(convert_out, yuv_out);\
	rgb_in += 4;\
	yuv_out += 2;\
	pixel_count -= 16;\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
		y_conv_fn(unpack_out, &convert_out[2]);\
		avg_422_downsample_r_g_b_vectors_n_save_previous_##instr_set(unpack_out, previous, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 4;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\


#define AVG_DOWNSAMPLE_N_CONVERT2_RGB_TO_YUV422(unpack_fn_prefix, y_conv_fn, uv_conv_fn, pack_fn, instr_set) \
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
	previous[0] = _mm_load_si128(&unpack_out[3]);\
	previous[1] = _mm_load_si128(&unpack_out[4]);\
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
		y_conv_fn(unpack_out, convert_out);\
		avg_422_downsample_ag_rb_vectors_n_save_previous_##instr_set(unpack_out, previous, downsample_out);\
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

#endif /* ARGB_CONVERSION_COMMON_H_ */
