/*
 * @UBERXMHF_LICENSE_HEADER_START@
 *
 * uber eXtensible Micro-Hypervisor Framework (Raspberry Pi)
 *
 * Copyright 2018 Carnegie Mellon University. All Rights Reserved.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF
 * THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF
 * ANY KIND WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 * INFRINGEMENT.
 *
 * Released under a BSD (SEI)-style license, please see LICENSE or
 * contact permission@sei.cmu.edu for full terms.
 *
 * [DISTRIBUTION STATEMENT A] This material has been approved for public
 * release and unlimited distribution.  Please see Copyright notice for
 * non-US Government use and distribution.
 *
 * Carnegie Mellon is registered in the U.S. Patent and Trademark Office by
 * Carnegie Mellon University.
 *
 * @UBERXMHF_LICENSE_HEADER_END@
 */

/*
 * Author: Amit Vasudevan (amitvasudevan@acm.org)
 *
 */

/*
	broadcom 2837 pl011 uart support

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#ifndef __PL011UART_H__
#define __PL011UART_H__



#ifndef __ASSEMBLY__

void bcm2837_pl011uart_init(void);
void bcm2837_pl011uart_putc(u8 ch);
void bcm2837_pl011uart_puts(char *buffer);
bool bcm2837_pl011uart_getc(u8 *recv_ch);
void bcm2837_pl011uart_flush(void);


#endif // __ASSEMBLY__



#endif //__PL011UART_H__
