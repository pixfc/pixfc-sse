/*
 * common.c
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

#include <stdint.h>
#ifdef WIN32
	#include <intrin.h>
#endif

#include "common.h"

const float       rgb_8bit_to_yuv_8bit_coef[][3][3] =
	{
		// Full range
		{{ 0.299f,  0.587f,  0.114f},
		 {-0.169f, -0.331f,  0.500f},
		 { 0.500f, -0.419f, -0.081f}},

		// bt601
		{{ 0.257f,  0.504f,  0.098f},
		 {-0.148f, -0.291f,  0.439f},
		 { 0.439f, -0.368f, -0.071f}},

		// bt709
		{{ 0.183f,  0.614f,  0.062f},
		 {-0.101f, -0.339f,  0.439f},
		 { 0.439f, -0.399f, -0.040f}},
	};

const int32_t     rgb_8bit_to_yuv_8bit_off[][3] =
	{
		{ 0, 128, 128 },	// full range
		{ 16, 128, 128 },	// bt601
		{ 16, 128, 128 },	// bt709
	};

const float       rgb_8bit_to_yuv_10bit_coef[][3][3] =
	{
		// full range
		{{ 1.196f,  2.348f,  0.456f},
		 {-0.676f, -1.324f,  2.0   },
		 { 2.0f,   -1.676f, -0.324f}},

		// bt601
		{{ 1.028f,  2.016f,  0.392f},
		 {-0.592f, -1.164f,  1.756f},
		 { 1.756f, -1.472f, -0.284f}},

		// bt709
		{{ 0.732f,  2.456f,  0.248f},
		 {-0.404f, -1.356f,  1.756f},
		 { 1.756f, -1.596f, -0.160f}},
	};
const int32_t     rgb_8bit_to_yuv_10bit_off[][3] =
	{
		{ 0, 512, 512 },	// full range
		{ 64, 512, 512 },	// bt601
		{ 64, 512, 512 },	// bt709
	};

const float       rgb_10bit_to_yuv_8bit_coef[][3][3] =
	{
		// full range
		{{ 0.075f,  0.147f,  0.029f },
		 {-0.042f, -0.083f,  0.125f },
		 { 0.125f, -0.105f, -0.020f}},

		// bt601
		{{ 0.064f,  0.126f,  0.025f },
		 {-0.037f, -0.073f,  0.110f },
		 { 0.110f, -0.092f,	-0.018  }},

		// bt709
		{{ 0.046f,  0.154f,  0.016f },
		 {-0.025f, -0.085f,  0.110f },
		 { 0.110f, -0.100f, -0.010f }},
	};
const int32_t     rgb_10bit_to_yuv_8bit_off[][3] =
	{
		{ 0,  128, 128 },	// full range
		{ 16, 128, 128 },	// bt601
		{ 16, 128, 128 },	// bt709
	};

const float       rgb_10bit_to_yuv_10bit_coef[][3][3] =
	{
		// Full range
		{{ 0.299f,  0.587f,  0.114f},
		 {-0.169f, -0.331f,  0.500f},
		 { 0.500f, -0.419f, -0.081f}},

		// bt601
		{{ 0.257f,  0.504f,  0.098f},
		 {-0.148f, -0.291f,  0.439f},
		 { 0.439f, -0.368f, -0.071f}},

		// bt709
		{{ 0.183f,  0.614f,  0.062f},
		 {-0.101f, -0.339f,  0.439f},
		 { 0.439f, -0.399f, -0.040f}},
	};

const int32_t     rgb_10bit_to_yuv_10bit_off[][3] =
	{
		{ 0,  512, 512 },	// full range
		{ 64, 512, 512 },	// bt601
		{ 64, 512, 512 },	// bt709
	};

//
//	YUV to RGB
//

const float       yuv_8bit_to_rgb_8bit_coef[][3][3] =
	{
		// full range
		{{ 1.0f,    0.0f,    1.4f  },
		 { 1.0f,   -0.343f, -0.711f},
		 { 1.0f,    1.765f,  0.0f  }},

		// bt601
		{{ 1.164f,  0.0f,    1.596f},
		 { 1.164f, -0.392f, -0.813f},
		 { 1.164f,  2.017f,  0.0f  }},

		// bt709
		{{ 1.164f,  0.0f,    1.793f},
		 { 1.164f, -0.213f, -0.533f},
		 { 1.164f,  2.112f,  0.0f  }},
	};

const int32_t     yuv_8bit_to_rgb_8bit_off[][3] =
	{
		{   0, -128, -128},	// full range
		{ -16, -128, -128},	// bt601
		{ -16, -128, -128}	// bt709
	};


const float       yuv_8bit_to_rgb_10bit_coef[][3][3] =
	{
		// full range
		{{ 4.0f,	0.0f,	 5.6f   },
		 { 4.0f,   -1.372f, -2.844f },
		 { 4.0f,    7.06f,   0.0f   }},
	
		// bt601
		{{ 4.656f,	0.0f,	 6.384f },
		 { 4.656f, -1.568f, -3.252f },
		 { 4.656f,	8.068f,	 0.0f   }},
			
		// bt709
		{{ 4.656f, 	0.0f,	 7.172f },
 		 { 4.656f, -0.852f, -2.132f },
		 { 4.656f,	8.448f,	 0.0f   }},
	};

const int32_t     yuv_8bit_to_rgb_10bit_off[][3] =
	{
		{   0, -128, -128},	// full range
		{ -16, -128, -128},	// bt601
		{ -16, -128, -128}	// bt709
	};


const float       yuv_10bit_to_rgb_8bit_coef[][3][3] =
	{
		// full range
		{{ 0.250f,  0.0f,    0.350f},
		 { 0.250f, -0.086f, -0.178f},
	 	 { 0.250f,  0.441f,  0.0f   }},

		// bt601
		{{ 0.291f,  0.0f,    0.399f},
		 { 0.291f, -0.098f, -0.203f},
		 { 0.291f,  0.504f,  0.0f  }},

		// bt709
		{{ 0.291f,  0.0f,    0.448f},
		 { 0.291f, -0.053f, -0.133f},
		 { 0.291f,  0.528f,  0.0f  }},
	};
const int32_t     yuv_10bit_to_rgb_8bit_off[][3] =
	{
		{   0, -512, -512 },
		{ -64, -512, -512 },
		{ -64, -512, -512 },
	};


const float       yuv_10bit_to_rgb_10bit_coef[][3][3] =
	{
		// full range
		{{ 1.0f,    0.0f,    1.4f  },
		 { 1.0f,   -0.343f, -0.711f},
		 { 1.0f,    1.765f,  0.0f  }},

		// bt601
		{{ 1.164f,  0.0f,    1.596f},
		 { 1.164f, -0.392f, -0.813f},
		 { 1.164f,  2.017f,  0.0f  }},

		// bt709
		{{ 1.164f,  0.0f,    1.793f},
		 { 1.164f, -0.213f, -0.533f},
		 { 1.164f,  2.112f,  0.0f  }},
	};


const int32_t     yuv_10bit_to_rgb_10bit_off[][3] =
	{
		{   0, -512, -512},	// full range
		{ -64, -512, -512},	// bt601
		{ -64, -512, -512}	// bt709
	};



/*
 *
 *  8 - B I T   L E F T   S H I F T E D
 *
 *	I N T E G E R   C O E F F I C I E N T S
 *
 */
