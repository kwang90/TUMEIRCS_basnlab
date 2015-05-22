#include "fftreal.h"

#if defined (_MSC_VER)
#pragma pack (push, 8)
#endif	// _MSC_VER

using namespace std;

/*\\\ PUBLIC MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

/*==========================================================================*/
/*      Name: Constructor                                                   */
/*      Input parameters:                                                   */
/*        - length: length of the array on which we want to do a FFT.       */
/*                  Range: power of 2 only, > 0.                            */
/*      Throws: std::bad_alloc, anything                                    */
/*==========================================================================*/

FFTReal::FFTReal (unsigned int length)
    :	_bit_rev_lut (int (floor (log ((double)length) / log ((double)2) + 0.5)))
    ,	_trigo_lut (int (floor (log ((double)length) / log ((double)2) + 0.5)))
    ,	_sqrt2_2 (flt_t (sqrt ((double)2) * 0.5))
    ,	_length (length)
    ,	_nbr_bits (int (floor ((double)log ((double)length) / (double)log ((double)2) + 0.5)))
{

    /*
    :    _bit_rev_lut (int (floor (log ((double)length) / log ((double)2) + 0.5)))
    ,	_trigo_lut (int (floor (log (length) / log (2) + 0.5)))
    ,	_sqrt2_2 (flt_t (sqrt (2) * 0.5))
    ,	_length (length)
    ,	_nbr_bits (int (floor (log (length) / log (2) + 0.5)))


    :	_bit_rev_lut (int (floor (log ((double)length) / log ((double)2) + 0.5)))
    ,	_trigo_lut (int (floor (log ((double)length) / log ((double)2) + 0.5)))
    ,	_sqrt2_2 (flt_t (sqrt ((double)2) * 0.5))
    ,	_length (length)
    ,	_nbr_bits (int (floor ((double)log ((double)length) / (double)log ((double)2) + 0.5)))
*/

//:	_bit_rev_lut (int (floor (log ((double)length) / log ((double)2) + 0.5)))
    //  :	_bit_rev_lut (int (floor (log (length) / log (2) + 0.5)))
    /*
    const BitReversedLUT	_bit_rev_lut;
    const TrigoLUT	_trigo_lut;
    const flt_t		_sqrt2_2;
    const long		_length;
    const int		_nbr_bits;

*/
    //assert ((1L << _nbr_bits) == length);

    _buffer_ptr = 0;
    if (_nbr_bits > 2)
    {
        _buffer_ptr = new flt_t [_length];
    }
}



/*==========================================================================*/
/*      Name: Destructor                                                    */
/*==========================================================================*/

FFTReal::~FFTReal (void)
{
    delete [] _buffer_ptr;
    _buffer_ptr = 0;
}



/*==========================================================================*/
/*      Name: do_fft                                                        */
/*      Description: Compute the FFT of the array.                          */
/*      Input parameters:                                                   */
/*        - x: pointer on the source array (time).                          */
/*      Output parameters:                                                  */
/*        - f: pointer on the destination array (frequencies).              */
/*             f [0...length(x)/2] = real values,                           */
/*             f [length(x)/2+1...length(x)-1] = imaginary values of        */
/*               coefficents 1...length(x)/2-1.                             */
/*      Throws: Nothing                                                     */
/*==========================================================================*/

