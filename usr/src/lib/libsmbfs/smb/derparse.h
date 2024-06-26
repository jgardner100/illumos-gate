// Copyright 2012 Nexenta Systems, Inc.  All rights reserved.
// Copyright (C) 2002 Microsoft Corporation
// All rights reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS"
// WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
// OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE IMPLIED WARRANTIES OF MERCHANTIBILITY
// AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
// Date    - 10/08/2002
// Author  - Sanj Surati

/////////////////////////////////////////////////////////////
//
// DERPARSE.H
//
// SPNEGO Token Handler Header File
//
// Contains the definitions required to properly parse the
// SPNEGO DER encoding.
//
/////////////////////////////////////////////////////////////

#ifndef __DERPARSE_H__
#define __DERPARSE_H__

// C++ Specific
#if defined(__cplusplus)
extern "C"
{
#endif

/* Identifier Types */
#define  IDENTIFIER_MASK               0xC0  // Bits 7 and 8
#define  IDENTIFIER_UNIVERSAL          0x00  // 00 = universal
#define  IDENTIFIER_APPLICATION        0x40  // 01 = application
#define  IDENTIFIER_CONTEXT_SPECIFIC   0x80  // 10 = context specific
#define  IDENTIFIER_PRIVATE            0xC0  // 11 = Private

/* Encoding type */

#define FORM_MASK       0x20    /* Bit 6 */
#define PRIMITIVE       0x00    /* 0 = primitive */
#define CONSTRUCTED     0x20    /* 1 = constructed */

/* Universal tags */

#define TAG_MASK        0x1F    /* Bits 5 - 1 */
#define BOOLEAN         0x01    /*  1: TRUE or FALSE */
#define INTEGER         0x02    /*  2: Arbitrary precision integer */
#define BITSTRING       0x03    /*  2: Sequence of bits */
#define OCTETSTRING     0x04    /*  4: Sequence of bytes */
#define NULLTAG         0x05    /*  5: NULL */
#define OID             0x06    /*  6: Object Identifier (numeric sequence) */
#define OBJDESCRIPTOR   0x07    /*  7: Object Descriptor (human readable) */
#define EXTERNAL        0x08    /*  8: External / Instance Of */
#define REAL            0x09    /*  9: Real (Mantissa * Base^Exponent) */
#define ENUMERATED      0x0A    /* 10: Enumerated */
#define EMBEDDED_PDV    0x0B    /* 11: Embedded Presentation Data Value */
#define SEQUENCE        0x10    /* 16: Constructed Sequence / Sequence Of */
#define SET             0x11    /* 17: Constructed Set / Set Of */
#define NUMERICSTR      0x12    /* 18: Numeric String (digits only) */
#define PRINTABLESTR    0x13    /* 19: Printable String */
#define T61STR          0x14    /* 20: T61 String (Teletex) */
#define VIDEOTEXSTR     0x15    /* 21: Videotex String */
#define IA5STR          0x16    /* 22: IA5 String */
#define UTCTIME         0x17    /* 23: UTC Time */
#define GENERALIZEDTIME 0x18    /* 24: Generalized Time */
#define GRAPHICSTR      0x19    /* 25: Graphic String */
#define VISIBLESTR      0x1A    /* 26: Visible String (ISO 646) */
#define GENERALSTR      0x1B    /* 27: General String */
#define UNIVERSALSTR    0x1C    /* 28: Universal String */
#define BMPSTR          0x1E    /* 30: Basic Multilingual Plane String */

/* Length encoding */

#define LEN_XTND  0x80      /* Indefinite or long form */
#define LEN_MASK  0x7f      /* Bits 7 - 1 */

#define SEQ_ELM(n) (IDENTIFIER_CONTEXT_SPECIFIC | CONSTRUCTED | ((n)&TAG_MASK))

//
// SPNEGO Token Parsing Constants
//


// Fixed Length of NegTokenInit ReqFlags field
#define  SPNEGO_NEGINIT_MAXLEN_REQFLAGS   2

// Difference in bits for ReqFlags token
#define  SPNEGO_NEGINIT_REQFLAGS_BITDIFF  1

// Fixed Length of NegTokenTarg NegResult field
#define  SPNEGO_NEGTARG_MAXLEN_NEGRESULT  1

// Application Specific Construct - Always at the start of a NegTokenInit
#define  SPNEGO_NEGINIT_APP_CONSTRUCT     ( IDENTIFIER_APPLICATION | CONSTRUCTED ) // 0x60

// Constructed Sequence token - after the actual token identifier token
#define  SPNEGO_CONSTRUCTED_SEQUENCE      ( SEQUENCE | CONSTRUCTED )

// MechList Type Identifier
#define  SPNEGO_MECHLIST_TYPE      ( SEQUENCE | CONSTRUCTED | OID )

//
// NegTokenInit - Token Identifier and Elements
//

// NegTokenInit - 0xa0
#define  SPNEGO_NEGINIT_TOKEN_IDENTIFIER  ( IDENTIFIER_CONTEXT_SPECIFIC | CONSTRUCTED |  \
                                             SPNEGO_TOKEN_INIT )

