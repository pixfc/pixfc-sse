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

#include "common.h"
#include "conversion_blocks.h"
#include "pixfmt_descriptions.h"
#include "platform_util.h"
#include "test-utils.h"

#ifdef __INTEL_CPU__
#include <emmintrin.h>
#else
void _mm_store_si128(__m128i *dest, __m128i src) {
	uint8_t* d = (uint8_t*)dest;
	uint8_t* s = (uint8_t*)&src;
	uint32_t i = 0;
	
	for(i = 0; i < 16; i++) {
		*d++ = *s++;
	}
}

void _mm_storeu_si128(__m128i *dest, __m128i src) {
	_mm_store_si128(dest, src);
}
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
	#include <windows.h>
	#include <intrin.h>
#else
	#include <sys/resource.h>
	#include <sys/time.h>

	#ifdef __APPLE__
		#include <mach/mach.h>
		#include <mach/clock.h>
		#include <mach/mach_time.h>
	#endif
#endif

/*
 * This array must be sorted as follows:
 * - all elements describing the same pixel format must be contiguous (ie all PixFcYUYV must be next to each other),
 * - the first element for a pixel format must be the one with the highest resolution and the subsequent elements
 *   must be sorted be descending resolution.
 */
const InputFile		input_files[] = {

	{	PixFcYUYV,		1280, 	1024,		"1280x1024.yuyv",	},
	{	PixFcYUYV,		48, 	1,			NULL,				},
	{	PixFcYUYV,		32, 	1,			NULL,				},
	{	PixFcYUYV,		16, 	1,			NULL,				},

	{	PixFcUYVY,		1920, 	1080,		"1920x1080.uyvy",	},
	{	PixFcUYVY,		48, 	1,			NULL,				},
	{	PixFcUYVY,		32, 	1,			NULL,				},
	{	PixFcUYVY,		16, 	1,			NULL,				},

	{	PixFcYUV422P,	1280, 	1024,		"1280x1024.yuv422p",},
	{	PixFcYUV422P,	32, 	1,			NULL,				},

	{	PixFcYUV420P,	1280, 	1024,		"1280x1024.yuv420p",},
	{	PixFcYUV420P,	32, 	2,			NULL,				},

	{	PixFcV210,		1920, 	1080,		"1920x1080.v210",	},
	{	PixFcV210,		1280, 	720,		NULL,				},
	{	PixFcV210,		48, 	1,			NULL,				},
	{	PixFcV210,		32, 	1,			NULL,				},
	{	PixFcV210,		16, 	1,			NULL,				},

	{	PixFcARGB,		1920, 	1080,		NULL,				},
	{	PixFcARGB,		32,		2,			NULL,				}, // to test to YUV420p
	{	PixFcARGB,		48, 	1,			NULL,				}, // to test to v210
	{	PixFcARGB,		16, 	1,			NULL,				}, // to test to v210

	{	PixFcBGRA,		1920, 	1080,		NULL,				},
	{	PixFcBGRA,		32, 	2,			NULL,				},
	{	PixFcBGRA,		48, 	1,			NULL,				},
	{	PixFcBGRA,		16, 	1,			NULL,				},

	{	PixFcRGB24,		1920, 	1080,		NULL,				},
	{	PixFcRGB24,		32, 	2,			NULL,				},
	{	PixFcRGB24,		48, 	1,			NULL,				},
	{	PixFcRGB24,		16, 	1,			NULL,				},

	{	PixFcBGR24,		1920, 	1080,		NULL,				},
	{	PixFcBGR24,		32, 	2,			NULL,				},
	{	PixFcBGR24,		48, 	1,			NULL,				},
	{	PixFcBGR24,		16, 	1,			NULL,				},
};
const uint32_t		input_files_size = sizeof(input_files) / sizeof(input_files[0]);


//
// Return the InputFile matching the given format, or NULL
const InputFile* 	find_input_file_for_format(PixFcPixelFormat format, uint32_t index){
	uint32_t	count = 0;

	while (count < input_files_size) {
		if ((input_files[count].format == format) && (index-- == 0))
			return &input_files[count];

		count++;
	}

	return NULL;
}

