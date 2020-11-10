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
 * XMHF core initialization slab code
 *
 * author: amit vasudevan (amitvasudevan@acm.org)
 */

#include <xmhf.h>
#include <xmhf-debug.h>

#include <xmhfgeec.h>
#include <xmhfhw.h>

#include <xc.h>
#include <uapi_gcpustate.h>
#include <xh_hyperdep.h>
#include <xh_syscalllog.h>
#include <xh_ssteptrace.h>
#include <xh_aprvexec.h>

#include <xc_init.h>



//////
// call guest uobj
//////
static void xcinit_do_callguest(slab_params_t *sp){
	slab_params_t spl;

	memset(&spl, 0, sizeof(spl));
	spl.cpuid = sp->cpuid;
	spl.src_slabid = XMHFGEEC_SLAB_XC_INIT;
	spl.dst_slabid = XMHFGEEC_SLAB_XG_RICHGUEST;
	XMHF_SLAB_CALLNEW(&spl);

}




//////
// setup guest uobj
//////
static void xcinit_setup_guest(slab_params_t *sp, bool isbsp){

	//setup guest slab VMCS state
	{
		slab_params_t spl;
		xmhf_uapi_gcpustate_vmrw_params_t *gcpustate_vmrwp =
			(xmhf_uapi_gcpustate_vmrw_params_t *)spl.in_out_params;

		spl.cpuid = sp->cpuid;
		spl.src_slabid = XMHFGEEC_SLAB_XC_INIT;
		spl.dst_slabid = XMHFGEEC_SLAB_UAPI_GCPUSTATE;
		spl.dst_uapifn = XMHF_HIC_UAPI_CPUSTATE_VMWRITE;

		//generic guest VMCS setup
		gcpustate_vmrwp->encoding = VMCS_CONTROL_CR4_SHADOW;
		gcpustate_vmrwp->value =(uint64_t)CR4_VMXE;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_CONTROL_PAGEFAULT_ERRORCODE_MASK;
		gcpustate_vmrwp->value = 0x00000000;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_CONTROL_PAGEFAULT_ERRORCODE_MATCH;
		gcpustate_vmrwp->value = 0x00000000;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_CONTROL_EXCEPTION_BITMAP;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_CONTROL_CR3_TARGET_COUNT;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_CONTROL_VM_ENTRY_EXCEPTION_ERRORCODE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_CONTROL_VM_ENTRY_INTERRUPTION_INFORMATION;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		//GDTR
		gcpustate_vmrwp->encoding = VMCS_GUEST_GDTR_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_GDTR_LIMIT;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		//IDTR
		gcpustate_vmrwp->encoding = VMCS_GUEST_IDTR_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_IDTR_LIMIT;
		gcpustate_vmrwp->value = 0x3ff;
		XMHF_SLAB_CALLNEW(&spl);

		//LDTR, unusable
		gcpustate_vmrwp->encoding = VMCS_GUEST_LDTR_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_LDTR_LIMIT;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_LDTR_SELECTOR;
		gcpustate_vmrwp->value = 0 ;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_LDTR_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x10000;
		XMHF_SLAB_CALLNEW(&spl);

		//TR
		gcpustate_vmrwp->encoding = VMCS_GUEST_TR_BASE ;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_TR_LIMIT;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_TR_SELECTOR;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_TR_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x83;
		XMHF_SLAB_CALLNEW(&spl);

		//CS segment
		gcpustate_vmrwp->encoding = VMCS_GUEST_CS_SELECTOR;
		gcpustate_vmrwp->value = 0x0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_CS_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_CS_LIMIT;
		gcpustate_vmrwp->value = 0x0000FFFFUL;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_CS_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x0093;
		XMHF_SLAB_CALLNEW(&spl);

		//DS segment
		gcpustate_vmrwp->encoding = VMCS_GUEST_DS_SELECTOR;
		gcpustate_vmrwp->value = 0x0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_DS_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_DS_LIMIT;
		gcpustate_vmrwp->value = 0x0000FFFFUL;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_DS_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x0093;
		XMHF_SLAB_CALLNEW(&spl);

		//ES segment
		gcpustate_vmrwp->encoding = VMCS_GUEST_ES_SELECTOR;
		gcpustate_vmrwp->value = 0x0 ;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_ES_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_ES_LIMIT;
		gcpustate_vmrwp->value = 0x0000FFFFUL;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_ES_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x0093;
		XMHF_SLAB_CALLNEW(&spl);

		//FS segment
		gcpustate_vmrwp->encoding = VMCS_GUEST_FS_SELECTOR;
		gcpustate_vmrwp->value = 0x0 ;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_FS_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_FS_LIMIT;
		gcpustate_vmrwp->value = 0x0000FFFFUL;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_FS_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x0093;
		XMHF_SLAB_CALLNEW(&spl);

		//GS segment
		gcpustate_vmrwp->encoding = VMCS_GUEST_GS_SELECTOR;
		gcpustate_vmrwp->value = 0x0 ;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_GS_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_GS_LIMIT;
		gcpustate_vmrwp->value = 0x0000FFFFUL;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_GS_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x0093;
		XMHF_SLAB_CALLNEW(&spl);

		//SS segment
		gcpustate_vmrwp->encoding = VMCS_GUEST_SS_SELECTOR;
		gcpustate_vmrwp->value = 0x0 ;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_SS_BASE;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_SS_LIMIT;
		gcpustate_vmrwp->value = 0x0000FFFFUL;
		XMHF_SLAB_CALLNEW(&spl);

		gcpustate_vmrwp->encoding = VMCS_GUEST_SS_ACCESS_RIGHTS;
		gcpustate_vmrwp->value = 0x0093;
		XMHF_SLAB_CALLNEW(&spl);

		//guest EIP and activity state
		if(isbsp){
			_XDPRINTF_("%s[%u]: BSP: setting RIP and activity state for boot\n", __func__, (uint16_t)sp->cpuid);
			gcpustate_vmrwp->encoding = VMCS_GUEST_RIP;
			gcpustate_vmrwp->value = 0x00007C00;
			XMHF_SLAB_CALLNEW(&spl);

			gcpustate_vmrwp->encoding = VMCS_GUEST_ACTIVITY_STATE;
			gcpustate_vmrwp->value = 0;
			XMHF_SLAB_CALLNEW(&spl);
		}else{
			gcpustate_vmrwp->encoding = VMCS_GUEST_RIP;
			gcpustate_vmrwp->value = 0x00000000;
			XMHF_SLAB_CALLNEW(&spl);

			gcpustate_vmrwp->encoding = VMCS_GUEST_ACTIVITY_STATE;
			gcpustate_vmrwp->value = 3;	//wait-for-SIPI
			XMHF_SLAB_CALLNEW(&spl);
		}

		//interruptibility
		gcpustate_vmrwp->encoding = VMCS_GUEST_INTERRUPTIBILITY;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		//guest ESP
		gcpustate_vmrwp->encoding = VMCS_GUEST_RSP;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);

		//guest RFLAGS
		gcpustate_vmrwp->encoding = VMCS_GUEST_RFLAGS;
		gcpustate_vmrwp->value = ((((0 & ~((1<<3)|(1<<5)|(1<<15)) ) | (1 <<1)) | (1<<9)) & ~(1<<14));
		XMHF_SLAB_CALLNEW(&spl);

		//other guest GPRS (EAX, EBX, ECX, EDX, ESI, EDI, EBP)
		spl.dst_uapifn = XMHF_HIC_UAPI_CPUSTATE_GUESTGPRSWRITE;
		spl.in_out_params[0] = 0;	//EDI
		spl.in_out_params[1] = 0;	//ESI
		spl.in_out_params[2] = 0;	//EBP
		spl.in_out_params[3] = 0;	//Reserved (ESP)
		spl.in_out_params[4] = 0;	//EBX
		spl.in_out_params[5] = 0;	//EDX
		spl.in_out_params[6] = 0;	//ECX
		spl.in_out_params[7] = 0;	//EAX
		XMHF_SLAB_CALLNEW(&spl);

		//guest control registers (CR0, CR3 and CR0_SHADOW)
		spl.dst_uapifn = XMHF_HIC_UAPI_CPUSTATE_VMREAD;
		gcpustate_vmrwp->encoding = VMCS_GUEST_CR0;
		XMHF_SLAB_CALLNEW(&spl);
		spl.dst_uapifn = XMHF_HIC_UAPI_CPUSTATE_VMWRITE;
		gcpustate_vmrwp->encoding = VMCS_GUEST_CR0;
		gcpustate_vmrwp->value = gcpustate_vmrwp->value & ~(CR0_PE) & ~(CR0_PG);
		XMHF_SLAB_CALLNEW(&spl);
		gcpustate_vmrwp->encoding = VMCS_GUEST_CR3;
		gcpustate_vmrwp->value = 0;
		XMHF_SLAB_CALLNEW(&spl);
		spl.dst_uapifn = XMHF_HIC_UAPI_CPUSTATE_VMREAD;
		gcpustate_vmrwp->encoding = VMCS_GUEST_CR0;
		XMHF_SLAB_CALLNEW(&spl);
		spl.dst_uapifn = XMHF_HIC_UAPI_CPUSTATE_VMWRITE;
		gcpustate_vmrwp->encoding = VMCS_CONTROL_CR0_SHADOW;
		XMHF_SLAB_CALLNEW(&spl);

	}



}



