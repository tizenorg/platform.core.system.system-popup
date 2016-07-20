/*
 * popup-launcher
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
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

#include "launcher.h"

#define TERMINATE_TIMEOUT  5

static E_DBus_Connection *edbus_conn;
static DBusPendingCall *edbus_request_name;
static Ecore_Timer *term_timer = NULL;

extern DBusMessage *activate_notification_no_param(E_DBus_Object *obj, DBusMessage *msg, int type);
extern DBusMessage *launch_system_servant_app(E_DBus_Object *obj,	DBusMessage *msg, char **argv);
extern DBusMessage *activate_notification_single_param(E_DBus_Object *obj, DBusMessage *msg, int type);
extern DBusMessage *activate_notification_double_param(E_DBus_Object *obj, DBusMessage *msg, int type);
extern DBusMessage *deactivate_notification(E_DBus_Object *obj, DBusMessage *msg);
extern DBusMessage *update_notification_double_param(E_DBus_Object *obj, DBusMessage *msg, int type);

static Eina_Bool exit_idler_cb(void *data)
{
	e_dbus_connection_close(edbus_conn);
	e_dbus_shutdown();

	ecore_main_loop_quit();
	return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool terminate_launcher(void *data)
{
	if (term_timer)
		ecore_timer_del(term_timer);

	if (ecore_idler_add(exit_idler_cb, NULL))
		return ECORE_CALLBACK_CANCEL;

	exit_idler_cb(NULL);
	return ECORE_CALLBACK_CANCEL;
}

static void set_timer_to_terminate(void)
{
	if (term_timer)
		ecore_timer_reset(term_timer);
	else {
		term_timer = ecore_timer_add(TERMINATE_TIMEOUT, terminate_launcher, NULL);
		if (!term_timer)
			terminate_launcher(NULL);
	}
}

/* Basic popups */
static DBusMessage *system_popup(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return launch_popup(obj, msg, SYSTEM_SYSPOPUP);
}

static const struct edbus_method
dbus_system_methods[] = {
	{ "PopupLaunch", "a{ss}", "i", system_popup },
	/* Add methods here */
};

/* Powerkey popup */
static DBusMessage *powerkey_popup(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return launch_powerkey_popup(obj, msg, POWERKEY_SYSPOPUP);
}

/* Overheat popup */
static DBusMessage *overheat_popup(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return launch_overheat_popup(obj, msg, OVERHEAT_SYSPOPUP);
}

/* Crash popup */
static DBusMessage *crash_popup(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return launch_popup(obj, msg, CRASH_SYSPOPUP);
}

/* Battery notifications */
static DBusMessage *battery_full_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_no_param(obj, msg, BATTERY_FULL);
}

static DBusMessage *battery_charge_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	char param[2];
	char *args[3];

	set_timer_to_terminate();

	args[0] = SERVANT_APP_NAME;
	snprintf(param, sizeof(param), "%d", CHARGER_CONNECTION);
	args[1] = param;
	args[2] = NULL;
	return launch_system_servant_app(obj, msg, args);
}

/* Notification Off */
static DBusMessage *noti_off(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return deactivate_notification(obj, msg);
}

/* LED */
static DBusMessage *led_torch_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_no_param(obj, msg, LED_TORCH);
}

/* USB host notifications */
static DBusMessage *usb_storage_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_single_param(obj, msg, USB_STORAGE);
}

/* Cooldown notification */
static DBusMessage *cooldown_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_no_param(obj, msg, TEMP_COOLDOWN);
}

static DBusMessage *usb_storage_ro_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_single_param(obj, msg, USB_STORAGE_RO);
}

static DBusMessage *usb_device_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_double_param(obj, msg, USB_DEVICE);
}

static DBusMessage *usb_device_noti_update(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return update_notification_double_param(obj, msg, USB_DEVICE);
}

static DBusMessage *media_device_noti_on(E_DBus_Object *obj, DBusMessage *msg)
{
	set_timer_to_terminate();
	return activate_notification_no_param(obj, msg, MEDIA_DEVICE);
}

static const struct edbus_method
dbus_powerkey_methods[] = {
	{ "PopupLaunch", NULL, "i", powerkey_popup },
	/* Add methods here */
};

static const struct edbus_method
dbus_overheat_methods[] = {
	{ "PopupLaunch", "a{ss}", "i", overheat_popup },
	/* Add methods here */
};

static const struct edbus_method
dbus_crash_methods[] = {
	{ "PopupLaunch", "a{ss}", "i", crash_popup	},
	/* Add methods here */
};

