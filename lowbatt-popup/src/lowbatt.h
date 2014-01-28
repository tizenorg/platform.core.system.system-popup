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


#ifndef __DEF_lowbatt_H_
#define __DEF_lowbatt_H_

#include <Elementary.h>

#ifndef PREFIX
#define PREFIX "/usr"
#endif /* PREFIX */

#define TEMP_DIR			"/tmp"
#define PACKAGE				"lowbatt-popup"
#define APPNAME				"lowbatt-popup"
#define MAIN_W				(480)
#define MAIN_H				(800)
#define EDJ_PATH			PREFIX"/apps/org.tizen.lowbat-syspopup/res/edje/lowbatt"
#define EDJ_NAME			EDJ_PATH"/lowbatt.edj"
#define GRP_MAIN			"main"
#define GRP_POPUP			"popup"
#define NEW_INDI
#define APPLICATION_BG			1
#define INDICATOR_HEIGHT		(38)

#ifndef PREDEF_POWEROFF
#define PREDEF_POWEROFF			"poweroff"
#endif /* PREFEF_POWEROFF */

#define BEAT

/* Acct profiling support */
#define  ACCT_PROF
#ifdef   ACCT_PROF
#include <sys/acct.h>
#endif /* ACCT_PROF */

#endif				/* __DEF_lowbatt_H__ */
