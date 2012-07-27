/*
 * yuv_to_rgb_convert.h
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

#ifndef YUV_TO_RGB_CONVERT_H_
#define YUV_TO_RGB_CONVERT_H_


#include "debug_support.h"
#include "platform_util.h"

#ifdef	__INTEL_CPU__

#include <emmintrin.h>
#include <tmmintrin.h>


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
EXTERN_INLINE void nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_sse2(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)
{
	M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
	M128I(uvRCoeffs, 0x0166000001660000LL, 0x0166000001660000LL);
	M128I(uvGCoeffs, 0xFF4AFFA8FF4AFFA8LL, 0xFF4AFFA8FF4AFFA8LL);
	M128I(uvBCoeffs, 0x000001C4000001C4LL, 0x000001C4000001C4LL);
	
	// U - 128	V - 128
	_M(sub128) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));// PADDW		1	0.5
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvRCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvRCoeffs));		// PMADDWD		3	1
	// U12*0 + V12*358	U34*0 + V34*358	U56*0 + V56*358	U78*0 + V78*358
	// C12				C34				C56				C78		(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					358*127 = 45466 (16 bit wide)
	//					min value:
	//					358*-128= 2's(45824) (45824 fits in 16 bits)
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 8);								// PSRAD		1	1
	// C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
	
	_M(uvRCoeffs) = _mm_shufflehi_epi16(_M(uvRCoeffs), 0xA0);						// PSHUFHW		1	0.5
	// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
	
	_M(uvRCoeffs) = _mm_shufflelo_epi16(_M(uvRCoeffs), 0xA0);						// PSHUFLW		1	0.5
	// C12 0 C12 0		C34 0 C34 0		C56 0 C56 0		C78 0 C78 0
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvRCoeffs)); // PADDW	1	0.5
	//
	
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvGCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvGCoeffs));		// PMADDWD		3	1
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	// C12					C34					C56					C78
	
	// shift right by 8
	_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 8);								// PSRAD		1	1
	// C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
	
	_M(uvGCoeffs) = _mm_shufflehi_epi16(_M(uvGCoeffs), 0xA0);						// PSHUFHW		1	0.5
	// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
	
	_M(uvGCoeffs) = _mm_shufflelo_epi16(_M(uvGCoeffs), 0xA0);						// PSHUFLW		1	0.5
	// C12 0 C12 0		C34 0 C34 0		C56 0 C56 0		C78 0 C78 0
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvGCoeffs));// PADDW	1	0.5
	//
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvBCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvBCoeffs));		// PMADDWD		3	1
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	// C12					C34					C56					C78
	
	// shift right by 8
	_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 8);								// PSRAD		1	1
	// C12	0		C34	 0		C56	 0		C78	 0
	
	_M(uvBCoeffs) = _mm_shufflehi_epi16(_M(uvBCoeffs), 0xA0);						// PSHUFHW		1	0.5
	// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
	
	_M(uvBCoeffs) = _mm_shufflelo_epi16(_M(uvBCoeffs), 0xA0);						// PSHUFLW		1	0.5
	// C12 0 C12 0		C34 0 C12 0		C56 0 C56 0		C78 0 C78 0
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0],  _M(uvBCoeffs));// PADDW	1	0.5
	//
	
};


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
EXTERN_INLINE void convert_y_uv_vectors_to_rgb_vectors_sse2(__m128i* in_3_v16i_y_uvOdd_uyEven_vectors, __m128i* out_3_v16i_rgb_vectors)
{
	CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
	CONST_M128I(uvRCoeffs, 0x0166000001660000LL, 0x0166000001660000LL);
	CONST_M128I(uvGCoeffs, 0xFF4AFFA8FF4AFFA8LL, 0xFF4AFFA8FF4AFFA8LL);
	CONST_M128I(uvBCoeffs, 0x000001C4000001C4LL, 0x000001C4000001C4LL);
	CONST_M128I(zeroHiWord, 0x0000FFFF0000FFFFLL, 0x0000FFFF0000FFFFLL);
	CONST_M128I(zeroLoWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);
	M128I(uvOdd, 0x0LL, 0x0LL);
	M128I(uvEven, 0x0LL, 0x0LL);
	
	// U - 128	V - 128
	in_3_v16i_y_uvOdd_uyEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(sub128));
	// 																					PADDW		1	0.5
	in_3_v16i_y_uvOdd_uyEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(sub128));
	// 																					PADDW		1	0.5
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvRCoeffs));
	// U1*0 + V1*358	U3*0 + V3*358	U5*0 + V5*358	U7*0 + V7*358				// PMADDWD		3	1
	// C1		C3		C5		C7			(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					452*127 = 57404 (16 bit wide)
	//					min value:
	//					452*-128= 2's(57856) (57856 fits in 16 bit wide)
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 8), _M(zeroHiWord));		// PSRAD		1	1
	// C1 0			C3  0		C5 0		C7 0									// PAND			1	0.33
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvRCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		3	1
	
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 8),  _M(zeroLoWord));		// PSLLD		1	1
	// 0 C2			0 C4		0 C6		0 C8									// PAND			1	0.33
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		1	0.5
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvGCoeffs));	// PMADDWD		3	1
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	// C12		C34		C56		C78
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 8),  _M(zeroHiWord));		// PSRAD		1	0.5
	// C1 0			C3  0		C5 0		C7 0									// PAND			1	0.33
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvGCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		3	1
	
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 8),  _M(zeroLoWord));		// PSLLD		1	1
	// 0 C2			0 C4		0 C6		0 C8									// PAND			1	0.33
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		1	0.5
	
	
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvBCoeffs));	// PMADDWD		3	1
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	// 0 C12		0 C34		0 C56		0 C78
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 8),  _M(zeroHiWord));		// PSRAD		1	1
	// C1 0			C3  0		C5 0		C7 0									// PAND			1	0.33
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvBCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		3	1
	
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 8),  _M(zeroLoWord));		// PSLLD		1	1
	// 0 C2			0 C4		0 C6		0 C8									// PAND			1	0.33
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		1	0.5
};


/*
 * Convert 2 vectors of 8 short Y, UV into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 * Total latency: 			16 cycles
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
EXTERN_INLINE void nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)
{
	CONST_M128I(shuffMask, 0x0605060502010201LL, 0x0E0D0E0D0A090A09LL);
	M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
	M128I(uvRCoeffs, 0x0166000001660000LL, 0x0166000001660000LL);
	M128I(uvGCoeffs, 0xFF4AFFA8FF4AFFA8LL, 0xFF4AFFA8FF4AFFA8LL);
	M128I(uvBCoeffs, 0x000001C4000001C4LL, 0x000001C4000001C4LL);
	
	// U - 128	V - 128
	_M(sub128) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));// PADDW		1	0.5
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvRCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvRCoeffs));		// PMADDWD		3	1
	// U12*0 + V12*358	U34*0 + V34*358	U56*0 + V56*358	U78*0 + V78*358
	// C12		C34		C56		C78			(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					452*127 = 57404 (16 bit wide)
	//					min value:
	//					452*-128= 2's(57856) (57856 fits in 16 bit wide)
	
	// Shuffle bytes:
	// shift right by 8 to account for left shift by 8 of coefficients,
	// keep next signed byte and duplicate in the hi word
	_M(uvRCoeffs) = _mm_shuffle_epi8 (_M(uvRCoeffs), _M(shuffMask));				// PSHUFB		1	0.5
	// C12 C12		C34 C34 	C56 C56		C78 C78
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvRCoeffs)); // PADDW	1	0.5
	//
	
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvGCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvGCoeffs));		// PMADDWD		3	1
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	// C12		C34		C56		C78
	
	_M(uvGCoeffs) = _mm_shuffle_epi8 (_M(uvGCoeffs), _M(shuffMask));				// PSHUFB		1	0.5
	// C12 C12		C34 C34 	C56 C56		C78 C78
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvGCoeffs));// PADDW	1	0.5
	//
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvBCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvBCoeffs));		// PMADDWD		3	1
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	// 0 C12		0 C34		0 C56		0 C78
	
	_M(uvBCoeffs) = _mm_shuffle_epi8 (_M(uvBCoeffs), _M(shuffMask));				// PSHUFB		1	0.5
	// C12 C12		C34 C34 	C56 C56		C78 C78
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvBCoeffs));// PADDW	1	0.5
	//
};

/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			32 cycles
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
EXTERN_INLINE void convert_y_uv_vectors_to_rgb_vectors_sse2_ssse3(__m128i* in_3_v16i_y_uvOdd_uyEven_vectors, __m128i* out_3_v16i_rgb_vectors)
{
	CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
	CONST_M128I(uvRCoeffs, 0x0166000001660000LL, 0x0166000001660000LL);
	CONST_M128I(uvGCoeffs, 0xFF4AFFA8FF4AFFA8LL, 0xFF4AFFA8FF4AFFA8LL);
	CONST_M128I(uvBCoeffs, 0x000001C4000001C4LL, 0x000001C4000001C4LL);
	CONST_M128I(shuff1,	0xFFFF0605FFFF0201LL, 0xFFFF0E0DFFFF0A09LL);
	CONST_M128I(shuff2,	0x0605FFFF0201FFFFLL, 0x0E0DFFFF0A09FFFFLL);
	M128I(uvOdd, 0x0LL, 0x0LL);
	M128I(uvEven, 0x0LL, 0x0LL);
	
	// U - 128	V - 128
	in_3_v16i_y_uvOdd_uyEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(sub128));
	// 																					PADDW		1	0.5
	in_3_v16i_y_uvOdd_uyEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(sub128));
	// 																					PADDW		1	0.5
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvRCoeffs));
	// U1*0 + V1*358	U3*0 + V3*358	U5*0 + V5*358	U7*0 + V7*358				// PMADDWD		3	1
	// C1		C3		C5		C7			(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					452*127 = 57404 (16 bit wide)
	//					min value:
	//					452*-128= 2's(57856) (57856 fits in 16 bit wide)
	
	// shuffle bytes: shift right by 8 to account for left shift by 8 of coefficients, keep next signed byte and zero rest of dword
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1	0.5
	// C1 0			C3  0		C5 0		C7 0
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvRCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		3	1
	
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1	0.5
	// 0 C2			0 C4		0 C6		0 C8
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		1	0.5
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvGCoeffs));	// PMADDWD		3	1
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	
	// shuffle bytes: shift right by 8 to account for left shift by 8 of coefficients, keep next signed byte and zero rest of dword
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1	0.5
	// C1 0			C3  0		C5 0		C7 0
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvGCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		3	1
	
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1	0.5
	// 0 C2			0 C4		0 C6		0 C8
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		1	0.5
	
	
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvBCoeffs));	// PMADDWD		3	1
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	
	// shuffle bytes: shift right by 8 to account for left shift by 8 of coefficients, keep next signed byte and zero rest of dword
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1	0.5
	// C1 0			C3  0		C5 0		C7 0
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvBCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		3	1
	
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1	0.5
	// 0 C2			0 C4		0 C6		0 C8
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		1	0.5
};






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
 * NNB SSE2 conversion
 *
 */
