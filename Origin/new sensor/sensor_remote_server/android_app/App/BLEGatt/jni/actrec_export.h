/*
 * File: actrec_export.h
 *
 * Code generated for Simulink model 'actrec_export'.
 *
 * Model version                  : 1.65
 * Simulink Coder version         : 8.4 (R2013a) 13-Feb-2013
 * TLC version                    : 8.4 (Jan 18 2013)
 * C/C++ source code generated on : Mon Dec 16 12:10:46 2013
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_actrec_export_h_
#define RTW_HEADER_actrec_export_h_
#include <stddef.h>
#include "rtwtypes.h"
#ifndef actrec_export_COMMON_INCLUDES_
# define actrec_export_COMMON_INCLUDES_
#include <math.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* actrec_export_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */
#ifndef rtmGetErrorStatus
# define rtmGetErrorStatus(rtm)        ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
# define rtmSetErrorStatus(rtm, val)   ((rtm)->errorStatus = (val))
#endif

#ifndef rtmGetStopRequested
# define rtmGetStopRequested(rtm)      ((void*) 0)
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_actrec_export_T RT_MODEL_actrec_export_T;

/* Block signals (auto storage) */
typedef struct {
  real_T PeakFinder_o2[10];            /* '<S1>/Peak Finder' */
  real_T Abs1[3];                      /* '<Root>/Abs1' */
} B_actrec_export_T;

/* Block states (auto storage) for system '<Root>' */
typedef struct {
  real_T u6Frame2_CircBuf[512];        /* '<Root>/256-Frame2' */
  real_T u6Frame_CircBuf[512];         /* '<Root>/256-Frame' */
  real_T u6Frame3_CircBuf[512];        /* '<Root>/256-Frame3' */
  real_T Variance_AccVal;              /* '<S4>/Variance' */
  real_T Variance_SqData;              /* '<S4>/Variance' */
  real_T Variance_AccVal_a;            /* '<S3>/Variance' */
  real_T Variance_SqData_i;            /* '<S3>/Variance' */
  int32_T u6Frame2_inBufPtrIdx;        /* '<Root>/256-Frame2' */
  int32_T u6Frame2_outBufPtrIdx;       /* '<Root>/256-Frame2' */
  int32_T u6Frame2_bufferLength;       /* '<Root>/256-Frame2' */
  int32_T u6Frame_inBufPtrIdx;         /* '<Root>/256-Frame' */
  int32_T u6Frame_outBufPtrIdx;        /* '<Root>/256-Frame' */
  int32_T u6Frame_bufferLength;        /* '<Root>/256-Frame' */
  int32_T u6Frame3_inBufPtrIdx;        /* '<Root>/256-Frame3' */
  int32_T u6Frame3_outBufPtrIdx;       /* '<Root>/256-Frame3' */
  int32_T u6Frame3_bufferLength;       /* '<Root>/256-Frame3' */
} DW_actrec_export_T;

/* Constant parameters (auto storage) */
typedef struct {
  /* Pooled Parameter (Expression: )
   * Referenced by:
   *   '<S5>/FFT'
   *   '<S6>/FFT'
   */
  real_T pooled1[192];
} ConstP_actrec_export_T;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real_T Zaxisacceleration;            /* '<Root>/Z axis acceleration' */
  real_T Yaxisacceleration;            /* '<Root>/Y axis acceleration' */
  real_T Xaxisacceleration;            /* '<Root>/X axis acceleration' */
  real_T auxZacc;                      /* '<Root>/aux Z acc' */
  real_T auxYacc;                      /* '<Root>/aux Y acc' */
  real_T auxXacc;                      /* '<Root>/aux X acc' */
} ExtU_actrec_export_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real_T Activity;                     /* '<Root>/Activity' */
  real_T fftOut[256];                  /* '<Root>/fftOut' */
  boolean_T var;                       /* '<Root>/var' */
  real_T auxout;                       /* '<Root>/aux out' */
  real_T auxfft[256];                  /* '<Root>/aux fft' */
} ExtY_actrec_export_T;

