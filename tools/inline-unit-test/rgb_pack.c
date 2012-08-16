/*
 * rgb_pack.c
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
#include "rgb_pack.h"

/* 
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 4 BGRA vectors
 *
 * Total latency:			11 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 *
 * OUTPUT:
 *
 * B1 G1	R1 A1	B2 G2	R2 A2	B3 G3	R3 A3	B4 G4	R4 A4
 * B5 G5	R5 A5	B6 G6	R6 A6	B7 G7	R7 A7	B8 G8	R8 A8
 * B9 G9	R9 A9	B10 G10	R10 A10	B11 G11	R11 A11	B12 G12	R12 A12
 * B13 G13	R13 A13	B14 G14	R14 A14	B15 G15	R15 A15	B16 G16	R16 A16
 */
void pack_6_rgb_vectors_in_4_bgra_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t*   in = (uint16_t*) input;
    uint8_t*    out = (uint8_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        *out++ = (uint8_t) (CLIP_PIXEL(in[16]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[8]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[0]));
        *out++ = 0;

        in++;
        if (pixel_count == 8)
            in += 16;
    }
};

uint32_t    check_pack_6_rgb_vectors_in_4_bgra_vectors() {
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_4_bgra_vectors_scalar, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, DECLARE_6_8BIT_VECT, 4, MAX_DIFF_PACKING, compare_8bit_output);
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_4_bgra_vectors_scalar, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, DECLARE_6_10BIT_VECT, 4, MAX_DIFF_PACKING, compare_8bit_output);

    return 0;
}


/*
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 4 ARGB vectors
 *
 * Total latency:			11 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * R9 0		R10 0	R11 0	R12 0	R13 0	R14 0	R15 0	R16 0
 * G9 0		G10 0	G11 0	G12 0	G13 0	G14 0	G15 0	G16 0
 * B9 0		B10 0	B11 0	B12 0	B13 0	B14 0	B15 0	B16 0
 *
 *
 * OUTPUT
 *
 * A1 R1	G1 B1	A2 R2	G2 B2	A3 R3	G3 B3	A4 R4	G4 B4
 * A5 R5	G5 B5	A6 R6	G6 B6	A7 R7	G7 B7	A8 R8	G8 B8
 * A9 R9	G9 B9	A10 R10	G10 B10	A11 R11	G11 B11	A12 R12	G12 B12
 * A13 R13	G13 B13	A14 R14	G14 B14	A15 R15	G15 B15	A16 R16	G16 B16
 *
 */
void pack_6_rgb_vectors_in_4_argb_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t*   in = (uint16_t*) input;
    uint8_t*    out = (uint8_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        *out++ = 0;
        *out++ = (uint8_t) (CLIP_PIXEL(in[0]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[8]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[16]));

        in++;
        if (pixel_count == 8)
            in += 16;
    }
}

uint32_t    check_pack_6_rgb_vectors_in_4_argb_vectors() {
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_4_argb_vectors_scalar, pack_6_rgb_vectors_in_4_argb_vectors_sse2, DECLARE_6_8BIT_VECT, 4, MAX_DIFF_PACKING, compare_8bit_output);
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_4_argb_vectors_scalar, pack_6_rgb_vectors_in_4_argb_vectors_sse2, DECLARE_6_10BIT_VECT, 4, MAX_DIFF_PACKING, compare_8bit_output);
    return 0;
}



/*
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 3 RGB24 vectors of 16 char
 *
 * Total latency:			18 cycles
 * Num of pixel handled:	16 pixels
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * R1 0     R2 0    R3 0    R4 0    R5 0    R6 0    R7 0    R8 0
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * R9 0     R10 0   R11 0   R12 0   R13 0   R14 0   R15 0   R16 0
 * G9 0     G10 0   G11 0   G12 0   G13 0   G14 0   G15 0   G16 0
 * B9 0     B10 0   B11 0   B12 0   B13 0   B14 0   B15 0   B16 0
 *
 * OUTPUT
 *
 * 3 vectors of 16 chars
 *
 * R1 G1	B1 R2	G2 B2	R3 G3	B3 R4 	G4 B4	R5 G5	B5 R6
 * G6 B6	R7 G7	B7 R8	G8 B8	R9 G9	B9 R10	G10B10	R11G11
 * B11R12	G12B12	R13G13	B13R14	G14B14	R15G15	B15R16	G16B16
 */