#define	DEFINE_NNB_Y_UV_TO_RGB_SSE2_INLINE(fn_name, yOffset, uvOffset, yCoef, uvRCoef, uvGCoef, uvBCoef) \
EXTERN_INLINE void fn_name(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(yCoeffs, yCoef, yCoef);\
	M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	M128I(uvVect, uvOffset, uvOffset);\
	M128I(yVect, yOffset, yOffset);\
	_M(uvVect) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(uvVect));\
	_M(yVect) = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(yVect));\
    print_xmm16("Y - off", &yVect);\
    print_xmm16("UV - off", &uvVect);\
	_M(yVect) = _mm_slli_epi16(_M(yVect), 2);\
	_M(yVect) = _mm_mulhi_epi16(_M(yVect), _M(yCoeffs));\
	/* _M(yVect) = _mm_srai_epi16(_M(yVect), 7); */\
    print_xmm16("(Y - off) * 1.164", &yVect);\
	_M(uvRCoeffs) = _mm_madd_epi16(_M(uvVect), _M(uvRCoeffs));\
	_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 7);\
    print_xmm32("(U - off)*0 + (V - off) * 1.596", &uvRCoeffs);\
	_M(uvRCoeffs) = _mm_shufflehi_epi16(_M(uvRCoeffs), 0xA0);\
	_M(uvRCoeffs) = _mm_shufflelo_epi16(_M(uvRCoeffs), 0xA0);\
    print_xmm16("(U - off)*0 + (V - off) * 1.596", &uvRCoeffs);\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVect), _M(uvRCoeffs));\
	_M(uvGCoeffs) = _mm_madd_epi16(_M(uvVect), _M(uvGCoeffs));\
	_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 7);\
	_M(uvGCoeffs) = _mm_shufflehi_epi16(_M(uvGCoeffs), 0xA0);\
	_M(uvGCoeffs) = _mm_shufflelo_epi16(_M(uvGCoeffs), 0xA0);\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVect), _M(uvGCoeffs));\
	_M(uvBCoeffs) = _mm_madd_epi16(_M(uvVect), _M(uvBCoeffs));\
	_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 7);\
    print_xmm32("(U-off)*2.017 + (V-off)*0", &uvBCoeffs);\
	_M(uvBCoeffs) = _mm_shufflehi_epi16(_M(uvBCoeffs), 0xA0);\
	_M(uvBCoeffs) = _mm_shufflelo_epi16(_M(uvBCoeffs), 0xA0);\
    print_xmm16("(U-off)*2.017 + (V-off)*0", &uvBCoeffs);\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVect),  _M(uvBCoeffs));\
}
/*
 * The previous macro expands to:
 *
 *
 *	EXTERN_INLINE void nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)
 *	{
 *		CONST_M128I(yCoeffs, 0x94FE94FE94FE94FELL, 0x94FE94FE94FE94FELL);
 *		M128I(uvRCoeffs, 0x00CC000000CC0000LL, 0x00CC000000CC0000LL);
 *		M128I(uvGCoeffs, 0xFF98FFCEFF98FFCELL, 0xFF98FFCEFF98FFCELL);
 *		M128I(uvBCoeffs, 0x0000018200000182LL, 0x0000018200000182LL);
 *		M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
 *		M128I(sub16, 0xFFF0FFF0FFF0FFF0LL, 0xFFF0FFF0FFF0FFF0LL);
 *		
 *		// U - 128	V - 128
 *		_M(sub128) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));		// PADDW		1	0.5
 *	
 *		// Y - 16
 *		_M(sub16) = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(sub16));		// PADDW		1	0.5
 *		
 *		//
 *		// Y values
 *		//
 *		// left shift Y values by 8 bits
 *		// Ymin << 8 = 0 		Ymax << 8 = 235 << 8 = 60160
 *		_M(sub16) = _mm_slli_epi16(_M(sub16), 8);								// PSLLW		1	1
 *		
 *		// Multiply 8-bit left-shifted Y values by 15-bit left-shifted Y coeffs 
 *		// and keep highest 16 bits
 *		_M(sub16) = _mm_mulhi_epu16(_M(sub16), _M(yCoeffs));					// PMULHUW		3	1
 *		
 *		//
 *		// We have multiplied 8-bit left-shifted Y values by 15-bit left shifted Y coeffs
 *		// Overall, we have left-shifted the result by 8 + 15 = 23 bits, which we must undo.
 *		// PMULHUW has inherently done a 16-bit right shift, so we still have to right
 *		// shift by 7 bits.
 *		_M(sub16) = _mm_srli_epi16(_M(sub16), 7);								// PSRLD		1	1
 *		// Y1 	Y2		Y3		Y4		Y5		Y6		Y7		Y8
 *		
 *		//
 *		// R
 *		// U and V coefficients
 *		// 0, 204, 0, 204, 0, 204, 0, 204
 *		_M(uvRCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvRCoeffs));				// PMADDWD		3	1
 *		// U12*0 + V12*204	U34*0 + V34*204	U56*0 + V56*204	U78*0 + V78*204
 *		// C12				C34				C56				C78		(4 * 32-bits values)
 *		// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
 *		//					max value with 7bit left shift of coeffs:
 *		//					386*127 = 49022 (16 bit wide)
 *		//					min value:
 *		//					386*-128= 2's(49408) (49408 fits in 16 bit)
 *	
 *		// shift right by 7 to account for left shift by 7 of coefficients
 *		_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 7);						// PSRAD		1	1
 *		// C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
 *		
 *		_M(uvRCoeffs) = _mm_shufflehi_epi16(_M(uvRCoeffs), 0xA0);				// PSHUFHW		1	0.5
 *		// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
 *		
 *		_M(uvRCoeffs) = _mm_shufflelo_epi16(_M(uvRCoeffs), 0xA0);				// PSHUFLW		1	0.5
 *		// C12 0 C12 0		C34 0 C34 0		C56 0 C56 0		C78 0 C78 0
 *		
 *		out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(sub16), _M(uvRCoeffs));	// PADDW		1	0.5
 *		//
 *		
 *		
 *		//
 *		// G
 *		// U and V coeffs ()
 *		// -50, -104, -50, -104, -50, -104, -50, -104
 *		_M(uvGCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvGCoeffs));				// PMADDWD		3	1
 *		// U12*-50 + V12*-104	U34*-50 + V34*-104	U56*-50 + V56*-104	U78*-50 + V78*-104
 *		// C12		C34		C56		C78
 *		
 *		// shift right by 7
 *		_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 7);						// PSRAD		1	1
 *		// C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
 *		
 *		_M(uvGCoeffs) = _mm_shufflehi_epi16(_M(uvGCoeffs), 0xA0);				// PSHUFHW		1	0.5
 *		// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
 *		
 *		_M(uvGCoeffs) = _mm_shufflelo_epi16(_M(uvGCoeffs), 0xA0);				// PSHUFLW		1	0.5
 *		// C12 0 C12 0		C34 0 C34 0		C56 0 C56 0		C78 0 C78 0
 *		
 *		out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(sub16), _M(uvGCoeffs));	// PADDW		1	0.5
 *		//
 *		
 *		//
 *		// B
 *		// U and V coeffs ()
 *		// 0, 386, 0, 386, 0, 386, 0, 386
 *		_M(uvBCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvBCoeffs));				// PMADDWD		3	1
 *		// U12*0 + V12*386	U34*0 + V34*386	U56*0 + V56*386	U78*0 + V78*386
 *		// 0 C12		0 C34		0 C56		0 C78
 *		
 *		// shift right by 7
 *		_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 7);						// PSRAD		1	1
 *		// C12	0		C34	 0		C56	 0		C78	 0
 *		
 *		_M(uvBCoeffs) = _mm_shufflehi_epi16(_M(uvBCoeffs), 0xA0);				// PSHUFHW		1	0.5
 *		// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
 *		
 *		_M(uvBCoeffs) = _mm_shufflelo_epi16(_M(uvBCoeffs), 0xA0);				// PSHUFLW		1	0.5
 *		// C12 0 C12 0		C34 0 C12 0		C56 0 C56 0		C78 0 C78 0
 *		
 *		out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(sub16),  _M(uvBCoeffs));	// PADDW		1	0.5
 *		//
 *	};
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
 * B = 	[ 1.164		2.017	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 14 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		204		]
 * 		[ 19071		-50		-104	]
 * 		[ 19071		258		0		]
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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_INLINE(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2,
									0xFFF0FFF0FFF0FFF0LL,
									0xFF80FF80FF80FF80LL,
                                    0x4A7F4A7F4A7F4A7FLL,
									0x00CC000000CC0000LL,
									0xFF98FFCEFF98FFCELL,
									0x0000010200000102LL);



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
 * Y coeffs left shifted by 14 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		229		]
 * 		[ 19071		-27		-68		]
 * 		[ 19071		270		0		]
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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_INLINE(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_sse2,
										0xFFF0FFF0FFF0FFF0LL,
										0xFF80FF80FF80FF80LL,
										0x4A7F4A7F4A7F4A7FLL,
										0x00E5000000E50000LL,
										0xFFBCFFE5FFBCFFE5LL,
										0x0000010E0000010ELL);




/*
 *
 *	Upsampled YUV to RGB SSE2 conversion
 *
 */
