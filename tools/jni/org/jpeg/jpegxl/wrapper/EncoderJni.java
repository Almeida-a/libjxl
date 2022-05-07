// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package org.jpeg.jpegxl.wrapper;

import java.nio.Buffer;

/**
 * Low level JNI wrapper.
 *
 * This class is package-private, should be only be used by high level wrapper.
 */
class EncoderJni {
  private static native void nativeGetJxlStream(int[] context, Buffer pixels, Buffer data, Buffer icc);

  static Status makeStatus(int statusCode) {
    switch (statusCode) {
      case 0:
        return Status.OK;
      case -1:
        return Status.INVALID_STREAM;
      case 1:
        return Status.NOT_ENOUGH_INPUT;
      default:
        throw new IllegalStateException(String.format("Unknown status code: %d", statusCode));
    }
  }

  static StreamInfo makeStreamInfo(int[] context) {
    StreamInfo result = new StreamInfo();
    result.status = makeStatus(context[0]);
    result.width = context[1];
    result.height = context[2];
    result.pixelsSize = context[3];
    result.iccSize = context[4];
    result.alphaBits = context[5];
    return result;
  }


  /** One-shot decoding. */
  static Status getJxlStream(Buffer data, Buffer pixels, Buffer icc, PixelFormat pixelFormat) {
    if (!data.isDirect()) {
      throw new IllegalArgumentException("data must be direct buffer");
    }
    if (!pixels.isDirect()) {
      throw new IllegalArgumentException("pixels must be direct buffer");
    }
    if (!icc.isDirect()) {
      throw new IllegalArgumentException("icc must be direct buffer");
    }
    int[] context = new int[1];
    context[0] = pixelFormat.ordinal();
    nativeGetJxlStream(context, pixels, data, icc);
    return makeStatus(context[0]);
  }

  /** Utility library, disable object construction. */
  private EncoderJni() {}

  public static StreamInfo getBasicInfo(Buffer pixels, int width, int height, PixelFormat pixelFormat) {

    if (!pixels.isDirect()) {
      throw new IllegalArgumentException("PixelData must be direct buffer");
    }
    int[] context = new int[6];
    context[0] = (pixelFormat == null) ? -1 : 0; // 0 for now
    context[1] = width;
    context[2] = height;
    context[3] = 8;  // number of bits
    context[4] = 0;  // iccSize
    context[5] = 0;  // alphaBits
    
    return makeStreamInfo(context);
  }
}
