/*
 *  powerkey-popup
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

#define SYSTEMD_STOP_POWER_OFF 4

#define DEVICED_BUS_NAME        "org.tizen.system.deviced"
#define POWER_OBJECT_PATH     "/Org/Tizen/System/DeviceD/Power"
#define POWER_INTERFACE_NAME  DEVICED_BUS_NAME".power"

#define POWER_METHOD            "reboot"
#define POWER_OPERATION_OFF     "poweroff"
#define POWER_OPERATION_RESTART     "reboot"
#define TIMEOUT_POWEROFF 5 /* seconds */

#define DEVICED_OBJECT_PATH		"/Org/Tizen/System/DeviceD"
#define DEVICED_INTERFACE_NAME	DEVICED_BUS_NAME

int clicked_index = 0;

static void remove_popup(const struct popup_ops *ops);
static void pm_state_changed(keynode_t *key, void *data);
static void event_back_key_up(void *data, Evas_Object *obj, void *event_info);
static void register_handlers(const struct popup_ops *ops);
static void unregister_handlers(const struct popup_ops *ops);
static int poweroff_launch(bundle *b, const struct popup_ops *ops);
static void poweroff_terminate(const struct popup_ops *ops);
static void powerkey_terminate(const struct popup_ops *ops);
static void poweroff_clicked(const struct popup_ops *ops);
static __attribute__ ((constructor)) void powerkey_register_popup(void);
static const struct popup_ops powerkey_ops;
static const struct popup_ops poweroff_ops;
static const struct popup_ops restart_ops;

static char *items[] = {
	"IDS_ST_BODY_POWER_OFF",
	"IDS_COM_SK_RESTART_ABB"
};

static int restart_launch(bundle *b, const struct popup_ops *ops)
{
	return 0;
}

static void restart_terminate(const struct popup_ops *ops)
{

}

static void restart_clicked(const struct popup_ops *ops)
{
	Evas_Object *rect, *win;
	Evas_Coord w, h, size;
	static int bReset = 0;
	char *param[2];
	char data[8];
	int ret;

	if (bReset == 1) return;
	bReset = 1;

	unload_simple_popup(ops);

	win = get_window();
	if (!win) popup_terminate();

	rect = evas_object_rectangle_add(evas_object_evas_get(win));
	evas_object_geometry_get(win, NULL, NULL, &w, &h);
	size = max(w, h);
	evas_object_resize(rect, size, size);
	evas_object_color_set(rect, 0, 0, 0, 255);
	evas_object_show(rect);

	param[0] = POWER_OPERATION_RESTART;
	snprintf(data, sizeof(data), "0");
	param[1] = data;

	ret = popup_dbus_method_sync(DEVICED_BUS_NAME,
			POWER_OBJECT_PATH,
			POWER_INTERFACE_NAME,
			POWER_METHOD,
			"si", param);

	if (ret < 0) _E("Failed to request restart to deviced (%d)", ret);
}

static char* gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	int index = (int) data;

	switch (index) {
	case 0:
		if (!strncmp("elm.text", part, sizeof("elm.text")))
			return strdup(_(items[index]));
		else
			return NULL;

	case 1:
		if (!strncmp("elm.text", part, sizeof("elm.text")))
			return strdup(_(items[index]));
		else
			return NULL;
	}

	return NULL;
}

static void gl_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = event_info;
	bundle *b = NULL;

	clicked_index = (int)elm_object_item_data_get(item);
	unload_simple_popup(data);

	if (clicked_index == 0) {
		_D("poweroff is chosen");
		load_simple_popup(b, &poweroff_ops);
	} else if (clicked_index == 1) {
		_D("restart is chosen");
		load_simple_popup(b, &restart_ops);
	} else
		_E("Wrong button is pressed");
}

static Evas_Object* gl_image_get_cb(void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *img = elm_image_add(obj);
	Evas_Object *check;
	int index = (int) data;

	if (!img) {
		_E("BAD image");
		return NULL;
	}

	switch (index) {
	case 0:
		if (!strncmp(part, "elm.swallow.icon", sizeof("elm.swallow.icon"))) {
			elm_image_file_set(img, RESDIR"/core_power_off.png", NULL);
			evas_object_size_hint_min_set(img, ELM_SCALE_SIZE(50), ELM_SCALE_SIZE(50));
			_D("Power off img");
			return img;
		} else if (!strncmp(part, "elm.swallow.icon.end", sizeof("elm.swallow.icon.end"))) {
			check = elm_check_add(obj);
			return check;
		} else
			return NULL;

	case 1:
		if (!strncmp(part, "elm.swallow.icon", sizeof("elm.swallow.icon"))) {
			elm_image_file_set(img, RESDIR"/core_restart.png", NULL);
			evas_object_size_hint_min_set(img, ELM_SCALE_SIZE(50), ELM_SCALE_SIZE(50));
			_D("Restart img");
			return img;
		} else if (!strncmp(part, "elm.swallow.icon.end", sizeof("elm.swallow.icon.end"))) {
			check = elm_check_add(obj);
			return check;
		} else
			return NULL;

	default:
		_E("BAD data!");
		return NULL;
	}
}

