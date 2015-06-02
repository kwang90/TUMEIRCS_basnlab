/*
 * File: fft4android.c
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

#include "fft4android.h"

/* Constant parameters (auto storage) */
const ConstP_fft4android_T fft4android_ConstP = {
  /* Computed Parameter: FFT_TwiddleTable
   * Referenced by: '<S1>/FFT'
   */
  { 1.0F, 0.999698818F, 0.99879545F, 0.997290432F, 0.99518472F, 0.992479563F,
    0.989176512F, 0.985277653F, 0.980785251F, 0.975702107F, 0.970031261F,
    0.963776052F, 0.956940353F, 0.949528158F, 0.941544056F, 0.932992816F,
    0.923879504F, 0.914209783F, 0.903989315F, 0.893224299F, 0.881921291F,
    0.870086968F, 0.857728601F, 0.84485358F, 0.831469595F, 0.817584813F,
    0.803207517F, 0.78834641F, 0.773010433F, 0.757208824F, 0.740951121F,
    0.724247098F, 0.707106769F, 0.689540565F, 0.671559F, 0.653172851F,
    0.634393275F, 0.615231574F, 0.59569931F, 0.575808167F, 0.555570245F,
    0.534997642F, 0.514102757F, 0.492898196F, 0.471396744F, 0.449611336F,
    0.427555084F, 0.405241311F, 0.382683426F, 0.359895051F, 0.336889863F,
    0.313681751F, 0.290284663F, 0.266712755F, 0.242980182F, 0.219101235F,
    0.195090324F, 0.170961887F, 0.146730468F, 0.122410677F, 0.0980171412F,
    0.0735645667F, 0.0490676761F, 0.024541229F, 6.12323426E-17F, -0.024541229F,
    -0.0490676761F, -0.0735645667F, -0.0980171412F, -0.122410677F, -0.146730468F,
    -0.170961887F, -0.195090324F, -0.219101235F, -0.242980182F, -0.266712755F,
    -0.290284663F, -0.313681751F, -0.336889863F, -0.359895051F, -0.382683426F,
    -0.405241311F, -0.427555084F, -0.449611336F, -0.471396744F, -0.492898196F,
    -0.514102757F, -0.534997642F, -0.555570245F, -0.575808167F, -0.59569931F,
    -0.615231574F, -0.634393275F, -0.653172851F, -0.671559F, -0.689540565F,
    -0.707106769F, -0.724247098F, -0.740951121F, -0.757208824F, -0.773010433F,
    -0.78834641F, -0.803207517F, -0.817584813F, -0.831469595F, -0.84485358F,
    -0.857728601F, -0.870086968F, -0.881921291F, -0.893224299F, -0.903989315F,
    -0.914209783F, -0.923879504F, -0.932992816F, -0.941544056F, -0.949528158F,
    -0.956940353F, -0.963776052F, -0.970031261F, -0.975702107F, -0.980785251F,
    -0.985277653F, -0.989176512F, -0.992479563F, -0.99518472F, -0.997290432F,
    -0.99879545F, -0.999698818F, -1.0F, -0.999698818F, -0.99879545F,
    -0.997290432F, -0.99518472F, -0.992479563F, -0.989176512F, -0.985277653F,
    -0.980785251F, -0.975702107F, -0.970031261F, -0.963776052F, -0.956940353F,
    -0.949528158F, -0.941544056F, -0.932992816F, -0.923879504F, -0.914209783F,
    -0.903989315F, -0.893224299F, -0.881921291F, -0.870086968F, -0.857728601F,
    -0.84485358F, -0.831469595F, -0.817584813F, -0.803207517F, -0.78834641F,
    -0.773010433F, -0.757208824F, -0.740951121F, -0.724247098F, -0.707106769F,
    -0.689540565F, -0.671559F, -0.653172851F, -0.634393275F, -0.615231574F,
    -0.59569931F, -0.575808167F, -0.555570245F, -0.534997642F, -0.514102757F,
    -0.492898196F, -0.471396744F, -0.449611336F, -0.427555084F, -0.405241311F,
    -0.382683426F, -0.359895051F, -0.336889863F, -0.313681751F, -0.290284663F,
    -0.266712755F, -0.242980182F, -0.219101235F, -0.195090324F, -0.170961887F,
    -0.146730468F, -0.122410677F, -0.0980171412F, -0.0735645667F, -0.0490676761F,
    -0.024541229F }
};

