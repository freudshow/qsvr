#ifndef A_XDR_H
#define A_XDR_H

#include "libutil/basedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	offsetof	/* If not defined by <stddef.h> */
#define	offsetof(s, m)	((ptrdiff_t)&(((s *)0)->m) - (ptrdiff_t)((s *)0))
#endif	/* offsetof */

#if defined(__GNUC__)
    #if	__GNUC__ >= 3
        #ifndef	GCC_PRINTFLIKE
            #define	GCC_PRINTFLIKE(fmt,var)	__attribute__((format(printf,fmt,var)))
        #endif

        #ifndef	GCC_NOTUSED
            #define	GCC_NOTUSED		__attribute__((unused))
        #endif
    #else
        #ifndef	GCC_PRINTFLIKE
            #define	GCC_PRINTFLIKE(fmt,var)	/* nothing */
        #endif

        #ifndef	GCC_NOTUSED
            #define	GCC_NOTUSED
        #endif
    #endif
#endif



/*
 * forword declaretion
 */
struct asn_TYPE_descriptor_s;
typedef struct asn_TYPE_descriptor_s asn_TYPE_descriptor_t;

/*
 * This structure defines a set of parameters that may be passed
 * to every ASN.1 encoder or decoder function.
 * WARNING: if max_stack_size member is set, and you are calling the
 *   function pointers of the asn_TYPE_descriptor_t directly,
 *   this structure must be ALLOCATED ON THE STACK!
 *   If you can't always satisfy this requirement, use ber_decode(),
 *   xer_decode() and uper_decode() functions instead.
 */
typedef struct asn_codec_ctx_s {
    /*
     * Limit the decoder routines to use no (much) more stack than a given
     * number of bytes. Most of decoders are stack-based, and this
     * would protect against stack overflows if the number of nested
     * encodings is high.
     * The OCTET STRING, BIT STRING and ANY BER decoders are heap-based,
     * and are safe from this kind of overflow.
     * A value from getrlimit(RLIMIT_STACK) may be used to initialize
     * this variable. Be careful in multithreaded environments, as the
     * stack size is rather limited.
     */
    s32  max_stack_size; /* 0 disables stack bounds checking */
} asn_codec_ctx_t;

/*
 * Type of the return value of the decoding functions (ber_decode, xer_decode)
 *
 * Please note that the number of consumed bytes is ALWAYS meaningful,
 * even if code==RC_FAIL. This is to indicate the number of successfully
 * decoded bytes, hence providing a possibility to fail with more diagnostics
 * (i.e., print the offending remainder of the buffer).
 */
enum asn_dec_rval_code_e {
    RC_OK,		/* Decoded successfully */
    RC_WMORE,	/* More data expected, call again */
    RC_FAIL		/* Failure to decode data */
};

typedef struct asn_dec_rval_s {
    enum asn_dec_rval_code_e code;	/* Result code */
    s32 consumed;		/* Number of bytes consumed */
} asn_dec_rval_t;

/*
 * Type of the return value of the encoding functions (der_encode, xer_encode).
 */
typedef struct asn_enc_rval_s {
    /*
     * Number of bytes encoded.
     * -1 indicates failure to encode the structure.
     * In this case, the members below this one are meaningful.
     */
    s32 encoded;

    /*
     * Members meaningful when (encoded == -1), for post mortem analysis.
     */

    /* Type which cannot be encoded */
    struct asn_TYPE_descriptor_s *failed_type;

    /* Pointer to the structure of that type */
    void *structure_ptr;
} asn_enc_rval_t;


/*
 * Return the outmost tag of the type.
 * If the type is untagged CHOICE, the dynamic operation is performed.
 * NOTE: This function pointer type is only useful internally.
 * Do not use it in your application.
 */
typedef u32 (axdr_outmost_tag_f)(
        const struct asn_TYPE_descriptor_s *type_descriptor,
        const void *struct_ptr, int tag_mode, u32 tag);

/* The instance of the above function type; used internally. */
axdr_outmost_tag_f asn_TYPE_outmost_tag;

/*
 * Generic type of an application-defined callback to return various
 * types of data to the application.
 * EXPECTED RETURN VALUES:
 *  -1: Failed to consume bytes. Abort the mission.
 * Non-negative return values indicate success, and ignored.
 */
