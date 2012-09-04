/*
 * rgb_to_yuv_convert.h
 *
 * Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
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

#ifndef RGB_TO_YUV_CONVERT_H_
#define RGB_TO_YUV_CONVERT_H_


#include "debug_support.h"
#include "platform_util.h"

#ifdef __INTEL_CPU__

#include <emmintrin.h>
#include <tmmintrin.h>



/*
 *
 *		R		G		B
 *
 * 		to
 *
 *		Y
 *
 */

#define DEFINE_R_G_B_TO_Y_INLINE(fn_name, rCoeffVal, gCoeffVal, bCoeffVal)\
	EXTERN_INLINE void fn_name(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(rYCoeffs, rCoeffVal, rCoeffVal);\
		CONST_M128I(gYCoeffs, gCoeffVal, gCoeffVal);\
		CONST_M128I(bYCoeffs, bCoeffVal, bCoeffVal);\
		M128I(rScratch, 0x0LL, 0x0LL);\
		M128I(gScratch, 0x0LL, 0x0LL);\
		M128I(bScratch, 0x0LL, 0x0LL);\
		_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));	/* PMULHUW		3	1*/\
		_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));	/* PMULHUW		3	1*/\
		_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));	/* PMULHUW		3	1*/\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));					/* PADDW		1	0.5*/\
		*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(bScratch));			/* PADDW		1	0.5*/\
	}\

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
DEFINE_R_G_B_TO_Y_INLINE(convert_r_g_b_vectors_to_y_vector_sse2, 0x4C8B4C8B4C8B4C8BLL, 0x9646964696469646LL, 0x1D2F1D2F1D2F1D2FLL)



/*
 * R, G, B vectors to Y vector - bt{601,709}
 */
#define DEFINE_R_G_B_TO_Y_INLINE2(fn_name, rCoeffVal, gCoeffVal, bCoeffVal, offsetVal)\
	EXTERN_INLINE void fn_name(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(rYCoeffs, rCoeffVal, rCoeffVal);\
		CONST_M128I(gYCoeffs, gCoeffVal, gCoeffVal);\
		CONST_M128I(bYCoeffs, bCoeffVal, bCoeffVal);\
		CONST_M128I(resultOffset, offsetVal, offsetVal);\
		M128I(rScratch, 0x0LL, 0x0LL);\
		M128I(gScratch, 0x0LL, 0x0LL);\
		M128I(bScratch, 0x0LL, 0x0LL);\
		_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));\
		_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));\
		_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));\
		*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(resultOffset));\
	}
/*
 * The previous macro expands to:
 *
 * 	EXTERN_INLINE void convert_r_g_b_vectors_to_y_vector_bt709_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {
 *		CONST_M128I(rYCoeffs, 0x2ED92ED92ED92ED9LL, 0x2ED92ED92ED92ED9LL);
 *		CONST_M128I(gYCoeffs, 0x9D2F9D2F9D2F9D2FLL, 0x9D2F9D2F9D2F9D2FLL);
 *		CONST_M128I(bYCoeffs, 0x0FDF0FDF0FDF0FDFLL, 0x0FDF0FDF0FDF0FDFLL);
 *		CONST_M128I(add_16, 0x0010001000100010LL, 0x0010001000100010LL);
 *
 *		M128I(rScratch, 0x0LL, 0x0LL);
 *		M128I(gScratch, 0x0LL, 0x0LL);
 *		M128I(bScratch, 0x0LL, 0x0LL);
 *
 *		//
 *		// Y
 *		// R coeffs
 *		// Multiply R values by 16-bit left-shifted Y coeffs and keep highest 16 bits
 *		_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));		// PMULHUW		3	1
 *
 *		// G coeffs
 *		// Multiply G values by 16-bit left-shifted Y coeffs and keep highest 16 bits
 *		_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));		// PMULHUW		3	1
 *
 *		// B coeffs
 *		// Multiply B values by 16-bit left-shifted Y coeffs and keep highest 16 bits
 *		_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));		// PMULHUW		3	1
 *
 *		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));						// PADDW		1	0.5
 *		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));						// PADDW		1	0.5
 *		*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(add_16));					// PADDW		1	0.5
 *		// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
 *	}
 *
 */


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
DEFINE_R_G_B_TO_Y_INLINE2(convert_r_g_b_vectors_to_y_vector_bt601_sse2, 0x41CB41CB41CB41CBLL, 0x8106810681068106LL, 0x1917191719171917LL, 0x0010001000100010LL);

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
DEFINE_R_G_B_TO_Y_INLINE2(convert_r_g_b_vectors_to_y_vector_bt709_sse2, 0x2ED92ED92ED92ED9LL, 0x9D2F9D2F9D2F9D2FLL, 0x0FDF0FDF0FDF0FDFLL, 0x0010001000100010LL);


