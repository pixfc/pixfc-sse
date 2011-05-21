/*
 * conversion_blocks.c
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

#include <stdint.h>

#include "common.h"
#include "conversion_blocks.h"
#include "conversion_routines_from_yuyv.h"
#include "conversion_routines_from_uyvy.h"
#include "conversion_routines_from_argb.h"

#define 	DECLARE_CONV_BLOCK(convert_fn, src_fmt, dst_fmt, cpuid_flags, attributes, pix_mult_count, desc)\
{ convert_fn, src_fmt, dst_fmt, cpuid_flags, attributes, pix_mult_count, desc }

// NNB resampling NON-SSE {FR, bt.601, bt.709) macros
#define		DECLARE_NNB_CONV_BLOCK(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(convert_fn_prefix##_nonsse, 			src_fmt, dst_fmt, 		CPUID_FEATURE_NONE, 		NNB_RESAMPLING, 1, desc_str_prefix " - NON-SSE - fast resampling")
#define		DECLARE_NNB_BT601_CONV_BLOCK(bt601_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt601_convert_fn_prefix##_nonsse, 		src_fmt, dst_fmt, 		CPUID_FEATURE_NONE, 		BT601_CONVERSION | NNB_RESAMPLING, 1, desc_str_prefix " - bt.601 - NON-SSE - fast resampling")
#define		DECLARE_NNB_BT709_CONV_BLOCK(bt709_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt709_convert_fn_prefix##_nonsse, 		src_fmt, dst_fmt, 		CPUID_FEATURE_NONE, 		BT709_CONVERSION | NNB_RESAMPLING, 1, desc_str_prefix " - bt.709 - NON-SSE - fast resampling")

// NNB resampling SSE2 {FR, bt.601, bt.709) macros
#define		DECLARE_NNB_SSE2_CONV_BLOCK(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(convert_fn_prefix##_sse2, 				src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2, 		NNB_RESAMPLING, 16, desc_str_prefix " - SSE2 - fast resampling")
#define		DECLARE_NNB_BT601_SSE2_CONV_BLOCK(bt601_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt601_convert_fn_prefix##_sse2, 			src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2, 		BT601_CONVERSION | NNB_RESAMPLING, 16, desc_str_prefix " - bt.601 - SSE2 - fast resampling")
#define		DECLARE_NNB_BT709_SSE2_CONV_BLOCK(bt709_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt709_convert_fn_prefix##_sse2, 			src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2, 		BT709_CONVERSION | NNB_RESAMPLING, 16, desc_str_prefix " - bt.709 - SSE2 - fast resampling")

// AVG resampling SSE2 {FR, bt.601, bt.709) macros
#define		DECLARE_AVG_SSE2_CONV_BLOCK(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(convert_fn_prefix##_sse2, 				src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2, 		DEFAULT_ATTRIBUTE, 16, desc_str_prefix " - SSE2")
#define		DECLARE_AVG_BT601_SSE2_CONV_BLOCK(bt601_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt601_convert_fn_prefix##_sse2, 		src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2, 		BT601_CONVERSION, 16, desc_str_prefix " - bt.601 - SSE2")
#define		DECLARE_AVG_BT709_SSE2_CONV_BLOCK(bt709_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt709_convert_fn_prefix##_sse2, 		src_fmt, dst_fmt,		CPUID_FEATURE_SSE2, 		BT709_CONVERSION, 16, desc_str_prefix " - bt.709 - SSE2")

// NNB resampling SSSE3 {FR, bt.601, bt.709) macros
#define		DECLARE_NNB_SSE2_SSSE3_CONV_BLOCK(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(convert_fn_prefix##_sse2_ssse3, 		src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3, 	NNB_RESAMPLING, 16, desc_str_prefix " - SSE2 / SSSE3 - fast resampling")
#define		DECLARE_NNB_BT601_SSE2_SSSE3_CONV_BLOCK(bt601_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt601_convert_fn_prefix##_sse2_ssse3, 	src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3, 	BT601_CONVERSION | NNB_RESAMPLING, 16, desc_str_prefix " - bt.601 - SSE2 / SSSE3 - fast resampling")
#define		DECLARE_NNB_BT709_SSE2_SSSE3_CONV_BLOCK(bt709_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt709_convert_fn_prefix##_sse2_ssse3, 	src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3, 	BT709_CONVERSION | NNB_RESAMPLING, 16, desc_str_prefix " - bt.709 - SSE2 / SSSE3 - fast resampling")

// AVG resampling SSSE3 {FR, bt.601, bt.709) macros
#define		DECLARE_AVG_SSE2_SSSE3_CONV_BLOCK(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(convert_fn_prefix##_sse2_ssse3, 		src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3, 	DEFAULT_ATTRIBUTE, 16, desc_str_prefix " - SSE2 / SSSE3")
#define		DECLARE_AVG_BT601_SSE2_SSSE3_CONV_BLOCK(bt601_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt601_convert_fn_prefix##_sse2_ssse3, 	src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3, 	BT601_CONVERSION, 16, desc_str_prefix " - bt.601 - SSE2 / SSSE3")
#define		DECLARE_AVG_BT709_SSE2_SSSE3_CONV_BLOCK(bt709_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_CONV_BLOCK(bt709_convert_fn_prefix##_sse2_ssse3, 	src_fmt, dst_fmt, 		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3, 	BT709_CONVERSION, 16, desc_str_prefix " - bt.709 - SSE2 / SSSE3")


/*
 * The following macro declares the following conversion blocks:
 * - Fast Nearest NeighBour resampling SSE2/SSSE3 full range
 * - Fast Nearest NeighBour resampling SSE2/SSSE3 bt.601
 * - Fast Nearest NeighBour resampling SSE2/SSSE3 bt.709
 *
 * - Average resampling SSE2/SSSE3 full range
 * - Average resampling SSE2/SSSE3 bt.601
 * - Average resampling SSE2/SSSE3 bt.709
 *
 * - Fast Nearest NeighBour resampling SSE2 full range
 * - Fast Nearest NeighBour resampling SSE2 bt.601
 * - Fast Nearest NeighBour resampling SSE2 bt.709
 *
 * - Average resampling SSE2 full range
 * - Average resampling SSE2 bt.601
 * - Average resampling SSE2 bt.709
 */
