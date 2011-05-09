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
 * Convert 3 vectors of 8 short R, GB1, GB2 into 3 vectors of 8 short Y, U & V
 * using full range RGB to YCbCr conversion equations from
 * http://www.equasys.de/colorconversion.html
 *
 * Uses nearest neighbour upsampling:
 * U12 & V12 are created from chromas value for pixel 1 only.
 *
 * Total latency: 			94 cycles
 * Num of pixel handled:	8
 *
 * Y = 	[  0  ] + [  0.299		 0.587		 0.114	]	( R )
 * U = 	[ 128 ] + [ -0.169		-0.331		 0.500	]	( G )
 * V = 	[ 128 ] + [  0.500		-0.419		-0.081	]	( B )
 *
 *				R coeffs left-shifted by 16 bits
 *				G & B coeffs left-shifted by 8 bits
 * 					[  19595	 150		 29		]
 * 					[ -11075	-84			 128	]
 * 					[  32767	-107		-21		]
 *
 *	Note: the V-R coeff (32767) should really be 32768 but because we need it to be
 *			a 16-bit signed integer, it has been capped to its maximum value for this
 *			type. 32767 maps to 0.499985 instead of 0.500 which is an acceptable
 *			approximation.
 *
 * INPUT:
 *
 * 3 vectors of 8 short:
 * rVect
 * R1 0		R2 0	R3 0	R4 0	R5 0	R6 0	R7 0	R8 0
 *
 * gb1Vect
 * G1 0		B1 0	G2 0	B2 0	G3 0	B3 0	G4 0	B4 0
 *
 * gb1Vect
 * G5 0		B5 0	G6 0	B6 0	G7 0	B7 0	G8 0	B8 0
 *
 * OUTPUT:
 *
 * yVect
 * Y1 0		Y2 0	Y3 0	Y4 0	Y5 0	Y6 0	Y7 0	Y8 0
 *
 * uvVect
 * U12 0	V12 0	U34 0	V34 0	U56 0	V56 0	U78 0	V78 0
 */
