/*
 * multibytecodec_compat.h: Compatiblility Support
 *
 * Copyright (C) 2003-2004 Hye-Shik Chang. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: multibytecodec_compat.h,v 1.4 2004/06/17 18:49:33 perky Exp $
 */

/* We don't support 2.0 and older */
#if PY_VERSION_HEX < 0x02010000
# error "multibytecodec requires python 2.1 or compatible"
#endif

/* PyDoc_VAR: For ~ Python 2.2 */
#if PY_VERSION_HEX < 0x02030000
# define PyDoc_VAR(name) static char name[]
# define PyDoc_STRVAR(name,str) PyDoc_VAR(name) = PyDoc_STR(str)
# define PyDoc_STR(str) str
#endif

/* PEP293 Codec Error Callbacks are only for Python 2.3 and over */
#if PY_VERSION_HEX < 0x02030000
# define NO_ERROR_CALLBACKS		1
#endif

/* Python 2.1 doesn't have sth */
#if PY_VERSION_HEX < 0x02020000
# define Py_USING_UNICODE		1
# define Py_UNICODE_SIZE		2
# define METH_NOARGS			METH_VARARGS
# define NO_METH_O			1
# define OLD_STYLE_TYPE			1
# define OLD_GETATTR_DEF(prefix)					\
	static PyObject *						\
	prefix##_getattr(PyObject *self, char *name)			\
	{								\
		return Py_FindMethod(prefix##_methods, self, name);	\
	}
# define GETATTR_FUNC(prefix)		prefix##_getattr
# define GETATTRO_FUNC(prefix)		0
#else
# define OLD_GETATTR_DEF(prefix)
# define GETATTR_FUNC(prefix)		0
# ifdef __MINGW32__
__inline static PyObject* __dummy_getattro(PyObject* self, PyObject* args)
{
	return PyObject_GenericGetAttr(self, args);
}
#  define GETATTRO_FUNC(prefix)		__dummy_getattro
# else
#  define GETATTRO_FUNC(prefix)		PyObject_GenericGetAttr
# endif
#endif
