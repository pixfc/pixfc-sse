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

#include <time.h>
#include <stdint.h>
#ifdef WIN32
	#include <windows.h>
	#include <intrin.h>
#else
	#include <sys/resource.h>
	#include <sys/time.h>

	#ifdef __APPLE__
		#include <mach/mach.h>
		#include <mach/clock.h>
		#include <mach/mach_time.h>
	#endif
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
	
	return cpu_features;
}

// Return the amount of ticks in nanoseconds elapsed since startup
ticks		getticks() {
#ifdef __linux__

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ticks) (ts.tv_sec * 1000000000LL + (ticks)ts.tv_nsec);
	// the following returns ticks as a number of clock cycles
	//	unsigned long a, d;
	//	__asm__ __volatile__ (
	//			"xor %%eax, %%eax;\n"
	//			"rdtsc;\n"
	//			: "=a" (a), "=d" (d)
	//			  :	// no input
	//			  : "%ebx", "%ecx");
	//
	//	return (ticks) (((ticks)a) | ( ((ticks)d) << 32ULL ) );

#elif defined(__APPLE__)

	static clock_serv_t		clock_server;
	static uint32_t			valid_clock_server = 0;
	static uint32_t			has_asked_for_clock_server = 0;
	ticks					result = 0;

	// Get clock server if not already done
	if (has_asked_for_clock_server == 0) {
		if (host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &clock_server) == KERN_SUCCESS)
			valid_clock_server = 1;
		else
			dprintf("Error getting clock server\n");

		has_asked_for_clock_server = 1;
	}

	// Get time
	if (valid_clock_server == 1) {
		mach_timespec_t ts;
		if (clock_get_time(clock_server, &ts) == KERN_SUCCESS)
			result = ts.tv_sec * 1000000000LL + (ticks) ts.tv_nsec;
		else 
			dprintf("Error getting time\n");
	}
	return result;

#else	// WIN32

	static LARGE_INTEGER	frequency = { 0 };
	static uint32_t			has_asked_for_frequency = 0;
	LARGE_INTEGER			counter;
	uint64_t				result = 0;

	if (has_asked_for_frequency == 0) {
		if (QueryPerformanceFrequency(&frequency) != TRUE) {
			dprintf("Error getting timer frequency\n");
			frequency.QuadPart = 0;
		}
		has_asked_for_frequency = 1;
	}

	if ((frequency.QuadPart != 0) && (QueryPerformanceCounter(&counter) == TRUE))
		result = (uint64_t)(counter.QuadPart * 1000000000LL/ frequency.QuadPart);

	return result;
#endif
}

// Not thread safe !!
#ifndef WIN32
static struct rusage            last_rusage;
#endif

static ticks					last_ticks;
#define TV_TO_NS(tv)			((tv).tv_sec * 1000000000ULL + (uint64_t)(tv).tv_usec * 1000)

void			do_timing(struct timings *timings) {
	if (! timings) {
#ifndef WIN32
		getrusage(RUSAGE_SELF, &last_rusage);
#endif
		last_ticks = getticks();
	} else {
		timings->total_time_ns += getticks() - last_ticks;
#ifndef WIN32
		struct rusage   now;

		if (getrusage(RUSAGE_SELF, &now) == 0) {
			struct timeval user_time;
			struct timeval sys_time;

			timersub(&now.ru_stime, &last_rusage.ru_stime, &sys_time);
			timersub(&now.ru_utime, &last_rusage.ru_utime, &user_time);

			timings->user_time_ns += TV_TO_NS(user_time);
			timings->sys_time_ns += TV_TO_NS(sys_time);
			timings->vcs += (now.ru_nvcsw - last_rusage.ru_nvcsw);
			timings->ivcs += (now.ru_nivcsw - last_rusage.ru_nivcsw);
		} else
			dprintf("Error calling getrusage()\n");
#endif
	}
}




#ifdef DEBUG

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

#endif




