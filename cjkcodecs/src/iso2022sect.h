/*
 * iso2022sect.h: Common Codec Routines for ISO-2022 codecs.
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
 * $Id: iso2022sect.h,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

/* This ISO-2022 implementation is intended to comply ECMA-43 Level 1
 * rather than RFCs itself */

#ifndef _ISO2022COMMON_H_ONCE_
#define _ISO2022COMMON_H_ONCE_

#define ESC     0x1b
#define SO      0x0e
#define SI      0x0f

#define MAX_ESCSEQLEN       16

#define IS_ESCEND(c)        (((c) >= 'A' && (c) <= 'Z') || (c) == '@')
#define IS_ISO2022ESC(c2)   ((c2) == '(' || (c2) == ')' || (c2) == '$' || \
                             (c2) == '.' || (c2) == '&')
        /* this is not a full list of ISO-2022 escape sequence headers.
         * but, it's enough to implement CJK instances of iso-2022. */

/* STATE

  state->c[0-3]

        00000000
        ||^^^^^|
        |+-----+----  G0-3 Character Set
        +-----------  Is G0-3 double byte?

  state->c[4]

        00000000
              ||
              |+----  Locked-Shift?
              +-----  ESC Throughout
*/

#define CHARSET_DOUBLEBYTE  0x80

#define CHARSET_ASCII       'B'

#define CHARSET_ISO8859_1   'A'
#define CHARSET_ISO8859_7   'F'

#define CHARSET_KSX1001     ('C'|CHARSET_DOUBLEBYTE)

#define CHARSET_JISX0201_R  'J'
#define CHARSET_JISX0201_K  'I'
#define CHARSET_JISX0208    ('B'|CHARSET_DOUBLEBYTE)
#define CHARSET_JISX0208_O  ('@'|CHARSET_DOUBLEBYTE)
#define CHARSET_JISX0212    ('D'|CHARSET_DOUBLEBYTE)
#define CHARSET_JISX0213_1  ('O'|CHARSET_DOUBLEBYTE)
#define CHARSET_JISX0213_2  ('P'|CHARSET_DOUBLEBYTE)

#define CHARSET_GB2312      ('A'|CHARSET_DOUBLEBYTE)
#define CHARSET_GB2312_8565 ('E'|CHARSET_DOUBLEBYTE)

#define CHARSET_DESIGN(c)   ((c) & 0x7f)
#define CHARSET_ISDBCS(c)   ((c) & 0x80)

#define F_SHIFTED           0x01
#define F_ESCTHROUGHOUT     0x02

#define STATE_SETG(dn, s, v)    ((s)->c[dn]) = (v);
#define STATE_GETG(dn, s)       ((s)->c[dn])

#define STATE_SETG0(s, v)   STATE_SETG(0, s, v)
#define STATE_GETG0(s)      STATE_GETG(0, s)
#define STATE_SETG1(s, v)   STATE_SETG(1, s, v)
#define STATE_GETG1(s)      STATE_GETG(1, s)
#define STATE_SETG2(s, v)   STATE_SETG(2, s, v)
#define STATE_GETG2(s)      STATE_GETG(2, s)
#define STATE_SETG3(s, v)   STATE_SETG(3, s, v)
#define STATE_GETG3(s)      STATE_GETG(3, s)

#define STATE_SETFLAG(s, f)     ((s)->c[4]) |= (f);
#define STATE_GETFLAG(s, f)     ((s)->c[4] & (f))
#define STATE_CLEARFLAG(s, f)   ((s)->c[4]) &= ~(f);
#define STATE_CLEARFLAGS(s)     ((s)->c[4]) = 0;

#define ISO2022_GETCHARSET(charset, c1)                             \
    if ((c) >= 0x80)                                                \
        return 1;                                                   \
    if (STATE_GETFLAG(state, F_SHIFTED)) /* G1 */                   \
        (charset) = STATE_GETG1(state);                             \
    else /* G1 */                                                   \
        (charset) = STATE_GETG0(state);                             \

#endif /* def _ISO2022COMMON_H_ONCE_ */

/* -*-*-*- */

#ifdef _ISO2022_TOGGLED_ON_
/* this section undefines all iso-2022 related macros and functions */
#undef _ISO2022_TOGGLED_ON_
#undef ISO2022_DESIGNATIONS
#undef ISO2022_USE_G2_DESIGNATION
#undef ISO2022_USE_JISX0208EXT
#undef ISO2022_NO_SHIFT
#undef SHIFT_CASES
#undef SS2_ROUTINE
#undef ISO2022_BASECASES
#undef ISO2022_ESCTHROUGHOUT
#undef ISO2022_LOOP_BEGIN
#undef ISO2022_LOOP_END
#else /* !_ISO2022_TOGGLED_ON_ */
#define _ISO2022_TOGGLED_ON_
#ifdef ISO2022_USE_G2_DESIGNATION
/* hardcoded for iso-2022-jp-2 for now. we'll need to generalize it
   when we have more G2 designating encodings */
#define SS2_ROUTINE                                                 \
    if (IN2 == 'N') { /* SS2 */                                     \
        RESERVE_INBUF(3)                                            \
        if (STATE_GETG2(state) == CHARSET_ISO8859_1) {              \
            ISO8859_1_DECODE(IN3 ^ 0x80, **outbuf)                  \
            else return 3;                                          \
        } else if (STATE_GETG2(state) == CHARSET_ISO8859_7) {       \
            ISO8859_7_DECODE(IN3 ^ 0x80, **outbuf)                  \
            else return 3;                                          \
        } else if (STATE_GETG2(state) == CHARSET_ASCII) {           \
            if (IN3 & 0x80) return 3;                               \
            else **outbuf = IN3;                                    \
        } else                                                      \
            return MBERR_INTERNAL;                                  \
        NEXT(3, 1)                                                  \
    } else
