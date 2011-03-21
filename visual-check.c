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

#include <emmintrin.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "conversion_blocks.h"
#include "pixfc-sse.h"
#include "pixfmt_descriptions.h"
#include "platform_util.h"
#include "test-utils.h"

typedef struct {
	PixFcPixelFormat	format;
	uint32_t			width;
	uint32_t			height;
	char *				filename;
} InputFile;


/*
 * Global variables
 */

const static InputFile		input_files[] = {
	{
		PixFcYUYV,
		1280, 1024,
		"test-images/1280x1024.yuyv",
	},
	{
		PixFcUYVY,
		1920, 1080,
		"test-images/1920x1080.uyvy",
	},
};
const static uint32_t		input_files_size = sizeof(input_files) / sizeof(input_files[0]);


//
// Return the InputFile matching the given format, or NULL
static const InputFile* 	find_input_file_for_format(PixFcPixelFormat format){
	uint32_t	index = input_files_size;

	while (index-- > 0) {
		if (input_files[index].format == format)
			return &input_files[index];
	}

	return NULL;
}


//
// Use the provided conversion block to convert image of given width and height in in buffer to out buffer
static int		do_image_conversion(const struct ConversionBlock *block, void* in, void *out, uint32_t w, uint32_t h) {
	struct PixFcSSE *	pixfc = NULL;
	
	// Create struct pixfc
	if (create_pixfc(&pixfc, block->source_fmt, block->dest_fmt, w, h, PIXFC_DEFAULT_FLAGS) != 0) {
		dprint("Error create struct pixfc\n");
		return -1;
	}
	
	// Perform conversion
	block->convert_fn(pixfc, in, out);
		
	destroy_pixfc(pixfc);
	
	return 0;
}

int 		main(int argc, char **argv) {
	uint32_t				index;
	const InputFile*		in_file = NULL;
	char 					out_filename[128] = {0};
	char					*r;
	// In / out buffers
	__m128i	*				in = NULL;
	__m128i	*				out = NULL;	
	
	// Loop over all conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {
		
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
			
			in_file = find_input_file_for_format(conversion_blocks[index].source_fmt);
			if (in_file == NULL) {
				dprint("Error looking for input file for format '%s'\n",
					   pixfmt_descriptions[conversion_blocks[index].source_fmt].name);
				return 1;
			}
			
			if (get_buffer_from_file(conversion_blocks[index].source_fmt, in_file->width, in_file->height, in_file->filename, (void **)&in) < 0) {
				dprint("Error getting buffer from input file '%s'\n", in_file->filename);
				return 1;
			}
		}
		
		// Release previous output buffer and create new one
		ALIGN_FREE(out);
		
		if (allocate_buffer(conversion_blocks[index].dest_fmt, in_file->width, in_file->height, (void **)&out) != 0) {
			dprint("Error allocating output buffer");
			return 1;
		}

		printf("%-60s %dx%d\n", conversion_blocks[index].name, in_file->width, in_file->height);
		
		// Do conversion
		if (do_image_conversion(&conversion_blocks[index], in, out, in_file->width, in_file->height) != 0)
			return -1;
		
		// Save output buffer
		strcpy(out_filename, in_file->filename);
		strcat(out_filename, " - ");
		strcat(out_filename, conversion_blocks[index].name);
		strcat(out_filename, ".ppm");
		while ( (r = strchr(out_filename, '/')) != NULL)
			   r[0] = '-';
		write_anyrgb_buffer_to_ppm_file(conversion_blocks[index].dest_fmt, in_file->width, in_file->height, out_filename, out);
		
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


