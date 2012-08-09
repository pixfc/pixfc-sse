/*
 * example.c
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


/*
 * This file shows how to use PixFC-SSE in your own application.
 * Here, we will convert a YUYV image to ARGB:
 * - we first allocated the buffers,
 * - we then fill the input buffer with a YUYV fill pattern,
 * - we create a struct PixFcSSE,
 * - we call the conversion routine,
 * - we release resources.
 *
 * On Linux, compile with :
 * gcc example.c BUILD_DIR/libpixfc-sse.a -I include -o example -lrt
 * (replace BUILD_DIR with the path to the actual build directory)
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pixfc-sse.h"

int main(int argc, char ** argv){
	struct PixFcSSE *	pixfc;
	PixFcPixelFormat	input_format = PixFcYUYV;
	PixFcPixelFormat	output_format = PixFcARGB;

	void *		input; // input and output buffers
	void *		output;

	uint32_t	width = 1280;
	uint32_t	height = 1024;
	uint32_t	input_row_size = width * 2;	// size of each row in byte
	uint32_t	input_buf_size = input_row_size * height;	// size of YUYV input buffer
	uint32_t	output_buf_size = width * height * 4;	// size of ARGB output buffer
	uint32_t	bytes_left;

	// Fill pattern for the YUYV input buffer
	// 2 red pix - 2 green pix - 2 blue pix - 2 white pix
	uint64_t	yuyv_fill_pattern[] = { 0x15942B95FF4B544CLL, 0x80FE80FF6B1CFF1DLL };
	char *		fill_ptr;

	// Allocate buffers 
	// (should be 16-byte aligned for best performance, but dont have to)
	if (posix_memalign(&input, 16, input_buf_size) != 0) {
		fprintf(stderr, "Error allocating input buffer\n");
		return 1;
	}
	if (posix_memalign(&output, 16, output_buf_size) != 0) {
		fprintf(stderr, "Error allocating output buffer\n");
		free(input);
		return 1;
	}

	// Fill input buffer
	bytes_left = input_buf_size;
	fill_ptr = input;
	while(bytes_left > 0) {
		// Copy 16 bytes at a time
		memcpy(fill_ptr, yuyv_fill_pattern, 16);
		fill_ptr += 16;
		bytes_left -= 16;
	}


	// Create struct pixfc
	if (create_pixfc(&pixfc, input_format, output_format, width, height, input_row_size, PixFcFlag_Default) != 0) {
		fprintf(stderr, "Error creating struct pixfc\n");
		free(output);
		free(input);
		return 1;
	}

	// Do conversion
	pixfc->convert(pixfc, input, output);

	// Release resources
	destroy_pixfc(pixfc);
	free(output);
	free(input);

	return 0;
}






