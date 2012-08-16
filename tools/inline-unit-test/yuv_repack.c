/*
 * yuv_repack.c
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


#define GENERATE_UNALIGNED_INLINES 0
#include "yuv_repack.h"



/*
 * Convert YUV422P to YUYV:
 * 4 vectors of 16 char: 2xY, 1xU, 1xV to 4 vectors of 16 char YUYV
 *
 * Total latency:				6
 * Number of pixels handled:	32
 *
 * INPUT
 * 4 vectors of 16 char
 * y_input
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 * Y17 Y18	Y19 Y20	Y21 Y22	Y23 Y24	Y25 Y26	Y27 Y28 Y29 Y30	Y31 Y32
 *
 * u_input
 * U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14	U15 U16
 *
 * v_input
 * V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14	V15 V16
 *
 *
 * OUTPUT:
 *
 * 4 vectors of 16 char
 * Y1 U1	Y2 V1	Y3 U2	Y4 V2	Y5 U3	Y6 V3	Y7 U4	Y8 V4
 * Y9 U5	Y10 V5	Y11 U6	Y12 V6	Y13 U7	Y14 V7	Y15 U8	Y16 V8
 * Y17 U9	Y18 V9	Y19 U10	Y20 V10	Y21 U11	Y22 V11	Y23 U12	Y24 V12
 * Y25 U13	Y26 V13	Y27 U14	Y28 V14	Y29 U15	Y30 V15	Y31 U16	Y32 V16
 *
 */
void repack_yuv422p_to_yuyv_scalar(__m128i* y_input, __m128i* u_input, __m128i* v_input, __m128i* yuyv_out) {
    uint8_t *y = (uint8_t *) y_input;
    uint8_t *u = (uint8_t *) u_input;
    uint8_t *v = (uint8_t *) v_input;
    uint8_t *out = (uint8_t *) yuyv_out;
    uint8_t pixel_count = 0;
    
    for(pixel_count = 0; pixel_count < 32; pixel_count += 2) {
        *out++ = *y++;
        *out++ = *u++;
        *out++ = *y++;
        *out++ = *v++;
    }
}

uint32_t check_repack_yuv422p_to_yuyv() {
    DO_CHECK_INLINE_3IN(repack_yuv422p_to_yuyv_scalar, repack_yuv422p_to_yuyv_sse2, DECLARE_2_8BIT_VECT, DECLARE_1_8BIT_VECT, DECLARE_1_8BIT_VECT, 4, MAX_DIFF_PACKING, compare_8bit_output); 

    return 0;
}






/*
 * Convert YUV422P to UYVY:
 * 4 vectors of 16 char: 2xY, 1xU, 1xV to 4 vectors of 16 char YUYV
 *
 * Total latency:				4
 * Number of pixels handled:	32
 *
 * INPUT
 * 4 vectors of 16 char
 * y_input
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 * Y17 Y18	Y19 Y20	Y21 Y22	Y23 Y24	Y25 Y26	Y27 Y28 Y29 Y30	Y31 Y32
 *
 * u_input
 * U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14	U15 U16
 *
 * v_input
 * V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14	V15 V16
 *
 *
 * OUTPUT:
 *
 * 4 vectors of 16 char
 * U1 Y1	V1 Y2	U2 Y3	V2 Y4	U3 Y5	V3 Y6	U4 Y7	V4 Y8
 * U5 Y9	V5 Y10	U6 Y11	V6 Y12	U7 Y13	V7 Y14	U8 Y15	V8 Y16
 * U9 Y17	V9 Y18	U10 Y19	V10 Y20	U11 Y21	V11 Y22	U12 Y23	V12 Y24
 * U13 Y25	V13 Y26	U14 Y27	V14 Y28	U15 Y29	V15 Y30	U16 Y31	V16 Y32
 *
 */
void repack_yuv422p_to_uyvy_scalar(__m128i* y_input, __m128i* u_input, __m128i* v_input, __m128i* yuyv_out) {
    uint8_t *y = (uint8_t *) y_input;
    uint8_t *u = (uint8_t *) u_input;
    uint8_t *v = (uint8_t *) v_input;
    uint8_t *out = (uint8_t *) yuyv_out;
    uint8_t pixel_count = 0;
    
    for(pixel_count = 0; pixel_count < 32; pixel_count += 2) {
        *out++ = *u++;
        *out++ = *y++;
        *out++ = *v++;
        *out++ = *y++;
    }
}

uint32_t check_repack_yuv422p_to_uyvy() {
    DO_CHECK_INLINE_3IN(repack_yuv422p_to_uyvy_scalar, repack_yuv422p_to_uyvy_sse2, DECLARE_2_8BIT_VECT, DECLARE_1_8BIT_VECT, DECLARE_1_8BIT_VECT, 4, MAX_DIFF_PACKING, compare_8bit_output); 

    return 0;
}