/*
 * 8-bit RGB to 10-bit Y Full range
 */
#define DEFINE_8BIT_R_G_B_TO_10BIT_Y_INLINE(fn_name, rCoeffVal, gCoeffVal, bCoeffVal, valLeftShift)\
	EXTERN_INLINE void fn_name(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(rYCoeffs, rCoeffVal, rCoeffVal);\
		CONST_M128I(gYCoeffs, gCoeffVal, gCoeffVal);\
		CONST_M128I(bYCoeffs, bCoeffVal, bCoeffVal);\
		M128I(rScratch, 0x0LL, 0x0LL);\
		M128I(gScratch, 0x0LL, 0x0LL);\
		M128I(bScratch, 0x0LL, 0x0LL);\
		_M(rScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[0], valLeftShift);	/* PSLLW		1	1*/\
		_M(gScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[1], valLeftShift);	/* PSLLW		1	1*/\
		_M(bScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[2], valLeftShift);	/* PSLLW		1	1*/\
		_M(rScratch) = _mm_mulhi_epu16(_M(rScratch), _M(rYCoeffs));	/* PMULHUW		3	1*/\
		_M(gScratch) = _mm_mulhi_epu16(_M(gScratch), _M(gYCoeffs));	/* PMULHUW		3	1*/\
		_M(bScratch) = _mm_mulhi_epu16(_M(bScratch), _M(bYCoeffs));	/* PMULHUW		3	1*/\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));					/* PADDW		1	0.5*/\
		*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(bScratch));			/* PADDW		1	0.5*/\
	}\


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
DEFINE_8BIT_R_G_B_TO_10BIT_Y_INLINE(convert_8bit_r_g_b_vectors_to_10bit_y_vector_sse2,
								0x4C8B4C8B4C8B4C8BLL, // rYCoeff
								0x9646964696469646LL, // gYCoeff
								0x1D2F1D2F1D2F1D2FLL, // bYCoeff
								2	// left shift RGB value
		)


/*
 * 8-bit R, G, B vectors to 10-bit Y vector - bt{601,709}
 */
#define DEFINE_8BIT_R_G_B_TO_10BIT_Y_INLINE2(fn_name, rCoeffVal, gCoeffVal, bCoeffVal, valLeftShift, offsetVal)\
	EXTERN_INLINE void fn_name(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(rYCoeffs, rCoeffVal, rCoeffVal);\
		CONST_M128I(gYCoeffs, gCoeffVal, gCoeffVal);\
		CONST_M128I(bYCoeffs, bCoeffVal, bCoeffVal);\
		CONST_M128I(resultOffset, offsetVal, offsetVal);\
		M128I(rScratch, 0x0LL, 0x0LL);\
		M128I(gScratch, 0x0LL, 0x0LL);\
		M128I(bScratch, 0x0LL, 0x0LL);\
		_M(rScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[0], valLeftShift);	/* PSLLW		1	1*/\
		_M(gScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[1], valLeftShift);	/* PSLLW		1	1*/\
		_M(bScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[2], valLeftShift);	/* PSLLW		1	1*/\
		_M(rScratch) = _mm_mulhi_epu16(_M(rScratch), _M(rYCoeffs));\
		_M(gScratch) = _mm_mulhi_epu16(_M(gScratch), _M(gYCoeffs));\
		_M(bScratch) = _mm_mulhi_epu16(_M(bScratch), _M(bYCoeffs));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));\
		*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(resultOffset));\
	}\


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
DEFINE_8BIT_R_G_B_TO_10BIT_Y_INLINE2(convert_8bit_r_g_b_vectors_to_10bit_y_vector_bt601_sse2,
									0x41CB41CB41CB41CBLL,// rYCoeff
									0x8106810681068106LL,// gYCoeff
									0x1917191719171917LL,// bYCoeff
									2,	// rgb left shift value
									0x0040004000400040LL // offset
	);

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
DEFINE_8BIT_R_G_B_TO_10BIT_Y_INLINE2(convert_8bit_r_g_b_vectors_to_10bit_y_vector_bt709_sse2,
									0x2ED92ED92ED92ED9LL,
									0x9D2F9D2F9D2F9D2FLL,
									0x0FDF0FDF0FDF0FDFLL,
									2,
									0x0040004000400040LL
		);



/*
 * 8-bit RGB to 10-bit Y Full range, bt601 & bt709
 */


