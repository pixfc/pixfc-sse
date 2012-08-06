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
#include "common.h"
#include "conversion_blocks.h"
#include "pixfc-sse.h"
#include "pixfmt_descriptions.h"

#include <string.h>
#include <stdlib.h>

/*
 * Check if the given conversion block:
 * - converts from the given source format to the given destination format, 
 * - and if the CPU supports the features required by the conversion block
 */
static uint32_t		block_matches_and_is_supported(struct PixFcSSE* conv, const struct ConversionBlock *block,
		PixFcPixelFormat src_fmt, PixFcPixelFormat dest_fmt, uint32_t flags) {

	// If this block does not handle the requested conversion, error out.
	if ((block->source_fmt != src_fmt) || (block->dest_fmt != dest_fmt)) {
		return PixFc_UnsupportedConversionError;
	}

	// If we were told to require conversion blocks performing NNB resampling,
	// enforce it.
	if ((((flags & PixFcFlag_NNbResamplingOnly) == 0) != ((block->attributes & NNB_RESAMPLING) == 0))) {
		dprint("Skipping '%s' - Enforcing NNbResampling flag\n", block->name);
		return PixFc_UnsupportedConversionError;
	}
	
	// If we were told to disable SSE conversion blocks, make sure this block
	// does not require any SSE features.
	if ((flags & PixFcFlag_NoSSE) && (block->required_cpu_features != CPUID_FEATURE_NONE)) {
		dprint("Skipping '%s' - Enforcing FORCE_NO_SSE flag\n", block->name);
		return PixFc_UnsupportedConversionError;
	}

	// If we were told to use ITU-R Rec. BT601 conversion, make sure this block
	// uses these equations.
	if ((flags & PixFcFlag_BT601Conversion) && ! (block->attributes & BT601_CONVERSION)) {
		dprint("Skipping '%s' - Enforcing SD_FORMAT_CONVERSION flag\n", block->name);
		return PixFc_UnsupportedConversionError;
	}

	// If we were told to use ITU-R Rec. BT709 conversion, make sure this block
	// uses these equations.
	if ((flags & PixFcFlag_BT709Conversion) && ! (block->attributes & BT709_CONVERSION)) {
		dprint("Skipping '%s' - Enforcing HD_FORMAT_CONVERSION flag\n", block->name);
		return PixFc_UnsupportedConversionError;
	}

	// If we were told to use an SSE2-only routine, make sure that's the case
	if ((flags & PixFcFlag_SSE2Only) && (block->required_cpu_features != CPUID_FEATURE_SSE2)) {
		dprint("Skipping '%s' - Enforcing FORCE_SSE2_ONLY flag\n", block->name);
		return PixFc_UnsupportedConversionError;
	}

	// If we were told to use an SSE2 and SSSE3 routine, make sure that's the case
	if ((flags & PixFcFlag_SSE2_SSSE3Only) && (block->required_cpu_features != (CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3))) {
		dprint("Skipping '%s' - Enforcing FORCE_SSE2_SSSE3_ONLY flag\n", block->name);
		return PixFc_UnsupportedConversionError;
	}

	// If the cpu does not have the required features, error out.
	if (does_cpu_support(block->required_cpu_features) != 0) {
		dprint("Skipping '%s' - CPU feature mismatch:\n", block->name);
		dprint("Required CPU features:  %#08llx\n", (long long unsigned int)block->required_cpu_features);
		dprint("Supported CPU features: %#08llx\n", (long long unsigned int)get_cpu_features());

		// If both 'PixFcFlag_SSE2_SSSE3Only' and 'PixFcFlag_SSE2Only' are not set,
		// then return PixFc_UnsupportedConversionError, so we keep looking for a potential
		// conversion block supported by the CPU. However, if either of these 2 flags is present,
		// return PixFc_NoCPUSupport since the user requested a specific SSE version, but the CPU
		// does not support it.
		return ((! (flags & PixFcFlag_SSE2_SSSE3Only)) && (! (flags & PixFcFlag_SSE2Only))) ? PixFc_UnsupportedConversionError : PixFc_NoCPUSupport;
	}

	
	//
	// At this stage, we have found a conversion block which matches the flags given to us.
	
	// Check the bytes-per-row value
	if (conv->row_bytes != ROW_SIZE(block->source_fmt, conv->width)) {
		dprint("Skipping '%s' - Invalid row bytes %u - expected %d\n", block->name, conv->row_bytes, ROW_SIZE(block->source_fmt, conv->width));
		return PixFc_InvalidSourceImageRowSize;
	}

	// If the width is not multiple of the required value, error out.
	if (conv->width % block->width_multiple != 0) {
		dprint("Skipping '%s' - Width (%u) not multiple of %u\n", block->name, conv->pixel_count, block->width_multiple);
		return PixFc_UnsupportedSourceImageDimension;
	}

	// If the height is not multiple of the required value, error out.
	if (conv->pixel_count % block->height_multiple != 0) {
		dprint("Skipping '%s' - Height (%u) not multiple of %u\n", block->name, conv->pixel_count, block->height_multiple);
		return PixFc_UnsupportedSourceImageDimension;
	}

	dprint("Using '%s'\n", block->name);

	return PixFc_OK;
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
	uint32_t						result;

	// Go over the array of conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {

		// Check if this conversion block matches the source and 
		// destination formats and if the CPU has the required features.
		block = &conversion_blocks[index];

		result = block_matches_and_is_supported(conv, block, src_fmt, dest_fmt, flags);
		if (result != PixFc_UnsupportedConversionError)
		{
			if (result == PixFc_OK) {

				// We have a match, finish setting up the struct PixFcSSE
				conv->convert = block->convert_fn;
				conv->uses_sse = (block->required_cpu_features == CPUID_FEATURE_NONE) ? 0 : 1;

				dprint("Found conversion block (uses SSE ? %s)\n", (conv->uses_sse == 1) ? "yes" : "no");
			}
			
			return result;
		}
	}

	// No conversion blocks found
	return PixFc_UnsupportedConversionError;
}


uint32_t		create_pixfc(struct PixFcSSE** pc, PixFcPixelFormat src_fmt,
		PixFcPixelFormat dest_fmt, uint32_t width, uint32_t height, uint32_t row_bytes,
		uint32_t flags){

	struct PixFcSSE *		conv;
	uint32_t				result;

	// Make sure we have a valid pointer.
	if (! pc)
		return PixFc_Error;

	// Allocate and zero structure PixFcSSE
	conv = (struct PixFcSSE *) malloc(sizeof(*conv));
	if (! conv)
		return PixFc_OOMError;
	memset(conv, 0x0, sizeof(*conv));

	// Initialise members
	conv->source_fmt = src_fmt;
	conv->dest_fmt = dest_fmt;
	conv->width = width;
	conv->height = height;
	conv->row_bytes = row_bytes;
	conv->pixel_count = width * height;

	dprint("Requested src / dst fmt:\t%s\t%s\n", pixfmt_descriptions[src_fmt].name,
			pixfmt_descriptions[dest_fmt].name);
	dprint("Supported CPU features: %#08llx\n", (long long unsigned int)get_cpu_features());

	// Look for a conversion block to do the requested conversion
	result = look_for_matching_conversion_block(conv, src_fmt, dest_fmt, flags);

	// Return struct PixFcSSE if OK, release it otherwise
	if (result == PixFc_OK)
		*pc = conv;
	else // Error or conversion not supported
		destroy_pixfc(conv);

	return result;
}

void			destroy_pixfc(struct PixFcSSE* conv) {
	if (conv)
		free(conv);
}

