--- Lib/site.py.orig	2003-07-27 08:00:28.000000000 +0900
+++ Lib/site.py	2003-07-27 08:00:42.000000000 +0900
@@ -318,7 +318,7 @@
 
 encoding = "ascii" # Default value set by _PyUnicode_Init()
 
-if 0:
+if 1:
     # Enable to support locale aware default string encodings.
     import locale
     loc = locale.getdefaultlocale()
