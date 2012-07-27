/*
 * yuv_conversion_recipes.h
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

#ifndef YUV_CONVERSION_RECIPES_H_
#define YUV_CONVERSION_RECIPES_H_

#ifndef DEBUG
/*
 * Include the unpack & pack routines twice to generate both aligned & unaligned versions
 */
#undef  GENERATE_UNALIGNED_INLINES
#define GENERATE_UNALIGNED_INLINES 0
#include "rgb_pack.h"
#include "yuv_unpack.h"
#include "yuv_pack.h"	// Used in v210 to yuv422i

#undef  GENERATE_UNALIGNED_INLINES
#define GENERATE_UNALIGNED_INLINES 1
#include "rgb_pack.h"
#include "yuv_unpack.h"
#include "yuv_pack.h"	// Used in v210 to yuv422i

#include "yuv_repack.h"
#include "yuv_upsample.h"
#include "yuv_to_rgb_convert.h"
#endif

/*
 * Convert YUV422 interleaved to RGB with upsampling
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
#define UPSAMPLE_YUV422I_TO_RGB_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
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
 * Convert v210 to RGB using AVG upsampling
 * (based off the UPSAMPLE_YUV422I_TO_RGB_RECIPE, adjusted to v210 unpacking creating 6
 * output vectors instead of 2 as YUV422I unpacking does)
 */
