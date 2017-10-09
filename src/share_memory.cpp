/*****************************************************************************
*  Copyright (C) 2014 - 2017, Justin Mo.
*  All rights reserverd.
*
*  FileName:  	intel_dec.h
*  Author:     Justin Mo(mojing1999@gmail.com)
*  Date:       2017-06-10
*  Version:    V0.01
*  Desc:       This file implement Intel Media SDK Decode
*****************************************************************************/
#include "share_memory.h"
#include "jm_share_mem.h"
#include <stdio.h>

static int write_int_to_buf(int n, uint8_t *buf)
{
	buf[0] = (n >> 24) & 0xFF;
	buf[1] = (n >> 16) & 0xFF;
	buf[2] = (n >> 8) & 0xFF;
	buf[3] = (n >> 0) & 0xFF;

	return n;
}

static int read_int_from_buf(uint8_t *buf)
{
	int ret = 0;

	ret = ((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);

	return ret;
}

share_mem_ctx *sharem_mem_create()
{
	share_mem_ctx *ctx = new share_mem_ctx;

	memset(ctx, 0x0, sizeof(share_mem_ctx));

	return ctx;
}


int share_memory_init(int is_read, int wait_ms, int mem_size, const char *share_mem_name, share_mem_ctx *ctx)
{
	int mem_create_mode = 0;
	// 0 - write , 1 - read
	ctx->is_read = is_read;
	ctx->index = 0xFFFFFFFF;
	if (wait_ms <= 0) {
		ctx->wait_ms = INFINITE;
	}
	//else if (wait_ms < 100) {
	//	ctx->wait_ms = wait_ms * 1000;
	//}
	else {
		ctx->wait_ms = wait_ms;
	}

	if (mem_size < MAX_BUFFER_SIZE) {
		ctx->mem_size = MAX_BUFFER_SIZE;
	}
	else {
		ctx->mem_size = mem_size;
	}

	// 
	ctx->handle_mem = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, share_mem_name);
	if (NULL == ctx->handle_mem) {
		// Create Share memory
		mem_create_mode = TRUE;
		ctx->handle_mem = CreateFileMappingA(INVALID_HANDLE_VALUE,
			NULL, PAGE_READWRITE, 0, ctx->mem_size, share_mem_name);

		if (NULL == ctx->handle_mem) {
			goto SHARE_MEM_ERROR;
		}
	}
	
	char readEventName[MAX_PATH] = { 0 };
	char writeEventName[MAX_PATH] = { 0 };
	sprintf(readEventName, "%s_%s", share_mem_name, READ_EVENT_NAME);
	sprintf(writeEventName, "%s_%s", share_mem_name, WRITE_EVENT_NAME);
	if (mem_create_mode) {
		// write
		// Create event
		ctx->evt_read = CreateEventA(NULL, FALSE, TRUE, readEventName);
		ctx->evt_write = CreateEventA(NULL, FALSE, FALSE, writeEventName);
		if (NULL == ctx->evt_read || NULL == ctx->evt_write)
		{
			// Error
			goto SHARE_MEM_ERROR;
		}

		ctx->share_mem = MapViewOfFile(ctx->handle_mem, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (NULL == ctx->share_mem) {
			goto SHARE_MEM_ERROR;
		}

		memset(ctx->share_mem, 0x0, ctx->mem_size);

		return 0;

	}
	else {
		// read
		ctx->evt_read = OpenEventA(EVENT_ALL_ACCESS, FALSE, readEventName);
		ctx->evt_write = OpenEventA(EVENT_ALL_ACCESS, FALSE, writeEventName);
		if (NULL == ctx->evt_read || NULL == ctx->evt_write)
		{
			// Error
			goto SHARE_MEM_ERROR;
		}
		
		ctx->share_mem = MapViewOfFile(ctx->handle_mem, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		if (NULL == ctx->share_mem) {
			goto SHARE_MEM_ERROR;
		}

		return 0;
	}

SHARE_MEM_ERROR:
	//release share memory 
	if (NULL != ctx->evt_read)		CloseHandle(ctx->evt_read);
	if (NULL != ctx->evt_write)		CloseHandle(ctx->evt_write);
	if (NULL != ctx->share_mem)	 		UnmapViewOfFile(ctx->share_mem);
	if (NULL != ctx->handle_mem)		CloseHandle(ctx->handle_mem);

	return -1;

}

int share_memory_deinit(share_mem_ctx *ctx)
{
	int ret = 0;

	share_memory_set_exit_code(1, ctx);

	if (0 == ctx->is_read) {
		// write process, waiting for read process exit, than close share memory
		ret = WaitForSingleObject(ctx->evt_read, 1000);	

	}

	SetEvent(ctx->evt_write);
	SetEvent(ctx->evt_read);

	if (NULL != ctx->evt_write) {
		SetEvent(ctx->evt_write);
		CloseHandle(ctx->evt_write);
	}

	if (NULL != ctx->evt_read) {
		SetEvent(ctx->evt_read);
		CloseHandle(ctx->evt_read);
	}

	if (NULL != ctx->share_mem)	 		UnmapViewOfFile(ctx->share_mem);
	if (NULL != ctx->handle_mem)		CloseHandle(ctx->handle_mem);

	return 0;
}


int share_memory_write(int stream_type, int len, uint8_t *data, share_mem_ctx *ctx)
{
	DWORD ret = 0;
	uint8_t *mem = NULL;
	uint8_t *data_mem = NULL;

	ret = WaitForSingleObject(ctx->evt_read, ctx->wait_ms);

	mem = (uint8_t *)ctx->share_mem;

	data_mem = mem + MEM_PAYLOAD_OFFSET;

	// write head info
	write_int_to_buf(stream_type, mem+ MEM_HEAD_INFO_TYPE_OFFSET);
	write_int_to_buf(len, mem + MEM_HEAD_INFO_LEN_OFFSET);
	ctx->index += 1;
	write_int_to_buf(ctx->index, mem + MEM_HEAD_INFO_INDEX_OFFSEST);	// index
	LOG("write data: index = %d, len = %d\n", ctx->index, len);


	memcpy(data_mem, data, len);

	SetEvent(ctx->evt_write);

	return 0;
}

int share_memory_read(int *stream_type, int *len, uint8_t *data, share_mem_ctx *ctx)
{
	DWORD ret = 0;
	uint8_t *mem = NULL;
	uint8_t *head_mem = NULL;
	uint8_t *data_mem = NULL;

	int type = 0, data_len = 0;
	uint32_t index = 0;

	int is_exit = 0;
	is_exit = share_memory_get_exit_code(ctx);

	*len = 0;

	if (1 == is_exit) {
		return 0;
	}

	ret = WaitForSingleObject(ctx->evt_write, ctx->wait_ms);//MAX_WAITING_TIME
	if (WAIT_TIMEOUT == ret) {

		SetEvent(ctx->evt_read);

		return -1;
	}
	mem = (uint8_t *)ctx->share_mem;

	head_mem = mem + MEM_HEAD_INFO_OFFSET;
	data_mem = mem + MEM_PAYLOAD_OFFSET;

	// read head info from buf
	type = read_int_from_buf(mem + MEM_HEAD_INFO_TYPE_OFFSET);
	data_len = read_int_from_buf(mem + MEM_HEAD_INFO_LEN_OFFSET);
	index = read_int_from_buf(mem + MEM_HEAD_INFO_INDEX_OFFSEST);
	LOG("read share memory: cur index = %d, pre index = %d, len = %d\n", index, ctx->index, data_len);
	if (index == ctx->index) {
		// same data
		SetEvent(ctx->evt_read);
		return 0;
	}

	ctx->index = index;

	*stream_type = type;
	*len = data_len;

	memcpy(data, data_mem, data_len);


	SetEvent(ctx->evt_read);

	return 0;
}


int share_memory_set_exit_code(int is_exit, share_mem_ctx *ctx)
{
	uint8_t *mem = (uint8_t *)ctx->share_mem;

	write_int_to_buf(is_exit, mem + MEM_HEAD_INFO_EXIT_OFFSET);

	return 0;
}

int share_memory_get_exit_code(share_mem_ctx *ctx)
{
	int exit_code = 0;

	uint8_t *mem = (uint8_t *)ctx->share_mem;
	exit_code = read_int_from_buf(mem + MEM_HEAD_INFO_EXIT_OFFSET);

	return exit_code;
}

/**********************************************************************************
*		export function
**********************************************************************************/




JMDLL_FUNC handle_mem JMDLL_API jm_share_mem_create()
{
	return sharem_mem_create();
}

JMDLL_FUNC int JMDLL_API jm_share_mem_init(int is_read, int wait_ms, int mem_size, const char *name, handle_mem handle)
{
	return share_memory_init(is_read, wait_ms, mem_size, name, (share_mem_ctx *)handle);
}

JMDLL_FUNC int JMDLL_API jm_share_mem_deinit(handle_mem handle)
{
	return share_memory_deinit((share_mem_ctx*)handle);
}

JMDLL_FUNC int JMDLL_API jm_share_mem_read(int *stream_type, int *len, unsigned char *data, handle_mem handle)
{
	return share_memory_read(stream_type, len, data, (share_mem_ctx *)handle);
}

JMDLL_FUNC int JMDLL_API jm_share_mem_write(int is_video, int len, unsigned char *data, handle_mem handle)
{
	return share_memory_write(is_video, len, data, (share_mem_ctx *)handle);
}

JMDLL_FUNC int JMDLL_API jm_share_mem_set_exit(int is_exit, handle_mem handle)
{
	return share_memory_set_exit_code(is_exit, (share_mem_ctx *)handle);
}

JMDLL_FUNC int JMDLL_API jm_share_mem_is_exit(handle_mem handle)
{
	return share_memory_get_exit_code((share_mem_ctx *)handle);
}
