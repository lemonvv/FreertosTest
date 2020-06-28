#ifndef __LTE_H__
#define __LTE_H__

#include "main.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum
{
    EC20_Power_S = 0,
    EC20_Find_S,
    EC20_Set_Echo_S,
    EC20_Find_Sim_S,
    EC20_Check_Sim_S,
    EC20_Check_Csq_S,
/*     EC20_Get_Imei_S,
    EC20_Get_Iccid_S, */
    EC20_Check_Creg_S,
    EC20_Check_Net_S,
    EC20_Check_Cgatt_S,
    EC20_Set_Cgatt_S,
    EC20_Set_Context_S,
    EC20_Check_Context_S,
    EC20_Active_Context_S,
}EC20_INIT_E;

typedef enum
{
    EC20_Init_R = 0,
    EC20_ConnectInit_R,
    EC20_Connect_R,
    EC20_Verify_R,
    EC20_Send_R,
    EC20_Disconnect_R,
}EC20_RUN_E;

void ec20_run(void);
#endif
