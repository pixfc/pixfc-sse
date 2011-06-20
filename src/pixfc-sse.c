/*
 * pixfc.c
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

#include <string.h>
#include "common.h"
#include "conversion_blocks.h"
#include "pixfc-sse.h"
#include "pixfmt_descriptions.h"

/*
 * Check if the given conversion block:
 * - converts from the given source format to the given destination format, 
 * - and if the CPU supports the features required by the conversion block
 */
static uint32_t		block_matches_and_is_supported(struct PixFcSSE* conv, const struct ConversionBlock *block,
		PixFcPixelFormat src_fmt, PixFcPixelFormat dest_fmt, uint32_t flags) {

	dprint("Checking conversion block '%s'\n", block->name);
	dprint("Block src / dst fmt: %s\t%s\n", pixfmt_descriptions[block->source_fmt].name,
			pixfmt_descriptions[block->dest_fmt].name);

	// If this block does not handle the requested conversion, error out.
	if ((block->source_fmt != src_fmt) || (block->dest_fmt != dest_fmt)) {
		dprint("Source / Destination mismatch\n");
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	// If we were told to require conversion blocks performing NNB resampling,
	// enforce it.
	if (((flags & PixFcFlag_NNbResampling) == 0 ) != ((block->attributes & NNB_RESAMPLING) == 0)) {
		dprint("Enforcing NNbResampling flag\n");
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}
	
	// If we were told to disable SSE conversion blocks, make sure this block
	// does not require any SSE features.
	if ((flags & PixFcFlag_NoSSE) && (block->required_cpu_features != CPUID_FEATURE_NONE)) {
		dprint("Enforcing FORCE_NO_SSE flag\n");
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	// If we were told to use ITU-R Rec. BT601 conversion, make sure this block
	// uses these equations.
	if ((flags & PixFcFlag_BT601Conversion) && ! (block->attributes & BT601_CONVERSION)) {
		dprint("Enforcing SD_FORMAT_CONVERSION flag\n");
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	// If we were told to use ITU-R Rec. BT709 conversion, make sure this block
	// uses these equations.
	if ((flags & PixFcFlag_BT709Conversion) && ! (block->attributes & BT709_CONVERSION)) {
		dprint("Enforcing HD_FORMAT_CONVERSION flag\n");
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	// If the cpu does not have the required features, error out.
	if (does_cpu_support(block->required_cpu_features) != 0) {
		dprint("CPU feature mismatch:\n");
		dprint("Required CPU features:  %#08llx\n", (long long unsigned int)block->required_cpu_features);
		dprint("Supported CPU features: %#08llx\n", (long long unsigned int)get_cpu_features());
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	// If we were told to use an SSE2-only routine, make sure that's the case
	if ((flags & PixFcFlag_SSE2Only) && (block->required_cpu_features != CPUID_FEATURE_SSE2)) {
		dprint("Enforcing FORCE_SSE2_ONLY flag\n");
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	// If the number of pixels is not multiple of the required value, error out.
	if (conv->pixel_count % block->pixel_count_multiple != 0) {
		dprint("Pixel count (%u) not multiple of %u\n", conv->pixel_count, block->pixel_count_multiple);
		return PIXFC_CONVERSION_NOT_SUPPORTED;
	}

	return PIXFC_OK;
}


/*
 * Go through the list of conversion blocks and look for one that matches
 * the requested source and destination formats. Also check that
 * the CPU has the features required to run the conversion block.
 */
static uint32_t	look_for_matching_conversion_block(struct PixFcSSE* conv, 
		PixFcPixelFormat src_fmt, PixFcPixelFormat dest_fmt, uint32_t flags) {

	uint32_t						index;
	const struct ConversionBlock *	block;

	// Go over the array of conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {

		// Check if this conversion block matches the source and 
		// destination formats and if the CPU has the required features.
		block = &conversion_blocks[index];

		if (block_matches_and_is_supported(conv, block, src_fmt, dest_fmt, flags) == PIXFC_OK) {

			// We have a match, finish setting up the struct PixFcSSE
			conv->convert = block->convert_fn;
			conv->uses_sse = (block->required_cpu_features == CPUID_FEATURE_NONE) ? 0 : 1;

			dprint("Found conversion block (uses SSE ? %s)\n", (conv->uses_sse == 1) ? "yes" : "no");

			return PIXFC_OK;
		}
	}

	// No conversion blocks found
	return PIXFC_CONVERSION_NOT_SUPPORTED;
}


uint32_t		create_pixfc(struct PixFcSSE** pc, PixFcPixelFormat src_fmt,
		PixFcPixelFormat dest_fmt, uint32_t width, uint32_t height,
		uint32_t flags){

	struct PixFcSSE *		conv;
	uint32_t				result;

	// Make sure we have a valid pointer.
	if (! pc)
		return PIXFC_ERROR;

	// Allocate and zero structure PixFcSSE
	conv = (struct PixFcSSE *) malloc(sizeof(*conv));
	if (! conv)
		return PIXFC_OOM;
	memset(conv, 0x0, sizeof(*conv));

	// Initialise members
	conv->source_fmt = src_fmt;
	conv->dest_fmt = dest_fmt;
	conv->width = width;
	conv->height = height;
	conv->pixel_count = width * height;

	dprint("Requested src / dst fmt:\t%s\t%s\n", pixfmt_descriptions[src_fmt].name,
			pixfmt_descriptions[dest_fmt].name);
	dprint("Supported CPU features: %#08llx\n", (long long unsigned int)get_cpu_features());

	// Look for a conversion block to do the requested conversion
	result = look_for_matching_conversion_block(conv, src_fmt, dest_fmt, flags);

	// Return struct PixFcSSE if OK, release it otherwise
	if (result == PIXFC_OK)
		*pc = conv;
	else // Error or conversion not supported
		destroy_pixfc(conv);

	return result;
}

void			destroy_pixfc(struct PixFcSSE* conv) {
	if (conv)
		free(conv);
}

