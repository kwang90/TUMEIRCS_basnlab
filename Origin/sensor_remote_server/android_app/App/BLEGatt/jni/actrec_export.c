/*
 * File: actrec_export.c
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

#include "actrec_export.h"
#define NumBitsPerChar                 8U

/* Block parameters (auto storage) */
P_actrec_export_T actrec_export_P = {
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<Root>/No relevant Motion Activity'
                                        */
  -1.0,                                /* Expression: -1
                                        * Referenced by: '<Root>/Unknown'
                                        */
  1.0,                                 /* Expression: 1
                                        * Referenced by: '<Root>/Walking Activity'
                                        */
  1.0,                                 /* Expression: 1.00
                                        * Referenced by: '<Root>/Constant4'
                                        */
  2.0,                                 /* Expression: 2
                                        * Referenced by: '<Root>/Running Activity'
                                        */
  3.0E+6,                              /* Expression: 3e6
                                        * Referenced by: '<Root>/Constant2'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S2>/Constant'
                                        */
  1.4E+6,                              /* Expression: 14e5
                                        * Referenced by: '<Root>/Constant1'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<S1>/Constant'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<Root>/256-Frame2'
                                        */
  0.05,                                /* Expression: 0.05
                                        * Referenced by: '<Root>/Constant3'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<Root>/256-Frame'
                                        */
  0.0,                                 /* Expression: 0
                                        * Referenced by: '<Root>/256-Frame3'
                                        */
  0U,                                  /* Computed Parameter: Constant_Value_o
                                        * Referenced by: '<S8>/Constant'
                                        */
  2U                                   /* Computed Parameter: Constant_Value_m
                                        * Referenced by: '<S9>/Constant'
                                        */
};

/* Constant parameters (auto storage) */
const ConstP_actrec_export_T actrec_export_ConstP = {
  /* Pooled Parameter (Expression: )
   * Referenced by:
   *   '<S5>/FFT'
   *   '<S6>/FFT'
   */
  { 1.0, 0.99969881869620425, 0.99879545620517241, 0.99729045667869021,
    0.99518472667219693, 0.99247953459871, 0.989176509964781,
    0.98527764238894122, 0.98078528040323043, 0.97570213003852857,
    0.970031253194544, 0.96377606579543984, 0.95694033573220882,
    0.94952818059303667, 0.94154406518302081, 0.932992798834739,
    0.92387953251128674, 0.91420975570353069, 0.90398929312344334,
    0.89322430119551532, 0.881921264348355, 0.87008699110871146,
    0.85772861000027212, 0.84485356524970712, 0.83146961230254524,
    0.81758481315158371, 0.80320753148064494, 0.78834642762660634,
    0.773010453362737, 0.75720884650648457, 0.74095112535495911,
    0.724247082951467, 0.70710678118654757, 0.68954054473706694,
    0.67155895484701833, 0.65317284295377676, 0.63439328416364549,
    0.61523159058062682, 0.59569930449243347, 0.57580819141784534,
    0.55557023301960229, 0.53499761988709726, 0.51410274419322166,
    0.49289819222978409, 0.47139673682599781, 0.4496113296546066,
    0.4275550934302822, 0.40524131400498986, 0.38268343236508984,
    0.35989503653498828, 0.33688985339222005, 0.31368174039889157,
    0.29028467725446233, 0.26671275747489842, 0.24298017990326398,
    0.21910124015686977, 0.19509032201612833, 0.17096188876030136,
    0.14673047445536175, 0.12241067519921628, 0.09801714032956077,
    0.073564563599667454, 0.049067674327418126, 0.024541228522912264,
    6.123233995736766E-17, -0.024541228522912142, -0.049067674327418008,
    -0.073564563599667329, -0.098017140329560645, -0.12241067519921615,
    -0.14673047445536164, -0.17096188876030124, -0.19509032201612819,
    -0.21910124015686966, -0.24298017990326387, -0.26671275747489831,
    -0.29028467725446216, -0.31368174039889141, -0.33688985339221994,
    -0.35989503653498817, -0.38268343236508973, -0.40524131400498975,
    -0.42755509343028186, -0.44961132965460671, -0.4713967368259977,
    -0.492898192229784, -0.51410274419322166, -0.534997619887097,
    -0.555570233019602, -0.57580819141784534, -0.59569930449243336,
    -0.61523159058062671, -0.63439328416364538, -0.65317284295377653,
    -0.67155895484701844, -0.68954054473706694, -0.70710678118654746,
    -0.72424708295146678, -0.74095112535495888, -0.75720884650648457,
    -0.773010453362737, -0.78834642762660623, -0.80320753148064483,
    -0.8175848131515836, -0.83146961230254535, -0.84485356524970712,
    -0.857728610000272, -0.87008699110871135, -0.88192126434835494,
    -0.89322430119551521, -0.90398929312344334, -0.91420975570353069,
    -0.92387953251128674, -0.93299279883473885, -0.9415440651830207,
    -0.94952818059303667, -0.95694033573220882, -0.96377606579543984,
    -0.970031253194544, -0.97570213003852846, -0.98078528040323043,
    -0.98527764238894122, -0.989176509964781, -0.99247953459871,
    -0.99518472667219682, -0.99729045667869021, -0.99879545620517241,
    -0.99969881869620425, -1.0, -0.99969881869620425, -0.99879545620517241,
    -0.99729045667869021, -0.99518472667219693, -0.99247953459871,
    -0.989176509964781, -0.98527764238894133, -0.98078528040323043,
    -0.97570213003852857, -0.970031253194544, -0.96377606579544,
    -0.95694033573220894, -0.94952818059303679, -0.94154406518302081,
    -0.932992798834739, -0.92387953251128685, -0.91420975570353069,
    -0.90398929312344345, -0.89322430119551532, -0.881921264348355,
    -0.87008699110871146, -0.85772861000027212, -0.84485356524970723,
    -0.83146961230254546, -0.81758481315158371, -0.80320753148064494,
    -0.78834642762660634, -0.7730104533627371, -0.75720884650648479,
    -0.74095112535495911, -0.724247082951467, -0.70710678118654768,
    -0.689540544737067, -0.67155895484701866, -0.65317284295377709,
    -0.63439328416364593, -0.61523159058062726, -0.59569930449243313,
    -0.57580819141784523, -0.55557023301960218, -0.53499761988709726,
    -0.51410274419322177, -0.4928981922297842, -0.47139673682599786,
    -0.44961132965460693, -0.42755509343028247, -0.40524131400499036,
    -0.38268343236509034, -0.35989503653498794, -0.33688985339221994,
    -0.31368174039889146, -0.29028467725446244, -0.26671275747489853,
    -0.24298017990326412, -0.2191012401568701, -0.19509032201612866,
    -0.17096188876030169, -0.1467304744553623, -0.12241067519921596,
    -0.098017140329560451, -0.073564563599667357, -0.049067674327418029,
    -0.024541228522912389 }
};