const int32_t       rgb_8bit_to_yuv_8bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{ 77,   150,  29 },
		 {-43,  -85,   128},
		 { 128, -107, -21 }},

		// bt601
		{{ 66,   129,  25 },
		 {-38,  -74,   112},
		 { 112, -94,  -18 }},

		// bt709
		{{ 47,   157,  16 },
		 {-26,  -87,   112},
		 { 112, -102, -10 }},
	};

const int32_t       rgb_8bit_to_yuv_10bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{ 306,  601,  117},
		 {-173, -339,  512},
		 { 512, -429, -83}},

		// bt601
		{{ 263,  516,  100},
		 {-152, -298,  450},
		 { 450, -377, -73}},

		// bt709
		{{ 187,  629,  63 },
		 {-103, -347,  450},
		 { 450, -409, -41}},
	};

const int32_t       rgb_10bit_to_yuv_8bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{ 19,	 38,   7 },
		 {-11,  -21,  32 },
		 { 32,  -27,  -5 }},

		// bt601
		{{ 16,   32,   6 },
		  { -9, -19,  28 },
		  {	28,	-24,  -5 }},

		// bt709
		{{ 12,   39,   4 },
		 { -6,  -22,  28 },
		 { 28,  -26,  -3 }},
	};

const int32_t       rgb_10bit_to_yuv_10bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{  77,  150,  29 },
		 { -43, -85,  128 },
		 { 128, -107, -21 }},

		// bt601
		{{  66,	 129,  25 },
		 { -38,	 -74, 112 },
		 { 112,	 -94, -18 }},

		// bt709
		{{  47,	 157,  16 },
		 { -26,	-87,  112 },
		 { 112,	-102, -10 }},
	};

