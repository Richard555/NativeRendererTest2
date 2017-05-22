package com.via.utility;

import android.util.Log;
import android.view.Surface;

import java.nio.ByteBuffer;

/**
 * Created by hankwu on 3/8/17.
 */

public class NativeRender {
    static {
        System.loadLibrary("viautility");
    }
    public static final int TOP_HALF = 0;
    public static final int BOTTON_HALF = 1;

    public static native void renderingToSurface(Surface s, byte[] pixelBytes, int offset, int width, int height, int size);
    public static native void renderingToSurface2(Surface s, ByteBuffer pixelBytes,int offset, int width, int height, int size ,int colorFormat, int stride);
    public static native void renderingTopOrBottonHalfToSurface(Surface s, ByteBuffer pixelBytes,int offset,int type, int width, int height, int size, int colorFormat, int stride);
    public static native long getPointerFromByteBuffer(ByteBuffer b, int offset);
    public static native void renderingYUV(Surface s, ByteBuffer y,ByteBuffer u,ByteBuffer v,int y_stride,int u_stride,int v_stride,int y_rowStride,int u_rowStride,int v_rowStride,int width, int height);
    public static native void renderingYUVTopOrBottonHalf(Surface s, ByteBuffer y,ByteBuffer u,ByteBuffer v,int y_stride,int u_stride,int v_stride,int y_rowStride,int u_rowStride,int v_rowStride,int type,int width, int height);
    public static native long allocateMemory(int size);
    public static native void copyImageAccordingPitch(ByteBuffer b, int offset,int src_rowStride, long pt,int width, int height, int pitch);
    public static native void releaseMemory(long pt);
    public static native int drawSomething(Surface s,int w, int h, int value, int colorFormat);
    
    public static void renderSurface(Surface s, ByteBuffer pixelBytes, int offset, int width, int height, int size, int colorFormat, int stride) {
        if(pixelBytes.isDirect()) {
            renderingToSurface2(s,pixelBytes,offset,width,height,size,colorFormat,stride);
        } else {
            byte[] b = new byte[pixelBytes.remaining()];
            pixelBytes.get(b);
            renderingToSurface(s,b,offset,width,height,size);
        }
    }
}
