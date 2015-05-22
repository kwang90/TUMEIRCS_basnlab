
/*
#ifndef FFTREAL_H
#define FFTREAL_H
*/
#include <QList>

#if defined (FFTReal_CURRENT_HEADER)
#error Recursive inclusion of FFTReal header file.
#endif
#define	FFTReal_CURRENT_HEADER

#if ! defined (FFTReal_HEADER_INCLUDED)
#define	FFTReal_HEADER_INCLUDED


#if defined (_MSC_VER)
#pragma pack (push, 8)
#endif	// _MSC_VER

#include	<cassert>
#include	<cmath>
#include	<stdlib.h>
#include <cstdio>
#include <iostream>
#include <QList>
#include <QObject>
#include "motdetglobal.h"

//#include <e32base.h> // EDITED

/* Change this typedef to use a different flt_ting point type in your FFTs
            (i.e. flt_t, double or long double). */
    typedef float	flt_t;
    /*! \brief Fast Fourier Implementation in C++ */
    class DLL_EXPORT FFTReal //: public CBase EDITED
    {
    public:




        explicit			FFTReal (unsigned int length);
        ~FFTReal ();
          /*! \brief Calculates the fast fourier transformation for a QList float f with output x. The output array x has imaginary and real values in one array, to get the values in each array use the method getImagAndReal. Obtain the length of the array as nbr_points. Must be a potence of 2, e.g. 2^n, 2, 4, 8, 16, 32, 64, 128 etc.. */
        void				do_fft (QList<flt_t>* f, QList<flt_t>*x, const int nbr_points);

          /*! \brief Calculates the fast fourier transformation for a float array f with output QList float x. Obtain the length of the array as nbr_points. Must be a potence of 2, e.g. 2^n, 2, 4, 8, 16, 32, 64, 128 etc.. */
        void				do_fft (flt_t* f, QList<flt_t>*x, const int nbr_points);

        /*! \brief Calculates the inverse fast fourier transformation for a float array f with output x.*/
        void				do_ifft (const flt_t f [], flt_t * x) const;

        /*! \brief If the length of the float QList isn't a potence of 2, use zeroPadding to append the input list x with zeros until the next potence of 2 has been reached.*/
        void                            zeroPadding(QList<flt_t> * x, const int nbr_points,unsigned int* nextPotence);

        /*! \brief If the size of the array can vary use the checkZeroPadding method to verify if the array length is potence of 2. Returns true if zeroPadding is needed, otherwise false. The integer value nextPotence holds the value of the next possible potence of 2.*/
        bool                            checkZeroPadding(const int nbr_points,unsigned int* nextPotence);

        /*! \brief Scaling must be done if you want to retrieve x after FFT and IFFT.Actually, IFFT (FFT (x)) = x * length(x).*/
        void				rescale (flt_t x []) const;

        /*! \brief To get two QList arrays either for real and imaginary values use this method. Input the QList f array (after do_fft()) and receive the QLists real & imag. */
        void getImagAndReal(QList<flt_t> *f, const int nbr_points, QList<flt_t>* real, QList<flt_t>* imag);

    /*\\\ PRIVATE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

    /* Bit-reversed look-up table nested class */
    class BitReversedLUT
    {
    public:
        explicit			BitReversedLUT (const int nbr_bits);
        ~BitReversedLUT ();
        const long *	get_ptr () const
        {
            return (_ptr);
        }
    private:
        long *			_ptr;
    };

    /* Trigonometric look-up table nested class */
    class	TrigoLUT
    {
    public:
        explicit			TrigoLUT (const int nbr_bits);
        ~TrigoLUT ();
        const flt_t	*	get_ptr (const int level) const
        {
            return (_ptr + (1L << (level - 1)) - 4);
        };
    private:
        flt_t	*			_ptr;
    };


    const BitReversedLUT	_bit_rev_lut;
    const TrigoLUT	_trigo_lut;
    const flt_t		_sqrt2_2;
    const long		_length;
    const int		_nbr_bits;
    flt_t *			_buffer_ptr;

    /*\\\ FORBIDDEN MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

private:

    FFTReal (const FFTReal &other);
    const FFTReal&	operator = (const FFTReal &other);
    int				operator == (const FFTReal &other);
    int				operator != (const FFTReal &other);
};



#if defined (_MSC_VER)
#pragma pack (pop)
#endif	// _MSC_VER



#endif	// FFTReal_HEADER_INCLUDED
#undef FFTReal_CURRENT_HEADER

//#endif // FFTREAL_H
