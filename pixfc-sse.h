/*
 * pixfc.h
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

#ifndef _PIXFC_SSE_H_
#define _PIXFC_SSE_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdint.h>

// Forward declaration
struct PixFcSSE;


/*
 * List of known pixel formats
 *
 * (see pixfmt_description.h for a struct containing more
 * info about each format, including name and fill pattern)
 */
typedef enum {
	// YUV formats
	PixFcYUYV = 0,
	PixFcUYVY,

	// RGB formats
	PixFcARGB,	// 32-bit ARGB
	PixFcBGRA,	// 32-bit BGRA
	PixFcRGB24,	// 24-bit RGB
	PixFcBGR24,	// 24-bit BGR

	//
	PixFcFormatCount

} PixFcPixelFormat;


/*
 * A conversion block function converts pixels from an input buffer in a specific
 * format to a different format and places them in an output buffer.
 *
 * Prototype for conversion block functions 
 */
typedef		void (*ConversionBlockFn)(const struct PixFcSSE *pixfc, void *inBuffer, void *outBuffer);


/*
 *  This structure is the entry point for pixel conversion.
 *  - You obtain one by calling by create_pixfc(),
 *  - Convert images by calling the convert() function pointer,
 *  - and release with destroy_pixfc().
 *  All members in this structure are read-only.
 */
struct PixFcSSE{
	ConversionBlockFn 			convert;	// Conversion function - call me to perform the conversion

	PixFcPixelFormat 			source_fmt;
	PixFcPixelFormat 			dest_fmt;
	uint32_t					pixel_count;
	uint32_t					width;
	uint32_t					height;
	uint32_t					uses_sse;	// set to 1 if this converter uses
											// the SSE implementation, 0 if not.
};

/*
 * This function creates a struct PixFcSSE and sets it up
 * for a conversion from the given source format to the destination
 * one if supported. See macros further down for returned error codes.
 */
uint32_t		create_pixfc(struct PixFcSSE**,	// out - returns a struct PixFcSSE
								PixFcPixelFormat,  	// in  - source format
								PixFcPixelFormat, 	// in  - destination format
								uint32_t,				// in  - width
								uint32_t, 				// in  - height
								uint32_t				// in  - flags (see #define below)
);

/*
 * List of flags that can be passed to create_pixfc() to modify
 * the conversion block selection process.
 */
#define			PIXFC_DEFAULT_FLAGS					0
#define			PIXFC_NO_SSE_FLAG					(1 << 0)	// Force the use of a non-SSE conversion routine
#define			PIXFC_SSE2_ONLY_FLAG				(1 << 1)	// Force the use of a SSE2-only conversion routine
#define			PIXFC_REQUIRE_INTERPOLATION_FLAG	(1 << 2)	// Force the use of a conversion routine which recreates missing components from existing ones
#define			PIXFC_BT601_CONVERSION_FLAG			(1 << 3)	// Force the use of conversion equations suitable for SD image formats, as defined in bt.601
#define			PIXFC_BT709_CONVERSION_FLAG			(1 << 4)	// Force the use of conversion equations suitable for HD image formats, as defined in bt.709



/*
 * This function releases the given struct PixFcSSE
 */
void			destroy_pixfc(struct PixFcSSE*);

/*
 * Error codes
 */
// All good
#define			PIXFC_OK								0

// Generic error code
#define			PIXFC_ERROR							-1

// Conversion from source to destination format not supported
#define			PIXFC_CONVERSION_NOT_SUPPORTED		-2

// Out of memory
#define 		PIXFC_OOM								-3


#ifdef __cplusplus
}
#endif

#endif /* _PIXFC_SSE_H_ */
