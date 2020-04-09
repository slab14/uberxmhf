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
 * Author: Amit Vasudevan (amitvasudevan@acm.org)
 *
 */

/*
	pa5-encfs encrypted filesystem test application

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#ifndef __PA5ENCFS_H__
#define __PA5ENCFS_H__

#define UAPP_PA5ENCFS_FUNCTION_START	0x30
#define UAPP_PA5ENCFS_FUNCTION_ENCRYPT	0x31
#define UAPP_PA5ENCFS_FUNCTION_DECRYPT	0x32
#define UAPP_PA5ENCFS_FUNCTION_DONE		0x33


#define BLOCKSIZE 1024
#define FAILURE 0
#define SUCCESS 1

#define ENCRYPT 1
#define DECRYPT 0
#define PASS_THROUGH (-1)


#ifndef __ASSEMBLY__

typedef struct {
    unsigned char inbuf[BLOCKSIZE];
    int inlen;
    unsigned char outbuf[BLOCKSIZE];
    int outlen;
    int writelen;
    int result;
}pa5encfs_param_t;


#endif // __ASSEMBLY__



#endif //__PA5ENCFS_H__
