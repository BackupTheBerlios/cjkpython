/*
 * mapdata_zh_TW.c: Map Provider for Traditional Chinese Encodings
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
 * $Id: mapdata_zh_TW.c,v 1.1 2003/09/24 17:47:01 perky Exp $
 */

#include "Python.h"
#include "../cjkcommon.h"
#include "map_big5.h"
#include "map_cp950ext.h"

static struct dbcs_map mapholders[] = {
    {"big5",        big5_encmap,        big5_decmap},
    {"cp950ext",    cp950ext_encmap,    cp950ext_decmap},
    {"",            NULL,               NULL},
};

static struct PyMethodDef __methods[] = {
    {NULL, NULL},
};

void
initmapdata_zh_TW(void)
{
    struct dbcs_map *h;
    PyObject        *m;

    m = Py_InitModule("mapdata_zh_TW", __methods);

    for (h = mapholders; h->charset[0] != '\0'; h++) {
        char     mhname[256] = "__map_";

        strcpy(mhname + sizeof("__map_") - 1, h->charset);
        PyModule_AddObject(m, mhname, PyCObject_FromVoidPtr(h, NULL));
    }

    if (PyErr_Occurred())
        Py_FatalError("can't initialize the mapdata_zh_TW module");
}

/*
 * ex: ts=8 sts=4 et
 */
