/*
 * _shift_jisx0213.c: the SHIFT-JISX0213 codec
 *
 * Copyright (C) 2003-2004 Hye-Shik Chang <perky@FreeBSD.org>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: _shift_jisx0213.c,v 1.3 2004/03/10 07:44:09 perky Exp $
 */

#define USING_BINARY_PAIR_SEARCH
#include "codeccommon.h"
#include "maps/alg_jisx0201.h"
#include "maps/map_jisx0213_pairs.h"

ENCMAP(jisxcommon)
DECMAP(jisx0208)
ENCMAP(jisx0213_bmp)
DECMAP(jisx0213_1_bmp)
DECMAP(jisx0213_2_bmp)
ENCMAP(jisx0213_emp)
DECMAP(jisx0213_1_emp)
DECMAP(jisx0213_2_emp)

#define EMPBASE 0x20000

ENCODER(shift_jisx0213)
{
  while (inleft > 0) {
    ucs4_t   c = IN1;
    DBCHAR   code = NOCHAR;
    int      c1, c2;
    size_t   insize;

    JISX0201_ENCODE(c, code)
    else DECODE_SURROGATE(c)

    if (code < 0x80 || (code >= 0xa1 && code <= 0xdf)) {
        WRITE1((unsigned char)code)
        NEXT(1, 1)
        continue;
    }

    RESERVE_OUTBUF(2)
    insize = GET_INSIZE(c);

    if (code == NOCHAR) {
        if (c <= 0xffff) {
            TRYMAP_ENC(jisx0213_bmp, code, c) {
                if (code == MULTIC) {
                    if (inleft < 2) {
                        if (flags & MBENC_FLUSH) {
                            code = find_pairencmap((ucs2_t)c, 0,
                                     jisx0213_pairencmap, JISX0213_ENCPAIRS);
                            if (code == DBCINV)
                                return 1;
                        } else
                            return MBERR_TOOFEW;
                    } else {
                        code = find_pairencmap((ucs2_t)c, IN2,
                                    jisx0213_pairencmap, JISX0213_ENCPAIRS);
                        if (code == DBCINV) {
                            code = find_pairencmap((ucs2_t)c, 0,
                                     jisx0213_pairencmap, JISX0213_ENCPAIRS);
                            if (code == DBCINV)
                                return 1;
                        } else
                            insize = 2;
                    }
                }
            } else TRYMAP_ENC(jisxcommon, code, c) {
                if (code & 0x8000)
                    return 1; /* abandon JIS X 0212 codes */
            } else return 1;
        } else if (c >> 16 == EMPBASE >> 16) {
            TRYMAP_ENC(jisx0213_emp, code, c & 0xffff);
            else return insize;
        } else
            return insize;
    }

    c1 = code >> 8;
    c2 = (code & 0xff) - 0x21;

    if (c1 & 0x80) { /* Plane 2 */
        if (c1 >= 0xee) c1 -= 0x87;
        else if (c1 >= 0xac || c1 == 0xa8) c1 -= 0x49;
        else c1 -= 0x43;
    } else /* Plane 1 */
        c1 -= 0x21;

    if (c1 & 1) c2 += 0x5e;
    c1 >>= 1;
    OUT1(c1 + (c1 < 0x1f ? 0x81 : 0xc1))
    OUT2(c2 + (c2 < 0x3f ? 0x40 : 0x41))

    NEXT(insize, 2)
  }

  return 0;
}

DECODER(shift_jisx0213)
{
    while (inleft > 0) {
        unsigned char    c = IN1;

        RESERVE_OUTBUF(1)
        JISX0201_DECODE(c, **outbuf)
        else if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
            unsigned char    c1, c2 = IN2;
            ucs4_t           code;

            RESERVE_INBUF(2)
            if (c2 < 0x40 || (c2 > 0x7e && c2 < 0x80) || c2 > 0xfc)
                return 2;

            c1 = (c < 0xe0 ? c - 0x81 : c - 0xc1);
            c2 = (c2 < 0x80 ? c2 - 0x40 : c2 - 0x41);
            c1 = (2 * c1 + (c2 < 0x5e ? 0 : 1));
            c2 = (c2 < 0x5e ? c2 : c2 - 0x5e) + 0x21;

            if (c1 < 0x5e) { /* Plane 1 */
                c1 += 0x21;
                TRYMAP_DEC(jisx0208, **outbuf, c1, c2) {
                    NEXT_OUT(1)
                } else TRYMAP_DEC(jisx0213_1_bmp, **outbuf, c1, c2) {
                    NEXT_OUT(1)
                } else TRYMAP_DEC(jisx0213_1_emp, code, c1, c2) {
                    PUTUCS4(EMPBASE | code)
                } else TRYMAP_DEC(jisx0213_pair, code, c1, c2) {
                    WRITE2(code >> 16, code & 0xffff)
                    NEXT_OUT(2)
                } else
                    return 2;
                NEXT_IN(2)
            } else { /* Plane 2 */
                if (c1 >= 0x67) c1 += 0x07;
                else if (c1 >= 0x63 || c1 == 0x5f) c1 -= 0x37;
                else c1 -= 0x3d;

                TRYMAP_DEC(jisx0213_2_bmp, **outbuf, c1, c2) {
                    NEXT_OUT(1)
                } else TRYMAP_DEC(jisx0213_2_emp, code, c1, c2) {
                    PUTUCS4(EMPBASE | code)
                } else
                    return 2;
                NEXT_IN(2)
            }
            continue;
        } else
            return 2;

        NEXT(1, 1) /* JIS X 0201 */
    }

    return 0;
}

#include "codecentry.h"
BEGIN_CODEC_REGISTRY(shift_jisx0213)
    MAPOPEN(ja_JP)
        IMPORTMAP_DEC(jisx0208)
        IMPORTMAP_ENC(jisxcommon)
        IMPORTMAP_ENC(jisx0213_bmp)
        IMPORTMAP_DEC(jisx0213_1_bmp)
        IMPORTMAP_DEC(jisx0213_2_bmp)
        IMPORTMAP_ENC(jisx0213_emp)
        IMPORTMAP_DEC(jisx0213_1_emp)
        IMPORTMAP_DEC(jisx0213_2_emp)
    MAPCLOSE()
END_CODEC_REGISTRY(shift_jisx0213)

/*
 * ex: ts=8 sts=4 et
 */
