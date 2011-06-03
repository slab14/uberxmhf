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

#ifndef TRUSTVISOR_H
#define TRUSTVISOR_H

/*
 * Hyper-Call constants
 */
enum HCcmd {
  /* pal manipulation ops */
  TV_HC_REG =1,
  TV_HC_UNREG =2,
  TV_HC_SHARE =6,

  /* uTPM ops */
  TV_HC_UTPM_SEAL_DEPRECATED	=3,
  TV_HC_UTPM_UNSEAL_DEPRECATED =4,
  TV_HC_UTPM_QUOTE =5,
  TV_HC_UTPM_PCRREAD =7,
  TV_HC_UTPM_PCREXT  =8,
  TV_HC_UTPM_GENRAND =9,
  TV_HC_UTPM_SEAL	=10,
  TV_HC_UTPM_UNSEAL	=11,
  /* Reserving up through 20 for more UTPM stuff; don't touch! */
  
  /* misc */
  TV_HC_TEST =255,
};

/*
 * structs for pal-registration descriptor
 */

enum tv_pal_section_type {
  TV_PAL_SECTION_CODE =1,
  TV_PAL_SECTION_DATA =2,
  TV_PAL_SECTION_PARAM =3,
  TV_PAL_SECTION_STACK =4,
  TV_PAL_SECTION_SHARED_CODE =5,

  /* for internal use. */
  TV_PAL_SECTION_SHARED =6,
  TV_PAL_SECTION_GUEST_PAGE_TABLES =7,
};

struct tv_pal_section {
  enum tv_pal_section_type type;
  uint32_t start_addr;
  uint32_t page_num; /* size of section in pages */
};

#define TV_MAX_SECTIONS 10  /* max sections that are allowed in pal registration */
struct tv_pal_sections {
  uint32_t num_sections;
  struct tv_pal_section sections[TV_MAX_SECTIONS];
};

/* parameter type */
enum tv_pal_param_type {
  TV_PAL_PM_INTEGER =1,
  TV_PAL_PM_POINTER =2,
};

struct tv_pal_param {
  enum tv_pal_param_type type;  /* 1: integer ;  2:pointer*/
  uint32_t size;
};

#define TV_MAX_PARAMS 10
struct tv_pal_params {
  uint32_t num_params;
  struct tv_pal_param params[TV_MAX_PARAMS];
};

#endif

/* Local Variables: */
/* mode:c           */
/* indent-tabs-mode:nil */
/* c-basic-offset:2      */
/* End:             */
