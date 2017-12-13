#include <linux/delay.h>
#include "mdss_dsi.h"
#include "mdss_mdp.h"
#include <soc/qcom/lge/board_lge.h>
#include "lge/reader_mode.h"

#ifdef NO_USE //CONFIG_MACH_LGE
extern int sched_set_boost(int enable);
static bool is_panel_off = false;
bool is_boost_en = false;
#endif

static int mdss_dsi_request_power_gpios(struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	int rc = 0;

	rc = lge_extra_gpio_request(ctrl_pdata, "iovcc");
	if (rc) {
		pr_err("request iovcc gpio failed, rc=%d\n", rc);
		return rc;
	}

	return rc;
}

int gpio_power_ctrl(struct mdss_panel_data *pdata, int enable)
{
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;
	struct mdss_panel_info *pinfo = NULL;
	int rc = 0;

	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}

	ctrl_pdata = container_of(pdata, struct mdss_dsi_ctrl_pdata,
			panel_data);

	pinfo = &(ctrl_pdata->panel_data.panel_info);

	pr_info("%s: enable = %d\n", __func__, enable);

	if (enable) {
		rc = mdss_dsi_request_power_gpios(ctrl_pdata);
		if (rc) {
			pr_err("gpio request failed\n");
			return rc;
		}

		if (!pinfo->cont_splash_enabled) {
			pr_info("%s: turn panel power on\n", __func__);

			lge_extra_gpio_set_value(ctrl_pdata, "iovcc", 1);
			pr_info("%s: turn on vddi\n", __func__);
		}
	} else {
		pr_info("%s: turn panel power off\n", __func__);
		lge_extra_gpio_set_value(ctrl_pdata, "iovcc", 0);
		lge_extra_gpio_free(ctrl_pdata, "iovcc");
		pr_info("%s: turn off vddi\n", __func__);
	}

	pr_info("%s: -n", __func__);

	return rc;
}
#if IS_ENABLED(CONFIG_LGE_DISPLAY_OVERRIDE_MDSS_DSI_PANEL_POWER_ON)
int mdss_dsi_panel_power_on(struct mdss_panel_data *pdata)
{
	int ret = 0;
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;
#ifdef NO_USE //CONFIG_MACH_LGE
	static int is_first_time = 1;
#endif

	pr_info("%s ++ \n", __func__);

	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		return -EINVAL;
	}
#ifdef NO_USE //CONFIG_MACH_LGE
	if (!is_first_time && is_panel_off) {
		sched_set_boost(1);
		is_boost_en = true;
		pr_info("display:set_boost++\n");
	} else
		is_first_time = 0;
#endif

	ctrl_pdata = container_of(pdata, struct mdss_dsi_ctrl_pdata,
			panel_data);

	ret = gpio_power_ctrl(pdata, 1);	//vddi high
	if (ret)
		pr_err("%s: Panel power on failed, rc=%d\n",
				__func__, ret);
	usleep_range(100, 100);

	ret = msm_dss_enable_vreg(
			ctrl_pdata->panel_power_data.vreg_config,
			ctrl_pdata->panel_power_data.num_vreg, 1);
	if (ret) {
		pr_err("%s: failed to enable vregs for %s\n",
				__func__, __mdss_dsi_pm_name(DSI_PANEL_PM));
		return ret;
	}

#ifdef NO_USE //CONFIG_MACH_LGE
	is_panel_off = false;
#endif

	pr_info("%s -- \n", __func__);

	return ret;
}
#endif

#if IS_ENABLED(CONFIG_LGE_DISPLAY_OVERRIDE_MDSS_DSI_PANEL_POWER_OFF)
int mdss_dsi_panel_power_off(struct mdss_panel_data *pdata)
{
	int ret = 0;
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = NULL;

	if (pdata == NULL) {
		pr_err("%s: Invalid input data\n", __func__);
		ret = -EINVAL;
		return ret;
	}

	ctrl_pdata = container_of(pdata, struct mdss_dsi_ctrl_pdata,
			panel_data);

	ret = gpio_power_ctrl(pdata, 0); // vddi low

	if (ret)
		pr_err("%s: Panel Power off failed, rc=%d\n",
				__func__, ret);
	usleep_range(5000, 5000);

	ret = msm_dss_enable_vreg(
			ctrl_pdata->panel_power_data.vreg_config,
			ctrl_pdata->panel_power_data.num_vreg, 0);

	if (ret)
		pr_err("%s: failed to disable vregs for %s\n",
				__func__, __mdss_dsi_pm_name(DSI_PANEL_PM));

#ifdef NO_USE //CONFIG_MACH_LGE
	is_panel_off = false;
#endif

	pr_info("%s: -- \n", __func__);
	return ret;
}
#endif

