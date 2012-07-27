/*
 * yuv_upsample.c
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

#include "inline-testing-common.h"
#include "yuv_upsample.h"


/*
 * Calculate missing chroma values for the last 8 pix in an image
 * by averaging previous and current chroma values. The last pixel reuses
 * the chroma values of the before-last pix.
 *
 *
 * TOTAL LATENCY:			4
 *
 * INPUT:
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * OUTPUT:
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 */
void reconstruct_last_missing_uv_scalar(__m128i* input, __m128i* output) {
    uint16_t *in = (uint16_t *) input;
    uint16_t *out = (uint16_t *) output;

    out[0] = (in[0] + in [2]) / 2;
    out[1] = (in[1] + in [3]) / 2;

    out[2] = (in[2] + in [4]) / 2;
    out[3] = (in[3] + in [5]) / 2;

    out[4] = (in[4] + in [6]) / 2;
    out[5] = (in[5] + in [7]) / 2;

    out[6] = (in[6] + in [6]) / 2;
    out[7] = (in[7] + in [7]) / 2;
}

uint32_t check_reconstruct_last_missing_uv() {
    CHECK_INLINE_1IN(reconstruct_last_missing_uv_scalar, reconstruct_last_missing_uv_sse2, DECLARE_1_10BIT_VECT, 1, MAX_DIFF_8BIT, compare_16bit_output);
    CHECK_INLINE_1IN(reconstruct_last_missing_uv_scalar, reconstruct_last_missing_uv_sse2_ssse3, DECLARE_1_10BIT_VECT, 1, MAX_DIFF_8BIT, compare_16bit_output);
    CHECK_INLINE_1IN(reconstruct_last_missing_uv_scalar, reconstruct_last_missing_uv_sse2_ssse3_sse41, DECLARE_1_10BIT_VECT, 1, MAX_DIFF_8BIT, compare_16bit_output);

    return 0;
}

/*
 * Calculate missing chroma values by averaging previous and current chroma values
 *
 * TOTAL LATENCY:			4
 *
 * INPUT:
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 * U9 0		V9 0	U11 0 	V11 0	U13 0	V13 0	U15 0	V15 0
 *
 * OUTPUT:
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 */
void reconstruct_missing_uv_scalar(__m128i* input, __m128i *input2, __m128i* output) {
    uint16_t *in = (uint16_t *) input;
    uint16_t *in2 = (uint16_t *) input2;
    uint16_t *out = (uint16_t *) output;

    out[0] = (in[0] + in [2]) / 2;
    out[1] = (in[1] + in [3]) / 2;

    out[2] = (in[2] + in [4]) / 2;
    out[3] = (in[3] + in [5]) / 2;

    out[4] = (in[4] + in [6]) / 2;
    out[5] = (in[5] + in [7]) / 2;

    out[6] = (in[6] + in2 [0]) / 2;
    out[7] = (in[7] + in2 [1]) / 2;
}

uint32_t check_reconstruct_missing_uv() {
    DO_CHECK_INLINE_2IN(reconstruct_missing_uv_scalar, reconstruct_missing_uv_sse2, DECLARE_1_10BIT_VECT, 1, MAX_DIFF_8BIT, compare_16bit_output);
    DO_CHECK_INLINE_2IN(reconstruct_missing_uv_scalar, reconstruct_missing_uv_sse2_ssse3, DECLARE_1_10BIT_VECT, 1, MAX_DIFF_8BIT, compare_16bit_output);
    DO_CHECK_INLINE_2IN(reconstruct_missing_uv_scalar, reconstruct_missing_uv_sse2_ssse3_sse41, DECLARE_1_10BIT_VECT, 1, MAX_DIFF_8BIT, compare_16bit_output);

    return 0;
}


	