void	FFTReal::do_fft (QList<flt_t>* k, QList<flt_t>* x, const int nbr_points)
{
    flt_t* f = new flt_t[nbr_points];

    /*______________________________________________
 *
 * General case
 *______________________________________________
 */

    if (_nbr_bits > 2)
    {
        flt_t *			sf;
        flt_t *			df;

        if (_nbr_bits & 1)
        {
            df = _buffer_ptr;
            sf = f;
        }
        else
        {
            df = f;
            sf = _buffer_ptr;
        }

        /* Do the transformation in several pass */
        {
            int		pass;
            long		nbr_coef;
            long		h_nbr_coef;
            long		d_nbr_coef;
            long		coef_index;

            /* First and second pass at once */
            {
                const long * const	bit_rev_lut_ptr = _bit_rev_lut.get_ptr ();
                coef_index = 0;
                do
                {
                    const long  rev_index_0 = bit_rev_lut_ptr [coef_index];
                    const long	rev_index_1 = bit_rev_lut_ptr [coef_index + 1];
                    const long	rev_index_2 = bit_rev_lut_ptr [coef_index + 2];
                    const long	rev_index_3 = bit_rev_lut_ptr [coef_index + 3];

                    flt_t	* const	df2 = df + coef_index;
                    df2 [1] = x->at(rev_index_0) - x->at(rev_index_1);
                    df2 [3] = x->at(rev_index_2) - x->at(rev_index_3);

                    const flt_t		sf_0 = x->at(rev_index_0) + x->at(rev_index_1);
                    const flt_t		sf_2 = x->at(rev_index_2) + x->at(rev_index_3);

                    df2 [0] = sf_0 + sf_2;
                    df2 [2] = sf_0 - sf_2;

                    coef_index += 4;
                }
                while (coef_index < _length);
            }

            /* Third pass */
            {
                coef_index = 0;
                const flt_t		sqrt2_2 = _sqrt2_2;
                do
                {
                    flt_t				v;

                    sf [coef_index] = df [coef_index] + df [coef_index + 4];
                    sf [coef_index + 4] = df [coef_index] - df [coef_index + 4];
                    sf [coef_index + 2] = df [coef_index + 2];
                    sf [coef_index + 6] = df [coef_index + 6];

                    v = (df [coef_index + 5] - df [coef_index + 7]) * sqrt2_2;
                    sf [coef_index + 1] = df [coef_index + 1] + v;
                    sf [coef_index + 3] = df [coef_index + 1] - v;

                    v = (df [coef_index + 5] + df [coef_index + 7]) * sqrt2_2;
                    sf [coef_index + 5] = v + df [coef_index + 3];
                    sf [coef_index + 7] = v - df [coef_index + 3];

                    coef_index += 8;
                }
                while (coef_index < _length);
            }

            /* Next pass */
            for (pass = 3; pass < _nbr_bits; ++pass)
            {
                coef_index = 0;
                nbr_coef = 1 << pass;
                h_nbr_coef = nbr_coef >> 1;
                d_nbr_coef = nbr_coef << 1;
                const flt_t	* const	cos_ptr = _trigo_lut.get_ptr (pass);
                do
                {
                    long				i;
                    const flt_t	*	const sf1r = sf + coef_index;
                    const flt_t	*	const sf2r = sf1r + nbr_coef;
                    flt_t *			const dfr = df + coef_index;
                    flt_t *			const dfi = dfr + nbr_coef;

                    /* Extreme coefficients are always real */
                    dfr [0] = sf1r [0] + sf2r [0];
                    dfi [0] = sf1r [0] - sf2r [0];	// dfr [nbr_coef] =
                    dfr [h_nbr_coef] = sf1r [h_nbr_coef];
                    dfi [h_nbr_coef] = sf2r [h_nbr_coef];

                    /* Others are conjugate complex numbers */
                    const flt_t	* const	sf1i = sf1r + h_nbr_coef;
                    const flt_t	* const	sf2i = sf1i + nbr_coef;
                    for (i = 1; i < h_nbr_coef; ++ i)
                    {
                        const flt_t		c = cos_ptr [i];					// cos (i*PI/nbr_coef);
                        const flt_t		s = cos_ptr [h_nbr_coef - i];	// sin (i*PI/nbr_coef);
                        flt_t				v;

                        v = sf2r [i] * c - sf2i [i] * s;
                        dfr [i] = sf1r [i] + v;
                        dfi [-i] = sf1r [i] - v;	// dfr [nbr_coef - i] =

                        v = sf2r [i] * s + sf2i [i] * c;
                        dfi [i] = v + sf1i [i];
                        dfi [nbr_coef - i] = v - sf1i [i];
                    }

                    coef_index += d_nbr_coef;
                }
                while (coef_index < _length);

                /* Prepare to the next pass */
                {
                    flt_t	* const		temp_ptr = df;
                    df = sf;
                    sf = temp_ptr;
                }
            }
        }
    }

    /*______________________________________________
 *
 * Special cases
 *______________________________________________
 */

    /* 4-point FFT */
    else if (_nbr_bits == 2)
    {
        f [1] = x->at(0) - x->at(2);
        f [3] = x->at(1) - x->at(3);

        const flt_t			b_0 = x->at(0) + x->at(2);
        const flt_t			b_2 = x->at(1) + x->at(3);

        f [0] = b_0 + b_2;
        f [2] = b_0 - b_2;
    }

    /* 2-point FFT */
    else if (_nbr_bits == 1)
    {
        f [0] = x->at(0) + x->at(1);
        f [1] = x->at(0) - x->at(1);
    }

    /* 1-point FFT */
    else
    {
        f [0] = x->at(0);
    }

    //to QList
    for(int m = 0; m < nbr_points; m++)
        k->append(f[m]);

    //clear all
    delete [] f;
}

