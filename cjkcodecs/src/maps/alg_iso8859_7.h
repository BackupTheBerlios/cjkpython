/*
 * alg_iso8859_7.c: Encoder/Decoder macro for ISO8859-7
 *
 * Copyright (C) 2003 Hye-Shik Chang <perky@FreeBSD.org>.
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
 * $Id: alg_iso8859_7.h,v 1.1 2003/09/26 04:25:53 perky Exp $
 */

/*
 * 0x2888fbc9 and 0xbffffd77 are magic number that indicates availability
 * of mapping for each differences. (0 and 0x2d0)
 */

#define ISO8859_7_ENCODE(c, assi)                               \
    if ((c) <= 0xa0) (assi) = (c);                              \
    else if ((c) < 0xc0 && (0x288f3bc9L & (1L << ((c)-0xa0))))  \
        (assi) = (c);                                           \
    else if ((c) >= 0x0384 && (c) <= 0x03ce && ((c) >= 0x03a4 ||\
             (0xbffffd77L & (1L << ((c)-0x0384)))))             \
        (assi) = (c) - 0x02d0;                                  \
    else if ((c)>>1 == 0x2018>>1) (assi) = (c) - 0x1f77;        \
    else if ((c) == 0x2015) (assi) = 0xaf;

#define ISO8859_7_DECODE(c, assi)                               \
    if ((c) < 0xa0) (assi) = (c);                               \
    else if ((c) < 0xc0 && (0x288f3bc9L & (1L << ((c)-0xa0))))  \
        (assi) = (c);                                           \
    else if ((c) >= 0xb4 && (c) <= 0xfe && ((c) >= 0xd4 ||      \
             (0xbffffd77L & (1L << ((c)-0xb4)))))               \
        (assi) = 0x02d0 + (c);                                  \
    else if ((c) == 0xa1) (assi) = 0x2018;                      \
    else if ((c) == 0xa2) (assi) = 0x2019;                      \
    else if ((c) == 0xaf) (assi) = 0x2015;

/*
 * ex: ts=8 sts=4 et
 */
