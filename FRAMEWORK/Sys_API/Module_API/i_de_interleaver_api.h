/*
 * i_scrambler_api.h
 *
 *  Created on: May 3, 2018
 *      Author: eglotman
 */

#ifndef API_MODULE_API_I_DE_INTERLEAVER_API_H_
#define API_MODULE_API_I_DE_INTERLEAVER_API_H_


struct De_InterleaverIO
{
	uint32_t size;
	uint32_t *io;
};



// Name of input FIFO for scrambler
//#define SCRAMBLER_FIFO_IN   "SCRAMBLER_FIFO_IN"
//#define SCRAMBLER_MODDULE   "SCRAMBLER_LIB"

struct de_Interleaver_API
{
    virtual uint32_t     Process(uint32_t Qm, uint32_t Er, int16_t *deInterleaver_in, int16_t *deInterleaver_out)=0;

};
#endif /* API_MODULE_API_I_DE_INTERLEAVER_API_H_ */
