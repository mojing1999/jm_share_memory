# jm share memory library


## 1. 介绍
jm_share_memory 项目是封装操作共享内存的一个库。实现两个进程之间使用共享内存通信。


#### 1). 共享内存库 pisoft_share_memory，主要API


```



/*
 *	Create Share memory handle
 */
JMDLL_FUNC handle_mem JMDLL_API jm_share_mem_create();

/*
 *	Init share memory
 */
JMDLL_FUNC int JMDLL_API jm_share_mem_init(int is_read, int wait_ms, int mem_size, const char *name, handle_mem handle);

/*
 *	deinit share memory
 */
JMDLL_FUNC int JMDLL_API jm_share_mem_deinit(handle_mem handle);

/*
 *	read progress read share memory
 */
JMDLL_FUNC int JMDLL_API jm_share_mem_read(int *stream_type, int *len, unsigned char *data, handle_mem handle);

/*
 *	write progress write share memory
 */
JMDLL_FUNC int JMDLL_API jm_share_mem_write(int is_video, int len, unsigned char *data, handle_mem handle);

/*
 *	write progress finish to write data, notify read progress exit share memory
 */
JMDLL_FUNC int JMDLL_API jm_share_mem_set_exit(int is_exit, handle_mem handle);

/*
 *	read progress check share memory status for exit
 */
JMDLL_FUNC int JMDLL_API jm_share_mem_is_exit(handle_mem handle);

```

---	
#### 2). 写进程测试程序 test_share_mem_write
write progress sample code.


---

#### 3). 读进程测试程序 test_share_mem_read

read progress sample code.

---

### TODO List

- [ ] do more test
