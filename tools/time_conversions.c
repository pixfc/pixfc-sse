/*
 * time_conversions.c
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
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
#include <stdlib.h>

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
			pixfc_log("Error allocating input buffer\n");
			goto done;
		}

		fill_image(pixfc->source_fmt, (w * h), input[i]);
	}

	// Allocate destination buffers
	if (allocate_aligned_buffer(pixfc->dest_fmt, w, h, &output) != 0){
		pixfc_log("Error allocating out buffer\n");
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

static uint32_t		time_conversion_blocks(PixFcPixelFormat source_fmt, PixFcPixelFormat dest_fmt) {
	uint32_t			index = 0;
	struct PixFcSSE		*pixfc;
	struct timings		timings;
	FILE				*file;
	char				filename[255] = {0};
	char				*csv_conv_name = NULL;
	
	SNPRINTF(filename, sizeof(filename), "timings_%dx%d_%d_runs_per_conversion.csv", WIDTH, HEIGHT, NUM_RUNS);
	
	file = fopen(filename, "w");
	if (file == NULL) {
		pixfc_log("Error opening the CSV file for writing\n");
		return -1;
	}
	
	pixfc_log("Input size: %d x %d - %d run(s) per conversion routine.\n", WIDTH, HEIGHT, NUM_RUNS);
	printf("%-80s\t%10s\t%10s\t%10s\t%10s\n","Conversion Block Name", "Avg Time(ms)", "Avg User(ms)",
			"Avg Sys(ms)", "Total Ctx Sw");
	fprintf(file, "Source,Destination,Standard,SSE,Resampling,Avg Time(ms),Avg User Time(ms),Avg Sys Time(ms),Total Ctx Sw\n");

	// Loop over all conversion blocks
	for(index = 0; index < conversion_blocks_count; index++) {
		if ((source_fmt != PixFcFormatCount) && (conversion_blocks[index].source_fmt != source_fmt))
			continue;

		if ((dest_fmt != PixFcFormatCount) && (conversion_blocks[index].dest_fmt != dest_fmt))
			continue;

		if (create_pixfc_for_conversion_block(index, &pixfc, WIDTH, HEIGHT) != 0) {
			printf("Unable to test conversion block '%s'\n", conversion_blocks[index].name);
			continue;
		}

		// Reset timing info
		memset((void *)&timings, 0, sizeof(timings));

		// Time this conversion block
		if (time_conversion_block(pixfc, &timings) != 0)
			return -1;

		destroy_pixfc(pixfc);

		printf("%-80s\t%10f\t%10f\t%10f\t%10llu\n",
				conversion_blocks[index].name,
				(double)((double)timings.total_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)timings.user_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)timings.sys_time_ns / (double) (NUM_RUNS * 1000000)),
				timings.vcs + timings.ivcs
		);
		make_conv_block_name_csv_friendly(index, &csv_conv_name);
		fprintf(file, "%s,%.3f,%.3f,%.3f,%llu\n",
				csv_conv_name,
				(double)((double)timings.total_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)timings.user_time_ns / (double) (NUM_RUNS * 1000000)),
				(double)((double)timings.sys_time_ns / (double) (NUM_RUNS * 1000000)),
				timings.vcs + timings.ivcs);
		free(csv_conv_name);

		// Add a blank line if the next conversion block uses different
		// source or destinaton format
		if (((index + 1) < conversion_blocks_count)
				&& ((conversion_blocks[(index + 1)].source_fmt !=
						conversion_blocks[index].source_fmt)
						|| (conversion_blocks[(index + 1)].dest_fmt !=
								conversion_blocks[index].dest_fmt)))
			printf("\n");
	}
	
	fclose(file);

	return 0;
}

int 				main(int argc, char **argv) {
	PixFcPixelFormat	source_fmt = PixFcFormatCount;
	PixFcPixelFormat	dest_fmt = PixFcFormatCount;

	if (argc >= 2)
		source_fmt = find_matching_pixel_format(argv[1]);

	if (argc == 3)
		dest_fmt = find_matching_pixel_format(argv[2]);

	if (time_conversion_blocks(source_fmt, dest_fmt) != 0) {
		pixfc_log("FAILED\n");
		return -1;
	}

	pixfc_log("\n");
	return 0;
}




