/*
 * inline-testing-common.h
 *
 * Copyright (C) 2012 PixFC Team (pixelfc@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public  License as published by the
 * Free Software Foundation, either version 3 of the License, or
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


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "platform_util.h"
#include "common.h"

#define MAX_DIFF_8BIT		2
#define MAX_DIFF_10BIT		3
#define MAX_DIFF_PACKING	0
#define MAX_DIFF_UNPACKING	0



// 0		15		16		235		236		240		241     255
#define DECLARE_1_8BIT_VECT(var)	M128I_ARRAY_VAL(var, 1, { 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL } )

// 0		1		2		15		16		17		127		128
// 129		234		235		236		240		241		254		255
#define DECLARE_2_8BIT_VECT(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x000F000200010000LL, 0x0080007F00110010LL }, \
	                                                    { 0x00EC00EB00EA0081LL, 0x00FF00FE00F100F0LL } )

// 0		1		2		15		16		17		127		128
// 0		0		15		15		16		16		235		235
#define DECLARE_1_Y_UV_8BIT_VECT1(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x000F000200010000LL, 0x0080007F00110010LL }, \
														{ 0x000F000F00000000LL, 0x00EB00EB00100010LL } )

// 129		234		235		236		240		241		254		255
// 236		236		240		240		241     241		255		255
#define DECLARE_1_Y_UV_8BIT_VECT2(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x00EC00EB00EA0081LL, 0x00FF00FE00F100F0LL }, \
														{ 0x00F000F000EC00ECLL, 0x00FF00FF00F100F1LL } )

// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
#define DECLARE_3_8BIT_VECT(var)	M128I_ARRAY_VAL(var, 3, \
														{ 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, \
														{ 0x00EB0010000F0001LL, 0x00FF00F100F000ECLL}, \
														{ 0x00EB0010000F0002LL, 0x00FF00F100F000ECLL}, )

// 0		1		2		15		16		17		127		128
// 0		1		2		15		16		17		127		128
// 129		234		235		236		240		241		254		255
// 129		234		235		236		240		241		254		255
#define DECLARE_4_8BIT_VECT(var)	M128I_ARRAY_VAL(var, 4, \
														{ 0x000F000200010000LL, 0x0080007F00110010LL }, \
		                                                { 0x000F000200010000LL, 0x0080007F00110010LL }, \
	                                                    { 0x00EC00EB00EA0081LL, 0x00FF00FE00F100F0LL }, \
														{ 0x00EC00EB00EA0081LL, 0x00FF00FE00F100F0LL },)

// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
// 0		15		16		235		236		240		241     255
#define DECLARE_6_8BIT_VECT(var)	M128I_ARRAY_VAL(var, 6,\
														{ 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, \
										    			{ 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, \
										    			{ 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, \
                                                        { 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, \
												    	{ 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, \
													    { 0x00EB0010000F0000LL, 0x00FF00F100F000ECLL}, )



// 0		63		64		940		941		960		961     1023
#define DECLARE_1_10BIT_VECT(var)	M128I_ARRAY_VAL(var, 1, { 0x03AC0040003F0000LL, 0x03FF03C103C003ADLL })

// 0		1		2		63		64		65		511		512
// 513		939		940		941		960		961		1022	1023
#define DECLARE_2_10BIT_VECT(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x003F000200010000LL, 0x020001FF00410040LL }, \
														{ 0x03AD03AC03AB0201LL, 0x03FF03FE03C103C0LL }, )

// 0, 1, 2, /**/ 63,64,65 /**/, 127,128,129 /**/,  511, 512, 513 /**/
// 840, 841, 842, /**/ 939, 940, 941, /**/ 960, 961, 962, /**/ 1021, 1022, 1023
#define DECLARE_2_RGB_10BIT_VECT(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x3F00110400042000LL, 0xFF0118207F001208LL }, \
														{ 0xABB3DE3A4827AD34LL, 0xFDFBFF3FC0072F3CLL }, )

// 0		1		2		63		64		65		511		512
// 0		0		63		63		64		64		940		940
#define DECLARE_1_Y_UV_10BIT_VECT1(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x003F000200010000LL, 0x020001FF00410040LL }, \
														{ 0x003F003F00000000LL, 0x03AC03AC00400040LL }, )