/* Block signals (auto storage) */
B_actrec_export_T actrec_export_B;

/* Block states (auto storage) */
DW_actrec_export_T actrec_export_DW;

/* External inputs (root inport signals with auto storage) */
ExtU_actrec_export_T actrec_export_U;

/* External outputs (root outports fed by signals with auto storage) */
ExtY_actrec_export_T actrec_export_Y;

/* Real-time model */
RT_MODEL_actrec_export_T actrec_export_M_;
RT_MODEL_actrec_export_T *const actrec_export_M = &actrec_export_M_;
extern void MWDSPCG_FFT_Interleave_R2BR_D(const real_T x[], creal_T y[], const
  int32_T nChans, const int32_T nRows);
extern void MWDSPCG_R2DIT_TBLS_Z(creal_T y[], const int32_T nChans, const
  int32_T nRows, const int32_T fftLen, const int32_T offset, const real_T
  tablePtr[], const int32_T twiddleStep, const boolean_T isInverse);
extern void MWDSPCG_FFT_DblLen_Z(creal_T y[], const int32_T nChans, const
  int32_T nRows, const real_T twiddleTable[], const int32_T twiddleStep);
static void rate_scheduler(void);
extern real_T rtInf;
extern real_T rtMinusInf;
extern real_T rtNaN;
extern real32_T rtInfF;
extern real32_T rtMinusInfF;
extern real32_T rtNaNF;
extern void rt_InitInfAndNaN(size_t realSize);
extern boolean_T rtIsInf(real_T value);
extern boolean_T rtIsInfF(real32_T value);
extern boolean_T rtIsNaN(real_T value);
extern boolean_T rtIsNaNF(real32_T value);
typedef struct {
  struct {
    uint32_T wordH;
    uint32_T wordL;
  } words;
} BigEndianIEEEDouble;

typedef struct {
  struct {
    uint32_T wordL;
    uint32_T wordH;
  } words;
} LittleEndianIEEEDouble;

typedef struct {
  union {
    real32_T wordLreal;
    uint32_T wordLuint;
  } wordL;
} IEEESingle;