#if IS_ENABLED(CONFIG_LGE_DISPLAY_OVERRIDE_MDSS_DSI_PANEL_RESET)
/*
 * mdss_dsi_request_gpios() should be defined in each panel file
 */
int mdss_dsi_panel_reset(struct mdss_panel_data *pdata, int enable)
{
	int rc = 0;
	// SKIP RESET if using INX+NT51021 Panel
	return rc;
}
#endif


#if IS_ENABLED(CONFIG_LGE_DISPLAY_OVERRIDE_MDSS_DSI_CTRL_SHUTDOWN)
void mdss_dsi_ctrl_shutdown(struct platform_device *pdev)
{
	struct mdss_dsi_ctrl_pdata *ctrl_pdata = platform_get_drvdata(pdev);

	if (!ctrl_pdata) {
		pr_err("%s: no driver data\n", __func__);
		return;
	}

	lge_extra_gpio_set_value(ctrl_pdata, "iovcc", 0);	//iovcc low
	pr_info("%s: turn panel shutdown\n", __func__);

	return;
}
#endif

extern int mdss_dsi_parse_dcs_cmds(struct device_node *np, struct dsi_panel_cmds *pcmds, char *cmd_key, char *link_key);
extern void mdss_dsi_panel_cmds_send(struct mdss_dsi_ctrl_pdata *ctrl, struct dsi_panel_cmds *pcmds, u32 flags);

static struct dsi_panel_cmds reader_mode_cmds[4];

int lge_mdss_dsi_parse_reader_mode_cmds(struct device_node *np, struct mdss_dsi_ctrl_pdata *ctrl_pdata)
{
	mdss_dsi_parse_dcs_cmds(np, &reader_mode_cmds[READER_MODE_OFF], "qcom,panel-reader-mode-off-command", "qcom,mdss-dsi-reader-mode-command-state");
	mdss_dsi_parse_dcs_cmds(np, &reader_mode_cmds[READER_MODE_STEP_1], "qcom,panel-reader-mode-step1-command", "qcom,mdss-dsi-reader-mode-command-state");
	mdss_dsi_parse_dcs_cmds(np, &reader_mode_cmds[READER_MODE_STEP_2], "qcom,panel-reader-mode-step2-command", "qcom,mdss-dsi-reader-mode-command-state");
	mdss_dsi_parse_dcs_cmds(np, &reader_mode_cmds[READER_MODE_STEP_3], "qcom,panel-reader-mode-step3-command", "qcom,mdss-dsi-reader-mode-command-state");

	return 0;
}

static bool change_reader_mode(struct mdss_dsi_ctrl_pdata *ctrl, int new_mode)
{
	if (new_mode == READER_MODE_MONO) {
		pr_info("%s: READER_MODE_MONO is not supported. reader mode is going off.\n", __func__);
		new_mode = READER_MODE_STEP_2;
	}

	if(reader_mode_cmds[new_mode].cmd_cnt) {
		pr_info("%s: sending reader mode commands [%d]\n", __func__, new_mode);
		mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_ON);
		mdss_dsi_panel_cmds_send(ctrl, &reader_mode_cmds[new_mode], CMD_REQ_COMMIT);
		mdss_mdp_clk_ctrl(MDP_BLOCK_POWER_OFF);
	}
	return true;
}

bool lge_change_reader_mode(struct mdss_dsi_ctrl_pdata *ctrl, int old_mode, int new_mode)
{
	if (old_mode == new_mode) {
		pr_info("%s: same mode [%d]\n", __func__, new_mode);
		return true;
	}

	return change_reader_mode(ctrl, new_mode);
}

int lge_mdss_dsi_panel_send_post_on_cmds(struct mdss_dsi_ctrl_pdata *ctrl, int cur_mode)
{
	if (cur_mode != READER_MODE_OFF)
		change_reader_mode(ctrl, cur_mode);
	return 0;
}
