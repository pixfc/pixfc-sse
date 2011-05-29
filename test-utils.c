/*
 * test-utils.c
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
#define _CRT_SECURE_NO_WARNINGS		// MS only
#include <emmintrin.h>
#include <stdio.h>
#include <string.h>

#include "pixfmt_descriptions.h"
#include "platform_util.h"
#include "rgb_image.h"
#include "test-utils.h"


PixFcPixelFormat	find_matching_pixel_format(char *format_string) {
	PixFcPixelFormat		format = PixFcFormatCount;
	uint32_t				index = pixfmt_descriptions_count;

	while(index-- > 0) {
		if (strcmp(pixfmt_descriptions[index].name, format_string) == 0) {
			format = pixfmt_descriptions[index].pixFcFormat;
			break;
		}
	}

	return format;
}

uint32_t	validate_image_dimensions(PixFcPixelFormat fmt, uint32_t width, uint32_t height) {
	const PixelFormatDescription *desc = &pixfmt_descriptions[fmt];

	// make sure the number of pixels is multiple of 16
	if ((width * height) % 16 != 0) {
		dprint("pixel count is not multiple of 16\n");
		return -1;
	}

	// make sure the buffer size has no decimal part
	if (((width * height) * desc->bytes_per_pix_num) % desc->bytes_per_pix_denom != 0) {
		dprint("buffer size error: (%d * %d) * %d %% %d != 0\n",
				width, height, desc->bytes_per_pix_num, desc->bytes_per_pix_denom);
		return -1;
	}

	return 0;
}




uint32_t	allocate_buffer(PixFcPixelFormat fmt, uint32_t width, uint32_t height, void **buffer) {
	// validate buffer dimensions
	if (validate_image_dimensions(fmt, width, height) != 0) {
		dprint("buffer size error\n");
		return -1;
	}

	//  allocate image buffer
	ALIGN_MALLOC(*buffer, IMG_SIZE(fmt, width, height), 16);
	if (! buffer) {
		dprint("Unable to allocate memory for in buffer\n");
		return -1;
	}

	return 0;
}



void		fill_image(PixFcPixelFormat fmt, uint32_t buffer_size, void * buf) {
	const PixelFormatDescription *	desc = &pixfmt_descriptions[fmt];
	__m128i * 						buffer = buf;
	uint32_t						index = 0;


	// Fill in the buffer 16 pixels at a time
	// while alternating the fill patterns
	while (buffer_size > 0) {
		*buffer = _mm_load_si128(&(_M(desc->fill_patterns[index])));

		// move on to next fill buffer
		index = ((index+1) < desc->fill_patterns_count) ? (index + 1) : 0;

		// copy 16 pixels at a time
		buffer++;
		buffer_size -= 16;
	}
}


uint32_t		fill_argb_image_with_rgb_buffer(PixFcPixelFormat fmt, uint32_t width, uint32_t height, void * buf) {
	uint32_t		pixel_count = width * height;
	uint8_t*		dest = (uint8_t *) buf;
	uint8_t			pixel[3] = {0};

	// Make sure we are converting the image into an RGB buffer
	if ((fmt != PixFcARGB) && (fmt != PixFcBGRA) && (fmt != PixFcRGB24) && (fmt != PixFcBGR24)){
		dprint("Expected RGB buffer\n");
		return -1;
	}

	if ((width != rgb_img_width) || (height != rgb_img_height)) {
		dprint("RGB buffer dimensions (%dx%d) are different from RGB image in header file (%dx%d)\n",
				width, height, rgb_img_width, rgb_img_height);
		return -1;
	}

	// Fill the buffer
	while (pixel_count > 0) {
		HEADER_PIXEL(header_data,  pixel);

		switch (fmt) {
		case PixFcARGB:
			*dest++ = 0;
		case PixFcRGB24:
			*dest++ = pixel[0];
			*dest++ = pixel[1];
			*dest++ = pixel[2];
			break;
		default:
		case PixFcBGRA:
		case PixFcBGR24:
			*dest++ = pixel[2];
			*dest++ = pixel[1];
			*dest++ = pixel[0];
			if (fmt == PixFcBGRA)
				*dest++ = 0;
			break;
		}

		pixel_count--;
	}

	return 0;
}


int32_t 	get_buffer_from_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void **buffer) {
	uint8_t *						file_buffer;
	struct stat						file_stat;
	int								fd;
	int32_t							bytes_read = 0;

	// validate width, height and pixel format
	if (validate_image_dimensions(fmt, width, height) != 0) {
		dprint("Error validating image dimensions\n");
		return -1;
	}

	// open file
	fd = OPEN(filename, RD_ONLY_FLAG);
	if (fd == -1) {
		dprint("Error opening input file '%s'\n", filename);
		return -1;
	}

	// get file size
	if (fstat(fd, &file_stat) != 0) {
		dprint("Error stat'ing file\n");
		CLOSE(fd);
		return -1;
	}

	// ensure file size is consistent with width, height and pixel format
	if (file_stat.st_size != IMG_SIZE(fmt, width, height))
	{
		dprint("file size not consistent with width, height and image "
				"format: file size %u - expected: %u\n", 
				(uint32_t) file_stat.st_size, IMG_SIZE(fmt, width, height));
		CLOSE(fd);
		return -1;
	}

	// allocate buffer
	ALIGN_MALLOC(file_buffer, file_stat.st_size * sizeof(char), 16);
	if (! file_buffer) {
		dprint("Error allocating memory\n");
		CLOSE(fd);
		return -1;
	}

	// place data in buffer
	while(bytes_read < file_stat.st_size)
		bytes_read += READ(fd, (file_buffer + bytes_read), (file_stat.st_size - bytes_read));

	*buffer = file_buffer;

	CLOSE(fd);
	return bytes_read;
}



static void write_anyrgb_buffer_to_ppm_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void *rgb_buffer) {
	uint32_t 						rgb_buf_size = IMG_SIZE(fmt, width, height);
	uint8_t *						buffer = rgb_buffer;
	char	 						header[1024] = {0};
	char							filename_fixed[128] = {0};
	int32_t							fd;
	uint32_t						stride = ((fmt == PixFcARGB) || (fmt == PixFcBGRA)) ? 4 * 16 : 3 * 16;
	// above values multiplied by 16 bc we handle 16 pixels in one iteration

	SNPRINTF(filename_fixed, sizeof(filename_fixed), "%.120s.ppm", filename);
	dprint("Writing file '%s'...\n", filename_fixed);

	if (rgb_buf_size % stride != 0) {
		dprint("Error saving to PPM: the RGB buffer size not multiple of %u\n", stride);
		return;
	}

	// to speed up things, this routine handles 16 pixels at a time
	// since all conversion routines require input buffers to have
	// a multiple of 16 pixels.
	if (rgb_buf_size % 16 != 0) {
		dprint("Error saving to PPM: the RGB buffer size not multiple of %u\n", stride);
		return;
	}

	// open file
	fd = OPEN(filename_fixed, WR_CREATE_FLAG, RW_PERM);
	if (fd == -1) {
		dprint("Error opening the PPM file for writing\n");
		return;
	}

	// write header
	sprintf(header, "P3\n%d %d\n255\n", width, height);
	if (WRITE(fd, header, strlen(header)) == -1) {
		dprint("Error writing header to PPM file\n");
		CLOSE(fd);
		return;
	}

	// write pixels
	while(rgb_buf_size > 0) {
		char pixel[4 * 3 * 16 + 3] = {0};
		int count;

		// We are going to write another 16 pixels
		rgb_buf_size -= stride;

#define PATTERN	"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "\
		"%hhu %hhu %hhu "


		if (fmt == PixFcARGB)
			count =	sprintf(pixel, PATTERN, buffer[1], buffer[2], buffer[3],
					buffer[5], buffer[6], buffer[7],
					buffer[9], buffer[10], buffer[11],
					buffer[13], buffer[14], buffer[15],
					buffer[17], buffer[18], buffer[19],
					buffer[21], buffer[22], buffer[23],
					buffer[25], buffer[26], buffer[27],
					buffer[29], buffer[30], buffer[31],
					buffer[33], buffer[34], buffer[35],
					buffer[37], buffer[38], buffer[39],
					buffer[41], buffer[42], buffer[43],
					buffer[45], buffer[46], buffer[47],
					buffer[49], buffer[50], buffer[51],
					buffer[53], buffer[54], buffer[55],
					buffer[57], buffer[58], buffer[59],
					buffer[61], buffer[62], buffer[63]
					);
		else if (fmt == PixFcBGRA)
			count =	sprintf(pixel, PATTERN, buffer[2], buffer[1], buffer[0],
					buffer[6], buffer[5], buffer[4],
					buffer[10], buffer[9], buffer[8],
					buffer[14], buffer[13], buffer[12],
					buffer[18], buffer[17], buffer[16],
					buffer[22], buffer[21], buffer[20],
					buffer[26], buffer[25], buffer[24],
					buffer[30], buffer[29], buffer[28],
					buffer[34], buffer[33], buffer[32],
					buffer[38], buffer[37], buffer[36],
					buffer[42], buffer[41], buffer[40],
					buffer[46], buffer[45], buffer[44],
					buffer[50], buffer[49], buffer[48],
					buffer[54], buffer[53], buffer[52],
					buffer[58], buffer[57], buffer[56],
					buffer[62], buffer[61], buffer[60]
					);
		else if (fmt == PixFcRGB24)
			count =	sprintf(pixel, PATTERN, buffer[0], buffer[1], buffer[2],
					buffer[3], buffer[4], buffer[5],
					buffer[6], buffer[7], buffer[8],
					buffer[9], buffer[10], buffer[11],
					buffer[12], buffer[13], buffer[14],
					buffer[15], buffer[16], buffer[17],
					buffer[28], buffer[19], buffer[20],
					buffer[21], buffer[22], buffer[23],
					buffer[24], buffer[25], buffer[26],
					buffer[27], buffer[28], buffer[29],
					buffer[30], buffer[31], buffer[32],
					buffer[33], buffer[34], buffer[35],
					buffer[36], buffer[37], buffer[38],
					buffer[39], buffer[40], buffer[41],
					buffer[42], buffer[43], buffer[44],
					buffer[45], buffer[46], buffer[47]
					);
		else if (fmt == PixFcBGR24)
			count =	sprintf(pixel, PATTERN, buffer[2], buffer[1], buffer[0],
					buffer[5], buffer[4], buffer[3],
					buffer[8], buffer[7], buffer[6],
					buffer[11], buffer[10], buffer[9],
					buffer[14], buffer[13], buffer[12],
					buffer[17], buffer[16], buffer[15],
					buffer[20], buffer[19], buffer[18],
					buffer[23], buffer[22], buffer[21],
					buffer[26], buffer[25], buffer[24],
					buffer[29], buffer[28], buffer[27],
					buffer[32], buffer[31], buffer[30],
					buffer[35], buffer[34], buffer[33],
					buffer[38], buffer[37], buffer[36],
					buffer[41], buffer[40], buffer[39],
					buffer[44], buffer[43], buffer[42],
					buffer[47], buffer[46], buffer[45]
					);
		else {
			dprint("Error saving PPM file: Unknown RGB format %d\n", fmt);
			break;
		}

		if (WRITE(fd, pixel, count) == -1) {
			dprint("Error writing PPM file contents\n");
			break;
		}

		// Move on to next source pixel
		buffer += stride;
	}

	CLOSE(fd);

	dprint("done\n");
}
static void write_raw_buffer_to_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void * in) {
	int32_t			fd;
	uint32_t		count = 0;
	uint8_t			*buffer = (uint8_t *) in;
	uint32_t 		buf_size = IMG_SIZE(fmt, width, height);
	char			filename_fixed[128] = {0};

	// append the extension to the given file name
	SNPRINTF(filename_fixed, sizeof(filename_fixed), "%.120s.%s", filename, pixfmt_descriptions[fmt].name);

	dprint("Writing file '%s'...\n", filename_fixed);

	fd = OPEN(filename_fixed, WR_CREATE_FLAG, RW_PERM);
	if (fd == -1) {
		dprint("Error opening the file for writing\n");
		return;
	}

	while (count < buf_size) {
		int ret = WRITE(fd, (buffer + count), (buf_size - count));
		if (ret < 0) {
			dprint("Error writing PPM file contents\n");
			break;
		}
		count += ret;
	}

	CLOSE(fd);

	dprint("done\n");
}

void 		write_buffer_to_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void * buffer) {

	// if the buffer is in an RGB format, save it as a PPM file
	if ((fmt == PixFcARGB) || (fmt == PixFcBGRA) || (fmt == PixFcRGB24) || (fmt == PixFcBGR24))
		write_anyrgb_buffer_to_ppm_file(fmt, width, height, filename, buffer);
	else
		write_raw_buffer_to_file(fmt, width, height, filename, buffer);
}



