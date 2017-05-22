#include "colorConvert.h"
#include "jni.h"
#include <string.h>
#include <android/native_window_jni.h>
#include <android/log.h>

#define TAG    "NATIVERENDER_JNI"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
#define GO_GENERAL_PATH 0
int ColorFormatNV21 = 0x11;
int ColorFormatYV12 = 0x32315659;
int ColorFormatYV21 = 0x13;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv* env = NULL;
	jint result = -1;

	if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return JNI_ERR;
	}

    return JNI_VERSION_1_4;

}

JNIEXPORT void JNICALL Java_com_via_utility_ColorHelper_NV21TOYUV420SP
(JNIEnv * env, jobject thiz, jbyteArray srcarray,jbyteArray dstarray,jint ySize) {
	unsigned char *src = (unsigned char *)(*env)->GetByteArrayElements(env,srcarray, 0);
	unsigned char *dst = (unsigned char*)(*env)->GetByteArrayElements(env,dstarray, 0);
	NV21TOYUV420SP(src,dst,ySize);
	(*env)->ReleaseByteArrayElements(env,srcarray,src,JNI_ABORT);
	(*env)->ReleaseByteArrayElements(env,dstarray,dst,JNI_ABORT);
	return;
}

JNIEXPORT void JNICALL Java_com_via_utility_ColorHelper_YUV420SPTOYUV420P
(JNIEnv * env, jobject thiz, jbyteArray srcarray,jbyteArray dstarray,jint ySize) {
	unsigned char *src = (unsigned char *)(*env)->GetByteArrayElements(env,srcarray, 0);
	unsigned char *dst = (unsigned char*)(*env)->GetByteArrayElements(env,dstarray, 0);
	YUV420SPTOYUV420P(src,dst,ySize);
	(*env)->ReleaseByteArrayElements(env,srcarray,src,JNI_ABORT);
	(*env)->ReleaseByteArrayElements(env,dstarray,dst,JNI_ABORT);
	return;
}

JNIEXPORT void JNICALL Java_com_via_utility_ColorHelper_NV21TOYUV420P
(JNIEnv * env, jobject thiz, jbyteArray srcarray,jbyteArray dstarray,jint ySize) {
	unsigned char *src = (unsigned char *)(*env)->GetByteArrayElements(env,srcarray, 0);
	unsigned char *dst = (unsigned char*)(*env)->GetByteArrayElements(env,dstarray, 0);
	NV21TOYUV420P(src,dst,ySize);
	(*env)->ReleaseByteArrayElements(env,srcarray,src,JNI_ABORT);
	(*env)->ReleaseByteArrayElements(env,dstarray,dst,JNI_ABORT);
	return;
}

JNIEXPORT void JNICALL Java_com_via_utility_ColorHelper_NV21TOARGB
(JNIEnv *env, jobject thiz,jbyteArray srcarray,jintArray dstarray,jint width,jint height){
		unsigned char *src = (unsigned char *)(*env)->GetByteArrayElements(env,srcarray, 0);
		unsigned int *dst = (unsigned int*)(*env)->GetIntArrayElements(env,dstarray, 0);
		NV21TOARGB(src,dst,width,height);
		(*env)->ReleaseByteArrayElements(env,srcarray,src,JNI_ABORT);
		(*env)->ReleaseIntArrayElements(env,dstarray,dst,JNI_ABORT);
		return;
}

JNIEXPORT void JNICALL Java_com_via_utility_ColorHelper_NV21Transform
(JNIEnv * env, jobject thiz, jbyteArray srcarray,jbyteArray dstarray,jint srcwidth,jint srcheight,jint directionflag) {
	unsigned char *src = (unsigned char*)(*env)->GetByteArrayElements(env,srcarray, 0);
	unsigned char *dst = (unsigned char*)(*env)->GetByteArrayElements(env,dstarray, 0);
	NV21Transform(src,dst,srcwidth,srcheight,directionflag);
	(*env)->ReleaseByteArrayElements(env,srcarray,src,JNI_ABORT);
	(*env)->ReleaseByteArrayElements(env,dstarray,dst,JNI_ABORT);
	return;
}

JNIEXPORT void JNICALL Java_com_via_utility_ColorHelper_FIXGLPIXEL
(JNIEnv * env, jobject thiz, jintArray srcarray,jintArray dstarray,jint w,jint h) {
        unsigned int *src = (unsigned int *)(*env)->GetIntArrayElements(env,srcarray, 0);
        unsigned int *dst = (unsigned int *)(*env)->GetIntArrayElements(env,dstarray, 0);
        FIXGLPIXEL(src,dst,w,h);
        (*env)->ReleaseIntArrayElements(env,srcarray,src,JNI_ABORT);
        (*env)->ReleaseIntArrayElements(env,dstarray,dst,JNI_ABORT);
        return;
}

