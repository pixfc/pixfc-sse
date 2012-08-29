/*
 * conversion_routines_from_r210.c
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

#include "common.h"
#include "pixfc-sse.h"
#include "rgb_conversion_recipes.h"

/*
 *
 * 		R 2 1 0
 *
 * 		T O
 *
 * 		R G B 3 2  /  2 4
 *
 */
void		convert_r210_to_argb_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_4_argb_vectors_sse2, 4);
}

void		convert_r210_to_bgra_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_4_bgra_vectors_sse2, 4);
}

void		convert_r210_to_rgb24_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_3_rgb24_vectors_sse2_ssse3, 3);
}

void		convert_r210_to_bgr24_sse2_ssse3(const struct PixFcSSE *pixfc, void* source_buffer, void* dest_buffer) {
	DO_REPACK2(R210_TO_RGB_RECIPE, unpack_4_r210_to_r_g_b_vectors_sse2_ssse3, pack_6_rgb_vectors_in_3_bgr24_vectors_sse2_ssse3, 3);
}

