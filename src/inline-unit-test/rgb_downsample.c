/*
 * rgb_downsample.c
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


#include "inline-testing-common.h"
#include "rgb_downsample.h"

/*
 * Create 3 422 downsampled R, G, B vectors from 3 R, G, B vectors
 * using nearest neighbour interpolation
 *
 * INPUT:
 * 4 vectors of 8 short
 * agVect1
 * A1 0     G1 0    A2 0    G2 0    A3 0    G3 0    A4 0    G4 0
 *
 * rbVect1
 * R1 0     B1 0    R2 0    B2 0    R3 0    B3 0    R4 0    B4 0
 *
 * agVect2
 * A5 0     G5 0    A6 0    G6 0    A7 0    G7 0    A8 0    G8 0
 *
 * rbVect3
 * R5 0     B5 0    R6 0    B6 0    R7 0    B7 0    R8 0    B8 0
 *
 * OUTPUT:
 * 2 vectors of 8 short:
 * agVect
 * A1 0     G1 0    A3 0    G3 0    A5 0    G5 0    A7 0    G7 0
 *
 * rbVect
 * R1 0     B1 0    R3 0    B3 0    R5 0    B5 0    R7 0    B7 0
 */
static void nnb_422_downsample_ag_rb_vectors_scalar(__m128i *input, __m128i* output) {
	uint16_t*	in = (uint16_t*) input;
	uint16_t*	out = (uint16_t*) output;

	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[4];
	out[3] = in[5];
	out[4] = in[16];
	out[5] = in[17];
	out[6] = in[20];
	out[7] = in[21];

	out[8] = in[8];
	out[9] = in[9];
	out[10] = in[12];
	out[11] = in[13];
	out[12] = in[24];
	out[13] = in[25];
	out[14] = in[28];
	out[15] = in[29];
}

uint32_t	check_nnb_422_downsample_ag_rb_vectors() {
    CHECK_SSE2_SSSE3_INLINE_1IN(nnb_422_downsample_ag_rb_vectors_, DECLARE_4_8BIT_VECT, 2, 0, compare_16bit_output);
    CHECK_SSE2_SSSE3_INLINE_1IN(nnb_422_downsample_ag_rb_vectors_, DECLARE_4_10BIT_VECT, 2, 0, compare_16bit_output);

	return 0;
}


/*
 * Create 3 422 downsampled R, G, B vectors from 3 R, G, B vectors
 * using nearest neighbour interpolation
 *
 * TOTAL LATENCY:   6
 *
 * INPUT:
 * 3 vectors of 8 short
 * rVect
 * R1  0    R2  0   R3  0   R4  0   R5  0   R6  0   R7  0   R8  0
 *
 * gVect
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 *
 * bVect
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * OUTPUT:
 * 3 vectors of 8 short
 * rVect
 * R1 0 R1 0    R3 0 R3 0   R5 0 R5 0   R7 0 R7 0
 *
 * gVect
 * G1 0 G1 0    G3 0 G3 0   G5 0 G5 0   G7 0 G7 0
 *
 * bVect
 * B1 0 B1 0    B3 0 B3 0   B5 0 B5 0   B7 0 B7 0
 */
void  nnb_422_downsample_r_g_b_vectors_scalar(__m128i* input, __m128i *output) {
    uint16_t*   in = (uint16_t*) input;
    uint16_t*   out = (uint16_t*) output;
	uint8_t		index = 0;

	for(index = 0; index < 8; index += 2) {
		out[index] = in[index];
		out[index + 1] = in[index];

		out[index + 8] = in[index + 8];
		out[index + 9] = in[index + 8];

		out[index + 16] = in[index + 16];
		out[index + 17] = in[index + 16];
	}
}

uint32_t	check_nnb_422_downsample_r_g_b_vectors() {
    CHECK_SSE2_SSSE3_INLINE_1IN(nnb_422_downsample_r_g_b_vectors_, DECLARE_3_8BIT_VECT, 3, 0, compare_16bit_output);
    CHECK_SSE2_SSSE3_INLINE_1IN(nnb_422_downsample_r_g_b_vectors_, DECLARE_3_10BIT_VECT, 3, 0, compare_16bit_output);

	return 0;
}