real_T rtInf;
real_T rtMinusInf;
real_T rtNaN;
real32_T rtInfF;
real32_T rtMinusInfF;
real32_T rtNaNF;
extern real_T rtGetInf(void);
extern real32_T rtGetInfF(void);
extern real_T rtGetMinusInf(void);
extern real32_T rtGetMinusInfF(void);
extern real_T rtGetNaN(void);
extern real32_T rtGetNaNF(void);

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
void rt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
boolean_T rtIsInf(real_T value)
{
  return (boolean_T)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
boolean_T rtIsInfF(real32_T value)
{
  return (boolean_T)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
boolean_T rtIsNaN(real_T value)
{
  return (boolean_T)((value!=value) ? 1U : 0U);
}

/* Test if single-precision value is not a number */
boolean_T rtIsNaNF(real32_T value)
{
  return (boolean_T)(((value!=value) ? 1U : 0U));
}

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0x7FF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    inf = tmpVal.fltVal;
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetMinusInf(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF00000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    minf = tmpVal.fltVal;
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
real_T rtGetNaN(void)
{
  size_t bitsPerReal = sizeof(real_T) * (NumBitsPerChar);
  real_T nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      real_T fltVal;
    } tmpVal;

    tmpVal.bitVal.words.wordH = 0xFFF80000U;
    tmpVal.bitVal.words.wordL = 0x00000000U;
    nan = tmpVal.fltVal;
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
real32_T rtGetNaNF(void)
{
  IEEESingle nanF = { { 0 } };

  nanF.wordL.wordLuint = 0xFFC00000U;
  return nanF.wordL.wordLreal;
}

/*
 *   This function updates active task flag for each subrate.
 * The function is called at model base rate, hence the
 * generated code self-manages all its subrates.
 */
static void rate_scheduler(void)
{
  /* Compute which subrates run during the next base time step.  Subrates
   * are an integer multiple of the base rate counter.  Therefore, the subtask
   * counter is reset when it reaches its limit (zero means run).
   */
  (actrec_export_M->Timing.TaskCounters.TID[1])++;
  if ((actrec_export_M->Timing.TaskCounters.TID[1]) > 2) {/* Sample time: [1.0s, 0.0s] */
    actrec_export_M->Timing.TaskCounters.TID[1] = 0;
  }

  (actrec_export_M->Timing.TaskCounters.TID[2])++;
  if ((actrec_export_M->Timing.TaskCounters.TID[2]) > 63) {/* Sample time: [21.333333333333332s, 0.0s] */
    actrec_export_M->Timing.TaskCounters.TID[2] = 0;
  }

  (actrec_export_M->Timing.TaskCounters.TID[3])++;
  if ((actrec_export_M->Timing.TaskCounters.TID[3]) > 95) {/* Sample time: [32.0s, 0.0s] */
    actrec_export_M->Timing.TaskCounters.TID[3] = 0;
  }

  (actrec_export_M->Timing.TaskCounters.TID[4])++;
  if ((actrec_export_M->Timing.TaskCounters.TID[4]) > 191) {/* Sample time: [64.0s, 0.0s] */
    actrec_export_M->Timing.TaskCounters.TID[4] = 0;
  }
}

void MWDSPCG_FFT_Interleave_R2BR_D(const real_T x[], creal_T y[], const int32_T
  nChans, const int32_T nRows)
{
  int32_T br_j;
  int32_T yIdx;
  int32_T uIdx;
  int32_T j;
  int32_T nChansBy;
  int32_T bit_fftLen;

  /* S-Function (sdspfft2): '<S5>/FFT' */
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

  /* End of S-Function (sdspfft2): '<S5>/FFT' */
}

void MWDSPCG_R2DIT_TBLS_Z(creal_T y[], const int32_T nChans, const int32_T nRows,
  const int32_T fftLen, const int32_T offset, const real_T tablePtr[], const
  int32_T twiddleStep, const boolean_T isInverse)
{
  int32_T nHalf;
  real_T twidRe;
  real_T twidIm;
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
  real_T tmp_re;
  real_T tmp_im;

  /* S-Function (sdspfft2): '<S5>/FFT' */
  /* DSP System Toolbox Decimation in Time FFT  */
  /* Computation performed using table lookup  */
  /* Output type: complex real_T */
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
        twidIm = tablePtr[j + nQtr] * (real_T)fwdInvFactor;
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

  /* End of S-Function (sdspfft2): '<S5>/FFT' */
}

void MWDSPCG_FFT_DblLen_Z(creal_T y[], const int32_T nChans, const int32_T nRows,
  const real_T twiddleTable[], const int32_T twiddleStep)
{
  real_T tempOut0Re;
  real_T tempOut0Im;
  real_T tempOut1Re;
  real_T temp2Re;
  int32_T N;
  int32_T N_0;
  int32_T W;
  int32_T yIdx;
  int32_T ix;
  int32_T k;
  real_T cTemp_re;
  real_T cTemp_im;

  /* S-Function (sdspfft2): '<S5>/FFT' */
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
    y[N + yIdx].im = 0.0;
  }

  tempOut0Re = y[yIdx].re;
  tempOut0Re += y[yIdx].im;
  y[yIdx].re = tempOut0Re;
  y[yIdx].im = 0.0;
  k = twiddleStep;
  for (ix = 1; ix < N_0; ix++) {
    tempOut0Re = y[ix + yIdx].re;
    tempOut0Re += y[(N - ix) + yIdx].re;
    tempOut0Re /= 2.0;
    temp2Re = tempOut0Re;
    tempOut0Re = y[ix + yIdx].im;
    tempOut0Re -= y[(N - ix) + yIdx].im;
    tempOut0Re /= 2.0;
    tempOut0Im = tempOut0Re;
    tempOut0Re = y[ix + yIdx].im;
    tempOut0Re += y[(N - ix) + yIdx].im;
    tempOut0Re /= 2.0;
    tempOut1Re = tempOut0Re;
    tempOut0Re = y[(N - ix) + yIdx].re;
    tempOut0Re -= y[ix + yIdx].re;
    tempOut0Re /= 2.0;
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

  /* End of S-Function (sdspfft2): '<S5>/FFT' */
}

/* Model step function */
void actrec_export_step(void)
{
  int32_T m;
  int32_T i;
  int32_T count;
  int32_T endIdx;
  int32_T outIdx;
  int32_T currentOffset;
  real_T rtb_VectorConcatenate1[3];
  real_T rtb_ActivityOutput;
  boolean_T rtb_RelationalOperator_b;
  int32_T rtb_PeakFinder;
  int32_T rtb_PeakFinder_o1;
  creal_T rtb_FFT[256];
  real_T rtb_MagnitudeSquared[256];
  real_T rtb_u6Frame2[256];
  real_T u;
  if (actrec_export_M->Timing.TaskCounters.TID[2] == 0) {
    /* Sum: '<S2>/Sum1' incorporates:
     *  Constant: '<Root>/Constant2'
     */
    u = 0.0 - actrec_export_P.Constant2_Value;

    /* MinMax: '<S2>/MinMax' incorporates:
     *  Constant: '<S2>/Constant'
     */
    if ((u >= actrec_export_P.Constant_Value) || rtIsNaN
        (actrec_export_P.Constant_Value)) {
      rtb_VectorConcatenate1[0] = u;
    } else {
      rtb_VectorConcatenate1[0] = actrec_export_P.Constant_Value;
    }

    /* Sum: '<S2>/Sum1' incorporates:
     *  Constant: '<Root>/Constant2'
     */
    u = 0.0 - actrec_export_P.Constant2_Value;

    /* MinMax: '<S2>/MinMax' incorporates:
     *  Constant: '<S2>/Constant'
     */
    if ((u >= actrec_export_P.Constant_Value) || rtIsNaN
        (actrec_export_P.Constant_Value)) {
      rtb_VectorConcatenate1[1] = u;
    } else {
      rtb_VectorConcatenate1[1] = actrec_export_P.Constant_Value;
    }

    /* Sum: '<S2>/Sum1' incorporates:
     *  Constant: '<Root>/Constant2'
     */
    u = 0.0 - actrec_export_P.Constant2_Value;

    /* MinMax: '<S2>/MinMax' incorporates:
     *  Constant: '<S2>/Constant'
     */
    if ((u >= actrec_export_P.Constant_Value) || rtIsNaN
        (actrec_export_P.Constant_Value)) {
      rtb_VectorConcatenate1[2] = u;
    } else {
      rtb_VectorConcatenate1[2] = actrec_export_P.Constant_Value;
    }

    /* S-Function (sdsppeaks): '<S2>/Peak Finder' incorporates:
     *  MinMax: '<S2>/MinMax'
     */
    /* with noise rejection and scalar threshold != 0 */
    count = 0;
    for (i = 0; i + 2 < 3; i++) {
      if ((count < 10) && ((rtb_VectorConcatenate1[i + 1] -
                            rtb_VectorConcatenate1[i] > 1.0E+6) &&
                           (rtb_VectorConcatenate1[i + 1] -
                            rtb_VectorConcatenate1[i + 2] > 1.0E+6))) {
        /* To find extrema, we need three adjacent points mU[i-2,i-1,i]  */
        /* Compute Curr - Prev  */
        /* Compute Curr - Next */
        /* Local minimum with Threshold  */
        count++;
      }
    }

    rtb_PeakFinder = count;

    /* End of S-Function (sdsppeaks): '<S2>/Peak Finder' */

    /* Sum: '<S1>/Sum1' incorporates:
     *  Constant: '<Root>/Constant1'
     */
    u = 0.0 - actrec_export_P.Constant1_Value;

    /* MinMax: '<S1>/MinMax' incorporates:
     *  Constant: '<S1>/Constant'
     */
    if ((u >= actrec_export_P.Constant_Value_c) || rtIsNaN
        (actrec_export_P.Constant_Value_c)) {
      rtb_VectorConcatenate1[0] = u;
    } else {
      rtb_VectorConcatenate1[0] = actrec_export_P.Constant_Value_c;
    }

    /* Sum: '<S1>/Sum1' incorporates:
     *  Constant: '<Root>/Constant1'
     */
    u = 0.0 - actrec_export_P.Constant1_Value;

    /* MinMax: '<S1>/MinMax' incorporates:
     *  Constant: '<S1>/Constant'
     */
    if ((u >= actrec_export_P.Constant_Value_c) || rtIsNaN
        (actrec_export_P.Constant_Value_c)) {
      rtb_VectorConcatenate1[1] = u;
    } else {
      rtb_VectorConcatenate1[1] = actrec_export_P.Constant_Value_c;
    }

    /* Sum: '<S1>/Sum1' incorporates:
     *  Constant: '<Root>/Constant1'
     */
    u = 0.0 - actrec_export_P.Constant1_Value;

    /* MinMax: '<S1>/MinMax' incorporates:
     *  Constant: '<S1>/Constant'
     */
    if ((u >= actrec_export_P.Constant_Value_c) || rtIsNaN
        (actrec_export_P.Constant_Value_c)) {
      rtb_VectorConcatenate1[2] = u;
    } else {
      rtb_VectorConcatenate1[2] = actrec_export_P.Constant_Value_c;
    }

    /* S-Function (sdsppeaks): '<S1>/Peak Finder' incorporates:
     *  MinMax: '<S1>/MinMax'
     */
    memset(&actrec_export_B.PeakFinder_o2[0], 0, 10U * sizeof(real_T));

    /* no noise rejection or scalar threshold = 0 */
    outIdx = 0;
    count = 0;

    /* find peaks for each individual channels */
    for (i = 1; i + 1 < 3; i++) {
      if ((count < 10) && ((rtb_VectorConcatenate1[i] > rtb_VectorConcatenate1[i
                            - 1]) && (rtb_VectorConcatenate1[i] >
            rtb_VectorConcatenate1[i + 1]))) {
        /* Local maximum without threshold */
        actrec_export_B.PeakFinder_o2[outIdx] = rtb_VectorConcatenate1[i];
        count++;
        outIdx++;
      }
    }

    rtb_PeakFinder_o1 = count;

    /* End of S-Function (sdsppeaks): '<S1>/Peak Finder' */
  }

  if (actrec_export_M->Timing.TaskCounters.TID[1] == 0) {
    /* Abs: '<Root>/Abs1' incorporates:
     *  Abs: '<Root>/Abs'
     *  Inport: '<Root>/X axis acceleration'
     *  Inport: '<Root>/Y axis acceleration'
     *  Inport: '<Root>/Z axis acceleration'
     *  SignalConversion: '<Root>/ConcatBufferAtVector Concatenate1In2'
     *  SignalConversion: '<Root>/ConcatBufferAtVector ConcatenateIn1'
     *  SignalConversion: '<Root>/ConcatBufferAtVector ConcatenateIn2'
     */
    actrec_export_B.Abs1[0] = fabs(actrec_export_U.Xaxisacceleration);
    actrec_export_B.Abs1[1] = fabs(actrec_export_U.Yaxisacceleration);
    actrec_export_B.Abs1[2] = fabs(actrec_export_U.Zaxisacceleration);

    /* Buffer: '<Root>/256-Frame2' */
    m = 0;
    outIdx = 3;
    count = 512 - actrec_export_DW.u6Frame2_inBufPtrIdx;
    endIdx = actrec_export_DW.u6Frame2_inBufPtrIdx;
    if (count <= 3) {
      for (i = 0; i < count; i++) {
        actrec_export_DW.u6Frame2_CircBuf[actrec_export_DW.u6Frame2_inBufPtrIdx
          + i] = actrec_export_B.Abs1[i];
      }

      m = count;
      endIdx = 0;
      outIdx = 3 - count;
    }

    for (i = 0; i < outIdx; i++) {
      actrec_export_DW.u6Frame2_CircBuf[endIdx + i] = actrec_export_B.Abs1[m + i];
    }

    actrec_export_DW.u6Frame2_inBufPtrIdx += 3;
    if (actrec_export_DW.u6Frame2_inBufPtrIdx >= 512) {
      actrec_export_DW.u6Frame2_inBufPtrIdx -= 512;
    }

    actrec_export_DW.u6Frame2_bufferLength += 3;
    if (actrec_export_DW.u6Frame2_bufferLength > 512) {
      actrec_export_DW.u6Frame2_outBufPtrIdx =
        (actrec_export_DW.u6Frame2_outBufPtrIdx +
         actrec_export_DW.u6Frame2_bufferLength) - 512;
      if (actrec_export_DW.u6Frame2_outBufPtrIdx > 512) {
        actrec_export_DW.u6Frame2_outBufPtrIdx -= 512;
      }
    }
  }

  /* Buffer: '<Root>/256-Frame2' */
  if (actrec_export_M->Timing.TaskCounters.TID[2] == 0) {
    actrec_export_DW.u6Frame2_bufferLength -= 64;
    if (actrec_export_DW.u6Frame2_bufferLength < 192) {
      actrec_export_DW.u6Frame2_outBufPtrIdx =
        (actrec_export_DW.u6Frame2_outBufPtrIdx +
         actrec_export_DW.u6Frame2_bufferLength) - 192;
      if (actrec_export_DW.u6Frame2_outBufPtrIdx < 0) {
        actrec_export_DW.u6Frame2_outBufPtrIdx += 512;
      }

      actrec_export_DW.u6Frame2_bufferLength = 192;
    }

    m = 0;
    currentOffset = actrec_export_DW.u6Frame2_outBufPtrIdx - 192;
    if (currentOffset < 0) {
      currentOffset += 512;
    }

    count = 512 - currentOffset;
    outIdx = 256;
    if (count <= 256) {
      for (i = 0; i < count; i++) {
        rtb_u6Frame2[i] = actrec_export_DW.u6Frame2_CircBuf[currentOffset + i];
      }

      m = count;
      currentOffset = 0;
      outIdx = 256 - count;
    }

    for (i = 0; i < outIdx; i++) {
      rtb_u6Frame2[m + i] = actrec_export_DW.u6Frame2_CircBuf[currentOffset + i];
    }

    actrec_export_DW.u6Frame2_outBufPtrIdx = currentOffset + outIdx;

    /* S-Function (sdspstatfcns): '<S3>/Variance' */
    for (i = 0; i < 256; i += 256) {
      for (currentOffset = i; currentOffset < i + 1; currentOffset++) {
        actrec_export_DW.Variance_AccVal_a = rtb_u6Frame2[currentOffset];
        actrec_export_DW.Variance_SqData_i = rtb_u6Frame2[currentOffset] *
          rtb_u6Frame2[currentOffset];
        m = 1;
        for (endIdx = 254; endIdx >= 0; endIdx += -1) {
          count = currentOffset + m;
          actrec_export_DW.Variance_AccVal_a += rtb_u6Frame2[count];
          actrec_export_DW.Variance_SqData_i += rtb_u6Frame2[count] *
            rtb_u6Frame2[count];
          m++;
        }

        rtb_ActivityOutput = (actrec_export_DW.Variance_SqData_i -
                              actrec_export_DW.Variance_AccVal_a *
                              actrec_export_DW.Variance_AccVal_a / 256.0) /
          255.0;
      }
    }

    /* End of S-Function (sdspstatfcns): '<S3>/Variance' */

    /* RelationalOperator: '<S3>/Relational Operator' incorporates:
     *  Constant: '<Root>/Constant3'
     */
    rtb_RelationalOperator_b = (rtb_ActivityOutput >=
      actrec_export_P.Constant3_Value);

    /* Switch: '<Root>/Activity Output' */
    if (rtb_RelationalOperator_b) {
      /* Switch: '<Root>/Switch1' incorporates:
       *  Constant: '<Root>/Constant4'
       *  Constant: '<S8>/Constant'
       *  Constant: '<S9>/Constant'
       *  Logic: '<Root>/Logical Operator'
       *  RelationalOperator: '<S4>/Relational Operator'
       *  RelationalOperator: '<S8>/Compare'
       *  RelationalOperator: '<S9>/Compare'
       *  Switch: '<Root>/Switch2'
       */
      if ((uint32_T)rtb_PeakFinder == actrec_export_P.Constant_Value_m) {
        /* Outport: '<Root>/Activity' incorporates:
         *  Constant: '<Root>/Running Activity'
         */
        actrec_export_Y.Activity = actrec_export_P.RunningActivity_Value;
      } else if ((0.0 >= actrec_export_P.Constant4_Value) && ((uint32_T)
                  rtb_PeakFinder_o1 >= actrec_export_P.Constant_Value_o)) {
        /* Switch: '<Root>/Switch2' incorporates:
         *  Constant: '<Root>/Walking Activity'
         *  Outport: '<Root>/Activity'
         */
        actrec_export_Y.Activity = actrec_export_P.WalkingActivity_Value;
      } else {
        /* Outport: '<Root>/Activity' incorporates:
         *  Constant: '<Root>/Unknown'
         *  Switch: '<Root>/Switch2'
         */
        actrec_export_Y.Activity = actrec_export_P.Unknown_Value;
      }
    } else {
      /* Outport: '<Root>/Activity' incorporates:
       *  Constant: '<Root>/No relevant Motion Activity'
       *  Switch: '<Root>/Switch1'
       */
      actrec_export_Y.Activity = actrec_export_P.NorelevantMotionActivity_Value;
    }

    /* End of Switch: '<Root>/Activity Output' */
  }

  /* Buffer: '<Root>/256-Frame' incorporates:
   *  Inport: '<Root>/Z axis acceleration'
   */
  if (actrec_export_M->Timing.TaskCounters.TID[1] == 0) {
    outIdx = 1;
    count = 512 - actrec_export_DW.u6Frame_inBufPtrIdx;
    endIdx = actrec_export_DW.u6Frame_inBufPtrIdx;
    if (count <= 1) {
      i = 0;
      while (i < count) {
        actrec_export_DW.u6Frame_CircBuf[actrec_export_DW.u6Frame_inBufPtrIdx] =
          actrec_export_U.Zaxisacceleration;
        i = 1;
      }

      endIdx = 0;
      outIdx = 1 - count;
    }

    for (i = 0; i < outIdx; i++) {
      actrec_export_DW.u6Frame_CircBuf[endIdx + i] =
        actrec_export_U.Zaxisacceleration;
    }

    actrec_export_DW.u6Frame_inBufPtrIdx++;
    if (actrec_export_DW.u6Frame_inBufPtrIdx >= 512) {
      actrec_export_DW.u6Frame_inBufPtrIdx -= 512;
    }

    actrec_export_DW.u6Frame_bufferLength++;
    if (actrec_export_DW.u6Frame_bufferLength > 512) {
      actrec_export_DW.u6Frame_outBufPtrIdx =
        (actrec_export_DW.u6Frame_outBufPtrIdx +
         actrec_export_DW.u6Frame_bufferLength) - 512;
      if (actrec_export_DW.u6Frame_outBufPtrIdx > 512) {
        actrec_export_DW.u6Frame_outBufPtrIdx -= 512;
      }
    }
  }

  if (actrec_export_M->Timing.TaskCounters.TID[4] == 0) {
    actrec_export_DW.u6Frame_bufferLength -= 64;
    if (actrec_export_DW.u6Frame_bufferLength < 192) {
      actrec_export_DW.u6Frame_outBufPtrIdx =
        (actrec_export_DW.u6Frame_outBufPtrIdx +
         actrec_export_DW.u6Frame_bufferLength) - 192;
      if (actrec_export_DW.u6Frame_outBufPtrIdx < 0) {
        actrec_export_DW.u6Frame_outBufPtrIdx += 512;
      }

      actrec_export_DW.u6Frame_bufferLength = 192;
    }

    m = 0;
    currentOffset = actrec_export_DW.u6Frame_outBufPtrIdx - 192;
    if (currentOffset < 0) {
      currentOffset += 512;
    }

    count = 512 - currentOffset;
    outIdx = 256;
    if (count <= 256) {
      for (i = 0; i < count; i++) {
        rtb_MagnitudeSquared[i] = actrec_export_DW.u6Frame_CircBuf[currentOffset
          + i];
      }

      m = count;
      currentOffset = 0;
      outIdx = 256 - count;
    }

    for (i = 0; i < outIdx; i++) {
      rtb_MagnitudeSquared[m + i] =
        actrec_export_DW.u6Frame_CircBuf[currentOffset + i];
    }

    actrec_export_DW.u6Frame_outBufPtrIdx = currentOffset + outIdx;

    /* S-Function (sdspfft2): '<S5>/FFT' */
    MWDSPCG_FFT_Interleave_R2BR_D(&rtb_MagnitudeSquared[0U], &rtb_FFT[0U], 1,
      256);
    MWDSPCG_R2DIT_TBLS_Z(&rtb_FFT[0U], 1, 256, 128, 0,
                         actrec_export_ConstP.pooled1, 2, FALSE);
    MWDSPCG_FFT_DblLen_Z(&rtb_FFT[0U], 1, 256, actrec_export_ConstP.pooled1, 1);

    /* Math: '<S5>/Magnitude Squared'
     *
     * About '<S5>/Magnitude Squared':
     *  Operator: magnitude^2
     */
    for (i = 0; i < 256; i++) {
      rtb_MagnitudeSquared[i] = rtb_FFT[i].re * rtb_FFT[i].re + rtb_FFT[i].im *
        rtb_FFT[i].im;
    }

    /* End of Math: '<S5>/Magnitude Squared' */

    /* MinMax: '<Root>/MinMax1' */
    rtb_ActivityOutput = rtb_MagnitudeSquared[0];
    for (m = 0; m < 255; m++) {
      if (!((rtb_ActivityOutput <= rtb_MagnitudeSquared[m + 1]) || rtIsNaN
            (rtb_MagnitudeSquared[m + 1]))) {
        rtb_ActivityOutput = rtb_MagnitudeSquared[m + 1];
      }
    }

    /* Outport: '<Root>/fftOut' incorporates:
     *  MinMax: '<Root>/MinMax1'
     *  Sum: '<Root>/Sum'
     */
    for (i = 0; i < 256; i++) {
      actrec_export_Y.fftOut[i] = rtb_MagnitudeSquared[i] - rtb_ActivityOutput;
    }

    /* End of Outport: '<Root>/fftOut' */
  }

  /* End of Buffer: '<Root>/256-Frame' */
  if (actrec_export_M->Timing.TaskCounters.TID[2] == 0) {
    /* Outport: '<Root>/var' */
    actrec_export_Y.var = rtb_RelationalOperator_b;
  }

  if (actrec_export_M->Timing.TaskCounters.TID[1] == 0) {
    /* Outport: '<Root>/aux out' */
    actrec_export_Y.auxout = 0.0;

    /* Buffer: '<Root>/256-Frame3' incorporates:
     *  Inport: '<Root>/aux Z acc'
     */
    outIdx = 1;
    count = 512 - actrec_export_DW.u6Frame3_inBufPtrIdx;
    endIdx = actrec_export_DW.u6Frame3_inBufPtrIdx;
    if (count <= 1) {
      i = 0;
      while (i < count) {
        actrec_export_DW.u6Frame3_CircBuf[actrec_export_DW.u6Frame3_inBufPtrIdx]
          = actrec_export_U.auxZacc;
        i = 1;
      }

      endIdx = 0;
      outIdx = 1 - count;
    }

    for (i = 0; i < outIdx; i++) {
      actrec_export_DW.u6Frame3_CircBuf[endIdx + i] = actrec_export_U.auxZacc;
    }

    actrec_export_DW.u6Frame3_inBufPtrIdx++;
    if (actrec_export_DW.u6Frame3_inBufPtrIdx >= 512) {
      actrec_export_DW.u6Frame3_inBufPtrIdx -= 512;
    }

    actrec_export_DW.u6Frame3_bufferLength++;
    if (actrec_export_DW.u6Frame3_bufferLength > 512) {
      actrec_export_DW.u6Frame3_outBufPtrIdx =
        (actrec_export_DW.u6Frame3_outBufPtrIdx +
         actrec_export_DW.u6Frame3_bufferLength) - 512;
      if (actrec_export_DW.u6Frame3_outBufPtrIdx > 512) {
        actrec_export_DW.u6Frame3_outBufPtrIdx -= 512;
      }
    }
  }

  /* Buffer: '<Root>/256-Frame3' */
  if (actrec_export_M->Timing.TaskCounters.TID[4] == 0) {
    actrec_export_DW.u6Frame3_bufferLength -= 64;
    if (actrec_export_DW.u6Frame3_bufferLength < 192) {
      actrec_export_DW.u6Frame3_outBufPtrIdx =
        (actrec_export_DW.u6Frame3_outBufPtrIdx +
         actrec_export_DW.u6Frame3_bufferLength) - 192;
      if (actrec_export_DW.u6Frame3_outBufPtrIdx < 0) {
        actrec_export_DW.u6Frame3_outBufPtrIdx += 512;
      }

      actrec_export_DW.u6Frame3_bufferLength = 192;
    }

    m = 0;
    currentOffset = actrec_export_DW.u6Frame3_outBufPtrIdx - 192;
    if (currentOffset < 0) {
      currentOffset += 512;
    }

    count = 512 - currentOffset;
    outIdx = 256;
    if (count <= 256) {
      for (i = 0; i < count; i++) {
        rtb_MagnitudeSquared[i] =
          actrec_export_DW.u6Frame3_CircBuf[currentOffset + i];
      }

      m = count;
      currentOffset = 0;
      outIdx = 256 - count;
    }

    for (i = 0; i < outIdx; i++) {
      rtb_MagnitudeSquared[m + i] =
        actrec_export_DW.u6Frame3_CircBuf[currentOffset + i];
    }

    actrec_export_DW.u6Frame3_outBufPtrIdx = currentOffset + outIdx;

    /* S-Function (sdspfft2): '<S6>/FFT' */
    MWDSPCG_FFT_Interleave_R2BR_D(&rtb_MagnitudeSquared[0U], &rtb_FFT[0U], 1,
      256);
    MWDSPCG_R2DIT_TBLS_Z(&rtb_FFT[0U], 1, 256, 128, 0,
                         actrec_export_ConstP.pooled1, 2, FALSE);
    MWDSPCG_FFT_DblLen_Z(&rtb_FFT[0U], 1, 256, actrec_export_ConstP.pooled1, 1);

    /* Math: '<S6>/Magnitude Squared'
     *
     * About '<S6>/Magnitude Squared':
     *  Operator: magnitude^2
     */
    for (i = 0; i < 256; i++) {
      rtb_MagnitudeSquared[i] = rtb_FFT[i].re * rtb_FFT[i].re + rtb_FFT[i].im *
        rtb_FFT[i].im;
    }

    /* End of Math: '<S6>/Magnitude Squared' */

    /* MinMax: '<Root>/MinMax2' */
    rtb_ActivityOutput = rtb_MagnitudeSquared[0];
    for (m = 0; m < 255; m++) {
      if (!((rtb_ActivityOutput <= rtb_MagnitudeSquared[m + 1]) || rtIsNaN
            (rtb_MagnitudeSquared[m + 1]))) {
        rtb_ActivityOutput = rtb_MagnitudeSquared[m + 1];
      }
    }

    /* Outport: '<Root>/aux fft' incorporates:
     *  MinMax: '<Root>/MinMax2'
     *  Sum: '<Root>/Sum1'
     */
    for (i = 0; i < 256; i++) {
      actrec_export_Y.auxfft[i] = rtb_MagnitudeSquared[i] - rtb_ActivityOutput;
    }

    /* End of Outport: '<Root>/aux fft' */
  }

  rate_scheduler();
}

/* Model initialize function */
void actrec_export_initialize(void)
{
  /* Registration code */

  /* initialize non-finites */
  rt_InitInfAndNaN(sizeof(real_T));

  /* initialize real-time model */
  (void) memset((void *)actrec_export_M, 0,
                sizeof(RT_MODEL_actrec_export_T));

  /* block I/O */
  (void) memset(((void *) &actrec_export_B), 0,
                sizeof(B_actrec_export_T));

  /* states (dwork) */
  (void) memset((void *)&actrec_export_DW, 0,
                sizeof(DW_actrec_export_T));

  /* external inputs */
  (void) memset((void *)&actrec_export_U, 0,
                sizeof(ExtU_actrec_export_T));

  /* external outputs */
  (void) memset((void *)&actrec_export_Y, 0,
                sizeof(ExtY_actrec_export_T));

  {
    int32_T i;

    /* InitializeConditions for Buffer: '<Root>/256-Frame2' */
    actrec_export_DW.u6Frame2_inBufPtrIdx = 256;
    actrec_export_DW.u6Frame2_bufferLength = 256;
    actrec_export_DW.u6Frame2_outBufPtrIdx = 192;

    /* InitializeConditions for Buffer: '<Root>/256-Frame' */
    actrec_export_DW.u6Frame_inBufPtrIdx = 256;
    actrec_export_DW.u6Frame_bufferLength = 256;
    actrec_export_DW.u6Frame_outBufPtrIdx = 192;
    for (i = 0; i < 512; i++) {
      /* InitializeConditions for Buffer: '<Root>/256-Frame2' */
      actrec_export_DW.u6Frame2_CircBuf[i] = actrec_export_P.u6Frame2_ic;

      /* InitializeConditions for Buffer: '<Root>/256-Frame' */
      actrec_export_DW.u6Frame_CircBuf[i] = actrec_export_P.u6Frame_ic;

      /* InitializeConditions for Buffer: '<Root>/256-Frame3' */
      actrec_export_DW.u6Frame3_CircBuf[i] = actrec_export_P.u6Frame3_ic;
    }

    /* InitializeConditions for Buffer: '<Root>/256-Frame3' */
    actrec_export_DW.u6Frame3_inBufPtrIdx = 256;
    actrec_export_DW.u6Frame3_bufferLength = 256;
    actrec_export_DW.u6Frame3_outBufPtrIdx = 192;
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
