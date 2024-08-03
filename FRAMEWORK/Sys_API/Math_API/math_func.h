/******************************************************************
 * neon_math.h
 * Author: Elad Cohen
 * Date : 26/5/2018
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef MATH_FUNC_H_
#define MATH_FUNC_H_


#include "i_api_common.h"
#include <math.h>
#include <stdio.h>

#include <xmmintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>

/////////////////////////////////////////////////
///// Fixed Point Format Q15 Math functions  ////
/////////////////////////////////////////////////

class MathFunc
{
    //static const int16_t*    angle_lut_;
    //static const int16_t*    inv_full_lut_;
public:
    int16_t Angle(Complex32fix input, const int16_t* inv_2x_table, const int16_t* ANGLE_LUT,NeonMathAPI *neon_math)
    {
        int16_t x=input.re>>16; //shift is 16 and not 15 , since leading bits must not be zero (due neon function shift does not support shift by zero)
        int16_t y=input.im>>16;
        int16_t absx=abs(x);
        int16_t absy=abs(y);
          if(absy>absx)
         {
            absy=absx; //no support for phase > pi/4 (tan =1)
         }
      int16_t leading_bits;
        neon_math->clb_sign(&absx, 1, &leading_bits); //leading_bits>=1
        if(leading_bits!=0)
        {
            neon_math->shift_I(&absx,(int16_t)leading_bits,1);
            neon_math->shift_I(&absy,leading_bits,1);
        }
        int16_t x1=absx-0x4000;
        x1=x1>>3;
        int32_t div_result=((absy<<1)*inv_2x_table[x1])>>15; //since table is 1/2*x , also y should be multiple by 2
        div_result=div_result+8; //for rounding
         int16_t div_result_16=int16_t((div_result)>>4); // div by 16 (16*2048[LUT length]=32768)

        int16_t angle_result=ANGLE_LUT[div_result_16];
          if(x*y<0) //negative
              angle_result=-angle_result;
          return angle_result;

   }

    /******************************************************************
    *@brief  Function Name: Angles_new
    *@brief  *@brief  Desc:	Function to calculate atan2
    *@brief
    *@brief  Input: Complex32fix *input
    *@brief  uint16_t length_of_input
    *@brief  Output:int16_t *angle_result
    *@brief
    *******************************************************************/
   void Angles_new(Complex32fix *input, uint16_t length_of_input,const int16_t* inv_2x_table, const int16_t* ANGLE_LUT,NeonMathAPI *neon_math_,int16_t *angle_result)
   {

         	int32_t ulLoopNumb;
         	int32_t i;            /* loop counter */
         	int32_t *inp;
         	__m128i *out;


         	inp = (int32_t *)&input[0];
         	out = (__m128i *)&angle_result[0];

         	__m128i  v1_i;                  /* vector of four integer 32 bit words   */

         	__m128  v1;                  /* vector of four float 32 bit words   */
         	__m128  v2;                  /* vector of four float 32 bit words   */
         	__m128  v3;                  /* vector of four float 32 bit words   */
         	__m128  v4;                  /* vector of four float 32 bit words   */
         	__m128  v5;                  /* vector of four float 32 bit words   */
         	__m128  v6;                  /* vector of four float 32 bit words   */
         	__m128  v7;                  /* vector of four float 32 bit words   */
         	__m128  v8;                  /* vector of four float 32 bit words   */
         	__m128  v9;                  /* vector of four float 32 bit words   */
         	__m128  v10;                  /* vector of four float 32 bit words   */

         	__m128i atan_res;                  /* vector of four integer 32 bit words   */
         	__m128i atan_res_1;                  /* vector of four integer 32 bit words   */
         	__m128i v_res;

         	__m128 v_pol0;    /* vector with polynom first coeff  */
         	__m128 v_pol1;    /* vector with polynom second coeff */
         	__m128 v_pol2;    /* vector with polynom third coeff  */
         	__m128 v_pol3;    /* vector with polynom first coeff  */
         	__m128 v_pol4;    /* vector with polynom second coeff */
         	__m128 v_pol5;    /* vector with polynom third coeff  */
         	__m128 v_pol6;    /* vector with polynom third coeff  */
         	__m128 v_int_to_float  = _mm_set_ps( (float)0.000030518509475997f,    (float)0.000030518509475997f,    (float)0.000030518509475997f,   (float)0.000030518509475997f); // 1/(2^15-1)
        //      	__m128 v_int_to_float  = _mm_set_ps( (float)0.000122085215f,    (float)0.000122085215f,    (float)0.000122085215f,   (float)0.000122085215f);  // 1/(2^13-1)
        //   	__m128 v_float_to_int  = _mm_set_ps( (float)32767.0f,    (float)32767.0f,    (float)32767.0f,   (float)32767.0f);  // 2^15-1
           	__m128 v_float_to_int  = _mm_set_ps( (float)8191.0f,    (float)8191.0f,    (float)8191.0f,   (float)8191.0f);    // 2^13-1

               // Theoretical polynomials for atan2 calculation with of 10^-5 10^-6 calculation accuracy
        	      v_pol0 = _mm_set_ps( 0.9999993782f,    0.9999993782f,    0.9999993782f,   0.9999993782f );
        	      v_pol1 = _mm_set_ps( -0.33326514917f,  -0.33326514917f,  -0.33326514917f, -0.33326514917f );
        	      v_pol2 = _mm_set_ps( 0.1988148243f, 0.1988148243f, 0.1988148243f, 0.1988148243f);
        	      v_pol3 = _mm_set_ps( -0.1348719133f,  -0.1348719133f,  -0.1348719133f, -0.1348719133f );
        	      v_pol4 = _mm_set_ps( 0.08387118962f,  0.08387118962f,  0.08387118962f, 0.08387118962f );
        	      v_pol5 = _mm_set_ps( -0.03701299998f, -0.03701299998f, -0.03701299998f, -0.03701299998f);
        	      v_pol6 = _mm_set_ps( 0.00786337627f,  0.00786337627f,  0.00786337627f, 0.00786337627f );

         	  // FOR DEBUG ONLY
        //    	  v1 = _mm_set_ps( 1.216266664410369f, -0.130298253491466f, 0.961509429223831f,  -0.065291676778746f );
        //    	  v1 = _mm_set_ps( 1.280926838962954f, -0.040228578033460f, 0.945810639520111f,  -0.129226007954713f );

        //    	  1.216266664410369 - 0.130298253491466i
        //    	   0.961509429223831 - 0.065291676778746i
        //    	   1.280926838962954 - 0.040228578033460i
        //    	   0.945810639520111 - 0.129226007954713i

         	  __m128 v_pi;
         	  __m128 v_pi_div_2;
         	  __m128 v_atan_res;
         	  __m128 v_atan_res1;
         	  __m128 v_atan_res2;
         	  __m128 v_atan_res3;
         	  __m128 v_atan_res4;
         	  __m128 v_zero;
         	  __m128 v_add;
         	  __m128 v_norm;
         	  __m128 v_x;
         	  __m128 v_y;

         	   v_pi   = _mm_set_ps( 3.14159265f, 3.14159265f, 3.14159265f, 3.14159265f);
         	   v_pi_div_2 = _mm_set_ps( 1.5707963f, 1.5707963f, 1.5707963f, 1.5707963f);
         	   v_add  = _mm_set_ps( (float)10.0e-8, (float)10.0e-8, (float)10.0e-8, (float)10.0e-8);
         	   v_zero = _mm_setzero_ps();

         	   if((length_of_input / 4) && 0x1)
         	   { // if number of input values doesn't divided by 4
         	     ulLoopNumb = (length_of_input / 4) + 1;
         	   }
         	   else
         	   {
         	     ulLoopNumb = (length_of_input / 4);
         	   } // End of if-else


               int j = 0;
         	   for(i = 0; i < ulLoopNumb; i++)
         	   {
         		   // Calc. on first four complex values befor atan calc
         		   // ===================================================
         		   v1_i = _mm_loadu_si128((__m128i *)&inp[j]);           /* v1 = Re1,  Im1,  Re2,  Im2 */
         		   j += 4;                                        /* shift pointer to next 2 complex values     */
         		   v1 = _mm_cvtepi32_ps(v1_i);
         		   v1 = _mm_mul_ps( v1, v_int_to_float);
         		   v1_i = _mm_loadu_si128((__m128i *)&inp[j]);           /* v1 = Re3,  Im3,  Re4,  Im4 */
         		   j += 4;                                        /* shift pointer to next 2 complex values     */
         		   v2 = _mm_cvtepi32_ps(v1_i);
         		   v1 = _mm_mul_ps( v1, v_int_to_float);

        	   	   // Inputs, first [image, real] for test vs. matlab's atan2. FOR DEBUG ONLY
          	       //v1 = _mm_set_ps( 0.130298253491466f, 1.216266664410369f,  0.065291676778746f, 0.961509429223831f );
             	   //v2 = _mm_set_ps( 0.040228578033460f, 1.280926838962954f,  0.129226007954713f,  0.945810639520111f );

         		   v_x = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(0, 2, 0, 2)); /* get 4 real values       */
         		   v4 = _mm_sub_ps( v_zero, v_x);
         		   v5 = _mm_max_ps( v_x, v4);
         		   v_y = _mm_shuffle_ps(v1, v2, _MM_SHUFFLE(1, 3, 1, 3)); /* get 4 imag values       */
         		   v2 = _mm_sub_ps( v_zero, v_y);
         		   v6 = _mm_max_ps( v_y, v2);                /* v6 = abs(v4)                     */
         		   v6 = _mm_add_ps(v6, v_add);               /* add small value, if v6 is zero   */

         		   v3  = _mm_cmple_ps(v5,v6);                /* Find min from x and y            */
         		   v4  = _mm_cmple_ps(v6,v5);                /* Find min from x and y            */

         		   v7 = _mm_and_ps(v5, v3);                  // Only x < y ,other del
         		   v8 = _mm_and_ps(v6, v4);
         		   v7 = _mm_or_ps(v7, v8);

         		   v8 = _mm_and_ps(v5, v4);                  // Only x < y ,other del
         		   v9 = _mm_and_ps(v6, v3);
         		   v10 = _mm_or_ps(v8, v9);
         		   v_norm  = _mm_and_ps(v3, v_pi_div_2);

         		   v5  = _mm_rcp_ps(v10);
         		   v6  = _mm_mul_ps(v7, v5);

         		   /* Calc. atan */
         		   v1 = _mm_mul_ps(v6, v6);                   /* v3 = x^2                          */
         		   v2 = _mm_mul_ps(v6, v_pol0);               /* v2 = x * pol0                     */
         		   v3 = _mm_mul_ps(v1, v6);                   /* v3 = x^3                          */
         		   v4 = _mm_mul_ps(v3, v_pol1);               /* v2 = x^3*v_pol1                   */
         		   v2 = _mm_add_ps(v2, v4);                   /* v4 = x^3 * pol1 + x*pol0          */
         		   v3 = _mm_mul_ps(v1, v3);                   /* v3 = x^5                          */
         		   v4 = _mm_mul_ps(v3, v_pol2);               /* v4 = x^5*v_pol2                   */
         	       v2 = _mm_add_ps(v2, v4);                   /* v2 = x^5*v_pol2 + x^3 * pol1 + x*pol0          */
         		   v3 = _mm_mul_ps(v1, v3);                   /* v3 = x^7                          */
               	   v4 = _mm_mul_ps(v3, v_pol3);               /* v4 = x^7*v_pol3                   */
         		   v2 = _mm_add_ps(v2, v4);                   /* v2 = x^7*v_pol3 + x^5*v_pol2 + x^3 * pol1 + x*pol0          */
         		   v3 = _mm_mul_ps(v1, v3);                   /* v3 = x^9                          */
         		   v4 = _mm_mul_ps(v3, v_pol4);               /* v4 = x^9*v_pol4                   */
         		   v2 = _mm_add_ps(v2, v4);                   /* v2 = x^9*v_pol4 + x^7*v_pol3 + x^5*v_pol2 + x^3 * pol1 + x*pol0          */
         		   v3 = _mm_mul_ps(v1, v3);                   /* v3 = x^11                          */
         		   v4 = _mm_mul_ps(v3, v_pol5);               /* v4 = x^11*v_pol5                   */
         		   v2 = _mm_add_ps(v2, v4);                   /* v2 = x^11*v_pol5 + x^9*v_pol4 + x^7*v_pol3 + x^5*v_pol2 + x^3 * pol1 + x*pol0          */
         		   v3 = _mm_mul_ps(v1, v3);                   /* v3 = x^13                          */
         		   v4 = _mm_mul_ps(v3, v_pol6);               /* v4 = x^13*v_pol6                   */
         		   v2 = _mm_add_ps(v2, v4);                   /* v2 = x^13*v_pol6 + x^11*v_pol5 + x^9*v_pol4 + x^7*v_pol3 + x^5*v_pol2 + x^3 * pol1 + x*pol0          */
         		   v6 = _mm_sub_ps(v2, v_norm);

         		   v2 = _mm_sub_ps( v_zero, v6);
         		   v_atan_res = _mm_max_ps( v6, v2);           /* v_atan_res = abs(v6)                       */

         		   /* Find quarter */
         		   /* Case x > 0 && y > 0 */
         		   v3 = _mm_cmple_ps(v_zero, v_x);             /* if real > 0                       */
         		   v4 = _mm_cmple_ps(v_zero, v_y);             /* if imag > 0                       */
         		   v5 = _mm_and_ps(v3, v4);                    /* if real > 0 and imag > 0          */
         		   v_atan_res1 = _mm_and_ps(v_atan_res, v5);
        			   /* Case x < 0 && y < 0 */
         		   v3 = _mm_cmple_ps(v_x, v_zero);             /* if real > 0                       */
         		   v4 = _mm_cmple_ps(v_y, v_zero);             /* if imag < 0                       */
         		   v5 = _mm_and_ps(v3, v4);                    /* if real > 0 and imag < 0          */
         		   v6 = _mm_and_ps(v_pi, v5);
         		   v_atan_res2 = _mm_and_ps(v_atan_res, v5);
         		   v_atan_res2 = _mm_sub_ps(v_atan_res2, v6);  /*  atan_res3 = - atan               */

         		   /* Case x > 0 && y < 0 */
         		   v3 = _mm_cmple_ps(v_zero, v_x);             /* if real < 0                       */
         		   v4 = _mm_cmple_ps(v_y, v_zero);             /* if imag < 0                       */
         		   v5 = _mm_and_ps(v3, v4);                    /* if real < 0 and imag < 0          */
         		   v_atan_res3 = _mm_and_ps(v_atan_res, v5);
         		   v_atan_res3 = _mm_sub_ps(v_zero, v_atan_res3);

         		   /* Case x < 0 && y > 0 */
         		   v3 = _mm_cmple_ps(v_x, v_zero);             /* if real < 0                       */
         		   v4 = _mm_cmple_ps(v_zero, v_y);             /* if imag > 0                       */
         		   v5 = _mm_and_ps(v3, v4);                    /* if real < 0 and imag > 0          */
         		   v_atan_res4 = _mm_and_ps(v_atan_res, v5);

         		   v3 = _mm_and_ps(v_pi, v5);
         		   v_atan_res4 = _mm_sub_ps(v3, v_atan_res4);  /* atan_res4 = pi- atan_res          */

         		   v1 = _mm_or_ps(v_atan_res1, v_atan_res2);
         		   v1 = _mm_or_ps(v1, v_atan_res3);
         		   v1 = _mm_or_ps(v1, v_atan_res4);

       		       // Result of predefined inputs, FOR DEBUG ONLY
        		   // v1 = 0.106732719, 0.0678019002, 0.0313937701, 0.135768309

            	   v1 = _mm_mul_ps( v1, v_float_to_int);  // convert result atan float value to integer


                   // if v_x(i) * v_y(i) < 0 then v1(i) = -v1(i)
         		   v2 = _mm_mul_ps(v_x, v_y);
         		   v3 = _mm_cmplt_ps(v2, v_zero);
         		   v4 = _mm_cmpge_ps(v2, v_zero);
         		   v5 = _mm_set_ps( (float)(-1.0), (float)(-1.0), (float)(-1.0), (float)(-1.0));
                   v6 = _mm_mul_ps(v1, v5);
         		   v7 = _mm_and_ps(v1, v4);
         		   v8 = _mm_and_ps(v6, v3);
                   v1 = _mm_or_ps(v7, v8);

                    if(i%2 == 0)
                    {
             		  atan_res = _mm_cvtps_epi32(v1);          // convert float vector to integer
                    }
                    else
                    {
             		  atan_res_1 = _mm_cvtps_epi32(v1);   // convert float vector to integer
             		  v_res = _mm_packs_epi32(atan_res, atan_res_1);

             		  _mm_store_si128(out++, v_res); // write 8 of 16-bit result words to memory
                    }

         	   } // End of for()

   }  // End of Angles_new_2


