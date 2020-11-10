{
 "uobj-name": "uapi_uhmpgtbl",
 "uobj-type": "VfT_SLAB",
 "uobj-subtype": "UAPI",
 "uobj-uapifunctions":[
  {
   "uapifunction-id": "0",
   "uapifunction-definition" :
    "void _uhmpgtbl_initmempgtbl(uapi_uhmpgtblmempgtbl_initmempgtbl_params_t *initmempgtblp)",
   "uapifunction-drivercode" :
    "{uapi_uhmpgtblmempgtbl_initmempgtbl_params_t initmempgtblp; initmempgtblp.dst_slabid = framac_nondetu32(); _uhmpgtbl_initmempgtbl(&initmempgtblp);}"
  },
  {
   "uapifunction-id": "1",
   "uapifunction-definition" : "void _uhmpgtbl_setentryforpaddr(uapi_uhmpgtbl_setentryforpaddr_params_t *setentryforpaddrp)",
   "uapifunction-drivercode" : "{uapi_uhmpgtbl_setentryforpaddr_params_t setentryforpaddrp; setentryforpaddrp.dst_slabid = framac_nondetu32(); setentryforpaddrp.gpa = framac_nondetu32(); setentryforpaddrp.entry = framac_nondetu32(); _uhmpgtbl_setentryforpaddr(&setentryforpaddrp);}"
  }
 ],
 "uobj-callees": "	geec_sentinel
     ",
 "uobj-uapicallees":[],
 "uobj-resource-devices":[],
 "uobj-resource-memory":[],
 "uobj-exportfunctions": "",
 "uobj-binary-sections":[
  {
   "section-name": "code",
   "section-size": "0x200000"
  },
  {
   "section-name": "data",
   "section-size": "0xA00000"
  },
  {
   "section-name": "stack",
   "section-size": "0x600000"
  },
  {
   "section-name": "dmadata",
   "section-size": "0x200000"
  }
 ],
 "c-files": "",
 "v-harness": []
}
