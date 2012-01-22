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
#include "rgb_image.h"
#include "test-utils.h"

#define		PATH_TO_TEST_IMG	"../test-images/"

typedef struct {
	PixFcPixelFormat	format;
	uint32_t			width;
	uint32_t			height;
	char *				filename;
} InputFile;

//
// mplayer is a useful tool to read and display / save in JPEG/PNM/PNg raw yuv file:
//
// to display a raw yuyv file:
// mplayer -demuxer rawvideo -rawvideo format=yuy2:w=1920:h=1080 output.YUYV -loop 0
//
// to convert a raw yuyv file to PNG
// mplayer -demuxer rawvideo -rawvideo format=yuy2:w=1920:h=1080 output.YUYV -vo png:z=0
//

/*
 * Global variables
 */

const static InputFile		input_files[] = {
	{
		PixFcYUYV,
		1280, 1024,
		"1280x1024.yuyv",
	},
	{
		PixFcUYVY,
		1920, 1080,
		"1920x1080.uyvy",
	},
	{
		PixFcYUV422P,
		1280, 1024,
		"1280x1024.yuv422p",
	},
	{
		PixFcARGB,
		1920, 1080,
		NULL,
	},
	{
		PixFcBGRA,
		1920, 1080,
		NULL,
	},
	{
		PixFcRGB24,
		1920, 1080,
		NULL,
	},
	{
		PixFcBGR24,
		1920, 1080,
		NULL,
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
	// In / out buffers
	__m128i	*				in = NULL;
	__m128i	*				out = NULL;	
	
	// Check if we should restrict to a single source format
	if (argc == 2) {
		src_fmt = find_matching_pixel_format(argv[1]);
		if (src_fmt != PixFcFormatCount)
			printf("Using source pixel format '%s'\n", pixfmt_descriptions[src_fmt].name);
		else
		{
			printf("Unknown pixel format '%s'\n", argv[1]);
			exit(1);
		}
	}

	// Loop over all conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {
		// Check if we have to restrict to the specified source format
		if ((src_fmt != PixFcFormatCount) && (src_fmt != conversion_blocks[index].source_fmt))
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
			
			in_file = find_input_file_for_format(conversion_blocks[index].source_fmt);
			if (in_file == NULL) {
				pixfc_log("Error looking for input file for format '%s'\n",
					   pixfmt_descriptions[conversion_blocks[index].source_fmt].name);
				return 1;
			}
			
			if (in_file->filename != NULL) {
				char 	in_filename[128] = {0};
				strcat(in_filename, PATH_TO_TEST_IMG);
				strcat(in_filename, in_file->filename);

				// Load buffer from specified file
				if (get_buffer_from_file(conversion_blocks[index].source_fmt, in_file->width, in_file->height, in_filename, (void **)&in) < 0) {
					pixfc_log("Error getting buffer from input file '%s'\n", in_file->filename);
					return 1;
				}
			} else {
				// no input file, assume its an RGB format
				// try to generate the image from RGB buffer from GIMP

				// allocate buffer
				if (allocate_aligned_buffer(conversion_blocks[index].source_fmt, in_file->width, in_file->height, (void **) &in)){
					pixfc_log("Error allocating memory\n");
					return -1;
				}
				if (fill_argb_image_with_rgb_buffer(conversion_blocks[index].source_fmt, in_file->width, in_file->height, in) != 0) {
					pixfc_log("Error getting buffer from RGB GIMP image\n");
					return 1;
				}
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


