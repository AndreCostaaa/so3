/*
 * Copyright (C) 2025 André Costa <andre_miguel_costa@hotmail.com>
 * Copyright (C) 2020 Nikolaos Garanis <nikolaos.garanis@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <unistd.h>
#include <bits/ioctl.h>
#include <fcntl.h>
#include <stdio.h>

#include "slv_keyboard.h"

#include <lvgl.h>

static void slv_keyboard_cb(lv_indev_t *indev, lv_indev_data_t *data);

int slv_keyboard_init(void)
{
	int kfd = open(KB_DEV, 0);
	if (kfd < 0) {
		printf("Couldn't open input device.\n");
		return -1;
	}

	/**
	 * Initialisation of the keyboard input driver.
	 */
	lv_indev_t *keyboard = lv_indev_create();
	lv_indev_set_type(keyboard, LV_INDEV_TYPE_KEYPAD);
	lv_indev_set_read_cb(keyboard, slv_keyboard_cb);
	lv_indev_set_user_data(keyboard, (void *)(ssize_t)kfd);

	return kfd;
}

void slv_keyboard_terminate(int kfd)
{
	if (kfd > 0) {
		close(kfd);
	}
}

static void slv_keyboard_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
	/* Retrieve mouse state from the driver. */
	static struct ps2_key key;
	int kfd = (int)(ssize_t)lv_indev_get_user_data(indev);

	ioctl(kfd, IOCTL_KB_GET_KEY, &key);

	if (key.value != 0) {
		data->key = key.value;
		data->state = key.state & 1;
	}
}
