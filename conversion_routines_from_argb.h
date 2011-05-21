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

/*
 * Original implementation
 */
// ARGB to YUYV			SSE2 SSSE3
void		convert_argb_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_sse2_ssse3(const struct PixFcSSE *, void*, void*);
// ARGB to YUYV			SSE2
void		convert_argb_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert_argb_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);



/*
 * Alternate faster implementation (to be tested !!)
 */
void		convert2_argb_to_yuyv_sse2(const struct PixFcSSE *, void*, void*);
void		downsample_n_convert2_argb_to_yuyv_sse2(const struct PixFcSSE *, void* , void*);

#endif /* CONVERSION_ROUTINES_FROM_ARGB_H_ */
