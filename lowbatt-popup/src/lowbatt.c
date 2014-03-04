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
#include <vconf.h>
#include <svi.h>
#include "common.h"

#define DEVICED_PATH_SYSNOTI        "/Org/Tizen/System/DeviceD/SysNoti"
#define DEVICED_INTERFACE_SYSNOTI   "org.tizen.system.deviced.SysNoti"
#define SIGNAL_CHARGEERR_RESPONSE   "ChargeErrResponse"


#define LOWBATT_WARNING_TITLE      "IDS_ST_BODY_LEDOT_LOW_BATTERY"
#define LOWBATT_WARNING_CONTENT    "IDS_COM_POP_LOW_BATTERY_CHARGE_YOUR_PHONE"
#define LOWBATT_POWEROFF_TITLE     "IDS_ST_BODY_LEDOT_LOW_BATTERY"
#define LOWBATT_POWEROFF_CONTENT   "IDS_COM_POP_LOW_BATTERY_PHONE_WILL_SHUT_DOWN"
#define LOWBATT_CRITICAL_TITLE     LOWBATT_WARNING_TITLE
#define LOWBATT_CRITICAL_CONTENT   LOWBATT_WARNING_CONTENT
#define LOWBATT_EXTREME_TITLE      LOWBATT_WARNING_TITLE
#define LOWBATT_EXTREME_CONTENT    LOWBATT_WARNING_CONTENT
#define CHARGE_ERR_TITLE           "IDS_ST_POP_WARNING_MSG"
#define CHARGE_ERR_CONTENT         "IDS_COM_BODY_CHARGING_PAUSED_DUE_TO_EXTREME_TEMPERATURE"
#define CHARGE_ERR_LOW_TITLE       "IDS_ST_POP_WARNING_MSG"
#define CHARGE_ERR_LOW_CONTENT     "IDS_IDLE_POP_UNABLE_CHANGE_BATTERY_TEMA_LOW"
#define CHARGE_ERR_HIGH_TITLE      "IDS_ST_POP_WARNING_MSG"
#define CHARGE_ERR_HIGH_CONTENT    "IDS_IDLE_POP_UNABLE_CHANGE_BATTERY_TEMA_HIGH"
#define CHARGE_ERR_OVP_TITLE       "IDS_ST_POP_WARNING_MSG"
#define CHARGE_ERR_OVP_CONTENT     "IDS_COM_POP_CHARGING_PAUSED_VOLTAGE_TOO_HIGH"
#define BATT_DISCONNECTED_TITLE    "IDS_COM_BODY_NO_BATTERY"
#define BATT_DISCONNECTED_CONTENT  "IDS_COM_POP_BATTERY_DISCONNECTED_ABB"

enum lowbat_options {
	LOWBAT_WARNING,
	LOWBAT_POWEROFF,
	LOWBAT_CRITICAL,
	LOWBAT_EXTREME,
	LOWBAT_CHARGE_ERR,
	LOWBAT_CHARGE_ERR_LOW,
	LOWBAT_CHARGE_ERR_HIGH,
	LOWBAT_CHARGE_ERR_OVP,
	LOWBAT_BATT_DISCONNECT,
};

struct popup_type {
	char *name;
	int type;
};

static const struct popup_type lowbat_type[] = {
	{ "warning"           , LOWBAT_WARNING            },
	{ "poweroff"          , LOWBAT_POWEROFF           },
	{ "critical"          , LOWBAT_CRITICAL           },
	{ "extreme"           , LOWBAT_EXTREME           },
	{ "chargeerr"         , LOWBAT_CHARGE_ERR         },
	{ "chargeerrlow"      , LOWBAT_CHARGE_ERR_LOW     },
	{ "chargeerrhigh"     , LOWBAT_CHARGE_ERR_HIGH    },
	{ "chargeerrovp"      , LOWBAT_CHARGE_ERR_OVP     },
	{ "battdisconnect"    , LOWBAT_BATT_DISCONNECT    },
};

static void lowbatt_ok_clicked(void *data, Evas_Object *obj, void *event_info)
{
	_I("OK clicked");
	object_cleanup(data);
	popup_terminate();
}

static void lowbatt_shutdown_clicked(void *data, Evas_Object *obj, void *event_info)
{
	_I("Shutdown clicked");
	object_cleanup(data);
	if (vconf_set_int(VCONFKEY_SYSMAN_POWER_OFF_STATUS, VCONFKEY_SYSMAN_POWER_OFF_DIRECT) != 0)
		_E("Failed to request poweroff to deviced");
	popup_terminate();
}

