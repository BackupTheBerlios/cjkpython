--- Modules/pyexpat.c.orig	2003-07-22 02:22:44.000000000 +0900
+++ Modules/pyexpat.c	2003-08-03 10:13:16.000000000 +0900
@@ -1266,6 +1266,32 @@
     template_buffer[256] = 0;
 }
 
+#ifdef SJIS
+
+int convert(void *data, const char *s) {
+  PyUnicodeObject *_u_string;
+  Py_UNICODE *u;
+  Py_UNICODE ret;
+
+  _u_string = (PyUnicodeObject *)PyUnicode_Decode(
+        s, 2, data, NULL );
+
+  if (_u_string) {
+      u = PyUnicode_AS_UNICODE(_u_string);
+      if (u) {
+          ret = *u;
+          Py_DECREF(_u_string);
+          return ret;
+      }
+  }
+  return 0;
+}
+
+void release(void *data) {
+}
+
+#endif
+
 static int
 PyUnknownEncodingHandler(void *encodingHandlerData,
                          const XML_Char *name,
@@ -1274,6 +1300,57 @@
     PyUnicodeObject *_u_string = NULL;
     int result = 0;
     int i;
+#ifdef SJIS
+    int sjis=0;
+#define strcasecmp stricmp
+
+  Py_UNICODE *unicode;
+  char c[2];
+
+  memset(c, 0, sizeof(c));
+
+  if (strcasecmp(name, "shift_jis") == 0) {
+      info->data = "shift_jis";
+      sjis = 1;
+  }
+  else if (strcasecmp(name, "windows-31j") == 0) {
+      info->data = "cp932";
+      sjis = 1;
+  }
+  else if (strcasecmp(name, "euc-jp") == 0) {
+      info->data = "euc_jp";
+  }
+  if (info->data != NULL) {
+      for (i = 0; i <= 0xff; i++) {
+          if ((i < 0x80) || (sjis && (0xa1 <= i) && (i <= 0xDF))) {
+              c[0] = i;
+              _u_string = (PyUnicodeObject *)PyUnicode_Decode(
+                  c, 1, info->data, NULL );
+
+              if (_u_string) {
+                  unicode = PyUnicode_AS_UNICODE(_u_string);
+                  if (unicode)
+                      info->map[i] = *unicode;
+                  else
+                      info->map[i] = -1;
+                  Py_DECREF(_u_string );
+              }
+              else {
+                    info->map[i] = i;
+              }
+          }
+          else {
+                info->map[i] = -2;
+          }
+      }
+
+      info->convert = convert;
+      info->release= release;
+
+      return 1;
+  }
+  else {
+#endif
 
     /* Yes, supports only 8bit encodings */
     _u_string = (PyUnicodeObject *)
@@ -1296,8 +1373,10 @@
     result = 1;
     Py_DECREF(_u_string);
     return result;
+#ifdef SJIS
+	}
+#endif
 }
-
 #endif
 
 static PyObject *
