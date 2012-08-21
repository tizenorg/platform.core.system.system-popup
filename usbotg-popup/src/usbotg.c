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
#include <mmf/mm_sound.h>
#include <pmapi.h>
#include <sysman.h>
#include "usbotg.h"
#include <Ecore_X.h>
#include <utilX.h>
#include <notification.h>
#include <syspopup_caller.h>
#include <appsvc.h>

#define APPLICATION_BG		1
#define INDICATOR_HEIGHT	(38)	/* the case of 480*800 */
#define SOUND_PATH		"/opt/apps/org.tizen.usborg-syspopup/res/keysound/02_Warning.wav"
#define NEW_INDI

#define DEVICE_ADDED	1
#define DEVICE_REMOVED	0

#define UNKNOWN_USB_ICON_PATH	"/opt/apps/org.tizen.usbotg-syspopup/res/icons/usb_icon.png"
#define USB_ICON_PATH			"/opt/apps/org.tizen.usbotg-syspopup/res/icons/usb_icon.png"

#define ACCT_PROF
#ifdef ACCT_PROF
#include <sys/acct.h>
#endif /* ACCT_PROF */

#include <syspopup.h>

#define USB_MOUNT_PATH		"/opt/storage/usb"

#define GALLERY_APP_NAME	"org.tizen.gallery"
#define MYFILE_APP_NAME		"org.tizen.myfile"
#define CAMERA_DEVICE	1

static int connected_device = 0;
static char *otg_path = NULL;

int unknown_usb_noti(int option);
int camera_noti(int option, char* device_name);

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

	opt = bundle_get_val(b, "_SYSPOPUP_CONTENT_");
	if (!strcmp(opt,"unknown_add")) {
		unknown_usb_noti(DEVICE_ADDED);
	} else if (!strcmp(opt,"unknown_remove")) {
		unknown_usb_noti(DEVICE_REMOVED);
	} else {
		if(!strcmp(opt,"camera_add")) {
			connected_device = CAMERA_DEVICE;
			camera_noti(DEVICE_ADDED, bundle_get_val(b, "device_name"));
		} else if (!strcmp(opt,"camera_remove")) {
			camera_noti(DEVICE_REMOVED, NULL);
			removenoti = DEVICE_REMOVED;
		} else if (!strcmp(opt,"otg_add")) {
			otg_path = bundle_get_val(b, "path");
			otg_noti(DEVICE_ADDED, (strrchr(otg_path, '/')+1));
		} else if (!strcmp(opt,"otg_remove")) {
			otg_noti(DEVICE_REMOVED, NULL);
			removenoti = DEVICE_REMOVED;
		}

		if (syspopup_has_popup(b)) {
			if (removenoti == DEVICE_REMOVED)
				return 0;
			syspopup_reset(b);
		} else {
			if (removenoti == DEVICE_REMOVED)
				exit(0);
			syspopup_create(b, &handler, ad->win_main, ad);
			evas_object_show(ad->win_main);

			/* Start Main UI */
			usbotg_start((void *)ad);
		}
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
void usbotg_cleanup(struct appdata *ad)
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

/* Browse clicked noti */
void browse_clicked_cb(void *data, Evas * e, Evas_Object * obj, void *event_info)
{
	system_print("\n system-popup : Bwose Noti \n");

	int ret;
	bundle *b;
	b = bundle_create();

	// TO DO
	// launch my files with option
	if (connected_device == CAMERA_DEVICE) {
		appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
		appsvc_add_data(b, "album-id", "GALLERY_ALBUM_PTP_ID");
		appsvc_set_pkgname(b, GALLERY_APP_NAME);
		ret = appsvc_run_service(b, 0, NULL, (void*)NULL);
	} else {
		appsvc_set_operation(b, APPSVC_OPERATION_VIEW);
		if (otg_path != NULL) {
			appsvc_add_data(b, "path", otg_path);
		} else {
			appsvc_add_data(b, "path", USB_MOUNT_PATH);
		}
		appsvc_set_pkgname(b, MYFILE_APP_NAME);
		ret = appsvc_run_service(b, 0, NULL, (void*)NULL);
	}
	if (ret < 0)
		system_print("app launching fail(%d)", ret);
	bundle_free(b);

	fflush(stdout);
	exit(0);
}

void usbotg_clicked_cb(void *data, Evas * e, Evas_Object * obj,
		       void *event_info)
{
	system_print("\n system-popup : Screen clicked \n");
	fflush(stdout);
	elm_exit();
	exit(0);
}

/* Create indicator bar */
int usbotg_create_indicator(struct appdata *ad)
{
	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_HIDE);
	return 0;
}

/* Play vibration */
int usbotg_play_vibration()
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

/* Basic popup widget */
int usbotg_create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;
	Evas_Object *btn2;

	/* Initialization */
	int ret_val = 0;

	/* Add notify */
	/* No need to give main window, it will create internally */
	ad->popup = elm_popup_add(ad->win_main);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (connected_device == CAMERA_DEVICE)
		elm_object_text_set(ad->popup, "Browse connected CAMERA?");
	else
		elm_object_text_set(ad->popup, "Browse connected USB Storage?");
	elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_BODY_SYSTEM_INFO_ABB"));

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, "Browse");
	elm_object_part_content_set(ad->popup, "button1", btn1);
	elm_object_style_set(btn1, "popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", browse_clicked_cb, ad);

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

