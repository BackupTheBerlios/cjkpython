--- Objects/stringobject.c.orig	2003-06-18 23:17:02.000000000 +0900
+++ Objects/stringobject.c	2003-08-03 10:20:38.000000000 +0900
@@ -4,6 +4,12 @@
 
 #include <ctype.h>
 
+#ifdef SJIS
+#include <mbctype.h>
+#include <mbstring.h>
+#endif  /* SJIS */
+
+
 #ifdef COUNT_ALLOCS
 int null_strings, one_strings;
 #endif
@@ -540,7 +546,11 @@
 	p = buf = PyString_AsString(v);
 	end = s + len;
 	while (s < end) {
+#ifndef SJIS
 		if (*s != '\\') {
+#else
+		if (*s != '\\' || (_ismblead(*s) && s + 1 < end && _ismbbtrail(*(s + 1)))) {
+#endif
 		  non_esc:
 #ifdef Py_USING_UNICODE
 			if (recode_encoding && (*s & 0x80)) {
@@ -782,6 +792,16 @@
 	fputc(quote, fp);
 	for (i = 0; i < op->ob_size; i++) {
 		c = op->ob_sval[i];
+#ifdef SJIS
+		if ((i != op->ob_size - 1) && _ismbblead(c) && _ismbbtrail(op->ob_sval[i+1])) {
+			fprintf(fp, "%c%c", c, op->ob_sval[i+1]);
+			i++;
+		}
+		else if (_ismbbkana(c)) {
+			fprintf(fp, "%c", c);
+		}
+		else
+#endif
 		if (c == quote || c == '\\')
 			fprintf(fp, "\\%c", c);
                 else if (c == '\t')
@@ -833,6 +853,17 @@
 			   and a closing quote. */
 			assert(newsize - (p - PyString_AS_STRING(v)) >= 5);
 			c = op->ob_sval[i];
+#ifdef SJIS
+			if ((i != op->ob_size - 1) && _ismbblead(c) && _ismbbtrail(op->ob_sval[i+1])) {
+				*p++ = c;
+				*p++ = op->ob_sval[i+1];
+				i++;
+			}
+			else if (_ismbbkana(c)) {
+				*p++ = c;
+			}
+			else
+#endif
 			if (c == quote || c == '\\')
 				*p++ = '\\', *p++ = c;
 			else if (c == '\t')