/*
 * Convert 3 vectors of 8 short 10-bit R, G, B into 1 vector of 8 short 8-bit Y
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			11 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  0.075		0.147		0.029	]	( R )
 *
 *				All coeffs are left-shifted by 16 bits
  * 			  [	  4899		9617		1868	]
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
DEFINE_R_G_B_TO_Y_INLINE(convert_10bit_r_g_b_vectors_to_8bit_y_vector_sse2, 0x1323132313231323LL, 0x2591259125912591LL, 0x074C074C074C074CLL)


/*
 * Convert 3 vectors of 8 short 10-bit R, G, B into 1 vector of 8 short 9-bit Y
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  16  ] + [  0.064		  0.126		0.025	]	( R )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[  4211		  8258		1606	]
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
DEFINE_R_G_B_TO_Y_INLINE2(convert_10bit_r_g_b_vectors_to_8bit_y_vector_bt601_sse2, 0x1073107310731073LL, 0x2042204220422042LL, 0x0646064606460646LL, 0x0010001000100010LL);

/*
 * Convert 3 vectors of 8 short 10-bit R, G, B into 1 vector of 8 short 8-bit Y
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  16  ] + [  0.046		 0.154		0.016	]	( R )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[  2998		10060		1016	]
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
DEFINE_R_G_B_TO_Y_INLINE2(convert_10bit_r_g_b_vectors_to_8bit_y_vector_bt709_sse2, 0x0BB60BB60BB60BB6LL, 0x274C274C274C274CLL, 0x03F803F803F803F8LL, 0x0010001000100010LL);




/*
 * 10-bit RGB to 10-bit Y Full range, bt601 & bt709
 */




/*
 * Convert 3 vectors of 8 short 10-bit R, G, B into 1 vector of 8 10-bit short Y
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
DEFINE_R_G_B_TO_Y_INLINE(convert_10bit_r_g_b_vectors_to_10bit_y_vector_sse2, 0x4C8B4C8B4C8B4C8BLL, 0x9646964696469646LL, 0x1D2F1D2F1D2F1D2FLL)



/*
 * Convert 3 vectors of 8 short 10-bit R, G, B into 1 vector of 8 short 10-bit Y
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  64  ] + [  0.257		 0.504		 0.098	]	( R )
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
DEFINE_R_G_B_TO_Y_INLINE2(convert_10bit_r_g_b_vectors_to_10bit_y_vector_bt601_sse2, 0x41CB41CB41CB41CBLL, 0x8106810681068106LL, 0x1917191719171917LL, 0x0040004000400040LL);

/*
 * Convert 3 vectors of 8 short 10-bit R, G, B into 1 vector of 8 short 10-bit Y
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  64  ] + [  0.183		 0.614		 0.062	]	( R )
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
DEFINE_R_G_B_TO_Y_INLINE2(convert_10bit_r_g_b_vectors_to_10bit_y_vector_bt709_sse2, 0x2ED92ED92ED92ED9LL, 0x9D2F9D2F9D2F9D2FLL, 0x0FDF0FDF0FDF0FDFLL, 0x0040004000400040LL);



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
 * R, G, B vectors to UV vectors - full range, bt{601, 709}
 */
#define DEFINE_R_G_B_TO_UV_INLINE(fn_name, rUVCoeffVal, gUVCoeffVal, bUVCoeffVal, offsetVal)\
	EXTERN_INLINE void fn_name(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_uv_vector) {\
		CONST_M128I(rUVCoeffsInterleaved, rUVCoeffVal, rUVCoeffVal);\
		CONST_M128I(gUVCoeffsInterleaved, gUVCoeffVal, gUVCoeffVal);\
		CONST_M128I(bUVCoeffsInterleaved, bUVCoeffVal, bUVCoeffVal);\
		CONST_M128I(resultOffset, offsetVal, offsetVal);\
		M128I(rScratch, 0x0LL, 0x0LL);\
		M128I(gScratch, 0x0LL, 0x0LL);\
		M128I(bScratch, 0x0LL, 0x0LL);\
		_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[0], _M(rUVCoeffsInterleaved));\
		_M(gScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[1], _M(gUVCoeffsInterleaved));\
		_M(bScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[2], _M(bUVCoeffsInterleaved));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));\
		*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(resultOffset));\
	}
