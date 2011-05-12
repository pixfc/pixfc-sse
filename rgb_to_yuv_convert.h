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
 * Convert 3 vectors of 8 short R, G, B into 3 vectors of 8 short Y, U & V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are created from chromas value for pixel 1 only.
 *
 * Total latency: 			76 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  0.299		 0.587		 0.114	]	( R )
 * U = 	[ 128 ] + [ -0.169		-0.331		 0.500	]	( G )
 * V = 	[ 128 ] + [  0.500		-0.419		-0.081	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
 * 					[  19595	 38470		 7471	]
 * 					[ -11076	-21692		 32767	]
 * 					[  32767	-27460		-5308	]
 *
 *	Note: the R-V & B-U coeffs (32767) should really be 32768 but because we need them to be
 *			a 16-bit signed integer, they have been capped to the maximum value for this
 *			type. 32767 maps to 0.499985 instead of 0.500 which is an acceptable
 *			approximation.
 *  Note2: the Y calculation involves only positive values and coefficients, and thus uses
 *         using unsigned math. Therefore, the G-Y coefficient can exceed 32767 (up to 65535).
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gb1Vect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * gb1Vect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
EXTERN_INLINE void convert_n_nnb_downsample_r_g_b_vectors_to_y_uv_vectors_sse2(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
	CONST_M128I(rYCoeffs, 0x4C8B4C8B4C8B4C8BLL, 0x4C8B4C8B4C8B4C8BLL);
	CONST_M128I(gYCoeffs, 0x9646964696469646LL, 0x9646964696469646LL);
	CONST_M128I(bYCoeffs, 0x1D2F1D2F1D2F1D2FLL, 0x1D2F1D2F1D2F1D2FLL);
	CONST_M128I(rUVCoeffsInterleaved, 0x7FFFD4BC7FFFD4BCLL, 0x7FFFD4BC7FFFD4BCLL);
	CONST_M128I(gUVCoeffsInterleaved, 0x94BCAB4494BCAB44LL, 0x94BCAB4494BCAB44LL);
	CONST_M128I(bUVCoeffsInterleaved, 0xEB447FFFEB447FFFLL, 0xEB447FFFEB447FFFLL);
	
	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);
	M128I(rOdd, 0x0LL, 0x0LL);
	M128I(gOdd, 0x0LL, 0x0LL);
	M128I(bOdd, 0x0LL, 0x0LL);

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

	out_2_v16i_y_uv_vectors[0] = _mm_add_epi16(_M(rScratch), _M(gScratch));			// PADDW		2	2
	out_2_v16i_y_uv_vectors[0] = _mm_add_epi16(out_2_v16i_y_uv_vectors[0], _M(bScratch));// PADDW	2	2
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0

	
	//
	// Since we are going doing nearest neighbour downsampling, lets only
	// do calculation for chroma values U and V we are going to keep, which are
	// the ones from odd pixels (1, 3, 5 and 7). So we first gather those pixels
	// together and  then do the conversion.
	_M(rOdd) = _mm_shufflehi_epi16(in_3_v16i_r_g_b_vectors[0], 0xA0);				// PSHUFHW		2	2
	// R1 0 0 0		R3 0 0 0	R5 0 R5 0	R7 0 R7 0
	
	_M(rOdd) = _mm_shufflelo_epi16(_M(rOdd), 0xA0);									// PSHUFLW		2	2
	// R1 0 R1 0	R3 0 R3 0	R5 0 R5 0	R7 0 R7 0
							  
	_M(gOdd) = _mm_shufflehi_epi16(in_3_v16i_r_g_b_vectors[1], 0xA0);				// PSHUFHW		2	2
	// G1 0 0 0		G3 0 0 0	G5 0 G5	0	G7 0 G7	0
	
	_M(gOdd) = _mm_shufflelo_epi16(_M(gOdd), 0xA0);									// PSHUFLW		2	2
	// G1 0 G1 0	G3 0 G3 0	G5 0 G5	0	G7 0 G7	0	

	_M(bOdd) = _mm_shufflehi_epi16(in_3_v16i_r_g_b_vectors[2], 0xA0);				// PSHUFHW		2	2
	// B1 0 0 0		B3 0 0 0	B5 0 B5	0	B7 0 B7	0
	
	_M(bOdd) = _mm_shufflelo_epi16(_M(bOdd), 0xA0);									// PSHUFLW		2	2
	// B1 0 B1 0	B3 0 B3 0	B5 0 B5	0	B7 0 B7	0	
	
	//
	// r UV 
	_M(rScratch) = _mm_mulhi_epi16(_M(rOdd), _M(rUVCoeffsInterleaved));				// PMULHW		9 8 2 2

	// g UV 
	_M(gScratch) = _mm_mulhi_epi16(_M(gOdd), _M(gUVCoeffsInterleaved));				// PMULHW		9 8 2 2
	
	// b UV 
	_M(bScratch) = _mm_mulhi_epi16(_M(bOdd), _M(bUVCoeffsInterleaved));				// PMULHW		9 8 2 2
	
	// r UV + g UV + b UV
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(_M(rScratch), _M(gScratch));			// PADDW		2	2
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(out_2_v16i_y_uv_vectors[1], _M(bScratch));// PADDW	2	2
	
	// U,V + 128
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(out_2_v16i_y_uv_vectors[1], _M(add128));// PADDW		2	2
	// U12 V12			U34 V34			U56 V56			U78 V78
};


