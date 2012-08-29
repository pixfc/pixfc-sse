/*
 * conversion_routines_from_argb.h
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

#ifndef CONVERSION_ROUTINES_FROM_ARGB_H_
#define CONVERSION_ROUTINES_FROM_ARGB_H_


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

// ARGB to YUYV			SSE2 SSSE3
void		convert_argb_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to UYVY			SSE2 SSSE3
void		convert_argb_to_uyvy_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_uyvy_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to UYVY			SSE2
void		convert_argb_to_uyvy_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_uyvy_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to YUV422P			SSE2 SSSE3
void		convert_argb_to_yuv422p_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuv422p_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to YUV422P			SSE2
void		convert_argb_to_yuv422p_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuv422p_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to YUV420P			SSE2 SSSE3
void		convert_argb_to_yuv420p_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to YUV420P			SSE2
void		convert_argb_to_yuv420p_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to V210			SSE2 SSSE3
void		convert_argb_to_v210_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_v210_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
// ARGB to V210			SSE2
void		convert_argb_to_v210_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_v210_sse2_ssse3(const struct PixFcSSE *, void*, void*);

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

// ARGB to YUYV		SSE2 SSSE3
void		convert_argb_to_yuyv_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_bt601_sse2(const struct PixFcSSE *, void*, void*);



// ARGB to UYVY		SSE2 SSSE3
void		convert_argb_to_uyvy_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_uyvy_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// ARGB to UYVY			SSE2
void		convert_argb_to_uyvy_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_uyvy_bt601_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to YUV422P		SSE2 SSSE3
void		convert_argb_to_yuv422p_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuv422p_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// ARGB to YUV422P			SSE2
void		convert_argb_to_yuv422p_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuv422p_bt601_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to YUV420P			SSE2 SSSE3
void		convert_argb_to_yuv420p_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to YUV420P			SSE2
void		convert_argb_to_yuv420p_bt601_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to V210			SSE2 SSSE3
void		convert_argb_to_v210_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_v210_bt601_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
// ARGB to V210			SSE2
void		convert_argb_to_v210_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_v210_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);


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

// ARGB to YUYV			SSE2 SSSE3
void		convert_argb_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_bt709_sse2(const struct PixFcSSE *, void*, void*);



// ARGB to UYVY			SSE2 SSSE3
void		convert_argb_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_uyvy_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// ARGB to UYVY			SSE2
void		convert_argb_to_uyvy_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_uyvy_bt709_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to YUV422P			SSE2 SSSE3
void		convert_argb_to_yuv422p_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuv422p_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);

// ARGB to YUV422P			SSE2
void		convert_argb_to_yuv422p_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuv422p_bt709_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to YUV420P			SSE2 SSSE3
void		convert_argb_to_yuv420p_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to YUV420P			SSE2
void		convert_argb_to_yuv420p_bt709_sse2(const struct PixFcSSE *, void*, void*);


// ARGB to V210			SSE2 SSSE3
void		convert_argb_to_v210_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_v210_bt709_sse2_ssse3_sse41(const struct PixFcSSE *, void*, void*);
// ARGB to V210			SSE2
void		convert_argb_to_v210_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_v210_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);




// RGB to YUV422		NON SSE integer
void 		convert_rgb_to_yuv422_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_yuv422_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv422_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_yuv422_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv422_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_yuv422_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);

// RGB to YUV422		NON SSE float
void 		convert_rgb_to_yuv422_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_yuv422_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv422_bt601_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_yuv422_bt601_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv422_bt709_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_yuv422_bt709_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);




// RGB to V210		NON SSE integer
void 		convert_rgb_to_v210_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_v210_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_v210_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_v210_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_v210_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_v210_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);

// RGB to V210		NON SSE float
void 		convert_rgb_to_v210_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_v210_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_v210_bt601_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_v210_bt601_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_v210_bt709_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		downsample_n_convert_rgb_to_v210_bt709_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);




// RGB to YUV420		NON SSE integer
void 		convert_rgb_to_yuv420_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv420_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv420_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);

// RGB to YUV420		NON SSE float
void 		convert_rgb_to_yuv420_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv420_bt601_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);
void 		convert_rgb_to_yuv420_bt709_nonsse_float(const struct PixFcSSE* conv, void* in, void* out);



/*
 *
 * 		A R G B
 *
 * 		T O
 *
 * 		R 2 1 0
 *
 */
void		convert_argb_to_r210_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_rgb_to_10bit_rgb_nonsse(const struct PixFcSSE *, void *, void *);



#endif /* CONVERSION_ROUTINES_FROM_ARGB_H_ */
