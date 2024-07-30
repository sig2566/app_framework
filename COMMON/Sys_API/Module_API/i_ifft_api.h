/******************************************************************
 * i_beamforming_api.h
 * Author: Niv arad
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/


#ifndef API_MODULE_API_I_IFFT_API_H_
#define API_MODULE_API_I_IFFT_API_H_


struct Complex_double
{
	double	 re, im;
};


struct IFFT_API{
		    virtual void IFFT_CP(Complex *input_symbols, int32_t num_REs,int32_t num_OFDM_symbols,int32_t num_slots, Complex *output_symbols)=0;
};



#endif /* API_MODULE_API_I_IFFT_API_H_ */
