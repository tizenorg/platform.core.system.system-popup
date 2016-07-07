/*
 *  system-popup
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
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

#define BUF_MAX 512
#define SYSTEMD_STOP_POWER_OFF 4

#define POWER_BUS_NAME        "org.tizen.system.deviced"
#define POWER_OBJECT_PATH     "/Org/Tizen/System/DeviceD/Power"
#define POWER_INTERFACE_NAME  POWER_BUS_NAME".power"

#define POWER_METHOD            "reboot"
#define POWER_OPERATION_OFF     "poweroff"

static const struct popup_ops overheat_ops;
static const struct popup_ops overheat_poweroff_warning_ops;
static void register_handlers(const struct popup_ops *ops);
static void unregister_handlers(const struct popup_ops *ops);
static void overheat_poweroff(const struct popup_ops *ops);
static int overheat_launch(bundle *b, const struct popup_ops *ops);
static void overheat_poweroff_warning_terminate(const struct popup_ops *ops);
static __attribute__ ((constructor)) void overheat_register_popup(void);

static char *items[] = {
	"IDS_IDLE_HEADER_PHONE_OVERHEATING_ABB",
	"IDS_QP_POP_YOUR_DEVICE_IS_OVERHEATING_IT_WILL_NOW_POWER_OFF_TO_COOL_DOWN",
	"IDS_IDLE_POP_PD_SECONDS_ARE_LEFT_BEFORE_YOUR_DEVICE_POWERS_OFF",
	"IDS_ST_BUTTON_TURN_OFF_NOW",
};

char* gl_text_get(int index)
{
	char *text = NULL, buffer[BUF_MAX] = {NULL};

	if (index == 2) {
		text = _(items[2]);
		snprintf(buffer, sizeof(buffer), text, 30);

		return strdup(buffer);
	} else return strdup(_(items[index]));
}

static void
_popup_turnoff_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	const struct popup_ops *ops = data;

	overheat_poweroff(ops);
}

static void
_popup_cancel_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	const struct popup_ops *ops = data;
	bundle *b = NULL;

	unload_simple_popup(ops);
	load_simple_popup(b, &overheat_poweroff_warning_ops);
}

static Eina_Bool
progressbar_timer_cb(void *data)
{
	Evas_Object *popup = data;
	Evas_Object *progressbar = evas_object_data_get(popup, "progressbar");
	double value = 0.0;

	value = elm_progressbar_value_get(progressbar);
	if (value == 1.0) {
		evas_object_data_del(popup, "timer");
		evas_object_del(popup);
		return ECORE_CALLBACK_CANCEL;
	}
	value = value + 0.01;
	elm_progressbar_value_set(progressbar, value);

	return ECORE_CALLBACK_RENEW;
}

static void
progressbar_popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	Ecore_Timer *timer = data;

	ecore_timer_del(timer);
}

static void unregister_handlers(const struct popup_ops *ops)
{
	return;
}

static void register_handlers(const struct popup_ops *ops)
{
		return;
}

static void overheat_poweroff(const struct popup_ops *ops)
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
		window_terminate();

	rect = evas_object_rectangle_add(evas_object_evas_get(win));
	evas_object_geometry_get(win, NULL, NULL, &w, &h);
	size = max(w, h);
	evas_object_resize(rect, size, size);
	evas_object_color_set(rect, 0, 0, 0, 255);
	evas_object_show(rect);

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

static int overheat_poweroff_warning_launch(bundle *b, const struct popup_ops *ops)
{
	register_handlers(ops);
	return 0;
}

static int overheat_launch(bundle *b, const struct popup_ops *ops)
{
	register_handlers(ops);
	return 0;
}

static void overheat_terminate(const struct popup_ops *ops)
{
	unregister_handlers(ops);
}

static void overheat_poweroff_warning_terminate(const struct popup_ops *ops)
{
	unregister_handlers(ops);
}

int overheat_popup(bundle *b, const struct popup_ops *ops)
{
	Evas_Object *popup;
	Evas_Object *layout;
	Evas_Object *btn;
	Evas_Object *progressbar;
	Evas_Object *win;
	Ecore_Timer *timer;
	struct object_ops *obj;
	int ret;

	ret = get_object_by_ops(ops, &obj);
	if (ret < 0) {
		_E("Failed to get object (%d)", ret);
		return -EINVAL;
	}

	win = get_window();
	if (!win)
		return -ENOMEM;

	evas_object_show(win);

	popup = elm_popup_add(win);
	if (!popup)
		return -ENOMEM;

	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	elm_object_part_text_set(popup, "title,text", gl_text_get(0));
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* cancel button */
	btn = elm_button_add(popup);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, "Cancel");
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", _popup_cancel_btn_clicked_cb, ops);

	/* turn off button */
	btn = elm_button_add(popup);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, gl_text_get(3));
	elm_object_part_content_set(popup, "button2", btn);
	evas_object_smart_callback_add(btn, "clicked", _popup_turnoff_btn_clicked_cb, ops);

	/* back key */
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, event_back_key_up, (void*)ops);

	/* layout */
	layout = elm_layout_add(popup);
	_D("ELM_OVERHEAT_EDC = %s", ELM_OVERHEAT_EDC);
	elm_layout_file_set(layout, ELM_OVERHEAT_EDC, "overheat_view_layout");
	elm_object_part_text_set(layout, "elm.text.contents1", gl_text_get(1));
	elm_object_part_text_set(layout, "elm.text.contents2", gl_text_get(2));
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* progressbar */
	progressbar = elm_progressbar_add(layout);
	elm_object_style_set(progressbar, "process_large");
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_part_content_set(layout, "processing", progressbar);
	timer = ecore_timer_add(0.3, progressbar_timer_cb, popup);

	elm_object_content_set(popup, layout);

	evas_object_data_set(popup, "progressbar", progressbar);
	evas_object_data_set(popup, "timer", timer);
	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, progressbar_popup_del_cb, timer);

	evas_object_show(popup);
	obj->popup = popup;

	return 0;
}

static const struct popup_ops overheat_ops = {
	.name		= "overheat", //overheat first popup
	.show		= overheat_popup,
	.pre		= overheat_launch,
	.terminate  = overheat_terminate,
};

static const struct popup_ops overheat_poweroff_warning_ops = {
	.name		= "overheat_poweroff_warning", //overheat second popup
	.show		= load_simple_popup,
	.title		= "IDS_IDLE_HEADER_PHONE_POWERING_OFF_ABB",
	.content	= "IDS_QP_POP_YOUR_DEVICE_OVERHEATED_IT_POWERED_OFF_TO_PREVENT_DAMAGE_MSG",
	.left_text	= "IDS_COM_SK_OK",
	.pre		= overheat_poweroff_warning_launch,
	.terminate  = overheat_poweroff_warning_terminate,
};

/* Constructor to register lowbattery button */
static __attribute__ ((constructor)) void overheat_register_popup(void)
{
	register_popup(&overheat_ops);
	register_popup(&overheat_poweroff_warning_ops);
}
