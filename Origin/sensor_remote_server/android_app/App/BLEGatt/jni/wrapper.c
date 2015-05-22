#include <jni.h>
#include "actrec_export.h"               /* Model's header file */

void Java_com_example_bluetooth_le_DataPlotActivity_initializeModel(JNIEnv* env,
		jobject javaThis)
{
	actrec_export_initialize();
}

void Java_com_example_bluetooth_le_DataPlotActivity_stepModel(JNIEnv* env,
		jobject javaThis)
{
	actrec_export_step();
}

void Java_com_example_bluetooth_le_DataPlotActivity_setModelInput(JNIEnv* env,
		jobject javaThis, jdouble val, jint index)
{
	switch (index)
	{
	case 0:
		actrec_export_U.Xaxisacceleration = val;
		break;
	case 1:
		actrec_export_U.Yaxisacceleration = val;
		break;
	case 2:
		actrec_export_U.Zaxisacceleration = val;
		break;
	case 3:
		actrec_export_U.auxXacc = val;
		break;
	case 4:
		actrec_export_U.auxYacc = val;
		break;
	case 5:
		actrec_export_U.auxZacc = val;
		break;
	}

}

real_T Java_com_example_bluetooth_le_DataPlotActivity_getModelOutput(
		JNIEnv* env, jobject javaThis, jint type, jint index)
{
	if (type == -1)
		return actrec_export_Y.Activity;
	if (type == -2)
		return actrec_export_Y.var;
	if (type == -3)
		return actrec_export_Y.auxout;
	if (type == 0)
		return actrec_export_Y.fftOut[index];
	if (type == 1)
		return actrec_export_Y.auxfft[index];
	return 0;
}

