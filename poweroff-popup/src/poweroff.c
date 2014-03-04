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
#include <Elementary.h>
#include <bundle.h>
#include <Ecore_X.h>
#include <utilX.h>
#include "common.h"

#define PREDEF_POWEROFF "poweroff"

static void response_poweroff_yes_clicked(void *data, Evas_Object * obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	Evas_Object *rect;
	Evas_Coord w, h, size;
	static bool multi = false;

	if (multi)
		return;
	multi = true;

	_I("OK clicked");

	if (ad && ad->popup && ad->win_main) {
		release_evas_object(&(ad->popup));

		rect = evas_object_rectangle_add(evas_object_evas_get(ad->win_main));
		evas_object_geometry_get(ad->win_main, NULL, NULL, &w, &h);
		size = max(w, h);
		evas_object_resize(rect, size, size);
		evas_object_color_set(rect, 0, 0, 0, 255);
		evas_object_show(rect);
	}

/*	if (vconf_set_int(VCONFKEY_SYSMAN_POWER_OFF_STATUS, SYSTEMD_STOP_POWER_OFF) != 0)*/
	if (sysman_call_predef_action(PREDEF_POWEROFF, 0) == -1)
		_E("Failed to request poweroff to deviced");
}

static void response_poweroff_no_clicked(void *data, Evas_Object * obj, void *event_info)
{
	_I("Cancel clicked");
	object_cleanup(data);
	popup_terminate();
}

static int show_poweroff_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_("IDS_ST_BODY_POWER_OFF"),
			_("IDS_COM_BODY_OUR_PHONE_WILL_SHUT_DOWN"),
			dgettext("sys_string","IDS_COM_SK_CANCEL"),
			response_poweroff_no_clicked,
			dgettext("sys_string","IDS_COM_SK_OK"),
			response_poweroff_yes_clicked);
	if (!(ad->popup)) {
		_E("Failed to make popup");
		return -ENOMEM;
	}

	return 0;
}

static int app_create(void *data)
{
	Evas_Object *win;
	struct appdata *ad = data;
	int ret;

	ad->handler.def_term_fn = NULL;
	ad->handler.def_timeout_fn = NULL;

	win = create_win(PACKAGE);
	if (win == NULL)
		return -1;

	ad->win_main = win;

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

	return 0;
}

static int app_terminate(void *data)
{
	struct appdata *ad = data;

	release_evas_object(&(ad->win_main));

	return 0;
}

static int app_pause(void *data)
{
	return 0;
}

static int app_resume(void *data)
{
	return 0;
}

static int app_reset(bundle *b, void *data)
{
	struct appdata *ad = data;
	int ret;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
		return 0;
	}

	ret = syspopup_create(b, &(ad->handler), ad->win_main, ad);
	if (ret < 0) {
		_E("Failed to create syspopup(%d)", ret);
		goto out;
	}

	evas_object_show(ad->win_main);

	ret = show_poweroff_popup(ad);
	if (ret < 0)
		_E("Failed to show poweroff popup (%d)", ret);

out:
	if (ret < 0)
		popup_terminate();

	return ret;
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