/*
 * Convert YUYV to YUV422P:
 * 4 vectors of 16 char YUYV to 4 vectors of 16 char: 2xY, 1xU, 1xV
 *
 * Total latency:				28
 * Number of pixels handled:	32
 *
 * INPUT
 * 4 vectors of 16 char
 * Y1 U1	Y2 V1	Y3 U2	Y4 V2	Y5 U3	Y6 V3	Y7 U4	Y8 V4
 * Y9 U5	Y10 V5	Y11 U6	Y12 V6	Y13 U7	Y14 V7	Y15 U8	Y16 V8
 * Y17 U9	Y18 V9	Y19 U10	Y20 V10	Y21 U11	Y22 V11	Y23 U12	Y24 V12
 * Y25 U13	Y26 V13	Y27 U14	Y28 V14	Y29 U15	Y30 V15	Y31 U16	Y32 V16
 *
 *
 * OUTPUT:
 * 4 vectors of 16 char
 * y_input
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 * Y17 Y18	Y19 Y20	Y21 Y22	Y23 Y24	Y25 Y26	Y27 Y28 Y29 Y30	Y31 Y32
 *
 * u_input
 * U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14	U15 U16
 *
 * v_input
 * V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14	V15 V16
 *
 *
 */
void repack_yuyv_to_yuv422p_scalar(__m128i* input, __m128i* y_output, __m128i* u_output, __m128i* v_output) {
    uint8_t *in = (uint8_t *) input;
    uint8_t *y = (uint8_t *) y_output;
    uint8_t *u = (uint8_t *) u_output;
    uint8_t *v = (uint8_t *) v_output;
    uint8_t pixel_count = 0;
    
    for(pixel_count = 0; pixel_count < 32; pixel_count += 2) {
        *y++ = *in++;
        *u++ = *in++;
        *y++ = *in++;
        *v++ = *in++;
    }
}

uint32_t check_repack_yuyv_to_yuv422p() {
    CHECK_INLINE_1IN_3OUT2(repack_yuyv_to_yuv422p_scalar, repack_yuyv_to_yuv422p_sse2, DECLARE_4_8BIT_VECT, 2, 1, MAX_DIFF_PACKING, compare_8bit_output, 0, 0); 
    CHECK_INLINE_1IN_3OUT2(repack_yuyv_to_yuv422p_scalar, repack_yuyv_to_yuv422p_sse2_ssse3, DECLARE_4_8BIT_VECT, 2, 1, MAX_DIFF_PACKING, compare_8bit_output, 0, 0); 

    return 0;
}



/*
 * Convert UYVY to YUV422P:
 * 4 vectors of 16 char UYVY to 4 vectors of 16 char: 2xY, 1xU, 1xV
 *
 * Total latency:				26
 * Number of pixels handled:	32
 *
 * INPUT
 * 4 vectors of 16 char
 * U1 Y1	V1 Y2	U2 Y3	V2 Y4	U3 Y5	V3 Y6	U4 Y7	V4 Y8
 * U5 Y9	V5 Y10	U6 Y11	V6 Y12	U7 Y13	V7 Y14	U8 Y15	V8 Y16
 * U9 Y17	V9 Y18	U10 Y19	V10 Y20	U11 Y21	V11 Y22	U12 Y23	V12 Y24
 * U13 Y25	V13 Y26	U14 Y27	V14 Y28	U15 Y29	V15 Y30	U16 Y31	V16 Y32
 *
 *
 * OUTPUT:
 * 4 vectors of 16 char
 * y_input
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12 Y13 Y14	Y15 Y16
 * Y17 Y18	Y19 Y20	Y21 Y22	Y23 Y24	Y25 Y26	Y27 Y28 Y29 Y30	Y31 Y32
 *
 * u_input
 * U1 U2	U3 U4 	U5 U6	U7 U8	U9 U10	U11 U12	U13 U14	U15 U16
 *
 * v_input
 * V1 V2	V3 V4 	V5 V6	V7 V8	V9 V10	V11 V12	V13 V14	V15 V16
 *
 *
 */
void repack_uyvy_to_yuv422p_scalar(__m128i* input, __m128i* y_output, __m128i* u_output, __m128i* v_output) {
    uint8_t *in = (uint8_t *) input;
    uint8_t *y = (uint8_t *) y_output;
    uint8_t *u = (uint8_t *) u_output;
    uint8_t *v = (uint8_t *) v_output;
    uint8_t pixel_count = 0;
    
    for(pixel_count = 0; pixel_count < 32; pixel_count += 2) {
        *u++ = *in++;
        *y++ = *in++;
        *v++ = *in++;
        *y++ = *in++;
    }
}

uint32_t check_repack_uyvy_to_yuv422p() {
    CHECK_INLINE_1IN_3OUT2(repack_uyvy_to_yuv422p_scalar, repack_uyvy_to_yuv422p_sse2, DECLARE_4_8BIT_VECT, 2, 1, MAX_DIFF_PACKING, compare_8bit_output, 0, 0); 
    CHECK_INLINE_1IN_3OUT2(repack_uyvy_to_yuv422p_scalar, repack_uyvy_to_yuv422p_sse2_ssse3, DECLARE_4_8BIT_VECT, 2, 1, MAX_DIFF_PACKING, compare_8bit_output, 0, 0); 

    return 0;
}


