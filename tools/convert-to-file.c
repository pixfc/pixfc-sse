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


/*
 * Global variables
 */
PixFcPixelFormat		src_fmt = PixFcYUYV;
PixFcPixelFormat		dst_fmt = PixFcARGB;
char *					src_filename = NULL;
uint32_t				w = 1280;
uint32_t				h = 1024;
PixFcFlag				flags = PixFcFlag_Default;

static void 		print_usage(char *prog_name) {
	printf("Convert an image in a source pixel format to a destination pixel format.\n");
	printf("Usage: %s <input format> [input file] <output format> <width> <height> <flags>\n", prog_name);
	printf("Where <input format> and <output format> are one of:\n");
	print_known_pixel_formats();
	printf("If [input file] is specified and exists, read the source image from that file.\n");
	printf("Otherwise, fill the source image with a predefined fill pattern.\n");
	printf("<flags> is a bitmask from:\n");
	print_known_flags();
}

// Parse arguments and set global variables accordingly
// Return -1 is cmd line is invalid.
static uint32_t		parse_args(int argc, char **argv){
	uint32_t arg_index = 1;

	// Need 5 or 6 args
	if ((argc != 6) && (argc != 7)) {
		printf("Invalid argument count\n");
		return -1;
	}

	// Parse input format
	if ((src_fmt = find_matching_pixel_format(argv[arg_index++])) == PixFcFormatCount) {
		printf("Invalid source format '%s'\n", argv[(arg_index - 1)]);
		return -1;
	}

	// If we have 6 args, the next arg is the input file
	if (argc == 7)
		src_filename = argv[arg_index++]; // the filename is checked when the file is opened.

	// Parse output format
	if ((dst_fmt = find_matching_pixel_format(argv[arg_index++])) == PixFcFormatCount) {
		printf("Invalid destination format '%s'\n", argv[arg_index - 1]);
		return -1;
	}

	// Parse width and height
	if (((w = atoi(argv[arg_index++])) == 0) || ((h = atoi(argv[arg_index++])) == 0)) {
		printf("Invalid width / height (%d x %d)\n", w, h);
		return -1;
	}

	flags = get_matching_flags(argv[arg_index++]);

	return 0;
}

int 		main(int argc, char **argv) {
	struct PixFcSSE * 		pixfc;
	struct timings			timings;
	// In / out buffers
	__m128i	*				in;
	__m128i	*				out;

	// Parse arguments and set global variables accordingly
	// (exit if cmd line is invalid)
	if (parse_args(argc, argv) != 0) {
		print_usage(argv[0]);
		return 1;
	}

	// Reset timing info
	memset(&timings, 0, sizeof(timings));

	// If we were given an input file name, get its contents
	if (src_filename != NULL) {
		if (get_buffer_from_file(src_fmt, w, h, src_filename, (void **)&in) < 0) {
			pixfc_log("Error getting buffer from file\n");
			return 1;
		}
	} else {
		const InputFile *in_file;

		// Try instantiating an RGB image (will fail if src_fmt is not RGB, or if the dimentsions dont match)
		if (fill_buffer_with_rgb_image(src_fmt, w, h,(void **) &in) != 0) {
			// Otherwise, use one of our own InputFile to generate the input buffer
			in_file = find_input_file_for_format(src_fmt, 0);
			if (in_file == NULL) {
				pixfc_log("Error looking for input file for format '%s'\n", pixfmt_descriptions[src_fmt].name);
				return 1;
			}

			// if the dimensions of the input file match the requeted dimensions,
			// use the input file.
			if ((in_file->width == w) && (in_file->height == h))
			{
				if (get_buffer_from_input_file(in_file, (void **) &in) != 0) {
					pixfc_log("Error setting up input buffer");
					return 1;
				}
			} else {
				// fill buffer with know pattern
				if (allocate_aligned_buffer(src_fmt, w, h, (void**)&in)){
					pixfc_log("Error allocating memory\n");
					return -1;
				}

				fill_image(src_fmt, IMG_SIZE(src_fmt, w, h), in);
			}
		}

		// save input buffer
		write_buffer_to_file(src_fmt, w, h, "input", in);
	}

	// allocate out buffer
	if (allocate_aligned_buffer(dst_fmt, w, h, (void **)&out) != 0) {
		pixfc_log("Error allocating out buffer\n");
		free(in);
		return 1;
	}

	pixfc_log("Image size:\t%d x %d\n", w, h);
	pixfc_log("Input format:\t%s\n", pixfmt_descriptions[src_fmt].name);
	pixfc_log("Output format:\t%s\n", pixfmt_descriptions[dst_fmt].name);
	pixfc_log("Flags: ");
	print_flags(flags);

	// create struct pixfc
	if (create_pixfc(&pixfc, src_fmt, dst_fmt, w, h, ROW_SIZE(src_fmt, w), flags) != 0)
	{
		pixfc_log("error creating struct pixfc\n");
		ALIGN_FREE(in);
		ALIGN_FREE(out);
		return 1;
	}

	pixfc_log("%10s\t%10s\t%10s\t%5s\n", "Avg Time", "Avg User Time", "Avg Sys Time", "Ctx Sw");

	// Run conversion function
	do_timing(NULL);
	pixfc->convert(pixfc, in, out);
	do_timing(&timings);

	pixfc_log("%10f\t%10f\t%10f\t%5.1f\n",
			(double)((double)timings.total_time_ns / 1000000.0),
			(double)((double)timings.user_time_ns / 1000000.0),
			(double)((double)timings.sys_time_ns / 1000000.0),
			(double)((double)(timings.vcs + timings.ivcs))
	);

	// save output buffer
	write_buffer_to_file(dst_fmt, w, h, "output", out);

	// Free data structures
	destroy_pixfc(pixfc);

	ALIGN_FREE(in);
	ALIGN_FREE(out);

	return 0;
}