/*
 * Create 3 422 downsampled R, G, B vectors from 6 AG, RB vectors
 * using a simple 3-tap average filter. Also, copy the contents of the
 * current vectors in the previous ones
 *
 * TOTAL LATENCY:   20
 *
 * INPUT:
 * 6 vectors of 8 short
 * previous agVect2
 * A5 0     G5 0    A6 0    G6 0    A7 0    G7 0    A8 0    G8 0
 *
 * previous rbVect3
 * R5 0     B5 0    R6 0    B6 0    R7 0    B7 0    R8 0    B8 0
 *
 * current agVect1
 * A1 0     G1 0    A2 0    G2 0    A3 0    G3 0    A4 0    G4 0
 *
 * current rbVect1
 * R1 0     B1 0    R2 0    B2 0    R3 0    B3 0    R4 0    B4 0
 *
 * current agVect2
 * A5 0     G5 0    A6 0    G6 0    A7 0    G7 0    A8 0    G8 0
 *
 * current rbVect3
 * R5 0     B5 0    R6 0    B6 0    R7 0    B7 0    R8 0    B8 0
 *
 * OUTPUT:
 * 2 vectors of 8 short
 * agVect
 * A12 0    G12 0   A34 0   G34 0   A56 0   G56 0   A78 0   G78 0
 *
 * rbVect
 * R12 0    B12 0   R34 0   B34 0   R56 0   B56 0   R78 0   B78 0
 */
void  avg_422_downsample_ag_rb_vectors_n_save_previous_scalar(__m128i* input, __m128i* input_output, __m128i *output) {
    uint16_t*   in = (uint16_t*) input;
	uint16_t	prev_a = ((uint16_t*) input_output)[6];
    uint16_t	prev_g = ((uint16_t*) input_output)[7];
	uint16_t	prev_r = ((uint16_t*) input_output)[8 + 6];
	uint16_t	prev_b = ((uint16_t*) input_output)[8 + 7];
    uint16_t*   out = (uint16_t*) output;
    uint8_t     out_pixel = 0;

	for(out_pixel = 0; out_pixel < 4; out_pixel++) {
		// A
		// TODO: we should really be setting this to 0
		out[0] = ( ((prev_a + in[2]) / 2) + in[0]) / 2; 
		prev_a = in[2];
		
		// G
		out[1] = ( ((prev_g + in[3]) / 2) + in[1]) / 2;
		prev_g = in[3];

		// R
		out[8] = ( ((prev_r + in[8 + 2]) / 2) + in[8]) / 2;
		prev_r = in[8 + 2];

		// B
		out[8 + 1] = ( ((prev_b + in[8 + 3]) / 2) + in[8 + 1]) / 2;
		prev_b = in[8 + 3];

		out += 2;
		in += 4;
		
		if (out_pixel % 2 == 1)
			in += 8;
	}

	input_output[0] = input[2];
	input_output[1] = input[3];
}

uint32_t check_avg_422_downsample_ag_rb_vectors_n_save_previous() {
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_ag_rb_vectors_n_save_previous_, DECLARE_4_8BIT_VECT, DECLARE_2_8BIT_VECT, 2, MAX_DIFF_8BIT, compare_16bit_output, 2);
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_ag_rb_vectors_n_save_previous_, DECLARE_4_10BIT_VECT, DECLARE_2_10BIT_VECT, 2, MAX_DIFF_8BIT, compare_16bit_output, 2);

    return 0;
}


/*
 * Create 3 422 downsampled R, G, B vectors from 6 R, G, B vectors
 * using a simple 3-tap average filter. Also, copy the contents of the
 * current vectors in the previous ones
 *
 * TOTAL LATENCY:   27
 *
 * INPUT:
 * 3 vectors of 8 short
 * previous rVect
 * R1  0    R2  0   R3  0   R4  0   R5  0   R6  0   R7  0   R8  0
 *
 * previous gVect
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 *
 * previous bVect
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * 3 vectors of 8 short
 * current rVect
 * R1  0    R2  0   R3  0   R4  0   R5  0   R6  0   R7  0   R8  0
 *
 * current gVect
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 *
 * current bVect
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * OUTPUT:
 * 3 vectors of 8 short
 * rVect
 * R1 0     R1 0    R3 0    R3  0   R5 0    R5  0   R7 0    R7 0
 *
 * gVect
 * G1 0     G1 0    G3 0    G3 0    G5 0    G5  0   G7 0    G7  0
 *
 * bVect
 * B1 0     B1 0    B3 0    B3 0    B5 0    B5  0   B7 0    B7  0
 */