#define JNI_FALSE 0
#define JNI_TRUE 1

//rendering
JNIEXPORT void JNICALL Java_com_via_utility_NativeRender_renderingToSurface
(JNIEnv * env, jobject thiz,jobject javaSurface,jbyteArray pixelsArray,jint srcBufferOffset,jint srcWidth,jint srcHeight,jint srcSize) {
	ANativeWindow* window = ANativeWindow_fromSurface(env, javaSurface);
	if(window!=NULL)
	{
		ANativeWindow_setBuffersGeometry(window,srcWidth,srcHeight,ColorFormatNV21);
		ANativeWindow_Buffer buffer;
		if (ANativeWindow_lock(window, &buffer, NULL) == 0) {
			unsigned char *pSrc = (unsigned char*)(*env)->GetByteArrayElements(env,pixelsArray, 0);
			unsigned char *pDstBuffer = buffer.bits;

			pSrc = pSrc+srcBufferOffset;

			if(buffer.width==buffer.stride){
				memcpy(pDstBuffer, pSrc,  srcSize);
			}else{
				int height = srcHeight*3/2;
				int width = srcWidth;
				int i=0;
				for(;i<height;++i)
					memcpy(pDstBuffer +  buffer.stride * i
						, pSrc + width * i
						, width);
			}
			(*env)->ReleaseByteArrayElements(env,pixelsArray,pSrc,JNI_ABORT);
			ANativeWindow_unlockAndPost(window);
		}
		ANativeWindow_release(window);
	}
	return;
}

JNIEXPORT jlong JNICALL Java_com_via_utility_NativeRender_getPointerFromByteBuffer(JNIEnv * env,jobject thiz,jbyteArray pixelsArray, jint offset) { 
	unsigned char *pix = (unsigned char *)(*env)->GetDirectBufferAddress(env,pixelsArray);
	unsigned char *pixels = pix+offset;
	return (jlong) pixels;
}

JNIEXPORT jlong JNICALL Java_com_via_utility_NativeRender_copyImageAccordingPitch(JNIEnv * env,jobject thiz, jbyteArray pixelsArray, jint offset,jint src_rowStride, jlong dest_pt,jint width, jint height, jint pitch) {
	unsigned char *pix = (unsigned char *)(*env)->GetDirectBufferAddress(env,pixelsArray);
	unsigned char *pixels = pix+offset;
	unsigned char *dest = (unsigned char*) dest_pt;

	unsigned char *dest_tmp = dest;
	int i = 0;
	while(i<height) {
		memcpy(dest_tmp, pixels, width);
		dest_tmp+=pitch;
		pixels+=src_rowStride;
		i++;
	}

	return (jlong) dest;
}

JNIEXPORT jlong JNICALL Java_com_via_utility_NativeRender_allocateMemory(JNIEnv * env,jobject thiz, jint size) { 
	unsigned char *dst = (unsigned char*) malloc(size);
	return (jlong) dst;
}


JNIEXPORT void JNICALL Java_com_via_utility_NativeRender_releaseMemory(JNIEnv * env,jobject thiz,jlong pt) { 
	unsigned char *pix = (unsigned char*) pt;
	free(pix);
}

