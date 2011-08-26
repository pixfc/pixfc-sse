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

#ifndef DEBUG
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
#define UPSAMPLE_AND_CONVERT_YUV422I_TO_RGB(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
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
#define UPSAMPLE_AND_CONVERT_YUV422P_TO_RGB(unpack_y_fn, unpack_lo_uv_fn, unpack_hi_uv_fn, pack_fn, conv_fn_prefix, output_stride, instr_set) \
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
#define CONVERT_YUV422I_TO_RGB(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
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
#define CONVERT_YUV422P_TO_RGB(unpack_y_fn, unpack_lo_uv_fn, unpack_hi_uv_fn, pack_fn, conv_fn_prefix, output_stride, instr_set) \
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




#endif /* YUYV_CONVERSION_COMMON_H_ */
