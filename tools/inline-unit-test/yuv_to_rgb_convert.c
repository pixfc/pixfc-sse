/*
 * yuv_to_rgb_convert.c
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
#include "yuv_to_rgb_convert.h"

#define CHECK_FN_8BIT_1(fn_suffix)\
    uint32_t    check_ ## fn_suffix() {\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_8BIT,  compare_16bit_output);\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_8BIT,  compare_16bit_output);\
        return 0;\
    }

#define CHECK_FN_8BIT_2(fn_suffix)\
	uint32_t    check_ ## fn_suffix() {\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_1_Y_UV_8BIT_VECT1,  3, MAX_DIFF_8BIT,  compare_16bit_output);\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2, DECLARE_1_Y_UV_8BIT_VECT2, 3, MAX_DIFF_8BIT,  compare_16bit_output);\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_1_Y_UV_8BIT_VECT1,  3, MAX_DIFF_8BIT,  compare_16bit_output);\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_1_Y_UV_8BIT_VECT2, 3, MAX_DIFF_8BIT,  compare_16bit_output);\
		return 0;\
	}

#define CHECK_FN_10BIT_1(fn_suffix)\
    uint32_t    check_ ## fn_suffix() {\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_3_10BIT_VECT, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3_sse41, DECLARE_3_8BIT_VECT, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
        CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3_sse41, DECLARE_3_10BIT_VECT, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
        return 0;\
    }

#define CHECK_FN_10BIT_2(fn_suffix)\
	uint32_t    check_ ## fn_suffix() {\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_1_Y_UV_10BIT_VECT1, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3, DECLARE_1_Y_UV_10BIT_VECT2, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3_sse41, DECLARE_1_Y_UV_10BIT_VECT1, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
		CHECK_INLINE_1IN(fn_suffix ## _scalar, fn_suffix ## _sse2_ssse3_sse41, DECLARE_1_Y_UV_10BIT_VECT2, 3, MAX_DIFF_10BIT,  compare_16bit_output);\
		return 0;\
	}


#define DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(fn_prefix, coeffs, offset, check_fn_prefix) \
    void fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
		uint16_t* in = (uint16_t*) input;\
		uint16_t* out = (uint16_t*) output;\
		uint32_t  pixel_count;\
		\
		for(pixel_count = 0; pixel_count < 8; pixel_count += 2) {\
			out[0] = (uint16_t)((in[0] + offset[0]) * coeffs[0][0] + (in[8] + offset[1]) * coeffs[0][1] + (in[8 + 1] + offset[2]) * coeffs[0][2]);\
			out[8] = (uint16_t)((in[0] + offset[0]) * coeffs[1][0] + (in[8] + offset[1]) * coeffs[1][1] + (in[8 + 1] + offset[2]) * coeffs[1][2]);\
			out[16]= (uint16_t)((in[0] + offset[0]) * coeffs[2][0] + (in[8] + offset[1]) * coeffs[2][1] + (in[8 + 1] + offset[2]) * coeffs[2][2]);\
			out++;\
			in++;\
			out[0] = (uint16_t)((in[0] + offset[0]) * coeffs[0][0] + (in[8 - 1] + offset[1]) * coeffs[0][1] + (in[8] + offset[2]) * coeffs[0][2]);\
			out[8] = (uint16_t)((in[0] + offset[0]) * coeffs[1][0] + (in[8 - 1] + offset[1]) * coeffs[1][1] + (in[8] + offset[2]) * coeffs[1][2]);\
			out[16]= (uint16_t)((in[0] + offset[0]) * coeffs[2][0] + (in[8 - 1] + offset[1]) * coeffs[2][1] + (in[8] + offset[2]) * coeffs[2][2]);\
			out++;\
			in++;\
		}\
	}\
    \
    check_fn_prefix ## _2(fn_prefix);\


#define DECLARE_CONVERT_CHECK(fn_prefix, coeffs, offset, check_fn_prefix) \
	void fn_prefix ## _scalar(__m128i* input, __m128i* output) {\
		uint16_t* in = (uint16_t*) input;\
		uint16_t* out = (uint16_t*) output;\
		uint32_t  pixel_count;\
		\
		for(pixel_count = 0; pixel_count < 8; pixel_count += 2) {\
			out[0] = (uint16_t)((in[0] + offset[0]) * coeffs[0][0] + (in[8] + offset[1]) * coeffs[0][1] + (in[8 + 1] + offset[2]) * coeffs[0][2]);\
			out[8] = (uint16_t)((in[0] + offset[0]) * coeffs[1][0] + (in[8] + offset[1]) * coeffs[1][1] + (in[8 + 1] + offset[2]) * coeffs[1][2]);\
			out[16]= (uint16_t)((in[0] + offset[0]) * coeffs[2][0] + (in[8] + offset[1]) * coeffs[2][1] + (in[8 + 1] + offset[2]) * coeffs[2][2]);\
			out++;\
			in++;\
			out[0] = (uint16_t)((in[0] + offset[0]) * coeffs[0][0] + (in[16 - 1] + offset[1]) * coeffs[0][1] + (in[16] + offset[2]) * coeffs[0][2]);\
			out[8] = (uint16_t)((in[0] + offset[0]) * coeffs[1][0] + (in[16 - 1] + offset[1]) * coeffs[1][1] + (in[16] + offset[2]) * coeffs[1][2]);\
			out[16]= (uint16_t)((in[0] + offset[0]) * coeffs[2][0] + (in[16 - 1] + offset[1]) * coeffs[2][1] + (in[16] + offset[2]) * coeffs[2][2]);\
			out++;\
			in++;\
		}\
	}\
	\
	check_fn_prefix ## _1(fn_prefix)


/*
 * Convert 2 vectors of 8 short Y, UY into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 * Total latency: 			22 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1		0		1.4		]	( Y )
 * G = 	[ 1		-0.343	-0.711	]	( U - 128 )
 * B = 	[ 1		1.765	0		]	( V - 128 )
 *
 *
 * 		[ 256	0		358		]	left shift by 8
 * 		[ 256	-88		-182	]
 * 		[ 256	452		0		]
 *
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * OUTPUT:
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
 */
DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors, yuv_8bit_to_rgb_8bit_coef[0], yuv_8bit_to_rgb_8bit_off[0], CHECK_FN_8BIT);

/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			38 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1		0		1.4		]	( Y )
 * G = 	[ 1		-0.343	-0.711	]	( U - 128 )
 * B = 	[ 1		1.765	0		]	( V - 128 )
 *
 *
 * 		[ 256	0		358		]	left shift by 8
 * 		[ 256	-88		-182	]
 * 		[ 256	452		0		]
 *
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect Odd
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * uvVect Even
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 *
 * OUTPUT:
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
 */
DECLARE_CONVERT_CHECK(convert_y_uv_vectors_to_rgb_vectors, yuv_8bit_to_rgb_8bit_coef[0], yuv_8bit_to_rgb_8bit_off[0], CHECK_FN_8BIT);


/*
 *
 *
 *	B T 6 0 1  
 *
 *  &
 * 
 *  B T 7 0 9
 *
 *	&
 *
 *	10 - B I T   TO   8 - B I T
 *
 *  C O N V E R S I O N S
 *
 *
 */

/*
 *
 *	8-bit to 8-bit NNB SSE2
 *
 */

/*
 * Convert 2 vectors of 8 short Y, UY into 3 vectors of 8 short R, G & B
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 * Total latency: 			28 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164		0		1.596	]	( Y - 16)
 * G = 	[ 1.164		-0.392	-0.813	]	( U - 128 )
 * B = 	[ 1.164		3.017	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 15 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 38142		0		204		]
 * 		[ 38142		-50		-104	]
 * 		[ 38142		386		0		]
 *
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * OUTPUT:
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
 */
DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601, yuv_8bit_to_rgb_8bit_coef[1], yuv_8bit_to_rgb_8bit_off[1], CHECK_FN_8BIT);


/*
 * Convert 2 vectors of 8 short Y, UY into 3 vectors of 8 short R, G & B
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 *
 * Total latency: 			28 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164		0		1.793	]	( Y - 16)
 * G = 	[ 1.164		-0.213	-0.533	]	( U - 128 )
 * B = 	[ 1.164		2.112	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 15 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 38142		0		229		]
 * 		[ 38142		-27		-68		]
 * 		[ 38142		270		0		]
 *
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * OUTPUT:
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
 */
DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709, yuv_8bit_to_rgb_8bit_coef[2], yuv_8bit_to_rgb_8bit_off[2], CHECK_FN_8BIT);




/*
 *
 *	8-bit to 8-bit Upsampled YUV to RGB SSE2 conversion
 *
 */

/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			43 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164		0		1.596		]	( Y - 16)
 * G = 	[ 1.164		-0.392	-0.813	]	( U - 128 )
 * B = 	[ 1.164		3.017	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 15 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 38142		0		204		]	
 * 		[ 38142		-50		-104	]
 * 		[ 38142		386		0		]
 * 
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect Odd
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * uvVect Even
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 *
 * OUTPUT:
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
 */
DECLARE_CONVERT_CHECK(convert_y_uv_vectors_to_rgb_vectors_bt601, yuv_8bit_to_rgb_8bit_coef[1], yuv_8bit_to_rgb_8bit_off[1], CHECK_FN_8BIT);

/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			43 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164		0		1.793	]	( Y - 16)
 * G = 	[ 1.164		-0.213	-0.533	]	( U - 128 )
 * B = 	[ 1.164		2.112	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 15 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 38142		0		229		]
 * 		[ 38142		-27		-68		]
 * 		[ 38142		270		0		]
 * 
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect Odd
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * uvVect Even
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 *
 * OUTPUT:
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
 */
DECLARE_CONVERT_CHECK(convert_y_uv_vectors_to_rgb_vectors_bt709, yuv_8bit_to_rgb_8bit_coef[2], yuv_8bit_to_rgb_8bit_off[2], CHECK_FN_8BIT);
	

/*
 *
 * 10-bit to 8-bit NNB SSE2 / SSSE3 conversion
 *
 */






/*
 * Convert 2 vectors of 8 short 10-bit Y, UV into 3 vectors of 8 short 8-bit R, G & B
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 * Total latency: 			25 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164/4		0			1.596/4		]	( Y - 64 )
 * G = 	[ 1.164/4		-0.392/4	-0.813/4	]	( U - 512 )
 * B = 	[ 1.164/4		2.017/4		0			]	( V - 512 )
 *
 * Y coeffs left shifted by 16 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		51	]
 * 		[ 19071		-13		-26	]
 * 		[ 19071		64		0	]
 *
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * OUTPUT:
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
 */
DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601, yuv_10bit_to_rgb_8bit_coef[1], yuv_10bit_to_rgb_8bit_off[1], CHECK_FN_10BIT);

/*
 * Convert 2 vectors of 8 short 10-bit Y, UV into 3 vectors of 8 short 8-bit R, G & B
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 *
 * Total latency: 			23 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164/4		0			1.793/4		]	( Y - 64)
 * G = 	[ 1.164/4		-0.213/4	-0.533/4	]	( U - 512 )
 * B = 	[ 1.164/4		2.112/4		0			]	( V - 512 )
 *
 * Y coeffs left shifted by 16 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		57		]
 * 		[ 19071		-7		-17		]
 * 		[ 19071		68		0		]
 *
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * OUTPUT:
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
 */
DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709, yuv_10bit_to_rgb_8bit_coef[2], yuv_10bit_to_rgb_8bit_off[2], CHECK_FN_10BIT);


