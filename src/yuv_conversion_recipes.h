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

#include "common.h"

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

#include "pixfmt_descriptions.h"

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
	/* number of __m128i padding vectors left until the end of line */\
	DECLARE_PADDING_VECT_COUNT(padding_vect_to_eol, pixfc->dest_fmt, pixfc->width);\
	__m128i		unpack_out[8];\
	__m128i		convert_out[6];\
	__m128i*    yuyv_8pixels = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	uint32_t	pixel_count;\
	uint32_t 	lines_remaining = pixfc->height;\
	while(lines_remaining-- > 0) {\
		pixel_count = pixfc->width - 16;\
		unpack_fn_prefix##instr_set(yuyv_8pixels, unpack_out);\
		while(pixel_count > 0) {\
			print_xmm16u("Y1-8", unpack_out);\
			print_xmm16u("UV1-8odd", &unpack_out[1]);\
			unpack_fn_prefix##instr_set(&yuyv_8pixels[1], &unpack_out[3]);\
			print_xmm16u("Y9-16", &unpack_out[3]);\
			print_xmm16u("UV9-16odd", &unpack_out[4]);\
			unpack_fn_prefix##instr_set(&yuyv_8pixels[2], &unpack_out[6]);\
			reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
			print_xmm16u("UV1-8even", &unpack_out[2]);\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			print_xmm16("R1-8", &convert_out[0]);\
			print_xmm16("G1-8", &convert_out[1]);\
			print_xmm16("B1-8", &convert_out[2]);\
			reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
			print_xmm16u("UV9-16even", &unpack_out[5]);\
			conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
			print_xmm16("R9-16", &convert_out[3]);\
			print_xmm16("G9-16", &convert_out[4]);\
			print_xmm16("B9-16", &convert_out[5]);\
			pack_fn(convert_out, rgb_out_buf);\
			yuyv_8pixels += 2;\
			rgb_out_buf += output_stride;\
			pixel_count -= 16;\
			unpack_out[0] = _mm_load_si128(&unpack_out[6]);\
			unpack_out[1] = _mm_load_si128(&unpack_out[7]);\
		}\
		print_xmm16u("Y1-8", unpack_out);\
		print_xmm16u("UV1-8odd", &unpack_out[1]);\
		unpack_fn_prefix##instr_set(&yuyv_8pixels[1], &unpack_out[3]);\
		print_xmm16u("Y9-16", &unpack_out[3]);\
		print_xmm16u("UV9-16odd", &unpack_out[4]);\
		reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
		reconstruct_last_missing_uv_##instr_set(&unpack_out[4], &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
		pack_fn(convert_out, rgb_out_buf);\
		yuyv_8pixels += 2;\
		rgb_out_buf += output_stride;\
		rgb_out_buf += padding_vect_to_eol;\
	}


/*
 *
 * AVG upsampling V210 to RGB
 *
 */

// Preamble: unpacks 24 pixels
#define AVG_UPSAMPLE_V210_TO_RGB_PREAMBLE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[3], &unpack_out[6]);\
	v210_in += 4;\
	print_xmm16u("Pre Y1-8", &unpack_out[0]);\
	print_xmm16u("Pre UVodd1-8", &unpack_out[1]);\
	print_xmm16u("Pre Y9-16", &unpack_out[3]);\
	print_xmm16u("Pre UVodd9-16", &unpack_out[4]);\
	print_xmm16u("Pre Y17-24", &unpack_out[6]);\
	print_xmm16u("Pre UVodd17-24", &unpack_out[7]);\

/*
 *  48 pixel loop core
 *  - expects 24 pixels previously unpacked
 *  - unpacks 24 more
 *  - converts 48 pixels
 *  - unpacks 24 more for the next round
 */