/*
 * Convert 3 vectors of 8 short R, G, B into 3 vectors of 8 short Y, U & V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are created from chromas value for pixel 1 only.
 *
 * Total latency: 			67 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  0.299		 0.587		 0.114	]	( R )
 * U = 	[ 128 ] + [ -0.169		-0.331		 0.500	]	( G )
 * V = 	[ 128 ] + [  0.500		-0.419		-0.081	]	( B )
 *
 *				All coeffs are left-shifted by 16 bits
* 					[  19595	 38470		 7471	]
 * 					[ -11076	-21692		 32767	]
 * 					[  32767	-27460		-5308	]
 *
 *	Note: the R-V & B-U coeffs (32767) should really be 32768 but because we need them to be
 *			a 16-bit signed integer, they have been capped to the maximum value for this
 *			type. 32767 maps to 0.499985 instead of 0.500 which is an acceptable
 *			approximation.
 *  Note2: the Y calculation involves only positive values and coefficients, and thus uses
 *         using unsigned math. Therefore, the G-Y coefficient can exceed 32767 (up to 65535).
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gb1Vect
 * G1 0		G2 0	G3 0	G4 0	G5 0	G6 0	G7 0	G8 0
 *
 * gb1Vect
 * B1 0		B2 0	B3 0	B4 0	B5 0	B6 0	B7 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
EXTERN_INLINE void convert_n_nnb_downsample_r_g_b_vectors_to_y_uv_vectors_sse2_ssse3(__m128i* in_3_v16i_r_g_b_vectors, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
	CONST_M128I(rYCoeffs, 0x4C8B4C8B4C8B4C8BLL, 0x4C8B4C8B4C8B4C8BLL);
	CONST_M128I(gYCoeffs, 0x9646964696469646LL, 0x9646964696469646LL);
	CONST_M128I(bYCoeffs, 0x1D2F1D2F1D2F1D2FLL, 0x1D2F1D2F1D2F1D2FLL);
	CONST_M128I(rUVCoeffsInterleaved, 0x7FFFD4BC7FFFD4BCLL, 0x7FFFD4BC7FFFD4BCLL);
	CONST_M128I(gUVCoeffsInterleaved, 0x94BCAB4494BCAB44LL, 0x94BCAB4494BCAB44LL);
	CONST_M128I(bUVCoeffsInterleaved, 0xEB447FFFEB447FFFLL, 0xEB447FFFEB447FFFLL);
	CONST_M128I(shuf_odd, 0xFF04FF04FF00FF00LL, 0xFF0CFF0CFF08FF08LL);

	
	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gScratch, 0x0LL, 0x0LL);
	M128I(bScratch, 0x0LL, 0x0LL);
	M128I(rOdd, 0x0LL, 0x0LL);
	M128I(gOdd, 0x0LL, 0x0LL);
	M128I(bOdd, 0x0LL, 0x0LL);
	
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

	out_2_v16i_y_uv_vectors[0] = _mm_add_epi16(_M(rScratch), _M(gScratch));			// PADDW		2	2
	out_2_v16i_y_uv_vectors[0] = _mm_add_epi16(out_2_v16i_y_uv_vectors[0], _M(bScratch));// PADDW	2	2
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0
	
	
	//
	// Since we are going doing nearest neighbour downsampling, lets only
	// do calculation for chroma values U and V we are going to keep, which are
	// the ones from odd pixels (1, 3, 5 and 7). So we first gather those pixels
	// together and  then do the conversion.
	_M(rOdd) = _mm_shuffle_epi8(in_3_v16i_r_g_b_vectors[0], _M(shuf_odd));		// PSHUFB		1 1 3 0 1 2
	// R1 0 R1 0	R3 0 R3	0	R5 0 R5	0	R7 0 R7 0

	_M(gOdd) = _mm_shuffle_epi8(in_3_v16i_r_g_b_vectors[1], _M(shuf_odd));		// PSHUFB		1 1 3 0 1 2
	// G1 0 G1 0	G3 0 G3 0	G5 0 G5	0	G7 0 G7	0

	_M(bOdd) = _mm_shuffle_epi8(in_3_v16i_r_g_b_vectors[2], _M(shuf_odd));		// PSHUFB		1 1 3 0 1 2
	// B1 0 B1 0	B3 0 B3 0	B5 0 B5	0	B7 0 B7	0	
	
	//
	// r UV 
	_M(rScratch) = _mm_mulhi_epi16(_M(rOdd), _M(rUVCoeffsInterleaved));				// PMULHW		9 8 2 2

	// g UV 
	_M(gScratch) = _mm_mulhi_epi16(_M(gOdd), _M(gUVCoeffsInterleaved));				// PMULHW		9 8 2 2

	// b UV 
	_M(bScratch) = _mm_mulhi_epi16(_M(bOdd), _M(bUVCoeffsInterleaved));				// PMULHW		9 8 2 2

	// r UV + g UV + b UV
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(_M(rScratch), _M(gScratch));			// PADDW		2	2
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(out_2_v16i_y_uv_vectors[1], _M(bScratch));// PADDW	2	2
	
	// U,V + 128
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(out_2_v16i_y_uv_vectors[1], _M(add128));// PADDW		2	2
	// U12 V12			U34 V34			U56 V56			U78 V78
};

#endif /* RGB_TO_YUV_CONVERT_H_ */

