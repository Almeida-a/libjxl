package org.jpeg.jpegxl.wrapper;

import java.nio.Buffer;
import java.nio.ByteBuffer;

/** JPEG XL JNI decoder wrapper. */
public class Encoder {
    /** Utility library, disable object construction. */
    private Encoder() {}
    
    /** One-shot encoding. */
    public static ImageData encode(Buffer pixels) {

        PixelFormat pixelFormat = PixelFormat.RGBA_8888; // TODO change to gray(?)

        StreamInfo basicInfo = EncoderJni.getBasicInfo(pixels, pixelFormat);
        if (basicInfo.status != Status.OK) {
            throw new IllegalStateException("Encoding failed");
        }
        if (basicInfo.width < 0 || basicInfo.height < 0 || basicInfo.pixelsSize < 0
            || basicInfo.iccSize < 0) {
            throw new IllegalStateException("JNI has returned negative size");
        }
        Buffer data = ByteBuffer.allocateDirect(basicInfo.pixelsSize);
        Buffer icc = ByteBuffer.allocateDirect(basicInfo.iccSize);
        Status status = EncoderJni.getJxlStream(data, pixels, icc, pixelFormat);
        if (status != Status.OK) {
            throw new IllegalStateException("Encoding failed");
        }
        return new ImageData(basicInfo.width, basicInfo.height, pixels, icc, pixelFormat);
    }
}
