/*
 * linux/drivers/video/fbdev/exynos/panel/s6e3fc3/s6e3fc3.c
 *
 * S6E3FC3 Dimming Driver
 *
 * Copyright (c) 2016 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/of_gpio.h>
#include <video/mipi_display.h>
#include "../decon_panel_kunit.h"
#include "../decon_panel.h"
#include "s6e3fc3.h"
#ifdef CONFIG_PANEL_AID_DIMMING
#include "../decon_dimming.h"
#include "../decon_panel_dimming.h"
#endif
#include "../decon_panel_drv.h"
#include "../decon_panel_debug.h"

#if IS_ENABLED(CONFIG_SEC_ABC)
#include <linux/sti/abc_common.h>
#endif

#if IS_ENABLED(CONFIG_DISPLAY_USE_INFO)
#include "../decon_dpui.h"
#endif

#ifdef CONFIG_PANEL_AID_DIMMING

int generate_brt_step_table(struct brightness_table *brt_tbl)
{
	int ret = 0;
	int i = 0, j = 0, k = 0;

	if (unlikely(!brt_tbl || !brt_tbl->brt)) {
		panel_err("invalid parameter\n");
		return -EINVAL;
	}
	if (unlikely(!brt_tbl->step_cnt)) {
		if (likely(brt_tbl->brt_to_step)) {
			panel_info("we use static step table\n");
			return ret;
		} else {
			panel_err("invalid parameter, all table is NULL\n");
			return -EINVAL;
		}
	}

	brt_tbl->sz_brt_to_step = 0;
	for(i = 0; i < brt_tbl->sz_step_cnt; i++)
		brt_tbl->sz_brt_to_step += brt_tbl->step_cnt[i];

	brt_tbl->brt_to_step =
		(u32 *)kmalloc(brt_tbl->sz_brt_to_step * sizeof(u32), GFP_KERNEL);

	if (unlikely(!brt_tbl->brt_to_step)) {
		panel_err("alloc fail\n");
		return -EINVAL;
	}
	brt_tbl->brt_to_step[0] = brt_tbl->brt[0];
	i = 1;
	while (i < brt_tbl->sz_brt_to_step) {
		for (k = 1; k < brt_tbl->sz_brt; k++) {
			for (j = 1; j <= brt_tbl->step_cnt[k]; j++, i++) {
				brt_tbl->brt_to_step[i] = decon_disp_interpolation64(brt_tbl->brt[k - 1] * decon_disp_pow(10, 2),
					brt_tbl->brt[k] * decon_disp_pow(10, 2), j, brt_tbl->step_cnt[k]);
				brt_tbl->brt_to_step[i] = decon_disp_pow_round(brt_tbl->brt_to_step[i], 2);
				brt_tbl->brt_to_step[i] = decon_disp_div64(brt_tbl->brt_to_step[i], decon_disp_pow(10, 2));
				if (brt_tbl->brt[brt_tbl->sz_brt - 1] < brt_tbl->brt_to_step[i]) {

					brt_tbl->brt_to_step[i] = decon_disp_pow_round(brt_tbl->brt_to_step[i], 2);
				}
				if (i >= brt_tbl->sz_brt_to_step) {
					panel_err("step cnt over %d %d\n", i, brt_tbl->sz_brt_to_step);
					break;
				}
			}
		}
	}
	return ret;
}

#endif /* CONFIG_PANEL_AID_DIMMING */

int init_common_table(struct maptbl *tbl)
{
	if (tbl == NULL) {
		panel_err("maptbl is null\n");
		return -EINVAL;
	}

	if (tbl->pdata == NULL) {
		panel_err("pdata is null\n");
		return -EINVAL;
	}

	return 0;
}

#ifdef CONFIG_EXYNOS_DECON_MDNIE_LITE
int getidx_common_maptbl(struct maptbl *tbl)
{
	return 0;
}
#endif

#ifdef CONFIG_MCD_PANEL_LPM
#ifdef CONFIG_SUPPORT_AOD_BL
int init_aod_dimming_table(struct maptbl *tbl)
{
	int id = PANEL_BL_SUBDEV_TYPE_AOD;
	struct panel_device *panel;
	struct panel_bl_device *panel_bl;

	if (unlikely(!tbl || !tbl->pdata)) {
		panel_err("panel_bl-%d invalid param (tbl %pK, pdata %pK)\n",
				id, tbl, tbl ? tbl->pdata : NULL);
		return -EINVAL;
	}

	panel = tbl->pdata;
	panel_bl = &panel->panel_bl;

	if (unlikely(!panel->panel_data.panel_dim_info[id])) {
		panel_err("panel_bl-%d panel_dim_info is null\n", id);
		return -EINVAL;
	}

	memcpy(&panel_bl->subdev[id].brt_tbl,
			panel->panel_data.panel_dim_info[id]->brt_tbl,
			sizeof(struct brightness_table));

	return 0;
}
#endif
#endif

void copy_tset_maptbl(struct maptbl *tbl, u8 *dst)
{
	struct panel_device *panel;
	struct panel_info *panel_data;

	if (!tbl || !dst)
		return;

	panel = (struct panel_device *)tbl->pdata;
	if (unlikely(!panel))
		return;

	panel_data = &panel->panel_data;

	*dst = (panel_data->props.temperature < 0) ?
		BIT(7) | abs(panel_data->props.temperature) :
		panel_data->props.temperature;
}

