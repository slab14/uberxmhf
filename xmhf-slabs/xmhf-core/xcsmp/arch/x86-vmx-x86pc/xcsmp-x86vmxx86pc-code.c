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
 * XMHF core smp slab (xcsmp), x86-vmx-x86pc arch. backend
 * author: amit vasudevan (amitvasudevan@acm.org)
 */

#include <xmhf.h>
#include <xmhf-core.h>
#include <xmhf-debug.h>

#include <xcsmp.h>
#include <xcexhub.h>

static bool _ap_entry(void) __attribute__((naked));
void _xcsmp_cpu_x86_smpinitialize_commonstart(void);

static u8 _cpustack[MAX_PLATFORM_CPUS][MAX_PLATFORM_CPUSTACK_SIZE] __attribute__(( section(".stack") )); // platform cpu stacks
static xc_cputable_t _cputable[MAX_PLATFORM_CPUS];// cpu table
static u32 _cpucount = 0; // count of platform cpus

static mtrr_state_t _mtrrs;
static u64 _ap_cr3=0;

__attribute__(( aligned(16) )) static u64 _xcsmp_ap_init_gdt_start[]  = {
	0x0000000000000000ULL,	//NULL descriptor
	0x00af9b000000ffffULL,	//CPL-0 64-bit code descriptor (CS64)
	0x00af93000000ffffULL,	//CPL-0 64-bit data descriptor (DS/SS/ES/FS/GS)
};

__attribute__(( aligned(16) )) static arch_x86_gdtdesc_t _xcsmp_ap_init_gdt  = {
	.size=sizeof(_xcsmp_ap_init_gdt_start)-1,
	.base=&_xcsmp_ap_init_gdt_start,
};

__attribute__((naked)) static void _ap_bootstrap_code(void) {

    asm volatile (
           " .code32 \r\n"
           " movw %0, %%ax \r\n"
           " movw %%ax, %%ds \r\n"

           " movl %1, %%ebx \r\n"
           " movl (%%ebx), %%ebx \r\n"

           " jmpl *%%ebx \r\n"
           " hlt \r\n"
           " .balign 4096 \r\n"
           ".code64"
            :
            : "i" (__DS_CPL0),
              "i" ((X86SMP_APBOOTSTRAP_DATASEG << 4) + offsetof(x86smp_apbootstrapdata_t, ap_entrypoint))
            :

        );
}



static void _xcsmp_cpu_x86_savecpumtrrstate(void){
	xmhfhw_cpu_x86_save_mtrrs(&_mtrrs);
}

static void _xcsmp_cpu_x86_restorecpumtrrstate(void){
	xmhfhw_cpu_x86_restore_mtrrs(&_mtrrs);
}


//wake up APs using the LAPIC by sending the INIT-SIPI-SIPI IPI sequence
static void _xcsmp_cpu_x86_wakeupAPs(void){
	u32 eax, edx;
	volatile u32 *icr;

	//read LAPIC base address from MSR
	rdmsr(MSR_APIC_BASE, &eax, &edx);
	HALT_ON_ERRORCOND( edx == 0 ); //APIC is below 4G

	//construct the command register address (offset 0x300)
	icr = (u32 *) (((u32)eax & 0xFFFFF000UL) + 0x300);

	//our AP boot-strap code is at physical memory location 0x10000.
	//so use 0x10 as the vector (0x10000/0x1000 = 0x10)

	//send INIT
	*icr = 0x000c4500UL;

	xmhf_baseplatform_arch_x86_udelay(10000);

	//wait for command completion
	{
		u32 val;
		do{
		  val = *icr;
		}while( (val & 0x1000) );
	}

	//send SIPI (twice as per the MP protocol)
	{
		int i;
		for(i=0; i < 2; i++){
			*icr = 0x000c4610UL;
			xmhf_baseplatform_arch_x86_udelay(200);
			//wait for command completion
			{
			  u32 val;
			  do{
				val = *icr;
			  }while( (val & 0x1000) );
			}
		}
	}

}