typedef int (asn_app_consume_bytes_f)(const void *buffer, s32 size,
    void *application_specific_key);

/*
 * A callback of this type is called whenever constraint validation fails
 * on some ASN.1 type. See "constraints.h" for more details on constraint
 * validation.
 * This callback specifies a descriptor of the ASN.1 type which failed
 * the constraint check, as well as human readable message on what
 * particular constraint has failed.
 */
typedef void (asn_app_constraint_failed_f)(void *application_specific_key,
    struct asn_TYPE_descriptor_s *type_descriptor_which_failed,
    const void *structure_which_failed_ptr,
    const char *error_message_format, ...) GCC_PRINTFLIKE(4, 5);


/*
 * Free the structure according to its specification.
 * If (free_contents_only) is set, the wrapper structure itself (struct_ptr)
 * will not be freed. (It may be useful in case the structure is allocated
 * statically or arranged on the stack, yet its elements are allocated
 * dynamically.)
 */
typedef void (axdr_struct_free_f)(
        struct asn_TYPE_descriptor_s *type_descriptor,
        void *struct_ptr, int free_contents_only);

#define	ASN_STRUCT_FREE(asn_DEF, ptr)	(asn_DEF).free_struct(&(asn_DEF),ptr,0)
#define	ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF, ptr)	\
                    (asn_DEF).free_struct(&(asn_DEF),ptr,1)

/*
 * Print the structure according to its specification.
 */
typedef int (axdr_struct_print_f)(
        struct asn_TYPE_descriptor_s *type_descriptor,
        const void *struct_ptr,
        int level,	/* Indentation level */
        asn_app_consume_bytes_f *callback, void *app_key);

/*
 * Generic type for constraint checking callback,
 * associated with every type descriptor.
 */
typedef int (axdr_constr_check_f)(
    struct asn_TYPE_descriptor_s *type_descriptor,
    const void *struct_ptr,
    asn_app_constraint_failed_f *optional_callback,	/* Log the error */
    void *optional_app_key		/* Opaque key passed to a callback */
    );

/*
 * Type of generic function which decodes the byte stream into the structure.
 */
typedef asn_dec_rval_t (axdr_type_decoder_f)(
        struct asn_codec_ctx_s *opt_codec_ctx,
        struct asn_TYPE_descriptor_s *type_descriptor,
        void **struct_ptr, const void *buf_ptr, s32 size,
        int tag_mode);

/*
 * Type of the generic axdr encoder.
 */
typedef asn_enc_rval_t (axdr_type_encoder_f)(
        struct asn_TYPE_descriptor_s *type_descriptor,
        void *struct_ptr,	/* Structure to be encoded */
        int tag_mode,		/* {-1,0,1}: IMPLICIT, no, EXPLICIT */
        u32 tag,
        asn_app_consume_bytes_f *consume_bytes_cb,	/* Callback */
        void *app_key		/* Arbitrary callback argument */
    );

/*
 * Pre-computed PER constraints.
 */
typedef const struct asn_per_constraint_s {
    enum asn_per_constraint_flags {
        APC_UNCONSTRAINED	= 0x0,	/* No PER visible constraints */
        APC_SEMI_CONSTRAINED	= 0x1,	/* Constrained at "lb" */
        APC_CONSTRAINED		= 0x2,	/* Fully constrained */
        APC_EXTENSIBLE		= 0x4	/* May have extension */
    } flags;
    int  range_bits;		/* Full number of bits in the range */
    int  effective_bits;		/* Effective bits */
    long lower_bound;		/* "lb" value */
    long upper_bound;		/* "ub" value */
} asn_per_constraint_t;

typedef const struct axdr_constraints_s {
    asn_per_constraint_t value;
    asn_per_constraint_t size;
    int (*value2code)(unsigned int value);
    int (*code2value)(unsigned int code);
} axdr_constraints_t;

#define	ASN__DECODE_FAILED do {					\
    asn_dec_rval_t tmp_error;				\
    tmp_error.code = RC_FAIL;				\
    tmp_error.consumed = 0;					\
    ASN_DEBUG("Failed to decode element %s", td ? td->name : "");	\
    return tmp_error;					\
} while(0)

