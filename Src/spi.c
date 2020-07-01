/**
  ******************************************************************************
  * File Name          : SPI.c
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

/* Includes ------------------------------------------------------------------*/
#include "spi.h"

/* USER CODE BEGIN 0 */
#define SPI_FLASH_PageSize 256
#define SPI_FLASH_PerWritePageSize 256
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg 0x05
#define W25X_WriteStatusReg 0x01
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

#define WIP_Flag 0x01 /* Write In Progress (WIP) flag */

#define Dummy_Byte 0xFF

SPI_HandleTypeDef hspiflash;
/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (spiHandle->Instance == SPI1)
    {
        /* USER CODE BEGIN SPI1_MspInit 0 */

        /* USER CODE END SPI1_MspInit 0 */
        /* SPI1 clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**SPI1 GPIO Configuration    
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI 
    */
        GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI1_MspInit 1 */

        /* USER CODE END SPI1_MspInit 1 */
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle)
{

    if (spiHandle->Instance == SPI1)
    {
        /* USER CODE BEGIN SPI1_MspDeInit 0 */

        /* USER CODE END SPI1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /**SPI1 GPIO Configuration    
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    PB5     ------> SPI1_MOSI 
    */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

        /* USER CODE BEGIN SPI1_MspDeInit 1 */
        HAL_GPIO_DeInit(SPI1_CS_GPIO_Port, SPI1_CS_Pin);
        /* USER CODE END SPI1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
/**
  * ��������: ��������
  * �������: SectorAddr��������������ַ��Ҫ��Ϊ4096����
  * �� �� ֵ: ��
  * ˵    ��������Flash��С�������СΪ4KB(4096�ֽ�)����һ��������С��Ҫ���������
  *           Ϊ4096��������������FlashоƬд������֮ǰҪ���Ȳ����ռ䡣
  */
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
    /* ����FLASHдʹ������ */
    SPI_FLASH_WriteEnable();
    SPI_FLASH_WaitForWriteEnd();
    /* �������� */
    /* ѡ����FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();
    /* ������������ָ��*/
    SPI_FLASH_SendByte(W25X_SectorErase);
    /*���Ͳ���������ַ�ĸ�λ*/
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    /* ���Ͳ���������ַ����λ */
    SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    /* ���Ͳ���������ַ�ĵ�λ */
    SPI_FLASH_SendByte(SectorAddr & 0xFF);
    /* ���ô���FLASH: CS �ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();
    /* �ȴ��������*/
    SPI_FLASH_WaitForWriteEnd();
}

/**
  * ��������: ������Ƭ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������������Flash��Ƭ�ռ�
  */
void SPI_FLASH_BulkErase(void)
{
    /* ����FLASHдʹ������ */
    SPI_FLASH_WriteEnable();

    /* ��Ƭ���� Erase */
    /* ѡ����FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();
    /* ������Ƭ����ָ��*/
    SPI_FLASH_SendByte(W25X_ChipErase);
    /* ���ô���FLASH: CS�ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();

    /* �ȴ��������*/
    SPI_FLASH_WaitForWriteEnd();
}

/**
  * ��������: ������FLASH��ҳд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ��ȣ�����С�ڵ���SPI_FLASH_PerWritePageSize
  * �� �� ֵ: ��
  * ˵    ��������Flashÿҳ��СΪ256���ֽ�
  */
void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    /* ����FLASHдʹ������ */
    SPI_FLASH_WriteEnable();

    /* Ѱ�Ҵ���FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();
    /* д��дָ��*/
    SPI_FLASH_SendByte(W25X_PageProgram);
    /*����д��ַ�ĸ�λ*/
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    /*����д��ַ����λ*/
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    /*����д��ַ�ĵ�λ*/
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    if (NumByteToWrite > SPI_FLASH_PerWritePageSize)
    {
        NumByteToWrite = SPI_FLASH_PerWritePageSize;
        //printf("Err: SPI_FLASH_PageWrite too large!\n");
    }

    /* д������*/
    while (NumByteToWrite--)
    {
        /* ���͵�ǰҪд����ֽ����� */
        SPI_FLASH_SendByte(*pBuffer);
        /* ָ����һ�ֽ����� */
        pBuffer++;
    }

    /* ���ô���FLASH: CS �ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();

    /* �ȴ�д�����*/
    SPI_FLASH_WaitForWriteEnd();
}

