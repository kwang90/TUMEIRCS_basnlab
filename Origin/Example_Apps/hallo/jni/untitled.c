/*
 * File: untitled.c
 *
 * Code generated for Simulink model 'untitled'.
 *
 * Model version                  : 1.0
 * Simulink Coder version         : 8.4 (R2013a) 13-Feb-2013
 * TLC version                    : 8.4 (Jan 18 2013)
 * C/C++ source code generated on : Fri Aug 23 10:47:22 2013
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: 32-bit Generic
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#define NULL 0

#include "untitled.h"
#include "untitled_private.h"

/* External inputs (root inport signals with auto storage) */
ExtU_untitled_T untitled_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_untitled_T untitled_Y;

/* Real-time model */
RT_MODEL_untitled_T untitled_M_;
RT_MODEL_untitled_T *const untitled_M = &untitled_M_;

/* Model step function */
void untitled_step(void)
{
  /* Outport: '<Root>/Out1' incorporates:
   *  Abs: '<Root>/Abs'
   *  Inport: '<Root>/In1'
   */
  untitled_Y.Out1 = fabs(untitled_U.In1);
}

/* Model initialize function */
void untitled_initialize(void)
{
  /* Registration code */

  /* initialize error status */
  rtmSetErrorStatus(untitled_M, (NULL));

  /* external inputs */
  untitled_U.In1 = 0.0;

  /* external outputs */
  untitled_Y.Out1 = 0.0;
}

/* Model terminate function */
void untitled_terminate(void)
{
  /* (no terminate code required) */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
