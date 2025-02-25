/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) Samsung Electronics Co., Ltd.
 * Gwanghui Lee <gwanghui.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __OLED_FUNCTION_H__
#define __OLED_FUNCTION_H__

#include <linux/types.h>
#include <linux/kernel.h>
#include "../usdm_panel_function.h"

enum oled_function {
	OLED_MAPTBL_INIT_DEFAULT,
	OLED_MAPTBL_GETIDX_DEFAULT,
	OLED_MAPTBL_GETIDX_GM2_BRT,
	OLED_MAPTBL_COPY_DEFAULT,
	OLED_MAPTBL_COPY_TSET,
	OLED_MAPTBL_COPY_DUMMY,
#ifdef CONFIG_USDM_PANEL_COPR
	OLED_MAPTBL_COPY_COPR,
#endif
#if IS_ENABLED(CONFIG_USDM_PANEL_SELF_DISPLAY)
	OLED_MAPTBL_GETIDX_AOD_SELF_MODE_POS,
	OLED_MAPTBL_COPY_AOD_DIGITAL_POS,
	OLED_MAPTBL_COPY_AOD_TIME,
	OLED_MAPTBL_COPY_AOD_TIMER_RATE,
	OLED_MAPTBL_COPY_AOD_TIMER_RATE_HOP,
	OLED_MAPTBL_COPY_AOD_SELF_MOVE_ON_CTRL,
	OLED_MAPTBL_COPY_AOD_ANALOG_POS,
	OLED_MAPTBL_COPY_AOD_ANALOG_EN,
	OLED_MAPTBL_COPY_AOD_DIGITAL_EN,
	OLED_MAPTBL_COPY_AOD_SELF_MOVE_RESET,
	OLED_MAPTBL_COPY_AOD_ICON_CTRL,
	OLED_MAPTBL_COPY_AOD_DIGITAL_COLOR,
	OLED_MAPTBL_COPY_AOD_DIGITAL_UN_WIDTH,
	OLED_MAPTBL_COPY_AOD_PARTIAL_MODE,
	OLED_MAPTBL_COPY_AOD_PARTIAL_AREA,
	OLED_MAPTBL_COPY_AOD_PARTIAL_HLPM,
#ifdef SUPPORT_NORMAL_SELF_MOVE
	OLED_MAPTBL_GETIDX_AOD_SELF_PATTERN,
	OLED_MAPTBL_COPY_AOD_SELF_MOVE_PATTERN,
#endif
#endif /* CONFIG_USDM_PANEL_SELF_DISPLAY */
#if IS_ENABLED(CONFIG_USDM_MDNIE)
	OLED_MAPTBL_GETIDX_MDNIE_SCENARIO_MODE,
#ifdef CONFIG_USDM_PANEL_HMD
	OLED_MAPTBL_GETIDX_MDNIE_HMD,
#endif
	OLED_MAPTBL_GETIDX_MDNIE_HDR,
	OLED_MAPTBL_GETIDX_MDNIE_TRANS_MODE,
	OLED_MAPTBL_GETIDX_MDNIE_NIGHT_MODE,
	OLED_MAPTBL_GETIDX_COLOR_LENS,
	OLED_MAPTBL_COPY_SCR_WHITE,
	OLED_MAPTBL_COPY_SCR_WHITE_ANTI_GLARE,
	OLED_MAPTBL_COPY_SCR_CR,
#ifdef CONFIG_USDM_MDNIE_AFC
	OLED_MAPTBL_COPY_AFC,
#endif
#endif /* CONFIG_USDM_MDNIE */
	OLED_COND_IS_PANEL_DISPLAY_MODE_CHANGED,
	OLED_COND_IS_PANEL_REFRESH_RATE_CHANGED,
	OLED_COND_IS_PANEL_REFRESH_MODE_CHANGED,
	OLED_COND_IS_PANEL_REFRESH_MODE_NS,
	OLED_COND_IS_PANEL_REFRESH_MODE_HS,
	OLED_COND_IS_PANEL_STATE_LPM,
	OLED_COND_IS_PANEL_STATE_NOT_LPM,
	OLED_COND_IS_FIRST_SET_BL,
	OLED_COND_IS_PANEL_MRES_UPDATED,
	OLED_COND_IS_PANEL_MRES_UPDATED_BIGGER,
	OLED_COND_IS_ACL_PWRSAVE_ON,
	OLED_DUMP_SHOW_EXPECTS,
	OLED_DUMP_SHOW_RESOURCE,
	OLED_DUMP_SHOW_RESOURCE_AND_PANIC,
	OLED_DUMP_SHOW_RDDPM,
	OLED_DUMP_SHOW_RDDPM_BEFORE_SLEEP_IN,
	OLED_DUMP_SHOW_RDDSM,
	OLED_DUMP_SHOW_ERR,
	OLED_DUMP_SHOW_ERR_FG,
	OLED_DUMP_SHOW_DSI_ERR,
	OLED_DUMP_SHOW_SELF_DIAG,
#ifdef CONFIG_USDM_DDI_CMDLOG
	OLED_DUMP_SHOW_CMDLOG,
#endif
	OLED_DUMP_SHOW_SSR_ERR,
	OLED_DUMP_SHOW_ECC_ERR,
	OLED_DUMP_SHOW_FLASH_LOADED,
#ifdef CONFIG_USDM_PANEL_MAFPC
	OLED_DUMP_SHOW_MAFPC_LOG,
	OLED_DUMP_SHOW_MAFPC_FLASH_LOG,
	OLED_DUMP_SHOW_ABC_CRC_LOG,
#endif
	OLED_DUMP_SHOW_SELF_MASK_CRC,
	MAX_OLED_FUNCTION,
};

