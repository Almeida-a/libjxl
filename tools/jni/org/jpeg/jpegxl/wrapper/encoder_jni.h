// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef TOOLS_JNI_ORG_JPEG_JPEGXL_WRAPPER_ENCODER_JNI
#define TOOLS_JNI_ORG_JPEG_JPEGXL_WRAPPER_ENCODER_JNI

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get encoded image JXL stream.
 *
 * @param ctx {in_pixel_format_out_status} tuple
 * @param pixels [in] Buffer to place pixels to
 * @param data [out] Buffer with encoded JXL stream
 */
JNIEXPORT void JNICALL Java_org_jpeg_jpegxl_wrapper_EncoderJni_nativeGetJxlStream(
    JNIEnv* env, jobject /* jobj */, jintArray ctx, jobject pixels_buffer,
    jobject data_buffer, jobject icc_buffer);

#ifdef __cplusplus
}
#endif

#endif  // TOOLS_JNI_ORG_JPEG_JPEGXL_WRAPPER_ENCODER_JNI