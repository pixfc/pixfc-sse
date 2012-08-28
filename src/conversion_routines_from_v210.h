/*
 * conversion_routines_from_v210.h
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
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

#ifndef CONVERSION_ROUTINES_FROM_V210_H_
#define CONVERSION_ROUTINES_FROM_V210_H_

#include "pixfc-sse.h"

/*
 *
 *
 *	F U L L   R A N G E   
 *
 *  C O N V E R S I O N S
 *
 *
 */

// V210 to ARGB			SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_argb_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_argb_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to ARGB  		SSE2 SSSE3
void		upsample_n_convert_v210_to_argb_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_argb_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to BGRA 		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_bgra_sse2_ssse3_sse41(const struct PixFcSSE *, void* , void*);
void		convert_v210_to_bgra_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to BGRA 		SSE2 SSSE3
void		upsample_n_convert_v210_to_bgra_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_v210_to_bgra_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to RGB24		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_rgb24_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
void		convert_v210_to_rgb24_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to RGB24		SSE2 SSSE3
void		upsample_n_convert_v210_to_rgb24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_rgb24_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to BGR24		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_bgr24_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_bgr24_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
// V210 to BGR24		SSE2 SSSE3
void		upsample_n_convert_v210_to_bgr24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_bgr24_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to R210			SSE2 SSSE3
void		convert_v210_to_r210_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		upsample_n_convert_v210_to_r210_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to RGB32/24		NON SSE integer
void 		convert_v210_to_any_rgb_nonsse(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_rgb_nonsse(const struct PixFcSSE* , void* , void*);
// V210 to RGB32/24		NON SSE float
void 		convert_v210_to_any_rgb_nonsse_float(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_rgb_nonsse_float(const struct PixFcSSE* , void* , void*);


// V210 to 10bitRGB		NON SSE integer
void 		convert_v210_to_any_10bit_rgb_nonsse(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_10bit_rgb_nonsse(const struct PixFcSSE* , void* , void*);
// V210 to 10bitRGB		NON SSE float
void 		convert_v210_to_any_10bit_rgb_nonsse_float(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_10bit_rgb_nonsse_float(const struct PixFcSSE* , void* , void*);





/*
 *
 *
 *	S T A N D A R D   D E F I N I T I O N   
 *
 *  C O N V E R S I O N S
 *
 *  ( BT 601 )
 *
 *
 */

// V210 to ARGB			SSE2 SSS3 SSE41
void		upsample_n_convert_v210_to_argb_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_argb_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to ARGB  		SSE2 SSSE3
void		upsample_n_convert_v210_to_argb_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_argb_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to BGRA 		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_bgra_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void* , void*);
void		convert_v210_to_bgra_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to BGRA 		SSE2 SSSE3
void		upsample_n_convert_v210_to_bgra_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_v210_to_bgra_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to RGB24		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_rgb24_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
void		convert_v210_to_rgb24_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to RGB24		SSE2 SSSE3
void		upsample_n_convert_v210_to_rgb24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_rgb24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to BGR24		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_bgr24_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_bgr24_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
// V210 to BGR24		SSE2 SSSE3
void		upsample_n_convert_v210_to_bgr24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_bgr24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to R210			SSE2 SSSE3
void		convert_v210_to_r210_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		upsample_n_convert_v210_to_r210_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to RGB32/24		NON SSE integer
void 		convert_v210_to_any_rgb_bt601_nonsse(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_rgb_bt601_nonsse(const struct PixFcSSE* , void* , void*);
// V210 to RGB32/24		NON SSE float
void 		convert_v210_to_any_rgb_bt601_nonsse_float(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_rgb_bt601_nonsse_float(const struct PixFcSSE* , void* , void*);


// V210 to 10bitRGB		NON SSE integer
void 		convert_v210_to_any_10bit_rgb_bt601_nonsse(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_10bit_rgb_bt601_nonsse(const struct PixFcSSE* , void* , void*);
// V210 to 10bitRGB		NON SSE float
void 		convert_v210_to_any_10bit_rgb_bt601_nonsse_float(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_10bit_rgb_bt601_nonsse_float(const struct PixFcSSE* , void* , void*);



/*
 *
 *
 *	H I G H   D E F I N I T I O N
 *
 *  C O N V E R S I O N S
 *
 *  ( BT 709 )
 *
 *
 */

// V210 to ARGB			SSE2 SSS3 SSE41
void		upsample_n_convert_v210_to_argb_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_argb_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to ARGB  		SSE2 SSSE3
void		upsample_n_convert_v210_to_argb_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_argb_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to BGRA 		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_bgra_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void* , void*);
void		convert_v210_to_bgra_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to BGRA 		SSE2 SSSE3
void		upsample_n_convert_v210_to_bgra_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_v210_to_bgra_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to RGB24		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_rgb24_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
void		convert_v210_to_rgb24_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
// V210 to RGB24		SSE2 SSSE3
void		upsample_n_convert_v210_to_rgb24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_rgb24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to BGR24		SSE2 SSSE3 SSE41
void		upsample_n_convert_v210_to_bgr24_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_bgr24_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
// V210 to BGR24		SSE2 SSSE3
void		upsample_n_convert_v210_to_bgr24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_v210_to_bgr24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to R210			SSE2 SSSE3
void		convert_v210_to_r210_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		upsample_n_convert_v210_to_r210_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);


// V210 to RGB32/24		NON SSE integer
void 		convert_v210_to_any_rgb_bt709_nonsse(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_rgb_bt709_nonsse(const struct PixFcSSE* , void* , void*);
// V210 to RGB32/24		NON SSE float
void 		convert_v210_to_any_rgb_bt709_nonsse_float(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_rgb_bt709_nonsse_float(const struct PixFcSSE* , void* , void*);


// V210 to 10bitRGB		NON SSE integer
void 		convert_v210_to_any_10bit_rgb_bt709_nonsse(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_10bit_rgb_bt709_nonsse(const struct PixFcSSE* , void* , void*);
// V210 to 10bitRGB		NON SSE float
void 		convert_v210_to_any_10bit_rgb_bt709_nonsse_float(const struct PixFcSSE* , void* , void*);
void 		upsample_n_convert_v210_to_any_10bit_rgb_bt709_nonsse_float(const struct PixFcSSE* , void* , void*);



/*
 *
 *	V 2 1 0
 *
 *	T O
 *
 *	Y U V 4 2 2 I
 *
 *
 */
// V210 to YUYV
void		convert_v210_to_yuyv_sse2_ssse3_sse41(const struct PixFcSSE* , void* , void*);
void		convert_v210_to_yuyv_sse2_ssse3(const struct PixFcSSE* , void* , void*);

// V210 to UYVY
void		convert_v210_to_uyvy_sse2_ssse3_sse41(const struct PixFcSSE* , void* , void*);
void		convert_v210_to_uyvy_sse2_ssse3(const struct PixFcSSE* , void* , void*);

void		convert_v210_to_yuv422i_nonsse(const struct PixFcSSE* , void* , void*);



#endif 		// CONVERSION_ROUTINES_FROM_V210_H_

