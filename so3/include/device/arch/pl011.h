/*
 * Copyright (C) 2014-2017 Daniel Rossier <daniel.rossier@heig-vd.ch>
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

#ifndef PL011_H
#define PL011_H

/* -------------------------------------------------------------------------------
 *  From AMBA UART (PL010) Block Specification
 * -------------------------------------------------------------------------------
 *  UART Register Offsets.
 */
#define UART01x_DR 0x00 /* Data read or written from the interface. */
#define UART01x_RSR 0x04 /* Receive status register (Read). */
#define UART01x_ECR 0x04 /* Error clear register (Write). */
#define UART010_LCRH 0x08 /* Line control register, high byte. */
#define UART010_LCRM 0x0C /* Line control register, middle byte. */
#define UART010_LCRL 0x10 /* Line control register, low byte. */
#define UART010_CR 0x14 /* Control register. */
#define UART01x_FR 0x18 /* Flag register (Read only). */
#define UART010_IIR 0x1C /* Interrupt identification register (Read). */
#define UART010_ICR 0x1C /* Interrupt clear register (Write). */
#define UART01x_ILPR 0x20 /* IrDA low power counter register. */
#define UART011_IBRD 0x24 /* Integer baud rate divisor register. */
#define UART011_FBRD 0x28 /* Fractional baud rate divisor register. */
#define UART011_LCRH 0x2c /* Line control register. */
#define UART011_CR 0x30 /* Control register. */
#define UART011_IFLS 0x34 /* Interrupt fifo level select. */
#define UART011_IMSC 0x38 /* Interrupt mask. */
#define UART011_RIS 0x3c /* Raw interrupt status. */
#define UART011_MIS 0x40 /* Masked interrupt status. */
#define UART011_ICR 0x44 /* Interrupt clear register. */
#define UART011_DMACR 0x48 /* DMA control register. */

#define UART011_DR_OE (1 << 11)
#define UART011_DR_BE (1 << 10)
#define UART011_DR_PE (1 << 9)
#define UART011_DR_FE (1 << 8)

#define UART01x_RSR_OE 0x08
#define UART01x_RSR_BE 0x04
#define UART01x_RSR_PE 0x02
#define UART01x_RSR_FE 0x01

#define UART011_FR_RI 0x100
#define UART011_FR_TXFE 0x080
#define UART011_FR_RXFF 0x040
#define UART01x_FR_TXFF 0x020
#define UART01x_FR_RXFE 0x010
#define UART01x_FR_BUSY 0x008
#define UART01x_FR_DCD 0x004
#define UART01x_FR_DSR 0x002
#define UART01x_FR_CTS 0x001
#define UART01x_FR_TMSK (UART01x_FR_TXFF + UART01x_FR_BUSY)

#define UART011_CR_CTSEN 0x8000 /* CTS hardware flow control */
#define UART011_CR_RTSEN 0x4000 /* RTS hardware flow control */
#define UART011_CR_OUT2 0x2000 /* OUT2 */
#define UART011_CR_OUT1 0x1000 /* OUT1 */
#define UART011_CR_RTS 0x0800 /* RTS */
#define UART011_CR_DTR 0x0400 /* DTR */
#define UART011_CR_RXE 0x0200 /* receive enable */
#define UART011_CR_TXE 0x0100 /* transmit enable */
#define UART011_CR_LBE 0x0080 /* loopback enable */
#define UART010_CR_RTIE 0x0040
#define UART010_CR_TIE 0x0020
#define UART010_CR_RIE 0x0010
#define UART010_CR_MSIE 0x0008
#define UART01x_CR_IIRLP 0x0004 /* SIR low power mode */
#define UART01x_CR_SIREN 0x0002 /* SIR enable */
#define UART01x_CR_UARTEN 0x0001 /* UART enable */

#define UART011_LCRH_SPS 0x80
#define UART01x_LCRH_WLEN_8 0x60
#define UART01x_LCRH_WLEN_7 0x40
#define UART01x_LCRH_WLEN_6 0x20
#define UART01x_LCRH_WLEN_5 0x00
#define UART01x_LCRH_FEN 0x10
#define UART01x_LCRH_STP2 0x08
#define UART01x_LCRH_EPS 0x04
#define UART01x_LCRH_PEN 0x02
#define UART01x_LCRH_BRK 0x01

