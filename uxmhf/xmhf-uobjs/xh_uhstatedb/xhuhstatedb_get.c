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

// uhstatedb hypapp main module
// author: amit vasudevan (amitvasudevan@acm.org)

#include <xmhf.h>
#include <xmhfgeec.h>
#include <xmhf-debug.h>

#include <xc.h>
#include <uapi_gcpustate.h>
#include <uapi_slabmempgtbl.h>

#include <xh_uhstatedb.h>


void uhstatedb_get(uint32_t cpuindex, uint32_t guest_slab_index, uint64_t gpa){
  slab_params_t spl;
  xmhfgeec_uapi_slabmempgtbl_getentryforpaddr_params_t *getentryforpaddrp =
    (xmhfgeec_uapi_slabmempgtbl_getentryforpaddr_params_t *)spl.in_out_params;
  xmhfgeec_uapi_slabmempgtbl_setentryforpaddr_params_t *setentryforpaddrp =
    (xmhfgeec_uapi_slabmempgtbl_setentryforpaddr_params_t *)spl.in_out_params;

  spl.src_slabid = XMHFGEEC_SLAB_XH_UHSTATEDB;
  spl.dst_slabid = XMHFGEEC_SLAB_UAPI_SLABMEMPGTBL;
  spl.cpuid = cpuindex;

  if( gpa != 0){
    spl.dst_uapifn = XMHFGEEC_UAPI_SLABMEMPGTBL_GETENTRYFORPADDR;
    getentryforpaddrp->dst_slabid = guest_slab_index;
    getentryforpaddrp->gpa = gpa;
    //@assert getentryforpaddrp->gpa == gpa;
    XMHF_SLAB_CALLNEW(&spl);

    _XDPRINTF_("%s[%u]: original entry for gpa=%016llx is %016llx\n",
	       __func__, (uint16_t)cpuindex,
	       gpa, getentryforpaddrp->result_entry);

    spl.dst_uapifn = XMHFGEEC_UAPI_SLABMEMPGTBL_SETENTRYFORPADDR;
    setentryforpaddrp->dst_slabid = guest_slab_index;
    setentryforpaddrp->gpa = gpa;
    setentryforpaddrp->entry = getentryforpaddrp->result_entry & ~(0x4); //execute-disable
    //@assert setentryforpaddrp->gpa == gpa;
    //@assert !(setentryforpaddrp->entry & 0x4);
    XMHF_SLAB_CALLNEW(&spl);

    uhstatedb_param_t *uhctp;
    uhctp = (uhstatedb_param_t *)gpa;

    uhctp->stateVal=stateDB[uhctp->deviceID];

    _XDPRINTF_("%s[%u]: finished uhstatedb init \n",
	       __func__, (uint16_t)cpuindex);
  }else{
    //do nothing
  }
}
