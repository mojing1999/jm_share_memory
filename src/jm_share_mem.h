/*****************************************************************************
*  Copyright (C) 2014 - 2017, Justin Mo.
*  All rights reserverd.
*
*  FileName:  	jm_share_mem.h
*  Author:     Justin Mo(mojing1999@gmail.com)
*  Date:       2017-10-09
*  Version:    V0.01
*  Desc:       This file implement share memory interface
*****************************************************************************/
#ifndef _JM_SHARE_MEMORY_H_
#define _JM_SHARE_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JMDLL_FUNC
#define JMDLL_FUNC		_declspec(dllexport)
#define JMDLL_API		__stdcall
#endif


typedef void* handle_mem;


JMDLL_FUNC handle_mem JMDLL_API jm_share_mem_create();
JMDLL_FUNC int JMDLL_API jm_share_mem_init(int is_read, int wait_ms, int mem_size, const char *name, handle_mem handle);
JMDLL_FUNC int JMDLL_API jm_share_mem_deinit(handle_mem handle);
JMDLL_FUNC int JMDLL_API jm_share_mem_read(int *stream_type, int *len, unsigned char *data, handle_mem handle);
JMDLL_FUNC int JMDLL_API jm_share_mem_write(int is_video, int len, unsigned char *data, handle_mem handle);

JMDLL_FUNC int JMDLL_API jm_share_mem_set_exit(int is_exit, handle_mem handle);
JMDLL_FUNC int JMDLL_API jm_share_mem_is_exit(handle_mem handle);

#ifdef __cplusplus
}
#endif

#endif	// _JM_SHARE_MEMORY_H_