extern struct pnobj_func usdm_oled_function_table[MAX_OLED_FUNCTION];

#define OLED_FUNC(_index) (usdm_oled_function_table[_index])

#define __OLED_MAPTBL_OVERRIDE_INIT_INITIALIZER(_name, _init_func, ...) \
	__SIMPLE_MAPTBL_INITIALIZER(_name, \
			_init_func, \
			&OLED_FUNC(OLED_MAPTBL_GETIDX_DEFAULT), \
			&OLED_FUNC(OLED_MAPTBL_COPY_DEFAULT), \
			##__VA_ARGS__, PANEL_PROPERTY_NUMBER_0)

#define __OLED_MAPTBL_OVERRIDE_COPY_INITIALIZER(_name, _copy_func, ...) \
	__SIMPLE_MAPTBL_INITIALIZER(_name, \
			&OLED_FUNC(OLED_MAPTBL_INIT_DEFAULT), \
			&OLED_FUNC(OLED_MAPTBL_GETIDX_DEFAULT), \
			_copy_func, \
			##__VA_ARGS__, PANEL_PROPERTY_NUMBER_0)

#define __OLED_MAPTBL_COPY_ONLY_INITIALIZER(_name, _copy_func) \
	DEFINE_0D_MAPTBL(_name, \
			&OLED_FUNC(OLED_MAPTBL_INIT_DEFAULT), \
			&OLED_FUNC(OLED_MAPTBL_GETIDX_DEFAULT), \
			_copy_func)

#define __OLED_MAPTBL_DEFAULT_INITIALIZER(_name, ...) \
	__SIMPLE_MAPTBL_INITIALIZER(_name, \
			&OLED_FUNC(OLED_MAPTBL_INIT_DEFAULT), \
			&OLED_FUNC(OLED_MAPTBL_GETIDX_DEFAULT), \
			&OLED_FUNC(OLED_MAPTBL_COPY_DEFAULT), \
			##__VA_ARGS__, PANEL_PROPERTY_NUMBER_0)

#endif /* __OLED_FUNCTION_H__ */
