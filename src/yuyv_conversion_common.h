/*
 * yuyv_conversion_routines_common.h
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

#ifndef YUYV_CONVERSION_COMMON_H_
#define YUYV_CONVERSION_COMMON_H_


/*
 * Include the unpack & pack routines twice to generate both aligned & unaligned versions
 */
#undef  GENERATE_UNALIGNED_INLINES
#define GENERATE_UNALIGNED_INLINES 0
#include "rgb_pack.h"
#include "yuv_unpack.h"

#undef  GENERATE_UNALIGNED_INLINES
#define GENERATE_UNALIGNED_INLINES 1
#include "rgb_pack.h"
#include "yuv_unpack.h"

#include "yuv_upsample.h"
#include "yuv_to_rgb_convert.h"
#include "yuv_to_rgb_convert_bt601.h"
#include "yuv_to_rgb_convert_bt709.h"

/*
 * Convert YUYV to RGB with upsampling
 *
 *
 * Example of the expansion for a conversion to ARGB :
 *
 * 	__m128i		unpack_out[8];	// contents: 0: Y - 1: UV odd - 2: UV even - 3: Y - 4: UV odd - 5: UV even - 6: Y - 7 UV odd
 *	__m128i		convert_out[6];
 *	__m128i*    yuyv_8pixels = (__m128i *) source_buffer;
 *	__m128i*	argb_4pixels = (__m128i *) dest_buffer;
 *	uint32_t	pixel_count = pixfc->pixel_count - 16; // handle the last 16 pixels outside the while loop
 *
 *
 *	// unpack first 8 pixels to first 2 elems in unpack_out array
 *	unpack_yuyv_to_y_uv_vectors_sse2_ssse3(yuyv_8pixels, unpack_out);
 *
 *	while(pixel_count > 0) {
 *		// unpack 2nd group of 8 pixels
 *		unpack_yuyv_to_y_uv_vectors_sse2_ssse3(&yuyv_8pixels[1], &unpack_out[3]);
 *
 *		// unpack following group of 8 pixels (converted in the next while iteration, not this one.
 *		// used here only to recreate missing chroma values for the current 16 pix)
 *		unpack_yuyv_to_y_uv_vectors_sse2_ssse3(&yuyv_8pixels[2], &unpack_out[6]);
 *
 *		// reconstruct missing U-V values for first 8 pix
 *		reconstruct_missing_uv_sse2_ssse3(&unpack_out[1], &unpack_out[4], &unpack_out[2]);
 *
 *		// convert first 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(unpack_out, convert_out);
 *
 *		// reconstruct missing U-V values for next 8 pix
 *		reconstruct_missing_uv_sse2_ssse3(&unpack_out[4], &unpack_out[7], &unpack_out[5]);
 *
 *		// convert next 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(&unpack_out[3], &convert_out[3]);
 *
 *		// pack both sets of 8 pixels
 *		pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 *		// each iteration converts 16 pixels
 *		yuyv_8pixels += 2;
 *		argb_4pixels += 4;
 *		pixel_count -= 16;
 *
 *		// Copy the next iteration first 8 pix luma and chromas unpacked above
 *		unpack_out[0] = _mm_load_si128(&unpack_out[6]);
 *		unpack_out[1] = _mm_load_si128(&unpack_out[7]);
 *	}
 *
 *	// convert the last 16 pixels
 *	// unpack next 8 pixels
 *	unpack_yuyv_to_y_uv_vectors_sse2_ssse3(&yuyv_8pixels[1], &unpack_out[3]);
 *
 *	// reconstruct missing U-V values for first 8 pix
 *	reconstruct_missing_uv_sse2_ssse3(&unpack_out[1], &unpack_out[4], &unpack_out[2]);
 *
 *	// reconstruct missing U-V values for last 8 pix
 *	reconstruct_last_missing_uv_sse2_ssse3(&unpack_out[4], &unpack_out[5]);
 *
 *	// convert first 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(unpack_out, convert_out);
 *
 *	// convert next 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(&unpack_out[3], &convert_out[3]);
 *
 *	// pack both sets of 8 pixels
 *	pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 */
#define UPSAMPLE_AND_CONVERT_YUV_TO_RGB(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	__m128i		unpack_out[8];\
	__m128i		convert_out[6];\
	__m128i*    yuyv_8pixels = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count - 16;\
	unpack_fn_prefix##instr_set(yuyv_8pixels, unpack_out);\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(&yuyv_8pixels[1], &unpack_out[3]);\
		unpack_fn_prefix##instr_set(&yuyv_8pixels[2], &unpack_out[6]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
		conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
		pack_fn(convert_out, rgb_out_buf);\
		yuyv_8pixels += 2;\
		rgb_out_buf += output_stride;\
		pixel_count -= 16;\
		unpack_out[0] = _mm_load_si128(&unpack_out[6]);\
		unpack_out[1] = _mm_load_si128(&unpack_out[7]);\
	}\
	unpack_fn_prefix##instr_set(&yuyv_8pixels[1], &unpack_out[3]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	reconstruct_last_missing_uv_##instr_set(&unpack_out[4], &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	pack_fn(convert_out, rgb_out_buf);\


/*
 *
 * Convert YUYV to RGB using Nearest Neighbour upsampling
 * ie. re-create the missing chromas by duplicating the previous existing ones.
 *
 *
 * Expansion for a conversion to ARGB:
 *
 * 	__m128i*        yuyv_8pixels = (__m128i *) source_buffer;
 *	__m128i*	argb_4pixels = (__m128i *) dest_buffer;
 *	uint32_t	pixel_count = pixfc->pixel_count;
 *	__m128i		unpack_out[2];
 *	__m128i		convert_out[6];
 *
 *
 *	while(pixel_count > 0) {
 *
 *		// unpack first 8 pixels
 *		unpack_yuyv_to_y_uv_vectors_sse2_ssse3(yuyv_8pixels, unpack_out);
 *
 *		// convert first 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(unpack_out, convert_out);
 *
 *		// unpack next 8 pixels
 *		unpack_yuyv_to_y_uv_vectors_sse2_ssse3(&yuyv_8pixels[1], unpack_out);
 *
 *		// convert next 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(unpack_out, &convert_out[3]);
 *
 *		// pack both sets of 8 pixels
 *		pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 *		// each iteration converts 16 pixels
 *		yuyv_8pixels += 2;
 *		argb_4pixels += 4;
 *		pixel_count -= 16;
 *	}
 *
 */
#define CONVERT_YUV_TO_RGB(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	__m128i*	yuyv_8pixels = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[2];\
	__m128i		convert_out[6];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(yuyv_8pixels, unpack_out);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		unpack_fn_prefix##instr_set(&yuyv_8pixels[1], unpack_out);\
		conv_fn_prefix##instr_set(unpack_out, &convert_out[3]);\
		pack_fn(convert_out, rgb_out_buf);\
		yuyv_8pixels += 2;\
		rgb_out_buf += output_stride;\
		pixel_count -= 16;\
	};\


#endif /* YUYV_CONVERSION_COMMON_H_ */
