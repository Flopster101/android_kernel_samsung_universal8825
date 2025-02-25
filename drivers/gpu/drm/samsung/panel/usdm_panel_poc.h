/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) Samsung Electronics Co., Ltd.
 * Gwanghui Lee <gwanghui.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __PANEL_POC_H__
#define __PANEL_POC_H__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>

#include "usdm_panel.h"

#define POC_PAGE		(4096)
#define POC_TEST_PATTERN_SIZE	(4096)
#define POC_IMG_PATH "/data/poc_img"
#define POC_DATA_PATH "/data/poc_data"
#define POC_CHKSUM_DATA_LEN		(4)
#define POC_CHKSUM_RES_LEN		(1)
#define POC_CHKSUM_LEN		(POC_CHKSUM_DATA_LEN + POC_CHKSUM_RES_LEN)
#ifdef CONFIG_USDM_PANEL_DPUI
#define POC_TOTAL_TRY_COUNT_FILE_PATH	("/efs/afc/apply_count")
#define POC_TOTAL_FAIL_COUNT_FILE_PATH	("/efs/afc/fail_count")
#define POC_INFO_FILE_PATH	("/efs/FactoryApp/poc_info")
#define POC_USER_FILE_PATH	("/efs/FactoryApp/poc_user")
#endif

#define BIT_RATE_DIV_2	(0)
#define BIT_RATE_DIV_4	(1)
#define BIT_RATE_DIV_32	(4)

#ifdef CONFIG_USDM_POC_SPI
#define POC_SPI_WAIT_WRITE_CNT 100
#define POC_SPI_WAIT_ERASE_CNT 100
#endif

#define PANEL_POC_SPI_BUSY_WAIT

/* poc erase */
#define POC_ERASE_ENTER_SEQ ("poc_erase_enter_seq")
#define POC_ERASE_SEQ ("poc_erase_seq")
#define POC_ERASE_4K_SEQ ("poc_erase_4k_seq")
#define POC_ERASE_32K_SEQ ("poc_erase_32k_seq")
#define POC_ERASE_64K_SEQ ("poc_erase_64k_seq")
#define POC_ERASE_EXIT_SEQ ("poc_erase_exit_seq")

/* poc write */
#define POC_WRITE_ENTER_SEQ ("poc_write_enter_seq")
#define POC_WRITE_STT_SEQ ("poc_write_stt_seq")
#define POC_WRITE_DAT_SEQ ("poc_write_dat_seq")
#define POC_WRITE_DAT_STT_END_SEQ ("poc_write_dat_stt_end_seq")
#define POC_WRITE_END_SEQ ("poc_write_end_seq")
#define POC_WRITE_EXIT_SEQ ("poc_write_exit_seq")

/* poc read */
#define POC_READ_PRE_ENTER_SEQ ("poc_read_pre_enter_seq")
#define POC_READ_ENTER_SEQ ("poc_read_enter_seq")
#define POC_READ_DAT_SEQ ("poc_read_dat_seq")
#define POC_READ_EXIT_SEQ ("poc_read_exit_seq")

/* poc spi */
#ifdef CONFIG_USDM_POC_SPI
#define POC_SPI_INIT_SEQ ("poc_spi_init_seq")
#define POC_SPI_EXIT_SEQ ("poc_spi_exit_seq")
#define POC_SPI_ERASE_64K_SEQ ("poc_spi_erase_64k_seq")
#define POC_SPI_ERASE_32K_SEQ ("poc_spi_erase_32k_seq")
#define POC_SPI_ERASE_4K_SEQ ("poc_spi_erase_4k_seq")
#define POC_SPI_WRITE_SEQ ("poc_spi_write_seq")
#define POC_SPI_READ_SEQ ("poc_spi_read_seq")
#define POC_SPI_STATUS_SEQ ("poc_spi_status_seq")
#define POC_SPI_WAIT_WRITE_SEQ ("poc_spi_wait_write_seq")
#define POC_SPI_WAIT_ERASE_SEQ ("poc_spi_wait_erase_seq")
#endif

enum {
	/* IMG PARTITION */
	POC_IMG_PARTITION,
	/* DIM PARTITION */
	POC_DIM_PARTITION,
	POC_DIM_PARTITION_1,
	POC_DIM_PARTITION_2,
	POC_DIM_PARTITION_END = POC_DIM_PARTITION_2,
	/* MTP PARTITION */
	POC_MTP_PARTITION,
	POC_MTP_PARTITION_1,
	POC_MTP_PARTITION_2,
	POC_MTP_PARTITION_END = POC_MTP_PARTITION_2,
	/* MCD PARTITION */
	POC_MCD_PARTITION,
	POC_GM2_PARTITION,
	POC_GM2_VBIAS_PARTITION = POC_GM2_PARTITION,
	POC_GM2_GAMMA_PARTITION,
	POC_GM2_PARTITION_END = POC_GM2_GAMMA_PARTITION,
	MAX_POC_PARTITION,
};