/**
  * ��������: ������FLASHд�����ݣ����ñ�����д������ǰ��Ҫ�Ȳ�������
  * �������: pBuffer����д�����ݵ�ָ��
  *           WriteAddr��д���ַ
  *           NumByteToWrite��д�����ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú���������������д�����ݳ���
  */
void SPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

    Addr = WriteAddr % SPI_FLASH_PageSize;
    count = SPI_FLASH_PageSize - Addr;
    NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
    NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

    if (Addr == 0) /* ����ַ�� SPI_FLASH_PageSize ����  */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr += SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
        }
    }
    else /* ����ַ�� SPI_FLASH_PageSize ������ */
    {
        if (NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
        {
            if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
            {
                temp = NumOfSingle - count;

                SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
                WriteAddr += count;
                pBuffer += count;

                SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
            }
            else
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
        }
        else /* NumByteToWrite > SPI_FLASH_PageSize */
        {
            NumByteToWrite -= count;
            NumOfPage = NumByteToWrite / SPI_FLASH_PageSize;
            NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;

            SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
            WriteAddr += count;
            pBuffer += count;

            while (NumOfPage--)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
                WriteAddr += SPI_FLASH_PageSize;
                pBuffer += SPI_FLASH_PageSize;
            }

            if (NumOfSingle != 0)
            {
                SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
    }
}

/**
  * ��������: �Ӵ���Flash��ȡ����
  * �������: pBuffer����Ŷ�ȡ�����ݵ�ָ��
  *           ReadAddr����ȡ����Ŀ���ַ
  *           NumByteToRead����ȡ���ݳ���
  * �� �� ֵ: ��
  * ˵    �����ú����������������ȡ���ݳ���
  */
void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    /* ѡ����FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();

    /* ���� �� ָ�� */
    SPI_FLASH_SendByte(W25X_ReadData);

    /* ���� �� ��ַ��λ */
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    /* ���� �� ��ַ��λ */
    SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
    /* ���� �� ��ַ��λ */
    SPI_FLASH_SendByte(ReadAddr & 0xFF);

    while (NumByteToRead--) /* ��ȡ���� */
    {
        /* ��ȡһ���ֽ�*/
        *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
        /* ָ����һ���ֽڻ����� */
        pBuffer++;
    }

    /* ���ô���FLASH: CS �ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();
}

/**
  * ��������: ��ȡ����Flash�ͺŵ�ID
  * �������: ��
  * �� �� ֵ: uint32_t������Flash���ͺ�ID
  * ˵    ����  FLASH_ID      IC�ͺ�      �洢�ռ��С         
                0xEF3015      W25X16        2M byte
                0xEF4015	    W25Q16        4M byte
                0XEF4017      W25Q64        8M byte
                0XEF4018      W25Q128       16M byte
  */
uint32_t SPI_FLASH_ReadID(void)
{
    uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

    /* ѡ����FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();

    /* ���������ȡоƬ�ͺ�ID */
    SPI_FLASH_SendByte(W25X_JedecDeviceID);

    /* �Ӵ���Flash��ȡһ���ֽ����� */
    Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

    /* �Ӵ���Flash��ȡһ���ֽ����� */
    Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

    /* �Ӵ���Flash��ȡһ���ֽ����� */
    Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

    /* ���ô���Flash��CS�ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();

    Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
    return Temp;
}

/**
  * ��������: ��ȡ����Flash�豸ID
  * �������: ��
  * �� �� ֵ: uint32_t������Flash���豸ID
  * ˵    ����
  */
uint32_t SPI_FLASH_ReadDeviceID(void)
{
    uint32_t Temp = 0;

    /* ѡ����FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();

    /* ���������ȡоƬ�豸ID * */
    SPI_FLASH_SendByte(W25X_DeviceID);
    SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_SendByte(Dummy_Byte);
    SPI_FLASH_SendByte(Dummy_Byte);

    /* �Ӵ���Flash��ȡһ���ֽ����� */
    Temp = SPI_FLASH_SendByte(Dummy_Byte);

    /* ���ô���Flash��CS�ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();

    return Temp;
}

/**
  * ��������: ����������ȡ���ݴ�
  * �������: ReadAddr����ȡ��ַ
  * �� �� ֵ: ��
  * ˵    ����Initiates a read data byte (READ) sequence from the Flash.
  *           This is done by driving the /CS line low to select the device,
  *           then the READ instruction is transmitted followed by 3 bytes
  *           address. This function exit and keep the /CS line low, so the
  *           Flash still being selected. With this technique the whole
  *           content of the Flash is read with a single READ instruction.
  */
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr)
{
    /* Select the FLASH: Chip Select low */
    FLASH_SPI_CS_ENABLE();

    /* Send "Read from Memory " instruction */
    SPI_FLASH_SendByte(W25X_ReadData);

    /* Send the 24-bit address of the address to read from -----------------------*/
    /* Send ReadAddr high nibble address byte */
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    /* Send ReadAddr medium nibble address byte */
    SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
    /* Send ReadAddr low nibble address byte */
    SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/**
  * ��������: �Ӵ���Flash��ȡһ���ֽ�����
  * �������: ��
  * �� �� ֵ: uint8_t����ȡ��������
  * ˵    ����This function must be used only if the Start_Read_Sequence
  *           function has been previously called.
  */
uint8_t SPI_FLASH_ReadByte(void)
{
    uint8_t d_read, d_send = Dummy_Byte;
    if (HAL_SPI_TransmitReceive(&hspiflash, &d_send, &d_read, 1, 0xFFFFFF) != HAL_OK)
        d_read = Dummy_Byte;

    return d_read;
}

/**
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
  */
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
    uint8_t d_read, d_send = byte;
    if (HAL_SPI_TransmitReceive(&hspiflash, &d_send, &d_read, 1, 0xFFFFFF) != HAL_OK)
        d_read = Dummy_Byte;

    return d_read;
}

/**
  * ��������: ������Flash��ȡд�����(16bit)���ݲ����հ�������
  * �������: byte������������
  * �� �� ֵ: uint16_t�����յ�������
  * ˵    ������
  */
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord)
{
    uint8_t d_read[2], d_send[2];
    d_send[0] = HalfWord & 0x00FF;
    d_send[1] = (HalfWord >> 8) & 0x00FF;
    if (HAL_SPI_TransmitReceive(&hspiflash, d_send, d_read, 2, 0xFFFFFF) != HAL_OK)
    {
        d_read[0] = Dummy_Byte;
        d_read[1] = Dummy_Byte;
    }

    return ((d_read[1] << 8) + d_read[0]);
}

/**
  * ��������: ʹ�ܴ���Flashд����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SPI_FLASH_WriteEnable(void)
{
    /* ѡ����FLASH: CS�͵�ƽ */
    FLASH_SPI_CS_ENABLE();

    /* �������дʹ�� */
    SPI_FLASH_SendByte(W25X_WriteEnable);

    /* ���ô���Flash��CS�ߵ�ƽ */
    FLASH_SPI_CS_DISABLE();
}