EXTERN_INLINE void convert_n_nnb_downsample_r_gb_vectors_to_y_uv_vectors_sse2(__m128i* in_3_v16i_r_gb_vectors, __m128i* out_2_v16i_y_uv_vectors)
{
	CONST_M128I(add128, 0x0080008000800080LL, 0x0080008000800080LL);
	CONST_M128I(rYCoeffs, 0x4C8B4C8B4C8B4C8BLL, 0x4C8B4C8B4C8B4C8BLL);
	CONST_M128I(rUVCoeffsInterleaved, 0xFFFFD4BDFFFFD4BDLL, 0xFFFFD4BDFFFFD4BDLL);
	CONST_M128I(gbYCoeffs, 0x001D0096001D0096LL, 0x001D0096001D0096LL);
	CONST_M128I(gbUCoeffs, 0x0080FFAC0080FFACLL, 0x0080FFAC0080FFACLL);
	CONST_M128I(gbVCoeffs, 0xFFEBFF95FFEBFF95LL, 0xFFEBFF95FFEBFF95LL);
	CONST_M128I(zeroHiWord, 0x0000FFFF0000FFFFLL, 0x0000FFFF0000FFFFLL);
	CONST_M128I(zeroLoWord, 0xFFFF0000FFFF0000LL, 0xFFFF0000FFFF0000LL);

	M128I(rScratch, 0x0LL, 0x0LL);
	M128I(gb1Scratch, 0x0LL, 0x0LL);
	M128I(gb2Scratch, 0x0LL, 0x0LL);
	M128I(gbOdd, 0x0LL, 0x0LL);
	M128I(rOdd, 0x0LL, 0x0LL);

	print_xmm16("R", in_3_v16i_r_gb_vectors);
	print_xmm16("GB1", &in_3_v16i_r_gb_vectors[1]);
	print_xmm16("GB2", &in_3_v16i_r_gb_vectors[2]);
	//
	// Y
	// R coeffs
	// Multiply R values by 16-bit left-shifted Y coeffs and keep highest 16 bits
	_M(rScratch) = _mm_mulhi_epi16(in_3_v16i_r_gb_vectors[0], _M(rYCoeffs));		// PMULHW		9 8 2 2

	// G and B coefficients 1 to 4
	// 150, 29, 150, 29, 150, 29, 150, 29
	_M(gb1Scratch) = _mm_madd_epi16(in_3_v16i_r_gb_vectors[1], _M(gbYCoeffs));		// PMADDWD		9 8 2 2
	// G1*150 + B1*29	G2*150 + B2*29	G3*150 + B3*29	G4*150 + B4*29
	// C1				C2				C3				C4		(4 * 32-bits values)
	// A B Sb Sb 		upper 16 bits are always the sign bit due to the coeffs and pixels values
	//					max value with 8bit left shift of coeffs:
	//					150*255 = 38250 (16 bit wide)
	//					min value:
	//					-107*255= 2's(27285) (27285 fits in 16 bits)

	// shift right by 8 to account for left shift by 8 of coefficients
	_M(gb1Scratch) = _mm_srai_epi32(_M(gb1Scratch), 8);								// PSRAD		2	2
	// C1 Sb Sb Sb		C2 Sb Sb Sb		C3 Sb Sb Sb		C4 Sb Sb Sb

	// G and B coefficients 5 to 8
	// 150, 29, 150, 29, 150, 29, 150, 29
	_M(gb2Scratch) = _mm_madd_epi16(in_3_v16i_r_gb_vectors[2], _M(gbYCoeffs));		// PMADDWD		9 8 2 2
	// C5				C6				C7				C8		(4 * 32-bits values)

	// shift right by 8 to account for left shift by 8 of coefficients
	_M(gb2Scratch) = _mm_srai_epi32(_M(gb2Scratch), 8);								// PSRAD		2	2
	// C5 Sb Sb Sb		C6 Sb Sb Sb		C7 Sb Sb Sb		C8 Sb Sb Sb

	// pack both sets
	_M(gb1Scratch) = _mm_packs_epi32(_M(gb1Scratch), _M(gb2Scratch) );				// PACKSSDW		4 4 2 2
	// C1 C2		C3 C4		C5 C6		C7 C8		(8 16-bit values)

	out_2_v16i_y_uv_vectors[0] = _mm_add_epi16(_M(rScratch), _M(gb1Scratch));		// PADDW		2	2
	// Y1 0 Y2 0	Y3 0 Y4 0	Y5 0 Y6 0	Y7 0 Y8 0

	print_xmm16("Y 1-8", out_2_v16i_y_uv_vectors);

	//
	// Since we are going doing nearest neighbour downsampling, lets only
	// do calculation for chroma values U and V we are going to keep, which are
	// the ones from odd pixels (1, 3, 5 and 7). So we first gather those pixels
	// together and  then do the conversion.
	_M(gbOdd) = _mm_unpacklo_epi64(_mm_shuffle_epi32(in_3_v16i_r_gb_vectors[1], 0x08), _mm_shuffle_epi32(in_3_v16i_r_gb_vectors[2], 0x08));
	// G1 B1	G3 B3	G5 B5	G7 B7
	//																				   PSHUFD		4 4 2 2
	//																				   PSHUFD		4 4 2 2
	//																				   PUNPCKLQDQ	4 4 1 1

	_M(rOdd) = _mm_shufflehi_epi16(in_3_v16i_r_gb_vectors[0], 0xA0);				// PSHUFHW		2	2
	// R1 0 0 0		R3 0 0 0	R5 0 R5 0	R7 0 R7 0

	_M(rOdd) = _mm_shufflelo_epi16(_M(rOdd), 0xA0);									// PSHUFLW		2	2
	// R1 0 R1 0	R3 0 R3 0	R5 0 R5 0	R7 0 R7 0


	//
	// U

	// G and B odd coefficients
	_M(gb1Scratch) = _mm_madd_epi16(_M(gbOdd), _M(gbUCoeffs));						// PMADDWD		9 8 2 2
	// C1				C3				C5				C7		(4 * 32-bits values)

	// shift right by 8 to account for left shift by 8 of coefficients
	_M(gb1Scratch) = _mm_srai_epi32(_M(gb1Scratch), 8);								// PSRAD		2	2
	// C1 Sb Sb Sb		C3 Sb Sb Sb		C5 Sb Sb Sb		C7 Sb Sb Sb

	// zero hi word
	_M(gb1Scratch) = _mm_and_si128(_M(gb1Scratch), _M(zeroHiWord));					// PAND			2   2
	// C1 Sb 0 0		C3 Sb 0 0		C5 Sb 0 0		C7 Sb 0 0

	//
	// V
	// G and B odd coefficients
	_M(gb2Scratch) = _mm_madd_epi16(_M(gbOdd), _M(gbVCoeffs));						// PMADDWD		9 8 2 2
	// C1				C3				C5				C7		(4 * 32-bits values)

	// shift right by 8 to account for left shift by 8 of coefficients
	// here we shift left since we need to place the coeffs in the right spot
	_M(gb2Scratch) = _mm_slli_epi32(_M(gb2Scratch), 8);								// PSLLD		2	2

	// zero lo word
	_M(gb2Scratch) = _mm_and_si128(_M(gb2Scratch), _M(zeroLoWord));					// PAND			2   2
	// 0 0 C1 Sb	0 0 C3 Sb		0 0 C5 Sb		0 0 C7 Sb


	// OR both U & V scratch vars
	_M(gb1Scratch) = _mm_or_si128(_M(gb1Scratch), _M(gb2Scratch));					// POR			2   2
	// U12 V12			U34 V34			U56 V56			U78 V78

	// Calculate R coeffs
	_M(rScratch) = _mm_mulhi_epi16(_M(rOdd), _M(rUVCoeffsInterleaved));				// PMULHW		9 8 2 2

	// add R and GB coeffs
	_M(gb2Scratch) = _mm_add_epi16(_M(rScratch), _M(gb1Scratch));					// PADDW		2	2

	// U,V + 128
	out_2_v16i_y_uv_vectors[1] = _mm_add_epi16(_M(gb2Scratch), _M(add128));			// PADDW		2	2
	// U12 V12			U34 V34			U56 V56			U78 V78

	print_xmm16("UV", &out_2_v16i_y_uv_vectors[1]);
};



#endif /* RGB_TO_YUV_CONVERT_H_ */