//////
// invoke hypapp initialization callbacks
//////
static uint32_t xc_hcbinvoke(uint32_t src_slabid, uint32_t cpuid, uint32_t cbtype, uint32_t cbqual, uint32_t guest_slab_index){
    uint32_t status = XC_HYPAPPCB_CHAIN;
    uint32_t i;
    slab_params_t spl;
    xc_hypappcb_params_t *hcbp = (xc_hypappcb_params_t *)&spl.in_out_params[0];

    spl.src_slabid = src_slabid;
    spl.cpuid = cpuid;
    spl.dst_uapifn = 0;
    hcbp->cbtype=cbtype;
    hcbp->cbqual=cbqual;
    hcbp->guest_slab_index=guest_slab_index;
    hcbp->cbresult = 0;

    for(i=0; i < HYPAPP_INFO_TABLE_NUMENTRIES; i++){
        if(_xcihub_hypapp_info_table[i].cbmask & XC_HYPAPPCB_MASK(cbtype)){
            spl.dst_slabid = _xcihub_hypapp_info_table[i].xmhfhic_slab_index;
            XMHF_SLAB_CALLNEW(&spl);
            if(hcbp->cbresult == XC_HYPAPPCB_NOCHAIN){
                status = XC_HYPAPPCB_NOCHAIN;
                break;
            }
        }
    }

    return status;
}