// 513		939		940		941		960		961		1022	1023
// 941		641		960		960		961     961		1023	1023
#define DECLARE_1_Y_UV_10BIT_VECT2(var)	M128I_ARRAY_VAL(var, 2, \
														{ 0x03AD03AC03AB0201LL, 0x03FF03FE03C103C0LL }, \
														{ 0x03C003C003AD03ADLL, 0x03FF03FF03C103C1LL }, )


// 0		61		64		940		941		960		961     1021
// 1		62		65		940		941		960		961     1022
// 2		63		66		940		941		960		961     1023
#define DECLARE_3_10BIT_VECT(var)	M128I_ARRAY_VAL(var, 3, \
														{ 0x03AC0040003D0000LL, 0x03FD03C103C003ADLL },\
														{ 0x03AC0041003E0001LL, 0x03FE03C103C003ADLL },\
														{ 0x03AC0042003F0002LL, 0x03FF03C103C003ADLL }, )

// 0		1		2		63		64		65		511		512
// 0		1		2		63		64		65		511		512
// 513		939		940		941		960		961		1022	1023
// 513		939		940		941		960		961		1022	1023
#define DECLARE_4_10BIT_VECT(var)	M128I_ARRAY_VAL(var, 4,\
														{ 0x003F000200010000LL, 0x020001FF00410040LL }, \
														{ 0x003F000200010000LL, 0x020001FF00410040LL }, \
														{ 0x03AD03AC03AB0201LL, 0x03FF03FE03C103C0LL }, \
														{ 0x03AD03AC03AB0201LL, 0x03FF03FE03C103C0LL }, )
 
// 0		61		64		940		941		960		961     1021
// 1		62		65		940		941		960		961     1022
// 2		63		66		940		941		960		961     1023
// 0		61		64		940		941		960		961     1021
// 1		62		65		940		941		960		961     1022
// 2		63		66		940		941		960		961     1023
#define DECLARE_6_10BIT_VECT(var)	M128I_ARRAY_VAL(var, 6, \
														{ 0x03AC0040003D0000LL, 0x03FD03C103C003ADLL },\
														{ 0x03AC0041003E0001LL, 0x03FE03C103C003ADLL },\
														{ 0x03AC0042003F0002LL, 0x03FF03C103C003ADLL },\
														{ 0x03AC0040003D0000LL, 0x03FD03C103C003ADLL },\
														{ 0x03AC0041003E0001LL, 0x03FE03C103C003ADLL },\
														{ 0x03AC0042003F0002LL, 0x03FF03C103C003ADLL }, )



#ifndef WIN32
#define dprintf(fmt, ...)  do { fprintf (stderr, "[ %s:%-3d ] " fmt,\
                                strrchr(__FILE__, '/')+1,\
                                __LINE__, ## __VA_ARGS__);\
                                fflush(stderr); } while(0)
#else
#define dprintf(fmt, ...)  do { fprintf (stderr, "[ %s:%-3d ] " fmt,\
                                strrchr(__FILE__, '\\')+1,\
                                __LINE__, ## __VA_ARGS__);\
                                fflush(stderr); } while(0)

#endif

#ifdef DEBUG
	void	print_xmm8u_array(uint32_t count, char *prefix, void *array);
	void	print_xmm10leu_array(uint32_t count, char *prefix, void *array);
	void	print_xmm10beu_array(uint32_t count, char *prefix, void *array);
	void	print_xmm16u_array(uint32_t count, char *prefix, void *array);
	void	print_xmm16_array(uint32_t count, char *prefix, void *array);
#else
	#define print_xmm8u_array(...)
	#define print_xmm10leu_array(...)
	#define print_xmm10beu_array(...)
	#define print_xmm16u_array(...)
	#define print_xmm16_array(...)
#endif

/*
 * check_last specifies how many of the last values should be check. If check_last is negative, then check from start. If 0, check all values.
 */
