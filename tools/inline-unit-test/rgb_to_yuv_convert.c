/*
 * rgb_to_yuv_convert.c
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
#include "rgb_to_yuv_convert.h"

#define CHECK_FN_8BIT_3IN(fn_suffix, out_count)\
    uint32_t    check_ ## fn_suffix() {\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_3_8BIT_VECT, out_count, MAX_DIFF_8BIT,  compare_16bit_output);\
        return 0;\
    }

#define CHECK_FN_10BIT_3IN(fn_suffix, out_count)\
    uint32_t    check_ ## fn_suffix() {\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_3_8BIT_VECT, out_count, MAX_DIFF_8BIT,  compare_16bit_output);\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_3_10BIT_VECT, out_count, MAX_DIFF_8BIT,  compare_16bit_output);\
        return 0;\
    }

#define CHECK_FN_8BIT_4IN(fn_suffix, out_count)\
    uint32_t    check_ ## fn_suffix() {\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_4_8BIT_VECT, out_count, MAX_DIFF_8BIT,  compare_16bit_output);\
        return 0;\
    }

#define CHECK_FN_8BIT_2IN(fn_suffix, out_count)\
    uint32_t    check_ ## fn_suffix() {\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_2_8BIT_VECT, out_count, MAX_DIFF_8BIT,  compare_16bit_output);\
        return 0;\
    }

#define DECLARE_RGB_TO_Y_CHECK(fn_prefix, coeffs, offset, check_fn) \
    void fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
        uint16_t* in = (uint16_t*) input;\
        uint16_t* out = (uint16_t*) output;\
        uint32_t  pixel_count;\
        \
        for(pixel_count = 0; pixel_count < 8; pixel_count++) {\
            *out++ = in[0] * coeffs[0][0] + in[8] * coeffs[0][1] + in[16] * coeffs[0][2] + offset[0];\
            in++;\
        }\
    }\
    \
    check_fn(fn_prefix, 1)

#define DECLARE_RGB_TO_UV_CHECK(fn_prefix, coeffs, offset, check_fn) \
    void fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
        uint16_t* in = (uint16_t*) input;\
        uint16_t* out = (uint16_t*) output;\
        uint32_t  pixel_count;\
        \
        for(pixel_count = 0; pixel_count < 4; pixel_count++) {\
            *out++ = in[0] * coeffs[1][0] + in[8] * coeffs[1][1] + in[16] * coeffs[1][2] + offset[1];\
            *out++ = in[1] * coeffs[2][0] + in[8 + 1] * coeffs[2][1] + in[16 + 1] * coeffs[2][2] + offset[2];\
            in += 2;\
        }\
    }\
    \
    check_fn(fn_prefix, 1)


#define DECLARE_AG_RB_TO_Y_CHECK(ag_rb_fn_prefix, ga_br_fn_prefix, coeffs, offset, check_fn) \
    void ag_rb_fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
        uint16_t* in = (uint16_t*) input;\
        uint16_t* out = (uint16_t*) output;\
        uint32_t  pixel_count;\
        \
        for(pixel_count = 1; pixel_count <= 8; pixel_count++) {\
            *out++ = in[8] * coeffs[0][0] + in[1] * coeffs[0][1] + in[8 + 1] * coeffs[0][2] + offset[0];\
            in += 2;\
            if(pixel_count==4)\
                in += 8;\
        }\
    }\
    \
    check_fn(ag_rb_fn_prefix, 1)\
    void ga_br_fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
        uint16_t* in = (uint16_t*) input;\
        uint16_t* out = (uint16_t*) output;\
        uint32_t  pixel_count;\
        \
        for(pixel_count = 1; pixel_count <= 8; pixel_count++) {\
            *out++ = in[8 + 1] * coeffs[0][0] + in[0] * coeffs[0][1] + in[8] * coeffs[0][2] + offset[0];\
            in += 2;\
            if(pixel_count==4)\
                in += 8;\
        }\
    }\
    \
    check_fn(ga_br_fn_prefix, 1)\



#define DECLARE_AG_RB_TO_UV_CHECK(ag_rb_fn_prefix, ga_br_fn_prefix, coeffs, offset, check_fn) \
    void ag_rb_fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
        uint16_t* in = (uint16_t*) input;\
        uint16_t* out = (uint16_t*) output;\
        uint32_t  pixel_count;\
        \
        for(pixel_count = 0; pixel_count < 4; pixel_count++) {\
            *out++ = in[8] * coeffs[1][0] + in[1] * coeffs[1][1] + in[8 + 1] * coeffs[1][2] + offset[1];\
            *out++ = in[8] * coeffs[2][0] + in[1] * coeffs[2][1] + in[8 + 1] * coeffs[2][2] + offset[2];\
            in += 2;\
        }\
    }\
    \
    check_fn(ag_rb_fn_prefix, 1)\
    void ga_br_fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
        uint16_t* in = (uint16_t*) input;\
        uint16_t* out = (uint16_t*) output;\
        uint32_t  pixel_count;\
        \
        for(pixel_count = 0; pixel_count < 4; pixel_count++) {\
            *out++ = in[8 + 1] * coeffs[1][0] + in[0] * coeffs[1][1] + in[8] * coeffs[1][2] + offset[1];\
            *out++ = in[8 + 1] * coeffs[2][0] + in[0] * coeffs[2][1] + in[8] * coeffs[2][2] + offset[2];\
            in += 2;\
        }\
    }\
    \
    check_fn(ga_br_fn_prefix, 1)




/*
 * Convert 3 vectors of 8 short R, G, B into 1 vector of 8 short Y
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			11 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  0.299		 0.587		 0.114	]	( R )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[  19595	 38470		 7471	]
 *
 * Note: the Y calculation involves only positive values and coefficients and
 * thus uses only unsigned math.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_RGB_TO_Y_CHECK(convert_r_g_b_vectors_to_y_vector, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0], CHECK_FN_8BIT_3IN);


/*
 * Convert 3 vectors of 8 short R, G, B into 1 vector of 8 short Y
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  16  ] + [  0.257		 0.504		 0.098	]	( R )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[  16843	 33030		 6423	]
 *
 * Note: the Y calculation involves only positive values and coefficients and
 * thus uses only unsigned math.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_RGB_TO_Y_CHECK(convert_r_g_b_vectors_to_y_vector_bt601, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1], CHECK_FN_8BIT_3IN);

/*
 * Convert 3 vectors of 8 short R, G, B into 1 vector of 8 short Y
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  16  ] + [  0.183		 0.614		 0.062	]	( R )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[  11993	 40239		 4063	]
 *
 * Note: the Y calculation involves only positive values and coefficients and
 * thus uses only unsigned math.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_RGB_TO_Y_CHECK(convert_r_g_b_vectors_to_y_vector_bt709, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2], CHECK_FN_8BIT_3IN);



/*
 * Convert 3 vectors of 8 short 8-bit R, G, B into 1 vector of 8 short 10-bit Y
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			14 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  1.196		 2.348		 0.456	]	( R )
 *
 *				All coeffs are left-shifted by 14 bits
 * 					[  19595	 38470		 7471	]
 *
 * Note: the Y calculation involves only positive values and coefficients and
 * thus uses only unsigned math.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_RGB_TO_Y_CHECK(convert_8bit_r_g_b_vectors_to_10bit_y_vector, rgb_8bit_to_yuv_10bit_coef[0], rgb_8bit_to_yuv_10bit_off[0], CHECK_FN_10BIT_3IN);


/*
 * Convert 3 vectors of 8 short R, G, B into 1 vector of 8 short Y
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			15 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  64  ] + [  1.028		 2.016		 0.392	]	( R )
 *
 *				All coeffs are left-shifted by 14 bits
 * 					[  16843	 33030		 6423	]
 *
 * Note: the Y calculation involves only positive values and coefficients and
 * thus uses only unsigned math.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_RGB_TO_Y_CHECK(convert_8bit_r_g_b_vectors_to_10bit_y_vector_bt601, rgb_8bit_to_yuv_10bit_coef[1], rgb_8bit_to_yuv_10bit_off[1], CHECK_FN_10BIT_3IN);


/*
 * Convert 3 vectors of 8 short R, G, B into 1 vector of 8 short Y
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			15 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  64  ] + [  0.732		 2.456		 0.248	]	( R )
 *
 *				All coeffs are left-shifted by 14 bits
 * 					[  11993	 40239		 4063	]
 *
 * Note: the Y calculation involves only positive values and coefficients and
 * thus uses only unsigned math.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gVect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * bVect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_RGB_TO_Y_CHECK(convert_8bit_r_g_b_vectors_to_10bit_y_vector_bt709, rgb_8bit_to_yuv_10bit_coef[2], rgb_8bit_to_yuv_10bit_off[2], CHECK_FN_10BIT_3IN);



/*
 *
 *		R		G		B
 *
 * 		to
 *
 *		U		V
 *
 */


