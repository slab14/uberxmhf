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
#include <xmhf-debug.h>
#include <xmhfgeec.h>

#include <geec_prime.h>

/*@
	requires 0 <= slabid < XMHFGEEC_TOTAL_SLABS;

	behavior othermem:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER)
			);
		ensures (\result == (uint64_t)(_PAGE_PRESENT | _PAGE_RW));

	behavior vobjcode:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 ((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			 ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE)
			);
		ensures (\result == (_PAGE_PRESENT));

	behavior vobjdata:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 ((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			!((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE) &&
			 ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL
			 )
			);
		ensures (\result == (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX));

	behavior vobjmmio:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 ((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			!((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE) &&
			!((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL
			 ) &&
			 ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DEVICEMMIO)
			);
		ensures (\result == (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX | _PAGE_PCD));

	behavior objcode:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 !((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			  ((spatype & _SLAB_SPATYPE_MASK_SAMESLAB) || (spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_SENTINEL) &&
 			  ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE)
			);
		ensures (\result == (_PAGE_PRESENT | _PAGE_USER));



	behavior objdata:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 !((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			  ((spatype & _SLAB_SPATYPE_MASK_SAMESLAB) || (spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_SENTINEL) &&
 			  !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE) &&
   			 ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL
			 )

			);
		ensures (\result == (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX | _PAGE_USER));

	behavior objmmio:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 !((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			  ((spatype & _SLAB_SPATYPE_MASK_SAMESLAB) || (spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_SENTINEL) &&
 			  !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE) &&
   			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL
			  ) &&
			 ((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DEVICEMMIO)
			);
		ensures (\result == (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX | _PAGE_PCD | _PAGE_USER));

	behavior otheruhobj:
		assumes (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 !((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			 !((spatype & _SLAB_SPATYPE_MASK_SAMESLAB) || (spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_SENTINEL)
			);
		ensures (\result == (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX));


        behavior invalid:
		assumes (
			 !(xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) ||

			 (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 ((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			!((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE) &&
			!((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DEVICEMMIO)
			 ) ||

			 (
			 (xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
			  xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG
			 ) &&
			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER) &&
			 !((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG) &&
			  ((spatype & _SLAB_SPATYPE_MASK_SAMESLAB) || (spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_SENTINEL) &&
  			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE) &&
    			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
			  (spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL
			  ) &&
  			 !((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DEVICEMMIO)
			 )

			);
		ensures (\result == 0);

        complete behaviors;
        disjoint behaviors;
@*/
uint64_t gp_s2_setupmpgtbluh_getflags(uint32_t slabid, uint32_t spa, uint32_t spatype){
	uint64_t flags=0;

	if(xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVT_PROG ||
		xmhfgeec_slab_info_table[slabid].slabtype == XMHFGEEC_SLABTYPE_uVU_PROG){
		//self slab: code=rx, data,stack,dmadata,mmio=rw, perms=USER
		//other slab vft: code=rx, data,stack,dmadata,mmio=rw, perms=SUPER
		//SPATYPE_OTHER => rw perms=SUPER
		//anything else: mapped rw perms=SUPER
		if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_OTHER){
			flags = (uint64_t)(_PAGE_PRESENT | _PAGE_RW);
		}else{
			if((spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_PROG){
				//verified obj
				if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE){
					flags = (_PAGE_PRESENT);
				}else if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
					(spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
					(spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
					(spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL){
				    flags = (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX);
				}else if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DEVICEMMIO){
				    flags = (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX | _PAGE_PCD);
				}else{
				    flags = 0;
				}

			}else if((spatype & _SLAB_SPATYPE_MASK_SAMESLAB) || (spatype & 0x000000F0UL) == XMHFGEEC_SLABTYPE_VfT_SENTINEL){
                                //same object or sentinel
				if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_CODE){
					flags = (_PAGE_PRESENT | _PAGE_USER);
				}else if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DATA ||
					(spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_STACK ||
					(spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DMADATA ||
					(spatype & 0x0000000FUL) == _SLAB_SPATYPE_GEEC_PRIME_IOTBL){
				    flags = (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX | _PAGE_USER);
				}else if((spatype & 0x0000000FUL) == _SLAB_SPATYPE_SLAB_DEVICEMMIO){
				    flags = (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX | _PAGE_PCD | _PAGE_USER);

				}else{
				    flags = 0;
				}

			}else{
			    flags = (_PAGE_PRESENT | _PAGE_RW | _PAGE_NX);
			}
		}
        }else{
		flags = 0;
	}

    return flags;
}