int32_t 		get_buffer_from_input_file(const InputFile* in_file, void **buffer) {
	if (in_file->filename != NULL) {
			char 	in_filename[128] = {0};
			strcat(in_filename, PATH_TO_TEST_IMG);
			strcat(in_filename, in_file->filename);

			// Load buffer from specified file
			if (get_buffer_from_file(in_file->format, in_file->width, in_file->height, in_filename, buffer) < 0) {
				pixfc_log("Error getting buffer from input file '%s'\n", in_file->filename);
				return -1;
			}
		} else {
			// no input file, fill the buffer with a know pattern

			// allocate buffer
			if (allocate_aligned_buffer(in_file->format, in_file->width, in_file->height, buffer)){
				pixfc_log("Error allocating memory\n");
				return -1;
			}

			fill_image(in_file->format, IMG_SIZE(in_file->format, in_file->width, in_file->height), *buffer);
		}

	return 0;
}

void 				print_known_pixel_formats() {
	uint32_t index = 0;

	while(index < pixfmt_descriptions_count) {
		printf("  %s\n", pixfmt_descriptions[index].name);
		index++;
	}
}

void 				print_known_flags() {
	printf("  PixFcFlag_Default           = 0\n");
	printf("  PixFcFlag_NoSSE             = 1\n");
	printf("  PixFcFlag_SSE2Only          = 2\n");
	printf("  PixFcFlag_SSE2_SSSE3Only    = 4\n");
	printf("  PixFcFlag_BT601Conversion   = 128\n");
	printf("  PixFcFlag_BT709Conversion   = 256\n");
	printf("  PixFcFlag_NNbResamplingOnly = 16384\n");
}

void				print_flags(PixFcFlag flags) {
	if ((flags & PixFcFlag_NoSSE) != 0)
		printf("  NoSSE");

	if ((flags & PixFcFlag_SSE2Only) != 0)
		printf("  SSE2Only;");

	if ((flags & PixFcFlag_SSE2_SSSE3Only) != 0)
		printf("  SSE2_SSSE3Only");

	if ((flags & PixFcFlag_BT601Conversion) != 0)
		printf("  BT601Conversion");

	if ((flags & PixFcFlag_BT709Conversion) != 0)
		printf("  BT709Conversion");

	if ((flags & PixFcFlag_NNbResamplingOnly) != 0)
		printf("  NNbResamplingOnly");

	if (flags == PixFcFlag_Default)
		printf("  Default");

	printf("\n");
}

PixFcFlag	get_matching_flags(char *flag_string) {
	PixFcFlag	flags = PixFcFlag_Default;
	uint32_t	flag_value = atoi(flag_string);

	if ((flag_value & PixFcFlag_NoSSE) != 0)
		flags |= PixFcFlag_NoSSE;

	if ((flag_value & PixFcFlag_SSE2Only) != 0)
			flags |= PixFcFlag_SSE2Only;

	if ((flag_value & PixFcFlag_SSE2_SSSE3Only) != 0)
			flags |= PixFcFlag_SSE2_SSSE3Only;

	if ((flag_value & PixFcFlag_BT601Conversion) != 0)
			flags |= PixFcFlag_BT601Conversion;

	if ((flag_value & PixFcFlag_BT709Conversion) != 0)
			flags |= PixFcFlag_BT709Conversion;

	if ((flag_value & PixFcFlag_NNbResamplingOnly) != 0)
			flags |= PixFcFlag_NNbResamplingOnly;

	return flags;
}

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

	// make sure the buffer size has no decimal part IFF the buffer size has no
	// alignment requirements, ie row_pixel_multiple == 1.
	if ((desc->row_pixel_multiple == 1) && (((width * height) * desc->bytes_per_pix_num) % desc->bytes_per_pix_denom != 0)) {
		pixfc_log("buffer size error: (%d * %d) * %d %% %d != 0\n",
				width, height, desc->bytes_per_pix_num, desc->bytes_per_pix_denom);
		return -1;
	}

	// make sure the height is valid
	if (width % desc->width_multiple != 0) {
		pixfc_log("width is not multiple of %u\n", desc->width_multiple);
		return -1;
	}

	// make sure the height is valid
	if (height % desc->height_multiple != 0) {
		pixfc_log("image height error: %d %% %d != 0\n", height, desc->height_multiple);
		return -1;
	}

	return 0;
}




uint32_t	allocate_aligned_buffer(PixFcPixelFormat fmt, uint32_t width, uint32_t height, void **buffer) {
	// validate buffer dimensions
	if (validate_image_dimensions(fmt, width, height) != 0) {
		pixfc_log("buffer size error\n");
		return -2;
	}

	//  allocate image buffer
	ALIGN_MALLOC(*buffer, IMG_SIZE(fmt, width, height), 16);
	if (! buffer) {
		pixfc_log("Unable to allocate aligned memory for buffer\n");
		return -1;
	}

	memset(*buffer, 0x0, IMG_SIZE(fmt, width, height));

	return 0;
}


