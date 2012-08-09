/*
 * yuv_pack.c
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
#include "yuv_pack.h"


/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to 2 vectors YUYV
 *
 * Total latency:			4 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 *
 * Y1 U12	Y2 V12		Y3 U34		Y4 V34		Y5 U56		Y6 V56		Y7 U78		Y8 V78
 * Y9 U910	Y10 V910	Y11 U1112	Y12 V1112	Y13 U1314	Y14 V1314	Y15 U1516	Y16 V1516
 *
 */
void pack_4_y_uv_422_vectors_in_2_yuyv_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t *in = (uint16_t*) input;
    uint8_t *out = (uint8_t*) output;
    uint32_t pixel_count;

    for(pixel_count = 2; pixel_count <= 16; pixel_count += 2) {
        *out++ = CLIP_PIXEL(in[0]);
        *out++ = CLIP_PIXEL(in[8]);
        *out++ = CLIP_PIXEL(in[1]);
        *out++ = CLIP_PIXEL(in[8 + 1]);
        in += 2;

        if(pixel_count == 8)
            in += 8;
    }
}

uint32_t    check_pack_4_y_uv_422_vectors_in_2_yuyv_vectors() {
    CHECK_INLINE_1IN(pack_4_y_uv_422_vectors_in_2_yuyv_vectors_scalar, pack_4_y_uv_422_vectors_in_2_yuyv_vectors_sse2, DECLARE_4_8BIT_VECT, 2, MAX_DIFF_8BIT, compare_8bit_output);

    return 0;
}


/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to 2 vectors UYVY
 *
 * Total latency:			4 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 *
 * U12 Y1 	V12 Y2		U34 Y3 		V34 Y4		U56 Y5		V56 Y6		U78 Y7		V78 Y8
 * U910 Y9	V910 Y10	U1112 Y11	V1112 Y12	U1314 Y13	V1314 Y14	U1516 Y15	V1516 Y16
 *
 */
void pack_4_y_uv_422_vectors_in_2_uyvy_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t *in = (uint16_t*) input;
    uint8_t *out = (uint8_t*) output;
    uint32_t pixel_count;

    for(pixel_count = 2; pixel_count <= 16; pixel_count += 2) {
        *out++ = CLIP_PIXEL(in[8]);
        *out++ = CLIP_PIXEL(in[0]);
        *out++ = CLIP_PIXEL(in[8 + 1]);
        *out++ = CLIP_PIXEL(in[1]);
        in += 2;

        if(pixel_count == 8)
            in += 8;
    }
}

uint32_t    check_pack_4_y_uv_422_vectors_in_2_uyvy_vectors() {
    CHECK_INLINE_1IN(pack_4_y_uv_422_vectors_in_2_uyvy_vectors_scalar, pack_4_y_uv_422_vectors_in_2_uyvy_vectors_sse2, DECLARE_4_8BIT_VECT, 2, MAX_DIFF_8BIT, compare_8bit_output);

    return 0;
}


/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to YUV422p - the lowest 4 bytes are
 * filled in the U and V planes
 *
 *
 * Total latency:			6 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 *
 * OUTPUT:
 *
 * 3 vectors of 16 char
 *
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 *
 * U1 U2	U3 U4	U5 U6	U7 U8	0 0		0 0		0 0		0 0
 *
 * V1 V2	V3 V4	V5 V6	V7 V8	0 0		0 0		0 0		0 0
 */
void pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_scalar( __m128i* input, __m128i* output_y, __m128i* output_u, __m128i* output_v) {
    uint16_t *in = (uint16_t*) input;
    uint8_t *out_y = (uint8_t*) output_y;
    uint8_t *out_u = (uint8_t*) output_u;
    uint8_t *out_v = (uint8_t*) output_v;
    uint32_t pixel_count;

    for(pixel_count = 2; pixel_count <= 16; pixel_count += 2) {
        *out_y++ = CLIP_PIXEL(in[0]);
        out_u[8] = 0;
        *out_u++ = CLIP_PIXEL(in[8]);
        *out_y++ = CLIP_PIXEL(in[1]);
        out_v[8] = 0;
        *out_v++ = CLIP_PIXEL(in[8 + 1]);
        in += 2;

        if(pixel_count == 8)
            in += 8;
    }
}

uint32_t    check_pack_4_y_uv_422_vectors_in_yuvp_lo_vectors() {
    CHECK_INLINE_1IN_3OUT(pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_scalar, pack_4_y_uv_422_vectors_to_yuvp_lo_vectors_sse2, DECLARE_4_8BIT_VECT, 1, MAX_DIFF_8BIT, compare_8bit_output, 0, 0);

    return 0;
}



/*
 * Pack 2 pairs of 422 downsampled Y, UV vectors to YUV422p - the highest 4 bytes are
 * filled in the U and V planes (lowest ones remain untouched)
 *
 *
 * Total latency:			6 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect1
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 * uvVect2
 * U910 0	V910 0	U1112 0	V1112 0	U1314 0	V1314 0	U1516 0	V1516 0
 *
 *
 * OUTPUT:
 *
 * 3 vectors of 16 char
 *
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 *
 * x x		x x		x x		x x		U1 U2	U3 U4	U5 U6	U7 U8
 *
 * x x		x x		x x		x x		V1 V2	V3 V4	V5 V6	V7 V8
 *
 */
void pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_scalar(__m128i* input, __m128i* output_y, __m128i* output_u, __m128i* output_v) {
    uint16_t *in = (uint16_t*) input;
    uint8_t *out_y = (uint8_t*) output_y;
    uint8_t *out_u = (uint8_t*) output_u;
    uint8_t *out_v = (uint8_t*) output_v;
    uint32_t pixel_count;

    out_u += 8;
    out_v += 8;

    for(pixel_count = 2; pixel_count <= 16; pixel_count += 2) {
        *out_y++ = CLIP_PIXEL(in[0]);
        *out_u++ = CLIP_PIXEL(in[8]);
        *out_y++ = CLIP_PIXEL(in[1]);
        *out_v++ = CLIP_PIXEL(in[8 + 1]);
        in += 2;

        if(pixel_count == 8)
            in += 8;
    }
}

uint32_t    check_pack_4_y_uv_422_vectors_in_yuvp_hi_vectors() {
    CHECK_INLINE_1IN_3OUT(pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_scalar, pack_4_y_uv_422_vectors_to_yuvp_hi_vectors_sse2, DECLARE_4_8BIT_VECT, 1, MAX_DIFF_8BIT, compare_8bit_output, 0, 8);
  return 0;
}




/*
 * Pack 2 pairs of Y to one Y vector
 *
 *
 * Total latency:			1 cycle
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 2 vectors of 8 short
 *
 * yVect1
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * yVect2
 * Y9 0		Y10 0	Y11 0	Y12 0	Y13 0	Y14 0	Y15 0	Y16 0
 *
 *
 *
 * OUTPUT:
 *
 * 1 vector of 16 char
 *
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 *
 */
void pack_2_y_vectors_to_1_y_vector_scalar(__m128i* in_y1, __m128i* in_y2, __m128i* output) {
    uint16_t *in = (uint16_t*) in_y1;
    uint8_t *out = (uint8_t*) output;
    uint32_t pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        *out++ = CLIP_PIXEL(*in);
        in++;
        if (pixel_count == 8)
            in = (uint16_t*) in_y2;
    }
}

uint32_t    check_pack_2_y_vectors_to_1_y_vector() {
    DO_CHECK_INLINE_2IN(pack_2_y_vectors_to_1_y_vector_scalar, pack_2_y_vectors_to_1_y_vector_sse2, DECLARE_1_8BIT_VECT, 1, 0, compare_8bit_output);
    return 0;
}


/*
 * Pack 4 UV vectors UV vectors to U plane and V plane
 *
 * Total latency:			8 cycles
 * Num of pixel handled:	16
 *
 * INPUT:
 *
 * 4 vectors of 8 short
 *
 * uvVect1
 * U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4 0
 *
 * uvVect2
 * U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8 0
 *
 * uvVect3
 * U9 0		V9 0	U10 0	V10 0	U11 0	V11 0	U12 0	V12 0
 *
 * uvVect4
 * U13 0	V13 0	U14 0	V14 0	U15 0	V15 0	U16 0	V16 0
 *
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 *
 *
 * U1 U2	U3 U4	U5 U6	U7 U8	U9 U10	U11	U12	U13	U14	U15	U16
 *
 * V1 V2	V3 V4	V5 V6	V7 V8	V9 V10	V11 V12	V13	V14	V14	V16
 *
 */
void pack_4_uv_vectors_to_yup_vectors_scalar(__m128i* input, __m128i* output_u, __m128i* output_v) {
    uint16_t *in = (uint16_t*) input;
    uint8_t *out_u = (uint8_t*) output_u;
    uint8_t *out_v = (uint8_t*) output_v;
    uint32_t pixel_count;

    for(pixel_count = 1; pixel_count <= 16; pixel_count++) {
        *out_u++ = CLIP_PIXEL(in[0]);
        *out_v++ = CLIP_PIXEL(in[1]);
        in+= 2;
    }
}

uint32_t    check_pack_4_uv_vectors_to_yup_vectors() {
    CHECK_INLINE_1IN_2OUT(pack_4_uv_vectors_to_yup_vectors_scalar, pack_4_uv_vectors_to_yup_vectors_sse2, DECLARE_4_8BIT_VECT, 1, 0, compare_8bit_output, 0, 0);
    return 0;
}