/* Parameters (auto storage) */
struct P_actrec_export_T_ {
  real_T NorelevantMotionActivity_Value;/* Expression: 0
                                         * Referenced by: '<Root>/No relevant Motion Activity'
                                         */
  real_T Unknown_Value;                /* Expression: -1
                                        * Referenced by: '<Root>/Unknown'
                                        */
  real_T WalkingActivity_Value;        /* Expression: 1
                                        * Referenced by: '<Root>/Walking Activity'
                                        */
  real_T Constant4_Value;              /* Expression: 1.00
                                        * Referenced by: '<Root>/Constant4'
                                        */
  real_T RunningActivity_Value;        /* Expression: 2
                                        * Referenced by: '<Root>/Running Activity'
                                        */
  real_T Constant2_Value;              /* Expression: 3e6
                                        * Referenced by: '<Root>/Constant2'
                                        */
  real_T Constant_Value;               /* Expression: 0
                                        * Referenced by: '<S2>/Constant'
                                        */
  real_T Constant1_Value;              /* Expression: 14e5
                                        * Referenced by: '<Root>/Constant1'
                                        */
  real_T Constant_Value_c;             /* Expression: 0
                                        * Referenced by: '<S1>/Constant'
                                        */
  real_T u6Frame2_ic;                  /* Expression: 0
                                        * Referenced by: '<Root>/256-Frame2'
                                        */
  real_T Constant3_Value;              /* Expression: 0.05
                                        * Referenced by: '<Root>/Constant3'
                                        */
  real_T u6Frame_ic;                   /* Expression: 0
                                        * Referenced by: '<Root>/256-Frame'
                                        */
  real_T u6Frame3_ic;                  /* Expression: 0
                                        * Referenced by: '<Root>/256-Frame3'
                                        */
  uint32_T Constant_Value_o;           /* Computed Parameter: Constant_Value_o
                                        * Referenced by: '<S8>/Constant'
                                        */
  uint32_T Constant_Value_m;           /* Computed Parameter: Constant_Value_m
                                        * Referenced by: '<S9>/Constant'
                                        */
};

/* Parameters (auto storage) */
typedef struct P_actrec_export_T_ P_actrec_export_T;

/* Real-time Model Data Structure */
struct tag_RTM_actrec_export_T {
  const char_T *errorStatus;

  /*
   * Timing:
   * The following substructure contains information regarding
   * the timing information for the model.
   */
  struct {
    struct {
      uint16_T TID[5];
    } TaskCounters;
  } Timing;
};

/* Block parameters (auto storage) */
extern P_actrec_export_T actrec_export_P;

/* Block signals (auto storage) */
extern B_actrec_export_T actrec_export_B;

/* Block states (auto storage) */
extern DW_actrec_export_T actrec_export_DW;

/* External inputs (root inport signals with auto storage) */
extern ExtU_actrec_export_T actrec_export_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_actrec_export_T actrec_export_Y;

/* Constant parameters (auto storage) */
extern const ConstP_actrec_export_T actrec_export_ConstP;

/* Model entry point functions */
extern void actrec_export_initialize(void);
extern void actrec_export_step(void);

/* Real-time Model object */
extern RT_MODEL_actrec_export_T *const actrec_export_M;

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'actrec_export'
 * '<S1>'   : 'actrec_export/Check if there are any peaks over walking threshold'
 * '<S2>'   : 'actrec_export/Check if there are exactly two peaks over a threshold'
 * '<S3>'   : 'actrec_export/Check if there is relevant motion'
 * '<S4>'   : 'actrec_export/Check if there is relevant swaying motion'
 * '<S5>'   : 'actrec_export/Magnitude FFT'
 * '<S6>'   : 'actrec_export/Magnitude FFT1'
 * '<S7>'   : 'actrec_export/Subsystem'
 * '<S8>'   : 'actrec_export/Check if there are any peaks over walking threshold/Compare To Constant1'
 * '<S9>'   : 'actrec_export/Check if there are exactly two peaks over a threshold/Compare To Constant1'
 */
#endif                                 /* RTW_HEADER_actrec_export_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
