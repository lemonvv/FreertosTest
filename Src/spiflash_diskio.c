#include "spiflash_diskio.h"

#include <string.h>
#include "ff_gen_drv.h"
#include "spi.h"

#define SPI_FLASH_REBUILD 1         // 1:ʹ�ܸ�ʽ������Flash��0�����ø�ʽ������Flash
#define SPI_FLASH_SECTOR_SIZE 4096  // ����Flash������С
#define SPI_FLASH_START_SECTOR 1792 // ����Flash�ļ�ϵͳFatFSƫ����
#define SPI_FLASH_SECTOR_COUNT 2304 // ����Flash�ļ�ϵͳFatFSռ����������

/* ˽�б��� ------------------------------------------------------------------*/
static __IO DSTATUS Stat = STA_NOINIT;

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
DSTATUS SPIFLASH_initialize(BYTE);
DSTATUS SPIFLASH_status(BYTE);
DRESULT SPIFLASH_read(BYTE, BYTE *, DWORD, UINT);

#if _USE_WRITE == 1 // �������д����
DRESULT SPIFLASH_write(BYTE, const BYTE *, DWORD, UINT);
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1 // ������������������
DRESULT SPIFLASH_ioctl(BYTE, BYTE, void *);
#endif /* _USE_IOCTL == 1 */

/* ���崮��FLASH�ӿں��� */
const Diskio_drvTypeDef SPIFLASH_Driver =
    {
        SPIFLASH_initialize, // ����FLASH��ʼ��
        SPIFLASH_status,     // ����FLASH״̬��ȡ
        SPIFLASH_read,       // ����FLASH������
#if _USE_WRITE == 1
        SPIFLASH_write, // ����FLASHд����
#endif                  /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
        SPIFLASH_ioctl, // ��ȡ����FLASH��Ϣ
#endif                  /* _USE_IOCTL == 1 */
};

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: ����FLASH��ʼ������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
DSTATUS SPIFLASH_initialize(BYTE lun)
{
#if SPI_FLASH_REBUILD == 1
    static uint8_t startflag = 1;
#endif
    Stat = STA_NOINIT;
    /* ��ʼ��SPIFLASHIO���� */
    MX_SPIFlash_Init();
    /* ��ȡ����FLASH״̬ */
    if (SPI_FLASH_ReadID() == SPI_FLASH_ID)
    {
#if SPI_FLASH_REBUILD == 1
        if (startflag)
        {
            SPI_FLASH_SectorErase(SPI_FLASH_START_SECTOR * SPI_FLASH_SECTOR_SIZE);
            startflag = 0;
        }
#endif
        Stat &= ~STA_NOINIT;
    }
    return Stat;
}

/**
  * ��������: ����FLASH״̬��ȡ
  * �������: lun : ����
  * �� �� ֵ: DSTATUS������FLASH״̬����ֵ
  * ˵    ��: ��
  */
DSTATUS SPIFLASH_status(BYTE lun)
{
    Stat = STA_NOINIT;

    if (SPI_FLASH_ReadID() == SPI_FLASH_ID)
    {
        Stat &= ~STA_NOINIT;
    }
    return Stat;
}

/**
  * ��������: �Ӵ���FLASH��ȡ���ݵ�������
  * �������: lun : ����
  *           buff����Ŷ�ȡ�����ݻ�����ָ��
  *           sector��������ַ(LBA)
  *           count��������Ŀ
  * �� �� ֵ: DSTATUS���������
  * ˵    ��: ��
  */
DRESULT SPIFLASH_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    sector += SPI_FLASH_START_SECTOR;
    SPI_FLASH_BufferRead(buff, sector * SPI_FLASH_SECTOR_SIZE, count * SPI_FLASH_SECTOR_SIZE);
    return RES_OK;
}

/**
  * ��������: ������������д�뵽����FLASH��
  * �������: lun : ����
  *           buff����Ŵ�д�����ݵĻ�����ָ��
  *           sector��������ַ(LBA)
  *           count��������Ŀ
  * �� �� ֵ: DSTATUS���������
  * ˵    ��: ��
  */
#if _USE_WRITE == 1
DRESULT SPIFLASH_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    uint32_t write_addr;
    /* ����ƫ��7MB���ⲿFlash�ļ�ϵͳ�ռ����SPI Flash����9MB�ռ� */
    sector += SPI_FLASH_START_SECTOR;
    write_addr = sector * SPI_FLASH_SECTOR_SIZE;
    SPI_FLASH_SectorErase(write_addr);
    SPI_FLASH_BufferWrite((uint8_t *)buff, write_addr, count * SPI_FLASH_SECTOR_SIZE);
    return RES_OK;
}
#endif /* _USE_WRITE == 1 */

/**
  * ��������: ����������Ʋ���(I/O control operation)
  * �������: lun : ����
  *           cmd����������
  *           buff����Ŵ�д����߶�ȡ���ݵĻ�����ָ��
  * �� �� ֵ: DSTATUS���������
  * ˵    ��: ��
  */