void  avg_422_downsample_r_g_b_vectors_n_save_previous_scalar(__m128i* input, __m128i* previous, __m128i *output) {
    uint16_t*   in = (uint16_t*) input;
    uint16_t    prev_r = ((uint16_t*) previous)[7];
    uint16_t    prev_g = ((uint16_t*) previous)[8 + 7];
    uint16_t    prev_b = ((uint16_t*) previous)[16 + 7];
    uint16_t*   out = (uint16_t*) output;
    uint8_t     out_pixel = 0;

    for(out_pixel = 0; out_pixel < 4; out_pixel++) {
        // R
        out[0] = ( ((prev_r + in[1]) / 2) + in[0]) / 2;
        prev_r = in[1];
        out[1] = out[0];

        // G
        out[8] = ( ((prev_g + in[8 + 1]) / 2) + in[8]) / 2;
        prev_g = in[8 + 1];
        out[8 + 1] = out[8];

        // G
        out[16] = ( ((prev_b + in[16 + 1]) / 2) + in[16]) / 2;
        prev_b = in[16 + 1];
        out[16 + 1] = out[16];

        out += 2;
        in += 2;
    }

    previous[0] = input[0];
    previous[1] = input[1];
    previous[2] = input[2];
}

uint32_t check_avg_422_downsample_r_g_b_vectors_n_save_previous() {
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_r_g_b_vectors_n_save_previous_, DECLARE_3_8BIT_VECT, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output, 1);
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_r_g_b_vectors_n_save_previous_, DECLARE_3_10BIT_VECT, DECLARE_3_10BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output, 1);

    return 0;
}

/*
 * Create 3 422 downsampled R, G, B vectors from the first 3 R, G, B vectors in an image
 * using a simple 3-tap average filter. Since there is not previous pixel to average with,
 * this method uses the first pixel as the previous one.
 *
 * TOTAL LATENCY:   24
 *
 * INPUT:
 *
 * 3 vectors of 8 short
 * current rVect
 * R1  0    R2  0   R3  0   R4  0   R5  0   R6  0   R7  0   R8  0
 *
 * current gVect
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 *
 * current bVect
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * OUTPUT:
 * 3 vectors of 8 short
 * rVect
 * R1 0     R1 0    R3 0    R3  0   R5 0    R5  0   R7 0    R7 0
 *
 * gVect
 * G1 0     G1 0    G3 0    G3 0    G5 0    G5  0   G7 0    G7  0
 *
 * bVect
 * B1 0     B1 0    B3 0    B3 0    B5 0    B5  0   B7 0    B7  0
 */
void  avg_422_downsample_first_r_g_b_vectors_n_save_previous_scalar(__m128i* input,  __m128i* previous, __m128i *output) {
    uint16_t*   in = (uint16_t*) input;
    uint16_t    prev_r = in[0];
    uint16_t    prev_g = in[8];
    uint16_t    prev_b = in[16];
    uint16_t*   out = (uint16_t*) output;
    uint8_t     out_pixel = 0;

    for(out_pixel = 0; out_pixel < 4; out_pixel++) {
        // R
        out[0] = ( ((prev_r + in[1]) / 2) + in[0]) / 2;
        prev_r = in[1];
        out[1] = out[0];

        // G
        out[8] = ( ((prev_g + in[8 + 1]) / 2) + in[8]) / 2;
        prev_g = in[8 + 1];
        out[8 + 1] = out[8];

        // G
        out[16] = ( ((prev_b + in[16 + 1]) / 2) + in[16]) / 2;
        prev_b = in[16 + 1];
        out[16 + 1] = out[16];

        out += 2;
        in += 2;
    }

    previous[0] = input[0];
    previous[1] = input[1];
    previous[2] = input[2];
}

uint32_t check_avg_422_downsample_first_r_g_b_vectors_n_save_previous() {
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_first_r_g_b_vectors_n_save_previous_, DECLARE_3_8BIT_VECT, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output, 1);
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_first_r_g_b_vectors_n_save_previous_, DECLARE_3_10BIT_VECT, DECLARE_3_10BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output, 1);


    return 0;
}

