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
#include "lowmem.h"
#include <utilX.h>
#include <svi.h>
#include "common.h"

#define APPLICATION_BG		1
#define INDICATOR_HEIGHT	(38)	/* the case of 480*800 */
#define NEW_INDI

#define PROCESS_NOTI_ACT	0
#define LOWMEM_NOTI_ACT		1
#define LOWMEM_LEVEL_WARNING	"warning"
#define LOWMEM_LEVEL_CRITICAL	"critical"

static int lowmem_option = -1;

static const char *process_name = NULL;
static const char *memnoti_level = NULL;
static const char *memnoti_size = NULL;

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

/* Cleanup objects to avoid mem-leak */
void lowmem_cleanup(struct appdata *ad)
{
	if (ad == NULL)
		return;

	if (ad->popup)
		evas_object_del(ad->popup);
	if (ad->layout_main)
		evas_object_del(ad->layout_main);
}

/* Background clicked noti */
void bg_clicked_cb(void *data, Evas_Object * obj, void *event_info)
{
	_D("system-popup : In BG Noti ");
	fflush(stdout);
	popup_terminate();
}

void lowmem_clicked_cb(void *data, Evas * e, Evas_Object * obj,
		       void *event_info)
{
	_D("system-popup : Screen clicked ");
	fflush(stdout);
	popup_terminate();
}

/* Create indicator bar */
int lowmem_create_indicator(struct appdata *ad)
{
	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_HIDE);
	return 0;
}

static int lowmem_svi_play(void)
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

void lowmem_timeout_func(void *data)
{
	_D(" System-popup : In Lowmem timeout");

	/* Cleanup */
	lowmem_cleanup(data);

	/* Now get lost */
	popup_terminate();
}

/* Basic popup widget */
int lowmem_create_and_show_basic_popup(struct appdata *ad)
{
	Evas_Object *btn1;

	/* Initialization */
	char *note = (char *)malloc(MAX_PROCESS_NAME * (sizeof(char)));
	char note_buf[MAX_PROCESS_NAME] = {0, };
	char *text;
	char *title;

	if (!note) {
		_E("System-popup : can not malloc ");
		return -1;
	}

	title = _("IDS_IDLE_BODY_LOW_MEMORY");

	if (lowmem_option == PROCESS_NOTI_ACT) {
		_D("System-popup : process name is %s ", process_name);
		text = _("IDS_IDLE_POP_PS_CLOSED");
		snprintf(note_buf, MAX_PROCESS_NAME, text, process_name);
		snprintf(note, MAX_PROCESS_NAME, "%s %s", _("IDS_COM_POP_NOT_ENOUGH_MEMORY"), note_buf);
	} else if (lowmem_option == LOWMEM_NOTI_ACT) {
		char *p;
		_D("System-popup : lowmem noti is %s ", memnoti_level);

		if (strncmp(memnoti_level, LOWMEM_LEVEL_WARNING,
		    strlen(LOWMEM_LEVEL_WARNING)) == 0) {
			p = _("IDS_DAV_BODY_LOW_MEMORY_LEFT_ORANGE");
		} else {
			p = _("IDS_COM_POP_NOT_ENOUGH_MEMORY");
		}
		snprintf(note, MAX_PROCESS_NAME, "%s", p);
	}

	/* Add notify */
	/* No need to give main window, it will create internally */
	ad->popup = elm_popup_add(ad->win_main);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND,
					 EVAS_HINT_EXPAND);
	elm_object_style_set(ad->popup, "transparent");
	elm_popup_timeout_set(ad->layout_main, 3);
	elm_object_text_set(ad->popup, note);
	elm_object_part_text_set(ad->popup, "title,text", title);

	btn1 = elm_button_add(ad->popup);
	elm_object_text_set(btn1, _("IDS_COM_SK_OK"));
	elm_object_part_content_set(ad->popup, "button1", btn1);
	elm_object_style_set(btn1, "popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", bg_clicked_cb, ad);

	Ecore_X_Window xwin;
	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	evas_object_show(ad->popup);

	free(note);

	return 0;
}

int lowmem_start(void *data)
{
	struct appdata *ad = data;
	int ret_val = 0;

	/* Create and show popup */
	ret_val = lowmem_create_and_show_basic_popup(ad);
	if (ret_val != 0)
		return -1;

	/* Play vibration */
	lowmem_svi_play();

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

	/* create window */
	win = create_win(PACKAGE);
	if (win == NULL)
		return -1;

	ad->win_main = win;

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
	int ret;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
		return 0;
	}
	ret = syspopup_create(b, &handler, ad->win_main, ad);
	if (ret < 0) {
		_E("Failed to create popup(%d)", ret);
		return ret;
	}
	evas_object_show(ad->win_main);

	memnoti_level = bundle_get_val(b, "_MEM_NOTI_");
	if (memnoti_level != NULL) {
		lowmem_option = LOWMEM_NOTI_ACT;
		memnoti_size = bundle_get_val(b, "_MEM_SIZE_");
		goto LOWMEM_START;
	}

	process_name = bundle_get_val(b, "_APP_NAME_");
	if (process_name == NULL)
		process_name = "unknown_app";
	lowmem_option = PROCESS_NOTI_ACT;


LOWMEM_START:
	/* Start Main UI */
	lowmem_start((void *)ad);
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

	//deviced_conf_set_mempolicy(OOM_IGNORE);

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
