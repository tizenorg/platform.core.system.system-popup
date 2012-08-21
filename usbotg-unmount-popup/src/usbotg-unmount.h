/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/



#ifndef __DEF_usbotg_unmount_H_
#define __DEF_usbotg_unmount_H_

#include <Elementary.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifndef PREFIX
#define PREFIX "/usr"
#endif /* PREFIX */

#define TEMP_DIR		"/tmp"
#define PACKAGE			"usbotg-unmount-popup"
#define APPNAME			"usbotg-unmount-popup"
#define ICON_DIR		"/opt/apps/org.tizen.usbotg-unmount-popup/res/images"
#define BG_IMAGE		TEMP_DIR"/01_popup_bg.jpg"
#define MAIN_W			(480)
#define MAIN_H			(800)
#define EDJ_PATH		"/opt/apps/org.tizen.usbotg-unmount-popup/res/edje/usbotg-unmount"
#define EDJ_NAME		EDJ_PATH"/usbotg-unmount.edj"
#define EDJ_POPUP_NAME		EDJ_PATH"/usbotg_unmount_popup.edj"
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

	const char *device_name;

};

#endif				/* __DEF_usbotg_unmount_H__ */