/*
 * Create 3 422 downsampled R, G, B vectors from 4 AG, RB vectors
 * using a simple 3-tap average filter. Since there is not previous pixel to average with,
 * this method uses the first pixel as the previous one.
 *
 * TOTAL LATENCY:   18
 *
 * INPUT:
 * 6 vectors of 8 short
 *
 * current agVect1
 * A1 0     G1 0    A2 0    G2 0    A3 0    G3 0    A4 0    G4 0
 *
 * current rbVect1
 * R1 0     B1 0    R2 0    B2 0    R3 0    B3 0    R4 0    B4 0
 *
 * current agVect2
 * A5 0     G5 0    A6 0    G6 0    A7 0    G7 0    A8 0    G8 0
 *
 * current rbVect3
 * R5 0     B5 0    R6 0    B6 0    R7 0    B7 0    R8 0    B8 0
 *
 * OUTPUT:
 * 2 vectors of 8 short
 * agVect
 * A12 0    G12 0   A34 0   G34 0   A56 0   G56 0   A78 0   G78 0
 *
 * rbVect
 * R12 0    B12 0   R34 0   B34 0   R56 0   B56 0   R78 0   B78 0
 */
void  avg_422_downsample_first_ag_rb_vectors_n_save_previous_scalar(__m128i* input, __m128i* previous, __m128i *output) {
    uint16_t*   in = (uint16_t*) input;
    uint16_t    prev_a = in[0];
    uint16_t    prev_g = in[1];
    uint16_t    prev_r = in[8];
    uint16_t    prev_b = in[8 + 1];
    uint16_t*   out = (uint16_t*) output;
    uint8_t     out_pixel = 0;

    for(out_pixel = 0; out_pixel < 4; out_pixel++) {
        // A
        // TODO: we should really be setting this to 0
        out[0] = ( ((prev_a + in[2]) / 2) + in[0]) / 2;
        prev_a = in[2];

        // G
        out[1] = ( ((prev_g + in[3]) / 2) + in[1]) / 2;
        prev_g = in[3];

        // R
        out[8] = ( ((prev_r + in[8 + 2]) / 2) + in[8]) / 2;
        prev_r = in[8 + 2];

        // B
        out[8 + 1] = ( ((prev_b + in[8 + 3]) / 2) + in[8 + 1]) / 2;
        prev_b = in[8 + 3];

        out += 2;
        in += 4;

        if (out_pixel % 2 == 1)
            in += 8;
    }

    previous[0] = input[2];
    previous[1] = input[3];
}

uint32_t  check_avg_422_downsample_first_ag_rb_vectors_n_save_previous() {
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_first_ag_rb_vectors_n_save_previous_, DECLARE_4_8BIT_VECT, DECLARE_2_8BIT_VECT, 2, MAX_DIFF_8BIT, compare_16bit_output, 2);
    CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(avg_422_downsample_first_ag_rb_vectors_n_save_previous_, DECLARE_4_10BIT_VECT, DECLARE_2_10BIT_VECT, 2, MAX_DIFF_8BIT, compare_16bit_output, 2);


    return 0;
}

/*
 * Create 3 420 downsampled R, G, B vectors from 6 R, G, B vectors.
 *
 * TOTAL LATENCY:   12
 *
 * INPUT:
 * 3 vectors of 8 short
 * Line 1 rVect
 * R1  0    R2  0   R3  0   R4  0   R5  0   R6  0   R7  0   R8  0
 *
 * Line 1 gVect
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 *
 * Line 1 bVect
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * 3 vectors of 8 short
 * Line 2 rVect
 * R1  0    R2  0   R3  0   R4  0   R5  0   R6  0   R7  0   R8  0
 *
 * Line 2 gVect
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 *
 * Line 2 bVect
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * OUTPUT:
 * 3 vectors of 8 short
 * rVect
 * R1 0     R1 0    R3 0    R3  0   R5 0    R5  0   R7 0    R7 0
 *
 * gVect
 * G1 0     G1 0    G3 0    G3 0    G5 0    G5  0   G7 0    G7  0
 *
 * bVect
 * B1 0     B1 0    B3 0    B3 0    B5 0    B5  0   B7 0    B7  0
 */
