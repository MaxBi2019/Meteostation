#include "logger.h"
uint32_t byteswritten,bytesread;
uint8_t result;
FRESULT res;
uint32_t free_space;
FATFS SDFatFs;
FATFS *sd_fs;
FIL MyFile;


static char* 
new_buf(char str[])
{
	char* nbuf = malloc(strlen(str));
	for (int x = strlen(str); x > 0; --x)
		nbuf[x-1] = str[x-1];
	return nbuf;
}


void Logger_Init()
{
	disk_initialize(SDFatFs.drv);
}


int write_logs(char str[])
{
	if (!sdinfo.is_init || !sdinfo.is_compatible)
	{
		sdinfo.is_init = 0;
		f_close(&MyFile);
		return 1;
	}
	
	char *tem_buf;
	if(f_mount(&SDFatFs,(TCHAR const*)USERPath,0)!=FR_OK)
	{
		sdinfo.is_init = 0;
		f_close(&MyFile);
		return 2;
	}
	
    f_getfree("/", &free_space, &sd_fs);
    free_space = (free_space*(sd_fs->csize))/(2*1024);
    
    /*
    sprintf(str_buf, "free: %lu MB\r\n", free_space);
    print(str_buf);
	*/
		  
    if (free_space < MIN_SPACE)
    {
    	sdinfo.is_init = 0;
    	f_close(&MyFile);
    	return 3;
    }
			 
	if(f_open(&MyFile, FILE_NAME,FA_WRITE|FA_READ)!=FR_OK)
	{
		sprintf(str_buf, "%s\n%s", HEADER, str);
		sprintf(str, "%s", str_buf);
		if (f_open(&MyFile, FILE_NAME,FA_WRITE|FA_READ|FA_CREATE_ALWAYS)!=FR_OK)
		{
			sdinfo.is_init = 0;
			f_close(&MyFile);
			return 4;
		}
	}
	/*
	sprintf(str_buf, "Previous size of file: %u\r\n", MyFile.obj.objsize);
    print(str_buf);
    */

	if (FR_OK != f_lseek(&MyFile, MyFile.obj.objsize))
	{
		sdinfo.is_init = 0;
		f_close(&MyFile);
		return 5;
	}
	
	tem_buf = new_buf(str);
    res=f_write(&MyFile,tem_buf,strlen(str),(void*)&byteswritten);
	free(tem_buf);
			  
	if((byteswritten==0)||(res!=FR_OK))
	{
		sdinfo.is_init = 0;
		f_close(&MyFile);
		return 6;
	}
	/*
	sprintf(str_buf, "Current  size of file: %u\r\n", MyFile.obj.objsize);
	print(str_buf);
	*/
	f_close(&MyFile);
	
}
