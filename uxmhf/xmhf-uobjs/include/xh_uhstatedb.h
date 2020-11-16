/*
 * @XMHF_LICENSE_HEADER_START@
 *
 * eXtensible, Modular Hypervisor Framework (XMHF)
 * Copyright (c) 2009-2012 Carnegie Mellon University
 * Copyright (c) 2010-2012 VDG Inc.
 * All Rights Reserved.
 *
 * Developed by: XMHF Team
 *               Carnegie Mellon University / CyLab
 *               VDG Inc.
 *               http://xmhf.org
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the names of Carnegie Mellon or VDG Inc, nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @XMHF_LICENSE_HEADER_END@
 */


/*
 *
 *  uhstatedb hypapp slab decls.
 *
 *  author: amit vasudevan (amitvasudevan@acm.org)
 *  author: matt mccormack (matthew.mccormack@live.com)
 */

#ifndef __XH_UHSTATEDB_H__
#define __XH_UHSTATEDB_H__

#define UAPP_UHSTATEDB_FUNCTION_INIT 0x60
#define UAPP_UHSTATEDB_FUNCTION_GET 0x61
#define UAPP_UHSTATEDB_FUNCTION_NEXT 0x62

#define MAX_STATES 10


#ifndef __ASSEMBLY__

typedef struct {
  uint32_t maxArray[MAX_STATES];
  uint32_t numStates;
  uint32_t vaddr;
  uint32_t deviceID;
  uint32_t stateVal;
}uhstatedb_param_t;

extern int32_t stateDB[10];
extern uint32_t maxStateDB[10];
extern int32_t DB_SET;

void uhstatedb_hcbshutdown(uint32_t cpuindex, uint32_t guest_slab_index);
void uhstatedb_hcbinit(uint32_t cpuindex);
void uhstatedb_hcbhypercall(uint32_t cpuindex, uint32_t guest_slab_index);
void uhstatedb_init(uint32_t cpuindex, uint32_t guest_slab_index, uint64_t gpa);
void uhstatedb_next(uint32_t cpuindex, uint32_t guest_slab_index, uint64_t gpa);
void uhstatedb_get(uint32_t cpuindex, uint32_t guest_slab_index, uint64_t gpa);

#endif	//__ASSEMBLY__

#endif //__XH_UHSTATEDB_H__
