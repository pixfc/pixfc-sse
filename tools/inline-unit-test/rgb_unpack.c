/*
 * rgb_unpack.c
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

#define GENERATE_UNALIGNED_INLINES 0
#include "rgb_unpack.h"

/*
 * Convert 2 vectors of 16 char ARGB to 4 vectors of 8 short AG1, RB1, AG2 & RB2
 *
 * Total latency:				4
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * A1  R1	G1  B1	A2  R2	G2  B2	A3  R3	G3  B3	A4  R4	G4  B4
 *
 * A5  R5	G5  B5	A6  R6	G6  B6	A7  R7	G7  B7	A8  R8	G8  B8
 *
 * OUTPUT:
 *
 * 4 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect3
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 */
void unpack_argb_to_ag_rb_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 0; pixel_count < 8; pixel_count++) {
        out[0] = in[0];
        out[1] = in[2];

        out[8] = in[1];
        out[8 + 1] = in[3];

        in += 4;
        out += 2;

        if (pixel_count == 3)
            out += 8;
    }
}

uint32_t    check_unpack_argb_to_ag_rb_vectors() {
    CHECK_SSE2_SSSE3_INLINE_1IN(unpack_argb_to_ag_rb_vectors_, DECLARE_2_8BIT_VECT, 4, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}


/*
 * Convert 2 vectors of 16 char ARGB to 3 vectors of 8 short:
 *
 * Total latency:				15
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * A1  R1	G1  B1	A2  R2	G2  B2	A3  R3	G3  B3	A4  R4	G4  B4
 *
 * A5  R5	G5  B5	A6  R6	G6  B6	A7  R7	G7  B7	A8  R8	G8  B8
 *
 * OUTPUT:
 *
 * 3 vectors of 8 short
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gb1Vect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * gb2Vect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
void unpack_argb_to_r_g_b_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 0; pixel_count < 8; pixel_count++) {
        out[0] = in[1];
        out[8] = in[2];
        out[16] = in[3];

        in += 4;
        out += 1;
   }
}

uint32_t    check_unpack_argb_to_r_g_b_vectors(){
    CHECK_SSE2_SSSE3_INLINE_1IN(unpack_argb_to_r_g_b_vectors_, DECLARE_2_8BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}


/*
 * Convert 2 vectors of 16 char BGRA to 4 vectors of 8 short GA1, BR1, GA2 & BR2
 *
 * Total latency:				4
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * B1  G1	R1  A1	B2  G2	R2  A2	B3  G3	R3  A3	B4  G4	R4  A4
 *
 * B5  G5	R5  A5	B6  G6	R6  A6	B7  G7	R7  A7	B8  G8	R8  A8
 *
 * OUTPUT:
 *
 * 4 vectors of 8 short
 * gaVect1
 * G1 0		A1 0	G2 0	A2 0	G3 0	A3 0	G4 0	A4 0
 *
 * brVect1
 * B1 0		R1 0	B2 0	R2 0	B3 0	R3 0	B4 0	R4 0
 *
 * gaVect2
 * G5 0		A5 0	G6 0	A6 0	G7 0	A7 0	G8 0	A8 0
 *
 * brVect3
 * B5 0		R5 0	B6 0	R6 0	B7 0	R7 0	B8 0	R8 0
 */
void unpack_bgra_to_ga_br_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 0; pixel_count < 8; pixel_count++) {
        out[0] = in[1];
        out[1] = in[3];

        out[8] = in[0];
        out[8 + 1] = in[2];

        in += 4;
        out += 2;

        if (pixel_count == 3)
            out += 8;
    }
}

