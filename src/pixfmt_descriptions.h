/*
 * pixfmt_descriptions.h
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

#ifndef PIXFMT_DESCRIPTIONS_H_
#define PIXFMT_DESCRIPTIONS_H_


#include "pixfc-sse.h"
#include "platform_util.h"

#ifdef __INTEL_CPU__
#include <emmintrin.h>
#endif
/*
 * A structure grouping together interesting bits of info about
 * a pixel format:
 * - bytes_per_pix_num & bytes_per_pix_denom are used to calculate 
 *   the total size (in bytes) of an image in a particular format
 *   given its width and height.
 * - fill_patterns[] is an array of 16-byte vectors containing a fill
 *   pattern which can be used to initialise a buffer with valid data.
 */
typedef struct {
		// The format this structure describes
        PixFcPixelFormat		pixFcFormat;

        /*
		 * To find out the total size of an array large enough to store
		 * an image in this format, use the IMG_SIZE() macro below.
		 */
        uint32_t				bytes_per_pix_num;
        uint32_t				bytes_per_pix_denom;

        // Is this format planar (1) or interleaved (0) ?
        uint8_t					is_planar;

        uint8_t					width_multiple;
        uint8_t					height_multiple;	// 1 if not applicable

		// Each line must have a multiple of this number of pixels,
		// or 1 if not applicable. This value is used to calculate
		// the size in bytes of each line:
		// (width + (row_pixel_multiple - 1) ) / row_pixel_multiple) * bytes_per_pix_num / bytes_per_pix_den
		uint8_t					row_pixel_multiple;
		
		/*
		 * An array of 16-byte vectors containing the fill pattern.
		 * The array contains 'fill_patterns_count' elements.
		 */
		#define					MAX_FILL_PATTERNS	6
		CONST_M128I_ARRAY		(fill_patterns, MAX_FILL_PATTERNS);
		uint32_t				fill_patterns_count;


        // A printable name for this format
        const char  *           name;
} PixelFormatDescription ;

/*
 * This array contains descriptions for all image formats supported
 * by PixFcSSE (see definition of PixFcPixelFormat in pixfc-sse.h)
 * Given a PixFcPixelFormat fmt, its description can be obtained with:
 * pixfmt_descriptions[fmt] .
 * There are pixfmt_descriptions_count elements in this array.
 */
extern const PixelFormatDescription		pixfmt_descriptions[];
extern const uint32_t					pixfmt_descriptions_count;

/*
 * This macro expands to the size in bytes of a single line in an image
 * of the given width and pixel format.
 */
#define ROW_SIZE(fmt, width) \
	( ((fmt)<0 || ((fmt)>=PixFcFormatCount)) ? 0 :\
		((width) + pixfmt_descriptions[(fmt)].row_pixel_multiple - 1) / pixfmt_descriptions[(fmt)].row_pixel_multiple * \
			pixfmt_descriptions[(fmt)].bytes_per_pix_num * pixfmt_descriptions[(fmt)].row_pixel_multiple / pixfmt_descriptions[(fmt)].bytes_per_pix_denom\
	)

#define PADDING_SIZE(fmt, width) \
	(\
			ROW_SIZE((fmt), (width)) - (width) * pixfmt_descriptions[(fmt)].bytes_per_pix_num / pixfmt_descriptions[(fmt)].bytes_per_pix_denom\
	)

/*
 * This macro expands to the size in bytes of an image of the given width and 
 * height in the given pixel format (of type PixFcPixelFormat).
 */
#define IMG_SIZE(fmt, width, height) (ROW_SIZE(fmt, width) * (height))

	

#endif 		// PIXFMT_DESCRIPTIONS_H_

