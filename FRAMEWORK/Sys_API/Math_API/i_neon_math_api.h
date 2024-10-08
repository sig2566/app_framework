/******************************************************************
 * neon_math_api
 * Author: Igor Shoihet
 *******************************************************************/

#ifndef API_MATH_API_I_NEON_MATH_API_H_
#define API_MATH_API_I_NEON_MATH_API_H_

#include "i_api_common.h"
/*******************************************************************************************//*!
*@class NeonMathAPI
*@brief The purpose of this class is :
*@brief API for ARM NEON Math Library
***********************************************************************************************/
#define NEON_MATH  "NEON_MATH_LIB"

struct NeonMathAPI
{
    /* c_out[i] = a_in[i] + b_in[i] */
    virtual void add(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = a_inout[i] + b_in[i] */
    virtual void add_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] + b_in[i]) >> 1 */
    virtual void add_shift1(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] + b_in[i]) >> 1 */
    virtual void add_shift1_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i] + value */
    virtual void addC(Complex *a_in, Complex value, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = a_inout[i] + value */
    virtual void addC_I(Complex *a_inout, Complex value, uint32_t length) = 0;

    /* c_out[i] = a_in[i] + b_in[i] */
    virtual void add(int16_t *a_in, int16_t *b_in, uint32_t length, int16_t *c_out) = 0;

    /* a_inout[i] = a_inout[i] + b_in[i] */
    virtual void add_I(int16_t *a_inout, int16_t *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i] + value */
    virtual void addC(int16_t *a_in, int16_t value, uint32_t length, int16_t *c_out) = 0;

    /* a_inout[i] = a_inout[i] + value */
    virtual void addC_I(int16_t *a_inout, int16_t value, uint32_t length) = 0;

    /* c_out[i] = a_in[i] - b_in[i] */
    virtual void sub(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = a_inout[i] - b_in[i] */
    virtual void sub_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i]>>1 - b_in[i]>>1 */
    virtual void sub_shift1(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = a_inout[i]>>1 - b_in[i]>>1 */
    virtual void sub_shift1_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i] - value */
    virtual void subC(Complex *a_in, Complex value, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = a_inout[i] + value */
    virtual void subC_I(Complex *a_inout, Complex value, uint32_t length) = 0;

    /* c_out[i] = a_in[i] - b_in[i] */
    virtual void sub(int16_t *a_in, int16_t *b_in, uint32_t length, int16_t *c_out) = 0;

    /* a_inout[i] = a_inout[i] - b_in[i] */
    virtual void sub_I(int16_t *a_inout, int16_t *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i] - value */
    virtual void subC(int16_t *a_in, int16_t value, uint32_t length, int16_t *c_out) = 0;

    /* a_inout[i] = a_inout[i] - value */
    virtual void subC_I(int16_t *a_inout, int16_t value, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * b_in[i])>>16 */
    virtual void mul_shift16(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * b_in[i])>>16 */
    virtual void mul_shift16_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * value)>>16 */
    virtual void mulC_shift16(Complex *a_in, Complex value, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * value)>>16 */
    virtual void mulC_shift16_I(Complex *a_inout, Complex value, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * conjugate(value))>>16 */
    virtual void CmulC_shift16(Complex *a_in, Complex value, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * value)>>16 */
    virtual void CmulC_shift16_I(Complex *a_inout, Complex value, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * conjugate(b_in[i]))>>16 */
    virtual void Cmul_shift16(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * conjugate(b_in[i]))>>16 */
    virtual void Cmul_shift16_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * b_in[i])>>15 */
    virtual void mul_shift15(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * b_in[i])>>15 */
    virtual void mul_shift15_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * value)>>15 */
    virtual void mulC_shift15(Complex *a_in, Complex value, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * value)>>15 */
    virtual void mulC_shift15_I(Complex *a_inout, Complex value, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * conjugate(value))>>15 */
    virtual void CmulC_shift15(Complex *a_in, Complex value, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * value)>>15 */
    virtual void CmulC_shift15_I(Complex *a_inout, Complex value, uint32_t length) = 0;

    /* c_out[i] = (a_in[i] * conjugate(b_in[i]))>>15 */
    virtual void Cmul_shift15(Complex *a_in, Complex *b_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = (a_inout[i] * conjugate(b_in[i]))>>15 */
    virtual void Cmul_shift15_I(Complex *a_inout, Complex *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i] * b_in[i] */
    virtual void mul(Complex *a_in, Complex *b_in, uint32_t length, Complex32fix *c_out) = 0;

    /* c_out[i] = a_in[i] * b_in[i] */
    virtual void mul32(Complex32fix *a_in, Complex32fix *b_in, uint32_t length, Complex64 *c_out) = 0;

    /* c_out[i] = real(a_in[i] * conjugate(a_in[i])) */
    virtual void real_square_Cmul32(Complex32fix *a_in, uint32_t length, int64_t *c_out) = 0;

    /* c_out[i] = a_in[i] * value */
    virtual void mulC(Complex *a_in, Complex value, uint32_t length, Complex32fix *c_out) = 0;

    /* c_out[i] = a_in[i] * value */
    virtual void mulC32(Complex32fix *a_in, Complex32fix value, uint32_t length, Complex64 *c_out) = 0;

    /* c_out[i] = a_in[i] * conjugate(b_in[i]) */
    virtual void Cmul(Complex *a_in, Complex *b_in, uint32_t length, Complex32fix *c_out) = 0;

    /* c_out[i] = a_in[i] * b_in[i] */
    virtual void mul(int16_t *a_in, int16_t *b_in, uint32_t length, int16_t *c_out) = 0;

    /* a_inout[i] = a_inout[i] * b_in[i] */
    virtual void mul_I(int16_t *a_inout, int16_t *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i] * value */
    virtual void mulC(int16_t *a_in, int16_t value, uint32_t length, int16_t *c_out) = 0;

    /* a_inout[i] = a_inout[i] * value */
    virtual void mulC_I(int16_t *a_inout, int16_t value, uint32_t length) = 0;

    /* c_out[i] = a_in[i] * b_in[i] */
    virtual void mul(int16_t *a_in, int16_t *b_in, uint32_t length, int32_t *c_out) = 0;

    /* c_out[i] = a_in[i] * value */
    virtual void mulC(int16_t *a_in, int16_t value, uint32_t length, int32_t *c_out) = 0;

    /* c_out[i] = |a_in[i]|^2 */
    virtual void abs_square(Complex *a_in, uint32_t length, int32_t *c_out) = 0;

    /* c_out[i] = conjugate(a_in[i]) */
    virtual void conj(Complex *a_in, uint32_t length, Complex *c_out) = 0;

    /* a_inout[i] = conjugate(a_inout[i]) */
    virtual void conj_I(Complex *a_inout, uint32_t length) = 0;

    /* c_out[i] = real(a_in[i]) */
    virtual void real(Complex *a_in, uint32_t length, int16_t *c_out) = 0;

    /* c_out[i] = image(a_in[i]) */
    virtual void image(Complex *a_in, uint32_t length, int16_t *c_out) = 0;

    /* return sum{a_in[i]} */
    virtual int32_t sum(int16_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]} */
    virtual int32_t sum(int32_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>1} */
    virtual int32_t shift1_sum(int32_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>2} */
    virtual int32_t shift2_sum(int32_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>4} */
    virtual int32_t shift4_sum(int32_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>6} */
    virtual int32_t shift6_sum(int32_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>8} */
    virtual int32_t shift8_sum(int32_t *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]} */
    virtual Complex32fix sum(Complex *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]} */
    virtual Complex32fix sum(Complex32fix *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>1} */
    virtual Complex32fix shift1_sum(Complex32fix *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>2} */
    virtual Complex32fix shift2_sum(Complex32fix *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>4} */
    virtual Complex32fix shift4_sum(Complex32fix *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>6} */
    virtual Complex32fix shift6_sum(Complex32fix *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]>>8} */
    virtual Complex32fix shift8_sum(Complex32fix *a_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]} */
    virtual int32_t mac(int16_t *a_in, int16_t *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>1} */
    virtual int32_t mac_shift1(int16_t *a_in, int16_t *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>2} */
    virtual int32_t mac_shift2(int16_t *a_in, int16_t *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>4} */
    virtual int32_t mac_shift4(int16_t *a_in, int16_t *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>6} */
    virtual int32_t mac_shift6(int16_t *a_in, int16_t *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>8} */
    virtual int32_t mac_shift8(int16_t *a_in, int16_t *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]} */
    virtual Complex32fix mac(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>1} */
    virtual Complex32fix mac_shift1(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>2} */
    virtual Complex32fix mac_shift2(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>4} */
    virtual Complex32fix mac_shift4(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>6} */
    virtual Complex32fix mac_shift6(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*b_in[i]>>8} */
    virtual Complex32fix mac_shift8(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return real(sum{a_in[i]*b_in[i]>>1}) */
    virtual int32_t real_mac_shift1(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*conjugate(b_in[i])} */
    virtual Complex32fix Cmac(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*conjugate(b_in[i])>>1} */
    virtual Complex32fix Cmac_shift1(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*conjugate(b_in[i])>>2} */
    virtual Complex32fix Cmac_shift2(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*conjugate(b_in[i])>>4} */
    virtual Complex32fix Cmac_shift4(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*conjugate(b_in[i])>>6} */
    virtual Complex32fix Cmac_shift6(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return sum{a_in[i]*conjugate(b_in[i])>>8} */
    virtual Complex32fix Cmac_shift8(Complex *a_in, Complex *b_in, uint32_t length) = 0;

    /* return real(sum{a_in[i]*conjugate(a_in[i])>>1}) */
    virtual int32_t square_vector_norm_shift1(Complex *a_in, uint8_t length) = 0;

    /* return max{|a_in[i]|^2} */
    virtual int16_t max_square_abs(Complex *a_in, uint32_t length) = 0;

    /* return min{|a_in[i]|^2} */
    virtual int16_t min_square_abs(Complex *a_in, uint32_t length) = 0;

    /* return exp(j*tau*k) , k=[0,1,2...length-1] */
    virtual void linear_exp(int16_t tau, uint32_t length, Complex* exp_out) = 0;

    /* return c_out[i] = value for all i */
    virtual void set(int16_t value, uint32_t length, int16_t* c_out) = 0;

    /* return c_out[i] = value for all i */
    virtual void set(Complex value, uint32_t length, Complex* c_out) = 0;

    /* return c_out[i] = clip{a_min <= a_in[i] <= a_max} */
    virtual void clip(int16_t *a_in, int16_t a_min, int16_t a_max, uint32_t length, int16_t *c_out) = 0;

    /* return a_inout[i] = clip{a_min <= a_inout[i] <= a_max} */
    virtual void clip_I(int16_t *a_inout, int16_t a_min, int16_t a_max, uint32_t length) = 0;

    /* c_out[i] = a_in[i]<<scale */
    virtual void shift(Complex *a_in, int16_t scale, uint32_t length, Complex*) = 0;

	/* a_inout[i] = a_inout[i]<<scale */
    virtual void shift_I(Complex *a_inout, int16_t scale, uint32_t length) = 0;

	/* c_out[i] = a_in[i]<<scale */
    virtual void shift(int16_t *a_in, int16_t scale, uint32_t, int16_t*) = 0;

	/* a_inout[i] = a_inout[i]<<scale */
    virtual void shift_I(int16_t *a_inout, int16_t scale, uint32_t length) = 0;

	/* c_out[i] = a_in[i]<<b_in[i] */
    virtual void shift(Complex *a_in, int16_t *b_in, uint32_t, Complex*) = 0;

	/* a_inout[i] = a_inout[i]<<b_in[i] */
    virtual void shift_I(Complex *a_inout, int16_t *b_in, uint32_t length) = 0;

	/* c_out[i] = a_in[i]<<b_in[i] */
    virtual void shift(int16_t *a_in, int16_t *b_in, uint32_t length, int16_t*) = 0;

    /* c_out[i] = a_in[i]<<b_in[i] */
    virtual void shift(int32_t *a_in, int32_t *b_in, uint32_t length, int32_t *c_out) = 0;

    /* a_inout[i] = a_inout[i]<<b_in[i] */
    virtual void shift_I(int32_t *a_inout, int32_t *b_in, uint32_t length) = 0;

	/* a_inout[i] = a_inout[i]<<b_in[i] */
    virtual void shift_I(int16_t *a_inout, int16_t *b_in, uint32_t length) = 0;

    /* c_out[i] = a_in[i]<<scale */
    virtual void shift(int32_t *a_in, int32_t scale, uint32_t length, int32_t *c_out) = 0;

	/* a_inout[i] = a_inout[i]<<scale */
    virtual void shift_I(int32_t *a_inout, int32_t scale, uint32_t length) = 0;

	/* c_out[i] = Counting leading zeros{a_in[i]} */
    virtual void clb(int16_t *a_in, uint32_t length, int16_t*) = 0;

	/* c_out[i] = Counting leading zeros sign{a_in[i]} */
	virtual void clb_sign(int16_t *a_in, uint32_t length, int16_t*) = 0;


};
#endif /* API_MATH_API_I_NEON_MATH_API_H_ */