#define AVG_UPSAMPLE_V210_TO_RGB_CORE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("Main UVeven1-8", &unpack_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("Main UVeven9-16", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("Main R1-8", &convert_out[0]);\
	print_xmm16("Main G1-8", &convert_out[1]);\
	print_xmm16("Main B1-8", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("Main R9-16", &convert_out[3]);\
	print_xmm16("Main G9-16", &convert_out[4]);\
	print_xmm16("Main B9-16", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	unpack_out[0] = _mm_load_si128(&unpack_out[6]);\
	unpack_out[1] = _mm_load_si128(&unpack_out[7]);\
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[3], &unpack_out[6], &unpack_out[9]);\
	v210_in += 4;\
	print_xmm16u("Main Y25-32", &unpack_out[3]);\
	print_xmm16u("Main UVodd25-32", &unpack_out[4]);\
	print_xmm16u("Main Y33-40", &unpack_out[6]);\
	print_xmm16u("Main UVodd33-40", &unpack_out[7]);\
	print_xmm16u("Main Y41-48", &unpack_out[9]);\
	print_xmm16u("Main UVodd41-48", &unpack_out[10]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("Main UVeven17-24", &unpack_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("Main UVeven25-32", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("Main R17-24", &convert_out[0]);\
	print_xmm16("Main G17-24", &convert_out[1]);\
	print_xmm16("Main B17-24", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("Main R25-32", &convert_out[3]);\
	print_xmm16("Main G25-32", &convert_out[4]);\
	print_xmm16("Main B25-32", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	reconstruct_missing_uv_##instr_set(&unpack_out[7], &unpack_out[10], &unpack_out[8]);\
	print_xmm16u("Main UVeven33-40", &unpack_out[8]);\
	conv_fn_prefix##instr_set(&unpack_out[6], convert_out);\
	print_xmm16("Main R33-40", &convert_out[0]);\
	print_xmm16("Main G33-40", &convert_out[1]);\
	print_xmm16("Main B33-40", &convert_out[2]);\
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[3], &unpack_out[6]);\
	v210_in += 4;\
	print_xmm16u("Main NEXT Y1-8", &unpack_out[0]);\
	print_xmm16u("Main NEXT UVodd1-8", &unpack_out[1]);\
	print_xmm16u("Main NEXT Y9-16", &unpack_out[3]);\
	print_xmm16u("Main NEXT UVodd9-16", &unpack_out[4]);\
	print_xmm16u("Main NEXT Y17-24", &unpack_out[6]);\
	print_xmm16u("Main NEXT UVodd17-24", &unpack_out[7]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[10], &unpack_out[1], &unpack_out[11]);\
	print_xmm16u("Main UVeven41-48", &unpack_out[11]);\
	conv_fn_prefix##instr_set(&unpack_out[9], &convert_out[3]);\
	print_xmm16("Main R41-48", &convert_out[3]);\
	print_xmm16("Main G41-48", &convert_out[4]);\
	print_xmm16("Main B41-48", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\

/*
 * Handles last 48 pixels
 */
#define AVG_UPSAMPLE_V210_TO_RGB_CORE_LAST48(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("L48 UVeven1-8", &unpack_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("L48 UVeven9-16", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L48 R1-8", &convert_out[0]);\
	print_xmm16("L48 G1-8", &convert_out[1]);\
	print_xmm16("L48 B1-8", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("L48 R9-16", &convert_out[3]);\
	print_xmm16("L48 G9-16", &convert_out[4]);\
	print_xmm16("L48 B9-16", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	unpack_out[0] = _mm_load_si128(&unpack_out[6]);\
	unpack_out[1] = _mm_load_si128(&unpack_out[7]);\
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[3], &unpack_out[6], &unpack_out[9]);\
	v210_in += 4;\
	print_xmm16u("L48 Y25-32", &unpack_out[3]);\
	print_xmm16u("L48 UVodd25-32", &unpack_out[4]);\
	print_xmm16u("L48 Y33-40", &unpack_out[6]);\
	print_xmm16u("L48 UVodd33-40", &unpack_out[7]);\
	print_xmm16u("L48 Y41-48", &unpack_out[9]);\
	print_xmm16u("L48 UVodd41-48", &unpack_out[10]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("L48 UVeven17-24", &unpack_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("L48 UVeven25-32", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L48 R17-24", &convert_out[0]);\
	print_xmm16("L48 G17-24", &convert_out[1]);\
	print_xmm16("L48 B17-24", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("L48 R25-32", &convert_out[3]);\
	print_xmm16("L48 G25-32", &convert_out[4]);\
	print_xmm16("L48 B25-32", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	reconstruct_missing_uv_##instr_set(&unpack_out[7], &unpack_out[10], &unpack_out[8]);\
	print_xmm16u("L48 UVeven33-40", &unpack_out[8]);\
	conv_fn_prefix##instr_set(&unpack_out[6], convert_out);\
	print_xmm16("L48 R33-40", &convert_out[0]);\
	print_xmm16("L48 G33-40", &convert_out[1]);\
	print_xmm16("L48 B33-40", &convert_out[2]);\
	reconstruct_last_missing_uv_##instr_set(&unpack_out[10], &unpack_out[11]);\
	print_xmm16u("L48 UVeven41-48", &unpack_out[11]);\
	conv_fn_prefix##instr_set(&unpack_out[9], &convert_out[3]);\
	print_xmm16("L48 R41-48", &convert_out[3]);\
	print_xmm16("L48 G41-48", &convert_out[4]);\
	print_xmm16("L48 B41-48", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\

/*
 * Handles 16 pixels leftover
 */
#define AVG_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER16(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("L16 UVeven1-8", &unpack_out[2]);\
	reconstruct_last_missing_uv_##instr_set(&unpack_out[4], &unpack_out[5]);\
	print_xmm16u("L16 UVeven9-16", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L16 R1-8", &convert_out[0]);\
	print_xmm16("L16 G1-8", &convert_out[1]);\
	print_xmm16("L16 B1-8", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("L16 R9-16", &convert_out[3]);\
	print_xmm16("L16 G9-16", &convert_out[4]);\
	print_xmm16("L16 B9-16", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	/* Move the v210 pointer as if we read 48 pixels. It was already incremented by 4 (24 pix)
	 * when pix 1-8 were read, move it a further 4 (24pix) to make it 48 pix.*/\
	v210_in += 4;\

/*
 * Handles 32 pixels leftover
 */
#define AVG_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER32(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("L32 UVeven1-8", &unpack_out[2]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[4], &unpack_out[7], &unpack_out[5]);\
	print_xmm16u("L32 UVeven9-16", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L32 R1-8", &convert_out[0]);\
	print_xmm16("L32 G1-8", &convert_out[1]);\
	print_xmm16("L32 B1-8", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("L32 R9-16", &convert_out[3]);\
	print_xmm16("L32 G9-16", &convert_out[4]);\
	print_xmm16("L32 B9-16", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\
	unpack_out[0] = _mm_load_si128(&unpack_out[6]);\
	unpack_out[1] = _mm_load_si128(&unpack_out[7]);\
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[3], &unpack_out[6], &unpack_out[9]);\
	v210_in += 4;\
	print_xmm16u("L32 Y25-32", &unpack_out[3]);\
	print_xmm16u("L32 UVodd25-32", &unpack_out[4]);\
	reconstruct_missing_uv_##instr_set(&unpack_out[1], &unpack_out[4], &unpack_out[2]);\
	print_xmm16u("L32 UVeven17-24", &unpack_out[2]);\
	reconstruct_last_missing_uv_##instr_set(&unpack_out[4], &unpack_out[5]);\
	print_xmm16u("L32 UVeven25-32", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L32 R17-24", &convert_out[0]);\
	print_xmm16("L32 G17-24", &convert_out[1]);\
	print_xmm16("L32 B17-24", &convert_out[2]);\
	conv_fn_prefix##instr_set(&unpack_out[3], &convert_out[3]);\
	print_xmm16("L32 R25-32", &convert_out[3]);\
	print_xmm16("L32 G25-32", &convert_out[4]);\
	print_xmm16("L32 B25-32", &convert_out[5]);\
	pack_fn(convert_out, rgb_out_buf);\
	rgb_out_buf += output_stride;\



/*
 * Convert v210 to RGB using AVG upsampling
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
	FROM_V120_48_PIX_OUTER_CONVERSION_LOOP(\
			AVG_UPSAMPLE_V210_TO_RGB_PREAMBLE,\
			AVG_UPSAMPLE_V210_TO_RGB_CORE,\
			AVG_UPSAMPLE_V210_TO_RGB_CORE_LAST48,\
			AVG_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER16,\
			AVG_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER32,\
			unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set\
	)


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
	uint32_t	pixel_count;\
	uint32_t 	lines_remaining = pixfc->height;\
	DECLARE_PADDING_VECT_COUNT(padding_vect_to_eol, pixfc->dest_fmt, pixfc->width);\
	\
	while(lines_remaining-- > 0) {\
		pixel_count = pixfc->width - 32;\
		unpack_lo_uv_fn(u_plane, v_plane, &unpack_out[1], &unpack_out[4]);\
		while(pixel_count > 0) {\
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
		y_plane++;\
		u_plane++;\
		v_plane++;\
		rgb_out += output_stride;\
		rgb_out += padding_vect_to_eol;\
	}



/*
 *
 * Convert YUV422 interleaved to RGB using Nearest Neighbour upsampling
 * ie. re-create the missing chromas by duplicating the previous existing ones.
 *
 *
 */
#define YUV422I_TO_RGB_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	DECLARE_PADDING_VECT_COUNT(padding_vect_to_eol, pixfc->dest_fmt, pixfc->width);\
	__m128i*	yuyv_8pixels = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	uint32_t	pixel_count;\
	uint32_t 	lines_remaining = pixfc->height;\
	__m128i		unpack_out[2];\
	__m128i		convert_out[6];\
	while(lines_remaining-- > 0) {\
		pixel_count = pixfc->width;\
		while(pixel_count > 0) {\
			unpack_fn_prefix##instr_set(yuyv_8pixels, unpack_out);\
			print_xmm16u("Y", unpack_out);\
			print_xmm16u("UV", &unpack_out[1]);\
			conv_fn_prefix##instr_set(unpack_out, convert_out);\
			print_xmm16u("R", &convert_out[0]);\
			print_xmm16u("G", &convert_out[1]);\
			print_xmm16u("B", &convert_out[2]);\
			unpack_fn_prefix##instr_set(&yuyv_8pixels[1], unpack_out);\
			print_xmm16u("Y", unpack_out);\
			print_xmm16u("UV", &unpack_out[1]);\
			conv_fn_prefix##instr_set(unpack_out, &convert_out[3]);\
			print_xmm16u("R", &convert_out[3]);\
			print_xmm16u("G", &convert_out[4]);\
			print_xmm16u("B", &convert_out[5]);\
			pack_fn(convert_out, rgb_out_buf);\
			yuyv_8pixels += 2;\
			rgb_out_buf += output_stride;\
			pixel_count -= 16;\
		}\
		rgb_out_buf += padding_vect_to_eol;\
	}

/*
 * NNB upsampling v210 to RGB
 */
// Loop core handling 48 pixels
#define NNB_UPSAMPLE_V210_TO_RGB_CORE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		print_xmm16u("Main Y1-8", &unpack_out[0]);\
		print_xmm16u("Main UV1-8", &unpack_out[1]);\
		print_xmm16u("Main Y9-16", &unpack_out[2]);\
		print_xmm16u("Main UV9-16", &unpack_out[3]);\
		print_xmm16u("Main Y17-24", &unpack_out[4]);\
		print_xmm16u("Main UV17-24", &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		print_xmm16("Main R1-8", &convert_out[0]);\
		print_xmm16("Main G1-8", &convert_out[1]);\
		print_xmm16("Main B1-8", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
		print_xmm16("Main R9-16", &convert_out[3]);\
		print_xmm16("Main G9-16", &convert_out[4]);\
		print_xmm16("Main B9-16", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		conv_fn_prefix##instr_set(&unpack_out[4], convert_out);\
		print_xmm16("Main R17-24", &convert_out[0]);\
		print_xmm16("Main G17-24", &convert_out[1]);\
		print_xmm16("Main B17-24", &convert_out[2]);\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		print_xmm16u("Main Y25-32", &unpack_out[0]);\
		print_xmm16u("Main UV25-32", &unpack_out[1]);\
		print_xmm16u("Main Y33-40", &unpack_out[2]);\
		print_xmm16u("Main UV33-40", &unpack_out[3]);\
		print_xmm16u("Main Y41-48", &unpack_out[4]);\
		print_xmm16u("Main UV41-48", &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, &convert_out[3]);\
		print_xmm16("Main R25-32", &convert_out[3]);\
		print_xmm16("Main G25-32", &convert_out[4]);\
		print_xmm16("Main B25-32", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		conv_fn_prefix##instr_set(&unpack_out[2], convert_out);\
		print_xmm16("Main R33-40", &convert_out[0]);\
		print_xmm16("Main G33-40", &convert_out[1]);\
		print_xmm16("Main B33-40", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[4], &convert_out[3]);\
		print_xmm16("Main R33-48", &convert_out[3]);\
		print_xmm16("Main G33-48", &convert_out[4]);\
		print_xmm16("Main B33-48", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\

// loop core handling the last 16 pixels
#define NNB_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER16(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		/* We are supposed to read 2x24 v210 pixels, but since we know there are
		 * only 16, we only read 1x24, but we still move the v210 pointer as if
		 * we read 2x24 pixels, hence the "+8" on the next line. */\
		v210_in += 8; \
		print_xmm16u("L16 Y1-8", &unpack_out[0]);\
		print_xmm16u("L16 UV1-8", &unpack_out[1]);\
		print_xmm16u("L16 Y9-16", &unpack_out[2]);\
		print_xmm16u("L16 UV9-16", &unpack_out[3]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		print_xmm16("L16 R1-8", &convert_out[0]);\
		print_xmm16("L16 G1-8", &convert_out[1]);\
		print_xmm16("L16 B1-8", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
		print_xmm16("L16 R9-16", &convert_out[3]);\
		print_xmm16("L16 G9-16", &convert_out[4]);\
		print_xmm16("L16 B9-16", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\

// Loop core handling the last 32 pixels
#define NNB_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER32(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		print_xmm16u("L32 Y1-8", &unpack_out[0]);\
		print_xmm16u("L32 UV1-8", &unpack_out[1]);\
		print_xmm16u("L32 Y9-16", &unpack_out[2]);\
		print_xmm16u("L32 UV9-16", &unpack_out[3]);\
		print_xmm16u("L32 Y17-24", &unpack_out[4]);\
		print_xmm16u("L32 UV17-24", &unpack_out[5]);\
		conv_fn_prefix##instr_set(unpack_out, convert_out);\
		print_xmm16("L32 R1-8", &convert_out[0]);\
		print_xmm16("L32 G1-8", &convert_out[1]);\
		print_xmm16("L32 B1-8", &convert_out[2]);\
		conv_fn_prefix##instr_set(&unpack_out[2], &convert_out[3]);\
		print_xmm16("L32 R9-16", &convert_out[3]);\
		print_xmm16("L32 G9-16", &convert_out[4]);\
		print_xmm16("L32 B9-16", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\
		conv_fn_prefix##instr_set(&unpack_out[4], convert_out);\
		print_xmm16("L32 R17-24", &convert_out[0]);\
		print_xmm16("L32 G17-24", &convert_out[1]);\
		print_xmm16("L32 B17-24", &convert_out[2]);\
		unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
		v210_in += 4;\
		print_xmm16u("L32 Y25-32", &unpack_out[0]);\
		print_xmm16u("L32 UV25-32", &unpack_out[1]);\
		conv_fn_prefix##instr_set(unpack_out, &convert_out[3]);\
		print_xmm16("L32 R25-32", &convert_out[3]);\
		print_xmm16("L32 G25-32", &convert_out[4]);\
		print_xmm16("L32 B25-32", &convert_out[5]);\
		pack_fn(convert_out, rgb_out_buf);\
		rgb_out_buf += output_stride;\

/*
 * Convert v210 to RGB using NNB upsampling
 */
#define V210_TO_RGB_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	__m128i*	v210_in = (__m128i *) source_buffer;\
	__m128i*	rgb_out_buf = (__m128i *) dest_buffer;\
	__m128i		unpack_out[6];\
	__m128i		convert_out[6];\
	FROM_V120_48_PIX_OUTER_CONVERSION_LOOP(\
			EMPTY, 							/* no preamble */\
			NNB_UPSAMPLE_V210_TO_RGB_CORE,	/* 48-pixel */\
			NNB_UPSAMPLE_V210_TO_RGB_CORE,	/* last 48-pixel */\
			NNB_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER16,	/* last 16 pixels */\
			NNB_UPSAMPLE_V210_TO_RGB_CORE_LEFTOVER32,	/* last 32 pixels */\
			unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set\
		);





/*
 * NNB upsampling v210 to r210
 */
// Loop core handling 48 pixels
#define NNB_UPSAMPLE_V210_TO_R210_CORE(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
	v210_in += 4;\
	print_xmm16u("Main Y1-8", &unpack_out[0]);\
	print_xmm16u("Main UV1-8", &unpack_out[1]);\
	print_xmm16u("Main Y9-16", &unpack_out[2]);\
	print_xmm16u("Main UV9-16", &unpack_out[3]);\
	print_xmm16u("Main Y17-24", &unpack_out[4]);\
	print_xmm16u("Main UV17-24", &unpack_out[5]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("Main R1-8", &convert_out[0]);\
	print_xmm16("Main G1-8", &convert_out[1]);\
	print_xmm16("Main B1-8", &convert_out[2]);\
	pack_fn(convert_out, r210_out);\
	r210_out += 2;\
	conv_fn_prefix##instr_set(&unpack_out[2], convert_out);\
	print_xmm16("Main R9-16", &convert_out[0]);\
	print_xmm16("Main G9-16", &convert_out[1]);\
	print_xmm16("Main B9-16", &convert_out[2]);\
	pack_fn(convert_out, r210_out);\
	r210_out += 2;\
	conv_fn_prefix##instr_set(&unpack_out[4], convert_out);\
	print_xmm16("Main R17-24", &convert_out[0]);\
	print_xmm16("Main G17-24", &convert_out[1]);\
	print_xmm16("Main B17-24", &convert_out[2]);\
	pack_fn(convert_out, r210_out);\
	r210_out += 2;\

// Last 16 pixels
#define NNB_UPSAMPLE_V210_TO_R210_LAST_16PIX(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
	print_xmm16u("L16 Y1-8", &unpack_out[0]);\
	print_xmm16u("L16 UV1-8", &unpack_out[1]);\
	print_xmm16u("L16 Y9-16", &unpack_out[2]);\
	print_xmm16u("L16 UV9-16", &unpack_out[3]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L16 R1-8", &convert_out[0]);\
	print_xmm16("L16 G1-8", &convert_out[1]);\
	print_xmm16("L16 B1-8", &convert_out[2]);\
	pack_fn(convert_out, r210_out);\
	r210_out += 2;\
	conv_fn_prefix##instr_set(&unpack_out[2], convert_out);\
	print_xmm16("L16 R9-16", &convert_out[0]);\
	print_xmm16("L16 G9-16", &convert_out[1]);\
	print_xmm16("L16 B9-16", &convert_out[2]);\
	pack_fn(convert_out, r210_out);\

// Last 8 pixels
#define NNB_UPSAMPLE_V210_TO_R210_LAST_8PIX(unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set) \
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
	/* dont update v210_ptr as this is the end of the line and it will be udpated by the loop macro anyway. */\
	/* Same goes for r210_out */\
	print_xmm16u("L8 Y1-8", &unpack_out[0]);\
	print_xmm16u("L8 UV1-8", &unpack_out[1]);\
	conv_fn_prefix##instr_set(unpack_out, convert_out);\
	print_xmm16("L8 R1-8", &convert_out[0]);\
	print_xmm16("L8 G1-8", &convert_out[1]);\
	print_xmm16("L8 B1-8", &convert_out[2]);\
	pack_fn(convert_out, r210_out);\

/*
 * Convert v210 to r210 using NNB upsampling
 */
#define V210_TO_R210_RECIPE(unpack_fn_prefix, pack_fn, conv_fn_prefix, instr_set) \
	__m128i*	v210_in = (__m128i *) source_buffer;\
	__m128i*	r210_out = (__m128i *) dest_buffer;\
	__m128i		unpack_out[6];\
	__m128i		convert_out[3];\
	FROM_V120_24_PIX_OUTER_CONVERSION_LOOP(\
		v210_in, r210_out,\
		NNB_UPSAMPLE_V210_TO_R210_CORE, /* 24-pixel core */\
		NNB_UPSAMPLE_V210_TO_R210_CORE,	/* last 24 pixels */\
		NNB_UPSAMPLE_V210_TO_R210_LAST_8PIX,	/* last 8 pixels */\
		NNB_UPSAMPLE_V210_TO_R210_LAST_16PIX,	/* last 16 pixels */\
		unpack_fn_prefix, pack_fn, conv_fn_prefix, output_stride, instr_set\
	);


/*
 * Convert V210 to YUV422I
 */
#define V210_TO_YUV422I_CORE(unpack_fn_prefix, pack_fn, instr_set) \
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

#define V210_TO_YUV422I_CORE_LEFTOVER16(unpack_fn_prefix, pack_fn, instr_set) \
	unpack_fn_prefix##instr_set(v210_in, &unpack_out[0], &unpack_out[2], &unpack_out[4]);\
	/* We are supposed to read 2x24 v210 pixels, but since we know there are
	 * only 16, we only read 1x24, but we still move the v210 pointer as if
	 * we read 2x24 pixels, hence the "+8" on the next line. */\
	v210_in += 8;\
	unpack_out[0] = _mm_srai_epi16(unpack_out[0], 2);\
	unpack_out[1] = _mm_srai_epi16(unpack_out[1], 2);\
	unpack_out[2] = _mm_srai_epi16(unpack_out[2], 2);\
	unpack_out[3] = _mm_srai_epi16(unpack_out[3], 2);\
	pack_fn(unpack_out, yuv_out);\
	yuv_out += 2;\

#define V210_TO_YUV422I_CORE_LEFTOVER32(unpack_fn_prefix, pack_fn, instr_set) \
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

#define V210_TO_YUV422I_RECIPE(unpack_fn_prefix, pack_fn, instr_set) \
	__m128i*	v210_in = (__m128i*) source_buffer;\
	__m128i*	yuv_out = (__m128i*) dest_buffer;\
	__m128i		unpack_out[8];\
	FROM_V120_48_PIX_OUTER_CONVERSION_LOOP(\
			EMPTY,									/* No preamble */\
			V210_TO_YUV422I_CORE,					/* 48 pixel core loop */\
			V210_TO_YUV422I_CORE,					/* Last 48 pixels */\
			V210_TO_YUV422I_CORE_LEFTOVER16,		/* Last 16 pixels */\
			V210_TO_YUV422I_CORE_LEFTOVER32,		/* Last 32 pixels */\
			unpack_fn_prefix, pack_fn, instr_set\
		)


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
		DECLARE_PADDING_VECT_COUNT(padding_vect_to_eol, pixfc->dest_fmt, pixfc->width);\
		__m128i*    y_plane = (__m128i *) source_buffer;\
		__m128i*    u_plane = (__m128i*)((uint8_t *) source_buffer + pixfc->pixel_count);\
		__m128i*    v_plane = (__m128i*)((uint8_t *) u_plane + pixfc->pixel_count / 2);\
		__m128i*	rgb_out = (__m128i *) dest_buffer;\
		uint32_t	pixel_count;\
		uint32_t 	lines_remaining = pixfc->height;\
		__m128i		unpack_out[4];\
		__m128i		convert_out[6];\
		while(lines_remaining-- > 0) {\
			pixel_count = pixfc->width - 32;\
			while(pixel_count > 0) {\
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
			y_plane++;\
			u_plane++;\
			v_plane++;\
			rgb_out += output_stride;\
			rgb_out += padding_vect_to_eol;\
		}


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



/*
 * Convert YUV422I to V210
 */
#define YUV422I_TO_V210_CORE(unpack_fn, pack_fn) \
	unpack_fn(yuv_in, &unpack_out[0]);\
	/*print_xmm16u("M24 Y1-8", &unpack_out[0]);\
	print_xmm16u("M24 UV1-8", &unpack_out[1]);*/\
	yuv_in++;\
	unpack_fn(yuv_in, &unpack_out[2]);\
	/*print_xmm16u("M24 Y9-16", &unpack_out[2]);\
	print_xmm16u("M24 UV9-16", &unpack_out[3]);*/\
	yuv_in++;\
	unpack_fn(yuv_in, &unpack_out[4]);\
	/*print_xmm16u("M24 Y17-24", &unpack_out[4]);\
	print_xmm16u("M24 UV17-24", &unpack_out[5]);*/\
	yuv_in++;\
	unpack_out[0] = _mm_slli_epi16(unpack_out[0], 2);\
	unpack_out[1] = _mm_slli_epi16(unpack_out[1], 2);\
	unpack_out[2] = _mm_slli_epi16(unpack_out[2], 2);\
	unpack_out[3] = _mm_slli_epi16(unpack_out[3], 2);\
	unpack_out[4] = _mm_slli_epi16(unpack_out[4], 2);\
	unpack_out[5] = _mm_slli_epi16(unpack_out[5], 2);\
	print_xmm16u("M24 Y1-8 10 bits", &unpack_out[0]);\
	print_xmm16u("M24 UV1-8 10 bits", &unpack_out[1]);\
	print_xmm16u("M24 Y9-16 10 bits", &unpack_out[2]);\
	print_xmm16u("M24 UV9-16 10 bits", &unpack_out[3]);\
	print_xmm16u("M24 Y17-24 10 bits", &unpack_out[4]);\
	print_xmm16u("M24 UV17-24 10 bits", &unpack_out[5]);\
	pack_fn(unpack_out, v210_out);\
	v210_out += 4;

#define YUV422I_TO_V210_8PIX_LEFTOVER(unpack_fn, pack_fn) \
	unpack_fn(yuv_in, &unpack_out[0]);\
	/*print_xmm16u("L8 Y1-8", &unpack_out[0]);\
	print_xmm16u("L8 UV1-8", &unpack_out[1]);*/\
	yuv_in++;\
	unpack_out[0] = _mm_slli_epi16(unpack_out[0], 2);\
	unpack_out[1] = _mm_slli_epi16(unpack_out[1], 2);\
	unpack_out[2] = _mm_setzero_si128();\
	unpack_out[3] = _mm_setzero_si128();\
	unpack_out[4] = _mm_setzero_si128();\
	unpack_out[5] = _mm_setzero_si128();\
	print_xmm16u("L8 Y1-8 10 bits", &unpack_out[0]);\
	print_xmm16u("L8 UV1-8 10 bits", &unpack_out[1]);\
	print_xmm16u("L8 Y9-16 10 bits", &unpack_out[2]);\
	print_xmm16u("L8 UV9-16 10 bits", &unpack_out[3]);\
	print_xmm16u("L8 Y17-24 10 bits", &unpack_out[4]);\
	print_xmm16u("L8 UV17-24 10 bits", &unpack_out[5]);\
	pack_fn(unpack_out, v210_out);\
	v210_out += 4;\

#define YUV422I_TO_V210_16PIX_LEFTOVER(unpack_fn, pack_fn) \
	unpack_fn(yuv_in, &unpack_out[0]);\
	/*print_xmm16u("L16 Y1-8", &unpack_out[0]);\
	print_xmm16u("L16 UV1-8", &unpack_out[1]);*/\
	yuv_in++;\
	unpack_fn(yuv_in, &unpack_out[2]);\
	/*print_xmm16u("L16 Y9-16", &unpack_out[2]);\
	print_xmm16u("L16 UV9-16", &unpack_out[3]);*/\
	yuv_in++;\
	unpack_out[0] = _mm_slli_epi16(unpack_out[0], 2);\
	unpack_out[1] = _mm_slli_epi16(unpack_out[1], 2);\
	unpack_out[2] = _mm_slli_epi16(unpack_out[2], 2);\
	unpack_out[3] = _mm_slli_epi16(unpack_out[3], 2);\
	unpack_out[4] = _mm_setzero_si128();\
	unpack_out[5] = _mm_setzero_si128();\
	print_xmm16u("L16 Y1-8 10 bits", &unpack_out[0]);\
	print_xmm16u("L16 UV1-8 10 bits", &unpack_out[1]);\
	print_xmm16u("L16 Y9-16 10 bits", &unpack_out[2]);\
	print_xmm16u("L16 UV9-16 10 bits", &unpack_out[3]);\
	print_xmm16u("L16 Y17-24 10 bits", &unpack_out[4]);\
	print_xmm16u("L16 UV17-24 10 bits", &unpack_out[5]);\
	pack_fn(unpack_out, v210_out);\
	v210_out += 4;\

#define YUV422I_TO_V210_RECIPE(unpack_fn, pack_fn) \
	__m128i*	yuv_in = (__m128i*) source_buffer;\
	__m128i*	v210_out = (__m128i*) dest_buffer;\
	__m128i		unpack_out[6];\
	TO_V120_24_PIX_OUTER_CONVERSION_LOOP(\
			YUV422I_TO_V210_CORE, 			/* loop core for first 24 pixels */\
			YUV422I_TO_V210_CORE, 			/* loop core for remainder groups of 24 pixels */\
			YUV422I_TO_V210_8PIX_LEFTOVER,	/* loop core for last 8 pixels */\
			YUV422I_TO_V210_16PIX_LEFTOVER, /* loop core if width = 16 pixels */\
			YUV422I_TO_V210_16PIX_LEFTOVER, /* loop core for last 16 pixels */\
			v210_out, unpack_fn, pack_fn);




/*
 *
 * YUV420 to RGB
 *
 */
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
