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
	base types

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#ifndef __TYPES_H__
#define __TYPES_H__

#include <uberspark/uobjcoll/platform/rpi3/uxmhf/main/include/config.h>
#include <uberspark/uobjrtl/crt/include/stdint.h>
#include <uberspark/uobjrtl/crt/include/stdbool.h>
#include <uberspark/uobjrtl/crt/include/stddef.h>
#include <uberspark/uobjrtl/crt/include/stdarg.h>
#include <uberspark/uobjrtl/crt/include/string.h>


#ifndef __ASSEMBLY__

typedef unsigned int u32;
typedef unsigned char u8;
typedef unsigned long long u64;

#endif // __ASSEMBLY__





#endif //__TYPES_H__