/* External inputs (root inport signals with auto storage) */
ExtU_fft4android_T fft4android_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_fft4android_T fft4android_Y;
extern void MWDSPCG_FFT_Interleave_R2BR_S(const real32_T x[], creal32_T y[],
  const int32_T nChans, const int32_T nRows);
extern void MWDSPCG_R2DIT_TBLS_C(creal32_T y[], const int32_T nChans, const
  int32_T nRows, const int32_T fftLen, const int32_T offset, const real32_T
  tablePtr[], const int32_T twiddleStep, const boolean_T isInverse);
extern void MWDSPCG_FFT_DblLen_C(creal32_T y[], const int32_T nChans, const
  int32_T nRows, const real32_T twiddleTable[], const int32_T twiddleStep);
void MWDSPCG_FFT_Interleave_R2BR_S(const real32_T x[], creal32_T y[], const
  int32_T nChans, const int32_T nRows)
{
  int32_T br_j;
  int32_T yIdx;
  int32_T uIdx;
  int32_T j;
  int32_T nChansBy;
  int32_T bit_fftLen;

  /* S-Function (sdspfft2): '<S1>/FFT' */
  /* Bit-reverses the input data simultaneously with the interleaving operation,
     obviating the need for explicit data reordering later.  This requires an
     FFT with bit-reversed inputs.
   */
  br_j = 0;
  yIdx = 0;
  uIdx = 0;
  for (nChansBy = nChans >> 1; nChansBy != 0; nChansBy--) {
    for (j = nRows; j - 1 > 0; j--) {
      y[yIdx + br_j].re = x[uIdx];
      y[yIdx + br_j].im = x[uIdx + nRows];
      uIdx++;

      /* Compute next bit-reversed destination index */
      bit_fftLen = nRows;
      do {
        bit_fftLen = (int32_T)((uint32_T)bit_fftLen >> 1);
        br_j ^= bit_fftLen;
      } while (!((br_j & bit_fftLen) != 0));
    }

    y[yIdx + br_j].re = x[uIdx];
    y[yIdx + br_j].im = x[uIdx + nRows];
    uIdx = (uIdx + nRows) + 1;
    yIdx += nRows << 1;
    br_j = 0;
  }

  /* For an odd number of channels, prepare the last channel
     for a double-length real signal algorithm.  No actual
     interleaving is required, just a copy of the last column
     of real data, but now placed in bit-reversed order.
     We need to cast the real u pointer to a cDType_T pointer,
     in order to fake the interleaving, and cut the number
     of elements in half (half as many complex interleaved
     elements as compared to real non-interleaved elements).
   */
  if ((nChans & 1) != 0) {
    for (j = nRows >> 1; j - 1 > 0; j--) {
      y[yIdx + br_j].re = x[uIdx];
      y[yIdx + br_j].im = x[uIdx + 1];
      uIdx += 2;

      /* Compute next bit-reversed destination index */
      bit_fftLen = nRows >> 1;
      do {
        bit_fftLen = (int32_T)((uint32_T)bit_fftLen >> 1);
        br_j ^= bit_fftLen;
      } while (!((br_j & bit_fftLen) != 0));
    }

    y[yIdx + br_j].re = x[uIdx];
    y[yIdx + br_j].im = x[uIdx + 1];
  }

  /* End of S-Function (sdspfft2): '<S1>/FFT' */
}

