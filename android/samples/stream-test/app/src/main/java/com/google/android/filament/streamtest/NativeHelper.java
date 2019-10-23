package com.google.android.filament.streamtest;

import android.hardware.HardwareBuffer;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLES11Ext;
import android.opengl.GLES30;
import android.opengl.GLU;

import org.jetbrains.annotations.Nullable;

import static android.opengl.EGL15.EGL_OPENGL_ES3_BIT;

public class NativeHelper {

    public static void init() {
        System.loadLibrary("native-lib");
    }

    public static EGLContext createEGLContext() {
        EGLContext shareContext = EGL14.EGL_NO_CONTEXT;
        EGLDisplay display = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY);

        int[] minorMajor = null;
        EGL14.eglInitialize(display, minorMajor, 0, minorMajor, 0);
        EGLConfig[] configs = new EGLConfig[1];
        int[] numConfig = {0};
        int[] attribs = {EGL14.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL14.EGL_NONE};
        EGL14.eglChooseConfig(display, attribs, 0, configs, 0, 1, numConfig, 0);

        int[] contextAttribs = {EGL14.EGL_CONTEXT_CLIENT_VERSION, 3, EGL14.EGL_NONE};
        EGLContext context =
                EGL14.eglCreateContext(display, configs[0], shareContext, contextAttribs, 0);

        int[] surfaceAttribs = {
                EGL14.EGL_WIDTH, 1,
                EGL14.EGL_HEIGHT, 1,
                EGL14.EGL_NONE
        };

        EGLSurface surface = EGL14.eglCreatePbufferSurface(display, configs[0], surfaceAttribs, 0);

        if (!EGL14.eglMakeCurrent(display, surface, surface, context)) {
            throw new IllegalStateException("Error making GL context.");
        }

        return context;
        //return (EGLContext) nCreateEGLContext();
    }

    public static int createCameraTexture(EGLContext context) {
        int[] textures = new int[1];
        GLES30.glGenTextures(1, textures, 0);
        int result = textures[0];

        final int textureTarget = GLES11Ext.GL_TEXTURE_EXTERNAL_OES;
        GLES30.glBindTexture(textureTarget, result);
        GLES30.glTexParameteri(textureTarget, GLES30.GL_TEXTURE_WRAP_S, GLES30.GL_CLAMP_TO_EDGE);
        GLES30.glTexParameteri(textureTarget, GLES30.GL_TEXTURE_WRAP_T, GLES30.GL_CLAMP_TO_EDGE);
        GLES30.glTexParameteri(textureTarget, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_NEAREST);
        GLES30.glTexParameteri(textureTarget, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_NEAREST);

        if (!GLES30.glIsTexture(result)) {
            throw new RuntimeException("OpenGL error: $result is an invalid texture.");
        }

        int error = GLES30.glGetError();
        if (error != GLES30.GL_NO_ERROR) {
            String errorString = GLU.gluErrorString(error);
            throw new RuntimeException("OpenGL error: " + errorString + "!");
        }

        return result;
    }

    @Nullable
    public static long hwBufferToEglImage(HardwareBuffer hwbuffer) {
        return nHardwareBufferToEglImage(hwbuffer);
    }

    private static native Object nCreateEGLContext();
    private static native long nHardwareBufferToEglImage(HardwareBuffer hwbuffer);
}
