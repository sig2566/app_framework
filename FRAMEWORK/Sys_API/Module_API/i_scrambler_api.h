/*
 * i_scrambler_api.h
 *
 *  Created on: May 3, 2018
 *      Author: eglotman
 */

#ifndef API_MODULE_API_I_SCRAMBLER_API_H_
#define API_MODULE_API_I_SCRAMBLER_API_H_

// Maximal size of scrambler sequence 192 * 4 bytes
#define MAX_SCRAMBLER_IO_SIZE   (192)

struct ScramblerIO
{
	uint32_t size;
	uint32_t cinit;
	uint32_t *in;
	uint32_t *out;
};

typedef    enum Stream_Type_e
{
    PUSCH_FORMAT,
    PUCCH_FORMT2,
    PUCCH_FORMT34,
    PDSCH_FORMAT,
    PDCCH_FORMAT,
    PBCH,
    PUSCH_RS,
    PUCCH_FORMT2_RS,
    PDSCH_RS,
    PBCH_RS,
    CSI_RS
}Stream_Type_t;


// Name of input FIFO for scrambler
//#define SCRAMBLER_FIFO_IN   "SCRAMBLER_FIFO_IN"
//#define SCRAMBLER_MODDULE   "SCRAMBLER_LIB"

struct Scrambler_API
{
    virtual uint32_t    Calc_Cinit(Stream_Type_e Str_Type  , uint8_t rnti, uint16_t CellID, uint8_t q_codeword) =0;
    virtual int16_t     Process(uint32_t *Prbs, uint32_t Proc_Size, uint32_t cinit_par)=0;
    virtual void        Init(uint32_t cinit)=0;                                           // Init_new_allocation

};
#endif /* API_MODULE_API_I_SCRAMBLER_API_H_ */
