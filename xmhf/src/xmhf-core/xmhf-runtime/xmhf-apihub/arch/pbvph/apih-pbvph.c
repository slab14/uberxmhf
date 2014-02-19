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

#include <xmhf.h>


/*
 * 	apih-swfp.c
 * 
 *  XMHF core API interface component software function-pointer backend
 * 
 *  author: amit vasudevan (amitvasudevan@acm.org)
 */

XMHF_HYPAPP_HEADER *hypappheader=(XMHF_HYPAPP_HEADER *)__TARGET_BASE_XMHFHYPAPP;

//hypapp callback hub entry point and hypapp top of stack
u32 hypapp_cbhub_pc=0;
u32 hypapp_tos=0;

//core and hypapp page table base address (PTBA)
u32 core_ptba=0;
u32 hypapp_ptba=0;

/*//function to transfer execution to the hypapp space at the specified
//entry point
void xmhf_apihub_arch_tohypapp(u32 hypappcallnum){
	typedef void (*HYPAPPFROMCORE)(u32 callnum);
	HYPAPPFROMCORE hypappfromcore = (HYPAPPFROMCORE)hypappheader->addr_hypappfromcore;
	hypappfromcore(hypappcallnum);
	return;
}*/

//function where we get control when hypapp tries to invoke a function in
//the core
void xmhf_apihub_arch_fromhypapp(u32 callnum){
	printf("\n%s: starting, callnum=%u...", __FUNCTION__, callnum);
	
	//if paramhypapp->param1 is of type VCPU * then it basically points to paramcore->vcpu, the original vcpu data structure is in paramcore->param1
	switch(callnum){
			case XMHF_APIHUB_COREAPI_OUTPUTDEBUGSTRING:{	//void xmhfc_puts(const char *s)
					extern void xmhfc_puts(const char *s);	//TODO: move this into an appropriate header
					xmhfc_puts( __xmhfattribute__(hypapp-ro) (char *)(u32)paramhypapp->param1 );
					break;
				}
				
			case XMHF_APIHUB_COREAPI_REBOOT:{	//void xmhf_baseplatform_reboot(VCPU *vcpu)
					//xmhf_baseplatform_reboot( __xmhfattribute__(core-rw) (VCPU *)(u32)paramhypapp->param1 );
					xmhf_baseplatform_reboot(paramhypapp->context_desc );
					printf("\n%s: xmhf_baseplatform_reboot should never return. halting!", __FUNCTION__);
					HALT();
					break;
				}

			case XMHF_APIHUB_COREAPI_SETMEMPROT:{ //void xmhf_memprot_setprot(VCPU *vcpu, u64 gpa, u32 prottype)
				//xmhf_memprot_setprot( __xmhfattribute__(core-rw) (VCPU *)(u32)paramhypapp->param1, (u64)paramhypapp->param2, (u32)paramhypapp->param3);
				//xmhf_memprot_setprot( __xmhfattribute__(core-rw) (VCPU *)(u32)paramcore->param1, (u64)paramhypapp->param2, (u32)paramhypapp->param3);
				xmhf_memprot_setprot(paramhypapp->context_desc, (u64)paramhypapp->param1, (u32)paramhypapp->param2);
				break;
			}
			
			/*case XMHF_APIHUB_COREAPI_MEMPROT_GETPROT:{ //void xmhfcore_memprot_getprot(VCPU *vcpu, u64 gpa);
				//paramcore->result = xmhf_memprot_getprot( __xmhfattribute__(core-rw) (VCPU *)(u32)paramhypapp->param1, (u64)paramhypapp->param2);
				paramcore->result = xmhf_memprot_getprot( __xmhfattribute__(core-rw) (VCPU *)(u32)paramcore->param1, (u64)paramhypapp->param2);
				break;
			}*/
			
			case XMHF_APIHUB_COREAPI_MEMPROT_FLUSHMAPPINGS:{ //void xmhfcore_memprot_flushmappings(VCPU *vcpu);
				//xmhf_memprot_flushmappings( __xmhfattribute__(core-rw) (VCPU *)(u32)paramhypapp->param1 );
				xmhf_memprot_flushmappings( paramhypapp->context_desc );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_SMPGUEST_WALK_PAGETABLES:{ //u8 * xmhf_smpguest_walk_pagetables(VCPU *vcpu, u32 vaddr);
				//paramcore->result = (u32)xmhf_smpguest_walk_pagetables( __xmhfattribute__(core-rw) (VCPU *)(u32)paramhypapp->param1, (u32)paramhypapp->param2);
				//paramcore->result = (u32)xmhf_smpguest_walk_pagetables( __xmhfattribute__(core-rw) (VCPU *)(u32)paramcore->param1, (u32)paramhypapp->param2);
				paramcore->result = (u32)xmhf_smpguest_walk_pagetables( paramhypapp->context_desc, (u32)paramhypapp->param1);
				break;
			}
			
			/*case XMHF_APIHUB_COREAPI_BASEPLATFORM_GETCPUTABLE:{ //xmhfcoreapiretval_t xmhf_baseplatform_getcputable(void);
				paramcore->retval = xmhf_baseplatform_getcputable();
				break;
			}*/

			case XMHF_APIHUB_COREAPI_MEMPROT_SETSINGULARHPT:{ 
				xmhf_memprot_setsingularhpt(paramhypapp->param1);
				break;
			}

			case XMHF_APIHUB_COREAPI_MEMPROT_GETHPTROOT:{ //u64 xmhfcore_memprot_getHPTroot(VCPU *vcpu)
				paramcore->result=xmhf_memprot_getHPTroot(paramhypapp->context_desc);
				break;
			}

//-----------------------------------------------------------------------

			/*
			case XMHF_APIHUB_COREAPI_MEMPROT_ARCH_X86VMX_GET_EPTP:{ //u64 xmhfcore_memprot_arch_x86vmx_get_EPTP(VCPU *vcpu);
				//paramcore->result = xmhf_memprot_arch_x86vmx_get_EPTP( __xmhfattribute__(core-rw) (VCPU *)paramhypapp->param1  );
				paramcore->result = xmhf_memprot_arch_x86vmx_get_EPTP( __xmhfattribute__(core-rw) (VCPU *)paramcore->param1  );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_MEMPROT_ARCH_X86VMX_SET_EPTP:{ //void xmhfcore_memprot_arch_x86vmx_set_EPTP(VCPU *vcpu, u64 eptp);
				//xmhf_memprot_arch_x86vmx_set_EPTP( __xmhfattribute__(core-rw) (VCPU *)paramhypapp->param1, (u64)paramhypapp->param2  );
				xmhf_memprot_arch_x86vmx_set_EPTP( __xmhfattribute__(core-rw) (VCPU *)paramcore->param1, (u64)paramhypapp->param2  );
				break;
			}

			case XMHF_APIHUB_COREAPI_MEMPROT_ARCH_X86SVM_GET_H_CR3:{ //u64 xmhfcore_memprot_arch_x86svm_get_h_cr3(VCPU *vcpu);
				//paramcore->result = xmhf_memprot_arch_x86svm_get_h_cr3( __xmhfattribute__(core-rw) (VCPU *)paramhypapp->param1  );
				paramcore->result = xmhf_memprot_arch_x86svm_get_h_cr3( __xmhfattribute__(core-rw) (VCPU *)paramcore->param1  );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_MEMPROT_ARCH_X86SVM_SET_H_CR3:{ //void xmhfcore_memprot_arch_x86svm_set_h_cr3(VCPU *vcpu, u64 n_cr3);
				//xmhf_memprot_arch_x86svm_set_h_cr3( __xmhfattribute__(core-rw) (VCPU *)paramhypapp->param1, (u64)paramhypapp->param2  );
				xmhf_memprot_arch_x86svm_set_h_cr3( __xmhfattribute__(core-rw) (VCPU *)paramcore->param1, (u64)paramhypapp->param2  );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_PARTITION_LEGACYIO_SETPROT:{ //void xmhfcore_partition_legacyIO_setprot(VCPU *vcpu, u32 port, u32 size, u32 prottype)
				xmhf_partition_legacyIO_setprot( __xmhfattribute__(core-rw) (VCPU *)(u32)paramhypapp->param1, (u32)paramhypapp->param2, (u32)paramhypapp->param3, (u32)paramhypapp->param4 );
				break;
			}
			
			
			case XMHF_APIHUB_COREAPI_TPM_OPEN_LOCALITY:{ //int xmhfcore_tpm_open_locality(int locality);
				paramcore->result = (u32)xmhf_tpm_open_locality( (int)paramhypapp->param1  );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_TPM_DEACTIVATE_ALL_LOCALITIES:{ //void xmhfcore_tpm_deactivate_all_localities(void);
				xmhf_tpm_deactivate_all_localities();
				break;
			}

			case XMHF_APIHUB_COREAPI_BASEPLATFORM_ARCH_X86_ACPI_GETRSDP:{ //u32 xmhfcore_baseplatform_arch_x86_acpi_getRSDP(ACPI_RSDP *rsdp);
				paramcore->result = (u32)xmhf_baseplatform_arch_x86_acpi_getRSDP( (ACPI_RSDP *)(u32)paramhypapp->param1 );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_TPM_WRITE_CMD_FIFO:{ //uint32_t xmhfcore_tpm_write_cmd_fifo(uint32_t locality, uint8_t *in, uint32_t in_size, uint8_t *out, uint32_t *out_size);
				paramcore->result = (u32)tpm_write_cmd_fifo( (uint32_t)paramhypapp->param1, (uint8_t *)paramhypapp->param2, (uint32_t)paramhypapp->param3, (uint8_t *)paramhypapp->param4, (uint32_t *)paramhypapp->param5  );
				break;
			}*/
			
			/*case XMHF_APIHUB_COREAPI_:{ //
				paramhypapp->result = (u32)xxxx(  );
				break;
			}
			
			case XMHF_APIHUB_COREAPI_:{ //
				xxxx(  );
				break;
			}*/
			
			default:
				printf("\n%s: WARNING, unsupported core API call (%u), returning", __FUNCTION__, callnum);
				break;
	}

	return;	//back to hypapp
}

// initialization function for the core API interface
void xmhf_apihub_arch_initialize (void){

#ifndef __XMHF_VERIFICATION__

	printf("\n%s: starting...", __FUNCTION__);
	printf("\n%s: hypappheader at %08x", __FUNCTION__, hypappheader);
	printf("\n%s: hypappheader->magic is %08x", __FUNCTION__, hypappheader->magic);
	
	printf("\n%s: paramcore at 0x%08x", __FUNCTION__, (u32)paramcore);
	printf("\n%s: paramhypapp at 0x%08x", __FUNCTION__, (u32)paramhypapp);
	
	hypapp_cbhub_pc = (u32)hypappheader->addr_hypappfromcore;
	hypapp_tos = (u32)hypappheader->addr_tos;

	printf("\n%s: hypapp cbhub entry point=%x, TOS=%x", __FUNCTION__, hypapp_cbhub_pc, hypapp_tos);

	//cast hypapp header information into hypappheader 
	//(a data structure of type XMHF_HYPAPP_HEADER) and populate the
	//hypapp parameter block field
	{
		hypappheader->apb.bootsector_ptr = (u32)rpb->XtGuestOSBootModuleBase;
		hypappheader->apb.bootsector_size = (u32)rpb->XtGuestOSBootModuleSize;
		hypappheader->apb.optionalmodule_ptr = (u32)rpb->runtime_appmodule_base;
		hypappheader->apb.optionalmodule_size = (u32)rpb->runtime_appmodule_size;
		hypappheader->apb.runtimephysmembase = (u32)rpb->XtVmmRuntimePhysBase;  
		strncpy(hypappheader->apb.cmdline, rpb->cmdline, sizeof(hypappheader->apb.cmdline));
		printf("\n%s: sizeof(XMHF_HYPAPP_HEADER)=%u", __FUNCTION__, sizeof(XMHF_HYPAPP_HEADER));
		printf("\n%s: sizeof(APP_PARAM_BLOCK)=%u", __FUNCTION__, sizeof(APP_PARAM_BLOCK));
			
	}

	hypappheader->addr_hypapptocore = (u32)&xmhf_apihub_arch_fromhypapp;

	//check for PCID support (if present)
	{
			u32 eax, ebx, ecx, edx;
			
			cpuid(0x1, &eax, &ebx, &ecx, &edx);
			
			if( ecx & (1UL << 17) )
				printf("\n%s: PCID supported", __FUNCTION__);
			else
				printf("\n%s: PCID not supported", __FUNCTION__);
	}
	
	//initialize core PAE page-tables
	{
		u32 i, hva=0;
		u64 default_flags = (u64)(_PAGE_PRESENT);
		
		//init pdpt
		for(i = 0; i < PAE_PTRS_PER_PDPT; i++) {
			u64 pdt_spa = hva2spa((void *)core_3level_pdt) + (i << PAGE_SHIFT_4K);
			core_3level_pdpt[i] = pae_make_pdpe(pdt_spa, default_flags);
		}

		//init pdts with unity mappings
		default_flags = (u64)(_PAGE_PRESENT | _PAGE_RW | _PAGE_PSE | _PAGE_USER);
		for(i = 0, hva = 0; i < (ADDR_4GB >> (PAE_PDT_SHIFT)); i++, hva += PAGE_SIZE_2M) {
			u64 spa = hva2spa((void*)hva);
			u64 flags = default_flags;

			//mark core code/data/stack pages supervisor
			if(spa >= 0x10000000 && spa < 0x1CC00000)
				flags &= ~(u64)(_PAGE_USER);
			
			//mark core parameter region as read-write, no-execute
			if(spa == 0x1CC00000){
				//flags |= (u64)(_PAGE_RW | _PAGE_NX);
				flags |= (u64)(_PAGE_RW);
				flags |= (u64)(_PAGE_NX);
			}	
			
			//mark hypapp parameter region as user-read-only, no-execute
			if(spa == 0x1CE00000){
				flags &= ~(u64)(_PAGE_RW);
				flags |= (u64)_PAGE_NX;
			}
			//mark hypapp code/data/stack pages as read-only, no-execute
			if(spa >= 0x1D000000 && spa < 0x20000000){
				flags &= ~(u64)(_PAGE_RW);
				flags |= (u64)_PAGE_NX;
			}
		
			if(spa == 0xfee00000 || spa == 0xfec00000) {
				//Unity-map some MMIO regions with Page Cache disabled 
				//0xfed00000 contains Intel TXT config regs & TPM MMIO 
				//0xfee00000 contains LAPIC base 
				flags |= (u64)(_PAGE_PCD);
			}

			core_3level_pdt[i] = pae_make_pde_big(spa, flags);
		}	
	}
	
	//initialize hypapp PAE page-tables
	{
		u32 i, hva=0;
		u64 default_flags = (u64)(_PAGE_PRESENT);
		
		//init pdpt
		for(i = 0; i < PAE_PTRS_PER_PDPT; i++) {
			u64 pdt_spa = hva2spa((void *)hypapp_3level_pdt) + (i << PAGE_SHIFT_4K);
			hypapp_3level_pdpt[i] = pae_make_pdpe(pdt_spa, default_flags);
		}

		//init pdts with unity mappings
		default_flags = (u64)(_PAGE_PRESENT | _PAGE_RW | _PAGE_PSE | _PAGE_USER);
		for(i = 0, hva = 0; i < (ADDR_4GB >> (PAE_PDT_SHIFT)); i++, hva += PAGE_SIZE_2M) {
			u64 spa = hva2spa((void*)hva);
			u64 flags = default_flags;

			//mark core code/data/stack pages supervisor
			if(spa >= 0x10000000 && spa < 0x1CC00000)
				flags &= ~(u64)(_PAGE_USER);
			
			//mark core parameter region as user-read-only, no-execute
			if(spa == 0x1CC00000){
				flags &= ~(u64)(_PAGE_RW);
				flags |= (u64)(_PAGE_NX);
			}
		
			//mark hypapp parameter region as user-read-write, no-execute
			if(spa == 0x1CE00000){
				flags |= (u64)(_PAGE_RW);
				flags |= (u64)(_PAGE_NX);
			}
			
			//mark hypapp code/data/stack pages as read-write
			if(spa >= 0x1D000000 && spa < 0x20000000)
				flags |= (u64)(_PAGE_RW);

			if(spa == 0xfee00000 || spa == 0xfec00000) {
				//Unity-map some MMIO regions with Page Cache disabled 
				//0xfed00000 contains Intel TXT config regs & TPM MMIO 
				//0xfee00000 contains LAPIC base 
				flags |= (u64)(_PAGE_PCD);
			}

			hypapp_3level_pdt[i] = pae_make_pde_big(spa, flags);
		}	
	}


	//setup core and hypapp page table base addresses and print them out
	{
		core_ptba = (u32)&core_3level_pdpt;
		hypapp_ptba = (u32)&hypapp_3level_pdpt;
		printf("\n%s: core_ptba=%08x, hypapp_ptba=%08x", core_ptba, hypapp_ptba);
	}

		
	//change CR3 to point to core page tables
	{
		u32 cr3;

		cr3 = read_cr3();
		printf("\n%s: CR3=%08x", __FUNCTION__, cr3);
		cr3 = (u32)&core_3level_pdpt;
		printf("\n%s: attempting to change CR3 to %08x", __FUNCTION__, cr3);
		write_cr3(cr3);
		cr3 = read_cr3();
		printf("\n%s: CR3 changed to %08x", __FUNCTION__, cr3);
	}

	//turn on WP bit in CR0 register for supervisor mode read-only permission support
	{
		u32 cr0;
		cr0=read_cr0();
		printf("\n%s: CR0=%08x", __FUNCTION__, cr0);
		cr0 |= CR0_WP;
		printf("\n%s: attempting to change CR0 to %08x", __FUNCTION__, cr0);
		write_cr0(cr0);
		cr0 = read_cr0();
		printf("\n%s: CR0 changed to %08x", __FUNCTION__, cr0);
	}

	//turn on NX protections
	{
		u32 eax, edx;
		rdmsr(MSR_EFER, &eax, &edx);
		eax |= (1 << EFER_NXE);
		wrmsr(MSR_EFER, eax, edx);
		printf("\n%s: NX protections enabled: MSR_EFER=%08x%08x", __FUNCTION__, edx, eax);
	}

#endif //__XMHF_VERIFICATION__
}

