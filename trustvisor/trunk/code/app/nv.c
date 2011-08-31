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

#include <stdbool.h>

#include <types.h> /* u32, ... */
#include <error.h> /* HALT() */
#include <processor.h> /* rdtsc64() */
#include <target.h>
#include <tpm.h> /* tpm_nv_*() */
#include <tpm_emhf.h> /* hwtpm_open_locality() */
#include <scode.h> /* copy_from_guest */
#include <random.h> /* rand_bytes_or_die() */
#include <nv.h>

/* defined in scode.c */
/* TODO: more elegant organization of these data structures */
extern int *scode_curr;
extern whitelist_entry_t *whitelist;

/**
 * Checks that supplied index is defined, is of the appropriate size,
 * and has appropriate access restrictions in place.  Those are PCRs
 * 17 and 18, and accessible for reading and writing exclusively from
 * locality 2.
 *
 * returns 0 on success.
 */
static int validate_mss_nv_region(unsigned int locality,
                                  tpm_nv_index_t idx,
                                  unsigned int expected_size) {
    int rv = 0;
    unsigned int actual_size = 0;
    
    if(0 != (rv = tpm_get_nvindex_size(locality, idx, &actual_size))) {
        dprintf(LOG_ERROR, "\n[TV] %s: tpm_get_nvindex_size returned an ERROR!",
                __FUNCTION__);
        return rv;
    }

    if(actual_size != expected_size) {
        dprintf(LOG_ERROR, "\n[TV] ERROR: %s: actual_size (%d) != expected_size (%d)!",
                __FUNCTION__, actual_size, expected_size);
        return 1;
    }

    /* XXX TODO XXX: Check ACL on NV Region! */
    dprintf(LOG_ERROR, "\n\n[TV] %s: XXX -- NV REGION ACCESS CONTROL CHECK UNIMPLEMENTED -- XXX\n"
            "Micro-TPM SEALED STORAGE MAY NOT BE SECURE\n\n",
            __FUNCTION__);
    
    return rv;
}


/**
 * Take appropriate action to initialize the Micro-TPM's sealed
 * storage facility by populating the MasterSealingSecret based on the
 * contents of the hardware TPM's NVRAM.
 *
 * jtt nv_definespace --index 0x00015213 --size 20 -o tpm -e ASCII \
 * -p 17,18 -w --permission 0x00000000 --writelocality 2 \
 * --readlocality 2
 *
 * returns 0 on success.
 */

static int _trustvisor_nv_get_mss(unsigned int locality, uint32_t idx,
                                  uint8_t *mss, unsigned int mss_size) {
    int rv;
    unsigned int i;
    unsigned int actual_size = mss_size;
    bool first_boot;

    if(0 != (rv = validate_mss_nv_region(locality, idx, mss_size))) {
        dprintf(LOG_ERROR, "\n\n[TV] %s: ERROR: validate_mss_nv_region FAILED\n",
                __FUNCTION__);
        return rv;
    }

    if(0 != (rv = tpm_nv_read_value(locality, idx, 0, mss, &actual_size))) {
        dprintf(LOG_ERROR, "\n[TV] %s: tpm_nv_read_value FAILED! with error %d\n",
                __FUNCTION__, rv);
        return rv;
    }

    if(actual_size != mss_size) {
        dprintf(LOG_ERROR, "\n[TV] %s: NVRAM read size %d != MSS expected size %d\n",
                __FUNCTION__, actual_size, mss_size);
        return 1;
    }

    /**
     * Check whether the read bytes are all 0xff.  If so, we assume
     * "first-boot" and initialize the contents of NV.
     */
    first_boot = true;
    for(i=0; i<actual_size; i++) {
        if(mss[i] != 0xff) {
            first_boot = false;
            break;
        }
    }

    /* TODO: Get random bytes directly from TPM instead of using PRNG
      (for additional simplicitly / less dependence on PRNG
      security) */
    if(first_boot) {
        dprintf(LOG_TRACE, "\n[TV] %s: first_boot detected!", __FUNCTION__);
        rand_bytes_or_die(mss, mss_size); /* "or_die" is VERY important! */
        if(0 != (rv = tpm_nv_write_value(locality, idx, 0, mss, mss_size))) {
            dprintf(LOG_ERROR, "\n[TV] %s: ERROR: Unable to write new MSS to TPM NVRAM (%d)!\n",
                    __FUNCTION__, rv);
            return rv;
        }
    } else {
      dprintf(LOG_TRACE, "\n[TV] %s: MSS successfully read from TPM NVRAM",
              __FUNCTION__);
    }
    
    return rv;
}

int trustvisor_nv_get_mss(unsigned int locality, uint32_t idx,
                          uint8_t *mss, unsigned int mss_size) {
  int rv;

  ASSERT(NULL != mss);
  ASSERT(mss_size >= 20); /* Sanity-check security level wrt SHA-1 */

  dprintf(LOG_TRACE, "\n[TV] %s: locality %d, idx 0x%08x, mss@%p, mss_size %d",
          __FUNCTION__, locality, idx, mss, mss_size);
  
  rv = _trustvisor_nv_get_mss(locality, idx, mss, mss_size);
  if(0 == rv) {
      return rv; /* Success. */
  }

  /**
   * Something went wrong in the optimistic attempt to read /
   * initialize the MSS.  If configured conservatively, halt now!
   */
  if(HALT_UPON_NV_PROBLEM) {
    dprintf(LOG_ERROR, "\n[TV] %s MasterSealingSeed initialization FAILED! SECURITY HALT!\n",
            __FUNCTION__);
    HALT();
  }

  /**
   * If we're still here, then we're configured to attempt to run in a
   * degraded "ephemeral" mode where there is no long-term (across
   * reboots) sealing support.  Complain loudly.  We will still halt
   * if random keys are not available.
   */
  dprintf(LOG_ERROR, "\n[TV] %s MasterSealingSeed initialization FAILED!\n"
          "Continuing to operate in degraded mode. EMPHEMERAL SEALING ONLY!\n",
          __FUNCTION__);
  rand_bytes_or_die(mss, mss_size);
  
  /* XXX TODO: Eliminate degraded mode once we are sufficiently robust
     to support development and testing without it. */
  return 0;  
}


