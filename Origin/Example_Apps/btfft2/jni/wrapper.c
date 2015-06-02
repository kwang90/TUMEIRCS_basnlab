#include <jni.h>
#include "fft4android.h"               /* Model's header file */
#include "rtwtypes.h"                  /* MathWorks types */

void Java_com_rcsexample_btfft2_MainActivity_initializeModel(JNIEnv* env,
		jobject javaThis)
{
	fft4android_initialize();
}

void Java_com_rcsexample_btfft2_MainActivity_stepModel(JNIEnv* env,
		jobject javaThis)
{
	fft4android_step();
}

void Java_com_rcsexample_btfft2_MainActivity_setModelInput(JNIEnv* env,
		jobject javaThis, jdouble val, jint index)
{
	fft4android_U.In1[index] = val;
}

real_T Java_com_rcsexample_btfft2_MainActivity_getModelOutput(
		JNIEnv* env, jobject javaThis, jint index)
{
	return fft4android_Y.Out1[index];
}