//////
// setup E820 hook for guest uobj
//////
static void	xcinit_e820initializehooks(void){
		uint16_t orig_int15h_ip, orig_int15h_cs;

		//implant VMCALL followed by IRET at 0040:04AC
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu8, 0x4ac, 0x0f); //VMCALL
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu8, 0x4ad, 0x01);
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu8, 0x4ae, 0xc1);
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu8, 0x4af, 0xcf); //IRET

		//store original INT 15h handler CS:IP following VMCALL and IRET
		orig_int15h_ip = CASM_FUNCCALL(xmhfhw_sysmemaccess_readu16, 0x54);
		orig_int15h_cs = CASM_FUNCCALL(xmhfhw_sysmemaccess_readu16, 0x56);
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu16, 0x4b0, orig_int15h_ip); //original INT 15h IP
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu16, 0x4b2, orig_int15h_cs); //original INT 15h CS

		//point IVT INT15 handler to the VMCALL instruction
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu16, 0x54, 0x00ac);
		CASM_FUNCCALL(xmhfhw_sysmemaccess_writeu16, 0x56, 0x0040);

}


//////
// copy guest boot module into appropriate location
//////
static void	xcinit_copyguestbootmodule(uint32_t g_bm_base, uint32_t g_bm_size){
	_XDPRINTF_("%s: boot-module at 0x%08x, size=0x%08x (%u) bytes\n", __func__, g_bm_base, g_bm_size, g_bm_size);
	if( (g_bm_size == 512) ){
		CASM_FUNCCALL(xmhfhw_sysmemaccess_copy, 0x00007C00, g_bm_base, g_bm_size);
	}else{
		_XDPRINTF_("%s: invalid boot-module at 0x%08x, size=0x%08x (%u) bytes, Halting!\n", __func__, g_bm_base, g_bm_size, g_bm_size);
		CASM_FUNCCALL(xmhfhw_cpu_hlt, CASM_NOPARAM);
	}
}



