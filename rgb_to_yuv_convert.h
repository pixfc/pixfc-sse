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


#include <emmintrin.h>
#include <tmmintrin.h>

#include "platform_util.h"

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
EXTERN_INLINE void convert_ag_rb_vectors_to_y_vector_sse2(__m128i* in_4_v16i_ag_rb_vectors, __m128i* out_1_v16i_y_vector) {
	CONST_M128I(agYCoeffs, 0x4B7D00004B7D0000LL, 0x4B7D00004B7D0000LL);
	CONST_M128I(rbYCoeffs, 0x0E9826460E982646LL, 0x0E9826460E982646LL);
	
	M128I(y1Scratch, 0x0LL, 0x0LL);
	M128I(y2Scratch, 0x0LL, 0x0LL);
	M128I(scratch, 0x0LL, 0x0LL);

	
	//
	// Y 1-4
	// AG coeffs
	// Multiply A & G values by 15-bit left-shifted Y coeffs
	_M(y1Scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[0], _M(agYCoeffs));		// PMADDWD		3	1
	
	// RB coeffs
	// Multiply R & B values by 15-bit left-shifted Y coeffs
	_M(scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[1], _M(rbYCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and shift right by 15
	_M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));						// PADDD		1	0.5
	_M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);								// PSRLD		1	1
	
	
	//
	// Y 5-8
	// AG coeffs
	// Multiply A & G values by 15-bit left-shifted Y coeffs
	_M(y2Scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[2], _M(agYCoeffs));		// PMADDWD		3	1
	
	// RB coeffs
	// Multiply R & B values by 15-bit left-shifted Y coeffs
	_M(scratch) = _mm_madd_epi16(in_4_v16i_ag_rb_vectors[3], _M(rbYCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and shift right by 15
	_M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));						// PADDD		1	0.5
	_M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);								// PSRLD		1	1
	
	
	// pack both sets of Y values (32 bit to 16 bit values)
	*out_1_v16i_y_vector = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));			// PACKSSDW		1	0.5
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
}


