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

float       rgb_8bit_to_yuv_8bit_fr_coef[][3] =     { {0.299, 0.587, 0.114}, {-0.169, -0.331, 0.500}, {0.500, -0.419, -0.081} };
int32_t     rgb_8bit_to_yuv_8bit_fr_off[] =         { 0, 128, 128 };
float       rgb_8bit_to_yuv_8bit_bt601_coef[][3] =  { {0.257, 0.504, 0.098}, {-0.148, -0.291, 0.439}, {0.439, -0.368, -0.071} };
int32_t     rgb_8bit_to_yuv_8bit_bt601_off[] =      { 16, 128, 128 };
float       rgb_8bit_to_yuv_8bit_bt709_coef[][3] =  { {0.183, 0.614, 0.062}, {-0.101, -0.339, 0.439}, {0.439, -0.399, -0.040} };
int32_t     rgb_8bit_to_yuv_8bit_bt709_off[] =      { 16, 128, 128 };

float       rgb_8bit_to_yuv_10bit_fr_coef[][3] =    { {1.196, 2.348, 0.456}, {-0.676, -1.324, 2    }, {2,     -1.676, -0.324} };
int32_t     rgb_8bit_to_yuv_10bit_fr_off[] =        { 0, 512, 512 };
float       rgb_8bit_to_yuv_10bit_bt601_coef[][3] = { {1.028, 2.016, 0.392}, {-0.592, -1.164, 1.756}, {1.756, -1.472, -0.284} };
int32_t     rgb_8bit_to_yuv_10bit_bt601_off[] =     { 64, 512, 512 };
float       rgb_8bit_to_yuv_10bit_bt709_coef[][3] = { {0.732, 2.456, 0.248}, {-0.404, -1.356, 1.756}, {1.756, -1.596, -0.160} };
int32_t     rgb_8bit_to_yuv_10bit_bt709_off[] =     { 64, 512, 512 };


float       yuv_8bit_to_rgb_8bit_fr_coef[][3] =     { {1,     0,     1.4  }, {1,     -0.343, -0.711}, {1,     1.765, 0} };
int32_t     yuv_8bit_to_rgb_8bit_fr_off[] =         { 0, -128, -128 };
float       yuv_8bit_to_rgb_8bit_bt601_coef[][3] =  { {1.164, 0,     1.596}, {1.164, -0.392, -0.813}, {1.164, 2.017, 0} };
int32_t		yuv_8bit_to_rgb_8bit_bt601_off[] =      { -16, -128, -128};
float       yuv_8bit_to_rgb_8bit_bt709_coef[][3] =  { {1.164, 0,     1.793}, {1.164, -0.213, -0.533}, {1.164, 2.112, 0} };
int32_t     yuv_8bit_to_rgb_8bit_bt709_off[] =      { -16, -128, -128};

float       yuv_10bit_to_rgb_8bit_fr_coef[][3] =    { {0.250, 0,     0.350}, {0.250, -0.086, -0.178}, {0.250, 0.441, 0} };
int32_t     yuv_10bit_to_rgb_8bit_fr_off[] =        { 0, -512, -512 };
float       yuv_10bit_to_rgb_8bit_bt601_coef[][3]=  { {0.291, 0,     0.399}, {0.291, -0.098, -0.203}, {0.291, 0.504, 0} };
int32_t     yuv_10bit_to_rgb_8bit_bt601_off[] =     { -64, -512, -512 };
float       yuv_10bit_to_rgb_8bit_bt709_coef[][3] = { {0.291, 0,     0.448}, {0.291, -0.053, -0.133}, {0.291, 0.528, 0} };
int32_t     yuv_10bit_to_rgb_8bit_bt709_off[] =     { -64, -512, -512 };


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
