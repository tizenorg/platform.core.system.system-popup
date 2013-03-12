/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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
 */



#include <stdio.h>
#include <appcore-efl.h>
#include <sensor.h>
#include <devman_haptic.h>
#include <mmf/mm_sound.h>
#include <pmapi.h>
#include <sysman.h>
#include "usbotg-unmount.h"
#include <Ecore_X.h>
#include <utilX.h>
#include <notification.h>
#include <syspopup_caller.h>
#include <appsvc.h>

#define APPLICATION_BG		1
#define INDICATOR_HEIGHT	(38)	/* the case of 480*800 */
#define NEW_INDI

#define ACCT_PROF
#ifdef ACCT_PROF
#include <sys/acct.h>
#endif /* ACCT_PROF */

#include <syspopup.h>

static const char *dev_name = NULL;

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

struct text_part {
	char *part;
	char *msgid;
};

static struct text_part main_txt[] = {
	{"txt_title", N_("USB otg popup"),},
	{"txt_mesg", N_(""),},
};


/* App Life cycle funtions */
static void win_del(void *data, Evas_Object *obj, void *event)
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
	int removenoti = -1;
	char *opt = NULL;

	dev_name = bundle_get_val(b, "device_name");
	if (dev_name == NULL)
		return 0;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
	} else {
		syspopup_create(b, &handler, ad->win_main, ad);
		evas_object_show(ad->win_main);

		/* Start Main UI */
		usbotg_unmount_start((void *)ad);
	}

	return 0;
}

/* Customized print */
void system_print(const char *format, ...)
{
	/* Un-comment return to disable logs */
	//return;

	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

/* Cleanup objects to avoid mem-leak */
void usbotg_unmount_cleanup(struct appdata *ad)
{
	if (ad == NULL)
		return;

	if (ad->popup)
		evas_object_del(ad->popup);
	if (ad->layout_main)
		evas_object_del(ad->layout_main);
}

/* Background clicked noti */
void bg_clicked_cb(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
	system_print("\n system-popup : In BG Noti \n");
	fflush(stdout);
	exit(0);
}

/* Yes clicked noti */
// TO DO
// following is private vconf key for system-server
#define VCONFKEY_REMOVED_USB_STORAGE   "memory/private/sysman/removed_storage_uevent"

void ok_clicked_cb(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
	system_print("\n system-popup : Yes Noti \n");

	struct appdata *ad = data;
	vconf_set_str(VCONFKEY_REMOVED_USB_STORAGE, ad->device_name);
	free(ad->device_name);

	fflush(stdout);
	exit(0);
}

/* Create indicator bar */
int usbotg_unmount_create_indicator(struct appdata *ad)
{
	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_HIDE);
	return 0;
}

/* Basic popup widget */
int usbotg_unmount_create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;
	Evas_Object *btn2;
	char buf[PATH_MAX] = {0, };

	/* Initialization */
	int ret_val = 0;
	snprintf(buf, PATH_MAX, "Unmount %s?", dev_name);
	ad->device_name = malloc(strlen(dev_name)+1);
	strncpy(ad->device_name, dev_name, strlen(dev_name));

	/* Add notify */
	/* No need to give main window, it will create internally */
	ad->popup = elm_popup_add(ad->win_main);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_style_set(ad->popup, "transparent");
	elm_object_text_set(ad->popup, buf);
	elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_BODY_SYSTEM_INFO_ABB"));

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, "OK");
	elm_object_part_content_set(ad->popup, "button1", btn1);
	elm_object_style_set(btn1, "popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", ok_clicked_cb, ad);

	btn2 = elm_button_add(ad->popup);
	elm_object_text_set(btn2, "Cancel");
	elm_object_part_content_set(ad->popup, "button2", btn2);
	elm_object_style_set(btn2, "popup_button/default");
	evas_object_smart_callback_add(btn2, "clicked", bg_clicked_cb, ad);

	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	evas_object_show(ad->popup);

	return 0;
}

int usbotg_unmount_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = usbotg_unmount_create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;

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

	sysconf_set_mempolicy(OOM_IGNORE);

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
