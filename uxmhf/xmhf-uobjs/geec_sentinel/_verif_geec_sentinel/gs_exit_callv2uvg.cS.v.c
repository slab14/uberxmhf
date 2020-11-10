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
 GEEC sentinel low-level support routines
 author: amit vasudevan (amitvasudevan@acm.org)
*/

#include <xmhf.h>
#include <xmhf-hwm.h>
#include <xmhf-debug.h>
#include <xmhfgeec.h>
#include <geec_sentinel.h>



//if we return, we had an error and we return the
//corresponding error code
CASM_FUNCDEF(uint32_t, gs_exit_callv2uvg,
{
	xmhfhwm_cpu_insn_movl_imm_edx(0x80);
    xmhfhwm_cpu_insn_vmlaunch();
    xmhfhwm_cpu_insn_jc(__vmx_start_hvm_failinvalid);
    xmhfhwm_cpu_insn_jnz(__vmx_start_hvm_undefinedimplementation);
    xmhfhwm_cpu_insn_movl_imm_eax(0x1);		//VMLAUNCH error, XXX: need to read from VM instruction error field in VMCS
    xmhfhwm_cpu_insn_jmplabel(__vmx_start_continue);
    CASM_LABEL(__vmx_start_hvm_undefinedimplementation);
    xmhfhwm_cpu_insn_movl_imm_eax(0x2);		//violation of VMLAUNCH specs., handle it anyways
    xmhfhwm_cpu_insn_jmplabel(__vmx_start_continue);
    CASM_LABEL(__vmx_start_hvm_failinvalid);
    xmhfhwm_cpu_insn_xorl_eax_eax();		//return 0 as we have no error code available
    CASM_LABEL(__vmx_start_continue);
    xmhfhwm_cpu_insn_retu32();
},
void *noparam)

