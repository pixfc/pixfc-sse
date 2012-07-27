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

// CPU features are obtained the first time and stored here.
static uint64_t		cpu_features = 0;

// Check if the CPU supports some features passed as a mask of CPUID_FEATURE_* macros
uint32_t			does_cpu_support(uint64_t mask) {
	return ((mask != CPUID_FEATURE_NONE) && ((get_cpu_features() & mask) != mask)) ? -1 : 0;
}

// Retrieve the features supported by the CPU
INLINE uint64_t		get_cpu_features() {
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
	// print register as 16bit signed
	int32_t*	array = (int32_t *)reg;
	printf("%s:\n %9d %9d %9d %9d\n", name,
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
void print_xmm16(char *name, __m128i *reg) {}
void print_xmm16u(char *name, __m128i *reg) {}
void print_xmm10u(char *name, __m128i *reg) {}
void print_xmm8u(char *name, __m128i *reg) {}
void print_xmm8x(char *name, __m128i *reg) {}
#endif

#endif