int powerkey_list(bundle *b, const struct popup_ops *ops)
{
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	Evas_Object *popup = NULL;
	Evas_Object *box = NULL;
	Evas_Object *genlist = NULL;
	struct object_ops *obj;
	int i;
	Evas_Object *win;
	int ret;

	if (!ops) return -EINVAL;

	ret = get_object_by_ops(ops, &obj);
	if (ret < 0) return -EINVAL;

	win = get_window();
	if (!win) return -ENOMEM;

	evas_object_show(win);

	popup = elm_popup_add(win);
	if (!popup) return -ENOMEM;

	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_style_set(genlist, "default");

	/* box */
	box = elm_box_add(popup);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* genlist */
	genlist = elm_genlist_add(box);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	itc->item_style = "type1";
	itc->func.text_get = gl_text_get_cb;
	itc->func.content_get = gl_image_get_cb;
	itc->func.state_get = NULL;
	itc->func.del = NULL;

	for (i = 0; i < 2; i++)
		elm_genlist_item_append(genlist, itc, (void *) i, NULL, ELM_GENLIST_ITEM_NONE, gl_sel_cb, ops);

	evas_object_show(genlist);
	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1, 350);
	elm_object_content_set(popup, box);

	evas_object_show(popup);
	obj->popup = popup;

	elm_genlist_item_class_free(itc);
	return 0;
}

static void remove_popup(const struct popup_ops *ops)
{
	static bool terminating = false;

	if (terminating) return;
	terminating = true;

	unload_simple_popup(ops);
	popup_terminate();
}

static void pm_state_changed(keynode_t *key, void *data)
{
	const struct popup_ops *ops = data;

	if (!key) return;
	if (vconf_keynode_get_int(key) != VCONFKEY_PM_STATE_LCDOFF) return;

	remove_popup(ops);
}

static void event_back_key_up(void *data, Evas_Object *obj, void *event_info)
{
	const struct popup_ops *ops = data;

	if (ops) remove_popup(ops);
	terminate_if_no_popup();
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
	if (win) eext_object_event_callback_add(win, EEXT_CALLBACK_BACK, event_back_key_up, (void*)ops);
}

static void unregister_handlers(const struct popup_ops *ops)
{
	Evas_Object *win;

	vconf_ignore_key_changed(VCONFKEY_PM_STATE, pm_state_changed);

	win = get_window();
	if (win) eext_object_event_callback_del(win, EEXT_CALLBACK_BACK, event_back_key_up);
}

static int powerkey_list_launch(bundle *b, const struct popup_ops *ops)
{
	register_handlers(ops);
	return 0;
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

static void powerkey_terminate(const struct popup_ops *ops)
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

	if (bPowerOff == 1) return;
	bPowerOff = 1;

	unload_simple_popup(ops);

	win = get_window();
	if (!win) popup_terminate();

	rect = evas_object_rectangle_add(evas_object_evas_get(win));
	evas_object_geometry_get(win, NULL, NULL, &w, &h);
	size = max(w, h);
	evas_object_resize(rect, size, size);
	evas_object_color_set(rect, 0, 0, 0, 255);
	evas_object_show(rect);

	param[0] = POWER_OPERATION_OFF;
	snprintf(data, sizeof(data), "0");
	param[1] = data;
	ret = popup_dbus_method_sync(DEVICED_BUS_NAME,
			POWER_OBJECT_PATH,
			POWER_INTERFACE_NAME,
			POWER_METHOD,
			"si", param);
	if (ret < 0) _E("Failed to request poweroff to deviced (%d)", ret);
}

static const struct popup_ops restart_ops = {
	.name				= "restart",
	.show				= load_simple_popup,
	.title				= "IDS_COM_SK_RESTART_ABB",
	.content			= "IDS_ST_POP_DEVICE_WILL_RESTART",
	.left_text			= "IDS_COM_SK_CANCEL",
	.right_text			= "IDS_IDLE_BUTTON_RESTART_ABB3",
	.right				= restart_clicked,
	.pre				= restart_launch,
	.terminate			= restart_terminate,
};

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

static const struct popup_ops powerkey_ops = {
	.name		= "powerkey",
	.show		= powerkey_list,
	.pre		= powerkey_list_launch,
	.terminate	= powerkey_terminate,
};

static __attribute__ ((constructor)) void powerkey_register_popup(void)
{
	register_popup(&powerkey_ops);
	register_popup(&poweroff_ops);
	register_popup(&restart_ops);
}
