#include "lte.h"

#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>

const char CONTEXT_ID[] = "1"; //上下文ID

EC20_INIT_E EC20_INTE = EC20_Power_S;
EC20_RUN_E EC20_RUNE = EC20_Init_R;
uint8_t ec20_send_cmd(char *cmd, char *ack, char *ack2, char *err, uint32_t _timeout)
{
    /* 
    //把ms转换成系统滴答时间
    TickType_t xMaxBlockTime = pdMS_TO_TICKS(300);
    
    //获取当前的时间
    vTaskSetTimeOutState();
    //与过去的时间比较是否超时
    xTaskCheckForTimeOut();
     */
    TimeOut_t Timeout;
    /* 把超时的毫秒转换成系统滴答时间 */
    TickType_t xMaxBlockTime = pdMS_TO_TICKS(_timeout);

    /* 串口2 */
    clean_usart_data(2);
    USART_DATA_T *usart_data = get_usart_data_fifo(2);
    /* 发送指令 */
    Usart2_DMA_Send_Data((uint8_t *)cmd, strlen(cmd));
    osDelay(10);
    /* 获取当前的系统时间， 用于超时判断 */
    vTaskSetTimeOutState(&Timeout);
    //BSP_Printf("[%d] %s\r\n", __LINE__, cmd);
    Usart1_Send_Str(cmd);
    while (1)
    {
        if (usart_data->len > 0)
        {
            if (strstr((char *)usart_data->rxbuf, ack))
            {
                //BSP_Printf("[%d]%s\r\n", __LINE__, usart_data->rxbuf);
                Usart1_Send_Str((char *)usart_data->rxbuf);
                return TRUE;
            }
            else if (strlen(ack2) > 0 && strstr((char *)usart_data->rxbuf, ack2))
            {
                //BSP_Printf("[%d]%s\r\n", __LINE__, usart_data->rxbuf);
                Usart1_Send_Str((char *)usart_data->rxbuf);
                return TRUE;
            }
            else if (strstr((char *)usart_data->rxbuf, "ERROR"))
            {
                //BSP_Printf("[%d]%s\r\n", __LINE__, usart_data->rxbuf);
                Usart1_Send_Str((char *)usart_data->rxbuf);
                return FALSE;
            }
            else if (strlen(err) > 0 && (strstr((char *)usart_data->rxbuf, err) != NULL))
            {
                //BSP_Printf("[%d ERR:]%s\r\n", __LINE__, usart_data->rxbuf);
                Usart1_Send_Str((char *)usart_data->rxbuf);
                return FALSE;
            }
            else
            {
                //BSP_Printf("[%d]%s\r\n", __LINE__, usart_data->rxbuf);
                //return FALSE;
            }
        }
        if (xTaskCheckForTimeOut(&Timeout, &xMaxBlockTime) == pdTRUE)
        {
            /* 超时 */
            
            //BSP_Printf("[ERR:] Timeout\r\n");
            Usart1_Send_Str("Timeout\r\n");
            //osDelay(10);
            return FALSE;
        }
        //USART1->DR = 'A';
        osDelay(10);
    }
}

uint8_t find_ec20(void)
{
    return ec20_send_cmd("AT\r\n", "OK", "", "ERROR", 1000);
}
uint8_t set_ec20_echo(void)
{
    return ec20_send_cmd("ATE0\r\n", "OK", "", "ERROR", 1000);
}
uint8_t find_ec20_sim(void)
{
    return ec20_send_cmd("AT+QSIMSTAT?\r\n", "+QSIMSTAT: 0,1", "+QSIMSTAT: 0,1", "ERROR", 1000);
}
uint8_t check_ec20_sim_state(void)
{
    return ec20_send_cmd("AT+CPIN?\r\n", "+CPIN: READY", "", "ERROR", 1000);
}
uint8_t check_ec20_csq(void)
{
    return ec20_send_cmd("AT+CSQ\r\n", "OK", "", "ERROR", 1000);
}
uint8_t check_ec20_creg(void)
{
    return ec20_send_cmd("AT+CREG?\r\n", "+CREG: 0,1", "+CREG: 0,5", "ERROR", 1000);
}
uint8_t check_ec20_net_state(void)
{
    return ec20_send_cmd("AT+CGREG?\r\n", "+CGREG: 0,1", "+CGREG: 0,5", "ERROR", 1000);
}
uint8_t check_ec20_cgatt_state(void)
{
    return ec20_send_cmd("AT+CGATT?\r\n", "+CGATT: 1", "", "ERROR", 5000);
}
uint8_t set_ec20_cgatt(void)
{
    return ec20_send_cmd("AT+CGATT=1\r\n", "OK", "", "ERROR", 5000);
}
/* 设置上下文，APN */
uint8_t set_ec20_context(void)
{
    char str[50];
    sprintf(str, "AT+QICSGP=%s,1,\"CMNET\"\r\n", CONTEXT_ID);
    return ec20_send_cmd(str, "OK", "", "ERROR", 1000);
}
/* 检查上下文 */
uint8_t check_ec20_context(void)
{
    return ec20_send_cmd("AT+QIACT?\r\n", "OK", "", "ERROR", 1000);
}
/* 激活上下文 */
uint8_t active_ec20_context(void)
{
    char str[50];
    sprintf(str, "AT+QIACT=%s\r\n", CONTEXT_ID);
    return ec20_send_cmd(str, "OK", "", "ERROR", 1000);
}
uint8_t set_ec20_at_power_off(void)
{
    return ec20_send_cmd("AT+QPOWD\r\n", "OK", "", "ERROR", 2000);
}
/* TCP 连接函数 */
const char SOCKEA_CONNECT_ID[] = "1";
const char LOCA_PORT[] = "5555"; //建立连接的本地端口
const char Access_Mode[] = "2";  //0为非透传Buffer模式， 1为非透传Push模式，2为透传模式
uint8_t ec20_tcp_connect(char *ip, char *port, uint32_t _timeout)
{
    char str[50];
    sprintf(str, "AT+QIOPEN=%s,%s,\"TCP\",\"%s\",%s,%s,%s\r\n", CONTEXT_ID, SOCKEA_CONNECT_ID, ip, port, LOCA_PORT, Access_Mode);
    return ec20_send_cmd(str, "CONNECT", "", "NO CARRIER", _timeout);
}

