/*
 * system-popup
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
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

#include <dlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <bundle_internal.h>
#include <aul.h>
#include <appcore-efl.h>
#include <Ecore.h>
#include <Elementary.h>
#include <syspopup_caller.h>
#include <dbus/dbus.h>
#include "macro.h"

#define DBUS_REPLY_TIMEOUT  (120 * 1000)
#define BUF_MAX 256

#define MMC_ENCRYPTION_UG  "setting-mmc-encryption-efl"
#define SECURITY_UG        "setting-security-efl"

enum ode_error_type {
	NOT_ENOUGH_SPACE,
	OPERATION_FAILED,
	ODE_ERROR_MAX,
};

static bool (*is_storage_encryption_restricted)(void) = NULL;

void register_storage_encryption_restricted_function(bool (*func)(void))
{
	if (func)
		is_storage_encryption_restricted = func;
}

static Eina_Bool exit_idler_cb(void *data)
{
	elm_exit();
	return ECORE_CALLBACK_CANCEL;
}

static void sender_terminate(void)
{
	if (ecore_idler_add(exit_idler_cb, NULL))
		return;
	exit_idler_cb(NULL);
}

static int append_variant(DBusMessageIter *iter, const char *sig, char *param[])
{
	char *ch;
	int i, int_type;

	if (!sig || !param)
		return 0;

	for (ch = (char*)sig, i = 0; *ch != '\0'; ++i, ++ch) {
		switch (*ch) {
		case 's':
			dbus_message_iter_append_basic(iter, DBUS_TYPE_STRING, &param[i]);
			break;
		case 'i':
			int_type = atoi(param[i]);
			dbus_message_iter_append_basic(iter, DBUS_TYPE_INT32, &int_type);
			break;
		default:
			_E("ERROR: %s %c", sig, *ch);
			return -EINVAL;
		}
	}
	return 0;
}

DBusMessage *call_dbus_method(const char *dest, const char *path,
		const char *interface, const char *method,
		const char *sig, char *param[])
{
	DBusConnection *conn;
	DBusMessage *msg = NULL;
	DBusMessageIter iter;
	DBusMessage *ret;
	DBusError err;
	int r;

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
	if (!conn) {
		ret = NULL;
		goto out;
	}

	msg = dbus_message_new_method_call(dest, path, interface, method);
	if (!msg) {
		ret = NULL;
		goto out;
	}

	dbus_message_iter_init_append(msg, &iter);
	r = append_variant(&iter, sig, param);
	if (r < 0) {
		ret = NULL;
		goto out;
	}

	/*This function is for synchronous dbus method call */
	dbus_error_init(&err);
	ret = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_REPLY_TIMEOUT, &err);
	dbus_error_free(&err);

out:
	dbus_message_unref(msg);

	return ret;
}

int call_dbus_method_for_pairs(const char *dest, const char *path,
		const char *interface, const char *method,
		const char *sig, char *param[])
{
	DBusConnection *conn;
	DBusMessage *msg = NULL;
	DBusMessage *reply;
	DBusMessageIter iter, aiter, piter;
	DBusError err;
	int ret, result, i;
	char *key, *value;

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
	if (!conn) {
		ret = 0;
		_E("dbus_bus_get error");
		return -EPERM;
	}

	msg = dbus_message_new_method_call(dest, path, interface, method);
	if (!msg) {
		_E("dbus_message_new_method_call(%s:%s-%s)", path, interface, method);
		return -EBADMSG;
	}

	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{ss}", &aiter);
	for (i = 0 ; i < sizeof(param)/sizeof(param[0]) ; i = i + 2) {
		key = param[i];
		value = param[i+1];
		_I("key(%s), value(%s)", key, value);
		dbus_message_iter_open_container(&aiter, DBUS_TYPE_DICT_ENTRY, NULL, &piter);
		dbus_message_iter_append_basic(&piter, DBUS_TYPE_STRING, &key);
		dbus_message_iter_append_basic(&piter, DBUS_TYPE_STRING, &value);
		dbus_message_iter_close_container(&aiter, &piter);
	}

	dbus_message_iter_close_container(&iter, &aiter);
	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_REPLY_TIMEOUT, &err);
	dbus_message_unref(msg);
	if (!reply) {
		_E("dbus_connection_send error(%s:%s) %s %s:%s-%s", err.name, err.message, dest, path, interface, method);
		dbus_error_free(&err);
		return -ECOMM;
	}

	ret = dbus_message_get_args(reply, &err, DBUS_TYPE_INT32, &result, DBUS_TYPE_INVALID);
	dbus_message_unref(reply);
	if (!ret) {
		_E("no message : [%s:%s] %s %s:%s-%s", err.name, err.message, dest, path, interface, method);
		dbus_error_free(&err);
		return -ENOMSG;
	}

	return result;
}