//rendering
JNIEXPORT void JNICALL Java_com_via_utility_NativeRender_renderingToSurface2
(JNIEnv * env, jobject thiz,jobject javaSurface,jbyteArray pixelsArray,jint srcBufferOffset,jint srcWidth,jint srcHeight,jint srcSize, jint srcColorFormat, jint srcStride) {
	ANativeWindow* window = ANativeWindow_fromSurface(env, javaSurface);
	if(window!=NULL)
	{
        if(srcColorFormat == ColorFormatYV21) { //Because ImageFormat do not support YV21, we set Buffer format YV12
            ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeight, ColorFormatYV12);
        }
        else { //for nv21
            ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeight, ColorFormatNV21);
        }
        ANativeWindow_Buffer buffer;
        if (ANativeWindow_lock(window, &buffer, NULL) == 0) {
            unsigned char *pSrc = (unsigned char *) (*env)->GetDirectBufferAddress(env,
                                                                                       pixelsArray);
            pSrc = pSrc + srcBufferOffset;
            unsigned char *pDstBuffer = buffer.bits;

            //for srcFormat == YV21 to dstFormat == YV12
            if(srcColorFormat == ColorFormatYV21){
                int i=0, j=0, k=0;

                for(;i<srcHeight;++i)
                    memcpy(pDstBuffer +  buffer.stride * i
                            , pSrc + srcStride * i
                            , srcWidth);

                for(j = srcHeight;j<srcHeight*5/4;++j)
                    memcpy(pDstBuffer +  buffer.stride * (j+buffer.height/4)
                            , pSrc + srcStride * j
                            , srcWidth);

                for(k = srcHeight*5/4;k<srcHeight*3/2;++k)
                    memcpy(pDstBuffer +  buffer.stride * (k-buffer.height/4)
                            , pSrc + srcStride * k
                            , srcWidth);
            }
            //for srcFormat == dstFormat
            else if (buffer.width == buffer.stride) {
                memcpy(pDstBuffer, pSrc, srcSize);
            }
            else {
                int yuvHeight = srcHeight * 3 / 2;
                int i = 0;

                for (; i < yuvHeight; ++i)
                    memcpy(pDstBuffer + buffer.stride * i, pSrc + srcWidth * i, srcWidth);
            }
            ANativeWindow_unlockAndPost(window);
        }
        ANativeWindow_release(window);
	}
	return;
}

//rendering
JNIEXPORT void JNICALL Java_com_via_utility_NativeRender_renderingTopOrBottonHalfToSurface
		(JNIEnv * env, jobject thiz,jobject javaSurface,jbyteArray pixelsArray,jint srcBufferOffset,jint type,jint srcWidth,jint srcHeight,jint size, jint srcColorFormat, jint srcStride) {
	ANativeWindow* window = ANativeWindow_fromSurface(env, javaSurface);
	if(window!=NULL)
	{
        if(srcColorFormat == ColorFormatYV21) { //Because ImageFormat do not support YV21, we set Buffer format YV12
            ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeight / 2, ColorFormatYV12);
        }
        else { //for nv21
            ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeight / 2, ColorFormatNV21);
        }
		ANativeWindow_Buffer dstBuffer;
		if (ANativeWindow_lock(window, &dstBuffer, NULL) == 0) {
			unsigned char *pSrc = (unsigned char*)(*env)->GetDirectBufferAddress(env,pixelsArray);
			pSrc = pSrc+srcBufferOffset;

			int height = srcHeight/2;
			int width = srcWidth;
			int i;

            int yOffset=type*(srcWidth*srcHeight/2);
            int uvOffsetForNV21=type*(srcWidth*srcHeight/4);
            int uvOffserForYV21=type*(srcWidth*srcHeight/8);

			unsigned char* pSrcYPlane = pSrc + yOffset;
			unsigned char* pSrcUVPlane = pSrc + srcWidth*srcHeight;

			unsigned char* pDstBuffer = dstBuffer.bits;

			memset(pDstBuffer,0,height*width*3/2);

            //step1 : Src y to Dst y
			for(i=0;i<height;++i) {
				memcpy(pDstBuffer, pSrcYPlane, width);
				pDstBuffer+=dstBuffer.stride;
				pSrcYPlane+=width;
			}

            //step2 : Src uv to Dst uv
            if(srcColorFormat == ColorFormatYV21){ // step 2 ,case 1:for srcFormat == YV21 to dstFormat == YV12
                pSrcUVPlane += uvOffserForYV21;//jump to corresponding Src U plane
                for (i = 0; i < srcHeight / 8; ++i) {
                    int j = 0;
                    for (; j < width; ++j) {
                        *(pDstBuffer + j) = *(pSrcUVPlane + j + srcStride*srcHeight/4);
                    }
                    pDstBuffer += dstBuffer.stride;
                    pSrcUVPlane += srcStride;
                }
                pSrcUVPlane +=srcStride*srcHeight/8;//jump to corresponding Src V plane
                for (i = srcHeight / 8; i < srcHeight / 4; ++i) {
                    int j = 0;
                    for (; j < width; ++j) {
                        *(pDstBuffer + j) = *(pSrcUVPlane + j - srcStride*srcHeight/4);
                    }
                    pDstBuffer += dstBuffer.stride;
                    pSrcUVPlane += srcStride;
                }
            }
            else {//step 2 ,case 2: for nv21
                pSrcUVPlane += uvOffsetForNV21;
                for (i = 0; i < height / 2; ++i) {
                    int j = 0;
                    for (; j < width / 2; ++j) {
                        *(pDstBuffer + j * 2 + 0) = *(pSrcUVPlane + j * 2 + 0);
                        *(pDstBuffer + j * 2 + 1) = *(pSrcUVPlane + j * 2 + 1);
                    }
                    pDstBuffer += dstBuffer.stride;
                    pSrcUVPlane += width;
                }
            }
			ANativeWindow_unlockAndPost(window);
		}
		ANativeWindow_release(window);
	}
	return;
}