/**
 * **********************************************************
 * NV functions specific to Rollback Resistance follow.
 *
 * This includes functions that handle hypercalls.
 *
 * jtt nv_definespace --index 0x00014e56 --size 32 \
 *     -o tpm -e ASCII \
 *     -p 11,12 \
 *     -w --permission 0x00000000 --writelocality 2 --readlocality 2
 *
 * **********************************************************
 */

/**
 * Only one PAL on the entire system is granted privileges to
 * {getsize|readall|writeall} the actual hardware TPM NV Index
 * dedicated to rollback resistance.  This is the NVRAM Multiplexor
 * PAL, or NvMuxPal.
 *
 * The purpose of this function is to make sure that a PAL that is
 * trying to make one of those hypercalls is actually the PAL that is
 * authorized to do so.
 *
 * TODO: ACTUALLY CHECK THIS!
 *
 * Returns: 0 on success, non-zero otherwise.
 * TODO: Define some more meaningful failure codes.
 */
static uint32_t authenticate_nv_mux_pal(VCPU *vcpu) {
  dprintf(LOG_TRACE, "\n[TV] Entered %s", __FUNCTION__);

  /* make sure that this vmmcall can only be executed when a PAL is
	 * running */
  if (scode_curr[vcpu->id]== -1) {
    dprintf(LOG_ERROR, "\n[TV] GenRandom ERROR: no PAL is running!\n");
    return 1;
  }
    
	dprintf(LOG_ERROR, "\n[TV] SECURITY VULNERABILITY: XXX NvMuxPal Authentication"
					" UNIMPLEMENTED XXX");

	return 0; /* XXX Actual check unimplemented XXX */
}

uint32_t hc_tpmnvram_getsize(VCPU* vcpu, uint32_t size_addr) {
    uint32_t rv = 0;
		uint32_t actual_size;
		
    dprintf(LOG_TRACE, "\n[TV] Entered %s", __FUNCTION__);

		/* Make sure the asking PAL is authorized */
    if(0 != (rv = authenticate_nv_mux_pal(vcpu))) {
        dprintf(LOG_ERROR, "\n[TV] %s: ERROR: authenticate_nv_mux_pal"
                " FAILED with error code %d", __FUNCTION__, rv);
        return 1;
    }

		/* Open TPM */
		/* TODO: Make sure this plays nice with guest OS */
		if(0 != (rv = hwtpm_open_locality(TRUSTVISOR_HWTPM_NV_LOCALITY))) {
				dprintf(LOG_ERROR, "\nFATAL ERROR: Could not access HW TPM.\n");
				return 1; /* no need to deactivate */
		}

		/* Make the actual TPM call */
    if(0 != (rv = tpm_get_nvindex_size(TRUSTVISOR_HWTPM_NV_LOCALITY,
																				HW_TPM_ROLLBACK_PROT_INDEX, &actual_size))) {
        dprintf(LOG_ERROR, "\n[TV] %s: tpm_get_nvindex_size returned"
								" ERROR %d!", __FUNCTION__, rv);
        rv = 1; /* failed. */
    }

		/* Close TPM */
		deactivate_all_localities();

		dprintf(LOG_TRACE, "\n[TV] HW_TPM_ROLLBACK_PROT_INDEX 0x%08x size"
						" = %d", HW_TPM_ROLLBACK_PROT_INDEX, actual_size);
						
		put_32bit_aligned_value_to_guest(vcpu, size_addr, actual_size);		

		return rv;
}

uint32_t hc_tpmnvram_readall(VCPU* vcpu, uint32_t out_addr) {
    uint32_t rv = 0;
    dprintf(LOG_TRACE, "\n[TV] Entered %s", __FUNCTION__);

		/* Make sure the asking PAL is authorized */
    if(0 != (rv = authenticate_nv_mux_pal(vcpu))) {
        dprintf(LOG_ERROR, "\n[TV] %s: ERROR: authenticate_nv_mux_pal"
                " FAILED with error code %d", __FUNCTION__, rv);
        return 1;
    }

		return rv;
}

uint32_t hc_tpmnvram_writeall(VCPU* vcpu, uint32_t in_addr) {
    uint32_t rv = 0;
    dprintf(LOG_TRACE, "\n[TV] Entered %s", __FUNCTION__);
    
		/* Make sure the asking PAL is authorized */
    if(0 != (rv = authenticate_nv_mux_pal(vcpu))) {
        dprintf(LOG_ERROR, "\n[TV] %s: ERROR: authenticate_nv_mux_pal"
                " FAILED with error code %d", __FUNCTION__, rv);
        return 1;
    }

		return rv;
}

/* Local Variables: */
/* mode:c           */
/* indent-tabs-mode:'t */
/* tab-width:2      */
/* End:             */
