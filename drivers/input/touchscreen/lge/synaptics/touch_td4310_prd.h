/* production_test.h
 *
 * Copyright (C) 2015 LGE.
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <touch_core.h>

#include "touch_td4310.h"

#ifndef PRODUCTION_TEST_H
#define PRODUCTION_TEST_H

#define ROW_SIZE			30
#define COL_SIZE			18
#define MUX_SIZE			9
#define LOG_BUF_SIZE		4096	/* 4x1024 */
#define BUF_SIZE			(PAGE_SIZE * 2)
#define MAX_LOG_FILE_SIZE	(10 * 1024 * 1024)	/* 10 M byte */
#define MAX_LOG_FILE_COUNT	(4)
#define REPORT_DATA_LEN		(ROW_SIZE*COL_SIZE*2)

#define REPORT_TYPE_DELTA			94	/* 16-bit normalized image */
#define REPORT_TYPE_RAW_DATA			92	/* raw capacitance (pF) */
#define REPORT_TYPE_E2E_SHORT			95	/* raw capacitance delta */
#define REPORT_TYPE_P2P_NOISE			94	/* raw capacitance delta */

/* Normal Mode SET Spec - (td4310) */
#define RAW_DATA_MAX 			10000//3000
#define RAW_DATA_MIN 			0//800
#define RAW_DATA_MARGIN 		0
#define P2P_NOISE_MAX 			1000//60
#define P2P_NOISE_MIN 			0

#define NOISE_TEST_FRM			50
#define AMP_SHORT_MAX 			1000//110	/* Upper limit for Image1 */
#define AMP_SHORT_MIN 			0//90	/* Lower Limit for Image2 */
#define AMP_SHORT_RESULT		0
#define AMP_OPEN_MAX			1000//200
#define AMP_OPEN_MIN			0//30

#define LPWG_RAW_DATA_MAX		10000//3000
#define LPWG_RAW_DATA_MIN		0//800
#define LPWG_P2P_NOISE_MAX		1000//60
#define LPWG_P2P_NOISE_MIN		0

enum {
	TIME_INFO_SKIP,
	TIME_INFO_WRITE,
};

enum {
	RAW_DATA_TEST = 0,
	P2P_NOISE_TEST,
	AMP_SHORT_TEST,
	AMP_OPEN_TEST,
	LPWG_RAW_DATA_TEST,
	LPWG_P2P_NOISE_TEST,
	DELTA_SHOW,
};

enum {
	TEST_PASS = 0,
	TEST_FAIL,
};

extern void touch_msleep(unsigned int msecs);
int td4310_prd_register_sysfs(struct device *dev);

#endif


