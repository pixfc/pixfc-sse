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
 * Convert 3 vectors of 8 short R, G, B into 1 vector of 8 short Y
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are created from chromas value for pixel 1 only.
 *
 * Total latency: 			31 cycles
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
	_M(rScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[0], _M(rYCoeffs));		// PMULHUW		9 8 2 2
	
	// G coeffs
	// Multiply G values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(gScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[1], _M(gYCoeffs));		// PMULHUW		9 8 2 2
	
	// B coeffs
	// Multiply B values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(bScratch) = _mm_mulhi_epu16(in_3_v16i_r_g_b_vectors[2], _M(bYCoeffs));		// PMULHUW		9 8 2 2
	
	*out_1_v16i_y_vector = _mm_add_epi16(_M(rScratch), _M(gScratch));				// PADDW		2	2
	*out_1_v16i_y_vector = _mm_add_epi16(*out_1_v16i_y_vector, _M(bScratch));		// PADDW		2	2
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
}

/*
 * Convert 3 vectors of 8 short downsampled 422 R, G, B into 1 vector of 8 short U-V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Total latency: 			33 cycles
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
	_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[0], _M(rUVCoeffsInterleaved));// PMULHW	9 8 2 2
	
	// g UV 
	_M(gScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[1], _M(gUVCoeffsInterleaved));// PMULHW	9 8 2 2
	
	// b UV 
	_M(bScratch) = _mm_mulhi_epi16(in_3_v16i_r_g_b_vectors[2], _M(bUVCoeffsInterleaved));// PMULHW	9 8 2 2
	
	// r UV + g UV + b UV
	*out_1_v16i_uv_vector = _mm_add_epi16(_M(rScratch), _M(gScratch));					//	PADDW	2	2
	*out_1_v16i_uv_vector = _mm_add_epi16(*out_1_v16i_uv_vector, _M(bScratch));			//	PADDW	2	2
	
	// U,V + 128
	*out_1_v16i_uv_vector = _mm_add_epi16(*out_1_v16i_uv_vector, _M(add128));			//	PADDW	2	2
	// U12 V12			U34 V34			U56 V56			U78 V78
};

#endif /* RGB_TO_YUV_CONVERT_H_ */