uint32_t    check_unpack_bgra_to_ga_br_vectors() {
    CHECK_SSE2_SSSE3_INLINE_1IN(unpack_bgra_to_ga_br_vectors_, DECLARE_2_8BIT_VECT, 4, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}




/*
 * Convert 2 vectors of 16 char BGRA to 3 vectors of 8 short:
 *
 * Total latency:				13
 * Number of pixels handled:	8
 *
 * INPUT
 * 2 vectors of 16 char
 * B1  G1	R1  A1	B2  G2	R2  A2	B3  G3	R3  A3	B4  G4	R4  A4
 *
 * B5  G5	R5  A5	B6  G6	R6  A6	B7  G7	R7  A7	B8  G8	R8  A8
 *
 * OUTPUT:
 *
 * 3 vectors of 8 short
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
void unpack_bgra_to_r_g_b_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 0; pixel_count < 8; pixel_count++) {
        out[0] = in[2];
        out[8] = in[1];
        out[16] = in[0];

        in += 4;
        out += 1;
    }
}

uint32_t    check_unpack_bgra_to_r_g_b_vectors(){
    CHECK_SSE2_SSSE3_INLINE_1IN(unpack_bgra_to_r_g_b_vectors_, DECLARE_2_8BIT_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}


/*
 * Convert 2 vectors of 16 char RGB24 to 7 vectors of 8 short AG1, RB1, AG2 & RB2,
 * AG3, RB3, AG4 & RB4
 *
 * Total latency:				X
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4	G4 B4	R5 G5	B5 R6
 *
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10 B10	R11 G11
 *
 * B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16 G16 B16
 *
 * OUTPUT:
 *
 * 8 vectors of 8 short
 * agVect1
 * A1 0		G1 0	A2 0	G2 0	A3 0	G3 0	A4 0	G4 0
 *
 * rbVect1
 * R1 0		B1 0	R2 0	B2 0	R3 0	B3 0	R4 0	B4 0
 *
 * agVect2
 * A5 0		G5 0	A6 0	G6 0	A7 0	G7 0	A8 0	G8 0
 *
 * rbVect2
 * R5 0		B5 0	R6 0	B6 0	R7 0	B7 0	R8 0	B8 0
 *
 * agVect3
 * A9 0		G9 0	A10 0	G10 0	A11 0	G11 0	A12 0	G12 0
 *
 * rbVect3
 * R9 0		B9 0	R10 0	B10 0	R11 0	B11 0	R12 0	B12 0
 *
 * agVect4
 * A13 0	G13 0	A14 0	G14 0	A15 0	G15 0	A16 0	G16 0
 *
 * rbVect4
 * R13 0	B13 0	R14 0	B14 0	R15 0	B15 0	R16 0	B16 0
 */
void unpack_rgb24_to_ag_rb_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        out[0] = 0;
        out[1] = in[1];

        out[8] = in[0];
        out[8 + 1] = in[2];

        in += 3;
        out += 2;

        if (pixel_count % 4 == 0)
            out += 8;
    }
}

uint32_t    check_unpack_rgb24_to_ag_rb_vectors() {
    CHECK_SSE2_SSSE3_INLINE_1IN(unpack_rgb24_to_ag_rb_vectors_, DECLARE_4_8BIT_VECT, 8, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}



/*
 * Convert 2 vectors of 16 char RGB24 to 6 vectors of 8 short:
 *
 * Total latency:				x
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4	G4 B4	R5 G5	B5 R6
 *
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10 B10	R11 G11
 *
 * B11 R12	G12 B12	R13 G13	B13 R14	G14 B14	R15 G15	B15 R16 G16 B16
 *
 * OUTPUT:
 * 6 vectors of 8 short
  *
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * rVect
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 *
 * gVect
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 *
 * bVect
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 */
void unpack_rgb24_to_r_g_b_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        out[0] = in[0];
        out[8] = in[1];
        out[16] = in[2];

        in += 3;
        out += 1;

        if (pixel_count == 8)
            out += 16;
    }
}

uint32_t    check_unpack_rgb24_to_r_g_b_vectors(){
    CHECK_SSE2_SSSE3_INLINE_1IN(unpack_rgb24_to_r_g_b_vectors_, DECLARE_3_8BIT_VECT, 6, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}

/*
 * Convert 2 vectors of 16 char BGR24 to 7 vectors of 8 short AG1, RB1, AG2 & RB2,
 * AG3, RB3, AG4 & RB4
 *
 * Total latency:               X
 * Number of pixels handled:    16
 *
 * INPUT
 * 3 vectors of 16 char
 * B1 G1    R1 B2   G2 R2   B3 G3   R3 B4   G4 R4   B5 G5   R5 B6
 *
 * G6 R6    B7 G7   R7 B8   G8 R8   B9 G9   R9 B10  G10 R10 B11 G11
 *
 * R11 B12  G12 R12 B13 G13 R13 B14 G14 R14 B15 G15 R15 B16 G16 R16
 *
 * OUTPUT:
 *
 * 8 vectors of 8 short
 * gaVect1
 * G1 0     A1 0    G2 0    A2 0    G3 0    A3 0    G4 0    A4 0
 *
 * brVect1
 * B1 0     R1 0    B2 0    R2 0    B3 0    R3 0    B4 0    R4 0
 *
 * gaVect2
 * G5 0     A5 0    G6 0    A6 0    G7 0    A7 0    G8 0    A8 0
 *
 * brVect2
 * B5 0     R5 0    B6 0    R6 0    B7 0    R7 0    B8 0    R8 0
 *
 * agVect3
 * G9 0     A9 0    G10 0   A10 0   G11 0   A11 0   G12 0   A12 0
 *
 * brVect3
 * B9 0     R9 0    B10 0   R10 0   B11 0   R11 0   B12 0   R12 0
 *
 * gaVect4
 * G13 0    A13 0   G14 0   A14 0   G15 0   A15 0   G16 0   A16 0
 *
 * brVect4
 * B13 0    R13 0   B14 0   R14 0   B15 0   R15 0   B16 0   R16 0
 */
void unpack_bgr24_to_ga_br_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        out[0] = in[1];
        out[1] = 0;

        out[8] = in[0];
        out[8 + 1] = in[2];

        in += 3;
        out += 2;

        if (pixel_count % 4 == 0)
            out += 8;
    }
}