/* 合成ntrip server头部 FindMM需要使用 ntrip server连接 挂载点为站点的账号*/
void get_ntrip_server(char *dst, char *mountpoint, char *username, char *pwd)
{
    uint16_t len = 0;
    len = sprintf(dst, "SOURCE %s /%s\r\n", pwd, mountpoint);
    len += sprintf((dst+len), "Source-Agent: NTRIP HUASINtripServerCMD/1.0\r\n\r\n");

}

/* ntrip server 连接 */
uint8_t Ntrip_Server_Connect(char *ntrip)
{
    return ec20_send_cmd(ntrip, "ICY 200 OK", "", "ERROR", 5000);
}

uint8_t set_ec20_power_on(void)
{
    HAL_GPIO_WritePin(GPIOC, EC20_POWER_KEY_Pin, GPIO_PIN_SET);
    osDelay(500);
    HAL_GPIO_WritePin(GPIOC, EC20_POWER_KEY_Pin, GPIO_PIN_RESET);
    return 1;
}

uint8_t set_ec20_power_off(void)
{
    HAL_GPIO_WritePin(GPIOC, EC20_POWER_KEY_Pin, GPIO_PIN_SET);
    osDelay(650);
    HAL_GPIO_WritePin(GPIOC, EC20_POWER_KEY_Pin, GPIO_PIN_RESET);
    return 1;
}

uint8_t set_ec20_rst(void)
{
    HAL_GPIO_WritePin(GPIOC, EC20_RST_KEY_Pin, GPIO_PIN_SET);
    osDelay(500);
    HAL_GPIO_WritePin(GPIOC, EC20_RST_KEY_Pin, GPIO_PIN_RESET);
    return 1;
}
/* 4G模块相关的初始化 */
uint8_t ec20_init(void)
{
    //uint8_t err = 0;
    switch (EC20_INTE)
    {
    case EC20_Power_S:
        set_ec20_power_on();
        EC20_INTE = EC20_Find_S;
        break;
    case EC20_Find_S:
        if (find_ec20() == TRUE)
        {
            EC20_INTE = EC20_Set_Echo_S;
            osDelay(100);
        }
            
        break;
    case EC20_Set_Echo_S:
        if (set_ec20_echo() == TRUE)
        {
            EC20_INTE = EC20_Find_Sim_S;
            osDelay(100);
        }
            
        break;
    case EC20_Find_Sim_S:
        if (find_ec20_sim() == TRUE)
        {
            EC20_INTE = EC20_Check_Sim_S;
            osDelay(100);
        }
            
        break;
    case EC20_Check_Sim_S:
        if (check_ec20_sim_state() == TRUE)
            EC20_INTE = EC20_Check_Csq_S;
        break;
    case EC20_Check_Csq_S:
        if (check_ec20_csq() == TRUE)
            EC20_INTE = EC20_Check_Creg_S;
        break;
    case EC20_Check_Creg_S:
        if (check_ec20_creg() == TRUE)
            EC20_INTE = EC20_Check_Net_S;
        break;
    case EC20_Check_Net_S:
        if (check_ec20_net_state() == TRUE)
            EC20_INTE = EC20_Check_Cgatt_S;
        break;
    case EC20_Check_Cgatt_S:
        if (check_ec20_cgatt_state() == TRUE)
            EC20_INTE = EC20_Set_Cgatt_S;
        break;
    case EC20_Set_Cgatt_S:
        if (set_ec20_cgatt() == TRUE)
            EC20_INTE = EC20_Set_Context_S;
        break;
    case EC20_Set_Context_S:
        if (set_ec20_context() == TRUE)
            EC20_INTE = EC20_Check_Context_S;
        break;
    case EC20_Check_Context_S:
        if (check_ec20_context() == TRUE)
            EC20_INTE = EC20_Active_Context_S;
        break;
    case EC20_Active_Context_S:
        if (active_ec20_context() == TRUE)
        {
            EC20_INTE = EC20_Power_S;
            return TRUE;
        }

        break;
    default:
        break;
    }
    return FALSE;
}

void ec20_run(void)
{
    switch (EC20_RUNE)
    {
    case EC20_Init_R:
        if (ec20_init() == TRUE)
            EC20_RUNE = EC20_ConnectInit_R;
        break;
    case EC20_ConnectInit_R:
        
            EC20_RUNE = EC20_Connect_R;
        break;
    case EC20_Connect_R:
        if (ec20_tcp_connect("127.0.0.1", "8000", 5000) == TRUE)
        EC20_RUNE = EC20_Verify_R;
        break;
    case EC20_Verify_R:
        EC20_RUNE = EC20_Send_R;
        break;
    case EC20_Send_R:

        break;
    case EC20_Disconnect_R:

        break;
    default:
        break;
    }
}
