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


#include <stdio.h>
#include <appcore-efl.h>
#include <sensor.h>
#include <devman_haptic.h>
#include <devman.h>
#include <mmf/mm_sound.h>
#include <pmapi.h>
#include <sysman.h>
#include "lowbatt.h"
#include <vconf.h>
#include <vconf-keys.h>
#include <Ecore_X.h>
#include <utilX.h>
#include <syspopup.h>

#define CHECK_ACT 			0
#define WARNING_ACT 		1
#define POWER_OFF_ACT 		2
#define CHARGE_ERROR_ACT 	3

#define BATTERY_FULL_ICON_PATH			"/opt/apps/com.samsung.lowbat-syspopup/res/icons/batt_full_icon.png"
#define VCONFKEY_TESTMODE_LOW_BATT_POPUP	"db/testmode/low_batt_popup"

static int option = -1;

int myterm(bundle *b, void *data)
{
	return 0;
}

int mytimeout(bundle *b, void *data)
{
	lowbatt_timeout_func(data);
	return 0;
}

syspopup_handler handler = {
	.def_term_fn = myterm,
	.def_timeout_fn = mytimeout
};

/* App Life cycle funtions */
static void win_del(void *data, Evas_Object * obj, void *event)
{
	elm_exit();
}

/* Quit  */
static void main_quit_cb(void *data, Evas_Object *obj, const char *emission,
		             const char *source)
{
	elm_exit();
}

/* Update text font */
static void update_ts(Evas_Object *eo, struct text_part *tp, int size)
{
	int i;

	if (eo == NULL || tp == NULL || size < 0)
		return;

	for (i = 0; i < size; i++) {
		if (tp[i].part && tp[i].msgid)
			edje_object_part_text_set(eo, tp[i].part,
						  _(tp[i].msgid));
	}
}

/* Language changed noti handler */
static int lang_changed(void *data)
{
	struct appdata *ad = data;

	if (ad->layout_main == NULL)
		return 0;

	update_ts(elm_layout_edje_get(ad->layout_main), main_txt,
		  sizeof(main_txt) / sizeof(main_txt[0]));
	return 0;
}

/* Create main window */
static Evas_Object *create_win(const char *name)
{
	Evas_Object *eo;
	int w, h;

	eo = elm_win_add(NULL, name, ELM_WIN_DIALOG_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		evas_object_smart_callback_add(eo, "delete,request", win_del, NULL);
		elm_win_alpha_set(eo, EINA_TRUE);
		ecore_x_window_size_get(ecore_x_window_root_first_get(), &w,
					&h);
		evas_object_resize(eo, w, h);
	}

	return eo;
}

/* Read from EDJ file */
static Evas_Object *load_edj(Evas_Object * parent, const char *file,
			     const char *group)
{
	Evas_Object *eo;
	int r;

	eo = elm_layout_add(parent);
	if (eo) {
		r = elm_layout_file_set(eo, file, group);
		if (!r) {
			evas_object_del(eo);
			return NULL;
		}

		evas_object_size_hint_weight_set(eo, EVAS_HINT_EXPAND,
						 EVAS_HINT_EXPAND);
	}

	return eo;
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
	char *opt = NULL;

	opt = bundle_get_val(b, "_SYSPOPUP_CONTENT_");
	if (opt == NULL)
		option = CHECK_ACT;
	else if (!strcmp(opt,"warning"))
		option = WARNING_ACT;
	else if (!strcmp(opt,"poweroff"))
		option = POWER_OFF_ACT;
	else if (!strcmp(opt,"chargeerr"))
		option = CHARGE_ERROR_ACT;
	else
		option = CHECK_ACT;

	if (syspopup_has_popup(b)) {
		if(option == CHECK_ACT) {
			return 0;
		}
		syspopup_reset(b);
	} else {
		if(option == CHECK_ACT) {
			exit(0);
		}
		syspopup_create(b, &handler, ad->win_main, ad);
		evas_object_show(ad->win_main);

		/* Start Main UI */
		lowbatt_start((void *)ad);
	}

	return 0;
}