uint32_t    check_unpack_bgr24_to_ga_br_vectors() {
    CHECK_INLINE_1IN(unpack_bgr24_to_ga_br_vectors_scalar, unpack_bgr24_to_ga_br_vectors_sse2, DECLARE_3_8BIT_VECT, 8, MAX_DIFF_8BIT, compare_16bit_output);
    return 0;
}



/*
 * Convert 2 vectors of 16 char BGR24 to 6 vectors of 8 short:
 *
 * Total latency:				x
 * Number of pixels handled:	16
 *
 * INPUT
 * 3 vectors of 16 char
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4	G4 R4	B5 G5	R5 B6
 *
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10 R10	B11 G11
 *
 * R11 B12	G12 R12	B13 G13	R13 B14	G14 R14	B15 G15	R15 B16 G16 R16
 *
 * OUTPUT:
 * 6 vectors of 8 short
 *
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * rVect
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 *
 * gVect
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 *
 * bVect
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 */
void unpack_bgr24_to_r_g_b_vectors_scalar(__m128i* input, __m128i* output) {
    uint8_t*   in = (uint8_t*) input;
    uint16_t*    out = (uint16_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        out[0] = in[2];
        out[8] = in[1];
        out[16] = in[0];

        in += 3;
        out += 1;

        if (pixel_count == 8)
            out += 16;
    }
}

uint32_t    check_unpack_bgr24_to_r_g_b_vectors(){
   CHECK_SSE2_SSSE3_INLINE_1IN(unpack_bgr24_to_r_g_b_vectors_, DECLARE_3_8BIT_VECT, 6, MAX_DIFF_8BIT, compare_16bit_output);
   return 0;
}


/*
 * Unpack 2 r210 vectors (8 pixels) into 3 R, G, B vectors 
 *
 * Total latency:				8
 * Number of pixels handled:	15
 *
 * INPUT
 * 2 vectors of 4 r210 pixels
 *	R1	G1	B1		R2	G2	B2		R3	G3	B3		R4	G4	B4
 *	R5	G5	B5		R6	G6	B6		R7	G7	B7		R8	G8	B8
 *
 *
 * OUTPUT:
 *
 * 3 vectors of 8 short
 * rVect
 * R1  0	R2  0	R3  0	R4  0	R5  0	R6  0	R7  0	R8  0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 */
void unpack_2_r210_to_r_g_b_vectors_scalar(__m128i* in, __m128i* out) {
	uint8_t		le_input[32] = {0};
	uint8_t*	le_in = le_input;
	uint8_t*	input = (uint8_t *) in;
	uint32_t*	input32 = (uint32_t *)le_input;
	uint16_t*	output = (uint16_t *)out;
	uint32_t	index = 0;
	
	// Switch r210 buffer's endianness
	for(index = 0; index < 8; index++) {
		le_in[3] = input[0];
		le_in[2] = input[1];
		le_in[1] = input[2];
		le_in[0] = input[3];
		
		le_in += 4;
		input += 4;
	}
	
	for(index = 0; index < 8; index++) {
		output[0] = (input32[0] >> 20) & 0x3FF;	// R
		output[8] = (input32[0] >> 10) & 0x3FF;	// G
		output[16]= (input32[0]) & 0x3FF;		// B
		input32++;
		output++;
	}
}

uint32_t    check_unpack_2_r210_to_r_g_b_vectors(){
	CHECK_INLINE_1IN(unpack_2_r210_to_r_g_b_vectors_scalar, unpack_2_r210_to_r_g_b_vectors_sse2_ssse3,
			DECLARE_2_RGB_10BIT_LE_VECT, 3, MAX_DIFF_8BIT, compare_16bit_output);
	return 0;
}