#else
#define SS2_ROUTINE
#endif

#ifndef ISO2022_NO_SHIFT
#define SHIFT_CASES                                                 \
    case SI:                                                        \
        STATE_CLEARFLAG(state, F_SHIFTED)                           \
        NEXT_IN(1)                                                  \
        break;                                                      \
    case SO:                                                        \
        STATE_SETFLAG(state, F_SHIFTED)                             \
        NEXT_IN(1)                                                  \
        break;
#else
/* for compatibility with JapaneseCodecs */
#define SHIFT_CASES
#endif

#define ISO2022_BASECASES(c1)                                       \
    case ESC:                                                       \
        RESERVE_INBUF(2)                                            \
        if (IS_ISO2022ESC(IN2)) {                                   \
            int err;                                                \
            err = iso2022processesc(state, inbuf, &inleft);         \
            if (err != 0)                                           \
                return err;                                         \
        } else SS2_ROUTINE {                                        \
            STATE_SETFLAG(state, F_ESCTHROUGHOUT)                   \
            OUT1(ESC)                                               \
            NEXT(1, 1)                                              \
        }                                                           \
        break;                                                      \
    SHIFT_CASES                                                     \
    case '\n':                                                      \
        STATE_CLEARFLAG(state, F_SHIFTED)                           \
        WRITE1('\n')                                                \
        NEXT(1, 1)                                                  \
        break;

#define ISO2022_ESCTHROUGHOUT(c)                                    \
    if (STATE_GETFLAG(state, F_ESCTHROUGHOUT)) {                    \
        /* ESC throughout mode: for non-iso2022 escape sequences */ \
        RESERVE_OUTBUF(1)                                           \
        OUT1(c) /* assume as ISO-8859-1 */                          \
        NEXT(1, 1)                                                  \
        if (IS_ESCEND(c)) {                                         \
            STATE_CLEARFLAG(state, F_ESCTHROUGHOUT)                 \
        }                                                           \
        continue;                                                   \
    }

#define ISO2022_LOOP_BEGIN                                          \
    while (inleft > 0) {                                            \
        unsigned char c = IN1;                                      \
        ISO2022_ESCTHROUGHOUT(c)                                    \
        switch(c) {                                                 \
        ISO2022_BASECASES(c)                                        \
        default:                                                    \
            if (c < 0x20) { /* C0 */                                \
                RESERVE_OUTBUF(1)                                   \
                OUT1(c)                                             \
                NEXT(1, 1)                                          \
            } else if (c >= 0x80)                                   \
                return 1;                                           \
            else {
#define ISO2022_LOOP_END                                            \
            }                                                       \
        }                                                           \
    }

static int
iso2022processesc(MultibyteCodec_State *state,
                  const unsigned char **inbuf, size_t *inleft)
{
    unsigned char charset, designation;
    size_t  i, esclen;

    for (i = 1;i < MAX_ESCSEQLEN;i++) {
        if (i >= *inleft)
            return MBERR_TOOFEW;
        if (IS_ESCEND((*inbuf)[i])) {
            esclen = i + 1;
            break;
        }
#ifdef ISO2022_USE_JISX0208EXT
        else if (i+1 < *inleft && (*inbuf)[i] == '&' && (*inbuf)[i+1] == '@')
            i += 2;
#endif
    }

    if (i >= MAX_ESCSEQLEN)
        return 1; /* unterminated escape sequence */

    switch (esclen) {
    case 3:
        if (IN2 == '$') {
            charset = IN3 | CHARSET_DOUBLEBYTE;
            designation = 0;
        } else {
            charset = IN3;
            if (IN2 == '(') designation = 0;
            else if (IN2 == ')') designation = 1;
#ifdef ISO2022_USE_G2_DESIGNATION
            else if (IN2 == '.') designation = 2;
#endif
            else return 3;
        }
        break;
    case 4:
        if (IN2 != '$')
            return 4;

        charset = IN4 | CHARSET_DOUBLEBYTE;
        if (IN3 == '(') designation = 0;
        else if (IN3 == ')') designation = 1;
        else return 4;
        break;
#ifdef ISO2022_USE_JISX0208EXT
    case 6: /* designation with prefix */
        if ((*inbuf)[3] == ESC && (*inbuf)[4] == '$' && (*inbuf)[5] == 'B') {
            charset = 'B' | CHARSET_DOUBLEBYTE;
            designation = 0;
        } else
            return 6;
        break;
#endif
    default:
        return esclen;
    }

    { /* raise error when the charset is not designated for this encoding */
        const unsigned char dsgs[] = {ISO2022_DESIGNATIONS, '\x00'};

        for (i = 0; dsgs[i] != '\x00'; i++)
            if (dsgs[i] == charset)
                break;

        if (dsgs[i] == '\x00')
            return esclen;
    }

    STATE_SETG(designation, state, charset)
    *inleft -= esclen;
    (*inbuf) += esclen;
    return 0;
}
#endif /* _ISO2022_TOGGLED_ON_ */

/*
 * ex: ts=8 sts=4 sw=4 et
 */