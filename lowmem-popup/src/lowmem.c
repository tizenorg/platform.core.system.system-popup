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
#include <Elementary.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "common.h"

#define LOWMEM_LEVEL_WARNING	"warning"
#define LOWMEM_LEVEL_CRITICAL	"critical"

#define BUF_MAX 512

void lowmem_ok_clicked(void *data, Evas_Object *obj, void *event_info)
{
	_I("OK clicked");
	object_cleanup(data);
	popup_terminate();
}

static int load_low_storage_popup(struct appdata *ad, char *level)
{
	char *content;
	if (!ad || !level)
		return -EINVAL;

	if (!strncmp(LOWMEM_LEVEL_WARNING, level, strlen(LOWMEM_LEVEL_WARNING)))
		content = _("IDS_DAV_BODY_LOW_MEMORY_LEFT_ORANGE");
	else if (!strncmp(LOWMEM_LEVEL_CRITICAL, level, strlen(LOWMEM_LEVEL_CRITICAL)))
		content = _("IDS_COM_POP_NOT_ENOUGH_MEMORY");
	else
		content = _("IDS_COM_POP_NOT_ENOUGH_MEMORY");

	ad->popup = load_normal_popup(ad,
			NULL,
			content,
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowmem_ok_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_low_memory_popup(struct appdata *ad, char *process_name)
{
	char text[BUF_MAX];
	char content[BUF_MAX];
	char *subtext1, *subtext2;

	if (!ad || !process_name)
		return -EINVAL;

	subtext1 = _("IDS_IDLE_POP_PS_CLOSED");
	subtext2 = _("IDS_COM_POP_NOT_ENOUGH_MEMORY");
	snprintf(text, sizeof(text), subtext1, process_name);

	snprintf(content, sizeof(content), "%s %s", subtext2, text);

	ad->popup = load_normal_popup(ad,
			_("IDS_IDLE_BODY_LOW_MEMORY"),
			content,
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowmem_ok_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

int app_create(void *data)
{
	Evas_Object *win;
	struct appdata *ad = data;
	int ret;

	ad->handler.def_term_fn = NULL;
	ad->handler.def_timeout_fn = NULL;

	win = create_win(PACKAGE);
	if (win == NULL)
		return -1;

	ad->win_main = win;

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

	return 0;

}

static int app_terminate(void *data)
{
	struct appdata *ad = data;

	release_evas_object(&(ad->win_main));

	return 0;
}

static int app_pause(void *data)
{
	return 0;
}

static int app_resume(void *data)
{
	return 0;
}

static int app_reset(bundle *b, void *data)
{
	struct appdata *ad = data;
	int ret;
	char *level, *process_name;

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
		return 0;
	}

	ret = syspopup_create(b, &(ad->handler), ad->win_main, ad);
	if (ret < 0) {
		_E("Failed to create popup(%d)", ret);
		return ret;
	}

	evas_object_show(ad->win_main);

	level = (char *)bundle_get_val(b, "_MEM_NOTI_");
	if (level) {
		ret = load_low_storage_popup(ad, level);
		goto out;
	}

	process_name = (char *)bundle_get_val(b, "_APP_NAME_");
	if (!process_name)
		process_name = "Unknown app";
	ret = load_low_memory_popup(ad, process_name);

out:
	if (ret < 0)
		popup_terminate();

	return ret;
}

int main(int argc, char *argv[])
{
	struct appdata ad;

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