/*
 * Convert 2 vectors of 8 short 10-bit Y, UV into 3 vectors of 8 short 8-bit R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 * Total latency: 			16 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1		0			1.4/4		]	( Y )
 * G = 	[ 1		-0.343/4	-0.711/4	]	( U - 128 )
 * B = 	[ 1		1.765/4		0			]	( V - 128 )
 *
 *
 * 		[ 1/4	0		90		]
 * 		[ 1/4	-22		-46		]	U, V coeffs left shifted by 8
 * 		[ 1/4	113		0		]
 *
 *
 * INPUT:
 *
 * 2 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 * OUTPUT:
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
 */
DECLARE_NNB_UPSAMPLE_N_CONVERT_CHECK(nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors, yuv_10bit_to_rgb_8bit_coef[0], yuv_10bit_to_rgb_8bit_off[0], CHECK_FN_10BIT);



/*
 * Upsampled SSE2 / SSSE3 conversion
 */

/*
 * Convert 3 vectors of 8 short 10-bit Y, UV into 3 vectors of 8 short 8-bit R, G & B
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			37 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164/4		0			1.596/4		]	( Y - 64 )
 * G = 	[ 1.164/4		-0.392/4	-0.813/4	]	( U - 512 )
 * B = 	[ 1.164/4		2.017/4		0			]	( V - 512 )
 *
 * Y coeffs left shifted by 16 bits
 * U & V coeffs left shifted by 8 bits
 * 		[ 19071		0		102	]
 * 		[ 19071		-25		-52	]
 * 		[ 19071		129		0	]
 *
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect Odd
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * uvVect Even
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 *
 * OUTPUT:
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
 */
DECLARE_CONVERT_CHECK(convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601, yuv_10bit_to_rgb_8bit_coef[1], yuv_10bit_to_rgb_8bit_off[1], CHECK_FN_10BIT);


/*
 * Convert 2 vectors of 8 short 10-bit Y, UV into 3 vectors of 8 short 8-bit R, G & B
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			37 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164/4		0			1.793/4		]	( Y - 64)
 * G = 	[ 1.164/4		-0.213/4	-0.533/4	]	( U - 512 )
 * B = 	[ 1.164/4		2.112/4		0			]	( V - 512 )
 *
 * Y coeffs left shifted by 16 bits
 * U & V coeffs left shifted by 8 bits
 * 		[ 19071		0		115		]
 * 		[ 19071		-14		-34		]
 * 		[ 19071		135		0		]
 *
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect Odd
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * uvVect Even
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 *
 * OUTPUT:
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
 */
DECLARE_CONVERT_CHECK(convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709, yuv_10bit_to_rgb_8bit_coef[2], yuv_10bit_to_rgb_8bit_off[2], CHECK_FN_10BIT);




/*
 * Convert 2 vectors of 8 short 10-bit Y, UV into 3 vectors of 8 short 8-bit R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			33 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1		0			1.4/4		]	( Y )
 * G = 	[ 1		-0.343/4	-0.711/4	]	( U - 128 )
 * B = 	[ 1		1.765/4		0			]	( V - 128 )
 *
 *
 * 		[ 1/4	0		90		]
 * 		[ 1/4	-22		-46		]	U, V coeffs left shifted by 8
 * 		[ 1/4	113		0		]
 *
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect Odd
 * U1 0		V1 0	U3 0	V3 0	U5 0	V5 0	U7 0	V7 0
 *
 * uvVect Even
 * U2 0		V2 0	U4 0	V4 0	U6 0	V6 0	U8 0	V8 0
 *
 * OUTPUT:
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
 */
DECLARE_CONVERT_CHECK(convert_10bit_y_uv_vectors_to_8bit_rgb_vectors, yuv_10bit_to_rgb_8bit_coef[0], yuv_10bit_to_rgb_8bit_off[0], CHECK_FN_10BIT);


