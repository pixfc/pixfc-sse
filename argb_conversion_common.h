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

#include "rgb_unpack.h"
#include "rgb_to_yuv_convert.h"
#include "yuv_pack.h"

#define CONVERT_RGB_TO_YUV422(unpack_fn_prefix, downsample_fn_prefix, y_conv_fn, uv_conv_fn, pack_fn, instr_set) \
	__m128i*	rgb_in = (__m128i *) source_buffer;\
	__m128i*	yuv_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[3];\
	__m128i		convert_out[4];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(rgb_in, unpack_out);\
		y_conv_fn(unpack_out, convert_out);\
		downsample_fn_prefix##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[1]);\
		unpack_fn_prefix##instr_set(&rgb_in[2], unpack_out);\
		y_conv_fn(unpack_out, &convert_out[2]);\
		downsample_fn_prefix##instr_set(unpack_out, unpack_out);\
		uv_conv_fn(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, yuv_out);\
		rgb_in += 4;\
		yuv_out += 2;\
		pixel_count -= 16;\
	};\

#endif /* ARGB_CONVERSION_COMMON_H_ */
