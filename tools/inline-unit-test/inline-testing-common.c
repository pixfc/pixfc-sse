/*
 * inline-testing-common.c
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

#include <stdint.h>
#include "inline-testing-common.h"


#ifdef DEBUG
void	print_xmm8u_array(uint32_t count, char *prefix, void* array) {
	uint32_t	index = 0;
	char		title[256] = {0};
	__m128i*	data = (__m128i*) array;

	for(index = 0; index < count; index++){
		sprintf(title, "%s %u", prefix, index);
		print_xmm8u(title, &data[index]);
	}
}

void	print_xmm10leu_array(uint32_t count, char *prefix, void* array) {
	uint32_t	index = 0;
	char		title[256] = {0};
	__m128i*	data = (__m128i*) array;

	for(index = 0; index < count; index++){
		sprintf(title, "%s %u", prefix, index);
		print_xmm10u(title, &data[index]);
	}
}

void	print_xmm10beu_array(uint32_t count, char *prefix, void* array) {
	uint32_t	index = 0;
	char		title[256] = {0};
	__m128i *	data = (__m128i*) array;
	__m128i		data_le = {0x0LL, 0x0LL};
	uint8_t	*	in8;
	uint8_t *	out8;
	
	for(index = 0; index < count; index++){
		int i;
		sprintf(title, "BE %s %u", prefix, index);
		in8 = (uint8_t *)&data[index];
		out8 = (uint8_t *)&data_le;
		for(i = 0; i < 4; i++) {
			out8[3] = in8[0];
			out8[2] = in8[1];
			out8[1] = in8[2];
			out8[0] = in8[3];
			out8 += 4;
			in8 += 4;
		}
		print_xmm10u(title, &data_le);
	}
}


void	print_xmm16u_array(uint32_t count, char *prefix, void* array) {
	uint32_t	index = 0;
	char		title[256] = {0};
	__m128i*	data = (__m128i*) array;

	for(index = 0; index < count; index++){
		sprintf(title, "%s %u", prefix, index);
		print_xmm16u(title, &data[index]);
	}
}

void	print_xmm16_array(uint32_t count, char *prefix, void* array) {
	uint32_t	index = 0;
	char		title[256] = {0};
	__m128i*	data = (__m128i*) array;
	
	for(index = 0; index < count; index++){
		sprintf(title, "%s %u", prefix, index);
		print_xmm16(title, &data[index]);
	}
}

#endif

void   compare_8bit_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t sse_out_count, uint32_t max_diff, char *prefix) {
	uint8_t* scalar = (uint8_t*) scalar_out;
	uint8_t* sse = (uint8_t*) sse_out;
	uint8_t  index;
	for(index = 0; index < 16 * sse_out_count; index++) {
		if (check_last != 0) {
             if (((check_last > 0) && (16 - (index % 16) > check_last))
                    || ((check_last < 0) && ((index % 16) > (-check_last))))
    			continue;

        }

        if (abs(scalar[index] - sse[index]) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ %hhu in '%s' vector %u differs by %u: sse: %hhu - scalar: %hhu\n", (unsigned char)(index % 16), check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 16), abs(sse[index]-scalar[index]), sse[index], scalar[index]);
			print_xmm8u("SSE   ", (__m128i*) &sse[index / 16]);
			print_xmm8u("Scalar", (__m128i*) &scalar[index / 16]);
			break;
		}
	}
}

void   compare_10bit_be_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t sse_out_count, uint32_t max_diff, char *prefix) {
#ifndef WIN32
	__m128i scalar_out_le[sse_out_count];
	__m128i sse_out_le[sse_out_count];
#else
	// hmm this may blow on windows ...
	__m128i scalar_out_le[10];
	__m128i sse_out_le[10];
#endif
	uint8_t *in8;
	uint8_t *out8;
	uint32_t index;
	
	// convert scalar_out to little endian
	in8 = (uint8_t *)scalar_out;
	out8 = (uint8_t *)scalar_out_le;
	for(index = 0; index < 4 * 4 * sse_out_count; index += 4) {
		out8[3] = in8[0];
		out8[2] = in8[1];
		out8[1] = in8[2];
		out8[0] = in8[3];
		out8 += 4;
		in8 += 4;
	}
	
	// convert sse_out to little endian
	in8 = (uint8_t *)sse_out;
	out8 = (uint8_t *)sse_out_le;
	for(index = 0; index < 4 * 4 * sse_out_count; index += 4) {
		out8[3] = in8[0];
		out8[2] = in8[1];
		out8[1] = in8[2];
		out8[0] = in8[3];
		out8 += 4;
		in8 += 4;
	}

	compare_10bit_le_output(check_last, scalar_out_le, sse_out_le, sse_out_count, max_diff, prefix);
}

void   compare_10bit_le_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t sse_out_count, uint32_t max_diff, char *prefix) {
	uint32_t* scalar = (uint32_t*) scalar_out;
	uint32_t* sse = (uint32_t*) sse_out;
    uint32_t  scalar_val, sse_val;
	uint8_t  index;

    // there are 12 10bit values per __m128i vector
	for(index = 0; index < sse_out_count * 12; index += 3) {
		if (check_last != 0) {
             if (((check_last > 0) && (12 - (index % 12) > check_last))
                    || ((check_last < 0) && ((index % 12) > (-check_last))))
    			continue;   
        }

        scalar_val = *scalar & 0x3FF;
        sse_val = *sse & 0x3FF;
        if (abs(scalar_val - sse_val) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ 0 in '%s' vector %u differs by %u: sse: %u - scalar: %u\n", check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 12), abs(scalar_val - sse_val), (unsigned int)sse_val, (unsigned int)scalar_val);
			print_xmm10u("SSE   ", (__m128i*) &sse[index / 12]);
			print_xmm10u("Scalar", (__m128i*) &scalar[index / 12]);
			break;
		}

        scalar_val = (*scalar >>10) & 0x3FF;
        sse_val = (*sse >> 10) & 0x3FF;
        if (abs(scalar_val - sse_val) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ 1 in '%s' vector %u differs by %u: sse: %u - scalar: %u\n", check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 12), abs(scalar_val - sse_val), (unsigned int)sse_val, (unsigned int)scalar_val);
			print_xmm10u("SSE   ", (__m128i*) &sse[index / 12]);
			print_xmm10u("Scalar", (__m128i*) &scalar[index / 12]);
			break;
		}

        scalar_val = (*scalar >>20) & 0x3FF;
        sse_val = (*sse >> 20) & 0x3FF;
        if (abs(scalar_val - sse_val) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ 2 in '%s' vector %u differs by %u: sse: %u - scalar: %u\n", check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 12), abs(scalar_val - sse_val), (unsigned int)sse_val, (unsigned int)scalar_val);
			print_xmm10u("SSE   ", (__m128i*) &sse[index / 12]);
			print_xmm10u("Scalar", (__m128i*) &scalar[index / 12]);
			break;
		}

        scalar++;
        sse++;
	}
}


void  compare_16bit_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t sse_out_count, uint32_t max_diff, char *prefix){
	int16_t* scalar = (int16_t*) scalar_out;
	int16_t* sse = (int16_t*) sse_out;
	uint8_t  index;
	for(index = 0; index < 8 * sse_out_count; index++) {
        if (check_last != 0) {
             if (((check_last > 0) && (8 - (index % 8) > check_last))
                    || ((check_last < 0) && ((index % 8) > (-check_last))))
    			continue;

        }	
		if (abs(scalar[index] - sse[index]) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ %hhu in '%s' vector %u differs by %u: sse: %hd - scalar: %hd\n", (unsigned char)(index % 8), check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 8), abs(sse[index]-scalar[index]), sse[index], scalar[index]);
			print_xmm16("SSE   ", (__m128i*) &sse[index / 8]);
			print_xmm16("Scalar", (__m128i*) &scalar[index / 8]);
			break;
		}
	}
}
