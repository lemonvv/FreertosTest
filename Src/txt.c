#include "txt.h"
#include "ff.h"
#include "usart.h"

char USER_Path[4] = "0:"; /* USER logical drive path */

FATFS fs;
void mount_disk(void)
{
    uint8_t res = f_mount(&fs, USER_Path, 0);
    if (res != FR_OK)
    {
        if(res == FR_NO_FILESYSTEM)
        {
            BSP_Printf("FR_NO_FILESYSTEM\n");
            res = f_mkfs(USER_Path, FM_ANY, 1, &work, 4096);
            if(res != FR_OK)
            {
                BSP_Printf("failed with\n");
            }
            else
            {
                BSP_Printf("Fatfs Init OK\n");
                res = f_mount(&fs, USER_Path, 0);
                if(res != FR_OK)
                {
                    BSP_Printf("failed error\n");
                }
                else
                {
                    BSP_Printf("MOUNT OK\n");
                }
            }
        }
        else
        {
            BSP_Printf("F error\n");
        }
        return;
    }
    BSP_Printf("MOUNT OK\n");
}

void format_disk(void)
{
    uint8_t res = 0;
    BSP_Printf("PROCESSING...\n");
    res = f_mkfs(USER_Path, FM_ANY, 1, &work, 4096);
    if (res == FR_OK)
    {
        BSP_Printf("OK!\n");
    }
    else
    {
        BSP_Printf("failed with: %d\n", res);
    }
}

void create_file(void)
{
    FIL file;
    FIL *pf = &file;
    uint8_t res;

    res = f_open(pf, "0:/test.txt", FA_OPEN_ALWAYS | FA_WRITE);
    if (res == FR_OK)
    {
        BSP_Printf("creat ok\n");
    }
    else
    {
        BSP_Printf("creat failed\n");
        BSP_Printf("error code: %d\n", res);
    }

    f_printf(pf, "hello fatfs!\n");

    res = f_close(pf);
    if (res != FR_OK)
    {
        BSP_Printf("close file error\n");
        BSP_Printf("error code: %d\n", res);
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
        BSP_Printf("%d KB Total Drive Space.\n"
               "%d KB Available Space.\n",
               ((fls->n_fatent - 2) * fls->csize) * 4, (fre_clust * fls->csize) * 4);
    }
    else
    {
        BSP_Printf("get disk info error\n");
        BSP_Printf("error code: %d\n", res);
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
        BSP_Printf("open error: %d\n", res);
        return;
    }
    f_read(&file, rbuf, 20, &bw);
    BSP_Printf("%s\n", rbuf);

    res = f_close(&file);
    if (res != FR_OK)
    {
        BSP_Printf("close file error\n");
        BSP_Printf("error code: %d\n", res);
    }
}

void Test_Txt(void)
{
    mount_disk();
    create_file();
    read_file();

}

void test_txt_creat(void)
{
    
}