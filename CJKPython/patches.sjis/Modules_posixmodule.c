--- Modules/posixmodule.c.orig	2003-06-11 09:18:10.000000000 +0900
+++ Modules/posixmodule.c	2003-08-03 10:07:08.000000000 +0900
@@ -375,6 +375,9 @@
 	return d;
 }
 
+#ifdef SJIS
+#include <mbctype.h>
+#endif
 
 /* Set a POSIX-specific error from errno, and return NULL */
 
@@ -986,6 +989,9 @@
 	*/
 	if (pathlen > 0 &&
 	    (path[pathlen-1]== '\\' || path[pathlen-1] == '/')) {
+#ifdef SJIS
+          if (!_ismbstrail(path, path + pathlen-1)) {
+#endif
 	    	/* It does end with a slash -- exempt the root drive cases. */
 	    	/* XXX UNC root drives should also be exempted? */
 	    	if (pathlen == 1 || (pathlen == 3 && path[1] == ':'))
@@ -996,6 +1002,9 @@
 			pathcopy[pathlen-1] = '\0';
 			path = pathcopy;
 		}
+#ifdef SJIS
+	    }
+#endif	    
 	}
 #endif /* MS_WINDOWS */
 
@@ -1421,6 +1430,10 @@
 		char ch = namebuf[len-1];
 		if (ch != SEP && ch != ALTSEP && ch != ':')
 			namebuf[len++] = '/';
+#ifdef SJIS
+		else if (_ismbstrail(namebuf, namebuf + len-1))
+			namebuf[len++] = '/';
+#endif
 	}
 	strcpy(namebuf + len, "*.*");
 
@@ -1489,6 +1502,10 @@
             *pt = SEP;
     if (namebuf[len-1] != SEP)
         namebuf[len++] = SEP;
+#ifdef SJIS
+    else if (_ismbstrail(namebuf, namebuf + len-1))
+        namebuf[len++] = '/';
+#endif
     strcpy(namebuf + len, "*.*");
 
 	if ((d = PyList_New(0)) == NULL)
