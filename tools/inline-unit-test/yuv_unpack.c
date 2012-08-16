/*
 * yuv_unpack.c
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
#include "yuv_unpack.h"

/*
 * Convert 1 vector of 16 char YUYV to 2 vectors of 8 short Y & UV
 *
 * Total latency:				4
 * Number of pixels handled:	8
 *
 * INPUT
 * 1 vector of 16 char
 * Y1 U12	Y2 V12	Y3 U34	Y4 V34	Y5 U56	Y6 V56	Y7 U78	Y8 V78
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
void	unpack_yuyv_to_y_uv_vectors_scalar(__m128i* input, __m128i* output) {
	uint8_t *in = (uint8_t *) input;
	uint16_t *out = (uint16_t *) output;
	uint32_t pixel_count;
	
	for(pixel_count = 0; pixel_count < 8; pixel_count += 2) {
		out[0] = *in++;
		out[8] = *in++;
		out[1] = *in++;
		out[8 + 1] = *in++;
		out += 2;
	}
}

uint32_t check_unpack_yuyv_to_y_uv_vectors() {
	CHECK_INLINE_1IN(unpack_yuyv_to_y_uv_vectors_scalar, unpack_yuyv_to_y_uv_vectors_sse2, DECLARE_1_8BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output);	
	CHECK_INLINE_1IN(unpack_yuyv_to_y_uv_vectors_scalar, unpack_yuyv_to_y_uv_vectors_sse2_ssse3, DECLARE_1_8BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output);	
	
	return 0;
}


/*
 * Convert 1 vector of 16 char UYVY to 2 vectors of 8 short Y & UV
 *
 * Total latency:				2
 * Number of pixels handled:	8
 *
 * INPUT
 * 1 vector of 16 char
 * U12 Y1	V12 Y2	U34	Y3	V34 Y4	U56 Y5	V56 Y6	U78 Y7	V78 Y8
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * uVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
void	unpack_uyvy_to_y_uv_vectors_scalar(__m128i* input, __m128i* output) {
	uint8_t *in = (uint8_t *) input;
	uint16_t *out = (uint16_t *) output;
	uint32_t pixel_count;
	
	for(pixel_count = 0; pixel_count < 8; pixel_count += 2) {
		out[8] = *in++;
		out[0] = *in++;
		out[8 + 1] = *in++;
		out[1] = *in++;
		
		out += 2;
	}
}

uint32_t check_unpack_uyvy_to_y_uv_vectors() {
	CHECK_INLINE_1IN(unpack_uyvy_to_y_uv_vectors_scalar, unpack_uyvy_to_y_uv_vectors_sse2, DECLARE_1_8BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output);	
	CHECK_INLINE_1IN(unpack_uyvy_to_y_uv_vectors_scalar, unpack_uyvy_to_y_uv_vectors_sse2_ssse3, DECLARE_1_8BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output);
	
	return 0;
}





/*
 * Convert 1 vector of 16 char Y (as found in YUV422P / YUV420P) to 2 vectors of 8 short Y
 *
 * Total latency:				2
 *
 * INPUT
 * 1 vector of 16 char
 * Y1 Y2	Y3 Y4	Y5 Y6	Y7 Y8	Y9 Y10	Y11 Y12		Y13 Y14		Y15 Y16
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect1
 * Y1  0	Y2  0	Y3  0	Y4  0	Y5  0	Y6  0	Y7  0	Y8  0
 *
 * yVect2
 * Y9  0	Y10  0	Y11  0	Y12  0	Y13  0	Y14  0	Y15  0	Y16  0
 */
void	unpack_yuv42Xp_to_2_y_vectors_scalar(__m128i* input, __m128i* output1, __m128i *output2) {
	uint8_t *in = (uint8_t *) input;
	uint16_t *out = (uint16_t *) output1;

	uint32_t pixel_count;
	
	for(pixel_count = 0; pixel_count < 16; pixel_count++) {
        if (pixel_count == 8)
            out = (uint16_t *) output2;

		*out++ = *in++;
	}
}