//void Angles(Complex32fix *input, uint16_t length_of_input,const int16_t* inv_2x_table, const int16_t* ANGLE_LUT,NeonMathAPI *neon_math_,int16_t *angle_result)
//  {
//    int16_t xvec[MAX_NUM_OF_RBS]; //MAX_NUM_OF_ANGLES = MAX_NUM_OF_RBS*12/stride/ channel_resoloution
//    int16_t yvec[MAX_NUM_OF_RBS];
//    int16_t absxvec[MAX_NUM_OF_RBS]; //MAX_NUM_OF_ANGLES = MAX_NUM_OF_RBS*12/stride/ channel_resoloution
//    int16_t absyvec[MAX_NUM_OF_RBS];
//    int16_t leading_bits_vec[MAX_NUM_OF_RBS];
//    int16_t div_resultvec[MAX_NUM_OF_RBS];
//
//
//
//
//      for(int i=0;i<length_of_input;i++)
//      {
//          xvec[i]=input[i].re>>16; //shift is 16 and not 15 , since leading bits must not be zero (due neon function shift does not support shift by zero)
//          yvec[i]=input[i].im>>16;
//          absxvec[i]=abs(xvec[i]);
//          absyvec[i]=abs(yvec[i]);
//
//          if(absyvec[i]>absxvec[i])
//          {
//                absyvec[i]=absxvec[i]; //no support for phase > pi/4 (tan =1)
//          }
//
//      }
//
//       int16_t leading_bits;
//
//
//       neon_math_->shift_I(absxvec,-1,length_of_input); //in order shift by clb will not be 0, due neon doesn't shift by 0
//       neon_math_->shift_I(absyvec,-1,length_of_input); //in order shift by clb will not be 0, due neon doesn't shift by 0
//       neon_math_->clb_sign(absxvec, length_of_input, leading_bits_vec); //leading_bits>=1
//       neon_math_->shift_I(absxvec,leading_bits_vec,length_of_input);
//       neon_math_->shift_I(absyvec,leading_bits_vec,length_of_input);
//       neon_math_->subC_I(absxvec, int16_t(0x4000), length_of_input);  // int16_t x1=absx-0x4000;
//       neon_math_->shift_I(absxvec,-3,length_of_input);  // x1=x1>>3;
//       neon_math_->shift_I(absyvec,1,length_of_input);  // (absy<<1) - since table is 1/2*x , also y should be multiple by 2
//
//
//       for (int i=0; i<length_of_input; i++)
//       {
//    	   if ((int)absxvec[i]%2 !=0)
//    		   absxvec[i] +=1;
//    	   if ((int)absyvec[i]%2!=0)
//    	       		   absyvec[i] +=1;
//       	   absxvec[i] = absxvec[i]/2;
//       	   absyvec[i] = absyvec[i]/2;
//       }
//       for (int i=0; i<length_of_input; i++)
//       {
//    	   int16_t logx = log2((double)absxvec[i]);
//    	   leading_bits_vec[i] = 16 - (2+logx);   //one bit for guard
//       }
//       for (int i=0; i<length_of_input; i++)
//       {
//    	   int pow_i = leading_bits_vec[i];
//    	   absxvec[i] = absxvec[i]*pow(2,pow_i);
//    	   absyvec[i] = absyvec[i]*pow(2,pow_i); //leading_bits_vec[i]>1
//       }
//       for (int i=0; i<length_of_input; i++)
//       {
//    	   absxvec[i] = absxvec[i] - (int16_t)(0x4000);
//       }
//       for (int i=0; i<length_of_input; i++)
//       {
//    	   absxvec[i] = absxvec[i]/8;
//    	   absyvec[i] = absyvec[i]*2;
//       }
//
//
//      for(int i=0;i<length_of_input;i++)
//           div_resultvec[i]=(absyvec[i]*inv_2x_table[absxvec[i]]);//int32_t div_result=((absy<<1)*inv_2x_table[x1])>>15
//      neon_math_->shift_I(div_resultvec,int16_t(-19) /*15+4*/,length_of_input); //shift -4 -  div by 16 (16*2048[LUT length]=32768)
//      for(int i=0;i<length_of_input;i++)
//	   {
//		   angle_result[i]=ANGLE_LUT[int16_t(div_resultvec[i])];
//		   if(xvec[i]*yvec[i]<0) //negative
//			   angle_result[i]=-angle_result[i];
//	   }
//
//  }

};



#endif /* NEON_API_MATH_H_ */