int request_to_launch_by_dbus(char *bus, char *path, char *iface,
		char *method, char *ptype, char *param[])
{
	DBusMessage *msg;
	DBusError err;
	int i, r, ret_val;

	i = 0;
	do {
		msg = call_dbus_method(bus, path, iface, method, ptype, param);
		if (msg)
			break;
		i++;
	} while (i < RETRY_MAX);
	if (!msg) {
		_E("fail to call dbus method");
		return -ECONNREFUSED;
	}

	dbus_error_init(&err);
	r = dbus_message_get_args(msg, &err, DBUS_TYPE_INT32, &ret_val, DBUS_TYPE_INVALID);
	if (!r) {
		_E("no message : [%s:%s]", err.name, err.message);
		ret_val = -EBADMSG;
	}

	dbus_message_unref(msg);
	dbus_error_free(&err);

	return ret_val;
}


int request_to_launch_by_dbus_for_pairs(char *bus, char *path, char *iface,
		char *method, char *ptype, char *param[])
{
	return (call_dbus_method_for_pairs(bus, path, iface, method, ptype, param));
}

static int send_recovery_popup_signal(void)
{
	char *param[2];

	param[0] = "_SYSPOPUP_CONTENT_";
	param[1] = "recovery";

	return request_to_launch_by_dbus(BUS_NAME, POPUP_PATH_SYSTEM, POPUP_IFACE_SYSTEM,
			"RecoveryPopupLaunch", "ss", param);
}

static int send_usbstorage_unmount_popup_signal(char *path)
{
	char *param[4];
	char buf[BUF_MAX];

	if (!path)
		return -EINVAL;

	snprintf(buf, sizeof(buf), "%s", path);

	param[0] = "_SYSPOPUP_CONTENT_";
	param[1] = "usbotg_unmount_storage";
	param[2] = "_DEVICE_PATH_";
	param[3] = buf;

	return request_to_launch_by_dbus(BUS_NAME, POPUP_PATH_SYSTEM, POPUP_IFACE_SYSTEM,
			"PopupLaunchDouble", "ssss", param);
}

static int send_cooldown_popup_signal(void)
{
	char *param[2];
	int i;

	param[0] = "_SYSPOPUP_CONTENT_";
	param[1] = "cooldown";

	return request_to_launch_by_dbus_for_pairs(BUS_NAME, POPUP_PATH_SYSTEM, POPUP_IFACE_SYSTEM,
			"PopupLaunch", "a{ss}", param);
}

static int get_err_and_space(bundle *b, char *type,
		char *error, int error_len, char *space, int space_len)
{
	char *cErr, *spc;
	int iErr;

	cErr = (char *)bundle_get_val(b, SIGNAL_SENDER_ERROR_TYPE);
	if (!cErr) {
		_E("Failed to get error type");
		return -ENOMEM;
	}

	spc = (char *)bundle_get_val(b, SIGNAL_SENDER_MEMORY_SPACE);
	if (!spc) {
		_E("Failed to get memory space");
		return -ENOMEM;
	}

	iErr = atoi(cErr);
	switch (iErr) {
	case NOT_ENOUGH_SPACE:
		snprintf(error, error_len, "%s_not_enough_space", type);
		break;
	case OPERATION_FAILED:
		snprintf(error, error_len, "%s_operation_failed", type);
		break;
	default:
		_E("Unknown type (%d)", iErr);
		return -EINVAL;
	}

	snprintf(space, space_len, "%s", spc);

	return 0;
}