#define	DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_INLINE(fn_name, yCoef, uvRCoef, uvGCoef, uvBCoef) \
EXTERN_INLINE void fn_name(__m128i* in_3_v16i_y_uvOdd_uvEven_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);\
	CONST_M128I(yCoeffs, yCoef, yCoef);\
	CONST_M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	CONST_M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	CONST_M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	CONST_M128I(zeroHiWord, 0x0000FFFF0000FFFFLL, 0x0000FFFF0000FFFFLL);\
	CONST_M128I(zeroLoWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);\
	M128I(sub16, 0xFFF0FFF0FFF0FFF0LL, 0xFFF0FFF0FFF0FFF0LL);\
	M128I(uvOdd, 0x0LL, 0x0LL);\
	M128I(uvEven, 0x0LL, 0x0LL);\
	_M(sub16) = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[0], _M(sub16));\
    print_xmm16("Y-off", &sub16);\
	in_3_v16i_y_uvOdd_uvEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(sub128));\
	in_3_v16i_y_uvOdd_uvEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(sub128));\
    print_xmm16("YUodd - off", &in_3_v16i_y_uvOdd_uvEven_vectors[1]);\
    print_xmm16("YUeven - off", &in_3_v16i_y_uvOdd_uvEven_vectors[2]);\
	_M(sub16) = _mm_slli_epi16(_M(sub16), 2);\
    print_xmm16("y-off * 256", &sub16);\
	_M(sub16) = _mm_mulhi_epi16(_M(sub16), _M(yCoeffs));\
    print_xmm16("(Y-off)*1.164", &sub16);\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvRCoeffs));\
    print_xmm32("(Uodd - off) * 0 + (Uodd - off)*1.4", &uvOdd);\
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 7), _M(zeroHiWord));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvRCoeffs));\
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 9), _M(zeroLoWord));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(sub16), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvGCoeffs));\
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 7), _M(zeroHiWord));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvGCoeffs));\
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 9), _M(zeroLoWord));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(sub16), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvBCoeffs));\
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 7), _M(zeroHiWord));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvBCoeffs));\
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 9), _M(zeroLoWord));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(sub16), _M(uvEven));\
}
/*
 * The previous macro expands to
 *
 *	EXTERN_INLINE void convert_y_uv_vectors_to_rgb_vectors_bt709_sse2(__m128i* in_3_v16i_y_uvOdd_uvEven_vectors, __m128i* out_3_v16i_rgb_vectors)
 *	{
 *		CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
 *		CONST_M128I(yCoeffs, 0x94FE94FE94FE94FELL, 0x94FE94FE94FE94FELL);
 *		 CONST_M128I(uvRCoeffs, 0x00E5000000E50000LL, 0x00E5000000E50000LL);
 *		 CONST_M128I(uvGCoeffs, 0xFFBCFFE5FFBCFFE5LL, 0xFFBCFFE5FFBCFFE5LL);
 *		 CONST_M128I(uvBCoeffs, 0x0000010E0000010ELL, 0x0000010E0000010ELL);
 *		 CONST_M128I(zeroHiWord, 0x0000FFFF0000FFFFLL, 0x0000FFFF0000FFFFLL);
 *		 CONST_M128I(zeroLoWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);
 *		 M128I(sub16, 0xFFF0FFF0FFF0FFF0LL, 0xFFF0FFF0FFF0FFF0LL);
 *		 M128I(uvOdd, 0x0LL, 0x0LL);
 *		 M128I(uvEven, 0x0LL, 0x0LL);
 *		 
 *		 // Y - 16
 *		 _M(sub16) = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[0], _M(sub16));		// PADDW		1	0.5
 *		 
 *		 // U - 128	V - 128
 *		 in_3_v16i_y_uvOdd_uvEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(sub128));	// PADDW		1	0.5
 *		 in_3_v16i_y_uvOdd_uvEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(sub128));	// PADDW		1	0.5
 *		 
 *		 //
 *		 // Y values
 *		 //
 *		 // left shift Y values by 8 bits
 *		 // Ymin << 8 = 0 		Ymax << 8 = 235 << 8 = 60160
 *		 _M(sub16) = _mm_slli_epi16(_M(sub16), 8);										// PSLLW		1	1
 *		 
 *		 // Multiply 8-bit left-shifted Y values by 15-bit left-shifted Y coeffs 
 *		 // and keep highest 16 bits
 *		 _M(sub16) = _mm_mulhi_epu16(_M(sub16), _M(yCoeffs));							// PMULHUW		3	1
 *		 
 *		 // R
 *		 // U and V coefficients
 *		 // 0, 204, 0, 204, 0, 204, 0, 204
 *		 _M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvRCoeffs));// PMADDWD		3	1
 *		 // U1*0 + V1*204	U3*0 + V3*204	U5*0 + V5*204	U7*0 + V7*204
 *		 // C1			C3				C5				C7		(4 * 32-bits values)
 *		 // A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
 *		 //					max value with 7bit left shift of coeffs:
 *		 //					386*127 = 49022 (16 bit wide)
 *		 //					min value:
 *		 //					386*-128= 2's(49408) (49408 fits in 16 bit)
 *		 
 *		 // shift right by 7 to account for left shift by 7 of coefficients
 *		 _M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 7), _M(zeroHiWord));		// PSRAD		1	1
 *		 // C1 0 0 0		C3 0 0 0		C5 0 0 0		C7 0 0 0					// PAND			1	0.33
 *		 
 *		 _M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvRCoeffs));// PMADDWD		3	1
 *		 // U2*0 + V2*204	U4*0 + V4*204	U6*0 + V6*204	U8*0 + V8*204
 *		 // C2			C4				C6				C8
 *		 
 *		 // Move these values to the hi word and right shift by 7 to account for 7-bit left shift
 *		 // of coefficients: 16 - 7 = 9 - we need to left shift by 9 bits.
 *		 _M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 9), _M(zeroLoWord));		// PSLLD		1	1
 *		 // 0 C2			0 C4		0 C6		0 C8								// PAND			1	0.33
 *		 
 *		 _M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
 *		 // C1 C2		C3 C4		C5 C6		C7 C8
 *		 
 *		 out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(sub16), _M(uvEven));				// PADDW		1	0.5
 *		 
 *		 //
 *		 // G
 *		 // U and V coeffs
 *		 // -50, -104, -50, -104, -50, -104, -50, -104,
 *		 _M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvGCoeffs));// PMADDWD		3	1
 *		 // C1			C3				C5				C7		(4 * 32-bits values)
 *		 
 *		 _M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 7), _M(zeroHiWord));		// PSRAD		1	1
 *		 // C1 0 0 0		C3 0 0 0		C5 0 0 0		C7 0 0 0					// PAND			1	0.33
 *		 
 *		 _M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvGCoeffs));// PMADDWD		3	1
 *		 // U2*0 + V2*204	U4*0 + V4*204	U6*0 + V6*204	U8*0 + V8*204
 *		 // C2			C4				C6				C8
 *		 
 *		 _M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 9), _M(zeroLoWord));		// PSLLD		1	1
 *		 // 0 C2			0 C4		0 C6		0 C8								// PAND			1	0.33
 *		 
 *		 _M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
 *		 // C1 C2		C3 C4		C5 C6		C7 C8
 *		 
 *		 out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(sub16), _M(uvEven));				// PADDW		1	0.5
 *		  
 *		 //
 *		 // B
 *		 // U and V coeffs
 *		 // 386, 0, 386, 0, 386, 0, 386, 0,
 *		 _M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvBCoeffs));// PMADDWD		3	1
 *		 // C1			C3				C5				C7		(4 * 32-bits values)
 *		 
 *		 _M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 7), _M(zeroHiWord));		// PSRAD		1	1
 *		 // C1 0 0 0		C3 0 0 0		C5 0 0 0		C7 0 0 0					// PAND			1	0.33
 *		 
 *		 _M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvBCoeffs));// PMADDWD		3	1
 *		 // U2*0 + V2*204	U4*0 + V4*204	U6*0 + V6*204	U8*0 + V8*204
 *		 // C2			C4				C6				C8
 *		 
 *		 _M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 9), _M(zeroLoWord));		// PSLLD		1	1
 *		 // 0 C2			0 C4		0 C6		0 C8								// PAND			1	0.33
 *		 
 *		 _M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
 *		 // C1 C2		C3 C4		C5 C6		C7 C8
 *		 
 *		 out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(sub16), _M(uvEven));				// PADDW		1	0.5
 *	};
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
 * R = 	[ 1.164		0		1.596	]	( Y - 16)
 * G = 	[ 1.164		-0.392	-0.813	]	( U - 128 )
 * B = 	[ 1.164		2.017	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 14 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		204		]	
 * 		[ 19071		-50		-104	]
 * 		[ 19071		258		0		]
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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_INLINE(convert_y_uv_vectors_to_rgb_vectors_bt601_sse2,
                                         0x4A7F4A7F4A7F4A7FLL,
										 0x00CC000000CC0000LL,
										 0xFF98FFCEFF98FFCELL,
										 0x0000010200000102LL);
	

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
 * Y coeffs left shifted by 8 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		229		]
 * 		[ 19071		-27		-68		]
 * 		[ 19071		270		0		]
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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_INLINE(convert_y_uv_vectors_to_rgb_vectors_bt709_sse2,
                                         0x4A7F4A7F4A7F4A7FLL,
										 0x00E5000000E50000LL,
										 0xFFBCFFE5FFBCFFE5LL,
										 0x0000010E0000010ELL);
	


/*
 * NNB SSE2 / SSSE3 conversion
 */

