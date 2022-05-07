// Copyright (c) the JPEG XL Project Authors. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package org.jpeg.jpegxl.wrapper;

import java.nio.ByteBuffer;
import java.nio.Buffer;
import java.io.File;

public class EncoderTest {
    static {
      String jniLibrary = System.getProperty("org.jpeg.jpegxl.wrapper.lib");
      if (jniLibrary != null) {
        try {
          File jniLibraryFile = new File(jniLibrary);
          System.load(jniLibraryFile.getAbsolutePath());
        } catch (UnsatisfiedLinkError ex) {
          String message =
              "If the nested exception message says that some standard library (stdc++, tcmalloc, etc.) was not found, "
              + "it is likely that JDK discovered by the build system overrides library search path. "
              + "Try specifying a different JDK via JAVA_HOME environment variable and doing a clean build.";
          throw new RuntimeException(message, ex);
        }
      }
    }

    private static final int SIMPLE_IMAGE_DIM = 1024;

    private static final byte[] SIMPLE_IMAGE_BYTES = {-1, 10, -6, 31, 65, -111, 8, 6, 1, 0, 96, 0, 75,
        56, 36, -70, 121, 30, 85, -74, -110, 6, -17, 121, 17, 112, -25, -47, -18, -64, 7, 61, -110,
        -124, 3, 0};

    static ByteBuffer makeByteBuffer(byte[] src, int length) {
        ByteBuffer buffer = ByteBuffer.allocateDirect(length);
        buffer.put(src, 0, length);
        return buffer;
    }

    static void checkSimpleImageData(ImageData imageData) {
        if (imageData.width != SIMPLE_IMAGE_DIM) {
          throw new IllegalStateException("invalid width");
        }
        if (imageData.height != SIMPLE_IMAGE_DIM) {
          throw new IllegalStateException("invalid height");
        }
        int iccSize = imageData.icc.capacity();
        // Do not expect ICC profile to be some exact size; currently it is 732
        if (iccSize < 300 || iccSize > 1000) {
          throw new IllegalStateException("unexpected ICC profile size");
        }
    }

    static Buffer makeSimpleImage() {
      return ByteBuffer.allocateDirect(1024 * 1024);
    }

    static void checkPixelFormat(int bytesPerPixel) {
      ImageData imageData = Encoder.encode(makeSimpleImage(), 1024, 1024);
      checkSimpleImageData(imageData);
      if (imageData.pixels.limit() != SIMPLE_IMAGE_DIM * SIMPLE_IMAGE_DIM * bytesPerPixel) {
        throw new IllegalStateException("Unexpected pixels size");
      }
    }

    static void testGray() {
      checkPixelFormat(1);
    }

    public static void main(String[] args) {
        testGray();
    }
}