//
//	YUV to RGB
//
const int32_t       yuv_8bit_to_rgb_8bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{ 256,   0,  358},
		 { 256, -88, -182},
		 { 256, 452,    0}},

		// bt601
		{{ 298,    0,  409},
		 { 298, -100, -208},
		 { 298,  516,    0}},

		// bt709
		{{ 298,   0,  459},
		 { 298, -55, -136},
		 { 298, 541,    0}},
	};

const int32_t       yuv_10bit_to_rgb_8bit_coef_lhs8[][3][3] =
	{
		// full range
		{{ 64,   0,  90},
		 { 64, -22, -46},
		 { 64, 113,   0}},

		 // bt601
		{{ 74,   0,	102},
		 { 74, -25,	-52},
		 { 74, 129,	  0}},

		 // bt709
		{{ 74,   0, 115},
		 { 74, -14,	-34},
		 { 74, 135,   0}},
	};

const int32_t       yuv_8bit_to_rgb_10bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{ 1024,	 0,		 1434 },
		 { 1024,	-351,	-728  },
		 { 1024,	 1807,	 0    }},		
		// bt601
		{{ 1192,	 0,		 1634 },
		 { 1192,	-401,	-833  },
		 { 1192,	 2065,	 0    }},		
		// bt709
		{{ 1192,	 0,		 1836 },
		 { 1192,	-218,	-546  },
		 { 1192,	 2163,	 0    }}
	};

const int32_t       yuv_10bit_to_rgb_10bit_coef_lhs8[][3][3] =
	{
		// Full range
		{{ 256,   0,  358},
		 { 256, -88, -182},
		 { 256, 452,    0}},

		// bt601
		{{ 298,    0,  409},
		 { 298, -100, -208},
		 { 298,  516,    0}},

		// bt709
		{{ 298,   0,  459},
		 { 298, -55, -136},
		 { 298, 541,    0}},
	};


// CPU features are obtained the first time and stored here.
static uint64_t		cpu_features = 0;

// Check if the CPU supports some features passed as a mask of CPUID_FEATURE_* macros
uint32_t			does_cpu_support(uint64_t mask) {
	return ((mask != CPUID_FEATURE_NONE) && ((get_cpu_features() & mask) != mask)) ? -1 : 0;
}

// Retrieve the features supported by the CPU
uint64_t		get_cpu_features() {
	uint32_t	features[4]= {0};	// CPU feature Lo in [3], Hi in [2]

#ifdef __INTEL_CPU__
	// Return cached value if valid
	if (cpu_features != 0)
		return cpu_features;

#ifdef WIN32
	__cpuid(features, 1);	// ask for CPU features
#else
	uint32_t 	eax = 1;	// ask for CPU features

	__asm__ __volatile__ (
			// After a CPUID, eax, ebx, ecx and edx are clobbered.
			// We only collect the CPU features stored in ecx and edx.
			// As we may be compiled with -fPIC which uses ebx on
			// Linux x86, we need to save it before the CPUID and
			// restore it afterwards.
#if ! defined(__x86_64__)
			"pushl %%ebx;\n"
#endif
			"cpuid;\n"
#if ! defined(__x86_64__)
			"popl %%ebx;"
#endif
			: "=d" (features[3]), "=c" (features[2])
			  : "a" (eax)
#if defined(__x86_64__)
			    : "%ebx"
#endif
	);
#endif
	
	// Cache value for next time.
	cpu_features = (uint64_t) (((uint64_t)features[2]) << 32 | (uint64_t)  features[3]);
#endif	// __INTEL_CPU__

	return cpu_features;
}