#define	DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1(fn_name, yOffset, uvOffset, yCoef, yCoefLeftShift, uvRCoef, uvRCoefLeftShift, uvGCoef, uvGCoefLeftShift, uvBCoef, uvBCoefLeftShift) \
EXTERN_INLINE void fn_name(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(shuffMask, 0x0504050401000100LL, 0x0D0C0D0C09080908LL);\
	CONST_M128I(yCoeffs, yCoef, yCoef);\
	M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	M128I(uvVector, uvOffset, uvOffset);\
	M128I(yVector, yOffset, yOffset);\
	_M(uvVector) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(uvVector));\
	_M(yVector) = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(yVector));\
	_M(yVector) = _mm_slli_epi16(_M(yVector), (16-yCoefLeftShift));\
	_M(yVector) = _mm_mulhi_epi16(_M(yVector), _M(yCoeffs));\
	_M(uvRCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvRCoeffs));\
	_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), uvRCoefLeftShift);\
	_M(uvRCoeffs) = _mm_shuffle_epi8 (_M(uvRCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVector), _M(uvRCoeffs));\
	_M(uvGCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvGCoeffs));\
	_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), uvGCoefLeftShift);\
	_M(uvGCoeffs) = _mm_shuffle_epi8 (_M(uvGCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVector), _M(uvGCoeffs));\
	_M(uvBCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvBCoeffs));\
	_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), uvBCoefLeftShift);\
	_M(uvBCoeffs) = _mm_shuffle_epi8 (_M(uvBCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVector),  _M(uvBCoeffs));\
}
// variation from the previous routine where the y coefficient MUST be 16-bit left shifted
#define	DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE2(fn_name, yOffset, uvOffset, yCoef, uvRCoef, uvRCoefLeftShift, uvGCoef, uvGCoefLeftShift, uvBCoef, uvBCoefLeftShift) \
EXTERN_INLINE void fn_name(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(shuffMask, 0x0504050401000100LL, 0x0D0C0D0C09080908LL);\
	CONST_M128I(yCoeffs, yCoef, yCoef);\
	M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	M128I(uvVector, uvOffset, uvOffset);\
	M128I(yVector, yOffset, yOffset);\
	_M(uvVector) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(uvVector));\
	_M(yVector) = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(yVector));\
	_M(yVector) = _mm_mulhi_epi16(_M(yVector), _M(yCoeffs));\
	_M(uvRCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvRCoeffs));\
	_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), uvRCoefLeftShift);\
	_M(uvRCoeffs) = _mm_shuffle_epi8 (_M(uvRCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVector), _M(uvRCoeffs));\
	_M(uvGCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvGCoeffs));\
	_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), uvGCoefLeftShift);\
	_M(uvGCoeffs) = _mm_shuffle_epi8 (_M(uvGCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVector), _M(uvGCoeffs));\
	_M(uvBCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvBCoeffs));\
	_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), uvBCoefLeftShift);\
	_M(uvBCoeffs) = _mm_shuffle_epi8 (_M(uvBCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVector),  _M(uvBCoeffs));\
}
/*
 *  variation from the first routine where:
 *  - there is no y offset
 *  - there is no y coefficient but a right shift is made on the Y vectors,
 *  - the uvR, uvG & uvB coefficients are all 8-bit left shifted
 */
