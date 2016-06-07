/*
 *  system-cooperator
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

#include <stdio.h>
#include <unistd.h>
#include <appcore-efl.h>
#include <Ecore.h>
#include <glib.h>
#include <dlog.h>
#include <wav_player.h>
#include <sound_manager.h>
#include <vconf.h>
#include <feedback.h>
#include <appcore-common.h>

#undef LOG_TAG
#define LOG_TAG "SYSTEM_APPS"
#define _D(fmt, args...)   SLOGD(fmt, ##args)
#define _E(fmt, args...)   SLOGE(fmt, ##args)
#define _I(fmt, args...)   SLOGI(fmt, ##args)

#define CHARGER_TIMEOUT        2000 /* Milliseconds */

#define CHARGER_CONNECTION_PATH "/usr/share/feedback/sound/operation/operation.wav"

enum service_type {
	CHARGER_CONNECTION,
	SERVICE_MAX,
};

enum loop_type {
	LOOP_ERROR,
	LOOP_G_MAIN_LOOP,
};

enum sound_status {
	SOUND,
	VIBRATION,
	MUTE,
};

static GMainLoop *loop = NULL;
static int loop_type = LOOP_ERROR;

static void quit_main_loop(ltype)
{
	switch (ltype) {
	case LOOP_G_MAIN_LOOP:
		g_main_loop_quit(loop);
		break;
	default:
		break;
	}
}

static int get_sound_state(void)
{
	int state;

	if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &state) == 0
			&& state == 1)
		return SOUND;

	if (vconf_get_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, &state) == 0
			&& state == 1)
		return VIBRATION;

	return MUTE;
}

static bool get_call_state(void)
{
	int state;
	if (vconf_get_int(VCONFKEY_CALL_STATE, &state) == 0
			&& state != VCONFKEY_CALL_OFF)
		return true;
	return false;
}

static bool get_voice_recording_state(void)
{
	int state;
	if (vconf_get_int(VCONFKEY_RECORDER_STATE, &state) == 0
			&& (state == VCONFKEY_RECORDER_STATE_RECORDING))
		return true;
	return false;
}

static void play_completed(int id, void *data)
{
	quit_main_loop(loop_type);
}

static gboolean time_expired(gpointer data)
{
	quit_main_loop(loop_type);
	return FALSE;
}

static void play_notification_sound(void)
{
	int ret;
	static bool session = false;

	if (access(CHARGER_CONNECTION_PATH, F_OK) != 0) {
		_E("The sound file does not exist (%d)", CHARGER_CONNECTION_PATH);
		return;
	}

	if (!session) {
		ret = sound_manager_set_session_type(SOUND_SESSION_TYPE_NOTIFICATION);
		if (ret != SOUND_MANAGER_ERROR_NONE) {
			_E("Failed to set session to play sound(%d)", ret);
			return;
		}
	}

	ret = wav_player_start(CHARGER_CONNECTION_PATH, SOUND_TYPE_NOTIFICATION, play_completed, NULL, NULL);
	if (ret != WAV_PLAYER_ERROR_NONE)
		_E("Failed to play sound file (%d, %s)", ret, CHARGER_CONNECTION_PATH);
}

static void play_feedback(int pattern)
{
	feedback_initialize();
	feedback_play_type(FEEDBACK_TYPE_VIBRATION, pattern);
	feedback_deinitialize();
}

static int play_battery_sound(void)
{
	int sound;

	sound = get_sound_state();
	switch (sound) {
	case SOUND:
		if (!get_call_state() && !get_voice_recording_state()) {
			play_notification_sound();
			break;
		}
	case VIBRATION:
		play_feedback(FEEDBACK_PATTERN_CHARGERCONN);
		break;
	case MUTE:
	default:
		break;
	}
	g_timeout_add(CHARGER_TIMEOUT, time_expired, NULL);
	return 0;
}

static int app_create(int type)
{
	switch (type) {
	case CHARGER_CONNECTION:
		return play_battery_sound();
	default:
		_E("Unknown type(%d)", type);
		return -EINVAL;
	}
}

static int app_terminate(int type)
{
	switch (type) {
	case CHARGER_CONNECTION:
		return 0;
	default:
		_E("Unknown type(%d)", type);
		return -EINVAL;
	}
}

static int get_main_loop_type(int type)
{
	switch (type) {
	case CHARGER_CONNECTION:
		return LOOP_G_MAIN_LOOP;
	default:
		return LOOP_ERROR;
	}
}

static int init_main_loop(int ltype)
{
	switch (ltype) {
	case LOOP_G_MAIN_LOOP:
		loop = g_main_loop_new(NULL, TRUE);
		if (!loop)
			return -ENOMEM;
		return 0;
	default:
		return -EINVAL;
	}
}

static void start_main_loop(int ltype)
{
	switch (ltype) {
	case LOOP_G_MAIN_LOOP:
		g_main_loop_run(loop);
		break;
	default:
		break;
	}
}

static void finalize_main_loop(int ltype)
{
	switch (ltype) {
	case LOOP_G_MAIN_LOOP:
		g_main_loop_unref(loop);
		break;
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	int ret, type;

	ret = appcore_set_i18n(LANG_DOMAIN, LOCALE_DIR);
	if (ret != 0)
		_E("FAIL: appcore_set_i18n()");

	if (argc <= 1)
		return 0;

	type = atoi(argv[1]);
	loop_type = get_main_loop_type(type);

	ret = init_main_loop(loop_type);
	if (ret < 0) {
		_E("Failed to init main loop (%d)", ret);
		return ret;
	}

	ret = app_create(type);
	if (ret < 0) {
		_E("Failed to register handlers");
		goto out;
	}

	start_main_loop(loop_type);

out:
	ret = app_terminate(type);
	if (ret < 0)
		_E("Failed to release handlers(%d)", ret);

	finalize_main_loop(loop_type);

	return 0;
}