uint32_t check_unpack_yuv42Xp_to_2_y_vectors() {
	CHECK_INLINE_1IN_2OUT(unpack_yuv42Xp_to_2_y_vectors_scalar, unpack_yuv42Xp_to_2_y_vectors_sse2, DECLARE_1_8BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output, 0, 0);	
	
	return 0;
}


/*
 * Convert the first 8 chroma values in 2 vectors of 16 char UV
 * (as found in YUV422P / YUV420P) to 2 vectors of 8 interleaved short UV
 *
 * Total latency:				3
 *
 * INPUT
 * 2 vectors of 16 char
 * u_input
 * U1 U2	U3 U4	U5 U6	U7 U8	U9 U10	U11 U12		U13 U14		U15 U16
 *
 * v_input
 * V1 V2	V3 V4	V5 V6	V7 V8	V9 V10	V11 V12		V13 V14		V15 V16
 *
 * OUTPUT:
 *
 * 2 vectors of 16 char
 * uvVect1
 * U1 0		V1 0	U2 0	V2 0	U3 0	V3 0	U4 0	V4	0
 *
 * uvVect2
 * U5 0		V5 0	U6 0	V6 0	U7 0	V7 0	U8 0	V8	0
 */
void	unpack_low_yuv42Xp_to_uv_vector_scalar(__m128i* input_u, __m128i *input_v, __m128i* output1, __m128i *output2) {
	uint8_t *in_u = (uint8_t *) input_u;
    uint8_t *in_v = (uint8_t *) input_v;
	uint16_t *out = (uint16_t *) output1;
	uint32_t pixel_count;
	
	for(pixel_count = 0; pixel_count < 16; pixel_count += 2) {
        if (pixel_count == 8)
            out = (uint16_t *) output2;

		*out++ = *in_u++;
		*out++ = *in_v++;
	}
}

uint32_t check_unpack_low_yuv42Xp_to_uv_vector() {
	DO_CHECK_INLINE_2IN_2OUT(unpack_low_yuv42Xp_to_uv_vector_scalar, unpack_low_yuv42Xp_to_uv_vector_sse2, DECLARE_1_8BIT_VECT, 1, MAX_DIFF_UNPACKING, compare_16bit_output);

	return 0;
}



/*
 * Convert the last 8 chroma values in 2 vectors of 16 char UV
 * (as found in YUV422P / YUV420P) to 1 vector of 8 interleaved short UV
 *
 * Total latency:				3
 *
 * INPUT
 * 2 vectors of 16 char
 * u_input
 * U1 U2	U3 U4	U5 U6	U7 U8	U9 U10	U11 U12		U13 U14		U15 U16
 *
 * v_input
 * V1 V2	V3 V4	V5 V6	V7 V8	V9 V10	V11 V12		V13 V14		V15 V16
 *
 * OUTPUT:
 *
 * 1 vector of 16 char
 * uvVect1
 * U9 0		V9 0	U10 0 	V10 0	U11 0 	V11 0	U12 0 	V12 0
 *
 * uvVect2
 * U13 0	V13 0	U14 0 	V14 0	U15 0 	V15 0	U16 0	V16 0
 */

void	unpack_high_yuv42Xp_to_uv_vector_scalar(__m128i* input_u, __m128i *input_v, __m128i* output1, __m128i *output2) {
	uint8_t *in_u = (uint8_t *) input_u;
    uint8_t *in_v = (uint8_t *) input_v;
	uint16_t *out = (uint16_t *) output1;
	uint32_t pixel_count;
	
    in_v += 8;
    in_u += 8;

	for(pixel_count = 0; pixel_count < 16; pixel_count += 2) {
        if (pixel_count == 8)
            out = (uint16_t *) output2;

		*out++ = *in_u++;
		*out++ = *in_v++;
	}
}