void pack_6_rgb_vectors_in_3_rgb24_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t*   in = (uint16_t*) input;
    uint8_t*    out = (uint8_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        *out++ = (uint8_t) (CLIP_PIXEL(in[0]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[8]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[16]));

        in++;
        if (pixel_count == 8)
            in += 16;
    }
}

uint32_t check_6_rgb_vectors_in_3_rgb24_vectors() {
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_rgb24_vectors_scalar, pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, DECLARE_6_8BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_rgb24_vectors_scalar, pack_6_rgb_vectors_to_3_rgb24_vectors_sse2_slowpacking, DECLARE_6_10BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);


    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_rgb24_vectors_scalar, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, DECLARE_6_8BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_rgb24_vectors_scalar, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, DECLARE_6_10BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);
    return 0;
}

/*
 * Pack R1, G1, B1 and R2, G2, B2 vectors of signed short
 * in 3 BGR24 vectors of 16 char
 *
 * Total latency:			18 cycles
 * Num of pixel handled:	16 pixels
 *
 * INPUT:
 *
 * 6 vectors of 8 short:
 * R1 0     R2 0    R3 0    R4 0    R5 0    R6 0    R7 0    R8 0
 * G1 0     G2 0    G3 0    G4 0    G5 0    G6 0    G7 0    G8 0
 * B1 0     B2 0    B3 0    B4 0    B5 0    B6 0    B7 0    B8 0
 *
 * R9 0     R10 0   R11 0   R12 0   R13 0   R14 0   R15 0   R16 0
 * G9 0     G10 0   G11 0   G12 0   G13 0   G14 0   G15 0   G16 0
 * B9 0     B10 0   B11 0   B12 0   B13 0   B14 0   B15 0   B16 0
 *
 * OUTPUT
 *
 * 3 vectors of 16 chars
 *
 * B1 G1	R1 B2	G2 R2	B3 G3	R3 B4 	G4 R4	B5 G5	R5 B6
 * G6 R6	B7 G7	R7 B8	G8 R8	B9 G9	R9 B10	G10R10	B11G11
 * R11B12	G12R12	B13G13	R13B14	G14R14	B15G15	R15B16	G16R16
 */
void pack_6_rgb_vectors_in_3_bgr24_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t*   in = (uint16_t*) input;
    uint8_t*    out = (uint8_t*) output;
    uint32_t    pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        *out++ = (uint8_t) (CLIP_PIXEL(in[16]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[8]));
        *out++ = (uint8_t) (CLIP_PIXEL(in[0]));

        in++;
        if (pixel_count == 8)
            in += 16;
    }
}

uint32_t check_6_rgb_vectors_in_3_bgr24_vectors() {
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_bgr24_vectors_scalar, pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, DECLARE_6_8BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_bgr24_vectors_scalar, pack_6_rgb_vectors_to_3_bgr24_vectors_sse2_slowpacking, DECLARE_6_10BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);


    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_bgr24_vectors_scalar, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, DECLARE_6_8BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);
    CHECK_INLINE_1IN(pack_6_rgb_vectors_in_3_bgr24_vectors_scalar, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, DECLARE_6_10BIT_VECT, 3, MAX_DIFF_PACKING, compare_8bit_output);

    return 0;
}


/*
 * Pack 3 R, G, B vectors (8 pixels) into 2 r210 vectors
 *
 * Total latency:				16
 * Number of pixels handled:	15
 *
 * INPUT
 * 3 vectors of 8 short
 * rVect
 * R1		R2		R3		R4  	R5  	R6  	R7  	R8  
 *
 * gVect
 * G1 		G2		G3		G4		G5		G6		G7		G8 
 *
 * bVect
 * B1 		B2		B3		B4		B5		B6		B7		B8 
 *
 *
 * OUTPUT:
 * 2 vectors of 4 r210 pixels
 *  R1	G1	B1		R2	G2	B2		R3	G3	B3		R4	G4	B4
 *  R5	G5	B5		R6	G6	B6		R7	G7	B7		R8	G8	B8
 *
 */
