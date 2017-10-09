/*****************************************************************************
*  Copyright (C) 2014 - 2017, Justin Mo.
*  All rights reserverd.
*
*  FileName:  	intel_dec.h
*  Author:     Justin Mo(mojing1999@gmail.com)
*  Date:       2017-06-10
*  Version:    V0.01
*  Desc:       This file implement share memory read
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
	int ret = 0;

	FILE *ofile = NULL;
	int len = 0;
	int data_size = 1000 * 1024;
	unsigned char *data = new unsigned char[data_size];

	int is_video = 0;

	if (argc < 2) {
		printf("Usage: test_share_mem_read.exe output_file\n");
		return -1;
	}

	char *file_name = argv[1];// "f:\\star_war_2_test_mem.h264";
	ofile = fopen(file_name, "wb");


	handle = jm_share_mem_create();

	ret = jm_share_mem_init(1, 0, SHARE_MEM_SIZE, SHARE_MEMORY_NAME, handle);

	if (ret < 0) {
		if (data) delete[] data;
		if (ofile) fclose(ofile);
		return 0;
	}

	while (true)
	{

		jm_share_mem_read(&is_video, &len, data, handle);
		
		if (len > 0) {
			len = fwrite(data, 1, len, ofile);
		}

		if (jm_share_mem_is_exit(handle)) {
			break;
		}

		if (is_user_exit()) {
			jm_share_mem_set_exit(1, handle);
		}

#if 0
		Sleep(100);
#endif // DEBUG

	}

	jm_share_mem_deinit(handle);


	if (data) delete[] data;
	if (ofile) fclose(ofile);

	return 0;
}