#include "txt.h"
#include "ff.h"
#include "usart.h"

char USER_Path[4] = "0:"; /* USER logical drive path */

FATFS fs;
BYTE work[1024];
void mount_disk(void)
{
    uint8_t res = f_mount(&fs, USER_Path, 0);
    if (res != FR_OK)
    {
        if(res == FR_NO_FILESYSTEM)
        {
            Usart1_Send_Str("FR_NO_FILESYSTEM\n");
            res = f_mkfs(USER_Path, FM_ANY, 1, &work, 4096);
            if(res != FR_OK)
            {
                Usart1_Send_Str("failed with\n");
            }
            else
            {
                Usart1_Send_Str("Fatfs Init OK\n");
                res = f_mount(&fs, USER_Path, 0);
                if(res != FR_OK)
                {
                    Usart1_Send_Str("failed error\n");
                }
                else
                {
                    Usart1_Send_Str("MOUNT OK 2\n");
                }
            }
        }
        else
        {
            Usart1_Send_Str("F error\n");
        }
        return;
    }
    Usart1_Send_Str("MOUNT OK 1\n");
}

void format_disk(void)
{
    uint8_t res = 0;
    Usart1_Send_Str("PROCESSING...\n");
    res = f_mkfs(USER_Path, FM_ANY, 1, &work, 4096);
    if (res == FR_OK)
    {
        Usart1_Send_Str("OK!\n");
    }
    else
    {
        Usart1_Send_Str("failed with\n");
    }
}

void create_file(void)
{
    FIL file;
    FIL *pf = &file;
    uint8_t res;
    Usart1_Send_Str("Open\n");
    res = f_open(pf, "0:/test.txt", FA_OPEN_ALWAYS | FA_WRITE);
    Usart1_Send_Str("Open 2\n");
    if (res == FR_OK)
    {
        Usart1_Send_Str("creat ok\n");
    }
    else
    {
        Usart1_Send_Str("creat failed\n");
        Usart1_Send_Str("error code:\n");
    }

    f_printf(pf, "hello fatfs!\n");

    res = f_close(pf);
    if (res != FR_OK)
    {
        Usart1_Send_Str("close file error\n");
        Usart1_Send_Str("error code: \n");
    }
}

void get_disk_info(void)
{
    FATFS fs;
    FATFS *fls = &fs;
    FRESULT res;
    DWORD fre_clust;

    res = f_getfree("/", &fre_clust, &fls); /* Get Number of Free Clusters */
    if (res == FR_OK)
    {
        /* Print free space in unit of MB (assuming 4096 bytes/sector) */
        //Usart1_Send_Str(((fls->n_fatent - 2) * fls->csize) * 4, (fre_clust * fls->csize) * 4);
        Usart1_Send_Str("OK\n");
    }
    else
    {
        Usart1_Send_Str("get disk info error\n");
        Usart1_Send_Str("error code: \n");
    }
}

void read_file(void)
{
    FIL file;
    FRESULT res;
    UINT bw;
    uint8_t rbuf[100] = {0};

    res = f_open(&file, "0:/test.txt", FA_READ);
    if (res != FR_OK)
    {
        Usart1_Send_Str("open error: \n");
        return;
    }
    f_read(&file, rbuf, 20, &bw);
    Usart1_Send_Str((char *)rbuf);

    res = f_close(&file);
    if (res != FR_OK)
    {
        Usart1_Send_Str("close file error\n");
        Usart1_Send_Str("error code: \n");
    }
}

void Test_Txt(void)
{
    mount_disk();
    Usart1_Send_Str("last\n");
    create_file();
    //read_file();

}

void test_txt_creat(void)
{
    
}
