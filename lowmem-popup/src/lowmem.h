/* 
 * Copyright (c) 2000 - 2012 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * This file is part of system-popup
 * Written by DongGi Jang <dg0402.jang@samsung.com>
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered
 * into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability
 * of the software, either express or implied, including but not limited
 * to the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by licensee as a result of using, modifying or
 * distributing this software or its derivatives.
*/


#ifndef __DEF_lowmem_H_
#define __DEF_lowmem_H_

#include <Elementary.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef PREFIX
#define PREFIX "/usr"
#endif /* PREFIX */

#define TEMP_DIR		"/tmp"
#define PACKAGE			"lowmem-popup"
#define APPNAME			"lowmem-popup"
#define ICON_DIR		"/opt/apps/org.tizen.lowmem-syspopup/res/images"
#define BG_IMAGE		TEMP_DIR"/01_popup_bg.jpg"
#define MAIN_W			(480)
#define MAIN_H			(800)
#define EDJ_PATH		"/opt/apps/org.tizen.lowmem-syspopup/res/edje/lowmem"
#define EDJ_NAME		EDJ_PATH"/lowmem.edj"
#define EDJ_POPUP_NAME		EDJ_PATH"/lowmem_popup.edj"
#define GRP_MAIN		"main"
#define GRP_POPUP		"popup"
#define MAX_PROCESS_NAME	100
#define PROCESS_NAME_FILE	"/tmp/processname.txt"
#define BEAT

struct appdata {
	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *popup;

	Evas_Object *layout_main;	/* layout widget based on EDJ */

	Evas_Object *root_w;
	Evas_Object *root_h;
	Evas_Object *bg;
	Evas_Object *indicator;

	double w_ratio;
	double h_ratio;

};

#endif				/* __DEF_lowmem_H__ */