void pack_3_r_g_b_vectors_to_2_r210_scalar(__m128i* input, __m128i* output) {
	uint32_t temp;
	uint16_t *in = (uint16_t *) input;
	uint8_t *in8 = (uint8_t *) &temp;
	uint8_t *out8 = (uint8_t *) output;
	uint32_t index;
	
	for(index = 0; index < 8; index++) {
		temp = ((uint32_t)in[16] & 0x3FF);		// B
		temp |= (((uint32_t)in[8] & 0x3FF) << 10);// G
		temp |= (((uint32_t)in[0] & 0x3FF) << 20);// R

		out8[3] = in8[0];
		out8[2] = in8[1];
		out8[1] = in8[2];
		out8[0] = in8[3];
		
		out8 += 4;
		in++;
	}	
}

uint32_t	check_pack_3_r_g_b_vectors_to_2_r210() {
   CHECK_INLINE_1IN(pack_3_r_g_b_vectors_to_2_r210_scalar, pack_3_r_g_b_vectors_to_2_r210_sse2_ssse3, DECLARE_3_10BIT_VECT, 2, MAX_DIFF_PACKING, compare_10bit_be_output);
	
	return 0;
}

/*
 * Pack 3 R, G, B vectors (8 pixels) into 2 r210 vectors
 *
 * Total latency:				32
 * Number of pixels handled:	16
 *
 * INPUT
 * 6 vectors of 8 short
 * R1 		R2		R3		R4		R5		R6		R7		R8
 * G1 		G2		G3		G4		G5		G6		G7		G8
 * B1 		B2		B3		B4		B5		B6		B7		B8
 *
 * R9 		R10 	R11 	R12 	R13 	R14 	R15 	R16
 * G9 		G10 	G11 	G12 	G13		G14 	G15 	G16
 * B9 		B10 	B11 	B12 	B13 	B14 	B15 	B16
 *
 *
 * OUTPUT:
 * 2 vectors of 4 r210 pixels
 *  R1	G1	B1		R2	G2	B2		R3	G3	B3		R4	G4	B4
 *  R5	G5	B5		R6	G6	B6		R7	G7	B7		R8	G8	B8
 *  R9	G9	B9		R10	G10	B10		R11	G11	B11		R12	G12	B12
 *  R13	G13	B13		R14	G14	B14		R15	G15	B15		R16	G16	B16
 *
 */
void pack_6_r_g_b_vectors_to_4_r210_scalar(__m128i* input, __m128i* output) {
	uint32_t temp;
	uint16_t *in = (uint16_t *) input;
	uint8_t *in8 = (uint8_t *) &temp;
	uint8_t *out8 = (uint8_t *) output;
	uint32_t index;
	
	for(index = 1; index <= 16; index++) {
		temp = ((uint32_t)in[16] & 0x3FF);		// B
		temp |= (((uint32_t)in[8] & 0x3FF) << 10);// G
		temp |= (((uint32_t)in[0] & 0x3FF) << 20);// R
		
		out8[3] = in8[0];
		out8[2] = in8[1];
		out8[1] = in8[2];
		out8[0] = in8[3];
		
		out8 += 4;
		in++;
		
		if (index == 8)
			in += 16;
	}	
}

uint32_t	check_pack_6_r_g_b_vectors_to_4_r210() {
	CHECK_INLINE_1IN(pack_6_r_g_b_vectors_to_4_r210_scalar, pack_6_r_g_b_vectors_to_4_r210_sse2_ssse3, DECLARE_6_10BIT_VECT, 4, MAX_DIFF_PACKING, compare_10bit_be_output);
	
	return 0;
}