/*
 * Pack 6 vectors (Y, UV) into 4 v210 vectors (24 pixels) 
 *
 * Total latency:				18 (sse41) / 22 (ssse3)
 * Number of pixels handled:	24
 *
 * INPUT
 * 6 vectors of 8 short
 * yVect1
 * Y0		Y1		Y2		Y3		Y4		Y5		Y6		Y7
 *
 * uvVect1
 * U01		V01		U23		V23		U45		V45		U67		V67
 *
 * yVect2
 * Y8		Y9		Y10		Y11		Y12		Y13		Y14		Y15
 *
 * uvVect2
 * U89		V89		U1011	V1011	U1213	V1213	U1415	V1516
 *
 * yVect3
 * Y16		Y17		Y18		Y19		Y20		Y21		Y22		Y23
 *
 * uvVect3
 * U1617	V1617	U1819	V1819	U2021	V2021	U2223	V2223
 *
 *
 * OUTPUT:
 * 4 vectors of 24 v210 pixels
 * U01		Y0		V01		Y1	... U45	  Y4	V45	    Y5
 * U67		Y6		V67		Y7	... U1011 Y10	V1011	Y11
 * U1213	Y12		V1213	Y13	... U1617 Y16	V1617	Y17
 * U1819	Y18		V1819	Y19	... U2223 Y22	V2223	Y23 
 *
 *
 */
//DEFINE_PACK_4_V210_VECTORS(sse2_ssse3_sse41);
//DEFINE_PACK_4_V210_VECTORS(sse2_ssse3);
void    pack_6_y_uv_vectors_to_4_v210_vectors_scalar(__m128i* input, __m128i* output) {
    uint16_t *in = (uint16_t*) input;
    uint32_t *out = (uint32_t*) output;
    uint32_t pixel_count = 0;

    while(pixel_count < 24) {
        *out = CLIP_10BIT_PIXEL(in[8]) & 0x3FF;
        *out |= (CLIP_10BIT_PIXEL(in[0]) & 0x3FF) << 10;
        *(out++) |= (CLIP_10BIT_PIXEL(in[8 + 1]) & 0x3FF) << 20;
        *out = CLIP_10BIT_PIXEL(in[1]) & 0x3FF;
        
        pixel_count += 2;
        in += 2;
        if (pixel_count % 8 == 0)
            in += 8;
        
        *out |= (CLIP_10BIT_PIXEL(in[8]) & 0x3FF) << 10;
        *(out++) |= (CLIP_10BIT_PIXEL(in[0]) & 0x3FF) << 20;
        *out = CLIP_10BIT_PIXEL(in[8 + 1]) & 0x3FF;
        *out |= (CLIP_10BIT_PIXEL(in[1]) & 0x3FF) << 10;

        pixel_count += 2;
        in += 2;
        if (pixel_count % 8 == 0)
            in += 8;
        
        *(out++) |= (CLIP_10BIT_PIXEL(in[8]) & 0x3FF) << 20;
        *out = CLIP_10BIT_PIXEL(in[0]) & 0x3FF;
        *out |= (CLIP_10BIT_PIXEL(in[8 + 1]) & 0x3FF) << 10;
        *(out++) |= (CLIP_10BIT_PIXEL(in[1]) & 0x3FF) << 20;

        pixel_count += 2;
        in += 2;
        if (pixel_count % 8 == 0)
            in += 8;
    }
}

uint32_t    check_pack_6_y_uv_vectors_to_4_v210_vectors() {
    CHECK_INLINE_1IN(pack_6_y_uv_vectors_to_4_v210_vectors_scalar, pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3, DECLARE_6_10BIT_VECT, 4, MAX_DIFF_8BIT, compare_10bit_le_output);
    CHECK_INLINE_1IN(pack_6_y_uv_vectors_to_4_v210_vectors_scalar, pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3_sse41, DECLARE_6_10BIT_VECT, 4, MAX_DIFF_8BIT, compare_10bit_le_output);
//    do { 
//		__declspec(align(16)) const __int64 input[(6)][2]={
//			{ 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL }, 
//			{ 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL }, 
//			{ 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL }, 
//			{ 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL }, 
//			{ 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL }, 
//			{ 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL },
//		};
//		__m128i scalar_out[4];
//		__m128i sse_out[4]; 
//		do { 
//			fprintf ((&__iob_func()[2]), "[ %s:%-3d ] " "Checking " "pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3_sse41" "\n", strrchr("..\\..\\..\\tools\\inline-unit-test\\yuv_pack.c", '\\')+1, 441 ); fflush((&__iob_func()[2])); 
//		} while(0); 
//		print_xmm16u_array((sizeof(input)/sizeof((input)[0])), "INPUT", ((__m128i *)(&(input[0])))); 
//		pack_6_y_uv_vectors_to_4_v210_vectors_scalar(((__m128i *)(&(input[0]))), scalar_out); 
//		print_xmm10leu_array((sizeof(scalar_out)/sizeof((scalar_out)[0])), "SCALAR OUT", scalar_out); 
//		pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3_sse41(((__m128i *)(&(input[0]))), sse_out); 
//		print_xmm10leu_array((sizeof(sse_out)/sizeof((sse_out)[0])), "SSE OUT", sse_out); 
//		compare_10bit_le_output(0, scalar_out, sse_out, 4, 2, "pack_6_y_uv_vectors_to_4_v210_vectors_sse2_ssse3_sse41"); 
//	} while (0);


    return 0;
}


