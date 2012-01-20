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
 * This file is part of the EMHF historical reference
 * codebase, and is released under the terms of the
 * GNU General Public License (GPL) version 2.
 * Please see the LICENSE file for details.
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
 * EMHF partition component interface
 * x86 backend
 * author: amit vasudevan (amitvasudevan@acm.org)
 */

#include <emhf.h>

//initialize partition monitor for a given CPU
void emhf_partition_arch_initializemonitor(VCPU *vcpu){
	ASSERT(vcpu->cpu_vendor == CPU_VENDOR_AMD || vcpu->cpu_vendor == CPU_VENDOR_INTEL);
	
	if(vcpu->cpu_vendor == CPU_VENDOR_AMD){
		emhf_partition_arch_x86svm_initializemonitor(vcpu);
	}else{ //CPU_VENDOR_INTEL
		emhf_partition_arch_x86vmx_initializemonitor(vcpu);
	}

}

//setup guest OS state for the partition
void emhf_partition_arch_setupguestOSstate(VCPU *vcpu){
	ASSERT(vcpu->cpu_vendor == CPU_VENDOR_AMD || vcpu->cpu_vendor == CPU_VENDOR_INTEL);
	
	if(vcpu->cpu_vendor == CPU_VENDOR_AMD){
		emhf_partition_arch_x86svm_setupguestOSstate(vcpu);
	}else{ //CPU_VENDOR_INTEL
		emhf_partition_arch_x86vmx_setupguestOSstate(vcpu);
	}
}

//start executing the partition and guest OS
void emhf_partition_arch_start(VCPU *vcpu){
	ASSERT(vcpu->cpu_vendor == CPU_VENDOR_AMD || vcpu->cpu_vendor == CPU_VENDOR_INTEL);
	
	if(vcpu->cpu_vendor == CPU_VENDOR_AMD){
		emhf_partition_arch_x86svm_start(vcpu);
	}else{ //CPU_VENDOR_INTEL
		emhf_partition_arch_x86vmx_start(vcpu);
	}
	
}