uint32_t check_unpack_high_yuv42Xp_to_uv_vector() {
	DO_CHECK_INLINE_2IN_2OUT(unpack_high_yuv42Xp_to_uv_vector_scalar, unpack_high_yuv42Xp_to_uv_vector_sse2, DECLARE_1_8BIT_VECT, 1, MAX_DIFF_UNPACKING, compare_16bit_output);	
	
	return 0;
}

/*
 * Unpack 1 v210 vector (6 pixels) into 2 vectors (Y, UV)
 *
 * Total latency:				10
 * Number of pixels handled:	16
 *
 * INPUT
 * 1 vector of 6 v210 pixels
 * U01	Y0	V01	Y1	... U45	Y4	V45	Y5
 *
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y0		Y1		Y2		Y3		Y4		Y5		0		0
 *
 * uvVect
 * U01		V01		U23		V23		U45		V45		0		0
 */
/*INLINE_NAME(unpack_1v_v210_to_y_uv_vectors_sse2_ssse3_sse41, __m128i* input, __m128i* out_y, __m128i* out_uv) {
	CONST_M128I(keep_low_component, 0x000003FF000003FFLL, 0x000003FF000003FFLL);
	CONST_M128I(keep_middle_component, 0x03FF000003FF0000LL, 0x03FF000003FF0000LL);
	CONST_M128I(blend_uv_mask, 0x00000000FFFFFFFFLL, 0x00000000FFFFFFFFLL);
	CONST_M128I(shuffle1, 0x0908070603020100LL, 0xFFFFFFFF0F0E0B0ALL);
	CONST_M128I(shuffle2, 0x0B0A070605040302LL, 0xFFFFFFFF0F0E0D0CLL);
	CONST_M128I(blend_y_mask, 0xFFFFFFFF00000000LL, 0xFFFFFFFF00000000LL);
	M128I(scratch, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);
	M128I(scratch3, 0x0LL, 0x0LL);

	UNALIGNED_YUV422I_INPUT_PREAMBLE;


	_M(scratch) = _mm_and_si128(*INPUT_VECT, _M(keep_low_component));				// PAND			1	0.33
	_M(scratch2) = _mm_srai_epi32(*INPUT_VECT, 4);									// PSRAD		1	1
	_M(scratch2) = _mm_and_si128(_M(scratch2), _M(keep_middle_component));			// PAND			1	0.33
	_M(scratch2) = _mm_or_si128(_M(scratch2), _M(scratch));							// POR			1	0.33
	// U01		V01		Y1		Y2		V23		U45		Y4		Y5

	_M(scratch) = _mm_slli_epi32(*INPUT_VECT, 6);									// PSLLD        1	1
	_M(scratch) = _mm_and_si128(_M(scratch), _M(keep_middle_component));			// PAND			1	0.33
	// 0		Y0		0		U23		0		Y3		0		V45


	_M(scratch3) = _mm_blendv_epi8(_M(scratch), _M(scratch2), _M(blend_uv_mask));	// PBLENDVB		1	1
	//	U01		V01		0		U23		V23		U45		0		V45
	*out_uv = _mm_shuffle_epi8(_M(scratch3), _M(shuffle1));							//	PSHUFB		1	0.5
	//	U01		V01		U23		V23		U45		V45		0		0


	_M(scratch3) = _mm_blendv_epi8(_M(scratch), _M(scratch2), _M(blend_y_mask));	// PBLENDVB		1	1
	//	0		Y0		Y1		Y2		0		Y3		Y4		Y5
	*out_y = _mm_shuffle_epi8(_M(scratch3), _M(shuffle2));							//	PSHUFB		1	0.5
	// Y0		Y1		Y2		Y3		Y4		Y5		0		0
}*/

