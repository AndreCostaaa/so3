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

#ifndef SLV_FB_H
#define SLV_FB_H

#include <stdint.h>

/* Framebuffer ioctl commands. */
#define IOCTL_FB_HRES 1
#define IOCTL_FB_VRES 2
#define IOCTL_FB_SIZE 3

#define FB_DEV	      "/dev/fb"

typedef struct {
	uint32_t hres, vres;
	void *priv;
} slv_fb_t;

int slv_fb_init(slv_fb_t *data);
void slv_fb_terminate(slv_fb_t *data);
#endif