#define	ASN__DECODE_STARVED do {				\
    asn_dec_rval_t tmp_error;				\
    tmp_error.code = RC_WMORE;				\
    tmp_error.consumed = 0;					\
    return tmp_error;					\
} while(0)


/*
 * This type describes an element of the constructed type,
 * i.e. SEQUENCE, SET, CHOICE, etc.
 */
enum asn_TYPE_flags_e {
    ATF_NOFLAGS,
    ATF_POINTER	= 0x01,	/* Represented by the pointer */
    ATF_OPEN_TYPE	= 0x02	/* ANY type, without meaningful tag */
};

typedef struct asn_TYPE_member_s {
    enum asn_TYPE_flags_e flags;	/* Element's presentation flags */
    int optional;	/* Following optional members, including current */
    int memb_offset;		/* Offset of the element */
    u32 tag;		/* Outmost (most immediate) tag */
    int tag_mode;		/* IMPLICIT/no/EXPLICIT tag at current level */
    asn_TYPE_descriptor_t *type;	/* Member type descriptor */
    axdr_constr_check_f *memb_constraints;	/* Constraints validator */
    axdr_constraints_t *per_constraints;	/* PER compiled constraints */
    int (*default_value)(int setval, void **sptr);	/* DEFAULT <value> */
    const char *name;			/* ASN.1 identifier of the element */
} asn_TYPE_member_t;





typedef struct asn_TYPE_descriptor_s {
    const char *name;	/* A name of the ASN.1 type. "" in some cases. */
    const char *xml_tag;	/* Name used in XML tag */

    /*
     * Generalized functions for dealing with the specific type.
     * May be directly invoked by applications.
     */
    axdr_struct_free_f  *free_struct;	/* Free the structure */
    axdr_struct_print_f *print_struct;	/* Human readable output */
    axdr_constr_check_f *check_constraints;	/* Constraints validator */
    axdr_type_decoder_f *decoder;	/* decoder */
    axdr_type_encoder_f *encoder;	/* encoder */

    /***********************************************************************
     * Internally useful members. Not to be used by applications directly. *
     **********************************************************************/

    /*
     * Tags that are expected to occur.
     */
    axdr_outmost_tag_f  *outmost_tag;	/* <optional, internal> */
    const u32 *tags;                    /* Effective tags sequence for this type */
    int tags_count;                     /* Number of tags which are expected */
    const u32 *all_tags;                /* Every tag for BER/containment */
    int all_tags_count;                 /* Number of tags */

    axdr_constraints_t *constraints;	/* PER compiled constraints */

    /*
     * An ASN.1 production type members (members of SEQUENCE, SET, CHOICE).
     */
    asn_TYPE_member_t *elements;
    int elements_count;

    /*
     * Additional information describing the type, used by appropriate
     * functions above.
     */
    const void *specifics;
} asn_TYPE_descriptor_t;





/***********************************************************************
 * axdr types free, print, check constraints, decoder, encoder funtions
 * start
 **********************************************************************/
extern asn_TYPE_descriptor_t axdr_DEF_Integer;
axdr_struct_free_f  *free_struct_integer;	/* Free the structure */
axdr_struct_print_f *print_struct_integer;	/* Human readable output */
axdr_constr_check_f *check_constraints_integer;	/* Constraints validator */
axdr_type_decoder_f *decoder_integer;	/* decoder */
axdr_type_encoder_f *encoder_integer;	/* encoder */

axdr_struct_free_f  *free_struct_boolean;	/* Free the structure */
axdr_struct_print_f *print_struct_boolean;	/* Human readable output */
axdr_constr_check_f *check_constraints_boolean;	/* Constraints validator */
axdr_type_decoder_f *decoder_boolean;	/* decoder */
axdr_type_encoder_f *encoder_boolean;	/* encoder */

axdr_struct_free_f  *free_struct_enumerated;	/* Free the structure */
axdr_struct_print_f *print_struct_enumerated;	/* Human readable output */
axdr_constr_check_f *check_constraints_enumerated;	/* Constraints validator */
axdr_type_decoder_f *decoder_enumerated;	/* decoder */
axdr_type_encoder_f *encoder_enumerated;	/* encoder */