/*
 * The previous macro expands to
 *
 * EXTERN_INLINE void convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_uv_vector) {
 *		CONST_M128I(rUVCoeffsInterleaved, 0x7FFFD4BC7FFFD4BCLL, 0x7FFFD4BC7FFFD4BCLL);
 *		CONST_M128I(gUVCoeffsInterleaved, 0x94BCAB4494BCAB44LL, 0x94BCAB4494BCAB44LL);
 *		CONST_M128I(bUVCoeffsInterleaved, 0xEB447FFFEB447FFFLL, 0xEB447FFFEB447FFFLL);
 *		CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
 *
 *		M128I(rScratch, 0x0LL, 0x0LL);
 *		M128I(gScratch, 0x0LL, 0x0LL);
 *		M128I(bScratch, 0x0LL, 0x0LL);
 *
 *		//
 *		// r UV
 *		_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[0], _M(rUVCoeffsInterleaved));// PMULHW	3	1
 *
 *		// g UV
 *		_M(gScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[1], _M(gUVCoeffsInterleaved));// PMULHW	3	1
 *
 *		// b UV
 *		_M(bScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[2], _M(bUVCoeffsInterleaved));// PMULHW	3	1
 *
 *		// r UV + g UV + b UV
 *		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));							//	PADDW	1	0.5
 *		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));							//	PADDW	1	0.5
 *
 *		// U,V + 128
 *		*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(add128));					//	PADDW	1	0.5
 *		// U12 V12			U34 V34			U56 V56			U78 V78
 *	};
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2, 0x7FFFD4BC7FFFD4BCLL, 0x94BCAB4494BCAB44LL, 0xEB447FFFEB447FFFLL, 0x0080008000800080LL);


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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt601_sse2, 0x7062DA1D7062DA1DLL, 0xA1CBB581A1CBB581LL, 0xEDD37062EDD37062LL, 0x0080008000800080LL);


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

DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2, 0x7062E6257062E625LL, 0x99DBA93799DBA937LL, 0xF5C37062F5C37062LL, 0x0080008000800080LL);



/*
 * 8-bit R, G, B vectors to 10-bit UV vectors - full range, bt{601, 709}
 */
#define DEFINE_8BIT_R_G_B_TO_10BIT_UV_INLINE(fn_name, rUVCoeffVal, gUVCoeffVal, bUVCoeffVal, valLeftShift, offsetVal)\
	EXTERN_INLINE void fn_name(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_uv_vector) {\
		CONST_M128I(rUVCoeffsInterleaved, rUVCoeffVal, rUVCoeffVal);\
		CONST_M128I(gUVCoeffsInterleaved, gUVCoeffVal, gUVCoeffVal);\
		CONST_M128I(bUVCoeffsInterleaved, bUVCoeffVal, bUVCoeffVal);\
		CONST_M128I(resultOffset, offsetVal, offsetVal);\
		M128I(rScratch, 0x0LL, 0x0LL);\
		M128I(gScratch, 0x0LL, 0x0LL);\
		M128I(bScratch, 0x0LL, 0x0LL);\
		_M(rScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[0], valLeftShift);	/* PSLLW		1	1*/\
		_M(gScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[1], valLeftShift);	/* PSLLW		1	1*/\
		_M(bScratch) = _mm_slli_epi16(in_3_v16i_r_g_b_vectors[2], valLeftShift);	/* PSLLW		1	1*/\
		_M(rScratch) = _mm_mulhi_epi16(_M(rScratch), _M(rUVCoeffsInterleaved));\
		_M(gScratch) = _mm_mulhi_epi16(_M(gScratch), _M(gUVCoeffsInterleaved));\
		_M(bScratch) = _mm_mulhi_epi16(_M(bScratch), _M(bUVCoeffsInterleaved));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));\
		_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));\
		*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(resultOffset));\
	}


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
DEFINE_8BIT_R_G_B_TO_10BIT_UV_INLINE(convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_sse2,
		0x4000EA5E4000EA5ELL,
		0xCA5ED5A2CA5ED5A2LL,
		0xF5A24000F5A24000LL,
		3,
		0x0200020002000200LL);


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
 *				All coeffs are left-shifted by 13 bits
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
DEFINE_8BIT_R_G_B_TO_10BIT_UV_INLINE(convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_bt601_sse2,
									0x3831ED0E3831ED0ELL,
									0xD0E5DAC1D0E5DAC1LL,
									0xF6E93831F6E93831LL,
									3,
									0x0200020002000200LL
		);


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

DEFINE_8BIT_R_G_B_TO_10BIT_UV_INLINE(convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_bt709_sse2,
									0x3831F3123831F312LL,
									0xCCEED49CCCEED49CLL,
									0xFAE13831FAE13831LL,
									3,
									0x0200020002000200LL
		);





/*
 * 10-bit R, G, B vectors to UV 8-bit vectors - full range, bt{601, 709}
 */

