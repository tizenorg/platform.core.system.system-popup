/*
 *  system-popup
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd. All rights reserved.
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

#include "popup-ui.h"

#define BUF_MAX 512

int load_normal_popup(const struct popup_ops *ops)
{
	Evas_Object *btn;
	Evas_Object *popup;
	Evas_Object *layout;
	Evas_Object *icon;
	Evas_Object *win;
	Evas_Smart_Cb clicked;
	char *text;
	char content[BUF_MAX];
	struct object_ops *obj;
	int ret;

	if (!ops)
		return -EINVAL;

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

	elm_object_style_set(popup, "circle");
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	layout = elm_layout_add(popup);
	if (!layout)
		return -ENOMEM;

	if (!ops->left_text && !ops->right_text)
		return -ENOENT;
	if (ops->left_text && ops->right_text)
		elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons2");
	else
		elm_layout_theme_set(layout, "layout", "popup", "content/circle/buttons1");
	if (ops->title)
		elm_object_part_text_set(layout, "elm.text.title", _(ops->title));

	if (ops->content)
		snprintf(content, sizeof(content), "%s", _(ops->content));
	else if (ops->get_content) {
		ret = ops->get_content(ops, content, sizeof(content));
		if (ret < 0) {
			_E("Failed to get popup content");
			return ret;
		}
	} else
		return -ENOENT;

	text = elm_entry_utf8_to_markup(content);
	if (!text)
		return -ENOMEM;
	elm_object_part_text_set(layout, "elm.text", text);
	elm_object_content_set(popup, layout);
	free(text);

	if (ops->left_text && ops->right_text) { /* Two buttons */
		/* Left button */
		btn = elm_button_add(popup);
		if (btn) {
			elm_object_style_set(btn, "popup/circle/left");
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			elm_object_part_content_set(popup, "button1", btn);
			evas_object_smart_callback_add(btn, "clicked", left_clicked, ops);

			icon = elm_image_add(btn);
			if (icon) {
				elm_image_file_set(icon, RESDIR"/circle_btn_delete.png", NULL);
				evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				elm_object_part_content_set(btn, "elm.swallow.content", icon);
				evas_object_show(icon);
			}
		}

		/* Right button */
		btn = elm_button_add(popup);
		if (btn) {
			elm_object_style_set(btn, "popup/circle/right");
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			elm_object_part_content_set(popup, "button2", btn);
			evas_object_smart_callback_add(btn, "clicked", right_clicked, ops);

			icon = elm_image_add(btn);
			if (icon) {
				elm_image_file_set(icon, RESDIR"/circle_btn_check.png", NULL);
				evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
				elm_object_part_content_set(btn, "elm.swallow.content", icon);
				evas_object_show(icon);
			}
		}
	} else { /* One button */
		if (ops->left_text) {
			text = ops->left_text;
			clicked = left_clicked;
		} else {
			text = ops->right_text;
			clicked = right_clicked;
		}

		btn = elm_button_add(popup);
		if (btn) {
			elm_object_style_set(btn, "bottom");
			elm_object_text_set(btn, _(text));
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			elm_object_part_content_set(popup, "button1", btn);
			evas_object_smart_callback_add(btn, "clicked", clicked, ops);
		}
	}

	evas_object_show(popup);

	obj->popup = popup;

	return 0;
}