#define		DECLARE_CONV_BLOCKS(convert_fn_prefix, resample_n_convert_fn_prefix, non_sse_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix)\
DECLARE_NNB_SSE2_SSSE3_CONV_BLOCK		(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_BT601_SSE2_SSSE3_CONV_BLOCK	(convert_fn_prefix##_bt601, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_BT709_SSE2_SSSE3_CONV_BLOCK	(convert_fn_prefix##_bt709, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_AVG_SSE2_SSSE3_CONV_BLOCK		(resample_n_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_AVG_BT601_SSE2_SSSE3_CONV_BLOCK	(resample_n_convert_fn_prefix##_bt601, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_AVG_BT709_SSE2_SSSE3_CONV_BLOCK	(resample_n_convert_fn_prefix##_bt709, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_SSE2_CONV_BLOCK				(convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_BT601_SSE2_CONV_BLOCK		(convert_fn_prefix##_bt601, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_BT709_SSE2_CONV_BLOCK		(convert_fn_prefix##_bt709, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_AVG_SSE2_CONV_BLOCK				(resample_n_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_AVG_BT601_SSE2_CONV_BLOCK		(resample_n_convert_fn_prefix##_bt601, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_AVG_BT709_SSE2_CONV_BLOCK		(resample_n_convert_fn_prefix##_bt709, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_CONV_BLOCK					(non_sse_convert_fn_prefix, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_BT601_CONV_BLOCK			(non_sse_convert_fn_prefix##_bt601, src_fmt, dst_fmt, desc_str_prefix),\
DECLARE_NNB_BT709_CONV_BLOCK			(non_sse_convert_fn_prefix##_bt709, src_fmt, dst_fmt, desc_str_prefix)



/*
 * In this array, conversion blocks for a given pair of source and destination
 * formats must be sorted: fastest first, slowest last !!!
 */
const struct  ConversionBlock		conversion_blocks[] = {

	/*
	 *	S O U R C E   F O R M A T :
	 *	
	 *	A R G B
	 *
	 */
	// ARGB to YUYV
	{
		convert_argb_to_yuyv_sse2_ssse3,
		PixFcARGB, PixFcYUYV,
		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3,
		NNB_RESAMPLING,
		16,
		"ARGB to YUYV - SSE2 / SSSE3- fast downsampling",
	},
	{
		convert_argb_to_yuyv_sse2,
		PixFcARGB, PixFcYUYV,
		CPUID_FEATURE_SSE2,
		NNB_RESAMPLING,
		16,
		"ARGB to YUYV - SSE2 - fast downsampling",
	},
	{
		convert2_argb_to_yuyv_sse2,
		PixFcARGB, PixFcYUYV,
		CPUID_FEATURE_SSE2,
		NNB_RESAMPLING,
		16,
		"ARGB to YUYV (2)- SSE2 - fast downsampling",
	},
	{
		downsample_n_convert_argb_to_yuyv_sse2_ssse3,
		PixFcARGB, PixFcYUYV,
		CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3,
		DEFAULT_ATTRIBUTE,
		16,
		"ARGB to YUYV - SSE2 / SSSE3",
		
	},
	{
		downsample_n_convert_argb_to_yuyv_sse2,
		PixFcARGB, PixFcYUYV,
		CPUID_FEATURE_SSE2,
		DEFAULT_ATTRIBUTE,
		16,
		"ARGB to YUYV - SSE2",
		
	},
	{
		downsample_n_convert2_argb_to_yuyv_sse2,
		PixFcARGB, PixFcYUYV,
		CPUID_FEATURE_SSE2,
		DEFAULT_ATTRIBUTE,
		16,
		"ARGB to YUYV (2)- SSE2",

	},

	// YUYV to ARGB
	DECLARE_CONV_BLOCKS(convert_yuyv_to_argb, upsample_n_convert_yuyv_to_argb, convert_yuyv_to_any_rgb, PixFcYUYV, PixFcARGB, "YUYV to ARGB"),

	// YUYV to BGRA
	DECLARE_CONV_BLOCKS(convert_yuyv_to_bgra, upsample_n_convert_yuyv_to_bgra, convert_yuyv_to_any_rgb, PixFcYUYV, PixFcBGRA, "YUYV to BGRA"),

	// YUYV to RGB24
	DECLARE_CONV_BLOCKS(convert_yuyv_to_rgb24, upsample_n_convert_yuyv_to_rgb24, convert_yuyv_to_any_rgb, PixFcYUYV, PixFcRGB24, "YUYV to RGB24"),

	// YUYV to BGR24
	DECLARE_CONV_BLOCKS(convert_yuyv_to_bgr24, upsample_n_convert_yuyv_to_bgr24, convert_yuyv_to_any_rgb, PixFcYUYV, PixFcBGR24, "YUYV to BGR24"),

	// UYVY to ARGB
	DECLARE_CONV_BLOCKS(convert_uyvy_to_argb, upsample_n_convert_uyvy_to_argb, convert_uyvy_to_any_rgb, PixFcUYVY, PixFcARGB, "UYVY to ARGB"),

	// UYVY to BGRA
	DECLARE_CONV_BLOCKS(convert_uyvy_to_bgra, upsample_n_convert_uyvy_to_bgra, convert_uyvy_to_any_rgb, PixFcUYVY, PixFcBGRA, "UYVY to BGRA"),

	// UYVY to RGB24
	DECLARE_CONV_BLOCKS(convert_uyvy_to_rgb24, upsample_n_convert_uyvy_to_rgb24, convert_uyvy_to_any_rgb, PixFcUYVY, PixFcRGB24, "UYVY to RGB24"),

	// UYVY to BGR24
	DECLARE_CONV_BLOCKS(convert_uyvy_to_bgr24, upsample_n_convert_uyvy_to_bgr24, convert_uyvy_to_any_rgb, PixFcUYVY, PixFcBGR24, "UYVY to BGR24"),
};

const uint32_t		conversion_blocks_count = sizeof(conversion_blocks) / sizeof(conversion_blocks[0]);


