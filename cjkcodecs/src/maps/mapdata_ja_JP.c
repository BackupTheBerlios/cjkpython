/*
 * mapdata_ja_JP.c: Map Provider for Japanese Encodings
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
 * $Id: mapdata_ja_JP.c,v 1.3 2004/01/17 11:26:10 perky Exp $
 */

#include "Python.h"
#include "../cjkcommon.h"
#include "map_jisx0208.h"
#include "map_jisx0212.h"
#include "map_jisx0213.h"
#include "map_jisxcommon.h"
#include "map_cp932ext.h"

static struct dbcs_map mapholders[] = {
    {"jisx0208",        NULL,               jisx0208_decmap},
    {"jisx0212",        NULL,               jisx0212_decmap},
    {"jisxcommon",      jisxcommon_encmap,  NULL},
    {"jisx0213_1_bmp",  NULL,               jisx0213_1_bmp_decmap},
    {"jisx0213_2_bmp",  NULL,               jisx0213_2_bmp_decmap},
    {"jisx0213_bmp",    jisx0213_bmp_encmap, NULL},
    {"jisx0213_1_emp",  NULL,               jisx0213_1_emp_decmap},
    {"jisx0213_2_emp",  NULL,               jisx0213_2_emp_decmap},
    {"jisx0213_emp",    jisx0213_emp_encmap, NULL},
    {"cp932ext",        cp932ext_encmap,    cp932ext_decmap},
    {"",                NULL,               NULL},
};

static struct PyMethodDef __methods[] = {
    {NULL, NULL},
};

void
init_codecs_mapdata_ja_JP(void)
{
    struct dbcs_map *h;
    PyObject        *m;

    m = Py_InitModule("_codecs_mapdata_ja_JP", __methods);

    for (h = mapholders; h->charset[0] != '\0'; h++) {
        char     mhname[256] = "__map_";

        strcpy(mhname + sizeof("__map_") - 1, h->charset);
        PyModule_AddObject(m, mhname, PyCObject_FromVoidPtr(h, NULL));
    }

    if (PyErr_Occurred())
        Py_FatalError("can't initialize the _codecs_mapdata_ja_JP module");
}

/*
 * ex: ts=8 sts=4 et
 */