void	FFTReal::do_fft (flt_t* f, QList<flt_t>* x, const int nbr_points)
{


    /*______________________________________________
 *
 * General case
 *______________________________________________
 */

    if (_nbr_bits > 2)
    {
        flt_t *			sf;
        flt_t *			df;

        if (_nbr_bits & 1)
        {
            df = _buffer_ptr;
            sf = f;
        }
        else
        {
            df = f;
            sf = _buffer_ptr;
        }

        /* Do the transformation in several pass */
        {
            int		pass;
            long		nbr_coef;
            long		h_nbr_coef;
            long		d_nbr_coef;
            long		coef_index;

            /* First and second pass at once */
            {
                const long * const	bit_rev_lut_ptr = _bit_rev_lut.get_ptr ();
                coef_index = 0;
                do
                {
                    const long       rev_index_0 = bit_rev_lut_ptr [coef_index];
                    const long	rev_index_1 = bit_rev_lut_ptr [coef_index + 1];
                    const long	rev_index_2 = bit_rev_lut_ptr [coef_index + 2];
                    const long	rev_index_3 = bit_rev_lut_ptr [coef_index + 3];

                    flt_t	* const	df2 = df + coef_index;
                    df2 [1] = x->at(rev_index_0) - x->at(rev_index_1);

//                    //this is bullshit... nothing works so printf... dammit!
//                    printf("at first Revisions Values:\n");
//                    printf("rev0 = %d ###  rev1 = %d \n" , rev_index_0,rev_index_1);
//                    printf("Now with the Qlist element x:\n");
//                    printf("x[rev1]: %4f  #### x[rev2]: %4f", x->at(rev_index_0), x->at(rev_index_1));

                    df2 [3] = x->at(rev_index_2) - x->at(rev_index_3);

                    const flt_t		sf_0 = x->at(rev_index_0) + x->at(rev_index_1);
                    const flt_t		sf_2 = x->at(rev_index_2) + x->at(rev_index_3);

                    df2 [0] = sf_0 + sf_2;
                    df2 [2] = sf_0 - sf_2;

                    coef_index += 4;
                }
                while (coef_index < _length);
           }

            /* Third pass */
            {
                coef_index = 0;
                const flt_t		sqrt2_2 = _sqrt2_2;
                do
                {
                    flt_t				v;

                    sf [coef_index] = df [coef_index] + df [coef_index + 4];
                    sf [coef_index + 4] = df [coef_index] - df [coef_index + 4];
                    sf [coef_index + 2] = df [coef_index + 2];
                    sf [coef_index + 6] = df [coef_index + 6];

                    v = (df [coef_index + 5] - df [coef_index + 7]) * sqrt2_2;
                    sf [coef_index + 1] = df [coef_index + 1] + v;
                    sf [coef_index + 3] = df [coef_index + 1] - v;

                    v = (df [coef_index + 5] + df [coef_index + 7]) * sqrt2_2;
                    sf [coef_index + 5] = v + df [coef_index + 3];
                    sf [coef_index + 7] = v - df [coef_index + 3];

                    coef_index += 8;
                }
                while (coef_index < _length);
            }

            /* Next pass */
            for (pass = 3; pass < _nbr_bits; ++pass)
            {
                coef_index = 0;
                nbr_coef = 1 << pass;
                h_nbr_coef = nbr_coef >> 1;
                d_nbr_coef = nbr_coef << 1;
                const flt_t	* const	cos_ptr = _trigo_lut.get_ptr (pass);
                do
                {
                    long				i;
                    const flt_t	*	const sf1r = sf + coef_index;
                    const flt_t	*	const sf2r = sf1r + nbr_coef;
                    flt_t *			const dfr = df + coef_index;
                    flt_t *			const dfi = dfr + nbr_coef;

                    /* Extreme coefficients are always real */
                    dfr [0] = sf1r [0] + sf2r [0];
                    dfi [0] = sf1r [0] - sf2r [0];	// dfr [nbr_coef] =
                    dfr [h_nbr_coef] = sf1r [h_nbr_coef];
                    dfi [h_nbr_coef] = sf2r [h_nbr_coef];

                    /* Others are conjugate complex numbers */
                    const flt_t	* const	sf1i = sf1r + h_nbr_coef;
                    const flt_t	* const	sf2i = sf1i + nbr_coef;
                    for (i = 1; i < h_nbr_coef; ++ i)
                    {
                        const flt_t		c = cos_ptr [i];					// cos (i*PI/nbr_coef);
                        const flt_t		s = cos_ptr [h_nbr_coef - i];	// sin (i*PI/nbr_coef);
                        flt_t				v;

                        v = sf2r [i] * c - sf2i [i] * s;
                        dfr [i] = sf1r [i] + v;
                        dfi [-i] = sf1r [i] - v;	// dfr [nbr_coef - i] =

                        v = sf2r [i] * s + sf2i [i] * c;
                        dfi [i] = v + sf1i [i];
                        dfi [nbr_coef - i] = v - sf1i [i];
                    }

                    coef_index += d_nbr_coef;
                }
                while (coef_index < _length);

                /* Prepare to the next pass */
                {
                    flt_t	* const		temp_ptr = df;
                    df = sf;
                    sf = temp_ptr;
                }
            }
        }
    }

    /*______________________________________________
 *
 * Special cases
 *______________________________________________
 */

    /* 4-point FFT */
    else if (_nbr_bits == 2)
    {
        f [1] = x->at(0) - x->at(2);
        f [3] = x->at(1) - x->at(3);

        const flt_t			b_0 = x->at(0) + x->at(2);
        const flt_t			b_2 = x->at(1) + x->at(3);

        f [0] = b_0 + b_2;
        f [2] = b_0 - b_2;
    }

    /* 2-point FFT */
    else if (_nbr_bits == 1)
    {
        f [0] = x->at(0) + x->at(1);
        f [1] = x->at(0) - x->at(1);
    }

    /* 1-point FFT */
    else
    {
        f [0] = x->at(0);
    }
}