#define UART010_IIR_RTIS 0x08
#define UART010_IIR_TIS 0x04
#define UART010_IIR_RIS 0x02
#define UART010_IIR_MIS 0x01

#define UART011_IFLS_RX1_8 (0 << 3)
#define UART011_IFLS_RX2_8 (1 << 3)
#define UART011_IFLS_RX4_8 (2 << 3)
#define UART011_IFLS_RX6_8 (3 << 3)
#define UART011_IFLS_RX7_8 (4 << 3)
#define UART011_IFLS_TX1_8 (0 << 0)
#define UART011_IFLS_TX2_8 (1 << 0)
#define UART011_IFLS_TX4_8 (2 << 0)
#define UART011_IFLS_TX6_8 (3 << 0)
#define UART011_IFLS_TX7_8 (4 << 0)

#define UART011_OEIM (1 << 10) /* overrun error interrupt mask */
#define UART011_BEIM (1 << 9) /* break error interrupt mask */
#define UART011_PEIM (1 << 8) /* parity error interrupt mask */
#define UART011_FEIM (1 << 7) /* framing error interrupt mask */
#define UART011_RTIM (1 << 6) /* receive timeout interrupt mask */
#define UART011_TXIM (1 << 5) /* transmit interrupt mask */
#define UART011_RXIM (1 << 4) /* receive interrupt mask */
#define UART011_DSRMIM (1 << 3) /* DSR interrupt mask */
#define UART011_DCDMIM (1 << 2) /* DCD interrupt mask */
#define UART011_CTSMIM (1 << 1) /* CTS interrupt mask */
#define UART011_RIMIM (1 << 0) /* RI interrupt mask */

#define UART011_OEIS (1 << 10) /* overrun error interrupt status */
#define UART011_BEIS (1 << 9) /* break error interrupt status */
#define UART011_PEIS (1 << 8) /* parity error interrupt status */
#define UART011_FEIS (1 << 7) /* framing error interrupt status */
#define UART011_RTIS (1 << 6) /* receive timeout interrupt status */
#define UART011_TXIS (1 << 5) /* transmit interrupt status */
#define UART011_RXIS (1 << 4) /* receive interrupt status */
#define UART011_DSRMIS (1 << 3) /* DSR interrupt status */
#define UART011_DCDMIS (1 << 2) /* DCD interrupt status */
#define UART011_CTSMIS (1 << 1) /* CTS interrupt status */
#define UART011_RIMIS (1 << 0) /* RI interrupt status */

#define UART011_OEIC (1 << 10) /* overrun error interrupt clear */
#define UART011_BEIC (1 << 9) /* break error interrupt clear */
#define UART011_PEIC (1 << 8) /* parity error interrupt clear */
#define UART011_FEIC (1 << 7) /* framing error interrupt clear */
#define UART011_RTIC (1 << 6) /* receive timeout interrupt clear */
#define UART011_TXIC (1 << 5) /* transmit interrupt clear */
#define UART011_RXIC (1 << 4) /* receive interrupt clear */
#define UART011_DSRMIC (1 << 3) /* DSR interrupt clear */
#define UART011_DCDMIC (1 << 2) /* DCD interrupt clear */
#define UART011_CTSMIC (1 << 1) /* CTS interrupt clear */
#define UART011_RIMIC (1 << 0) /* RI interrupt clear */

#define UART011_DMAONERR (1 << 2) /* disable dma on error */
#define UART011_TXDMAE (1 << 1) /* enable transmit dma */
#define UART011_RXDMAE (1 << 0) /* enable receive dma */

#define UART01x_RSR_ANY (UART01x_RSR_OE | UART01x_RSR_BE | UART01x_RSR_PE | UART01x_RSR_FE)
#define UART01x_FR_MODEM_ANY (UART01x_FR_DCD | UART01x_FR_DSR | UART01x_FR_CTS)

#endif /* PL011_H */