void	compare_8bit_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t output_count, uint32_t max_diff, char *prefix);
void	compare_10bit_be_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t output_count, uint32_t max_diff, char *prefix);
void	compare_10bit_le_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t output_count, uint32_t max_diff, char *prefix);
void	compare_16bit_output(int8_t check_last, void *scalar_out, void *sse_out, uint8_t output_count, uint32_t max_diff, char *prefix);

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define CHECK_INLINE_1IN_3OUT(inline_scalar, inline_sse, input_type, output_count, max_diff, compare_fn, check_last1, check_last23)\
	do {\
        input_type(input);\
        __m128i scalar_out[3 * output_count] = {{0,0}}; /* assume each output vector has the same size */\
        __m128i sse_out[3 * output_count] = {{0,0}};\
        dprintf("Checking " #inline_sse "\n");\
        /*print_xmm16u_array(ARRAY_SIZE(input), "INPUT", input); */\
		inline_scalar(_AM(input[0]), &scalar_out[0], &scalar_out[output_count], &scalar_out[2 * output_count]);\
        /* print_xmm16u_array(ARRAY_SIZE(scalar_out), "SCALAR OUT", scalar_out); */\
		inline_sse(_AM(input[0]), &sse_out[0], &sse_out[output_count], &sse_out[2 * output_count]);\
        /* print_xmm16u_array(ARRAY_SIZE(sse_out), "SSE OUT", sse_out); */\
		compare_fn(check_last1, scalar_out, sse_out, output_count, max_diff, #inline_sse " output vect 1");\
        compare_fn(check_last23, &scalar_out[output_count], &sse_out[output_count], output_count, max_diff, #inline_sse " output vect 2");\
        compare_fn(check_last23, &scalar_out[2 * output_count], &sse_out[2 * output_count], output_count, max_diff, #inline_sse " output vect 3");\
	} while (0)

#define CHECK_INLINE_1IN_3OUT2(inline_scalar, inline_sse, input_type, output_count1, output_count23, max_diff, compare_fn, check_last1, check_last23)\
	do {\
        input_type(input);\
        __m128i scalar_out1[output_count1] = {{0,0}};\
        __m128i scalar_out2[output_count23] = {{0,0}};\
        __m128i scalar_out3[output_count23] = {{0,0}};\
        __m128i sse_out1[output_count1] = {{0,0}};\
        __m128i sse_out2[output_count23] = {{0,0}};\
        __m128i sse_out3[output_count23] = {{0,0}};\
        dprintf("Checking " #inline_sse "\n");\
        /*print_xmm16u_array(ARRAY_SIZE(input), "INPUT", input); */\
		inline_scalar(_AM(input[0]), scalar_out1, scalar_out2, scalar_out3);\
        /*print_xmm16u_array(ARRAY_SIZE(scalar_out), "SCALAR OUT", scalar_out); */\
		inline_sse(_AM(input[0]), sse_out1, sse_out2, sse_out3);\
        /*print_xmm16u_array(ARRAY_SIZE(sse_out), "SSE OUT", sse_out); */\
		compare_fn(check_last1, scalar_out1, sse_out1, output_count1, max_diff, #inline_sse " output vect 1");\
        compare_fn(check_last23, scalar_out2, sse_out2, output_count23, max_diff, #inline_sse " output vect 2");\
        compare_fn(check_last23, scalar_out3, sse_out3, output_count23, max_diff, #inline_sse " output vect 3");\
	} while (0)



#define CHECK_INLINE_1IN_2OUT(inline_scalar, inline_sse, input_type, output_count, max_diff, compare_fn, check_last1, check_last2)\
	do {\
        input_type(input);\
        __m128i scalar_out[2 * output_count] = {{0,0}}; /* assume each output vector has the same size */\
        __m128i sse_out[2 * output_count] = {{0,0}};\
        dprintf("Checking " #inline_sse "\n");\
        /*print_xmm16u_array(ARRAY_SIZE(input[0]), "INPUT", _AM(input[0])); */\
		inline_scalar(_AM(input[0]), &scalar_out[0], &scalar_out[output_count]);\
        /*print_xmm16u_array(ARRAY_SIZE(scalar_out), "SCALAR OUT", scalar_out); */\
		inline_sse(_AM(input[0]), &sse_out[0], &sse_out[output_count]);\
        /*print_xmm16u_array(ARRAY_SIZE(sse_out), "SSE OUT", sse_out); */\
		compare_fn(check_last1, scalar_out, sse_out, output_count, max_diff, #inline_sse " output vect 1");\
        compare_fn(check_last2, &scalar_out[output_count], &sse_out[output_count], output_count, max_diff, #inline_sse " output vect 2");\
	} while (0)



#define CHECK_INLINE_1IN(inline_scalar, inline_sse, input_type, output_count, max_diff, compare_fn)\
	do {\
        input_type(input);\
        __m128i scalar_out[output_count];\
        __m128i sse_out[output_count];\
		dprintf("Checking " #inline_sse "\n");\
        print_xmm16u_array(ARRAY_SIZE(input), "INPUT", _AM(input[0]));\
		inline_scalar(_AM(input[0]), scalar_out);\
        print_xmm16_array(ARRAY_SIZE(scalar_out), "SCALAR OUT", scalar_out);\
		inline_sse(_AM(input[0]), sse_out);\
        print_xmm16_array(ARRAY_SIZE(sse_out), "SSE OUT", sse_out);\
		compare_fn(0, scalar_out, sse_out, output_count, max_diff, #inline_sse);\
	} while (0)


#define	 CHECK_SSE2_SSSE3_INLINE_1IN(inline_prefix, input_type, output_count, max_diff, compare_fn)	\
    do {\
    	CHECK_INLINE_1IN(inline_prefix ## scalar, inline_prefix ## sse2, input_type, output_count, max_diff, compare_fn);\
        CHECK_INLINE_1IN(inline_prefix ## scalar, inline_prefix ## sse2_ssse3, input_type, output_count, max_diff, compare_fn);\
    } while(0)


#define DO_CHECK_INLINE_1IN_1PREV(inline_scalar, inline_sse, input_type, prev_input_type, output_count, max_diff, compare_fn, check_last)\
	do {\
        prev_input_type(scalar_prev_input);\
        prev_input_type(sse_prev_input);\
        input_type(input);\
       __m128i sse_out[output_count];\
        __m128i scalar_out[output_count];\
		dprintf("Checking " #inline_sse "\n");\
		/* print_xmm16u_array(ARRAY_SIZE(input), "INPUT SCALAR ", _AM(input[0])); \
		print_xmm16u_array(ARRAY_SIZE(scalar_prev_input), "SCALAR_PREV_INPUT", _AM(scalar_prev_input[0])); */\
		inline_scalar(_AM(input[0]), _AM(scalar_prev_input[0]), scalar_out);\
		/* print_xmm16u_array(ARRAY_SIZE(scalar_prev_input), "SCALAR_PREV_INPUT", _AM(scalar_prev_input[0]));\
		print_xmm16u_array(output_count, "SCALAR_OUTPUT", scalar_out);\
		print_xmm16u_array(ARRAY_SIZE(input[0]), "INPUT SSE ", _AM(input[0]));\
		print_xmm16u_array(ARRAY_SIZE(sse_prev_input[0]), "SSE_PREV_INPUT", _AM(sse_prev_input[0])); */\
		inline_sse(_AM(input[0]), _AM(sse_prev_input[0]), sse_out);\
		/*print_xmm16u_array(ARRAY_SIZE(sse_prev_input[0]), "SSE_PREV_INPUT", _AM(sse_prev_input[0]));\
		print_xmm16u_array(output_count, "SSE_OUT", sse_out); */\
		compare_fn(0, scalar_out, sse_out, output_count, max_diff, #inline_sse);\
		compare_fn(check_last, _AM(scalar_prev_input[0]), _AM(sse_prev_input[0]), ARRAY_SIZE(sse_prev_input), 0, #inline_sse);\
    } while (0)

#define	 CHECK_SSE2_SSSE3_INLINE_1IN_1PREV(inline_prefix, input_type, prev_input_type, output_count, max_diff, compare_fn, check_last)	\
	do {\
        DO_CHECK_INLINE_1IN_1PREV(inline_prefix ## scalar, inline_prefix ## sse2, input_type, prev_input_type, output_count, max_diff, compare_fn, check_last);\
        DO_CHECK_INLINE_1IN_1PREV(inline_prefix ## scalar, inline_prefix ## sse2_ssse3, input_type, prev_input_type, output_count, max_diff, compare_fn, check_last);\
    }while(0)

#define DO_CHECK_INLINE_2IN(inline_scalar, inline_sse, input_type, output_count, max_diff, compare_fn)\
	do {\
        input_type(input1);\
        input_type(input2);\
        __m128i scalar_out[output_count];\
        __m128i sse_out[output_count];\
        dprintf("Checking " #inline_sse "\n");\
		/* print_xmm16u_array(ARRAY_SIZE(input1[0]), "INPUT1 SCALAR ", _AM(input1[0]));\
		print_xmm16u_array(ARRAY_SIZE(input2[0]), "INPUT2 SCALAR", _AM(input2[0])); */\
		inline_scalar(_AM(input1[0]), _AM(input2[0]), scalar_out);\
		/* print_xmm16u_array(output_count, "SCALAR_OUTPUT", scalar_out);\
        print_xmm16u_array(ARRAY_SIZE(input1[0]), "INPUT1 SCALAR ", _AM(input1[0]));\
		print_xmm16u_array(ARRAY_SIZE(input2[0]), "INPUT2 SCALAR", _AM(input2[0])); */\
		inline_sse(_AM(input1[0]), _AM(input2[0]), sse_out);\
		/* print_xmm16u_array(output_count, "SSE_OUT", sse_out); */\
		compare_fn(0, scalar_out, sse_out, output_count, max_diff, #inline_sse);\
    } while (0)

#define	 CHECK_SSE2_SSSE3_INLINE_2IN(inline_prefix, input_type, output_count, max_diff, compare_fn)	\
    do{\
        DO_CHECK_INLINE_2IN(inline_prefix ## scalar, inline_prefix ## sse2, input_type, output_count, max_diff, compare_fn);\
        DO_CHECK_INLINE_2IN(inline_prefix ## scalar, inline_prefix ## sse2_ssse3, input_type, output_count, max_diff, compare_fn);\
    } while(0)

#define DO_CHECK_INLINE_2IN_2OUT(inline_scalar, inline_sse, input_type, output_count, max_diff, compare_fn)\
	do {\
        input_type(input1);\
        input_type(input2);\
        __m128i scalar_out[2 * output_count] = {{0x0LL, 0x0LL}};\
        __m128i sse_out[2 * output_count] = {{0x0LL, 0x0LL}};\
        dprintf("Checking " #inline_sse "\n");\
		/* print_xmm8u_array(ARRAY_SIZE(input1[0]), "INPUT1 SCALAR ", _AM(input1[0]));\
		print_xmm8u_array(ARRAY_SIZE(input2[0]), "INPUT2 SCALAR", _AM(input2[0])); */\
		inline_scalar(_AM(input1[0]), _AM(input2[0]), &scalar_out[0], &scalar_out[output_count]);\
		/* print_xmm16u_array(output_count, "SCALAR_OUTPUT vect 1", scalar_out);\
        print_xmm16u_array(output_count, "SCALAR_OUTPUT vect 2", &scalar_out[output_count]);\
        print_xmm16u_array(ARRAY_SIZE(input1[0]), "INPUT1 SCALAR ", _AM(input1[0]));\
		print_xmm16u_array(ARRAY_SIZE(input2[0]), "INPUT2 SCALAR", _AM(input2[0])); */\
		inline_sse(_AM(input1[0]), _AM(input2[0]), &sse_out[0], &sse_out[output_count]);\
		/* print_xmm16u_array(output_count, "SSE_OUT vect 1", sse_out);\
        print_xmm16u_array(output_count, "SSE_OUT vect 2", &sse_out[output_count]); */\
		compare_fn(0, scalar_out, sse_out, output_count, max_diff, #inline_sse " output vect 1");\
        compare_fn(0, &scalar_out[output_count], &sse_out[output_count], output_count, max_diff, #inline_sse "output vect 2");\
    } while (0)



#define DO_CHECK_INLINE_3IN(inline_scalar, inline_sse, input1_type, input2_type, input3_type, output_count, max_diff, compare_fn)\
	do {\
        input1_type(input1);\
        input2_type(input2);\
        input3_type(input3);\
        __m128i scalar_out[output_count];\
        __m128i sse_out[output_count];\
        dprintf("Checking " #inline_sse "\n");\
		/* print_xmm16u_array(ARRAY_SIZE(input1[0]), "INPUT1 SCALAR ", _AM(input1[0]));\
		print_xmm16u_array(ARRAY_SIZE(input2[0]), "INPUT2 SCALAR", _AM(input2[0])); */\
		inline_scalar(_AM(input1[0]), _AM(input2[0]), _AM(input3[0]), scalar_out);\
		/* print_xmm16u_array(output_count, "SCALAR_OUTPUT", scalar_out);\
        print_xmm16u_array(ARRAY_SIZE(input1[0]), "INPUT1 SCALAR ", _AM(input1[0]));\
		print_xmm16u_array(ARRAY_SIZE(input2[0]), "INPUT2 SCALAR", _AM(input2[0])); */\
		inline_sse(_AM(input1[0]), _AM(input2[0]), _AM(input3[0]), sse_out);\
		/* print_xmm16u_array(output_count, "SSE_OUT", sse_out); */\
		compare_fn(0, scalar_out, sse_out, output_count, max_diff, #inline_sse);\
    } while (0)



