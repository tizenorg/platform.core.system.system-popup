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

#include "common.h"

/* Terminate popup */
static Eina_Bool exit_idler_cb(void *data)
{
	elm_exit();
	return ECORE_CALLBACK_CANCEL;
}

void popup_terminate(void)
{
	if (ecore_idler_add(exit_idler_cb, NULL))
		return;

	exit_idler_cb(NULL);
}

int load_normal_popup(struct appdata *ad,
			char *title,
			char *content,
			char *lbtnText,
			Evas_Smart_Cb lbtn_cb,
			char *rbtnText,
			Evas_Smart_Cb rbtn_cb)
{
	Evas_Object *lbtn;
	Evas_Object *rbtn;
	Ecore_X_Window xwin;

	if (!ad || !(ad->win_main) || !content)
		return -EINVAL;

	evas_object_show(ad->win_main);
	ad->popup = elm_popup_add(ad->win_main);
	evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_style_set(ad->popup, "transparent");
	elm_object_text_set(ad->popup, content);

	if (title) {
		/* Popup title */
		elm_object_part_text_set(ad->popup, "title,text", title);
	}

	if (lbtnText && lbtn_cb) {
		/* Left button */
		lbtn = elm_button_add(ad->popup);
		elm_object_text_set(lbtn, lbtnText);
		elm_object_part_content_set(ad->popup, "button1", lbtn);
		evas_object_smart_callback_add(lbtn, "clicked", lbtn_cb, ad);
	}

	if (rbtnText && rbtn_cb) {
		/* Right button */
		rbtn = elm_button_add(ad->popup);
		elm_object_text_set(rbtn, rbtnText);
		elm_object_part_content_set(ad->popup, "button2", rbtn);
		evas_object_smart_callback_add(rbtn, "clicked", rbtn_cb, ad);
	}

	xwin = elm_win_xwindow_get(ad->popup);
	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);

	evas_object_show(ad->popup);

	return 0;
}