void MWDSPCG_R2DIT_TBLS_C(creal32_T y[], const int32_T nChans, const int32_T
  nRows, const int32_T fftLen, const int32_T offset, const real32_T tablePtr[],
  const int32_T twiddleStep, const boolean_T isInverse)
{
  int32_T nHalf;
  real32_T twidRe;
  real32_T twidIm;
  int32_T nQtr;
  int32_T fwdInvFactor;
  int32_T iCh;
  int32_T offsetCh;
  int32_T idelta;
  int32_T ix;
  int32_T k;
  int32_T kratio;
  int32_T istart;
  int32_T i;
  int32_T j;
  int32_T i_0;
  real32_T tmp_re;
  real32_T tmp_im;

  /* S-Function (sdspfft2): '<S1>/FFT' */
  /* DSP System Toolbox Decimation in Time FFT  */
  /* Computation performed using table lookup  */
  /* Output type: complex real32_T */
  nHalf = (fftLen >> 1) * twiddleStep;
  nQtr = nHalf >> 1;
  fwdInvFactor = isInverse ? -1 : 1;

  /* For each channel */
  offsetCh = offset;
  for (iCh = 0; iCh < nChans; iCh++) {
    /* Perform butterflies for the first stage, where no multiply is required. */
    for (ix = offsetCh; ix < (fftLen + offsetCh) - 1; ix += 2) {
      i_0 = ix + 1;
      tmp_re = y[i_0].re;
      tmp_im = y[i_0].im;
      y[i_0].re = y[ix].re - tmp_re;
      y[i_0].im = y[ix].im - tmp_im;
      y[ix].re += tmp_re;
      y[ix].im += tmp_im;
    }

    idelta = 2;
    k = fftLen >> 2;
    kratio = k * twiddleStep;
    while (k > 0) {
      i = offsetCh;

      /* Perform the first butterfly in each remaining stage, where no multiply is required. */
      for (ix = 0; ix < k; ix++) {
        i_0 = i + idelta;
        tmp_re = y[i_0].re;
        tmp_im = y[i_0].im;
        y[i_0].re = y[i].re - tmp_re;
        y[i_0].im = y[i].im - tmp_im;
        y[i].re += tmp_re;
        y[i].im += tmp_im;
        i += idelta << 1;
      }

      istart = offsetCh;

      /* Perform remaining butterflies */
      for (j = kratio; j < nHalf; j += kratio) {
        i = istart + 1;
        twidRe = tablePtr[j];
        twidIm = tablePtr[j + nQtr] * (real32_T)fwdInvFactor;
        for (ix = 0; ix < k; ix++) {
          i_0 = i + idelta;
          tmp_re = y[i_0].re * twidRe - y[i_0].im * twidIm;
          tmp_im = y[i_0].re * twidIm + y[i_0].im * twidRe;
          y[i_0].re = y[i].re - tmp_re;
          y[i_0].im = y[i].im - tmp_im;
          y[i].re += tmp_re;
          y[i].im += tmp_im;
          i += idelta << 1;
        }

        istart++;
      }

      idelta <<= 1;
      k >>= 1;
      kratio >>= 1;
    }

    /* Point to next channel */
    offsetCh += nRows;
  }

  /* End of S-Function (sdspfft2): '<S1>/FFT' */
}

