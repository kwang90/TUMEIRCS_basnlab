#include <jni.h>
#include "untitled.h"               /* Model's header file */
#include "rtwtypes.h"                  /* MathWorks types */

void Java_com_example_hallo_MainActivity_initializeModel(JNIEnv* env,
		jobject javaThis)
{
	untitled_initialize();
}

void Java_com_example_hallo_MainActivity_stepModel(JNIEnv* env,
		jobject javaThis)
{
	untitled_step();
}

void Java_com_example_hallo_MainActivity_setModelInput(JNIEnv* env,
		jobject javaThis, jdouble val)
{
	untitled_U.In1 = val;
}

real_T Java_com_example_hallo_MainActivity_getModelOutput(
		JNIEnv* env, jobject javaThis)
{
	return untitled_Y.Out1;
}