/*
 * Convert 3 vectors of 8 short downsampled 422 R, G, B into 1 vector of 8 short U-V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.169		-0.331		 0.500	]	( G )
 * V = 	[ 128 ] + [  0.500		-0.419		-0.081	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -11076	-21692		 32767	]
 * 					[  32767	-27460		-5308	]
 *
 *	Note: the R-V & B-U coeffs (32767) should really be 32768 but because we need them to be
 *			16-bit signed integers, they have been capped to the maximum value for this
 *			type. 32767 maps to 0.499985 instead of 0.500 which is an acceptable
 *			approximation.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R12 0	R12 0	R34 0	R34 0	R56 0	R56 0	R78 0	R78 0
 *
 * gVect
 * G12 0	G12 0	G34 0	G34 0	G56 0	G56 0	G78 0	G78 0
 *
 * bVect
 * B12 0	B12 0	B34 0	B34 0	B56 0	B56 0	B78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_RGB_TO_UV_CHECK(convert_downsampled_422_r_g_b_vectors_to_uv_vector, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0], CHECK_FN_8BIT_3IN);

/*
 * Convert 3 vectors of 8 short downsampled 422 R, G, B into 1 vector of 8 short U-V
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.148		-0.291		 0.439	]	( G )
 * V = 	[ 128 ] + [  0.439		-0.368		-0.071	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -9699		-19071		 28770	]
 * 					[  28770	-24117		-4653	]
 *
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R12 0	R12 0	R34 0	R34 0	R56 0	R56 0	R78 0	R78 0
 *
 * gVect
 * G12 0	G12 0	G34 0	G34 0	G56 0	G56 0	G78 0	G78 0
 *
 * bVect
 * B12 0	B12 0	B34 0	B34 0	B56 0	B56 0	B78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_RGB_TO_UV_CHECK(convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt601, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1], CHECK_FN_8BIT_3IN);


/*
 * Convert 3 vectors of 8 short downsampled 422 R, G, B into 1 vector of 8 short U-V
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.101		-0.339		 0.439	]	( G )
 * V = 	[ 128 ] + [  0.439		-0.399		-0.040	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -6619		-22217		 28770	]
 * 					[  28770	-26149		-2621	]
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R12 0	R12 0	R34 0	R34 0	R56 0	R56 0	R78 0	R78 0
 *
 * gVect
 * G12 0	G12 0	G34 0	G34 0	G56 0	G56 0	G78 0	G78 0
 *
 * bVect
 * B12 0	B12 0	B34 0	B34 0	B56 0	B56 0	B78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_RGB_TO_UV_CHECK(convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2], CHECK_FN_8BIT_3IN);



/*
 * Convert 3 vectors of 8 short downsampled 422 8-bit R, G, B into 1 vector of 8 short 10-bit U-V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			15 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 512 ] + [ -0.676		-1.324		 2		]	( G )
 * V = 	[ 512 ] + [  2			-1.676		-0.324	]	( B )
 *
 *				All coeffs are left-shifted by 13 bits
 * 					[ -5538		-10846		 16384	]
 * 					[  16384	-13730		-2654	]

 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R12 0	R12 0	R34 0	R34 0	R56 0	R56 0	R78 0	R78 0
 *
 * gVect
 * G12 0	G12 0	G34 0	G34 0	G56 0	G56 0	G78 0	G78 0
 *
 * bVect
 * B12 0	B12 0	B34 0	B34 0	B56 0	B56 0	B78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */

DECLARE_RGB_TO_UV_CHECK(convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector, rgb_8bit_to_yuv_10bit_coef[0], rgb_8bit_to_yuv_10bit_off[0], CHECK_FN_10BIT_3IN);

/*
 * Convert 3 vectors of 8 short downsampled 422 b-bit R, G, B into 1 vector of 8 short 10-bit U-V
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			15 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 512 ] + [ -0.592		-1.164		 1.756	]	( G )
 * V = 	[ 512 ] + [  1.756		-1.472		-0.284	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -4850		-9535		 14385	]
 * 					[  14385	-12059		-2327	]
 *
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R12 0	R12 0	R34 0	R34 0	R56 0	R56 0	R78 0	R78 0
 *
 * gVect
 * G12 0	G12 0	G34 0	G34 0	G56 0	G56 0	G78 0	G78 0
 *
 * bVect
 * B12 0	B12 0	B34 0	B34 0	B56 0	B56 0	B78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_RGB_TO_UV_CHECK(convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_bt601, rgb_8bit_to_yuv_10bit_coef[1], rgb_8bit_to_yuv_10bit_off[1], CHECK_FN_10BIT_3IN);


/*
 * Convert 3 vectors of 8 short downsampled 422 8-bit R, G, B into 1 vector of 8 short 10-bit U-V
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			15 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 512 ] + [ -0.404		-1.356		 1.756	]	( G )
 * V = 	[ 512 ] + [  1.756		-1.596		-0.160	]	( B )
 *
 *				All coeffs are left-shifted by 13 bits
 * 					[ -3310		-11108		 14385	]
 * 					[  14385	-13074		-1311	]
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R12 0	R12 0	R34 0	R34 0	R56 0	R56 0	R78 0	R78 0
 *
 * gVect
 * G12 0	G12 0	G34 0	G34 0	G56 0	G56 0	G78 0	G78 0
 *
 * bVect
 * B12 0	B12 0	B34 0	B34 0	B56 0	B56 0	B78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_RGB_TO_UV_CHECK(convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_bt709, rgb_8bit_to_yuv_10bit_coef[2], rgb_8bit_to_yuv_10bit_off[2], CHECK_FN_10BIT_3IN);


/*
 *
 *		A	G		R	B
 *
 * 		to
 *
 *		Y
 *
 */