#ifdef FAKE_SSE41_BLENDV
#define _mm_blendv_epi8 _fake_mm_blendv_epi8
__m128i     _fake_mm_blendv_epi8(__m128i v1, __m128i v2, __m128i mask) {
    char *m = (char *)&mask;
    __m128i result = {0x0LL, 0x0LL};
    char *dest = (char*) &result;
    char *s1 = (char*)&v1;
    char *s2 = (char*)&v2;
    int i = 0;

    for (i = 0; i < 16; i++) {
        if (m[i] & 0xFF)
            dest[i] = s2[i];
        else
            dest[i] = s1[i];
    }

    return result;
}
#endif


#ifdef DEBUG
#if 1
void print_xmm32(char *name, __m128i *reg) {
	// print register as 32bit signed
	int32_t*	array = (int32_t *)reg;
	printf("%s:\n %9d %9d %9d %9d\n", name,
			array[0], array[1], array[2], array[3]);
}

void print_xmm32x(char *name, __m128i *reg) {
	// print register as 32bit hex
	int32_t*	array = (int32_t *)reg;
	printf("%s:\n %08x %08x %08x %08x\n", name,
		   array[0], array[1], array[2], array[3]);
}

void print_xmm16(char *name, __m128i *reg) {
	// print register as 16bit signed
	short*	array = (short *)reg;
	printf("%s:\n %4hd %4hd %4hd %4hd %4hd %4hd %4hd %4hd\n", name,
			array[0], array[1], array[2], array[3],
			array[4], array[5], array[6], array[7]);
}

void print_xmm16u(char *name, __m128i *reg) {
	// print register as 16bit signed
	short*	array = (short *)reg;
	printf("%s:\n %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu\n", name,
			array[0], array[1], array[2], array[3],
			array[4], array[5], array[6], array[7]);
}

void print_xmm16x(char *name, __m128i *reg) {
	// print register as 16bit hex
	short*	array = (short *)reg;
	printf("%s:\n %4hx %4hx %4hx %4hx %4hx %4hx %4hx %4hx\n", name,
		   array[0], array[1], array[2], array[3],
		   array[4], array[5], array[6], array[7]);
}

void print_xmm10u(char *name, __m128i *reg) {
	// print register as 10bit unsigned
	uint32_t*	array = (uint32_t *)reg;
	printf("%s:\n %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu\n", name,
			(unsigned short) (array[0] & 0x3FF),
			(unsigned short) ((array[0] >> 10) & 0x3FF),
			(unsigned short) ((array[0] >> 20) & 0x3FF),
			(unsigned short) (array[1] & 0x3FF),
			(unsigned short) ((array[1] >> 10) & 0x3FF),
			(unsigned short) ((array[1] >> 20) & 0x3FF),
			(unsigned short) (array[2] & 0x3FF),
			(unsigned short) ((array[2] >> 10) & 0x3FF),
			(unsigned short) ((array[2] >> 20) & 0x3FF),
			(unsigned short) (array[3] & 0x3FF),
			(unsigned short) ((array[3] >> 10) & 0x3FF),
			(unsigned short) ((array[3] >> 20) & 0x3FF)
		);
}

void print_xmm8u(char *name, __m128i *reg) {
	// print register as 8bit signed
	char*	array = (char *)reg;
	printf("%s:\n %4hhu %4hhu %4hhu %4hhu %4hhu %4hhu %4hhu %4hhu "
			"%4hhu %4hhu %4hhu %4hhu %4hhu %4hhu %4hhu %4hhu\n", name,
			array[0], array[1], array[2], array[3],
			array[4], array[5], array[6], array[7],
			array[8], array[9], array[10], array[11],
			array[12], array[13], array[14], array[15]);
}

void print_xmm8x(char *name, __m128i *reg) {
	// print register as 8bit signed
	char*	array = (char *)reg;
	printf("%s:\n %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx "
			"%02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx %02hhx\n", name,
			array[0], array[1], array[2], array[3],
			array[4], array[5], array[6], array[7],
			array[8], array[9], array[10], array[11],
			array[12], array[13], array[14], array[15]);
}
#else
void print_xmm32(char *name, __m128i *reg) {}
void print_xmm32x(char *name, __m128i *reg) {}
void print_xmm16(char *name, __m128i *reg) {}
void print_xmm16u(char *name, __m128i *reg) {}
void print_xmm16x(char *name, __m128i *reg) {}
void print_xmm10u(char *name, __m128i *reg) {}
void print_xmm8u(char *name, __m128i *reg) {}
void print_xmm8x(char *name, __m128i *reg) {}
#endif

#endif