#if 0
void copy_grayspot_cal_maptbl(struct maptbl *tbl, u8 *dst)
{
	struct panel_device *panel;
	struct panel_info *panel_data;
	u8 val;
	int ret = 0;

	if (!tbl || !dst)
		return;

	panel = (struct panel_device *)tbl->pdata;
	if (unlikely(!panel))
		return;

	panel_data = &panel->panel_data;

	ret = resource_copy_by_name(panel_data, &val, "grayspot_cal");
	if (unlikely(ret)) {
		panel_err("grayspot_cal not found in panel resource\n");
		return;
	}

	panel_info("grayspot_cal 0x%02x\n", val);
	*dst = val;
}
#endif

#ifdef CONFIG_SUPPORT_XTALK_MODE
int getidx_vgh_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	struct panel_info *panel_data;
	int row = 0;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	panel_data = &panel->panel_data;

	row = ((panel_data->props.xtalk_mode) ? 1 : 0);
	panel_info("xtalk_mode %d\n", row);

	return decon_maptbl_index(tbl, 0, row, 0);
}
#endif

int getidx_hbm_transition_table(struct maptbl *tbl)
{
	int layer, row;
	struct panel_info *panel_data;
	struct panel_bl_device *panel_bl;
	struct panel_device *panel = (struct panel_device *)tbl->pdata;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	panel_data = &panel->panel_data;
	panel_bl = &panel->panel_bl;

	layer = decon_is_hbm_brightness(panel_bl, panel_bl->props.brightness);
	row = panel_bl->props.smooth_transition;

	return decon_maptbl_index(tbl, layer, row, 0);
}

int getidx_smooth_transition_table(struct maptbl *tbl)
{
	int row;
	struct panel_info *panel_data;
	struct panel_bl_device *panel_bl;
	struct panel_device *panel = (struct panel_device *)tbl->pdata;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	panel_data = &panel->panel_data;
	panel_bl = &panel->panel_bl;

	row = panel_bl->props.smooth_transition;

	return decon_maptbl_index(tbl, 0, row, 0);
}

int getidx_acl_onoff_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	return decon_maptbl_index(tbl, 0, decon_panel_bl_get_acl_pwrsave(&panel->panel_bl), 0);
}

int getidx_hbm_onoff_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	struct panel_bl_device *panel_bl;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	panel_bl = &panel->panel_bl;

	return decon_maptbl_index(tbl, 0,
			decon_is_hbm_brightness(panel_bl, panel_bl->props.brightness), 0);
}

int getidx_acl_dim_onoff_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	struct panel_bl_device *panel_bl;
	int row = S6E3FC3_ACL_DIM_ON;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}
	panel_bl = &panel->panel_bl;
#ifdef CONFIG_SUPPORT_MASK_LAYER
	row = panel_bl->props.mask_layer_br_hook == MASK_LAYER_HOOK_ON ? S6E3FC3_ACL_DIM_OFF : S6E3FC3_ACL_DIM_ON;
#endif
	return decon_maptbl_index(tbl, 0, row, 0);
}

int getidx_acl_opr_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	int row;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	if (decon_panel_bl_get_acl_pwrsave(&panel->panel_bl) == ACL_PWRSAVE_OFF)
		row = S6E3FC3_ACL_OPR_0;
	else
		row = decon_panel_bl_get_acl_opr(&panel->panel_bl);

	if (row >= MAX_S6E3FC3_ACL_OPR) {
		panel_warn("invalid range %d\n", row);
		row = S6E3FC3_ACL_OPR_1;
	}

	return decon_maptbl_index(tbl, 0, row, 0);
}

int init_lpm_brt_table(struct maptbl *tbl)
{
#ifdef CONFIG_SUPPORT_AOD_BL
	return init_aod_dimming_table(tbl);
#else
	return init_common_table(tbl);
#endif
}

int getidx_lpm_brt_table(struct maptbl *tbl)
{
	int row = 0;
	struct panel_device *panel;
	struct panel_bl_device *panel_bl;
	struct panel_properties *props;

	panel = (struct panel_device *)tbl->pdata;
	panel_bl = &panel->panel_bl;
	props = &panel->panel_data.props;

#ifdef CONFIG_MCD_PANEL_LPM
#ifdef CONFIG_SUPPORT_AOD_BL
	panel_bl = &panel->panel_bl;
	row = decon_get_subdev_actual_brightness_index(panel_bl, PANEL_BL_SUBDEV_TYPE_AOD,
			panel_bl->subdev[PANEL_BL_SUBDEV_TYPE_AOD].brightness);

	props->lpm_brightness = panel_bl->subdev[PANEL_BL_SUBDEV_TYPE_AOD].brightness;
	panel_info("alpm_mode %d, brightness %d, row %d\n", props->cur_alpm_mode,
		panel_bl->subdev[PANEL_BL_SUBDEV_TYPE_AOD].brightness, row);

#else
	switch (props->alpm_mode) {
	case ALPM_LOW_BR:
	case HLPM_LOW_BR:
		row = 0;
		break;
	case ALPM_HIGH_BR:
	case HLPM_HIGH_BR:
		row = maptbl_get_countof_row(tbl) - 1;
		break;
	default:
		panel_err("Invalid alpm mode : %d\n", props->alpm_mode);
		break;
	}

	panel_info("alpm_mode %d, row %d\n", props->alpm_mode, row);
#endif
#endif

	return decon_maptbl_index(tbl, 0, row, 0);
}

int getidx_s6e3fc3_vrr_fps(int vrr_fps)
{
	int fps_index = S6E3FC3_VRR_FPS_60;

	switch (vrr_fps) {
	case 120:
		fps_index = S6E3FC3_VRR_FPS_120;
		break;
	case 90:
		fps_index = S6E3FC3_VRR_FPS_90;
		break;
	case 60:
		fps_index = S6E3FC3_VRR_FPS_60;
		break;
	default:
		fps_index = S6E3FC3_VRR_FPS_60;
		panel_err("undefined FPS:%d\n", vrr_fps);
	}

	return fps_index;
}