/*
 * Unpack 1 v210 vector (6 pixels) into 2 vectors (Y, UV)
 *
 * Total latency:				12
 * Number of pixels handled:	16
 *
 * INPUT
 * 1 vector of 6 v210 pixels
 * U01	Y0	V01	Y1	... U45	Y4	V45	Y5
 *
 *
 * OUTPUT:
 *
 * 2 vectors of 8 short
 * yVect
 * Y0		Y1		Y2		Y3		Y4		Y5		0		0
 *
 * uvVect
 * U01		V01		U23		V23		U45		V45		0		0
 */
/*INLINE_NAME(unpack_1v_v210_to_y_uv_vectors_sse2_ssse3, __m128i* input, __m128i* out_y, __m128i* out_uv) {
	CONST_M128I(keep_low_component, 0x000003FF000003FFLL, 0x000003FF000003FFLL);
	CONST_M128I(keep_middle_component, 0x03FF000003FF0000LL, 0x03FF000003FF0000LL);
	CONST_M128I(shuffle_uv1, 0x0908FFFF03020100LL, 0xFFFFFFFFFFFF0B0ALL);
	CONST_M128I(shuffle_uv2, 0xFFFF0706FFFFFFFFLL, 0xFFFFFFFF0F0EFFFFLL);
	CONST_M128I(shuffle_y1, 0xFFFF07060504FFFFLL, 0xFFFFFFFF0F0E0D0CLL);
	CONST_M128I(shuffle_y2, 0x0B0AFFFFFFFF0302LL, 0xFFFFFFFFFFFFFFFFLL);
	M128I(scratch, 0x0LL, 0x0LL);
	M128I(scratch2, 0x0LL, 0x0LL);
	M128I(scratch3, 0x0LL, 0x0LL);
	M128I(scratch4, 0x0LL, 0x0LL);

	UNALIGNED_YUV422I_INPUT_PREAMBLE;


	_M(scratch) = _mm_and_si128(*INPUT_VECT, _M(keep_low_component));				// PAND			1	0.33
	_M(scratch2) = _mm_srai_epi32(*INPUT_VECT, 4);									// PSRAD		1	1
	_M(scratch2) = _mm_and_si128(_M(scratch2), _M(keep_middle_component));			// PAND			1	0.33
	_M(scratch2) = _mm_or_si128(_M(scratch2), _M(scratch));							// POR			1	0.33
	// U01		V01		Y1		Y2		V23		U45		Y4		Y5

	_M(scratch) = _mm_slli_epi32(*INPUT_VECT, 6);									// PSLLD        1	1
	_M(scratch) = _mm_and_si128(_M(scratch), _M(keep_middle_component));			// PAND			1	0.33
	// 0		Y0		0		U23		0		Y3		0		V45


	_M(scratch3) = _mm_shuffle_epi8(_M(scratch2), _M(shuffle_uv1));					// PSHUFB		1	0.5
	// U01		V01		0		V23		U45		0		0		0

	_M(scratch4) = _mm_shuffle_epi8(_M(scratch), _M(shuffle_uv2));					// PSHUFB		1	0.5
	// 0		0		U23		0		0		V45		0		0

	*out_uv = _mm_or_si128(_M(scratch3), _M(scratch4));								// POR			1	0.33
	//	U01		V01		U23		V23		U45		V45		0		0


	_M(scratch3) = _mm_shuffle_epi8(_M(scratch2), _M(shuffle_y1));					// PSHUFB		1	0.5
	// 	0		Y1		Y2		0		Y4		Y5		0		0

	_M(scratch4) = _mm_shuffle_epi8(_M(scratch), _M(shuffle_y2));					// PSHUFB		1	0.5
	//	Y0		0		0		Y3		0		0		0		0

	*out_y = _mm_or_si128(_M(scratch3), _M(scratch4));								// POR			1	0.33
	// Y0		Y1		Y2		Y3		Y4		Y5		0		0
}
*/

