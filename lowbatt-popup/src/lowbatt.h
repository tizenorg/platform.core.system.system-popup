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


#ifndef __DEF_lowbatt_H_
#define __DEF_lowbatt_H_

#include <Elementary.h>

#ifndef PREFIX
#define PREFIX "/usr"
#endif /* PREFIX */

#define TEMP_DIR			"/tmp"
#define PACKAGE				"lowbatt-popup"
#define APPNAME				"lowbatt-popup"
#define ICON_DIR			"/opt/apps/org.tizen.lowbat-syspopup/res/images"
#define BG_IMAGE			TEMP_DIR"/01_popup_bg.jpg"
#define MAIN_W				(480)
#define MAIN_H				(800)
#define EDJ_PATH			"/opt/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt"
#define EDJ_NAME			EDJ_PATH"/lowbatt.edj"
#define GRP_MAIN			"main"
#define GRP_POPUP			"popup"
#define NEW_INDI
#define APPLICATION_BG			1
#define INDICATOR_HEIGHT		(38)
#define SOUND_PATH			"/opt/apps/org.tizen.lowbat-syspopup/res/keysound/09_Low_Battery.wav"

#ifndef PREDEF_POWEROFF
#define PREDEF_POWEROFF			"poweroff"
#endif /* PREFEF_POWEROFF */
	
#define BEAT

/* Acct profiling support */
#define  ACCT_PROF
#ifdef   ACCT_PROF
#include <sys/acct.h>
#endif /* ACCT_PROF */

/* Text layout */
struct text_part {
	char *part;
	char *msgid;
};

/* Main text */
struct text_part main_txt[] = {
	{"txt_title", N_("Lowbatt"),},
	{"txt_mesg", N_(""),},
};

struct appdata {
	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *popup;
	Evas_Object *layout_main;

	Evas_Object *root_w;
	Evas_Object *root_h;
	Evas_Object *bg;
	Evas_Object *indicator;

	double w_ratio;
	double h_ratio;

};

#endif				/* __DEF_lowbatt_H__ */