static const struct edbus_method
dbus_noti_methods[] = {
	/* LED */
	{ "LedTorchNotiOn"		, NULL		, "i"	, led_torch_noti_on		},
	{ "LedTorhNotiOff"		, "i"		, "i"	, noti_off			},
	/* USB storage */
	{ "UsbStorageNotiOn"		, "s"		, "i"	, usb_storage_noti_on		},
	{ "UsbStorageRoNotiOn"		, "s"		, "i"	, usb_storage_ro_noti_on	},
	{ "UsbStorageNotiOff"		, "i"		, "i"	, noti_off			},
	{ "UsbDeviceNotiOn"		, "ss"		, "i"	, usb_device_noti_on		},
	{ "UsbDeviceNotiUpdate"		, "isss"	, "i"	, usb_device_noti_update	},
	{ "UsbDeviceNotiOff"		, "i"		, "i"	, noti_off			},
	/* usb connection */
	{ "MediaDeviceNotiOn"       	, NULL      	, "i"   , media_device_noti_on      	},
	{ "MediaDeviceNotiOff"      	, "i"       	, "i"   , noti_off                  	},
	/* Battery */
	{ "BatteryFullNotiOn"		, NULL		, "i"	, battery_full_noti_on		},
	{ "BatteryFullNotiOff"		, "i"		, "i"	, noti_off			},
	{ "BatteryChargeNotiOn"		, NULL		, "i"	, battery_charge_noti_on	},
	/* Temperature */
	{ "TempCooldownNotiOn"		, NULL		, "i"	, cooldown_noti_on		},
	{ "TempCooldownNotiOff"		, "i"		, "i"	, noti_off			},
	/* Add notifications here */
};

static struct edbus_object
edbus_objects[] = {
	{ POPUP_PATH_SYSTEM		, POPUP_IFACE_SYSTEM	, NULL	, NULL	,
		dbus_system_methods	, ARRAY_SIZE(dbus_system_methods)		},
	{ POPUP_PATH_POWERKEY		, POPUP_IFACE_POWERKEY	, NULL	, NULL	,
		dbus_powerkey_methods	, ARRAY_SIZE(dbus_powerkey_methods)		},
	{ POPUP_PATH_OVERHEAT		, POPUP_IFACE_OVERHEAT	, NULL	, NULL	,
		dbus_overheat_methods	, ARRAY_SIZE(dbus_overheat_methods)		},
	{ POPUP_PATH_NOTI		, POPUP_IFACE_NOTI	, NULL	, NULL	,
		dbus_noti_methods	, ARRAY_SIZE(dbus_noti_methods)			},
	{ POPUP_PATH_CRASH		, POPUP_IFACE_CRASH	, NULL	, NULL	,
		dbus_crash_methods	, ARRAY_SIZE(dbus_crash_methods)		},
	/* Add new object & interface here*/
};

static int init_methods(void)
{
	int ret;
	int i, j;


	for (i = 0; i < ARRAY_SIZE(edbus_objects); i++) {
		for (j = 0; j < edbus_objects[i].methods_len; j++) {
			ret = e_dbus_interface_method_add(edbus_objects[i].iface,
					edbus_objects[i].methods[j].member,
					edbus_objects[i].methods[j].signature,
					edbus_objects[i].methods[j].reply_signature,
					edbus_objects[i].methods[j].func);
			if (!ret) {
				_E("fail to add method %s!", edbus_objects[i].methods[j].member);
				return -ECONNREFUSED;
			}
		}
	}
	return 0;
}

static int register_dbus(void)
{
	DBusError error;
	int retry, ret, i;

	dbus_error_init(&error);

	retry = 0;
	do {
		if (e_dbus_init())
			break;
		if (++retry == RETRY_MAX) {
			_E("fail to init edbus");
			return -ECONNREFUSED;
		}
	} while (retry <= RETRY_MAX);

	retry = 0;
	do {
		edbus_conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
		if (edbus_conn)
			break;
		if (++retry == RETRY_MAX) {
			_E("fail to get edbus");
			ret = -ECONNREFUSED;
			goto out1;
		}
	} while (retry <= RETRY_MAX);

	retry = 0;
	do {
		edbus_request_name = e_dbus_request_name(edbus_conn, BUS_NAME, 0, NULL, NULL);
		if (edbus_request_name)
			break;
		if (++retry == RETRY_MAX) {
			_E("fail to request edbus name");
			ret = -ECONNREFUSED;
			goto out2;
		}
	} while (retry <= RETRY_MAX);

	for (i = 0; i < ARRAY_SIZE(edbus_objects); i++) {
		edbus_objects[i].obj = e_dbus_object_add(edbus_conn, edbus_objects[i].path, NULL);
		if (!(edbus_objects[i].obj)) {
			_E("fail to add edbus obj");
			ret = -ECONNREFUSED;
			goto out2;
		}

		edbus_objects[i].iface = e_dbus_interface_new(edbus_objects[i].interface);
		if (!(edbus_objects[i].iface)) {
			_E("fail to add edbus interface");
			ret = -ECONNREFUSED;
			goto out2;
		}

		e_dbus_object_interface_attach(edbus_objects[i].obj, edbus_objects[i].iface);
	}

	return 0;

out2:
	e_dbus_connection_close(edbus_conn);
out1:
	e_dbus_shutdown();

	return ret;
}

int main(int argc, char *argv[])
{
	int ret;

	ecore_init();

	ret = register_dbus();
	if (ret < 0)
		return ret;

	ret = init_methods();
	if (ret < 0)
		return ret;

	ecore_main_loop_begin();
	ecore_shutdown();
	return 0;
}