/*
 * Convert 4 vectors of 8 short AG, RB into 1 vector of 8 short Y
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			17 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  0.299		 0.587		 0.114	]	( R )
 *
 *				All coeffs are left-shifted by 15 bits
 * 					[  9798	 	19325		 3736	]
 *
 * Note: the Y calculation involves only positive values and coefficients but
 * the SSE2 instruction (PMADDWD) uses 16-bit signed operands. Hence the 15-bit
 * shift (instead of 16, which would cause the yG coefficient to go over the
 * 32767 limit).
 *
 * INPUT:
 *
 * 4 vectors of 8 short:
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
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_AG_RB_TO_Y_CHECK(convert_ag_rb_vectors_to_y_vector, convert_ga_br_vectors_to_y_vector, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0], CHECK_FN_8BIT_4IN);

/*
 * Convert 4 vectors of 8 short AG, RB (or GA, BR) into 1 vector of 8 short Y
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			18 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  16  ] + [  0.257		 0.504		 0.098	]	( R )
 *
 *				All coeffs are left-shifted by 15 bits
 * 					[  8421	 	16515		 3211	]
 *
 * Note: the Y calculation involves only positive values and coefficients but
 * the SSE2 instruction (PMADDWD) uses 16-bit signed operands. Hence the 15-bit
 * shift (instead of 16, which would cause the yG coefficient to go over the
 * 32767 limit).
 *
 * INPUT:
 *
 * 4 vectors of 8 short:
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
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_AG_RB_TO_Y_CHECK(convert_ag_rb_vectors_to_y_vector_bt601, convert_ga_br_vectors_to_y_vector_bt601, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1], CHECK_FN_8BIT_4IN);


/*
 * Convert 4 vectors of 8 short AG, RB (or GA , BR) into 1 vector of 8 short Y
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			18 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  16  ] + [  0.183		 0.614		 0.062	]	( R )
 *
 *				All coeffs are left-shifted by 15 bits
 * 					[  5997	 	20119		 2032	]
 *
 * Note: the Y calculation involves only positive values and coefficients but
 * the SSE2 instruction (PMADDWD) uses 16-bit signed operands. Hence the 15-bit
 * shift (instead of 16, which would cause the yG coefficient to go over the
 * 32767 limit).
 *
 * INPUT:
 *
 * 4 vectors of 8 short:
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
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 */
DECLARE_AG_RB_TO_Y_CHECK(convert_ag_rb_vectors_to_y_vector_bt709, convert_ga_br_vectors_to_y_vector_bt709, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2], CHECK_FN_8BIT_4IN);