void  avg_420_downsample_r_g_b_vectors_scalar(__m128i* input1, __m128i* input2, __m128i *output) {
    uint16_t*   in1 = (uint16_t*) input1;
    uint16_t*   in2 = (uint16_t*) input2;
    uint16_t*   out = (uint16_t*) output;
    uint8_t     out_pixel = 0;

    for(out_pixel = 0; out_pixel < 4; out_pixel++) {
        out[0] = (in1[0] + in1[1] + in2[0] + in2[1]) / 4;
        out[1] = out[0];

        out[8] = (in1[8] + in1[8 + 1] + in2[8] + in2[8 + 1]) / 4;
        out[8 + 1] = out[8];

        out[16] = (in1[16] + in1[16 + 1] + in2[16] + in2[16 + 1]) / 4;
        out[16 + 1] = out[16];

        in1 += 2;
        in2 += 2;
        out += 2;
    }
}

uint32_t check_avg_420_downsample_r_g_b_vectors() {
    CHECK_SSE2_SSSE3_INLINE_2IN(avg_420_downsample_r_g_b_vectors_, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output);
    CHECK_SSE2_SSSE3_INLINE_2IN(avg_420_downsample_r_g_b_vectors_, DECLARE_3_10BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output);

    return 0;
}

/*
 * Create 2 420 downsampled AG, RB vectors from 8 AG, RB vectors
 *
 * TOTAL LATENCY:   14
 *
 * INPUT:
 * 8 vectors of 8 short
 *
 * line1 agVect1
 * A1 0     G1 0    A2 0    G2 0    A3 0    G3 0    A4 0    G4 0
 *
 * line1 rbVect1
 * R1 0     B1 0    R2 0    B2 0    R3 0    B3 0    R4 0    B4 0
 *
 * line1 agVect2
 * A5 0     G5 0    A6 0    G6 0    A7 0    G7 0    A8 0    G8 0
 *
 * line1 rbVect2
 * R5 0     B5 0    R6 0    B6 0    R7 0    B7 0    R8 0    B8 0
 *
 * line2 agVect1
 * A1 0     G1 0    A2 0    G2 0    A3 0    G3 0    A4 0    G4 0
 *
 * line2 rbVect1
 * R1 0     B1 0    R2 0    B2 0    R3 0    B3 0    R4 0    B4 0
 *
 * line2 agVect2
 * A5 0     G5 0    A6 0    G6 0    A7 0    G7 0    A8 0    G8 0
 *
 * line2 rbVect2
 * R5 0     B5 0    R6 0    B6 0    R7 0    B7 0    R8 0    B8 0
 *
 * OUTPUT:
 * 2 vectors of 8 short
 * agVect
 * A12 0    G12 0   A34 0   G34 0   A56 0   G56 0   A78 0   G78 0
 *
 * rbVect
 * R12 0    B12 0   R34 0   B34 0   R56 0   B56 0   R78 0   B78 0
 */
void  avg_420_downsample_ag_rb_vectors_scalar(__m128i* input1, __m128i* input2, __m128i *output) {
    uint16_t*   in1 = (uint16_t*) input1;
    uint16_t*   in2 = (uint16_t*) input2;
    uint16_t*   out = (uint16_t*) output;
    uint8_t     out_pixel = 0;

    for(out_pixel = 0; out_pixel < 4; out_pixel++) {
        // A
        out[0] = (in1[0] + in1[2] + in2[0] + in2[2]) / 4;

        // G
        out[1] = (in1[1] + in1[3] + in2[1] + in2[3]) / 4;

        // R
        out[8] = (in1[8] + in1[8 + 2] + in2[8] + in2[8 + 2]) / 4;

        // B
        out[8 + 1] = (in1[8 + 1] + in1[8 + 3] + in2[8 + 1] + in2[8 + 3]) / 4;

        in1 += 4;
        in2 += 4;
        out += 2;

        if (out_pixel % 2 == 1) {
            in1 += 8;
            in2 += 8;
        }
    }
}

uint32_t check_avg_420_downsample_ag_rb_vectors() {
    CHECK_SSE2_SSSE3_INLINE_2IN(avg_420_downsample_ag_rb_vectors_, DECLARE_4_8BIT_VECT, 2, MAX_DIFF_8BIT, compare_16bit_output);
    CHECK_SSE2_SSSE3_INLINE_2IN(avg_420_downsample_ag_rb_vectors_, DECLARE_4_10BIT_VECT, 2, MAX_DIFF_8BIT, compare_16bit_output);

    return 0;
}



