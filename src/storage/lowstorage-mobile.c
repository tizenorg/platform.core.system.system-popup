/*
 *  system-popup
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd. All rights reserved.
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

static const struct popup_ops lowstorage_warning_ops;
static const struct popup_ops lowstorage_critical_ops;

static int remove_other_lowstorage_popups(bundle *b, const struct popup_ops *ops)
{
	_D("remove_other_lowstorage_popups() is entered");

	if (ops != &lowstorage_warning_ops)
		unload_simple_popup(&lowstorage_warning_ops);

	if (ops != &lowstorage_critical_ops)
		unload_simple_popup(&lowstorage_critical_ops);
	
	_D("remove_other_lowstorage_popups() is finished");
	return 0;
}

static const struct popup_ops lowstorage_warning_ops = {
	.name		= "lowstorage_warning",
	.show		= load_simple_popup,
	.content	= "IDS_DAV_BODY_LOW_MEMORY_LEFT_ORANGE",
	.left_text	= "IDS_COM_SK_OK",
	.pre		= remove_other_lowstorage_popups,
};

static const struct popup_ops lowstorage_critical_ops = {
	.name		= "lowstorage_critical",
	.show		= load_simple_popup,
	.content	= "IDS_DAV_BODY_LOW_MEMORY_LEFT_ORANGE",
	.left_text	= "IDS_COM_SK_OK",
	.pre		= remove_other_lowstorage_popups,
};

static __attribute__ ((constructor)) void lowstorage_register_popup(void)
{
	register_popup(&lowstorage_warning_ops);
	register_popup(&lowstorage_critical_ops);
}
