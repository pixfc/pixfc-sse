/*
 * conversion_routines_from_yuyv.h
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

#ifndef CONVERSION_ROUTINES_FROM_YUYV_H_
#define CONVERSION_ROUTINES_FROM_YUYV_H_

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

// YUYV to ARGB			SSE2 SSS3
void		upsample_n_convert_yuyv_to_argb_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_argb_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to ARGB  		SSE2
void		upsample_n_convert_yuyv_to_argb_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_argb_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to BGRA 		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_bgra_sse2_ssse3(const struct PixFcSSE *, void* , void*);
void		convert_yuyv_to_bgra_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to BGRA 		SSE2
void		upsample_n_convert_yuyv_to_bgra_sse2(const struct PixFcSSE *, void*, void*);
void		convert_yuyv_to_bgra_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to RGB24		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_rgb24_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_yuyv_to_rgb24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to RGB24		SSE2 slow repacking
void		upsample_n_convert_yuyv_to_rgb24_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_rgb24_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to BGR24		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_bgr24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_bgr24_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// YUYV to BGR24		SSE2 slow repacking
void		upsample_n_convert_yuyv_to_bgr24_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_bgr24_sse2(const struct PixFcSSE *, void *, void *);

// YUYV to R210			SSE2 SSSE3
void		upsample_n_convert_yuyv_to_r210_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_r210_sse2_ssse3(const struct PixFcSSE *, void *, void *);

// anyYUV422I to anyRGB		NON SSE
void 		convert_yuv422i_to_any_rgb_nonsse(const struct PixFcSSE*, void*, void*);
void		upsample_n_convert_yuv422i_to_any_rgb_nonsse(const struct PixFcSSE*, void*, void*);






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

// YUYV to ARGB			SSE2 SSS3
void		upsample_n_convert_yuyv_to_argb_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_argb_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to ARGB  		SSE2
void		upsample_n_convert_yuyv_to_argb_bt601_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_argb_bt601_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to BGRA 		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_bgra_bt601_sse2_ssse3(const struct PixFcSSE *, void* , void*);
void		convert_yuyv_to_bgra_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to BGRA 		SSE2
void		upsample_n_convert_yuyv_to_bgra_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		convert_yuyv_to_bgra_bt601_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to RGB24		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_rgb24_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_yuyv_to_rgb24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to RGB24		SSE2 slow repacking
void		upsample_n_convert_yuyv_to_rgb24_bt601_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_rgb24_bt601_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to BGR24		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_bgr24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_bgr24_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// YUYV to BGR24		SSE2 slow repacking
void		upsample_n_convert_yuyv_to_bgr24_bt601_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_bgr24_bt601_sse2(const struct PixFcSSE *, void *, void *);


// anyYUV422I to anyRGB		NON SSE
void 		convert_yuv422i_to_any_rgb_bt601_nonsse(const struct PixFcSSE*, void*, void*);
void		upsample_n_convert_yuv422i_to_any_rgb_bt601_nonsse(const struct PixFcSSE*, void*, void*);




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

// YUYV to ARGB			SSE2 SSS3
void		upsample_n_convert_yuyv_to_argb_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_argb_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to ARGB  		SSE2
void		upsample_n_convert_yuyv_to_argb_bt709_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_argb_bt709_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to BGRA 		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_bgra_bt709_sse2_ssse3(const struct PixFcSSE *, void* , void*);
void		convert_yuyv_to_bgra_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to BGRA 		SSE2
void		upsample_n_convert_yuyv_to_bgra_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		convert_yuyv_to_bgra_bt709_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to RGB24		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_rgb24_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_yuyv_to_rgb24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// YUYV to RGB24		SSE2 slow repacking
void		upsample_n_convert_yuyv_to_rgb24_bt709_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_rgb24_bt709_sse2(const struct PixFcSSE *, void *, void *);


// YUYV to BGR24		SSE2 SSSE3
void		upsample_n_convert_yuyv_to_bgr24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_bgr24_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// YUYV to BGR24		SSE2 slow repacking
void		upsample_n_convert_yuyv_to_bgr24_bt709_sse2(const struct PixFcSSE *, void *, void *);
void		convert_yuyv_to_bgr24_bt709_sse2(const struct PixFcSSE *, void *, void *);


// anyYUV422I to anyRGB		NON SSE
void 		convert_yuv422i_to_any_rgb_bt709_nonsse(const struct PixFcSSE* , void* , void* );
void		upsample_n_convert_yuv422i_to_any_rgb_bt709_nonsse(const struct PixFcSSE*, void*, void*);



/*
 *  	Y U Y V
 *
 * 		T O
 *
 * 		Y U V 4 2 2 P
 */
void		convert_yuyv_to_yuv422p_sse2(const struct PixFcSSE*, void*, void*);
void		convert_yuyv_to_yuv422p_sse2_ssse3(const struct PixFcSSE*, void*, void*);
void		convert_yuv422i_to_yuv422p_nonsse(const struct PixFcSSE*, void*, void*);


/*
 *
 *	Y U Y V
 *
 *	T O
 *
 *	V 2 1 0
 *
 *
 */
// YUYV to V210
void		convert_yuyv_to_v210_sse2_ssse3_sse41(const struct PixFcSSE*, void* , void*);
void		convert_yuyv_to_v210_sse2_ssse3(const struct PixFcSSE* , void* , void*);

void		convert_yuv422i_to_v210_nonsse(const struct PixFcSSE* , void* , void*);


#endif 		// CONVERSION_ROUTINES_FROM_YUYV_H_