/*
 * Convert 4 vectors of 8 short GA, BR into 1 vector of 8 short Y
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
EXTERN_INLINE void convert_ga_br_vectors_to_y_vector_sse2(__m128i* in_4_v16i_ga_br_vectors, __m128i* out_1_v16i_y_vector) {
	CONST_M128I(gaYCoeffs, 0x00004B7D00004B7DLL, 0x00004B7D00004B7DLL);
	CONST_M128I(brYCoeffs, 0x26460E9826460E98LL, 0x26460E9826460E98LL);
	
	M128I(y1Scratch, 0x0LL, 0x0LL);
	M128I(y2Scratch, 0x0LL, 0x0LL);
	M128I(scratch, 0x0LL, 0x0LL);
	
	
	//
	// Y 1-4
	// GA coeffs
	// Multiply A & G values by 15-bit left-shifted Y coeffs
	_M(y1Scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[0], _M(gaYCoeffs));		// PMADDWD		3	1
	
	// BR coeffs
	// Multiply R & B values by 15-bit left-shifted Y coeffs
	_M(scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[1], _M(brYCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and shift right by 15
	_M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));						// PADDD		1	0.5
	_M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);								// PSRLD		1	1
	
	
	//
	// Y 5-8
	// GA coeffs
	// Multiply A & G values by 15-bit left-shifted Y coeffs
	_M(y2Scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[2], _M(gaYCoeffs));		// PMADDWD		3	1
	
	// BR coeffs
	// Multiply R & B values by 15-bit left-shifted Y coeffs
	_M(scratch) = _mm_madd_epi16(in_4_v16i_ga_br_vectors[3], _M(brYCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and shift right by 15
	_M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));						// PADDD		1	0.5
	_M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);								// PSRLD		1	1
	
	
	// pack both sets of Y values (32 bit to 16 bit values)
	*out_1_v16i_y_vector = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));			// PACKSSDW		1	0.5
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
}


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
EXTERN_INLINE void convert_r_g_b_vectors_to_y_vector_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {
	CONST_M128I(rYCoeffs, 0x4C8B4C8B4C8B4C8BLL, 0x4C8B4C8B4C8B4C8BLL);
	CONST_M128I(gYCoeffs, 0x9646964696469646LL, 0x9646964696469646LL);
	CONST_M128I(bYCoeffs, 0x1D2F1D2F1D2F1D2FLL, 0x1D2F1D2F1D2F1D2FLL);

	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);
	
	//
	// Y
	// R coeffs
	// Multiply R values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));		// PMULHUW		3	1
	
	// G coeffs
	// Multiply G values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));		// PMULHUW		3	1
	
	// B coeffs
	// Multiply B values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));		// PMULHUW		3	1
	
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));						// PADDW		1	0.5
	*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(bScratch));				// PADDW		1	0.5
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
}

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

EXTERN_INLINE void convert_downsampled_422_r_g_b_vectors_to_uv_vector_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_uv_vector) {
	CONST_M128I(rUVCoeffsInterleaved, 0x7FFFD4BC7FFFD4BCLL, 0x7FFFD4BC7FFFD4BCLL);
	CONST_M128I(gUVCoeffsInterleaved, 0x94BCAB4494BCAB44LL, 0x94BCAB4494BCAB44LL);
	CONST_M128I(bUVCoeffsInterleaved, 0xEB447FFFEB447FFFLL, 0xEB447FFFEB447FFFLL);
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
	
	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);
	
	//
	// r UV 
	_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[0], _M(rUVCoeffsInterleaved));// PMULHW	3	1
	
	// g UV 
	_M(gScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[1], _M(gUVCoeffsInterleaved));// PMULHW	3	1
	
	// b UV 
	_M(bScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[2], _M(bUVCoeffsInterleaved));// PMULHW	3	1
	
	// r UV + g UV + b UV
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));							//	PADDW	1	0.5
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));							//	PADDW	1	0.5
	
	// U,V + 128
	*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(add128));					//	PADDW	1	0.5
	// U12 V12			U34 V34			U56 V56			U78 V78
};


/*
 * Convert 4 vectors of 8 short downsampled 422 AG, RB into 1 vector of 8 short U-V
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

EXTERN_INLINE void convert_downsampled_422_ag_rb_vectors_to_uv_vector_sse2(__m128i* in_2_v16i_ag_rb_vectors, __m128i* out_1_v16i_uv_vector) {
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
	CONST_M128I(agUCoeffs, 0xAB440000AB440000LL, 0xAB440000AB440000LL);
	CONST_M128I(rbUCoeffs, 0x7FFFD4BC7FFFD4BCLL, 0x7FFFD4BC7FFFD4BCLL);
	CONST_M128I(agVCoeffs, 0x94BC000094BC0000LL, 0x94BC000094BC0000L);
	CONST_M128I(rbVCoeffs, 0xEB447FFFEB447FFFLL, 0xEB447FFFEB447FFFLL);
	CONST_M128I(zeroLowWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);

	M128I(rbScratch, 0x0LL, 0x0LL);
	M128I(agScratch, 0x0LL, 0x0LL);

	
	//
	// U
	// Multiply A & G values by 16-bit left-shifted U coeffs
	_M(agScratch) = _mm_madd_epi16(in_2_v16i_ag_rb_vectors[0], _M(agUCoeffs));		// PMADDWD		3	1
	// Multiply R & B values by 16-bit left-shifted U coeffs
	_M(rbScratch) = _mm_madd_epi16(in_2_v16i_ag_rb_vectors[1], _M(rbUCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and shift right by 16
	_M(agScratch) = _mm_add_epi32(_M(agScratch), _M(rbScratch));					// PADDD		1	0.5
	*out_1_v16i_uv_vector = _mm_srli_epi32 (_M(agScratch), 16);						// PSRLD		1	1
	// U12		0		U34		0		U56		0		U78		0

	//
	// V
	// Multiply A & G values by 16-bit left-shifted V coeffs
	_M(agScratch) = _mm_madd_epi16(in_2_v16i_ag_rb_vectors[0], _M(agVCoeffs));		// PMADDWD		3	1
	// Multiply R & B values by 16-bit left-shifted V coeffs
	_M(rbScratch) = _mm_madd_epi16(in_2_v16i_ag_rb_vectors[1], _M(rbVCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and zero low word(to account for 16-bit shift of
	// coefficients and place the bits in the right place for the following OR)
	_M(agScratch) = _mm_add_epi32(_M(agScratch), _M(rbScratch));					// PADDD		1	0.5
	_M(agScratch) = _mm_and_si128 (_M(agScratch), _M(zeroLowWord));					// PAND			1	0.33
	// 0		V12		0		V34		0		V56		0		V78

	// combine U and V
	_M(agScratch) = _mm_or_si128(*out_1_v16i_uv_vector, _M(agScratch));				// POR			1	0.33
	// U12		V12		U34		V34		U56		V56		U78		V78
	
	// U,V + 128
	*out_1_v16i_uv_vector = _mm_add_epi16(_M(agScratch), _M(add128));				//PADDW			1	0.5
	// U12 		V12		U34 	V34		U56 	V56		U78 	V78
};



/*
 * Convert 4 vectors of 8 short downsampled 422 AG, RB into 1 vector of 8 short U-V
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
 * gaVect
 * G12 0	A12 0	G34 0	A34 0	G56 0	A56 0	G78 0	A78 0
 *
 * rbVect
 * B12 0	R12 0	B34 0	R34 0	B56 0	R56 0	B78 0	R78 0
 *
 * OUTPUT:
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 *
 */

