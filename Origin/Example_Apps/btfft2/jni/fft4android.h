/*
 * File: fft4android.h
 *
 * Code generated for Simulink model 'fft4android'.
 *
 * Model version                  : 1.50
 * Simulink Coder version         : 8.4 (R2013a) 13-Feb-2013
 * TLC version                    : 8.4 (Jan 18 2013)
 * C/C++ source code generated on : Mon Apr 15 14:54:43 2013
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Passed (8), Warnings (4), Error (0)
 */

#ifndef RTW_HEADER_fft4android_h_
#define RTW_HEADER_fft4android_h_
#ifndef fft4android_COMMON_INCLUDES_
# define fft4android_COMMON_INCLUDES_
#include <math.h>
#include <string.h>
#include "rtwtypes.h"
#endif                                 /* fft4android_COMMON_INCLUDES_ */

/* Macros for accessing real-time model data structure */

/* Constant parameters (auto storage) */
typedef struct {
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<S1>/FFT'
   */
  real32_T FFT_TwiddleTable[192];
} ConstP_fft4android_T;

/* External inputs (root inport signals with auto storage) */
typedef struct {
  real32_T In1[256];                   /* '<Root>/In1' */
} ExtU_fft4android_T;

/* External outputs (root outports fed by signals with auto storage) */
typedef struct {
  real32_T Out1[256];                  /* '<Root>/Out1' */
} ExtY_fft4android_T;

/* External inputs (root inport signals with auto storage) */
extern ExtU_fft4android_T fft4android_U;

/* External outputs (root outports fed by signals with auto storage) */
extern ExtY_fft4android_T fft4android_Y;

/* Constant parameters (auto storage) */
extern const ConstP_fft4android_T fft4android_ConstP;

/* Model entry point functions */
extern void fft4android_initialize(void);
extern void fft4android_step(void);

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
 * '<Root>' : 'fft4android'
 * '<S1>'   : 'fft4android/Magnitude FFT'
 */

/*-
 * Requirements for '<Root>': fft4android
 */
#endif                                 /* RTW_HEADER_fft4android_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
