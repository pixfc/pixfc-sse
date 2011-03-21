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

#include <emmintrin.h>

#include "pixfc-sse.h"
#include "platform_util.h"

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
		 * an image in this format:
		 * (width * height) * byte_per_pix_num / byte_per_pix_denom
		 * - The result cannot have a decimal part, and
		 * - The buffer size MUST BE MULTIPLE OF 16 for most conversion blocks
		 *   to work.
		 */
        uint32_t				bytes_per_pix_num;
        uint32_t				bytes_per_pix_denom;

		
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
 * This macro expands to the size in bytes of an image of the given width and 
 * height in the given pixel format (of type PixFcPixelFormat).
 */
#define IMG_SIZE(pixFcPixFormat, width, height)	\
	( ((pixFcPixFormat)<0 || (pixFcPixFormat>=PixFcFormatCount)) ? 0 :\
		(width * height * pixfmt_descriptions[(pixFcPixFormat)].bytes_per_pix_num\
		 / pixfmt_descriptions[(pixFcPixFormat)].bytes_per_pix_denom)\
	)

	

#endif 		// PIXFMT_DESCRIPTIONS_H_

