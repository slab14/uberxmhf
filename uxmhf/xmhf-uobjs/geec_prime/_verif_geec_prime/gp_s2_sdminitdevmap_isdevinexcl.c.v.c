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

//returns true if a given device vendor_id:device_id is in the slab device exclusion
//list
/*@
	requires 0 <= slabid < XMHFGEEC_TOTAL_SLABS;
	requires 0 <= xmhfgeec_slab_info_table[slabid].excl_devices_count <= XMHF_CONFIG_MAX_EXCLDEVLIST_ENTRIES;

	assigns \nothing;

	ensures isinexclres: \exists integer x; 0 <= x < xmhfgeec_slab_info_table[slabid].excl_devices_count &&
			(xmhfgeec_slab_info_table[slabid].excl_devices[x].vendor_id == vendor_id &&
           xmhfgeec_slab_info_table[slabid].excl_devices[x].device_id == device_id) ==>
			(\result == true);

	ensures isnotinexclres: !(\exists integer x; 0 <= x < xmhfgeec_slab_info_table[slabid].excl_devices_count &&
			(xmhfgeec_slab_info_table[slabid].excl_devices[x].vendor_id == vendor_id &&
           xmhfgeec_slab_info_table[slabid].excl_devices[x].device_id == device_id)) ==>
			(\result == false);
	ensures (\result == true) || (\result == false);
@*/
bool gp_s2_sdminitdevmap_isdevinexcl(uint32_t slabid, uint32_t vendor_id, uint32_t device_id){
    uint32_t i;

	/*@
		loop invariant a1: 0 <= i <= xmhfgeec_slab_info_table[slabid].excl_devices_count;
		loop invariant a2: \forall integer x; 0 <= x < i ==>
			!(xmhfgeec_slab_info_table[slabid].excl_devices[x].vendor_id == vendor_id &&
           xmhfgeec_slab_info_table[slabid].excl_devices[x].device_id == device_id);
		loop assigns i;
		loop variant xmhfgeec_slab_info_table[slabid].excl_devices_count - i;
	@*/
    for(i=0; i < xmhfgeec_slab_info_table[slabid].excl_devices_count; i++){
        if(xmhfgeec_slab_info_table[slabid].excl_devices[i].vendor_id == vendor_id &&
           xmhfgeec_slab_info_table[slabid].excl_devices[i].device_id == device_id)
            return true;
    }

    /*@assert a3: \forall integer x; 0 <= x < xmhfgeec_slab_info_table[slabid].excl_devices_count ==>
		!(xmhfgeec_slab_info_table[slabid].excl_devices[x].vendor_id == vendor_id &&
   	   xmhfgeec_slab_info_table[slabid].excl_devices[x].device_id == device_id);
   	@*/
    return false;
}

