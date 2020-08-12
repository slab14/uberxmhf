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

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/*
 * From: FreeBSD sys/libkern/strncpy.c
 */

#include <uberspark/uobjrtl/crt/include/string.h>


/** 
 *
 * @brief Copy characters from string
 * 
 * @param[out] dst Pointer to the destination array where the content is to be copied
 * @param[in] src C string to be copied
 * @param[in] n Maximum number of characters to be copied from src
 * 
 * @retval dst is returned
 *  
 * @details_begin 
 * Copies the first ``n`` characters of ``src`` to ``dst``. If the end of the source C string (which is signaled by a 
 * NULL character) is found before ``n`` characters have been copied, destination is padded with zeros until a total of 
 * ``n`` characters have been written to it.
 * @details_end
 *
 *  @uobjrtl_namespace{uberspark/uobjrtl/crt}
 * 
 * @headers_begin 
 * #include <uberspark/uobjrtl/crt/include/string.h>
 * @headers_end
 * 
 * @comments_begin
 * No NULL character is implicitly appended at the end of ``dst`` if ``src`` is longer than ``n``. Thus, in this case, 
 * ``dst`` shall not be considered a NULL terminated C string.
 *
 * .. note:: ``dst`` and ``src`` shall not overlap 
 *
 * .. note:: Functional correctness specified
 * @comments_end
 * 
 */
/*@
	requires n >= 0;
	requires \exists integer i; Length_of_str_is(src, i);
	requires \valid(dst+(0..n-1));
	requires \valid(((char*)src)+(0..n-1));
	requires \separated(src+(0..n-1), dst+(0..n-1));
	assigns dst[0..n-1];
	ensures \result == dst;
@*/
char *uberspark_uobjrtl_crt__strncpy(char *dst, const char *src, size_t n)
{
	char *q = dst;
	const char *p = src;
	char ch;
	size_t i;

	/*@
		loop invariant 0 <= n <= \at(n,Pre);
		loop invariant \at(n, Here) != \at(n, Pre) ==> ch != 0;
		loop invariant p == ((char*)src)+(\at(n, Pre) - n);
		loop invariant q == ((char*)dst)+(\at(n, Pre) - n);
		loop invariant (char*)dst <= q <= (char*)dst+\at(n,Pre);
		loop invariant (char*)src <= p <= (char*)src+\at(n,Pre);
		loop assigns n, q, ch, p, ((char*)dst)[0..(\at(n,Pre)- n - 1)];
		loop variant n;
	@*/
	while (n) {
		ch = *p;
		*q = ch;
		if (!ch)
			break;
		q++;
		p++;
		n--;
	}

	//memset(q, 0, n);
	/*@
		loop invariant 0 <= i <= n;
		loop assigns i;
		loop assigns q[0..(n-1)];
		loop variant n-i;
	@*/
	for(i=0; i < n; i++){
		q[i]=(char)0;
	}

	return dst;
}