//rendering
JNIEXPORT void JNICALL Java_com_via_utility_NativeRender_renderingYUV
(JNIEnv * env, jobject thiz,jobject javaSurface,jbyteArray y,jbyteArray u,jbyteArray v,
 jint yPixelStride,jint uPixelStride,jint vPixelStride,
 jint yRowSrtide, jint uRowStride, jint vRowStride, jint srcWidth,jint srcHeight) {

	ANativeWindow* window = ANativeWindow_fromSurface(env, javaSurface);
	if(window!=NULL)
	{
		ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeight, ColorFormatNV21);
		ANativeWindow_Buffer dstBuffer;
		if (ANativeWindow_lock(window, &dstBuffer, NULL) == 0) {
			unsigned char *pSrcYPlane = (unsigned char*)(*env)->GetDirectBufferAddress(env,y);
			unsigned char *pSrcUPlane = (unsigned char*)(*env)->GetDirectBufferAddress(env,u);
			unsigned char *pSrcVPlane = (unsigned char*)(*env)->GetDirectBufferAddress(env,v);

			unsigned char* pDstBuffer = dstBuffer.bits;
			int yPlaneSize = srcWidth*srcHeight;
			int i,j;

			if((dstBuffer.width==dstBuffer.stride  && srcWidth == yRowSrtide && uPixelStride == 2) && !GO_GENERAL_PATH) {
				// faster
				// Y
				memcpy(pDstBuffer, pSrcYPlane, yPlaneSize);

				// UV
				int uvPlaneSize = yPlaneSize/2;
				int uSize = uvPlaneSize/2;

				// copy V
				memcpy(pDstBuffer + yPlaneSize, pSrcUPlane + 1, uvPlaneSize - 1);
				// copy U
				unsigned char *src = pSrcUPlane;
				unsigned char *dst = pDstBuffer + yPlaneSize + 1;
				int i = 0;
				while (i < uSize) {
					(*dst) = (*src);
					dst += 2;
					src += 2;
					++i;
				}

			} else {
				// Y
				for(i=0;i<srcHeight;i++) {
					memcpy(pDstBuffer+dstBuffer.stride*i, pSrcYPlane+yRowSrtide*i, srcWidth);
				}
				// UV
				for(i=0;i<srcHeight/2;i++) {
					for(j=0;j<srcWidth/2;j++) {
						*(pDstBuffer + yPlaneSize + dstBuffer.stride*i + j*2 + 0) =  *(pSrcVPlane + vRowStride*i + j*vPixelStride);
						*(pDstBuffer + yPlaneSize + dstBuffer.stride*i + j*2 + 1) =  *(pSrcUPlane + uRowStride*i + j*uPixelStride);
					}
				}
			}
			ANativeWindow_unlockAndPost(window);
		}
		ANativeWindow_release(window);
	}
	return;
}