#define	DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE3(fn_name, uvOffset, yRightShift, uvRCoef, uvGCoef, uvBCoef) \
EXTERN_INLINE void fn_name(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(shuffMask, 0x0605060502010201LL, 0x0E0D0E0D0A090A09LL);\
	M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	M128I(uvVector, uvOffset, uvOffset);\
	M128I(yVector, 0x0LL, 0x0LL);\
	_M(uvVector) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(uvVector));\
	_M(yVector) = _mm_srli_epi16(in_2_v16i_y_uv_vectors[0], yRightShift);\
	_M(uvRCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvRCoeffs));\
	_M(uvRCoeffs) = _mm_shuffle_epi8 (_M(uvRCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVector), _M(uvRCoeffs));\
	_M(uvGCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvGCoeffs));\
	_M(uvGCoeffs) = _mm_shuffle_epi8 (_M(uvGCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVector), _M(uvGCoeffs));\
	_M(uvBCoeffs) = _mm_madd_epi16(_M(uvVector), _M(uvBCoeffs));\
	_M(uvBCoeffs) = _mm_shuffle_epi8 (_M(uvBCoeffs), _M(shuffMask));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVector),  _M(uvBCoeffs));\
}
/*
 * DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1 expands to:
 *
 *	EXTERN_INLINE void nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors)
 *	{
 *	 CONST_M128I(shuffMask, 0x0504050401000100LL, 0x0D0C0D0C09080908LL);
 *	 CONST_M128I(yCoeffs, 0x94FE94FE94FE94FELL, 0x94FE94FE94FE94FELL);
 *	 M128I(uvRCoeffs, 0x00CC000000CC0000LL, 0x00CC000000CC0000LL);
 *	 M128I(uvGCoeffs, 0xFF98FFCEFF98FFCELL, 0xFF98FFCEFF98FFCELL);
 *	 M128I(uvBCoeffs, 0x0000018200000182LL, 0x0000018200000182LL);
 *	 M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
 *	 M128I(sub16, 0xFFF0FFF0FFF0FFF0LL, 0xFFF0FFF0FFF0FFF0LL);
 *	 
 *	 // U - 128	V - 128
 *	 _M(sub128) = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));		// PADDW		1	0.5
 *	 
 *	 // Y - 16
 *	 _M(sub16) = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(sub16));		// PADDW		1	0.5
 *	 
 *	 //
 *	 // Y values
 *	 //
 *	 // left shift Y values by (16 - number of left-shifted bits in y coefficient) bits
 *	 // The next instruction (_mm_mulhi_epu16) will perform a 16-bit right shift. As the Y
 *   // coefficients are already left-shifted, we only need to shift the y vector by
 *   // 16 - (number of left-shifted bits in y coefficients)
 *	 _M(sub16) = _mm_slli_epi16(_M(sub16), (16 - 15));						// PSLLW		1	1
 *	 
 *	 // Multiply 8-bit left-shifted Y values by 15-bit left-shifted Y coeffs
 *	 // and keep highest 16 bits
 *	 _M(sub16) = _mm_mulhi_epu16(_M(sub16), _M(yCoeffs));					// PMULHUW		3	1
 *	 
 *	 //
 *	 // R
 *	 // U and V coefficients
 *	 // 0, 204, 0, 204, 0, 204, 0, 204
 *	 _M(uvRCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvRCoeffs));				// PMADDWD		3	1
 *	 // U12*0 + V12*204	U34*0 + V34*204	U56*0 + V56*204	U78*0 + V78*204
 *	 // C12				C34				C56				C78		(4 * 32-bits values)
 *	 // A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
 *	 //					max value with 7bit left shift of coeffs:
 *	 //					386*127 = 49022 (16 bit wide)
 *	 //					min value:
 *	 //					386*-128= 2's(49408) (49408 fits in 16 bit)
 *	 
 *	 // shift right by 7 to account for left shift by 7 of coefficients
 *	 _M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 7);						// PSRAD		1	1
 *	 // C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
 *	 
 *	 _M(uvRCoeffs) = _mm_shuffle_epi8 (_M(uvRCoeffs), _M(shuffMask));		// PSHUFB		1	0.5
 *	 // C12 C12		C34 C34 	C56 C56		C78 C78
 *	 
 *	 out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(sub16), _M(uvRCoeffs)); 	// PADDW		1	0.5
 *	 //
 *	 
 *	 
 *	 //
 *	 // G
 *	 // U and V coeffs ()
 *	 // -50, -104, -50, -104, -50, -104, -50, -104
 *	 _M(uvGCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvGCoeffs));				// PMADDWD		3	1
 *	 // U12*-50 + V12*-104	U34*-50 + V34*-104	U56*-50 + V56*-104	U78*-50 + V78*-104
 *	 // C12		C34		C56		C78
 *	 
 *	 // shift right by 7
 *	 _M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 7);						// PSRAD		1	1
 *	 // C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
 *	 
 *	 _M(uvGCoeffs) = _mm_shuffle_epi8 (_M(uvGCoeffs), _M(shuffMask));		// PSHUFB		1	0.5
 *	 // C12 C12		C34 C34 	C56 C56		C78 C78
 *	 
 *	 out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(sub16), _M(uvGCoeffs));	// PADDW		1	0.5
 *	 //
 *	 
 *	 //
 *	 // B
 *	 // U and V coeffs ()
 *	 // 0, 386, 0, 386, 0, 386, 0, 386
 *	 _M(uvBCoeffs) = _mm_madd_epi16(_M(sub128), _M(uvBCoeffs));				// PMADDWD		3	1
 *	 // U12*0 + V12*386	U34*0 + V34*386	U56*0 + V56*386	U78*0 + V78*386
 *	 // 0 C12		0 C34		0 C56		0 C78
 *	 
 *	 // shift right by 7
 *	 _M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 7);						// PSRAD		1	1
 *	 // C12	0		C34	 0		C56	 0		C78	 0
 *	 
 *	 _M(uvBCoeffs) = _mm_shuffle_epi8 (_M(uvBCoeffs), _M(shuffMask));		// PSHUFB		1	0.5
 *	 // C12 C12		C34 C34 	C56 C56		C78 C78
 *	 
 *	 out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(sub16),  _M(uvBCoeffs));	// PADDW		1	0.5
 *	 //
 * };
 */ 