#undef DEFINE_UNPACK_4_V210_VECTORS
#define DEFINE_UNPACK_4_V210_VECTORS(instr_set) \
INLINE_NAME(unpack_4v_v210_to_y_uv_vectors_ ## instr_set, __m128i* input, __m128i* yuv1_8_out, __m128i* yuv9_16_out, __m128i* yuv17_24_out) {\
	M128I(y1, 0x0LL, 0x0LL);\
	M128I(uv1, 0x0LL, 0x0LL);\
	M128I(y2, 0x0LL, 0x0LL);\
	M128I(uv2, 0x0LL, 0x0LL);\
	M128I(y3, 0x0LL, 0x0LL);\
	M128I(uv3, 0x0LL, 0x0LL);\
	CALL_INLINE(unpack_1v_v210_to_y_uv_vectors_ ## instr_set, &input[0], &(_M(y1)), &(_M(uv1)));\
	/* Y0		Y1		Y2		Y3		Y4		Y5		0		0 */\
	/* U10		V10		U23		V23		U45		V45		0		0 */\
	\
	CALL_INLINE(unpack_1v_v210_to_y_uv_vectors_ ## instr_set, &input[1], &(_M(y2)), &(_M(uv2)));\
	/* Y6		Y7		Y8		Y9		Y10		Y11		0		0 */\
	/* U67		V67		U89		V89		U1011	V1011	0		0 */\
	\
	_M(y3) = _mm_slli_si128(_M(y2), 12);								/* PSLLDQ			1	0.5 */\
	yuv1_8_out[0] = _mm_or_si128(_M(y3), _M(y1));						/* POR              1	0.33 */\
	/* Y0		Y1		Y2		Y3		Y4		Y5		Y6		Y7 */\
	_M(y2) = _mm_srli_si128(_M(y2), 4);									/* PSRLDQ			1	0.5 */\
	/* Y8		Y9		Y10		Y11		0		0		0		0 */\
	\
	\
	_M(uv3) = _mm_slli_si128(_M(uv2), 12);								/* PSLLDQ			1	0.5 */\
	yuv1_8_out[1] = _mm_or_si128(_M(uv3), _M(uv1));						/* POR              1	0.33 */\
	/* U01		V01		U23		V23		U45		V45		U67		V67 */\
	_M(uv2) = _mm_srli_si128(_M(uv2), 4);								/* PSRLDQ			1	0.5 */\
	/* U89		V89		U1011	V1011	0		0		0		0 */\
	\
	\
	\
	CALL_INLINE(unpack_1v_v210_to_y_uv_vectors_ ## instr_set, &input[2], &(_M(y3)), &(_M(uv3)));\
	/* Y12		Y13		Y14		Y15		Y16		Y17		0		0 */\
	/* U1213	V1213	U1415	V1415	U1617	V1617	0		0 */\
	\
	yuv9_16_out[0] = _mm_unpacklo_epi64(_M(y2), _M(y3));				/* PUNPCKLQDQ		1	0.5 */\
	/* Y8		Y9		Y10		Y11		Y12		Y13		Y14		Y15 */\
	_M(y3) = _mm_srli_si128(_M(y3), 8);									/* PSRLDQ			1	0.5 */\
	/* Y16		Y17		0		0		0		0		0		0 */\
	\
	\
	yuv9_16_out[1] = _mm_unpacklo_epi64(_M(uv2), _M(uv3));				/* PUNPCKLQDQ		1	0.5 */\
	/* U89		V89		U1011	V1011	U1213	V1213	U1415	V1415 */\
	_M(uv3) = _mm_srli_si128(_M(uv3), 8);								/* PSRLDQ			1	0.5 */\
	/* U1617	V1617	0		0		0		0		0		0 */\
	\
	\
	CALL_INLINE(unpack_1v_v210_to_y_uv_vectors_ ## instr_set, &input[3], &(_M(y1)), &(_M(uv1)));\
	/* Y18		Y19		Y20		Y21		Y22		Y23		0		0 */\
	/* U1819	V1819	U2021	V2021	U2223	V2223	0		0 */\
	\
	_M(y1) = _mm_slli_si128(_M(y1), 4);									/* PSLLDQ			1	0.5 */\
	yuv17_24_out[0] = _mm_or_si128(_M(y1), _M(y3));						/* POR              1	0.33 */\
	/*	Y16		Y17		Y18		Y19		Y20		Y21		Y22		Y23 */\
	\
	_M(uv1) = _mm_slli_si128(_M(uv1), 4);								/* PSLLDQ			1	0.5 */\
	yuv17_24_out[1] = _mm_or_si128(_M(uv1), _M(uv3));					/* POR              1	0.33 */\
	/* U1617	V1617	U1819	V1819	U2021	V2021	U2223	V2223 */\
}

/*
 * Unpack 4 v210 vectors (24 pixels) into 6 vectors (Y, UV)
 *
 * Total latency:				56 (sse41) / 64 (ssse3)
 * Number of pixels handled:	24
 *
 * INPUT
 * 4 vectors of 24 v210 pixels
 * U01		Y0		V01		Y1	... U45	  Y4	V45	    Y5
 * U67		Y6		V67		Y7	... U1011 Y10	V1011	Y11
 * U1213	Y12		V1213	Y13	... U1617 Y16	V1617	Y17
 * U1819	Y18		V1819	Y19	... U2223 Y22	V2223	Y23
 *
 *
 * OUTPUT:
 *
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
 */
void unpack_4v_v210_to_y_uv_vectors_scalar(__m128i* input, __m128i* yuv1_8_out, __m128i* yuv9_16_out, __m128i* yuv17_24_out) {
    uint32_t *src = (uint32_t *) input;
    uint16_t *out = (uint16_t *) yuv1_8_out;
    uint32_t  pixel_count = 0;

    while(pixel_count < 24) {
        /* Pixel 1 and 2 */
        out[8] = *src & 0x3FF;
        out[0] = (*src >> 10) & 0x3FF;
        out[8 + 1] = (*src >> 20) & 0x3FF;
        out[1] = src[1] & 0x3FF;
        out += 2;

        pixel_count += 2;
        if (pixel_count  == 8)
            out = (uint16_t *) yuv9_16_out;
        else if (pixel_count == 16)
            out = (uint16_t *) yuv17_24_out;

        /* Pixel 3 and 4 */
        out[8] = (src[1] >> 10) & 0x3FF;
        out[0] = (src[1] >> 20) & 0x3FF;
        out[8 + 1] = src[2] & 0x3FF;
        out[1] = (src[2] >> 10) & 0x3FF;
        out += 2;

        pixel_count += 2;
        if (pixel_count  == 8)
            out = (uint16_t *) yuv9_16_out;
        else if (pixel_count == 16)
            out = (uint16_t *) yuv17_24_out;

        /* Pixel 5 and 6 */
        out[8] = (src[2] >> 20) & 0x3FF;
        out[0] = src[3] & 0x3FF;
        out[8 + 1] = (src[3] >> 10) & 0x3FF;
        out[1] = (src[3] >> 20) & 0x3FF;
        out += 2;

        pixel_count += 2;
         if (pixel_count  == 8)
            out = (uint16_t *) yuv9_16_out;
        else if (pixel_count == 16)
            out = (uint16_t *) yuv17_24_out;

        src += 4;
    }
}
uint32_t check_unpack_4v_v210_to_y_uv_vectors() {
    CHECK_INLINE_1IN_3OUT(unpack_4v_v210_to_y_uv_vectors_scalar, unpack_4v_v210_to_y_uv_vectors_sse2_ssse3, DECLARE_4_10BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output, 0, 0);
    CHECK_INLINE_1IN_3OUT(unpack_4v_v210_to_y_uv_vectors_scalar, unpack_4v_v210_to_y_uv_vectors_sse2_ssse3_sse41, DECLARE_4_10BIT_VECT, 2, MAX_DIFF_UNPACKING, compare_16bit_output, 0, 0);


    return 0;
}