/* Customized print */
void system_print(const char *format, ...)
{
	/* Un-comment return to disable logs */
	return;

	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

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

/* Background clicked noti */
static void bg_clicked_cb(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
	system_print("\n system-popup : Inside bg clicked \n");
	exit(0);
}   
        
static void bg_noti_cb(void *data)
{   
	ui_bgimg_reload((Evas_Object *) data);
}

/* Create indicator bar */
static int lowbatt_create_indicator(struct appdata *ad)
{
	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_HIDE);
	return 0;
}

/* Play vibration */
static int lowbatt_play_vibration()
{
	int ret_val = 0;
	int dev_handle = 0;
	int mode = 0;

	/* Open the haptic device */
	dev_handle = device_haptic_open(DEV_IDX_0, mode);
	if (dev_handle < 0)
		return -1;

	/* Play a monotone pattern for 1s */
	ret_val = device_haptic_play_monotone(dev_handle, 1000);
	device_haptic_close(dev_handle);
	if (ret_val < 0)
		return -1;

	return 0;
}

void lowbatt_timeout_func(void *data)
{
	system_print("\n System-popup : In Lowbatt timeout\n");
	lowbatt_cleanup(data);

	/* If poweroff requested */
	if (option == POWER_OFF_ACT) {
			if (sysman_call_predef_action(PREDEF_POWEROFF, 0) == -1) {
				system_print
				    ("System-popup : failed to request poweroff to system_server \n");
				fflush(stdout);
				system("poweroff");
			}
		}
	/* Now get lost */
	exit(0);
}

/* Basic popup widget */
static int lowbatt_create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;

	/* Add beat ui popup */
	/* No need to pass main window ptr */
	ad->popup = elm_popup_add(ad->win_main);
	if (ad->popup == NULL) {
		system_print("\n System-popup : Add popup failed \n");
		return -1;
	}
	evas_object_smart_callback_add(ad->popup, "block,clicked", lowbatt_timeout_func, ad);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_popup_timeout_set(ad->layout_main, 3);

	/* Check launch option */
	if (option == CHARGE_ERROR_ACT)
		elm_object_text_set(ad->popup, _("IDS_COM_BODY_CHARGING_PAUSED_DUE_TO_EXTREME_TEMPERATURE"));
	else if (option == WARNING_ACT)
		elm_object_text_set(ad->popup, _("IDS_COM_POP_BATTERYLOW"));
	else
		elm_object_text_set(ad->popup, _("IDS_COM_POP_LOW_BATTERY_PHONE_WILL_SHUT_DOWN"));
	elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_BODY_SYSTEM_INFO_ABB"));

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, _("IDS_ST_SK_OK"));
	elm_object_part_content_set(ad->popup, "button1", btn1);
	evas_object_smart_callback_add(btn1, "clicked", lowbatt_timeout_func, ad);


	/* Add callback */
	evas_object_smart_callback_add(ad->popup, "response", lowbatt_timeout_func, ad);

	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(ecore_x_display_get(), xwin, UTILX_NOTIFICATION_LEVEL_HIGH);
	evas_object_show(ad->popup);

	return 0;
}

int lowbatt_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = lowbatt_create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;

	/* Change LCD brightness */
	ret_val = pm_change_state(LCD_NORMAL);
	if (ret_val != 0)
		return -1;

	/* Play vibration */
	ret_val = lowbatt_play_vibration();
	if (ret_val == -1)
		system_print("\n Lowbatt : Play vibration failed \n");

	/* Play the sound alert */
	ret_val = mm_sound_play_keysound(SOUND_PATH, 1);
	if (ret_val != 0)
		system_print("\n Lowmem : Play sound failed \n");

	return 0;
}

/* App init */
int app_create(void *data)
{
	Evas_Object *win;
	struct appdata *ad = data;

	/* create window */
	win = create_win(PACKAGE);
	if (win == NULL)
		return -1;

	ad->win_main = win;

	elm_theme_overlay_add(NULL,EDJ_NAME); 

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

	int val = -1, ret = -1;

	ret = vconf_get_int(VCONFKEY_TESTMODE_LOW_BATT_POPUP, &val);
	if(ret == 0 && val == 1) {
		system_print("Testmode without launching popup");
		return 0;
	}

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
