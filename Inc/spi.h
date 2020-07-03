/**
  ******************************************************************************
  * File Name          : SPI.h
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */
#define W25Q80 0XEF13
#define W25Q16 0XEF14
#define W25Q32 0XEF15
#define W25Q64 0XEF16
#define W25Q128 0XEF17
#define W25Q256 0XEF18

    extern uint16_t W25QXX_TYPE; //定义W25QXX芯片型号
#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)


//////////////////////////////////////////////////////////////////////////////////
//指令表
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg1 0x05
#define W25X_ReadStatusReg2 0x35
#define W25X_ReadStatusReg3 0x15
#define W25X_WriteStatusReg1 0x01
#define W25X_WriteStatusReg2 0x31
#define W25X_WriteStatusReg3 0x11
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
#define W25X_Enable4ByteAddr 0xB7
#define W25X_Exit4ByteAddr 0xE9

/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */
    void W25QXX_Init(void);
    uint16_t W25QXX_ReadID(void);                    //读取FLASH ID
    uint8_t W25QXX_ReadSR(uint8_t regno);            //读取状态寄存器
    void W25QXX_4ByteAddr_Enable(void);              //使能4字节地址模式
    void W25QXX_Write_SR(uint8_t regno, uint8_t sr); //写状态寄存器
    void W25QXX_Write_Enable(void);                  //写使能
    void W25QXX_Write_Disable(void);                 //写保护
    void W25QXX_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
    void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);    //读取flash
    void W25QXX_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); //写入flash
    void W25QXX_Erase_Chip(void);                                                     //整片擦除
    void W25QXX_Erase_Sector(uint32_t Dst_Addr);                                      //扇区擦除
    void W25QXX_Wait_Busy(void);                                                      //等待空闲
    void W25QXX_PowerDown(void);                                                      //进入掉电模式
    void W25QXX_WAKEUP(void);                                                         //唤醒
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