#if defined (__XMHF_VERIFICATION__) && defined (__USPARK_FRAMAC_VA__)
uint32_t check_esp, check_eip = CASM_RET_EIP;
slab_params_t test_sp;
uint32_t cpuid = 0;	//cpu id

void main(void){
	//populate hardware model stack and program counter
	xmhfhwm_cpu_gprs_esp = _slab_tos[cpuid];
	xmhfhwm_cpu_gprs_eip = check_eip;
	check_esp = xmhfhwm_cpu_gprs_esp; // pointing to top-of-stack

	//inform hardware model to treat rich guest memory region as valid memory
	xmhfhwm_sysmemaccess_physmem_extents[xmhfhwm_sysmemaccess_physmem_extents_total].addr_start =
		xmhfgeec_slab_info_table[XMHFGEEC_SLAB_XG_RICHGUEST].slab_physmem_extents[0].addr_start;
	xmhfhwm_sysmemaccess_physmem_extents[xmhfhwm_sysmemaccess_physmem_extents_total].addr_end =
		xmhfgeec_slab_info_table[XMHFGEEC_SLAB_XG_RICHGUEST].slab_physmem_extents[0].addr_end;
	xmhfhwm_sysmemaccess_physmem_extents_total++;
	xmhfhwm_sysmemaccess_physmem_extents[xmhfhwm_sysmemaccess_physmem_extents_total].addr_start =
		xmhfgeec_slab_info_table[XMHFGEEC_SLAB_XG_RICHGUEST].slab_physmem_extents[1].addr_start;
	xmhfhwm_sysmemaccess_physmem_extents[xmhfhwm_sysmemaccess_physmem_extents_total].addr_end =
		xmhfgeec_slab_info_table[XMHFGEEC_SLAB_XG_RICHGUEST].slab_physmem_extents[1].addr_end;
	xmhfhwm_sysmemaccess_physmem_extents_total++;


    test_sp.slab_ctype = framac_nondetu32();
    test_sp.src_slabid = framac_nondetu32();
    test_sp.dst_slabid = framac_nondetu32();
    test_sp.dst_uapifn = framac_nondetu32();
    test_sp.cpuid = framac_nondetu32();
	test_sp.in_out_params[0] =  framac_nondetu32(); 	test_sp.in_out_params[1] = framac_nondetu32();
	test_sp.in_out_params[2] = framac_nondetu32(); 	test_sp.in_out_params[3] = framac_nondetu32();
	test_sp.in_out_params[4] = framac_nondetu32(); 	test_sp.in_out_params[5] = framac_nondetu32();
	test_sp.in_out_params[6] = framac_nondetu32(); 	test_sp.in_out_params[7] = framac_nondetu32();
	test_sp.in_out_params[8] = framac_nondetu32(); 	test_sp.in_out_params[9] = framac_nondetu32();
	test_sp.in_out_params[10] = framac_nondetu32(); 	test_sp.in_out_params[11] = framac_nondetu32();
	test_sp.in_out_params[12] = framac_nondetu32(); 	test_sp.in_out_params[13] = framac_nondetu32();
	test_sp.in_out_params[14] = framac_nondetu32(); 	test_sp.in_out_params[15] = framac_nondetu32();

	slab_main(&test_sp);

	/*@assert ((xmhfhwm_cpu_state == CPU_STATE_RUNNING && xmhfhwm_cpu_gprs_esp == check_esp && xmhfhwm_cpu_gprs_eip == check_eip) ||
		(xmhfhwm_cpu_state == CPU_STATE_HALT));
	@*/
}
#endif

