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

#include <stdint.h>
#include <stdbool.h>

#include <tee-sdk/tzmarshal.h>
#include <tee-sdk/svcapi.h>

#include <trustvisor/tv_utpm.h>

#include "libarb.h"
#include "pals.h"
#include "sha1.h"

/**
 * TODO: Split this into three C files: PAL-specific that need to
 * touch global variables, PAL-specific that do not, and
 * AntiRollBack-specifc.
 */

/**
 * This global variable is needed to isolate PAL-specific state data
 * structures from the general-purpose libarb.  This could probably be
 * cleaned up through the use of function pointers or perhaps just a
 * better design (time will tell).
 */
pal_state_t g_pal_state; /* pals.h */

/* Move state from global variable into serialized buffer. If
 * destination buffer is NULL, just populate the _len parameter with
 * how much space would have been needed. serialized_state should
 * already point to enough space. */
arb_err_t pal_arb_serialize_state(OUT uint8_t *serialized_state,
                                  OUT size_t *serialized_state_len) {
	unsigned int i;

	/* Answer a length-only request if one is given. */
	if(!serialized_state && serialized_state_len) {
		*serialized_state_len = sizeof(pal_state_t);
		return ARB_ENONE;
	}
	
	/* Otherwise, normal request. "serialized_state" should have already
	 * been allocated by the caller. */
	if(!serialized_state || !serialized_state_len) {
		return ARB_EPARAM;
	}

	*serialized_state_len = sizeof(pal_state_t);

	for(i=0; i<*serialized_state_len; i++) {
		serialized_state[i] = ((uint8_t*)&g_pal_state)[i];
	}

	return ARB_ENONE;
}

/* Move state from serialized buffer into global variable */
arb_err_t pal_arb_deserialize_state(IN const uint8_t *serialized_state,
                                    IN const size_t serialized_state_len) {
	unsigned int i;
	
	/* State should have already been allocated by the caller. */
	if(!serialized_state || !serialized_state_len) {
		return ARB_EPARAM;
	}

	if(serialized_state_len != sizeof(pal_state_t)) {
		return ARB_EBADSTATE;
	}

	for(i=0; i<serialized_state_len; i++) {
		((uint8_t*)&g_pal_state)[i] = serialized_state[i];
	}

	return ARB_ENONE;	
}

arb_err_t pal_arb_initialize_state()
{
	/* Initialization is extremely trivial here. */
	g_pal_state.counter = 0;

	return ARB_ENONE;
}

arb_err_t pal_arb_advance_state(IN const uint8_t *request,
                                IN size_t request_len)
{
	if(!request || request_len < sizeof(int)) {
		return ARB_EPARAM;
	}

	switch(((pal_request_t*)request)->cmd) {
			case PAL_ARB_INCREMENT:
				g_pal_state.counter++;
				break;
			default:
				return ARB_EBADCMDHANDLE;
	}

	return ARB_ENONE;
}