int getidx_s6e3fc3_current_vrr_fps(struct panel_device *panel)
{
	int vrr_fps;

	vrr_fps = decon_get_panel_refresh_rate(panel);
	if (vrr_fps < 0)
		return -EINVAL;

	return getidx_s6e3fc3_vrr_fps(vrr_fps);
}

int getidx_vrr_fps_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	int row = 0, layer = 0, index;

	index = getidx_s6e3fc3_current_vrr_fps(panel);
	if (index < 0)
		row = 0;
	else
		row = index;

	return decon_maptbl_index(tbl, layer, row, 0);
}

#if defined(__PANEL_NOT_USED_VARIABLE__)
int getidx_vrr_gamma_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	int row = 0, layer = 0, index;
	struct panel_bl_device *panel_bl;

	panel_bl = &panel->panel_bl;

	index = getidx_s6e3fc3_current_vrr_fps(panel);
	if (index < 0)
		layer = 0;
	else
		layer = index;

	switch (panel_bl->bd->props.brightness) {
	case S6E3FC3_R8S_VRR_GAMMA_INDEX_0_BR ... S6E3FC3_R8S_VRR_GAMMA_INDEX_1_BR - 1:
		row = S6E3FC3_GAMMA_BR_INDEX_0;
		break;
	case S6E3FC3_R8S_VRR_GAMMA_INDEX_1_BR ... S6E3FC3_R8S_VRR_GAMMA_INDEX_2_BR - 1:
		row = S6E3FC3_GAMMA_BR_INDEX_1;
		break;
	case S6E3FC3_R8S_VRR_GAMMA_INDEX_2_BR ... S6E3FC3_R8S_VRR_GAMMA_INDEX_3_BR - 1:
		row = S6E3FC3_GAMMA_BR_INDEX_2;
		break;
	case S6E3FC3_R8S_VRR_GAMMA_INDEX_3_BR ... S6E3FC3_R8S_VRR_GAMMA_INDEX_MAX_BR:
		row = S6E3FC3_GAMMA_BR_INDEX_3;
		break;
	}

	return decon_maptbl_index(tbl, layer, row, 0);
}
#endif

#ifdef CONFIG_EXYNOS_DECON_MDNIE_LITE
void copy_dummy_maptbl(struct maptbl *tbl, u8 *dst)
{
	return;
}
#endif

void copy_common_maptbl(struct maptbl *tbl, u8 *dst)
{
	int idx;

	if (!tbl || !dst) {
		panel_err("invalid parameter (tbl %pK, dst %pK)\n",
				tbl, dst);
		return;
	}

	idx = decon_maptbl_getidx(tbl);
	if (idx < 0) {
		panel_err("failed to getidx %d\n", idx);
		return;
	}

	memcpy(dst, &(tbl->arr)[idx], sizeof(u8) * maptbl_get_sizeof_copy(tbl));
	panel_dbg("copy from %s %d %d\n",
			tbl->name, idx, maptbl_get_sizeof_copy(tbl));
	print_data(dst, maptbl_get_sizeof_copy(tbl));
}

#if defined(CONFIG_MCD_PANEL_FACTORY) && defined(CONFIG_SUPPORT_FAST_DISCHARGE)
int getidx_fast_discharge_table(struct maptbl *tbl)
{
	struct panel_device *panel = (struct panel_device *)tbl->pdata;
	struct panel_info *panel_data;
	int row = 0;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}

	panel_data = &panel->panel_data;

	row = ((panel_data->props.enable_fd) ? 1 : 0);
	panel_info("fast_discharge %d\n", row);

	return decon_maptbl_index(tbl, 0, row, 0);
}
#endif

#ifdef CONFIG_EXYNOS_DECON_MDNIE_LITE
int init_color_blind_table(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if (tbl->arr == NULL) {
		panel_err("arr is null\n");
		return -EINVAL;
	}

	if (S6E3FC3_SCR_CR_OFS + mdnie->props.sz_scr > maptbl_get_sizeof_maptbl(tbl)) {
		panel_err("invalid size (maptbl_size %d, sz_scr %d)\n",
				maptbl_get_sizeof_maptbl(tbl), mdnie->props.sz_scr);
		return -EINVAL;
	}

	/* TODO: boundary check in mdnie driver */
	if (mdnie->props.sz_scr > ARRAY_SIZE(mdnie->props.scr)) {
		panel_err("invalid sz_scr %d\n", mdnie->props.sz_scr);
		return -EINVAL;
	}

	memcpy(&tbl->arr[S6E3FC3_SCR_CR_OFS],
			mdnie->props.scr, mdnie->props.sz_scr);

	return 0;
}

int getidx_mdnie_scenario_maptbl(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	return decon_maptbl_get_indexof_row(tbl, mdnie->props.mode);
}

int getidx_mdnie_hdr_maptbl(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	return decon_maptbl_get_indexof_row(tbl, mdnie->props.hdr);
}

int getidx_mdnie_night_mode_maptbl(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	int mode = NIGHT_MODE_OFF;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if (mdnie->props.mode != AUTO)
		mode = NIGHT_MODE_ON;

	return decon_maptbl_index(tbl, mode, mdnie->props.night_level, 0);
}

