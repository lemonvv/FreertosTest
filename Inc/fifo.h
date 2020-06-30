#ifndef __FIFO_H__
#define __FIFO_H__

#include "main.h"

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

typedef struct
{
    uint8_t *buf;
    __IO uint16_t write;
    __IO uint16_t read;
    __IO uint16_t len;
    uint16_t bufSize;
}FIFO_T;
#endif
