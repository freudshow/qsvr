#include "a_xdr.h"


/*
 * INTEGER specific human-readable output.
 */
//int print_struct_integer(asn_TYPE_descriptor_t *td, const void *sptr, int ilevel,
//    asn_app_consume_bytes_f *cb, void *app_key) {
//    asn_INTEGER_specifics_t *specs=(asn_INTEGER_specifics_t *)td->specifics;
//    const long *native = (const long *)sptr;
//    char scratch[32];	/* Enough for 64-bit int */
//    int ret;

//    (void)td;	/* Unused argument */
//    (void)ilevel;	/* Unused argument */

//    if(native) {
//        ret = snprintf(scratch, sizeof(scratch),
//            (specs && specs->field_unsigned)
//            ? "%lu" : "%ld", *native);
//        assert(ret > 0 && (size_t)ret < sizeof(scratch));
//        return (cb(scratch, ret, app_key) < 0) ? -1 : 0;
//    } else {
//        return (cb("<absent>", 8, app_key) < 0) ? -1 : 0;
//    }
//}

//void free_struct_integer(asn_TYPE_descriptor_t *td, void *ptr, int contents_only) {

//    if(!td || !ptr)
//        return;

//    ASN_DEBUG("Freeing %s as INTEGER (%d, %p, Native)",
//        td->name, contents_only, ptr);

//    if(!contents_only) {
//        FREEMEM(ptr);
//    }
//}
