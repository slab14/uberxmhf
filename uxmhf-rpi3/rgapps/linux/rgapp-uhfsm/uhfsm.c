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
 * Author: Matt McCormack (matthew.mccormack@live.com)
 *
 */

/*
 * hypercall program (uhfsm)
 * author: matt mccormack (<matthew.mccormack@live.com>)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include <uhcall.h>
#include <uhfsm.h>

__attribute__((aligned(4096))) __attribute__((section(".data"))) uhfsm_param_t uhcp;

void do_uhfsm(void *bufptr) {
  uhfsm_param_t *ptr_uhcp = (uhfsm_param_t *)bufptr;
  if(!uhcall(UAPP_UHFSM_FUNCTION_SIGN, ptr_uhcp, sizeof(uhfsm_param_t)))    
    printf("hypercall FAILED\n");
  else
    printf("SUCCESS\n");

  printf("newCurrentState = %d", ptr_uhcp->newCurrentState);
  
}


int main() {
  uhcp.currentState=1;
  uint8_t *testInput=(uint8_t *) "max-login-attempts";
  memcpy(&uhcp.eventString, testInput, 18);
  uhcp.samplingRate=10;
  uhcp.vaddr = (uint32_t)&uhcp;

  printf("[] passing uhfsm_param_t\n");

  do_uhfsm((void *)&uhcp);

  printf("[] test complete\n");
    
  return 0;
}
  