uint32_t	allocate_unaligned_buffer(PixFcPixelFormat fmt, uint32_t width, uint32_t height, void **buffer) {
	// validate buffer dimensions
	if (validate_image_dimensions(fmt, width, height) != 0) {
		pixfc_log("buffer size error\n");
		return -1;
	}

	//  allocate image buffer
	*buffer = malloc(IMG_SIZE(fmt, width, height));
	if (! buffer) {
		pixfc_log("Unable to allocate unaligned memory for buffer\n");
		return -1;
	}

	return 0;
}


void		fill_image(PixFcPixelFormat fmt, uint32_t buffer_size, void * buf) {
	const PixelFormatDescription *	desc = &pixfmt_descriptions[fmt];
	__m128i * 						buffer = buf;
	uint32_t						index = 0;


	if (desc->is_planar) {
		// Planar format
		__m128i* y_plane, *u_plane, *v_plane;

		// Only supports YUV422P & YUV420P for now - back out if any other format, as we
		// dont know how to handle them yet.
		if (fmt == PixFcYUV422P){
			uint32_t pixel_count = buffer_size * desc->bytes_per_pix_denom / desc->bytes_per_pix_num;

			y_plane = buffer;
			u_plane = (__m128i*) (((uint8_t*) y_plane) + pixel_count);
			v_plane = (__m128i*) (((uint8_t*) u_plane) + pixel_count / 2);

			// Assume 4 fill vectors: 2 for Y plane, 1 for U plane and 1 for V plane
			if (desc->fill_patterns_count != 4) {
				pixfc_log("FIXME !!!! Dont know how to fill '%s' buffer with %u fill pattern vectors\n",
						desc->name, desc->fill_patterns_count);
				return;
			}

			if (((uintptr_t) buffer & 0x0F) == 0) {
				while (pixel_count > 0) {
					_mm_store_si128(y_plane++, _M(desc->fill_patterns[0]));
					_mm_store_si128(y_plane++, _M(desc->fill_patterns[1]));

					_mm_store_si128(u_plane++, _M(desc->fill_patterns[2]));

					_mm_store_si128(v_plane++, _M(desc->fill_patterns[3]));

					// copy 32 pixels at a time
					pixel_count -= 32;
				}
			} else {
				while (pixel_count > 0) {
					_mm_storeu_si128(y_plane++, _M(desc->fill_patterns[0]));
					_mm_storeu_si128(y_plane++, _M(desc->fill_patterns[1]));

					_mm_storeu_si128(u_plane++, _M(desc->fill_patterns[2]));

					_mm_storeu_si128(v_plane++, _M(desc->fill_patterns[3]));

					// copy 32 pixels at a time
					pixel_count -= 32;
				}
			}
		} else if (fmt == PixFcYUV420P){
			uint32_t pixel_count = buffer_size * desc->bytes_per_pix_denom / desc->bytes_per_pix_num;

			y_plane = buffer;
			u_plane = (__m128i*) (((uint8_t*) y_plane) + pixel_count);
			v_plane = (__m128i*) (((uint8_t*) u_plane) + pixel_count / 4);

			// Assume 6 fill vectors: 4 for Y plane, 1 for U plane and 1 for V plane
			if (desc->fill_patterns_count != 6) {
				pixfc_log("FIXME !!!! Dont know how to fill '%s' buffer with %u fill pattern vectors\n",
						desc->name, desc->fill_patterns_count);
				return;
			}

			if (((uintptr_t) buffer & 0x0F) == 0) {
				while (pixel_count > 0) {
					_mm_store_si128(y_plane++, _M(desc->fill_patterns[0]));
					_mm_store_si128(y_plane++, _M(desc->fill_patterns[1]));
					_mm_store_si128(y_plane++, _M(desc->fill_patterns[2]));
					_mm_store_si128(y_plane++, _M(desc->fill_patterns[3]));

					_mm_store_si128(u_plane++, _M(desc->fill_patterns[4]));

					_mm_store_si128(v_plane++, _M(desc->fill_patterns[5]));

					// copy 64 pixels at a time
					pixel_count -= 64;
				}
			} else {
				while (pixel_count > 0) {
					_mm_storeu_si128(y_plane++, _M(desc->fill_patterns[0]));
					_mm_storeu_si128(y_plane++, _M(desc->fill_patterns[1]));
					_mm_storeu_si128(y_plane++, _M(desc->fill_patterns[2]));
					_mm_storeu_si128(y_plane++, _M(desc->fill_patterns[3]));

					_mm_storeu_si128(u_plane++, _M(desc->fill_patterns[2]));

					_mm_storeu_si128(v_plane++, _M(desc->fill_patterns[3]));

					// copy 64 pixels at a time
					pixel_count -= 64;
				}
			}
		} else {
			pixfc_log("FIXME !!!! Dont know how to fill a buffer in '%s' planar image format\n", desc->name);
			return;
		}
	} else {
		// Interleave pixel format
		//
		// Fill in the buffer 16 pixels at a time
		// while alternating the fill patterns
		// FIXME: we should be using row_bytes here...
		if (((uintptr_t) buffer & 0x0F) == 0) {
			while (buffer_size > 0) {
				_mm_store_si128(buffer, _M(desc->fill_patterns[index]));

				// move on to next fill buffer
				index = ((index+1) < desc->fill_patterns_count) ? (index + 1) : 0;

				// copy 16 pixels at a time
				buffer++;
				buffer_size -= 16;
			}
		} else {
			while (buffer_size > 0) {
				_mm_storeu_si128(buffer, _M(desc->fill_patterns[index]));

				// move on to next fill buffer
				index = ((index+1) < desc->fill_patterns_count) ? (index + 1) : 0;

				// copy 16 pixels at a time
				buffer++;
				buffer_size -= 16;
			}
		}
	}
}