#define UPSAMPLE_V210_TO_RGB_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	__m128i		unpack_out[12];\
	/*	The above array holds:			*/\
	/*	0:	Y  1 - 8					*/\
	/*	1:	UVodd 1 - 8					*/\
	/*	2:	UVeven 1 - 8				*/\
	/*	3:	Y  9 - 16					*/\
	/*	4:	UVodd 9 - 16				*/\
	/*	5:	UVeven 9 - 16				*/\
	/*	6:	Y  17 - 24					*/\
	/*	7:	UVodd 17 - 24				*/\
	/*	8:	UVeven 17 - 24				*/\
	/*	9:	Y  25 - 32					*/\
	/*	10:	UVodd 25 - 32				*/\
	/*	11:	UVeven 25 - 32				*/\
	__m128i		convert_out[6];\
	__m128i*    v210_in = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[3], &unpack_out[6]);\
	v210_in += 4;\
	print_xmm16u("Y1-8", &unpack_out[0]);\
	print_xmm16u("UVodd1-8", &unpack_out[1]);\
	print_xmm16u("Y9-16", &unpack_out[3]);\
	print_xmm16u("UVodd9-16", &unpack_out[4]);\
	print_xmm16u("Y17-24", &unpack_out[6]);\
	print_xmm16u("UVodd17-24", &unpack_out[7]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("UVeven1-8", &unpack_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("UVeven9-16", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("R1-8", &convert_out[0]);\
	print_xmm16("G1-8", &convert_out[1]);\
	print_xmm16("B1-8", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("R9-16", &convert_out[3]);\
	print_xmm16("G9-16", &convert_out[4]);\
	print_xmm16("B9-16", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	unpack_out[0] = _mm_load_si128(&unpack_out[6]);\
	unpack_out[1] = _mm_load_si128(&unpack_out[7]);\
	pixel_count -= 16;\
	while(pixel_count > 32) /* handle the last 32 pixels outside the loop */ {\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[3], &unpack_out[6], &unpack_out[9]);\
		v210_in += 4;\
		print_xmm16u("Loop: Y-8 - -1", &unpack_out[0]);\
		print_xmm16u("Loop: UVodd-8 - -1", &unpack_out[1]);\
		print_xmm16u("Loop: Y1-8", &unpack_out[3]);\
		print_xmm16u("Loop: UVodd1-8", &unpack_out[4]);\
		print_xmm16u("Loop: Y9-16", &unpack_out[6]);\
		print_xmm16u("Loop: UVodd9-16", &unpack_out[7]);\
		print_xmm16u("Loop: Y17-24", &unpack_out[9]);\
		print_xmm16u("Loop: UVodd17-24", &unpack_out[10]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
		print_xmm16u("Loop: UVeven-8 - -1", &unpack_out[2]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
		print_xmm16u("Loop: UVeven1-8", &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		print_xmm16("Loop: R-8 - -1", &convert_out[0]);\
		print_xmm16("Loop: G-8 - -1", &convert_out[1]);\
		print_xmm16("Loop: B-8 - -1", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
		print_xmm16("Loop: R1-8", &convert_out[3]);\
		print_xmm16("Loop: G1-8", &convert_out[4]);\
		print_xmm16("Loop: B1-8", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		\
		reconstruct_missing_uv_##instr_set(&unpack_out[7], &unpack_out[10], &unpack_out[8]);\
		print_xmm16u("Loop: UVeven9-16", &unpack_out[8]);\
		conv_fn_prefix##instr_set(&unpack_out[6], convert_out);\
		print_xmm16("Loop: R9-16", &convert_out[0]);\
		print_xmm16("Loop: G9-16", &convert_out[1]);\
		print_xmm16("Loop: B9-16", &convert_out[2]);\
		unpack_out[0] = _mm_load_si128(&unpack_out[9]);\
		unpack_out[1] = _mm_load_si128(&unpack_out[10]);\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[3], &unpack_out[6], &unpack_out[9]);\
		v210_in += 4;\
		print_xmm16u("Loop: Y17-24", &unpack_out[0]);\
		print_xmm16u("Loop: UVodd17-24", &unpack_out[1]);\
		print_xmm16u("Loop: Y25-32", &unpack_out[3]);\
		print_xmm16u("Loop: UVodd25-32", &unpack_out[4]);\
		print_xmm16u("Loop: Y33-40", &unpack_out[6]);\
		print_xmm16u("Loop: UVodd33-40", &unpack_out[7]);\
		print_xmm16u("Loop: Y41-48", &unpack_out[9]);\
		print_xmm16u("Loop: UVodd41-48", &unpack_out[10]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
		print_xmm16u("Loop: UVeven17-24", &unpack_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[0], &convert_out[3]);\
		print_xmm16("Loop: R17-24", &convert_out[3]);\
		print_xmm16("Loop: G17-24", &convert_out[4]);\
		print_xmm16("Loop: B17-24", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		\
		reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
		print_xmm16u("Loop: UVeven25-32", &unpack_out[5]);\
		conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[0]);\
		print_xmm16("Loop: R25-32", &convert_out[0]);\
		print_xmm16("Loop: G25-32", &convert_out[1]);\
		print_xmm16("Loop: B25-32", &convert_out[2]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[7], &unpack_out[10], &unpack_out[8]);\
		print_xmm16u("Loop: UVeven33-40", &unpack_out[5]);\
		conv_fn_prefix##instr_set(&unpack_out[6], &convert_out[3]);\
		print_xmm16("Loop: R33-40", &convert_out[3]);\
		print_xmm16("Loop: G33-40", &convert_out[4]);\
		print_xmm16("Loop: B33-40", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		unpack_out[0] = _mm_load_si128(&unpack_out[9]);\
		unpack_out[1] = _mm_load_si128(&unpack_out[10]);\
		pixel_count -= 48;\
	}\
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[3], &unpack_out[6], &unpack_out[9]);\
	print_xmm16u("PostLoop Y1-8", &unpack_out[0]);\
	print_xmm16u("PostLoop UVodd1-8", &unpack_out[1]);\
	print_xmm16u("PostLoop Y9-16", &unpack_out[3]);\
	print_xmm16u("PostLoop UVodd9-16", &unpack_out[4]);\
	print_xmm16u("PostLoop Y17-24", &unpack_out[6]);\
	print_xmm16u("PostLoop UVodd17-24", &unpack_out[7]);\
	print_xmm16u("PostLoop Y25-32", &unpack_out[9]);\
	print_xmm16u("PostLoop UVodd25-32", &unpack_out[10]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("PostLoop: UVeven1-8", &unpack_out[2]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("PostLoop: R1-8", &convert_out[0]);\
	print_xmm16("PostLoop: G1-8", &convert_out[1]);\
	print_xmm16("PostLoop: B1-8", &convert_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("PostLoop: UVeven9-16", &unpack_out[5]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("PostLoop: R9-16", &convert_out[3]);\
	print_xmm16("PostLoop: G9-16", &convert_out[4]);\
	print_xmm16("PostLoop: B9-16", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	reconstruct_missing_uv_##instr_set(&unpack_out[7], &unpack_out[10], &unpack_out[8]);\
	print_xmm16u("PostLoop: UVeven17-24", &unpack_out[8]);\
	conv_fn_prefix##instr_set(&unpack_out[6], convert_out);\
	print_xmm16("PostLoop: R17-24", &convert_out[0]);\
	print_xmm16("PostLoop: G17-24", &convert_out[1]);\
	print_xmm16("PostLoop: B17-24", &convert_out[2]);\
	reconstruct_last_missing_uv_##instr_set(&unpack_out[10], &unpack_out[11]);\
	print_xmm16u("PostLoop: UVeven25-32", &unpack_out[8]);\
	conv_fn_prefix##instr_set(&unpack_out[9], &convert_out[3]);\
	print_xmm16("PostLoop: R25-32", &convert_out[0]);\
	print_xmm16("PostLoop: G25-32", &convert_out[1]);\
	print_xmm16("PostLoop: B25-32", &convert_out[2]);\
	pack_fn(convert_out, rgb_out_buf);\


/*
 * Convert YUV422 planar to RGB with upsampling
 *
 * We have the following constraints:
 * - SSE instructions handle 16 bytes (ie. 16 values for a given component
 *   Y, U or V) at once,
 * - We have downsampled chromas at a ratio of 2:1 ie. 2 Y values for 1 U
 *   and 1 V value,
 * These constraints have the following implications:
 * - Because each access to the U & V planes will consume 16 values (belonging to
 *   32 pixels), the main loop must handle 32 pixels at a time. This means doing
 *   2 reads (of 16 pixels each) in the Y plane and 1 read in the U and V planes.
 * - Lastly, the most important implication is that the number of pixels MUST
 *   be a multiple of 32. Otherwise, the start of the V plane will not be
 *   16-byte aligned.
 *
 * I was originally going to require a pixel count multiple of 16, but it would have
 * introduced the following:
 * - the buffer size would have to be multiple of 64 bytes. Otherwise, the last
 *   read in the V plane would be 8 bytes past the end of the input buffer
 * - a check would have to be made in the DO_CONVERSION_3U_1P macro to verify
 *   the alignment of each plane, and made the macro situation even uglier and
 *   harder to read than what it is now.
 * - PixFC would have to impose and enforce a check on the buffer size, in addition
 *   to the check on pixel count.
 *
 * Enforcing a pixel count multiple of 32 was easier, and I decided to go that way
 * instead.
 *
 * Example of the expansion for a conversion to ARGB :
 *
 *   // content of the following array:
 *   // 0: Y1-8
 *   // 1: UV1-8 odd
 *   // 2: UV1-8 upsampled
 *   // 3: Y9-16
 *   // 4: UV9-16 odd
 *   // 5: UV9-16 upsampled
 *   // 6: UV17-24 odd
 *   // 7: UV25-32 odd
 * 	__m128i		unpack_out[8];
 *	__m128i		convert_out[6];
 *	__m128i*    y_plane = (__m128i *) source_buffer;
 *	__m128i*    u_plane = (uint8_t *) source_buffer + pixfc->pixel_count;
 *	__m128i*    v_plane = (uint8_t *) u_plane  + pixfc->pixel_count / 2;
 *	__m128i*	argb_4pixels = (__m128i *) dest_buffer;
 *	int32_t	pixel_count = pixfc->pixel_count;
 *
 *
 *	// Each iteration converts 32 pixel at a time
 *
 *	// unpack the 8 U / V samples for the first 16 pixels
 *	unpack_low_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[4]);
 *
 *	while(pixel_count > 32) {  // handle the last 32 pixels outside the while loop
 *		//
 *		// First set of 16 pix
 *
 *		// unpack 16 Y samples
 *		unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[3]);
 *
 *		// unpack the 8 U / V samples corresponding to the next 16 pixels
 *		// (converted in the next set of 16 pixels, not this one. used here only
 *		// to recreate missing chroma values for the current 16 pix)
 *		unpack_high_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[6], &unpack_out[7]);
 *
 *		// reconstruct missing U-V values for first 8 pix
 *		reconstruct_missing_uv_sse2_ssse3(&unpack_out[1], &unpack_out[4], &unpack_out[2]);
 *
 *		// convert first 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(unpack_out, convert_out);
 *
 *		// reconstruct missing U-V values for next 8 pix
 *		reconstruct_missing_uv_sse2_ssse3(&unpack_out[4], &unpack_out[6], &unpack_out[5]);
 *
 *		// convert next 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(&unpack_out[3], &convert_out[3]);
 *
 *		// pack both sets of 8 pixels
 *		pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 *		// Move on to next 16 Y samples
 *		y_plane++;
 *		// U and V stay where they are as we have converted only the 8 out of 16 samples
 *		//
 *		// Move to next 4 output pixels
 *		argb_4pixels += 4;
 *
 *		//
 *		// Second set of 16 pix
 *
 *		// unpack 16 Y samples
 *		unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[3]);
 *
 *		// Copy the previously-unpacked chromas for this set of 16 pix
 *		unpack_out[1] = _mm_load_si128(&unpack_out[6]);
 *		unpack_out[4] = _mm_load_si128(&unpack_out[7]);
 *
 *		// unpack the 8 U / V samples corresponding to the following 16 pixels
 *		// (converted in the next while iteration, not this one. used here only
 *		// to recreate missing chroma values for the current 16 pix)
 *		unpack_low_yuv42Xp_to_uv_vector_sse2(&u_plane[1], &v_plane[1], &unpack_out[6], &unpack_out[7]);
 *
 *		// reconstruct missing U-V values for first 8 pix
 *		reconstruct_missing_uv_sse2_ssse3(&unpack_out[1], &unpack_out[4], &unpack_out[2]);
 *
 *		// convert first 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(unpack_out, convert_out);
 *
 *		// reconstruct missing U-V values for next 8 pix
 *		reconstruct_missing_uv_sse2_ssse3(&unpack_out[4], &unpack_out[6], &unpack_out[5]);
 *
 *		// convert next 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(&unpack_out[3], &convert_out[3]);
 *
 *		// pack both sets of 8 pixels
 *		pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 *
 *		// Move on to next 16 Y, U, V samples
 *		y_plane += 1;
 *		u_plane += 1;
 *		v_plane += 1;
 *		argb_4pixels += 4;
 *		pixel_count -= 32;
 *
 *		// Copy the next iteration first 8 pix luma and chromas unpacked above
 *		unpack_out[1] = _mm_load_si128(&unpack_out[6]);
 *		unpack_out[4] = _mm_load_si128(&unpack_out[7]);
 *	}
 *
 *	//
 *	// convert the remaining pixels
 *
 *	// First set of 16 pix
 *
 *	// unpack 16 Y samples
 *	unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[3]);
 *
 *	// unpack the 8 U / V samples corresponding to the next 16 pixels
 *	// (converted in the next set of 16 pixels, not this one. used here only
 *	// to recreate missing chroma values for the current 16 pix)
 *	unpack_high_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[6], &unpack_out[7]);
 *
 *	// reconstruct missing U-V values for first 8 pix
 *	reconstruct_missing_uv_sse2_ssse3(&unpack_out[1], &unpack_out[4], &unpack_out[2]);
 *
 *	// convert first 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(unpack_out, convert_out);
 *
 *	// reconstruct missing U-V values for next 8 pix
 *	reconstruct_missing_uv_sse2_ssse3(&unpack_out[4], &unpack_out[6], &unpack_out[5]);
 *
 *	// convert next 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(&unpack_out[3], &convert_out[3]);
 *
 *	// pack both sets of 8 pixels
 *	pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 *	// Move on to next 16 Y samples
 *	y_plane++;
 *	// U and V stay where they are as we have converted only the 8 out of 16 samples
 *	//
 *	// Move to next 4 output pixels
 *	argb_4pixels += 4;
 *
 *	//
 *	// Second set of 16 pix
 *
 *	// unpack 16 Y samples
 *	unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[3]);
 *
 *	// Copy the previously-unpacked chromas for the next set fo 16 pix
 *	unpack_out[1] = _mm_load_si128(&unpack_out[6]);
 *	unpack_out[4] = _mm_load_si128(&unpack_out[7]);
 *
 *	// reconstruct missing U-V values for first 8 pix
 *	reconstruct_missing_uv_sse2_ssse3(&unpack_out[1], &unpack_out[4], &unpack_out[2]);
 *
 *	// convert first 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(unpack_out, convert_out);
 *
 *	// reconstruct missing U-V values for next 8 pix
 *	reconstruct_last_missing_uv_sse2_ssse3(&unpack_out[4], &unpack_out[5]);
 *
 *	// convert next 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(&unpack_out[3], &convert_out[3]);
 *
 *	// pack both sets of 8 pixels
 *	pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, argb_4pixels);
 *
 *
 */
#define UPSAMPLE_YUV422P_TO_RGB_RECIPE(unpack_y_fn, unpack_lo_uv_fn, unpack_hi_uv_fn, pack_fn, conv_fn_prefix, output_stride, instr_set) \
 	__m128i		unpack_out[8];\
	__m128i		convert_out[6];\
	__m128i*    y_plane = (__m128i *) source_buffer;\
	__m128i*    u_plane = (__m128i*)((uint8_t *) source_buffer + pixfc->pixel_count);\
	__m128i*    v_plane = (__m128i*)((uint8_t *) u_plane  + pixfc->pixel_count / 2);\
	__m128i*	rgb_out = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	unpack_lo_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[4]);\
	while(pixel_count > 32) {\
		unpack_y_fn(y_plane, unpack_out, &unpack_out[3]);\
		unpack_hi_uv_fn(u_plane, v_plane, &unpack_out[6], &unpack_out[7]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[6], &unpack_out[5]);\
		conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
		pack_fn(convert_out, rgb_out);\
		y_plane++;\
		rgb_out += output_stride;\
		unpack_y_fn(y_plane, unpack_out, &unpack_out[3]);\
		unpack_out[1] = _mm_load_si128(&unpack_out[6]);\
		unpack_out[4] = _mm_load_si128(&unpack_out[7]);\
		unpack_lo_uv_fn(&u_plane[1], &v_plane[1], &unpack_out[6], &unpack_out[7]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[6], &unpack_out[5]);\
		conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
		pack_fn(convert_out, rgb_out);\
		y_plane++;\
		u_plane++;\
		v_plane++;\
		rgb_out += output_stride;\
		pixel_count -= 32;\
		unpack_out[1] = _mm_load_si128(&unpack_out[6]);\
		unpack_out[4] = _mm_load_si128(&unpack_out[7]);\
	}\
	unpack_y_fn(y_plane, unpack_out, &unpack_out[3]);\
	unpack_hi_uv_fn(u_plane, v_plane, &unpack_out[6], &unpack_out[7]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[6], &unpack_out[5]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	pack_fn(convert_out, rgb_out);\
	y_plane++;\
	rgb_out += output_stride;\
	unpack_y_fn(y_plane, unpack_out, &unpack_out[3]);\
	unpack_out[1] = _mm_load_si128(&unpack_out[6]);\
	unpack_out[4] = _mm_load_si128(&unpack_out[7]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	reconstruct_last_missing_uv_##instr_set(&unpack_out[4], &unpack_out[5]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	pack_fn(convert_out, rgb_out);\



/*
 *
 * Convert YUV422 interleaved to RGB using Nearest Neighbour upsampling
 * ie. re-create the missing chromas by duplicating the previous existing ones.
 *
 *
 * Expansion for a conversion to ARGB:
 *
 *	__m128i*    yuyv_8pixels = (__m128i *) source_buffer;
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
#define YUV422I_TO_RGB_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
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

/*
 * Convert v210 to RGB using NNB upsampling
 * (based off the YUV422I_TO_RGB_RECIPE, adjusted to v210 unpacking creating 6
 * output vectors instead of 2 as YUV422I unpacking does)
 */
#define V210_TO_RGB_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	__m128i*	v210_in = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[6];\
	__m128i		convert_out[6];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		print_xmm16u("Y1-8", &unpack_out[0]);\
		print_xmm16u("UV1-8", &unpack_out[1]);\
		print_xmm16u("Y9-16", &unpack_out[2]);\
		print_xmm16u("UV9-16", &unpack_out[3]);\
		print_xmm16u("Y17-24", &unpack_out[4]);\
		print_xmm16u("UV17-24", &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		print_xmm16u("R1-8", &convert_out[0]);\
		print_xmm16u("G1-8", &convert_out[1]);\
		print_xmm16u("B1-8", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
		print_xmm16u("R9-16", &convert_out[3]);\
		print_xmm16u("G9-16", &convert_out[4]);\
		print_xmm16u("B9-16", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		conv_fn_prefix##instr_set(&unpack_out[4], convert_out);\
		print_xmm16u("R17-24", &convert_out[0]);\
		print_xmm16u("G17-24", &convert_out[1]);\
		print_xmm16u("B17-24", &convert_out[2]);\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		print_xmm16u("Y25-32", &unpack_out[0]);\
		print_xmm16u("UV25-32", &unpack_out[1]);\
		print_xmm16u("Y33-40", &unpack_out[2]);\
		print_xmm16u("UV33-40", &unpack_out[3]);\
		print_xmm16u("Y41-48", &unpack_out[4]);\
		print_xmm16u("UV41-48", &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, &convert_out[3]);\
		print_xmm16u("R25-32", &convert_out[3]);\
		print_xmm16u("G25-32", &convert_out[4]);\
		print_xmm16u("B25-32", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		conv_fn_prefix##instr_set(&unpack_out[2], convert_out);\
		print_xmm16u("R33-40", &convert_out[0]);\
		print_xmm16u("G33-40", &convert_out[1]);\
		print_xmm16u("B33-40", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[4], &convert_out[3]);\
		print_xmm16u("R33-48", &convert_out[3]);\
		print_xmm16u("G33-48", &convert_out[4]);\
		print_xmm16u("B33-48", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		pixel_count -= 48;\
	};\

/*
 * Convert V210 to YUV422I
 */
#define V210_TO_YUV422I_RECIPE(unpack_fn_prefix, pack_fn, instr_set) \
	__m128i*	v210_in = (__m128i*) source_buffer;\
	__m128i*	yuv_out = (__m128i*) dest_buffer;\
	uint32_t	pixel_count = pixfc->pixel_count;\
	__m128i		unpack_out[8];\
	while(pixel_count > 0) {\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		unpack_out[0] = _mm_srai_epi16(unpack_out[0], 2);\
		unpack_out[1] = _mm_srai_epi16(unpack_out[1], 2);\
		unpack_out[2] = _mm_srai_epi16(unpack_out[2], 2);\
		unpack_out[3] = _mm_srai_epi16(unpack_out[3], 2);\
		pack_fn(unpack_out, yuv_out);\
		yuv_out += 2;\
		unpack_out[0] = _mm_srai_epi16(unpack_out[4], 2);\
		unpack_out[1] = _mm_srai_epi16(unpack_out[5], 2);\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[2], &unpack_out[4], &unpack_out[6]);\
		v210_in += 4;\
		unpack_out[2] = _mm_srai_epi16(unpack_out[2], 2);\
		unpack_out[3] = _mm_srai_epi16(unpack_out[3], 2);\
		pack_fn(unpack_out, yuv_out);\
		yuv_out += 2;\
		unpack_out[4] = _mm_srai_epi16(unpack_out[4], 2);\
		unpack_out[5] = _mm_srai_epi16(unpack_out[5], 2);\
		unpack_out[6] = _mm_srai_epi16(unpack_out[6], 2);\
		unpack_out[7] = _mm_srai_epi16(unpack_out[7], 2);\
		pack_fn(&unpack_out[4], yuv_out);\
		yuv_out += 2;\
		pixel_count -= 48;\
	}


/*
 *
 * Convert YUV422 planar to RGB using Nearest Neighbour upsampling
 * ie. re-create the missing chromas by duplicating the previous existing ones.
 *
 * We have the following constraints:
 * - SSE instructions handle 16 bytes (ie. 16 values for a given component
 *   Y, U or V) at once,
 * - We have downsampled chromas at a ratio of 2:1 ie. 2 Y values for 1 U
 *   and 1 V value,
 * These constraints have the following implications:
 * - Because each access to the U & V planes will consume 16 values (belonging to
 *   32 pixels), the main loop must handle 32 pixels at a time. This means doing
 *   2 reads (of 16 pixels each) in the Y plane and 1 read in the U and V planes.
 * - Lastly, the most important implication is that the number of pixels MUST
 *   be a multiple of 32. Otherwise, the start of the V plane will not be
 *   16-byte aligned.
 *
 * I was originally going to require a pixel count multiple of 16, but it would have
 * introduced the following:
 * - the buffer size would have to be multiple of 64 bytes. Otherwise, the last
 *   read in the V plane would be 8 bytes past the end of the input buffer
 * - a check would have to be made in the DO_CONVERSION_3U_1P macro to verify
 *   the alignment of each plane, and made the macro situation even uglier and
 *   harder to read than what it is now.
 * - PixFC would have to impose and enforce a check on the buffer size, in addition
 *   to the check on pixel count.
 *
 * Enforcing a pixel count multiple of 32 was easier, and I decided to go that way
 * instead.
 *
 * Expansion for a conversion to ARGB:
 *
 *  __m128i*    y_plane = (__m128i *) source_buffer;
 *	__m128i*    u_plane = (uint8_t *) source_buffer + pixfc->pixel_count;
 *	__m128i*    v_plane = (uint8_t *) u_plane  + pixfc->pixel_count / 2;
 *	__m128i*	rgb_out = (__m128i *) dest_buffer;
 *	uint32_t	pixel_count = pixfc->pixel_count;
 *	__m128i		unpack_out[4];
 *	__m128i		convert_out[6];
 *
 *
 *	while(pixel_count > 32) {	//
 *
 *		// unpack 16 Y samples
 *		unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[2]);
 *
 *		// unpack first 8 U / V samples
 *		unpack_low_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);
 *
 *		// convert first 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(unpack_out, convert_out);
 *
 *		// convert next 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(&unpack_out[2], &convert_out[3]);
 *
 *		// pack both sets of 8 pixels
 *		pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, rgb_out);
 *
 *		// Move on to next set of 16 Y samples
 *		y_plane++;
 *		// U and V stay where they are
 *		rgb_out += 4;
 *
 *		// unpack 16 Y samples
 *		unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[2]);
 *
 *		// unpack next 8 U / V samples
 *		unpack_hi_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);
 *
 *		// convert first 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(unpack_out, convert_out);
 *
 *		// convert next 8 pixels
 *		convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(&unpack_out[2], &convert_out[3]);
 *
 *		// pack both sets of 8 pixels
 *		pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, rgb_out);
 *
 *		// Move on to next set of 16 Y, U , V samples
 *		y_plane++;
 *		u_plane++;
 *		v_plane++;
 *		rgb_out += 4;
 *		pixel_count -= 32;
 *	}
 *
 *	// We have 32 pixels left, same code as in main loop
 *
 *	// unpack 16 Y samples
 *	unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[2]);
 *
 *	// unpack first 8 U / V samples
 *	unpack_low_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);
 *
 *	// convert first 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(unpack_out, convert_out);
 *
 *	// convert next 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(&unpack_out[2], &convert_out[3]);
 *
 *	// pack both sets of 8 pixels
 *	pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, rgb_out);
 *
 *	// Move on to next set of 16 Y samples
 *	y_plane++;
 *	// U and V stay where they are
 *	rgb_out += 4;
 *
 *	// unpack 16 Y samples
 *	unpack_yuv42Xp_to_2_y_vectors_sse2(y_plane, unpack_out, &unpack_out[2]);
 *
 *	// unpack next 8 U / V samples
 *	unpack_hi_yuv42Xp_to_uv_vector_sse2(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);
 *
 *	// convert first 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(unpack_out, convert_out);
 *
 *	// convert next 8 pixels
 *	convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(&unpack_out[2], &convert_out[3]);
 *
 *	// pack both sets of 8 pixels
 *	pack_6_rgb_vectors_in_4_argb_vectors_sse2(convert_out, rgb_out);
 *
 *
 */
#define YUV422P_TO_RGB_RECIPE(unpack_y_fn, unpack_lo_uv_fn, unpack_hi_uv_fn, pack_fn, conv_fn_prefix, output_stride, instr_set) \
		__m128i*    y_plane = (__m128i *) source_buffer;\
		__m128i*    u_plane = (__m128i*)((uint8_t *) source_buffer + pixfc->pixel_count);\
		__m128i*    v_plane = (__m128i*)((uint8_t *) u_plane + pixfc->pixel_count / 2);\
		__m128i*	rgb_out = (__m128i *) dest_buffer;\
		uint32_t	pixel_count = pixfc->pixel_count;\
		__m128i		unpack_out[4];\
		__m128i		convert_out[6];\
		while(pixel_count > 32) {\
			unpack_y_fn(y_plane, unpack_out, &unpack_out[2]);\
			unpack_lo_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
			pack_fn(convert_out, rgb_out);\
			y_plane++;\
			rgb_out += output_stride;\
			unpack_y_fn(y_plane, unpack_out, &unpack_out[2]);\
			unpack_hi_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
			pack_fn(convert_out, rgb_out);\
			y_plane++;\
			u_plane++;\
			v_plane++;\
			rgb_out += output_stride;\
			pixel_count -= 32;\
		}\
		unpack_y_fn(y_plane, unpack_out, &unpack_out[2]);\
		unpack_lo_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
		pack_fn(convert_out, rgb_out);\
		y_plane++;\
		rgb_out += output_stride;\
		unpack_y_fn(y_plane, unpack_out, &unpack_out[2]);\
		unpack_hi_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
		pack_fn(convert_out, rgb_out);\


/*
 * Convert a YUV422 planar buffer to YUV422 interleaved.
 */
#define YUV422P_TO_YUV422I_RECIPE(repack_fn, instr_set)	\
		__m128i*    y_plane = (__m128i *) source_buffer;\
		__m128i*    u_plane = (__m128i*)((uint8_t *) source_buffer + pixfc->pixel_count);\
		__m128i*    v_plane = (__m128i*)((uint8_t *) u_plane + pixfc->pixel_count / 2);\
		__m128i*	yuv422i_out = (__m128i *) dest_buffer;\
		uint32_t	pixel_count = pixfc->pixel_count;\
		while(pixel_count > 0) {\
			repack_fn##instr_set(y_plane, u_plane, v_plane, yuv422i_out);\
			y_plane += 2;\
			u_plane++;\
			v_plane++;\
			yuv422i_out += 4;\
			pixel_count -= 32;\
		}

/*
 * Convert a YUV422 interleaved buffer to YUV422 planar.
 */
#define YUV422I_TO_YUV422P_RECIPE(repack_fn, instr_set)	\
		__m128i*    y_plane = (__m128i *) dest_buffer;\
		__m128i*    u_plane = (__m128i*)((uint8_t *) dest_buffer + pixfc->pixel_count);\
		__m128i*    v_plane = (__m128i*)((uint8_t *) u_plane + pixfc->pixel_count / 2);\
		__m128i*	yuv422i_in = (__m128i *) source_buffer;\
		uint32_t	pixel_count = pixfc->pixel_count;\
		while(pixel_count > 0) {\
			repack_fn##instr_set(yuv422i_in, y_plane, u_plane, v_plane);\
			y_plane += 2;\
			u_plane++;\
			v_plane++;\
			yuv422i_in += 4;\
			pixel_count -= 32;\
		}



#define YUV420P_TO_RGB_RECIPE(unpack_y_fn, unpack_lo_uv_fn, unpack_hi_uv_fn, pack_fn, conv_fn_prefix, output_stride, instr_set) \
 	__m128i		unpack_out[4];\
	__m128i		convert_out[6];\
	__m128i*    y_line1 = (__m128i *) source_buffer;\
	__m128i*    y_line2 = (__m128i*)((uint8_t *) y_line1 + pixfc->width);\
	__m128i*    u_plane = (__m128i*)((uint8_t *) source_buffer + pixfc->pixel_count);\
	__m128i*    v_plane = (__m128i*)((uint8_t *) u_plane  + pixfc->pixel_count / 4);\
	__m128i*	rgb_out_line1 = (__m128i *) dest_buffer;\
	__m128i*	rgb_out_line2 = (__m128i *) ((uint8_t *)rgb_out_line1 + output_stride * pixfc->width);\
	uint32_t	pixels_remaining_on_line = pixfc->width;\
	uint32_t	lines_remaining = pixfc->height;\
	int i;\
	while(lines_remaining > 0) {\
		while(pixels_remaining_on_line > 0) {\
			unpack_y_fn(y_line1, unpack_out, &unpack_out[2]);\
			print_xmm8u("l1 Y 1 - 8:", unpack_out);\
			print_xmm8u("l1 Y 9 - 16:", &unpack_out[2]);\
			y_line1++;\
			unpack_lo_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);\
			print_xmm8u("l12 UV 1 - 4:", &unpack_out[1]);\
			print_xmm8u("l12 UV 5 - 8:", &unpack_out[3]);\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			print_xmm8u("l1 R 1 - 8:", convert_out);\
			print_xmm8u("l1 G 1 - 8:", &convert_out[1]);\
			print_xmm8u("l1 B 1 - 8:", &convert_out[2]);\
			conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
			print_xmm8u("l1 R 9 - 16:", &convert_out[3]);\
			print_xmm8u("l1 G 9 - 16:", &convert_out[4]);\
			print_xmm8u("l1 B 9 - 16:", &convert_out[5]);\
			pack_fn(convert_out, rgb_out_line1);\
			for(i = 0; i < output_stride; i++)\
			print_xmm8u("l1 RGB output 1 - 16:", &rgb_out_line1[i]);\
			rgb_out_line1 += output_stride;\
			unpack_y_fn(y_line2, unpack_out, &unpack_out[2]);\
			print_xmm8u("l2 Y 1 - 8:", unpack_out);\
			print_xmm8u("l2 Y 9 - 16:", &unpack_out[2]);\
			y_line2++;\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			print_xmm8u("l2 R 1 - 8:", convert_out);\
			print_xmm8u("l2 G 1 - 8:", &convert_out[1]);\
			print_xmm8u("l2 B 1 - 8:", &convert_out[2]);\
			conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
			print_xmm8u("l2 R 9 - 16:", &convert_out[3]);\
			print_xmm8u("l2 G 9 - 16:", &convert_out[4]);\
			print_xmm8u("l2 B 9 - 16:", &convert_out[5]);\
			pack_fn(convert_out, rgb_out_line2);\
			for(i = 0; i < output_stride; i++)\
			print_xmm8u("l2 RGB output 1 - 16:", rgb_out_line2);\
			rgb_out_line2 += output_stride;\
			unpack_y_fn(y_line1, unpack_out, &unpack_out[2]);\
			print_xmm8u("l1 Y 17 - 24:", unpack_out);\
			print_xmm8u("l1 Y 25 - 32:", &unpack_out[2]);\
			y_line1++;\
			unpack_hi_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[3]);\
			print_xmm8u("l12 UV 9 - 12:", &unpack_out[1]);\
			print_xmm8u("l12 UV 13 - 16:", &unpack_out[3]);\
			u_plane++;\
			v_plane++;\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			print_xmm8u("l1 R 17 - 24:", convert_out);\
			print_xmm8u("l1 G 17 - 24:", &convert_out[1]);\
			print_xmm8u("l1 B 17 - 24:", &convert_out[2]);\
			conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
			print_xmm8u("l1 R 25 - 32:", &convert_out[3]);\
			print_xmm8u("l1 G 25 - 32:", &convert_out[4]);\
			print_xmm8u("l1 B 25 - 32:", &convert_out[5]);\
			pack_fn(convert_out, rgb_out_line1);\
			for(i = 0; i < output_stride; i++)\
			print_xmm8u("l1 RGB output  17 - 32:", rgb_out_line1);\
			rgb_out_line1 += output_stride;\
			unpack_y_fn(y_line2, unpack_out, &unpack_out[2]);\
			print_xmm8u("l2 Y 17 - 24:", unpack_out);\
			print_xmm8u("l2 Y 25 - 32:", &unpack_out[2]);\
			y_line2++;\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			print_xmm8u("l2 R 17 - 24:", convert_out);\
			print_xmm8u("l2 G 17 - 24:", &convert_out[1]);\
			print_xmm8u("l2 B 17 - 24:", &convert_out[2]);\
			conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
			print_xmm8u("l2 R 25 - 32:", &convert_out[3]);\
			print_xmm8u("l2 G 25 - 32:", &convert_out[4]);\
			print_xmm8u("l2 B 25 - 32:", &convert_out[5]);\
			pack_fn(convert_out, rgb_out_line2);\
			for(i = 0; i < output_stride; i++)\
			print_xmm8u("l2 RGB output 17 - 32:", rgb_out_line1);\
			rgb_out_line2 += output_stride;\
			pixels_remaining_on_line -= 32;\
		}\
		y_line1 = (__m128i*)((uint8_t *) y_line1 + pixfc->width);\
		y_line2 = (__m128i*)((uint8_t *) y_line2 + pixfc->width);\
		rgb_out_line1 = (__m128i *) ((uint8_t *)rgb_out_line1 + output_stride * pixfc->width);\
		rgb_out_line2 = (__m128i *) ((uint8_t *)rgb_out_line2 + output_stride * pixfc->width);\
		lines_remaining -=2;\
		pixels_remaining_on_line = pixfc->width;\
	}\

#endif /* YUV_CONVERSION_RECIPES_H_ */