static int send_ode_error_popup_signal(bundle *b, char *type)
{
	int ret;
	char error[BUF_MAX];
	char space[BUF_MAX];
	char *param[6];

	if (!b || !type)
		return -EINVAL;

	ret = get_err_and_space(b, type,
			error, sizeof(error), space, sizeof(space));
	if (ret < 0)
		return ret;

	param[0] = "_SYSPOPUP_CONTENT_";
	param[1] = "ode_error";
	param[2] = SIGNAL_SENDER_ERROR_TYPE;
	param[3] = error;
	param[4] = SIGNAL_SENDER_MEMORY_SPACE;
	param[5] = space;

	return request_to_launch_by_dbus(BUS_NAME, POPUP_PATH_SYSTEM, POPUP_IFACE_SYSTEM,
			"PopupLaunchTriple", "ssssss", param);
}

static int load_ode_setting_ug(void)
{
	char *setting_ug;
	bundle *b;
	int ret;
	bool restricted = false;

	if (is_storage_encryption_restricted)
		restricted = is_storage_encryption_restricted();

	if (restricted)
		setting_ug = SECURITY_UG;
	else
		setting_ug = MMC_ENCRYPTION_UG;

	b = bundle_create();
	if (!b)
		return -ENOMEM;

	ret = aul_launch_app(setting_ug, b);
	bundle_free(b);
	if (ret <= 0) {
		_E("Failed to launch app (%s)(%d)", setting_ug, ret);
		return ret;
	}

	return 0;
}

static int app_create(void *data)
{
	return 0;
}

static int app_terminate(void *data)
{
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
	char *type;
	char *path;
	int ret;

	if (!b) {
		ret = -EINVAL;
		goto out;
	}

	type = (char *)bundle_get_val(b, SIGNAL_SENDER_TYPE);
	if (!type) {
		_E("FAIL: bundle_get_val()");
		ret = -ENOMEM;
		goto out;
	}

	if (!strncmp(type, SIGNAL_SENDER_TYPE_RECOVERY, strlen(SIGNAL_SENDER_TYPE_RECOVERY))) {
		ret = send_recovery_popup_signal();
		goto out;
	}

	if (!strncmp(type, SIGNAL_SENDER_TYPE_USBSTORAGE_UNMOUNT,
				strlen(SIGNAL_SENDER_TYPE_USBSTORAGE_UNMOUNT))) {
		path = (char *)bundle_get_val(b, "_DEVICE_PATH_");
		if (!path) {
			_E("FAIL: bundle_get_val()");
			ret = -ENOMEM;
			goto out;
		}

		ret = send_usbstorage_unmount_popup_signal(path);
		goto out;
	}

	if (!strncmp(type, SIGNAL_SENDER_TYPE_COOLDOWN, strlen(SIGNAL_SENDER_TYPE_COOLDOWN))) {
		ret = send_cooldown_popup_signal();
		goto out;
	}

	if (!strncmp(type, SIGNAL_SENDER_TYPE_ENCRYPT, strlen(SIGNAL_SENDER_TYPE_ENCRYPT))) {
		ret = send_ode_error_popup_signal(b, type);
		goto out;
	}

	if (!strncmp(type, SIGNAL_SENDER_TYPE_DECRYPT, strlen(SIGNAL_SENDER_TYPE_DECRYPT))) {
		ret = send_ode_error_popup_signal(b, type);
		goto out;
	}

	if (!strncmp(type, SIGNAL_SENDER_TYPE_ODE_UG, strlen(type))) {
		ret = load_ode_setting_ug();
		goto out;
	}

	ret = -EINVAL;

out:
	sender_terminate();
	return ret;
}

int main(int argc, char *argv[])
{
	struct appcore_ops ops = {
		.create = app_create,
		.terminate = app_terminate,
		.pause = app_pause,
		.resume = app_resume,
		.reset = app_reset,
	};

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
