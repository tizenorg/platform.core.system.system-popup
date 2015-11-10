/*
 * popup-launcher
 *
 * Copyright (c) 2013 Samsung Electronics Co., Ltd. All rights reserved.
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

#ifndef __LAUNCHER_H__
#define __LAUNCHER_H__

#include <stdio.h>
#include <bundle_internal.h>
#include <E_DBus.h>
#include <Ecore.h>
#include <Elementary.h>
#include <appcore-common.h>
#include <aul.h>
#include "macro.h"

/* Popup names */
#define POWEROFF_SYSPOPUP  "poweroff-syspopup"
#define SYSTEM_SYSPOPUP    "system-syspopup"

struct edbus_method {
	const char *member;
	const char *signature;
	const char *reply_signature;
	E_DBus_Method_Cb func;
	int type;
};

struct edbus_object {
	const char *path;
	const char *interface;
	E_DBus_Object *obj;
	E_DBus_Interface *iface;
	const struct edbus_method *methods;
	const int methods_len;
};

/* launch popup */
DBusMessage *launch_popup(E_DBus_Object *obj,
				DBusMessage *msg, char *name);
DBusMessage *launch_poweroff_popup(E_DBus_Object *obj,
				DBusMessage *msg, char *name);

#endif /* __LAUNCHER_H__ */

