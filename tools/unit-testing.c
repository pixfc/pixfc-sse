/*
 * unit-testing.c
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

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "conversion_blocks.h"
#include "pixfc-sse.h"
#include "pixfmt_descriptions.h"
#include "platform_util.h"
#include "test-utils.h"

#define		WIDTH				1920
#define 	HEIGHT				1080
#define		NUM_RUNS			100
#define		NUM_INPUT_BUFFERS	5



static int		time_conversion_block(struct PixFcSSE *pixfc, struct timings *timings) {
	void *					input[NUM_INPUT_BUFFERS] = {0};
	void *					output = NULL;
	uint32_t				w = WIDTH;
	uint32_t				h = HEIGHT;
	uint32_t				run_count;
	uint32_t				i;		// index into the input buffer array
	int						result = -1;

	// Allocate and fill source buffers
	for ( i = 0; i < NUM_INPUT_BUFFERS; i++) {
		if (allocate_aligned_buffer(pixfc->source_fmt, w, h, &input[i]) != 0) {
			log("Error allocating input buffer\n");
			goto done;
		}

		fill_image(pixfc->source_fmt, (w * h), input[i]);
	}

	// Allocate destination buffers
	if (allocate_aligned_buffer(pixfc->dest_fmt, w, h, &output) != 0){
		log("Error allocating out buffer\n");
		goto done;
	}

	i = 0;
	for(run_count = 0; run_count < NUM_RUNS; run_count++) {
		// Perform conversion
		do_timing(NULL);
		pixfc->convert(pixfc, input[i], output);
		do_timing(timings);

		// Force context switch to invalidate caches
		MSSLEEP(15);

		// Move on to next buffer
		i = (i + 1) < NUM_INPUT_BUFFERS ? (i + 1) : 0;
	}

	// All went well
	result = 0;

done:
	// Free resources
	i = NUM_INPUT_BUFFERS;
	while (i-- > 0)
		ALIGN_FREE(input[i]);
	ALIGN_FREE(output);

	return result;
}

static uint32_t		time_conversion_blocks() {
	uint32_t			index = 0;
	uint32_t			ret;
	struct PixFcSSE		pixfc;
	struct timings		timings;

	log("Input size: %d x %d - %d run(s) per conversion routine.\n", WIDTH, HEIGHT, NUM_RUNS);
	printf("%-80s\t%10s\t%10s\t%10s\t%5s\n","Conversion Block Name", "Avg Time", "Avg User Time",
			"Avg Sys Time", "Ctx Sw");

	// Loop over all conversion blocks
	while(enumerate_supported_conversions(index, &pixfc, WIDTH, HEIGHT) == 0) {
		// Reset timing info
		memset((void *)&timings, 0, sizeof(timings));

		// Time this conversion block
		if (time_conversion_block(&pixfc, &timings) != 0)
			return -1;

		printf("%-80s\t%10f\t%10f\t%10f\t%5.1f\n",
				conversion_blocks[index].name,
				(double)((double)timings.total_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)timings.user_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)timings.sys_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)(timings.vcs + timings.ivcs) / (double) NUM_RUNS )
		);

		// Add a blank line if the next conversion block uses different
		// source or destinaton format
		if (((index + 1) < conversion_blocks_count)
				&& ((conversion_blocks[(index + 1)].source_fmt !=
						conversion_blocks[index].source_fmt)
						|| (conversion_blocks[(index + 1)].dest_fmt !=
								conversion_blocks[index].dest_fmt)))
			printf("\n");

		// move on to next conversion block
		index++;
	}

	return 0;
}

static uint32_t			check_formats_enum() {
	uint32_t		index;
	uint32_t		result = 0;

	// Enumerate the entire PixFcSSE pixel formats
	for(index = 0; index<pixfmt_descriptions_count; index++) {

		// Each pixel format description has a member indicating which
		// PixFcPixelFormat it describes. Also, the pixel format
		// descriptions array must be sorted according to the
		// PixFcPixelFormat enum. Here we enforce this rule.
		if (pixfmt_descriptions[index].pixFcFormat != index) {
			log("Element at index %d in pixel format description does not"
					" match the expected PixFcPixelFormat enum entry (it "
					"matches entry %d)\n", index, pixfmt_descriptions[index].pixFcFormat);
			result = -1;
		}
	}


	// Check that both arrays have the same number of elements
	if (index != PixFcFormatCount) {
		log("The number of pixel format descriptions (%d does not"
				" match the number of PixFcPixelFormat entries (%d)).\n",
				index, PixFcFormatCount);
		result = -1;
	}

	return result;
}

static int			check_unaligned_conversions() {
	struct PixFcSSE 				pixfc;
	void *							input[2] = { NULL };	// 1 aligned & 1 unaligned input buffer
	void *							output[2] = { NULL };	// 1 aligned & 1 unaligned output buffer
	uint32_t						w = 64,	h = 2, index = 0;

	// Loop over all conversion blocks
	while(enumerate_supported_conversions(index, &pixfc, w, h) == 0) {

		printf("%-80s\t", conversion_blocks[index].name);

		// Allocate the input & output buffers
		if (allocate_aligned_buffer(pixfc.source_fmt, w, h, &input[0]) != 0) {
			log("Error allocating aligned input buffer\n");
			return -1;
		}
		if (allocate_unaligned_buffer(pixfc.source_fmt, w, h, &input[1]) != 0) {
			log("Error allocating unaligned input buffer\n");
			return -1;
		}
		if (allocate_aligned_buffer(pixfc.dest_fmt, w, h, &output[0]) != 0) {
			log("Error allocating aligned output buffer\n");
			return -1;
		}
		if (allocate_unaligned_buffer(pixfc.dest_fmt, w, h, &output[1]) != 0) {
			log("Error allocating unaligned output buffer\n");
			return -1;
		}

		// Fill input buffers
		fill_image(pixfc.source_fmt, IMG_SIZE(pixfc.source_fmt, w, h), input[0]);
		fill_image(pixfc.source_fmt, IMG_SIZE(pixfc.source_fmt, w, h), input[1]);

		// Do conversion with aligned input & output buffers
		pixfc.convert(&pixfc, input[0], output[0]);
		printf(".");

		// Do conversion with aligned input & unaligned output buffers
		pixfc.convert(&pixfc, input[0], output[1]);
		printf(".");

		// Do conversion with unaligned input & aligned output buffers
		pixfc.convert(&pixfc, input[1], output[0]);
		printf(".");

		// Do conversion with unaligned input & output buffers
		pixfc.convert(&pixfc, input[1], output[1]);
		printf(". OK !\n");

		// Free resources
		ALIGN_FREE(input[0]);
		free(input[1]);
		ALIGN_FREE(output[0]);
		free(output[1]);

		// Move on to next conversion block
		index++;
	}

	return 0;
}

/*
 * Here we run a few tests to make sure things are sound internally
 * and also to print some averages of conversion latency.
 */
int 				main(int argc, char **argv) {

	log("\n");
	log("\t\tU N I T   T E S T I N G\n");
	log("\n");
	log("Checking PixFcPixelFormat enum and description arrays...\n");
	if (check_formats_enum() == 0)
		log("PASSED\n");
	else
		log("FAILED\n");

	log("\n");
	log("\n");
	log("Testing conversion from aligned / unaligned buffers...\n");
	if (check_unaligned_conversions() == 0)
		log("PASSED\n");
	else
		log("FAILED\n");

	log("\n");
	log("\n");
	log("Checking conversion block timing ... \n");
	if (time_conversion_blocks() == 0)
		log("PASSED\n");
	else
		log("FAILED\n");

	log("\n");
	return 0;
}