/*==========================================================================*/
/*      Name: do_ifft                                                       */
/*      Description: Compute the inverse FFT of the array. Notice that      */
/*                   IFFT (FFT (x)) = x * length (x). Data must be          */
/*                   post-scaled.                                           */
/*      Input parameters:                                                   */
/*        - f: pointer on the source array (frequencies).                   */
/*             f [0...length(x)/2] = real values,                           */
/*             f [length(x)/2+1...length(x)] = imaginary values of          */
/*               coefficents 1...length(x)-1.                               */
/*      Output parameters:                                                  */
/*        - x: pointer on the destination array (time).                     */
/*      Throws: Nothing                                                     */
/*==========================================================================*/

void	FFTReal::do_ifft (const flt_t f [], flt_t x []) const
{

    /*______________________________________________
 *
 * General case
 *______________________________________________
 */

    if (_nbr_bits > 2)
    {
        flt_t *			sf = const_cast <flt_t *> (f);
        flt_t *			df;
        flt_t *			df_temp;

        if (_nbr_bits & 1)
        {
            df = _buffer_ptr;
            df_temp = x;
        }
        else
        {
            df = x;
            df_temp = _buffer_ptr;
        }

        /* Do the transformation in several pass */
        {
            int			pass;
            long			nbr_coef;
            long			h_nbr_coef;
            long			d_nbr_coef;
            long			coef_index;

            /* First pass */
            for (pass = _nbr_bits - 1; pass >= 3; --pass)
            {
                coef_index = 0;
                nbr_coef = 1 << pass;
                h_nbr_coef = nbr_coef >> 1;
                d_nbr_coef = nbr_coef << 1;
                const flt_t	*const cos_ptr = _trigo_lut.get_ptr (pass);
                do
                {
                    long				i;
                    const flt_t	*	const sfr = sf + coef_index;
                    const flt_t	*	const sfi = sfr + nbr_coef;
                    flt_t *			const df1r = df + coef_index;
                    flt_t *			const df2r = df1r + nbr_coef;

                    /* Extreme coefficients are always real */
                    df1r [0] = sfr [0] + sfi [0];		// + sfr [nbr_coef]
                    df2r [0] = sfr [0] - sfi [0];		// - sfr [nbr_coef]
                    df1r [h_nbr_coef] = sfr [h_nbr_coef] * 2;
                    df2r [h_nbr_coef] = sfi [h_nbr_coef] * 2;

                    /* Others are conjugate complex numbers */
                    flt_t * const	df1i = df1r + h_nbr_coef;
                    flt_t * const	df2i = df1i + nbr_coef;
                    for (i = 1; i < h_nbr_coef; ++ i)
                    {
                        df1r [i] = sfr [i] + sfi [-i];		// + sfr [nbr_coef - i]
                        df1i [i] = sfi [i] - sfi [nbr_coef - i];

                        const flt_t		c = cos_ptr [i];					// cos (i*PI/nbr_coef);
                        const flt_t		s = cos_ptr [h_nbr_coef - i];	// sin (i*PI/nbr_coef);
                        const flt_t		vr = sfr [i] - sfi [-i];		// - sfr [nbr_coef - i]
                        const flt_t		vi = sfi [i] + sfi [nbr_coef - i];

                        df2r [i] = vr * c + vi * s;
                        df2i [i] = vi * c - vr * s;
                    }

                    coef_index += d_nbr_coef;
                }
                while (coef_index < _length);

                /* Prepare to the next pass */
                if (pass < _nbr_bits - 1)
                {
                    flt_t	* const	temp_ptr = df;
                    df = sf;
                    sf = temp_ptr;
                }
                else
                {
                    sf = df;
                    df = df_temp;
                }
            }

            /* Antepenultimate pass */
            {
                const flt_t		sqrt2_2 = _sqrt2_2;
                coef_index = 0;
                do
                {
                    df [coef_index] = sf [coef_index] + sf [coef_index + 4];
                    df [coef_index + 4] = sf [coef_index] - sf [coef_index + 4];
                    df [coef_index + 2] = sf [coef_index + 2] * 2;
                    df [coef_index + 6] = sf [coef_index + 6] * 2;

                    df [coef_index + 1] = sf [coef_index + 1] + sf [coef_index + 3];
                    df [coef_index + 3] = sf [coef_index + 5] - sf [coef_index + 7];

                    const flt_t		vr = sf [coef_index + 1] - sf [coef_index + 3];
                    const flt_t		vi = sf [coef_index + 5] + sf [coef_index + 7];

                    df [coef_index + 5] = (vr + vi) * sqrt2_2;
                    df [coef_index + 7] = (vi - vr) * sqrt2_2;

                    coef_index += 8;
                }
                while (coef_index < _length);
            }

            /* Penultimate and last pass at once */
            {
                coef_index = 0;
                const long *	bit_rev_lut_ptr = _bit_rev_lut.get_ptr ();
                const flt_t	*	sf2 = df;
                do
                {
                    {
                        const flt_t		b_0 = sf2 [0] + sf2 [2];
                        const flt_t		b_2 = sf2 [0] - sf2 [2];
                        const flt_t		b_1 = sf2 [1] * 2;
                        const flt_t		b_3 = sf2 [3] * 2;

                        x [bit_rev_lut_ptr [0]] = b_0 + b_1;
                        x [bit_rev_lut_ptr [1]] = b_0 - b_1;
                        x [bit_rev_lut_ptr [2]] = b_2 + b_3;
                        x [bit_rev_lut_ptr [3]] = b_2 - b_3;
                    }
                    {
                        const flt_t		b_0 = sf2 [4] + sf2 [6];
                        const flt_t		b_2 = sf2 [4] - sf2 [6];
                        const flt_t		b_1 = sf2 [5] * 2;
                        const flt_t		b_3 = sf2 [7] * 2;

                        x [bit_rev_lut_ptr [4]] = b_0 + b_1;
                        x [bit_rev_lut_ptr [5]] = b_0 - b_1;
                        x [bit_rev_lut_ptr [6]] = b_2 + b_3;
                        x [bit_rev_lut_ptr [7]] = b_2 - b_3;
                    }

                    sf2 += 8;
                    coef_index += 8;
                    bit_rev_lut_ptr += 8;
                }
                while (coef_index < _length);
            }
        }
    }

    /*______________________________________________
 *
 * Special cases
 *______________________________________________
 */

    /* 4-point IFFT */
    else if (_nbr_bits == 2)
    {
        const flt_t		b_0 = f [0] + f [2];
        const flt_t		b_2 = f [0] - f [2];

        x [0] = b_0 + f [1] * 2;
        x [2] = b_0 - f [1] * 2;
        x [1] = b_2 + f [3] * 2;
        x [3] = b_2 - f [3] * 2;
    }

    /* 2-point IFFT */
    else if (_nbr_bits == 1)
    {
        x [0] = f [0] + f [1];
        x [1] = f [0] - f [1];
    }

    /* 1-point IFFT */
    else
    {
        x [0] = f [0];
    }
}