axdr_struct_free_f  *free_struct_bit_string;	/* Free the structure */
axdr_struct_print_f *print_struct_bit_string;	/* Human readable output */
axdr_constr_check_f *check_constraints_bit_string;	/* Constraints validator */
axdr_type_decoder_f *decoder_bit_string;	/* decoder */
axdr_type_encoder_f *encoder_bit_string;	/* encoder */

axdr_struct_free_f  *free_struct_byte_string;	/* Free the structure */
axdr_struct_print_f *print_struct_byte_string;	/* Human readable output */
axdr_constr_check_f *check_constraints_byte_string;	/* Constraints validator */
axdr_type_decoder_f *decoder_byte_string;	/* decoder */
axdr_type_encoder_f *encoder_byte_string;	/* encoder */

axdr_struct_free_f  *free_struct_choice;	/* Free the structure */
axdr_struct_print_f *print_struct_choice;	/* Human readable output */
axdr_constr_check_f *check_constraints_choice;	/* Constraints validator */
axdr_type_decoder_f *decoder_choice;	/* decoder */
axdr_type_encoder_f *encoder_choice;	/* encoder */

axdr_struct_free_f  *free_struct_flag;	/* Free the structure */
axdr_struct_print_f *print_struct_flag;	/* Human readable output */
axdr_constr_check_f *check_constraints_flag;	/* Constraints validator */
axdr_type_decoder_f *decoder_flag;	/* decoder */
axdr_type_encoder_f *encoder_flag;	/* encoder */

axdr_struct_free_f  *free_struct_optional;	/* Free the structure */
axdr_struct_print_f *print_struct_optional;	/* Human readable output */
axdr_constr_check_f *check_constraints_optional;	/* Constraints validator */
axdr_type_decoder_f *decoder_optional;	/* decoder */
axdr_type_encoder_f *encoder_optional;	/* encoder */

axdr_struct_free_f  *free_struct_default;	/* Free the structure */
axdr_struct_print_f *print_struct_default;	/* Human readable output */
axdr_constr_check_f *check_constraints_default;	/* Constraints validator */
axdr_type_decoder_f *decoder_default;	/* decoder */
axdr_type_encoder_f *encoder_default;	/* encoder */

axdr_struct_free_f  *free_struct_sequence;	/* Free the structure */
axdr_struct_print_f *print_struct_sequence;	/* Human readable output */
axdr_constr_check_f *check_constraints_sequence;	/* Constraints validator */
axdr_type_decoder_f *decoder_sequence;	/* decoder */
axdr_type_encoder_f *encoder_sequence;	/* encoder */

axdr_struct_free_f  *free_struct_sequence_of;	/* Free the structure */
axdr_struct_print_f *print_struct_sequence_of;	/* Human readable output */
axdr_constr_check_f *check_constraints_sequence_of;	/* Constraints validator */
axdr_type_decoder_f *decoder_sequence_of;	/* decoder */
axdr_type_encoder_f *encoder_sequence_of;	/* encoder */

axdr_struct_free_f  *free_struct_visiblestring;	/* Free the structure */
axdr_struct_print_f *print_struct_visiblestring;	/* Human readable output */
axdr_constr_check_f *check_constraints_visiblestring;	/* Constraints validator */
axdr_type_decoder_f *decoder_visiblestring;	/* decoder */
axdr_type_encoder_f *encoder_visiblestring;	/* encoder */

axdr_struct_free_f  *free_struct_generalizedtime;	/* Free the structure */
axdr_struct_print_f *print_struct_generalizedtime;	/* Human readable output */
axdr_constr_check_f *check_constraints_generalizedtime;	/* Constraints validator */
axdr_type_decoder_f *decoder_generalizedtime;	/* decoder */
axdr_type_encoder_f *encoder_generalizedtime;	/* encoder */

axdr_struct_free_f  *free_struct_null;	/* Free the structure */
axdr_struct_print_f *print_struct_null;	/* Human readable output */
axdr_constr_check_f *check_constraints_null;	/* Constraints validator */
axdr_type_decoder_f *decoder_null;	/* decoder */
axdr_type_encoder_f *encoder_null;	/* encoder */
/***********************************************************************
 * axdr types free, print, check constraints, decoder, encoder funtions
 * end
 **********************************************************************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // A_XDR_H