/*
 * Convert 3 vectors of 8 short downsampled 422 10-bit R, G, B into 1 vector of 8 short 8-bit U-V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.042		-0.083		 0.125	]	( G )
 * V = 	[ 128 ] + [  0.125		-0.105		-0.020	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 *					[ -2769		-5423		8,192	]
 *					[  8192		-6865	 	-1327	]
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_10bit_r_g_b_vectors_to_8bit_uv_vector_sse2, 0x2000F52F2000F52FLL, 0xE52FEAD1E52FEAD1LL, 0xFAD12000FAD12000LL, 0x0080008000800080LL);


/*
 * Convert 3 vectors of 8 short downsampled 422 10-bit R, G, B into 1 vector of 8 short 8-bit U-V
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.037		-0.073		 0.110	]	( G )
 * V = 	[ 128 ] + [  0.110		-0.092		-0.018	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -2425		-4768		 7193	]
 *					[  7193		-6029		-1163	]
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_10bit_r_g_b_vectors_to_8bit_uv_vector_bt601_sse2, 0x1C19F6871C19F687LL, 0xE873ED60E873ED60LL, 0xFB751C19FB751C19LL, 0x0080008000800080LL);


/*
 * Convert 3 vectors of 8 short downsampled 422 10-bit R, G, B into 1 vector of 8 short 8-bit U-V
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 128 ] + [ -0.025		-0.085		 0.110	]	( G )
 * V = 	[ 128 ] + [  0.110		-0.100		-0.010	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[ -1655		-5554		7193	]
 *					[  7193		-6537		-655	]
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_10bit_r_g_b_vectors_to_8bit_uv_vector_bt709_sse2, 0x1C19F9891C19F989LL, 0xE677EA4EE677EA4ELL, 0xFD711C19FD711C19LL, 0x0080008000800080LL);





/*
 * 10-bit R, G, B vectors to 10-bit UV vectors - full range, bt{601, 709}
 */

/*
 * Convert 3 vectors of 8 short downsampled 422 10-bit R, G, B into 1 vector of 8 short 10-bit U-V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 512 ] + [ -0.169		-0.331		 0.500	]	( G )
 * V = 	[ 512 ] + [  0.500		-0.419		-0.081	]	( B )
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_10bit_r_g_b_vectors_to_10bit_uv_vector_sse2, 0x7FFFD4BC7FFFD4BCLL, 0x94BCAB4494BCAB44LL, 0xEB447FFFEB447FFFLL, 0x0200020002000200LL);


/*
 * Convert 3 vectors of 8 short downsampled 422 10-bit R, G, B into 1 vector of 8 short 10-bit U-V
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 512 ] + [ -0.148		-0.291		 0.439	]	( G )
 * V = 	[ 512 ] + [  0.439		-0.368		-0.071	]	( B )
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_10bit_r_g_b_vectors_to_10bit_uv_vector_bt601_sse2, 0x7062DA1D7062DA1DLL, 0xA1CBB581A1CBB581LL, 0xEDD37062EDD37062LL, 0x0200020002000200LL);


/*
 * Convert 3 vectors of 8 short downsampled 422 10-bit R, G, B into 1 vector of 8 short 10-bit U-V
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
 * Num of pixel handled:	8
 *
 * U = 	[ 512 ] + [ -0.101		-0.339		 0.439	]	( G )
 * V = 	[ 512 ] + [  0.439		-0.399		-0.040	]	( B )
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
DEFINE_R_G_B_TO_UV_INLINE(convert_downsampled_422_10bit_r_g_b_vectors_to_10bit_uv_vector_bt709_sse2, 0x7062E6257062E625LL, 0x99DBA93799DBA937LL, 0xF5C37062F5C37062LL, 0x0200020002000200LL);




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
 * AG, RB vectors to Y vector - full range
 */
#define DEFINE_AG_RB_TO_Y_FR(fn_name, agYCoeffVal, rbYCoeffVal)\
	EXTERN_INLINE void fn_name(__m128i* in_4_v16i_ag_rb_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(agYCoeffs, agYCoeffVal, agYCoeffVal);\
		CONST_M128I(rbYCoeffs, rbYCoeffVal, rbYCoeffVal);\
		M128I(y1Scratch, 0x0LL, 0x0LL);\
		M128I(y2Scratch, 0x0LL, 0x0LL);\
		M128I(scratch, 0x0LL, 0x0LL);\
		_M(y1Scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[0], _M(agYCoeffs));\
		_M(scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[1], _M(rbYCoeffs));\
		_M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));\
		_M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);\
		_M(y2Scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[2], _M(agYCoeffs));\
		_M(scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[3], _M(rbYCoeffs));\
		_M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));\
		_M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);\
		*out_1_v16i_y_vector = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));\
	}

