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

#include "unistd.h"
#include <sys/mman.h>
#include <bits/ioctl.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lvgl.h>

#include "slv_fb.h"

typedef struct {
	int fd;
	void *fbp;
	size_t fb_size;
	bool is_real;
} slv_fb_priv_t;

static void my_fb_cb(lv_display_t *disp, const lv_area_t *area,
		     uint8_t *px_map);

static void dummy_fb_cb(lv_display_t *disp, const lv_area_t *area,
			uint8_t *px_map);

int slv_fb_init(slv_fb_t *fb)
{
	fb->priv = calloc(1, sizeof(slv_fb_priv_t));
	if (!fb->priv) {
		printf("Couldn't allocate fb data\n");
		return -1;
	}

	slv_fb_priv_t *priv = (slv_fb_priv_t *)fb->priv;

	/* Get file descriptor. */
	priv->fd = open(FB_DEV, 0);
	if (priv->fd < 0) {
		printf("Couldn't open framebuffer.\n");
		return -1;
	}

	/* Get screen resolution. */
	if (ioctl(priv->fd, IOCTL_FB_HRES, &fb->hres) ||
	    ioctl(priv->fd, IOCTL_FB_VRES, &fb->vres) ||
	    ioctl(priv->fd, IOCTL_FB_SIZE, &priv->fb_size)) {
		printf("Couldn't get framebuffer resolution.\n");
		return -1;
	}

	/* 
	 * We need to check if we're dealing with a real framebuffer or not
	 * so we know if we can use mmap
	 */
	int is_real;
	if (ioctl(priv->fd, IOCTL_FB_IS_REAL, &is_real)) {
		/* Only a fake framebuffer defines this ioctl */
		priv->is_real = true;
	} else {
		priv->is_real = is_real != 0;
	}

	uint8_t *buf = malloc(priv->fb_size);
	if (!buf) {
		printf("Couldn't allocate draw buffer\n");
		return -1;
	}

	/* 
	 * Unfortunately, there's not really a good way to
	 * allocate memory for user-space from kernel space as
	 * the prefix of the virtual addresses will not be the same 
	 * for 64 bit architectures
	 */
	if (priv->is_real) {
		/* Map the framebuffer into process memory. */
		priv->fbp = mmap(NULL, priv->fb_size, 0, 0, priv->fd, 0);
		if (priv->fbp == MAP_FAILED) {
			printf("Couldn't map framebuffer.\n");
			return -1;
		}
	}

	/*
	 * Initialisation and registration of the display driver.
	 * Also setting the flush callback function (flush_cb) which will write
	 * the lvgl buffer (buf) into our real framebuffer.
	 */
	lv_display_t *disp = lv_display_create(fb->hres, fb->vres);
	lv_display_set_buffers(disp, buf, NULL, priv->fb_size,
			       LV_DISPLAY_RENDER_MODE_DIRECT);

	const lv_display_flush_cb_t cb = priv->is_real ? my_fb_cb : dummy_fb_cb;
	lv_display_set_flush_cb(disp, cb);
	lv_display_set_user_data(disp, priv);

	return 0;
}
void slv_fb_terminate(slv_fb_t *data)
{
	slv_fb_priv_t *priv = (slv_fb_priv_t *)data->priv;
	if (priv->fd > 0) {
		close(priv->fd);
	}
	free(data->priv);
}

/*
 * Framebuffer callback. LVGL calls this function to redraw a screen area. If
 * the buffer given to LVGL is smaller than the framebuffer, this function will
 * be called multiple times until the whole screen has been redrawn.
 *
 * https://docs.lvgl.io/9.2/porting/display.html
 */
static void my_fb_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
	slv_fb_priv_t *priv = (slv_fb_priv_t *)lv_display_get_user_data(disp);
	memcpy(priv->fbp, px_map, priv->fb_size);
	lv_display_flush_ready(disp);
}
static void dummy_fb_cb(lv_display_t *disp, const lv_area_t *area,
			uint8_t *px_map)
{
	lv_display_flush_ready(disp);
}
