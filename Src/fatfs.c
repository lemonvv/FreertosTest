/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retUSER;    /* Return value for USER */
char USERPath[4];   /* USER logical drive path */
FATFS USERFatFS;    /* File system object for USER logical drive */
FIL USERFile;       /* File object for USER */

/* USER CODE BEGIN Variables */
#include "usart.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the USER driver ###########################*/
  retUSER = FATFS_LinkDriver(&USER_Driver, USERPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */     
  /* USER CODE END Init */
}

/* USER CODE BEGIN Application */
BYTE work[4096];

void tset_fatfs(void)
{
    UINT bw;
    UINT br;
    uint8_t buf[100];
    retUSER = f_mount(&USERFatFS, USERPath, 1);
    printf("res %d\r\n", retUSER);
    if(retUSER == FR_NO_FILESYSTEM)
    {
        retUSER = f_mkfs(USERPath, FM_ANY, 0, work, sizeof(work));
        printf("res %d\r\n", retUSER);
        if(retUSER != FR_OK)
        {
            printf("ERROR\r\n");
            return;
           
        }
        printf("Filesystem OK\r\n");
    }
    printf("Mount OK\r\n");
    if(retUSER == FR_OK)
    {
        retUSER = f_open(&USERFile, "0:/test.txt", FA_OPEN_ALWAYS | FA_WRITE);
        printf("res %d\r\n", retUSER);
        if(retUSER == FR_OK)
        {
            printf("Open OK\r\n");
            retUSER = f_write(&USERFile, "Hello!", 6, &bw);
            printf("res %d bw %d\r\n", retUSER, bw);
            retUSER = f_close(&USERFile);
            printf("res %d\r\n", retUSER);
        }
    }
    if(retUSER == FR_OK)
    {
        retUSER = f_open(&USERFile, "0:/test.txt", FA_READ);
        printf("res %d\r\n", retUSER);
        retUSER = f_read(&USERFile, buf, 6, &br);
        printf("res %d\r\n read:%s\r\n", retUSER, buf);
        retUSER = f_close(&USERFile);
        printf("res %d\r\n", retUSER);
    }
}


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
