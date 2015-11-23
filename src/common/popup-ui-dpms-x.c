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

#include <Ecore_X.h>
#include <utilX.h>
#include "popup-ui.h"

void resize_window(void)
{
	int w, h, len;
	Evas_Object *win;

	win = get_window();
	if (win) {
		ecore_x_window_size_get(
				ecore_x_window_root_first_get(), &w, &h);
		len = max(w,h);
		evas_object_resize(win, len, len);
	}
}

int window_priority(int priority)
{
	Ecore_X_Window xwin;
	Display *dpy;
	Evas_Object *win;

	win = get_window();
	if (!win)
		return -ENOMEM;

	switch (priority) {
	case WIN_PRIORITY_LOW:
		priority = UTILX_NOTIFICATION_LEVEL_LOW;
		break;
	case WIN_PRIORITY_NORMAL:
		priority = UTILX_NOTIFICATION_LEVEL_NORMAL;
		break;
	case WIN_PRIORITY_HIGH:
		priority = UTILX_NOTIFICATION_LEVEL_HIGH;
		break;
	default:
		return -EINVAL;
	}
	if (priority < WIN_PRIORITY_LOW || priority > WIN_PRIORITY_HIGH)
		return -EINVAL;

	xwin = elm_win_xwindow_get(win);
	dpy = ecore_x_display_get();

	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(dpy, xwin, priority);

	return 0;
}
