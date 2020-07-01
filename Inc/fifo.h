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

uint8_t queue_read_char(FIFO_T *ft, uint8_t * _byte);
uint8_t queue_write_char(FIFO_T *ft, uint8_t *_byte);
uint16_t queue_get_len(FIFO_T *ft);
void queue_clean_fifo(FIFO_T *ft);
#endif