/*==========================================================================*/
/*      Name: rescale                                                       */
/*      Description: Scale an array by divide each element by its length.   */
/*                   This function should be called after FFT + IFFT.       */
/*      Input/Output parameters:                                            */
/*        - x: pointer on array to rescale (time or frequency).             */
/*      Throws: Nothing                                                     */
/*==========================================================================*/

void	FFTReal::rescale (flt_t x []) const
{
    const flt_t		mul = flt_t (1.0 / _length);
    long				i = _length - 1;

    printf("for do-schleife");

    do
    {       
        x [i] *= mul;
        printf("Wert nach rescale %li: %4f \n", i, x[i]);
        --i;
    }
    while (i >= 0);
}



/*\\\ NESTED CLASS MEMBER FUNCTIONS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/



/*==========================================================================*/
/*      Name: Constructor                                                   */
/*      Input parameters:                                                   */
/*        - nbr_bits: number of bits of the array on which we want to do a  */
/*                    FFT. Range: > 0                                       */
/*      Throws: std::bad_alloc                                              */
/*==========================================================================*/

FFTReal::BitReversedLUT::BitReversedLUT (const int nbr_bits)
{
    long				length;
    long				cnt;
    long				br_index;
    long				bit;

    length = 1L << nbr_bits;
    _ptr = new long [length];

    br_index = 0;
    _ptr [0] = 0;
    for (cnt = 1; cnt < length; ++cnt)
    {
        /* ++br_index (bit reversed) */
        bit = length >> 1;
        while (((br_index ^= bit) & bit) == 0)
        {
            bit >>= 1;
        }

        _ptr [cnt] = br_index;
    }
}