//wake up application processors (cores) in the system
static void _xcsmp_container_vmx_wakeupAPs(void){
    static x86smp_apbootstrapdata_t apdata;

    apdata.ap_cr3 = read_cr3();
    apdata.ap_cr4 = read_cr4();
    apdata.ap_entrypoint = (u32)&_ap_entry;
    apdata.ap_gdtdesc_limit = sizeof(apdata.ap_gdt) - 1;
    apdata.ap_gdtdesc_base = (X86SMP_APBOOTSTRAP_DATASEG << 4) + offsetof(x86smp_apbootstrapdata_t, ap_gdt);
    apdata.ap_cs_selector = __CS_CPL0;
    apdata.ap_eip = (X86SMP_APBOOTSTRAP_CODESEG << 4);
    apdata.ap_gdt[0] = 0x0000000000000000ULL;
    apdata.ap_gdt[1] = 0x00cf9a000000ffffULL;
    apdata.ap_gdt[2] = 0x00cf92000000ffffULL;

    _XDPRINTF_("%s: sizeof(apdata)=%u bytes\n", __FUNCTION__, sizeof(apdata));
    _XDPRINTF_("  apdata.ap_gdtdesc_limit at %08x\n", &apdata.ap_gdtdesc_limit);
    _XDPRINTF_("  apdata.ap_gdt at %08x\n", &apdata.ap_gdt);

    memcpy((void *)(X86SMP_APBOOTSTRAP_DATASEG << 4), (void *)&apdata, sizeof(apdata));

    memcpy((void *)(X86SMP_APBOOTSTRAP_CODESEG << 4), (void *)&_ap_bootstrap_code, PAGE_SIZE_4K);

#if defined (__DRT__)
    {
        txt_heap_t *txt_heap;
        os_mle_data_t *os_mle_data;
        mle_join_t *mle_join;
        sinit_mle_data_t *sinit_mle_data;
        os_sinit_data_t *os_sinit_data;

        txt_heap = get_txt_heap();
        os_mle_data = get_os_mle_data_start(txt_heap);
        sinit_mle_data = get_sinit_mle_data_start(txt_heap);
        os_sinit_data = get_os_sinit_data_start(txt_heap);

        // enable SMIs on BSP before waking APs (which will enable them on APs)
        // because some SMM may take immediate SMI and hang if AP gets in first
        //_XDPRINTF_("Enabling SMIs on BSP\n");
        //__getsec_smctrl();

        mle_join = (mle_join_t *)((u32)(X86SMP_APBOOTSTRAP_DATASEG << 4) + offsetof(x86smp_apbootstrapdata_t, ap_gdtdesc_limit));

        _XDPRINTF_("\nBSP: mle_join.gdt_limit = %x", mle_join->gdt_limit);
        _XDPRINTF_("\nBSP: mle_join.gdt_base = %x", mle_join->gdt_base);
        _XDPRINTF_("\nBSP: mle_join.seg_sel = %x", mle_join->seg_sel);
        _XDPRINTF_("\nBSP: mle_join.entry_point = %x", mle_join->entry_point);

        write_priv_config_reg(TXTCR_MLE_JOIN, (uint64_t)(unsigned long)mle_join);

        if (os_sinit_data->capabilities.rlp_wake_monitor) {
            _XDPRINTF_("\nBSP: joining RLPs to MLE with MONITOR wakeup");
            _XDPRINTF_("\nBSP: rlp_wakeup_addr = 0x%x", sinit_mle_data->rlp_wakeup_addr);
            *((uint32_t *)(unsigned long)(sinit_mle_data->rlp_wakeup_addr)) = 0x01;
        }else {
            _XDPRINTF_("\nBSP: joining RLPs to MLE with GETSEC[WAKEUP]");
            __getsec_wakeup();
            _XDPRINTF_("\nBSP: GETSEC[WAKEUP] completed");
        }
    }

#else //!__DRT__

    _XDPRINTF_("\nBSP: Using APIC to awaken APs...");
    _xcsmp_cpu_x86_wakeupAPs();
    _XDPRINTF_("\nBSP: APs should be awake.");

#endif


}

//return 1 if the calling CPU is the BSP
static bool _xcsmp_cpu_x86_isbsp(void){
  u32 eax, edx;
  //read LAPIC base address from MSR
  rdmsr(MSR_APIC_BASE, &eax, &edx);
  HALT_ON_ERRORCOND( edx == 0 ); //APIC is below 4G

  if(eax & 0x100)
    return true;
  else
    return false;
}


