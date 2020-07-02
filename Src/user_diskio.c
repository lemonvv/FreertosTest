/* USER CODE BEGIN Header */
/**
 ******************************************************************************
  * @file    user_diskio.c
  * @brief   This file includes a diskio driver skeleton to be completed by the user.
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
/* USER CODE END Header */

#ifdef USE_OBSOLETE_USER_CODE_SECTION_0
/* 
 * Warning: the user section 0 is no more in use (starting from CubeMx version 4.16.0)
 * To be suppressed in the future. 
 * Kept to ensure backward compatibility with previous CubeMx versions when 
 * migrating projects. 
 * User code previously added there should be copied in the new user sections before 
 * the section contents can be deleted.
 */
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
#endif

/* USER CODE BEGIN DECL */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "ff_gen_drv.h"
#include "spi.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* USER CODE END DECL */

/* Private function prototypes -----------------------------------------------*/
DSTATUS USER_initialize(BYTE pdrv);
DSTATUS USER_status(BYTE pdrv);
DRESULT USER_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
#if _USE_WRITE == 1
DRESULT USER_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
DRESULT USER_ioctl(BYTE pdrv, BYTE cmd, void *buff);
#endif /* _USE_IOCTL == 1 */

Diskio_drvTypeDef USER_Driver =
    {
        USER_initialize,
        USER_status,
        USER_read,
#if _USE_WRITE
        USER_write,
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
        USER_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_initialize(
    BYTE pdrv /* Physical drive nmuber to identify the drive */
)
{
    /* USER CODE BEGIN INIT */
    if (W25QXX_ReadID() == W25Q16)
    {
        Stat &= ~STA_NOINIT;
    }
    else
        Stat = STA_NOINIT;
    return Stat;
    /* USER CODE END INIT */
}

/**
  * @brief  Gets Disk Status 
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
DSTATUS USER_status(
    BYTE pdrv /* Physical drive number to identify the drive */
)
{
    /* USER CODE BEGIN STATUS */
    if (pdrv != 0)
        Stat = STA_NOINIT;
    return Stat;
    /* USER CODE END STATUS */
}

/**
  * @brief  Reads Sector(s) 
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
DRESULT USER_read(
    BYTE pdrv,    /* Physical drive nmuber to identify the drive */
    BYTE *buff,   /* Data buffer to store read data */
    DWORD sector, /* Sector address in LBA */
    UINT count    /* Number of sectors to read */
)
{
    /* USER CODE BEGIN READ */
    if (pdrv != 0 || count == 0)
        return RES_PARERR;

    W25QXX_Read(buff, sector * 4096, count * 4096);
        return RES_OK;

    /* USER CODE END READ */
}

/**
  * @brief  Writes Sector(s)  
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
#if _USE_WRITE == 1
DRESULT USER_write(
    BYTE pdrv,        /* Physical drive nmuber to identify the drive */
    const BYTE *buff, /* Data to be written */
    DWORD sector,     /* Sector address in LBA */
    UINT count        /* Number of sectors to write */
)
{
    /* USER CODE BEGIN WRITE */
    for (uint8_t i = 0; i < count; i++)
    {
        W25QXX_Write((uint8_t *)buff + (i * 4096), (sector + i) * 4096, 4096);
    }

    /* USER CODE HERE */
    return RES_OK;
    /* USER CODE END WRITE */
}
#endif /* _USE_WRITE == 1 */

/**
  * @brief  I/O control operation  
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
#if _USE_IOCTL == 1
DRESULT USER_ioctl(
    BYTE pdrv, /* Physical drive nmuber (0..) */
    BYTE cmd,  /* Control code */
    void *buff /* Buffer to send/receive control data */
)
{
    /* USER CODE BEGIN IOCTL */
    DRESULT res = RES_ERROR;

    switch (cmd)
    {
    case CTRL_SYNC:
        res = RES_OK;
        break;

    case CTRL_TRIM:
        res = RES_OK;
        break;

    case GET_BLOCK_SIZE:
        /* 块大小 */
        *(DWORD *)buff = 1;
        break;

    case GET_SECTOR_SIZE:
        /* 扇区大小 4K*/
        *(DWORD *)buff = 4096;
        break;

    case GET_SECTOR_COUNT:
        /* w25q16 扇区数量 16Mbit  16/8  = 2M = 2 *1024 = 2048KB = 2048 * 1024 = 2097152 B
            2097152 B / 256(256字节一页) = 8192页 4K一个扇区 4096 / 256 = 16页（16页一个扇区）
            8192 / 16 = 512个扇区
        */
        *(DWORD *)buff = 512;
        break;

    default:
        res = RES_PARERR;
        break;
    }

    return res;
    /* USER CODE END IOCTL */
}
#endif /* _USE_IOCTL == 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
