/*
 * conversion_routines_from_r210.h
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
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


#ifndef CONVERSION_ROUTINES_FROM_R210_H_
#define CONVERSION_ROUTINES_FROM_R210_H_


#include "pixfc-sse.h"


// r210 to YUYV			SSE2 SSSE3
void		convert_r210_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		downsample_n_convert_r210_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void *, void *);




/*
 *
 * 		R 2 1 0
 *
 * 		T O
 *
 * 		R G B 3 2  /  2 4
 *
 */
void		convert_r210_to_argb_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_r210_to_bgra_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_r210_to_rgb24_sse2_ssse3(const struct PixFcSSE *, void *, void *);
void		convert_r210_to_bgr24_sse2_ssse3(const struct PixFcSSE *, void *, void *);

void		convert_10bit_rgb_to_any_rgb_nonsse(const struct PixFcSSE *, void *, void *);


#endif 	// CONVERSION_ROUTINES_FROM_R210_H_
