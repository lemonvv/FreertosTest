#include "fifo.h"
#include <string.h>

/* 
    ��ȡfifo��������� һ�ζ�ȡһ�ֽ� ��ȡ�ɹ�ʱ����1 �����ݷ���0
    û��ʹ�û��⣬ֱ�ӹ����ж�
*/
uint8_t queue_read_char(FIFO_T *ft, uint8_t * _byte)
{
    uint16_t len;
    DISABLE_INT();
	len = ft->len;
	ENABLE_INT();
    if(len == 0)
    {
        return 0;
    }
        
    if(ft->write == ft->read)
    {
        return 0;
    }
    *_byte = ft->buf[ft->read];
    DISABLE_INT();
    if(++ft->read >= ft->bufSize)
    {
        ft->read = 0;
    }
    ft->len--;
    ENABLE_INT();
    return 1;
}

/* ��fifo����д�����ݣ�һ��д��һ�ֽڣ��������������Ժ�Ḳ�Ǿ����ݣ���ָ�����ǰ�ƣ�����ʵ������޸� */
uint8_t queue_write_char(FIFO_T *ft, uint8_t *_byte)
{
    DISABLE_INT();
    ft->buf[ft->write] = *_byte;
    if(++ft->write >= ft->bufSize)
    {
        ft->write = 0;
    }
    if(ft->len < ft->bufSize)
    {
        ft->len++;
    }
    /* д���ˣ����ǾͲ����� ��������ǰ�� */
    if(ft->write == ft->read)
    {
        if(++ft->read >= ft->bufSize)
        {
            ft->read = 0;
        }
    }
    ENABLE_INT();
    return 1;
}
/* ��ȡ������Ԫ���ܳ��� */
uint16_t queue_get_len(FIFO_T *ft)
{
    uint16_t len;
    DISABLE_INT();
    len = ft->len;
    ENABLE_INT();
    return len;
}
/* ���fifo���� */
void queue_clean_fifo(FIFO_T *ft)
{
    DISABLE_INT();
    ft->write = 0;
    ft->read = 0;
    ft->len = 0;
    memset(ft->buf, 0, ft->bufSize);
    ENABLE_INT();
}