/* TODO: rename this to something better, e.g., pal_entry. */
void pals(uint32_t uiCommand, tzi_encode_buffer_t *psInBuf, tzi_encode_buffer_t *psOutBuf, tz_return_t *puiRv)
{

	/**
	 * AntiRollBack initialize / execute.
	 */
    
  switch(uiCommand) {
		
	case PAL_ARB_INITIALIZE:
		/**
		 * This command tells the PAL to wipe any previously existing
		 * state and initialize both its own state and the
		 * AntiRollBack-specific internal state.  CAUTION: You can lose
		 * your data by calling this carelessly.  There are no other
		 * inputs. An initial counter value and a state snapshot will be
		 * output.
		 */
	{
		size_t counter_len;
		uint8_t* counter;
		size_t new_snapshot_len;
		uint8_t* new_snapshot;


		
		break;
	}
  case PAL_SEAL:
    {
      uint8_t *in, *out;
      size_t inLen, outLen;
      TPM_PCR_INFO *tpmPcrInfo;
      size_t tpmPcrInfoLen;
      *puiRv = TZ_SUCCESS;

      {
        uint32_t inLen32, tpmPcrInfoLen32;
        if((*puiRv = TZIDecodeBufF(psInBuf,
                                   "%"TZI_DARRSPC "%"TZI_DARRSPC,
                                   &tpmPcrInfo, &tpmPcrInfoLen32,
                                   &in, &inLen32)))
          break;
        tpmPcrInfoLen = tpmPcrInfoLen32;
        inLen = inLen32;
      }
        
      outLen = inLen + 100; /* XXX guessing at seal overhead (real overhead is sizeof(IV + HMAC)) */

      if((*puiRv = TZIEncodeBufF(psOutBuf, "%"TZI_EARRSPC,
                                 &out, (uint32_t)outLen)))
        break;

      if((*puiRv = pal_seal(tpmPcrInfo, in, inLen, out, &outLen)))
        break;

      /* actual size of previous array */
      if((*puiRv = TZIEncodeBufF(psOutBuf, "%"TZI_EU32, (uint32_t)outLen)))
        break;
    }
    break;

  case PAL_UNSEAL:
    {
      uint8_t *in, *out, *digestAtCreation;
      size_t inLen, outLen;
      *puiRv = TZ_SUCCESS;

      {
        uint32_t inLen32;
        if((*puiRv = TZIDecodeBufF(psInBuf,
                                   "%"TZI_DARRSPC,
                                   &in, &inLen32)))
          break;
        inLen = inLen32;
      }

      outLen = inLen + 100; /* XXX guessing at unseal overhead, though should actually be negative */

      if((*puiRv = TZIEncodeBufF(psOutBuf,
                                "%"TZI_EARRSPC "%"TZI_EARRSPC,
                                 &out, (uint32_t)outLen,
                                 &digestAtCreation, (uint32_t)TPM_HASH_SIZE)))
        break;
      
      if((*puiRv = pal_unseal(in, inLen, out, &outLen, digestAtCreation)))
        break;

      /* actual size of previous array */
      if((*puiRv = TZIEncodeBufF(psOutBuf, "%"TZI_EU32, (uint32_t)outLen)))
        break;
    }
    break;

  case PAL_NV_ROLLBACK:
    {
      uint8_t *old;
      uint8_t *new;        
      uint32_t len = 32; /* XXX bad magic XXX */
      
      if((*puiRv = TZIEncodeBufF(psOutBuf, "%"TZI_EARRSPC,
                                 &old, len)))
          break;

      if((*puiRv = TZIDecodeBufF(psInBuf,
                                 "%"TZI_DARRSPC,
                                 &new, &len)))
          break;      

      if((*puiRv = pal_nv_rollback(new, &len, old)))
          break;
    }
    break;
  }
  return;
}

/* sensitive code  */
tz_return_t pal_seal(TPM_PCR_INFO *pcrInfo, uint8_t *input, uint32_t inputLen, uint8_t *output, size_t *outputLen)
{
  if (svc_utpm_seal(pcrInfo, input, inputLen, output, outputLen) == 0) {
    return TZ_SUCCESS;
  } else {
    return TZ_ERROR_GENERIC;
  }
}

tz_return_t pal_unseal(uint8_t *input, uint8_t inputLen, uint8_t *output, size_t *outputLen, uint8_t *digestAtCreation)
{
    if (svc_utpm_unseal(input, inputLen, output, outputLen, digestAtCreation) == 0) {
    return TZ_SUCCESS;
  } else {
    return TZ_ERROR_GENERIC;
  }
}


tz_return_t pal_nv_rollback(IN uint8_t *newval,
                            OUT uint32_t *nv_size,
                            OUT uint8_t *oldval)
{
	size_t size;

    if(svc_tpmnvram_getsize(&size)) {
        return TZ_ERROR_GENERIC;
    }

    *nv_size = (uint32_t)size;
    
    if(svc_tpmnvram_readall(oldval)) {
        return TZ_ERROR_GENERIC;
    }

    if(svc_tpmnvram_writeall(newval)) {
        return TZ_ERROR_GENERIC;
    }
    
    return TZ_SUCCESS;
}


/* Local Variables: */
/* mode:c           */
/* indent-tabs-mode:'t */
/* tab-width:2      */
/* c-basic-offset: 2 */
/* End:             */