/*
 * The previous macro expands to:
 *
 *	EXTERN_INLINE void convert_ag_rb_vectors_to_y_vector_sse2(__m128i* in_4_v16i_ag_rb_vectors, __m128i* out_1_v16i_y_vector) {
 *		CONST_M128I(agYCoeffs, 0x4B7D00004B7D0000LL, 0x4B7D00004B7D0000LL);
 *		CONST_M128I(rbYCoeffs, 0x0E9826460E982646LL, 0x0E9826460E982646LL);
 *
 *		M128I(y1Scratch, 0x0LL, 0x0LL);
 *		M128I(y2Scratch, 0x0LL, 0x0LL);
 *		M128I(scratch, 0x0LL, 0x0LL);
 *
 *
 *		//
 *		// Y 1-4
 *		// AG coeffs
 *		// Multiply A & G values by 15-bit left-shifted Y coeffs
 * 		_M(y1Scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[0], _M(agYCoeffs));		// PMADDWD		3	1
 *
 *		// RB coeffs
 *		// Multiply R & B values by 15-bit left-shifted Y coeffs
 *		_M(scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[1], _M(rbYCoeffs));		// PMADDWD		3	1
 *
 *		// Add both of the above and shift right by 15
 * 		_M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));						// PADDD		1	0.5
 *		_M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);								// PSRLD		1	1
 *
 *
 *		//
 *		// Y 5-8
 *		// AG coeffs
 *		// Multiply A & G values by 15-bit left-shifted Y coeffs
 *		_M(y2Scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[2], _M(agYCoeffs));		// PMADDWD		3	1
 *
 *		// RB coeffs
 *		// Multiply R & B values by 15-bit left-shifted Y coeffs
 *		_M(scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[3], _M(rbYCoeffs));		// PMADDWD		3	1
 *
 *		// Add both of the above and shift right by 15
 *		_M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));						// PADDD		1	0.5
 *		_M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);								// PSRLD		1	1
 *
 *
 *		// pack both sets of Y values (32 bit to 16 bit values)
 *		*out_1_v16i_y_vector = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));			// PACKSSDW		1	0.5
 *		// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
 *	}
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
DEFINE_AG_RB_TO_Y_FR(convert_ag_rb_vectors_to_y_vector_sse2, 0x4B7D00004B7D0000LL, 0x0E9826460E982646LL);

DEFINE_AG_RB_TO_Y_FR(convert_ga_br_vectors_to_y_vector_sse2, 0x00004B7D00004B7DLL, 0x26460E9826460E98LL);



/*
 * AG, RB vectors to Y vector - bt{601,709}
 */
#define DEFINE_AG_RB_TO_Y_INLINE(fn_name, gaYCoeffVal, brYCoeffVal, offsetVal)\
	EXTERN_INLINE void fn_name(__m128i* in_4_v16i_ga_br_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(gaYCoeffs, gaYCoeffVal, gaYCoeffVal);\
		CONST_M128I(brYCoeffs, brYCoeffVal, brYCoeffVal);\
		CONST_M128I(resultOffset, offsetVal, offsetVal);\
		M128I(y1Scratch, 0x0LL, 0x0LL);\
		M128I(y2Scratch, 0x0LL, 0x0LL);\
		M128I(scratch, 0x0LL, 0x0LL);\
		_M(y1Scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[0], _M(gaYCoeffs));\
		_M(scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[1], _M(brYCoeffs));\
		_M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));\
		_M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);\
		_M(y2Scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[2], _M(gaYCoeffs));\
		_M(scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[3], _M(brYCoeffs));\
		_M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));\
		_M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);\
		_M(y1Scratch) = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));\
		*out_1_v16i_y_vector = _mm_add_epi16(_M(y1Scratch), _M(resultOffset));\
	}