/*
 * Convert 2 vectors of 8 short Y, UV into 3 vectors of 8 short R, G & B
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are used as chroma values for both pixel 1 and 2
 *
 * Total latency: 			25 cycles
 * Num of pixel handled:	8
 *
 * R = 	[ 1.164		0		1.596	]	( Y - 16)
 * G = 	[ 1.164		-0.392	-0.813	]	( U - 128 )
 * B = 	[ 1.164		2.017	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 14 bits
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		204		]
 * 		[ 19071		-50		-104	]
 * 		[ 19071	    258		0		]
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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3,
												0xFFF0FFF0FFF0FFF0LL,
												0xFF80FF80FF80FF80LL,
												0x4A7F4A7F4A7F4A7FLL, 14,
												0x00CC000000CC0000LL, 7,
												0xFF98FFCEFF98FFCELL, 7,
												0x0000010200000102LL, 7);


/*
 * Convert 2 vectors of 8 short Y, UV into 3 vectors of 8 short R, G & B
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
 * R = 	[ 1.164		0		1.793	]	( Y - 16)
 * G = 	[ 1.164		-0.213	-0.533	]	( U - 128 )
 * B = 	[ 1.164		2.112	0		]	( V - 128 )
 *
 * Y coeffs left shifted by 14s
 * U & V coeffs left shifted by 7 bits
 * 		[ 19071		0		229		]
 * 		[ 19071		-27		-68		]
 * 		[ 19071		270		0		]
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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1(nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709_sse2_ssse3,
												0xFFF0FFF0FFF0FFF0LL,
												0xFF80FF80FF80FF80LL,
												0x4A7F4A7F4A7F4A7FLL, 14,
												0x00E5000000E50000LL, 7,
												0xFFBCFFE5FFBCFFE5LL, 7,
												0x0000010E0000010ELL, 7);




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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE2(nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601_sse2_ssse3,
												0xFFC0FFC0FFC0FFC0LL,
												0xFE00FE00FE00FE00LL,
												0x4A7F4A7F4A7F4A7FLL,
												0x0033000000330000LL, 7,
												0xFFE6FFF3FFE6FFF3LL, 7,
												0x0000004000000040LL, 7);
// no change for SSE41
EXTERN_INLINE void nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601_sse2_ssse3_sse41(__m128i* in, __m128i* out) {
	nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601_sse2_ssse3(in, out);
}


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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE2(nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709_sse2_ssse3,
												0xFFC0FFC0FFC0FFC0LL,
												0xFE00FE00FE00FE00LL,
												0x4A7F4A7F4A7F4A7FLL,
												0x0039000000390000LL, 7,
												0xFFEFFFF9FFEFFFF9LL, 7,
												0x0000004400000044LL, 7);
// no change for SSE41
EXTERN_INLINE void nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709_sse2_ssse3_sse41(__m128i* in, __m128i* out) {
	nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709_sse2_ssse3(in, out);
}




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
DEFINE_NNB_Y_UV_TO_RGB_SSE2_SSSE3_INLINE3(nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_sse2_ssse3,
												0xFE00FE00FE00FE00LL,
												2,
												0x005A0000005A0000LL,
												0xFFD2FFEAFFD2FFEALL,
												0x0000007100000071LL);
// no change for SSE41
EXTERN_INLINE void nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_sse2_ssse3_sse41(__m128i* in, __m128i* out) {
	nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_sse2_ssse3(in, out);
}



/*
 * Upsampled SSE2 / SSSE3 conversion
 */
#define	DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1(fn_name, yOffset, uvOffset, yCoef, yCoefLeftShift,uvRCoef, uvRCoefLeftShift, uvGCoef, uvGCoefLeftShift, uvBCoef, uvBCoefLeftShift) \
EXTERN_INLINE void fn_name(__m128i* in_3_v16i_y_uvOdd_uvEven_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(sub128, uvOffset, uvOffset);\
	CONST_M128I(yCoeffs, yCoef, yCoef);\
	CONST_M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	CONST_M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	CONST_M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	CONST_M128I(shuff1,	0xFFFF0504FFFF0100LL, 0xFFFF0D0CFFFF0908LL);\
	CONST_M128I(shuff2,	0x0504FFFF0100FFFFLL, 0x0D0CFFFF0908FFFFLL);\
	M128I(yVect, yOffset, yOffset);\
	M128I(uvOdd, 0x0LL, 0x0LL);\
	M128I(uvEven, 0x0LL, 0x0LL);\
	_M(yVect) = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[0], _M(yVect));\
	in_3_v16i_y_uvOdd_uvEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(sub128));\
	in_3_v16i_y_uvOdd_uvEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(sub128));\
	_M(yVect) = _mm_slli_epi16(_M(yVect), (16-yCoefLeftShift));\
	_M(yVect) = _mm_mulhi_epu16(_M(yVect), _M(yCoeffs));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvRCoeffs));\
	_M(uvOdd) = _mm_srai_epi32(_M(uvOdd), (uvRCoefLeftShift));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvRCoeffs));\
	_M(uvEven) = _mm_srai_epi32(_M(uvEven), (uvRCoefLeftShift));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVect), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvGCoeffs));\
	_M(uvOdd) = _mm_srai_epi32(_M(uvOdd), (uvGCoefLeftShift));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvGCoeffs));\
	_M(uvEven) = _mm_srai_epi32(_M(uvEven), (uvGCoefLeftShift));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVect), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvBCoeffs));\
	_M(uvOdd) = _mm_srai_epi32(_M(uvOdd), (uvBCoefLeftShift));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvBCoeffs));\
	_M(uvEven) = _mm_srai_epi32(_M(uvEven), (uvBCoefLeftShift));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVect), _M(uvEven));\
}

