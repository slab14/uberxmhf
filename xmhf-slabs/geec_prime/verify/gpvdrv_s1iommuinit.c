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
 * prime s1_chkreq verification driver
 * author: amit vasudevan (amitvasudevan@acm.org)
*/


#include <xmhf.h>
#include <xmhf-hwm.h>
#include <xmhfgeec.h>

#include <geec_prime.h>

u32 cpuid = 0;	//BSP cpu

//////
// frama-c non-determinism functions
//////

u32 Frama_C_entropy_source;

//@ assigns Frama_C_entropy_source \from Frama_C_entropy_source;
void Frama_C_update_entropy(void);

u32 framac_nondetu32(void){
  Frama_C_update_entropy();
  return (u32)Frama_C_entropy_source;
}

u32 framac_nondetu32interval(u32 min, u32 max)
{
  u32 r,aux;
  Frama_C_update_entropy();
  aux = Frama_C_entropy_source;
  if ((aux>=min) && (aux <=max))
    r = aux;
  else
    r = min;
  return r;
}


//////
u32 check_esp, check_eip = CASM_RET_EIP;


void xmhfhwm_vdriver_writeesp(u32 oldval, u32 newval){
	//@assert (newval >= ((u32)&_init_bsp_cpustack + 4)) && (newval <= ((u32)&_init_bsp_cpustack + MAX_PLATFORM_CPUSTACK_SIZE)) ;
}


void main(void){
	//populate hardware model stack and program counter
	xmhfhwm_cpu_gprs_esp = (u32)&_init_bsp_cpustack + MAX_PLATFORM_CPUSTACK_SIZE;
	xmhfhwm_cpu_gprs_eip = check_eip;
	check_esp = xmhfhwm_cpu_gprs_esp; // pointing to top-of-stack


	//execute harness
	vtd_drhd_maxhandle = 2;
	vtd_drhd_scanned = true;
	vtd_dmar_table_physical_address =XMHFHWM_BIOS_VTDDMARTABLEBASE;
	vtd_ret_address = 0xDEADB000;
	vtd_drhd[0].regbaseaddr =0x00000000fed90000ULL;
	vtd_drhd[0].iotlb_regaddr = 0x00000000fed90108ULL;
	vtd_drhd[0].iva_regaddr = 0x00000000fed90100ULL;
	vtd_drhd[1].regbaseaddr =0x00000000fed91000ULL;
	vtd_drhd[1].iotlb_regaddr = 0x00000000fed91108ULL;
	vtd_drhd[1].iva_regaddr = 0x00000000fed91100ULL;
	gp_s1_iommuinit();
	//@assert xmhfhwm_vtd_drhd_state[0].reg_rtaddr_lo == vtd_ret_address;
	//@assert xmhfhwm_vtd_drhd_state[0].reg_rtaddr_hi == 0;
	//@assert xmhfhwm_vtd_drhd_state[1].reg_rtaddr_lo == vtd_ret_address;
	//@assert xmhfhwm_vtd_drhd_state[1].reg_rtaddr_hi == 0;


	//@assert xmhfhwm_cpu_gprs_esp == check_esp;
	//@assert xmhfhwm_cpu_gprs_eip == check_eip;
}

