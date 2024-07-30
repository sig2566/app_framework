/******************************************************************
 * i_beamforming_api.h
 * Author: Niv arad
 * Copyright (c) 2018 - 2020 Airspan Networks
 * All Rights Reserved.
 * Confidential and Proprietary - Airspan Networks
 *******************************************************************/

#ifndef API_MODULE_API_I_BEAMFORMING_API_H_
#define API_MODULE_API_I_BEAMFORMING_API_H_

struct Complex_num
{
	int16_t	 real_var, image_var;
};

struct BEAMFORMING_call_api{
		    virtual void max_eigenvec(float *h_matrix, int32_t n, int32_t iter, Complex_num *out_eigan_vec)=0;
};


#endif /* API_MODULE_API_I_BEAMFORMING_API_H_ */
