/*
 * test-utils.h
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

#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#include <stdio.h>

#include "pixfc-sse.h"

// printf helper which prints file name and line number
#define dprint(fmt, ...) 	do { fprintf (stderr, "[ %s:%-3d ] " fmt,\
								__FILE__, __LINE__, ## __VA_ARGS__);\
								fflush(stderr); } while(0)

/*
 * Look for PixFcPixelFormat enum entry matching the given pixel format
 * passed as a string. (Uses pixfmt_descriptions array to look for match)
 * Return the matching PixFcPixelFormat enum entry or 'PixFcFormatCount'
 * if not found.
 */
PixFcPixelFormat	find_matching_pixel_format(char *format_string);


/*
 * Validate the image dimensions for a given image format:
 * - Make sure the number of pixels is multiple of 16 (many SSE conversion blocks have this req.)
 * - Make sure buffer size is an even number.
 * Return 0 if OK, -1 otherwise
 */
uint32_t	validate_image_dimensions(PixFcPixelFormat fmt, uint32_t width, uint32_t height);

/*
 * Validate the image dimensions (with function above) and allocate a 16-byte aligned memory buffer
 * large enough to contain an image in the give pixel format. The buffer can be released with free().
 * Returns 0 if OK, -1 otherwise
 */
uint32_t	allocate_buffer(PixFcPixelFormat fmt, uint32_t width, uint32_t height, void **buffer);

/*
 * Fill image buffer with the fill found in the struct PixelFormatDescription (see pifmt_descriptions.h)
 */
void		fill_image(PixFcPixelFormat fmt, uint32_t buffer_size, void *buffer);

/*
 * Allocate a buffer large enough for the given format, width and height, and 
 * fill it with the contents of the given file
 * Returns 0 if OK, -1 otherwise
 */
int32_t 	get_buffer_from_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void **buffer);

/*
 * Write pixels from rgb_buffer in any RGB formats to A PPM file
 * rgb_buf_size is the size of the buffer in bytes.
 */
void 		write_anyrgb_buffer_to_ppm_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void * rgb_buffer);


#endif /* _TEST_UTILS_H_ */