/*
 * The previous macro expands to
 *
 *	EXTERN_INLINE void convert_ga_br_vectors_to_y_vector_bt709_sse2(__m128i* in_4_v16i_ga_br_vectors, __m128i* out_1_v16i_y_vector) {
 *		 CONST_M128I(gaYCoeffs, 0x00004E9700004E97LL, 0x00004E9700004E97LL);
 *		 CONST_M128I(brYCoeffs, 0x176D07F0176D07F0LL, 0x176D07F0176D07F0LL);
 *		 CONST_M128I(add_16, 0x0010001000100010LL, 0x0010001000100010LL);
 *		 
 *		 M128I(y1Scratch, 0x0LL, 0x0LL);
 *		 M128I(y2Scratch, 0x0LL, 0x0LL);
 *		 M128I(scratch, 0x0LL, 0x0LL);
 *		 
 *		 
 *		 //
 *		 // Y 1-4
 *		 // AG coeffs
 *		 // Multiply A & G values by 16-bit left-shifted Y coeffs
 *		 _M(y1Scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[0], _M(gaYCoeffs));		// PMADDWD		3	1
 *		 
 *		 // RB coeffs
 *		 // Multiply R & B values by 16-bit left-shifted Y coeffs
 *		 _M(scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[1], _M(brYCoeffs));		// PMADDWD		3	1
 *		 
 *		 // Add both of the above and shift right by 15
 *		 _M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));					// PADDD		1	0.5
 *		 _M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);							// PSRLD		1	1
 *		 
 *		 
 *		 //
 *		 // Y 5-8
 *		 // AG coeffs
 *		 // Multiply A & G values by 16-bit left-shifted Y coeffs
 *		 _M(y2Scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[2], _M(gaYCoeffs));		// PMADDWD		3	1
 *		 
 *		 // RB coeffs
 *		 // Multiply R & B values by 16-bit left-shifted Y coeffs
 *		 _M(scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[3], _M(brYCoeffs));		// PMADDWD		3	1
 *		 
 *		 // Add both of the above and shift right by 15
 *		 _M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));					// PADDD		1	0.5
 *		 _M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);							// PSRLD		1	1
 *		 
 *		 
 *		 // pack both sets of Y values (32 bit to 16 bit values) and add 16
 *		 _M(y1Scratch) = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));					// PACKSSDW		1	0.5
 *		 *out_1_v16i_y_vector = _mm_add_epi16(_M(y1Scratch), _M(add_16));				// PADDW		1	0.5
 *		 // Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
 *	}
 */

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
DEFINE_AG_RB_TO_Y_INLINE(convert_ag_rb_vectors_to_y_vector_bt601_sse2, 0x4083000040830000LL, 0x0C8B20E50C8B20E5LL, 0x0010001000100010LL);

DEFINE_AG_RB_TO_Y_INLINE(convert_ga_br_vectors_to_y_vector_bt601_sse2, 0x0000408300004083LL, 0x20E50C8B20E50C8BLL, 0x0010001000100010LL);


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
DEFINE_AG_RB_TO_Y_INLINE(convert_ag_rb_vectors_to_y_vector_bt709_sse2, 0x4E9700004E970000LL, 0x07F0176D07F0176DLL, 0x0010001000100010LL);

DEFINE_AG_RB_TO_Y_INLINE(convert_ga_br_vectors_to_y_vector_bt709_sse2, 0x00004E9700004E97LL, 0x176D07F0176D07F0LL, 0x0010001000100010LL);





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
 * AG, RB vectors to UV vector - FR and bt{601,709}
 */
#define DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(fn_name, gaUCoeffVal, brUCoeffVal, gaVCoeffVal, brVCoeffVal, offsetVal)\
	EXTERN_INLINE void fn_name(__m128i* in_2_v16i_ga_br_vectors, __m128i* out_1_v16i_uv_vector) {\
		CONST_M128I(resultOffset, offsetVal, offsetVal);\
		CONST_M128I(gaUCoeffs, gaUCoeffVal, gaUCoeffVal);\
		CONST_M128I(brUCoeffs, brUCoeffVal, brUCoeffVal);\
		CONST_M128I(gaVCoeffs, gaVCoeffVal, gaVCoeffVal);\
		CONST_M128I(brVCoeffs, brVCoeffVal, brVCoeffVal);\
		CONST_M128I(zeroLowWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);\
		M128I(brScratch, 0x0LL, 0x0LL);\
		M128I(gaScratch, 0x0LL, 0x0LL);\
		_M(gaScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[0], _M(gaUCoeffs));\
		_M(brScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[1], _M(brUCoeffs));\
		_M(gaScratch) = _mm_add_epi32(_M(gaScratch), _M(brScratch));\
		*out_1_v16i_uv_vector = _mm_srli_epi32 (_M(gaScratch), 16);\
		_M(gaScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[0], _M(gaVCoeffs));\
		_M(brScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[1], _M(brVCoeffs));\
		_M(gaScratch) = _mm_add_epi32(_M(gaScratch), _M(brScratch));\
		_M(gaScratch) = _mm_and_si128 (_M(gaScratch), _M(zeroLowWord));\
		_M(gaScratch) = _mm_or_si128(*out_1_v16i_uv_vector, _M(gaScratch));\
		*out_1_v16i_uv_vector = _mm_add_epi16(_M(gaScratch), _M(resultOffset));\
	}

