/*
 * pixfmt_descriptions.c
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

#include "pixfmt_descriptions.h"

/*
 * For some reason, VS does not like intermediate curly braces when initialising
 * elements in a 2D array...
 */
#ifdef WIN32
#define	_E(v_lo, v_hi)		(v_lo), (v_hi)
#else
#define _E(v_lo, v_hi)		{(v_lo), (v_hi)}
#endif

/*
 * Formats in the following array must be in the same order as the
 * PixFcImageFormat enum defined in pixfc-sse.h
 */
const PixelFormatDescription		pixfmt_descriptions[] = {
	{
		    PixFcYUYV,
		    2, 1, 0, 16,
		    {
				// 2 red pix - 2 green pix - 2 blue pix - 2 white pix
				_E(0x15942B95FF4B544CLL, 0x80FE80FF6B1CFF1DLL) ,
	            /*		Decimal values:
	                    76      84      75      255
	                    149     43      148     21
	                    29      255     28      107
	                    255     128     254     128
	            */

				/*
						bright red(255 0 0)		light pink 	(255 179  176)
						76		84				255		255
						bright green(0 255 0)	dark green 	(0   106 0)
						149		41				0		21
						bright blue(0 0 255)	light purple(170 171 255)
						29		255				200		107
						orange (254 165 0)		bordeaux (81 0 0)
						173		30				0		186

				*/
				_E(0x15002995FFFF544CLL, 0xBA001EAD6BC8FF1DLL),
			},
					
		    2,				
		    "YUYV",
	},

	{
		    PixFcUYVY,
		    2, 1, 0, 16,
		    {
				// 2 red pix - 2 green pix - 2 blue pix - 2 white pix
				_E(0x9415952B4BFF4C54LL, 0xFE80FF801C6B1DFFLL),
	            /*		Decimal values:
	                    84      76      255		75
	                    43      149     21		148
	                    255     29      107		28
	                    128     255     128		254
	            */

				/*
						bright red(255 0 0)		light pink 	(255 179  176)
						84		76				255		255
						bright green(0 255 0)	dark green 	(0   106 0)
						41		149				21		0
						bright blue(0 0 255)	light purple(170 171 255)
						255		29				107		200
						orange (254 165 0)		bordeaux (81 0 0)
						30		173				186		0
				*/
				_E(0x00159529FFFF4C54LL, 0x00BAAD1EC86B1DFFLL)
			},
		    2,
		    "UYVY",
	},

	{
			PixFcYUV422P,
			2, 1, 1, 32,
			{
				/* 2 red pix - 2 green pix - 2 blue pix - 2 white pix
						Decimal values:
						84      76      255		75
						43      149     21		148
						255     29      107		28
						128     255     128		254

					bright red(255 0 0)		light pink 	(255 179  176)
						84		76				255		255
					bright green(0 255 0)	dark green 	(0   106 0)
						41		149				21		0
					bright blue(0 0 255)	light purple(170 171 255)
						255		29				107		200
					orange (254 165 0)		bordeaux (81 0 0)
						30		173				186		0
				*/

				_E(0xFEFF1C1D94954B4CLL, 0x00ADC81D0095FF4CLL), //Y
				_E(0xFEFF1C1D94954B4CLL, 0x00ADC81D0095FF4CLL),
				_E(0x1EFF295480FF2B54LL, 0x1EFF295480FF2B54LL), // U
				_E(0xBA6B15FF806B15FFLL, 0xBA6B15FF806B15FFLL), // V
			},
			4,
			"YUV422p",
	},


	{
		    PixFcARGB,
		    4, 1, 0, 16,
		    {
		    	// 2 red pix - 2 green pix - 2 blue pix - 2 white pix
				_E(0x0000FF000000FF00LL, 0x00FF000000FF0000LL),
				_E(0xFF000000FF000000LL, 0xFFFFFF00FFFFFF00LL)
				//
				// With average downsampling, converting the above to YUYV produces:
				// 76   84   76  255	149   53  149   78		29  201   29   85	254  158  254  121
				//
				// 76   93   76  222	149   53  149   78		29  201   29   85	254  158  254  121
				

				// 1 red pix - 1 black pix - 1 white pix - 1 green pix
		    	// 1 bordeaux pix - 1 blue pix - 1 grey pix - 1 red pix
				//_E(0x000000000000FF00LL, 0x00FF0000FFFFFF00LL),
				//_E(0xFF00000000005100LL, 0x0000FF0088888800LL)
			},
			2,
		    "ARGB",
	},

	{
		    PixFcBGRA,
		    4, 1, 0, 16,
		    {
				// 2 red pix - 2 green pix - 2 blue pix - 2 white pix
				_E(0x00FF000000FF0000LL, 0x0000FF000000FF00LL),
				_E(0x000000FF000000FFLL, 0x00FFFFFF00FFFFFFLL)
			},
			2,
		    "BGRA",
	},

	{
		    PixFcRGB24,
		    3, 1, 0, 16,
		    {
				// 2 groups of: 2 red pix - 2 green pix - 2 blue pix - 2 white pix
				_E(0xFF000000FF0000FFLL, 0x00FF000000FF0000LL),
				_E(0xFFFFFFFFFFFFFF00LL, 0xFF000000FF0000FFLL),
				_E(0x00FF000000FF0000LL, 0xFFFFFFFFFFFFFF00LL)
			},
			3,
		    "RGB24",
	},

	{
		    PixFcBGR24,
		    3, 1, 0, 16,
		    {
				// 2 groups of: 2 red pix - 2 green pix - 2 blue pix - 2 white pix
				_E(0xFF00FF0000FF0000LL, 0xFF0000FF00FF0000LL),
				_E(0xFFFFFFFFFFFF0000LL, 0xFF00FF0000FF0000LL),
				_E(0xFF0000FF00FF0000LL, 0xFFFFFFFFFFFF0000LL)
			},
			3,
		    "BGR24",
	},
};


const uint32_t			pixfmt_descriptions_count = sizeof(pixfmt_descriptions) / sizeof(pixfmt_descriptions[0]);


