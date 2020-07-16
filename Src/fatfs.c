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
#include "ff.h"
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
    //osDelay(100);
    UINT bw;    //д��ĳ���
    UINT br;    //ʵ��д��ĳ���
    DIR dir;    /* Ŀ¼ */
    FILINFO fno;
    uint8_t buf[100] = {0};
    uint8_t mkdir_flg = 0;
    char openpath[30];
    char uspath[20] = {0};
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
    
    /* �򿪸�Ŀ¼ */
    retUSER = f_opendir(&dir, USERPath);
    if(retUSER == FR_OK)
    {
        printf("opendir %s\r\n", USERPath);
        while(1)
        {
            //osDelay(1);
            retUSER = f_readdir(&dir, &fno);
            /* ���ش��� ���ߵ���β��ֱ���˳�*/
            if(retUSER != FR_OK || !fno.fname[0])
            {
                printf("exit \r\n");
                break;
            }
            if(fno.fname[0] == '.')/* ������.��ʼ���ļ��л����ļ� */
            {
                printf("continue\r\n");
                continue;
            }
            if(strstr(fno.fname, "."))  /* ���Դ�.�������ļ������ļ��� */
            {
                printf("continue ...\r\n");
                continue;
            }
                
            if(fno.fattrib & AM_DIR) //Ŀ¼
            {
                printf("filename: %s\r\n", fno.fname);
                
                if(strstr(fno.fname, "update"))
                {
                    printf("dir update\r\n");
                    mkdir_flg = 1;
                    
                    sprintf(uspath, "%s%s",USERPath,fno.fname);
                    
                    retUSER = f_opendir(&dir, uspath);
                    if(retUSER == FR_OK)
                    {
                        printf("open dir %s\r\n",uspath);
                        sprintf(openpath, "%s/dirt.txt", uspath);
                        retUSER = f_open(&USERFile, openpath, FA_OPEN_ALWAYS | FA_WRITE);
                        if(retUSER == FR_OK)
                        {
                            printf("Open OK\r\n");
                            retUSER = f_lseek(&USERFile, f_size(&USERFile));   //�ƶ���ĩβ��׷��
                            retUSER = f_write(&USERFile, "123456!", 7, &bw);
                            printf("write res:%d bw:%d\r\n", retUSER, bw);
                            retUSER = f_close(&USERFile);
                            printf("close %d\r\n", retUSER);
                        }
                        else
                        {
                            printf("open error\r\n");
                        }
                    }
                }
                else 
                {
                    printf("not found update\r\n");
                }
            }
            else    //�ļ�
            {

            }
        }
        if(mkdir_flg == 0)
        {
            retUSER = f_mkdir("update");
            if(retUSER == FR_OK)
            {
                printf("mkdir update ok \r\n");
            }
        }
        retUSER = f_findfirst(&dir, &fno, uspath, "*.txt"); /* ����uspathĿ¼�µ�txt�ļ� ʹ�øú�����Ҫ����Ӧ�ĺ�� */
        //printf(" find res:%d name:%s\r\n", retUSER, fno.fname);
        
        while (retUSER == FR_OK && fno.fname[0]) 
        {         /* Repeat while an item is found */
            printf("%s\n", fno.fname);                /* Print the object name */
            retUSER = f_findnext(&dir, &fno);               /* Search for next item */
        }

        f_closedir(&dir);

    }
    #if 0
    
    if(retUSER == FR_OK)
    {
        retUSER = f_open(&USERFile, "0:/test.txt", FA_OPEN_ALWAYS | FA_WRITE);
        printf("open res %d\r\n", retUSER);
        if(retUSER == FR_OK)
        {
            printf("Open OK\r\n");
            retUSER = f_lseek(&USERFile, f_size(&USERFile));   //�ƶ���ĩβ��׷��
            retUSER = f_write(&USERFile, "test0!", 7, &bw);
            printf("write res:%d bw:%d\r\n", retUSER, bw);
            retUSER = f_close(&USERFile);
            printf("close %d\r\n", retUSER);
        }
    }
    #endif
/*     if(retUSER == FR_OK)
    {
        retUSER = f_open(&USERFile, "0:/test1.txt", FA_READ);
        printf("res %d\r\n", retUSER);
        if(retUSER == FR_OK)
        {
            retUSER = f_read(&USERFile, buf, 7, &br);
            printf("res %d\r\n read:%s\r\n", retUSER, buf);
            retUSER = f_close(&USERFile);
            printf("res %d\r\n", retUSER);
        }
        

    } */
    //if(retUSER == FR_OK)
    {
        /* ��ʱ�򴴽����ļ�ʹ��f_unlink�޷�����ɾ�� ��ʱ�Ľ���취Ҳ�Ƚϼ򵥣�ֱ��f_unmountж�أ�Ȼ�����µ��ú���f_mount���أ��ٵ���f_unlink���� */
        //retUSER = f_unlink("0:/test02.txt");    /* ɾ���ļ� */

        //printf("f_unlink res %d\r\n", retUSER);
    }
}


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