/*
 * The previous macro expands to
 * 
 * EXTERN_INLINE void convert_downsampled_422_ga_br_vectors_to_uv_vector_bt709_sse2(__m128i* in_2_v16i_ga_br_vectors, __m128i* out_1_v16i_uv_vector) {
 *		 CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
 *		 CONST_M128I(gaUCoeffs, 0x0000A9370000A937LL, 0x0000A9370000A937LL);
 *		 CONST_M128I(brUCoeffs, 0xE6257062E6257062LL, 0xE6257062E6257062LL);
 *		 CONST_M128I(gaVCoeffs, 0x000099DB000099DBLL, 0x000099DB000099DBLL);
 *		 CONST_M128I(brVCoeffs, 0x7062F5C37062F5C3LL, 0x7062F5C37062F5C3LL);
 *		 CONST_M128I(zeroLowWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);
 *		 
 *		 M128I(brScratch, 0x0LL, 0x0LL);
 *		 M128I(gaScratch, 0x0LL, 0x0LL);
 *		 
 *		 
 *		 //
 *		 // U
 *		 // Multiply A & G values by 16-bit left-shifted U coeffs
 *		 _M(gaScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[0], _M(gaUCoeffs));		// PMADDWD		3	1
 *		 // Multiply R & B values by 16-bit left-shifted U coeffs
 *		 _M(brScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[1], _M(brUCoeffs));		// PMADDWD		3	1
 *		 
 *		 // Add both of the above and shift right by 16
 *		 _M(gaScratch) = _mm_add_epi32(_M(gaScratch), _M(brScratch));					// PADDD		1	0.5
 *		 *out_1_v16i_uv_vector = _mm_srli_epi32 (_M(gaScratch), 16);					// PSRLD		1	1
 *		 // U12		0		U34		0		U56		0		U78		0
 *		 
 *		 //
 *		 // V
 *		 // Multiply A & G values by 16-bit left-shifted V coeffs
 *		 _M(gaScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[0], _M(gaVCoeffs));		// PMADDWD		3	1
 *		 // Multiply R & B values by 16-bit left-shifted V coeffs
 *		 _M(brScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[1], _M(brVCoeffs));		// PMADDWD		3	1
 *		 
 *		 // Add both of the above and zero low word(to account for 16-bit shift of
 *		 // coefficients and place the bits in the right place for the following OR)
 *		 _M(gaScratch) = _mm_add_epi32(_M(gaScratch), _M(brScratch));					// PADDD		1	0.5
 *		 _M(gaScratch) = _mm_and_si128 (_M(gaScratch), _M(zeroLowWord));				// PAND			1	0.33
 *		 // 0		V12		0		V34		0		V56		0		V78
 *		 
 *		 // combine U and V
 *		 _M(gaScratch) = _mm_or_si128(*out_1_v16i_uv_vector, _M(gaScratch));			// POR			1	0.33
 *		 // U12		V12		U34		V34		U56		V56		U78		V78
 *		 
 *		 // U,V + 128
 *		 *out_1_v16i_uv_vector = _mm_add_epi16(_M(gaScratch), _M(add128));				//PADDW			1	0.5
 *		 // U12 		V12		U34 	V34		U56 	V56		U78 	V78
 * }
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

DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2,
										0xAB440000AB440000LL, 0x7FFFD4BC7FFFD4BCLL,
										0x94BC000094BC0000LL, 0xEB447FFFEB447FFFLL,
										0x0080008000800080LL);

DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ga_br_vectors_to_uv_vector_sse2,
										0x0000AB440000AB44LL, 0xD4BC7FFFD4BC7FFFLL,
										0x000094BC000094BCLL, 0x7FFFEB447FFFEB44LL,
										0x0080008000800080LL);



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
DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt601_sse2,
									  0xB5810000B5810000LL, 0x7062DA1D7062DA1DLL, 
									  0xA1CB0000A1CB0000LL, 0xEDD37062EDD37062LL,
									0x0080008000800080LL);

DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ga_br_vectors_to_uv_vector_bt601_sse2,
									  0x0000B5810000B581LL, 0xDA1D7062DA1D7062LL,
									  0x0000A1CB0000A1CBLL, 0x7062EDD37062EDD3LL,
									0x0080008000800080LL);


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
DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709_sse2,
										0xA9370000A9370000LL, 0x7062E6257062E625LL,
										0x99DB000099DB0000LL, 0xF5C37062F5C37062LL,
										0x0080008000800080LL);

DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ga_br_vectors_to_uv_vector_bt709_sse2,
										  0x0000A9370000A937LL, 0xE6257062E6257062LL,
										  0x000099DB000099DBLL, 0x7062F5C37062F5C3LL,
										0x0080008000800080LL);
#endif 	// __INTEL_CPU__

#endif /* RGB_TO_YUV_CONVERT_H_ */