EXTERN_INLINE void convert_downsampled_422_ga_br_vectors_to_uv_vector_sse2(__m128i* in_2_v16i_ga_br_vectors, __m128i* out_1_v16i_uv_vector) {
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
	CONST_M128I(gaUCoeffs, 0x0000AB440000AB44LL, 0x0000AB440000AB44LL);
	CONST_M128I(brUCoeffs, 0xD4BC7FFFD4BC7FFFLL, 0xD4BC7FFFD4BC7FFFLL);
	CONST_M128I(gaVCoeffs, 0x000094BC000094BCLL, 0x000094BC000094BCL);
	CONST_M128I(brVCoeffs, 0x7FFFEB447FFFEB44LL, 0x7FFFEB447FFFEB44LL);
	CONST_M128I(zeroLowWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);
	
	M128I(brScratch, 0x0LL, 0x0LL);
	M128I(gaScratch, 0x0LL, 0x0LL);
	
	
	//
	// U
	// Multiply A & G values by 16-bit left-shifted U coeffs
	_M(gaScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[0], _M(gaUCoeffs));		// PMADDWD		3	1
	// Multiply R & B values by 16-bit left-shifted U coeffs
	_M(brScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[1], _M(brUCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and shift right by 16
	_M(gaScratch) = _mm_add_epi32(_M(gaScratch), _M(brScratch));					// PADDD		1	0.5
	*out_1_v16i_uv_vector = _mm_srli_epi32 (_M(gaScratch), 16);						// PSRLD		1	1
	// U12		0		U34		0		U56		0		U78		0
	
	//
	// V
	// Multiply A & G values by 16-bit left-shifted V coeffs
	_M(gaScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[0], _M(gaVCoeffs));		// PMADDWD		3	1
	// Multiply R & B values by 16-bit left-shifted V coeffs
	_M(brScratch) = _mm_madd_epi16(in_2_v16i_ga_br_vectors[1], _M(brVCoeffs));		// PMADDWD		3	1
	
	// Add both of the above and zero low word(to account for 16-bit shift of
	// coefficients and place the bits in the right place for the following OR)
	_M(gaScratch) = _mm_add_epi32(_M(gaScratch), _M(brScratch));					// PADDD		1	0.5
	_M(gaScratch) = _mm_and_si128 (_M(gaScratch), _M(zeroLowWord));					// PAND			1	0.33
	// 0		V12		0		V34		0		V56		0		V78
	
	// combine U and V
	_M(gaScratch) = _mm_or_si128(*out_1_v16i_uv_vector, _M(gaScratch));				// POR			1	0.33
	// U12		V12		U34		V34		U56		V56		U78		V78
	
	// U,V + 128
	*out_1_v16i_uv_vector = _mm_add_epi16(_M(gaScratch), _M(add128));				//PADDW			1	0.5
	// U12 		V12		U34 	V34		U56 	V56		U78 	V78
};



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
EXTERN_INLINE void convert_r_g_b_vectors_to_y_vector_bt601_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {
	CONST_M128I(rYCoeffs, 0x41CB41CB41CB41CBLL, 0x41CB41CB41CB41CBLL);
	CONST_M128I(gYCoeffs, 0x8106810681068106LL, 0x8106810681068106LL);
	CONST_M128I(bYCoeffs, 0x1917191719171917LL, 0x1917191719171917LL);
	CONST_M128I(add_16, 0x0010001000100010LL, 0x0010001000100010LL);

	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);

	//
	// Y
	// R coeffs
	// Multiply R values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));		// PMULHUW		3	1

	// G coeffs
	// Multiply G values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));		// PMULHUW		3	1

	// B coeffs
	// Multiply B values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));		// PMULHUW		3	1

	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));						// PADDW		1	0.5
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));						// PADDW		1	0.5
	*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(add_16));					// PADDW		1	0.5
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
}

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