int init_mdnie_night_mode_table(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	struct maptbl *night_maptbl;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	night_maptbl = mdnie_find_etc_maptbl(mdnie, MDNIE_ETC_NIGHT_MAPTBL);
	if (!night_maptbl) {
		panel_err("NIGHT_MAPTBL not found\n");
		return -EINVAL;
	}

	if (maptbl_get_sizeof_maptbl(tbl) < (S6E3FC3_NIGHT_MODE_OFS +
			maptbl_get_sizeof_row(night_maptbl))) {
		panel_err("invalid size (maptbl_size %d, night_maptbl_size %d)\n",
				maptbl_get_sizeof_maptbl(tbl), maptbl_get_sizeof_row(night_maptbl));
		return -EINVAL;
	}

	decon_maptbl_copy(night_maptbl, &tbl->arr[S6E3FC3_NIGHT_MODE_OFS]);

	return 0;
}

int init_mdnie_color_lens_table(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	struct maptbl *color_lens_maptbl;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	color_lens_maptbl = mdnie_find_etc_maptbl(mdnie, MDNIE_ETC_COLOR_LENS_MAPTBL);
	if (!color_lens_maptbl) {
		panel_err("COLOR_LENS_MAPTBL not found\n");
		return -EINVAL;
	}

	if (maptbl_get_sizeof_maptbl(tbl) < (S6E3FC3_COLOR_LENS_OFS +
			maptbl_get_sizeof_row(color_lens_maptbl))) {
		panel_err("invalid size (maptbl_size %d, color_lens_maptbl_size %d)\n",
				maptbl_get_sizeof_maptbl(tbl), maptbl_get_sizeof_row(color_lens_maptbl));
		return -EINVAL;
	}

	if (IS_COLOR_LENS_MODE(mdnie))
		decon_maptbl_copy(color_lens_maptbl, &tbl->arr[S6E3FC3_COLOR_LENS_OFS]);

	return 0;
}

void update_current_scr_white(struct maptbl *tbl, u8 *dst)
{
	struct mdnie_info *mdnie;

	if (!tbl || !tbl->pdata) {
		panel_err("invalid param\n");
		return;
	}

	if (!dst) {
		panel_err("dst is null\n");
		return;
	}

	mdnie = (struct mdnie_info *)tbl->pdata;
	mdnie->props.cur_wrgb[0] = *dst;
	mdnie->props.cur_wrgb[1] = *(dst + 1);
	mdnie->props.cur_wrgb[2] = *(dst + 2);
}

int init_color_coordinate_table(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	struct maptbl_pos pos;
	int type, color, index;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if (maptbl_get_countof_row(tbl) != ARRAY_SIZE(mdnie->props.coord_wrgb)) {
		panel_err("invalid maptbl row size %d\n", maptbl_get_countof_row(tbl));
		return -EINVAL;
	}

	if (maptbl_get_countof_col(tbl) != ARRAY_SIZE(mdnie->props.coord_wrgb[0])) {
		panel_err("invalid maptbl col size %d\n", maptbl_get_countof_col(tbl));
		return -EINVAL;
	}

	maptbl_for_each(tbl, index) {
		if (decon_decon_maptbl_index_to_pos(tbl, index, &pos) < 0)
			return -EINVAL;

		type = pos.index[NDARR_2D];
		color = pos.index[NDARR_1D];
		tbl->arr[index] =
			mdnie->props.coord_wrgb[type][color];
	}

	return 0;
}

int init_sensor_rgb_table(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	struct maptbl_pos pos;
	int index, indexof_column;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if (maptbl_get_countof_row(tbl) != 1) {
		panel_err("invalid maptbl row size %d\n", maptbl_get_countof_row(tbl));
		return -EINVAL;
	}

	if (maptbl_get_countof_col(tbl) != ARRAY_SIZE(mdnie->props.ssr_wrgb)) {
		panel_err("invalid maptbl col size %d\n", maptbl_get_countof_col(tbl));
		return -EINVAL;
	}

	maptbl_for_each(tbl, index) {
		if (decon_decon_maptbl_index_to_pos(tbl, index, &pos) < 0)
			return -EINVAL;

		indexof_column = pos.index[NDARR_1D];
		tbl->arr[index] =
			mdnie->props.ssr_wrgb[indexof_column];
	}

	return 0;
}

int getidx_color_coordinate_maptbl(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	static int wcrd_type[MODE_MAX] = {
		WCRD_TYPE_D65, WCRD_TYPE_D65, WCRD_TYPE_D65,
		WCRD_TYPE_ADAPTIVE, WCRD_TYPE_ADAPTIVE,
	};

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if ((mdnie->props.mode < 0) || (mdnie->props.mode >= MODE_MAX)) {
		panel_err("out of mode range %d\n", mdnie->props.mode);
		return -EINVAL;
	}
	return decon_maptbl_index(tbl, 0, wcrd_type[mdnie->props.mode], 0);
}

int getidx_adjust_ldu_maptbl(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;
	static int wcrd_type[MODE_MAX] = {
		WCRD_TYPE_D65, WCRD_TYPE_D65, WCRD_TYPE_D65,
		WCRD_TYPE_ADAPTIVE, WCRD_TYPE_ADAPTIVE,
	};

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if ((mdnie->props.mode < 0) || (mdnie->props.mode >= MODE_MAX)) {
		panel_err("out of mode range %d\n", mdnie->props.mode);
		return -EINVAL;
	}

	if ((mdnie->props.ldu < LDU_MODE_OFF) || (mdnie->props.ldu >= MAX_LDU_MODE)) {
		panel_err("out of ldu mode range %d\n", mdnie->props.ldu);
		return -EINVAL;
	}

	return decon_maptbl_index(tbl, wcrd_type[mdnie->props.mode], mdnie->props.ldu, 0);
}

