/******************************************************************
 * TB_tables.cc

 *******************************************************************/

 #ifndef HANDLER_TB_TABLES_H
 #define HANDLER_TB_TABLES_H

#include  <cstdint>


#define	NUMBER_OF_MCS_TABLE_1	29
#define	NUMBER_OF_MCS_TABLE_2	28
#define LENGTH_TB_TABLE			93

extern float MCS_to_R_table_1[NUMBER_OF_MCS_TABLE_1]; //Table for Enable256QAM = 0
extern float MCS_to_R_table_2[NUMBER_OF_MCS_TABLE_2]; //Table for Enable256QAM = 1
extern float MCS_to_Qm_table_1[NUMBER_OF_MCS_TABLE_1]; //Table for Enable256QAM = 0
extern float MCS_to_Qm_table_2[NUMBER_OF_MCS_TABLE_2]; //Table for Enable256QAM = 1
extern uint16_t T_TBS_Table[LENGTH_TB_TABLE];
extern uint32_t lifting_size_table[8][8];
extern uint32_t num_of_elements_for_lifting_size_table[8];

#endif
