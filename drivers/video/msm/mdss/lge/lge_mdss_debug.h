#if IS_ENABLED(CONFIG_LGE_DISPLAY_DEBUG)

#ifndef LGE_MDSS_DEBUG_H
#define LGE_MDSS_DEBUG_H


#include <linux/fs.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/of_platform.h>

#include "mdss_fb.h"
#include "mdss_dsi.h"

//NOTE : DO NOT USE THESE EVENTS WITH PANEL_OFF DSI EVENT because user data partition could be unmounted during file-access when device power-off
enum {
	DEBUG_DSI_CMD_TX = 1,
	DEBUG_DSI_CMD_RX,
	DEBUG_DSI_TIMING_CHANGE,
	DEBUG_PWR_SEQ_DELAY,
	DEBUG_PWR_ALWAYS_ON,
	DEBUG_BLMAP_CHANGE,
	DEBUG_WLED_CURR_CHANGE,
	DEBUG_TEST,
	INVALID,
};

struct debug_file_info {
	char file_name[256];
	char *cbuf;
	int *ibuf;
	loff_t file_size;
	int data_len;
	int data_type;
	int event;
};

int lge_debug_event_trigger(struct mdss_panel_data *pdata,
	char *debug_file, int debug_event);

#endif

#endif