/*
 * variation from the previous routine where:
 * - the y coefficient must be 16-bit left shifted
 * - the uvR, uvG & uvB coefficients must all be 8-bit left shifted
 */
#define	DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE2(fn_name, yOffset, uvOffset, yCoef, uvRCoef, uvGCoef, uvBCoef) \
EXTERN_INLINE void fn_name(__m128i* in_3_v16i_y_uvOdd_uvEven_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(sub128, uvOffset, uvOffset);\
	CONST_M128I(yCoeffs, yCoef, yCoef);\
	CONST_M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	CONST_M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	CONST_M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	CONST_M128I(shuff1,	0xFFFF0605FFFF0201LL, 0xFFFF0E0DFFFF0A09LL);\
	CONST_M128I(shuff2,	0x0605FFFF0201FFFFLL, 0x0E0DFFFF0A09FFFFLL);\
	M128I(yVect, yOffset, yOffset);\
	M128I(uvOdd, 0x0LL, 0x0LL);\
	M128I(uvEven, 0x0LL, 0x0LL);\
	_M(yVect) = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[0], _M(yVect));\
	in_3_v16i_y_uvOdd_uvEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(sub128));\
	in_3_v16i_y_uvOdd_uvEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(sub128));\
	_M(yVect) = _mm_mulhi_epu16(_M(yVect), _M(yCoeffs));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvRCoeffs));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvRCoeffs));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVect), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvGCoeffs));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvGCoeffs));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVect), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvBCoeffs));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvBCoeffs));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVect), _M(uvEven));\
}

/*
 *  variation from the first routine where:
 *  - there is no y offset
 *  - there is no y coefficient but a right shift is made on the Y vectors,
 *  - the uvR, uvG & uvB coefficients must all be 8-bit left shifted
 */
#define	DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE3(fn_name, uvOffset, yRightShift, uvRCoef, uvGCoef, uvBCoef) \
EXTERN_INLINE void fn_name(__m128i* in_3_v16i_y_uvOdd_uvEven_vectors, __m128i* out_3_v16i_rgb_vectors)\
{\
	CONST_M128I(sub128, uvOffset, uvOffset);\
	CONST_M128I(uvRCoeffs, uvRCoef, uvRCoef);\
	CONST_M128I(uvGCoeffs, uvGCoef, uvGCoef);\
	CONST_M128I(uvBCoeffs, uvBCoef, uvBCoef);\
	CONST_M128I(shuff1,	0xFFFF0605FFFF0201LL, 0xFFFF0E0DFFFF0A09LL);\
	CONST_M128I(shuff2,	0x0605FFFF0201FFFFLL, 0x0E0DFFFF0A09FFFFLL);\
	M128I(yVect, 0x0LL, 0x0LL);\
	M128I(uvOdd, 0x0LL, 0x0LL);\
	M128I(uvEven, 0x0LL, 0x0LL);\
	in_3_v16i_y_uvOdd_uvEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(sub128));\
	in_3_v16i_y_uvOdd_uvEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(sub128));\
	_M(yVect) = _mm_srli_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[0], yRightShift);\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvRCoeffs));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvRCoeffs));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(yVect), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvGCoeffs));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvGCoeffs));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(yVect), _M(uvEven));\
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvBCoeffs));\
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));\
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvBCoeffs));\
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));\
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));\
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(yVect), _M(uvEven));\
}

