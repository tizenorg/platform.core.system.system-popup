/*
 *  system-popup
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

#include "popup-common.h"

static const struct popup_ops cooldown_poweroff_ops = {
	.name		= "cooldown_poweroff",
	.show		= load_simple_popup,
	.title		= "IDS_ST_HEADER_POWER_OFF_ABB",
	.content	= "IDS_QP_POP_YOUR_DEVICE_IS_OVERHEATING_IT_WILL_NOW_POWER_OFF_TO_COOL_DOWN",
	.left_text	= "IDS_QP_BUTTON_DO_NOT_POWER_OFF_ABB",
};

static const struct popup_ops cooldown_ops = {
	.name		= "cooldown",
	.show		= load_simple_popup,
	.title		= "IDS_IDLE_HEADER_PHONE_COOLING_DOWN_ABB",
	.content	= "IDS_IDLE_POP_YOUR_PHONE_IS_OVERHEATING_STEPS_WILL_BE_TAKEN_TO_COOL_IT_DOWN_N_NYOU_CAN_STILL_USE_YOUR_PHONE_MSG",
	.left_text	= "IDS_COM_SK_OK",
};

/* Constructor to register cooldown button */
static __attribute__ ((constructor)) void cooldown_register_popup(void)
{
	register_popup(&cooldown_poweroff_ops);
	register_popup(&cooldown_ops);
}