EXTERN_INLINE void convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt601_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_uv_vector) {
	CONST_M128I(rUVCoeffsInterleaved, 0x7062DA1D7062DA1DLL, 0x7062DA1D7062DA1DLL);
	CONST_M128I(gUVCoeffsInterleaved, 0xA1CBB581A1CBB581LL, 0xA1CBB581A1CBB581LL);
	CONST_M128I(bUVCoeffsInterleaved, 0xEDD37062EDD37062LL, 0xEDD37062EDD37062LL);
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);

	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);

	//
	// r UV
	_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[0], _M(rUVCoeffsInterleaved));// PMULHW	3	1

	// g UV
	_M(gScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[1], _M(gUVCoeffsInterleaved));// PMULHW	3	1

	// b UV
	_M(bScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[2], _M(bUVCoeffsInterleaved));// PMULHW	3	1

	// r UV + g UV + b UV
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));							//	PADDW	1	0.5
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));							//	PADDW	1	0.5

	// U,V + 128
	*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(add128));					//	PADDW	1	0.5
	// U12 V12			U34 V34			U56 V56			U78 V78
};



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
EXTERN_INLINE void convert_r_g_b_vectors_to_y_vector_bt709_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_y_vector) {
	CONST_M128I(rYCoeffs, 0x2ED92ED92ED92ED9LL, 0x2ED92ED92ED92ED9LL);
	CONST_M128I(gYCoeffs, 0x9D2F9D2F9D2F9D2FLL, 0x9D2F9D2F9D2F9D2FLL);
	CONST_M128I(bYCoeffs, 0x0FDF0FDF0FDF0FDFLL, 0x0FDF0FDF0FDF0FDFLL);
	CONST_M128I(add_16, 0x0010001000100010LL, 0x0010001000100010LL);

	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);

	//
	// Y
	// R coeffs
	// Multiply R values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));		// PMULHUW		3	1

	// G coeffs
	// Multiply G values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));		// PMULHUW		3	1

	// B coeffs
	// Multiply B values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));		// PMULHUW		3	1

	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));						// PADDW		1	0.5
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));						// PADDW		1	0.5
	*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(add_16));					// PADDW		1	0.5
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
}

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

EXTERN_INLINE void convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_1_v16i_uv_vector) {
	CONST_M128I(rUVCoeffsInterleaved, 0x7062E6257062E625LL, 0x7062E6257062E625LL);
	CONST_M128I(gUVCoeffsInterleaved, 0x99DBA93799DBA937LL, 0x99DBA93799DBA937LL);
	CONST_M128I(bUVCoeffsInterleaved, 0xF5C37062F5C37062LL, 0xF5C37062F5C37062LL);
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);

	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);

	//
	// r UV
	_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[0], _M(rUVCoeffsInterleaved));// PMULHW	3	1

	// g UV
	_M(gScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[1], _M(gUVCoeffsInterleaved));// PMULHW	3	1

	// b UV
	_M(bScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[2], _M(bUVCoeffsInterleaved));// PMULHW	3	1

	// r UV + g UV + b UV
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(gScratch));							//	PADDW	1	0.5
	_M(rScratch) = _mm_add_epi16(_M(rScratch), _M(bScratch));							//	PADDW	1	0.5

	// U,V + 128
	*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(add128));					//	PADDW	1	0.5
	// U12 V12			U34 V34			U56 V56			U78 V78
};