/*==========================================================================*/
/*      Name: Destructor                                                    */
/*==========================================================================*/

FFTReal::BitReversedLUT::~BitReversedLUT (void)
{
    delete [] _ptr;
    _ptr = 0;
}



/*==========================================================================*/
/*      Name: Constructor                                                   */
/*      Input parameters:                                                   */
/*        - nbr_bits: number of bits of the array on which we want to do a  */
/*                    FFT. Range: > 0                                       */
/*      Throws: std::bad_alloc, anything                                    */
/*==========================================================================*/

FFTReal::TrigoLUT::TrigoLUT (const int nbr_bits)
{
    long		total_len;

    _ptr = 0;
    if (nbr_bits > 3)
    {
        total_len = (1L << (nbr_bits - 1)) - 4;
        _ptr = new flt_t [total_len];

const double	PI = (double) atan ((double)1) * 4;
        //const double	PI = atan (1) * 4;
        for (int level = 3; level < nbr_bits; ++level)
        {
            const long		level_len = 1L << (level - 1);
            flt_t	* const	level_ptr = const_cast<flt_t *> (get_ptr (level));
            const double	mul = PI / (level_len << 1);

            for (long i = 0; i < level_len; ++ i)
            {
                level_ptr [i] = (flt_t) cos (i * mul);
            }
        }
    }
}

