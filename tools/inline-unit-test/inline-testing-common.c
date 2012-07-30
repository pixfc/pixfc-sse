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
			print_xmm8u("SSE   ", (__m128i*) &sse[(index / 16) * 16]);
			print_xmm8u("Scalar", (__m128i*) &scalar[(index / 16) * 16]);
			break;
		}
	}
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
			dprintf("Value @ 0 in '%s' vector %u differs by %u: sse: %u - scalar: %u\n", check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 12), abs(scalar - sse), (unsigned int)sse_val, (unsigned int)scalar_val);
			print_xmm10u("SSE   ", (__m128i*) &sse[(index / 12) * 12]);
			print_xmm10u("Scalar", (__m128i*) &scalar[(index / 12) * 12]);
			break;
		}

        scalar_val = (*scalar >>10) & 0x3FF;
        sse_val = (*sse >> 10) & 0x3FF;
        if (abs(scalar_val - sse_val) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ 1 in '%s' vector %u differs by %u: sse: %u - scalar: %u\n", check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 12), abs(scalar - sse), (unsigned int)sse_val, (unsigned int)scalar_val);
			print_xmm10u("SSE   ", (__m128i*) &sse[(index / 12) * 12]);
			print_xmm10u("Scalar", (__m128i*) &scalar[(index / 12) * 12]);
			break;
		}

        scalar_val = (*scalar >>20) & 0x3FF;
        sse_val = (*sse >> 20) & 0x3FF;
        if (abs(scalar_val - sse_val) > max_diff) {
			dprintf("== %s\n", prefix);
			dprintf("Value @ 2 in '%s' vector %u differs by %u: sse: %u - scalar: %u\n", check_last == 0 ? "OUTPUT" : "PREVIOUS", (index / 12), abs(scalar - sse), (unsigned int)sse_val, (unsigned int)scalar_val);
			print_xmm10u("SSE   ", (__m128i*) &sse[(index / 12) * 12]);
			print_xmm10u("Scalar", (__m128i*) &scalar[(index / 12) * 12]);
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
			print_xmm16("SSE   ", (__m128i*) &sse[(index / 8) * 8]);
			print_xmm16("Scalar", (__m128i*) &scalar[(index / 8) * 8]);
			break;
		}
	}
}