//rendering
JNIEXPORT void JNICALL Java_com_via_utility_NativeRender_renderingYUVTopOrBottonHalf
(JNIEnv * env, jobject thiz,jobject javaSurface,jbyteArray y,jbyteArray u,jbyteArray v,
 jint yPixelStride,jint uPixelStride,jint vPixelStride,
 jint yRowSrtide, jint uRowStride, jint vRowStride, jint type , jint srcWidth, jint srcHeight) {

	ANativeWindow* window = ANativeWindow_fromSurface(env, javaSurface);
	int srcHeightHalf = srcHeight/2;

	if(window!=NULL)
	{
		ANativeWindow_setBuffersGeometry(window, srcWidth, srcHeightHalf, ColorFormatNV21);
		ANativeWindow_Buffer dstBuffer;
		if (ANativeWindow_lock(window, &dstBuffer, NULL) == 0) {
			unsigned char *y_pixels = (unsigned char*)(*env)->GetDirectBufferAddress(env,y);
			unsigned char *u_pixels = (unsigned char*)(*env)->GetDirectBufferAddress(env,u);
			unsigned char *v_pixels = (unsigned char*)(*env)->GetDirectBufferAddress(env,v);

			// assume y_stride == 1
			unsigned char *pSrcYPlane = y_pixels + yRowSrtide*srcHeightHalf*type; // type==0 top type==1 botton
			unsigned char *pSrcUPlane = u_pixels + uRowStride*srcHeightHalf/2*type;;
			unsigned char *pSrcVPlane = v_pixels + vRowStride*srcHeightHalf/2*type;

			unsigned char* pDstBuffer = dstBuffer.bits;
			int yPlaneSize = srcWidth*srcHeightHalf;
			int i,j;
			if((dstBuffer.width == dstBuffer.stride && srcWidth == yRowSrtide && uPixelStride == 2) && !GO_GENERAL_PATH) {
				//faster
				// Y
				memcpy(pDstBuffer, pSrcYPlane, yPlaneSize);

				// UV
				int uvPlaneSize = yPlaneSize/2;
				int uSize = uvPlaneSize/2;
				// V
				memcpy(pDstBuffer + yPlaneSize, pSrcUPlane + 1, uvPlaneSize - 1);
				// U
				unsigned char *src = pSrcUPlane;
				unsigned char *dst = pDstBuffer + yPlaneSize + 1;
				int i = 0;
				while (i < uSize) {
					(*dst) = (*src);
					dst += 2;
					src += 2;
					++i;
				}
			} else {
				// Y
				for(i=0;i<srcHeightHalf;i++) {
					memcpy(pDstBuffer+dstBuffer.stride*i, pSrcYPlane+yRowSrtide*i, srcWidth);
				}
				// UV
				for(i=0;i<srcHeightHalf/2;i++) {
					for(j=0;j<srcWidth/2;j++) {
						*(pDstBuffer + yPlaneSize + dstBuffer.stride*i + j*2 + 0) =  *(pSrcVPlane + vRowStride*i + j*vPixelStride);
						*(pDstBuffer + yPlaneSize + dstBuffer.stride*i + j*2 + 1) =  *(pSrcUPlane + uRowStride*i + j*uPixelStride);
					}
				}
			}

			ANativeWindow_unlockAndPost(window);
		}
		ANativeWindow_release(window);
	}
	return;
}


int colorFormatARGB = WINDOW_FORMAT_RGBA_8888;
//rendering
JNIEXPORT jint JNICALL Java_com_via_utility_NativeRender_drawSomething
		(JNIEnv * env, jobject thiz,jobject javaSurface, jint width, jint height, jint value, jint colorFormat) {
    LOGD("ANativeWindow_fromSurface!");
	ANativeWindow* window = ANativeWindow_fromSurface(env, javaSurface);
    LOGD("ANativeWindow_fromSurface! done");

    if(window!=NULL)
	{
        LOGD("setBuffersGeometry!");
        int ret = ANativeWindow_setBuffersGeometry(window, width, height, colorFormat);
        LOGD("setBuffersGeometry done ret:%d",ret);

        if ( 0 > ret ) {
            LOGD("ANativeWindow_release!");
            ANativeWindow_release(window);
            LOGD("ANativeWindow_release! done");

            return -1;
		} else {
			ANativeWindow_Buffer dstBuffer;
            LOGD("ANativeWindow_lock!");
            if (ANativeWindow_lock(window, &dstBuffer, NULL) == 0) {
                LOGD("ANativeWindow_lock! done success");

                LOGD("draw value : %d",value);

                unsigned char *pDstBuffer = dstBuffer.bits;

                if(colorFormat==ColorFormatYV12 || colorFormat== ColorFormatNV21 || colorFormat == ColorFormatYV21)
                    memset(pDstBuffer, value, width * height * 3 / 2);
                else if (colorFormat==WINDOW_FORMAT_RGBA_8888 || colorFormat==WINDOW_FORMAT_RGBX_8888)
                    memset(pDstBuffer, value, width * height * 4);
                else if (colorFormat==WINDOW_FORMAT_RGB_565)
                    memset(pDstBuffer, value, width * height * 2);

                LOGD("draw value : %d done" ,value);

                LOGD("ANativeWindow_unlockAndPost!");

                ANativeWindow_unlockAndPost(window);
                LOGD("ANativeWindow_unlockAndPost! done");

            } else {
                LOGD("ANativeWindow_lock! done fail");
                LOGD("ANativeWindow_release!");

                ANativeWindow_release(window);
                LOGD("ANativeWindow_release! done");

                return -1;
            }

            LOGD("ANativeWindow_release!");
            ANativeWindow_release(window);
            LOGD("ANativeWindow_release! done");

            return 1;
		}
	}
	return -1;
}
