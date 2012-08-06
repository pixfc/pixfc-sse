/*
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
#include "platform_util.h"
#include "rgb_image.h"
#include "test-utils.h"

#ifdef __INTEL_CPU__
#include <emmintrin.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//
// Use the conversion block at the provided index to convert image of given width and height in in buffer to out buffer
static int		do_image_conversion(uint32_t index, void* in, void *out, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc = NULL;
	
	// Create struct pixfc
	if (create_pixfc_for_conversion_block(index, &pixfc, w, h) != 0) {
		pixfc_log("Error create struct pixfc\n");
		return -1;
	}
	
	// Perform conversion
	pixfc->convert(pixfc, in, out);
		
	destroy_pixfc(pixfc);
	
	return 0;
}

int 		main(int argc, char **argv) {
	uint32_t				index;
	const InputFile*		in_file = NULL;
	char 					out_filename[128] = {0};
	char					*r;
	PixFcPixelFormat		src_fmt = PixFcFormatCount;
	PixFcPixelFormat		dst_fmt = PixFcFormatCount;
	// In / out buffers
	__m128i	*				in = NULL;
	__m128i	*				out = NULL;	
	
	// Check if we should restrict to a single source format
	if (argc >= 2) {
		src_fmt = find_matching_pixel_format(argv[1]);
		if (src_fmt != PixFcFormatCount)
			printf("Using source pixel format '%s'\n", pixfmt_descriptions[src_fmt].name);
		else
		{
			printf("Unknown source pixel format '%s'\n", argv[1]);
			exit(1);
		}
	}

	if (argc == 3) {
		dst_fmt = find_matching_pixel_format(argv[2]);
		if (dst_fmt != PixFcFormatCount)
			printf("Using dest pixel format '%s'\n", pixfmt_descriptions[dst_fmt].name);
		else
		{
			printf("Unknown dest pixel format '%s'\n", argv[2]);
			exit(1);
		}
	}

	// Loop over all conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {
		// Check if we have to restrict to the specified source format
		if ((src_fmt != PixFcFormatCount) && (src_fmt != conversion_blocks[index].source_fmt))
			continue;
		
		// Check if we have to restrict to the specified dest format
		if ((dst_fmt != PixFcFormatCount) && (dst_fmt != conversion_blocks[index].dest_fmt))
			continue;

		// Make sure the CPU has the required features
		if (does_cpu_support(conversion_blocks[index].required_cpu_features) != 0) {
			printf("  (skipped %s - CPU missing required features)\n",
				   conversion_blocks[index].name);
			continue;
		}
		
		// If we have changed input format...
		if (! in_file || (in_file->format != conversion_blocks[index].source_fmt)){
			// Release previous image and load image in new format
			ALIGN_FREE(in);
			
			in_file = find_input_file_for_format(conversion_blocks[index].source_fmt, 0);
			if (in_file == NULL) {
				pixfc_log("Error looking for input file for format '%s'\n",
					   pixfmt_descriptions[conversion_blocks[index].source_fmt].name);
				return 1;
			}
			
			if (get_buffer_from_input_file(in_file, (void **) &in) != 0) {
				pixfc_log("Error setting up input buffer");
				return 1;
			}
		}
		
		// Release previous output buffer and create new one
		ALIGN_FREE(out);
		
		if (allocate_aligned_buffer(conversion_blocks[index].dest_fmt, in_file->width, in_file->height, (void **)&out) != 0) {
			pixfc_log("Error allocating output buffer");
			return 1;
		}

		printf("%-60s %dx%d\n", conversion_blocks[index].name, in_file->width, in_file->height);
		
		// Do conversion
		if (do_image_conversion(index, in, out, in_file->width, in_file->height) != 0)
			return -1;
		
		// Save output buffer
		strcpy(out_filename, (in_file->filename) ? in_file->filename : "rgb_buffer");
		strcat(out_filename, " - ");
		strcat(out_filename, conversion_blocks[index].name);
		while ( (r = strchr(out_filename, '/')) != NULL)
			   r[0] = '-';
		write_buffer_to_file(conversion_blocks[index].dest_fmt, in_file->width, in_file->height, out_filename, out);
		
		// Add a blank line if the next conversion block uses different
		// source or destinaton format
		if (((index + 1) < conversion_blocks_count)
			&& ((conversion_blocks[(index + 1)].source_fmt !=
				 conversion_blocks[index].source_fmt)
				|| (conversion_blocks[(index + 1)].dest_fmt !=
					conversion_blocks[index].dest_fmt)))
			printf("\n");
	}
	
	ALIGN_FREE(in);
	ALIGN_FREE(out);			   
	
	return 0;
}


