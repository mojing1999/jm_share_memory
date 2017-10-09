/*****************************************************************************
*  Copyright (C) 2014 - 2017, Justin Mo.
*  All rights reserverd.
*
*  FileName:  	intel_dec.h
*  Author:     Justin Mo(mojing1999@gmail.com)
*  Date:       2017-10-09
*  Version:    V0.01
*  Desc:       This file implement share memory write
*****************************************************************************/
#include <stdio.h>
#include "jm_share_mem.h"
#include <Windows.h>
#include <conio.h>

#pragma comment(lib,"jm_share_memory.lib")

//#define SHARE_MEMORY_NAME ("justin_test_share_memory")
#define SHARE_MEMORY_NAME ("pano_player_share_mem")
#define SHARE_MEM_SIZE (1*1024*1024)


bool is_user_exit()
{
	if (_kbhit()) {
		if ('q' == getch())
			return true;
	}

	return false;
}

int main(int argc, char *argv[])
{
	handle_mem handle;

	FILE *ifile = NULL;
	int len = 0;
	int data_size = 100 * 1024;
	unsigned char *data = new unsigned char[data_size];

	int read_len = 0;

	if (argc < 2) {
		printf("Usage: test_share_mem_write.exe input_file\n");
		return -1;
	}

	char *file_name = argv[1];	//"f:\\test\\arm_demo_day_4K.264";
	ifile = fopen(file_name, "rb");

	if (NULL == ifile) {
		printf("Can not open input file\n");
		return -2;
	}


	handle = jm_share_mem_create();

	jm_share_mem_init(0, 0, SHARE_MEM_SIZE, SHARE_MEMORY_NAME, handle);

	while (true)
	{
		len = fread(data, 1, data_size, ifile);
		if (len > 0) {
			jm_share_mem_write(1, len, data, handle);
		}
		else
		{
			jm_share_mem_set_exit(1, handle);
			break;
		}

		if (jm_share_mem_is_exit(handle)) {
			break;
		}

		if (is_user_exit()) {
			jm_share_mem_set_exit(1, handle);
			break;
		}
#if 0
		Sleep(100);
#endif // DEBUG

		//Sleep(2);
	}

	jm_share_mem_deinit(handle);


	if (data) delete[] data;
	if (ifile) fclose(ifile);

	return 0;
}