bool FFTReal::checkZeroPadding(const int nbr_points,unsigned int* nextPotence)
{
    /******************** BEGIN ******************/
    // ZERO PADDING CHECK

    int rest = 0;
    bool zeroPadding = false;
    int amountOfDivisions = 0;
    unsigned int lengthOfArray = (unsigned int) nbr_points;

    //check if array is a potency of 2
    while(true)
    {
        if(lengthOfArray >= 2)
        {
            rest = lengthOfArray % 2;

            if(rest == 1)
            {
                //not a potency of 2
                zeroPadding = true;
                lengthOfArray = (lengthOfArray - 1) / 2;
                amountOfDivisions += 1;
            }
            else
            {
                lengthOfArray = lengthOfArray / 2;
                amountOfDivisions += 1;
            }

        }
        else
        {
            break;
        }

    }
    if(zeroPadding == true)
    {
        *nextPotence = pow(2.0, (double) amountOfDivisions +1);
    }
    else{
        *nextPotence = (long) nbr_points;
    }

        return zeroPadding;

}

void FFTReal::zeroPadding(QList<flt_t> * x, const int nbr_points,unsigned int* nextPotence)
{
    /******************** BEGIN ******************/
    //check if zero padding is necessary
    bool zeroPadding = checkZeroPadding(nbr_points, nextPotence);

    //input array is not a potence of 2, fill array with zeros till next potence of 2
    if (zeroPadding == true)
    {
        //TODO: if length of array is only a bit larger than a 2 potence, delete
        // values to reach the size of a 2 potence

        //here, fill the end of the array up with zeros
        int lengthOfX = x->length();
        for(unsigned int i = 0; i < (*nextPotence - lengthOfX); i++)
        {
            x->append(0.0);
        }

        printf("Neue Laenge des Arrays: %i (muss eine 2-Potenz sein!\n", x->length());

    }
    /******************** END  ******************/
}


void FFTReal::getImagAndReal(QList<flt_t>* f, const int nbr_points, QList<flt_t>* real, QList<flt_t>* imag)
{
    //the first half are the Real-parts, the second half the  imaginary parts
    for (unsigned int i = 0; i <= round(nbr_points/2); i ++)
    {
        //define real
        real->append(f->at(i));


        //first and last half imaginary elements are zero
        if (i == 0 || i == round(nbr_points/2))
        {
            imag->append(0);
        }
        else
        {
            //define imaginary
            imag->append(f->at(i + (int) round(nbr_points/2)));
        }
    }

    int k = 1;

    //here the computing for the second half
    for( int i = nbr_points/2 + 1; i <= nbr_points - 1; i++)
    {
        //define real
        real->append(f->at(nbr_points - i));

        //define imaginary
        imag->append(f->at(nbr_points - k));

        k = k+1;
    }
}




/*==========================================================================*/
/*      Name: Destructor                                                    */
/*==========================================================================*/

FFTReal::TrigoLUT::~TrigoLUT (void)
{
    delete [] _ptr;
    _ptr = 0;
}



#if defined (_MSC_VER)
#pragma pack (pop)
#endif	// _MSC_VER



/*****************************************************************************

        LEGAL

        Source code may be freely used for any purpose, including commercial
        applications. Programs must display in their "About" dialog-box (or
        documentation) a text telling they use these routines by Laurent de Soras.
        Modified source code can be distributed, but modifications must be clearly
        indicated.

        CONTACT

        Laurent de Soras
        92 avenue Albert 1er
        92500 Rueil-Malmaison
        France

        ldesoras@club-internet.fr

*****************************************************************************/



/*\\\ EOF \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/

