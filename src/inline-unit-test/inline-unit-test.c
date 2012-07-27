/*
 * inline-unit-test.c
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

int main(int argc, char **argv) {
/*
	// RGB downsample
	check_nnb_422_downsample_ag_rb_vectors();
	check_nnb_422_downsample_r_g_b_vectors();
	check_avg_422_downsample_ag_rb_vectors_n_save_previous();
	check_avg_422_downsample_r_g_b_vectors_n_save_previous();
    check_avg_422_downsample_first_r_g_b_vectors_n_save_previous();
    check_avg_422_downsample_first_ag_rb_vectors_n_save_previous();
    check_avg_420_downsample_r_g_b_vectors();
    check_avg_420_downsample_ag_rb_vectors();

    // RGB pack
    check_pack_6_rgb_vectors_in_4_bgra_vectors();
    check_pack_6_rgb_vectors_in_4_argb_vectors();
    check_6_rgb_vectors_in_3_rgb24_vectors();
    check_6_rgb_vectors_in_3_bgr24_vectors();

    // RGB unpack
    check_unpack_argb_to_ag_rb_vectors();
    check_unpack_argb_to_r_g_b_vectors();
    check_unpack_bgra_to_ga_br_vectors();
    check_unpack_bgra_to_r_g_b_vectors();
    check_unpack_rgb24_to_ag_rb_vectors();
    check_unpack_rgb24_to_r_g_b_vectors();
    check_unpack_bgr24_to_ga_br_vectors();
    check_unpack_bgr24_to_r_g_b_vectors();

    // RGB to YUV convert
    check_convert_r_g_b_vectors_to_y_vector();
    check_convert_r_g_b_vectors_to_y_vector_bt601();
    check_convert_r_g_b_vectors_to_y_vector_bt709();

    check_convert_8bit_r_g_b_vectors_to_10bit_y_vector();
    check_convert_8bit_r_g_b_vectors_to_10bit_y_vector_bt601();
    check_convert_8bit_r_g_b_vectors_to_10bit_y_vector_bt709();

    check_convert_downsampled_422_r_g_b_vectors_to_uv_vector();
    check_convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt601();
    check_convert_downsampled_422_r_g_b_vectors_to_uv_vector_bt709();
    
    check_convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector();
    check_convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_bt601();
    check_convert_downsampled_422_8bit_r_g_b_vectors_to_10bit_uv_vector_bt709();

    check_convert_ag_rb_vectors_to_y_vector();
    check_convert_ga_br_vectors_to_y_vector();
    check_convert_ag_rb_vectors_to_y_vector_bt601();
    check_convert_ga_br_vectors_to_y_vector_bt601();
    check_convert_ag_rb_vectors_to_y_vector_bt709();
    check_convert_ga_br_vectors_to_y_vector_bt709();

    check_convert_downsampled_422_ag_rb_vectors_to_uv_vector();
    check_convert_downsampled_422_ga_br_vectors_to_uv_vector();
    check_convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt601();
    check_convert_downsampled_422_ga_br_vectors_to_uv_vector_bt601();
    check_convert_downsampled_422_ag_rb_vectors_to_uv_vector_bt709();
    check_convert_downsampled_422_ga_br_vectors_to_uv_vector_bt709();


    //
    // YUV pack
    check_pack_4_y_uv_422_vectors_in_2_yuyv_vectors();
    check_pack_4_y_uv_422_vectors_in_2_uyvy_vectors();
    check_pack_4_y_uv_422_vectors_in_yuvp_lo_vectors();
    check_pack_4_y_uv_422_vectors_in_yuvp_hi_vectors();
    check_pack_2_y_vectors_to_1_y_vector();
    check_pack_4_uv_vectors_to_yup_vectors();
    check_pack_6_y_uv_vectors_to_4_v210_vectors();

    // YUV repack
    check_repack_yuv422p_to_yuyv();
    check_repack_yuv422p_to_uyvy();
    check_repack_yuyv_to_yuv422p();
    check_repack_uyvy_to_yuv422p();
*/
	// YUV to RGB convert
	check_nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors();
	check_convert_y_uv_vectors_to_rgb_vectors();
	check_nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt601();
	check_nnb_upsample_n_convert_y_uv_vectors_to_rgb_vectors_bt709();
	check_convert_y_uv_vectors_to_rgb_vectors_bt601();
	check_convert_y_uv_vectors_to_rgb_vectors_bt709();
	check_nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601();
	check_nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709();
	check_nnb_upsample_n_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors();
	check_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt601();
	check_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors_bt709();
	check_convert_10bit_y_uv_vectors_to_8bit_rgb_vectors();
/*	
	// YUV unpack
	check_unpack_yuyv_to_y_uv_vectors();
	check_unpack_uyvy_to_y_uv_vectors();
	check_unpack_yuv42Xp_to_2_y_vectors();
	check_unpack_low_yuv42Xp_to_uv_vector();
	check_unpack_high_yuv42Xp_to_uv_vector();
    check_unpack_4v_v210_to_y_uv_vectors();

    // YUV upsample
    check_reconstruct_last_missing_uv();
    check_reconstruct_missing_uv();
*/	
	return 0;
}
