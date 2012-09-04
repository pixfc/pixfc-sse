/*
 * conversion_routines_from_r210_scalar.c
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public  License as published by the
 * Free Software Foundation, either version 3 of the License, or
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
#include "pixfc-sse.h"
#include "rgb_conversion_recipes.h"

#define UNPACK_RGB(r, g, b, src, src_fmt) \
	if (src_fmt == PixFcR210) {\
		uint32_t tmp;\
		uint8_t *p = (uint8_t *) &tmp;\
		p[3] = *src++;\
		p[2] = *src++;\
		p[1] = *src++;\
		p[0] = *src++;\
		b = tmp & 0x3ff;\
		g = (tmp >> 10) & 0x3ff;\
		r = (tmp >> 20) & 0x3ff;\
		/* dprint("R: %d G: %d B: %d\n", r, g, b); */\
	} else if (src_fmt == PixFcR10k) {\
		uint32_t tmp;\
		uint8_t *p = (uint8_t *) &tmp;\
		p[3] = *src++;\
		p[2] = *src++;\
		p[1] = *src++;\
		p[0] = *src++;\
		b = (tmp >> 2) & 0x3ff;\
		g = (tmp >> 12) & 0x3ff;\
		r = (tmp >> 22) & 0x3ff;\
		/* dprint("R: %d G: %d B: %d\n", r, g, b); */\
	} else {\
		printf("unknown rgb source format\n");\
	}



#define PACK_RGB(r, g, b, dst, dest_fmt) \
	if (dest_fmt == PixFcARGB) {\
		/* dprint("R: %d G: %d B: %d\n", CLIP_PIXEL(r), CLIP_PIXEL(g), CLIP_PIXEL(b)); */\
		*(dst++) = 0;\
		*(dst++) = CLIP_PIXEL(r);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(b);\
	} else if (dest_fmt == PixFcBGRA) {\
		*(dst++) = CLIP_PIXEL(b);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(r);\
		*(dst++) = 0;\
	} else  if (dest_fmt == PixFcRGB24) {\
		*(dst++) = CLIP_PIXEL(r);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(b);\
	} else if (dest_fmt == PixFcBGR24) {\
		*(dst++) = CLIP_PIXEL(b);\
		*(dst++) = CLIP_PIXEL(g);\
		*(dst++) = CLIP_PIXEL(r);\
	} else {\
		printf("unknown rgb destination format\n");\
	}

#define PACK_YUV422(y1, u, y2, v, dst, dest_fmt) do {\
			if (dest_fmt == PixFcYUYV) {\
				*(dst++) = CLIP_PIXEL(y1);\
				*(dst++) = CLIP_PIXEL(u);\
				*(dst++) = CLIP_PIXEL(y2);\
				*(dst++) = CLIP_PIXEL(v);\
			} else if (dest_fmt == PixFcUYVY) {\
				*(dst++) = CLIP_PIXEL(u);\
				*(dst++) = CLIP_PIXEL(y1);\
				*(dst++) = CLIP_PIXEL(v);\
				*(dst++) = CLIP_PIXEL(y2);\
			} else if (dest_fmt == PixFcYUV422P) {\
				*(y_plane++) = CLIP_PIXEL(y1);\
				*(y_plane++) = CLIP_PIXEL(y2);\
				*(u_plane++) = CLIP_PIXEL(u);\
				*(v_plane++) = CLIP_PIXEL(v);\
			} else {\
				printf("Unknown output format in non-SSE conversion from RGB\n");\
			}\
		} while(0)

/*
 *
 * 		R 2 1 0
 *
 * 		T O
 *
 * 		R G B 3 2  /  2 4
 *
 */
// This conversion makes no assumption on the number of pixels
void		convert_10bit_rgb_to_any_rgb_nonsse(const struct PixFcSSE* pixfc, void* in, void* out) {
	PixFcPixelFormat 	src_fmt = pixfc->source_fmt;
	PixFcPixelFormat 	dst_fmt = pixfc->dest_fmt;
	uint32_t 			pixel = 0;
	uint32_t			line = 0;
	uint8_t*			src = (uint8_t *) in;
	uint8_t*			dst = (uint8_t *) out;
	uint32_t			src_row_byte_count = ROW_SIZE(src_fmt, pixfc->width);
	int32_t				r = 0, g = 0, b = 0;
	while(line++ < pixfc->height){
		while(pixel < pixfc->width) {
			UNPACK_RGB(r, g, b, src, src_fmt);
			r >>= 2;
			g >>= 2;
			b >>= 2;
			PACK_RGB(r, g, b, dst, dst_fmt);
			pixel += 1;
		}
		src = (uint8_t*)in + line * src_row_byte_count;
		pixel = 0;
	}
}