int32_t 	get_buffer_from_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void **buffer) {
	uint8_t *						file_buffer;
	struct stat						file_stat;
	int								fd;
	int32_t							bytes_read = 0;

	// validate width, height and pixel format
	if (validate_image_dimensions(fmt, width, height) != 0) {
		pixfc_log("Error validating image dimensions\n");
		return -1;
	}

	// open file
	fd = OPEN(filename, RD_ONLY_FLAG);
	if (fd == -1) {
		pixfc_log("Error opening input file '%s'\n", filename);
		return -1;
	}

	// get file size
	if (fstat(fd, &file_stat) != 0) {
		pixfc_log("Error stat'ing file\n");
		CLOSE(fd);
		return -1;
	}

	// ensure file size is consistent with width, height and pixel format
	if (file_stat.st_size != IMG_SIZE(fmt, width, height))
	{
		pixfc_log("file size not consistent with width, height and image "
				"format: file size %u - expected: %u\n", 
				(uint32_t) file_stat.st_size, IMG_SIZE(fmt, width, height));
		CLOSE(fd);
		return -1;
	}

	// allocate buffer
	ALIGN_MALLOC(file_buffer, file_stat.st_size * sizeof(char), 16);
	if (! file_buffer) {
		pixfc_log("Error allocating memory\n");
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
	pixfc_log("Writing file '%s'...\n", filename_fixed);

	if (rgb_buf_size % stride != 0) {
		pixfc_log("Error saving to PPM: the RGB buffer size not multiple of %u\n", stride);
		return;
	}

	// to speed up things, this routine handles 16 pixels at a time
	// since all conversion routines require input buffers to have
	// a multiple of 16 pixels.
	if (rgb_buf_size % 16 != 0) {
		pixfc_log("Error saving to PPM: the RGB buffer size not multiple of %u\n", stride);
		return;
	}

	// open file
	fd = OPEN(filename_fixed, WR_CREATE_FLAG, RW_PERM);
	if (fd == -1) {
		pixfc_log("Error opening the PPM file for writing\n");
		return;
	}

	// write header
	sprintf(header, "P3\n%d %d\n255\n", width, height);
	if (WRITE(fd, header, strlen(header)) == -1) {
		pixfc_log("Error writing header to PPM file\n");
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
					buffer[18], buffer[19], buffer[20],
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
			pixfc_log("Error saving PPM file: Unknown RGB format %d\n", fmt);
			break;
		}

		if (WRITE(fd, pixel, count) == -1) {
			pixfc_log("Error writing PPM file contents\n");
			break;
		}

		// Move on to next source pixel
		buffer += stride;
	}

	CLOSE(fd);

	pixfc_log("done\n");
}
void write_raw_buffer_to_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void * in) {
	int32_t			fd;
	uint32_t		count = 0;
	uint8_t			*buffer = (uint8_t *) in;
	uint32_t 		buf_size = IMG_SIZE(fmt, width, height);
	char			filename_fixed[256] = {0};

	// append the extension to the given file name
	SNPRINTF(filename_fixed, sizeof(filename_fixed), "%u_%u_%.120s.%s", width, height, filename, pixfmt_descriptions[fmt].name);

	pixfc_log("Writing file '%s'...\n", filename_fixed);

	fd = OPEN(filename_fixed, WR_CREATE_FLAG, RW_PERM);
	if (fd == -1) {
		pixfc_log("Error opening the file for writing\n");
		return;
	}

	while (count < buf_size) {
		int ret = WRITE(fd, (buffer + count), (buf_size - count));
		if (ret < 0) {
			pixfc_log("Error writing PPM file contents\n");
			break;
		}
		count += ret;
	}

	CLOSE(fd);

	pixfc_log("done\n");
}

