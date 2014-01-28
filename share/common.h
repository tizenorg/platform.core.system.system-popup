/*
 *  system-popup
 *
 * Copyright (c) 2000 - 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __COMMON_H_
#define __COMMON_H_

#include <Ecore_X.h>
#include <appcore-efl.h>
#include <Elementary.h>
#include <utilX.h>
#include <syspopup.h>
#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "SYSTEM_POPUP"
#define _D(fmt, args...)   SLOGD(fmt, ##args)
#define _E(fmt, args...)   SLOGE(fmt, ##args)
#define _I(fmt, args...)   SLOGI(fmt, ##args)

#define FREE(arg) \
	do { \
		if(arg) { \
			free((void *)arg); \
			arg = NULL; \
		} \
	} while (0);

struct appdata {
	/* Common */
	Evas_Object *win_main;
	Evas_Object *layout_main;
	Evas_Object *popup;
	bundle      *b;
	syspopup_handler handler;

	/* For poweroff popup */
	Evas_Object *popup_poweroff;
	Evas_Object *popup_access;
	Evas_Object *popup_notification;
	Evas_Object *popup_chk;
	Evas_Object *list;
	Evas_Object *list_access;

	/* For usbotg popup */
	Evas_Object *storage_added_popup;
	Evas_Object *storage_unmount_popup;
	Evas_Object *camera_added_popup;
	char *storage_added_path;
	char *storage_unmount_path;

	/* IPC by dbus */
	E_DBus_Signal_Handler *edbus_handler;
	E_DBus_Connection     *edbus_conn;

};

void popup_terminate(void);
int load_normal_popup(struct appdata *ad,
		char *title,
		char *content,
		char *lbtnText,
		Evas_Smart_Cb lbtn_cb,
		char *rbtnText,
		Evas_Smart_Cb rbtn_cb);


#endif				/* __COMMON_H__ */
