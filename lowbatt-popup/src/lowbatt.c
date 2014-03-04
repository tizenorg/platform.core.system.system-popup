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
#include <vconf.h>
#include <utilX.h>
#include <notification.h>
#include <svi.h>
#include "common.h"

#define CHECK_ACT			0
#define WARNING_ACT			1
#define POWER_OFF_ACT		2
#define CHARGE_ERROR_ACT	3
#define BATTERY_DISCONNECT_ACT	4

#define EDJ_PATH            PREFIX"/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt"
#define EDJ_NAME            EDJ_PATH"/lowbatt.edj"

static int option = -1;

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

/* Cleanup objects to avoid mem-leak */
void lowbatt_cleanup(struct appdata *ad)
{
	if (ad == NULL)
		return;

	if (ad->popup)
		evas_object_del(ad->popup);
	if (ad->layout_main)
		evas_object_del(ad->layout_main);
}

void lowbatt_timeout_func(void *data, Evas_Object *obj, void *event_info)
{
	_D("System-popup : In Lowbatt timeout");
	lowbatt_cleanup(data);

	/* If poweroff requested */
	if (option != POWER_OFF_ACT)
		goto LOWBAT_EXIT;

	if (vconf_set_int(VCONFKEY_SYSMAN_POWER_OFF_STATUS, VCONFKEY_SYSMAN_POWER_OFF_DIRECT) != 0)
		if (system("poweroff") == -1)
			_E("FAIL: system(\"poweroff\")");
LOWBAT_EXIT:
	/* Now get lost */
	popup_terminate();
}

/* Basic popup widget */
static int lowbatt_create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;

	/* Add beat ui popup */
	/* No need to pass main window ptr */
	ad->popup = elm_popup_add(ad->win_main);
	if (ad->popup == NULL) {
		_E("System-popup : Add popup failed ");
		return -1;
	}
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_style_set(ad->popup, "transparent");
	elm_popup_timeout_set(ad->layout_main, 3);

	/* Check launch option */
	if (option == CHARGE_ERROR_ACT) {
		elm_object_text_set(ad->popup, _("IDS_COM_BODY_CHARGING_PAUSED_DUE_TO_EXTREME_TEMPERATURE"));
		elm_object_part_text_set(ad->popup, "title,text", _("IDS_ST_POP_WARNING_MSG"));
	} else if (option == BATTERY_DISCONNECT_ACT) {
		elm_object_text_set(ad->popup, _("IDS_COM_POP_BATTERY_DISCONNECTED_ABB"));
		elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_BODY_NO_BATTERY"));
	} else if (option == WARNING_ACT) {
		elm_object_text_set(ad->popup, _("IDS_COM_POP_LOW_BATTERY_CHARGE_YOUR_PHONE"));
		elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_POP_BATTERYLOW"));
	} else {
		elm_object_text_set(ad->popup, _("IDS_COM_POP_LOW_BATTERY_PHONE_WILL_SHUT_DOWN"));
		elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_POP_BATTERYLOW"));
	}

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, _("IDS_COM_SK_OK"));
	elm_object_part_content_set(ad->popup, "button1", btn1);
	elm_object_style_set(btn1, "popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", lowbatt_timeout_func, ad);


	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	evas_object_show(ad->popup);

	return 0;
}
static int lowbatt_svi_play(void)
{
	int r = 0;
	int handle = 0;
	r = svi_init(&handle); //Initialize SVI

	if ( r != SVI_SUCCESS ) {
		_E("Cannot initialize SVI.");
		return 0;
	} else {
		r = svi_play(handle, SVI_VIB_OPERATION_LOWBATT, SVI_SND_OPERATION_LOWBATT);
		if (r != SVI_SUCCESS) {
			_E("Cannot play sound or vibration.");
		}
		r = svi_fini(handle); //Finalize SVI
		if (r != SVI_SUCCESS) {
			_E("Cannot close SVI.");
			return 0;
		}
	}
	return 1;
}
int lowbatt_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = lowbatt_create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;
	lowbatt_svi_play();
	/* Change LCD brightness */
//	ret_val = display_change_state(LCD_NORMAL);
	if (ret_val != 0)
		return -1;

	return 0;
}

/* App init */
int app_create(void *data)
{
	Evas_Object *win;
	struct appdata *ad = data;
	int ret;

	/* create window */
	win = create_win(PACKAGE);
	if (win == NULL)
		return -1;

	ad->win_main = win;

	elm_theme_overlay_add(NULL,EDJ_NAME);

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

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
	const char *opt;

	opt = bundle_get_val(b, "_SYSPOPUP_CONTENT_");
	if (opt == NULL)
		option = CHECK_ACT;
	else if (!strcmp(opt,"warning"))
		option = WARNING_ACT;
	else if (!strcmp(opt,"poweroff"))
		option = POWER_OFF_ACT;
	else if (!strcmp(opt,"chargeerr"))
		option = CHARGE_ERROR_ACT;
	else if (!strcmp(opt,"battdisconnect"))
		option = BATTERY_DISCONNECT_ACT;
	else
		option = CHECK_ACT;

	if (syspopup_has_popup(b)) {
		if (option == CHECK_ACT) {
			return 0;
		}
		syspopup_reset(b);
	} else {
		if(option == CHECK_ACT) {
			popup_terminate();
			return 0;
		}
		syspopup_create(b, &handler, ad->win_main, ad);
		if (option == BATTERY_DISCONNECT_ACT)
			syspopup_reset_timeout(b, -1);
		evas_object_show(ad->win_main);

		/* Start Main UI */
		lowbatt_start((void *)ad);
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

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