static void charge_error_response(void *data, Evas_Object *obj, void *event_info)
{
	_I("Charge error");
	object_cleanup(data);

	if (broadcast_dbus_signal(DEVICED_PATH_SYSNOTI,
				DEVICED_INTERFACE_SYSNOTI,
				SIGNAL_CHARGEERR_RESPONSE,
				NULL, NULL) < 0)
		_E("Failed to send signal for error popup button");

	popup_terminate();
}

static int load_battery_warning_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(LOWBATT_WARNING_TITLE),
			_(LOWBATT_WARNING_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowbatt_ok_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_battery_poweroff_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(LOWBATT_POWEROFF_TITLE),
			_(LOWBATT_POWEROFF_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowbatt_shutdown_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_battery_critical_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(LOWBATT_CRITICAL_TITLE),
			_(LOWBATT_CRITICAL_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowbatt_ok_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_battery_extreme_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(LOWBATT_EXTREME_TITLE),
			_(LOWBATT_EXTREME_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowbatt_ok_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_charge_error_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(CHARGE_ERR_TITLE),
			_(CHARGE_ERR_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			charge_error_response,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_charge_error_low_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	reset_window_priority(ad->win_main, 2);

	ad->popup = load_normal_popup(ad,
			_(CHARGE_ERR_LOW_TITLE),
			_(CHARGE_ERR_LOW_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			charge_error_response,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_charge_error_high_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	reset_window_priority(ad->win_main, 2);

	ad->popup = load_normal_popup(ad,
			_(CHARGE_ERR_HIGH_TITLE),
			_(CHARGE_ERR_HIGH_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			charge_error_response,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_charge_error_ovp_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(CHARGE_ERR_OVP_TITLE),
			_(CHARGE_ERR_OVP_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowbatt_ok_clicked,
			NULL, NULL);
	if (!(ad->popup)) {
		_E("FAIL: load_normal_popup()");
		return -ENOMEM;
	}

	return 0;
}

static int load_battery_disconnected_popup(struct appdata *ad)
{
	if (!ad)
		return -EINVAL;

	ad->popup = load_normal_popup(ad,
			_(BATT_DISCONNECTED_TITLE),
			_(BATT_DISCONNECTED_CONTENT),
			dgettext("sys_string","IDS_COM_SK_OK"),
			lowbatt_ok_clicked,
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
	const char *opt;
	int type, i, ret;

	if (!ad || !b) {
		ret = -EINVAL;
		goto out;
	}

	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
		return 0;
	}

	opt = bundle_get_val(b, "_SYSPOPUP_CONTENT_");
	if (!opt) {
		_E("Failed to get bundle value");
		ret = -ENOMEM;
		goto out;
	}

	type = -1;
	for (i = 0 ; i < ARRAY_SIZE(lowbat_type) ; i++) {
		if (!strncmp(opt, lowbat_type[i].name, strlen(opt))) {
			type = lowbat_type[i].type;
			break;
		}
	}
	if (type < 0) {
		_E("Failed to get popup type(%d)", type);
		ret = -EINVAL;
		goto out;
	}

	ret = syspopup_create(b, &(ad->handler), ad->win_main, ad);
	if (ret < 0) {
		_E("Failed to create syspopup(%d)", ret);
		goto out;
	}

	syspopup_reset_timeout(b, -1);

	evas_object_show(ad->win_main);

	switch (type) {
	case LOWBAT_WARNING:
		ret = load_battery_warning_popup(ad);
		break;
	case LOWBAT_POWEROFF:
		ret = load_battery_poweroff_popup(ad);
		break;
	case LOWBAT_CRITICAL:
		ret = load_battery_critical_popup(ad);
		break;
	case LOWBAT_EXTREME:
		ret = load_battery_extreme_popup(ad);
		break;
	case LOWBAT_CHARGE_ERR:
		ret = load_charge_error_popup(ad);
		break;
	case LOWBAT_CHARGE_ERR_LOW:
		ret = load_charge_error_low_popup(ad);
		break;
	case LOWBAT_CHARGE_ERR_HIGH:
		ret = load_charge_error_high_popup(ad);
		break;
	case LOWBAT_CHARGE_ERR_OVP:
		ret = load_charge_error_ovp_popup(ad);
		break;
	case LOWBAT_BATT_DISCONNECT:
		ret = load_battery_disconnected_popup(ad);
		break;
	default:
		_E("Known popup type (%d)", type);
		ret = -EINVAL;
		break;
	}
	if (ret < 0)
		goto out;

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
