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


#include <emmintrin.h>
#include <tmmintrin.h>

#include "platform_util.h"

/*
 * Convert 2 vectors of 8 short Y, UY into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			53 cycles
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
EXTERN_INLINE void convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors) 
{
	CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
	M128I(uvRCoeffs, 0x0166000001660000LL, 0x0166000001660000LL);
	M128I(uvGCoeffs, 0xFF4AFFA8FF4AFFA8LL, 0xFF4AFFA8FF4AFFA8LL);
	M128I(uvBCoeffs, 0x000001C4000001C4LL, 0x000001C4000001C4LL);
	
	// U - 128	V - 128
	in_2_v16i_y_uv_vectors[1] = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));// PADDW		2	2
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvRCoeffs) = _mm_madd_epi16(in_2_v16i_y_uv_vectors[1], _M(uvRCoeffs));		// PMADDWD		9 8 2 2
	// U12*0 + V12*358	U34*0 + V34*358	U56*0 + V56*358	U78*0 + V78*358
	// C12				C34				C56				C78		(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					358*127 = 45466 (16 bit wide)
	//					min value:
	//					358*-128= 2's(45824) (45824 fits in 16 bits)
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvRCoeffs) = _mm_srai_epi32(_M(uvRCoeffs), 8);								// PSRAD		2	2
	// C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
	
	_M(uvRCoeffs) = _mm_shufflehi_epi16(_M(uvRCoeffs), 0xA0);						// PSHUFHW		2	2
	// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
	
	_M(uvRCoeffs) = _mm_shufflelo_epi16(_M(uvRCoeffs), 0xA0);						// PSHUFLW		2	2
	// C12 0 C12 0		C34 0 C34 0		C56 0 C56 0		C78 0 C78 0
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvRCoeffs)); // PADDW	2	2
	//
	
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvGCoeffs) = _mm_madd_epi16(in_2_v16i_y_uv_vectors[1], _M(uvGCoeffs));		// PMADDWD		9 8 2 2
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	// C12					C34					C56					C78
	
	// shift right by 8
	_M(uvGCoeffs) = _mm_srai_epi32(_M(uvGCoeffs), 8);								// PSRAD		2	2
	// C12 0 0 0		C34 0 0 0		C56 0 0 0		C78 0 0 0
	
	_M(uvGCoeffs) = _mm_shufflehi_epi16(_M(uvGCoeffs), 0xA0);						// PSHUFHW		2	2
	// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
	
	_M(uvGCoeffs) = _mm_shufflelo_epi16(_M(uvGCoeffs), 0xA0);						// PSHUFLW		2	2
	// C12 0 C12 0		C34 0 C34 0		C56 0 C56 0		C78 0 C78 0
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvGCoeffs));// PADDW	2	2
	//
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvBCoeffs) = _mm_madd_epi16(in_2_v16i_y_uv_vectors[1], _M(uvBCoeffs));		// PMADDWD		9 8 2 2
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	// C12					C34					C56					C78
	
	// shift right by 8
	_M(uvBCoeffs) = _mm_srai_epi32(_M(uvBCoeffs), 8);								// PSRAD		2	2
	// C12	0		C34	 0		C56	 0		C78	 0
	
	_M(uvBCoeffs) = _mm_shufflehi_epi16(_M(uvBCoeffs), 0xA0);						// PSHUFHW		2	2
	// C12 0 0 0		C34 0 0 0		C56 0 C56 0		C78 0 C78 0
	
	_M(uvBCoeffs) = _mm_shufflelo_epi16(_M(uvBCoeffs), 0xA0);						// PSHUFLW		2	2
	// C12 0 C12 0		C34 0 C12 0		C56 0 C56 0		C78 0 C78 0
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0],  _M(uvBCoeffs));// PADDW	2	2
	//
	
};


/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			97 cycles
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
	// 																					PADDW		2	2
	in_3_v16i_y_uvOdd_uyEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(sub128));
	// 																					PADDW		2	2
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvRCoeffs));
	// U1*0 + V1*358	U3*0 + V3*358	U5*0 + V5*358	U7*0 + V7*358				// PMADDWD		9 8 2 2
	// C1		C3		C5		C7			(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					452*127 = 57404 (16 bit wide)
	//					min value:
	//					452*-128= 2's(57856) (57856 fits in 16 bit wide)
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 8), _M(zeroHiWord));		// PSRAD		2	2
	// C1 0			C3  0		C5 0		C7 0									// PAND			2	2
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvRCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		9 8 2 2
	
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 8),  _M(zeroLoWord));		// PSLLD		2	2
	// 0 C2			0 C4		0 C6		0 C8									// PAND			2	2
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			2	2
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		2	2
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvGCoeffs));	// PMADDWD		9 8 2 2
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	// C12		C34		C56		C78
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 8),  _M(zeroHiWord));		// PSRAD		2	2
	// C1 0			C3  0		C5 0		C7 0									// PAND			2	2
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvGCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		9 8 2 2
	
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 8),  _M(zeroLoWord));		// PSLLD		2	2
	// 0 C2			0 C4		0 C6		0 C8									// PAND			2	2
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			2	2
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		2	2
	
	
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvBCoeffs));	// PMADDWD		9 8 2 2
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	// 0 C12		0 C34		0 C56		0 C78
	
	// shift right by 8 to account for left shift by 8 of coefficients
	_M(uvOdd) = _mm_and_si128(_mm_srai_epi32(_M(uvOdd), 8),  _M(zeroHiWord));		// PSRAD		2	2
	// C1 0			C3  0		C5 0		C7 0									// PAND			2	2
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvBCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		9 8 2 2
	
	_M(uvEven) = _mm_and_si128(_mm_slli_epi32(_M(uvEven), 8),  _M(zeroLoWord));		// PSLLD		2	2
	// 0 C2			0 C4		0 C6		0 C8									// PAND			2	2
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			2	2
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		2	2
};


/*
 * Convert 2 vectors of 8 short Y, UV into 3 vectors of 8 short R, G & B
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
EXTERN_INLINE void convert_y_uv_vectors_to_rgb_vectors_no_interpolation_sse2_ssse3(__m128i* in_2_v16i_y_uv_vectors, __m128i* out_3_v16i_rgb_vectors) 
{
	CONST_M128I(shuffMask, 0x0605060502010201LL, 0x0E0D0E0D0A090A09LL);
	CONST_M128I(sub128, 0xFF80FF80FF80FF80LL, 0xFF80FF80FF80FF80LL);
	M128I(uvRCoeffs, 0x0166000001660000LL, 0x0166000001660000LL);
	M128I(uvGCoeffs, 0xFF4AFFA8FF4AFFA8LL, 0xFF4AFFA8FF4AFFA8LL);
	M128I(uvBCoeffs, 0x000001C4000001C4LL, 0x000001C4000001C4LL);
	
	// U - 128	V - 128
	in_2_v16i_y_uv_vectors[1] = _mm_add_epi16(in_2_v16i_y_uv_vectors[1], _M(sub128));// PADDW		2	2
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvRCoeffs) = _mm_madd_epi16(in_2_v16i_y_uv_vectors[1], _M(uvRCoeffs));		// PMADDWD		9 8 2 2
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
	_M(uvRCoeffs) = _mm_shuffle_epi8 (_M(uvRCoeffs), _M(shuffMask));				// PSHUFB		1 1 3   0.5 1 2
	// C12 C12		C34 C34 	C56 C56		C78 C78
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvRCoeffs)); // PADDW	2	2
	//
	
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvGCoeffs) = _mm_madd_epi16(in_2_v16i_y_uv_vectors[1], _M(uvGCoeffs));		// PMADDWD		9 8 2 2
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	// C12		C34		C56		C78
	
	_M(uvGCoeffs) = _mm_shuffle_epi8 (_M(uvGCoeffs), _M(shuffMask));				// PSHUFB		1 1 3   0.5 1 2
	// C12 C12		C34 C34 	C56 C56		C78 C78
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvGCoeffs));// PADDW	2	2
	//
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvBCoeffs) = _mm_madd_epi16(in_2_v16i_y_uv_vectors[1], _M(uvBCoeffs));		// PMADDWD		9 8 2 2
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	// 0 C12		0 C34		0 C56		0 C78
	
	_M(uvBCoeffs) = _mm_shuffle_epi8 (_M(uvBCoeffs), _M(shuffMask));				// PSHUFB		1 1 3   0.5 1 2
	// C12 C12		C34 C34 	C56 C56		C78 C78
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_2_v16i_y_uv_vectors[0], _M(uvBCoeffs));// PADDW	2	2
	//
};

/*
 * Convert 3 vectors of 8 short Y, UVOdd, UVEven into 3 vectors of 8 short R, G & B
 * using full range YCbCr to RGB conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 *
 * Total latency: 			76 cycles
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
	// 																					PADDW		2	2
	in_3_v16i_y_uvOdd_uyEven_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(sub128));
	// 																					PADDW		2	2
	
	//
	// R
	// U and V coefficients
	// 0, 358, 0, 358, 0, 358, 0, 358
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvRCoeffs));
	// U1*0 + V1*358	U3*0 + V3*358	U5*0 + V5*358	U7*0 + V7*358				// PMADDWD		9 8		2 2
	// C1		C3		C5		C7			(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					452*127 = 57404 (16 bit wide)
	//					min value:
	//					452*-128= 2's(57856) (57856 fits in 16 bit wide)
	
	// shuffle bytes: shift right by 8 to account for left shift by 8 of coefficients, keep next signed byte and zero rest of dword
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1 1 3   0.5 1 2
	// C1 0			C3  0		C5 0		C7 0
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvRCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		9 8		2 2
	
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1 1 3   0.5 1 2
	// 0 C2			0 C4		0 C6		0 C8
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			2	2
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[0] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		2	2
	
	//
	// G
	// U and V coeffs ()
	// -88, -182, -88, -182, -88, -182, -88, -182
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvGCoeffs));	// PMADDWD		9 8		2 2
	// U12*-88 + V12*-182	U34*-88 + V34*-182	U56*-88 + V56*-182	U78*-88 + V78*-182
	
	// shuffle bytes: shift right by 8 to account for left shift by 8 of coefficients, keep next signed byte and zero rest of dword
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1 1 3   0.5 1 2
	// C1 0			C3  0		C5 0		C7 0
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvGCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		9 8		2 2
	
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1 1 3   0.5 1 2
	// 0 C2			0 C4		0 C6		0 C8
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			2	2
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[1] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		2	2
	
	
	
	//
	// B
	// U and V coeffs ()
	// 0, 452, 0, 452, 0, 452, 0, 452
	_M(uvOdd) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[1], _M(uvBCoeffs));	// PMADDWD		9 8 2 2
	// U12*0 + V12*452	U34*0 + V34*452	U56*0 + V56*452	U78*0 + V78*452
	
	// shuffle bytes: shift right by 8 to account for left shift by 8 of coefficients, keep next signed byte and zero rest of dword
	_M(uvOdd) = _mm_shuffle_epi8(_M(uvOdd), _M(shuff1));							// PSHUFB		1 1 3   0.5 1 2
	// C1 0			C3  0		C5 0		C7 0
	
	_M(uvEven) = _mm_madd_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[2], _M(uvBCoeffs));
	// U2*0 + V2*358	U4*0 + V4*358	U6*0 + V6*358	U8*0 + V8*358				// PMADDWD		9 8 2 2
	
	_M(uvEven) = _mm_shuffle_epi8(_M(uvEven), _M(shuff2));							// PSHUFB		1 1 3   0.5 1 2
	// 0 C2			0 C4		0 C6		0 C8
	
	_M(uvEven) = _mm_or_si128(_M(uvEven), _M(uvOdd));								// POR			2	2
	// C1 C2		C3 C4		C5 C6		C7 C8
	
	out_3_v16i_rgb_vectors[2] = _mm_add_epi16(in_3_v16i_y_uvOdd_uyEven_vectors[0], _M(uvEven));
	// 																					PADDW		2	2
};



#endif /* YUV_TO_RGB_CONVERT_H_ */