/*
 * Pack 6 R, G, B vectors (16 pixels) into 4 r10k vectors
 *
 * Total latency:				19
 * Number of pixels handled:	8
 *
 * INPUT
 * 3 vectors of 8 short
 * rVect
 * R1		R2		R3		R4  	R5  	R6  	R7  	R8
 *
 * gVect
 * G1 		G2		G3		G4		G5		G6		G7		G8
 *
 * bVect
 * B1 		B2		B3		B4		B5		B6		B7		B8
 *
 *
 * OUTPUT:
 * 2 vectors of 4 r210 pixels
 *  R1	G1	B1		R2	G2	B2		R3	G3	B3		R4	G4	B4
 *  R5	G5	B5		R6	G6	B6		R7	G7	B7		R8	G8	B8
 *
 */
void pack_3_r_g_b_vectors_to_2_r10k_scalar(__m128i* input, __m128i* output) {
	uint32_t temp;
	uint16_t *in = (uint16_t *) input;
	uint8_t *in8 = (uint8_t *) &temp;
	uint8_t *out8 = (uint8_t *) output;
	uint32_t index;

	for(index = 0; index < 8; index++) {
		temp = (((uint32_t)in[16] & 0x3FF) << 2); // B
		temp |= (((uint32_t)in[8] & 0x3FF) << 12);// G
		temp |= (((uint32_t)in[0] & 0x3FF) << 22);// R

		out8[3] = in8[0];
		out8[2] = in8[1];
		out8[1] = in8[2];
		out8[0] = in8[3];

		out8 += 4;
		in++;
	}
}


uint32_t	check_pack_3_r_g_b_vectors_to_2_r10k() {
   CHECK_INLINE_1IN(pack_3_r_g_b_vectors_to_2_r10k_scalar, pack_3_r_g_b_vectors_to_2_r10k_sse2_ssse3, DECLARE_3_10BIT_VECT, 2, MAX_DIFF_PACKING, compare_10bit_be_output);

	return 0;
}

/*
 * Pack 3 R, G, B vectors (8 pixels) into 2 r10k vectors
 *
 * Total latency:				38
 * Number of pixels handled:	16
 *
 * INPUT
 * 6 vectors of 8 short
 * R1 		R2		R3		R4		R5		R6		R7		R8
 * G1 		G2		G3		G4		G5		G6		G7		G8
 * B1 		B2		B3		B4		B5		B6		B7		B8
 *
 * R9 		R10 	R11 	R12 	R13 	R14 	R15 	R16
 * G9 		G10 	G11 	G12 	G13		G14 	G15 	G16
 * B9 		B10 	B11 	B12 	B13 	B14 	B15 	B16
 *
 *
 * OUTPUT:
 * 2 vectors of 4 r210 pixels
 *  R1	G1	B1		R2	G2	B2		R3	G3	B3		R4	G4	B4
 *  R5	G5	B5		R6	G6	B6		R7	G7	B7		R8	G8	B8
 *  R9	G9	B9		R10	G10	B10		R11	G11	B11		R12	G12	B12
 *  R13	G13	B13		R14	G14	B14		R15	G15	B15		R16	G16	B16
 *
 */
void 	pack_6_r_g_b_vectors_to_4_r10k_scalar(__m128i* input, __m128i* output) {
	uint32_t temp;
	uint16_t *in = (uint16_t *) input;
	uint8_t *in8 = (uint8_t *) &temp;
	uint8_t *out8 = (uint8_t *) output;
	uint32_t index;

	for(index = 1; index <= 16; index++) {
		temp = (((uint32_t)in[16] & 0x3FF) << 2); // B
		temp |= (((uint32_t)in[8] & 0x3FF) << 12);// G
		temp |= (((uint32_t)in[0] & 0x3FF) << 22);// R

		out8[3] = in8[0];
		out8[2] = in8[1];
		out8[1] = in8[2];
		out8[0] = in8[3];

		out8 += 4;
		in++;

		if (index == 8)
			in += 16;
	}
}

uint32_t	check_pack_6_r_g_b_vectors_to_4_r10k() {
	CHECK_INLINE_1IN(pack_6_r_g_b_vectors_to_4_r10k_scalar, pack_6_r_g_b_vectors_to_4_r10k_sse2_ssse3, DECLARE_6_10BIT_VECT, 4, MAX_DIFF_PACKING, compare_10bit_be_output);

	return 0;
}
