/*
 * cjkc_postlude.h: postlude for cjkcodecs
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
 * $Id: cjkc_postlude.h,v 1.1 2004/06/17 18:31:20 perky Exp $
 */

#ifndef _CJKC_POSTLUDE_H_
#define _CJKC_POSTLUDE_H_

static PyObject *
getmultibytecodec(void)
{
	static PyObject *cofunc = NULL;

	if (cofunc == NULL) {
		PyObject *mod = PyImport_ImportModule("_multibytecodec");
		if (mod == NULL)
			return NULL;
		cofunc = PyObject_GetAttrString(mod, "__create_codec");
		Py_DECREF(mod);
	}
	return cofunc;
}

static PyObject *
getcodec(PyObject *self, PyObject *encoding)
{
	PyObject *codecobj, *r, *cofunc;
	const MultibyteCodec *codec;
	const char *enc;
#ifdef NO_METH_O
	PyObject *args = encoding;

        if (!PyArg_ParseTuple(args, "O:getcodec", &encoding))
	        return NULL;
#endif

	if (!PyString_Check(encoding)) {
		PyErr_SetString(PyExc_TypeError,
				"encoding name must be a string.");
		return NULL;
	}

	cofunc = getmultibytecodec();
	if (cofunc == NULL)
		return NULL;

	enc = PyString_AS_STRING(encoding);
	for (codec = codec_list; codec->encoding[0]; codec++)
		if (strcmp(codec->encoding, enc) == 0)
			break;

	if (codec->encoding[0] == '\0') {
		PyErr_SetString(PyExc_LookupError,
				"no such codec is supported.");
		return NULL;
	}

	codecobj = PyCObject_FromVoidPtr((void *)codec, NULL);
	if (codecobj == NULL)
		return NULL;

	r = PyObject_CallFunctionObjArgs(cofunc, codecobj, NULL);
	Py_DECREF(codecobj);

	return r;
}

static struct PyMethodDef __methods[] = {
#ifndef NO_METH_O
	{"getcodec", (PyCFunction)getcodec, METH_O, ""},
#else
	{"getcodec", (PyCFunction)getcodec, METH_VARARGS, ""},
#endif
	{NULL, NULL},
};

static int
register_maps(PyObject *module)
{
	const struct dbcs_map *h;

	for (h = mapping_list; h->charset[0] != '\0'; h++) {
		char mhname[256] = "__map_";
		int r;
		strcpy(mhname + sizeof("__map_") - 1, h->charset);
		r = PyModule_AddObject(module, mhname,
				PyCObject_FromVoidPtr((void *)h, NULL));
		if (r == -1)
			return -1;
	}
	return 0;
}

#ifdef USING_BINARY_PAIR_SEARCH
static DBCHAR
find_pairencmap(ucs2_t body, ucs2_t modifier,
		struct pair_encodemap *haystack, int haystacksize)
{
	int pos, min, max;
	ucs4_t value = body << 16 | modifier;

	min = 0;
	max = haystacksize;

	for (pos = haystacksize >> 1; min != max; pos = (min + max) >> 1)
		if (value < haystack[pos].uniseq) {
			if (max == pos) break;
			else max = pos;
		}
		else if (value > haystack[pos].uniseq) {
			if (min == pos) break;
			else min = pos;
		}
		else
			break;

		if (value == haystack[pos].uniseq)
			return haystack[pos].code;
		else
			return DBCINV;
}
#endif

#ifdef USING_IMPORTED_MAPS
static int
importmap(PyObject *mod, const char *symbol,
	  const struct unim_index **encmap, const struct dbcs_index **decmap)
{
	PyObject *o;

	o = PyObject_GetAttrString(mod, (char*)symbol);
	if (o == NULL)
		return -1;
	else if (!PyCObject_Check(o)) {
		PyErr_SetString(PyExc_ValueError,
				"map data must be a CObject.");
		return -1;
	}
	else {
		struct dbcs_map *map;
		map = PyCObject_AsVoidPtr(o);
		if (encmap != NULL)
			*encmap = map->encmap;
		if (decmap != NULL)
			*decmap = map->decmap;
		Py_DECREF(o);
	}

	return 0;
}
#endif

#define I_AM_A_MODULE_FOR(loc)						\
	void								\
	init_codecs_##loc(void)						\
	{								\
		PyObject *m = Py_InitModule("_codecs_" #loc, __methods);\
		(void)register_maps(m);					\
	}

#endif