void slab_main(slab_params_t *sp){
    bool isbsp = xmhfhw_lapic_isbsp();

    #if defined (__DEBUG_SERIAL__)
	static volatile uint32_t cpucount=0;
	#endif //__DEBUG_SERIAL__

    //grab lock
    CASM_FUNCCALL(spin_lock,&__xcinit_smplock);

    _XDPRINTF_("XC_INIT[%u]: got control: ESP=%08x\n", (uint16_t)sp->cpuid, CASM_FUNCCALL(read_esp,CASM_NOPARAM));
    _XDPRINTF_("XC_INIT[%u]: HYPAPP_INFO_TABLE_NUMENTRIES=%u\n", (uint16_t)sp->cpuid, HYPAPP_INFO_TABLE_NUMENTRIES);

    //plant int 15h redirection code for E820 reporting and copy boot-module
    if(isbsp){
        _XDPRINTF_("XC_INIT[%u]: BSP: Proceeding to install E820 redirection...\n", (uint16_t)sp->cpuid);
    	xcinit_e820initializehooks();
        _XDPRINTF_("XC_INIT[%u]: BSP: E820 redirection enabled\n", (uint16_t)sp->cpuid);
        _XDPRINTF_("XC_INIT[%u]: BSP: Proceeding to copy guest boot-module...\n", (uint16_t)sp->cpuid);
    	xcinit_copyguestbootmodule(sp->in_out_params[0], sp->in_out_params[1]);
        _XDPRINTF_("XC_INIT[%u]: BSP: guest boot-module copied\n", (uint16_t)sp->cpuid);
    }

    //setup guest uobj state
    xcinit_setup_guest(sp, isbsp);


    //invoke hypapp initialization callbacks
    xc_hcbinvoke(XMHFGEEC_SLAB_XC_INIT, sp->cpuid, XC_HYPAPPCB_INITIALIZE, 0, XMHFGEEC_SLAB_XG_RICHGUEST);


    _XDPRINTF_("XC_INIT[%u]: Proceeding to call guest: ESP=%08x, eflags=%08x\n", (uint16_t)sp->cpuid,
    		CASM_FUNCCALL(read_esp,CASM_NOPARAM), CASM_FUNCCALL(read_eflags, CASM_NOPARAM));

    #if defined (__DEBUG_SERIAL__)
	cpucount++;
	#endif //__DEBUG_SERIAL__

    //release lock
    CASM_FUNCCALL(spin_unlock,&__xcinit_smplock);

    #if defined (__DEBUG_SERIAL__)
    while(cpucount < __XMHF_CONFIG_DEBUG_SERIAL_MAXCPUS__);
    #endif //__DEBUG_SERIAL__

    //call guest
    xcinit_do_callguest(sp);

    //_XDPRINTF_("%s[%u]: Should  never get here.Halting!\n", __func__, (uint16_t)sp->cpuid);
    CASM_FUNCCALL(xmhfhw_cpu_hlt, CASM_NOPARAM);

    return;
}