#define MAX_NR_DIM_PARTITION	((POC_DIM_PARTITION_END) + 1 - (POC_DIM_PARTITION))
#define MAX_NR_MTP_PARTITION	((POC_MTP_PARTITION_END) + 1 - (POC_MTP_PARTITION))
#define MAX_NR_GM2_PARTITION	((POC_GM2_PARTITION_END) + 1 - (POC_GM2_PARTITION))

enum poc_flash_state {
	POC_FLASH_STATE_UNKNOWN = -1,
	POC_FLASH_STATE_NOT_USE = 0,
	POC_FLASH_STATE_USE = 1,
	MAX_POC_FLASH_STATE,
};

enum {
	POC_OP_NONE = 0,
	POC_OP_ERASE = 1,
	POC_OP_WRITE = 2,
	POC_OP_READ = 3,
	POC_OP_CANCEL = 4,
	POC_OP_CHECKSUM = 5,
	POC_OP_CHECKPOC = 6,
	POC_OP_SECTOR_ERASE = 7,
	POC_OP_IMG_READ = 10,
	POC_OP_IMG_WRITE = 11,
	POC_OP_DIM_READ = 12,
	POC_OP_DIM_WRITE = 13,
	POC_OP_DIM_VALID = 14,
	POC_OP_DIM_CHKSUM = 15,
	POC_OP_BACKUP = 17,
	POC_OP_SELF_TEST = 18,
	POC_OP_READ_TEST = 19,
	POC_OP_WRITE_TEST = 20,
	POC_OP_IMG_READ_TEST = 21,
	POC_OP_DIM_READ_TEST = 22,
	POC_OP_DIM_READ_FROM_FILE = 23,
	POC_OP_MTP_READ = 24,
	POC_OP_MCD_READ = 25,
#ifdef CONFIG_USDM_POC_SPI
	POC_OP_SET_CONN_SRC = 26,
	POC_OP_SET_SPI_SPEED = 27,
	POC_OP_READ_SPI_STATUS_REG = 28,
#endif
	POC_OP_INITIALIZE = 29,
	POC_OP_UNINITIALIZE = 30,
	POC_OP_GM2_READ = 31,
	MAX_POC_OP,
};

#define IS_VALID_POC_OP(_op_)	\
	((_op_) > POC_OP_NONE && (_op_) < MAX_POC_OP)

#define IS_POC_OP_ACCESS_FLASH(_op_)	\
	((_op_) == POC_OP_ERASE ||\
	 (_op_) == POC_OP_WRITE ||\
	 (_op_) == POC_OP_READ ||\
	 (_op_) == POC_OP_BACKUP ||\
	 (_op_) == POC_OP_SELF_TEST)

enum poc_state {
	POC_STATE_NONE,
	POC_STATE_FLASH_EMPTY,
	POC_STATE_FLASH_FILLED,
	POC_STATE_ER_START,
	POC_STATE_ER_PROGRESS,
	POC_STATE_ER_COMPLETE,
	POC_STATE_ER_FAILED,
	POC_STATE_WR_START,
	POC_STATE_WR_PROGRESS,
	POC_STATE_WR_COMPLETE,
	POC_STATE_WR_FAILED,
	POC_STATE_RD_START,
	POC_STATE_RD_PROGRESS,
	POC_STATE_RD_COMPLETE,
	POC_STATE_RD_FAILED,
	MAX_POC_STATE,
};
#ifdef CONFIG_USDM_POC_SPI
enum poc_conn_src {
	POC_CONN_SRC_DSI = 0,
	POC_CONN_SRC_SPI = 1,
	MAX_POC_CONN_SRC,
};
#endif
struct panel_poc_info {
	u32 version;
	bool enabled;
	bool erased;

	enum poc_state state;
	u32 total_size;

	u32 waddr;
	u8 *wdata;
	u32 wdata_len;
	u32 raddr;

	u8 poc;
	u8 poc_chksum[POC_CHKSUM_LEN];
	u8 poc_ctrl[PANEL_POC_CTRL_LEN];

	u8 *wbuf;
	u32 wpos;
	u32 wsize;

	u8 *rbuf;
	u32 rpos;
	u32 rsize;

#ifdef CONFIG_USDM_PANEL_DPUI
	int total_failcount;
	int total_trycount;
	int erase_trycount;
	int erase_failcount;
	int write_trycount;
	int write_failcount;
	int read_trycount;
	int read_failcount;
#endif
#ifdef CONFIG_USDM_POC_SPI
	enum poc_conn_src conn_src;
	u32 spi_wdata_len;
	u32 state_mask;
	u32 state_init;
	u32 state_uninit;
	u32 busy_mask;
#endif
};

struct panel_poc_device {
	wait_queue_head_t wqh;
	struct miscdevice dev;
	__u64 count;
	unsigned int flags;
	struct panel_mutex op_lock;
	atomic_t cancel;
	bool opened;
	struct panel_poc_info poc_info;
#ifdef CONFIG_USDM_PANEL_DPUI
	struct notifier_block poc_notif;
#endif
	struct poc_partition *partition;
	u32 nr_partition;
	struct list_head seq_list;
	struct list_head maptbl_list;
};