// Structure elements for NegTokenInit
#define  SPNEGO_NEGINIT_MECHTYPES   0x0   // MechTypes is element 0
#define  SPNEGO_NEGINIT_REQFLAGS    0x1   // ReqFlags is element 1
#define  SPNEGO_NEGINIT_MECHTOKEN   0x2   // MechToken is element 2
#define  SPNEGO_NEGINIT_MECHLISTMIC 0x3   // MechListMIC is element 3

// MechTypes element is 0xa0
#define SPNEGO_NEGINIT_ELEMENT_MECHTYPES    SEQ_ELM(SPNEGO_NEGINIT_MECHTYPES)
// ReqFlags element is 0xa1
#define SPNEGO_NEGINIT_ELEMENT_REQFLAGS     SEQ_ELM(SPNEGO_NEGINIT_REQFLAGS)
// MechToken element is 0xa2
#define SPNEGO_NEGINIT_ELEMENT_MECHTOKEN    SEQ_ELM(SPNEGO_NEGINIT_MECHTOKEN)
// MechListMIC element is 0xa3
#define  SPNEGO_NEGINIT_ELEMENT_MECHLISTMIC SEQ_ELM(SPNEGO_NEGINIT_MECHLISTMIC)

//
// NegTokenTarg - Token Identifier and Elements
//

// NegTokenTarg - 0xa1
#define  SPNEGO_NEGTARG_TOKEN_IDENTIFIER  ( IDENTIFIER_CONTEXT_SPECIFIC | CONSTRUCTED |  \
                                             SPNEGO_TOKEN_TARG )

// Structure elements for NegTokenTarg
#define  SPNEGO_NEGTARG_NEGRESULT         0x0   // NegResult is element 0
#define  SPNEGO_NEGTARG_SUPPORTEDMECH     0x1   // SupportedMech is element 1
#define  SPNEGO_NEGTARG_RESPONSETOKEN     0x2   // ResponseToken is element 2
#define  SPNEGO_NEGTARG_MECHLISTMIC       0x3   // MechListMIC is element 3

// NegResult element is 0xa0
#define SPNEGO_NEGTARG_ELEMENT_NEGRESULT     SEQ_ELM(SPNEGO_NEGTARG_NEGRESULT)
// SupportedMech element is 0xa1
#define SPNEGO_NEGTARG_ELEMENT_SUPPORTEDMECH SEQ_ELM(SPNEGO_NEGTARG_SUPPORTEDMECH)
// ResponseToken element is 0xa2
#define SPNEGO_NEGTARG_ELEMENT_RESPONSETOKEN SEQ_ELM(SPNEGO_NEGTARG_RESPONSETOKEN)
// MechListMIC element is 0xa3
#define SPNEGO_NEGTARG_ELEMENT_MECHLISTMIC   SEQ_ELM(SPNEGO_NEGTARG_MECHLISTMIC)

//
// Defines a GSS Mechanism OID.  We keep a single static array
// of these which we'll use for validation/searches/parsing.
//

typedef struct _mechOID
{
   unsigned char*    ucOid;            // Byte representation of OID
   int               iLen;             // Length of the OID, length and identifier
   int               iActualDataLen;   // Length of the actual OID
   SPNEGO_MECH_OID   eMechanismOID;     // Which OID is this?
} MECH_OID;


//
// ASN Der functions
//

int ASNDerGetLength( unsigned char* pbLengthData, long nBoundaryLength, long* pnLength,
                    long* pnNumLengthBytes );
int ASNDerCheckToken( unsigned char* pbTokenData, unsigned char nToken,
                        long nCheckLength, long nBoundaryLength, long* pnLength,
                        long* pnTokenLength );
int ASNDerCheckOID( unsigned char* pbTokenData, SPNEGO_MECH_OID nMechOID, long nBoundaryLength,
                     long* pnTokenLength );
int ASNDerCalcNumLengthBytes( long nLength );
long ASNDerCalcTokenLength( long nLength, long nDataLength );
long ASNDerCalcElementLength( long nDataLength, long* pnInternalLength );
long ASNDerCalcMechListLength( SPNEGO_MECH_OID *mechOidLst, int mechOidCnt,
				 long* pnInternalLength );
int ASNDerWriteLength( unsigned char* pbData, long nLength );
int ASNDerWriteToken( unsigned char* pbData, unsigned char ucType,
                     unsigned char* pbTokenValue, long nLength );
int ASNDerWriteOID( unsigned char* pbData, SPNEGO_MECH_OID eMechOID );
long ASNDerWriteMechList( unsigned char* pbData, SPNEGO_MECH_OID *mechOidLst, int mechOidCnt );
int ASNDerWriteElement( unsigned char* pbData, unsigned char ucElementSequence,
                        unsigned char ucType, unsigned char* pbTokenValue, long nLength );


   // C++ Specific
#if defined(__cplusplus)
}
#endif

#endif
