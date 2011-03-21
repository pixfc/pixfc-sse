/*
 * conversion_routines_from_uyvy.h
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

#ifndef CONVERSION_ROUTINES_FROM_UYVY_H_
#define CONVERSION_ROUTINES_FROM_UYVY_H_

#include "pixfc-sse.h"

// UYVY to ARGB			SSE2 SSSE3
void		convert_uyvy_to_argb_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_argb_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// UYVY to ARGB			SSE2
void		convert_uyvy_to_argb_sse2(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_argb_no_interpolation_sse2(const struct PixFcSSE *, void*, void*);

// UYVY to BGRA			SSE2 SSSE3
void		convert_uyvy_to_bgra_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_bgra_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// UYVY to BGRA			SSE2
void		convert_uyvy_to_bgra_sse2(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_bgra_no_interpolation_sse2(const struct PixFcSSE *, void*, void*);

// UYVY to RGB24		SSE2 SSSE3
void		convert_uyvy_to_rgb24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_rgb24_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// UYVY to RGB24		SSE2 slow repacking
void		convert_uyvy_to_rgb24_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_rgb24_no_interpolation_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);


// UYVY to BGR24		SSE2 SSSE3
void		convert_uyvy_to_bgr24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_bgr24_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// UYVY to BGR24		SSE2 slow repacking
void		convert_uyvy_to_bgr24_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_bgr24_no_interpolation_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);



// UYVY to anyRGB		NON SSE
void 		convert_uyvy_to_any_rgb_nonsse(const struct PixFcSSE* conv, void* in, void* out);




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

// UYVY to ARGB			SSE2 SSSE3
void		convert_uyvy_to_argb_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_argb_bt601_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// UYVY to ARGB			SSE2
void		convert_uyvy_to_argb_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_argb_bt601_no_interpolation_sse2(const struct PixFcSSE *, void*, void*);

// UYVY to BGRA			SSE2 SSSE3
void		convert_uyvy_to_bgra_bt601_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_bgra_bt601_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// UYVY to BGRA			SSE2
void		convert_uyvy_to_bgra_bt601_sse2(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_bgra_bt601_no_interpolation_sse2(const struct PixFcSSE *, void*, void*);

// UYVY to RGB24		SSE2 SSSE3
void		convert_uyvy_to_rgb24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_rgb24_bt601_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// UYVY to RGB24		SSE2 slow repacking
void		convert_uyvy_to_rgb24_bt601_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_rgb24_bt601_no_interpolation_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);


// UYVY to BGR24		SSE2 SSSE3
void		convert_uyvy_to_bgr24_bt601_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_bgr24_bt601_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// UYVY to BGR24		SSE2 slow repacking
void		convert_uyvy_to_bgr24_bt601_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_bgr24_bt601_no_interpolation_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);



// UYVY to anyRGB		NON SSE
void 		convert_uyvy_to_any_rgb_bt601_nonsse(const struct PixFcSSE* conv, void* in, void* out);




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

// UYVY to ARGB			SSE2 SSSE3
void		convert_uyvy_to_argb_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_argb_bt709_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// UYVY to ARGB			SSE2
void		convert_uyvy_to_argb_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_argb_bt709_no_interpolation_sse2(const struct PixFcSSE *, void*, void*);

// UYVY to BGRA			SSE2 SSSE3
void		convert_uyvy_to_bgra_bt709_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_bgra_bt709_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// UYVY to BGRA			SSE2
void		convert_uyvy_to_bgra_bt709_sse2(const struct PixFcSSE *, void*, void*);
void		convert_uyvy_to_bgra_bt709_no_interpolation_sse2(const struct PixFcSSE *, void*, void*);

// UYVY to RGB24		SSE2 SSSE3
void		convert_uyvy_to_rgb24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_rgb24_bt709_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// UYVY to RGB24		SSE2 slow repacking
void		convert_uyvy_to_rgb24_bt709_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_rgb24_bt709_no_interpolation_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);


// UYVY to BGR24		SSE2 SSSE3
void		convert_uyvy_to_bgr24_bt709_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_bgr24_bt709_no_interpolation_sse2_ssse3(const struct PixFcSSE *, void *, void *);
// UYVY to BGR24		SSE2 slow repacking
void		convert_uyvy_to_bgr24_bt709_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);
void		convert_uyvy_to_bgr24_bt709_no_interpolation_sse2_slowrepacking(const struct PixFcSSE *, void *, void *);



// UYVY to anyRGB		NON SSE
void 		convert_uyvy_to_any_rgb_bt709_nonsse(const struct PixFcSSE* conv, void* in, void* out);

#endif 		// CONVERSION_ROUTINES_FROM_UYVY_H_

