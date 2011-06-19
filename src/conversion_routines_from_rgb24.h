/*
 * conversion_routines_from_rgb24.h
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


#ifndef CONVERSION_ROUTINES_FROM_RGB24_H_
#define CONVERSION_ROUTINES_FROM_RGB24_H_


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

// RGB24 to YUYV			SSE2 SSSE3
void		convert_rgb24_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// RGB24 to YUYV			SSE2
void		convert_rgb24_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);


// RGB24 to UYVY			SSE2 SSSE3
void		convert_rgb24_to_uyvy_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_uyvy_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// RGB24 to UYVY			SSE2
void		convert_rgb24_to_uyvy_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_uyvy_sse2(const struct PixFcSSE *, void*, void*);

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

// RGB24 to YUYV		SSE2 SSSE3
void		convert_rgb24_to_yuyv_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_yuyv_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// RGB24 to YUYV			SSE2
void		convert_rgb24_to_yuyv_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_yuyv_bt601_sse2(const struct PixFcSSE *, void*, void*);



// RGB24 to UYVY		SSE2 SSSE3
void		convert_rgb24_to_uyvy_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_uyvy_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// RGB24 to UYVY			SSE2
void		convert_rgb24_to_uyvy_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_uyvy_bt601_sse2(const struct PixFcSSE *, void*, void*);

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

// RGB24 to YUYV			SSE2 SSSE3
void		convert_rgb24_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// RGB24 to YUYV			SSE2
void		convert_rgb24_to_yuyv_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_yuyv_bt709_sse2(const struct PixFcSSE *, void*, void*);



// RGB24 to UYVY			SSE2 SSSE3
void		convert_rgb24_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// RGB24 to UYVY			SSE2
void		convert_rgb24_to_uyvy_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_rgb24_to_uyvy_bt709_sse2(const struct PixFcSSE *, void*, void*);



// RGB to YUV422		NON SSE
void 		convert_rgb_to_yuv422_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv422_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv422_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);



#endif /* CONVERSION_ROUTINES_FROM_RGB24_H_ */