int getidx_color_lens_maptbl(struct maptbl *tbl)
{
	struct mdnie_info *mdnie;

	if (!tbl || !tbl->pdata)
		return -EINVAL;

	mdnie = tbl->pdata;

	if (!IS_COLOR_LENS_MODE(mdnie))
		return -EINVAL;

	if ((mdnie->props.color_lens_color < 0) || (mdnie->props.color_lens_color >= COLOR_LENS_COLOR_MAX)) {
		panel_err("out of color lens color range %d\n", mdnie->props.color_lens_color);
		return -EINVAL;
	}

	if ((mdnie->props.color_lens_level < 0) || (mdnie->props.color_lens_level >= COLOR_LENS_LEVEL_MAX)) {
		panel_err("out of color lens level range %d\n", mdnie->props.color_lens_level);
		return -EINVAL;
	}

	return decon_maptbl_index(tbl, mdnie->props.color_lens_color, mdnie->props.color_lens_level, 0);
}

void copy_scr_white_maptbl(struct maptbl *tbl, u8 *dst)
{
	struct mdnie_info *mdnie;
	int idx;

	if (!tbl || !tbl->pdata || !dst)
		return;

	mdnie = tbl->pdata;
	if (maptbl_get_countof_col(tbl) != MAX_COLOR) {
		panel_err("invalid maptbl size %d\n", maptbl_get_countof_col(tbl));
		return;
	}

	idx = decon_maptbl_getidx(tbl);
	if (idx < 0 || (idx + MAX_COLOR > maptbl_get_sizeof_maptbl(tbl))) {
		panel_err("invalid index %d\n", idx);
		return;
	}

	decon_mdnie_update_wrgb(mdnie, tbl->arr[idx + RED],
			tbl->arr[idx + GREEN], tbl->arr[idx + BLUE]);
	decon_mdnie_cur_wrgb_to_byte_array(mdnie, dst, 1);
}

int getidx_mdnie_maptbl(struct pkt_update_info *pktui, int offset)
{
	struct panel_device *panel;
	struct mdnie_info *mdnie;
	int row, index;

	if (!pktui || !pktui->pdata)
		return -EINVAL;

	panel = pktui->pdata;
	mdnie = &panel->mdnie;

	if (offset < 0 || offset >= mdnie->nr_reg) {
		panel_err("invalid offset %d\n", offset);
		return -EINVAL;
	}

	row = mdnie_get_decon_maptbl_index(mdnie);
	if (row < 0) {
		panel_err("invalid row %d\n", row);
		return -EINVAL;
	}

	index = row * mdnie->nr_reg + offset;
	if (index >= mdnie->nr_maptbl) {
		panel_err("exceeded index %d row %d offset %d\n",
				index, row, offset);
		return -EINVAL;
	}
	return index;
}

int getidx_mdnie_0_maptbl(struct pkt_update_info *pktui)
{
	return getidx_mdnie_maptbl(pktui, 0);
}

int getidx_mdnie_1_maptbl(struct pkt_update_info *pktui)
{
	return getidx_mdnie_maptbl(pktui, 1);
}

int getidx_mdnie_2_maptbl(struct pkt_update_info *pktui)
{
	return getidx_mdnie_maptbl(pktui, 2);
}

int getidx_mdnie_3_maptbl(struct pkt_update_info *pktui)
{
	return getidx_mdnie_maptbl(pktui, 3);
}

int getidx_mdnie_scr_white_maptbl(struct pkt_update_info *pktui)
{
	struct panel_device *panel;
	struct mdnie_info *mdnie;
	int index;

	if (!pktui || !pktui->pdata)
		return -EINVAL;

	panel = pktui->pdata;
	mdnie = &panel->mdnie;

	if (mdnie->props.scr_white_mode < 0 ||
			mdnie->props.scr_white_mode >= MAX_SCR_WHITE_MODE) {
		panel_warn("out of range %d\n",
				mdnie->props.scr_white_mode);
		return -EINVAL;
	}

	if (mdnie->props.scr_white_mode == SCR_WHITE_MODE_COLOR_COORDINATE) {
		panel_dbg("coordinate maptbl\n");
		index = MDNIE_COLOR_COORDINATE_MAPTBL;
	} else if (mdnie->props.scr_white_mode == SCR_WHITE_MODE_ADJUST_LDU) {
		panel_dbg("adjust ldu maptbl\n");
		index = MDNIE_ADJUST_LDU_MAPTBL;
	} else if (mdnie->props.scr_white_mode == SCR_WHITE_MODE_SENSOR_RGB) {
		panel_dbg("sensor rgb maptbl\n");
		index = MDNIE_SENSOR_RGB_MAPTBL;
	} else {
		panel_dbg("empty maptbl\n");
		index = MDNIE_SCR_WHITE_NONE_MAPTBL;
	}

	return index;
}
#endif /* CONFIG_EXYNOS_DECON_MDNIE_LITE */

