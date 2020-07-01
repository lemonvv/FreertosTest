#include "fifo.h"
#include <string.h>

/* 
    读取fifo里面的数据 一次读取一字节 读取成功时返回1 无数据返回0
    没有使用互斥，直接关了中断
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

/* 往fifo里面写入数据，一次写入一字节，当缓冲区满了以后会覆盖旧数据，读指针会往前移，根据实际情况修改 */
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
    /* 写满了，覆盖就参数， 读索引往前移 */
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
/* 获取队列中元素总长度 */
uint16_t queue_get_len(FIFO_T *ft)
{
    uint16_t len;
    DISABLE_INT();
    len = ft->len;
    ENABLE_INT();
    return len;
}
/* 清空fifo数据 */
void queue_clean_fifo(FIFO_T *ft)
{
    DISABLE_INT();
    ft->write = 0;
    ft->read = 0;
    ft->len = 0;
    memset(ft->buf, 0, ft->bufSize);
    ENABLE_INT();
}
