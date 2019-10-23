/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <jni.h>

#include <android/hardware_buffer_jni.h>
#include <android/log.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>

using PFNEGLCREATENATIVECLIENTBUFFERANDROID = EGLClientBuffer(EGLAPIENTRYP)(const EGLint* attrib_list);
using PFNEGLGETNATIVECLIENTBUFFERANDROID = EGLClientBuffer(EGLAPIENTRYP)(const AHardwareBuffer* buffer);

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    __android_log_print(ANDROID_LOG_INFO, "streamtest", "Native wombat is online.");

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT jobject JNICALL
Java_com_google_android_filament_streamtest_NativeHelper_nCreateEGLContext(JNIEnv*, jclass) {
    return nullptr;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_google_android_filament_streamtest_NativeHelper_nHardwareBufferToEglImage(JNIEnv* env, jclass, jobject wrappedBuffer) {
    AHardwareBuffer* hwbuffer = AHardwareBuffer_fromHardwareBuffer(env, wrappedBuffer);
    if (!hwbuffer) {
        __android_log_print(ANDROID_LOG_ERROR, "streamtest", "Unable to get native hardware buffer.");
        return 0;
    }

    auto eglGetNativeClientBufferANDROID = (PFNEGLGETNATIVECLIENTBUFFERANDROID) eglGetProcAddress("eglGetNativeClientBufferANDROID");
    if (!hwbuffer) {
        __android_log_print(ANDROID_LOG_ERROR, "streamtest", "Unable to get proc for eglGetNativeClientBufferANDROID.");
        return 0;
    }

    EGLClientBuffer clientBuffer = eglGetNativeClientBufferANDROID(hwbuffer);
    if (!clientBuffer) {
        __android_log_print(ANDROID_LOG_ERROR, "streamtest", "Unable to get EGLClientBuffer from AHardwareBuffer.");
        return 0;
    }

    auto eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC) eglGetProcAddress("eglCreateImageKHR");
    auto eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)  eglGetProcAddress("eglDestroyImageKHR"); // TODO: where should we call this?

    EGLint attrs[] = {
        //EGL_PROTECTED_CONTENT_EXT, EGL_TRUE,
        EGL_NONE, EGL_NONE,
    };
    EGLImageKHR eglImage = eglCreateImageKHR(eglGetCurrentDisplay(), EGL_NO_CONTEXT, EGL_NATIVE_BUFFER_ANDROID, clientBuffer, attrs);
    if (eglImage == EGL_NO_IMAGE_KHR) {
        __android_log_print(ANDROID_LOG_ERROR, "streamtest", "eglCreateImageKHR returned no image.");
        return 0;
      }

    return (jlong) eglImage;
}