#ifdef CONFIG_USDM_PANEL_DIM_FLASH
enum dim_flash_items {
	DIM_FLASH_GAMMA,
	DIM_FLASH_AOR,
	DIM_FLASH_VINT,
	DIM_FLASH_ELVSS,
	DIM_FLASH_IRC,
	DIM_FLASH_HMD_GAMMA,
	DIM_FLASH_HMD_AOR,
	MAX_DIM_FLASH,
};

struct dim_flash_info {
	char *name;
	u32 offset;
	u32 nrow;
	u32 ncol;
};
#endif

enum {
	PARTITION_REGION_DATA,
	PARTITION_REGION_CHKSUM,
	PARTITION_REGION_MAGIC,
	MAX_PARTITION_REGION,
};

enum {
	PARTITION_WRITE_CHECK_NONE,
	PARTITION_WRITE_CHECK_NOK,
	PARTITION_WRITE_CHECK_OK,
	MAX_PARTITION_WRITE_CHECK,
};

struct poc_partition_data {
	u32 data_addr;
	u32 data_size;
};

#define MAX_POC_PARTITION_DATA 4
struct poc_partition {
	char *name;
	u32 addr;
	u32 size;

	u32 chksum_by_calc;
	u32 chksum_by_read;
	u32 magicnum;
	u32 magicnum_by_read;

	struct poc_partition_data data[MAX_POC_PARTITION_DATA];

	u32 checksum_addr;
	u32 checksum_size;
	u32 magicnum_addr;
	u32 magicnum_size;

	/* properties */
	int need_preload:1;

	/* result */
	int preload_done:1;
	int chksum_ok:1;
	int write_check;
	bool cache[MAX_PARTITION_REGION];
};

struct panel_poc_data {
	u32 version;
	struct seqinfo *seqtbl;
	u32 nr_seqtbl;
	struct maptbl *maptbl;
	u32 nr_maptbl;
	struct poc_partition *partition;
	u32 nr_partition;
	u32 wdata_len;
#ifdef CONFIG_USDM_POC_SPI
	enum poc_conn_src conn_src;
	u32 spi_wdata_len;
	u32 state_mask;
	u32 state_init;
	u32 state_uninit;
	u32 busy_mask;
#endif
};

#define IOC_GET_POC_STATUS	_IOR('A', 100, __u32)		/* 0:NONE, 1:ERASED, 2:WROTE, 3:READ */
#define IOC_GET_POC_CHKSUM	_IOR('A', 101, __u32)		/* 0:CHKSUM ERROR, 1:CHKSUM SUCCESS */
#define IOC_GET_POC_CSDATA	_IOR('A', 102, __u32)		/* CHKSUM DATA 4 Bytes */
#define IOC_GET_POC_ERASED	_IOR('A', 103, __u32)		/* 0:NONE, 1:NOT ERASED, 2:ERASED */
#define IOC_GET_POC_FLASHED	_IOR('A', 104, __u32)		/* 0:NOT POC FLASHED(0x53), 1:POC FLAHSED(0x52) */

#define IOC_SET_POC_ERASE	_IOR('A', 110, __u32)		/* ERASE POC FLASH */
#define IOC_SET_POC_TEST	_IOR('A', 112, __u32)		/* POC FLASH TEST - ERASE/WRITE/READ/COMPARE */

int panel_poc_probe(struct panel_device *panel, struct panel_poc_data *poc_data);
int panel_poc_remove(struct panel_device *panel);
extern int usdm_set_panel_poc(struct panel_poc_device *poc_dev, u32 cmd, void *arg);
extern int read_poc_partition(struct panel_poc_device *poc_dev, int index);
extern int get_poc_partition_addr(struct panel_poc_device *poc_dev, int index);
extern int get_poc_partition_size(struct panel_poc_device *poc_dev, int index);
extern int copy_poc_partition(struct panel_poc_device *poc_dev, u8 *dst,
		 int index, int offset, int size);
extern int usdm_check_poc_partition_exists(struct panel_poc_device *poc_dev, int index);
extern int usdm_get_poc_partition_chksum(struct panel_poc_device *poc_dev, int index,
		u32 *chksum_ok, u32 *chksum_by_calc, u32 *chksum_by_read);
extern int check_poc_partition_chksum(struct panel_poc_device *poc_dev, int index);
int cmp_poc_partition_data(struct panel_poc_device *poc_dev,
		int partition_index, int data_area_index, u8 *buf, u32 size);

extern void copy_poc_wr_addr_maptbl(struct maptbl *tbl, u8 *dst);
extern void copy_poc_wr_data_maptbl(struct maptbl *tbl, u8 *dst);
extern void copy_poc_rd_addr_maptbl(struct maptbl *tbl, u8 *dst);
extern void copy_poc_er_addr_maptbl(struct maptbl *tbl, u8 *dst);
#endif /* __PANEL_POC_H__ */
