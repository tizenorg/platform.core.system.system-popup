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


#include <stdio.h>
#include <svi.h>
#include <sysman.h>
#include "poweroff.h"
#include "common.h"

#include <Ecore_X.h>
#include <utilX.h>

int create_and_show_basic_popup_min(struct appdata *ad);
void poweroff_response_yes_cb(void *data, Evas_Object * obj, void *event_info);
void poweroff_response_no_cb(void *data, Evas_Object * obj, void *event_info);

int myterm(bundle *b, void *data)
{
	return 0;
}

int mytimeout(bundle *b, void *data)
{
	return 0;
}

syspopup_handler handler = {
	.def_term_fn = myterm,
	.def_timeout_fn = mytimeout
};

/* App Life cycle funtions */
static void win_del(void *data, Evas_Object * obj, void *event)
{
	popup_terminate();
}

/* Cleanup objects to avoid mem-leak */
void poweroff_cleanup(struct appdata *ad)
{
	if (!ad)
		return;

	if (ad->popup)
		evas_object_del(ad->popup);
	if (ad->layout_main)
		evas_object_del(ad->layout_main);
}

void poweroff_response_yes_cb(void *data, Evas_Object * obj, void *event_info)
{
	static int bPowerOff = 0;
	if (1 == bPowerOff)
		return;
	bPowerOff = 1;
	_I("System-popup : Switching off phone !! Bye Bye");
	/* This will cleanup the memory */
	poweroff_cleanup(data);

	if (sysman_call_predef_action(PREDEF_POWEROFF, 0) == -1) {
		_E("System-popup : failed to request poweroff to system_server");
		system("poweroff");
	}
}

void poweroff_response_no_cb(void *data, Evas_Object * obj, void *event_info)
{
	_I("System-popup: Option is Wrong");
	poweroff_cleanup(data);
	popup_terminate();
}

int create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;
	Evas_Object *btn2;

	ad->popup = elm_popup_add(ad->win_main);
	if (ad->popup == NULL) {
		_E("System-popup : Add popup failed ");
		return -1;
	}

	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_style_set(ad->popup, "transparent");
	elm_object_text_set(ad->popup, _("IDS_ST_BODY_POWER_OFF"));
	elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_BODY_SYSTEM_INFO_ABB"));

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, _("IDS_COM_SK_CANCEL"));
	elm_object_part_content_set(ad->popup, "button1", btn1);
	elm_object_style_set (btn1,"popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", poweroff_response_no_cb, ad);
	btn2 = elm_button_add(ad->popup);
	elm_object_text_set(btn2, _("IDS_COM_SK_OK"));
	elm_object_part_content_set(ad->popup, "button2", btn2);
	elm_object_style_set (btn2,"popup_button/default");
	evas_object_smart_callback_add(btn2, "clicked", poweroff_response_yes_cb, ad);

	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_grab_key(ecore_x_display_get(), xwin, KEY_SELECT, SHARED_GRAB);
	evas_object_show(ad->popup);
	return 0;
}

/* Start UI */
int poweroff_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;

	return 0;
}

/* App init */
int app_create(void *data)
{

	Evas_Object *win;
	struct appdata *ad = data;

	/* Create window (Reqd for sys-popup) */
	win = create_win(PACKAGE);
	if (win == NULL)
		return -1;

	ad->win_main = win;

	elm_theme_overlay_add(NULL,EDJ_NAME); 

	return 0;
}

/* Terminate noti handler */
static int app_terminate(void *data)
{
	struct appdata *ad = data;

	if (ad->layout_main)
		evas_object_del(ad->layout_main);

	if (ad->win_main)
		evas_object_del(ad->win_main);

	return 0;
}

/* Pause/background */
static int app_pause(void *data)
{
	return 0;
}

/* Resume */
static int app_resume(void *data)
{
	return 0;
}


/* Reset */
static int app_reset(bundle *b, void *data)
{
	struct appdata *ad = data;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
	} else {
		syspopup_create(b, &handler, ad->win_main, ad);
		evas_object_show(ad->win_main);

		/* Start Main UI */
		poweroff_start((void *)ad);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	struct appdata ad;

	/* App life cycle management */
	struct appcore_ops ops = {
		.create = app_create,
		.terminate = app_terminate,
		.pause = app_pause,
		.resume = app_resume,
		.reset = app_reset,
	};

	memset(&ad, 0x0, sizeof(struct appdata));
	ops.data = &ad;

	/* Go into loop */
	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