//common function which is entered by all CPUs upon SMP initialization
//note: this is specific to the x86 architecture backend
void _xcsmp_cpu_x86_smpinitialize_commonstart(void){
	/*u32 cpuid = xmhf_baseplatform_arch_x86_getcpulapicid();
	bool is_bsp = _xcsmp_cpu_x86_isbsp();
	u32 bcr0;

	//initialize base CPU state
	//set OSXSAVE bit in CR4 to enable us to pass-thru XSETBV intercepts
	//when the CPU supports XSAVE feature
	if(xmhf_baseplatform_arch_x86_cpuhasxsavefeature()){
		u32 t_cr4;
		t_cr4 = read_cr4();
		t_cr4 |= CR4_OSXSAVE;
		write_cr4(t_cr4);
	}

	//turn on NX protections
	{
		u32 eax, edx;
		rdmsr(MSR_EFER, &eax, &edx);
		eax |= (1 << EFER_NXE);
		wrmsr(MSR_EFER, eax, edx);
		_XDPRINTF_("\n%s: NX protections enabled: MSR_EFER=%08x%08x", __FUNCTION__, edx, eax);
	}

	//replicate common MTRR state on this CPU
	_xcsmp_cpu_x86_restorecpumtrrstate();

	//set bit 5 (EM) of CR0 to be VMX compatible in case of Intel cores
	bcr0 = read_cr0();
	bcr0 |= 0x20;
	write_cr0(bcr0);

    //load GDT and IDT
    asm volatile(	"lgdt %0\r\n"
					"lidt %1\r\n"
					:
					: "m" (_gdt), "m" (_idt)
	);

	//load TR
	{
	  u32 gdtstart = (u32)xmhf_baseplatform_arch_x86_getgdtbase();
	  u16 trselector = 	__TRSEL;
	  asm volatile("movl %0, %%edi\r\n"
		"xorl %%eax, %%eax\r\n"
		"movw %1, %%ax\r\n"
		"addl %%eax, %%edi\r\n"		//%edi is pointer to TSS descriptor in GDT
		"addl $0x4, %%edi\r\n"		//%edi points to top 32-bits of 64-bit TSS desc.
		"lock andl $0xFFFF00FF, (%%edi)\r\n"
		"lock orl  $0x00008900, (%%edi)\r\n"
		"ltr %%ax\r\n"				//load TR
	     :
	     : "m"(gdtstart), "m"(trselector)
	     : "edi", "eax"
	  );
	}*/


	/*_XDPRINTF_("\n%s: cpu %x, isbsp=%u, Proceeding to call init_entry...\n", __FUNCTION__, cpuid, is_bsp);

	if( XMHF_SLAB_CALL(xcrichguest_entry(cpuid, is_bsp)) ){
		_XDPRINTF_("%s: Fatal. Should never be here. Halting!\n", __FUNCTION__);
		HALT();
	}*/

	_XDPRINTF_("%s: should never be here!\n", __FUNCTION__);
	HALT();
}