int usbotg_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = usbotg_create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;

	/* Change LCD brightness */
	ret_val = pm_change_state(LCD_NORMAL);
	if (ret_val != 0)
		return -1;

	/* Play vibration */
	ret_val = usbotg_play_vibration();
	if (ret_val == -1)
		system_print("\n Lowmem : Play vibration failed \n");

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

int unknown_usb_noti(int option)
{
	notification_h noti = NULL;
	notification_error_e noti_err = NOTIFICATION_ERROR_NONE;

	if (option == DEVICE_REMOVED) {
		noti_err = notification_delete_all_by_type(NULL, NOTIFICATION_TYPE_NOTI);
		system_print("unknown usb device is removed\n");
		return -1;
	} else if (option == DEVICE_ADDED) {
		noti_err = notification_delete_all_by_type(NULL, NOTIFICATION_TYPE_NOTI);
		system_print("add notification for unknow usb device\n");
		noti = notification_new(NOTIFICATION_TYPE_NOTI, NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE);
		if(noti == NULL) {
			system_print("Errot noti == NULL\n");
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_TITLE, "Unknown USB device connected", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_title : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_CONTENT_FOR_DISPLAY_OPTION_IS_OFF, "Unknown USB device connected", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_content : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, UNKNOWN_USB_ICON_PATH);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_image : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_property(noti, NOTIFICATION_PROP_DISABLE_APP_LAUNCH | NOTIFICATION_PROP_DISABLE_TICKERNOTI | NOTIFICATION_PROP_VOLATILE_DISPLAY);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_property : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_display_applist(noti, NOTIFICATION_DISPLAY_APP_NOTIFICATION_TRAY);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_display_applist : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_insert(noti, NULL);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_insert : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_free(noti);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_free : %d\n", noti_err);
			return -1;
		}
	}

	return 0;
}

int camera_noti(int option, char* device_name)
{
	notification_h noti = NULL;
	notification_error_e noti_err = NOTIFICATION_ERROR_NONE;

	if (option == DEVICE_REMOVED) {
		noti_err = notification_delete_all_by_type(NULL, NOTIFICATION_TYPE_ONGOING);
		system_print("camera removed\n");
		return -1;
	} else if (option == DEVICE_ADDED) {
		noti_err = notification_delete_all_by_type(NULL, NOTIFICATION_TYPE_ONGOING);
		system_print("add notification for camera\n");
		noti = notification_new(NOTIFICATION_TYPE_ONGOING, NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE);
		if(noti == NULL) {
			system_print("Errot noti == NULL\n");
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_TITLE, "Camera connected", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_title : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_CONTENT, device_name, NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_contents : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_CONTENT_FOR_DISPLAY_OPTION_IS_OFF, "Camera connected", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_content : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, USB_ICON_PATH);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_image : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_property(noti, NOTIFICATION_PROP_DISABLE_APP_LAUNCH | NOTIFICATION_PROP_DISABLE_TICKERNOTI | NOTIFICATION_PROP_VOLATILE_DISPLAY);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_property : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_display_applist(noti, NOTIFICATION_DISPLAY_APP_NOTIFICATION_TRAY);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_display_applist : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_insert(noti, NULL);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_insert : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_free(noti);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_free : %d\n", noti_err);
			return -1;
		}
	}

	return 0;
}

int otg_noti(int option, char* device_name)
{
	notification_h noti = NULL;
	notification_error_e noti_err = NOTIFICATION_ERROR_NONE;

	if (option == DEVICE_REMOVED) {
		noti_err = notification_delete_all_by_type(NULL, NOTIFICATION_TYPE_ONGOING);
		system_print("usb otg removed\n");
		return -1;
	} else if (option == DEVICE_ADDED) {
		noti_err = notification_delete_all_by_type(NULL, NOTIFICATION_TYPE_ONGOING);
		system_print("add notification for usb otg\n");
		noti = notification_new(NOTIFICATION_TYPE_ONGOING, NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE);
		if(noti == NULL) {
			system_print("Errot noti == NULL\n");
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_TITLE, "USB mass storage connected", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_title : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_CONTENT, device_name, NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_contents : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_CONTENT_FOR_DISPLAY_OPTION_IS_OFF, "USB mass storage connected", NULL, NOTIFICATION_VARIABLE_TYPE_NONE);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_content : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, USB_ICON_PATH);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_image : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_property(noti, NOTIFICATION_PROP_DISABLE_TICKERNOTI | NOTIFICATION_PROP_VOLATILE_DISPLAY);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_property : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_set_display_applist(noti, NOTIFICATION_DISPLAY_APP_NOTIFICATION_TRAY);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_display_applist : %d\n", noti_err);
			return -1;
		}

		bundle *b;
		b = bundle_create();
		appsvc_set_pkgname(b, "org.tizen.usbotg-unmount-popup");
		appsvc_add_data(b, "device_name", device_name);

		noti_err = notification_set_execute_option(noti, NOTIFICATION_EXECUTE_TYPE_SINGLE_LAUNCH, "Launch", NULL, b);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_set_execute_option : %d\n", noti_err);
			return -1;
		}
		bundle_free(b);


		noti_err = notification_insert(noti, NULL);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_insert : %d\n", noti_err);
			return -1;
		}

		noti_err = notification_free(noti);
		if(noti_err != NOTIFICATION_ERROR_NONE) {
			system_print("Error notification_free : %d\n", noti_err);
			return -1;
		}
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

	sysconf_set_mempolicy(OOM_IGNORE);

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
