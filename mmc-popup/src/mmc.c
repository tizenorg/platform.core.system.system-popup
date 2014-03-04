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
#include <devman.h>
#include <pmapi.h>
#include <sysman.h>
#include <utilX.h>
#include <notification.h>
#include <syspopup.h>
#include <svi.h>
#include "common.h"

#define CHECK_ACT 			0
#define MOUNT_ERROR_ACT 		1


static int option = -1;

int mmc_popup_start(void *data);

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
	int ret;

	opt = (char *)bundle_get_val(b, "_SYSPOPUP_CONTENT_");
	if (!strcmp(opt,"mounterr"))
		option = MOUNT_ERROR_ACT;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
	} else {
		syspopup_create(b, &handler, ad->win_main, ad);
		evas_object_show(ad->win_main);
		/* Start Main UI */
		ret = mmc_popup_start((void *)ad);
		if (ret < 0) {
			_E("Failed to show popup (%d)", ret);
			return ret;
		}
	}

	return 0;
}

/* Customized print */
void system_print(const char *format, ...)
{
	/* Un-comment return to disable logs */
	return;
/*
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
*/
}

/* Cleanup objects to avoid mem-leak */
void mmc_cleanup(struct appdata *ad)
{
	if (ad == NULL)
		return;

	if (ad->popup)
		evas_object_del(ad->popup);
	if (ad->layout_main)
		evas_object_del(ad->layout_main);
}

void mmc_response(void *data, Evas_Object * obj, void *event_info)
{
	if (data != NULL)
		mmc_cleanup(data);

	popup_terminate();
}

/* Basic popup widget */
static int mmc_create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;

	/* Add beat ui popup */
	/* No need to pass main window ptr */
	ad->popup = elm_popup_add(ad->win_main);
	if (ad->popup == NULL) {
		system_print("\n System-popup : Add popup failed \n");
		return -1;
	}
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_object_style_set(ad->popup, "transparent");
	elm_popup_timeout_set(ad->layout_main, 3);

	/* Check launch option */
	if (option == MOUNT_ERROR_ACT)
		elm_object_text_set(ad->popup, _("IDS_DN_POP_FAILED_TO_MOUNT_SD_CARD_REINSERT_OR_FORMAT_SD_CARD"));
	elm_object_part_text_set(ad->popup, "title,text", _("IDS_COM_BODY_SYSTEM_INFO_ABB"));

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, _("IDS_COM_SK_OK"));
	elm_object_part_content_set(ad->popup, "button1", btn1);
	elm_object_style_set(btn1, "popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", mmc_response, ad);


	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	evas_object_show(ad->popup);

	return 0;
}
static int mmc_svi_play(void)
{
	int r = 0;
	int handle = 0;
	r = svi_init(&handle); //Initialize SVI

	if ( r != SVI_SUCCESS ) {
		system_print("Cannot initialize SVI.\n");
		return 0;
	} else {
		r = svi_play(handle, SVI_VIB_OPERATION_LOWBATT, SVI_SND_OPERATION_LOWBATT);
		if (r != SVI_SUCCESS) {
			system_print("Cannot play sound or vibration.\n");
		}
		r = svi_fini(handle); //Finalize SVI
		if (r != SVI_SUCCESS) {
			system_print("Cannot close SVI.\n");
			return 0;
		}
	}
	return 1;
}
int mmc_popup_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = mmc_create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;
	mmc_svi_play();
	/* Change LCD brightness */
	ret_val = pm_change_state(LCD_NORMAL);
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

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

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
