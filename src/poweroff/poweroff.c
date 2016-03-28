/*
 *  system-popup
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
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

#include "popup-common.h"

#define SYSTEMD_STOP_POWER_OFF 4

#define POWER_BUS_NAME        "org.tizen.system.deviced"
#define POWER_OBJECT_PATH     "/Org/Tizen/System/DeviceD/Power"
#define POWER_INTERFACE_NAME  POWER_BUS_NAME".power"

#define POWER_METHOD            "reboot"
#define POWER_OPERATION_OFF     "poweroff"

static void remove_popup(const struct popup_ops *ops)
{
	static bool terminating = false;

	if (terminating)
		return;

	terminating = true;

	unload_simple_popup(ops);
	popup_terminate();
}

static void pm_state_changed(keynode_t *key, void *data)
{
	const struct popup_ops *ops = data;

	if (!key)
		return;

	if (vconf_keynode_get_int(key) != VCONFKEY_PM_STATE_LCDOFF)
		return;

	remove_popup(ops);
}

static void event_back_key_up(void *data, Evas_Object *obj, void *event_info)
{
	const struct popup_ops *ops = data;
	remove_popup(ops);
}

static void register_handlers(const struct popup_ops *ops)
{
	Evas_Object *win;

	if (vconf_notify_key_changed(
				VCONFKEY_PM_STATE,
				pm_state_changed,
				(void *)ops) != 0)
		_E("Failed to register vconf");

	win = get_window();
	if (win)
		eext_object_event_callback_add(win, EEXT_CALLBACK_BACK, event_back_key_up, (void*)ops);
}

static void unregister_handlers(const struct popup_ops *ops)
{
	Evas_Object *win;

	vconf_ignore_key_changed(VCONFKEY_PM_STATE, pm_state_changed);

	win = get_window();
	if (win)
		eext_object_event_callback_del(win, EEXT_CALLBACK_BACK, event_back_key_up);
}

static int poweroff_launch(bundle *b, const struct popup_ops *ops)
{
	register_handlers(ops);
	return 0;
}

static void poweroff_terminate(const struct popup_ops *ops)
{
	unregister_handlers(ops);
}

static void poweroff_clicked(const struct popup_ops *ops)
{
	Evas_Object *rect, *win;
	Evas_Coord w, h, size;
	static int bPowerOff = 0;
	char *param[2];
	char data[8];
	int ret;

	if (bPowerOff == 1)
		return;
	bPowerOff = 1;

	unload_simple_popup(ops);

	win = get_window();
	if (!win)
		popup_terminate();

	rect = evas_object_rectangle_add(evas_object_evas_get(win));
	evas_object_geometry_get(win, NULL, NULL, &w, &h);
	size = max(w, h);
	evas_object_resize(rect, size, size);
	evas_object_color_set(rect, 0, 0, 0, 255);
	evas_object_show(rect);

	if (vconf_set_int(VCONFKEY_SYSMAN_POWER_OFF_STATUS, SYSTEMD_STOP_POWER_OFF) != 0)
		_E("Failed to request poweroff to deviced");

	param[0] = POWER_OPERATION_OFF;
	snprintf(data, sizeof(data), "0");
	param[1] = data;
	ret = popup_dbus_method_sync(POWER_BUS_NAME,
			POWER_OBJECT_PATH,
			POWER_INTERFACE_NAME,
			POWER_METHOD,
			"si", param);
	if (ret < 0)
		_E("Failed to request poweroff to deviced (%d)", ret);
}

static const struct popup_ops poweroff_ops = {
	.name		= "poweroff",
	.show		= load_simple_popup,
	.title		= "IDS_ST_BODY_POWER_OFF",
	.content	= "IDS_TPLATFORM_BODY_POWER_OFF_THE_DEVICE_Q",
	.left_text	= "IDS_COM_SK_CANCEL",
	.right_text	= "IDS_HS_BUTTON_POWER_OFF_ABB2",
	.right		= poweroff_clicked,
	.pre		= poweroff_launch,
	.terminate	= poweroff_terminate,
};

static __attribute__ ((constructor)) void poweroff_register_popup(void)
{
	register_popup(&poweroff_ops);
}
