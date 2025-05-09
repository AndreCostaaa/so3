/*
 * Copyright (C) 2020 Nikolaos Garanis <nikolaos.garanis@heig-vd.ch>
 * Copyright (C) 2021 Daniel Rossier <daniel.rossier@heig-vd.ch>
 * Copyright (C) 2025 André Costa <andre_miguel_costa@hotmail.com>
 *
 *
 * With the kind support and contribution of Gabor Kiss-Vamosi from LVGL. Thank You!
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

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <lvgl.h>
#include <demos/lv_demos.h>

#include <slv.h>

int main(int argc, char **argv)
{
	printf("LVGL Performance test\n");
	struct timeval tv_start, tv_end;

	/* Initialization of lvgl. */
	slv_t slv;
	int err = slv_init(&slv);
	if (err < 0) {
		printf("Couldn't init SVL\n");
		return EXIT_FAILURE;
	}

	/* Creating the UI. */
	lv_demo_stress();

	gettimeofday(&tv_start, NULL);
	lv_timer_handler();
	gettimeofday(&tv_end, NULL);

	uint64_t delta = tv_end.tv_usec - tv_start.tv_usec;

	printf("Performance test result:\n\n");
	printf("# Elapsed time of lv_timer_handler() function: %lld microseconds.\n", delta);
	printf("\n***************************************************************************\n");

	slv_terminate(&slv);

	return EXIT_SUCCESS;
}
