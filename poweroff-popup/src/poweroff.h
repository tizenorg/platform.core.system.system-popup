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


#ifndef __DEF_poweroff_H_
#define __DEF_poweroff_H_

#include <Elementary.h>
#include <bundle.h>

#ifndef PREFIX
#define PREFIX "/usr"
#endif
#define TEMP_DIR "/tmp"
#define PACKAGE "poweroff-popup"
#define APPNAME "poweroff-popup"
#define ICON_DIR "/opt/apps/org.tizen.poweroff-syspopup/res/images"
#define BG_IMAGE TEMP_DIR"/01_popup_bg.jpg"
#define EDJ_PATH "/opt/apps/org.tizen.poweroff-syspopup/res/edje/poweroff"
#define EDJ_NAME EDJ_PATH"/poweroff.edj"
#define GRP_MAIN "main"
#define APPLICATION_BG	1
#define INDICATOR_HEIGHT (38)
#define NEW_INDI
#define ACCT_PROF
#define PREDEF_POWEROFF "poweroff"
#define PREDEF_ENTERSLEEP "entersleep"
#define MAIN_W	(480)
#define MAIN_H	(800)
#define BEAT

#define NAME_BUF_LEN	128
#define TITLE_BUF_LEN	128
#define CONTENT_BUF_LEN	256

/* Popup Response */
enum {
	POPUP_RESPONSE_YES = 0,
	POPUP_RESPONSE_NO,
	POPUP_RESPONSE_SLEEP
} response;

/* Text layout */
struct text_part {
	char *part;
	char *msgid;
};

/* Text part */
static struct text_part main_txt[] = {
	{"txt_title", N_("Poweroff"),},
	{"txt_mesg", N_(""),},
};

/* Main UI structure */
struct appdata {
	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *layout_main;
	Evas_Object *popup;
	Evas_Object *popup_poweroff;
	Evas_Object *popup_access;
	Evas_Object *genlist;
	Evas_Object *genlist_access;

	Evas_Object *root_w;
	Evas_Object *root_h;
	Evas_Object *bg;
	Evas_Object *indicator;
	bundle *b;

	/* Added for syspopup */
	Evas_Object *title;
	Evas_Object *content;

	Elm_Genlist_Item_Class itc;
	Elm_Genlist_Item_Class itc1;

	double w_ratio;
	double h_ratio;

};

#endif				/* __DEF_poweroff_H__ */
