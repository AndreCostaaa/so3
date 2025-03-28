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

#include <fcntl.h>
#include <unistd.h>
#include <bits/ioctl.h>
#include <lvgl.h>
#include <stdio.h>

#include "slv_mouse.h"

static void slv_mouse_cb(lv_indev_t *indev, lv_indev_data_t *data);

int slv_mouse_init(uint16_t h, uint16_t v)
{
	ssize_t mfd = open(MOUSE_DEV, 0);
	if (mfd < 0) {
		printf("Couldn't open input device.\n");
		return -1;
	}

	/*
	 * Informing the driver of the screen size so it knows the bounds for
	 * the cursor coordinates.
	 */
	struct display_res res = { .h = h, .v = v };

	ioctl(mfd, IOCTL_MOUSE_SET_RES, &res);

	/*
	 * Initialisation of an input driver, in our case for a mouse. Also set
	 * a cursor image. We also set a callback function which will be called
	 * by lvgl periodically. This function queries the mouse driver for the
	 * xy coordinates and button states.
	 */

	lv_indev_t *mouse = lv_indev_create();
	lv_indev_set_type(mouse, LV_INDEV_TYPE_POINTER);
	lv_indev_set_read_cb(mouse, slv_mouse_cb);
	lv_indev_set_user_data(mouse, (void *)(ssize_t)mfd);

	lv_obj_t *cursor_obj =
		lv_image_create(lv_display_get_screen_active(NULL));
	lv_image_set_src(cursor_obj, LV_SYMBOL_PLUS);
	lv_indev_set_cursor(mouse, cursor_obj);

	return mfd;
}

void slv_mouse_terminate(int fd)
{
	if (fd > 0) {
		close(fd);
	}
}

static void slv_mouse_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
	/* Retrieve mouse state from the driver. */
	static struct ps2_mouse mouse;
	int mfd = (int)(ssize_t)lv_indev_get_user_data(indev);

	ioctl(mfd, IOCTL_MOUSE_GET_STATE, &mouse);

	data->state = mouse.left ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;

	data->point.x = mouse.x;
	data->point.y = mouse.y;
}
