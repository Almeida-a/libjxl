// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "tools/jni/org/jpeg/jpegxl/wrapper/encoder_jni.h"

#include <jni.h>

#include <cstddef>
#include <cstdlib>

#include "jxl/encode.h"
#include "jxl/encode_cxx.h"
#include "jxl/thread_parallel_runner.h"
#include "lib/jxl/base/status.h"

namespace {

template <typename From, typename To>
bool StaticCast(const From& from, To* to) {
  To tmp = static_cast<To>(from);
  // Check sign is preserved.
  if ((from < 0 && tmp > 0) || (from > 0 && tmp < 0)) return false;
  // Check value is preserved.
  if (from != static_cast<From>(tmp)) return false;
  *to = tmp;
  return true;
}

bool BufferToSpan(JNIEnv* env, jobject buffer, uint8_t** data, size_t* size) {
  if (buffer == nullptr) return true;

  *data = reinterpret_cast<uint8_t*>(env->GetDirectBufferAddress(buffer));
  if (*data == nullptr) return false;
  return StaticCast(env->GetDirectBufferCapacity(buffer), size);
}

int ToStatusCode(const jxl::Status& status) {
  if (status) return 0;
  if (status.IsFatalError()) return -1;
  return 1;  // Non-fatal -> not enough input.
}

constexpr const size_t kLastPixelFormat = 3;
// constexpr const size_t kNoPixelFormat = static_cast<size_t>(-1);

jxl::Status DoEncode(JNIEnv* env, jobject data_buffer, size_t* info_pixels_size,
                     size_t* info_icc_size, JxlBasicInfo* info,
                     size_t pixel_format, jobject pixels_buffer,
                     jobject icc_buffer) {

    if (pixels_buffer == nullptr) return JXL_FAILURE("No pixels buffer");

    uint8_t* pixels = nullptr;
    size_t pixels_size = 0;
    if (!BufferToSpan(env, data_buffer, &pixels, &pixels_size)) {
        return JXL_FAILURE("Failed to access pixels buffer");
    }

    uint8_t* data = nullptr;
    size_t data_size = 0;
    if (!BufferToSpan(env, data_buffer, &data, &data_size)) {
        return JXL_FAILURE("Failed to access data buffer");
    }

    uint8_t* icc = nullptr;
    size_t icc_size = 0;
    if (!BufferToSpan(env, icc_buffer, &icc, &icc_size)) {
        return JXL_FAILURE("Failed to access ICC buffer");
    }

    JxlEncoder* enc = JxlEncoderCreate(nullptr);

    constexpr size_t kNumThreads = 0;  // Do everything in this thread.
    void* runner = JxlThreadParallelRunnerCreate(NULL, kNumThreads);

    struct Defer {
        JxlEncoder* enc;
        void* runner;
        ~Defer() {
        JxlThreadParallelRunnerDestroy(runner);
        JxlEncoderDestroy(enc);
        }
    } defer{enc, runner};

    auto status =
        JxlEncoderSetParallelRunner(enc, JxlThreadParallelRunner, runner);
    if (status != JXL_ENC_SUCCESS) {
     return JXL_FAILURE("Failed to set parallel runner");
    }

    if (JXL_ENC_SUCCESS != JxlEncoderSetBasicInfo(enc, info)) {
        fprintf(stderr, "JxlEncoderSetBasicInfo failed\n");
        return false;
    }

    JxlColorEncoding color_encoding = {};
    JxlColorEncodingSetToSRGB(&color_encoding,
                                /*is_gray=*/1);

    if (JXL_ENC_SUCCESS !=
        JxlEncoderSetColorEncoding(enc, &color_encoding)) {
        fprintf(stderr, "JxlEncoderSetColorEncoding failed\n");
        return JXL_FAILURE("Unexpected notification (want: color encoding)");;
    }

    // Process output
    status = JxlEncoderProcessOutput(enc, &data, &data_size);
    if (status == JXL_ENC_NEED_MORE_OUTPUT) {
        return JXL_STATUS(jxl::StatusCode::kNotEnoughBytes, "Not enough input");
    }

    return jxl::StatusCode::kOk;
}

}  // namespace

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get image jxl code stream.
 *
 * @param ctx {out_status} tuple
 * @param pixels [in] Buffer to place pixels to
 * @param data [out] Buffer with encoded JXL stream
 */
JNIEXPORT void JNICALL Java_org_jpeg_jpegxl_wrapper_DecoderJni_nativeGetJxlStream(
    JNIEnv* env, jobject /* jobj */, jintArray ctx, jobject pixels_buffer,
    jobject data_buffer, jobject icc_buffer) {
  jint context[1] = {0};
  env->GetIntArrayRegion(ctx, 0, 1, context);

  size_t pixel_format = 0;

  jxl::Status status = true;

  if (status) {
    // Unlike getBasicInfo, "no-pixel-format" is not supported.
    pixel_format = context[0];
    if (pixel_format > kLastPixelFormat) {
      status = JXL_FAILURE("Unrecognized pixel format");
    }
  }

  if (status) {
    status = DoEncode(env, data_buffer, /* info_pixels_size= */ nullptr,
                      /* info_icc_size= */ nullptr, /* info= */ nullptr,
                      pixel_format, pixels_buffer, icc_buffer);
  }

  context[0] = ToStatusCode(status);
  env->SetIntArrayRegion(ctx, 0, 1, context);
}


/**
 * Get basic image information (size, etc.)
 *
 * @param ctx {in_pixel_format_out_status, out_width, out_height, pixels_size,
 *             icc_size} tuple
 * @param data [in] Buffer with pixels information
 */
JNIEXPORT void JNICALL Java_org_jpeg_jpegxl_wrapper_EncoderJni_nativeGetBasicInfo(
    JNIEnv* env, jobject /* jobj */, jintArray ctx, jobject pixels_buffer,
    jobject data_buffer, jobject icc_buffer) {
      // TODO implement
    }

#ifdef __cplusplus
}
#endif