/*
 *
 *		A	G		R	B
 *
 * 		to
 *
 *		U	V
 *
 */




/*
 * Convert 4 vectors of 8 short downsampled 422 AG, RB (or GA, BR) into 1 vector of 8 short U-V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			18 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.169		-0.331		 0.500	]	( G )
 * V = 	[ 128 ] + [  0.500		-0.419		-0.081	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -11076	-21692		 32767	]
 * 					[  32767	-27460		-5308	]
 *
 *	Note: the R-V & B-U coeffs (32767) should really be 32768 but because we need them to be
 *			16-bit signed integers, they have been capped to the maximum value for this
 *			type. 32767 maps to 0.499985 instead of 0.500 which is an acceptable
 *			approximation.
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * agVect
 * A12 0	G12 0	A34 0	G34 0	A56 0	G56 0	A78 0	G78 0
 *
 * rbVect
 * R12 0	B12 0	R34 0	B34 0	R56 0	B56 0	R78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_AG_RB_TO_UV_CHECK(convert_downsampled_422_ag_rb_vectors_to_uv_vector, convert_downsampled_422_ga_br_vectors_to_uv_vector, rgb_8bit_to_yuv_8bit_coef[0], rgb_8bit_to_yuv_8bit_off[0], CHECK_FN_8BIT_2IN);


/*
 * Convert 2 vectors of 8 short downsampled 422 AG, RB (or GA, BR) into 1 vector of 8 short U-V
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			18 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.148		-0.291		 0.439	]	( G )
 * V = 	[ 128 ] + [  0.439		-0.368		-0.071	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -9699		-19071		 28770	]
 * 					[  28770	-24117		-4653	]
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * gaVect
 * G12 0	A12 0	G34 0	A34 0	G56 0	A56 0	G78 0	A78 0
 *
 * brVect
 * B12 0	R12 0	B34 0	R34 0	B56 0	R56 0	B78 0	R78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_AG_RB_TO_UV_CHECK(convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt601, convert_downsampled_422_ga_br_vectors_to_uv_vector_bt601, rgb_8bit_to_yuv_8bit_coef[1], rgb_8bit_to_yuv_8bit_off[1], CHECK_FN_8BIT_2IN);

/*
 * Convert 2 vectors of 8 short downsampled 422 AG, RB ( or GA, BR) into 1 vector of 8 short U-V
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			18 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.101		-0.339		 0.439	]	( G )
 * V = 	[ 128 ] + [  0.439		-0.399		-0.040	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -6619		-22217		 28770	]
 * 					[  28770	-26149		-2621	]
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * agVect
 * A12 0	G12 0	A34 0	G34 0	A56 0	G56 0	A78 0	G78 0
 *
 * rbVect
 * R12 0	B12 0	R34 0	B34 0	R56 0	B56 0	R78 0	B78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */
DECLARE_AG_RB_TO_UV_CHECK(convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709, convert_downsampled_422_ga_br_vectors_to_uv_vector_bt709, rgb_8bit_to_yuv_8bit_coef[2], rgb_8bit_to_yuv_8bit_off[2], CHECK_FN_8BIT_2IN);