#if _USE_IOCTL == 1
DRESULT SPIFLASH_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;

    switch (cmd)
    {
    /* Make sure that no pending write process */
    case CTRL_SYNC:
        res = RES_OK;
        break;

    /* ��ȡ����FLASH��������Ŀ(DWORD) */
    case GET_SECTOR_COUNT:
        *(DWORD *)buff = SPI_FLASH_SECTOR_COUNT;
        res = RES_OK;
        break;

    /* ��ȡ��д������С(WORD) */
    case GET_SECTOR_SIZE:
        *(WORD *)buff = SPI_FLASH_SECTOR_SIZE;
        res = RES_OK;
        break;

    /* ��ȡ�������С(DWORD) */
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        res = RES_OK;
        break;

    default:
        res = RES_PARERR;
    }

    return res;
}
#endif /* _USE_IOCTL == 1 */

void test_write_txt(void)
{
    /* ע��һ��FatFS�豸������FLASH */
    if (FATFS_LinkDriver(&SPIFLASH_Driver, SPIFLASHPath) == 0)
    {
        //�ڴ���FLASH�����ļ�ϵͳ���ļ�ϵͳ����ʱ��Դ���FLASH��ʼ��
        f_res = f_mount(&fs, (TCHAR const *)SPIFLASHPath, 1);
        printf_fatfs_error(f_res);
        /*----------------------- ��ʽ������ ---------------------------*/
        /* ���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ */
        if (f_res == FR_NO_FILESYSTEM)
        {
            printf("������FLASH��û���ļ�ϵͳ���������и�ʽ��...\n");
            /* ��ʽ�� */
            f_res = f_mkfs((TCHAR const *)SPIFLASHPath, 0, 0);

            if (f_res == FR_OK)
            {
                printf("������FLASH�ѳɹ���ʽ���ļ�ϵͳ��\n");
                /* ��ʽ������ȡ������ */
                f_res = f_mount(NULL, (TCHAR const *)SPIFLASHPath, 1);
                /* ���¹���	*/
                f_res = f_mount(&fs, (TCHAR const *)SPIFLASHPath, 1);
            }
            else
            {
                printf("������ʽ��ʧ�ܡ�����\n");
                while (1)
                    ;
            }
        }
        else if (f_res != FR_OK)
        {
            printf("��������FLASH�����ļ�ϵͳʧ�ܡ�(%d)\n", f_res);
            printf_fatfs_error(f_res);
            while (1)
                ;
        }
        else
        {
            printf("���ļ�ϵͳ���سɹ������Խ��ж�д����\n");
        }

        /*----------------------- �ļ�ϵͳ���ԣ�д���� -----------------------------*/
        /* ���ļ�������ļ��������򴴽��� */
        printf("****** ���������ļ�д�����... ******\n");
        f_res = f_open(&file, "FatFs��д�����ļ�.txt", FA_CREATE_ALWAYS | FA_WRITE);
        if (f_res == FR_OK)
        {
            printf("����/����FatFs��д�����ļ�.txt�ļ��ɹ������ļ�д�����ݡ�\n");
            /* ��ָ���洢������д�뵽�ļ��� */
            f_res = f_write(&file, WriteBuffer, sizeof(WriteBuffer), &fnum);
            if (f_res == FR_OK)
            {
                printf("���ļ�д��ɹ���д���ֽ����ݣ�%d\n", fnum);
                printf("�����ļ�д�������Ϊ��\n%s\n", WriteBuffer);
            }
            else
            {
                printf("�����ļ�д��ʧ�ܣ�(%d)\n", f_res);
            }
            /* ���ٶ�д���ر��ļ� */
            f_close(&file);
        }
        else
        {
            printf("������/�����ļ�ʧ�ܡ�\n");
        }

        /*------------------- �ļ�ϵͳ���ԣ������� ------------------------------------*/
        printf("****** ���������ļ���ȡ����... ******\n");
        f_res = f_open(&file, "FatFs��д�����ļ�.txt", FA_OPEN_EXISTING | FA_READ);
        if (f_res == FR_OK)
        {
            printf("�����ļ��ɹ���\n");
            f_res = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum);
            if (f_res == FR_OK)
            {
                printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\n", fnum);
                printf("����ȡ�õ��ļ�����Ϊ��\n%s \n", ReadBuffer);
            }
            else
            {
                printf("�����ļ���ȡʧ�ܣ�(%d)\n", f_res);
            }
        }
        else
        {
            printf("�������ļ�ʧ�ܡ�\n");
        }
        /* ���ٶ�д���ر��ļ� */
        f_close(&file);

        /* ����ʹ�ã�ȡ������ */
        f_res = f_mount(NULL, (TCHAR const *)SPIFLASHPath, 1);
    }

    /* ע��һ��FatFS�豸������FLASH */
    FATFS_UnLinkDriver(SPIFLASHPath);
}
