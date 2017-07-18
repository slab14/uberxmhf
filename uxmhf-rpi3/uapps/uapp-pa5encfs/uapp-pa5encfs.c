/*
	pa5encfs hypapp
	FUSE encrypted filesystem hypapp

	author: amit vasudevan (amitvasudevan@acm.org)
*/

#include <types.h>
#include <arm8-32.h>
#include <bcm2837.h>
#include <miniuart.h>
#include <debug.h>

#include <xmhfcrypto.h>
#include <aes.h>
#include <pa5encfs.h>

/*
	do_crypt using libxmhfcrypto aes primitive
	aeskey of size TPM_AES_KEY_LEN_BYTES (16)
	iv of size TPM_AES_KEY_LEN_BYTES (16) which is random
	cbc_start
	cbc_encrypt
	cbc_done
 */
uint8_t aes_iv[AES_KEY_LEN_BYTES] =
	{
			0x1a, 0x2a, 0x3a, 0x4a, 0x5a, 0x6a, 0x7a, 0x8a,
			0x1b, 0x2b, 0x3b, 0x4b, 0x5b, 0x6b, 0x7b, 0x8b
	};
uint8_t aes_key[AES_KEY_LEN_BYTES] =
	{
			0xfa, 0xea, 0xda, 0xca, 0xba, 0xaa, 0x9a, 0x8a,
			0xfb, 0xeb, 0xdb, 0xcb, 0xbb, 0xab, 0x9b, 0x8b
	};

symmetric_CBC cbc_ctx;


//return true if handled the hypercall, false if not
bool uapp_pa5encfs_handlehcall(u32 uhcall_function, void *uhcall_buffer, u32 uhcall_buffer_len){
	pa5encfs_param_t *ep;

	ep = (pa5encfs_param_t *)uhcall_buffer;

	if(uhcall_function == UAPP_PA5ENCFS_FUNCTION_START){

		/* Init Engine */
		if( rijndael_cbc_start(aes_iv, aes_key, AES_KEY_LEN_BYTES, 0, &cbc_ctx) != CRYPT_OK )
			ep->result = 0;
		else
			ep->result = 1;

		return true;

	}else if (uhcall_function == UAPP_PA5ENCFS_FUNCTION_ENCRYPT){

		if( rijndael_cbc_encrypt(ep->inbuf, ep->outbuf, ep->inlen, &cbc_ctx) != CRYPT_OK)
			ep->result=0;
		else
			ep->result=1;

		return true;

	}else if (uhcall_function == UAPP_PA5ENCFS_FUNCTION_DECRYPT){

	    if( rijndael_cbc_decrypt(ep->inbuf, ep->outbuf, ep->inlen, &cbc_ctx) != CRYPT_OK)
	    	ep->result=0;
		else
			ep->result=1;


		return true;

	}else if (uhcall_function == UAPP_PA5ENCFS_FUNCTION_DONE){

		if( rijndael_cbc_done( &cbc_ctx) != CRYPT_OK)
	    	ep->result=0;
		else
			ep->result=1;

		return true;

	}else{

		return false;
	}

}