void 		write_buffer_to_file(PixFcPixelFormat fmt, uint32_t width, uint32_t height, char *filename, void * buffer) {

	// if the buffer is in an RGB format, save it as a PPM file
	if ((fmt == PixFcARGB) || (fmt == PixFcBGRA) || (fmt == PixFcRGB24) || (fmt == PixFcBGR24))
		write_anyrgb_buffer_to_ppm_file(fmt, width, height, filename, buffer);
	else
		write_raw_buffer_to_file(fmt, width, height, filename, buffer);
}

PixFcFlag	synthesize_pixfc_flags(uint32_t index) {
	PixFcFlag flags = PixFcFlag_Default;
	
	// Index valid ?
	if (index >= conversion_blocks_count) {
		pixfc_log("Invalid conversion block index\n");
		return flags;
	}
	
	// Synthesize the flags based on the conversion block's flags
	if (conversion_blocks[index].attributes & NNB_RESAMPLING)
		flags |= PixFcFlag_NNbResamplingOnly;
	
	if (conversion_blocks[index].required_cpu_features == CPUID_FEATURE_NONE)
		flags |= PixFcFlag_NoSSE;
	else if (conversion_blocks[index].required_cpu_features == CPUID_FEATURE_SSE2)
		flags |= PixFcFlag_SSE2Only;
	else if (conversion_blocks[index].required_cpu_features == (CPUID_FEATURE_SSE2 | CPUID_FEATURE_SSSE3))
		flags |= PixFcFlag_SSE2_SSSE3Only;
	
	if (conversion_blocks[index].attributes & BT601_CONVERSION)
		flags |= PixFcFlag_BT601Conversion;
	
	if (conversion_blocks[index].attributes & BT709_CONVERSION)
		flags |= PixFcFlag_BT709Conversion;
	
	return flags;
}

uint32_t	create_pixfc_for_conversion_block(uint32_t index, struct PixFcSSE** pixfc, uint32_t width, uint32_t height) {
	uint32_t	flags = PixFcFlag_Default;
	uint32_t	result;

	// Index valid ?
	if (index >= conversion_blocks_count) {
		pixfc_log("Invalid conversion block index\n");
		return -1;
	}

	// Does the CPU have the required features ?
	if (does_cpu_support(conversion_blocks[index].required_cpu_features) != 0) {
		pixfc_log("Conversion block required CPU features unsupported by CPU\n");
		return -2;
	}

	// Create flags for this conversion block
	flags = synthesize_pixfc_flags(index);

	// Create struct pixfc for this conversion block
	result = create_pixfc(pixfc, conversion_blocks[index].source_fmt, conversion_blocks[index].dest_fmt, width, height, ROW_SIZE(conversion_blocks[index].source_fmt, width), flags);
	if (result != 0) {
		pixfc_log("Error (%d) creating struct pixfc for conversion '%s' %ux%u\n", result, conversion_blocks[index].name, width, height);
		return -3;
	}

	return 0;
}

int32_t	find_conversion_block_index(PixFcPixelFormat src_fmt, PixFcPixelFormat dst_fmt, PixFcFlag flags, uint32_t width, uint32_t height, uint32_t row_bytes) {
	struct PixFcSSE *pixfc = NULL;
	int32_t index = -1;
	uint32_t result;

	result = create_pixfc(&pixfc, src_fmt, dst_fmt, width, height, row_bytes, flags);
	if (result == PixFc_OK) {
		uint32_t i;
		for(i = 0; i < conversion_blocks_count; i++) {
			if ((pixfc->convert == conversion_blocks[i].convert_fn)
					&& (conversion_blocks[i].source_fmt == src_fmt)
					&& (conversion_blocks[i].dest_fmt == dst_fmt)){
				index = i;
				break;
			}
		}

		destroy_pixfc(pixfc);
	} else
		pixfc_log("Error looking for conversion block - create_pixfc returned %d\n", result);

	return index;
}

