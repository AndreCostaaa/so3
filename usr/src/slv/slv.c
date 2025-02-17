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

#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <bits/ioctl.h>
#include <sys/mman.h>

#include <lvgl.h>

#include "slv.h"
#include "slv_fb.h"
#include "slv_keyboard.h"
#include "slv_fs.h"
#include "slv_mouse.h"

static void *slv_tick(void *args);
static void *slv_loop_inner(void *args);

int slv_init(slv_t *slv)
{
	lv_init();
	slv_fs_init();
	int err = slv_fb_init(&slv->fb);
	if (err < 0) {
		goto fb_err;
	}
	slv->kfd = slv_keyboard_init();
	if (slv->kfd < 0) {
		err = slv->kfd;
		goto kb_err;
	}
	slv->mfd = slv_mouse_init(slv->fb.hres, slv->fb.vres);
	if (slv->mfd < 0) {
		err = slv->mfd;
		goto mouse_err;
	}

	pthread_t thread;
	err = pthread_create(&thread, NULL, slv_tick, NULL);
	if (err < 0) {
		goto thread_err;
	}

	return 0;

fb_err:
	lv_deinit();
kb_err:
	slv_fb_terminate(&slv->fb);
mouse_err:
	slv_keyboard_terminate(slv->kfd);
thread_err:
	slv_mouse_terminate(slv->mfd);
	return err;
}

void slv_loop(void)
{
	slv_loop_inner(NULL);
}

int slv_loop_start(void)
{
	pthread_t thread;
	return pthread_create(&thread, NULL, slv_loop_inner, NULL);
}

void slv_terminate(slv_t *slv)
{
	lv_deinit();
	slv_fs_terminate();
	slv_fb_terminate(&slv->fb);
	slv_keyboard_terminate(slv->kfd);
	slv_mouse_terminate(slv->mfd);
}
static void *slv_loop_inner(void *args)
{
	(void)args;
	while (1) {
		lv_task_handler();
		usleep(5000);
	}
	return NULL;
}

static void *slv_tick(void *args)
{
	(void)args;
	while (1) {
		/* Tell LVGL that 1 millisecond were elapsed */
		usleep(1000);
		lv_tick_inc(1);
	}
	return NULL;
}
