--- Lib/os.py.orig	2003-07-02 11:49:34.000000000 +0900
+++ Lib/os.py	2003-08-03 09:55:38.000000000 +0900
@@ -58,7 +58,7 @@
         from nt import _exit
     except ImportError:
         pass
-    import ntpath as path
+    import ntmbcspath as path
 
     import nt
     __all__.extend(_get_exports_list(nt))