int32_t	make_conv_block_name_csv_friendly(uint32_t index, char **csv_name)
{
	char src_fmt[32] = {0};
	char dest_fmt[32] = {0};
	char conv_std[32] = {0};
	char sse_features[32] = {0};
	char resampling[32] = {0};
	char csv_friendly_name[255] = {0};
	
	// Split the name
	if (sscanf(conversion_blocks[index].name, "%s to %s - %s - %s - %20c", src_fmt, dest_fmt, conv_std, sse_features, resampling) != 5) {
		pixfc_log("Error parsing conversion block name '%s' to CSV name\n", conversion_blocks[index].name);
		return -1;
	}
	
	SNPRINTF(csv_friendly_name, sizeof(csv_friendly_name), "%s,%s,%s,%s,%s", src_fmt, dest_fmt, conv_std, sse_features, resampling);
	
	*csv_name = STRDUP(csv_friendly_name);
	if (*csv_name == NULL)
		return -1;
	
	return 0;
}


// Return the amount of ticks in nanoseconds elapsed since startup
ticks		getticks() {
#ifdef __linux__

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ticks) (ts.tv_sec * 1000000000LL + (ticks)ts.tv_nsec);

#elif defined(__APPLE__)

	static clock_serv_t		clock_server;
	static uint32_t			valid_clock_server = 0;
	static uint32_t			has_asked_for_clock_server = 0;
	ticks					result = 0;

	// Get clock server if not already done
	if (has_asked_for_clock_server == 0) {
		if (host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &clock_server) == KERN_SUCCESS)
			valid_clock_server = 1;
		else
			pixfc_log("Error getting clock server\n");

		has_asked_for_clock_server = 1;
	}

	// Get time
	if (valid_clock_server == 1) {
		mach_timespec_t ts;
		if (clock_get_time(clock_server, &ts) == KERN_SUCCESS)
			result = ts.tv_sec * 1000000000LL + (ticks) ts.tv_nsec;
		else
			pixfc_log("Error getting time\n");
	}
	return result;

#else	// WIN32

	static LARGE_INTEGER	frequency = { 0 };
	static uint32_t			has_asked_for_frequency = 0;
	LARGE_INTEGER			counter;
	uint64_t				result = 0;

	if (has_asked_for_frequency == 0) {
		if (QueryPerformanceFrequency(&frequency) != TRUE) {
			pixfc_log("Error getting timer frequency\n");
			frequency.QuadPart = 0;
		}
		has_asked_for_frequency = 1;
	}

	if ((frequency.QuadPart != 0) && (QueryPerformanceCounter(&counter) == TRUE))
		result = (uint64_t)(counter.QuadPart * 1000000000LL/ frequency.QuadPart);

	return result;

#endif
}

// Not thread safe !!
#ifndef WIN32
static struct rusage            last_rusage;
#endif

static ticks					last_ticks;
#define TV_TO_NS(tv)			((tv).tv_sec * 1000000000ULL + (uint64_t)(tv).tv_usec * 1000)

void			do_timing(struct timings *timings) {
	if (! timings) {
#ifndef WIN32
		getrusage(RUSAGE_SELF, &last_rusage);
#endif
		last_ticks = getticks();
	} else {
		timings->total_time_ns += getticks() - last_ticks;
#ifndef WIN32
		struct rusage   now;

		if (getrusage(RUSAGE_SELF, &now) == 0) {
			struct timeval user_time;
			struct timeval sys_time;

			timersub(&now.ru_stime, &last_rusage.ru_stime, &sys_time);
			timersub(&now.ru_utime, &last_rusage.ru_utime, &user_time);

			timings->user_time_ns += TV_TO_NS(user_time);
			timings->sys_time_ns += TV_TO_NS(sys_time);
			timings->vcs += (now.ru_nvcsw - last_rusage.ru_nvcsw);
			timings->ivcs += (now.ru_nivcsw - last_rusage.ru_nivcsw);
		} else
			pixfc_log("Error calling getrusage()\n");
#endif
	}
}

