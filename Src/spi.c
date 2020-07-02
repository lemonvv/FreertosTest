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
uint16_t W25QXX_TYPE = W25Q16;
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
        /* USER CODE END SPI1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&hspi1, &TxData, &Rxdata, 1, 1000);
    return Rxdata; //�����յ�������
}

//��ȡW25QXX��״̬�Ĵ�����W25QXXһ����3��״̬�Ĵ���
//״̬�Ĵ���1��
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
//״̬�Ĵ���2��
//BIT7  6   5   4   3   2   1   0
//SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
//״̬�Ĵ���3��
//BIT7      6    5    4   3   2   1   0
//HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
//regno:״̬�Ĵ����ţ���:1~3
//����ֵ:״̬�Ĵ���ֵ
uint8_t W25QXX_ReadSR(uint8_t regno)
{
    uint8_t byte = 0, command = 0;
    switch (regno)
    {
    case 1:
        command = W25X_ReadStatusReg1; //��״̬�Ĵ���1ָ��
        break;
    case 2:
        command = W25X_ReadStatusReg2; //��״̬�Ĵ���2ָ��
        break;
    case 3:
        command = W25X_ReadStatusReg3; //��״̬�Ĵ���3ָ��
        break;
    default:
        command = W25X_ReadStatusReg1;
        break;
    }
    FLASH_SPI_CS_ENABLE();                   //ʹ������
    SPI1_ReadWriteByte(command);     //���Ͷ�ȡ״̬�Ĵ�������
    byte = SPI1_ReadWriteByte(0Xff); //��ȡһ���ֽ�
    FLASH_SPI_CS_DISABLE();                   //ȡ��Ƭѡ
    return byte;
}
//дW25QXX״̬�Ĵ���
void W25QXX_Write_SR(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;
    switch (regno)
    {
    case 1:
        command = W25X_WriteStatusReg1; //д״̬�Ĵ���1ָ��
        break;
    case 2:
        command = W25X_WriteStatusReg2; //д״̬�Ĵ���2ָ��
        break;
    case 3:
        command = W25X_WriteStatusReg3; //д״̬�Ĵ���3ָ��
        break;
    default:
        command = W25X_WriteStatusReg1;
        break;
    }
    FLASH_SPI_CS_ENABLE();               //ʹ������
    SPI1_ReadWriteByte(command); //����дȡ״̬�Ĵ�������
    SPI1_ReadWriteByte(sr);      //д��һ���ֽ�
    FLASH_SPI_CS_DISABLE();               //ȡ��Ƭѡ
}
//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(void)
{
    FLASH_SPI_CS_ENABLE();                        //ʹ������
    SPI1_ReadWriteByte(W25X_WriteEnable); //����дʹ��
    FLASH_SPI_CS_DISABLE();                        //ȡ��Ƭѡ
}
//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void)
{
    FLASH_SPI_CS_ENABLE();                         //ʹ������
    SPI1_ReadWriteByte(W25X_WriteDisable); //����д��ָֹ��
    FLASH_SPI_CS_DISABLE();                         //ȡ��Ƭѡ
}

//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
//0XEF18,��ʾоƬ�ͺ�ΪW25Q256
uint16_t W25QXX_ReadID(void)
{
    uint16_t Temp = 0;
    FLASH_SPI_CS_ENABLE();
    SPI1_ReadWriteByte(0x90); //���Ͷ�ȡID����
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    Temp |= SPI1_ReadWriteByte(0xFF) << 8;
    Temp |= SPI1_ReadWriteByte(0xFF);
    FLASH_SPI_CS_DISABLE();
    return Temp;
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    uint16_t i;
    FLASH_SPI_CS_ENABLE();                     //ʹ������
    SPI1_ReadWriteByte(W25X_ReadData); //���Ͷ�ȡ����
    if (W25QXX_TYPE == W25Q256)        //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((uint8_t)((ReadAddr) >> 24));
    }
    SPI1_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); //����24bit��ַ
    SPI1_ReadWriteByte((uint8_t)((ReadAddr) >> 8));
    SPI1_ReadWriteByte((uint8_t)ReadAddr);
    for (i = 0; i < NumByteToRead; i++)
    {
        pBuffer[i] = SPI1_ReadWriteByte(0XFF); //ѭ������
    }
    FLASH_SPI_CS_DISABLE();
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    W25QXX_Write_Enable();                //SET WEL
    FLASH_SPI_CS_ENABLE();                        //ʹ������
    SPI1_ReadWriteByte(W25X_PageProgram); //����дҳ����
    if (W25QXX_TYPE == W25Q256)           //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((uint8_t)((WriteAddr) >> 24));
    }
    SPI1_ReadWriteByte((uint8_t)((WriteAddr) >> 16)); //����24bit��ַ
    SPI1_ReadWriteByte((uint8_t)((WriteAddr) >> 8));
    SPI1_ReadWriteByte((uint8_t)WriteAddr);
    for (i = 0; i < NumByteToWrite; i++)
        SPI1_ReadWriteByte(pBuffer[i]); //ѭ��д��
    FLASH_SPI_CS_DISABLE();                      //ȡ��Ƭѡ
    W25QXX_Wait_Busy();                 //�ȴ�д�����
}
//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t pageremain;
    pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���
    if (NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; //������256���ֽ�
    while (1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if (NumByteToWrite == pageremain)
            break; //д�������
        else       //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain; //��ȥ�Ѿ�д���˵��ֽ���
            if (NumByteToWrite > 256)
                pageremain = 256; //һ�ο���д��256���ֽ�
            else
                pageremain = NumByteToWrite; //����256���ֽ���
        }
    };
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//uint8_t W25QXX_BUFFER[4096];
void W25QXX_Write(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    #if 0
    uint16_t i;
    uint8_t *W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    #endif
    secpos = WriteAddr / 4096; //������ַ
    secoff = WriteAddr % 4096; //�������ڵ�ƫ��
    secremain = 4096 - secoff; //����ʣ��ռ��С
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
    if (NumByteToWrite <= secremain)
        secremain = NumByteToWrite; //������4096���ֽ�
    while (1)
    {
        #if 0
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096); //������������������
        for (i = 0; i < secremain; i++)               //У������
        {
            if (W25QXX_BUF[secoff + i] != 0XFF)
                break; //��Ҫ����
        }
        if (i < secremain) //��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);    //�����������
            for (i = 0; i < secremain; i++) //����
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //д����������
        }
        else
        #endif
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //д�Ѿ������˵�,ֱ��д������ʣ������.
        if (NumByteToWrite == secremain)
            break; //д�������
        else       //д��δ����
        {
            secpos++;   //������ַ��1
            secoff = 0; //ƫ��λ��Ϊ0

            pBuffer += secremain;        //ָ��ƫ��
            WriteAddr += secremain;      //д��ַƫ��
            NumByteToWrite -= secremain; //�ֽ����ݼ�
            if (NumByteToWrite > 4096)
                secremain = 4096; //��һ����������д����
            else
                secremain = NumByteToWrite; //��һ����������д����
        }
    };
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable(); //SET WEL
    W25QXX_Wait_Busy();
    FLASH_SPI_CS_ENABLE();                      //ʹ������
    SPI1_ReadWriteByte(W25X_ChipErase); //����Ƭ��������
    FLASH_SPI_CS_DISABLE();                      //ȡ��Ƭѡ
    W25QXX_Wait_Busy();                 //�ȴ�оƬ��������
}
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ������������ʱ��:150ms
void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{
    //����falsh�������,������
    //printf("fe:%x\r\n",Dst_Addr);
    Dst_Addr *= 4096;
    W25QXX_Write_Enable(); //SET WEL
    W25QXX_Wait_Busy();
    FLASH_SPI_CS_ENABLE();                        //ʹ������
    SPI1_ReadWriteByte(W25X_SectorErase); //������������ָ��
    if (W25QXX_TYPE == W25Q256)           //�����W25Q256�Ļ���ַΪ4�ֽڵģ�Ҫ�������8λ
    {
        SPI1_ReadWriteByte((uint8_t)((Dst_Addr) >> 24));
    }
    SPI1_ReadWriteByte((uint8_t)((Dst_Addr) >> 16)); //����24bit��ַ
    SPI1_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));
    SPI1_ReadWriteByte((uint8_t)Dst_Addr);
    FLASH_SPI_CS_DISABLE();      //ȡ��Ƭѡ
    W25QXX_Wait_Busy(); //�ȴ��������
}
//�ȴ�����
void W25QXX_Wait_Busy(void)
{
    while ((W25QXX_ReadSR(1) & 0x01) == 0x01)
        ; // �ȴ�BUSYλ���
}
//�������ģʽ
void W25QXX_PowerDown(void)
{
    FLASH_SPI_CS_ENABLE();                      //ʹ������
    SPI1_ReadWriteByte(W25X_PowerDown); //���͵�������
    FLASH_SPI_CS_DISABLE();                      //ȡ��Ƭѡ
    //delay_us(3);                        //�ȴ�TPD
}
//����
void W25QXX_WAKEUP(void)
{
    FLASH_SPI_CS_ENABLE();                             //ʹ������
    SPI1_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
    FLASH_SPI_CS_DISABLE();                             //ȡ��Ƭѡ
    //delay_us(3);                               //�ȴ�TRES1
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
