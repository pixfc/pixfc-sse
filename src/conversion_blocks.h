/*
 * conversion_block.h
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

#ifndef CONVERSION_BLOCK_H_
#define CONVERSION_BLOCK_H_

#include "pixfc-sse.h"

/*
 * Attributes for conversion blocks
 */

#define DEFAULT_ATTRIBUTE						0
// Set if a conversion routine uses nearest neighbour resampling
#define NNB_RESAMPLING				(1 << 1)
// Set if a conversion routine uses the conversion equations for SD formats (bt601)
#define BT601_CONVERSION			(1 << 2)
// Set if a conversion routine uses the conversion equations for HD formats (bt709)
#define BT709_CONVERSION			(1 << 3)

/*
 * A ConversionBlock is a structure grouping together 
 * a conversion function and associated metadata:
 * the source & destination formats, required CPU features, name, ... .
 * This structure describes both SSE and non-SSE conversion routines.
 */
struct ConversionBlock{
	// Conversion block function
	ConversionBlockFn		convert_fn;

	// The above function converts from source_fmt to dest_fmt.
	PixFcPixelFormat 		source_fmt;
	PixFcPixelFormat 		dest_fmt;

	// A 64-bit mask of CPU features required for this conversion function
	// to execute. CPU features are returned by the CPUID instruction.
	// See common.h for macros defining CPU features.
	uint64_t				required_cpu_features;
	
	// A mask of attributes applicable to this conversion block
	// (See #define above)
	uint32_t				attributes;

	// Some conversion routines require the number of pixels to be 
	// multiple of some value (or 1 if there is no such requirement).
	uint32_t				width_multiple;
	uint32_t				height_multiple;
	
	// Each line must have a multiple of this number of pixels,
	// or 1 if not applicable. This value is used to calculate
	// the size in bytes of each line using the ROW_SIZE() macro
	uint8_t					row_pixel_multiple;	

	char *					name;
};

/*
 * Global array of existing SSE and non-SSE conversion blocks, and the number of elements in it.
 * (Instantiated in conversion_blocks.c)
 */
extern const struct ConversionBlock		conversion_blocks[];
extern const uint32_t					conversion_blocks_count;


#endif /* CONVERSION_BLOCK_H_ */


