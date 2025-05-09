/*
 * Copyright (C) 2014-2019 Daniel Rossier <daniel.rossier@heig-vd.ch>
 * Copyright (C) 2017 Alexandre Malki <alexandre.malki@heig-vd.ch>
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

#include <common.h>
#include <string.h>
#include <signal.h>
#include <mutex.h>
#include <process.h>

#ifdef CONFIG_SO3VIRT
#include <avz/uapi/avz.h>
#endif

#include <device/irq.h>
#include <device/serial.h>

#include <asm/processor.h>
#include <asm/processor.h>

serial_ops_t serial_ops;
mutex_t read_lock;
tcb_t *tcb_owner;

#ifdef CONFIG_SO3VIRT
extern void (*__printch)(char c);
#endif /* CONFIG_SO3VIRT */

/* Outputs an ASCII string to console */
int serial_putc(char c)
{
	if (serial_ops.put_byte == NULL) {
		__ll_put_byte(c);

		return 1;
	} else
		return serial_ops.put_byte(c);
}

/* Get a byte from the UART. */
char serial_getc(void)
{
	char c;

#ifdef CONFIG_SO3VIRT
	static char *input_str[2] = { "Agency domain", "Agency AVZ Hypervisor" };
	static bool focus_on_avz = false;
#endif

	/* This function will get a byte but first take try to
	 * take the lock to access the uart */

	mutex_lock(&read_lock);

	/* Keep a reference to the waiting thread which owns the serial device temporary. */
	tcb_owner = current();

	c = serial_ops.get_byte(false);

	mutex_unlock(&read_lock);

#ifdef CONFIG_SO3VIRT

	if (c == 1) /* Ctrl+a has been pressed twice */
	{
		focus_on_avz = !focus_on_avz;

		lprintk("*** Serial input -> %s (type 'CTRL-%c' twice to switch input to %s).\n",
			input_str[(focus_on_avz ? 1 : 0)], 'a', input_str[(focus_on_avz ? 0 : 1)]);

		return 0;
	}

	if (focus_on_avz) {
		hypercall_trampoline(__HYPERVISOR_console_io, CONSOLEIO_process_char, 1, (long) &c, 0);

		return 0;
	}

#endif

	return c;
}

/*
 * Emergency printk() or very early printk() before the real uart driver is ready.
 * It may be used in the context of the virtualized ME SO3 as well.
 */
int ll_serial_write(char *str, int len)
{
	int i;

	for (i = 0; i < len; i++)
		if (str[i] != 0)
#ifdef CONFIG_SO3VIRT
			__printch(str[i]);
#else
			__ll_put_byte(str[i]);
#endif
	return len;
}

/* Sends some bytes to the UART */
int serial_write(char *str, int len)
{
	int i;
	unsigned long flags;

	/* Here, we disable IRQ since printk() can also be used with IRQs off */
	flags = local_irq_save();

	for (i = 0; i < len; i++)
		if (str[i] != 0)
#ifdef CONFIG_SO3VIRT
			__printch(str[i]);
#else
			serial_putc(str[i]);
#endif

	local_irq_restore(flags);

	return len;
}

/* This function will query the size of the serial terminal*/
int serial_gwinsize(struct winsize *wsz)
{
	/*
	 * The following code strongly depends on a patched version of QEMU which reacts
	 * to the '\254' ASCII code. When the emulated UART interface get this code, qemu
	 * queries the host via ioctl(stdout, TIOCGWINSZ) to retrieve the window size.
	 */

	/* We want to reserve the access to the uart read. */

	/* Prevent an interrupt on the UART generated by
	 * QEMU to read the two bytes. So far, we do not
	 * manage an internal buffer to read many chars.
	 */

#if defined(CONFIG_VIRT32) && !defined(CONFIG_SOO)

	serial_ops.disable_irq();

	if (serial_write(SERIAL_GWINSZ, 1) == 0)
		return -1;

	wsz->ws_row = serial_ops.get_byte(true);
	wsz->ws_col = serial_ops.get_byte(true);

	serial_ops.enable_irq();

#else

	wsz->ws_row = WINSIZE_ROW_SIZE_DEFAULT;
	wsz->ws_col = WINSIZE_COL_SIZE_DEFAULT;

#endif

	return 0;
}

#ifdef CONFIG_VIRT32
/*
 * Send a specific char code to Qemu to ask it to exit immediately.
 */
void send_qemu_halt(void)
{
	serial_write(SERIAL_SO3_HALT, 1);
}

#endif

/*
 * Release the lock if a process/thread is finishing during a read.
 */
void serial_cleanup(void)
{
	/* Check if the lock is owned by a thread which is currently terminated (via ctrl/c) or
	 * killed. In this case only, and if the thread is locking the serial device, the lock must be released.
	 */
	if ((current() == tcb_owner) && mutex_is_locked(&read_lock))
		mutex_unlock(&read_lock);
}

/*
 * Main initialization function of the UART device
 */
void serial_init(void)
{
	memset(&serial_ops, 0, sizeof(serial_ops_t));

	mutex_init(&read_lock);
}