/*
 * The previous macro expands to:
 *
 *	EXTERN_INLINE void convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3(__m128i* in_3_v16i_y_uvOdd_uvEven_vectors, __m128i* out_3_v16i_rgb_vectors)
 *	{
 *	 CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
 *	 CONST_M128I(yCoeffs, 0x94FE94FE94FE94FELL, 0x94FE94FE94FE94FELL);
 *	 CONST_M128I(uvRCoeffs, 0x00CC000000CC0000LL, 0x00CC000000CC0000LL);
 *	 CONST_M128I(uvGCoeffs, 0xFF98FFCEFF98FFCELL, 0xFF98FFCEFF98FFCELL);
 *	 CONST_M128I(uvBCoeffs, 0x0000018200000182LL, 0x0000018200000182LL);
 *	 CONST_M128I(shuff1,	0xFFFF0504FFFF0100LL, 0xFFFF0D0CFFFF0908LL);
 *	 CONST_M128I(shuff2,	0x0504FFFF0100FFFFLL, 0x0D0CFFFF0908FFFFLL);
 *	 M128I(sub16, 0xFFF0FFF0FFF0FFF0LL, 0xFFF0FFF0FFF0FFF0LL);
 *	 M128I(uvOdd, 0x0LL, 0x0LL);
 *	 M128I(uvEven, 0x0LL, 0x0LL);
 *	 
 *	 // Y - 16
 *	 _M(sub16) = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[0], _M(sub16));		// PADDW		1	0.5
 *	 
 *	 // U - 128	V - 128
 *	 in_3_v16i_y_uvOdd_uvEven_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(sub128));	// PADDW		1	0.5
 *	 in_3_v16i_y_uvOdd_uvEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(sub128));	// PADDW		1	0.5
 *	 
 *	 //
 *	 // Y values
 *	 //
 *   // left shift Y values by (16 - number of left-shifted bits in y coefficient) bits
 *	 // The next instruction (_mm_mulhi_epu16) will perform a 16-bit right shift. As the Y
 *   // coefficients are already left-shifted, we only need to shift the y vector by
 *   // 16 - (number of left-shifted bits in y coefficients)
 *	 _M(sub16) = _mm_slli_epi16(_M(sub16), 8);										// PSLLW		1	1
 *	 
 *	 // Multiply 8-bit left-shifted Y values by 15-bit left-shifted Y coeffs
 *	 // and keep highest 16 bits
 *	 _M(sub16) = _mm_mulhi_epu16(_M(sub16), _M(yCoeffs));							// PMULHUW		3	1
 *	 // Y1 	Y2		Y3		Y4		Y5		Y6		Y7		Y8
 *	 
 *	 //
 *	 // R
 *	 // U and V coefficients
 *	 // 0, 204, 0, 204, 0, 204, 0, 204
 *	 _M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvRCoeffs));// PMADDWD		3	1
 *	 // U1*0 + V1*204	U3*0 + V3*204	U5*0 + V5*204	U7*0 + V7*204
 *	 // C1			C3			C5			C7		(4 * 32-bit values)
 *	 // A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
 *	 //					max value with 7bit left shift of coeffs:
 *	 //					386*127 = 49022 (16 bit wide)
 *	 //					min value:
 *	 //					386*-128= 2's(49408) (49408 fits in 16 bit)
 *	 
 *	 // shift right by 7 to account for left shift by 7 of coefficients
 *	 _M(uvOdd) = _mm_srai_epi32(_M(uvOdd), 7);										// PSRAD		1	1
 *	 // C1 Sb Sb Sb	C3 Sb Sb Sb	C5 Sb Sb Sb	C7 Sb Sb Sb (4 * 32-bit values)
 *	 
 *	 // shuffle bytes: keep lo word and zero hi word
 *	 _M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1	0.5
 *	 // C1 Sb 0 0	C3 Sb 0 0	C5 Sb 0 0	C7 Sb 0 0
 *	 
 *	 _M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvRCoeffs));// PMADDWD		3	1
 *	 // U2*0 + V2*204	U4*0 + V4*204	U6*0 + V6*204	U8*0 + V8*204
 *	 // C2			C4				C6				C8
 *	 
 *	 // shift right by 7 to account for left shift by 7 of coefficients
 *	 _M(uvEven) = _mm_srai_epi32(_M(uvEven), 7);									// PSRAD		1	1
 *	 // C2 Sb Sb Sb	C4 Sb Sb Sb	C6 Sb Sb Sb	C8 Sb Sb Sb (4 * 32-bit values)
 *	 
 *	 // Move these values to the hi word and right shift by 7 to account for 7-bit left shift
 *	 // of coefficients: 16 - 7 = 9 - we need to left shift by 9 bits.
 *	 _M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1	0.5
 *	 // 0 C2			0 C4		0 C6		0 C8
 *	 
 *	 _M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
 *	 // C1 C2		C3 C4		C5 C6		C7 C8
 *	 
 *	 out_3_v16i_rgb_vectors[0] = _mm_add_epi16(_M(sub16), _M(uvEven));				// PADDW		1	1
 *	 
 *	 
 *	 
 *	 //
 *	 // G
 *	 // U and V coeffs
 *	 // -50, -104, -50, -104, -50, -104, -50, -104,
 *	 _M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvGCoeffs));// PMADDWD		3	1
 *	 // C1			C3				C5				C7		(4 * 32-bits values)
 *	 
 *	 _M(uvOdd) = _mm_srai_epi32(_M(uvOdd), 7);										// PSRAD		1	1
 *	 // C1 Sb Sb Sb	C3 Sb Sb Sb	C5 Sb Sb Sb	C7 Sb Sb Sb (4 * 32-bit values)
 *	 
 *	 _M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1	0.5
 *	 // C1 0			C3  0		C5 0		C7 0
 *	 
 *	 _M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvGCoeffs));// PMADDWD		3	1
 *	 // U2*0 + V2*204	U4*0 + V4*204	U6*0 + V6*204	U8*0 + V8*204
 *	 // C2			C4				C6				C8
 *	 
 *	 _M(uvEven) = _mm_srai_epi32(_M(uvEven), 7);									// PSRAD		1	1
 *	 
 *	 _M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1	0.5
 *	 // 0 C2			0 C4		0 C6		0 C8
 *	 
 *	 _M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
 *	 // C1 C2		C3 C4		C5 C6		C7 C8
 *	 
 *	 out_3_v16i_rgb_vectors[1] = _mm_add_epi16(_M(sub16), _M(uvEven));				// PADDW		1	0.5
 *	 
 *	 
 *	 
 *	 //
 *	 // B
 *	 // U and V coeffs
 *	 // 386, 0, 386, 0, 386, 0, 386, 0,
 *	 _M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[1], _M(uvBCoeffs));// PMADDWD		3	1
 *	 // C1			C3				C5				C7		(4 * 32-bits values)
 *	 
 *	 _M(uvOdd) = _mm_srai_epi32(_M(uvOdd), 7);										// PSRAD		1	1
 *	 // C1 Sb Sb Sb	C3 Sb Sb Sb	C5 Sb Sb Sb	C7 Sb Sb Sb (4 * 32-bit values)
 *	 
 *	 _M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1	0.5
 *	 // C1 0			C3  0		C5 0		C7 0
 *	 
 *	 _M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uvEven_vectors[2], _M(uvBCoeffs));// PMADDWD		3	1
 *	 // U2*0 + V2*204	U4*0 + V4*204	U6*0 + V6*204	U8*0 + V8*204
 *	 
 *	 _M(uvEven) = _mm_srai_epi32(_M(uvEven), 7);									// PSRAD		1	1
 *	 
 *	 _M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1	0.5
 *	 // 0 C2			0 C4		0 C6		0 C8
 *	 
 *	 _M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			1	0.33
 *	 // C1 C2		C3 C4		C5 C6		C7 C8
 *	 
 *	 out_3_v16i_rgb_vectors[2] = _mm_add_epi16(_M(sub16), _M(uvEven));				// PADDW		1	0.5
 *	};
 */
 

	


/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using BT601 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			44 cycles
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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1(convert_y_uv_vectors_to_rgb_vectors_bt601_sse2_ssse3,
											   0xFFF0FFF0FFF0FFF0LL,
											   0xFF80FF80FF80FF80LL,
											   0x94FE94FE94FE94FELL, 15,
											   0x00CC000000CC0000LL, 7,
											   0xFF98FFCEFF98FFCELL, 7,
											   0x0000018200000182LL, 7);

/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using BT709 YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			44 cycles
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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE1(convert_y_uv_vectors_to_rgb_vectors_bt709_sse2_ssse3,
											   0xFFF0FFF0FFF0FFF0LL,
											   0xFF80FF80FF80FF80LL,
											   0x94FE94FE94FE94FELL, 15,
											   0x00E5000000E50000LL, 7,
											   0xFFBCFFE5FFBCFFE5LL, 7,
											   0x0000010E0000010ELL, 7);


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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE2(convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601_sse2_ssse3,
										  0xFFC0FFC0FFC0FFC0LL,
										  0xFE00FE00FE00FE00LL,
										  0x4A7F4A7F4A7F4A7FLL,
										  0x0066000000660000LL,
										  0xFFCCFFE7FFCCFFE7LL,
										  0x0000008100000081LL
										  );
// no change for SSE41
EXTERN_INLINE void convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601_sse2_ssse3_sse41(__m128i* in, __m128i* out) {
	convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601_sse2_ssse3(in, out);
}


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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE2(convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709_sse2_ssse3,
										  0xFFC0FFC0FFC0FFC0LL,
										  0xFE00FE00FE00FE00LL,
										  0x4A7F4A7F4A7F4A7FLL,
										  0x0073000000730000LL,
										  0xFFDEFFF2FFDEFFF2LL,
										  0x0000008700000087LL
										  );
// no change for SSE41
EXTERN_INLINE void convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709_sse2_ssse3_sse41(__m128i* in, __m128i* out) {
	convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709_sse2_ssse3(in, out);
}




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
DEFINE_UPSAMPLED_Y_UV_TO_RGB_SSE2_SSSE3_INLINE3(convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_sse2_ssse3,
										  0xFE00FE00FE00FE00LL,
										  2,
										  0x005A0000005A0000LL,
										  0xFFD2FFEAFFD2FFEALL,
										  0x0000007100000071LL);
// no change for SSE41
EXTERN_INLINE void convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_sse2_ssse3_sse41(__m128i* in, __m128i* out) {
	convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_sse2_ssse3(in, out);
}


#endif	// __INTEL_CPU__

#endif /* YUV_TO_RGB_CONVERT_H_ */


