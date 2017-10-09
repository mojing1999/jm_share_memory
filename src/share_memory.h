/*****************************************************************************
*  Copyright (C) 2014 - 2017, Justin Mo.
*  All rights reserverd.
*
*  FileName:  	intel_dec.h
*  Author:     Justin Mo(mojing1999@gmail.com)
*  Date:       2017-06-10
*  Version:    V0.01
*  Desc:       This file implement share memory operation
*****************************************************************************/
#ifndef _SHARE_MEMORY_H_
#define _SHARE_MEMORY_H_
#include <Windows.h>
#include <stdint.h>

#define SHARE_MEM_NAME "JM_STREAM_MEM"
#define MAX_BUFFER_SIZE (5*1024*1024)
#define WRITE_EVENT_NAME "evt_mem_write"
#define READ_EVENT_NAME "evt_mem_read"


#define MEM_VIDEO_INFO_OFFSET (0)	//
#define MEM_AUDIO_INFO_OFFSET (100)	//
#define MEM_HEAD_INFO_OFFSET (200)	// | 
#define MEM_HEAD_INFO_EXIT_OFFSET	MEM_HEAD_INFO_OFFSET
#define MEM_HEAD_INFO_TYPE_OFFSET	(MEM_HEAD_INFO_EXIT_OFFSET + sizeof(int))
#define MEM_HEAD_INFO_LEN_OFFSET	(MEM_HEAD_INFO_TYPE_OFFSET + sizeof(int))
#define	MEM_HEAD_INFO_INDEX_OFFSEST	(MEM_HEAD_INFO_LEN_OFFSET + sizeof(int))

#define MEM_PAYLOAD_OFFSET (300)	//

#define MAX_WAITING_TIME (10000)


#ifdef _DEBUG
#define LOG( ... )				printf( __VA_ARGS__ )
#else
#define LOG( ... )              
#endif


typedef struct _share_mem_ctx
{
	HANDLE handle_mem;
	LPVOID share_mem;
	HANDLE evt_write;
	HANDLE evt_read;

	uint32_t index;
	int		is_read;

	int		wait_ms;
	int		mem_size;


}share_mem_ctx;

share_mem_ctx *sharem_mem_create();
int share_memory_init(int is_read, int wait_ms, int mem_size, const char *share_mem_name, share_mem_ctx *ctx);
int share_memory_deinit(share_mem_ctx *ctx);

int share_memory_write(int stream_type, int len, uint8_t *data, share_mem_ctx *ctx);
int share_memory_read(int *stream_type, int *len, uint8_t *data, share_mem_ctx *ctx);

int share_memory_set_exit_code(int is_exit, share_mem_ctx *ctx);
int share_memory_get_exit_code(share_mem_ctx *ctx);

#endif	// _SHARE_MEMORY_H_
