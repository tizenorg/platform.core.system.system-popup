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


#ifndef __DEF_poweroff_H_
#define __DEF_poweroff_H_

#include <Elementary.h>
#include <bundle.h>

#ifndef PREFIX
#define PREFIX "/usr"
#endif
#define PACKAGE "poweroff-popup"
#define APPNAME "poweroff-popup"
#define EDJ_PATH PREFIX"/apps/org.tizen.poweroff-syspopup/res/edje/poweroff"
#define EDJ_NAME EDJ_PATH"/poweroff.edj"
#define GRP_MAIN "main"
#define APPLICATION_BG	1
#define INDICATOR_HEIGHT (38)
#define NEW_INDI
#define ACCT_PROF
#define PREDEF_POWEROFF "poweroff"
#define PREDEF_ENTERSLEEP "entersleep"
#define MAIN_W	(480)
#define MAIN_H	(800)
#define BEAT

#define NAME_BUF_LEN	128
#define TITLE_BUF_LEN	128
#define CONTENT_BUF_LEN	256

#endif				/* __DEF_poweroff_H__ */