#define DEFINE_AG_RB_TO_Y_INLINE(fn_name, gaYCoeffVal, brYCoeffVal)\
	EXTERN_INLINE void fn_name(__m128i* in_4_v16i_ga_br_vectors, __m128i* out_1_v16i_y_vector) {\
		CONST_M128I(gaYCoeffs, gaYCoeffVal, gaYCoeffVal);\
		CONST_M128I(brYCoeffs, brYCoeffVal, brYCoeffVal);\
		CONST_M128I(add_16, 0x0010001000100010LL, 0x0010001000100010LL);\
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
		*out_1_v16i_y_vector = _mm_add_epi16(_M(y1Scratch), _M(add_16));\
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
 *		 _M(y1Scratch) = _mm_add_epi32 (_M(y1Scratch), _M(scratch));						// PADDD		1	0.5
 *		 _M(y1Scratch) = _mm_srli_epi32 (_M(y1Scratch), 15);								// PSRLD		1	1
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
 *		 _M(y2Scratch) = _mm_add_epi32 (_M(y2Scratch), _M(scratch));						// PADDD		1	0.5
 *		 _M(y2Scratch) = _mm_srli_epi32 (_M(y2Scratch), 15);								// PSRLD		1	1
 *		 
 *		 
 *		 // pack both sets of Y values (32 bit to 16 bit values) and add 16
 *		 _M(y1Scratch) = _mm_packs_epi32(_M(y1Scratch), _M(y2Scratch));					// PACKSSDW		1	0.5
 *		 *out_1_v16i_y_vector = _mm_add_epi16(_M(y1Scratch), _M(add_16));				//	PADDW		1	0.5
 *		 // Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
 *	}
 */

/*
 * Convert 4 vectors of 8 short AG, RB (or GA, BR) into 1 vector of 8 short Y
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
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
DEFINE_AG_RB_TO_Y_INLINE(convert_ag_rb_vectors_to_y_vector_bt601_sse2, 0x4083000040830000LL, 0x0C8B20E50C8B20E5LL);

DEFINE_AG_RB_TO_Y_INLINE(convert_ga_br_vectors_to_y_vector_bt601_sse2, 0x0000408300004083LL, 0x20E50C8B20E50C8BLL);


/*
 * Convert 4 vectors of 8 short AG, RB (or GA , BR) into 1 vector of 8 short Y
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			12 cycles
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
DEFINE_AG_RB_TO_Y_INLINE(convert_ag_rb_vectors_to_y_vector_bt709_sse2, 0x4E9700004E970000LL, 0x07F0176D07F0176DLL);

DEFINE_AG_RB_TO_Y_INLINE(convert_ga_br_vectors_to_y_vector_bt709_sse2, 0x176D07F0176D07F0LL, 0x176D07F0176D07F0LL);






#define DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(fn_name, gaUCoeffVal, brUCoeffVal, gaVCoeffVal, brVCoeffVal)\
	EXTERN_INLINE void fn_name(__m128i* in_2_v16i_ga_br_vectors, __m128i* out_1_v16i_uv_vector) {\
		CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);\
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
		*out_1_v16i_uv_vector = _mm_add_epi16(_M(gaScratch), _M(add128));\
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
 * Convert 2 vectors of 8 short downsampled 422 AG, RB (or GA, BR) into 1 vector of 8 short U-V
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
									  0xA1CB0000A1CB0000LL, 0xEDD37062EDD37062LL);

DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ga_br_vectors_to_uv_vector_bt601_sse2,
									  0x0000B5810000B581LL, 0xDA1D7062DA1D7062LL,
									  0x0000A1CB0000A1CBLL, 0x7062EDD37062EDD3LL);


/*
 * Convert 2 vectors of 8 short downsampled 422 AG, RB ( or GA, BR) into 1 vector of 8 short U-V
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
										0x99DB000099DB0000LL, 0xF5C37062F5C37062LL);

DEFINE_DOWNSAMPLED_AG_RB_TO_UV_INLINE(convert_downsampled_422_ga_br_vectors_to_uv_vector_bt709_sse2,
										  0x0000A9370000A937LL, 0xE6257062E6257062LL,
										  0x000099DB000099DBLL, 0x7062F5C37062F5C3LL);

#endif /* RGB_TO_YUV_CONVERT_H_ */

