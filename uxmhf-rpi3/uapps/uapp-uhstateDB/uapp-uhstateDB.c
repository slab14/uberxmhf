/*
 * @UBERXMHF_LICENSE_HEADER_START@
 *
 * uber eXtensible Micro-Hypervisor Framework (Raspberry Pi)
 *
 * Copyright 2018 Carnegie Mellon University. All Rights Reserved.
 *
 * NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 * INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 * UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR IMPLIED,
 * AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF FITNESS FOR
 * PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF
 * THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF
 * ANY KIND WITH RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 * INFRINGEMENT.
 *
 * Released under a BSD (SEI)-style license, please see LICENSE or
 * contact permission@sei.cmu.edu for full terms.
 *
 * [DISTRIBUTION STATEMENT A] This material has been approved for public
 * release and unlimited distribution.  Please see Copyright notice for
 * non-US Government use and distribution.
 *
 * Carnegie Mellon is registered in the U.S. Patent and Trademark Office by
 * Carnegie Mellon University.
 *
 * @UBERXMHF_LICENSE_HEADER_END@
 */

/*
 * Authors: Matt McCormack (<matthew.mccormack@live.com>)
 *          Amit Vasudevan (<amitvasudevan@acm.org>)
 */

/*
	uagent hypapp
	guest hypercall to protected DB, containing the current and max states

        authors: matt mccormack (<matthew.mccormack@live.com>)
                 amit vasudevan (<amitvasudevan@acm.org>)
*/

#include <types.h>
#include <arm8-32.h>
#include <bcm2837.h>
#include <uart.h>
#include <debug.h>

#include <uhstateDB.h>
#include <whitelist.h>

int stateDB[MAX_STATES]={0};
int maxStateDB[MAX_STATES]={0};
int set=0;

bool uapp_uhstateDB_handlehcall(u32  uhcall_function, void *uhcall_buffer, u32 uhcall_buffer_len)
{
  uhstatedb_param_t *uhcp;
  uhcp = (uhstatedb_param_t *)uhcall_buffer;
  int i;
  
  if(!set) {
    if(uhcall_function != UAPP_UHSTATEDB_FUNCTION_INIT)
      return false;

    //call acl function
    uapp_checkacl(sysreg_read_elrhyp());

    // Initialize maximum state values, based upon input
    for(i=0; i<uhcp->numStates; i++){
      maxStateDB[i]=uhcp->maxArray[i];
    }
    // only allow this to run once.
    set = 1;
    return true;
  }

  if((uhcall_function != UAPP_UHSTATEDB_FUNCTION_GET) || (uhcall_function != UAPP_UHSTATEDB_FUNCTION_NEXT))
    return false;

  //debug dump
  #if 0
  _XDPRINTFSMP_("%s: elr_hyp va=0x%08x\n", __func__, sysreg_read_elrhyp());
  #endif

  //call acl function
  uapp_checkacl(sysreg_read_elrhyp());

  if(uhcall_function == UAPP_UHSTATEDB_FUNCTION_GET) {
    memcpy(&uhcp->stateVal, &stateDB[uhcp->deviceID], sizeof(int));
    return true;
  } else if(uhcall_function == UAPP_UHSTATEDB_FUNCTION_NEXT) {
    if (stateDB[uhcp->deviceID]<maxStateDB[uhcp->deviceID])  
      stateDB[uhcp->deviceID]++;
    return true;
  } else {
    return false;
  }
}