static bool _ap_entry(void) __attribute__((naked)){

    asm volatile(
                    ".code32 \r\n"
					"_xcsmp_ap_start: \r\n"

					"movw %%ds, %%ax \r\n"
					"movw %%ax, %%es \r\n"
					"movw %%ax, %%fs \r\n"
					"movw %%ax, %%gs \r\n"
					"movw %%ax, %%ss \r\n"

    				"movl %%cr4, %%eax \r\n"
   					"orl $0x00000030, %%eax \r\n"
   					"movl %%eax, %%cr4 \r\n"

                    "movl %0, %%ebx \r\n"
                    "movl (%%ebx), %%ebx \r\n"
                    "movl %%ebx, %%cr3 \r\n"

                    "movl $0xc0000080, %%ecx \r\n"
                    "rdmsr \r\n"
                    "orl $0x00000100, %%eax \r\n"
                    "orl $0x00000800, %%eax \r\n"
                    "wrmsr \r\n"

                    "movl %%cr0, %%eax \r\n"
                    "orl $0x80000015, %%eax \r\n"
                    "movl %%eax, %%cr0 \r\n"

                    "movl %1, %%esi \r\n"
                    "lgdt (%%esi) \r\n"

                    "mov %2, %%ecx\r\n"
					"rdmsr\r\n"
					"andl $0xFFFFF000, %%eax\r\n"
					"addl $0x20, %%eax\r\n"
					"movl (%%eax), %%eax\r\n"
					"shr $24, %%eax\r\n"
					"movl %3, %%edx\r\n"
					"movl (%%edx), %%edx \r\n"
					"movl %4, %%ebx\r\n"
					"xorl %%ecx, %%ecx\r\n"
					"xorl %%edi, %%edi\r\n"
					"getidxloop:\r\n"
					"movl 0x0(%%ebx, %%edi), %%ebp\r\n"  	//ebp contains the lapic id
					"cmpl %%eax, %%ebp\r\n"
					"jz gotidx\r\n"
					"incl %%ecx\r\n"
					"addl %5, %%edi\r\n"
					"cmpl %%edx, %%ecx\r\n"
					"jb getidxloop\r\n"
					"hlt\r\n"								//we should never get here, if so just halt
					"gotidx:\r\n"							// ecx contains index into g_xc_cputable
					"movl 0x4(%%ebx, %%edi), %%eax\r\n"	 	// eax = g_xc_cputable[ecx].cpu_index
					"movl %6, %%edi \r\n"					// edi = &_cpustack
					"movl %7, %%ecx \r\n"					// ecx = sizeof(_cpustack[0])
					"mull %%ecx \r\n"						// eax = sizeof(_cpustack[0]) * eax
					"addl %%ecx, %%eax \r\n"				// eax = (sizeof(_cpustack[0]) * eax) + sizeof(_cpustack[0])
					"addl %%edi, %%eax \r\n"				// eax = &_cpustack + (sizeof(_cpustack[0]) * eax) + sizeof(_cpustack[0])
					"movl %%eax, %%esp \r\n"				// esp = top of stack for the cpu

                    "pushl $8 \r\n"
                    "pushl $_xcsmp_ap_start64 \r\n"
                    "lret \r\n"

                    ".code64 \r\n"
                    "_xcsmp_ap_start64: \r\n"

					"movw $0x10, %%ax \r\n"
					"movw %%ax, %%fs \r\n"
					"movw %%ax, %%gs \r\n"
					"movw %%ax, %%ss \r\n"
					"movw %%ax, %%ds \r\n"
					"movw %%ax, %%es \r\n"

                    "jmp _xcsmp_cpu_x86_smpinitialize_commonstart \r\n"
					:
					: "i" (&_ap_cr3), "i" (&_xcsmp_ap_init_gdt), "i" (MSR_APIC_BASE), "i" (&_cpucount), "i" (&_cputable), "i" (sizeof(xc_cputable_t)), "i" (&_cpustack), "i" (sizeof(_cpustack[0]))
	);

}

//////////////////////////////////////////////////////////////////////////////

//*
//re-initialize DMA protections (if needed) for the runtime
bool xcsmp_arch_dmaprot_reinitialize(void){
	//we don't need to reinitialize DMA protections since we setup
	//VT-d PMRs in the secure loader
	return true;
}


//initialize SMP
bool xcsmp_arch_smpinitialize(void){
	u32 i;

	_cpucount = xcbootinfo->cpuinfo_numentries;

	//initialize cpu table
	for(i=0; i < _cpucount; i++){
			_cputable[i].cpuid = xcbootinfo->cpuinfo_buffer[i].lapic_id;
			_cputable[i].cpu_index = i;
	}

	//save cpu MTRR state which we will later replicate on all APs
	_xcsmp_cpu_x86_savecpumtrrstate();

    //save page table base which we will later replicate on all APs
    _ap_cr3 = read_cr3();

	//wake up APS
	if(_cpucount > 1){
	  _xcsmp_container_vmx_wakeupAPs();
	}


	/*//fall through to common code
	_XDPRINTF_("\nRelinquishing BSP thread and moving to common...");
	if( _ap_pmode_entry_with_paging() ){
		_XDPRINTF_("\nBSP must never get here. HALT!");
		HALT();
	}*/

	_XDPRINTF_("%s:%u: BSP halting\n", __FUNCTION__, __LINE__);
	HALT();

}