void MWDSPCG_FFT_DblLen_C(creal32_T y[], const int32_T nChans, const int32_T
  nRows, const real32_T twiddleTable[], const int32_T twiddleStep)
{
  real32_T tempOut0Re;
  real32_T tempOut0Im;
  real32_T tempOut1Re;
  real32_T temp2Re;
  int32_T N;
  int32_T N_0;
  int32_T W;
  int32_T yIdx;
  int32_T ix;
  int32_T k;
  real32_T cTemp_re;
  real32_T cTemp_im;

  /* S-Function (sdspfft2): '<S1>/FFT' */
  /* In-place "double-length" data recovery
     Table-based mem-optimized twiddle computation

     Used to recover linear-ordered length-N point complex FFT result
     from a linear-ordered complex length-N/2 point FFT, performed
     on N interleaved real values.
   */
  N = nRows >> 1;
  N_0 = N >> 1;
  W = N_0 * twiddleStep;
  yIdx = (nChans - 1) * nRows;
  if (nRows > 2) {
    tempOut0Re = y[N_0 + yIdx].re;
    tempOut0Im = y[N_0 + yIdx].im;
    y[(N + N_0) + yIdx].re = tempOut0Re;
    y[(N + N_0) + yIdx].im = tempOut0Im;
    y[N_0 + yIdx].re = tempOut0Re;
    y[N_0 + yIdx].im = -tempOut0Im;
  }

  if (nRows > 1) {
    tempOut0Re = y[yIdx].re;
    tempOut0Re -= y[yIdx].im;
    y[N + yIdx].re = tempOut0Re;
    y[N + yIdx].im = 0.0F;
  }

  tempOut0Re = y[yIdx].re;
  tempOut0Re += y[yIdx].im;
  y[yIdx].re = tempOut0Re;
  y[yIdx].im = 0.0F;
  k = twiddleStep;
  for (ix = 1; ix < N_0; ix++) {
    tempOut0Re = y[ix + yIdx].re;
    tempOut0Re += y[(N - ix) + yIdx].re;
    tempOut0Re /= 2.0F;
    temp2Re = tempOut0Re;
    tempOut0Re = y[ix + yIdx].im;
    tempOut0Re -= y[(N - ix) + yIdx].im;
    tempOut0Re /= 2.0F;
    tempOut0Im = tempOut0Re;
    tempOut0Re = y[ix + yIdx].im;
    tempOut0Re += y[(N - ix) + yIdx].im;
    tempOut0Re /= 2.0F;
    tempOut1Re = tempOut0Re;
    tempOut0Re = y[(N - ix) + yIdx].re;
    tempOut0Re -= y[ix + yIdx].re;
    tempOut0Re /= 2.0F;
    y[ix + yIdx].re = tempOut1Re;
    y[ix + yIdx].im = tempOut0Re;
    cTemp_re = y[ix + yIdx].re * twiddleTable[k] - -twiddleTable[W - k] * y[ix +
      yIdx].im;
    cTemp_im = y[ix + yIdx].im * twiddleTable[k] + -twiddleTable[W - k] * y[ix +
      yIdx].re;
    tempOut1Re = cTemp_re;
    tempOut0Re = cTemp_im;
    y[ix + yIdx].re = temp2Re + cTemp_re;
    y[ix + yIdx].im = tempOut0Im + cTemp_im;
    cTemp_re = y[ix + yIdx].re;
    cTemp_im = -y[ix + yIdx].im;
    y[(nRows - ix) + yIdx].re = cTemp_re;
    y[(nRows - ix) + yIdx].im = cTemp_im;
    y[(N + ix) + yIdx].re = temp2Re - tempOut1Re;
    y[(N + ix) + yIdx].im = tempOut0Im - tempOut0Re;
    tempOut0Re = y[(N + ix) + yIdx].re;
    tempOut0Im = y[(N + ix) + yIdx].im;
    y[(N - ix) + yIdx].re = tempOut0Re;
    y[(N - ix) + yIdx].im = -tempOut0Im;
    k += twiddleStep;
  }

  /* End of S-Function (sdspfft2): '<S1>/FFT' */
}

/* Model step function */
void fft4android_step(void)
{
  creal32_T rtb_FFT[256];
  int32_T i;

  /* S-Function (sdspfft2): '<S1>/FFT' incorporates:
   *  Inport: '<Root>/In1'
   */
  MWDSPCG_FFT_Interleave_R2BR_S(&fft4android_U.In1[0U], &rtb_FFT[0U], 1, 256);
  MWDSPCG_R2DIT_TBLS_C(&rtb_FFT[0U], 1, 256, 128, 0,
                       fft4android_ConstP.FFT_TwiddleTable, 2, FALSE);
  MWDSPCG_FFT_DblLen_C(&rtb_FFT[0U], 1, 256, fft4android_ConstP.FFT_TwiddleTable,
                       1);

  /* Outport: '<Root>/Out1' */
  for (i = 0; i < 256; i++) {
    /* Abs: '<S1>/Magnitude' */
    fft4android_Y.Out1[i] = hypotf(rtb_FFT[i].re, rtb_FFT[i].im);
  }

  /* End of Outport: '<Root>/Out1' */
}

/* Model initialize function */
void fft4android_initialize(void)
{
  /* Registration code */

  /* external inputs */
  (void) memset(fft4android_U.In1, 0,
                256U*sizeof(real32_T));

  /* external outputs */
  (void) memset(&fft4android_Y.Out1[0], 0,
                256U*sizeof(real32_T));
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