int show_rddpm(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 rddpm[S6E3FC3_RDDPM_LEN] = { 0, };
#ifdef CONFIG_LOGGING_BIGDATA_BUG
	extern unsigned int g_rddpm;
#endif

	if (!res || ARRAY_SIZE(rddpm) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(rddpm, info->res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy rddpm resource\n");
		return -EINVAL;
	}

	panel_info("========== SHOW PANEL [0Ah:RDDPM] INFO (After DISPLAY_ON) ==========\n");
	panel_info("* Reg Value : 0x%02x, Result : %s\n",
			rddpm[0], ((rddpm[0] & 0x9C) == 0x9C) ? "GOOD" : "NG");
	panel_info("* Bootster Mode : %s\n", rddpm[0] & 0x80 ? "ON (GD)" : "OFF (NG)");
	panel_info("* Idle Mode     : %s\n", rddpm[0] & 0x40 ? "ON (NG)" : "OFF (GD)");
	panel_info("* Partial Mode  : %s\n", rddpm[0] & 0x20 ? "ON" : "OFF");
	panel_info("* Sleep Mode    : %s\n", rddpm[0] & 0x10 ? "OUT (GD)" : "IN (NG)");
	panel_info("* Normal Mode   : %s\n", rddpm[0] & 0x08 ? "OK (GD)" : "SLEEP (NG)");
	panel_info("* Display ON    : %s\n", rddpm[0] & 0x04 ? "ON (GD)" : "OFF (NG)");
	panel_info("=================================================\n");
#ifdef CONFIG_LOGGING_BIGDATA_BUG
	g_rddpm = (unsigned int)rddpm[0];
#endif

	return 0;
}

int show_rddpm_before_sleep_in(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 rddpm[S6E3FC3_RDDPM_LEN] = { 0, };

	if (!res || ARRAY_SIZE(rddpm) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(rddpm, info->res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy rddpm resource\n");
		return -EINVAL;
	}

	panel_info("========== SHOW PANEL [0Ah:RDDPM] INFO (Before SLEEP_IN) ==========\n");
	panel_info("* Reg Value : 0x%02x, Result : %s\n",
			rddpm[0], ((rddpm[0] & 0x9C) == 0x98) ? "GOOD" : "NG");
	panel_info("* Bootster Mode : %s\n", rddpm[0] & 0x80 ? "ON (GD)" : "OFF (NG)");
	panel_info("* Idle Mode     : %s\n", rddpm[0] & 0x40 ? "ON (NG)" : "OFF (GD)");
	panel_info("* Partial Mode  : %s\n", rddpm[0] & 0x20 ? "ON" : "OFF");
	panel_info("* Sleep Mode    : %s\n", rddpm[0] & 0x10 ? "OUT (GD)" : "IN (NG)");
	panel_info("* Normal Mode   : %s\n", rddpm[0] & 0x08 ? "OK (GD)" : "SLEEP (NG)");
	panel_info("* Display ON    : %s\n", rddpm[0] & 0x04 ? "ON (NG)" : "OFF (GD)");
	panel_info("=================================================\n");

	return 0;
}

int show_rddsm(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 rddsm[S6E3FC3_RDDSM_LEN] = { 0, };
#ifdef CONFIG_LOGGING_BIGDATA_BUG
	extern unsigned int g_rddsm;
#endif

	if (!res || ARRAY_SIZE(rddsm) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(rddsm, info->res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy rddsm resource\n");
		return -EINVAL;
	}

	panel_info("========== SHOW PANEL [0Eh:RDDSM] INFO ==========\n");
	panel_info("* Reg Value : 0x%02x, Result : %s\n",
			rddsm[0], (rddsm[0] == 0x80) ? "GOOD" : "NG");
	panel_info("* TE Mode : %s\n", rddsm[0] & 0x80 ? "ON(GD)" : "OFF(NG)");
	panel_info("=================================================\n");
#ifdef CONFIG_LOGGING_BIGDATA_BUG
	g_rddsm = (unsigned int)rddsm[0];
#endif

	return 0;
}

int show_err(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 err[S6E3FC3_ERR_LEN] = { 0, }, err_15_8, err_7_0;

	if (!res || ARRAY_SIZE(err) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(err, info->res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy err resource\n");
		return -EINVAL;
	}

	err_15_8 = err[0];
	err_7_0 = err[1];

	panel_info("========== SHOW PANEL [E9h:DSIERR] INFO ==========\n");
	panel_info("* Reg Value : 0x%02x%02x, Result : %s\n", err_15_8, err_7_0,
			(err[0] || err[1] || err[2] || err[3] || err[4]) ? "NG" : "GOOD");

	if (err_15_8 & 0x80)
		panel_info("* DSI Protocol Violation\n");

	if (err_15_8 & 0x40)
		panel_info("* Data P Lane Contention Detetion\n");

	if (err_15_8 & 0x20)
		panel_info("* Invalid Transmission Length\n");

	if (err_15_8 & 0x10)
		panel_info("* DSI VC ID Invalid\n");

	if (err_15_8 & 0x08)
		panel_info("* DSI Data Type Not Recognized\n");

	if (err_15_8 & 0x04)
		panel_info("* Checksum Error\n");

	if (err_15_8 & 0x02)
		panel_info("* ECC Error, multi-bit (detected, not corrected)\n");

	if (err_15_8 & 0x01)
		panel_info("* ECC Error, single-bit (detected and corrected)\n");

	if (err_7_0 & 0x80)
		panel_info("* Data Lane Contention Detection\n");

	if (err_7_0 & 0x40)
		panel_info("* False Control Error\n");

	if (err_7_0 & 0x20)
		panel_info("* HS RX Timeout\n");

	if (err_7_0 & 0x10)
		panel_info("* Low-Power Transmit Sync Error\n");

	if (err_7_0 & 0x08)
		panel_info("* Escape Mode Entry Command Error");

	if (err_7_0 & 0x04)
		panel_info("* EoT Sync Error\n");

	if (err_7_0 & 0x02)
		panel_info("* SoT Sync Error\n");

	if (err_7_0 & 0x01)
		panel_info("* SoT Error\n");

	if (err[2] != 0)
		panel_info("* CRC Error Count : %d\n", err[2]);

	if (err[3] != 0)
		panel_info("* ECC1 Error Count : %d\n", err[3]);

	if (err[4] != 0)
		panel_info("* ECC2 Error Count : %d\n", err[4]);

	panel_info("==================================================\n");

	return 0;
}

int show_err_fg(struct dumpinfo *info)
{
	int ret;
	u8 err_fg[S6E3FC3_ERR_FG_LEN] = { 0, };
	struct resinfo *res = info->res;

	if (!res || ARRAY_SIZE(err_fg) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(err_fg, res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy err_fg resource\n");
		return -EINVAL;
	}

	panel_info("========== SHOW PANEL [EEh:ERR_FG] INFO ==========\n");
	panel_info("* Reg Value : 0x%02x, Result : %s\n",
			err_fg[0], (err_fg[0] & 0x4C) ? "NG" : "GOOD");

	if (err_fg[0] & 0x04) {
		panel_info("* VLOUT3 Error\n");
//Todo
#ifdef CONFIG_DISPLAY_USE_INFO
		decon_inc_dpui_u32_field(DPUI_KEY_PNVLO3E, 1);
#endif
	}

	if (err_fg[0] & 0x08) {
		panel_info("* ELVDD Error\n");
//Todo
#ifdef CONFIG_DISPLAY_USE_INFO
		decon_inc_dpui_u32_field(DPUI_KEY_PNELVDE, 1);
#endif
	}

	if (err_fg[0] & 0x40) {
		panel_info("* VLIN1 Error\n");
//Todo
#ifdef CONFIG_DISPLAY_USE_INFO
		decon_inc_dpui_u32_field(DPUI_KEY_PNVLI1E, 1);
#endif
	}

	panel_info("==================================================\n");

	return 0;
}

int show_dsi_err(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 dsi_err[S6E3FC3_DSI_ERR_LEN] = { 0, };

	if (!res || ARRAY_SIZE(dsi_err) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(dsi_err, res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy dsi_err resource\n");
		return -EINVAL;
	}

	panel_info("========== SHOW PANEL [05h:DSIE_CNT] INFO ==========\n");
	panel_info("* Reg Value : 0x%02x, Result : %s\n",
			dsi_err[0], (dsi_err[0]) ? "NG" : "GOOD");
	if (dsi_err[0])
		panel_info("* DSI Error Count : %d\n", dsi_err[0]);
	panel_info("====================================================\n");
//Todo
#ifdef CONFIG_DISPLAY_USE_INFO
	decon_inc_dpui_u32_field(DPUI_KEY_PNDSIE, dsi_err[0]);
#endif
#if IS_ENABLED(CONFIG_SEC_ABC)
	if (dsi_err[0] > 0)
#if IS_ENABLED(CONFIG_SEC_FACTORY)
		sec_abc_send_event("MODULE=display@INFO=act_section_dsierr0");
#else
		sec_abc_send_event("MODULE=display@WARN=act_section_dsierr0");
#endif
#endif

	return 0;
}

int show_self_diag(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 self_diag[S6E3FC3_SELF_DIAG_LEN] = { 0, };

	ret = resource_copy(self_diag, res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy self_diag resource\n");
		return -EINVAL;
	}

	panel_info("========== SHOW PANEL [0Fh:SELF_DIAG] INFO ==========\n");
	panel_info("* Reg Value : 0x%02x, Result : %s\n",
			self_diag[0], (self_diag[0] & 0x40) ? "GOOD" : "NG");
	if ((self_diag[0] & 0x80) == 0)
		panel_info("* OTP value is changed\n");
	if ((self_diag[0] & 0x40) == 0)
		panel_info("* Panel Boosting Error\n");

	panel_info("=====================================================\n");
//Todo
#ifdef CONFIG_DISPLAY_USE_INFO
	decon_inc_dpui_u32_field(DPUI_KEY_PNSDRE, (self_diag[0] & 0x40) ? 0 : 1);
#endif

	return 0;
}

#ifdef CONFIG_SUPPORT_PANEL_DECODER_TEST
/*
 * s6e3fc3_decoder_test - test ddi's decoder function
 *
 * description of state values:
 * [0](14h 1st): 0x58 (OK) other (NG)
 * [1](14h 2nd): 0xAD (OK) other (NG)
 *
 * [0](15h 1st): 0x14 (OK) other (NG)
 * [1](15h 2nd): 0x74 (OK) other (NG)
 */
int s6e3fc3_decoder_test(struct panel_device *panel, void *data, u32 len)
{
	struct panel_info *panel_data;
	int ret = 0;
	u8 read_buf1[S6E3FC3_DECODER_TEST1_LEN] = { -1, -1 };
	u8 read_buf2[S6E3FC3_DECODER_TEST2_LEN] = { -1, -1 };
	u8 read_buf3[S6E3FC3_DECODER_TEST3_LEN] = { -1, -1 };
	u8 read_buf4[S6E3FC3_DECODER_TEST4_LEN] = { -1, -1 };

	if (!panel)
		return -EINVAL;

	panel_data = &panel->panel_data;

	ret = decon_decon_panel_do_seqtbl_by_index_nolock(panel, PANEL_DECODER_TEST_SEQ);
	if (unlikely(ret < 0)) {
		panel_err("failed to write decoder-test seq\n");
		return ret;
	}

	ret = resource_copy_by_name(panel_data, read_buf1, "decoder_test1"); // 0x14 in normal voltage
	if (unlikely(ret < 0)) {
		panel_err("decoder_test1 copy failed\n");
		return -ENODATA;
	}

	ret = resource_copy_by_name(panel_data, read_buf2, "decoder_test2"); // 0x15 in normal voltage
	if (unlikely(ret < 0)) {
		panel_err("decoder_test2 copy failed\n");
		return -ENODATA;
	}

	ret = resource_copy_by_name(panel_data, read_buf3, "decoder_test3"); // 0x14 in low voltage
	if (unlikely(ret < 0)) {
		panel_err("decoder_test1 copy failed\n");
		return -ENODATA;
	}

	ret = resource_copy_by_name(panel_data, read_buf4, "decoder_test4"); // 0x15 in low voltage
	if (unlikely(ret < 0)) {
		panel_err("decoder_test2 copy failed\n");
		return -ENODATA;
	}

	if ((read_buf1[0] == 0x58) && (read_buf1[1] == 0xAD) &&
		(read_buf2[0] == 0x14) && (read_buf2[1] == 0x74) &&
		(read_buf3[0] == 0x58) && (read_buf3[1] == 0xAD) &&
		(read_buf4[0] == 0x14) && (read_buf4[1] == 0x74)) {
		ret = PANEL_DECODER_TEST_PASS;
		panel_info("Fail [normal]:0x14->0x%02x,0x%02x, 0x15->0x%02x,0x%02x, [low]:0x14->0x%02x,0x%02x, 0x15->0x%02x,0x%02x, ret: %d\n",
			read_buf1[0], read_buf1[1], read_buf2[0], read_buf2[1],
			read_buf3[0], read_buf3[1], read_buf4[0], read_buf4[1],	ret);
	} else {
		ret = PANEL_DECODER_TEST_FAIL;
		panel_info("Fail [normal]:0x14->0x%02x,0x%02x, 0x15->0x%02x,0x%02x, [low]:0x14->0x%02x,0x%02x, 0x15->0x%02x,0x%02x, ret: %d\n",
			read_buf1[0], read_buf1[1], read_buf2[0], read_buf2[1],
			read_buf3[0], read_buf3[1], read_buf4[0], read_buf4[1],	ret);
	}

	snprintf((char *)data, len, "%02x %02x %02x %02x %02x %02x %02x %02x",
		read_buf1[0], read_buf1[1], read_buf2[0], read_buf2[1],
		read_buf3[0], read_buf3[1], read_buf4[0], read_buf4[1]);

	return ret;
}
#endif

#ifdef CONFIG_SUPPORT_DDI_CMDLOG
int show_cmdlog(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 cmdlog[S6E3FC3_CMDLOG_LEN];

	memset(cmdlog, 0, sizeof(cmdlog));
	ret = resource_copy(cmdlog, res);
	if (unlikely(ret < 0)) {
		panel_err("failed to copy cmdlog resource\n");
		return -EINVAL;
	}

	panel_info("dump:cmdlog\n");
	print_data(cmdlog, ARRAY_SIZE(cmdlog));

	return 0;
}
#endif

int show_self_mask_crc(struct dumpinfo *info)
{
	int ret;
	struct resinfo *res = info->res;
	u8 crc[S6E3FC3_SELF_MASK_CRC_LEN] = {0, };

	if (!res || ARRAY_SIZE(crc) != res->dlen) {
		panel_err("invalid resource\n");
		return -EINVAL;
	}

	ret = resource_copy(crc, info->res);
	if (unlikely(ret < 0)) {
		panel_err("failed to self mask crc resource\n");
		return -EINVAL;
	}

	panel_info("======= SHOW PANEL [7Fh:SELF_MASK_CRC] INFO =======\n");
	panel_info("* Reg Value : 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
			crc[0], crc[1], crc[2], crc[3]);
	panel_info("====================================================\n");

	return 0;
}

int init_gamma_mode2_brt_table(struct maptbl *tbl)
{
	struct panel_info *panel_data;
	struct panel_device *panel;
	struct panel_dimming_info *panel_dim_info;
	//todo:remove
	panel_info("++\n");
	if (tbl == NULL) {
		panel_err("maptbl is null\n");
		return -EINVAL;
	}

	if (tbl->pdata == NULL) {
		panel_err("pdata is null\n");
		return -EINVAL;
	}

	panel = tbl->pdata;
	panel_data = &panel->panel_data;

	panel_dim_info = panel_data->panel_dim_info[PANEL_BL_SUBDEV_TYPE_DISP];

	if (panel_dim_info == NULL) {
		panel_err("panel_dim_info is null\n");
		return -EINVAL;
	}

	if (panel_dim_info->brt_tbl == NULL) {
		panel_err("panel_dim_info->brt_tbl is null\n");
		return -EINVAL;
	}

	generate_brt_step_table(panel_dim_info->brt_tbl);

	/* initialize brightness_table */
	memcpy(&panel->panel_bl.subdev[PANEL_BL_SUBDEV_TYPE_DISP].brt_tbl,
			panel_dim_info->brt_tbl, sizeof(struct brightness_table));

	return 0;
}

int getidx_gamma_mode2_brt_table(struct maptbl *tbl)
{
	int row = 0;
	struct panel_info *panel_data;
	struct panel_bl_device *panel_bl;
	struct panel_device *panel = (struct panel_device *)tbl->pdata;

	if (panel == NULL) {
		panel_err("panel is null\n");
		return -EINVAL;
	}
	panel_bl = &panel->panel_bl;
	panel_data = &panel->panel_data;

	row = decon_decon_get_brightness_pac_step_by_subdev_id(panel_bl, 0, panel_bl->props.brightness);

	return decon_maptbl_index(tbl, 0, row, 0);
}

#ifdef CONFIG_SUPPORT_MASK_LAYER
bool s6e3fc3_is_120hz(struct panel_device *panel)
{
	return (getidx_s6e3fc3_current_vrr_fps(panel) == S6E3FC3_VRR_FPS_120);
}

bool s6e3fc3_is_60hz(struct panel_device *panel)
{
	return (getidx_s6e3fc3_current_vrr_fps(panel) == S6E3FC3_VRR_FPS_60);
}
#endif

bool is_panel_state_not_lpm(struct panel_device *panel)
{
	if (panel->state.cur_state != PANEL_STATE_ALPM)
		return true;

	return false;
}

MODULE_DESCRIPTION("Samsung Mobile Panel Driver");
MODULE_LICENSE("GPL");