/**
  * ��������: �ȴ�����д�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����Polls the status of the Write In Progress (WIP) flag in the
  *           FLASH's status  register  and  loop  until write  opertaion
  *           has completed.
  */
void SPI_FLASH_WaitForWriteEnd(void)
{
    uint8_t FLASH_Status = 0;

    /* Select the FLASH: Chip Select low */
    FLASH_SPI_CS_ENABLE();

    /* Send "Read Status Register" instruction */
    SPI_FLASH_SendByte(W25X_ReadStatusReg);

    /* Loop as long as the memory is busy with a write cycle */
    do
    {
        /* Send a dummy byte to generate the clock needed by the FLASH
    and put the value of the status register in FLASH_Status variable */
        FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
    } while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

    /* Deselect the FLASH: Chip Select high */
    FLASH_SPI_CS_DISABLE();
}

/**
  * ��������: �������ģʽ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SPI_Flash_PowerDown(void)
{
    /* Select the FLASH: Chip Select low */
    FLASH_SPI_CS_ENABLE();

    /* Send "Power Down" instruction */
    SPI_FLASH_SendByte(W25X_PowerDown);

    /* Deselect the FLASH: Chip Select high */
    FLASH_SPI_CS_DISABLE();
}

/**
  * ��������: ���Ѵ���Flash
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
void SPI_Flash_WAKEUP(void)
{
    /* Select the FLASH: Chip Select low */
    FLASH_SPI_CS_ENABLE();

    /* Send "Power Down" instruction */
    SPI_FLASH_SendByte(W25X_ReleasePowerDown);

    /* Deselect the FLASH: Chip Select high */
    FLASH_SPI_CS_DISABLE();
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
