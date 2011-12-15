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

// EMHF SMP guest component 
// declarations
// author: amit vasudevan (amitvasudevan@acm.org)

#ifndef __EMHF_SMPGUEST_H__
#define __EMHF_SMPGUEST_H__


#ifndef __ASSEMBLY__

//initialize SMP guest logic
void emhf_smpguest_initialize(VCPU *vcpu);

//handle LAPIC access #DB (single-step) exception event
void emhf_smpguest_eventhandler_dbexception(VCPU *vcpu, 
	struct regs *r);

//handle LAPIC access #NPF (nested page fault) event
void emhf_smpguest_eventhandler_hwpgtblviolation(VCPU *vcpu, u32 gpa, u32 errorcode);

//x86 SVM backends
void emhf_smpguest_arch_x86svm_initialize(VCPU *vcpu);
void emhf_smpguest_arch_x86svm_eventhandler_dbexception(VCPU *vcpu, 
	struct regs *r);
u32 emhf_smpguest_arch_x86svm_eventhandler_hwpgtblviolation(VCPU *vcpu, u32 paddr, u32 errorcode);


#endif	//__ASSEMBLY__

#endif //__EMHF_SMPGUEST_H__
